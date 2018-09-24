// DlgChatInput.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgChatInput.h"
#include "DlgToolbar.h"
#include "MMSystem.h"
#include <sys/timeb.h>
//#include "richole.h"
//#include "ChatOutputView.h"

#include <Gdiplus.h> 
#include <GdiPlusEnums.h> 

#define IDC_RICHEDIT_INPUT	0x1245
#define IDC_STA_SPLITTER1	0x1246
//#define IDC_STA_SPLITTER2	0x1247
#define TIMERID_SHOW_MRFRAME		100
#define TIMERID_DELETE_SELECT_MSG	101
#define TIMERID_SCROLLTOEND			102
//#define TIMERID_MOVE_ENTER			103
#define TIMERID_MOVE_LEAVE			104
#define TIMERID_SEND_CALL_NOTIFY	105
#define TIMERID_HIDE_NOTIFY1		106
#define TIMERID_CHECK_NOTIFY		107
#define TIMERID_SC_OK						108
#define TIMERID_CHECK_FORBID		109

// CDlgChatInput dialog
const int const_max_length = 80;

IMPLEMENT_DYNAMIC(CDlgChatInput, CEbDialogBase)

CDlgChatInput::CDlgChatInput(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgChatInput::IDD, pParent)
	//, m_bReceiveOffLineMsg(false)
	//, m_bMustInviteUser(false)
{
	//m_pwebOutWndPubc = NULL;
	//m_pwebOutWndSelf = NULL;
	//m_pDlgEmotionSelect = NULL;

	m_nLastMsgId = 0;
	m_nFromUid = 0;
	m_nToUid = 0;
	//m_nPrevReceivedFileMsgId = 0;
	//m_tLastMsgTime = 0;
	m_bOwnerCall = false;
	m_nMoveEnterMsgId = 0;

	thePrevTextLength = 0;
	m_tShowNotifyTime = 0;
	m_bLastReceiveMsg = false;
	m_pPanelStatus = NULL;
	m_pPanelInputForbidStatus = NULL;
}

CDlgChatInput::~CDlgChatInput()
{
	m_pCardInfoList.clear();
}

void CDlgChatInput::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_RICHEDIT_INPUT, m_richInput);
	//DDX_Control(pDX, IDC_BUTTON_FONT, m_btnFont);
	DDX_Control(pDX, IDC_BUTTON_IMAGE, m_btnImage);
	DDX_Control(pDX, IDC_BUTTON_EBSC, m_btnEBSC);
	DDX_Control(pDX, IDC_BUTTON_MIN_EBSC, m_btnMinEBSC);
	//DDX_Control(pDX, IDC_BUTTON_FILE_MANAGER, m_btnFileManager);
	DDX_Control(pDX, IDC_BUTTON_MSG_RECORD, m_btnMsgRecord);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_btnSend);
	DDX_Control(pDX, IDC_BUTTON_SWITCH_RETURN, m_btnSwitchReturn);
	DDX_Control(pDX, IDC_COMBO_SENDTO, m_comboSendTo);
	DDX_Control(pDX, IDC_STATIC_PRIVATE, m_checkPrivate);
	//DDX_Control(pDX, IDC_STATIC_NOTIFY1, m_labelNotify1);
}


BEGIN_MESSAGE_MAP(CDlgChatInput, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CDlgChatInput::OnBnClickedButtonSend)
	ON_NOTIFY(EN_MSGFILTER, IDC_RICHEDIT_INPUT, OnMsgfilterRich)
	ON_NOTIFY(EN_LINK, IDC_RICHEDIT_INPUT, OnRichEditExLink )
	ON_NOTIFY(EN_PROTECTED, IDC_RICHEDIT_INPUT, OnRichEditProtected)
	ON_BN_CLICKED(IDC_BUTTON_FONT, &CDlgChatInput::OnBnClickedButtonFont)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgChatInput::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_FILE_MANAGER, &CDlgChatInput::OnBnClickedButtonFileManager)
	ON_BN_CLICKED(IDC_BUTTON_MSG_RECORD, &CDlgChatInput::OnBnClickedButtonMsgRecord)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DROP_FILE, OnDropFile)
	ON_MESSAGE(WM_RICK_SAVEAS, OnRichSaveAs)
	ON_BN_CLICKED(IDC_BUTTON_EBSC, &CDlgChatInput::OnBnClickedButtonEbsc)
	ON_BN_CLICKED(IDC_BUTTON_MIN_EBSC, &CDlgChatInput::OnBnClickedButtonMinEbsc)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE, &CDlgChatInput::OnBnClickedButtonImage)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_RETURN, &CDlgChatInput::OnBnClickedButtonSwitchReturn)
	ON_COMMAND(EB_COMMAND_SWITCH_MIN_EBSC, OnCmdSwitchMinEBSC)
	ON_COMMAND(EB_COMMAND_SEND_TYPE_RETURN, OnCmdSendTypeReturn)
	ON_COMMAND(EB_COMMAND_SEND_TYPE_CTRL_RETURN, OnCmdSendTypeCtrlReturn)
	ON_MESSAGE(EB_COMMAND_OPEN_APP_URL, OnMsgOpenAppUrl)
END_MESSAGE_MAP()


bool CDlgChatInput::IsForbidSpeech(void) const
{
	return (m_pCallInfo.m_sGroupCode>0 && m_pPanelInputForbidStatus!=NULL && m_pPanelInputForbidStatus->IsWindowVisible())?true:false;
}

void CDlgChatInput::SetCtrlColor(bool bInvalidate)
{
	if (m_pDlgViewContact.GetSafeHwnd()!=NULL)
		m_pDlgViewContact.SetCtrlColor();

	//if (m_pMrFrameInterface!=NULL)
	//	m_pMrFrameInterface->SetBackgroundColor(theApp.GetBgColor1());
	//m_richInput.SetBackgroundColor(FALSE,theApp.GetBgColor1());
	//m_wndSplitter1.SetColor(theApp.GetBgColor3(),theApp.GetBgColor1());

	m_comboSendTo.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_checkPrivate.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor());
	m_checkPrivate.SetTextColor(theDefaultFlatBlackTextColor,theApp.GetHotColor());
	m_btnClose.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnSend.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnSwitchReturn.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	//m_btnSwitchReturn.SetDrawPanel(true,theApp.GetMainColor());
	if (m_pDlgToolbar.GetSafeHwnd()!=NULL)
		m_pDlgToolbar.SetCtrlColor(bInvalidate);
	//if (m_pDlgViewContact.GetSafeHwnd()!=NULL)
	//	m_pDlgViewContact.SetCtrlColor(bInvalidate);
	if (bInvalidate)
		this->Invalidate();
}
void CDlgChatInput::SetInputFocus(void)
{
	if (m_richInput.GetSafeHwnd()!=0) {
		m_richInput.SetFocus();
	}
}

// CDlgChatInput message handlers

BOOL CDlgChatInput::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//BOOST_ASSERT (m_pChatInfo.get() != 0);
	this->SetTransparentType(CEbDialogBase::TT_DLG|CEbDialogBase::TT_STATIC);
	this->SetMouseMove(FALSE);

	m_pPanelStatus = new CPanelText(this);
	m_pPanelStatus->Create(CPanelText::IDD,this);
	m_pPanelStatus->ShowWindow(SW_HIDE);
	m_pPanelStatus->SetBgColor(theDefaultFlatBgColor);
	m_pPanelStatus->SetDrawPos(0,0);
	m_pPanelStatus->SetBorder(0);

	m_pPanelInputForbidStatus = new CPanelText(this);
	m_pPanelInputForbidStatus->Create(CPanelText::IDD,this);
	m_pPanelInputForbidStatus->ShowWindow(SW_HIDE);
	m_pPanelInputForbidStatus->SetBgColor(theDefaultFlatBgColor);
	m_pPanelInputForbidStatus->SetDrawPos(0,0);
	m_pPanelInputForbidStatus->SetBorder(0);
	
	//m_pPanelStatus->SetMouseMove(TRUE);

	//m_bMustInviteUser = this->m_pCallInfo->m_bOffLineCall;//m_bReceiveOffLineMsg;
	//if (m_bMustInviteUser)
	//{
	//	m_btnSend.SetWindowText(_T("邀请"));
	//	m_btnSend.Invalidate();
	//}

	::CoInitialize(NULL);
	//CImageEx theBtnImage;
	//HGDIOBJ pBtnBitmap[3];
	if (!m_pMrFrameControl.CreateControl(__uuidof(EBRichMessage),NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,10,10),this,1111))
	{
		// 7E：126，找不到指定模块
		//CString stext;
		//stext.Format(_T("error1: 0x%x"),GetLastError());
		//AfxMessageBox(stext);
		return FALSE;
	}
	LPUNKNOWN pUnknown = m_pMrFrameControl.GetControlUnknown();
	if (pUnknown == NULL)
	{
		//CString stext;
		//stext.Format(_T("error2: 0x%x"),GetLastError());
		//AfxMessageBox(stext);
		return FALSE;
	}
	pUnknown->QueryInterface(__uuidof(IEBRichMessage),(void**)&m_pMrFrameInterface);
	if (m_pMrFrameInterface==NULL)
	{
		//CString stext;
		//stext.Format(_T("error3: 0x%x"),GetLastError());
		//AfxMessageBox(stext);
		return FALSE;
	}
	m_pMrFrameInterface->SetParameter(EBC_CONTRON_PARAMETER_DISABLE_RECEIPT,(m_pCallInfo.m_sGroupCode>0||theApp.GetDisableMsgReceipt())?1:0);
	m_pMrFrameInterface->SetSelectCbData1(EB_MR_CTRL_DATA_TYPE_SELECT_STRING);
	m_pMrFrameInterface->SetLineInterval(3);
	m_pMrFrameInterface->SetBackgroundColor(theDefaultFlatBgColor);
	((CEBRichMessageEventsSink*)this)->DispEventAdvise(m_pMrFrameInterface);
	// 右键菜单应用功能
	if (theApp.BuildChatMenuSubscribeFuncList()>0)
	{
		for (size_t i=0;i<theApp.m_pChatMenuSubscribeFuncList.size();i++)
		{
			const EB_SubscribeFuncInfo & pSubscribeFuncInfo = theApp.m_pChatMenuSubscribeFuncList[i];
			m_pMrFrameInterface->AddContextMenuItem(pSubscribeFuncInfo.m_sFunctionName.c_str(),i,VARIANT_FALSE);
		}
	}

	//m_pMrFrame.Create(CRect(0,0,0,0),this);
	//m_pMrFrame.SetScrollBarBmp(IDB_VERTICAL_SCROLLBAR_UPARROW, IDB_VERTICAL_SCROLLBAR_SPAN, IDB_VERTICAL_SCROLLBAR_DOWNARROW,
	//	IDB_VERTICAL_SCROLLBAR_THUMB, IDB_VERTICAL_SCROLLBAR_TOP, IDB_VERTICAL_SCROLLBAR_BOTTOM );
	//m_pMrFrame.SetLineAlignment(3);

	/*
	m_pwebOutWndPubc = new CChatOutputView();
	if ( !m_pwebOutWndPubc->Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 0x145, NULL))
	{
		delete m_pwebOutWndPubc;
		m_pwebOutWndPubc = NULL;
		AfxThrowResourceException();
		return FALSE;
	}
	m_pwebOutWndPubc->m_bAddBorder = false;
	m_pwebOutWndPubc->SetBackupCount(10);
	m_pwebOutWndPubc->SetBlockLimit(200);
	m_pwebOutWndPubc->SetAutoScroll(true);
	m_pwebOutWndSelf = new CChatOutputView();
	if ( !m_pwebOutWndSelf->Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0,0,0,120), this, 0x146, NULL))
	{
		delete m_pwebOutWndSelf;
		m_pwebOutWndSelf = NULL;
		AfxThrowResourceException();
		return FALSE;
	}
	m_pwebOutWndSelf->m_bAddBorder = false;
	m_pwebOutWndSelf->SetBackupCount(10);
	m_pwebOutWndSelf->SetBlockLimit(200);
	m_pwebOutWndSelf->SetAutoScroll(true);
	*/

	CRect rc(0, 350, 200, 350+1);
	m_wndSplitter1.SetStyle(SPS_HORIZONTAL);
	m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_STA_SPLITTER1);
	m_wndSplitter1.SetTransparent(false);
	m_wndSplitter1.SetColor(theDefaultFlatLineColor,theDefaultFlatBgColor);
	//m_wndSplitter.SetColor(RGB(188, 234, 246), RGB(188, 234, 246));
	//m_wndSplitter2.SetStyle(SPS_HORIZONTAL);
	//m_wndSplitter2.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_STA_SPLITTER2);
	//m_wndSplitter2.SetTransparent(true);

	//m_btnFont.ShowWindow(SW_HIDE);
	//m_btnFont.SetToolTipText(_T("设置字体"));
	//theBtnImage.LoadResource(IDB_BITMAP_BTN_FONT);
	//theBtnImage.Cut(3, pBtnBitmap);
	//m_btnFont.SetSkin(pBtnBitmap[0], pBtnBitmap[2], pBtnBitmap[1]);

//e.换行切换
//CRichEditView的OnInitialUpdate()函数中加入下面两句：
//m_nWordWrap = WrapNone;
//WrapChanged();
//WrapChanged实际上也是调用
//ctrl.SetTargetDevice(NULL, 1); //m_nWordWrap == WrapNone
//ctrl.SetTargetDevice(NULL, 0); //m_nWordWrap == WrapToWindow
//还有不常用的 m_nWordWrap == WrapToTargetDevice
//ctrl.SetTargetDevice(m_dcTarget, GetPrintWidth());
//如果是在Dialog中，可使用SetTargetDevice，注意在属性里面加上want return

	CRect rt(0, 0, 0, 46);
	DWORD dwStyle = WS_CHILD|WS_VSCROLL|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL;
	m_richInput.Create( dwStyle, rt, this, IDC_RICHEDIT_INPUT );
	//m_richInput.PasteSpecial(CF_BITMAP);
	m_richInput.SetEventMask(ENM_LINK|ENM_PROTECTED);
	m_richInput.SetDefaultCharFormat( theApp.m_cfDefault );
	//m_richInput.ModifyStyle(0,ES_WANTRETURN);
	//m_richInput.SetTargetDevice(NULL,1);
	m_richInput.SetBackgroundColor(FALSE,theDefaultFlatBgColor);
	m_pPanelInputForbidStatus->SetWindowPos(&m_richInput, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

	//m_richInput.SetFocus();
	m_richInput.LimitText(24*1024);
	this->EnableToolTips();
	this->SetToolTipText(IDC_COMBO_SENDTO,_T("选择跟某人说话（其他人能看到）"));
	this->SetToolTipText(IDC_STATIC_PRIVATE,_T("选择跟对方私聊（其他人看不到）"));
	if (theApp.GetSendType()==0)
	{
		this->SetToolTipText(IDC_RICHEDIT_INPUT,_T("Enter发送\n[Ctrl+Enter换行]"));
		this->SetToolTipText(IDC_BUTTON_SEND,_T("按Enter键发送消息，按Ctrl+Enter键换行"));
	}else
	{
		this->SetToolTipText(IDC_RICHEDIT_INPUT,_T("Ctrl+Enter发送\n[Enter换行]"));
		this->SetToolTipText(IDC_BUTTON_SEND,_T("按Ctrl+Enter键发送消息，按Enter键换行"));
	}
	this->SetToolTipText(IDC_BUTTON_IMAGE,_T("发送表情"));
	this->SetToolTipText(IDC_BUTTON_EBSC,_T("屏幕画布截图 Shift + Alt + A"));
	if (theApp.GetMinEBSC())
		this->SetToolTipText(IDC_BUTTON_MIN_EBSC,_T("截图时隐藏当前窗口（点击切换）"));
	else
		this->SetToolTipText(IDC_BUTTON_MIN_EBSC,_T("截图时显示当前窗口（点击切换）"));
	this->SetToolTipText(IDC_BUTTON_MSG_RECORD,_T("查看消息记录"));
	this->SetToolTipText(IDC_BUTTON_CLOSE,_T("关闭"));
	this->SetToolTipText(IDC_BUTTON_SWITCH_RETURN,_T("切换发送消息按键"));

	m_checkPrivate.SetTextTop(-1);
	m_checkPrivate.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_checkPrivate.SetTextLeft(8);
	m_btnImage.SetAutoSize(false);
	m_btnImage.Load(IDB_PNG_BTN_EMOTION);
	m_btnImage.SetWindowText(_T(""));
	m_btnEBSC.SetAutoSize(false);
	m_btnEBSC.Load(IDB_PNG_BTN_EBCS);
	m_btnEBSC.SetWindowText(_T(""));
	m_btnMinEBSC.SetWindowText(_T(""));
	m_btnMinEBSC.SetDrawTrianglePic(2,theDefaultFlatBlackText2Color,theDefaultTextBlackColor,-1,-1,8,4);
	m_btnMinEBSC.SetDrawPanel(true,theDefaultFlatBgColor);
	//m_btnFileManager.SetAutoSize(false);
	//m_btnFileManager.Load(IDB_PNG_58X24);
	//m_btnFileManager.SetToolTipText(_T("查看传输文件"));
	//m_btnFileManager.ShowWindow(SW_HIDE);	// ??暂时没有用
	//m_btnMsgRecord.SetFont(this->GetFont());
	m_btnMsgRecord.SetAutoSize(false);
	m_btnMsgRecord.Load(IDB_PNG_BTN_MSGRECORD);
	m_btnMsgRecord.SetWindowText(_T(""));
	//m_btnMsgRecord.SetTextLeft(15);
	m_btnClose.SetTextHotMove(false);
	m_btnClose.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnClose.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnSend.SetTextHotMove(false);
	m_btnSend.SetNorTextColor(theDefaultBtnWhiteColor);
	m_btnSend.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnSend.SetPreTextColor(theDefaultBtnWhiteColor);
	//m_btnSend.SetTextLeft(8);
	//m_btnSend.ShowWindow(SW_HIDE);
	m_btnSwitchReturn.SetDrawPanelRgn(true);
	m_btnSwitchReturn.SetWindowText(_T(""));
	m_btnSwitchReturn.SetDrawTrianglePic(2,theDefaultBtnWhiteColor,-1,-1,-1,10,5);

	SetCtrlColor(false);

	//m_btnSend.SetWindowPos(&m_richInput, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//theBtnImage.LoadResource(IDB_BITMAP_BTN_6821);
	//theBtnImage.Cut(3, pBtnBitmap);
	//m_btnSend.SetSkin(pBtnBitmap[0], pBtnBitmap[2], pBtnBitmap[1]);
	//m_btnSend.SetToolTipText(_T("发送消息"));

	//if (m_bReceiveOffLineMsg)
	//{
	//	m_wndSplitter.EnableWindow(FALSE);
	//	m_btnSend.EnableWindow(FALSE);
	//}
	m_comboSendTo.AddString(_T("所有人"));
	//m_comboSendTo.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor,theDefaultFlatLine2Color);
	m_comboSendTo.SetHighlightColor( RGB(115,138,174),RGB(255,255,255));
	m_comboSendTo.SetNormalPositionColor( RGB(255,255,255),RGB(0,0,0));

	//if (this->m_pCallInfo->m_bOffLineCall)
	//{
	//	if (theEBAppClient.InviteCall(this->m_pCallInfo->GetCallId().c_str()) != 0)
	//	{
	//		m_bMustInviteUser = true;
	//		m_btnSend.SetWindowText(_T("邀请"));
	//		m_btnSend.Invalidate();
	//	}
	//}
	//m_labelNotify1.SetWindowText(_T(""));
	//m_labelNotify1.ShowWindow(SW_HIDE);
	//m_labelNotify1.SetTextColor(theDefaultFlatLine2Color);
	//m_labelNotify1.SetAlignText(CLabelEx::Align_Default);

	m_pDlgToolbar.m_pCallInfo = m_pCallInfo;
	LoadMsgRecord();
	//ShowImageWindow(false);
	SetTimer(TIMERID_SHOW_MRFRAME,100,NULL);
	SetTimer(TIMERID_SEND_CALL_NOTIFY,6*1000,NULL);
	if (m_pCallInfo.m_sGroupCode==0)
	{
		const std::wstring sText(L"对方正在输入...");
		const int nWidth = m_pPanelStatus->MeasureTextWidth(sText.c_str(),230);
		m_pPanelStatus->MoveWindow(0,0,nWidth,22);
		m_pPanelStatus->SetDrawText(sText.c_str(),false);
		//m_labelNotify1.SetWindowText(_T("对方正在输入..."));
	}else
	{
		SetTimer(TIMERID_CHECK_NOTIFY,1000,NULL);
		CheckMyForbidSpeechState(false,false);
	}
	//SetCircle(false);
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChatInput::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgChatInput::OnCancel()
{
	if (m_richInput.GetTextLength()==0)
	{
		// 没有数据，按ESC退出；
		OnBnClickedButtonClose();
	}
	//CEbDialogBase::OnCancel();
}

void CDlgChatInput::OnDestroy()
{
	//m_pChatRoom.reset();
	//m_pCallInfo.reset();
	//m_pFromAccountInfo.reset();
	((CEBRichMessageEventsSink*)this)->DispEventUnadvise(m_pMrFrameInterface);
	m_pMrFrameInterface.Release();
	m_pMrFrameControl.DestroyWindow();
	//if (m_pDlgEmotionSelect)
	//{
	//	m_pDlgEmotionSelect->DestroyWindow();
	//	delete m_pDlgEmotionSelect;
	//	m_pDlgEmotionSelect = NULL;
	//}
	/*
	if( m_pwebOutWndPubc )
	{
		m_pwebOutWndPubc->DestroyWindow();
		delete m_pwebOutWndPubc;
		m_pwebOutWndPubc = NULL;
	}
	if( m_pwebOutWndSelf )
	{
		m_pwebOutWndSelf->DestroyWindow();
		delete m_pwebOutWndSelf;
		m_pwebOutWndSelf = NULL;
	}
	*/
	m_wndSplitter1.DestroyWindow();
	//m_wndSplitter2.DestroyWindow();
	m_richInput.DestroyWindow();

	m_pMinEBSCMenu.DestroyMenu();
	m_pSendTypeSwitchMenu.DestroyMenu();
	if (m_pDlgToolbar.GetSafeHwnd()!=NULL)
		m_pDlgToolbar.DestroyWindow();
	if (m_pDlgViewContact.GetSafeHwnd()!=NULL)
		m_pDlgViewContact.DestroyWindow();

	if (m_pPanelStatus!=NULL)
	{
		m_pPanelStatus->DestroyWindow();
		delete m_pPanelStatus;
		m_pPanelStatus = NULL;
	}
	if (m_pPanelInputForbidStatus!=NULL)
	{
		m_pPanelInputForbidStatus->DestroyWindow();
		delete m_pPanelInputForbidStatus;
		m_pPanelInputForbidStatus = NULL;
	}

	CEbDialogBase::OnDestroy();

	// TODO: Add your message handler code here
	//m_pChatInfo.reset();
}

void CDlgChatInput::DoResize(UINT nID,int delta)
{
	if (delta == 0)
	{
		return;
	}
	if (nID == IDC_STA_SPLITTER1)
	{
		//CSplitterControl::ChangePos(&m_comboSendTo, 0, delta);
		//CSplitterControl::ChangePos(this->GetDlgItem(IDC_STATIC_PRIVATE), 0, delta);
		//CSplitterControl::ChangePos(&m_btnImage, 0, delta);
		//CSplitterControl::ChangePos(&m_btnEBSC, 0, delta);
		//CSplitterControl::ChangePos(&m_btnMsgRecord, 0, delta);
		CSplitterControl::ChangeHeight(&m_pMrFrameControl, delta, CW_TOPALIGN);
		//CSplitterControl::ChangePos(m_pPanelStatus, 0, delta);
		CSplitterControl::ChangeHeight(&m_richInput, -delta, CW_BOTTOMALIGN);
		if (m_pPanelInputForbidStatus!=NULL && m_pPanelInputForbidStatus->GetSafeHwnd()!=NULL)
			CSplitterControl::ChangeHeight(m_pPanelInputForbidStatus, -delta, CW_BOTTOMALIGN);

	//}else if (nID == IDC_STA_SPLITTER2)
	//{
	//	CSplitterControl::ChangeHeight(m_pwebOutWndPubc, delta, CW_TOPALIGN);
	//	CSplitterControl::ChangeHeight(m_pwebOutWndSelf, -delta, CW_BOTTOMALIGN);
	}
	Invalidate();
	// 刷新父窗口
	CRect rect;
	this->GetWindowRect(&rect);
	this->GetParent()->ScreenToClient(&rect);
	this->GetParent()->InvalidateRect(&rect);
}

LRESULT CDlgChatInput::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_STA_SPLITTER1)// || wParam == IDC_STA_SPLITTER2)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResize(wParam, pHdr->delta);
		}
	}
	return CEbDialogBase::DefWindowProc(message, wParam, lParam);
}

void CDlgChatInput::MoveSize(int cx, int cy)
{
	//theApp.LogMessage("CDlgChatInput::MoveSize... (cx=%d,cy=%d)\r\n",cx,cy);
	if (cx==0 || cy==0) return;
	//if ((cx==0 || cy==0) && this->GetSafeHwnd()!=NULL)	// 窗口最小化，还原导致
	//{
	//	CRect rect;
	//	this->GetClientRect(&rect);
	//	cx = rect.Width();
	//	cy = rect.Height();
	//}
	const bool bIsDepDialog = m_pCallInfo.m_sGroupCode>0;
	CRect rectRichInput;
	if (m_richInput.GetSafeHwnd())
	{
		m_richInput.GetWindowRect(&rectRichInput);
		this->ScreenToClient(&rectRichInput);
	}
	CRect rectOutSelf;
	if (m_pMrFrameControl.GetSafeHwnd())
	{
		m_pMrFrameControl.GetWindowRect(&rectOutSelf);
		this->ScreenToClient(&rectOutSelf);
	}
	//if (m_pwebOutWndSelf != NULL && m_pwebOutWndSelf->GetSafeHwnd())
	//{
	//	m_pwebOutWndSelf->GetWindowRect(&rectOutSelf);
	//	this->ScreenToClient(&rectOutSelf);
	//}

	const int const_btn_right_inteval	= 5;
	const int const_splitter_height		= 3;
	const int const_bar_button_inteval	= 3;
	const int const_bar_button_width	= m_btnImage.GetImgWidth();
	//const int const_bar_button_height	= m_btnFont.GetHeight();
	const int const_bar_button_height	= 24;
	const int const_send_button_width	= 68;	// 81
	const int const_send_button_height	= 26;	// 30
	const int const_send_button_top		= cy-const_send_button_height-6;	// 最下面间隔
	int x = 0;
	//x = cx-const_send_button_width-const_btn_right_inteval;
	//m_btnClose.MovePoint(x, const_send_button_top);
	const int const_switch_button_width = 18;
	x = cx-const_switch_button_width-const_btn_right_inteval;
	if (m_btnSwitchReturn.GetSafeHwnd()!=NULL)
		m_btnSwitchReturn.MoveWindow(x,const_send_button_top,const_switch_button_width,const_send_button_height);
	x -= (const_send_button_width-2);
	m_btnSend.MovePoint(x,const_send_button_top,const_send_button_width,const_send_button_height);
	x -= (const_send_button_width+8);	// 关闭按钮间隔
	m_btnClose.MovePoint(x,const_send_button_top,const_send_button_width,const_send_button_height);
	const int const_close_left = x;

	const int const_rich_input_top = const_send_button_top-rectRichInput.Height()-4;	// 发送按钮上面间隔
	if (m_richInput.GetSafeHwnd())
	{
		m_richInput.MoveWindow(2, const_rich_input_top, cx-2, rectRichInput.Height());	// 左边间隔
		if (m_pPanelInputForbidStatus!=NULL && m_pPanelInputForbidStatus->GetSafeHwnd()!=NULL)
		{
			m_pPanelInputForbidStatus->MoveWindow(2, const_rich_input_top, cx-2, rectRichInput.Height());	// 左边间隔
		}
	}
	//const int const_bar_button_top = const_rich_input_top-const_bar_button_height-1;
	//const int const_splitter1_top = const_bar_button_top-const_splitter_height;
	const int const_splitter1_top = const_rich_input_top-const_splitter_height-2;
	const int const_outself_height = const_splitter1_top;
	const int const_outpubc_height = 0;
	//const int const_outself_height = bIsDepDialog?rectOutSelf.Height():const_splitter1_top;
	//const int const_outpubc_height = bIsDepDialog?(const_splitter1_top-const_outself_height-const_splitter_height):0;
	if (m_wndSplitter1.GetSafeHwnd())
	{
		m_wndSplitter1.MoveWindow(0, const_splitter1_top, cx, const_splitter_height);
		m_wndSplitter1.SetRange(50,cy-100);
		//m_wndSplitter1.SetRange(min(300,const_outself_height-10), min(200, rectRichInput.Height()-10), -1);
	}
#ifdef USES_NEW_UI_1220
	x = 2;
#else
	x = 5;
#endif
	int y = const_send_button_top;
	//int y = const_bar_button_top;
	if (bIsDepDialog)
	{
		const int const_combo_sentto_width = 80;
		const int const_check_private_width = 48;	// 65
		if (m_comboSendTo.GetSafeHwnd())
		{
			m_comboSendTo.MoveWindow(x,y,const_combo_sentto_width,12);
			m_comboSendTo.ShowWindow(SW_SHOW);
		}
		x += (const_combo_sentto_width+6);
		if (m_checkPrivate.GetSafeHwnd())
		{
			m_checkPrivate.MoveWindow(x,y+4,const_check_private_width,15);
			m_checkPrivate.ShowWindow(SW_SHOW);
		}
		x += const_check_private_width;
	}else
	{
		//x += const_bar_button_inteval;
	}
	m_btnImage.MovePoint(x,y);
	x += (const_bar_button_width+const_bar_button_inteval);
	m_btnEBSC.MovePoint(x,y);
	x += (const_bar_button_width);
	const int const_btn_min_ebsc_width = 10;
	if (m_btnMinEBSC.GetSafeHwnd()!=NULL)
		m_btnMinEBSC.MoveWindow(x,y,const_btn_min_ebsc_width,const_bar_button_height);
	//y -= 1;	// ?
	x += (const_btn_min_ebsc_width+const_bar_button_inteval);
	//x = cx-m_btnMsgRecord.GetImgWidth()-const_btn_right_inteval;
	m_btnMsgRecord.MovePoint(x,y);
	//x -= m_btnMsgRecord.GetImgWidth();
	//m_btnFileManager.MovePoint(x,y);
	x += (const_bar_button_width+const_bar_button_inteval);
	if (m_pPanelStatus!=NULL)
	{
		const int nPanelStatusWidth = const_close_left-x;
		m_pPanelStatus->MoveWindow(x,y+3,nPanelStatusWidth,22);

		//CRect rectPanelStatus;
		//m_pPanelStatus->GetClientRect(&rectPanelStatus);
		//m_pPanelStatus->MoveWindow(x,y+3,rectPanelStatus.Width(),22);

		//rectPanelStatus.bottom = const_splitter1_top;
		//rectPanelStatus.top = rectPanelStatus.bottom - 22;
		//m_pPanelStatus->MoveWindow(&rectPanelStatus);
	}
	//if (m_labelNotify1.GetSafeHwnd()!=NULL)
	//{
	//	const int nWidth1 = min(240,const_close_left-x);
	//	m_labelNotify1.MoveWindow(x,y+3,nWidth1,16);
	//}

	// 私聊
	//y = bIsDepDialog?(const_outpubc_height+const_splitter_height):0;
	y = 0;
	if (m_pMrFrameControl.GetSafeHwnd())
	{
		const bool hIsScrollEnd = m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE?true:false;
		m_pMrFrameInterface->UpdateSize(VARIANT_FALSE);
		m_pMrFrameControl.MoveWindow(0,y,cx,const_outself_height);
		if (hIsScrollEnd)
			m_pMrFrameInterface->ScrollToPos(-1);
	}
	//if (m_pwebOutWndSelf && m_pwebOutWndSelf->GetSafeHwnd())
	//{
	//	m_pwebOutWndSelf->MoveWindow(0, y, cx, const_outself_height);
	//}
}

void CDlgChatInput::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	MoveSize(cx,cy);
}

//static DWORD CALLBACK MyStreamOutCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
//{
//	////CFile* pFile = (CFile*) dwCookie;
//	////pFile->Write(pbBuff, cb);
//	*pcb = cb;
//	return 0;
//}

void CDlgChatInput::send()
{
	const tstring sLogonAccount = theApp.GetLogonAccount();
	BOOST_ASSERT (!sLogonAccount.empty());
	if (sLogonAccount.empty())
	{
		return;
	}
	// ??
	//if (m_bMustInviteUser)
	//{
	//	if (theEBAppClient.InviteCall(this->m_pCallInfo->GetCallId().c_str()) != 0)
	//		return;
	//	m_bMustInviteUser = false;
	//	m_btnSend.SetWindowText(_T("发送"));
	//	m_btnSend.Invalidate();
	//}

	EB_ChatRoomRichMsg pRichMsg;
	FormatMsg(&pRichMsg);
	const std::vector<EB_ChatRoomMsgItem*>& pRichMsgList = pRichMsg.GetList();
	if (pRichMsgList.empty())
	{
		m_richInput.SetFocus();
//		// 检查会话是否有离线成员，重新呼叫
//#ifdef USES_EBCOM_TEST
//		if (theEBClientCore->EB_CheckReCall(this->m_pCallInfo.GetCallId(),VARIANT_TRUE) == S_OK)
//#else
//		if (theEBAppClient.EB_CheckReCall(this->m_pCallInfo.GetCallId(),false) == 0)
//#endif
//		{
//			// 所有成员在线
//			m_btnSend.SetWindowText(_T("发送"));
//		}
		return;
	}

	const int nCurSel = m_comboSendTo.GetCurSel();
	eb::bigint nToUserId = 0;
	CString sToAccount;
	const bool bIsDepDialog = m_pCallInfo.m_sGroupCode>0;
	if (nCurSel > 0)
	{
		CTreeItemInfo * pTreeItemInfo = (CTreeItemInfo*)m_comboSendTo.GetItemData(nCurSel);
		if (pTreeItemInfo==NULL) return;
		sToAccount = pTreeItemInfo->m_sAccount.c_str();
		nToUserId = pTreeItemInfo->m_nUserId;
		UpdateData();
	}
#ifdef USES_EBCOM_TEST
	CComSafeArray<VARIANT> m_sa;
	const std::vector<EB_ChatRoomMsgItem*> & pList = pRichMsg.GetList();
	for (size_t i=0;i<pList.size();i++)
	{
		const EB_ChatRoomMsgItem* pItem = pList[i];
		CIEB_ChatMsgItem* pIEBChatMsgItem = new CIEB_ChatMsgItem(*pItem);
		CComPtr<IEB_ChatMsgItem> pOutInterface;
		pIEBChatMsgItem->QueryInterface(__uuidof(IEB_ChatMsgItem),(void**)&pOutInterface);
		m_sa.Add(CComVariant(pOutInterface));
	}
	CComVariant var(m_sa);
	if (theEBClientCore->EB_SendRich(m_pCallInfo.GetCallId(),&var,nToUserId,m_checkPrivate.GetCheck()?VARIANT_TRUE:VARIANT_FALSE)==S_OK)
#else
	int ret = theEBAppClient.EB_SendRich(m_pCallInfo.GetCallId(),&pRichMsg,nToUserId,m_checkPrivate.GetCheck()?true:false);
	if (ret == 0)
#endif
	{
		m_richInput.SetSel(0, -1);
		m_richInput.Clear();
		//m_richInput.SetWindowText(_T(""));
		//m_btnSend.SetWindowText(_T("发送"));
	}
#ifndef USES_EBCOM_TEST
	else {
		/// 3=底层已经自动重连
		/// other=其他情况，由上层自动重连
		if (ret != 3) {
			call();
		}
		CDlgMessageBox::EbMessageBox(this,"",_T("当前会话掉线：\r\n正在自动重连，请稍候再试！"),CDlgMessageBox::IMAGE_WARNING,10);
	}
#endif
	m_richInput.SetFocus();
}

void CDlgChatInput::call()
{

	if (m_pCallInfo.m_sGroupCode > 0) {
		theEBAppClient.EB_CallGroup(m_pCallInfo.m_sGroupCode);
	}
	else {
		theEBAppClient.EB_CallUserId(m_pCallInfo.GetFromUserId(), m_pCallInfo.GetCallId());
	}

}

void CDlgChatInput::OnBnClickedButtonSend()
{
	send();
}

void CDlgChatInput::CHARFORMAT2CR(const CHARFORMAT& cf,CR_CHARFORMATA& pOutCrcf)
{
	pOutCrcf.dwEffects = cf.dwEffects;
	pOutCrcf.yHeight = cf.yHeight;
	//crcf.yOffset = cf.yOffset;;
	pOutCrcf.crTextColor = cf.crTextColor;
	//crcf.bCharSet = cf.bCharSet;
	//crcf.bPitchAndFamily = cf.bPitchAndFamily;
	strcpy(pOutCrcf.szFaceName,cf.szFaceName);
}

//CString CDlgChatInput::CharFormat2Html(const CR_CHARFORMATA & cf, const char * lpszText)
//{
//	CString strTemp; 
//	TCHAR buf[32];
//
//	strTemp += _T("<font");
//	strTemp = strTemp + _T(" face=\"") + cf.szFaceName + _T("\"");
//	strTemp += _T(" style=\"font-size:");
//
//	_snprintf(buf, sizeof(buf) , _T("%dpx;\""), cf.yHeight/20+5 );
//	strTemp = strTemp + buf;
//
//	_snprintf(buf, sizeof(buf), _T("#%02X%02X%02X"), GetRValue(cf.crTextColor), GetGValue(cf.crTextColor), GetBValue(cf.crTextColor));
//	strTemp = strTemp + _T(" color=") + buf + _T(">");
//
//	if (/* (cf.dwMask&CFM_BOLD) &&*/
//		(cf.dwEffects&CFE_BOLD) )
//	{
//		strTemp += _T("<b>");
//	}
//	if(/* (cf.dwMask&CFM_UNDERLINE) &&*/
//		(cf.dwEffects&CFE_UNDERLINE) )
//	{
//		strTemp += _T("<u>");
//	}	
//	if( /*(cf.dwMask&CFM_STRIKEOUT) &&*/
//		(cf.dwEffects&CFM_STRIKEOUT) )
//	{
//		strTemp += _T("<Strike>");
//	}	
//	if( /*(cf.dwMask&CFM_ITALIC) &&*/
//		(cf.dwEffects&CFE_ITALIC) )
//	{
//		strTemp += _T("<i>");
//	}
//	strTemp += lpszText;
//
//	if(/* (cf.dwMask&CFM_ITALIC) &&*/
//		(cf.dwEffects&CFE_ITALIC) )
//	{
//		strTemp += _T("</i>");
//	}
//	if(/* (cf.dwMask&CFM_STRIKEOUT) &&*/
//		(cf.dwEffects&CFM_STRIKEOUT) )
//	{
//		strTemp += _T("</Strike>");
//	}	
//	if( /*(cf.dwMask&CFM_UNDERLINE) &&*/
//		(cf.dwEffects&CFE_UNDERLINE) )
//	{
//		strTemp += _T("</u>");
//	}
//	if( /*(cf.dwMask&CFM_BOLD) &&*/
//		(cf.dwEffects&CFE_BOLD) )
//	{
//		strTemp += _T("</b>");
//	}		
//
//	strTemp += _T("</font>");
//	return strTemp;
//}

void CDlgChatInput::ScrollToEnd(void)
{
	if (m_pMrFrameInterface!=NULL)
		m_pMrFrameInterface->ScrollToPos(-1);
}

void CDlgChatInput::SetScreenCopyFinished(void)
{
	// ** 使用定时器，解决某些情况下，不能正常复制屏幕截图问题
	SetTimer(TIMERID_SC_OK,1,NULL);
	return;
	//Send   control   键
	//BYTE pbKeyState[256];
	//GetKeyboardState((LPBYTE)&pbKeyState);
	//pbKeyState[VK_CONTROL] |= 0x80;
	//SetKeyboardState((LPBYTE)&pbKeyState);
	////Send   'v '   键
	////UINT wParam = 0x56;
	////::keybd_event(wParam,0,0,0);				//按下v
	////::keybd_event(wParam,0,KEYEVENTF_KEYUP, 0);	//放开v
	//m_richInput.PostMessage(WM_KEYDOWN, 0x56, 0);
	//m_richInput.PostMessage(WM_KEYUP, 0x56, 0);
}
void CDlgChatInput::SendFile(const char* lpszFilePath,bool bCheckImage)
{
	if (IsForbidSpeech())
	{
		// ** 禁言限制中
		return;
	}

	if (bCheckImage)
	{
		USES_CONVERSION;
		Gdiplus::Bitmap pImage(A2W_ACP(lpszFilePath));
		const bool bIsImage = (bool)(pImage.GetType()!= Gdiplus::ImageTypeUnknown);
		if (bIsImage)
		{
			HBITMAP hBitmap = NULL;
			pImage.GetHBITMAP(Color(255,255,255),&hBitmap);
			if (hBitmap!=NULL)
			{
				m_richInput.InsertBitmap(hBitmap,0);
				return;
			}
		}
	}

	if (m_pCallInfo.m_sGroupCode>0)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_AddTempGroupFileRes(lpszFilePath,"",m_pCallInfo.m_sGroupCode);
#else
		theEBAppClient.EB_AddTempGroupFileRes(lpszFilePath,"",m_pCallInfo.m_sGroupCode);
#endif
	}else
	{
		eb::bigint nToUserId = 0;
		//CString sToAccount;
		const int nCurSel = m_comboSendTo.GetCurSel();
		if (nCurSel > 0)	// >=
		{
			const CTreeItemInfo * pTreeItemInfo = (CTreeItemInfo*)m_comboSendTo.GetItemData(nCurSel);
			if (pTreeItemInfo==NULL) return;
			//sToAccount = pTreeItemInfo->m_sAccount.c_str();
			nToUserId = pTreeItemInfo->m_nUserId;
			UpdateData();	// update m_bPrivate
		}
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_SendFile(m_pCallInfo.GetCallId(),lpszFilePath,nToUserId,m_checkPrivate.GetCheck()?VARIANT_TRUE:VARIANT_FALSE,VARIANT_FALSE);
#else
		theEBAppClient.EB_SendFile(m_pCallInfo.GetCallId(),lpszFilePath,nToUserId,m_checkPrivate.GetCheck()?true:false);
#endif
	}
}

#ifdef USES_EBCOM_TEST
void CDlgChatInput::OnUserEmpInfo(IEB_MemberInfo* pMemberInfo)
{
	//if (m_pDlgUserList)
	//{
	//	m_pDlgUserList->OnUserEmpInfo(pMemberInfo);
	//}
}
#else
void CDlgChatInput::OnUserEmpInfo(const EB_MemberInfo* pMemberInfo)
{
	if (pMemberInfo->m_nMemberUserId==theApp.GetLogonUserId())
	{
		CheckMyForbidSpeechState(true,true);
	}

	//if (m_pDlgUserList)
	//{
	//	m_pDlgUserList->OnUserEmpInfo(pMemberInfo);
	//}
}
#endif

void CDlgChatInput::ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo)
{
	CheckMyForbidSpeechState(true,false);
}

#ifdef USES_EBCOM_TEST
void CDlgChatInput::SelectedEmp(IEB_MemberInfo* pMemberInfo)
{
	CTreeItemInfo::pointer pTreeItemInfo;
	int nSelIndex = 0;
	if (pMemberInfo == NULL)
		nSelIndex = 0;
	else
	{
		const CEBString sMemberAccount(pMemberInfo->MemberAccount.GetBSTR());
		const CEBString sUserName(pMemberInfo->UserName.GetBSTR());
		if (!m_pSendToAccount.find(pMemberInfo->MemberUserId,pTreeItemInfo))
		{
			nSelIndex = m_comboSendTo.AddString(sUserName.c_str());
			pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,nSelIndex);
			m_comboSendTo.SetItemData(nSelIndex,(DWORD)pTreeItemInfo.get());
			pTreeItemInfo->m_sGroupCode = pMemberInfo->GroupCode;
			pTreeItemInfo->m_sMemberCode = pMemberInfo->MemberCode;
			pTreeItemInfo->m_nUserId = pMemberInfo->MemberUserId;
			pTreeItemInfo->m_sAccount = sMemberAccount;
			m_pSendToAccount.insert(pMemberInfo->MemberUserId,pTreeItemInfo);
		}else
		{
			nSelIndex = pTreeItemInfo->m_nIndex;
		}
	}
	m_comboSendTo.SetCurSel(nSelIndex);
}
#else
void CDlgChatInput::SelectedEmp(const EB_MemberInfo* pMemberInfo)
{
	CTreeItemInfo::pointer pTreeItemInfo;
	int nSelIndex = 0;
	if (pMemberInfo == NULL)
		nSelIndex = 0;
	else if (!m_pSendToAccount.find(pMemberInfo->m_nMemberUserId,pTreeItemInfo))
	{
		nSelIndex = m_comboSendTo.AddString(pMemberInfo->m_sUserName.c_str());
		pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,nSelIndex);
		m_comboSendTo.SetItemData(nSelIndex,(DWORD)pTreeItemInfo.get());
		pTreeItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
		pTreeItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
		pTreeItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
		pTreeItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
		m_pSendToAccount.insert(pMemberInfo->m_nMemberUserId,pTreeItemInfo);
	}else
	{
		nSelIndex = pTreeItemInfo->m_nIndex;
	}
	m_comboSendTo.SetCurSel(nSelIndex);
}
#endif
void CDlgChatInput::SelectedEmp(eb::bigint nUserId)
{
	CTreeItemInfo::pointer pTreeItemInfo;
	int nSelIndex = 0;
	if (nUserId == NULL)
		nSelIndex = 0;
	else if (!m_pSendToAccount.find(nUserId,pTreeItemInfo))
	{
#ifdef USES_EBCOM_TEST
		CComPtr<IEB_MemberInfo> pMemberInfoTemp = theEBClientCore->EB_GetMemberInfoByUserId(m_pCallInfo.m_sGroupCode,nUserId);
		if (pMemberInfoTemp == NULL)
			return;
		EB_MemberInfo pMemberInfo(pMemberInfoTemp);
#else
		EB_MemberInfo pMemberInfo;
		if (!theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,m_pCallInfo.m_sGroupCode,nUserId))
			return;
#endif
		nSelIndex = m_comboSendTo.AddString(pMemberInfo.m_sUserName.c_str());
		pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,nSelIndex);
		m_comboSendTo.SetItemData(nSelIndex,(DWORD)pTreeItemInfo.get());
		pTreeItemInfo->m_sGroupCode = pMemberInfo.m_sGroupCode;
		pTreeItemInfo->m_sMemberCode = pMemberInfo.m_sMemberCode;
		pTreeItemInfo->m_nUserId = pMemberInfo.m_nMemberUserId;
		pTreeItemInfo->m_sAccount = pMemberInfo.m_sMemberAccount;
		m_pSendToAccount.insert(pMemberInfo.m_nMemberUserId,pTreeItemInfo);
	}else
	{
		nSelIndex = pTreeItemInfo->m_nIndex;
	}
	m_comboSendTo.SetCurSel(nSelIndex);
}


void CDlgChatInput::AddLineString(eb::bigint nMsgId,const CString& sText, int nAlignmentFormat, bool bWriteTime)
{
	m_pMrFrameInterface->AddLine(nMsgId);
	m_pMrFrameInterface->SetAlignmentFormat(nAlignmentFormat);
	m_pMrFrameInterface->WriteString((LPCTSTR)sText,theDefaultChatSystemColor);
	if (bWriteTime)
		m_pMrFrameInterface->WriteTime(0,"%H:%M");
	const bool hIsScrollEnd = (m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
	m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
	if (hIsScrollEnd)
		m_pMrFrameInterface->ScrollToPos(-1);
	//m_pMrFrame.Invalidate();
}

#ifdef USES_EBCOM_TEST
void CDlgChatInput::OnSentFile(IEB_ChatFileInfo* pCrFileInfo)
{
	//if (pCrFileInfo->ReceiveAccount==theApp.GetLogonUserId())
		ProcessFile(false,pCrFileInfo);
}
#else
bool CDlgChatInput::OnSentFile(const CCrFileInfo * pCrFileInfo, EB_STATE_CODE nState)
{
	//if (pCrFileInfo->m_sReceiveAccount.empty())	// 错误
	//	return;
	//if (pCrFileInfo->m_sReceiveAccount==theEBAppClient.EB_GetLogonUserId())
		return ProcessFile(false,pCrFileInfo,nState);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatInput::OnReceivedFile(IEB_ChatFileInfo* pCrFileInfo)
{
	ProcessFile(true,pCrFileInfo);
}
#else
void CDlgChatInput::OnReceivedFile(const CCrFileInfo * pCrFileInfo)
{
	ProcessFile(true,pCrFileInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatInput::ProcessFile(bool bReceive,IEB_ChatFileInfo* pCrFileInfo)
{
	CString sWindowText;
	m_pMrFrameInterface->AddLine(0);
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	const eb::bigint sSendFrom(pCrFileInfo->SendFrom);
	const eb::bigint sSendTo(pCrFileInfo->SendFrom);
	const eb::bigint sReceiveAccount(pCrFileInfo->ReceiveAccount);
	const CEBString sFileName(pCrFileInfo->FileName.GetBSTR());
	const CEBString sFileTime(pCrFileInfo->FileTime.GetBSTR());
	time_t tMsgTime = time(0);
	if (!sFileTime.empty())
	{
		libEbc::ChangeTime(sFileTime.c_str(),tMsgTime);
	}
	WriteMsgDate(tMsgTime);
	if (bReceive)
	{
		sWindowText.Format(_T("已经接收文件"));
	}else if (sReceiveAccount==theApp.GetLogonUserId())
	{
		if (pCrFileInfo->OffFile)
			sWindowText.Format(_T("成功发送离线文件"));
		else
			sWindowText.Format(_T("成功发送文件"));
	}else
	{
		if (this->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方已经接收文件"));
		else
			sWindowText.Format(_T("%lld已经接收文件"),sReceiveAccount);
	}
	m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,theDefaultChatSystemColor);
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M");

	m_pMrFrameInterface->AddLine(pCrFileInfo->MsgId);
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theDefaultChatBackGroundColor1);

	USES_CONVERSION;
	const std::wstring sImagePath = pCrFileInfo->FileName.GetBSTR();
	const tstring sFilePath = libEbc::GetFileName(W2A_ACP(sImagePath.c_str()));
	const int nWavTimeLength = libEbc::GetWaveTimeLength(sFilePath.c_str());
	if (nWavTimeLength > 0)
	{
		// 是WAV语音文件
		const tstring sFileName = libEbc::GetFileName(W2A_ACP(sImagePath.c_str()));
		m_pMrFrameInterface->WriteWav(sFileName.c_str(),W2A_ACP(sImagePath.c_str()),EB_MR_CTRL_TYPE_LCLICK_OPEN);
	}else
	{
		bool bIsImage = false;
		eb::bigint nFileSize = -1;
		FILE * f = fopen(sFilePath.c_str(), "rb");
		if (f != NULL)
		{
			_fseeki64(f, 0, SEEK_END);
			nFileSize = _ftelli64(f);
			fclose(f);
			Gdiplus::Image * image = Gdiplus::Image::FromFile(sImagePath.c_str());
			//Gdiplus::ImageType imagetype = image->GetType();
			//Gdiplus::Status status = image->GetLastStatus();
			bIsImage = (bool)(image->GetType()!= Gdiplus::ImageTypeUnknown);
			delete image;
		}
		if (bIsImage)
		{
			CString sTemp;
			sTemp.Format(_T("#CTRL:%d::#%s"),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,pCrFileInfo->FileName.c_str());
			m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
		}else
		{
			// SHGetFileInfo获取只能获取到32*32的图标。用SHDefExtractIcon能获取到48*48的图标，
			//HICON hIcon = ::ExtractIcon(AfxGetInstanceHandle(),_T("ebc.exe"),0);
			SHFILEINFO sfi; 
			ZeroMemory(&sfi,sizeof(sfi)); 
			DWORD ret = SHGetFileInfo(sFileName.c_str(),
				FILE_ATTRIBUTE_NORMAL, 
				&sfi, 
				sizeof(sfi), 
				SHGFI_USEFILEATTRIBUTES|SHGFI_ICON);
			if (ret == 1)
			{
				m_pMrFrameInterface->WriteHICON((ULONG*)sfi.hIcon,pCrFileInfo->FileName);
			}
			const tstring sFileName = libEbc::GetFileName(sFilePath.c_str());
			CString sFileText;
			if (nFileSize == -1)
				sFileText.Format(_T("%s (文件不存在)"),sFileName.c_str());
			else if (nFileSize >= const_gb_size)
				sFileText.Format(_T("%s (%.02fGB)"),sFileName.c_str(),(double)nFileSize/const_gb_size);
			else if (nFileSize >= const_mb_size)
				sFileText.Format(_T("%s (%.02fMB)"),sFileName.c_str(),(double)nFileSize/const_mb_size);
			else if (nFileSize >= const_kb_size)
				sFileText.Format(_T("%s (%.02fKB)"),sFileName.c_str(),(double)nFileSize/const_kb_size);
			else
				sFileText.Format(_T("%s (%lldByte)"),sFileName.c_str(),nFileSize);
			m_pMrFrameInterface->WriteSpace(1);
			m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, pCrFileInfo->FileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,theDefaultFlatBlackTextColor);
			//m_pMrFrameInterface->WriteUrl(sFileName.c_str(), pCrFileInfo->FileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,theDefaultFlatBlackTextColor);
		}
		m_pMrFrameInterface->WriteLine();
		m_pMrFrameInterface->WriteSpace(11);
		if (nFileSize>=0)
		{
			m_pMrFrameInterface->WriteOpenFile(L"打开",pCrFileInfo->FileName);
			m_pMrFrameInterface->WriteSpace(2);
		}
		m_pMrFrameInterface->WriteOpenDir(L"打开文件夹",pCrFileInfo->FileName);
	}
	const bool hIsScrollEnd = (!bReceive || m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
	m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
	if (hIsScrollEnd)
		m_pMrFrameInterface->ScrollToPos(-1);
}
#else
void CDlgChatInput::OnReceivingFile(bool bOffLineUser, const CCrFileInfo * pCrFileInfo, CString* sOutFirstMsg)
{
	const eb::bigint sSendFrom = pCrFileInfo->m_sSendFrom;
	const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);
	CString sWindowText;
	if (this->m_pCallInfo.m_sGroupCode>0 && pCrFileInfo->m_sResId>0)
	{
		CEBString sMemberName;
		theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,sSendFrom,sMemberName);
		sWindowText.Format(_T("%s 上传群共享文件：%s"),sMemberName.c_str(),sFileName.c_str());
//#ifdef USES_EBCOM_TEST
//		theEBClientCore->EB_CancelFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
//#else
//		theEBAppClient.EB_CancelFile(m_pCrFileInfo.GetCallId(),m_pCrFileInfo.m_nMsgId);
//#endif

		const mycp::bigint nFileSize = pCrFileInfo->m_nFileSize;
		// *
		CString sFileText;
		if (nFileSize >= const_gb_size)
			sFileText.Format(_T("上传群共享文件：%s(%.02fGB)"),sFileName.c_str(),(double)nFileSize/const_gb_size);
		else if (nFileSize >= const_mb_size)
			sFileText.Format(_T("上传群共享文件：%s(%.02fMB)"),sFileName.c_str(),(double)nFileSize/const_mb_size);
		else if (nFileSize >= const_kb_size)
			sFileText.Format(_T("上传群共享文件：%s(%.02fKB)"),sFileName.c_str(),(double)nFileSize/const_kb_size);
		else if (nFileSize>0)
			sFileText.Format(_T("上传群共享文件：%s(%lldByte)"),sFileName.c_str(),nFileSize);
		else
			sFileText.Format(_T("上传群共享文件：%s"), sFileName.c_str());

		CString sText;
		sText.Format(_T("%s"),sMemberName.c_str());
		//this->AddLineString(0, sText, 1);	// 显示中间
		const bool bReceive = true;
		m_pMrFrameInterface->AddLine(pCrFileInfo->m_nMsgId);
		//m_pMrFrameInterface->AddLine(0);
		//m_pMrFrameInterface->SetAlignmentFormat(1);
		m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
		m_pMrFrameInterface->WriteString((LPCTSTR)sText,theDefaultChatSystemColor);
		m_pMrFrameInterface->WriteSpace(1);
		m_pMrFrameInterface->WriteTime(0,"%H:%M");

		m_pMrFrameInterface->AddLine(pCrFileInfo->m_nMsgId);
		//m_pMrFrameInterface->SetAlignmentFormat(1);
		m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
		m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
		m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
		m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theDefaultChatBackGroundColor1);
		WriteFileHICON(sFileName.c_str(),0);
		m_pMrFrameInterface->WriteString((LPCTSTR)sFileText,theDefaultChatSystemColor);
		m_pMrFrameInterface->WriteSpace(1);
		sText.Format(_T("#CTRL:%d:%d:%lld,%s#下载"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB),(int)EB_MR_CTRL_DATA_TYPE_DOWNLOAD_RESOURCE,pCrFileInfo->m_sResId,sFileName.c_str());
		m_pMrFrameInterface->WriteString((LPCTSTR)sText,RGB(0, 0, 255));
		if (m_pCallInfo.m_sGroupCode>0 && !theApp.GetDisableGroupSharedCloud())
		{
			m_pMrFrameInterface->WriteSpace(2);
			sText.Format(_T("#CTRL:%d:%d:0#群共享"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB),(int)EB_MR_CTRL_DATA_TYPE_OPEN_SHARE);
			m_pMrFrameInterface->WriteString((LPCTSTR)sText,RGB(0, 0, 255));
		}
		const bool hIsScrollEnd = (!bReceive || m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
		m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
		if (hIsScrollEnd)
			m_pMrFrameInterface->ScrollToPos(-1);

		// ** 保存本地数据库
		if (theApp.GetSaveConversationLocal() && !theEBAppClient.EB_IsLogonVisitor())
		{
			CString sMsgText;
			sMsgText.Format(_T("%lld,%lld"),pCrFileInfo->m_sResId,nFileSize);
			tstring sInMemberName(sMemberName);
			theApp.m_pBoUsers->escape_string_in(sInMemberName);
			tstring sInFileName(sFileName);
			theApp.m_pBoUsers->escape_string_in(sInFileName);
			CString sSql;
			sSql.Format(_T("INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_name,msg_text) ")\
				_T("VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s','%s')"),
				pCrFileInfo->m_nMsgId,m_pCallInfo.m_sGroupCode,pCrFileInfo->m_sSendFrom,libEbc::ACP2UTF8(sInMemberName.c_str()).c_str(),
				pCrFileInfo->m_sSendTo,pCrFileInfo->m_bPrivate?1:0,MRT_RESOURCE,libEbc::ACP2UTF8(sInFileName.c_str()).c_str(),sMsgText);
			theApp.m_pBoUsers->execute(sSql);
		}
//#ifdef USES_EBCOM_TEST
//		theEBClientCore->EB_CancelFile(pCrFileInfo->GetCallId(),pCrFileInfo->m_nMsgId);
//#else
//		theEBAppClient.EB_CancelFile(pCrFileInfo->GetCallId(),pCrFileInfo->m_nMsgId);
//#endif
	}else if (bOffLineUser || pCrFileInfo->m_bOffFile)
		//if (m_pCallInfo->m_bOffLineCall)
		//if (m_bReceiveOffLineMsg)
	{
		sWindowText.Format(_T("对方发送离线文件：%s"),sFileName.c_str());
		AddLineString(pCrFileInfo->m_nMsgId,sWindowText);
	}else
	{
		sWindowText.Format(_T("对方发送文件：%s"),sFileName.c_str());
		AddLineString(pCrFileInfo->m_nMsgId,sWindowText);
	}
	if (sOutFirstMsg!=NULL)
	{
		if (m_pCallInfo.m_sGroupCode>0)
			sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-group://%lld\">%s</a>"),libEbc::ACP2UTF8(sWindowText).c_str(),m_pCallInfo.m_sGroupCode,libEbc::ACP2UTF8("接收文件").c_str());
		else
			sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-account://%lld\">%s</a>"),libEbc::ACP2UTF8(sWindowText).c_str(),m_pCallInfo.GetFromUserId(),libEbc::ACP2UTF8("接收文件").c_str());
	}
}
bool CDlgChatInput::ProcessFile(bool bReceive,const CCrFileInfo * pCrFileInfo,EB_STATE_CODE nState)
{
	time_t tMsgTime = time(0);
	if (!pCrFileInfo->m_sFileTime.empty())
	{
		libEbc::ChangeTime(pCrFileInfo->m_sFileTime.c_str(),tMsgTime);
	}
	WriteMsgDate(tMsgTime);

	CString sWindowText;
	m_pMrFrameInterface->AddLine(pCrFileInfo->m_nMsgId);
	//m_pMrFrameInterface->AddLine(0);
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	bool bUpdateMsgReceiptData = false;
	bool bUploadGroupFile = false;
	if (bReceive)
	{
		if (pCrFileInfo->m_sResId>0)
			sWindowText.Format(_T("成功下载文件"));
		else
			sWindowText.Format(_T("成功接收文件"));
	}else if (pCrFileInfo->m_sReceiveAccount==theEBAppClient.EB_GetLogonUserId())
	{
		if (pCrFileInfo->m_bOffFile)
			sWindowText.Format(_T("成功发送离线文件"));
		else
		{
			if (m_pPrevReceivedFileMsgIdList.remove(pCrFileInfo->m_nMsgId))
			{
				return false;	// *前面已经提示“对方已经接收文件”，直接返回不再打印提示
			}
			//if (m_nPrevReceivedFileMsgId>0 && m_nPrevReceivedFileMsgId==pCrFileInfo->m_nMsgId)
			//{
			//	m_nPrevReceivedFileMsgId = 0;
			//	return false;	// *前面已经提示“对方已经接收文件”，直接返回不再打印提示
			//}
			if (this->m_pCallInfo.m_sGroupCode>0)
			{
				bUploadGroupFile = true;
				sWindowText.Format(_T("上传群共享文件"));
			}else
				sWindowText.Format(_T("成功发送文件"));
		}
	}else
	{
		if (this->m_pCallInfo.m_sGroupCode==0)
		{
			sWindowText.Format(_T("对方已经接收文件"));
			m_pPrevReceivedFileMsgIdList.insert(pCrFileInfo->m_nMsgId,true,false);
			//m_nPrevReceivedFileMsgId = pCrFileInfo->m_nMsgId;
			bUpdateMsgReceiptData = true;
		}else
		{
			sWindowText.Format(_T("%lld已经接收"),pCrFileInfo->m_sReceiveAccount);
		}
	}
	if (bUploadGroupFile)
	{
		//time_t tMsgTime = time(0);
		//if (!pCrFileInfo->m_sFileTime.empty())
		//{
		//	libEbc::ChangeTime(pCrFileInfo->m_sFileTime.c_str(),tMsgTime);
		//}
		WriteMsgDate(tMsgTime);

		tstring sFromName;
		theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,pCrFileInfo->m_sSendFrom,sFromName);
		m_pMrFrameInterface->WriteString((LPCTSTR)sFromName.c_str(),theDefaultChatTitleColor1);
	}else
	{
		m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,theDefaultChatSystemColor);
	}
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M");

	m_pMrFrameInterface->AddLine(pCrFileInfo->m_nMsgId);
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	if (!bReceive && m_pCallInfo.m_sGroupCode==0)
	{
		const long nReceiptFlag = bUpdateMsgReceiptData?(EBC_CONTRON_RECEIPT_FLAG_TRUE|EBC_CONTRON_RECEIPT_FLAG_SHOW):EBC_CONTRON_RECEIPT_FLAG_SHOW;
		m_pMrFrameInterface->SetReceiptFlag(nReceiptFlag);
	}
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theDefaultChatBackGroundColor1);
	const int nWavTimeLength = libEbc::GetWaveTimeLength(pCrFileInfo->m_sFileName.c_str());
	if (nWavTimeLength > 0)
	{
		// 是WAV语音文件
		const tstring sFileName(libEbc::GetFileName((LPCTSTR)pCrFileInfo->m_sFileName.c_str()));
		CString sText;
		sText.Format(_T("%s %d\""),sFileName.c_str(),nWavTimeLength);
		m_pMrFrameInterface->WriteWav((LPCTSTR)sText,pCrFileInfo->m_sFileName.c_str(),EB_MR_CTRL_TYPE_LCLICK_OPEN);
		//m_pMrFrameInterface->WriteWav(sFileName.c_str(),pCrFileInfo->m_sFileName.c_str(),EB_MR_CTRL_TYPE_LCLICK_OPEN);
	}else
	{
		bool bIsImage = false;
		eb::bigint nFileSize = -1;
		FILE * f = fopen(pCrFileInfo->m_sFileName.c_str(), "rb");
		if (f != NULL)
		{
			_fseeki64(f, 0, SEEK_END);
			nFileSize = _ftelli64(f);
			fclose(f);
			USES_CONVERSION;
			const std::wstring sImagePath = A2W_ACP(pCrFileInfo->m_sFileName.c_str());
			Gdiplus::Image * image = Gdiplus::Image::FromFile(sImagePath.c_str());
			//Gdiplus::ImageType imagetype = image->GetType();
			//Gdiplus::Status status = image->GetLastStatus();
			bIsImage = (bool)(image->GetType()!= Gdiplus::ImageTypeUnknown);
			delete image;
		}
		if (bIsImage)
		{
			const tstring sFilePath(libEbc::URLEncode(pCrFileInfo->m_sFileName.c_str()));
			CString sTemp;
			sTemp.Format(_T("#CTRL:%d:%d:%s#%s"),(int)(EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_IMAGE,sFilePath.c_str(),pCrFileInfo->m_sFileName.c_str());
			m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
				m_pMrFrameInterface->WriteLine();
		}else
		{
			// SHGetFileInfo获取只能获取到32*32的图标。用SHDefExtractIcon能获取到48*48的图标，
			//HICON hIcon = ::ExtractIcon(AfxGetInstanceHandle(),_T("ebc.exe"),0);
			WriteFileHICON(pCrFileInfo->m_sFileName.c_str());
		}

		const CString sTextExt = bUploadGroupFile?_T("上传群共享文件："):_T("");
		{
			const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);
			CString sFileText;
			if (nFileSize == -1)
				sFileText.Format(_T("%s%s (文件不存在)"),sTextExt,sFileName.c_str());
			else if (nFileSize >= const_gb_size)
				sFileText.Format(_T("%s%s (%.02fGB)"),sTextExt,sFileName.c_str(),(double)nFileSize/const_gb_size);
			else if (nFileSize >= const_mb_size)
				sFileText.Format(_T("%s%s (%.02fMB)"),sTextExt,sFileName.c_str(),(double)nFileSize/const_mb_size);
			else if (nFileSize >= const_kb_size)
				sFileText.Format(_T("%s%s (%.02fKB)"),sTextExt,sFileName.c_str(),(double)nFileSize/const_kb_size);
			else
				sFileText.Format(_T("%s%s (%lldByte)"),sTextExt,sFileName.c_str(),nFileSize);
			m_pMrFrameInterface->WriteSpace(1);
			CString sTemp;
			sTemp.Format(_T("#CTRL:0:%d:#%s"),(int)EB_MR_CTRL_DATA_TYPE_FILE,sFileText);
			m_pMrFrameInterface->WriteUrl((LPCTSTR)sTemp, pCrFileInfo->m_sFileName.c_str(),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB,theDefaultFlatBlackTextColor);
			//m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, pCrFileInfo->m_sFileName.c_str(),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,theDefaultFlatBlackTextColor);
		}
		//m_pMrFrameInterface->WriteLine();
		//m_pMrFrameInterface->WriteSpace(11);
		//if (nFileSize>=0)
		//{
		//	m_pMrFrameInterface->WriteOpenFile(L"打开",pCrFileInfo->m_sFileName.c_str());
		//	m_pMrFrameInterface->WriteSpace(2);
		//}
		//m_pMrFrameInterface->WriteOpenDir(L"打开文件夹",pCrFileInfo->m_sFileName.c_str());
	}
	if (bUploadGroupFile)
	{
		const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);
		m_pMrFrameInterface->WriteSpace(1);
		CString sText;
		sText.Format(_T("#CTRL:%d:%d:%lld,%s#下载"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB),(int)EB_MR_CTRL_DATA_TYPE_DOWNLOAD_RESOURCE,pCrFileInfo->m_sResId,sFileName.c_str());
		m_pMrFrameInterface->WriteString((LPCTSTR)sText,RGB(0, 0, 255));
		if (m_pCallInfo.m_sGroupCode>0 && !theApp.GetDisableGroupSharedCloud())
		{
			m_pMrFrameInterface->WriteSpace(2);
			sText.Format(_T("#CTRL:%d:%d:0#群共享"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB),(int)EB_MR_CTRL_DATA_TYPE_OPEN_SHARE);
			m_pMrFrameInterface->WriteString((LPCTSTR)sText,RGB(0, 0, 255));
		}
	}

	if (bUpdateMsgReceiptData)
		UpdateMsgReceiptData(pCrFileInfo->m_nMsgId, m_pFromAccountInfo.GetUserId(), 0, nState);

	const bool hIsScrollEnd = (!bReceive || m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
	m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
	if (hIsScrollEnd)
		m_pMrFrameInterface->ScrollToPos(-1);
	return true;
}
#endif

//typedef char      s8;    //有符号8位整数  
//typedef short     s16;   //有符号16位整数  
//typedef int       s32;   //有符号32位整数  
//struct WAVE_TAG  
//{  
//	s8     riff[4];            //"RIFF",资源交换文件标志  
//	s32    fsize;              //文件大小(从下个地址开始到文件尾的总字节数)  
//	s8     wave[4];            //"WAVE",文件标志  
//};  
//struct WAVE_FMT  
//{  
//	s8     fmt[4];             //"fmt ",波形格式标志   
//	s32    chunsize;           //文件内部格式信息大小  
//	s16    wformattag;         //音频数据编码方式 1PCM  
//	s16    wchanles;           //声道数 1,2  
//	s32    dwsamplespersec;    //采样率 441000  
//	s32    dwavgbytespersec;   //波形数据传输速率（每秒平均字节数）  
//	s16    wblockalign;        //数据的调整数（按字节计算）  
//	s16    wbitspersample;     //样本数据位数  
//};  
//struct WAVE_DAT  
//{  
//	s8     data[4];            //"data",数据标志符  
//	s32    datasize;           //采样数据总长度   
//};  
//struct WAVE_HEAD  
//{  
//	WAVE_TAG   WaveTag;   
//	WAVE_FMT   WaveFmt;  
//	WAVE_DAT   WaveDat;  
//}wavhead;  
//
//int CDlgChatInput::GetTimeLength(const char* lpszWavFilePath)
////获取声音文件数据的函数，pString参数指向要打开的声音文件；
//{
//	HMMIO file;//定义HMMIO文件句柄；
//	file=mmioOpen((char*)lpszWavFilePath,NULL,MMIO_READ);//以读写模式打开所给的WAVE文件；
//	if(file==NULL)
//	{
//		TRACE("WAVE文件打开失败！");
//		return -1;
//	}
//
//	char style[4];//定义一个四字节的数据，用来存放文件的类型；
//	mmioSeek(file,8,SEEK_SET);//定位到WAVE文件的类型位置
//	mmioRead(file,style,4);
//	if(style[0]!='W'||style[1]!='A'||style[2]!='V'||style[3]!='E')//判断该文件是否为"WAVE"文件格式
//	{
//		TRACE("该文件不是WAVE格式的文件！");
//		::mmioClose(file, 0);
//		return -1;
//	}
//
//	// WAV格式文件所占容量（KB) = （取样频率 X 量化位数 X 声道） X 时间 / 8 (字节= 8bit) ，每一分钟WAV格式的音频文件的大小为10MB，其大小不随音量大小及清晰度的变化而变化。
//
//	PCMWAVEFORMAT format; //定义PCMWAVEFORMAT结构对象，用来判断WAVE文件格式；
//	mmioSeek(file,20,SEEK_SET);
//	//对打开的文件进行定位，此时指向WAVE文件的PCMWAVEFORMAT结构的数据；
//	mmioRead(file,(char*)&format,sizeof(PCMWAVEFORMAT));//获取该结构的数据；
//
//	// 获取WAVE文件 data 数据标识
//	mmioSeek(file,36,SEEK_SET);
//	mmioRead(file,style,4);
//	if(style[0]!='d'||style[1]!='a'||style[2]!='t'||style[3]!='a')	//判断是否标准data文件，如果是表示表示44字节文件头，否则使用46字节文件头
//		mmioSeek(file,42,SEEK_SET);
//	//mmioSeek(file,40,SEEK_SET);
//	////获取WAVE文件的声音数据的大小；
//	unsigned long size = 0;
//	mmioRead(file,(char*)&size,4);
//
//	//计算文件时长
//	const int timeLength = size/format.wf.nAvgBytesPerSec;
//
//	::mmioClose(file, 0);
//	return timeLength;
//}

void CDlgChatInput::WriteFileHICON(const char* lpszFilePath,long nCtrlType)
{
	SHFILEINFO sfi; 
	ZeroMemory(&sfi,sizeof(sfi)); 
	DWORD ret = SHGetFileInfo(lpszFilePath,
		FILE_ATTRIBUTE_NORMAL, 
		&sfi, 
		sizeof(sfi), 
		SHGFI_USEFILEATTRIBUTES|SHGFI_ICON);
	if (ret == 1)
	{
		m_pMrFrameInterface->WriteHICON((ULONG*)sfi.hIcon,lpszFilePath,nCtrlType);
	}
}


//void CDlgChatInput::AddMessage(OUT_TYPE nOutType, const CString & sText, BOOL bAddTimestamp)
//{
//	const bool bIsDepDialog = m_pCallInfo.get()!=NULL && !m_pCallInfo->m_sGroupCode.empty();
//	CString sHtmlText(sText);
//	sHtmlText.Replace(_T("\r\n"),_T("<br>"));
//	sHtmlText.Replace(_T("\r"),_T("<br>"));
//	sHtmlText.Replace(_T("\n"),_T("<br>"));
//	
//	const tstring sUTF8HtmlText = str_convert(sHtmlText,CP_ACP,CP_UTF8);
//	if (bIsDepDialog && nOutType == OT_PUBLIC && m_pwebOutWndPubc)
//	{
//		m_pwebOutWndPubc->WriteBlock(sHtmlText, FALSE, bAddTimestamp);
//	}else if (m_pwebOutWndSelf)
//	{
//		m_pwebOutWndSelf->WriteBlock(sHtmlText, FALSE, bAddTimestamp);
//	}
//}

void CDlgChatInput::OnUserNotify(const CCrNotifyInfo* pNotifyInfo,CString* pOutFirstMsg)
{
	switch (pNotifyInfo->m_nNotifyType)
	{
	case 0:	// *未知类型，可用于自定义
		{
		}break;
	case 1:	// *用户正在输入
		{
			if (m_pCallInfo.m_sGroupCode==0)
			{
				KillTimer(TIMERID_HIDE_NOTIFY1);
				SetTimer(TIMERID_HIDE_NOTIFY1,12*1000,NULL);	// * 显示12秒后自动隐藏
				if (!m_pPanelStatus->IsWindowVisible())
					m_pPanelStatus->ShowWindow(SW_SHOW);
				//if (!m_labelNotify1.IsWindowVisible())
				//{
				//	m_labelNotify1.ShowWindow(SW_SHOW);
				//}
			}else
			{
				m_pNotifyList.add(CEbNotifyInfo::create(pNotifyInfo->m_nFromAccount));
			}
		}break;
	//case 2:	// *群共享，收到资源文件
	//	{
	//		// ** notify_data=[RESOURCE_ID]*[RESOURCE_TYPE]*[RESOURCE_SHARE_TYPE]*[FILE_SIZE]*[FILE_NAME]
	//		std::vector<tstring> pList;
	//		if (libEbc::ParseString(pNotifyInfo->m_sNotifyData.c_str(),"*",pList)<5) break;
	//		const mycp::bigint nResourceId = cgc_atoi64(pList[0].c_str());
	//		const EB_RESOURCE_TYPE nResourceType = (EB_RESOURCE_TYPE)atoi(pList[1].c_str());
	//		if (nResourceType!=EB_RESOURCE_FILE) break;
	//		const EB_RESOURCE_SHARE_TYPE nResourceShareType = (EB_RESOURCE_SHARE_TYPE)atoi(pList[2].c_str());
	//		const mycp::bigint nFileSize = cgc_atoi64(pList[3].c_str());
	//		const tstring sFileName = pList[4];

	//		tstring sMemberName;
	//		if (theEBAppClient.EB_GetMemberNameByUserId(this->m_pCallInfo.m_sGroupCode,pNotifyInfo->m_nFromAccount,sMemberName))
	//		{
	//			// *
	//			CString sFileText;
	//			if (nFileSize >= const_gb_size)
	//				sFileText.Format(_T("%s(%.02fGB)"),sFileName.c_str(),(double)nFileSize/const_gb_size);
	//			else if (nFileSize >= const_mb_size)
	//				sFileText.Format(_T("%s(%.02fMB)"),sFileName.c_str(),(double)nFileSize/const_mb_size);
	//			else if (nFileSize >= const_kb_size)
	//				sFileText.Format(_T("%s(%.02fKB)"),sFileName.c_str(),(double)nFileSize/const_kb_size);
	//			else if (nFileSize>0)
	//				sFileText.Format(_T("%s(%lldByte)"),sFileName.c_str(),nFileSize);
	//			else
	//				sFileText = sFileName.c_str();
	//			//CString sWindowTextTemp;
	//			//sWindowTextTemp.Format(_T("%s#CTRL:%d:%d:%lld#%s%s："),sPrivateText,(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_UID,sSender,sFromName.c_str(),sToText);
	//			//m_pMrFrameInterface->WriteString((LPCTSTR)sWindowTextTemp,crTextColor);

	//			if (pOutFirstMsg!=NULL)
	//			{
	//				if (nResourceShareType==EB_RESOURCE_SHARE_TEMP)
	//					pOutFirstMsg->Format(_T("%s 上传群临时文件"),sMemberName.c_str());
	//				else
	//					pOutFirstMsg->Format(_T("%s 上传群共享文件"),sMemberName.c_str());
	//			}
	//			CString sText;
	//			if (nResourceShareType==EB_RESOURCE_SHARE_TEMP)
	//				sText.Format(_T("-------- %s 上传群临时文件 --------"),sMemberName.c_str());
	//			else
	//				sText.Format(_T("-------- %s 上传群共享文件 --------"),sMemberName.c_str());
	//			//this->AddLineString(0, sText, 1);	// 显示中间
	//			const bool bReceive = pNotifyInfo->m_nFromAccount==theApp.GetLogonUserId()?false:true;
	//			m_pMrFrameInterface->AddLine(0);
	//			//m_pMrFrameInterface->SetAlignmentFormat(1);
	//			m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	//			m_pMrFrameInterface->WriteString((LPCTSTR)sText,theDefaultChatSystemColor);
	//			//m_pMrFrameInterface->WriteSpace(1);
	//			//m_pMrFrameInterface->WriteTime(0,"%H:%M");

	//			m_pMrFrameInterface->AddLine(0);
	//			//m_pMrFrameInterface->SetAlignmentFormat(1);
	//			m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	//			m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	//			m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	//			m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theDefaultChatBackGroundColor1);
	//			WriteFileHICON(sFileName.c_str(),0);
	//			m_pMrFrameInterface->WriteString((LPCTSTR)sFileText,theDefaultChatSystemColor);
	//			m_pMrFrameInterface->WriteSpace(1);
	//			sText.Format(_T("#CTRL:%d:%d:%lld,%s#下载"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB),(int)EB_MR_CTRL_DATA_TYPE_DOWNLOAD_RESOURCE,nResourceId,sFileName.c_str());
	//			m_pMrFrameInterface->WriteString((LPCTSTR)sText,RGB(0, 0, 255));
	//			if (m_pCallInfo.m_sGroupCode>0 && !theApp.GetDisableGroupSharedCloud())
	//			{
	//				m_pMrFrameInterface->WriteSpace(2);
	//				sText.Format(_T("#CTRL:%d:%d:0#群共享"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB),(int)EB_MR_CTRL_DATA_TYPE_OPEN_SHARE);
	//				m_pMrFrameInterface->WriteString((LPCTSTR)sText,RGB(0, 0, 255));
	//			}
	//			const bool hIsScrollEnd = (!bReceive || m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
	//			m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
	//			if (hIsScrollEnd)
	//				m_pMrFrameInterface->ScrollToPos(-1);
	//		}
	//	}break;
	default:
		break;
	}

}

void CDlgChatInput::OnUserEnter(eb::bigint sFromAccount, const tstring& sFromInfo)
{
	if (theApp.IsLogonVisitor() ||
		m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_VISITOR)	// 游客不能被动邀请
	{
		if (!this->GetDlgItem(IDC_BUTTON_SEND)->IsWindowEnabled())
		{
			this->GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		}
	}
	//CString sWindowText;
	//m_pMrFrameInterface->AddLine();
	//sWindowText.Format(_T("%s(%s)进入会话！"),sFromAccount.c_str(), sFromInfo.c_str());
	//m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,theDefaultChatSystemColor);

	// ??
	//size_t nUserSize = m_pChatRoom->GetUserSize();
	//if (nUserSize > 1)
	//{
	//	m_richInput.EnableWindow(TRUE);
	//	this->GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	//}
//#ifdef USES_EBCOM_TEST
//	if (theEBClientCore->EB_CheckReCall(this->m_pCallInfo.GetCallId(),VARIANT_FALSE) == S_OK)
//#else
//	if (theEBAppClient.EB_CheckReCall(this->m_pCallInfo.GetCallId(),false) == 0)
//#endif
//	{
//		if (!m_richInput.IsWindowEnabled() &&
//			(theApp.IsLogonVisitor() ||
//			m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_VISITOR))	// 游客不能被动邀请
//		{
//			AddLineString(_T("对方重新进入本次会话！"));
//			m_richInput.EnableWindow(TRUE);
//			this->GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
//		}
//		//m_bMustInviteUser = false;
//		m_btnSend.SetWindowText(_T("发送"));
//		m_btnSend.Invalidate();
//	}
}
void CDlgChatInput::OnUserExit(eb::bigint sFromAccount)
{
	//CString sWindowText;
	//m_pMrFrameInterface->AddLine();
	//sWindowText.Format(_T("%s已经退出会话！"),sFromAccount.c_str());
	//m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,theDefaultChatSystemColor);

	//const bool bIsDepDialog = m_pCallInfo.m_sGroupCode>0;
	//if (!bIsDepDialog)	// ??群不会真正退出，所以不需要重新邀请
	//{
	//	//m_bMustInviteUser = true;
	//	m_btnSend.SetWindowText(_T("邀请"));
	//	m_btnSend.Invalidate();
	//}
	
	bool bIsDepCall = m_pCallInfo.m_sGroupCode>0;
	if (bIsDepCall) {
		CTreeItemInfo::pointer pTreeItemInfo;
		if (m_pSendToAccount.find(sFromAccount, pTreeItemInfo, true)) {
			m_comboSendTo.DeleteString(pTreeItemInfo->m_nIndex);
		}
	}
	else if (theApp.IsLogonVisitor() ||
		m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_VISITOR) {	// 游客不能被动邀请
		// 不是公司内部员工，需要重新呼叫才能通讯
		AddLineString(0,_T("对方已经退出本次会话！"),1);
		//m_richInput.EnableWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	}

}

#ifdef USES_EBCOM_TEST
void CDlgChatInput::OnSendRich(IEB_ChatRichInfo* pCrMsgInfo,EB_STATE_CODE nState,CString* sOutFirstMsg)
{
	ProcessMsg(false,pCrMsgInfo,sOutFirstMsg,nState);
}
#else
void CDlgChatInput::OnSendRich(const CCrRichInfo* pCrMsgInfo,EB_STATE_CODE nState,CString* sOutFirstMsg1,CString* sOutFirstMsg2)
{
	ProcessMsg(false,pCrMsgInfo,sOutFirstMsg1,sOutFirstMsg2,nState);
}

void CDlgChatInput::OnMsgReceipt(const CCrRichInfo* pCrMsgInfo, int nAckType)
{
	const eb::bigint nFromUserId = pCrMsgInfo->m_sSendFrom;
	const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();
	UpdateMsgReceiptData(nMsgId, nFromUserId, nAckType, pCrMsgInfo->GetStateCode());
}
#endif
void CDlgChatInput::UpdateMsgReceiptData(eb::bigint nMsgId, eb::bigint nFromUserId, int nAckType, EB_STATE_CODE nState)
{
	// nAckType=0 收到消息回执
	// nAckType=4 请求撤回消息
	// nAckType=6 请求“个人收藏”消息
	// nAckType=7 请求“群收藏”消息
	// *** read_flag=1 已经读；
	// *** read_flag=2 对方接收回执
	if (nState==EB_STATE_OK)
		theApp.UpdateMsgReceiptData(nMsgId, nFromUserId, nAckType);
	if (m_pMrFrameInterface!=NULL)
	{
		if (nAckType==6 ||	// 个人收藏
			nAckType==7)			// 群收藏
		{
			if (nState==EB_STATE_OK)
				CDlgMessageBox::EbMessageBox(this,"",_T("消息收藏成功！"),CDlgMessageBox::IMAGE_INFORMATION,3);
			else if (nState==EB_STATE_NOT_AUTH_ERROR)
				CDlgMessageBox::EbMessageBox(this,"",_T("没有操作权限：\r\n消息收藏失败！"),CDlgMessageBox::IMAGE_WARNING,3);
			else
				CDlgMessageBox::EbMessageBox(this,"",_T("消息不存在或者已经被删除：\r\n消息收藏失败！"),CDlgMessageBox::IMAGE_WARNING,3);
		}
		else if (nAckType==4)
		{
			if (nState==EB_STATE_OK)
			{
				// 1: 表示只更新第1行，第0行是标题，不更新；
				m_pMrFrameInterface->UpdateMsgText(nMsgId,1,"[撤回一条消息]",theDefaultChatSystemColor);
				m_pMrFrameInterface->SetMsgReceiptFlag(nMsgId, EBC_CONTRON_RECEIPT_FLAG_HIDE);
				//m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
				//m_pMrFrameControl.Invalidate();
			}
			if (nFromUserId==theApp.GetLogonUserId())
			{
				if (nState!=EB_STATE_OK)
					CDlgMessageBox::EbMessageBox(this,"",_T("发送时间超过2分钟的消息：\r\n不能撤回！"),CDlgMessageBox::IMAGE_WARNING,3);
				else
					AddLineString(0,_T("请求撤回了一条消息！"),1);
			}
			else if (m_pCallInfo.m_sGroupCode==0)
			{
				AddLineString(0,_T("对方撤回了一条消息！"),1);
			}
			else
			{
				tstring sMemberUserName;
				theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,nFromUserId,sMemberUserName);
				CString sText;
				sText.Format(_T("%s 撤回了一条消息！"),sMemberUserName.c_str());
				AddLineString(0,sText,1);
			}
		}
		else if (nAckType==0)	// ?
		{
			const long nReceiptFlag = EBC_CONTRON_RECEIPT_FLAG_TRUE|EBC_CONTRON_RECEIPT_FLAG_SHOW;
			m_pMrFrameInterface->SetMsgReceiptFlag(nMsgId, nReceiptFlag);
		}
	}
}

void CDlgChatInput::OnAlertingCall(void)
{
	AddLineString(0,_T("对方已经退出本次会话，正在重新邀请，请稍候...！"),1);
}

#ifdef USES_EBCOM_TEST
void CDlgChatInput::ProcessMsg(bool bReceive,IEB_ChatRichInfo* pCrMsgInfo,CString* sOutFirstMsg,EB_STATE_CODE nState)
{
	if (pCrMsgInfo == NULL) return;
	if (nState==EB_STATE_MAX_CAPACITY_ERROR)
	{
		AddLineString(0,_T("超过最大消息长度！"));
		return;
	}
	CIEB_ChatRichInfo pRichInfo;
	HRESULT hr = pRichInfo.put_RichMsg(&pCrMsgInfo->GetRichMsg());
	if (FAILED(hr))
		return;
	const EB_ChatRoomRichMsg* pRichMsg = pRichInfo.m_pRichMsg;
	const eb::bigint sSendFrom = pCrMsgInfo->SendFrom;
	const eb::bigint sSendTo = pCrMsgInfo->SendTo;
	const bool bIsDepDialog = m_pCallInfo.m_sGroupCode>0;
	CEBString sFromName;
	CEBString sToName;// = pCrMsgInfo->SendTo.GetBSTR();
	if (bReceive)
	{
		if (m_pCallInfo.m_sGroupCode==0)
		{
			sFromName = m_pFromAccountInfo.m_pFromCardInfo.m_sName;
			sToName = theEBClientCore->EB_UserName.GetBSTR();
		}else
		{
			sFromName = theEBClientCore->EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,sSendFrom).GetBSTR();
			if (pCrMsgInfo->SendTo>0)
				sToName = theEBClientCore->EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,pCrMsgInfo->SendTo).GetBSTR();
		}
	}else
	{
		sFromName = theEBClientCore->EB_UserName.GetBSTR();
		if (pCrMsgInfo->SendTo>0)
			sToName = theEBClientCore->EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,pCrMsgInfo->SendTo).GetBSTR();
	}

	CString sDBMsgTimeField;
	CString sDBMsgTimeValue;
	time_t tMsgTime = time(0);
	if (!pRichMsg->m_sMsgTime.empty())
	{
		libEbc::ChangeTime(pRichMsg->m_sMsgTime.c_str(),tMsgTime);
		sDBMsgTimeField = "msg_time,";
		sDBMsgTimeValue.Format(_T("Datetime('%s','utc'),"),pRichMsg->m_sMsgTime.c_str());
	}
	WriteMsgDate(tMsgTime);

	CString sPrivateText;
	CString sToText;
	if (m_pCallInfo.m_sGroupCode>0 && sSendTo>0)
	{
		if (bReceive)
		{
			//if (sSendTo>0)
			{
				if (pCrMsgInfo->Private)
					sPrivateText = _T("[私聊]");
				if (sSendTo != theApp.GetLogonUserId())
					sToText.Format(_T("对%s说"),sToName.c_str());
				else
					sToText.Format(_T("对你说"));
			}
		}else
		{
			//if (sSendTo>0)
			{
				sToText.Format(_T("对%s说"),sToName.c_str());
				if (pCrMsgInfo->Private)
					//if (m_checkPrivate.GetCheck())
					sPrivateText = _T("[私聊]");
			}
		}
	}

	//const COLORREF crTextColor = bReceive?RGB(0, 110, 254):RGB(66, 180, 117);
	CString sWindowText;
	//if (pRichMsg->m_bOffLineMsg && !pRichMsg->m_sMsgTime.empty())
	//{

	//	sWindowText.Format(_T("<font color=#%02X%02X%02X>%s%s%s</font><font color=#C0C0C0 style=\"FONT-SIZE:10pt\" > %s</font>"),
	//		GetRValue(crTextColor),GetGValue(crTextColor),GetBValue(crTextColor),sPrivateText,sSender.c_str(),sToText,pRichMsg->m_sMsgTime.c_str());	// RGB(66,191,184);
	//	AddMessage(nOutType,sWindowText,FALSE);
	//}else
	//{
	//	sWindowText.Format(_T("<font color=#%02X%02X%02X>%s%s%s</font>"),
	//		GetRValue(crTextColor),GetGValue(crTextColor),GetBValue(crTextColor),sPrivateText,sSender.c_str(),sToText);	// RGB(66,191,184);
	//	AddMessage(nOutType,sWindowText,TRUE);
	//}
	m_pMrFrameInterface->AddLine(pRichMsg->GetMsgId());
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	sWindowText.Format(_T("%s%s%s："),sPrivateText,sFromName.c_str(),sToText);
	const COLORREF crTextColor = bReceive?theDefaultChatTitleColor2:theDefaultChatTitleColor1;
	m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,crTextColor);
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M");
	const int nReadFlag = this->GetParent()->IsWindowVisible()?1:0;
	//if (bReceive && nReadFlag==0)
	//	m_pMrFrameInterface->WritePic(1,2,4,7,7,theDefaultBtnCloseColor,EB_MR_CTRL_TYPE_LCLICK_CB,EB_MR_CTRL_DATA_TYPE_READ_FLAG);

	if (sOutFirstMsg!=NULL)
		sOutFirstMsg->Format("%s<br/>",libEbc::ACP2UTF8(sWindowText).c_str());

	eb::bigint sSaveDbToAccount = sSendTo;
	if (m_pCallInfo.m_sGroupCode==0)
		sSaveDbToAccount = m_pFromAccountInfo.GetUserId();

	CString sSql;
	//int ret = 0;
	m_pMrFrameInterface->AddLine(pRichMsg->GetMsgId());
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theDefaultChatBackGroundColor1);

	int nOutMsgLength = 0;
	const std::vector<EB_ChatRoomMsgItem*>& pRichMsgList = pRichMsg->GetList();
	for (size_t i=0; i<pRichMsgList.size(); i++)
	{
		const EB_ChatRoomMsgItem* pMsgItem = pRichMsgList[i];
		if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_TEXT)
		{
			if (sOutFirstMsg!=NULL && nOutMsgLength<const_max_length)
			{
				const CString sTemp(pMsgItem->GetText().substr(0,100).c_str());
				nOutMsgLength += sTemp.GetLength();
				sOutFirstMsg->Append(sTemp);
			}
			m_pMrFrameInterface->WriteString(libEbc::UTF82ACP(pMsgItem->GetText().c_str()).c_str(),bReceive?theDefaultChatTextColor2:theDefaultChatTextColor1);

			if (theApp.GetSaveConversationLocal() && !theApp.IsLogonVisitor())
			{
				const int nReadFlag = this->GetParent()->IsWindowVisible()?1:0;
				CEBString sText(pMsgItem->GetText());
				CSqliteCdbc::escape_string_in(sText);
				while (!sText.empty())
				{
					const tstring sSaveText = sText.size()>theMaxDatabaseTextSize?sText.substr(0,theMaxDatabaseTextSize):sText;
					sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_text,read_flag) ")\
						_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s',%d)"),
						sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str()),
						sSaveDbToAccount,libEbc::ACP2UTF8(sToName.c_str()).c_str(),int(pCrMsgInfo->Private?1:0),(int)MRT_TEXT,sSaveText.c_str(),nReadFlag);
					theApp.m_pBoUsers->execute(sSql);
					sText = sText.substr(sSaveText.size());
				}
			}
		}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_RESOURCE)
		{
			const tstring sResourceInfo = libEbc::UTF82ACP(pMsgItem->GetText().c_str());
			std::vector<std::string> pList;
			if (libEbc::ParseString(sResourceInfo.c_str(),";",pList)>=3)
			{
				const tstring sResourceId = pList[0];
				const tstring sDescription = pList.size()>3?pList[3]:"";
				CString sImageFileName;
#ifdef USES_EBCOM_TEST
				sImageFileName.Format(_T("%s\\%s"), CEBString(theEBClientCore->EB_ResourcePath.GetBSTR()).c_str(),sResourceId.c_str());
#else
				sImageFileName.Format(_T("%s\\%s"), theEBAppClient.EB_GetResourcePath().c_str(),sResourceId.c_str());
#endif
				if (sOutFirstMsg!=NULL && nOutMsgLength<const_max_length)
				{
					CString sTemp;
					sTemp.Format(_T("<img src=\"%s\" alt=\"%s\"/>"),sImageFileName,sDescription.c_str());
					nOutMsgLength += 30;
					sOutFirstMsg->Append(libEbc::ACP2UTF8(sTemp).c_str());
				}
				CString sTemp;
				sTemp.Format(_T("#CTRL:%d::#%s"),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,sImageFileName);
				m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
				if (theApp.GetSaveConversationLocal() && !theApp.IsLogonVisitor())
				{
					const int nReadFlag = this->GetParent()->IsWindowVisible()?1:0;
					sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,read_flag) ")\
						_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s',%d)"),
						sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),
						sSaveDbToAccount,libEbc::ACP2UTF8(sToName.c_str()).c_str(),pCrMsgInfo->Private?1:0,MRT_JPG,libEbc::ACP2UTF8(sImageFileName).c_str(),nReadFlag);
					theApp.m_pBoUsers->execute(sSql);
				}
			}
		}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT)
		{
			CString sObjectFileName;
			EB_RICH_SUB_TYPE nSubType = EB_RICH_SUB_TYPE_JPG;
			MSG_RECORD_TYPE nRecordType = MRT_JPG;
			if (bReceive || nState==EB_STATE_FORWARD_MSG)
			{
				static unsigned int static_index = 0;
				static_index++;
				nSubType = (EB_RICH_SUB_TYPE)pRichMsg->GetSubType();
				if (nSubType == EB_RICH_SUB_TYPE_JPG)
				{
					sObjectFileName.Format(_T("%s\\%x%02x%02x.jpg"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,(static_index)%0xff);
				}else if (nSubType==EB_RICH_SUB_TYPE_AUDIO)	// wav格式
				{
					nRecordType = MRT_WAV;
					sObjectFileName.Format(_T("%s\\%x%02x%02x.wav"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,(static_index)%0xff);
				}else
				{
					// 普通文件；
					nRecordType = MRT_FILE;
					sObjectFileName.Format(_T("%s\\%x%02x%02x"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,(static_index)%0xff);
				}
				const char * lpObjectData = pMsgItem->GetData();
				DWORD dwDataSize = pMsgItem->GetSize();
				FILE * hFile = fopen(sObjectFileName,"wb");
				if (hFile!=NULL)
				{
					fwrite(lpObjectData,dwDataSize,1,hFile);
					fclose(hFile);
				}
/*
				sImageFileName.Format(_T("%s\\%x%02x%02x.jpg"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,static_index%0xff);
				const char * lpObjectData = pMsgItem->GetData();
				DWORD dwDataSize = pMsgItem->GetSize();
				FILE * hFile = fopen(sImageFileName,"wb");
				if (hFile!=NULL)
				{
					fwrite(lpObjectData,dwDataSize,1,hFile);
					fclose(hFile);
				}*/
			}else
			{
				sObjectFileName = pMsgItem->GetText().c_str();
			}
			if (nSubType == EB_RICH_SUB_TYPE_JPG)
			{
				CString sTemp;
				sTemp.Format(_T("#CTRL:%d::#%s"),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,sObjectFileName);
				m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
				if (sOutFirstMsg!=NULL && nOutMsgLength<const_max_length)
				{
					CString sTemp;
					sTemp.Format(_T("<img src=\"%s\" width=\"80\" height=\"80\" />"),libEbc::ACP2UTF8(sObjectFileName).c_str());
					nOutMsgLength += 30;
					sOutFirstMsg->Append(sTemp);
				}
			}else if (nSubType==EB_RICH_SUB_TYPE_AUDIO)	// wav格式
			{
				//WriteFileHICON(sObjectFileName);
				m_pMrFrameInterface->WriteWav("播放语音",(LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LCLICK_OPEN);
				if (sOutFirstMsg!=NULL && nOutMsgLength<const_max_length)
				{
					nOutMsgLength += 12;
					sOutFirstMsg->Append(libEbc::ACP2UTF8("语音消息").c_str());
				}
			}else
			{
				WriteFileHICON(sObjectFileName);
				const tstring sFileName = libEbc::GetFileName((LPCTSTR)sObjectFileName);
				eb::bigint nFileSize = -1;
				FILE * f = fopen(sObjectFileName, "rb");
				if (f != NULL)
				{
					_fseeki64(f, 0, SEEK_END);
					nFileSize = _ftelli64(f);
					fclose(f);
				}
				CString sFileText;
				if (nFileSize == -1)
					sFileText.Format(_T("%s (文件不存在)"),sFileName.c_str());
				else if (nFileSize >= const_gb_size)
					sFileText.Format(_T("%s (%.02fGB)"),sFileName.c_str(),(double)nFileSize/const_gb_size);
				else if (nFileSize >= const_mb_size)
					sFileText.Format(_T("%s (%.02fMB)"),sFileName.c_str(),(double)nFileSize/const_mb_size);
				else if (nFileSize >= const_kb_size)
					sFileText.Format(_T("%s (%.02fKB)"),sFileName.c_str(),(double)nFileSize/const_kb_size);
				else
					sFileText.Format(_T("%s (%lldByte)"),sFileName.c_str(),nFileSize);
				m_pMrFrameInterface->WriteSpace(1);
				m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, (LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,theDefaultFlatBlackTextColor);
				//m_pMrFrameInterface->WriteUrl(sFileName.c_str(), (LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,theDefaultFlatBlackTextColor);
				m_pMrFrameInterface->WriteLine();
				m_pMrFrameInterface->WriteSpace(11);
				if (nFileSize>=0)
				{
					m_pMrFrameInterface->WriteOpenFile(L"打开",(LPCTSTR)sObjectFileName);
					m_pMrFrameInterface->WriteSpace(2);
				}
				m_pMrFrameInterface->WriteOpenDir(L"打开文件夹",(LPCTSTR)sObjectFileName);
				if (sOutFirstMsg!=NULL && nOutMsgLength<const_max_length)
				{
					CString sTemp;
					sTemp.Format(_T("《%s》文件"),sFileName.c_str());
					nOutMsgLength += sTemp.GetLength();
					sOutFirstMsg->Append(libEbc::ACP2UTF8(sTemp).c_str());
				}
			}
			//m_pMrFrameInterface->WriteImage((LPCTSTR)sImageFileName,0,0);
			if (theApp.GetSaveConversationLocal() && !theApp.IsLogonVisitor())
			{
				const int nReadFlag = this->GetParent()->IsWindowVisible()?1:0;
				sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,read_flag) ")\
					_("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s',%d)"),
					sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),
					sSaveDbToAccount,libEbc::ACP2UTF8(sToName.c_str()).c_str(),pCrMsgInfo->Private?1:0,nRecordType,libEbc::ACP2UTF8(sObjectFileName).c_str(),nReadFlag);
				theApp.m_pBoUsers->execute(sSql);
			}
		}
	}
	const bool hIsScrollEnd = (!bReceive || m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
	m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
	if (hIsScrollEnd)
		m_pMrFrameInterface->ScrollToPos(-1);
	if (nState==EB_STATE_EXCESS_QUOTA_ERROR)
	{
		AddLineString(0,_T("发送不成功，对方离线消息空间已满！"));
	}

	if (sOutFirstMsg!=NULL)
	{
		const CString sTemp(*sOutFirstMsg);
		if (bIsDepDialog)
			sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-group://%lld\">%s</a>"),sTemp,m_pCallInfo.m_sGroupCode,libEbc::ACP2UTF8("查看消息").c_str());
		else
			sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-account://%lld\">%s</a>"),sTemp,m_pCallInfo.GetFromUserId(),libEbc::ACP2UTF8("查看消息").c_str());
	}
}
#else
void CDlgChatInput::ProcessMsg(bool bReceive,const CCrRichInfo* pCrMsgInfo,CString* sOutFirstMsg1,CString* sOutFirstMsg2,EB_STATE_CODE nState)
{
	if (pCrMsgInfo == NULL)
	{
		return;
	}
	if (nState==EB_STATE_MAX_CAPACITY_ERROR) {
		AddLineString(0,_T("超过最大消息长度！"),1);
		return;
	}
	else if (nState==EB_STATE_GROUP_FORBID_SPEECH) {
		AddLineString(0,_T("群禁言中！"),1);
		return;
	}
	else if (nState==EB_STATE_FORBID_SPEECH) {
		AddLineString(0,_T("你被禁言中！"),1);
		return;
	}
	else if (nState==EB_STATE_TIMEOUT_ERROR) {
		AddLineString(0,_T("当前会话掉线，发送消息超时，正在自动重连，请稍候再试！"),1);
		return;
	}

	const EB_ChatRoomRichMsg * pRichMsg = pCrMsgInfo->m_pRichMsg;
	const int nSubType = pRichMsg->GetSubType(); 
	//bool bHasCharFormat = false;
	//CRCHARFORMATA cf;
	//memset(&cf, 0, CRCHARFORMAT_SIZE);
	//if (pRichMsg->m_charFormat.get() != NULL && pRichMsg->m_charFormat->GetSize() == CRCHARFORMAT_SIZE)
	//{
	//	bHasCharFormat = true;
	//	memcpy(&cf, pRichMsg->m_charFormat->GetData(), pRichMsg->m_charFormat->GetSize());
	//}

	m_bLastReceiveMsg = bReceive;
	//if (m_pPanelStatus!=NULL && m_pPanelStatus->IsWindowVisible())
	//{
	//	CRect rectPanelStatus;
	//	m_pPanelStatus->GetClientRect(&rectPanelStatus);
	//	if (rectPanelStatus.left==0 && bReceive)
	//	{
	//		//CRect rect;
	//		//m_wndSplitter1.GetClientRect(&rect);
	//		m_pPanelStatus->MoveWindow(100,rectPanelStatus.top,rectPanelStatus.Width(),rectPanelStatus.Height());
	//		//m_pPanelStatus->MoveWindow(rect.right-rectPanelStatus.Width(),rectPanelStatus.top,rectPanelStatus.Width(),rectPanelStatus.Height());
	//		//m_pPanelStatus->ShowWindow(SW_HIDE);
	//	}
	//}
	const eb::bigint sSender = pCrMsgInfo->m_sSendFrom;
	const bool bIsDepDialog = m_pCallInfo.m_sGroupCode>0;
	CEBString sFromName;
	CEBString sToName;// = pCrMsgInfo->m_sSendTo;
	if (bReceive) {
		if (m_pCallInfo.m_sGroupCode ==0 ) {
			if (m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_VISITOR)
				sFromName = _T("游客") + m_pFromAccountInfo.GetAccount();
			else
				sFromName = m_pFromAccountInfo.m_pFromCardInfo.m_sName;
			sToName = theEBAppClient.EB_GetUserName();

			if (m_pPanelStatus != NULL && m_pPanelStatus->IsWindowVisible()) {
				KillTimer(TIMERID_HIDE_NOTIFY1);
				SetTimer(TIMERID_HIDE_NOTIFY1,1000,NULL);
			}
			//if (m_labelNotify1.IsWindowVisible())
			//{
			//	m_labelNotify1.ShowWindow(SW_HIDE);
			//	//SetTimer(TIMERID_HIDE_NOTIFY1,100,NULL);
			//}
		}
		else {
			if (!theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,sSender,sFromName)) {
				theEBAppClient.EB_GetMemberNameByUserId2(sSender, sFromName);
			}
			if (pCrMsgInfo->m_sSendTo>0) {
				if (!theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendTo,sToName)) {
					theEBAppClient.EB_GetMemberNameByUserId2(pCrMsgInfo->m_sSendTo, sToName);
				}
			}

			//if (m_pShowNotifyList.exist(sSender))
			//{
			//	m_pShowNotifyList.clear();
			//	m_labelNotify1.ShowWindow(SW_HIDE);
			//}
			//{
			//	BoostWriteLock wtlock(m_pNotifyList.mutex());
			//	CLockList<CEbNotifyInfo::pointer>::iterator pIter = m_pNotifyList.begin();
			//	for (; pIter!=m_pNotifyList.end(); pIter++)
			//	{
			//		CEbNotifyInfo::pointer pNotifyInfo = *pIter;
			//		if (pNotifyInfo->m_nFromUserId==sSender)
			//		{
			//			m_pNotifyList.erase(pIter);
			//			break;
			//		}
			//	}
			//}
		}
	}
	else {
		if (m_pCallInfo.m_sGroupCode ==0 ) {
			sFromName = theEBAppClient.EB_GetUserName();
		}
		else {
			if (!theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode, theApp.GetLogonUserId(), sFromName)) {
				theEBAppClient.EB_GetMemberNameByUserId2(theApp.GetLogonUserId(), sFromName);
			}
		}
		if (pCrMsgInfo->m_sSendTo>0) {
			if (!theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode, pCrMsgInfo->m_sSendTo, sToName)) {
				theEBAppClient.EB_GetMemberNameByUserId2(pCrMsgInfo->m_sSendTo, sToName);
			}
		}
	}
	CString sPrivateText;
	CString sToText;
	if (m_pCallInfo.m_sGroupCode>0 && pCrMsgInfo->m_sSendTo>0)
	{
		if (bReceive)
		{
			//if (pCrMsgInfo->m_sSendTo>0)
			{
				if (pCrMsgInfo->m_bPrivate)
					sPrivateText = _T("[私聊]");
				if (pCrMsgInfo->m_sSendTo != theEBAppClient.EB_GetLogonUserId())
					sToText.Format(_T("对%s说"),sToName.c_str());
				else
					sToText.Format(_T("对你说"));
			}
		}else
		{
			//if (m_pCallInfo.m_sGroupCode>0 && pCrMsgInfo->m_sSendTo>0)
			{
				sToText.Format(_T("对%s说"),sToName.c_str());
				if (pCrMsgInfo->m_bPrivate)
					//if (m_checkPrivate.GetCheck())
					sPrivateText = _T("[私聊]");
			}
		}
	}

	CString sDBMsgTimeField;
	CString sDBMsgTimeValue;
	time_t tMsgTime = time(0);
	if (!pRichMsg->m_sMsgTime.empty())
	{
		libEbc::ChangeTime(pRichMsg->m_sMsgTime.c_str(),tMsgTime);
		sDBMsgTimeField = "msg_time,";
		sDBMsgTimeValue.Format(_T("Datetime('%s','utc'),"),pRichMsg->m_sMsgTime.c_str());
	}
	WriteMsgDate(tMsgTime);

	//const COLORREF crTextColor = bReceive?RGB(0, 110, 254):RGB(66, 180, 117);
	CString sWindowText;
	m_pMrFrameInterface->AddLine(pRichMsg->GetMsgId());	// *** new line
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	sWindowText.Format(_T("%s%s%s"),sPrivateText,sFromName.c_str(),sToText);
	const COLORREF crTextColor = bReceive?theDefaultChatTitleColor2:theDefaultChatTitleColor1;
	if (bReceive && m_pCallInfo.m_sGroupCode>0)
	{
		CString sWindowTextTemp;
		sWindowTextTemp.Format(_T("%s#CTRL:%d:%d:%lld#%s%s："),sPrivateText,(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_UID,sSender,sFromName.c_str(),sToText);
		m_pMrFrameInterface->WriteString((LPCTSTR)sWindowTextTemp,crTextColor);
	//}else if (!bReceive)
	//{
	//	const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();
	//	CString sWindowTextTemp;
	//	sWindowTextTemp.Format(_T("#CTRL:%d:%d:%lld#%s"),(int)(EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_SEND_MSGID,nMsgId,sFromName.c_str());
	//	m_pMrFrameInterface->WriteString((LPCTSTR)sWindowTextTemp,crTextColor);
	}else
	{
		m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,crTextColor);
	}
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M");
	const int nReadFlag = 1;
	//const int nReadFlag = this->GetParent()->IsWindowVisible()?1:0;
	//if (bReceive && nReadFlag==0)
	//	m_pMrFrameInterface->WritePic(1,2,4,7,7,theDefaultBtnCloseColor,EB_MR_CTRL_TYPE_LCLICK_CB,EB_MR_CTRL_DATA_TYPE_READ_FLAG);

	if (sOutFirstMsg1!=NULL)
		sOutFirstMsg1->Format("%s<br/>",libEbc::ACP2UTF8(sWindowText).c_str());
	if (sOutFirstMsg2!=NULL)
		sOutFirstMsg2->Format("%s\n",sWindowText);

	eb::bigint sSaveDbToAccount = pCrMsgInfo->m_sSendTo;
	if (m_pCallInfo.m_sGroupCode==0)
		sSaveDbToAccount = m_pFromAccountInfo.GetUserId();
	CString sSql;
	//int ret = 0;
	m_pMrFrameInterface->AddLine(pRichMsg->GetMsgId());
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theDefaultChatBackGroundColor1);
	if (!bReceive && m_pCallInfo.m_sGroupCode==0)
		m_pMrFrameInterface->SetReceiptFlag(EBC_CONTRON_RECEIPT_FLAG_SHOW);

	tstring sInFromName(sFromName);
	theApp.m_pBoUsers->escape_string_in(sInFromName);
	tstring sInToName(sToName);
	theApp.m_pBoUsers->escape_string_in(sInToName);

	int nOutMsgLength = 0;
	const std::vector<EB_ChatRoomMsgItem*>& pRichMsgList = pRichMsg->GetList();
	for (size_t i=0; i<pRichMsgList.size(); i++)
	{
		const EB_ChatRoomMsgItem* pMsgItem = pRichMsgList[i];
		if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_TEXT)
		{
			if (sOutFirstMsg1!=NULL && nOutMsgLength<const_max_length)
			{
				const CString sTemp(pMsgItem->GetText().substr(0,const_max_length).c_str());
				nOutMsgLength += sTemp.GetLength();
				sOutFirstMsg1->Append(sTemp);
			}
			if (sOutFirstMsg2!=NULL && sOutFirstMsg2->GetLength()<const_max_length)
			{
				const CString sTemp(pMsgItem->GetText().substr(0,const_max_length).c_str());
				sOutFirstMsg2->Append(libEbc::UTF82ACP(sTemp).c_str());
			}
			m_pMrFrameInterface->WriteString(libEbc::UTF82ACP(pMsgItem->GetText().c_str()).c_str(),bReceive?theDefaultChatTextColor2:theDefaultChatTextColor1);

			if (theApp.GetSaveConversationLocal() && !theEBAppClient.EB_IsLogonVisitor())
			{
				tstring sText(pMsgItem->GetText());
				CSqliteCdbc::escape_string_in(sText);
				while (!sText.empty())
				{
					const tstring sSaveText = sText.size()>theMaxDatabaseTextSize?sText.substr(0,theMaxDatabaseTextSize):sText;
					sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_text,read_flag) ")\
						_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s',%d)"),
						sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendFrom,libEbc::ACP2UTF8(sInFromName.c_str()).c_str(),
						sSaveDbToAccount,libEbc::ACP2UTF8(sInToName.c_str()).c_str(),pCrMsgInfo->m_bPrivate?1:0,MRT_TEXT,sSaveText.c_str(),nReadFlag);
					theApp.m_pBoUsers->execute(sSql);
					sText = sText.substr(sSaveText.size());
				}
			}
		}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_RESOURCE)
		{
			const tstring sResourceInfo = libEbc::UTF82ACP(pMsgItem->GetText().c_str());
			std::vector<tstring> pList;
			if (libEbc::ParseString(sResourceInfo.c_str(),";",pList)>=3)
			{
				const tstring sResourceId(pList[0]);
				const tstring sDescription = pList.size()>3?pList[3]:"";
				CString sImageFileName;
				sImageFileName.Format(_T("%s\\%s"), theEBAppClient.EB_GetResourcePath().c_str(),sResourceId.c_str());
				if (sOutFirstMsg1!=NULL && nOutMsgLength<const_max_length)
				{
					CString sTemp;
					sTemp.Format(_T("<img src=\"%s\" alt=\"%s\"/>"),sImageFileName,sDescription.c_str());
					nOutMsgLength += 30;
					sOutFirstMsg1->Append(libEbc::ACP2UTF8(sTemp).c_str());
				}
				if (sOutFirstMsg2!=NULL && sOutFirstMsg2->GetLength()<const_max_length)
				{
					CString sTemp;
					sTemp.Format(_T("[%s]"),sDescription.c_str());
					sOutFirstMsg2->Append(sTemp);
				}

				const tstring sFilePath(libEbc::URLEncode(sImageFileName));
				CString sTemp;
				sTemp.Format(_T("#CTRL:%d:%d:%s#%s"),(int)(EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_IMAGE,sFilePath.c_str(),sImageFileName);
				m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
				if (theApp.GetSaveConversationLocal() && !theEBAppClient.EB_IsLogonVisitor())
				{
					sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,read_flag) ")\
						_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s',%d)"),
						sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendFrom,libEbc::ACP2UTF8(sInFromName.c_str()).c_str(),
						sSaveDbToAccount,libEbc::ACP2UTF8(sInToName.c_str()).c_str(),pCrMsgInfo->m_bPrivate?1:0,MRT_JPG,libEbc::ACP2UTF8(sImageFileName).c_str(),nReadFlag);
					theApp.m_pBoUsers->execute(sSql);
				}
			}
		}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT)
		{
			CString sObjectFileName;
			tstring sObjectSaveData;	// ** to save msg_text
			const EB_RICH_SUB_TYPE nSubType = (EB_RICH_SUB_TYPE)pCrMsgInfo->m_pRichMsg->GetSubType();
			bool bIsFile = true;
			MSG_RECORD_TYPE nRecordType = MRT_JPG;
			if (bReceive || nState==EB_STATE_FORWARD_MSG)
			{
				static unsigned int static_index = 0;
				static_index++;
				if (nSubType == EB_RICH_SUB_TYPE_JPG)
				{
					nRecordType = MRT_JPG;
					sObjectFileName.Format(_T("%s\\%x%02x%02x.jpg"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,(static_index)%0xff);
				}else if (nSubType==EB_RICH_SUB_TYPE_AUDIO)	// wav格式
				{
					nRecordType = MRT_WAV;
					sObjectFileName.Format(_T("%s\\%x%02x%02x.wav"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,(static_index)%0xff);
				}else if (nSubType==EB_RICH_SUB_TYPE_MAP_POS)	// 地图位置
				{
					bIsFile = false;
					nRecordType = MRT_MAP_POS;
					//sObjectFileName = pMsgItem->GetData();
				}else if (nSubType==EB_RICH_SUB_TYPE_CARD_INFO)	// 名片数据
				{
					bIsFile = false;
					nRecordType = MRT_CARD_INFO;
					//sObjectFileName = pMsgItem->GetData();
					sObjectSaveData = libEbc::UTF82ACP(pMsgItem->GetData());
				}else if (nSubType==EB_RICH_SUB_TYPE_USER_DATA)	// 用户自定义数据
				{
					nRecordType = MRT_USER_DATA;
					sObjectFileName.Format(_T("%s\\%x%02x%02x"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,(static_index)%0xff);
				}else
				{
					// 普通文件；
					nRecordType = MRT_FILE;
					sObjectFileName.Format(_T("%s\\%x%02x%02x"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,(static_index)%0xff);
				}
				if (bIsFile)
				//if (nRecordType != MRT_MAP_POS)
				{
					const char * lpObjectData = pMsgItem->GetData();
					DWORD dwDataSize = pMsgItem->GetSize();
					FILE * hFile = fopen(sObjectFileName,"wb");
					if (hFile!=NULL)
					{
						fwrite(lpObjectData,dwDataSize,1,hFile);
						fclose(hFile);
					}
				}
			}else
			{
				sObjectFileName = pMsgItem->GetText().c_str();
				if (nSubType == EB_RICH_SUB_TYPE_AUDIO)
					nRecordType = MRT_WAV;
				else if (nSubType == EB_RICH_SUB_TYPE_MAP_POS)		// ??
					nRecordType = MRT_MAP_POS;
				else if (nSubType == EB_RICH_SUB_TYPE_CARD_INFO)		// 名片信息
				{
					nRecordType = MRT_CARD_INFO;
					sObjectSaveData = libEbc::UTF82ACP(pMsgItem->GetData());
				}else if (nSubType == EB_RICH_SUB_TYPE_USER_DATA)	// ??
					nRecordType = MRT_USER_DATA;
			}
			if (nSubType == EB_RICH_SUB_TYPE_JPG)
			{
				const tstring sFilePath(libEbc::URLEncode(sObjectFileName));
				CString sTemp;
				sTemp.Format(_T("#CTRL:%d:%d:%s#%s"),(int)(EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_IMAGE,sFilePath.c_str(),sObjectFileName);
				m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
				if (sOutFirstMsg1!=NULL && nOutMsgLength<const_max_length)
				{
					CString sTemp;
					sTemp.Format(_T("<img src=\"%s\" width=\"80\" height=\"80\" />"),libEbc::ACP2UTF8(sObjectFileName).c_str());
					nOutMsgLength += 30;
					sOutFirstMsg1->Append(sTemp);
				}
				if (sOutFirstMsg2!=NULL && sOutFirstMsg2->GetLength()<const_max_length)
				{
					sOutFirstMsg2->Append(_T("[图片]"));
				}

			}else if (nSubType==EB_RICH_SUB_TYPE_AUDIO)	// wav格式
			{
				//WriteFileHICON(sObjectFileName);
				const int nWavTimeLength = libEbc::GetWaveTimeLength(sObjectFileName);
				CString sText;
				if (nWavTimeLength>=0)
					sText.Format(_T("语音消息 %d\""),nWavTimeLength);
				else if (nWavTimeLength==-1)
					sText =_T("语音消息不存在");
				else
					sText = _T("语音消息格式错误");
				m_pMrFrameInterface->WriteWav((LPCTSTR)sText,(LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LCLICK_OPEN);
				//m_pMrFrameInterface->WriteWav("语音消息",(LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LCLICK_OPEN);
				if (sOutFirstMsg1!=NULL && nOutMsgLength<const_max_length)
				{
					nOutMsgLength += 12;
					sOutFirstMsg1->Append(libEbc::ACP2UTF8(sText).c_str());
				}
				if (sOutFirstMsg2!=NULL && sOutFirstMsg2->GetLength()<const_max_length)
				{
					sOutFirstMsg2->Append(_T("[语音]"));
				}
			}else if (nSubType==EB_RICH_SUB_TYPE_MAP_POS)	// 地图位置
			{
				// ???
				if (sOutFirstMsg2!=NULL && sOutFirstMsg2->GetLength()<const_max_length)
				{
					sOutFirstMsg2->Append(_T("[地图位置]"));
				}
			}else if (nSubType==EB_RICH_SUB_TYPE_CARD_INFO)	// 名片信息
			{
				// *** 这里要解析并显示名片信息
				int nCardType = 0;
				tstring sCardData;
				theEBAppClient.EB_ParseCardInfo(sObjectSaveData,nCardType,sCardData);
				EB_ECardInfo pUserECard;
				if (nCardType==1 && theEBAppClient.EB_GetUserECardByFromInfo(sCardData,&pUserECard))
				{
					// ** 用户名片（个人名片）
					m_pCardInfoList.insert(pRichMsg->GetMsgId(),sObjectSaveData,false);
					const tstring sFilePath = theApp.GetUserHeadFilePath(pUserECard.m_nMemberUserId,"");
					CString sTemp;
					sTemp.Format(_T("#CTRL:%d:%d:#%s"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_USER_ECARD,sFilePath.c_str());
					m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,48,48);
					m_pMrFrameInterface->WriteSpace(1);
					sTemp.Format(_T("#CTRL:%d:%d:#%s"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_USER_ECARD,pUserECard.m_sAccountName.c_str());
					m_pMrFrameInterface->WriteString((LPCTSTR)sTemp,bReceive?theDefaultChatTextColor2:theDefaultChatTextColor1);
					m_pMrFrameInterface->WriteSpace(15);
					m_pMrFrameInterface->WriteLine();
					m_pMrFrameInterface->WritePic(2,0,-5,0,1,theDefaultFlatLine2Color,0,0);	// **画水平线条
					m_pMrFrameInterface->WriteString(L"个人名片",theDefaultChatSystemColor);

					if (sOutFirstMsg2!=NULL && sOutFirstMsg2->GetLength()<const_max_length)
					{
						sTemp.Format(_T("%s[个人名片]"),pUserECard.m_sAccountName.c_str());
						sOutFirstMsg2->Append(sTemp);
					}
				}else if (sOutFirstMsg2!=NULL && sOutFirstMsg2->GetLength()<const_max_length)
				{
					sOutFirstMsg2->Append(_T("[名片信息]"));
				}
			}else if (nSubType==EB_RICH_SUB_TYPE_USER_DATA)	// 用户自定义数据
			{
				// ???
				if (sOutFirstMsg2!=NULL && sOutFirstMsg2->GetLength()<const_max_length)
				{
					sOutFirstMsg2->Append(_T("[用户自定义数据]"));
				}
			}else
			{
				WriteFileHICON(sObjectFileName);
				const tstring sFileName = libEbc::GetFileName((LPCTSTR)sObjectFileName);
				eb::bigint nFileSize = -1;
				FILE * f = fopen(sObjectFileName, "rb");
				if (f != NULL)
				{
					_fseeki64(f, 0, SEEK_END);
					nFileSize = _ftelli64(f);
					fclose(f);
				}
				CString sFileText;
				if (nFileSize == -1)
					sFileText.Format(_T("%s (文件不存在)"),sFileName.c_str());
				else if (nFileSize >= const_gb_size)
					sFileText.Format(_T("%s (%.02fGB)"),sFileName.c_str(),(double)nFileSize/const_gb_size);
				else if (nFileSize >= const_mb_size)
					sFileText.Format(_T("%s (%.02fMB)"),sFileName.c_str(),(double)nFileSize/const_mb_size);
				else if (nFileSize >= const_kb_size)
					sFileText.Format(_T("%s (%.02fKB)"),sFileName.c_str(),(double)nFileSize/const_kb_size);
				else
					sFileText.Format(_T("%s (%lldByte)"),sFileName.c_str(),nFileSize);
				m_pMrFrameInterface->WriteSpace(1);
				CString sTemp;
				sTemp.Format(_T("#CTRL:0:%d:#%s"),(int)EB_MR_CTRL_DATA_TYPE_FILE,sFileText);
				m_pMrFrameInterface->WriteUrl((LPCTSTR)sTemp, (LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB,theDefaultFlatBlackTextColor);
				//m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, (LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,theDefaultFlatBlackTextColor);
				//m_pMrFrameInterface->WriteLine();
				//m_pMrFrameInterface->WriteSpace(11);
				//if (nFileSize>=0)
				//{
				//	m_pMrFrameInterface->WriteOpenFile(L"打开",(LPCTSTR)sObjectFileName);
				//	m_pMrFrameInterface->WriteSpace(2);
				//}
				//m_pMrFrameInterface->WriteOpenDir(L"打开文件夹",(LPCTSTR)sObjectFileName);
				if (sOutFirstMsg1!=NULL && nOutMsgLength<const_max_length)
				{
					CString sTemp;
					sTemp.Format(_T("《%s》文件"),sFileName.c_str());
					nOutMsgLength += sTemp.GetLength();
					sOutFirstMsg1->Append(libEbc::ACP2UTF8(sTemp).c_str());
				}
			}
			if (theApp.GetSaveConversationLocal() && !theEBAppClient.EB_IsLogonVisitor())
			{
				CSqliteCdbc::escape_string_in(sObjectSaveData);
				sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag) ")\
					_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s','%s',%d)"),
					sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendFrom,libEbc::ACP2UTF8(sInFromName.c_str()).c_str(),
					sSaveDbToAccount,libEbc::ACP2UTF8(sInToName.c_str()).c_str(),pCrMsgInfo->m_bPrivate?1:0,nRecordType,libEbc::ACP2UTF8(sObjectFileName).c_str(),libEbc::ACP2UTF8(sObjectSaveData.c_str()).c_str(),nReadFlag);
				theApp.m_pBoUsers->execute(sSql);
			}
		}
	}
	const bool hIsScrollEnd = (!bReceive || m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
	m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
	if (hIsScrollEnd)
		m_pMrFrameInterface->ScrollToPos(-1);
	if (nState==EB_STATE_EXCESS_QUOTA_ERROR)
	{
		AddLineString(0,_T("发送不成功，对方离线消息空间已满！"),1);
	}else if (EB_STATE_NOT_MEMBER_ERROR==nState && m_pCallInfo.m_sGroupCode>0)
	{
		AddLineString(0,_T("没有其他群组成员，不能发送聊天信息！"),1);
	}else if (nState!=EB_STATE_OK && nState!=EB_STATE_FORWARD_MSG)
	{
		CString sTemp;
		sTemp.Format(_T("发送失败，请重试！（CODE=%d）"),(int)nState);
		AddLineString(0,sTemp,1);
	}

	if (sOutFirstMsg1!=NULL)
	{
		const CString sTemp(*sOutFirstMsg1);
		if (bIsDepDialog)
			sOutFirstMsg1->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-group://%lld\">%s</a>"),sTemp,m_pCallInfo.m_sGroupCode,libEbc::ACP2UTF8("查看消息").c_str());
		else
			sOutFirstMsg1->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-account://%lld\">%s</a>"),sTemp,m_pCallInfo.GetFromUserId(),libEbc::ACP2UTF8("查看消息").c_str());
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatInput::OnReceiveRich(IEB_ChatRichInfo* pCrMsgInfo,CString* sOutFirstMsg)
{
	ProcessMsg(true,pCrMsgInfo,sOutFirstMsg);
}
#else
void CDlgChatInput::OnReceiveRich(const CCrRichInfo* pCrMsgInfo,CString* sOutFirstMsg1, CString* sOutFirstMsg2)
{
	if (theApp.IsLogonVisitor() ||
		m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_VISITOR)	// 游客不能被动邀请
	{
		if (!this->GetDlgItem(IDC_BUTTON_SEND)->IsWindowEnabled())
		{
			this->GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		}
	}
	//BOOST_ASSERT (pRichMsg.get() != 0);
	ProcessMsg(true,pCrMsgInfo,sOutFirstMsg1,sOutFirstMsg2);
}
#endif
//
//void CDlgChatInput::ReceiveMsg(CChatMsgInfo::pointer pChatMsgInfo)
//{
//	//BOOST_ASSERT (pChatMsgInfo.get() != 0);
//	//// m_richSelf
//
//	//switch (pChatMsgInfo->GetMsgType())
//	//{
//	//case EB_MSG_FILE:
//	//	{
//	//		if (pChatMsgInfo->m_sFileName.empty())
//	//		{
//	//			const tstring & sFileName = pChatMsgInfo->GetContent();
//	//			tstring sFileSimpleName;
//	//			tstring sFileExt;
//	//			GetFileExt(sFileName, sFileSimpleName, sFileExt);
//	//			size_t nFileSize = pChatMsgInfo->GetSize();
//	//			CString sSaveTo;
//	//			sSaveTo.Format(_T("%s\\%s"), theApp.GetUserFilePath(), sFileName.c_str());
//	//			int index = 0;
//	//			while (true)
//	//			{
//	//				if (!PathFileExists(sSaveTo))
//	//					break;
//	//				if (sFileExt.empty())
//	//					sSaveTo.Format(_T("%s\\%s(%d)"), theApp.GetUserFilePath(), sFileName.c_str(), ++index);
//	//				else
//	//					sSaveTo.Format(_T("%s\\%s(%d).%s"), theApp.GetUserFilePath(), sFileSimpleName.c_str(), ++index, sFileExt.c_str());
//	//			}
//	//			thePopClient.SendReceiveFile(m_sChatId, pChatMsgInfo->GetMsgId(), (LPCTSTR)sSaveTo);
//	//		}else
//	//		{
//	//			// 文件接收完成
//	//			tstring sFileName = GetFileName(pChatMsgInfo->m_sFileName);
//	//			CString sWindowText;
//	//			sWindowText.Format(_T("成功接收文件：<a href=\"%s\" />%s</a>"), pChatMsgInfo->m_sFileName.c_str(), sFileName.c_str());
//	//			m_pwebOutWndSelf->WriteBlock(sWindowText, FALSE);
//	//		}
//	//	}break;
//	//case EB_MSG_RICH:
//	//	{
//	//		cgcAttachment::pointer pAttachment;
//	//		if (!pChatMsgInfo->m_pRecvAttachList.find(0, pAttachment))
//	//		{
//	//			break;
//	//		}
//	//		const unsigned long ntotal = pAttachment->getTotal();
//	//		char * lpBuffer = new char[ntotal+1];
//	//		memset(lpBuffer, 0, ntotal+1);
//	//		boost::mutex::scoped_lock lockRecvAttachList(pChatMsgInfo->m_pRecvAttachList.mutex());
//	//		CLockMap<unsigned long, cgcAttachment::pointer>::iterator pIter;
//	//		for (pIter=pChatMsgInfo->m_pRecvAttachList.begin(); pIter!=pChatMsgInfo->m_pRecvAttachList.end(); pIter++)
//	//		{
//	//			pAttachment = pIter->second;
//	//			char * buffer = lpBuffer+(int)pAttachment->getIndex();
//	//			memcpy(buffer, pAttachment->getAttachData(), pAttachment->getAttachSize());
//	//		}
//	//		lockRecvAttachList.unlock();
//
//	//		const int ncharformatsize = sizeof(CHARFORMAT);
//	//		if (ntotal-7 < ncharformatsize)
//	//		{
//	//			delete []lpBuffer;	// error
//	//			break;
//	//		}
//	//		CHARFORMAT cf;
//	//		memcpy(&cf, lpBuffer, ncharformatsize);
//
//	//		int nrichlen = atoi(lpBuffer+ncharformatsize);
//	//		if (nrichlen+7 > (int)ntotal-ncharformatsize)
//	//		{
//	//			delete []lpBuffer;	// error
//	//			break;
//	//		}
//
//	//		CEBAccountInfo::pointer pFromAccount = CGC_OBJECT_CAST<CEBAccountInfo>(pChatMsgInfo->GetFromAccount());
//	//		const COLORREF crTextColor = RGB(0, 110, 254);
//	//		CString sWindowText;
//	//		sWindowText.Format(_T("<font color=#%02X%02X%02X>%s</font>"), GetRValue(crTextColor), GetGValue(crTextColor), GetBValue(crTextColor), pFromAccount->GetAccount().c_str());	// RGB(66,191,184);
//	//		m_pwebOutWndSelf->WriteBlock(sWindowText, FALSE, TRUE);
//
//	//		tstring sRichMsg(lpBuffer+ncharformatsize+7, nrichlen);
//	//		EB_ChatRoomRichMsg::pointer pRichMsg = EB_ChatRoomRichMsg::create();
//	//		pRichMsg->SetFormatMsg(sRichMsg);
//	//		const std::vector<EB_ChatRoomMsgItem::pointer> & pRichMsgList = pRichMsg->GetList();
//	//		nrichlen += (ncharformatsize+7);
//	//		for (size_t i=0; i<pRichMsgList.size(); i++)
//	//		{
//	//			EB_ChatRoomMsgItem::pointer pMsgItem = pRichMsgList[i];
//	//			if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_TEXT)
//	//			{
//	//				sWindowText.Format(_T("&nbsp;%s"), pMsgItem->GetText().c_str());
//	//				sWindowText = CharFormat2Html(cf, sWindowText);
//	//				m_pwebOutWndSelf->WriteBlock(sWindowText, FALSE);
//	//			}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT)
//	//			{
//	//				CString sImageFileName;
//	//				sImageFileName.Format(_T("%s\\%s%d.jpg"), theApp.GetUserImagePath(), pChatMsgInfo->GetMsgId().c_str(), (int)time(0));
//	//				char * lpObjectData = lpBuffer+nrichlen;
//	//				DWORD dwDataSize = pMsgItem->GetSize();
//	//				nrichlen += dwDataSize;
//	//				HANDLE hFile = ::CreateFile(sImageFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	//				if(hFile != INVALID_HANDLE_VALUE)
//	//				{
//	//					//Write Image Data
//	//					DWORD dwWritten = 0;
//	//					::WriteFile(hFile, lpObjectData, dwDataSize, &dwWritten, NULL);
//	//					::CloseHandle(hFile);
//	//				}
//	//				sWindowText.Format(_T("<img src=\"%s\" border=\"0\" />"), sImageFileName);
//	//				m_pwebOutWndSelf->WriteBlock(sWindowText, FALSE);
//	//			}
//	//		}
//	//		delete []lpBuffer;
//	//	}break;
//	//default:
//	//	break;
//	//}
//}

void CDlgChatInput::OnMsgfilterRich(NMHDR* pNMHDR, LRESULT* pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	if(pMsgFilter->msg==WM_CHAR)
	{
		//CRect rt;
		//CPoint p = m_richInput.GetCaretPos();
		//rt.SetRect(p.x - 5, p.y - 5, p.x + 10, p.y + 10);
		//InvalidateRect(&rt);
		//static bool shouldcheck = false;
		//static int curlen = 0; 
		//static long beginchr = 0;
		//if (pMsgFilter->wParam == '/')
		//{	
		//	curlen = 0;
		//	shouldcheck = true;
		//	beginchr = m_SendMsg.GetTextLength();
		//}
		//if (shouldcheck)
		//{
		//	if (curlen > 5)
		//	{
		//		shouldcheck = false;
		//		curlen = 0;
		//		beginchr = 0;
		//	}
		//	else
		//	{
		//		curlen ++;
		//	}

		//	char pos[20];
		//	char lens[20];
		//	
		//	ltoa(beginchr, pos, 10);
		//	itoa(curlen, lens, 10);
		//	char rel[255];
		//	sprintf(rel, "Begin Pos : %s,   Lens : %s \n", pos, lens);
		//	TRACE0(rel);

		//	LPSTR txt = "";
		//	int size = faces.GetSize();
		//	for (int index = 0; index < size; index ++)
		//	{
		//		m_SendMsg.SetSel(beginchr, beginchr + curlen);
		//		CHARRANGE cr;
		//		CHARRANGE rcr;
		//		m_SendMsg.GetSel(cr);
		//		TRACE0(m_SendMsg.GetSelText() + "\n");
		//		m_SendMsg.SetSel(m_SendMsg.GetTextLength(), m_SendMsg.GetTextLength() + 1);
		//		FINDTEXTEX ft;
		//		ft.chrg = cr;
		//		txt = (LPTSTR)(LPCTSTR)faces[index].FaceName;
		//		ft.lpstrText = txt;
		//		ft.chrgText = rcr;
		//		
		//		// ?? by hd
		//		//m_SendMsg.FindText(BOM_PLAINTEXT | BOM_DEFPARADIR, &ft);
		//		//if ( ft.chrgText.cpMax - ft.chrgText.cpMin > 0 )
		//		//{
		//		//	m_SendMsg.InsertFace(faces[index].FacePath);
		//		//	m_SendMsg.SetSel(ft.chrgText.cpMin, ft.chrgText.cpMax);
		//		//	m_SendMsg.ReplaceSel("");
		//		//	beginchr = 0;
		//		//	curlen = 0;
		//		//	shouldcheck = false;
		//		//}

		//		
		//	}
		//}
		CRect rt;
		::GetWindowRect(pMsgFilter->nmhdr.hwndFrom , &rt);
		ScreenToClient( &rt);
		InvalidateRect(&rt);
	}

	if(pMsgFilter->msg==WM_VSCROLL)
	{
		CRect rt;
		::GetWindowRect(pMsgFilter->nmhdr.hwndFrom , &rt);
		ScreenToClient( &rt);
		InvalidateRect(&rt);
	}

	if(pMsgFilter->msg==WM_RBUTTONUP)//for right mouse click
	{
		//if (pMsgFilter->nmhdr.hwndFrom == m_SendMsg.m_hWnd ) 
		//{
		//	AfxMessageBox("Click in the send one!");
		//}
		//else
		//{
		//	AfxMessageBox("Click!");
		//}
	}
	*pResult = 0;
}

void CDlgChatInput::OnRichEditProtected(NMHDR* pNMHDR, LRESULT* pResult)
{
	ENPROTECTED* pEP = (ENPROTECTED*)pNMHDR;
	switch (pEP->msg)
	{
	case WM_KEYDOWN: //按键，判断pEP->wParam
		{
		}break;
	case WM_PASTE: //粘贴
		{
			int i=0;
		}break;
	case WM_CUT: //剪切
	case EM_SETCHARFORMAT:
	default:
		break;
	};
	*pResult = FALSE;
}

void CDlgChatInput::OnRichEditExLink( 
			NMHDR*   in_pNotifyHeader, 
			LRESULT* out_pResult )
{
	
	ENLINK* l_pENLink = ( ENLINK* )in_pNotifyHeader ;
	
	*out_pResult = 0 ;

	switch( l_pENLink->msg )
	{
		default:
		{
		}
		break ;

		case WM_LBUTTONDOWN:
		{
			CString l_URL ;
			CHARRANGE l_CharRange ;
			COleRichEditCtrl *m_TempEdit;
			m_TempEdit = (COleRichEditCtrl*)COleRichEditCtrl::FromHandle(l_pENLink->nmhdr.hwndFrom);
			m_TempEdit->GetSel( l_CharRange ) ;
			m_TempEdit->SetSel( l_pENLink->chrg ) ;
			l_URL = m_TempEdit->GetSelText() ;
			m_TempEdit->SetSel( l_CharRange ) ;

			//CWaitCursor l_WaitCursor ;

			ShellExecute( this->GetSafeHwnd(), _T( "open" ), l_URL, NULL, NULL, SW_SHOWNORMAL ) ;

			*out_pResult = 1 ;
		}
		break ;

		case WM_LBUTTONUP:
		{
			*out_pResult = 1 ;
		}
		break ;
		
	}
}

bool save_2_bmp(const char * lpObjectData, unsigned long dwDataSize, const CString & sFileName)
{
	FILE * hFile = fopen(sFileName,"wb");
	if(hFile != NULL)
	//HANDLE hFile = ::CreateFile(sFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWritten = 0;
		//Writing Bitmap File header
		BITMAPFILEHEADER bmfh;
		bmfh.bfType = 0x4d42;    //'BM'
		int nColorTableEntries = 0;
		int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColorTableEntries;
		bmfh.bfSize = 0;
		bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColorTableEntries;
		fwrite((LPVOID)&bmfh,sizeof(BITMAPFILEHEADER),1,hFile);
		//::WriteFile(hFile, (LPVOID)&bmfh, sizeof(BITMAPFILEHEADER),	&dwWritten, NULL);

		BITMAPINFOHEADER* pInfoHead = (BITMAPINFOHEADER*)lpObjectData;
		pInfoHead->biXPelsPerMeter = pInfoHead->biYPelsPerMeter = 0;

		//Special Careful Here!!! Discard the Color Mask,
		//We need TRUE COLOR image
		//I bellieve you guys running Win2X/Xp use true color
		//screen setting
		if(pInfoHead->biCompression == BI_BITFIELDS)
		{
			pInfoHead->biCompression = BI_RGB;
			dwDataSize -= 3 * sizeof(RGBQUAD);  //delete the 3 DWORD
			//color mask
			LPBYTE pSrc, pDst;
			pSrc = (LPBYTE)lpObjectData;
			pSrc += sizeof(BITMAPINFOHEADER);
			pDst = pSrc;
			pSrc += 3 * sizeof(RGBQUAD);
			::MoveMemory(pDst, pSrc, dwDataSize - sizeof(BITMAPINFOHEADER));
		}

		//Write Image Data
		fwrite(lpObjectData,dwDataSize,1,hFile);
		fclose(hFile);
		//::WriteFile(hFile, lpObjectData, dwDataSize, &dwWritten, NULL);
		//::CloseHandle(hFile);
		return true;
	}
	return false;
}

char * get_file_data(const char * lpFile, unsigned long & pOutFileSize)
{
	FILE * f = fopen(lpFile, "rb");
	if (f)
	{
#ifdef WIN32
		_fseeki64(f, 0, SEEK_END);
		pOutFileSize = _ftelli64(f);
		_fseeki64(f, 0, SEEK_SET);
#else
		fseeko(f, 0, SEEK_END);
		pOutFileSize = ftello(f);
		fseeko(f, 0, SEEK_SET);
#endif
		char * pOutBuffer = new char [pOutFileSize];
		unsigned long nreadindex = 0;
		while (nreadindex < pOutFileSize)
		{
			size_t readed = fread(pOutBuffer+nreadindex, 1, min((pOutFileSize-nreadindex),10*1024), f);
			if (readed <= 0)
				break;
			nreadindex += readed;
		}
		fclose(f);
		return pOutBuffer;
	}
	return 0;
}

void CDlgChatInput::GetInputTextRange(int nGetTextFirst, int nGetTextLast, CString& refString) const
{
	const int const_max_size = 1024;
	const int ngettextcount = (nGetTextLast-nGetTextFirst+const_max_size-1)/const_max_size;
	for (int i=0; i<ngettextcount; i++)
	{
		int first = nGetTextFirst+i*const_max_size;
		int last = first+min(nGetTextLast-first,const_max_size);
		CString sTempText;
		//m_richInput.GetTextRange(first, last, sTempText);
		const_cast<COleRichEditCtrl&>(m_richInput).SetSel(first, last);
		sTempText = m_richInput.GetSelText();
		refString += sTempText;
	}
}
void CDlgChatInput::FormatMsg(EB_ChatRoomRichMsg* pOutMsgFormat)
{
	//long ncount = m_richInput.GetLimitText();
	//if (pOutMsgFormat->m_charFormat.get() == NULL)
	//{
	//	CHARFORMAT cf;
	//	m_richInput.GetDefaultCharFormat( cf );
	//	CRCHARFORMATA crcf;
	//	memset(&crcf, 0, CRCHARFORMAT_SIZE);
	//	CHARFORMAT2CR(cf,crcf);
	//	char * lpCharFormat = new char[CRCHARFORMAT_SIZE+1];
	//	memset(lpCharFormat, 0, sizeof(lpCharFormat));
	//	memcpy(lpCharFormat, &crcf, CRCHARFORMAT_SIZE);
	//	pOutMsgFormat->m_charFormat = EB_ChatRoomMsgItem::create(lpCharFormat, CRCHARFORMAT_SIZE, EB_ChatRoomMsgItem::MIT_CHARFORMAT);
	//}

	//pOutMsgFormat->SetSubType(11);	// audio
	long nStartChar = 0;
	WORD nPrevSelectionType = SEL_EMPTY;
	int nGetTextFirst = -1;
	int nGetTextLast = -1;
	long nGetObjectIndex = 0;
	while (nStartChar == 0 || nPrevSelectionType != SEL_EMPTY)
	{
		m_richInput.SetSel(nStartChar, nStartChar+1);
		WORD nSelectionType = m_richInput.GetSelectionType();
		switch (nSelectionType)
		{
		case SEL_EMPTY:
			{
				if (nPrevSelectionType == SEL_TEXT)
				{
					CString sText;
					GetInputTextRange(nGetTextFirst, nGetTextLast, sText);
					nGetTextFirst = -1;
					pOutMsgFormat->AddText(libEbc::ACP2UTF8(sText).c_str());
				}
			}break;
		case SEL_TEXT:
			{
				if (nGetTextFirst == -1)
					nGetTextFirst = nStartChar;
				nGetTextLast = nStartChar+1;
			}break;
		case SEL_OBJECT:
			{
				if (nPrevSelectionType == SEL_TEXT)
				{
					CString sText;
					GetInputTextRange(nGetTextFirst, nGetTextLast, sText);
					nGetTextFirst = -1;
					pOutMsgFormat->AddText(libEbc::ACP2UTF8(sText).c_str());
				}

				char * lpObjectData = 0;
				DWORD dwDataSize = 0;
				DWORD dwUserData = 0;
				if (m_richInput.GetObjectData(nGetObjectIndex, &lpObjectData, dwDataSize,dwUserData))
				{
					// 这里需要区分是否表情资源
					if (dwUserData > 0)
					{
						ASSERT (theApp.m_pDlgEmotionSelect!=NULL);
						eb::bigint sResourceId = 0;
						if (theApp.m_pDlgEmotionSelect->GetImageResourceId(dwUserData,sResourceId))
						{
#ifdef USES_EBCOM_TEST
							const CEBString sResourceCmInfo = theEBClientCore->EB_GetResourceCmInfo(sResourceId).GetBSTR();
#else
							tstring sResourceCmInfo;
							theEBAppClient.EB_GetResourceCmInfo(sResourceId,sResourceCmInfo);
#endif
							pOutMsgFormat->AddResource(libEbc::ACP2UTF8(sResourceCmInfo.c_str()).c_str());
						}
					}else
					{
						USES_CONVERSION;
						// ***以下代码是测试发送ＷＡＶ语音片段的
						//const CString sFileName = _T("f:\\wav\\鼓独奏音效3835.wav");
						//FILE * f = fopen(sFileName,"rb");
						//if (f!=NULL)
						//{
						//	_fseeki64(f, 0, SEEK_END);
						//	long nFileSize = _ftelli64(f);
						//	_fseeki64(f, 0, SEEK_SET);
						//	char * lpFileData  = new char[nFileSize+1];
						//	fread(lpFileData,1,nFileSize,f);
						//	fclose(f);
						//	EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)pOutMsgFormat->AddObject((char*)lpFileData, nFileSize);
						//	if (pMsgItem != NULL)
						//	{
						//		pMsgItem->SetText(sFileName);	// 用于本端显示
						//	}
						//}

						static unsigned int static_index = 0;
						CString sFileName;
						sFileName.Format(_T("%s\\%x%02x%02x.bmp"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,(++static_index)%0xff);
						save_2_bmp(lpObjectData, dwDataSize, sFileName);
						delete []lpObjectData;
						CString sJgpFileName;
						sJgpFileName.Format(_T("%s\\%x%02x%02x.jpg"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,static_index%0xff);
						const int theOneMB = 1024*1024;
						if (dwDataSize>=4*theOneMB)
							libEbc::bmp_2_jpg(A2W_ACP(sFileName), A2W_ACP(sJgpFileName), 75);
						else if (dwDataSize>=3*theOneMB)
							libEbc::bmp_2_jpg(A2W_ACP(sFileName), A2W_ACP(sJgpFileName), 80);
						else if (dwDataSize>=2*theOneMB)
							libEbc::bmp_2_jpg(A2W_ACP(sFileName), A2W_ACP(sJgpFileName), 85);
						else
							libEbc::bmp_2_jpg(A2W_ACP(sFileName), A2W_ACP(sJgpFileName), 90);
						DeleteFile(sFileName);
						unsigned long nFileSize = 0;
						char * lpFileData = get_file_data(sJgpFileName, nFileSize);
						EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)pOutMsgFormat->AddObject((char*)lpFileData, nFileSize);
						if (pMsgItem != NULL)
						{
							pMsgItem->SetText(sJgpFileName);	// 用于本端显示
						}
					}
				}
				nGetObjectIndex++;
			}break;
		default:
			{
				int i=0;
			}break;
		}
		nPrevSelectionType = nSelectionType;
		nStartChar++;
	}
}

void CDlgChatInput::save()
{
	////IRichEditOle* pReo = m_richInput.GetIRichEditOle();
	//If you use Platform SDK directly
	IRichEditOle* pReo;
	::SendMessage(m_richInput.GetSafeHwnd(), EM_GETOLEINTERFACE, 0, (LPARAM)(LPVOID*)&pReo);
	//Note: in both cases, inside pReo's AddRef got called, so
	//remember to release it later

	LONG nNumber = pReo->GetObjectCount();  //Your Images' Number
	//Handle Error yourself, code simplified for space limitation
	for(int i = 0; i < nNumber; i++)
	{
		REOBJECT reobject;
		memset( &reobject, 0 , sizeof(REOBJECT) );
		reobject.cbStruct = sizeof(REOBJECT);
		HRESULT hr = pReo->GetObject(i, &reobject, REO_GETOBJ_ALL_INTERFACES);
		if(FAILED(hr)) continue;

		// 是否当前选中对象
		if((reobject.dwFlags & REO_SELECTED)  == REO_SELECTED)
		{
			// 找到当前的图片对象
			//reobject.dwUser // 插入图片时指定的数值
		}
		// 向对象申请一个IDataObject指针，该指针包含对象的信息并将被放到剪切板上去
		//IDataObject * pDataObject;
		//hr = reobject.poleobj->GetClipboardData(0, &pDataObject);

		//caller should released the inner object
		IDataObject* lpDataObject;
		hr = (reobject.poleobj)->QueryInterface(IID_IDataObject, (void **)&lpDataObject);
		if(FAILED(hr)) continue;

		STGMEDIUM stgm;  //out
		FORMATETC fm;    //in

		fm.cfFormat = CF_DIB;  // Clipboard format
		fm.ptd = NULL;         // Target Device = Screen
		fm.dwAspect = DVASPECT_CONTENT;
		// Level of detail = Full content
		fm.lindex = -1;        // Index = Not applicaple
		fm.tymed = TYMED_HGLOBAL ;
		hr = lpDataObject->GetData(&fm, &stgm);
		if(FAILED(hr)) continue;

		ASSERT(::GlobalSize(stgm.hGlobal));

		HANDLE hFile = ::CreateFile(_T("c:\\img.bmp"),
			GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		DWORD dwWritten;
		//Writing Bitmap File header
		BITMAPFILEHEADER bmfh;
		bmfh.bfType = 0x4d42;    //'BM'
		int nColorTableEntries = 0;
		int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) *
			nColorTableEntries;
		bmfh.bfSize = 0;
		bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * nColorTableEntries;
		::WriteFile(hFile, (LPVOID)&bmfh, sizeof(BITMAPFILEHEADER),
			&dwWritten, NULL);

		DWORD dwGlobalSize = ::GlobalSize(stgm.hGlobal);
		LPVOID lpMem = ::VirtualAlloc(NULL, dwGlobalSize, MEM_COMMIT,
			PAGE_READWRITE);
		::CopyMemory(lpMem, (LPVOID)::GlobalLock(stgm.hGlobal),
			dwGlobalSize);
		BITMAPINFOHEADER* pInfoHead = (BITMAPINFOHEADER*)lpMem;
		pInfoHead->biXPelsPerMeter = pInfoHead->biYPelsPerMeter
			= 0;

		//Special Careful Here!!! Discard the Color Mask,
		//We need TRUE COLOR image
		//I bellieve you guys running Win2X/Xp use true color
		//screen setting
		if(pInfoHead->biCompression == BI_BITFIELDS)
		{
			pInfoHead->biCompression = BI_RGB;
			dwGlobalSize -= 3 * sizeof(RGBQUAD);  //delete the 3 DWORD
			//color mask
			LPBYTE pSrc, pDst;
			pSrc = (LPBYTE)lpMem;
			pSrc += sizeof(BITMAPINFOHEADER);
			pDst = pSrc;
			pSrc += 3 * sizeof(RGBQUAD);
			::MoveMemory(pDst, pSrc, dwGlobalSize -
				sizeof(BITMAPINFOHEADER));
		}

		//Write Image Data
		::WriteFile(hFile, lpMem, dwGlobalSize, &dwWritten, NULL);
		::GlobalUnlock(stgm.hGlobal);

		//You may find good if all image are same size, keeping the
		//memory for performance
		::VirtualFree(lpMem, 0, MEM_RELEASE);
		::CloseHandle(hFile);

		lpDataObject->Release();
	}
	pReo->Release();
}

void CDlgChatInput::OnBnClickedButtonFont()
{
	CFontDialog dlg( theApp.m_cfDefault, CF_EFFECTS | CF_SCREENFONTS, NULL, this);
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCharFormat( theApp.m_cfDefault );
		if( theApp.m_cfDefault.yHeight > 320 )
			theApp.m_cfDefault.yHeight = 320;
		theApp.m_cfDefault.bCharSet = GB2312_CHARSET ;
		m_richInput.SetDefaultCharFormat( theApp.m_cfDefault );
		m_richInput.Invalidate();
	}
}

void CDlgChatInput::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}
void CDlgChatInput::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgChatInput::OnBnClickedButtonClose()
{
	this->GetParent()->PostMessage(EB_MSG_EXIT_CHAT, 0, 0);
	//this->GetParent()->PostMessage(WM_CLOSE, 0, 0);
	
}

void CDlgChatInput::OnBnClickedButtonFileManager()
{
	theApp.GetMainWnd()->PostMessage(EB_COMMAND_FILE_MANAGER, 0, 0);
	//this->GetParent()->PostMessage(EB_COMMAND_FILE_MANAGER, 0, 0);
}
void CDlgChatInput::OnBnClickedButtonMsgRecord()
{
	this->GetParent()->PostMessage(EB_MSG_VIEW_MSG_RECORD, 0, 0);
}

BOOL CDlgChatInput::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		const bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
		const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
		////const bool bAlt = (GetKeyState(VK_MENU)&0x80)==0x80?true:false;
		if (bShift && bControl)
		{
			if (pMsg->wParam=='S')
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL, 1, 1);
				return TRUE;
			}
			//if (pMsg->wParam=='M')
			//{
			//	OnBnClickedButtonMsgRecord();
			//	return TRUE;
			////}else if (pMsg->wParam=='A')
			////{
			////	OnBnClickedButtonEbsc();
			////	return TRUE;
			//}
		}

		if (bControl)
		{
			if (pMsg->wParam==VK_UP)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
				return TRUE;
			}else if (pMsg->wParam==VK_DOWN)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
				return TRUE;
			}else if (pMsg->wParam>=VK_NUMPAD0 && pMsg->wParam<=VK_NUMPAD9)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_OFFSET, (WPARAM)(pMsg->wParam-VK_NUMPAD0), 0);
				return TRUE;
			}
		}
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_richInput.GetSafeHwnd())
	{
		//CString sInputText;
		//m_richInput.GetWindowText(sInputText);
		//if (!sInputText.IsEmpty() && sInputText.Left(1)=="/")
		//{
		//	// ***
		//}

		if (theApp.GetSendType()==0)
		{
			// 回车发送类型
			if ((GetKeyState(VK_CONTROL)&0x80)!=0x80)	// 没有Ctrl发送
			{
				send();
				return TRUE;
			}
			// Ctrl 控件会自动换行；
		}else
		{
			// 回车（没有Ctrl）发送
			//m_richInput.GetSel(
			if ((GetKeyState(VK_CONTROL)&0x80)==0x80)
				send();
			else
				m_richInput.ReplaceSel("\r\n");
			return TRUE;
		}
	}
	return CEbDialogBase::PreTranslateMessage(pMsg);
}

void CDlgChatInput::WriteMsgDate(time_t tMsgTime)
{
	const CTime pMsgTime(tMsgTime);
	if (pMsgTime.GetYear() != m_tLastMsgDayTime.GetYear() ||
		pMsgTime.GetMonth() != m_tLastMsgDayTime.GetMonth() ||
		pMsgTime.GetDay() != m_tLastMsgDayTime.GetDay())
	{
		m_tLastMsgDayTime = pMsgTime;
		const mycp::tstring sDayOfWeek = theDayOfWeek[m_tLastMsgDayTime.GetDayOfWeek()-1];
		CString sText;
		sText.Format(_T("--------  %04d-%02d-%02d %s  --------"),m_tLastMsgDayTime.GetYear(),m_tLastMsgDayTime.GetMonth(),m_tLastMsgDayTime.GetDay(),sDayOfWeek.c_str());
		m_pMrFrameInterface->AddLine(0);
		m_pMrFrameInterface->SetAlignmentFormat(1);
		m_pMrFrameInterface->WriteString((LPCTSTR)sText,theDefaultChatSystemColor);
		//const bool hIsScrollEnd = (m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
		//m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
		//if (hIsScrollEnd)
		//	m_pMrFrameInterface->ScrollToPos(-1);
	}

}
void CDlgChatInput::WriteTitle(eb::bigint nMsgId,bool bPrivate,eb::bigint nFromUid,const tstring& sFromName,eb::bigint nToUid,const tstring& sToName,time_t tMsgTime, int nReadFlag)
{
	if (m_nLastMsgId==nMsgId && m_nFromUid==nFromUid && m_nToUid==nToUid)
		return;
	m_nLastMsgId = nMsgId;
	m_nFromUid = nFromUid;
	m_nToUid = nToUid;
	//if (m_tLastMsgTime > 0)
	//{
	//	m_pMrFrameInterface->WriteTime(m_tLastMsgTime);
	//}
	//m_tLastMsgTime = tMsgTime;//+nTimezone*60;

	WriteMsgDate(tMsgTime);

	m_pMrFrameInterface->AddLine(nMsgId);	// *** new line

	if (nFromUid==theApp.GetLogonUserId())
	{
		m_pMrFrameInterface->SetAlignmentFormat(2);
	}
	CString sPrivateText;
	CString sToText;
	if (m_pCallInfo.m_sGroupCode>0)
	{
		if (bPrivate)
			sPrivateText = _T("[私聊]");
		if (nToUid == theApp.GetLogonUserId())
			sToText.Format(_T("对你说"));
		else if (nToUid>0)
			sToText.Format(_T("对%s说"),sToName.c_str());
	}
	//const CString sFilePath = theApp.GetAppPath() + _T("/img/defaultmember.png");
	//m_pMrFrameInterface->WriteImage((LPCTSTR)sFilePath,30,30,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN);
	const bool bReceive = nFromUid==theApp.GetLogonUserId()?false:true;
	CString sWindowText;
	if (bReceive && m_pCallInfo.m_sGroupCode>0)
	{
		if (nToUid>0)
		{
			sWindowText.Format(_T("%s#CTRL:%d:%d:%lld#%s#CTRL:%d:%d:%lld#%s"),sPrivateText,(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_UID,nFromUid,sFromName.c_str(),
				(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_UID,nToUid,sToText);
		}else if (nFromUid>0)
			sWindowText.Format(_T("%s#CTRL:%d:%d:%lld#%s%s"),sPrivateText,(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_UID,nFromUid,sFromName.c_str(),sToText);
		else
			sWindowText.Format(_T("%s%s%s"),sPrivateText,sFromName.c_str(),sToText);
	//}else if (!bReceive)
	//{
	//	if ((nReadFlag&EBC_READ_FLAG_WITHDRAW)==0)
	//		sWindowText.Format(_T("#CTRL:%d:%d:%lld#%s"),(int)(EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_SEND_MSGID,nMsgId,sFromName.c_str());
	//	else
	//		sWindowText.Format(_T("#CTRL:%d:%d:%lld#%s"),(int)(EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_DELETE_MSGID,nMsgId,sFromName.c_str());
	}else
	{
		sWindowText.Format(_T("%s%s%s"),sPrivateText,sFromName.c_str(),sToText);
	}
	const COLORREF crTextColor = bReceive?theDefaultChatTitleColor2:theDefaultChatTitleColor1;
	m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,crTextColor);
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M");
	//if (!bReadFlag)
	//	m_pMrFrameInterface->WritePic(1,2,4,7,7,theDefaultBtnCloseColor,EB_MR_CTRL_TYPE_LCLICK_CB,EB_MR_CTRL_DATA_TYPE_READ_FLAG);

	m_pMrFrameInterface->AddLine(nMsgId);	// *** new line
	//m_pMrFrameInterface->SetAlignmentValue(40,60);
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	if (nFromUid==theApp.GetLogonUserId())
	{
		m_pMrFrameInterface->SetAlignmentFormat(2);
		m_pMrFrameInterface->SetFrameBackGroundColor(theDefaultChatBackGroundColor1);
		//m_pMrFrameInterface->SetFrameBackGroundColor(theApp.GetBgColor3());

		if (m_pCallInfo.m_sGroupCode==0)
		{
			const long nReceiptFlag = (nReadFlag&EBC_READ_FLAG_RECEIPT)==EBC_READ_FLAG_RECEIPT?(EBC_CONTRON_RECEIPT_FLAG_SHOW|EBC_CONTRON_RECEIPT_FLAG_TRUE):EBC_CONTRON_RECEIPT_FLAG_SHOW;
			m_pMrFrameInterface->SetReceiptFlag(nReceiptFlag);
		}
	}else
	{
		m_pMrFrameInterface->SetFrameBackGroundColor(theDefaultChatBackGroundColor2);
	}
}

void CDlgChatInput::CheckMyForbidSpeechState(bool bNewMessage,bool bFromMemberInfo)
{
	if (m_pCallInfo.m_sGroupCode==0) return;
	// *** 群组聊天，检查当前用户是否禁言状态
	int nForbidMinutes = 0;
	if (theEBAppClient.EB_IsMemberForbidSpeech(m_pCallInfo.m_sGroupCode,theApp.GetLogonUserId(),nForbidMinutes))
	{
		// ** 检查到我的帐号，当前群组部门禁言
		CString sStringText;
		if (nForbidMinutes==0)
			sStringText = _T("你被永久禁言：");
		else
		{
			if (nForbidMinutes<60)
				sStringText.Format(_T("你被禁言 %d 分钟："),nForbidMinutes);
			else if (nForbidMinutes<1440)	// 1440分钟=24小时=1天
			{
				// a 剩余分钟数
				const int nRemainMinutes = nForbidMinutes%60;
				if (nRemainMinutes==0)
					sStringText.Format(_T("你被禁言 %d 小时："),(nForbidMinutes/60));
				else
					sStringText.Format(_T("你被禁言 %d 小时 %d 分钟："),(nForbidMinutes/60),nRemainMinutes);
			}else
			{
				const int nDays = nForbidMinutes/1440;
				const int nHours = (nForbidMinutes%1440)/60;
				const int nMinutes = nForbidMinutes%60;
				sStringText.Format(_T("你被禁言 %d 天"),nDays);
				CString sTemp;
				if (nHours>0)
				{
					sTemp.Format(_T(" %d 小时"),nHours);
					sStringText += sTemp;
				}
				if (nMinutes>0)
				{
					sTemp.Format(_T(" %d 分钟"),nMinutes);
					sStringText += sTemp;
				}
				sStringText += _T("：");
			}
		}
		if (bNewMessage && !m_pPanelInputForbidStatus->IsWindowVisible())
		//if (bNewMessage)
			AddLineString(0,sStringText,1,true);
		USES_CONVERSION;
		m_pPanelInputForbidStatus->SetDrawText(T2W(sStringText));
		m_pPanelInputForbidStatus->ShowWindow(SW_SHOW);
		m_richInput.EnableWindow(FALSE);
		m_btnImage.EnableWindow(FALSE);
		m_btnEBSC.EnableWindow(FALSE);
		m_btnSend.EnableWindow(FALSE);
		SetTimer(TIMERID_CHECK_FORBID,60*1000,NULL);
	}
	else if (theEBAppClient.EB_IsGroupForbidSpeech(m_pCallInfo.m_sGroupCode))
	{
		if (theEBAppClient.EB_IsGroupAdminLevel(m_pCallInfo.m_sGroupCode) || theEBAppClient.EB_IsGroupManager(m_pCallInfo.m_sGroupCode))
		{
			m_pPanelInputForbidStatus->ShowWindow(SW_HIDE);
			m_richInput.EnableWindow(TRUE);
			m_btnImage.EnableWindow(TRUE);
			m_btnEBSC.EnableWindow(TRUE);
			m_btnSend.EnableWindow(TRUE);
		}
		else
		{
			const CString sStringText(_T("群禁言中，群管理员可以发言："));
			if (bNewMessage && !m_pPanelInputForbidStatus->IsWindowVisible())
			{
				AddLineString(0,sStringText,1,true);
			}
			USES_CONVERSION;
			m_pPanelInputForbidStatus->SetDrawText(T2W(sStringText));
			m_pPanelInputForbidStatus->ShowWindow(SW_SHOW);
			m_richInput.EnableWindow(FALSE);
			m_btnImage.EnableWindow(FALSE);
			m_btnEBSC.EnableWindow(FALSE);
			m_btnSend.EnableWindow(FALSE);
			SetTimer(TIMERID_CHECK_FORBID,60*1000,NULL);
		}
		return;
	}
	else if (m_pPanelInputForbidStatus->IsWindowVisible())
	{
		if (bFromMemberInfo)
			AddLineString(0,_T("你已被解除禁言："),1,true);
		else
			AddLineString(0,_T("已经解除群禁言："),1,true);
		m_pPanelInputForbidStatus->ShowWindow(SW_HIDE);
		m_richInput.EnableWindow(TRUE);
		m_btnImage.EnableWindow(TRUE);
		m_btnEBSC.EnableWindow(TRUE);
		m_btnSend.EnableWindow(TRUE);
	}
}

void CDlgChatInput::LoadMsgRecord(void)
{
	if (m_pMrFrameInterface==NULL) return;

	const eb::bigint m_sAccount = m_pCallInfo.m_sGroupCode>0?0:m_pFromAccountInfo.GetUserId();
	const eb::bigint m_sGroupCode = m_pCallInfo.m_sGroupCode;
	CString sSql;
	if (m_sAccount>0)
	{
		sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t ")\
			_T("WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) ORDER BY msg_time DESC LIMIT 20"),m_sAccount,m_sAccount);
	}else if (m_sGroupCode>0)
	{
		sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t ")\
			_T("WHERE dep_code=%lld ORDER BY msg_time DESC LIMIT 20"),m_sGroupCode);
	}else
	{
		return;
	}

	m_pMrFrameInterface->Clear();
	m_nLastMsgId = 0;
	m_nFromUid = 0;
	m_nToUid = 0;
	USES_CONVERSION;
	//CString sMsgTime;
	//time_t nLocalMsgTime = 0;
	tstring sToWriteString;
	bool bReceive = false;
	int nCookie = 0;
	mycp::bigint nLastWithdrawMsgId = 0;
	const mycp::bigint nRet = theApp.m_pBoUsers->select(sSql, nCookie);
	cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->last(nCookie);
	while (pRecord.get()!=NULL)
	{
		const tstring sMsgTime(pRecord->getVector()[0]->getStrValue());
		const eb::bigint sMsgId = pRecord->getVector()[1]->getBigIntValue();
		const tstring soffTime(pRecord->getVector()[2]->getStrValue());
		const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
		const tstring sFromName(libEbc::UTF82ACP(pRecord->getVector()[4]->getStrValue().c_str()));
		const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
		const tstring sToName(libEbc::UTF82ACP(pRecord->getVector()[6]->getStrValue().c_str()));
		const int nPrivate = pRecord->getVector()[7]->getIntValue();
		const int nMsgType = pRecord->getVector()[8]->getIntValue();
		tstring sMsgName(libEbc::UTF82ACP(pRecord->getVector()[9]->getStrValue().c_str()));
		tstring sMsgText(libEbc::UTF82ACP(pRecord->getVector()[10]->getStrValue().c_str()));
		//theApp.m_pBoUsers->escape_string_out(sMsgName);
		//theApp.m_pBoUsers->escape_string_out(sMsgText);
		const unsigned int nMsgSize = sMsgText.size();
		const int nReadFlag = pRecord->getVector()[11]->getIntValue();
		pRecord = theApp.m_pBoUsers->previous(nCookie);

		// 前面未显示，并且当前文本不是最长文本，显示前面内容。
		if (!sToWriteString.empty() && nMsgSize!=theMaxDatabaseTextSize)
		{
			m_pMrFrameInterface->WriteString(sToWriteString.c_str(),bReceive?theDefaultChatTextColor2:theDefaultChatTextColor1);
			sToWriteString.clear();
		}
		bReceive = sFromAccount!=theApp.GetLogonUserId();
		//// 兼容旧版本
		//if (sFromName.empty())
		//	sFromName = sFromAccount;
		//if (sToName.empty())
		//	sToName = sToAccount;
		time_t nMsgTime = 0;
		libEbc::ChangeTime(sMsgTime.c_str(),nMsgTime);
		WriteTitle(sMsgId,nPrivate==1,sFromAccount,sFromName,sToAccount,sToName,nMsgTime,nReadFlag);
		if ((nReadFlag&EBC_READ_FLAG_WITHDRAW)==EBC_READ_FLAG_WITHDRAW)
		{
			if (nLastWithdrawMsgId!=sMsgId)
			{
				nLastWithdrawMsgId = sMsgId;
				m_pMrFrameInterface->WriteString("[撤回一条消息]",theDefaultChatSystemColor);
				m_pMrFrameInterface->SetReceiptFlag(EBC_CONTRON_RECEIPT_FLAG_HIDE);
			}
			continue;
		}
		nLastWithdrawMsgId = 0;
		CString sWindowText;
		switch (nMsgType)
		{
		case MRT_UNKNOWN:
			{
			}break;
		case MRT_TEXT:
			{
				CSqliteCdbc::escape_string_out(sMsgText);
				// 先临时保存，后面显示；处理分段保存长文本；
				if (sToWriteString.empty())
					sToWriteString = sMsgText;
				else
					sToWriteString = sMsgText+sToWriteString;
				//m_pMrFrameInterface->WriteString(sMsgText.c_str());
			}break;
		case MRT_JPG:
			{
				const tstring sFilePath(libEbc::URLEncode(sMsgName.c_str()));
				CString sTemp;
				sTemp.Format(_T("#CTRL:%d:%d:%s#%s"),(int)(EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_IMAGE,sFilePath.c_str(),sMsgName.c_str());
				m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
			}break;
		case MRT_MAP_POS:	// 地图位置
			{
			}break;
		case MRT_CARD_INFO:	// 名片信息
			{
				// *** 这里要解析并显示名片信息
				int nCardType = 0;
				tstring sCardData;
				theEBAppClient.EB_ParseCardInfo(sMsgText,nCardType,sCardData);
				EB_ECardInfo pUserECard;
				if (nCardType==1 && theEBAppClient.EB_GetUserECardByFromInfo(sCardData,&pUserECard))
				{
					// ** 用户名片（个人名片）
					m_pCardInfoList.insert(sMsgId,sMsgText,false);
					const tstring sFilePath = theApp.GetUserHeadFilePath(pUserECard.m_nMemberUserId,"");
					CString sTemp;
					sTemp.Format(_T("#CTRL:%d:%d:#%s"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_USER_ECARD,sFilePath.c_str());
					m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,48,48);
					m_pMrFrameInterface->WriteSpace(1);
					sTemp.Format(_T("#CTRL:%d:%d:#%s"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_USER_ECARD,pUserECard.m_sAccountName.c_str());
					m_pMrFrameInterface->WriteString((LPCTSTR)sTemp,bReceive?theDefaultChatTextColor2:theDefaultChatTextColor1);
					m_pMrFrameInterface->WriteSpace(15);
					m_pMrFrameInterface->WriteLine();
					m_pMrFrameInterface->WritePic(2,0,-5,0,1,theDefaultFlatLine2Color,0,0);	// **画水平线条
					m_pMrFrameInterface->WriteString(L"个人名片",theDefaultChatSystemColor);
				}
			}break;
		case MRT_USER_DATA:	// 用户自定义数据
			{
			}break;
		case MRT_WAV:
			{
				//WriteFileHICON(sMsgName.c_str());
				const int nWavTimeLength = libEbc::GetWaveTimeLength(sMsgName.c_str());
				CString sText;
				if (nWavTimeLength>=0)
					sText.Format(_T("语音消息 %d\""),nWavTimeLength);
				else if (nWavTimeLength==-1)
					sText =_T("语音消息不存在");
				else
					sText = _T("语音消息格式错误");
				m_pMrFrameInterface->WriteWav((LPCTSTR)sText,sMsgName.c_str(),EB_MR_CTRL_TYPE_LCLICK_OPEN);
				//m_pMrFrameInterface->WriteWav("语音消息",sMsgName.c_str(),EB_MR_CTRL_TYPE_LCLICK_OPEN);
			}break;
		case MRT_RESOURCE:
			{
				//
				//CString sMsgText;
				//sMsgText.Format(_T("%lld,%lld"),pCrFileInfo->m_sResId,nFileSize)
				const std::string::size_type nFind = sMsgText.find(",");
				if (nFind == std::string::npos) break;

				const tstring& sFileName = sMsgName;
				const mycp::bigint nResourceId = cgc_atoi64(sMsgText.substr(0,nFind).c_str());
				const mycp::bigint nFileSize = cgc_atoi64(sMsgText.substr(nFind+1).c_str());
				// *
				CString sFileText;
				if (nFileSize >= const_gb_size)
					sFileText.Format(_T("上传群共享文件：%s(%.02fGB)"),sFileName.c_str(),(double)nFileSize/const_gb_size);
				else if (nFileSize >= const_mb_size)
					sFileText.Format(_T("上传群共享文件：%s(%.02fMB)"),sFileName.c_str(),(double)nFileSize/const_mb_size);
				else if (nFileSize >= const_kb_size)
					sFileText.Format(_T("上传群共享文件：%s(%.02fKB)"),sFileName.c_str(),(double)nFileSize/const_kb_size);
				else if (nFileSize>0)
					sFileText.Format(_T("上传群共享文件：%s(%lldByte)"),sFileName.c_str(),nFileSize);
				else
					sFileText.Format(_T("上传群共享文件：%s"), sFileName.c_str());

				CString sText;
				WriteFileHICON(sFileName.c_str(),0);
				//sText.Format(_T("#CTRL:0:%d:#%s"),(int)EB_MR_CTRL_DATA_TYPE_FILE,sFileText);
				//m_pMrFrameInterface->WriteUrl((LPCTSTR)sText, sFileName.c_str(),EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB,theDefaultChatSystemColor);
				m_pMrFrameInterface->WriteString((LPCTSTR)sFileText,theDefaultChatSystemColor);
				m_pMrFrameInterface->WriteSpace(1);
				sText.Format(_T("#CTRL:%d:%d:%lld,%s#下载"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB),(int)EB_MR_CTRL_DATA_TYPE_DOWNLOAD_RESOURCE,nResourceId,sFileName.c_str());
				m_pMrFrameInterface->WriteString((LPCTSTR)sText,RGB(0, 0, 255));
				if (m_pCallInfo.m_sGroupCode>0 && !theApp.GetDisableGroupSharedCloud())
				{
					m_pMrFrameInterface->WriteSpace(2);
					sText.Format(_T("#CTRL:%d:%d:0#群共享"),(int)(EB_MR_CTRL_TYPE_LCLICK_CB),(int)EB_MR_CTRL_DATA_TYPE_OPEN_SHARE);
					m_pMrFrameInterface->WriteString((LPCTSTR)sText,RGB(0, 0, 255));
				}
			}break;
		case MRT_FILE:
			{
				////m_pMrFrame.AddLine();
				//m_pMrFrame.SetAlignment(bReceive?MR_ALIGNMENT_LEFT:MR_ALIGNMENT_RIGHT);
				//if (bReceive)
				//	m_pMrFrame.WriteArc(6,8,16,Gdiplus::Color::Gray,Gdiplus::Color(183,253,159));
				//else
				//	m_pMrFrame.WriteArc();
				bool bIsImage = false;
				eb::bigint nFileSize = -1;
				FILE * f = fopen(sMsgText.c_str(), "rb");
				if (f != NULL)
				{
					_fseeki64(f, 0, SEEK_END);
					nFileSize = _ftelli64(f);
					fclose(f);
					const std::wstring sImagePath = A2W_ACP(sMsgText.c_str());
					Gdiplus::Image * image = Gdiplus::Image::FromFile(sImagePath.c_str());
					bIsImage = (bool)(image->GetType()!= Gdiplus::ImageTypeUnknown);
					delete image;
				}
				if (bIsImage)
				{
					const tstring sFilePath(libEbc::URLEncode(sMsgText.c_str()));
					CString sTemp;
					sTemp.Format(_T("#CTRL:%d:%d:%s#%s"),(int)(EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB),(int)EB_MR_CTRL_DATA_TYPE_IMAGE,sFilePath.c_str(),sMsgText.c_str());
					m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
					m_pMrFrameInterface->WriteLine();
				}else
				{
					WriteFileHICON(sMsgText.c_str());
				}
				{
					const tstring sFileName = libEbc::GetFileName(sMsgText);
					CString sFileText;
					if (nFileSize == -1)
						sFileText.Format(_T("%s (文件不存在)"),sFileName.c_str());
					else if (nFileSize >= const_gb_size)
						sFileText.Format(_T("%s (%.02fGB)"),sFileName.c_str(),(double)nFileSize/const_gb_size);
					else if (nFileSize >= const_mb_size)
						sFileText.Format(_T("%s (%.02fMB)"),sFileName.c_str(),(double)nFileSize/const_mb_size);
					else if (nFileSize >= const_kb_size)
						sFileText.Format(_T("%s (%.02fKB)"),sFileName.c_str(),(double)nFileSize/const_kb_size);
					else
						sFileText.Format(_T("%s (%lldByte)"),sFileName.c_str(),nFileSize);
					m_pMrFrameInterface->WriteSpace(1);
					CString sTemp;
					sTemp.Format(_T("#CTRL:0:%d:#%s"),(int)EB_MR_CTRL_DATA_TYPE_FILE,sFileText);
					m_pMrFrameInterface->WriteUrl((LPCTSTR)sTemp, sMsgText.c_str(),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB,theDefaultFlatBlackTextColor);
					//m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, sMsgText.c_str(),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN|EB_MR_CTRL_TYPE_MOVE_ENTER_CB|EB_MR_CTRL_TYPE_MOVE_LEAVE_CB,theDefaultFlatBlackTextColor);
				}
				//m_pMrFrameInterface->WriteLine();
				//m_pMrFrameInterface->WriteSpace(11);
				//if (nFileSize >= 0)
				//{
				//	m_pMrFrameInterface->WriteOpenFile(L"打开",sMsgText.c_str());
				//	m_pMrFrameInterface->WriteSpace(2);
				//}
				//m_pMrFrameInterface->WriteOpenDir(L"打开文件夹",sMsgText.c_str());
			}break;
		default:
			break;
		}
			//pRecord = theApp.m_pBoUsers->previous(nCookie);
	}
	theApp.m_pBoUsers->reset(nCookie);

	if (!sToWriteString.empty())
	{
		m_pMrFrameInterface->WriteString(sToWriteString.c_str(),bReceive?theDefaultChatTextColor2:theDefaultChatTextColor1);
		sToWriteString.clear();
	}

	if (nRet > 0)
	{
		//m_pMrFrameInterface->WriteTime(m_tLastMsgTime);
		//m_tLastMsgTime = 0;
		m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
		//if (m_nViewMsgId>0)
		//{
		//	m_pMrFrameInterface->ViewLine(m_nViewMsgId);
		//	m_nViewMsgId = 0;
		//}else
		//{
		//	m_pMrFrameInterface->ScrollToPos(-1);
		//}
		AddLineString(0,_T("-------- 以上是历史消息  --------"),1);
		SetTimer(TIMERID_SCROLLTOEND,100,NULL);
	//}else if (m_bOwnerCall)
	//{
	//	WriteTitle(0,false,-1,"阿猫",0,"",0);
	//	m_pMrFrameInterface->WriteString(L"当前好像木有聊天内容哦",theDefaultChatTextColor2);
	//	WriteTitle(0,false,theApp.GetLogonUserId(),"阿狗",0,"",0);
	//	m_pMrFrameInterface->WriteString(L"那就赶紧给我发一个啊^_^",theDefaultChatTextColor2);
	}
}

void CDlgChatInput::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMERID_CHECK_FORBID:
		{
			KillTimer(TIMERID_CHECK_FORBID);
			CheckMyForbidSpeechState(false,false);
		}break;
	case TIMERID_SC_OK:
		{
			KillTimer(TIMERID_SC_OK);
			m_richInput.SetFocus();
			// Ctrl+V
			keybd_event(VK_CONTROL, 0, 0, 0);
			keybd_event('V', 0, 0, 0);
			keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
			keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
		}break;
	case TIMERID_HIDE_NOTIFY1:
		{
			KillTimer(TIMERID_HIDE_NOTIFY1);
			m_pPanelStatus->ShowWindow(SW_HIDE);
			//m_labelNotify1.ShowWindow(SW_HIDE);

			//if (m_pCallInfo.m_sGroupCode>0)
			//	m_pShowNotifyList.clear();
		}break;
	case TIMERID_CHECK_NOTIFY:
		{
			if (m_pCallInfo.m_sGroupCode>0)
			{
				if (m_tShowNotifyTime>0 && m_pNotifyList.size()<=3 && (m_tShowNotifyTime+3)>=time(0))	// *显示数据3个内，保证前面至少显示3秒
				{
					break;
				}

				CLockMap<eb::bigint,bool> m_pShowNotifyList;	// for group call
				int nCount = 0;
				tstring sDisplayMemberName;
				CEbNotifyInfo::pointer pNotifyInfo;
				while (m_pNotifyList.front(pNotifyInfo))
				{
					//if (nCount==0 && !m_pShowNotifyList.empty())
					//	m_pShowNotifyList.clear();
					if (!m_pShowNotifyList.insert(pNotifyInfo->m_nFromUserId,true,false)) continue;
					tstring sMemberUserName;
					if (!theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,pNotifyInfo->m_nFromUserId,sMemberUserName))
					{
						continue;
					}
					nCount++;
					if (nCount!=1)
						sDisplayMemberName.append("，");
					sDisplayMemberName.append(sMemberUserName);
					if (nCount>=3) break;
				}
				if (!sDisplayMemberName.empty())
				{
					m_tShowNotifyTime = time(0);
					CString sNotifyString;
					sNotifyString.Format(_T("%s正在输入..."),sDisplayMemberName.c_str());

					KillTimer(TIMERID_HIDE_NOTIFY1);
					SetTimer(TIMERID_HIDE_NOTIFY1,12*1000,NULL);	// * 显示12秒后自动隐藏
					CRect rectBtnClose;
					m_btnClose.GetWindowRect(&rectBtnClose);
					this->ScreenToClient(&rectBtnClose);

					USES_CONVERSION;
					const std::wstring sText = A2W(sNotifyString);
					CRect rect;
					m_pPanelStatus->GetWindowRect(&rect);
					this->ScreenToClient(&rect);
					const int nMaxWidth = rectBtnClose.left-rect.right;
					const int nWidth = m_pPanelStatus->MeasureTextWidth(sText.c_str(),nMaxWidth+100);
					rect.right = rect.left + min(nMaxWidth,nWidth);
					m_pPanelStatus->MoveWindow(&rect);
					
					//CRect rect;
					//m_wndSplitter1.GetClientRect(&rect);
					//const int nWidth = m_pPanelStatus->MeasureTextWidth(sText.c_str(),rect.Width());
					//m_pPanelStatus->MoveWindow(rect.left, rect.top-22,nWidth,22);
					m_pPanelStatus->SetDrawText(sText.c_str());

					//m_labelNotify1.ShowWindow(SW_HIDE);
					//m_labelNotify1.SetWindowText(sNotifyString);
					//m_labelNotify1.SetToolTipText(sNotifyString);
					//m_labelNotify1.ShowWindow(SW_SHOW);
				}
			}
		}break;
	case TIMERID_SEND_CALL_NOTIFY:
		{
			const long nCurrentTextLength = m_richInput.GetTextLength();
			CString sCurrentTextString;
			m_richInput.GetWindowText(sCurrentTextString);
			if (thePrevTextLength!=nCurrentTextLength || thePrevTextString!=sCurrentTextString)
			{
				thePrevTextLength = nCurrentTextLength;
				thePrevTextString = sCurrentTextString;
				if (thePrevTextLength>0 || !thePrevTextString.IsEmpty())
					theEBAppClient.EB_CallNotify(m_pCallInfo.GetCallId(),0,1,0,"");
			}
		}break;
	case TIMERID_MOVE_LEAVE:
		{
			if (m_pDlgToolbar.GetSafeHwnd()!=NULL)
			{
				CPoint pos;
				::GetCursorPos(&pos);
				CRect rect;
				m_pDlgToolbar.GetWindowRect(&rect);
				if (!rect.PtInRect(pos))
				{
					KillTimer(nIDEvent);
					m_pDlgToolbar.HideReset();
				}
			}
		}break;
	case TIMERID_DELETE_SELECT_MSG:
		{
			KillTimer(nIDEvent);
			if (!m_pDeletMsgId.empty())
			{
				CString sText;
				sText.Format(_T("确定删除选中的：\r\n%d 条聊天记录吗？"), (int)m_pDeletMsgId.size());
				if (CDlgMessageBox::EbDoModal(this,"删除聊天记录",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
				{
					for (size_t i=0;i<m_pDeletMsgId.size();i++)
					{
						const eb::bigint nMsgId = m_pDeletMsgId[i];
						m_pMrFrameInterface->DeleteLine(nMsgId);
						theApp.DeleteDbRecord(nMsgId);
					}
					m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
					m_pMrFrameInterface->ScrollToPos(-1);
				}
				m_pDeletMsgId.clear();
			}
		}break;
	case TIMERID_SHOW_MRFRAME:
		{
			KillTimer(TIMERID_SHOW_MRFRAME);
			this->m_pMrFrameControl.Invalidate();
			m_richInput.SetFocus();
		}break;
	case TIMERID_SCROLLTOEND:
		{
			KillTimer(TIMERID_SCROLLTOEND);
			m_pMrFrameInterface->ScrollToPos(-1);
		}break;
	default:
		break;
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

LRESULT CDlgChatInput::OnDropFile(WPARAM wParam, LPARAM lParam)
{
	//AfxMessageBox(_T("CDlgChatInput::OnDropFile"));
	//return 0;

	const char * lpszFile = (const char*)wParam;
	SendFile(lpszFile,true);
	return 1;
}
//int   GetCodecClsid(const   WCHAR*   format,   CLSID*   pClsid)
//{
//	UINT     num   =   0;                     //   number   of   image   encoders
//	UINT     size   =   0;                   //   size   of   the   image   encoder   array   in   bytes
//
//	Gdiplus::ImageCodecInfo*   pImageCodecInfo   =   NULL;
//
//	Gdiplus::GetImageEncodersSize(&num,   &size);
//	if(size   ==   0)
//		return   -1;     //   Failure
//
//	pImageCodecInfo   =   (Gdiplus::ImageCodecInfo*)(malloc(size));
//	if(pImageCodecInfo   ==   NULL)
//		return   -1;     //   Failure
//
//	Gdiplus::GetImageEncoders(num,   size,   pImageCodecInfo);
//	for(UINT   j   =   0;   j   <   num;   ++j)
//	{
//		if(   wcscmp(pImageCodecInfo[j].MimeType,   format)   ==   0   )
//		{
//			*pClsid   =   pImageCodecInfo[j].Clsid;
//			free(pImageCodecInfo);
//			return   j;     //   Success
//		}        
//	}   //   for
//	free(pImageCodecInfo);
//	return   -1;     //   Failure
//
//}   //   GetCodecClsid
LRESULT CDlgChatInput::OnRichSaveAs(WPARAM wParam, LPARAM lParam)
{
	if (wParam != (WPARAM)m_richInput.GetSafeHwnd())
	{
		return 0;
	}

	char szFileName[MAX_PATH*2] = {0} ; 
	const time_t tNow = time(0);
	const struct tm *newtime = localtime(&tNow);
	sprintf(szFileName,"EB图片%04d%02d%02d%02d%02d%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);

	OPENFILENAME ofn={0};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	ofn.lpstrFilter = "PNG (*.png)\0*.png\0JPEG (*.jpg)\0*.jpg\0BMP (*.bmp)\0*.bmp\0\0";
	ofn.lpstrDefExt = "png";
	ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
	if (GetSaveFileName(&ofn)==TRUE ) 
	{
		tstring sExt(ofn.lpstrFile);
		sExt = sExt.substr(sExt.size()-3);
		std::transform(sExt.begin(), sExt.end(), sExt.begin(), tolower);
		char * lpObjectData = 0;
		DWORD dwDataSize = 0;
		DWORD dwUserData = 0;
		if (m_richInput.GetObjectData(REO_IOB_SELECTION, &lpObjectData, dwDataSize,dwUserData))
		{
			// 这里需要区分是否表情资源
			USES_CONVERSION;
			if (dwUserData > 0)
			{
				ASSERT (theApp.m_pDlgEmotionSelect!=NULL);
				eb::bigint sResourceId = 0;
				if (theApp.m_pDlgEmotionSelect->GetImageResourceId(dwUserData,sResourceId))
				{
					CString sImageFileName;
					sImageFileName.Format(_T("%s\\%lld"), theEBAppClient.EB_GetResourcePath().c_str(),sResourceId);

					CLSID                           codecClsid;
					if (sExt=="png")
						libEbc::GetCodecClsid(L"image/png",   &codecClsid);
					else if (sExt=="bmp")
						libEbc::GetCodecClsid(L"image/bmp",   &codecClsid);
					else
						libEbc::GetCodecClsid(L"image/jpeg",   &codecClsid);
					{
						Gdiplus::Image   image(A2W_ACP(sImageFileName));
						image.Save(A2W_ACP(ofn.lpstrFile), &codecClsid);
					}
				}
			}else if (sExt=="bmp")
			{
				save_2_bmp(lpObjectData, dwDataSize, ofn.lpstrFile);
				delete []lpObjectData;
			}else
			{
				CLSID                           codecClsid;
				if (sExt=="png")
					libEbc::GetCodecClsid(L"image/png",   &codecClsid);
				//else if (sExt=="bmp")
				//	libEbc::GetCodecClsid(L"image/bmp",   &codecClsid);
				else
					libEbc::GetCodecClsid(L"image/jpeg",   &codecClsid);

				static unsigned int static_index = 0;
				CString sFileName;
				sFileName.Format(_T("%s\\%x%02x%02x.bmp"), theApp.GetUserImagePath(), (int)time(0),rand()%0xff,(++static_index)%0xff);
				save_2_bmp(lpObjectData, dwDataSize, sFileName);
				delete []lpObjectData;
				{
					Gdiplus::Image   image(A2W_ACP(sFileName));
					image.Save(A2W_ACP(ofn.lpstrFile), &codecClsid);
				}
				DeleteFile(sFileName);

			}
		}
	}
	return 1;
}

void CDlgChatInput::OnBnClickedButtonEbsc()
{
	this->GetParent()->PostMessage(EB_MSG_EBSC, 0, 0);
	m_richInput.SetFocus();
}
void CDlgChatInput::OnBnClickedButtonMinEbsc()
{
	if (m_pMinEBSCMenu.GetSafeHmenu()==NULL)
	{
		m_pMinEBSCMenu.CreatePopupMenu();
		m_pMinEBSCMenu.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SWITCH_MIN_EBSC,_T("截图时隐藏当前窗口"));
	}
	m_pMinEBSCMenu.CheckMenuItem(EB_COMMAND_SWITCH_MIN_EBSC,theApp.GetMinEBSC()?MF_CHECKED:MF_UNCHECKED);

	CPoint point;
	GetCursorPos(&point);
	m_pMinEBSCMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN|TPM_BOTTOMALIGN,point.x,point.y,this);
}
void CDlgChatInput::OnCmdSwitchMinEBSC(void)
{
	if (theApp.GetMinEBSC())
	{
		theApp.SetMinEBSC(false);
		this->SetToolTipText(IDC_BUTTON_MIN_EBSC,_T("截图时显示当前窗口（点击切换）"));
	}else
	{
		theApp.SetMinEBSC(true);
		this->SetToolTipText(IDC_BUTTON_MIN_EBSC,_T("截图时隐藏当前窗口（点击切换）"));
	}
}

//void CDlgChatInput::ShowImageWindow(bool bShow)
//{
//	if (m_pDlgEmotionSelect==NULL)
//	{
//		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
//		//CWnd * pParent = this;
//		m_pDlgEmotionSelect = new CDlgEmotionSelect(pParent);
//		m_pDlgEmotionSelect->Create(CDlgEmotionSelect::IDD,pParent);
//		m_pDlgEmotionSelect->SetCallback(this);
//	}
//
//	CRect wndRect;
//	m_btnImage.GetWindowRect(&wndRect);
//	const int const_dlg_width = 430;
//	const int const_dlg_height = 262;
//	CRect rect;
//	rect.left = wndRect.left-20;
//	rect.bottom = wndRect.top;
//	rect.top = rect.bottom - const_dlg_height;
//	rect.right = rect.left + const_dlg_width;
//	m_pDlgEmotionSelect->MoveWindow(&rect);
//	if (bShow)
//	{
//		m_pDlgEmotionSelect->ShowWindow(SW_SHOW);
//		m_pDlgEmotionSelect->SetForegroundWindow();
//	}else
//	{
//		m_pDlgEmotionSelect->ShowWindow(SW_HIDE);
//	}
//	//m_pDlgEmotionSelect->SetCircle();
//}

void CDlgChatInput::OnBnClickedButtonImage()
{
	CRect wndRect;
	m_btnImage.GetWindowRect(&wndRect);
	const int const_dlg_width = 430;
	const int const_dlg_height = 262;
	CRect rect;
	rect.left = wndRect.left-20;
	rect.bottom = wndRect.top;
	rect.top = rect.bottom - const_dlg_height;
	rect.right = rect.left + const_dlg_width;
	theApp.ShowImageWindow(true,this,&rect);
}

void CDlgChatInput::Fire_onContextMenuDelete(LONGLONG nMsgId,LONG nIndex)
{
	if (nMsgId>0)
	{
		m_pDeletMsgId.push_back(nMsgId);
	}else if (!m_pDeletMsgId.empty())
	{
		SetTimer(TIMERID_DELETE_SELECT_MSG,10,NULL);
	}
}

void CDlgChatInput::Fire_onContextMenuSelect(ULONG nItemData, const CEBString& sSelectString,LONGLONG nSelectMsgId)
{
	if (nItemData>=0 && nItemData<theApp.m_pChatMenuSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pChatMenuSubscribeFuncList[nItemData];
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,libEbc::ACP2UTF8(sSelectString.c_str()),"",NULL);
	}
}
void CDlgChatInput::Fire_onItemLBtnClick(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId)
{
	switch (nItemData)
	{
	case EB_MR_CTRL_DATA_TYPE_USER_ECARD:
		{
			ProcessUserECard(nSelectMsgId, 0, true);
		}break;
	case EB_MR_CTRL_DATA_TYPE_READ_FLAG:
		{
			//if (m_pMrFrameInterface!=NULL)
			//	m_pMrFrameInterface->DeleteLineItem(nLineId,nItemId);
			//CString sSql;
			//sSql.Format(_T("UPDATE msg_record_t SET read_flag=1 WHERE msg_id=%lld AND read_flag=0"),nSelectMsgId);
			//theApp.m_pBoUsers->execute(sSql);
		}break;
	case EB_MR_CTRL_DATA_TYPE_UID:
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_CallAccount(sParamString.c_str(),0);
#else
			theEBAppClient.EB_CallAccount(sParamString.c_str());
#endif
		}break;
	case EB_MR_CTRL_DATA_TYPE_DOWNLOAD_RESOURCE:
		{
			const std::string::size_type find = sParamString.find(",");
			if (find==std::string::npos) break;
			const mycp::bigint nResourceId = cgc_atoi64(sParamString.substr(0,find).c_str());
			const tstring sFullName = sParamString.substr(find+1);

			tstring sFileName;
			tstring sFileExt;
			libEbc::GetFileExt(sFullName,sFileName,sFileExt);
			CFileDialog dlg(FALSE, sFileExt.c_str(), sFullName.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, theAllFilesFilter, this);
			if (dlg.DoModal() == IDOK)
			{
				const CString sPathName(dlg.GetPathName());
#ifdef USES_EBCOM_TEST
				theEBClientCore->EB_DownloadFileRes(nResourceId, (LPCTSTR)sPathName);
#else
				theEBAppClient.EB_DownloadFileRes(nResourceId, sPathName);
#endif
			}
		}break;
	case EB_MR_CTRL_DATA_TYPE_OPEN_SHARE:
		{
			this->GetParent()->PostMessage(EB_COMMAND_VIEW_GROUP_SHARE);
		}break;
	default:
		{
		}break;
	}
}
void CDlgChatInput::Fire_onItemLBtnDblClick(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId)
{
	switch (nItemData)
	{
	case EB_MR_CTRL_DATA_TYPE_UID:
		{
//#ifdef USES_EBCOM_TEST
//			theEBClientCore->EB_CallAccount(sParamString.c_str(),0);
//#else
//			theEBAppClient.EB_CallAccount(sParamString.c_str());
//#endif
		}break;
	default:
		{
		}
		break;
	}
}
void CDlgChatInput::ProcessUserECard(mycp::bigint nSelectMsgId, ULONG nParam, bool bShowNow)
{
	if (m_pDlgToolbar.GetSafeHwnd()!=NULL && m_pDlgToolbar.IsWindowVisible())
		m_pDlgToolbar.HideReset();
	tstring sCardInfo;
	if (!m_pCardInfoList.find(nSelectMsgId,sCardInfo))
		return;//break;
	if (m_pDlgViewContact.GetSafeHwnd()==NULL)
	{
		m_pDlgViewContact.Create(CDlgViewContactInfo::IDD,this);
	}
	int nCardType = 0;
	tstring sCardData;
	theEBAppClient.EB_ParseCardInfo(sCardInfo,nCardType,sCardData);
	EB_ECardInfo pUserECard;
	if (nCardType==1 && theEBAppClient.EB_GetUserECardByFromInfo(sCardData,&pUserECard))
	{
		EB_MemberInfo pMemberInfo;
		EB_GroupInfo pGroupInfo;
		if (pUserECard.m_sMemberCode>0 && theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,&pGroupInfo,pUserECard.m_sMemberCode))
			m_pDlgViewContact.SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
		else if (pUserECard.m_nMemberUserId>0 && theEBAppClient.EB_GetMemberInfoByUserId2(&pMemberInfo,&pGroupInfo,pUserECard.m_nMemberUserId))
			m_pDlgViewContact.SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
		else
		{
			EB_ContactInfo pContactInfo;
			pContactInfo.m_nContactUserId = pUserECard.m_nMemberUserId;
			pContactInfo.m_sName = pUserECard.m_sAccountName;
			//pContactInfo.m_sDescription = pUserECard.m_sName;
			pContactInfo.m_sCompany = pUserECard.m_sEnterprise;
			pContactInfo.m_sGroupName = pUserECard.m_sGroupName;
			pContactInfo.m_sJobTitle = pUserECard.m_sTitle;
			pContactInfo.m_sPhone = pUserECard.m_sPhone;
			pContactInfo.m_sTel = pUserECard.m_sTel;
			pContactInfo.m_sEmail = pUserECard.m_sEmail;
			pContactInfo.m_sAddress = pUserECard.m_sAddress;
			//if ((m_pContactInfo.m_nContactType&EB_CONTACT_TYPE_MAIL)==EB_CONTACT_TYPE_MAIL)

			m_pDlgViewContact.SetContactInfo(&pContactInfo);
		}
		const int const_dlg_width = 380;
		const int const_dlg_height = 220;
		CPoint pos;
		if (nParam==0)
			::GetCursorPos(&pos);
		else
		{
			pos.x = LOWORD(nParam);
			pos.y = HIWORD(nParam);
		}
		const int x = pos.x+15 + (pos.x%15);
		const int y = pos.y-35 - (pos.y%35);
		m_pDlgViewContact.MoveWindow(x,y,const_dlg_width,const_dlg_height);
		m_pDlgViewContact.SetCircle();
		m_pDlgViewContact.SetMoveEnter(bShowNow);
	}
}
void CDlgChatInput::Fire_onItemMoveEnter(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId, ULONG nParam)
{
	//m_nMoveEnterMsgId = nSelectMsgId;
	//m_sMoveEnterString = sParamString;
	KillTimer(TIMERID_MOVE_LEAVE);

	switch (nItemData)
	{
	case EB_MR_CTRL_DATA_TYPE_USER_ECARD:
		{
			ProcessUserECard(nSelectMsgId, nParam, false);
		}break;
	case EB_MR_CTRL_DATA_TYPE_UID:
		{
			if (m_pDlgToolbar.GetSafeHwnd()!=NULL && m_pDlgToolbar.IsWindowVisible())
				m_pDlgToolbar.HideReset();
			const eb::bigint nUserId = eb_atoi64(sParamString.c_str());
			if (m_pCallInfo.m_sGroupCode==0 || nUserId==0)
				break;

			if (m_pDlgViewContact.GetSafeHwnd()==NULL)
			{
				m_pDlgViewContact.Create(CDlgViewContactInfo::IDD,this);
			}
			
			EB_GroupInfo pGroupInfo;
			if (!theEBAppClient.EB_GetGroupInfo(m_pCallInfo.m_sGroupCode,&pGroupInfo))
				break;
			EB_MemberInfo pMemberInfo;
			if (!theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,m_pCallInfo.m_sGroupCode,nUserId))
				break;
			m_pDlgViewContact.SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
			const int const_dlg_width = 380;
			const int const_dlg_height = 220;
			CPoint pos;
			pos.x = LOWORD(nParam);
			pos.y = HIWORD(nParam);
			//::GetCursorPos(&pos);
			const int x = pos.x+15 + (pos.x%15);
			const int y = pos.y-35 - (pos.y%35);
			m_pDlgViewContact.MoveWindow(x,y,const_dlg_width,const_dlg_height);
			m_pDlgViewContact.SetCircle();
			m_pDlgViewContact.SetMoveEnter();
			//m_pDlgViewContact.ShowWindow(SW_SHOW);
			//m_pDlgViewContact.SetCheckLeave();
		}break;
	//case EB_MR_CTRL_DATA_TYPE_READ_FLAG:
	//case EB_MR_CTRL_DATA_TYPE_DELETE_MSGID:
	//case EB_MR_CTRL_DATA_TYPE_SEND_MSGID:
	case EB_MR_CTRL_DATA_TYPE_URL:
	case EB_MR_CTRL_DATA_TYPE_IMAGE:
	case EB_MR_CTRL_DATA_TYPE_FILE:
	case EB_MR_CTRL_DATA_TYPE_SELECT_STRING:
		{
			if (m_pDlgViewContact.GetSafeHwnd()!=NULL)// && m_pDlgViewContact.IsWindowVisible())
				m_pDlgViewContact.HideReset();
			if (m_pDlgToolbar.GetSafeHwnd()==NULL)
			{
				m_pDlgToolbar.Create(CDlgToolbar::IDD,this);
				m_pDlgToolbar.SetMsgHwnd(this->GetSafeHwnd(),this);
				m_pDlgToolbar.SetChildMode(true);
			}
			bool bChangeData = false;
			const int nCount = m_pDlgToolbar.SetMoveEnterData((EB_MR_CTRL_DATA_TYPE)nItemData,libEbc::URLDecode(sParamString.c_str(),false),nSelectMsgId,bChangeData);
			if (bChangeData || !m_pDlgToolbar.IsWindowVisible())
			{
				CPoint pos;
				pos.x = LOWORD(nParam);
				pos.y = HIWORD(nParam);
				//::GetCursorPos(&pos);
				int x = pos.x-35 - (pos.x%35);
				const int y = pos.y+10 + (pos.y%10);
				const int nToolBarWidth = 55*nCount+7;
				const int nScreenWidth = theApp.GetScreenWidth();
				if (x+nToolBarWidth>nScreenWidth)
					x = nScreenWidth-nToolBarWidth;
				m_pDlgToolbar.MoveWindow(x,y,nToolBarWidth,31);
				m_pDlgToolbar.ShowWindow(SW_SHOW);
			}
		}break;
	default:
		break;
	}

}
void CDlgChatInput::Fire_onItemMoveLeave(LONG nLineId, LONG nItemId, ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId)
{
	switch (nItemData)
	{
	case EB_MR_CTRL_DATA_TYPE_USER_ECARD:
	case EB_MR_CTRL_DATA_TYPE_UID:
		{
			if (m_pDlgViewContact.GetSafeHwnd()!=NULL)// && m_pDlgViewContact.IsWindowVisible())
			{
				m_pDlgViewContact.SetMoveLeave();
			}
		}break;
	default:
		{
			KillTimer(TIMERID_MOVE_LEAVE);
			SetTimer(TIMERID_MOVE_LEAVE,400,NULL);
		}break;
	}
	//m_nMoveEnterMsgId = 0;
	//m_sMoveEnterString.clear();
}
tstring CDlgChatInput::GetSelectString(void) const
{
	if (m_pMrFrameInterface==NULL) return "";
	const CEBString sString(m_pMrFrameInterface->GetSelectString().GetBSTR());
	return sString;
}
void CDlgChatInput::OnDeleteMsg(mycp::bigint nMsgId)
{
	if (nMsgId==0) return;
	if (m_pMrFrameInterface!=NULL)
	{
		m_pMrFrameInterface->DeleteLine(nMsgId);
		m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
	}
	theApp.DeleteDbRecord(nMsgId);
}

void CDlgChatInput::OnSelectedImageInfo(const CEBImageDrawInfo& pSelectedImageInfo)
{
	USES_CONVERSION;
	m_richInput.InsertImage(A2W_ACP(pSelectedImageInfo.m_sResFile.c_str()),pSelectedImageInfo.m_dwUserData);
	m_richInput.SetFocus();
}

BOOL CDlgChatInput::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CDlgChatInput::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// 使图标在工作区矩形中居中
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		//GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		//// 绘制图标
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc, rectClient);
		this->ClearBgDrawInfo();
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
	}
}

void CDlgChatInput::SetCallInfo(const EB_CallInfo& pCallInfo, const EB_AccountInfo& pFromAccountInfo)
{
	m_pCallInfo = pCallInfo;
	m_pFromAccountInfo = pFromAccountInfo;
}

//void CDlgChatInput::OnDropFiles(HDROP hDropInfo)
//{
//	// TODO: Add your message handler code here and/or call default
//	AfxMessageBox(_T("CDlgChatInput::OnDropFiles"));
//	return;
//	{
//		int nFileSum = DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);   //得到文件个数
//		for (int i=0; i<nFileSum; i++)
//		{
//			char lpszFile[MAX_PATH];
//			memset(lpszFile,0,MAX_PATH);
//			DragQueryFile(hDropInfo,i,lpszFile,MAX_PATH);         //得到文件名
//			if (m_pCallInfo.m_sGroupCode>0)
//			{
//#ifdef USES_EBCOM_TEST
//				theEBClientCore->EB_AddTempGroupFileRes(lpszFile,"",m_pCallInfo.m_sGroupCode);
//#else
//				theEBAppClient.EB_AddTempGroupFileRes(lpszFile,"",m_pCallInfo.m_sGroupCode);
//#endif
//			}else// if (this->m_pDlgChatInput != NULL)
//			{
//				SendFile(lpszFile);
//			}
//		}
//	}
//	__super::OnDropFiles(hDropInfo);
//}

void CDlgChatInput::OnBnClickedButtonSwitchReturn()
{
	// TODO: Add your control notification handler code here
	if (m_pSendTypeSwitchMenu.GetSafeHmenu()==NULL)
	{
		m_pSendTypeSwitchMenu.CreatePopupMenu();
		m_pSendTypeSwitchMenu.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SEND_TYPE_RETURN,_T("按Enter键发送消息"));
		m_pSendTypeSwitchMenu.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SEND_TYPE_CTRL_RETURN,_T("按Ctrl+Enter键发送消息"));
	}
	m_pSendTypeSwitchMenu.CheckMenuItem(EB_COMMAND_SEND_TYPE_RETURN,theApp.GetSendType()==0?MF_CHECKED:MF_UNCHECKED);
	m_pSendTypeSwitchMenu.CheckMenuItem(EB_COMMAND_SEND_TYPE_CTRL_RETURN,theApp.GetSendType()==1?MF_CHECKED:MF_UNCHECKED);

	CPoint point;
	GetCursorPos(&point);
	m_pSendTypeSwitchMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN|TPM_BOTTOMALIGN,point.x,point.y,this);
}

void CDlgChatInput::OnCmdSendTypeReturn(void)
{
	theApp.SetSendType(0);
	m_btnSend.SetToolTipText(_T("按Enter键发送消息，按Ctrl+Enter键换行"));
	this->SetToolTipText(IDC_RICHEDIT_INPUT,_T("Enter发送\n[Ctrl+Enter换行]"));
}
void CDlgChatInput::OnCmdSendTypeCtrlReturn(void)
{
	theApp.SetSendType(1);
	m_btnSend.SetToolTipText(_T("按Ctrl+Enter键发送消息，按Enter键换行"));
	this->SetToolTipText(IDC_RICHEDIT_INPUT,_T("Ctrl+Enter发送\n[Enter换行]"));
}

LRESULT CDlgChatInput::OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam)
{
	this->GetParent()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)wParam,(LPARAM)lParam);
	return 0;
}

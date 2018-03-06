// DlgChatInput.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgChatInput.h"
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
#define TIMERID_TEST_TEST			103
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
	//m_tLastMsgTime = 0;

}

CDlgChatInput::~CDlgChatInput()
{
}

void CDlgChatInput::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_RICHEDIT_INPUT, m_richInput);
	//DDX_Control(pDX, IDC_BUTTON_FONT, m_btnFont);
	DDX_Control(pDX, IDC_BUTTON_IMAGE, m_btnImage);
	DDX_Control(pDX, IDC_BUTTON_EBSC, m_btnEBSC);
	//DDX_Control(pDX, IDC_BUTTON_FILE_MANAGER, m_btnFileManager);
	DDX_Control(pDX, IDC_BUTTON_MSG_RECORD, m_btnMsgRecord);
	DDX_Control(pDX, IDC_BUTTON_SEND_TEST, m_btnSendTest);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_btnSend);
	DDX_Control(pDX, IDC_BUTTON_SWITCH_RETURN, m_btnSwitchReturn);
	DDX_Control(pDX, IDC_COMBO_SENDTO, m_comboSendTo);
	DDX_Control(pDX, IDC_STATIC_PRIVATE, m_checkPrivate);
}


BEGIN_MESSAGE_MAP(CDlgChatInput, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CDlgChatInput::OnBnClickedButtonSend)
	ON_NOTIFY(EN_MSGFILTER, IDC_RICHEDIT_INPUT, OnMsgfilterRich)
	ON_NOTIFY(EN_LINK, IDC_RICHEDIT_INPUT, OnRichEditExLink )
	ON_NOTIFY(EN_PROTECTED, IDC_RICHEDIT_INPUT, OnRichEditProtected)
	ON_BN_CLICKED(IDC_BUTTON_FONT, &CDlgChatInput::OnBnClickedButtonFont)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgChatInput::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_FILE_MANAGER, &CDlgChatInput::OnBnClickedButtonFileManager)
	ON_BN_CLICKED(IDC_BUTTON_MSG_RECORD, &CDlgChatInput::OnBnClickedButtonMsgRecord)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DROP_FILE, OnDropFile)
	ON_BN_CLICKED(IDC_BUTTON_EBSC, &CDlgChatInput::OnBnClickedButtonEbsc)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE, &CDlgChatInput::OnBnClickedButtonImage)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_RETURN, &CDlgChatInput::OnBnClickedButtonSwitchReturn)
	ON_COMMAND(EB_COMMAND_SEND_TYPE_RETURN, OnCmdSendTypeReturn)
	ON_COMMAND(EB_COMMAND_SEND_TYPE_CTRL_RETURN, OnCmdSendTypeCtrlReturn)
	ON_COMMAND(EB_COMMAND_SEND_100TEST, OnCmdSend100Test)
	ON_BN_CLICKED(IDC_BUTTON_SEND_TEST, &CDlgChatInput::OnBnClickedButtonSendTest)
END_MESSAGE_MAP()

void CDlgChatInput::SetCtrlColor(void)
{
	if (m_pMrFrameInterface!=NULL)
		m_pMrFrameInterface->SetBackgroundColor(theApp.GetBgColor1());
	m_richInput.SetBackgroundColor(FALSE,theApp.GetBgColor1());
	m_wndSplitter1.SetColor(theApp.GetBgColor3(),theApp.GetBgColor1());

	m_checkPrivate.SetCheckBoxHoverBorder(TRUE,theApp.GetHotColor2());
	m_btnSendTest.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
	m_btnClose.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
	m_btnSend.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
	m_btnSwitchReturn.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
	//m_btnSwitchReturn.SetDrawPanel(true,theApp.GetMainColor());
}

// CDlgChatInput message handlers

BOOL CDlgChatInput::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//BOOST_ASSERT (m_pChatInfo.get() != 0);
	this->SetTransparentType(CEbDialogBase::TT_DLG|CEbDialogBase::TT_STATIC);
	this->SetMouseMove(FALSE);

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
	m_pMrFrameInterface->SetLineInterval(3);
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

	CRect rt(0, 0, 0, 60);
	DWORD dwStyle = WS_CHILD|WS_VSCROLL|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL;
	m_richInput.Create( dwStyle, rt, this, IDC_RICHEDIT_INPUT );
	//m_richInput.PasteSpecial(CF_BITMAP);
	m_richInput.SetEventMask(ENM_LINK|ENM_PROTECTED);
	m_richInput.SetDefaultCharFormat( theApp.m_cfDefault );
	//m_richInput.ModifyStyle(0,ES_WANTRETURN);
	//m_richInput.SetTargetDevice(NULL,1);

	//m_richInput.SetFocus();
	m_richInput.LimitText(24*1024);
	this->EnableToolTips();
	this->SetToolTipText(IDC_COMBO_SENDTO,_T("选择对某人说"));
	this->SetToolTipText(IDC_STATIC_PRIVATE,_T("选择跟某人私聊"));
	if (theApp.GetSendType()==0)
		this->SetToolTipText(IDC_RICHEDIT_INPUT,_T("Enter发送\n[Ctrl+Enter换行]"));
	else
		this->SetToolTipText(IDC_RICHEDIT_INPUT,_T("Ctrl+Enter发送\n[Enter换行]"));

	m_checkPrivate.LoadLabel(IDB_PNG_CHECKBOX,-1,0,2);
	m_checkPrivate.SetTextLeft(5);
	m_btnImage.SetAutoSize(false);
	m_btnImage.Load(IDB_PNG_BTN_EMOTION);
	m_btnImage.SetToolTipText(_T("发送表情"));
	m_btnImage.SetWindowText(_T(""));
	m_btnEBSC.SetAutoSize(false);
	m_btnEBSC.Load(IDB_PNG_BTN_EBCS);
	m_btnEBSC.SetToolTipText(_T("屏幕画布截图 Shift + Alt + A"));
	m_btnEBSC.SetWindowText(_T(""));
	//m_btnFileManager.SetAutoSize(false);
	//m_btnFileManager.Load(IDB_PNG_58X24);
	//m_btnFileManager.SetToolTipText(_T("查看传输文件"));
	//m_btnFileManager.ShowWindow(SW_HIDE);	// ??暂时没有用
	m_btnMsgRecord.SetFont(this->GetFont());
	m_btnMsgRecord.SetAutoSize(false);
	m_btnMsgRecord.Load(IDB_PNG_BTN_MSGRECORD);
	m_btnMsgRecord.SetToolTipText(_T("查看消息记录"));
	m_btnMsgRecord.SetTextLeft(15);
	m_btnClose.SetTextHotMove(false);
	m_btnClose.SetNorTextColor(RGB(250,250,250));
	m_btnClose.SetHotTextColor(RGB(250,250,250));
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnSend.SetTextHotMove(false);
	m_btnSend.SetNorTextColor(RGB(250,250,250));
	m_btnSend.SetHotTextColor(RGB(250,250,250));
	if (theApp.GetSendType()==0)
		m_btnSend.SetToolTipText(_T("按Enter键发送消息，按Ctrl+Enter键换行"));
	else
		m_btnSend.SetToolTipText(_T("按Ctrl+Enter键发送消息，按Enter键换行"));
	//m_btnSend.SetTextLeft(8);
	//m_btnSend.ShowWindow(SW_HIDE);
	m_btnSendTest.SetTextHotMove(false);
	m_btnSendTest.SetNorTextColor(RGB(250,250,250));
	m_btnSendTest.SetHotTextColor(RGB(250,250,250));
	//if (m_pCallInfo.m_sGroupCode==0)
	//	m_btnSendTest.ShowWindow(SW_SHOW);

	m_btnSwitchReturn.SetDrawPanelRgn(true);
	m_btnSwitchReturn.SetWindowText(_T(""));
	m_btnSwitchReturn.SetToolTipText(_T("切换发送消息按键"));
	m_btnSwitchReturn.SetDrawTrianglePic(2,RGB(250,250,250),-1,-1,-1,10,5);

	SetCtrlColor();

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
	m_comboSendTo.AddString(_T(""));
	m_comboSendTo.SetComboBitmap(IDB_COMBO_LEFT, IDB_COMBO_RIGHT, IDB_COMBO_CENTER);
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

	LoadMsgRecord();
	//ShowImageWindow(false);
	SetTimer(TIMERID_SHOW_MRFRAME,100,NULL);
	//SetCircle(false);
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

	m_pSendTypeSwitchMenu.DestroyMenu();

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
		CSplitterControl::ChangePos(&m_comboSendTo, 0, delta);
		CSplitterControl::ChangePos(this->GetDlgItem(IDC_STATIC_PRIVATE), 0, delta);
		//CSplitterControl::ChangePos(&m_btnFont, 0, delta);
		CSplitterControl::ChangePos(&m_btnImage, 0, delta);
		CSplitterControl::ChangePos(&m_btnEBSC, 0, delta);
		//CSplitterControl::ChangePos(&m_btnFileManager, 0, delta);
		CSplitterControl::ChangePos(&m_btnMsgRecord, 0, delta);
		//CSplitterControl::ChangePos(&m_BtnFile, 0, delta);
		//CSplitterControl::ChangePos(&m_chkSelfAutoRoll, 0, delta);
		CSplitterControl::ChangeHeight(&m_pMrFrameControl, delta, CW_TOPALIGN);
		CSplitterControl::ChangeHeight(&m_richInput, -delta, CW_BOTTOMALIGN);
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
	const bool bIsDepDialog = m_pCallInfo.m_sGroupCode>0;
	if (cx==0 && cy==0)
	{
		CRect rect;
		this->GetClientRect(&rect);
		cx = rect.Width();
		cy = rect.Height();
	}
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
	const int const_bar_button_inteval	= 5;
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
	x -= (const_send_button_width+10);	// 关闭按钮间隔
	m_btnClose.MovePoint(x,const_send_button_top,const_send_button_width,const_send_button_height);
	if (m_pCallInfo.m_sGroupCode==0)
	{
		x -= (const_send_button_width+60);	// 关闭按钮间隔
		m_btnSendTest.MovePoint(x,const_send_button_top,const_send_button_width,const_send_button_height);
	}

	const int const_rich_input_top = const_send_button_top-rectRichInput.Height()-8;	// 发送按钮上面间隔
	if (m_richInput.GetSafeHwnd())
	{
		m_richInput.MoveWindow(0, const_rich_input_top, cx, rectRichInput.Height());
	}
	const int const_bar_button_top = const_rich_input_top-const_bar_button_height-1;
	const int const_splitter1_top = const_bar_button_top-const_splitter_height;
	//if (bIsDepDialog && rectOutSelf.Height() >= const_splitter1_top-4)	// ?4
	//{
	//	// call2group
	//	rectOutSelf.top = rectOutSelf.bottom-120;
	//}
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
	x = 5;
	int y = const_bar_button_top;
	if (bIsDepDialog)
	{
		const int const_combo_sentto_width = 100;
		const int const_check_private_width = 65;
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

	//y -= 1;	// ?
	x = cx-m_btnMsgRecord.GetImgWidth()-const_btn_right_inteval;
	m_btnMsgRecord.MovePoint(x,y);
	//x -= m_btnMsgRecord.GetImgWidth();
	//m_btnFileManager.MovePoint(x,y);

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
	const std::string sLogonAccount = theApp.GetLogonAccount();
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
	if (theEBAppClient.EB_SendRich(m_pCallInfo.GetCallId(),&pRichMsg,nToUserId,m_checkPrivate.GetCheck()?true:false)==0)
#endif
	{
		m_richInput.SetSel(0, -1);
		m_richInput.Clear();
		//m_richInput.SetWindowText(_T(""));
		m_btnSend.SetWindowText(_T("发送"));
	}
	m_richInput.SetFocus();
	return;
	// TODO: Add your control notification handler code here
	//EDITSTREAM es;
	//es.dwCookie = 0;//(DWORD) &cFile; //设置用例参数,以便回调函数调用
	//es.pfnCallback = MyStreamOutCallback;
	//m_richInput.StreamOut(SF_RTF, es);
	//return; 

	save();
	return;

	CString sInputText;
	m_richInput.GetWindowText(sInputText);
	if (sInputText.IsEmpty())
	{
		m_richInput.SetFocus();
		return;
	}
	//CEBAccountInfo::pointer pMyAccount = thePopClient.GetMyAccountInfo();
	//BOOST_ASSERT (pMyAccount.get() != 0);
	//CString sWindowText;
	//sWindowText.Format(_T("%s\r\n\t%s\r\n"), pMyAccount->GetAccount().c_str(), sInputText);
	//m_richSelf.SetSel(-1, -1);
	//m_richSelf.ReplaceSel(sWindowText);
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
	m_richInput.SetFocus();
	// Ctrl+V
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('V', 0, 0, 0);
	keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
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
void CDlgChatInput::SendFile(const char* lpszFilePath,bool bDropFile)
{
	if (bDropFile)
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
		CString sToAccount;
		//const bool bIsDepDialog = m_pCallInfo.m_sGroupCode>0;
		const int nCurSel = m_comboSendTo.GetCurSel();
		if (nCurSel >= 0)
		{
			const CTreeItemInfo * pTreeItemInfo = (CTreeItemInfo*)m_comboSendTo.GetItemData(nCurSel);
			if (pTreeItemInfo==NULL) return;
			sToAccount = pTreeItemInfo->m_sAccount.c_str();
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
	//if (m_pDlgUserList)
	//{
	//	m_pDlgUserList->OnUserEmpInfo(pMemberInfo);
	//}
}
#endif

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


void CDlgChatInput::AddLineString(eb::bigint nMsgId,const CString& sText, int nAlignmentFormat)
{
	m_pMrFrameInterface->AddLine(nMsgId);
	m_pMrFrameInterface->SetAlignmentFormat(nAlignmentFormat);
	m_pMrFrameInterface->WriteString((LPCTSTR)sText,theDefaultChatSystemColor);
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
void CDlgChatInput::OnSentFile(const CCrFileInfo * pCrFileInfo)
{
	//if (pCrFileInfo->m_sReceiveAccount.empty())	// 错误
	//	return;
	//if (pCrFileInfo->m_sReceiveAccount==theEBAppClient.EB_GetLogonUserId())
		ProcessFile(false,pCrFileInfo);
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
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M:%S");

	m_pMrFrameInterface->AddLine(pCrFileInfo->MsgId);
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theApp.GetBgColor3());
	//if (bReceive)
	//{
	//	m_pMrFrameInterface->SetFrameBackGroundColor(183,253,159);
	//}else
	//{
	//	m_pMrFrameInterface->SetFrameBackGroundColor(211,211,211);
	//}

	USES_CONVERSION;
	const std::wstring sImagePath = pCrFileInfo->FileName.GetBSTR();
	const tstring sFilePath = libEbc::GetFileName(W2A_ACP(sImagePath.c_str()));
	const int nWavTimeLength = GetTimeLength(sFilePath.c_str());
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
			m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, pCrFileInfo->FileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
			//m_pMrFrameInterface->WriteUrl(sFileName.c_str(), pCrFileInfo->FileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
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
void CDlgChatInput::ProcessFile(bool bReceive,const CCrFileInfo * pCrFileInfo)
{
	time_t tMsgTime = time(0);
	if (!pCrFileInfo->m_sFileTime.empty())
	{
		libEbc::ChangeTime(pCrFileInfo->m_sFileTime.c_str(),tMsgTime);
	}
	WriteMsgDate(tMsgTime);

	CString sWindowText;
	m_pMrFrameInterface->AddLine(0);
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	if (bReceive)
	{
		sWindowText.Format(_T("已经接收文件"));
	}else if (pCrFileInfo->m_sReceiveAccount==theEBAppClient.EB_GetLogonUserId())
	{
		if (pCrFileInfo->m_bOffFile)
			sWindowText.Format(_T("成功发送离线文件"));
		else
			sWindowText.Format(_T("成功发送文件"));
	}else
	{
		if (this->m_pCallInfo.m_sGroupCode==0)
			sWindowText.Format(_T("对方已经接收文件"));
		else
			sWindowText.Format(_T("%lld已经接收"),pCrFileInfo->m_sReceiveAccount);
	}
	m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,theDefaultChatSystemColor);
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M:%S");

	m_pMrFrameInterface->AddLine(pCrFileInfo->m_nMsgId);
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theApp.GetBgColor3());
	const int nWavTimeLength = GetTimeLength(pCrFileInfo->m_sFileName.c_str());
	if (nWavTimeLength > 0)
	{
		// 是WAV语音文件
		const tstring sFileName = libEbc::GetFileName((LPCTSTR)pCrFileInfo->m_sFileName.c_str());
		m_pMrFrameInterface->WriteWav(sFileName.c_str(),pCrFileInfo->m_sFileName.c_str(),EB_MR_CTRL_TYPE_LCLICK_OPEN);
		//CString sText;
		//sText.Format(_T("%s（%d秒）"),sFileName.c_str(),nWavTimeLength);
		//m_pMrFrameInterface->WriteWav((LPCTSTR)sText,pCrFileInfo->m_sFileName.c_str());
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
			CString sTemp;
			sTemp.Format(_T("#CTRL:%d::#%s"),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,pCrFileInfo->m_sFileName.c_str());
			m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
		}else
		{
			// SHGetFileInfo获取只能获取到32*32的图标。用SHDefExtractIcon能获取到48*48的图标，
			//HICON hIcon = ::ExtractIcon(AfxGetInstanceHandle(),_T("ebc.exe"),0);
			WriteFileHICON(pCrFileInfo->m_sFileName.c_str());
			const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);
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
			m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, pCrFileInfo->m_sFileName.c_str(),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
			//m_pMrFrameInterface->WriteUrl(sFileName.c_str(), pCrFileInfo->m_sFileName.c_str(),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
		}
		m_pMrFrameInterface->WriteLine();
		m_pMrFrameInterface->WriteSpace(11);
		if (nFileSize>=0)
		{
			m_pMrFrameInterface->WriteOpenFile(L"打开",pCrFileInfo->m_sFileName.c_str());
			m_pMrFrameInterface->WriteSpace(2);
		}
		m_pMrFrameInterface->WriteOpenDir(L"打开文件夹",pCrFileInfo->m_sFileName.c_str());
	}
	const bool hIsScrollEnd = (!bReceive || m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
	m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
	if (hIsScrollEnd)
		m_pMrFrameInterface->ScrollToPos(-1);
}
#endif

int CDlgChatInput::GetTimeLength(const char* lpszWavFilePath)
//获取声音文件数据的函数，pString参数指向要打开的声音文件；
{
	HMMIO file;//定义HMMIO文件句柄；
	file=mmioOpen((char*)lpszWavFilePath,NULL,MMIO_READ);//以读写模式打开所给的WAVE文件；
	if(file==NULL)
	{
		TRACE("WAVE文件打开失败！");
		return -1;
	}

	char style[4];//定义一个四字节的数据，用来存放文件的类型；
	mmioSeek(file,8,SEEK_SET);//定位到WAVE文件的类型位置
	mmioRead(file,style,4);
	if(style[0]!='W'||style[1]!='A'||style[2]!='V'||style[3]!='E')//判断该文件是否为"WAVE"文件格式
	{
		TRACE("该文件不是WAVE格式的文件！");
		::mmioClose(file, 0);
		return -1;
	}

	PCMWAVEFORMAT format; //定义PCMWAVEFORMAT结构对象，用来判断WAVE文件格式；
	mmioSeek(file,20,SEEK_SET);
	//对打开的文件进行定位，此时指向WAVE文件的PCMWAVEFORMAT结构的数据；
	mmioRead(file,(char*)&format,sizeof(PCMWAVEFORMAT));//获取该结构的数据；

	//获取WAVE文件的声音数据的大小；
	mmioSeek(file,42,SEEK_SET);
	unsigned long size = 0;
	mmioRead(file,(char*)&size,4);

	//计算文件时长
	int timeLength;
	timeLength = size/format.wf.nAvgBytesPerSec;

	::mmioClose(file, 0);
	return timeLength;
}

void CDlgChatInput::WriteFileHICON(const char* lpszFilePath)
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
		m_pMrFrameInterface->WriteHICON((ULONG*)sfi.hIcon,lpszFilePath,EB_MR_CTRL_TYPE_LCLICK_OPEN);
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
//	const std::string sUTF8HtmlText = str_convert(sHtmlText,CP_ACP,CP_UTF8);
//	if (bIsDepDialog && nOutType == OT_PUBLIC && m_pwebOutWndPubc)
//	{
//		m_pwebOutWndPubc->WriteBlock(sHtmlText, FALSE, bAddTimestamp);
//	}else if (m_pwebOutWndSelf)
//	{
//		m_pwebOutWndSelf->WriteBlock(sHtmlText, FALSE, bAddTimestamp);
//	}
//}

void CDlgChatInput::OnUserEnter(eb::bigint sFromAccount, const tstring& sFromInfo)
{
	if (!this->GetDlgItem(IDC_BUTTON_SEND)->IsWindowEnabled())
	{
		this->GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
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
	if (theApp.IsLogonVisitor() ||
		m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_VISITOR)	// 游客不能被动邀请
	{
		// 不是公司内部员工，需要重新呼叫才能通讯
		AddLineString(0,_T("对方已经退出本次会话！"));
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
void CDlgChatInput::OnSendRich(const CCrRichInfo* pCrMsgInfo,EB_STATE_CODE nState,CString* sOutFirstMsg)
{
	ProcessMsg(false,pCrMsgInfo,sOutFirstMsg,nState);
}
#endif
void CDlgChatInput::OnAlertingCall(void)
{
	AddLineString(0,_T("对方已经退出本次会话，正在重新邀请，请稍候...！"));
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
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M:%S");

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
	m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theApp.GetBgColor3());
	//if (bReceive)
	//{
	//	m_pMrFrameInterface->SetFrameBackGroundColor(183,253,159);
	//}else
	//{
	//	m_pMrFrameInterface->SetFrameBackGroundColor(211,211,211);
	//}

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
			m_pMrFrameInterface->WriteString(libEbc::UTF82ACP(pMsgItem->GetText().c_str()).c_str(),theDefaultChatTextColor);

			if (theApp.GetSaveConversationLocal() && !theApp.IsLogonVisitor())
			{
				CEBString sText(pMsgItem->GetText());
				CSqliteCdbc::escape_string_in(sText);
				while (!sText.empty())
				{
					const std::string sSaveText = sText.size()>theMaxDatabaseTextSize?sText.substr(0,theMaxDatabaseTextSize):sText;
					sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_text) ")\
						_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s')"),
						sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str()),
						sSaveDbToAccount,libEbc::ACP2UTF8(sToName.c_str()).c_str(),int(pCrMsgInfo->Private?1:0),(int)MRT_TEXT,sSaveText.c_str());
					theApp.m_pBoUsers->execute(sSql);
					sText = sText.substr(sSaveText.size());
				}
			}
		}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_RESOURCE)
		{
			const std::string sResourceInfo = libEbc::UTF82ACP(pMsgItem->GetText().c_str());
			std::vector<std::string> pList;
			if (libEbc::ParseString(sResourceInfo.c_str(),";",pList)>=3)
			{
				const std::string sResourceId = pList[0];
				const std::string sDescription = pList.size()>3?pList[3]:"";
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
					sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name) ")\
						_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s')"),
						sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),
						sSaveDbToAccount,libEbc::ACP2UTF8(sToName.c_str()).c_str(),pCrMsgInfo->Private?1:0,MRT_JPG,libEbc::ACP2UTF8(sImageFileName).c_str());
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
				m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, (LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
				//m_pMrFrameInterface->WriteUrl(sFileName.c_str(), (LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
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
				sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name) ")\
					_("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s')"),
					sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),
					sSaveDbToAccount,libEbc::ACP2UTF8(sToName.c_str()).c_str(),pCrMsgInfo->Private?1:0,nRecordType,libEbc::ACP2UTF8(sObjectFileName).c_str());
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
void CDlgChatInput::ProcessMsg(bool bReceive,const CCrRichInfo* pCrMsgInfo,CString* sOutFirstMsg,EB_STATE_CODE nState)
{
	if (pCrMsgInfo == NULL)
	{
		return;
	}
	if (nState==EB_STATE_MAX_CAPACITY_ERROR)
	{
		AddLineString(0,_T("超过最大消息长度！"));
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

	const eb::bigint sSender = pCrMsgInfo->m_sSendFrom;
	const bool bIsDepDialog = m_pCallInfo.m_sGroupCode>0;
	CEBString sFromName;
	CEBString sToName;// = pCrMsgInfo->m_sSendTo;
	if (bReceive)
	{
		if (m_pCallInfo.m_sGroupCode==0)
		{
			if (m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_VISITOR)
				sFromName = m_pFromAccountInfo.GetAccount();
			else
				sFromName = m_pFromAccountInfo.m_pFromCardInfo.m_sName;
			sToName = theEBAppClient.EB_GetUserName();
		}else
		{
			theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,sSender,sFromName);
			if (pCrMsgInfo->m_sSendTo>0)
				theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendTo,sToName);
		}
	}else
	{
		if (m_pCallInfo.m_sGroupCode==0)
			sFromName = theEBAppClient.EB_GetUserName();
		else
		{
			theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,theApp.GetLogonUserId(),sFromName);
		}
		if (pCrMsgInfo->m_sSendTo>0)
			theEBAppClient.EB_GetMemberNameByUserId(m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendTo,sToName);
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
		sWindowTextTemp.Format(_T("%s#CTRL:1:%d:%lld#%s%s："),sPrivateText,(int)EB_COMMAND_CALL_USER,sSender,sFromName.c_str(),sToText);
		m_pMrFrameInterface->WriteString((LPCTSTR)sWindowTextTemp,crTextColor);
	}else
	{
		m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,crTextColor);
	}
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M:%S");
	if (sOutFirstMsg!=NULL)
		sOutFirstMsg->Format("%s<br/>",libEbc::ACP2UTF8(sWindowText).c_str());

	eb::bigint sSaveDbToAccount = pCrMsgInfo->m_sSendTo;
	if (m_pCallInfo.m_sGroupCode==0)
		sSaveDbToAccount = m_pFromAccountInfo.GetUserId();
	CString sSql;
	//int ret = 0;
	m_pMrFrameInterface->AddLine(pRichMsg->GetMsgId());
	m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theApp.GetBgColor3());

	int nOutMsgLength = 0;
	const std::vector<EB_ChatRoomMsgItem*>& pRichMsgList = pRichMsg->GetList();
	for (size_t i=0; i<pRichMsgList.size(); i++)
	{
		const EB_ChatRoomMsgItem* pMsgItem = pRichMsgList[i];
		if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_TEXT)
		{
			if (sOutFirstMsg!=NULL && nOutMsgLength<const_max_length)
			{
				const CString sTemp(pMsgItem->GetText().substr(0,const_max_length).c_str());
				nOutMsgLength += sTemp.GetLength();
				sOutFirstMsg->Append(sTemp);
			}
			m_pMrFrameInterface->WriteString(libEbc::UTF82ACP(pMsgItem->GetText().c_str()).c_str(),theDefaultChatTextColor);

			if (theApp.GetSaveConversationLocal() && !theEBAppClient.EB_IsLogonVisitor())
			{
				std::string sText(pMsgItem->GetText());
				CSqliteCdbc::escape_string_in(sText);
				while (!sText.empty())
				{
					const std::string sSaveText = sText.size()>theMaxDatabaseTextSize?sText.substr(0,theMaxDatabaseTextSize):sText;
					sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_text) ")\
						_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s')"),
						sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),
						sSaveDbToAccount,libEbc::ACP2UTF8(sToName.c_str()).c_str(),pCrMsgInfo->m_bPrivate?1:0,MRT_TEXT,sSaveText.c_str());
					theApp.m_pBoUsers->execute(sSql);
					sText = sText.substr(sSaveText.size());
				}
			}
		}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_RESOURCE)
		{
			const std::string sResourceInfo = libEbc::UTF82ACP(pMsgItem->GetText().c_str());
			std::vector<std::string> pList;
			if (libEbc::ParseString(sResourceInfo.c_str(),";",pList)>=3)
			{
				const std::string sResourceId = pList[0];
				const std::string sDescription = pList.size()>3?pList[3]:"";
				CString sImageFileName;
				sImageFileName.Format(_T("%s\\%s"), theEBAppClient.EB_GetResourcePath().c_str(),sResourceId.c_str());
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
				if (theApp.GetSaveConversationLocal() && !theEBAppClient.EB_IsLogonVisitor())
				{
					sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name) ")\
						_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s')"),
						sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),
						sSaveDbToAccount,libEbc::ACP2UTF8(sToName.c_str()).c_str(),pCrMsgInfo->m_bPrivate?1:0,MRT_JPG,libEbc::ACP2UTF8(sImageFileName).c_str());
					theApp.m_pBoUsers->execute(sSql);
				}
			}
		}else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT)
		{
			CString sObjectFileName;
			EB_RICH_SUB_TYPE nSubType = (EB_RICH_SUB_TYPE)pCrMsgInfo->m_pRichMsg->GetSubType();
			MSG_RECORD_TYPE nRecordType = MRT_JPG;
			if (bReceive || nState==EB_STATE_FORWARD_MSG)
			{
				static unsigned int static_index = 0;
				static_index++;
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
					fwrite(lpObjectData,1,dwDataSize,hFile);
					fclose(hFile);
				}
			}else
			{
				sObjectFileName = pMsgItem->GetText().c_str();
				if (nSubType == EB_RICH_SUB_TYPE_AUDIO)
					nRecordType = MRT_WAV;
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
				m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, (LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
				//m_pMrFrameInterface->WriteUrl(sFileName.c_str(), (LPCTSTR)sObjectFileName,EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
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
			if (theApp.GetSaveConversationLocal() && !theEBAppClient.EB_IsLogonVisitor())
			{
				sSql.Format(_T("INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name) ")\
					_T("VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s')"),
					sDBMsgTimeField,sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendFrom,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),
					sSaveDbToAccount,libEbc::ACP2UTF8(sToName.c_str()).c_str(),pCrMsgInfo->m_bPrivate?1:0,nRecordType,libEbc::ACP2UTF8(sObjectFileName).c_str());
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
#endif

#ifdef USES_EBCOM_TEST
void CDlgChatInput::OnReceiveRich(IEB_ChatRichInfo* pCrMsgInfo,CString* sOutFirstMsg)
{
	ProcessMsg(true,pCrMsgInfo,sOutFirstMsg);
}
#else
void CDlgChatInput::OnReceiveRich(const CCrRichInfo* pCrMsgInfo,CString* sOutFirstMsg)
{
	//BOOST_ASSERT (pRichMsg.get() != 0);
	ProcessMsg(true,pCrMsgInfo,sOutFirstMsg);
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
//	//		std::string sRichMsg(lpBuffer+ncharformatsize+7, nrichlen);
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
							std::string sResourceCmInfo;
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
	AFX_SWITCH_INSTANCED();
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
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_richInput.GetSafeHwnd())
	{
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
		const CString sDayOfWeek = theDayOfWeek[m_tLastMsgDayTime.GetDayOfWeek()-1];
		CString sText;
		sText.Format(_T("--------  %04d-%02d-%02d %s  --------"),m_tLastMsgDayTime.GetYear(),m_tLastMsgDayTime.GetMonth(),m_tLastMsgDayTime.GetDay(),sDayOfWeek);
		m_pMrFrameInterface->AddLine(0);
		m_pMrFrameInterface->SetAlignmentFormat(1);
		m_pMrFrameInterface->WriteString((LPCTSTR)sText,theDefaultChatSystemColor);
		//const bool hIsScrollEnd = (m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
		//m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
		//if (hIsScrollEnd)
		//	m_pMrFrameInterface->ScrollToPos(-1);
	}

}
void CDlgChatInput::WriteTitle(eb::bigint nMsgId,bool bPrivate,eb::bigint nFromUid,const tstring& sFromName,eb::bigint nToUid,const tstring& sToName,time_t tMsgTime)
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
	CString sWindowText;
	if (nFromUid!=theApp.GetLogonUserId() && m_pCallInfo.m_sGroupCode>0)
	{
		if (nToUid>0)
			sWindowText.Format(_T("%s#CTRL:1:%d:%lld#%s#CTRL:1:%d:%lld#%s"),sPrivateText,(int)EB_COMMAND_CALL_USER,nFromUid,sFromName.c_str(),(int)EB_COMMAND_CALL_USER,nToUid,sToText);
		else
			sWindowText.Format(_T("%s#CTRL:1:%d:%lld#%s%s"),sPrivateText,(int)EB_COMMAND_CALL_USER,nFromUid,sFromName.c_str(),sToText);
	}else
	{
		sWindowText.Format(_T("%s%s%s"),sPrivateText,sFromName.c_str(),sToText);
	}
	const COLORREF crTextColor = nFromUid==theApp.GetLogonUserId()?theDefaultChatTitleColor1:theDefaultChatTitleColor2;
	m_pMrFrameInterface->WriteString((LPCTSTR)sWindowText,crTextColor);
	m_pMrFrameInterface->WriteSpace(1);
	m_pMrFrameInterface->WriteTime(tMsgTime,"%H:%M:%S");

	m_pMrFrameInterface->AddLine(nMsgId);	// *** new line
	//m_pMrFrameInterface->SetAlignmentValue(40,60);
	m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
	m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
	if (nFromUid==theApp.GetLogonUserId())
	{
		m_pMrFrameInterface->SetAlignmentFormat(2);
		m_pMrFrameInterface->SetFrameBackGroundColor(theApp.GetBgColor3());
	}else
	{
		m_pMrFrameInterface->SetFrameBackGroundColor(theDefaultChatBackGroundColor2);
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
		sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text FROM msg_record_t ")\
			_T("WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) ORDER BY msg_time DESC LIMIT 12"),m_sAccount,m_sAccount);
	}else if (m_sGroupCode>0)
	{
		sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text FROM msg_record_t ")\
			_T("WHERE dep_code=%lld ORDER BY msg_time DESC LIMIT 12"),m_sGroupCode);
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
	int nCookie = 0;
	const mycp::bigint nRet = theApp.m_pBoUsers->select(sSql, nCookie);
	cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->last(nCookie);
	while (pRecord.get()!=NULL)
	{
		const tstring sMsgTime = pRecord->getVector()[0]->getStrValue();
		const eb::bigint sMsgId = pRecord->getVector()[1]->getBigIntValue();
		const tstring soffTime = pRecord->getVector()[2]->getStrValue();
		const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
		const tstring sFromName(libEbc::UTF82ACP(pRecord->getVector()[4]->getStrValue().c_str()));
		const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
		const tstring sToName(libEbc::UTF82ACP(pRecord->getVector()[6]->getStrValue().c_str()));
		const int nPrivate = pRecord->getVector()[7]->getIntValue();
		const int nMsgType = pRecord->getVector()[8]->getIntValue();
		const tstring sMsgName(libEbc::UTF82ACP(pRecord->getVector()[9]->getStrValue().c_str()));
		tstring sMsgText(libEbc::UTF82ACP(pRecord->getVector()[10]->getStrValue().c_str()));
		const unsigned int nMsgSize = sMsgText.size();

		// 前面未显示，并且当前文本不是最长文本，显示前面内容。
		if (!sToWriteString.empty() && nMsgSize!=theMaxDatabaseTextSize)
		{
			m_pMrFrameInterface->WriteString(sToWriteString.c_str(),theDefaultChatTextColor);
			sToWriteString.clear();
		}
		//// 兼容旧版本
		//if (sFromName.empty())
		//	sFromName = sFromAccount;
		//if (sToName.empty())
		//	sToName = sToAccount;
		time_t nMsgTime = 0;
		libEbc::ChangeTime(sMsgTime.c_str(),nMsgTime);
		WriteTitle(sMsgId,nPrivate==1,sFromAccount,sFromName,sToAccount,sToName,nMsgTime);
		CString sWindowText;
		if (MRT_UNKNOWN==nMsgType)
		{
			pRecord = theApp.m_pBoUsers->previous(nCookie);
			continue;
		}else if (MRT_TEXT==nMsgType)
		{
			CSqliteCdbc::escape_string_out(sMsgText);
			// 先临时保存，后面显示；处理分段保存长文本；
			if (sToWriteString.empty())
				sToWriteString = sMsgText;
			else
				sToWriteString = sMsgText+sToWriteString;
			//m_pMrFrameInterface->WriteString(sMsgText.c_str());
		}else if (MRT_JPG==nMsgType)
		{
			CString sTemp;
			sTemp.Format(_T("#CTRL:%d::#%s"),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,sMsgName.c_str());
			m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
		}else if (MRT_WAV==nMsgType)
		{
			//WriteFileHICON(sMsgName.c_str());
			m_pMrFrameInterface->WriteWav("播放语音",sMsgName.c_str(),EB_MR_CTRL_TYPE_LCLICK_OPEN);
		}else if (MRT_FILE==nMsgType)
		{
			bool bReceive = sFromAccount!=theApp.GetLogonUserId();
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
				CString sTemp;
				sTemp.Format(_T("#CTRL:%d::#%s"),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,sMsgText.c_str());
				m_pMrFrameInterface->WriteImage((LPCTSTR)sTemp,0,0);
			}else
			{
				WriteFileHICON(sMsgText.c_str());
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
				m_pMrFrameInterface->WriteUrl((LPCTSTR)sFileText, sMsgText.c_str(),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(60,60,60));
				//m_pMrFrameInterface->WriteUrl(sFileName.c_str(), sMsgText.c_str(),EB_MR_CTRL_TYPE_LDBLCLICK_OPEN,RGB(0,0,0));
			}
			m_pMrFrameInterface->WriteLine();
			m_pMrFrameInterface->WriteSpace(11);
			if (nFileSize >= 0)
			{
				m_pMrFrameInterface->WriteOpenFile(L"打开",sMsgText.c_str());
				m_pMrFrameInterface->WriteSpace(2);
			}
			m_pMrFrameInterface->WriteOpenDir(L"打开文件夹",sMsgText.c_str());
		}
		pRecord = theApp.m_pBoUsers->previous(nCookie);
	}
	theApp.m_pBoUsers->reset(nCookie);

	if (!sToWriteString.empty())
	{
		m_pMrFrameInterface->WriteString(sToWriteString.c_str(),theDefaultChatTextColor);
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
	}
}

void CDlgChatInput::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_DELETE_SELECT_MSG==nIDEvent)
	{
		KillTimer(nIDEvent);
		if (!m_pDeletMsgId.empty())
		{
			CString sText;
			sText.Format(_T("你确定删除选中的[%d]条聊天记录吗？"), m_pDeletMsgId.size());
			if (CDlgMessageBox::EbDoModal(this,"删除聊天记录：",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
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
	}else if (TIMERID_SHOW_MRFRAME==nIDEvent)
	{
		KillTimer(TIMERID_SHOW_MRFRAME);
		this->m_pMrFrameControl.Invalidate();
	}else if (TIMERID_SCROLLTOEND==nIDEvent)
	{
		KillTimer(TIMERID_SCROLLTOEND);
		m_pMrFrameInterface->ScrollToPos(-1);
	}else if (TIMERID_TEST_TEST==nIDEvent)
	{
		static int theSendIndex = 0;
		theSendIndex++;
		CString sText;
		m_richInput.GetWindowText(sText);
		if (sText.IsEmpty())
		{
			sText = _T("测试数据");
		}
		sText = sText.Left(100);
		CString sSendText;
		sSendText.Format(_T("%s(%d)"),sText,theSendIndex);
		EB_ChatRoomRichMsg pRichMsg;
		pRichMsg.AddText(libEbc::ACP2UTF8(sSendText).c_str());
		theEBAppClient.EB_SendRich(m_pCallInfo.GetCallId(),&pRichMsg,0,m_checkPrivate.GetCheck()?true:false);

		if (theSendIndex>=100)
		{
			theSendIndex = 0;
			KillTimer(nIDEvent);
		}
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

void CDlgChatInput::OnBnClickedButtonEbsc()
{
	this->GetParent()->PostMessage(EB_MSG_EBSC, 0, 0);
	m_richInput.SetFocus();
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
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,libEbc::ACP2UTF8(sSelectString.c_str()));
	}
}
void CDlgChatInput::Fire_onItemLBtnClick(ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId)
{
	switch (nItemData)
	{
	case EB_COMMAND_CALL_USER:
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_CallAccount(sParamString.c_str(),0);
#else
			theEBAppClient.EB_CallAccount(sParamString.c_str());
#endif
		}break;
	default:
		{
			//AddLineString(0,sParamString);
		}break;
	}
}
//void CDlgChatInput::Fire_onItemLBtnDblClick(ULONG nItemData, const CEBString& sParamString, LONGLONG nSelectMsgId)
//{
//	switch (nItemData)
//	{
//	case EB_COMMAND_CALL_USER:
//		{
////#ifdef USES_EBCOM_TEST
////			theEBClientCore->EB_CallAccount(sParamString.c_str(),0);
////#else
////			theEBAppClient.EB_CallAccount(sParamString.c_str());
////#endif
//		}break;
//	default:
//		AddLineString(0,sParamString);
//		break;
//	}
//}

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
		this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false));
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
		//if (m_pCallInfo.m_sGroupCode==0)
		//{
		//	m_pSendTypeSwitchMenu.AppendMenu(MF_SEPARATOR);
		//	m_pSendTypeSwitchMenu.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SEND_100TEST,_T("发送100条测试消息"));
		//}
	}
	m_pSendTypeSwitchMenu.CheckMenuItem(EB_COMMAND_SEND_TYPE_RETURN,theApp.GetSendType()==0?MF_CHECKED:MF_UNCHECKED);
	m_pSendTypeSwitchMenu.CheckMenuItem(EB_COMMAND_SEND_TYPE_CTRL_RETURN,theApp.GetSendType()==1?MF_CHECKED:MF_UNCHECKED);

	CPoint point;
	GetCursorPos(&point);
	m_pSendTypeSwitchMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
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
void CDlgChatInput::OnCmdSend100Test(void)
{
	SetTimer(TIMERID_TEST_TEST,25,NULL);
	//AfxMessageBox(_T("abc"));
	//CString sText;
	//m_richInput.GetWindowText(sText);
	//if (sText.IsEmpty())
	//{
	//	sText = _T("测试数据");
	//}
	//sText = sText.Left(100);
	//CString sSendText;
	//for (int i=i; i<=100; i++)
	//{
	//	sSendText.Format(_T("%s(%d)"),sText,i);
	//	EB_ChatRoomRichMsg pRichMsg;
	//	pRichMsg.AddText(libEbc::ACP2UTF8(sSendText).c_str());
	//	theEBAppClient.EB_SendRich(m_pCallInfo.GetCallId(),&pRichMsg,0,m_checkPrivate.GetCheck()?true:false);
	//	Sleep(10);
	//}
}

void CDlgChatInput::OnBnClickedButtonSendTest()
{
	SetTimer(TIMERID_TEST_TEST,10,NULL);
}

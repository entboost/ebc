// DlgFrameList.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgFrameList.h"
#include "DlgShrinkageBar.h"
#include "PanelSearch.h"
#ifdef USES_FRAMELIST_APPFRAME
#include "DlgAppFrame.h"
#endif

#pragma warning(disable:4018)

#define TIMERID_CHECK_CLOSE_DIALOG		0x128
#define TIMERID_SHRINKAGE_WIN			0x129
#define TIMERID_CHECK_MOUSE_POS			0x130
//#define TIMERID_REFRESH_APPFRAME		0x131
//#define TIMERID_ONE_SECOND				0x132
#define TIMERID_CHECK_SHRINKAGEBAR		0x133

//#define DEFAULT_DLG_WIDTH	710+120
//#define DEFAULT_DLG_HEIGHT	588

#define FRAME_BTN_ID_MIN		101
#define FRAME_BTN_ID_MAX		200
//#define FRAME_CLOSE_ID_MIN		301
//#define FRAME_CLOSE_ID_MAX		400
//#define FRAME_CLOSE_ID_OFFSET	100

const int theSearchLeftOffset = 3;
const int theSearchRightOffset = 24*3+5;
const int theSearchTopOffset = 6;

const int theDefaultWidth = 1180;
const int theDefaultHeight = 668;
const int nLimitDefaultWidth = 540;
const int nLimitDefaultHeight = 668;

// CDlgFrameList dialog

IMPLEMENT_DYNAMIC(CDlgFrameList, CEbDialogBase)

CDlgFrameList::CDlgFrameList(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgFrameList::IDD, pParent)
	, CFrameWndInfoProxy(this,false,this)
{
	m_bOpenOk = false;
	m_bShrinkageWin = false;
	//m_nCurrentFrameWndType = CFrameWndInfo::FRAME_WND_UNKNOWN;
	m_bShowedCloseTabCommand = false;

	m_pDlgShrinkageBar = NULL;
	memset(&m_rectAdjustOld,0,sizeof(m_rectAdjustOld));

	m_pPanelSearch = NULL;
#ifdef USES_FRAMELIST_APPFRAME
	m_pDlgAppFrame = NULL;
//#else
//	m_pWorkFrameCallback = NULL;
#endif
	m_pWorkFrameCallback = NULL;
}

CDlgFrameList::~CDlgFrameList()
{
}

void CDlgFrameList::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_editSearch);
	DDX_Control(pDX, IDC_BUTTON_SWITCH_LEFT, m_btnSwitchLeft);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BUTTON_MAX, m_btnMax);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
}

BEGIN_MESSAGE_MAP(CDlgFrameList, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_TIMER()
	//ON_WM_MOVING()
	ON_WM_SYSCOMMAND()
	//ON_MESSAGE(EB_COMMAND_REFRESH_URL, OnMsgRefreshUrl)
	ON_MESSAGE(EB_COMMAND_REFRESH_OR_STOP_URL, OnMsgRefreshOrStopUrl)
	ON_MESSAGE(EB_COMMAND_GO_BACK, OnMsgGoBack)
	ON_MESSAGE(EB_COMMAND_GO_FORWARD, OnMsgGoForward)
	ON_MESSAGE(EB_COMMAND_CHANGE_BROWSER_TYPE, OnMsgChangeBrowserType)
	ON_MESSAGE(EB_COMMAND_QUERY_CAN_SAVE_HISTORY, OnMsgQueryCanSaveHistory)
	ON_MESSAGE(EB_COMMAND_QUERY_BROWSER_TYPE, OnMsgQueryBrowserType)
	ON_MESSAGE(EB_COMMAND_SAVE_HISROTY, OnMsgSaveHistory)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MIN, OnFrameWndMin)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MAX, OnFrameWndMax)
	ON_MESSAGE(EB_COMMAND_RAME_WND_CLOSE, OnFrameWndClose)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MOVE_UP, OnMessageMoveUp)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MOVE_DOWN, OnMessageMoveDown)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MOVE_OFFSET, OnMessageMoveOffset)
	ON_MESSAGE(EB_COMMAND_RAME_WND_ADJUST_WIDTH, OnMessageAdjustWidth)
	ON_MESSAGE(EB_COMMAND_CHANGE_APP_URL, OnMsgChangeAppUrl)
	ON_MESSAGE(EB_COMMAND_SHOW_REFRESH_OR_STOP, OnMsgShowRefreshOrStop)

	ON_COMMAND(EB_COMMAND_SHOWHEAD_ONLY, OnShowHeadOnly)
	ON_COMMAND(EB_COMMAND_OPEN_WORKFRAME, OnOpenWorkFrame)
	ON_COMMAND(EB_COMMAND_CLOSE_TAB, OnCloseTab)
	
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND_RANGE(FRAME_BTN_ID_MIN, FRAME_BTN_ID_MAX, OnFrameBtnClicked)
	//ON_COMMAND_RANGE(FRAME_CLOSE_ID_MIN, FRAME_CLOSE_ID_MAX, OnFrameCloseClicked)
	//ON_COMMAND_RANGE(FRAME_BTN_ID_MIN+FRAME_CLOSE_ID_OFFSET, FRAME_BTN_ID_MAX+FRAME_CLOSE_ID_OFFSET, OnFrameCloseClicked)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &OnEnChangeEditSearch)
	ON_MESSAGE(EB_COMMAND_RETURN_MAINFRAME, OnMessageReturnMainFrame)
	ON_MESSAGE(EB_COMMAND_RAME_WND_COUNT, OnFrameWndCount)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_LEFT, &CDlgFrameList::OnBnClickedButtonSwitchLeft)
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgFrameList::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_MAX, &CDlgFrameList::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgFrameList::OnBnClickedButtonClose)
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CDlgFrameList message handlers

BOOL CDlgFrameList::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);

	this->SetSplitterBorder();
	this->DisableMoveBottom(TRUE);
	if (GetPrivateProfileInt(_T("default"),_T("frame-list-show-left"),1,theApp.GetUserSettingIniFile())==0)
	{
		CFrameWndInfoProxy::SetShowHideLeft(false);
	}
	this->EnableToolTips();
	this->SetToolTipText(IDC_BUTTON_SWITCH_LEFT,CFrameWndInfoProxy::GetShowedLeft()?_T("收起面板"):_T("展开面板"));
	//this->SetToolTipText(IDC_EDIT_SEARCH,_T("搜索：用户、帐号、联系人；回车开始聊天"));

	//this->MoveWindow(0, 0, DEFAULT_DLG_WIDTH, DEFAULT_DLG_HEIGHT);
	this->SetWindowText(_T("会话窗口"));

	// **暂时不支持搜索；
	m_editSearch.ShowWindow(SW_HIDE);
	//m_editSearch.SetPromptText(_T("搜索：用户、帐号、联系人；回车开始聊天"));
	//m_editSearch.SetRectangleColor(theApp.GetHotColor2(),RGB(255,255,255));

#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	const bool bForceShowBlack = nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?true:false;
#else
	const bool bForceShowBlack = false;
#endif
	m_btnMin.SetAutoSize(false);
	m_btnMin.SetDrawToolButtonPic(1,((bForceShowBlack||theApp.GetHideMainFrame())?theDefaultFlatBlackText2Color:theDefaultBtnWhiteColor),theDefaultBtnWhiteColor,-1,-1,2);
	m_btnMin.SetDrawPanelRgn(false);
	m_btnMin.SetToolTipText(_T("最小化"));
	m_btnMax.SetAutoSize(false);
	m_btnMax.SetDrawToolButtonPic(2,((bForceShowBlack||theApp.GetHideMainFrame())?theDefaultFlatBlackText2Color:theDefaultBtnWhiteColor),theDefaultBtnWhiteColor,-1,-1,2);
	m_btnMax.SetDrawPanelRgn(false);
	m_btnMax.SetToolTipText(_T("最大化"));
	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,((bForceShowBlack||theApp.GetHideMainFrame())?theDefaultFlatBlackText2Color:theDefaultBtnWhiteColor),theDefaultBtnWhiteColor,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnMin.ShowWindow(SW_HIDE);
	m_btnMax.ShowWindow(SW_HIDE);
	m_btnClose.ShowWindow(SW_HIDE);

	m_pPanelSearch = new CPanelSearch(this);
	m_pPanelSearch->Create(CPanelSearch::IDD,this);
	m_pPanelSearch->SetSearchEditShow(true);
#ifdef USES_FRAMELIST_APPFRAME
	m_pDlgAppFrame = new CDlgAppFrame(this);
	m_pDlgAppFrame->Create(CDlgAppFrame::IDD,this);
	m_pDlgAppFrame->SetCircle(true);
#endif
	m_btnSwitchLeft.SetWindowText(_T(""));
	m_btnSwitchLeft.SetDrawTrianglePic((CFrameWndInfoProxy::GetShowedLeft()?3:4),theDefaultFlatLineColor,theDefaultFlatBlackText2Color,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	m_btnSwitchLeft.ShowWindow(SW_HIDE);
	//m_btnSwitchLeft.ShowWindow(SW_SHOW);

	const int nScreenWidth = theApp.GetScreenWidth();
	const int nScreenHeight = theApp.GetScreenHeight();
	UINT nX = max(0,GetPrivateProfileInt(_T("default"),_T("frame-list-x"),(nScreenWidth-theDefaultWidth)/2,theApp.GetUserSettingIniFile()));
	UINT nY = max(0,GetPrivateProfileInt(_T("default"),_T("frame-list-y"),(nScreenHeight-theDefaultHeight)/2,theApp.GetUserSettingIniFile()));
	UINT nW = max(nLimitDefaultWidth,GetPrivateProfileInt(_T("default"),_T("frame-list-w"),theDefaultWidth,theApp.GetUserSettingIniFile()));
	UINT nH = max(nLimitDefaultHeight,GetPrivateProfileInt(_T("default"),_T("frame-list-h"),theDefaultHeight,theApp.GetUserSettingIniFile()));
	//m_nShrinkageWin = GetPrivateProfileInt(_T("default"),_T("shrinkage-win"),0,theApp.GetUserSettingIniFile());
	nX = min((nScreenWidth-nLimitDefaultWidth),nX);
	nY = min((nScreenHeight-nLimitDefaultHeight),nY);
	nW = min(nScreenWidth,nW);
	nH = min(nScreenHeight,nH);
	MoveWindow(nX, nY, nW, nH);
	if (nW>=nScreenWidth && nH>=nScreenHeight)
	{
		this->SetToolTipText(IDC_BUTTON_MAX2,_T("向下还原"));
		m_btnMax.SetDrawToolButtonPic(3,theDefaultFlatBlackText2Color,theDefaultBtnWhiteColor,-1,-1,2);
	}

	m_bOpenOk = true;
	SetCircle();

	SetTimer(TIMERID_CHECK_MOUSE_POS,50,NULL);
	SetTimer(TIMERID_CHECK_CLOSE_DIALOG,100,NULL);
	//SetTimer(TIMERID_ONE_SECOND,500,NULL);
	CFrameWndInfoProxy::SetChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFrameList::SetCtrlColor(bool bInvalidate)
{
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=NULL)
		m_pDlgAppFrame->SetCtrlColor();
#endif

#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT || theApp.GetHideMainFrame())
#else
	if (theApp.GetHideMainFrame())
#endif
	{
		m_btnMin.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetHotColor(),theApp.GetPreColor());
		m_btnMax.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetHotColor(),theApp.GetPreColor());
		m_btnClose.SetDrawPanel(true,theDefaultFlatBgColor,theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	//}else
	//{
	//	m_btnMin.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	//	m_btnMax.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	//	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	}

	m_editSearch.SetRectangleColor(theApp.GetPreColor(),theDefaultBtnWhiteColor);
	CFrameWndInfoProxy::SetCtrlColor(bInvalidate);
	if (bInvalidate)
		this->Invalidate();
}

#ifdef USES_FRAMELIST_APPFRAME
void CDlgFrameList::OnLogonSuccess(void)
{
	if (this->m_pDlgAppFrame!=NULL)
	{
		this->m_pDlgAppFrame->OnLogonSuccess();
	}
}
void CDlgFrameList::OnOffline(int nServerState)
{
	if (this->m_pDlgAppFrame!=NULL)
	{
		this->m_pDlgAppFrame->OnOffline(nServerState);
	}
}
#endif

void CDlgFrameList::CheckUnreadMsgTip(bool bShowTip)
{
	if (bShowTip && m_bShrinkageWin && !this->IsWindowVisible() && m_pDlgShrinkageBar!=NULL && m_pDlgShrinkageBar->IsWindowVisible())
		m_pDlgShrinkageBar->SetBgColor(RGB(255,128,128));
	else if (!bShowTip && m_pDlgShrinkageBar!=NULL)
		m_pDlgShrinkageBar->SetBgColor(-1);
}
void CDlgFrameList::ShowFrameList(bool bShow)
{
	if (bShow)
	{
		if (this->IsIconic())
			this->ShowWindow(SW_RESTORE);
		else
			this->ShowWindow(SW_SHOW);
		if (!m_bShrinkageWin)
		{
			CRect rect;
			this->GetWindowRect(&rect);
			if (CheckAutoHide(rect.left, rect.top, true))
			{
				m_pDlgShrinkageBar->ShowWindow(SW_SHOW);
				this->ShowWindow(SW_SHOW);
			}
		}
		KillTimer(TIMERID_CHECK_SHRINKAGEBAR);
		SetTimer(TIMERID_CHECK_SHRINKAGEBAR,200,NULL);
	}else
	{
		if (m_bShrinkageWin)
		{
			KillTimer(TIMERID_SHRINKAGE_WIN);
			m_bShrinkageWin = false;
			m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
			SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
		this->ShowWindow(SW_HIDE);
#ifdef USES_FRAMELIST_APPFRAME
		if (m_pCurrentFrameWndInfo.get()!=NULL &&
			(m_pList.empty() || (m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME && m_pDlgAppFrame->IsEmpty())))
		{
			m_pCurrentFrameWndInfo.reset();
		}
#endif
	}
}

//void CDlgFrameList::SetBtnMax(bool bMax)
//{
//	if (m_btnMax.GetSafeHwnd()==NULL || this->GetSafeHwnd()==NULL)
//		return;
//#ifdef USES_SUPPORT_UI_STYLE
//	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
//	const bool bForceShowBlack = nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?true:false;
//#else
//	const bool bForceShowBlack = false;
//#endif
//	if (bMax)
//	{
//		this->SetToolTipText(IDC_BUTTON_MAX,_T("最大化"));
//		m_btnMax.SetDrawToolButtonPic(2,((bForceShowBlack||theApp.GetHideMainFrame())?theDefaultFlatBlackText2Color:theDefaultBtnWhiteColor),theDefaultBtnWhiteColor,-1,-1,2);
//	}else
//	{
//		this->SetToolTipText(IDC_BUTTON_MAX,_T("向下还原"));
//		m_btnMax.SetDrawToolButtonPic(3,((bForceShowBlack||theApp.GetHideMainFrame())?theDefaultFlatBlackText2Color:theDefaultBtnWhiteColor),theDefaultBtnWhiteColor,-1,-1,2);
//	}
//}

void CDlgFrameList::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgFrameList::OnCancel()
{
	//OnBnClickedButtonClose();	// 支持实现，鼠标移动工作栏，点击关闭，最小化窗口
	//CEbDialogBase::OnCancel();
}

void CDlgFrameList::OnDestroy()
{
	CEbDialogBase::OnDestroy();

	m_menuTray.DestroyMenu();
	m_pList.clear();
	m_pHideList.clear();
	m_pCloseList.clear();

	if (m_pDlgShrinkageBar)
	{
		m_pDlgShrinkageBar->DestroyWindow();
		delete m_pDlgShrinkageBar;
		m_pDlgShrinkageBar = NULL;
	}
	if (m_pPanelSearch!=0)
	{
		m_pPanelSearch->DestroyWindow();
		delete m_pPanelSearch;
		m_pPanelSearch = NULL;
	}
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=NULL)
	{
		m_pDlgAppFrame->DestroyWindow();
		delete m_pDlgAppFrame;
		m_pDlgAppFrame = NULL;
	}
#endif

}

void CDlgFrameList::MoveSize(int cx, int cy)
{
	if (m_pPanelSearch!=NULL && m_pPanelSearch->GetSafeHwnd()!=NULL && m_pPanelSearch->IsWindowVisible())
	{
		const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left2;
		m_pPanelSearch->MoveWindow(nBorderLeft+theSearchLeftOffset,const_border_size+theSearchTopOffset,cx-nBorderLeft-const_border_size-theSearchLeftOffset-theSearchRightOffset,theSearchToolbarHeight-theSearchTopOffset-1);
	}

	{
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		const int nBorderLeft = this->GetShowedLeft()?const_border_left:(nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?const_hide_border_left2:const_hide_border_left1);
#else
		const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left1;
#endif
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CFrameWndInfo::pointer& pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)// && m_pWorkFrameCallback!=NULL)
			{
#ifdef USES_FRAMELIST_APPFRAME
				//if (m_pDlgAppFrame!=NULL)
				//{
				//	const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left2;
				//	m_pDlgAppFrame->MoveWindow(nBorderLeft,const_border_size+theSearchToolbarHeight,cx-nBorderLeft-const_border_size,cy-const_border_size*2-theSearchToolbarHeight);
				//}
#else
				m_pWorkFrameCallback->OnFrameSize(pFrameWndInfo, cx, cy);
#endif
			}else if (pFrameWndInfo->GetDialog().get()!=NULL)
			{
				pFrameWndInfo->GetDialog()->MoveWindow(nBorderLeft,const_border_size,cx-nBorderLeft-const_border_size,cy-const_border_size*2);
			}
		}
	}

	if (m_bOpenOk && !this->IsIconic() && cx>0 && cy>0)
	{
		char lpszBuffer[24];
		CRect rect;
		this->GetWindowRect(&rect);
		const int x = rect.left;
		const int y = rect.top;
		if (GetPrivateProfileInt(_T("default"),_T("frame-list-x"),0,theApp.GetUserSettingIniFile())!=x)
		{
			sprintf(lpszBuffer,"%d",x);
			WritePrivateProfileString(_T("default"),_T("frame-list-x"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
		if (GetPrivateProfileInt(_T("default"),_T("frame-list-y"),0,theApp.GetUserSettingIniFile())!=y)
		{
			sprintf(lpszBuffer,"%d",y);
			WritePrivateProfileString(_T("default"),_T("frame-list-y"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
		if (GetPrivateProfileInt(_T("default"),_T("frame-list-w"),theDefaultWidth,theApp.GetUserSettingIniFile())!=cx)
		{
			sprintf(lpszBuffer,"%d",cx);
			WritePrivateProfileString(_T("default"),_T("frame-list-w"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
		if (GetPrivateProfileInt(_T("default"),_T("frame-list-h"),theDefaultHeight,theApp.GetUserSettingIniFile())!=cy)
		{
			sprintf(lpszBuffer,"%d",cy);
			WritePrivateProfileString(_T("default"),_T("frame-list-h"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
	}

	if (m_bShrinkageWin)
	{
		this->GetWindowRect(&m_rectWin);

		CRect rectWin;
		m_pDlgShrinkageBar->GetWindowRect(&rectWin);
		if (rectWin.top==0 && rectWin.bottom==2)
		{
			// top
			rectWin.left = m_rectWin.left;
			rectWin.right = m_rectWin.right;
		}else
		{
			// left & right
			rectWin.top = m_rectWin.top;
			rectWin.bottom = m_rectWin.bottom;
		}
		m_pDlgShrinkageBar->MoveWindow(&rectWin);
	}
	if (m_rectAdjustOld.Width()>0 && GetSplitterPressed()>0)
	{
		this->GetWindowRect(&m_rectAdjustOld);
	}

#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=NULL)
	{
		const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left2;
		m_pDlgAppFrame->MoveWindow(nBorderLeft,const_border_size+theSearchToolbarHeight,cx-nBorderLeft-const_border_size,cy-const_border_size*2-theSearchToolbarHeight);
	}
#endif	// USES_FRAMELIST_APPFRAME

}

void CDlgFrameList::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	if (cx==0 || cy==0) return;

	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-3;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMax.GetSafeHwnd()!=NULL)
		m_btnMax.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMin.GetSafeHwnd()!=NULL)
		m_btnMin.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	if (m_editSearch.GetSafeHwnd()!=NULL)
		m_editSearch.MoveWindow(15, 15, const_border_left-28, 22);

	if (m_btnSwitchLeft.GetSafeHwnd()!=NULL)
	{
		const int const_minbtn_width = 24;
		//CRect m_rectSwitchLeft;
		m_rectSwitchLeft.left = 1;
		m_rectSwitchLeft.right = m_rectSwitchLeft.left + const_minbtn_width;
		m_rectSwitchLeft.bottom = cy - 3;
		m_rectSwitchLeft.top = m_rectSwitchLeft.bottom - const_minbtn_width;
		m_btnSwitchLeft.MoveWindow(&m_rectSwitchLeft);
	}
	MoveSize(cx, cy);
#ifdef USES_FRAMELIST_APPFRAME
	if (nType==SIZE_RESTORED && m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME && m_pDlgAppFrame!=NULL && m_pDlgAppFrame->GetSafeHwnd()!=NULL)
	{
		m_pDlgAppFrame->ShowWindow(SW_SHOW);
		m_pDlgAppFrame->RefreshAppWnd();	// 解决Chrome还原窗口，背景显示灰色问题
	}else if (nType==SIZE_RESTORED && m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG && m_pCurrentFrameWndInfo->GetDialog().get()!=NULL)
	{
		m_pCurrentFrameWndInfo->GetDialog()->RefreshWeb();
	}

#endif

}

void CDlgFrameList::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CEbDialogBase::OnClose();
}

void CDlgFrameList::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文
	}
	else
	{
		//CEbDialogBase::OnPaint();
		DrawInfo();
	}

}

void CDlgFrameList::DrawInfo(void)
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);
	this->ClearBgDrawInfo();
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	const int nBorderLeft = this->GetShowedLeft()?const_border_left:(nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?const_hide_border_left2:const_hide_border_left1);
#else
	const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left1;
#endif
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,nBorderLeft));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,0,theDefaultFlatBgColor));
#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		this->AddBgDrawInfo(CEbBackDrawInfo(theSearchToolbarHeight,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
	}
#endif
	this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,nBorderLeft,1);
	//Gdiplus::Graphics graphics(memDC.m_hDC);

	//Gdiplus::Graphics graphics(memDC.m_hDC);

	//const int const_head_size = 24;
	//int nIndex = 0;
	//BoostReadLock rdLock(m_pList.mutex());
	//CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	//for (; pIter!=m_pList.end(); pIter++)
	//{
	//	CFrameWndInfo::pointer pFrameWndInfo = *pIter;

	//	Image * pImage = NULL;
	//	pImage = theApp.m_imageDefaultMember->Clone();
	//	Gdiplus::Rect destRect(7, 7, const_head_size, const_head_size);
	//	graphics.DrawImage(pImage, destRect);
	//}

}

bool CDlgFrameList::CheckAutoHide(int x, int y, bool bFromShowCheck)
{
	CPoint pCursorPos;
	GetCursorPos(&pCursorPos);

	int nNeedHideWin = 0;
	if ((pCursorPos.x<=0 && y>0) || (bFromShowCheck && m_bOpenOk && x==0 && y>0))
	//if (pCursorPos.x<=0 && y>0)
		nNeedHideWin = 1;		// left
	else if ((pCursorPos.y<=0 && y<=0) || (bFromShowCheck && m_bOpenOk && x>0 && y==0))
	//else if (pCursorPos.y<=0 && x>0 && y<=0)
		nNeedHideWin = 2;		// top
	else
	{
		CRect rect;
		this->GetWindowRect(&rect);
		const int nScreenWidth = theApp.GetScreenWidth();
		if (((pCursorPos.x+1)>=nScreenWidth && y>0) || (bFromShowCheck && m_bOpenOk && rect.right==nScreenWidth && y>0))
		//if ((pCursorPos.x+1)>=nScreenWidth && y>0)
			nNeedHideWin = 3;	// right
	}
	if (nNeedHideWin>0)
	{
		CRect rect;
		this->GetWindowRect(&rect);
		const int nWinWidth = rect.Width();

		if (m_pDlgShrinkageBar==NULL)
		{
			CWnd* pParent = GetDesktopWindow();
			m_pDlgShrinkageBar = new CDlgShrinkageBar(pParent);
			m_pDlgShrinkageBar->Create(CDlgShrinkageBar::IDD,pParent);
			m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		}
		m_rectWin = rect;
		bool bNeedReMoveWindow = false;
		if (nNeedHideWin==1)
		{
			// left
			bNeedReMoveWindow = m_rectWin.left!=0?true:false;
			m_rectWin.left = 0;
			m_rectWin.right = nWinWidth;
			rect.left = 0;
			rect.right = 2;
		}else if (nNeedHideWin==2)
		{
			// top
			rect.top = 0;
			rect.bottom = 2;
		}else
		{
			// right
			if (bFromShowCheck)
			{
				const int nScreenWidth = theApp.GetScreenWidth();
				bNeedReMoveWindow = (m_rectWin.right!=nScreenWidth)?true:false;
				m_rectWin.right = nScreenWidth;
			}else
			{
				bNeedReMoveWindow = (m_rectWin.right!=pCursorPos.x+1)?true:false;
				m_rectWin.right = pCursorPos.x+1;
				rect.right = pCursorPos.x+1;
			}
			m_rectWin.left = m_rectWin.right-nWinWidth;
			rect.left = rect.right-2;
		}
		m_pDlgShrinkageBar->MoveWindow(&rect);
		if (bNeedReMoveWindow)
		{
			ReleaseCapture();
			MoveWindow(&m_rectWin);
		}

		if (!m_bShrinkageWin)
		{
			m_bShrinkageWin = true;
			SetTimer(TIMERID_SHRINKAGE_WIN,100,NULL);
			m_pDlgShrinkageBar->SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
			//SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}else if (nNeedHideWin==0 && m_bShrinkageWin && (x>0 || y>0))
	//}else if (nNeedHideWin==0 && m_bShrinkageWin && x>0 && y>0)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_bShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		this->ShowWindow(SW_SHOW);
		SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
	return m_bShrinkageWin;
}

void CDlgFrameList::OnMove(int x, int y)
{
	CEbDialogBase::OnMove(x, y);
	if (m_bOpenOk && !this->IsIconic() && (x>=0 && y>=0))
	//if (!this->IsIconic() && (x>0 || y>0))
	{
		char lpszBuffer[24];
		if (GetPrivateProfileInt(_T("default"),_T("frame-list-x"),0,theApp.GetUserSettingIniFile())!=x)
		{
			sprintf(lpszBuffer,"%d",x);
			WritePrivateProfileString(_T("default"),_T("frame-list-x"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
		if (GetPrivateProfileInt(_T("default"),_T("frame-list-y"),0,theApp.GetUserSettingIniFile())!=y)
		{
			sprintf(lpszBuffer,"%d",y);
			WritePrivateProfileString(_T("default"),_T("frame-list-y"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
	}

	if (m_pPanelSearch!=NULL && m_pPanelSearch->GetSafeHwnd()!=NULL)
		m_pPanelSearch->OnMove();

	CheckAutoHide(x, y, false);
	//CPoint pCursorPos;
	//GetCursorPos(&pCursorPos);

	//int nNeedHideWin = 0;
	////if ((pCursorPos.x<=0 && y>0) || (!m_bOpenOk && x<=0 && y>0))
	//if (pCursorPos.x<=0 && y>0)
	//	nNeedHideWin = 1;		// left
	////else if ((pCursorPos.y<=0 && y<=0) || (!m_bOpenOk && x>0 && y<=0))
	//else if (pCursorPos.y<=0 && x>0 && y<=0)
	//	nNeedHideWin = 2;		// top
	//else
	//{
	//	//CRect rect;
	//	//this->GetWindowRect(&rect);
	//	const int nScreenWidth = theApp.GetScreenWidth();
	//	//if (((pCursorPos.x+1)>=nScreenWidth && y>0) || (!m_bOpenOk && rect.right>=nScreenWidth && y>0))
	//	if ((pCursorPos.x+1)>=nScreenWidth && y>0)
	//		nNeedHideWin = 3;	// right
	//}
	//if (nNeedHideWin>0)
	//{
	//	CRect rect;
	//	this->GetWindowRect(&rect);
	//	const int nWinWidth = rect.Width();

	//	if (m_pDlgShrinkageBar==NULL)
	//	{
	//		CWnd* pParent = GetDesktopWindow();
	//		m_pDlgShrinkageBar = new CDlgShrinkageBar(pParent);
	//		m_pDlgShrinkageBar->Create(CDlgShrinkageBar::IDD,pParent);
	//		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
	//	}
	//	m_rectWin = rect;
	//	bool bNeedReMoveWindow = false;
	//	if (nNeedHideWin==1)
	//	{
	//		// left
	//		bNeedReMoveWindow = m_rectWin.left!=0?true:false;
	//		m_rectWin.left = 0;
	//		m_rectWin.right = nWinWidth;
	//		rect.left = 0;
	//		rect.right = 2;
	//	}else if (nNeedHideWin==2)
	//	{
	//		// top
	//		rect.top = 0;
	//		rect.bottom = 2;
	//	}else
	//	{
	//		// right
	//		bNeedReMoveWindow = (m_rectWin.right!=pCursorPos.x+1)?true:false;
	//		m_rectWin.right = pCursorPos.x+1;
	//		m_rectWin.left = m_rectWin.right-nWinWidth;
	//		rect.right = pCursorPos.x+1;
	//		rect.left = rect.right-2;
	//	}
	//	m_pDlgShrinkageBar->MoveWindow(&rect);
	//	if (bNeedReMoveWindow)
	//	{
	//		ReleaseCapture();
	//		MoveWindow(&m_rectWin);
	//	}

	//	if (!m_bShrinkageWin)
	//	{
	//		m_bShrinkageWin = true;
	//		SetTimer(TIMERID_SHRINKAGE_WIN,100,NULL);
	//		m_pDlgShrinkageBar->SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	//		SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	//	}
	//}else if (nNeedHideWin==0 && m_bShrinkageWin && (x>0 || y>0))
	////}else if (nNeedHideWin==0 && m_bShrinkageWin && x>0 && y>0)
	//{
	//	KillTimer(TIMERID_SHRINKAGE_WIN);
	//	m_bShrinkageWin = false;
	//	m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
	//	this->ShowWindow(SW_SHOW);
	//	SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	//}

	//if (m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG && m_pCurrentFrameWndInfo->GetDialog().get()!=NULL)
	//	m_pCurrentFrameWndInfo->GetDialog()->RefreshWeb();

	CFrameWndInfoProxy::OnMove();
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=NULL && m_pDlgAppFrame->IsWindowVisible())
	{
		m_pDlgAppFrame->OnMove();
	}
#endif
	//BoostReadLock rdLock(m_pList.mutex());
	//CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	//for (; pIter!=m_pList.end(); pIter++)
	//{
	//	CFrameWndInfo::pointer pFrameWndInfo = *pIter;
	//	switch (pFrameWndInfo->GetType())
	//	{
	//	case CFrameWndInfo::FRAME_WND_CALL_DIALOG:
	//		{
	//			if (pFrameWndInfo->GetDialog().get()!=NULL)
	//				pFrameWndInfo->GetDialog()->OnMove();
	//		}break;
	//	default:
	//		break;
	//	}
	//}
}

void CDlgFrameList::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case TIMERID_CHECK_CLOSE_DIALOG:
		{
			do
			{
				CFrameWndInfo::pointer pCloseInfo;
				m_pCloseList.front(pCloseInfo);
			}while (m_pCloseList.size()>2);
		}break;
	case TIMERID_CHECK_MOUSE_POS:
		{
			CFrameWndInfoProxy::CheckMousePos();
			if (m_btnSwitchLeft.GetSafeHwnd()!=NULL)
			{
				CPoint pos;
				GetCursorPos(&pos);
				this->ScreenToClient(&pos);

				const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
				if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_OFFICE)
				{
					if (m_btnSwitchLeft.IsWindowVisible())
						m_btnSwitchLeft.ShowWindow(SW_HIDE);
				}else
				{
					static bool thePtInRect = false;
					const bool bPtInRect = m_rectSwitchLeft.PtInRect(pos)?true:false;
					if (thePtInRect!=bPtInRect)
					{
						thePtInRect = bPtInRect;
						if (bPtInRect)
						{
							if (!m_btnSwitchLeft.IsWindowVisible())
							{
								m_btnSwitchLeft.ShowWindow(SW_SHOW);
								m_btnSwitchLeft.Invalidate();
							}
						}else
						{
							if (m_btnSwitchLeft.IsWindowVisible())
							{
								m_btnSwitchLeft.ShowWindow(SW_HIDE);
								m_btnSwitchLeft.Invalidate();
							}
						}
					}
				}
			}
		}break;
	case TIMERID_CHECK_SHRINKAGEBAR:
		{
			if (m_bShrinkageWin && this->IsWindowVisible() && m_pDlgShrinkageBar->IsWindowVisible())
			{
				CPoint pos;
				GetCursorPos(&pos);
				if (m_rectWin.PtInRect(pos))
				{
					KillTimer(nIDEvent);
					m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
				}
			}else
			{
				KillTimer(nIDEvent);
			}
		}break;
	case TIMERID_SHRINKAGE_WIN:
		{
			CPoint pos;
			GetCursorPos(&pos);
			CRect rectWin;
			m_pDlgShrinkageBar->GetWindowRect(&rectWin);
			CRect rectTemp(m_rectWin);
			rectTemp.InflateRect(3,3,3,3);
			if (pos.y>0 && !rectTemp.PtInRect(pos) && this->GetSplitterPressed()==0)
			//if (pos.y>0 && !m_rectWin.PtInRect(pos) && this->GetSplitterPressed()==0)
			{
				// up
				if (m_pDlgShrinkageBar->IsWindowVisible())
					break;
				m_pDlgShrinkageBar->ShowWindow(SW_SHOW);
				this->ShowWindow(SW_HIDE);
				SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
				if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
					m_pPanelSearch->CheckSearchResultShowHide();
			}else if (rectWin.PtInRect(pos))
			{
				// down
				if (!m_pDlgShrinkageBar->IsWindowVisible())
					break;
				m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
				this->ShowWindow(SW_SHOWNORMAL);
				SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
				CFrameWndInfoProxy::OnFirstWorkFrameWndShow();
				if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
					m_pPanelSearch->CheckSearchResultShowHide();
#ifdef USES_FRAMELIST_APPFRAME
				if (m_pDlgAppFrame!=NULL && m_pDlgAppFrame->IsWindowVisible())
				{
					m_pDlgAppFrame->RefreshAppWnd();
				}
				if (m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG && m_pCurrentFrameWndInfo->GetDialog().get()!=NULL)
					m_pCurrentFrameWndInfo->GetDialog()->RefreshWeb();
#endif
			}
		}break;
	default:
		break;
	}
	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgFrameList::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID==SC_RESTORE && m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)
	{
#ifdef USES_FRAMELIST_APPFRAME
		if (m_pDlgAppFrame!=0)
			m_pDlgAppFrame->RefreshAppWnd();
#else
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_REDRAW_APPFRAME, 0, 0);
#endif
	}else if (nID==SC_CLOSE)
	{
		OnBnClickedButtonClose();	// 支持实现，鼠标移动工作栏，点击关闭，最小化窗口
		return;
	}
	CEbDialogBase::OnSysCommand(nID, lParam);
}

LRESULT CDlgFrameList::OnMsgRefreshUrl(WPARAM wParam, LPARAM lParam)
{
//#ifdef USES_FRAMELIST_APPFRAME
//	if (m_pDlgAppFrame!=0)
//		m_pDlgAppFrame->doRefresh();
//#else
//	theApp.GetMainWnd()->PostMessage(EB_COMMAND_REFRESH_URL, wParam, lParam);
//#endif
	return 0;
}
LRESULT CDlgFrameList::OnMsgRefreshOrStopUrl(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->doRefreshOrStop();
#else
	theApp.GetMainWnd()->PostMessage(EB_COMMAND_REFRESH_OR_STOP_URL, wParam, lParam);
#endif
	return 0;
}

LRESULT CDlgFrameList::OnMsgGoBack(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->goBack();
#else
	theApp.GetMainWnd()->PostMessage(EB_COMMAND_GO_BACK, wParam, lParam);
#endif
	return 0;
}
LRESULT CDlgFrameList::OnMsgGoForward(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->goForward();
#else
	theApp.GetMainWnd()->PostMessage(EB_COMMAND_GO_FORWARD, wParam, lParam);
#endif
	return 0;
}
LRESULT CDlgFrameList::OnMsgChangeBrowserType(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->changeBrowserType();
#else
	theApp.GetMainWnd()->PostMessage(EB_COMMAND_CHANGE_BROWSER_TYPE, wParam, lParam);
#endif
	return 0;
}
LRESULT CDlgFrameList::OnMsgQueryCanSaveHistory(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=0)
		return (LRESULT)(m_pDlgAppFrame->canSaveHistory()?1:0);
	return (LRESULT)0;
#else
	return theApp.GetMainWnd()->SendMessage(EB_COMMAND_QUERY_CAN_SAVE_HISTORY);
#endif
}
LRESULT CDlgFrameList::OnMsgQueryBrowserType(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=0)
		return (LRESULT)m_pDlgAppFrame->queryBrowserType();
	return (LRESULT)EB_BROWSER_TYPE_CEF;
#else
	return theApp.GetMainWnd()->SendMessage(EB_COMMAND_QUERY_BROWSER_TYPE);
#endif
}
LRESULT CDlgFrameList::OnMsgSaveHistory(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->saveHistory();
#else
	theApp.GetMainWnd()->PostMessage(EB_COMMAND_SAVE_HISROTY, wParam, lParam);
#endif
	return 0;
}

LRESULT CDlgFrameList::OnFrameWndMin(WPARAM wParam, LPARAM lParam)
{
	//const CWnd * pWnd = (CWnd*)wParam;
	OnBnClickedButtonMin();
	return 0;
}
LRESULT CDlgFrameList::OnFrameWndMax(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedButtonMax();
	return 0;
}

LRESULT CDlgFrameList::OnFrameWndClose(WPARAM wParam, LPARAM lParam)
{
	const CWnd * pWnd = (CWnd*)wParam;
	DelWnd(pWnd);
	return 0;
}

void CDlgFrameList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnFrameWndMax(0,0);
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}

void CDlgFrameList::OnFrameBtnClicked(UINT nID)
{
	ClickFrame(nID);
}
//void CDlgFrameList::OnFrameCloseClicked(UINT nID)
//{
//	const unsigned int nCloseIndex = nID-FRAME_CLOSE_ID_OFFSET;
//	DelWnd(nCloseIndex);
//}
void CDlgFrameList::OnEnChangeEditSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEbDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	static CString theSearchString;
	CString sSearchString;
	m_editSearch.GetWindowText(sSearchString);
	if (sSearchString==theSearchString)
		return;
	theSearchString = sSearchString;

	if (!sSearchString.IsEmpty())
	{
//		m_pContactItem.clear();
//		m_pMemberItem.clear();
//		m_treeSearch.DeleteAllItems();
//#ifdef USES_EBCOM_TEST
//		//IDispatch* pCallback;
//		//((CEBSearchEventsSink*)this)->QueryInterface(IID_IDispatch,(void**)&pCallback);
//		theEBClientCore->EB_SearchUserInfo((IDispatch*)(CEBSearchEventsSink*)this, (LPCTSTR)sSearchString, 1);
//#else
//		theEBAppClient.EB_SearchUserInfo((CEBSearchCallback*)this, sSearchString, 1);
//#endif
//
//		if (m_treeSearch.GetCount() > 0)
//		{
//			ShowSearchResult();
//		}
//	}
//	if (m_treeSearch.GetCount() == 0 || sSearchString.IsEmpty())
//	{
//		m_treeSearch.DeleteAllItems();
//		m_treeSearch.ShowWindow(SW_HIDE);
//		
//		if (theApp.IsEnterpriseuserUser())
//		{
//			m_btnMyEnterprise.ShowWindow(SW_SHOW);
//		}
//		m_btnMyDepartment.ShowWindow(SW_SHOW);
//		m_btnMyContacts.ShowWindow(SW_SHOW);
//		m_btnMySession.ShowWindow(SW_SHOW);
//		if (m_btnMyEnterprise.GetChecked())
//			m_pDlgMyEnterprise->ShowWindow(SW_SHOW);
//		else if (m_btnMyDepartment.GetChecked())
//			m_pDlgMyGroup->ShowWindow(SW_SHOW);
//		else if (m_btnMyContacts.GetChecked())
//			m_pDlgMyContacts->ShowWindow(SW_SHOW);
//		else if (m_btnMySession.GetChecked())
//			m_pDlgMySession->ShowWindow(SW_SHOW);
	}
}
LRESULT CDlgFrameList::OnMessageMoveUp(WPARAM wParam, LPARAM lParam)
{
	ShowUpWnd();
	return 0;
}
LRESULT CDlgFrameList::OnMessageMoveDown(WPARAM wParam, LPARAM lParam)
{
	ShowDownWnd();
	return 0;
}
LRESULT CDlgFrameList::OnMessageMoveOffset(WPARAM wParam, LPARAM lParam)
{
	const int nOffset = (int)wParam;
	ShowOffsetWnd(nOffset-1);
	return 0;
}
LRESULT CDlgFrameList::OnMessageAdjustWidth(WPARAM wParam, LPARAM lParam)
{
	//const HWND hAdjustWidthWnd = (HWND)wParam;
	const int nWidthOffset = (int)lParam;
	CRect rectClient;
	this->GetWindowRect(&rectClient);
	if (m_rectAdjustOld.Width()==0 || m_rectAdjustOld.Height()==0)
		m_rectAdjustOld = rectClient;
	rectClient.right += nWidthOffset;
	this->MoveWindow(rectClient);
	return 0;
}
void CDlgFrameList::OnFrameWndEmpty(void)
{
	if (m_bShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_bShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
	this->ShowWindow(SW_HIDE);
	if (m_rectAdjustOld.Width()>0)
	{
		this->MoveWindow(m_rectAdjustOld);
		memset(&m_rectAdjustOld,0,sizeof(m_rectAdjustOld));
	}
}
void CDlgFrameList::OnFrameWndShow(const CFrameWndInfo::pointer& pFrameWndInfo, bool bShow)
{
	if (bShow)
		m_pCurrentFrameWndInfo = pFrameWndInfo;
	else if (m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetType() == pFrameWndInfo->GetType())
		m_pCurrentFrameWndInfo.reset();
	//if (bShow)
	//{
	//	m_pCurrentFrameWndInfo = pFrameWndInfo;
	//	//m_nCurrentFrameWndType = pFrameWndInfo->GetType();
	//}else if (m_nCurrentFrameWndType == pFrameWndInfo->GetType())
	//	m_nCurrentFrameWndType = CFrameWndInfo::FRAME_WND_UNKNOWN;

	//if (m_pWorkFrameCallback!=NULL)
#ifdef USES_FRAMELIST_APPFRAME
	if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)
#else
	if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME && m_pWorkFrameCallback!=NULL)
#endif
	{
		if (bShow)
		{
//#ifdef USES_EBCOM_TEST
//			const CEBString sUserName = theEBClientCore->EB_UserName.GetBSTR();
//			const CEBString sSettingEnterprise = theEBSetting->Enterprise.GetBSTR();
//#else
//			const CEBString sUserName = theEBAppClient.EB_GetUserName();
//			const CEBString sSettingEnterprise = theSetting.GetEnterprise();
//#endif
//			CString sWindowText;
//			if (theApp.IsLogonVisitor())
//				sWindowText.Format(_T("工作台-游客-%s"),sUserName.c_str());
//			else if (!theApp.GetProductName().IsEmpty())
//				sWindowText.Format(_T("工作台-%s-%s(%s)"),theApp.GetProductName(),sUserName.c_str(),theApp.GetLogonAccount());
//			else
//				sWindowText.Format(_T("工作台-%s-%s(%s)"),sSettingEnterprise.c_str(),sUserName.c_str(),theApp.GetLogonAccount());
//			this->SetWindowText(sWindowText);
			this->SetWindowText(_T("工作台"));
		}

#ifdef USES_FRAMELIST_APPFRAME
		if (bShow && m_pList.size(false)==1 && m_pDlgAppFrame->IsEmpty())
#else
		if (bShow && m_pList.size(false)==1 && m_pWorkFrameCallback->OnGetFrameSize(pFrameWndInfo)==0)
#endif
		{
			// ** 优化实现，切换显示最后一个空“工作台”时，直接隐藏当前界面
			OnMessageReturnMainFrame(0,0);
			m_pCurrentFrameWndInfo.reset();
			//m_nCurrentFrameWndType = CFrameWndInfo::FRAME_WND_UNKNOWN;
		}else
		{
			RefreshBtnShow(bShow);
			if (m_pPanelSearch!=NULL)
			{
				if (bShow)
				{
					CRect rectFrameList;
					this->GetClientRect(&rectFrameList);
					const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left2;
					m_pPanelSearch->MoveWindow(nBorderLeft+theSearchLeftOffset,const_border_size+theSearchTopOffset,rectFrameList.Width()-nBorderLeft-const_border_size-theSearchLeftOffset-theSearchRightOffset,theSearchToolbarHeight-theSearchTopOffset-1);
				}
				m_pPanelSearch->ShowWindow(bShow?SW_SHOW:SW_HIDE);
			}
#ifdef USES_FRAMELIST_APPFRAME
			if (m_pDlgAppFrame!=NULL)
			{
				if (bShow)
				{
					//OnBnClickedButtonSwitchFrame();	// *** 有异常
					if (m_pDlgAppFrame->IsEmpty())
					//if (m_pDlgAppFrame==NULL || m_pDlgAppFrame->IsEmpty())
					{
						const tstring sAppUrl("about:blank");
						EB_SubscribeFuncInfo pSubscribeFuncInfo;
						pSubscribeFuncInfo.m_sFunctionName = sAppUrl;
						pSubscribeFuncInfo.m_bClosePrompt = false;
						pSubscribeFuncInfo.m_bDisableScrollBar = false;
						pSubscribeFuncInfo.m_bDisableContextMenu = false;
						pSubscribeFuncInfo.m_bWinResizable = true;
						m_pDlgAppFrame->AddAppUrl(false,sAppUrl,"",pSubscribeFuncInfo,false,NULL);
						//AddAppUrl(false,"about:blank",NULL);
						//return;
					}
					CRect rectFrameList;
					this->GetClientRect(&rectFrameList);
					const int nBorderLeft = this->GetShowedLeft()?const_border_left:const_hide_border_left2;
					m_pDlgAppFrame->MoveWindow(nBorderLeft,const_border_size+theSearchToolbarHeight,rectFrameList.Width()-nBorderLeft-const_border_size,rectFrameList.Height()-const_border_size*2-theSearchToolbarHeight);
					m_pDlgAppFrame->ShowWindow(SW_SHOW);
					m_pDlgAppFrame->RefreshAppWnd();
					//SetTimer(TIMERID_REFRESH_APPFRAME,1,NULL);
				}else
				{
					m_pDlgAppFrame->ShowWindow(SW_HIDE);
				}
			}
#else
			m_pWorkFrameCallback->OnFrameWndShow(pFrameWndInfo, bShow);
#endif
		}
	}else if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG && pFrameWndInfo->GetDialog().get()!=NULL)
	{
		pFrameWndInfo->GetDialog()->ShowWindow(bShow?SW_SHOW:SW_HIDE);
		if (bShow)
		{
			this->SetWindowText(_T("会话窗口"));
			if (m_pWorkFrameCallback!=NULL)
				m_pWorkFrameCallback->OnFrameWndShow(pFrameWndInfo, bShow);
		}
	}
}

LRESULT CDlgFrameList::OnMessageReturnMainFrame(WPARAM wParam, LPARAM lParam)
{
	//const bool bShowWnd = wParam==0?true:false;
	//if (bShowWnd)
	//{
	//	//CFrameWndInfoProxy::ShowWnd((unsigned int)FRAME_BTN_ID_MAINFRAME);
	//}
	if (this->m_pList.size(false)<=1)
	{
		if (m_bShrinkageWin)
		{
			KillTimer(TIMERID_SHRINKAGE_WIN);
			m_bShrinkageWin = false;
			m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
			SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
		this->ShowWindow(SW_HIDE);
#ifdef USES_FRAMELIST_APPFRAME
		if (m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME && m_pDlgAppFrame->IsEmpty())
		{
			m_pCurrentFrameWndInfo.reset();
		}
#endif
	}else
	{
		CFrameWndInfoProxy::ShowDownWnd();
	}

	return 0;
}
LRESULT CDlgFrameList::OnFrameWndCount(WPARAM wParam, LPARAM lParam)
{
	const HWND pWnd = (HWND)wParam;
	const int nCount = (int)lParam;
	//if (pWnd!=NULL && m_pDlgAppFrame!=NULL && pWnd==m_pDlgAppFrame->GetSafeHwnd())
	{
		if (nCount==0)
		{
			CFrameWndInfoProxy::SetTitle(CFrameWndInfo::FRAME_WND_WORK_FRAME, _T("工作台"), _T("工作台 Ctrl+1"));
			if (!this->GetShowedLeft())
				CFrameWndInfoProxy::SetUnreadMsg(CFrameWndInfo::FRAME_WND_WORK_FRAME,0);
		}else
		{
			CString sText;
			sText.Format(_T("工作台  %d"),nCount);
			CString sTooltip;
			sTooltip.Format(_T("工作台(%d) Ctrl+1"),nCount);
			CFrameWndInfoProxy::SetTitle(CFrameWndInfo::FRAME_WND_WORK_FRAME, sText, sTooltip);
			if (!this->GetShowedLeft())
				CFrameWndInfoProxy::SetUnreadMsg(CFrameWndInfo::FRAME_WND_WORK_FRAME,(size_t)nCount);
		}
	}
	return 0;
}

void CDlgFrameList::OnBnClickedButtonSwitchLeft()
{
	const bool bShowedLeft = CFrameWndInfoProxy::GetShowedLeft()?false:true;
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CFrameWndInfo::pointer>::const_iterator pIter = m_pList.begin();
		if (pIter!=m_pList.end())
		{
			const CFrameWndInfo::pointer& pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)
			{
				rdLock.unlock();
				// ** 用于实现，隐藏左边栏时，显示工作区有打开多少个页面页；
				if (bShowedLeft)
				{
					CFrameWndInfoProxy::SetUnreadMsg(CFrameWndInfo::FRAME_WND_WORK_FRAME,0);
				}else// if (m_pWorkFrameCallback!=NULL)
				{
#ifdef USES_FRAMELIST_APPFRAME
					const size_t nSize = m_pDlgAppFrame->GetSize();
#else
					const size_t nSize = m_pWorkFrameCallback->OnGetFrameSize(pFrameWndInfo);
#endif
					CFrameWndInfoProxy::SetUnreadMsg(CFrameWndInfo::FRAME_WND_WORK_FRAME,nSize);
				}
			}
		}
	}

	WritePrivateProfileString(_T("default"),_T("frame-list-show-left"),(bShowedLeft?_T("1"):_T("0")),theApp.GetUserSettingIniFile());
	this->SetToolTipText(IDC_BUTTON_SWITCH_LEFT,bShowedLeft?_T("收起面板"):_T("展开面板"));
	m_btnSwitchLeft.SetDrawTrianglePic((bShowedLeft?3:4),theDefaultFlatLineColor,theDefaultFlatBlackText2Color,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	CFrameWndInfoProxy::SetShowHideLeft(bShowedLeft);
	CRect rect;
	this->GetClientRect(&rect);
	MoveSize(rect.Width(),rect.Height());
	this->Invalidate();
	m_btnSwitchLeft.Invalidate();
}

LRESULT CDlgFrameList::OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam)
{
	if (this->m_pPanelSearch!=NULL)
		this->m_pPanelSearch->PostMessage(EB_COMMAND_CHANGE_APP_URL,wParam,lParam);
	//theApp.GetMainWnd()->PostMessage(EB_COMMAND_CHANGE_APP_URL, wParam, 1);	// from CDlgFrameList
	return 0;
}
LRESULT CDlgFrameList::OnMsgShowRefreshOrStop(WPARAM wParam, LPARAM lParam)
{
	if (this->m_pPanelSearch!=NULL)
		this->m_pPanelSearch->PostMessage(EB_COMMAND_SHOW_REFRESH_OR_STOP,wParam,lParam);
	return 0;
}

void CDlgFrameList::OnShowHeadOnly()
{
	OnBnClickedButtonSwitchLeft();
}
void CDlgFrameList::OnOpenWorkFrame()
{
	theApp.GetMainWnd()->PostMessage(EB_COMMAND_OPEN_WORKFRAME, 0, 0);
}
void CDlgFrameList::OnCloseTab()
{
	if (m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetCallId()>0)
	{
		this->DelWnd(m_pCurrentFrameWndInfo->GetCallId());
	}
}

BOOL CDlgFrameList::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		//const bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
		const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
		//if (bShift && bControl)
		//{
		//	if (pMsg->wParam=='S')
		//	{
		//		this->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL,1,1);
		//		return TRUE;
		//	}
		//}
		//
		if (bControl)
		{
			if (pMsg->wParam==VK_UP)
			{
				CFrameWndInfoProxy::ShowUpWnd();
				return TRUE;
			}else if (pMsg->wParam==VK_DOWN)
			{
				CFrameWndInfoProxy::ShowDownWnd();
				return TRUE;
			}else if (pMsg->wParam>=VK_NUMPAD0 && pMsg->wParam<=VK_NUMPAD9)
			{
				const int nOffset = (int)pMsg->wParam-VK_NUMPAD0;
				if (CFrameWndInfoProxy::ShowOffsetWnd(nOffset-1))
					return TRUE;
			}
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
LRESULT CDlgFrameList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message==WM_ENTERMENULOOP)
	{
		if (!wParam) {
			// Entering the menu loop for the application menu.
			CefSetOSModalLoop(true);
		}
	}else if (message==WM_EXITMENULOOP)
	{
		if (!wParam) {
			// Exiting the menu loop for the application menu.
			CefSetOSModalLoop(false);
		}
#ifdef USES_FRAMELIST_APPFRAME
	}else if (message == WM_MOVING || message == WM_MOVE)
	{
		if (m_pDlgAppFrame!=NULL && m_pDlgAppFrame->GetSafeHwnd()!=NULL)
			m_pDlgAppFrame->NotifyMoveOrResizeStarted();
#endif
	}
	return __super::WindowProc(message, wParam, lParam);
}
void CDlgFrameList::OnBnClickedButtonMin()
{
	if (m_bShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_bShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CDlgFrameList::OnBnClickedButtonMax()
{
	const int nScreenWidth = theApp.GetScreenWidth();
	const int nScreenHeight = theApp.GetScreenHeight();

	bool bSetBtnMax = false;
	static CRect theRestoreRect;
	CRect rect;
	GetWindowRect(&rect);
	if (rect.Width() == nScreenWidth)
	{
		bSetBtnMax = true;
		if (theRestoreRect.Width()<=0)
		{
			theRestoreRect.left = (nScreenWidth-theDefaultWidth)/2;
			theRestoreRect.right = theRestoreRect.left+theDefaultWidth;
		}
		if (theRestoreRect.Height()<=0)
		{
			theRestoreRect.top = (nScreenHeight-theDefaultHeight)/2;
			theRestoreRect.bottom = theRestoreRect.top+theDefaultHeight;
		}
		MoveWindow(&theRestoreRect);
		this->SetToolTipText(IDC_BUTTON_MAX,_T("最大化"));
		m_btnMax.SetDrawToolButtonPic(2,theDefaultFlatBlackText2Color,theDefaultBtnWhiteColor,-1,-1,2);
	}else
	{
		theRestoreRect = rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = rect.left + nScreenWidth;
		rect.bottom = rect.top + nScreenHeight;
		MoveWindow(&rect);
		this->SetToolTipText(IDC_BUTTON_MAX,_T("向下还原"));
		m_btnMax.SetDrawToolButtonPic(3,theDefaultFlatBlackText2Color,theDefaultBtnWhiteColor,-1,-1,2);
	}
	//if (m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME && m_pDlgAppFrame!=NULL && m_pDlgAppFrame->IsWindowVisible())
	//{
	//	m_pDlgAppFrame->RefreshAppWnd();	// 解决Chrome还原窗口，背景显示灰色问题
	//}

	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CFrameWndInfo::pointer& pFrameWndInfo = *pIter;
		if (pFrameWndInfo->GetDialog().get()!=NULL)
		{
			pFrameWndInfo->GetDialog()->SetBtnMax(bSetBtnMax);
		}
	}
}
void CDlgFrameList::OnBnClickedButtonClose()
{
	if (m_bShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_bShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
	this->ShowWindow(SW_HIDE);
	if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
	{
		m_pPanelSearch->HideSearchResult();
		//m_pPanelSearch->ShowWindow(SW_HIDE);
	}
	//OnBnClickedButtonMin();
}
void CDlgFrameList::RefreshBtnShow(bool bShow)
{
	SetCtrlColor(true);
	m_btnMin.ShowWindow(bShow?SW_SHOW:SW_HIDE);
	m_btnMax.ShowWindow(bShow?SW_SHOW:SW_HIDE);
	m_btnClose.ShowWindow(bShow?SW_SHOW:SW_HIDE);
}

void CDlgFrameList::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_menuTray.GetSafeHmenu()==NULL)
	{
		m_menuTray.CreatePopupMenu();
		m_menuTray.SetBitmapBackground(RGB(192,192,192));
		m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SHOWHEAD_ONLY,_T("仅显示图标"));
		m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_OPEN_WORKFRAME,_T("打开工作台"));
		//m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CLOSE_TAB,_T("关闭当前会话"));
	}
	m_menuTray.CheckMenuItem(EB_COMMAND_SHOWHEAD_ONLY,CFrameWndInfoProxy::GetShowedLeft()?MF_UNCHECKED:MF_CHECKED);
	if (m_pCurrentFrameWndInfo.get()!=NULL && m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG)
	{
		if (!m_bShowedCloseTabCommand)
		{
			m_bShowedCloseTabCommand = true;
			m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CLOSE_TAB,_T("关闭当前会话"));
		}
	}else if (m_bShowedCloseTabCommand)
	{
		m_bShowedCloseTabCommand = false;
		m_menuTray.RemoveMenu(EB_COMMAND_CLOSE_TAB,MF_BYCOMMAND);
	}

	CPoint pt;
	GetCursorPos(&pt);
	m_menuTray.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,pt.x,pt.y,this);

	__super::OnRButtonUp(nFlags, point);
}

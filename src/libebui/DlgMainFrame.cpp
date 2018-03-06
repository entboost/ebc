// DlgMainFrame.cpp : implementation file
//

#include "stdafx.h"
#include "DlgMainFrame.h"

#include "DlgIncomingCall.h"
#include "DlgAlertingCall.h"
#include "DlgDiaRecord.h"
#include "DlgMyEnterprise.h"
#include "DlgMyGroup.h"
#include "DlgMyContacts.h"
#include "DlgMySession.h"
#include "DlgEditInfo.h"
#include "DlgFileManager.h"
#include "DlgFrameList.h"
#include <sys/timeb.h>
#include "DlgMessageBox.h"
#include "EBCCallInfo.h"
#include "Core/SkinMemDC.h"
#include "MMSystem.h"
#include "DlgFuncWindow.h"
#include "DlgShrinkageBar.h"
#include "DlgAppFrame.h"
#include "PanelSearch.h"
//#include "PanelBrowserNavigate.h"
#include "DlgViewContactInfo.h"

const std::string theUrlGroupName("[* *]");
const CString theUrlGroupResultName = _T("网站搜索结果：");

////////////////////////
bool theWantExitApp = false;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
#define WM_TRAYICON_NOTIFICATION WM_USER+1

#define LEN_DLG_WIDTH 274+DEFAULT_RETURN_BTN_WIDTH	// 12*2
#define LEN_DLG_HEIGHT 588

#define POS_ADIMG_LEFT 13
#define POS_ADIMG_TOP 43
#define POS_ADIMG_SIGE 60	// 128
//#define POS_STA_FIRST_Y (POS_ADIMG_TOP+POS_ADIMG_SIGE+10)	// 18
#define POS_LINESTATE_LEFT (POS_ADIMG_LEFT+POS_ADIMG_SIGE+10)
#define POS_LINESTATE_TOP (POS_ADIMG_TOP-1)
//#define POS_LINESTATE_WIDTH 30

//#define TIMERID_LOADINFO			0x121
//#define TIMERID_RELOGIN				0x122
#define TIMERID_LOGOUT				0x123
#define TIMERID_CHECK_CLOSE_DIALOG	0x124
#define TIMERID_CHECK_UPDATE		0x125
bool theUpdateResetTimer = false;

#define TIMERID_NEW_VERSION				0x126
#define TIMERID_SHOW_EMOTIPN			0x127
#define TIMERID_CHECK_MF_SUBFUNCINFO	0x128
#define TIMERID_SHRINKAGE_WIN			0x129
#define TIMERID_AUTO_OPEN_SUBID			0x12A
#define TIMERID_CHECK_MOUSE_POS			0x130
#define TIMERID_REFRESH_APPFRAME		0x131
#define TIMERID_ONE_SECOND				0x132

// CDlgMainFrame dialog

IMPLEMENT_DYNAMIC(CDlgMainFrame, CEbDialogBase)

CDlgMainFrame::CDlgMainFrame(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgMainFrame::IDD, pParent)
	//, m_sCallNumber(_T(""))
#ifndef USES_MIN_UI
	, m_pDlgEditInfo(NULL)
#endif
	, m_pDlgMyEnterprise(NULL)
	, m_pDlgMyGroup(NULL)
	, m_pDlgMyContacts(NULL)
	, m_pDlgMySession(NULL)
{
#ifndef USES_MIN_UI
	m_bShowedToolbar = true;
#endif
	m_pCurrentLabel = NULL;
	m_pOldCurrentLabel = NULL;
	m_pDlgFrameList = NULL;
	m_pDlgAppFrame = NULL;
	m_nMsgId = EBUI_WM_DEFAULT_MSG_ID;
	m_bAppFrameShowed = false;
	//m_bShowAppFrame = false;
	m_pDlgFileManager = NULL;
#ifndef USES_MIN_UI
	m_bRemoveAppFuncIndex = false;
	m_bOpenOk = false;
	m_nShrinkageWin = false;
	m_pDlgShrinkageBar = NULL;
	m_pPanelSearch = NULL;
	m_bCanSearch = true;
#endif
	//m_pPanelBrowserNavigate = NULL;
	m_pViewContactInfo = NULL;
}

CDlgMainFrame::~CDlgMainFrame()
{
}

void CDlgMainFrame::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
#ifndef USES_MIN_UI
	DDX_Control(pDX, IDC_BUTTON_SWITCH_TOOLBAR2, m_btnSwitchToolbar);
	DDX_Control(pDX, IDC_BUTTON_SKIN2, m_btnSkin);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BUTTON_MAX2, m_btnMax);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_LINESTATE, m_btnLineState);
	DDX_Control(pDX, IDC_BUTTON_MY_CENTER, m_btnMyCenter);
	DDX_Control(pDX, IDC_BUTTON_FILE_MGR, m_btnFileMgr);
	DDX_Control(pDX, IDC_BUTTON_MY_SHARE, m_btnMyShare);
	DDX_Control(pDX, IDC_BUTTON_MAIN_FUNC, m_btnMainFunc);
	DDX_Control(pDX, IDC_BUTTON_GOBACK, m_btnGoBack);
	DDX_Control(pDX, IDC_BUTTON_GOFORWARD, m_btnGoForward);
	//DDX_Control(pDX, IDC_COMBO_NUMBERS, m_comboNumbers);
	//DDX_Control(pDX, IDC_BUTTON_CALLUSER, m_btnCallUser);
	//DDX_Text(pDX, IDC_EDIT_SEARCH, m_sCallNumber);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_editSearch);
	//DDX_CBString(pDX, IDC_COMBO_NUMBERS, m_sCallNumber);
#endif
	DDX_Control(pDX, IDC_BUTTON_ENTERPRISE, m_btnMyEnterprise);
	DDX_Control(pDX, IDC_BUTTON_DEPARTMENT, m_btnMyDepartment);
	DDX_Control(pDX, IDC_BUTTON_CONTACT, m_btnMyContacts);
	DDX_Control(pDX, IDC_BUTTON_SESSION, m_btnMySession);
	DDX_Control(pDX, IDC_BUTTON_SWITCH_FRAME, m_btnSwitchFrame);
#ifndef USES_MIN_UI
	DDX_Control(pDX, IDC_TREE_SEARCH, m_treeSearch);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
	DDX_Control(pDX, IDC_BUTTON_NEW_APP, m_btnNewApp);
#endif
}


BEGIN_MESSAGE_MAP(CDlgMainFrame, CEbDialogBase)
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	
	ON_MESSAGE(WM_RETURN_MAINFRAME, OnMessageReturnMainFrame)
	ON_MESSAGE(EB_WM_AREA_INFO, OnMessageAreaInfo)
	ON_MESSAGE(EB_WM_BROADCAST_MSG, OnMessageBroadcastMsg)
	ON_MESSAGE(EB_WM_NEW_VERSION, OnMessageNewVersion)
	ON_MESSAGE(EB_WM_SERVER_CHENGE, OnMessageServerChange)
	ON_MESSAGE(EB_WM_STATE_CODE, OnMessageStateCode)
	ON_MESSAGE(EB_WM_ONLINE_ANOTHER, OnMessageOnlineAnother)
	ON_MESSAGE(EB_WM_LOGOUT, OnMessageLogoutResponse)
	ON_MESSAGE(EB_WM_LOGON_SUCCESS, OnMessageLogonSuccess)
	ON_MESSAGE(EB_WM_LOGON_TIMEOUT, OnMessageLogonTimeout)
	ON_MESSAGE(EB_WM_LOGON_ERROR, OnMessageLogonError)
	//ON_MESSAGE(CR_WM_ENTER_ROOM, OnMessageEnterRoom)
	//ON_MESSAGE(CR_WM_EXIT_ROOM, OnMessageExitRoom)
	ON_MESSAGE(CR_WM_USER_ENTER_ROOM, OnMessageUserEnterRoom)
	ON_MESSAGE(CR_WM_USER_EXIT_ROOM, OnMessageUserExitRoom)
	ON_MESSAGE(CR_WM_RECEIVE_RICH, OnMessageReceiveRich)
	ON_MESSAGE(CR_WM_SEND_RICH, OnMessageSendRich)
	ON_MESSAGE(CR_WM_SENDING_FILE, OnMessageSendingFile)
	ON_MESSAGE(CR_WM_SENT_FILE, OnMessageSendedFile)
	ON_MESSAGE(CR_WM_CANCEL_FILE, OnMessageCancelFile)
	ON_MESSAGE(CR_WM_RECEIVING_FILE, OnMessageReceivingFile)
	ON_MESSAGE(CR_WM_RECEIVED_FILE, OnMessageReceivedFile)
	ON_MESSAGE(CR_WM_FILE_PERCENT, OnMessageFilePercent)
	ON_MESSAGE(CR_WM_SAVE2CLOUD_DRIVE, OnMessageSave2CloudDrive)

	ON_MESSAGE(EB_WM_RD_REQUEST_RESPONSE, OnMessageRDRequestResponse)
	ON_MESSAGE(EB_WM_RD_ACK_RESPONSE, OnMessageRDAckResponse)
	ON_MESSAGE(EB_WM_RD_REQUEST, OnMessageRDRequest)
	ON_MESSAGE(EB_WM_RD_ACCEPT, OnMessageRDAccept)
	ON_MESSAGE(EB_WM_RD_REJECT, OnMessageRDCancel)
	ON_MESSAGE(EB_WM_RD_CLOSE, OnMessageRDEnd)

	ON_MESSAGE(EB_WM_V_REQUEST_RESPONSE, OnMessageVRequestResponse)
	ON_MESSAGE(EB_WM_V_ACK_RESPONSE, OnMessageVAckResponse)
	ON_MESSAGE(EB_WM_VIDEO_REQUEST, OnMessageVideoRequest)
	ON_MESSAGE(EB_WM_VIDEO_ACCEPT, OnMessageVideoAccept)
	ON_MESSAGE(EB_WM_VIDEO_REJECT, OnMessageVideoCancel)
	ON_MESSAGE(EB_WM_VIDEO_TIMEOUT, OnMessageVideoTimeout)
	ON_MESSAGE(EB_WM_VIDEO_CLOSE, OnMessageVideoEnd)

	ON_MESSAGE(EB_WM_USER_STATE_CHANGE, OnMessageUserStateChange)
	ON_MESSAGE(EB_WM_EDITINFO_RESPONSE, OnMessageSInfoResponse)
	ON_MESSAGE(EB_WM_CALL_CONNECTED, OnMessageCallConnected)
	ON_MESSAGE(EB_WM_CALL_HANGUP, OnMessageCallHangup)
	ON_MESSAGE(EB_WM_CALL_ALERTING, OnMessageAlertingCall)
	ON_MESSAGE(EB_WM_CALL_ERROR, OnMessageCallError)
	ON_MESSAGE(EB_WM_CALL_INCOMING, OnMessageIncomingCall)
	ON_MESSAGE(EB_WM_UG_INFO, OnMessageUGInfo)
	ON_MESSAGE(EB_WM_UG_DELETE, OnMessageUGDelete)
	ON_MESSAGE(EB_WM_CONTACT_INFO, OnMessageContactInfo)
	ON_MESSAGE(EB_WM_CONTACT_DELETE, OnMessageContactDelete)
	ON_MESSAGE(EB_WM_CONTACT_STATE_CHANGE, OnMessageContactStateChange)
	ON_MESSAGE(EB_WM_ENTERPRISE_INFO, OnMessageEnterpriseInfo)
	ON_MESSAGE(EB_WM_GROUP_INFO, OnMessageGroupInfo)
	ON_MESSAGE(EB_WM_GROUP_DELETE, OnMessageGroupDelete)
	ON_MESSAGE(EB_WM_GROUP_EDIT_ERROR, OnMessageGroupEditError)
	ON_MESSAGE(EB_WM_REMOVE_GROUP, OnMessageRemoveGroup)
	ON_MESSAGE(EB_WM_EXIT_GROUP, OnMessageExitGroup)
	ON_MESSAGE(EB_WM_REQUEST_ADD2GROUP, OnMessageRequestJoin2Group)
	ON_MESSAGE(EB_WM_INVITE_ADD2GROUP, OnMessageInviteJoin2Group)
	ON_MESSAGE(EB_WM_REJECT_ADD2GROUP, OnMessageRejectJoin2Group)
	ON_MESSAGE(EB_WM_MEMBER_INFO, OnMessageMemberInfo)
	//ON_MESSAGE(EB_WM_MEMBER_DELETE, OnMessageMemberDelete)
	ON_MESSAGE(EB_WM_MEMBER_EDIT_ERROR, OnMessageMemberEditError)
	ON_MESSAGE(EB_WM_REQUEST_ADDCONTACT, OnMessageRequestAddContact)
	ON_MESSAGE(EB_WM_REJECT_ADDCONTACT, OnMessageRejectAddContact)
	ON_MESSAGE(EB_WM_ACCEPT_ADDCONTACT, OnMessageAcceptAddContact)

	//ON_MESSAGE(EB_WM_APP_MSG, OnMessageAppMsgInfo)

	ON_MESSAGE(EB_WM_RESOURCE_INFO, OnMessageResourceInfo)
	ON_MESSAGE(EB_WM_RESOURCE_DELETE, OnMessageDeleteResource)
	ON_MESSAGE(EB_WM_RESOURCE_MOVE, OnMessageMoveResource)
	ON_MESSAGE(EB_COMMAND_FILE_MANAGER, OnMessageFileManager)

#ifndef USES_MIN_UI
	ON_MESSAGE(WM_HOTKEY, OnMessageHotKey)
#endif

	//ON_MESSAGE(POP_WM_WINDOW_SIZE, OnMessageWindowResize)
	//ON_CBN_SELCHANGE(IDC_COMBO_NUMBERS, &CDlgMainFrame::OnCbnSelchangeComboNumbers)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CONTACT, &CDlgMainFrame::OnBnClickedButtonContact)
	ON_BN_CLICKED(IDC_BUTTON_ENTERPRISE, &CDlgMainFrame::OnBnClickedButtonEnterprise)
	ON_BN_CLICKED(IDC_BUTTON_DEPARTMENT, &CDlgMainFrame::OnBnClickedButtonDepartment)
#ifndef USES_MIN_UI
	ON_BN_CLICKED(IDC_BUTTON_MAX2, &CDlgMainFrame::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgMainFrame::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgMainFrame::OnBnClickedButtonClose)
#endif
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CDlgMainFrame::OnEnChangeEditSearch)
	//ON_MESSAGE(WM_ITEM_SEL_CHANGED, OnTreeItemSelChange)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDoubleClick)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
#ifndef USES_MIN_UI
	ON_MESSAGE(WM_TRAYICON_NOTIFICATION, OnIconNotification)
#endif

	ON_MESSAGE(EB_COMMAND_REFRESH_URL, OnMsgRefreshUrl)
	ON_MESSAGE(EB_COMMAND_GO_BACK, OnMsgGoBack)
	ON_MESSAGE(EB_COMMAND_GO_FORWARD, OnMsgGoForward)
	ON_MESSAGE(EB_COMMAND_OPEN_APP_URL, OnMsgOpenAppUrl)
	ON_MESSAGE(EB_COMMAND_EXIT_APP, OnMsgExitApp)
	ON_MESSAGE(EB_COMMAND_LOGOUT, OnMsgLogout)
	ON_MESSAGE(EB_COMMAND_SHOWHIDE_MAIN, OnMsgShowHideMainFrame)
	ON_COMMAND(EB_COMMAND_EXIT_APP, &CDlgMainFrame::OnExitApp)
	ON_COMMAND(EB_COMMAND_LOGOUT, &CDlgMainFrame::OnLogout)
	ON_BN_CLICKED(IDC_BUTTON_LINESTATE, &CDlgMainFrame::OnBnClickedButtonLinestate)
	ON_COMMAND(EB_COMMAND_STATE_ONLINE, &CDlgMainFrame::OnStateOnline)
	ON_COMMAND(EB_COMMAND_STATE_AWAY, &CDlgMainFrame::OnStateAway)
	ON_COMMAND(EB_COMMAND_STATE_BUSY, &CDlgMainFrame::OnStateBusy)
	ON_COMMAND(EB_COMMAND_AUTO_LOGIN, &CDlgMainFrame::OnAutoLogin)
	ON_WM_MOUSEMOVE()
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, &CDlgMainFrame::OnEnKillfocusEditDescription)
	ON_BN_CLICKED(IDC_BUTTON_SESSION, &CDlgMainFrame::OnBnClickedButtonSession)
	ON_WM_LBUTTONDOWN()
	//ON_COMMAND(EB_COMMAND_MY_SETTING, &CDlgMainFrame::OnMySetting)
	//ON_COMMAND(EB_COMMAND_FILE_MANAGER, &CDlgMainFrame::OnFileManager)
	ON_COMMAND(EB_COMMAND_MY_SHARE, &CDlgMainFrame::OnMyShare)
	ON_BN_CLICKED(IDC_BUTTON_SKIN2, &CDlgMainFrame::OnBnClickedButtonSkin2)
	ON_COMMAND_RANGE(EB_COMMAND_SKIN_SETTING,EB_COMMAND_SKIN_2,&OnSkinSelect)
	ON_COMMAND(EB_COMMAND_SKIN_GRADIENT, &OnSkinGradient)
	ON_COMMAND(EB_COMMAND_OPEN_MAIN, &CDlgMainFrame::OnOpenMain)
	ON_COMMAND_RANGE(EB_COMMAND_MY_EMPLOYEE,EB_COMMAND_MY_EMPLOYEE+0x20,OnMyEmployeeInfo)
	ON_COMMAND_RANGE(EB_COMMAND_SUBSCRIBE_FUNC,EB_COMMAND_SUBSCRIBE_FUNC+0x20,OnSubscribeFunc)
	ON_COMMAND_RANGE(EB_COMMAND_MAINFRAME_FUNC,EB_COMMAND_MAINFRAME_FUNC+0x20,OnMainFrameFunc)

	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON_MY_CENTER, &CDlgMainFrame::OnBnClickedButtonMyCenter)
	ON_BN_CLICKED(IDC_BUTTON_FILE_MGR, &CDlgMainFrame::OnBnClickedButtonFileMgr)
	ON_BN_CLICKED(IDC_BUTTON_MY_SHARE, &CDlgMainFrame::OnBnClickedButtonMyShare)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_FUNC, &CDlgMainFrame::OnBnClickedButtonMainFunc)
	ON_WM_MOVE()
	ON_WM_MOVING()
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_FRAME, &CDlgMainFrame::OnBnClickedButtonSwitchFrame)
	ON_BN_CLICKED(IDC_BUTTON_NEW_APP, &CDlgMainFrame::OnBnClickedButtonNewApp)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_TOOLBAR2, &CDlgMainFrame::OnBnClickedButtonSwitchToolbar2)
	ON_MESSAGE(EB_COMMAND_CHANGE_APP_URL, OnMsgChangeAppUrl)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SEARCH, &CDlgMainFrame::OnNMClickTreeSearch)
	ON_BN_CLICKED(IDC_BUTTON_GOBACK, &CDlgMainFrame::OnBnClickedButtonGoback)
	ON_BN_CLICKED(IDC_BUTTON_GOFORWARD, &CDlgMainFrame::OnBnClickedButtonGoforward)
END_MESSAGE_MAP()


// CDlgMainFrame 消息处理程序
DWORD dwAdvise = 0;
//BOOL CALLBACK MyEnumFonts(CONST LOGFONTW* lplf, CONST TEXTMETRICW *lptm,DWORD dwType,LPARAM lparam)
//{
//	const wchar_t * lpszFine = (const wchar_t*)lparam;
//	const std::wstring tempFontName(lplf->lfFaceName);
//	if(tempFontName.find(lpszFine)!=std::wstring::npos)
//	{
//		theFontFamily = lpszFine;
//		return false;
//	}
//	return true;
//}

void CDlgMainFrame::SetCtrlColor(void)
{
	//m_btnSwitchFrame.SetDrawPanel(true,theApp.GetMainColor(),RGB(237,28,36),theApp.GetBgColor3());
#ifndef USES_MIN_UI
	m_editSearch.SetRectangleColor(theApp.GetHotColor2(),RGB(255,255,255));
#endif
	if (m_pDlgMyGroup!=NULL)
	{
		m_pDlgMyGroup->SetCtrlColor();
	}
	if (m_pDlgMyContacts!=NULL)
		m_pDlgMyContacts->SetCtrlColor();
	if (m_pDlgMySession!=NULL)
		m_pDlgMySession->SetCtrlColor();
	if (m_pDlgMyEnterprise!=NULL)
		m_pDlgMyEnterprise->SetCtrlColor();
	if (m_pDlgFileManager!=NULL)
		m_pDlgFileManager->SetCtrlColor();
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->SetCtrlColor();
	if (m_pDlgAppFrame!=NULL)
		m_pDlgAppFrame->SetCtrlColor();
}

BOOL CDlgMainFrame::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//this->ModifyStyle(0,WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
	this->SetMouseMove(FALSE);
	//ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
    CMenuXP::SetXPLookNFeel (this,false);
    CMenuXP::UpdateMenuBar (this);

	CPaintDC dc((CWnd*)this);
  //::EnumFontsW(dc.m_hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily1.c_str());	// 
  //::EnumFontsW(dc.m_hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily0.c_str());

	// IDM_ABOUTBOX 必须在系统命令范围内。
	//ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	//ASSERT(IDM_ABOUTBOX < 0xF000);
#ifndef USES_MIN_UI
	RegisterHotKey(this->m_hWnd, 0x2013, MOD_SHIFT|MOD_ALT, 'A'); 
	RegisterHotKey(this->m_hWnd, 0x2014, MOD_SHIFT|MOD_ALT, 'Z'); 
	RegisterHotKey(this->m_hWnd, 0x2015, MOD_SHIFT|MOD_ALT, 'T'); 
	//RegisterHotKey(this->m_hWnd, 0x2013, MOD_CONTROL|MOD_ALT, 'P'); 
#endif

	//CMenu* pSysMenu = GetSystemMenu(FALSE);
	//if (pSysMenu != NULL)
	//{
	//	BOOL bNameValid;
	//	CString strAboutMenu;
	//	bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
	//	ASSERT(bNameValid);
	//	if (!strAboutMenu.IsEmpty())
	//	{
	//		pSysMenu->AppendMenu(MF_SEPARATOR);
	//		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	//	}
	//}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetTransparentType(CEbDialogBase::TT_STATIC);
	//SetSplitterBorder();
	this->EnableToolTips();
	this->SetToolTipText(IDC_BUTTON_SWITCH_TOOLBAR2,_T("收缩工具栏"));
	this->SetToolTipText(IDC_BUTTON_SKIN2,_T("设置"));
	this->SetToolTipText(IDC_BUTTON_MIN,_T("最小化"));
	this->SetToolTipText(IDC_BUTTON_MAX2,_T("最大化"));
	this->SetToolTipText(IDC_BUTTON_CLOSE,_T("关闭"));
	this->SetToolTipText(IDC_BUTTON_LINESTATE,_T("在线状况"));
	this->SetToolTipText(IDC_BUTTON_MY_CENTER,_T("个人中心"));
	this->SetToolTipText(IDC_BUTTON_FILE_MGR,_T("文件传输管理"));
	this->SetToolTipText(IDC_BUTTON_MY_SHARE,_T("个人云盘"));
	this->SetToolTipText(IDC_BUTTON_MAIN_FUNC,_T("应用功能"));
	this->SetToolTipText(IDC_EDIT_DESCRIPTION,_T("点击编辑个性签名"));
	this->SetToolTipText(IDC_EDIT_SEARCH,_T("搜索用户、帐号、联系人：回车开始聊天；输入网址：回车打开网站"));
	//this->SetToolTipText(IDC_BUTTON_CALLUSER,_T("呼叫用户"));
	this->SetToolTipText(IDC_BUTTON_DEPARTMENT,_T("我的部门（群和讨论组）"));
	this->SetToolTipText(IDC_BUTTON_CONTACT,_T("联系人"));
	this->SetToolTipText(IDC_BUTTON_SESSION,_T("最近会话"));
	this->SetToolTipText(IDC_BUTTON_ENTERPRISE,_T("公司组织结构"));

#ifndef USES_MIN_UI
	m_editSearch.SetTextColor(RGB(61,61,61));
	m_editSearch.SetPromptText(_T("搜索用户：回车开始聊天；输入网址：回车打开网站"));
	//m_editSearch.SetFocusSelAll(TRUE);
#endif
	SetCtrlColor();

#ifndef USES_MIN_UI
	m_btnSwitchToolbar.SetWindowText(_T(""));
	m_btnSwitchToolbar.SetDrawTrianglePic(1,RGB(250,250,250),-1,-1,-1,12,6);
	m_btnSkin.Load(IDB_PNG_BTN_SKIN);
	m_btnMin.SetAutoSize(false);
	m_btnMin.Load(IDB_PNG_MIN);
	m_btnMax.SetAutoSize(false);
	m_btnMax.Load(IDB_PNG_MAX);
	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);
	m_btnLineState.SetAutoSize(false);
#ifdef USES_EBCOM_TEST
	EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
#else
	EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
#endif
	//m_btnLineState.Load(IDB_PNG_BTN);
	if (nOutLineState == EB_LINE_STATE_AWAY)
		m_btnLineState.Load(IDB_PNG_BTN_STATE_AWAY);
	else if (nOutLineState == EB_LINE_STATE_BUSY)
		m_btnLineState.Load(IDB_PNG_BTN_STATE_BUSY);
	else// if (nOutLineState == EB_LINE_STATE_ONLINE_NEW)
		m_btnLineState.Load(IDB_PNG_BTN_STATE_ONLINE);
	m_btnLineState.SetWindowText(_T(""));
	m_btnMyCenter.Load(IDB_PNG_BTN_MY_CENTER);
	m_btnMyCenter.SetWindowText(_T(""));
	m_btnFileMgr.Load(IDB_PNG_BTN_FILE_MGR);
	m_btnFileMgr.SetWindowText(_T(""));
	m_btnMyShare.Load(IDB_PNG_BTN_MY_SHARE);
	m_btnMyShare.SetWindowText(_T(""));
	if (theApp.GetDisableUserCloudDrive() || theApp.IsLogonVisitor())
	{
		m_btnMyShare.ShowWindow(SW_HIDE);
	}
	m_btnMainFunc.Load(IDB_PNG_BTN_MAIN_FUNC);
	m_btnMainFunc.SetWindowText(_T(""));
	m_btnGoBack.Load(IDB_PNG_BTN_GOBACK72X18);
	m_btnGoBack.SetWindowText(_T(""));
	m_btnGoBack.SetToolTipText(_T("转到上一页"));
	m_btnGoForward.Load(IDB_PNG_BTN_GOFORWARD72X18);
	m_btnGoForward.SetWindowText(_T(""));
	m_btnGoForward.SetToolTipText(_T("转到下一页"));

	//m_btnMainFunc.ShowWindow(SW_HIDE);	// 先隐藏，后面加载到功能数据再显示；
	m_pPanelSearch = new CPanelSearch(this);
	m_pPanelSearch->Create(CPanelSearch::IDD,this);
	//m_pPanelSearch->ShowWindow(SW_SHOW);
	//m_pPanelBrowserNavigate = new CPanelBrowserNavigate(this);
	//m_pPanelBrowserNavigate->Create(CPanelBrowserNavigate::IDD,this);

	m_rectHead = CRect(POS_ADIMG_LEFT, POS_ADIMG_TOP, POS_ADIMG_LEFT+POS_ADIMG_SIGE, POS_ADIMG_TOP+POS_ADIMG_SIGE);

	m_editDescription.EnableWindow(TRUE);
	m_editDescription.SetReadOnly(TRUE);
	m_editDescription.ShowWindow(SW_HIDE);
#endif
	//LOGFONT pLogFont;
	//m_editDescription.GetFont()->GetLogFont(&pLogFont);
	//USES_CONVERSION;
	//strcpy(pLogFont.lfFaceName,W2A_ACP(theFontFamily.c_str()));
	//CFont pFont; 
	//pFont.CreateFontIndirect(&pLogFont);
	//m_editDescription.SetFont(&pFont);

	//CImageEx theBtnImage;
	//HGDIOBJ pBtnBitmap[3];

	//theBtnImage.LoadResource(IDB_BITMAP_TAB_CHATRIGHT);
	m_btnMyDepartment.SetWindowText(_T(""));
	m_btnMyDepartment.SetAutoSize(false);
	m_btnMyDepartment.Load(IDB_PNG_TAB_DEP);
	m_btnMyDepartment.Invalidate();
	m_btnMyContacts.SetWindowText(_T(""));
	m_btnMyContacts.SetAutoSize(false);
	m_btnMyContacts.Load(IDB_PNG_TAB_CONTACT);
	m_btnMyContacts.ShowWindow(SW_HIDE);
	m_btnMySession.SetWindowText(_T(""));
	m_btnMySession.SetAutoSize(false);
	m_btnMySession.Load(IDB_PNG_TAB_SES);
	m_btnMyEnterprise.SetWindowText(_T(""));
	m_btnMyEnterprise.SetAutoSize(false);
	m_btnMyEnterprise.Load(IDB_PNG_TAB_ENT);
	m_btnMyEnterprise.ShowWindow(SW_HIDE);
	m_pDlgMyGroup = new CDlgMyGroup(this);
	m_pDlgMyGroup->Create(CDlgMyGroup::IDD, this);
	m_pDlgMyGroup->ShowWindow(SW_SHOW);
	m_pDlgMyContacts = new CDlgMyContacts(this);
	m_pDlgMyContacts->Create(CDlgMyContacts::IDD, this);
	m_pDlgMyContacts->ShowWindow(SW_HIDE);
	m_pDlgMySession = new CDlgMySession(this);
	m_pDlgMySession->Create(CDlgMySession::IDD, this);
	m_pDlgMySession->ShowWindow(SW_HIDE);
	m_pDlgMyEnterprise = new CDlgMyEnterprise(this);
	m_pDlgMyEnterprise->Create(CDlgMyEnterprise::IDD, this);
	m_pDlgMyEnterprise->ShowWindow(SW_HIDE);
	if (theApp.IsLogonVisitor())
	{
		m_btnMyDepartment.ShowWindow(SW_HIDE);
		m_btnMyContacts.ShowWindow(SW_HIDE);
		m_btnMyEnterprise.ShowWindow(SW_HIDE);
		OnBnClickedButtonSession();
	}else
	{
		OnBnClickedButtonDepartment();
	}
	m_btnSwitchFrame.SetDrawPanelRgn(false);
	m_btnSwitchFrame.SetWindowText(_T(""));
	m_btnSwitchFrame.SetToolTipText(_T("切换到应用工作区（网页浏览器）"));
	m_btnSwitchFrame.SetDrawTrianglePic(3,RGB(250,250,250),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	m_btnSwitchFrame.ShowWindow(SW_SHOW);
#ifndef USES_MIN_UI
	m_btnNewApp.SetDrawPanelRgn(false);
	m_btnNewApp.SetWindowText(_T(""));
	m_btnNewApp.SetToolTipText(_T("添加应用"));
	m_btnNewApp.SetDrawNewPic(true,RGB(250,250,250),-1,-1,-1,2,11);
	m_btnNewApp.ShowWindow(theApp.GetEntManagerurl().empty()?SW_HIDE:SW_SHOW);

	m_btnSearchTrackDel.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSearch, 0xffff);
	m_btnSearchTrackDel.SetAutoSize(false);
	m_btnSearchTrackDel.Load(IDB_PNG_HOT_DELETE);
	m_btnSearchTrackDel.SetToolTipText(_T("删除历史记录"));

	m_btnSearchTrackCall.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSearch, 0xffff);
	m_btnSearchTrackCall.SetAutoSize(false);
	m_btnSearchTrackCall.Load(IDB_PNG_HOT_CALL);
	m_btnSearchTrackCall.SetToolTipText(_T("呼叫"));
	//m_treeSearch.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_treeSearch.ModifyStyle(0, TVS_SINGLEEXPAND);
	m_treeSearch.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeSearch.SetItemHeight(40);
	m_treeSearch.SetIconSize(32,32);
	m_treeSearch.SetCallback(&theApp);
	m_treeSearch.ShowWindow(SW_HIDE);

#ifdef USES_EBCOM_TEST
	const CEBString sOutDesc = theEBClientCore->EB_Description.GetBSTR();
#else
	const std::string sOutDesc = theEBAppClient.EB_GetDescription();
#endif
	this->GetDlgItem(IDC_EDIT_DESCRIPTION)->SetWindowText(sOutDesc.c_str());
#endif

#ifdef USES_EBCOM_TEST
	(((CEBDispatch*)(CEBCoreEventsSink*)this))->AddRef();	// 增加计数，避免删除自己
	//theEBClientCore->EB_SetCallBack((IDispatch*)(CEBCoreEventsSink*)this);

	((CEBSearchEventsSink*)this)->AddRef();	// 增加计数，避免删除自己
	((CEBCoreEventsSink*)this)->m_hFireWnd = this->GetSafeHwnd();
	((CEBCoreEventsSink*)this)->DispEventAdvise(theEBClientCore);
	OnMessageLogonSuccess(0,0);
#else
	theEBAppClient.EB_SetMsgHwnd(this->GetSafeHwnd());
	OnMessageLogonSuccess(0,0);
#endif

#ifndef USES_MIN_UI
	//theBtnImage.LoadResource(IDB_BITMAP_BTN_6821);
	//theBtnImage.Cut(3, pBtnBitmap);
	//m_btnCallUser.SetSkin(pBtnBitmap[0], pBtnBitmap[2], pBtnBitmap[1]);
#ifdef USES_EBCOM_TEST
	const CEBString sUserName = theEBClientCore->EB_UserName.GetBSTR();
	const CEBString sSettingEnterprise = theEBSetting->Enterprise.GetBSTR();
#else
	const CEBString sUserName = theEBAppClient.EB_GetUserName();
	const CEBString sSettingEnterprise = theSetting.GetEnterprise();
#endif
	CString sWindowText;
	if (theApp.IsLogonVisitor())
		sWindowText.Format(_T("游客-%s"),sUserName.c_str());
	else if (!theApp.GetProductName().IsEmpty())
		sWindowText.Format(_T("%s-%s(%s)"),theApp.GetProductName(),sUserName.c_str(),theApp.GetLogonAccount());
	else
		sWindowText.Format(_T("%s-%s(%s)"),sSettingEnterprise.c_str(),sUserName.c_str(),theApp.GetLogonAccount());
	this->SetWindowText(sWindowText);
	int nScreenWidth = 0;
	int nScreenHeight = 0;
	theApp.GetScreenSize(nScreenWidth, nScreenHeight);
	UINT nX = max(0,GetPrivateProfileInt(_T("default"),_T("main-x"),(nScreenWidth-LEN_DLG_WIDTH)/2,theApp.GetUserSettingIniFile()));
	UINT nY = max(0,GetPrivateProfileInt(_T("default"),_T("main-y"),(nScreenHeight-LEN_DLG_HEIGHT)/2,theApp.GetUserSettingIniFile()));
	UINT nW = max(LEN_DLG_WIDTH,GetPrivateProfileInt(_T("default"),_T("main-w"),LEN_DLG_WIDTH,theApp.GetUserSettingIniFile()));
	UINT nH = max(LEN_DLG_WIDTH,GetPrivateProfileInt(_T("default"),_T("main-h"),LEN_DLG_HEIGHT,theApp.GetUserSettingIniFile()));
	nX = min((nScreenWidth-LEN_DLG_WIDTH),nX);
	nY = min((nScreenHeight-LEN_DLG_HEIGHT),nY);
	nW = min(nScreenWidth,nW);
	nH = min(nScreenHeight,nH);
	MoveWindow(nX, nY, nW, nH);
	if (nW>=nScreenWidth && nH>=nScreenHeight)
	{
		this->SetToolTipText(IDC_BUTTON_MAX2,_T("向下还原"));
		m_btnMax.Load(IDB_PNG_RESTORE);
	}
	m_bOpenOk = true;
#endif
	SetCircle(false);

	// 未处理呼叫
	m_btnMySession.SetTextLeft(-40);
	m_btnMySession.SetTextTop(-2);
	m_btnMySession.SetHotTextColor(RGB(255,0,128)); 
	m_btnMySession.SetNorTextColor(RGB(255,0,128)); 

#ifndef USES_MIN_UI
	// 系统托盘
	SetTimer(TIMERID_SHOW_EMOTIPN,12*1000,NULL);
	CString sTrayText;
	if (theApp.IsLogonVisitor())
		sTrayText.Format(_T("游客-%s-%s"), sUserName.c_str(),GetLineStateText(nOutLineState));
	else
		sTrayText.Format(_T("%s(%s)-%s"), sUserName.c_str(),theApp.GetLogonAccount(),GetLineStateText(nOutLineState));
	m_trayIconData.cbSize = sizeof(NOTIFYICONDATA);
	m_trayIconData.hIcon = m_hIcon;
	m_trayIconData.hWnd = this->GetSafeHwnd();
	m_trayIconData.uCallbackMessage = WM_TRAYICON_NOTIFICATION;
	m_trayIconData.uFlags = NIF_TIP|NIF_MESSAGE|NIF_ICON;
	m_trayIconData.uID = 120;
	lstrcpy(m_trayIconData.szTip, sTrayText);
	Shell_NotifyIcon(NIM_ADD, &m_trayIconData);

	if (theApp.GetAutoOpenSubId()>0)
	{
		SetTimer(TIMERID_AUTO_OPEN_SUBID,1000,NULL);
	}
	if (theApp.GetAutoHideMainFrame() || theApp.GetHideMainFrame())
	{
		this->ShowWindow(SW_HIDE);
		//SetTimer(TIMERID_HIDE_MAIN_FRAME,1,NULL);
	}else
	{
		this->ShowWindow(SW_SHOW);
		this->SetForegroundWindow();
	}
#endif
	SetTimer(TIMERID_CHECK_MOUSE_POS,50,NULL);
	SetTimer(TIMERID_CHECK_CLOSE_DIALOG,5000,NULL);
	SetTimer(TIMERID_CHECK_MF_SUBFUNCINFO,2000,NULL);
	SetTimer(TIMERID_ONE_SECOND,1000,NULL);
	SetWindowPos(&CWnd::wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//void CDlgMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
//{
//	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
//	{
//		CAboutDlg dlgAbout;
//		dlgAbout.DoModal();
//	}
//	else
//	{
//		CEbDialogBase::OnSysCommand(nID, lParam);
//	}
//}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

//void CDlgMainFrame::OnPaint()
//{
//	if (IsIconic())
//	{
//		CPaintDC dc(this); // 用于绘制的设备上下文
//
//		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
//
//		// 使图标在工作区矩形中居中
//		int cxIcon = GetSystemMetrics(SM_CXICON);
//		int cyIcon = GetSystemMetrics(SM_CYICON);
//		CRect rect;
//		GetClientRect(&rect);
//		int x = (rect.Width() - cxIcon + 1) / 2;
//		int y = (rect.Height() - cyIcon + 1) / 2;
//
//		// 绘制图标
//		dc.DrawIcon(x, y, m_hIcon);
//	}
//	else
//	{
//		//CEbDialogBase::OnPaint();
//		DrawInfo();
//	}
//}

////当用户拖动最小化窗口时系统调用此函数取得光标
////显示。
//HCURSOR CDlgMainFrame::OnQueryDragIcon()
//{
//	return static_cast<HCURSOR>(m_hIcon);
//}
//

void CDlgMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CEbDialogBase::OnClose();
}

void CDlgMainFrame::OnDestroy()
{
#ifndef USES_MIN_UI
	m_pMainFuncButtonList.clear();
	Shell_NotifyIcon(NIM_DELETE, &m_trayIconData);
	UnregisterHotKey(this->m_hWnd,0x2013); 
	UnregisterHotKey(this->m_hWnd,0x2014); 
	UnregisterHotKey(this->m_hWnd,0x2015); 
#endif

	CEbDialogBase::OnDestroy();
#ifndef USES_MIN_UI
	m_menuFunc.DestroyMenu();
	m_menuState.DestroyMenu();
	m_menuTray.DestroyMenu();
	m_menuSkin.DestroyMenu();
#endif

#ifdef USES_EBCOM_TEST
	//AfxConnectionUnadvise(theEBClientCore,__uuidof(_IEBClientCoreEvents), GetIDispatch(FALSE),FALSE,dwAdvise);
	((CEBCoreEventsSink*)this)->DispEventUnadvise(theEBClientCore);
#else
	theEBAppClient.EB_SetMsgHwnd(NULL);
#endif

#ifndef USES_MIN_UI
	m_btnSearchTrackDel.DestroyWindow();
	m_btnSearchTrackCall.DestroyWindow();
	if (m_pDlgEditInfo)
	{
		m_pDlgEditInfo->DestroyWindow();
		delete m_pDlgEditInfo;
		m_pDlgEditInfo = NULL;
	}
#endif
	if (m_pDlgMyEnterprise)
	{
		m_pDlgMyEnterprise->DestroyWindow();
		delete m_pDlgMyEnterprise;
		m_pDlgMyEnterprise = NULL;
	}
	if (m_pDlgMyGroup)
	{
		m_pDlgMyGroup->DestroyWindow();
		delete m_pDlgMyGroup;
		m_pDlgMyGroup = NULL;
	}
	if (m_pDlgMyContacts)
	{
		m_pDlgMyContacts->DestroyWindow();
		delete m_pDlgMyContacts;
		m_pDlgMyContacts = NULL;
	}
	if (m_pDlgMySession)
	{
		m_pDlgMySession->DestroyWindow();
		delete m_pDlgMySession;
		m_pDlgMySession = NULL;
	}
	if (m_pDlgFrameList!=NULL)
	{
		m_pDlgFrameList->DestroyWindow();
		delete m_pDlgFrameList;
		m_pDlgFrameList = NULL;
	}
	if (m_pDlgAppFrame!=NULL)
	{
		m_pDlgAppFrame->DestroyWindow();
		delete m_pDlgAppFrame;
		m_pDlgAppFrame = NULL;
	}
	if (m_pDlgFileManager)
	{
		m_pDlgFileManager->DestroyWindow();
		delete m_pDlgFileManager;
		m_pDlgFileManager = NULL;
	}
#ifndef USES_MIN_UI
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
#endif
	//if (m_pPanelBrowserNavigate!=0)
	//{
	//	m_pPanelBrowserNavigate->DestroyWindow();
	//	delete m_pPanelBrowserNavigate;
	//	m_pPanelBrowserNavigate = 0;
	//}
	if (m_pViewContactInfo!=0)
	{
		m_pViewContactInfo->DestroyWindow();
		delete m_pViewContactInfo;
		m_pViewContactInfo = NULL;
	}

}

void CDlgMainFrame::DeleteDlgIncomingCall(eb::bigint sFromAccount)
{
	CDlgIncomingCall::pointer pDlgIncomingCall;
	if (m_pIncomingCallList.find(sFromAccount, pDlgIncomingCall, true))
	{
		pDlgIncomingCall->DestroyWindow();
	}
}
void CDlgMainFrame::DeleteDlgAlertingCall(eb::bigint sFromAccount)
{
	CDlgAlertingCall::pointer pDlgAlertingCall;
	if (m_pAlertingCallList.find(sFromAccount, pDlgAlertingCall, true))
	{
		pDlgAlertingCall->DestroyWindow();
	}
}

void CDlgMainFrame::ShowDlgFileManager(void)
{
	if (m_pDlgFileManager == NULL)
	{
		AFX_SWITCH_INSTANCED();
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		m_pDlgFileManager = new CDlgFileManager(pParent);
		m_pDlgFileManager->Create(CDlgFileManager::IDD, pParent);
		int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
		const int const_dlg_width = 480;
		const int const_dlg_height = 380;
		CRect rect;
		rect.right = m_nScreenWidth;
		rect.left = rect.right-const_dlg_width;
		rect.top = 0;
		rect.bottom = rect.top + const_dlg_height;
		m_pDlgFileManager->MoveWindow(&rect);
	//}else
	//{
	//	//m_pDlgFileManager->SetCtrlColor();
	}
	m_pDlgFileManager->ShowWindow(SW_SHOW);
	m_pDlgFileManager->SetForegroundWindow();
	m_pDlgFileManager->SetCircle();
}

LRESULT CDlgMainFrame::OnMessageAreaInfo(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_AreaInfo* pAreaInfo = (IEB_AreaInfo*)wParam;
#else
	const EB_AreaInfo* pAreaInfo = (const EB_AreaInfo*)wParam;
#endif
	const unsigned int nParameter = (unsigned int)lParam;
	switch (nParameter)
	{
	case 1:
	case 2:
	case 3:
	case 4:
		{
#ifndef USES_MIN_UI
			if (m_pDlgEditInfo!=NULL)
			{
#ifdef USES_EBCOM_TEST
				EB_AreaInfo pAreaInfoTemp(pAreaInfo);
				m_pDlgEditInfo->OnAreaInfo(&pAreaInfoTemp,nParameter);
#else
				m_pDlgEditInfo->OnAreaInfo(pAreaInfo,nParameter);
#endif
			}
#endif
		}break;
	default:
		break;
	}
	return 0;
}

LRESULT CDlgMainFrame::OnMessageBroadcastMsg(WPARAM wParam, LPARAM lParam)
{
	// 广播消息；
#ifdef USES_EBCOM_TEST
	IEB_AccountInfo* pAccountInfo = (IEB_AccountInfo*)wParam;
	const eb::bigint nFromUserid = pAccountInfo->UserId;
	const CEBString sFromAccount(pAccountInfo->Account.GetBSTR());
	IEB_APMsgInfo* pApMsgInfo = (IEB_APMsgInfo*)lParam;
	const CEBString sMsgName(pApMsgInfo->MsgName.GetBSTR());
	const CEBString sMsgContent(pApMsgInfo->MsgContent.GetBSTR());
#else
	const EB_AccountInfo* pAccountInfo = (const EB_AccountInfo*)wParam;
	const eb::bigint nFromUserid = pAccountInfo->GetUserId();
	const CEBString sFromAccount(pAccountInfo->GetAccount());
	const EB_APMsgInfo* pApMsgInfo = (const EB_APMsgInfo*)lParam;
	const CEBString sMsgName(pApMsgInfo->m_sMsgName);
	const CEBString sMsgContent(pApMsgInfo->m_sMsgContent);

#endif

	// emp_code=-1,from_type=msg-type
	const int nMsgType = EBC_MSG_TYPE_BC_MSG;
	CString sSql;
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type,dep_name) ")\
		_T("VALUES(%lld,0,-1,%lld,'%s',%d,'%s')"),
		pApMsgInfo->m_nMsgId,nFromUserid,sFromAccount.c_str(),nMsgType,libEbc::ACP2UTF8(sMsgName.c_str()).c_str());
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = pApMsgInfo->m_nMsgId;
		pCallRecordInfo->m_sGroupName = sMsgName;	// *
		pCallRecordInfo->m_sGroupCode = 0;
		pCallRecordInfo->m_sMemberCode = -1;
		pCallRecordInfo->m_nFromUserId = nFromUserid;
		pCallRecordInfo->m_sFromAccount = sFromAccount;
		pCallRecordInfo->m_nFromType = nMsgType;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}

	CString sContent;
	if (theApp.GetGroupMsgSugId()>0)
		sContent.Format(_T("%s<br/><a href=\"eb-open-subid://%lld,tab_type=bc_msg\">查看我的消息</a>"),sMsgContent.c_str(),theApp.GetGroupMsgSugId());

	CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
	CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
	pFuncWindow->m_bDisableContextMenu = true;
	pFuncWindow->m_bBroadcastMsg = true;
	pFuncWindow->m_nMsgType = nMsgType;
	pFuncWindow->m_bOpenNewClose = true;
	pFuncWindow->m_sTitle = sMsgName;
	if (theApp.GetGroupMsgSugId()>0)
		pFuncWindow->m_sFuncUrl = libEbc::ACP2UTF8(sContent);
	else
		pFuncWindow->m_sFuncUrl = libEbc::ACP2UTF8(sMsgContent.c_str());
	pFuncWindow->m_nWidth = 250;
	pFuncWindow->m_nHeight = 180;
	CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW,true);
	pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
	pEbcMsgInfo->m_pParent = pParent;
	pEbcMsgInfo->m_bSetForegroundWindow = true;
	m_pEbcMsgList.add(pEbcMsgInfo);

	return 0;
}

LRESULT CDlgMainFrame::OnMessageNewVersion(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_VersionInfo* pVersionInfo = (IEB_VersionInfo*)wParam;
	// 已经下载自动更新打包文件，在下次重启时，会自动更新；
	const CString sPrevNewVersionFile = theApp.EBC_GetProfileString("new_version",_T("file"));
	if (PathFileExists(sPrevNewVersionFile) && pVersionInfo->VersionFile != (_bstr_t)(LPCTSTR)sPrevNewVersionFile)
	{
		// 删除前一个更新文件未处理
		DeleteFile(sPrevNewVersionFile);
	}
	theApp.EBC_SetProfileString(_T("new_version"),_T("version"),CEBString(pVersionInfo->Version.GetBSTR()).c_str());
	theApp.EBC_SetProfileString(_T("new_version"),_T("desc"),CEBString(pVersionInfo->Description.GetBSTR()).c_str());
	theApp.EBC_SetProfileString(_T("new_version"),_T("time"),CEBString(pVersionInfo->UpdateTime.GetBSTR()).c_str());
	theApp.EBC_SetProfileString(_T("new_version"),_T("file"),CEBString(pVersionInfo->VersionFile.GetBSTR()).c_str());
	theApp.EBC_SetProfileInt(_T("new_version"),_T("type"),pVersionInfo->UpdateType);
	SetTimer(TIMERID_NEW_VERSION,1000,NULL);
#else
	const EB_VersionInfo* pNewVersionInfo = (const EB_VersionInfo*)wParam;
	// 已经下载自动更新打包文件，在下次重启时，会自动更新；
	const CString sPrevNewVersionFile = theApp.EBC_GetProfileString("new_version",_T("file"));
	if (PathFileExists(sPrevNewVersionFile) && sPrevNewVersionFile!=pNewVersionInfo->m_sVersionFile.c_str())
	{
		// 删除前一个更新文件未处理
		DeleteFile(sPrevNewVersionFile);
	}
	theApp.EBC_SetProfileString(_T("new_version"),_T("version"),pNewVersionInfo->m_sVersion.c_str());
	theApp.EBC_SetProfileString(_T("new_version"),_T("desc"),pNewVersionInfo->m_sDescription.c_str());
	theApp.EBC_SetProfileString(_T("new_version"),_T("time"),pNewVersionInfo->m_sUpdateTime.c_str());
	theApp.EBC_SetProfileString(_T("new_version"),_T("file"),pNewVersionInfo->m_sVersionFile.c_str());
	theApp.EBC_SetProfileInt(_T("new_version"),_T("type"),pNewVersionInfo->m_nUpdateType);
	SetTimer(TIMERID_NEW_VERSION,1000,NULL);
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageServerChange(WPARAM wParam, LPARAM lParam)
{
	// 会话无效，提示重新登录
#ifdef USES_EBCOM_TEST
	const int nServerState = (int)wParam;
#else
	const CEBCallbackInterface::SERVER_STATE nServerState = (CEBCallbackInterface::SERVER_STATE)wParam;
#endif
	if (nServerState == 1)	// CEBCallbackInterface::SERVER_STOPED
	{
		CDlgMessageBox::EbMessageBox(this,"系统正在更新维护：",_T("成功后会自动重新登录，请稍等..."),CDlgMessageBox::IMAGE_WARNING,10);
	//}else if (nServerState == 3)	// CEBCallbackInterface::SERVER_RESTART)
	//{
	//	// **SDK内部会自动重新登录；
	//	//CDlgMessageBox::EbMessageBox(this,"更新成功：",_T("正在重新登录..."),CDlgMessageBox::IMAGE_WARNING,10);
	//	//SetTimer(TIMERID_RELOGIN,5000,NULL);
	}
	return 0;
}
LRESULT CDlgMainFrame::OnMessageStateCode(WPARAM wParam, LPARAM lParam)
{
	// 会话无效，在这里重新登录
	const EB_STATE_CODE nStateCode = (EB_STATE_CODE)wParam;
	if (EB_STATE_APPID_KEY_ERROR==nStateCode || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nStateCode)
		return 0;
	switch (nStateCode)
	{
	case EB_STATE_OK:
		break;
	case EB_STATE_MAX_UG_ERROR:
		CDlgMessageBox::EbMessageBox(this,"已经超过最在联系人分组数量：",_T(""),CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_MAX_CONTACT_ERROR:
		CDlgMessageBox::EbMessageBox(this,"已经超过最在联系人数量：",_T(""),CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_OAUTH_FORWARD:
		CDlgMessageBox::EbMessageBox(this,"操作成功，等待对方验证！",_T(""),CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_ACCOUNT_FREEZE:
		CDlgMessageBox::EbMessageBox(this,"帐号已经被临时冻结：","请联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_ACCOUNT_NOT_EXIST:
		CDlgMessageBox::EbMessageBox(this,"帐号不存在：","请联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
		break;
	default:
		{
			CString sText;
			sText.Format(_T("错误代码 %d："),(int)nStateCode);
			CDlgMessageBox::EbMessageBox(this,sText,_T("请重试或联系客服！"),CDlgMessageBox::IMAGE_WARNING,5);
		}break;
	}
	return 0;
}

LRESULT CDlgMainFrame::OnMessageOnlineAnother(WPARAM wParam, LPARAM lParam)
{
	// 已经在其他地方登录，退出当前连接；
	if (theApp.m_pHandler!=NULL)
		theApp.m_pHandler->OnLogonAnother();
	SetTimer(TIMERID_LOGOUT,10,NULL);
	return 0;
}
LRESULT CDlgMainFrame::OnMessageLogoutResponse(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDlgMainFrame::OnMessageLogonSuccess(WPARAM wParam, LPARAM lParam)
{
	if (wParam!=0)
	{
#ifdef USES_EBCOM_TEST
		unsigned long pEntManagerUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL);
#else
		unsigned long pEntManagerUrl = 0;
		theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL, &pEntManagerUrl);
#endif
		if (pEntManagerUrl != NULL && strlen((const char*)pEntManagerUrl)>0)
		{
			theApp.SetEntManagerurl((const char*)pEntManagerUrl);
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL,pEntManagerUrl);
#else
			theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL,pEntManagerUrl);
#endif
		}
	}

#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_LoadOrg();	// 加载组织结构、表情、头像
	if (theApp.IsLogonVisitor())
	{
		//this->GetDlgItem(IDC_BUTTON_MYCONTACTS)->EnableWindow(FALSE);
	}else
	{
		//theEBClientCore->EB_LoadContact();
	}
	const CEBString sHeadFile = theEBClientCore->EB_GetMyDefaultMemberHeadFile().GetBSTR();
#else
	theEBAppClient.EB_LoadOrg();	// 加载组织结构、表情、头像
	if (theApp.IsLogonVisitor())
	{
		//this->GetDlgItem(IDC_BUTTON_MYCONTACTS)->EnableWindow(FALSE);
	}else
	{
		//theEBAppClient.EB_LoadContact();
	}
	const CEBString sHeadFile = theEBAppClient.EB_GetMyDefaultMemberHeadFile();
#endif
#ifndef USES_MIN_UI
	if (!sHeadFile.empty())
	{
		this->Invalidate();
	}
#endif
	//SetTimer(TIMERID_LOADINFO, 2*1000, NULL);	// 改由SDK内部自动加载；
	theUpdateResetTimer = false;
#ifndef USES_MIN_UI
	KillTimer(TIMERID_CHECK_UPDATE);
	//SetTimer(TIMERID_CHECK_UPDATE,10*1000,NULL);	// 10秒测试检查
	SetTimer(TIMERID_CHECK_UPDATE,(1+rand()%5)*60*1000,NULL);	// 先1-5分钟检查
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageLogonTimeout(WPARAM wParam, LPARAM lParam)
{
	CDlgMessageBox::EbMessageBox(this,"登录超时！",_T("请重新登录。"),CDlgMessageBox::IMAGE_WARNING,5);
	//CDlgMessageBox::EbDoModal(this,"登录超时！",_T("请重新登录。"),CDlgMessageBox::IMAGE_WARNING);
	OnLogout();
	return 0;
}

LRESULT CDlgMainFrame::OnMessageLogonError(WPARAM wParam, LPARAM lParam)
{
	int nErrorCode = lParam;
	if (nErrorCode == EB_STATE_UNAUTH_ERROR)
		CDlgMessageBox::EbMessageBox(this,_T("该帐号邮箱未通过验证："),_T("请查收邮件，点击验证链接，完成注册！"),CDlgMessageBox::IMAGE_WARNING,5);
	else if (nErrorCode == EB_STATE_ACCOUNT_FREEZE)
		CDlgMessageBox::EbMessageBox(this,"帐号已经被临时冻结：","请联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
	else if (nErrorCode == EB_STATE_MAX_RETRY_ERROR)
		CDlgMessageBox::EbMessageBox(this,"错误次数太频繁，帐号被临时锁住：","请稍候再试！",CDlgMessageBox::IMAGE_WARNING,5);
	else
	{
		CString sText;
		sText.Format(_T("验证失败！（%d）"),nErrorCode);
		CDlgMessageBox::EbMessageBox(this,sText,_T("请重新登录。"),CDlgMessageBox::IMAGE_WARNING,5);
		//CDlgMessageBox::EbDoModal(this,sText,_T("请重新登录。"),CDlgMessageBox::IMAGE_WARNING);
		OnLogout();
	}
	return 0;
}

LRESULT CDlgMainFrame::OnMessageSInfoResponse(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nState = (EB_STATE_CODE)wParam;
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;
	const int nFlag = (int)lParam;
	if (nState==EB_STATE_ACC_PWD_ERROR)
	{
		CDlgMessageBox::EbMessageBox(this,_T("帐号密码错误！"),_T("请重新输入！"),CDlgMessageBox::IMAGE_ERROR);
		return 0;
	}else if (nFlag==1)
	{
		if (nState==EB_STATE_OK)
			CDlgMessageBox::EbMessageBox(this,_T("密码修改成功！"),_T(""),CDlgMessageBox::IMAGE_WARNING,3);
		else
		{
			CString sText;
			sText.Format(_T("密码修改失败！（%d）"),nState);
			CDlgMessageBox::EbMessageBox(this,sText,_T("请重试！"),CDlgMessageBox::IMAGE_WARNING,3);
		}
	}else if (nState==EB_STATE_OK)
	{
		CDlgMessageBox::EbMessageBox(this,_T("个人资料修改成功！"),_T(""),CDlgMessageBox::IMAGE_WARNING,3);
	}else
	{
		CString sText;
		sText.Format(_T("个人资料修改失败！（%d）"),nState);
		CDlgMessageBox::EbMessageBox(this,sText,_T("请重试！"),CDlgMessageBox::IMAGE_WARNING,3);
	}
#ifndef USES_MIN_UI

#ifdef USES_EBCOM_TEST
	m_editDescription.SetWindowText(CEBString(theEBClientCore->EB_Description.GetBSTR()).c_str());
	ChangeTrayText();
	this->Invalidate();
#else
	m_editDescription.SetWindowText(theEBAppClient.EB_GetDescription().c_str());
	ChangeTrayText();
	this->Invalidate();
#endif
#endif

	return 0;
}

//LRESULT CDlgMainFrame::OnMessageEnterRoom(WPARAM wParam, LPARAM lParam)
//{
//	const char * lpszCallId = (const char*)lParam;
//	EB_STATE_CODE nStateCode = (EB_STATE_CODE)lParam;
//	if (lpszCallId==NULL) return 1;
//	CDlgDialog::pointer pDlgDialog;
//	if (theApp.m_pDialogList.find(lpszCallId, pDlgDialog))
//	{
//		pDlgDialog->OnEnterRoom();
//	}
//	return 0;
//}
//LRESULT CDlgMainFrame::OnMessageExitRoom(WPARAM wParam, LPARAM lParam)
//{
//	const char * lpszCallId = (const char*)lParam;
//	if (lpszCallId==NULL) return 1;
//	theApp.m_pCallList.remove(lpszCallId);
//	theApp.m_pDialogList.remove(lpszCallId);
//	return 0;
//}

CDlgDialog::pointer CDlgMainFrame::GetCallIdDialog(eb::bigint nCallId, bool bRemove)
{
	return m_pDlgFrameList!=NULL?m_pDlgFrameList->GetCallIdDialog(nCallId, bRemove):NullDlgDialog;
}

LRESULT CDlgMainFrame::OnMessageUserEnterRoom(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_ChatAccountInfo* pChatAccountInfo = (IEB_ChatAccountInfo*)wParam;
	const eb::bigint sCallId = pChatAccountInfo->CallId;

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnUserEnterRoom(pAccountInfo);
	}
#else
	const CCrAccountInfo* pAccountInfo = (const CCrAccountInfo*)wParam;
	if (pAccountInfo==NULL) return 1;
	const eb::bigint sCallId = pAccountInfo->GetCallId();
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnUserEnterRoom(pAccountInfo);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageUserExitRoom(WPARAM wParam, LPARAM lParam)
{
	// ???
	bool bHangup = lParam==1;
#ifdef USES_EBCOM_TEST
	IEB_ChatAccountInfo* pChatAccountInfo = (IEB_ChatAccountInfo*)wParam;
	const eb::bigint sCallId = pChatAccountInfo->CallId;

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnUserExitRoom(pAccountInfo);
	}
#else
	const CCrAccountInfo* pAccountInfo = (const CCrAccountInfo*)wParam;
	if (pAccountInfo==NULL)
	{
		return 1;
	}
	const eb::bigint sCallId = pAccountInfo->GetCallId();

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnUserExitRoom(pAccountInfo);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageReceiveRich(WPARAM wParam, LPARAM lParam)
{
	CString sSoundFile;
	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	sndPlaySound(sSoundFile, SND_ASYNC);

#ifdef USES_EBCOM_TEST
	IEB_ChatRichInfo* pChatRichInfo = (IEB_ChatRichInfo*)wParam;
	const eb::bigint sCallId = pChatRichInfo->CallId;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,false);
	CString sFirstMsg;
	pDlgDialog->OnReceiveRich(pChatRichInfo,&sFirstMsg);
#else
	const CCrRichInfo * pCrMsgInfo = (const CCrRichInfo*)wParam;
	if (pCrMsgInfo==NULL) return 1;
	const eb::bigint sCallId = pCrMsgInfo->GetCallId();
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 1;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,false);
	CString sFirstMsg;
	pDlgDialog->OnReceiveRich(pCrMsgInfo,&sFirstMsg);
#endif
	//CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_RECEIVE_RICH);
	//pEbcMsgInfo->m_pEbCallInfo = pEbCallInfo;
	//m_pEbcMsgList.add(pEbcMsgInfo);

	this->GetParent()->PostMessage(m_nMsgId, EBUI_MSG_TYPE_RECEIVE_RICH, 0);
	if (!pDlgDialog->IsWindowVisible())
	{
		const bool bRet = m_pDlgFrameList->AddUnreadMsg(sCallId);
		if (bRet && m_pDlgFrameList->IsWindowVisible())
		{
			::FlashWindow(m_pDlgFrameList->GetSafeHwnd(), TRUE);
			return 0;
		}

		// 显示消息
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_pEbCallInfo = pEbCallInfo;
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->m_bOpenNewClose = true;
		pFuncWindow->m_sTitle = _T("聊天消息提醒");
		pFuncWindow->m_sFuncUrl = (LPCTSTR)sFirstMsg;
		pFuncWindow->m_nWidth = 250;
		pFuncWindow->m_nHeight = 180;
		CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
		pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
		pEbcMsgInfo->m_pParent = pParent;
		m_pEbcMsgList.add(pEbcMsgInfo);
	}
	return 0;
}

LRESULT CDlgMainFrame::OnMessageSendRich(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;
#ifdef USES_EBCOM_TEST
	IEB_ChatRichInfo* pChatRichInfo = (IEB_ChatRichInfo*)wParam;
	const eb::bigint sCallId = pChatRichInfo->CallId;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()==NULL)
	{
		return 1;
	}
	pDlgDialog->OnSendRich(pChatRichInfo,nState);
#else
	const CCrRichInfo * pCrMsgInfo = (const CCrRichInfo*)wParam;
	if (pCrMsgInfo==NULL) return 1;
	const eb::bigint sCallId = pCrMsgInfo->GetCallId();
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 1;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()==NULL)
	{
		return 1;
	}
	pDlgDialog->OnSendRich(pCrMsgInfo,nState);
#endif
	//if (!pDlgDialog->IsWindowVisible())
	//{
	//	// 显示消息
	//	CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
	//	CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
	//	pFuncWindow->m_pEbCallInfo = pEbCallInfo;
	//	pFuncWindow->m_bDisableContextMenu = true;
	//	pFuncWindow->m_bBroadcastMsg = true;
	//	pFuncWindow->m_bOpenNewClose = true;
	//	pFuncWindow->m_sTitle = _T("聊天消息提醒");
	//	pFuncWindow->m_sFuncUrl = (LPCTSTR)sFirstMsg;
	//	pFuncWindow->m_nWidth = 250;
	//	pFuncWindow->m_nHeight = 180;
	//	pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
	//}
	return 0;
}

LRESULT CDlgMainFrame::OnMessageSendingFile(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;
	switch (nState)
	{
	case EB_STATE_FILE_ALREADY_EXIST:
		return 0;
	case EB_STATE_OK:
		break;
	case EB_STATE_NOT_AUTH_ERROR:
		CDlgMessageBox::EbMessageBox(this,"系统不支持当前操作！","请联系客服！",CDlgMessageBox::IMAGE_WARNING,5);
		return 1;
	case EB_STATE_EXCESS_QUOTA_ERROR:
	case EB_STATE_FILE_BIG_LONG:
		CDlgMessageBox::EbMessageBox(this,"文件大小超出系统最大配额！","请发送更小文件或联系客服！",CDlgMessageBox::IMAGE_WARNING,5);
		return 1;
	case EB_STATE_TIMEOUT_ERROR:
		CDlgMessageBox::EbMessageBox(this,"文件发送超时！","请重新发送！",CDlgMessageBox::IMAGE_WARNING,5);
		return 1;
	default:
		{
			CString stext;
			stext.Format(_T("错误代码(%d)"),(int)nState);
			CDlgMessageBox::EbMessageBox(this,"文件发送失败！",stext,CDlgMessageBox::IMAGE_WARNING,5);
			return 1;
		}
	}
#ifdef USES_EBCOM_TEST
	IEB_ChatFileInfo* pCrFileInfo = (IEB_ChatFileInfo*)wParam;
	const eb::bigint sCallId = pCrFileInfo->CallId;
	const eb::bigint sResId = pCrFileInfo->ResId;
	if (sResId>0)
	{
		ShowDlgFileManager();
		m_pDlgFileManager->OnSendingFile(pCrFileInfo);
		m_pDlgFileManager->OnBnClickedButtonTraning();
		//return 0;
	}
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
	pDlgDialog->OnSendingFile(pCrFileInfo);

	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(sCallId, pDlgDialog))
	//{
	//	pDlgDialog->OnSendingFile(pCrFileInfo);
	//}
#else
	const CCrFileInfo * pCrFileInfo = (const CCrFileInfo*)wParam;
	if (pCrFileInfo==NULL) return 1;
	const eb::bigint sCallId = pCrFileInfo->GetCallId();
	const eb::bigint sResId = pCrFileInfo->m_sResId;
	if (sResId>0)
	{
		ShowDlgFileManager();
		m_pDlgFileManager->OnSendingFile(pCrFileInfo);
		m_pDlgFileManager->OnBnClickedButtonTraning();
		//return 0;
	}
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 1;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
	pDlgDialog->OnSendingFile(pCrFileInfo);

	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(sCallId, pDlgDialog))
	//{
	//	pDlgDialog->OnSendingFile(pCrFileInfo);
	//}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageSendedFile(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;
	CString sSoundFile;
	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	sndPlaySound(sSoundFile, SND_ASYNC);

#ifdef USES_EBCOM_TEST
	IEB_ChatFileInfo* pCrFileInfo = (IEB_ChatFileInfo*)wParam;
	const eb::bigint sCallId = pCrFileInfo->CallId;
	const eb::bigint sReceiveAccount = pCrFileInfo->ReceiveAccount;
	const eb::bigint sResId = pCrFileInfo->ResId;
	if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL && (sReceiveAccount==0 || sReceiveAccount==theApp.GetLogonUserId()))
		{
			m_pDlgFileManager->DeleteDlgTranFile(pCrFileInfo->MsgId);
			if (m_pDlgFileManager->IsEmpty())
			{
				m_pDlgFileManager->ShowWindow(SW_HIDE);
			}
		}
		//m_pDlgFileManager->OnSentFile(pChatRoomFileInfo);
		//return 0;
	}
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(lpszCallId,pEbCallInfo))
	//{
	//	return 1;
	//}
	//pEbCallInfo->m_tLastTime = time(0);

	if (sReceiveAccount>0)
	{
		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
		if (pDlgDialog.get()!=NULL)
		{
			pDlgDialog->OnSentFile(pCrFileInfo);
		}
	}
#else
	const CCrFileInfo * pCrFileInfo = (const CCrFileInfo*)wParam;
	if (pCrFileInfo==NULL) return 1;
	const eb::bigint sCallId = pCrFileInfo->GetCallId();
	const eb::bigint sResId = pCrFileInfo->m_sResId;
	if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL && (pCrFileInfo->m_sReceiveAccount==0 || pCrFileInfo->m_sReceiveAccount==theEBAppClient.EB_GetLogonUserId()))
		{
			m_pDlgFileManager->DeleteDlgTranFile(pCrFileInfo->m_nMsgId);
			if (m_pDlgFileManager->IsEmpty())
			{
				m_pDlgFileManager->ShowWindow(SW_HIDE);
			}
		}
		//m_pDlgFileManager->OnSentFile(pChatRoomFileInfo);
		//return 0;
	}
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(lpszCallId,pEbCallInfo))
	//{
	//	return 1;
	//}
	//pEbCallInfo->m_tLastTime = time(0);
	if (pCrFileInfo->m_sReceiveAccount>0)
	{
		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
		if (pDlgDialog.get()!=NULL)
		{
			pDlgDialog->OnSentFile(pCrFileInfo);
		}
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageCancelFile(WPARAM wParam, LPARAM lParam)
{
	CString sSoundFile;
	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	sndPlaySound(sSoundFile, SND_ASYNC);
#ifdef USES_EBCOM_TEST
	IEB_ChatFileInfo* pCrFileInfo = (IEB_ChatFileInfo*)wParam;
	const eb::bigint sCallId = pCrFileInfo->CallId;
	const eb::bigint nReceiveAccount = pCrFileInfo->ReceiveAccount;
	//const eb::bigint nSendToUid = pCrFileInfo->SendTo;
	const eb::bigint sResId = pCrFileInfo->ResId;
	if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL && nReceiveAccount==0)
			m_pDlgFileManager->DeleteDlgTranFile(pCrFileInfo->MsgId);
		//m_pDlgFileManager->OnCancelFile(pCrFileInfo);
		//return 0;
	}
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(lpszCallId,pEbCallInfo))
	//{
	//	return 1;
	//}
	//pEbCallInfo->m_tLastTime = time(0);

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnCancelFile(pCrFileInfo);
	}
	const tstring sFileName = libEbc::GetFileName(CEBString(pCrFileInfo->FileName.GetBSTR()));
#else
	const CCrFileInfo * pCrFileInfo = (const CCrFileInfo*)wParam;
	if (pCrFileInfo==NULL) return 1;
	const eb::bigint sCallId = pCrFileInfo->GetCallId();
	const eb::bigint sResId = pCrFileInfo->m_sResId;
	const eb::bigint nReceiveAccount = pCrFileInfo->m_sReceiveAccount;
	//const eb::bigint nSendToUid = pCrFileInfo->m_sSendTo;
	if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL && nReceiveAccount==0)
			m_pDlgFileManager->DeleteDlgTranFile(pCrFileInfo->m_nMsgId);
		//m_pDlgFileManager->OnCancelFile(pCrFileInfo);
		//return 0;
	}
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(lpszCallId,pEbCallInfo))
	//{
	//	return 1;
	//}
	//pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnCancelFile(pCrFileInfo);
	}
	const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);
#endif

	CEBCCallInfo::pointer pEbCallInfo;
	if (nReceiveAccount!=0 && theApp.m_pCallList.find(sCallId,pEbCallInfo))	// sReceiveAccount==0是主动取消
	{
		if (pDlgDialog.get()==0 || !pDlgDialog->IsWindowVisible())
		{
			CString sFirstMsg;
			if (pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			{
				sFirstMsg.Format(_T("用户取消发送文件：%s"),sFileName.c_str());
				const CString sTemp(libEbc::ACP2UTF8(sFirstMsg).c_str());
				sFirstMsg.Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-group://%lld\">%s</a>"),sTemp,pEbCallInfo->m_pCallInfo.m_sGroupCode,libEbc::ACP2UTF8("查看聊天内容").c_str());
			}else
			{
				sFirstMsg.Format(_T("用户[%s]取消发送文件：%s"),pEbCallInfo->m_pCallInfo.GetFromAccount().c_str(),sFileName.c_str());
				const CString sTemp(libEbc::ACP2UTF8(sFirstMsg).c_str());
				sFirstMsg.Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-account://%lld\">%s</a>"),sTemp,pEbCallInfo->m_pCallInfo.GetFromUserId(),libEbc::ACP2UTF8("查看聊天内容").c_str());
			}

			// 显示消息
			CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
			CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
			pFuncWindow->m_pEbCallInfo = pEbCallInfo;
			pFuncWindow->m_bDisableContextMenu = true;
			pFuncWindow->m_bBroadcastMsg = true;
			pFuncWindow->m_bOpenNewClose = true;
			pFuncWindow->m_sTitle = _T("文件消息提醒");
			pFuncWindow->m_sFuncUrl = (LPCTSTR)sFirstMsg;
			pFuncWindow->m_nWidth = 250;
			pFuncWindow->m_nHeight = 180;
			CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
			pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
			pEbcMsgInfo->m_pParent = pParent;
			m_pEbcMsgList.add(pEbcMsgInfo);
		}
	}


	//if (pDlgDialog.get()==NULL || !pDlgDialog->IsWindowVisible())
	//{
	//	// 显示消息
	//	CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
	//	CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
	//	pFuncWindow->m_pEbCallInfo = pEbCallInfo;
	//	pFuncWindow->m_bDisableContextMenu = true;
	//	pFuncWindow->m_bBroadcastMsg = true;
	//	pFuncWindow->m_bOpenNewClose = true;
	//	pFuncWindow->m_sTitle = _T("文件消息提醒");
	//	pFuncWindow->m_sFuncUrl = (LPCTSTR)sFirstMsg;
	//	pFuncWindow->m_nWidth = 250;
	//	pFuncWindow->m_nHeight = 180;
	//	pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
	//}
	return 0;
}

LRESULT CDlgMainFrame::OnMessageReceivingFile(WPARAM wParam, LPARAM lParam)
{
	CString sSoundFile;
	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	sndPlaySound(sSoundFile, SND_ASYNC);

#ifdef USES_EBCOM_TEST
	IEB_ChatFileInfo* pCrFileInfo = (IEB_ChatFileInfo*)wParam;
	const eb::bigint sResId = pCrFileInfo->ResId;
	if (sResId>0)
	{
		ShowDlgFileManager();
		m_pDlgFileManager->OnReceivingFile(pCrFileInfo);
		m_pDlgFileManager->OnBnClickedButtonTraning();
	}
	const eb::bigint sCallId = pCrFileInfo->CallId;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,false);
	CString sFirstMsg;
	pDlgDialog->OnReceivingFile(pCrFileInfo,&sFirstMsg);
#else
	const CCrFileInfo * pCrFileInfo = (const CCrFileInfo*)wParam;
	if (pCrFileInfo==NULL) return 1;
	const eb::bigint sResId = pCrFileInfo->m_sResId;
	if (sResId>0)
	{
		ShowDlgFileManager();
		m_pDlgFileManager->OnReceivingFile(pCrFileInfo);
		m_pDlgFileManager->OnBnClickedButtonTraning();
	}
	const eb::bigint sCallId = pCrFileInfo->GetCallId();
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 1;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,false);
	CString sFirstMsg;
	pDlgDialog->OnReceivingFile(pCrFileInfo,&sFirstMsg);
#endif

	this->GetParent()->PostMessage(m_nMsgId, EBUI_MSG_TYPE_RECEIVING_FILE, 0);
	if (!pDlgDialog->IsWindowVisible())
	{
		m_pDlgFrameList->AddUnreadMsg(sCallId);
		if (m_pDlgFrameList->IsWindowVisible())
		{
			::FlashWindow(m_pDlgFrameList->GetSafeHwnd(), TRUE);
			return 0;
		}
		// 显示消息
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_pEbCallInfo = pEbCallInfo;
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->m_bOpenNewClose = true;
		pFuncWindow->m_sTitle = _T("文件消息提醒");
		pFuncWindow->m_sFuncUrl = (LPCTSTR)sFirstMsg;
		pFuncWindow->m_nWidth = 250;
		pFuncWindow->m_nHeight = 180;
		CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
		pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
		pEbcMsgInfo->m_pParent = pParent;
		m_pEbcMsgList.add(pEbcMsgInfo);
	}
	return 0;
}

LRESULT CDlgMainFrame::OnMessageReceivedFile(WPARAM wParam, LPARAM lParam)
{
	CString sSoundFile;
	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	sndPlaySound(sSoundFile, SND_ASYNC);

#ifdef USES_EBCOM_TEST
	IEB_ChatFileInfo* pCrFileInfo = (IEB_ChatFileInfo*)wParam;
	const eb::bigint sCallId = pCrFileInfo->CallId;
	const eb::bigint sResId = pCrFileInfo->ResId;
	const bool bOffFile = pCrFileInfo->OffFile==VARIANT_TRUE?true:false;
	if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL)
		{
			m_pDlgFileManager->OnReceivedFile(pCrFileInfo);
			if (m_pDlgFileManager->IsEmpty())
			{
				m_pDlgFileManager->ShowWindow(SW_HIDE);
			}
		}
	}
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(lpszCallId,pEbCallInfo))
	//{
	//	return 1;
	//}
	//pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnReceivedFile(pCrFileInfo);
	}

#else
	const CCrFileInfo * pCrFileInfo = (const CCrFileInfo*)wParam;
	if (pCrFileInfo==NULL) return 1;
	const eb::bigint sCallId = pCrFileInfo->GetCallId();
	const eb::bigint sResId = pCrFileInfo->m_sResId;
	const bool bOffFile = pCrFileInfo->m_bOffFile;
	if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL)
		{
			m_pDlgFileManager->OnReceivedFile(pCrFileInfo);
			if (m_pDlgFileManager->IsEmpty())
			{
				m_pDlgFileManager->ShowWindow(SW_HIDE);
			}
		}		
	}
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(lpszCallId,pEbCallInfo))
	//{
	//	return 1;
	//}
	//pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnReceivedFile(pCrFileInfo);
	}

#endif
	this->GetParent()->PostMessage(m_nMsgId, EBUI_MSG_TYPE_RECEIVED_FILE, (int)(bOffFile?1:0));
	return 0;
}

LRESULT CDlgMainFrame::OnMessageFilePercent(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_ChatFilePercent* pChatFilePercent = (IEB_ChatFilePercent*)wParam;
	const eb::bigint sCallId = pChatFilePercent->CallId;
	const eb::bigint sResId = pChatFilePercent->ResId;
	if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL)
			m_pDlgFileManager->SetFilePercent(pChatFilePercent);
	}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnFilePercent(pChatFilePercent);
	}

#else
	const CChatRoomFilePercent * pChatRoomFilePercent = (const CChatRoomFilePercent*)wParam;
	if (pChatRoomFilePercent==NULL) return 1;
	const eb::bigint sCallId = pChatRoomFilePercent->GetCallId();
	const eb::bigint sResId = pChatRoomFilePercent->m_sResId;
	if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL)
			m_pDlgFileManager->SetFilePercent(pChatRoomFilePercent);
	}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnFilePercent(pChatRoomFilePercent);
	}

#endif
	return 0;
}
LRESULT CDlgMainFrame::OnMessageSave2CloudDrive(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;
	CString sSoundFile;
	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	sndPlaySound(sSoundFile, SND_ASYNC);

#ifdef USES_EBCOM_TEST
	IEB_ChatFileInfo* pCrFileInfo = (IEB_ChatFileInfo*)wParam;
	const eb::bigint sCallId = pCrFileInfo->CallId;

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnSave2CloutDrive(pCrFileInfo,nState);
	}

#else
	const CCrFileInfo * pCrFileInfo = (const CCrFileInfo*)wParam;
	const eb::bigint sCallId = pCrFileInfo->GetCallId();

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnSave2CloutDrive(pCrFileInfo,nState);
	}

#endif
	return 0;
}
LRESULT CDlgMainFrame::OnMessageRDRequestResponse(WPARAM wParam, LPARAM lParam)
{
	CString sSoundFile;
	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	sndPlaySound(sSoundFile, SND_ASYNC);

#ifdef USES_EBCOM_TEST
	//IEB_VideoInfo * pVideoInfo = (IEB_VideoInfo*)wParam;
	//EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	//const eb::bigint sCallId = pVideoInfo->CallId;
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	//{
	//	return 0;
	//}
	//pEbCallInfo->m_tLastTime = time(0);
	//if (nState==EB_STATE_NOT_AUTH_ERROR)
	//{
	//	CDlgMessageBox::EbMessageBox(this,_T("当前版本不支持视频会议："),_T("请联系公司咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR,5);
	//}else if (nState==EB_STATE_EXCESS_QUOTA_ERROR)
	//{
	//	CDlgMessageBox::EbMessageBox(this,_T("已经超过版本视频会议用户数："),_T("请联系公司咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR,5);
	//}

	////CDlgVideoConference::pointer pDlgVideoConference;
	////if (theApp.m_pVideoConferenceList.find(pEbCallInfo->m_pCallInfo.GetCallId(), pDlgVideoConference))
	////{
	////	pDlgVideoConference->VRequestResponse(pVideoInfo,nState);
	////	return 0;
	////}
	//CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
	//pDlgDialog->VRequestResponse(pVideoInfo,nState);
#else
	const EB_RemoteDesktopInfo* pVideoInfo = (const EB_RemoteDesktopInfo*)wParam;
	int nState = lParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}
	pEbCallInfo->m_tLastTime = time(0);
	if (nState==EB_STATE_ALREADY_IN_REMOTE_DESKTOP)
	{
		CDlgMessageBox::EbMessageBox(this,_T("已经在远程桌面中："),_T("请联系对方退出后再试！"),CDlgMessageBox::IMAGE_ERROR,5);
	}
	
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
	pDlgDialog->RDRequestResponse(pVideoInfo,nState);
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageRDAckResponse(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	//IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	//EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	//const eb::bigint sCallId = pVideoInfo->CallId;
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	//{
	//	return 1;
	//}

	////CDlgVideoConference::pointer pDlgVideoConference;
	////if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	////{
	////	pDlgVideoConference->VAckResponse(pVideoInfo,nState);
	////	return 0;
	////}
	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(sCallId, pDlgDialog))
	//{
	//	pDlgDialog->VAckResponse(pVideoInfo,nState);
	//}
#else
	const EB_RemoteDesktopInfo* pVideoInfo = (const EB_RemoteDesktopInfo*)wParam;
	int nStateValue = lParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pVideoInfo->m_sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->RDAckResponse(pVideoInfo,nStateValue);
	}

#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageRDRequest(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	//IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	//IEB_UserVideoInfo* pUserVideoInfo = (IEB_UserVideoInfo*)lParam;
	//const eb::bigint sCallId = pVideoInfo->CallId;
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	//{
	//	return 0;
	//}
	//pEbCallInfo->m_tLastTime = time(0);

	////CDlgVideoConference::pointer pDlgVideoConference;
	////if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	////{
	////	pDlgVideoConference->VideoRequest(pVideoInfo,pUserVideoInfo);
	////	return 0;
	////}
	////if (pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
	//{
	//	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
	//	pDlgDialog->VideoRequest(pVideoInfo,pUserVideoInfo);
	//}
#else
	const EB_RemoteDesktopInfo* pVideoInfo = (const EB_RemoteDesktopInfo*)wParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}
	pEbCallInfo->m_tLastTime = time(0);

	//if (pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
	{
		CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
		pDlgDialog->RDRequest(pVideoInfo);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageRDAccept(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	//IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	//IEB_UserVideoInfo* pUserVideoInfo = (IEB_UserVideoInfo*)lParam;
	//const eb::bigint sCallId(pVideoInfo->CallId);
	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	//{
	//	return 0;
	//}

	////CDlgVideoConference::pointer pDlgVideoConference;
	////if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	////{
	////	pDlgVideoConference->VideoAccept(pVideoInfo,pUserVideoInfo);
	////	return 0;
	////}
	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(sCallId, pDlgDialog))
	//{
	//	pDlgDialog->VideoAccept(pVideoInfo,pUserVideoInfo);
	//}
#else
	const EB_RemoteDesktopInfo* pVideoInfo = (const EB_RemoteDesktopInfo*)wParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pVideoInfo->m_sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->RDAccept(pVideoInfo);
	}

#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageRDCancel(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	IEB_UserVideoInfo* pUserVideoInfo = (IEB_UserVideoInfo*)lParam;
	const eb::bigint sCallId(pVideoInfo->CallId);
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoCancel(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}

	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(sCallId, pDlgDialog))
	//{
	//	pDlgDialog->VideoCancel(pVideoInfo,pUserVideoInfo);
	//}
#else
	const EB_RemoteDesktopInfo* pVideoInfo = (const EB_RemoteDesktopInfo*)wParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pVideoInfo->m_sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->RDCancel(pVideoInfo);
	}

#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageRDEnd(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	IEB_UserVideoInfo* pUserVideoInfo = (IEB_UserVideoInfo*)lParam;
	const eb::bigint sCallId(pVideoInfo->CallId);
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoEnd(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}

	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(sCallId, pDlgDialog))
	//{
	//	pDlgDialog->VideoEnd(pVideoInfo,pUserVideoInfo);
	//}
#else
	const EB_RemoteDesktopInfo* pVideoInfo = (const EB_RemoteDesktopInfo*)wParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pVideoInfo->m_sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->RDEnd(pVideoInfo);
	}

#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageVRequestResponse(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;
#ifdef USES_EBCOM_TEST
	IEB_VideoInfo * pVideoInfo = (IEB_VideoInfo*)wParam;
	const eb::bigint sCallId = pVideoInfo->CallId;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}
	pEbCallInfo->m_tLastTime = time(0);
	if (nState==EB_STATE_NOT_AUTH_ERROR)
	{
		CDlgMessageBox::EbMessageBox(this,_T("当前版本不支持视频会议："),_T("请联系公司咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR,5);
	}else if (nState==EB_STATE_EXCESS_QUOTA_ERROR)
	{
		CDlgMessageBox::EbMessageBox(this,_T("已经超过版本视频会议用户数："),_T("请联系公司咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR,5);
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(pEbCallInfo->m_pCallInfo.GetCallId(), pDlgVideoConference))
	//{
	//	pDlgVideoConference->VRequestResponse(pVideoInfo,nState);
	//	return 0;
	//}
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
	pDlgDialog->VRequestResponse(pVideoInfo,nState);
#else
	const EB_VideoInfo* pVideoInfo = (const EB_VideoInfo*)wParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}
	pEbCallInfo->m_tLastTime = time(0);
	if (nState==EB_STATE_NOT_AUTH_ERROR)
	{
		CDlgMessageBox::EbMessageBox(this,_T("当前版本不支持个人视频会议："),_T("请联系公司客服咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR,5);
		//CDlgMessageBox::EbMessageBox(this,_T("当前授权不支持视频会议："),_T("请联系恩布公司咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR,5);
	}else if (nState==EB_STATE_EXCESS_QUOTA_ERROR)
	{
		CDlgMessageBox::EbMessageBox(this,_T("已经超过版本视频会议用户数："),_T("请联系公司客服咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR,5);
		//CDlgMessageBox::EbMessageBox(this,_T("已经超过授权视频会议用户数："),_T("请联系恩布公司咨询相关服务！"),CDlgMessageBox::IMAGE_ERROR,5);
	}
	
	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(pEbCallInfo->m_pCallInfo.GetCallId(), pDlgVideoConference))
	//{
	//	pDlgVideoConference->VRequestResponse(pVideoInfo,nState);
	//	return 0;
	//}
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
	pDlgDialog->VRequestResponse(pVideoInfo,nState);
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageVAckResponse(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;
#ifdef USES_EBCOM_TEST
	IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	const eb::bigint sCallId = pVideoInfo->CallId;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 1;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VAckResponse(pVideoInfo,nState);
	//	return 0;
	//}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VAckResponse(pVideoInfo,nStateValue);
	}

#else
	const EB_VideoInfo* pVideoInfo = (const EB_VideoInfo*)wParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(pVideoInfo->m_sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VAckResponse(pVideoInfo,nStateValue);
	//	return 0;
	//}

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pVideoInfo->m_sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VAckResponse(pVideoInfo,nState);
	}

#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageVideoRequest(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	IEB_UserVideoInfo* pUserVideoInfo = (IEB_UserVideoInfo*)lParam;
	const eb::bigint sCallId = pVideoInfo->CallId;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}
	pEbCallInfo->m_tLastTime = time(0);

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoRequest(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}
	//if (pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
	{
		CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
		pDlgDialog->VideoRequest(pVideoInfo,pUserVideoInfo);
	}
#else
	const EB_VideoInfo* pVideoInfo = (const EB_VideoInfo*)wParam;
	const EB_UserVideoInfo* pUserVideoInfo = (const EB_UserVideoInfo*)lParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}
	pEbCallInfo->m_tLastTime = time(0);

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(pVideoInfo->m_sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoRequest(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}
	//if (pEbCallInfo->m_pCallInfo.m_sGroupCode==0)
	{
		CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
		pDlgDialog->VideoRequest(pVideoInfo,pUserVideoInfo);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageVideoAccept(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	IEB_UserVideoInfo* pUserVideoInfo = (IEB_UserVideoInfo*)lParam;
	const eb::bigint sCallId(pVideoInfo->CallId);
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoAccept(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VideoAccept(pVideoInfo,pUserVideoInfo);
	}

#else
	const EB_VideoInfo* pVideoInfo = (const EB_VideoInfo*)wParam;
	const EB_UserVideoInfo* pUserVideoInfo = (const EB_UserVideoInfo*)lParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}
	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(pVideoInfo->m_sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoAccept(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pVideoInfo->m_sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VideoAccept(pVideoInfo,pUserVideoInfo);
	}

#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageVideoCancel(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	IEB_UserVideoInfo* pUserVideoInfo = (IEB_UserVideoInfo*)lParam;
	const eb::bigint sCallId(pVideoInfo->CallId);
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoCancel(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VideoCancel(pVideoInfo,pUserVideoInfo);
	}

#else
	const EB_VideoInfo* pVideoInfo = (const EB_VideoInfo*)wParam;
	const EB_UserVideoInfo* pUserVideoInfo = (const EB_UserVideoInfo*)lParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(pVideoInfo->m_sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoCancel(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pVideoInfo->m_sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VideoCancel(pVideoInfo,pUserVideoInfo);
	}

#endif
	return 0;
}
LRESULT CDlgMainFrame::OnMessageVideoTimeout(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	IEB_UserVideoInfo* pUserVideoInfo = (IEB_UserVideoInfo*)lParam;
	const eb::bigint sCallId(pVideoInfo->CallId);
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoCancel(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VideoTimeout(pVideoInfo,pUserVideoInfo);
	}

#else
	const EB_VideoInfo* pVideoInfo = (const EB_VideoInfo*)wParam;
	const EB_UserVideoInfo* pUserVideoInfo = (const EB_UserVideoInfo*)lParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pVideoInfo->m_sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VideoTimeout(pVideoInfo,pUserVideoInfo);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageVideoEnd(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_VideoInfo* pVideoInfo = (IEB_VideoInfo*)wParam;
	IEB_UserVideoInfo* pUserVideoInfo = (IEB_UserVideoInfo*)lParam;
	const eb::bigint sCallId(pVideoInfo->CallId);
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return 0;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoEnd(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VideoEnd(pVideoInfo,pUserVideoInfo);
	}

#else
	const EB_VideoInfo* pVideoInfo = (const EB_VideoInfo*)wParam;
	const EB_UserVideoInfo* pUserVideoInfo = (const EB_UserVideoInfo*)lParam;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo))
	{
		return 1;
	}

	//CDlgVideoConference::pointer pDlgVideoConference;
	//if (theApp.m_pVideoConferenceList.find(pVideoInfo->m_sCallId, pDlgVideoConference))
	//{
	//	pDlgVideoConference->VideoEnd(pVideoInfo,pUserVideoInfo);
	//	return 0;
	//}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pVideoInfo->m_sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->VideoEnd(pVideoInfo,pUserVideoInfo);
	}

#endif
	return 0;
}

void CDlgMainFrame::OnFileManager()
{
	if (m_pDlgFileManager == NULL || !m_pDlgFileManager->IsWindowVisible())
		this->ShowDlgFileManager();
	else if (m_pDlgFileManager->IsWindowVisible())
		m_pDlgFileManager->ShowWindow(SW_HIDE);

}
LRESULT CDlgMainFrame::OnMessageFileManager(WPARAM wParam, LPARAM lParam)
{
	OnFileManager();
	return 0;
}

LRESULT CDlgMainFrame::OnMessageUserStateChange(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_MemberInfo * pEBMemberInfo = (IEB_MemberInfo*)wParam;
	//const bool bIsOwnerMember = lParam==1?true:false;
	// 更新界面用户状况改变
	if (pEBMemberInfo != NULL && m_pDlgMyEnterprise != NULL)
	{
		m_pDlgMyEnterprise->EmployeeInfo(pEBMemberInfo,true);
	}
	if (pEBMemberInfo != NULL && m_pDlgMyGroup != NULL)
	{
		m_pDlgMyGroup->MyDepMemberInfo(pEBMemberInfo,true);
	}
	if (m_pDlgFrameList!=NULL)
	{
		m_pDlgFrameList->UserLineStateChange(pEBMemberInfo->GroupCode, pEBMemberInfo->MemberUserId, pEBMemberInfo->LineState);
	}
	//CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	//if (pDlgDialog.get()!=NULL)
	//{
	//}

#else
	const EB_MemberInfo * pMemberInfo = (const EB_MemberInfo*)wParam;
	//const bool bIsOwnerMember = lParam==1?true:false;

	// 更新界面用户状况改变
	if (pMemberInfo != NULL && m_pDlgMyEnterprise != NULL)
	{
		m_pDlgMyEnterprise->EmployeeInfo(pMemberInfo,true);
		//m_pDlgMyEnterprise->ChangeEmployeeInfo(pMemberInfo);
	}
	if (pMemberInfo != NULL && m_pDlgMyGroup != NULL)
	{
		m_pDlgMyGroup->MyDepMemberInfo(pMemberInfo,true);
		//m_pDlgMyGroup->ChangeEmployeeInfo(pMemberInfo);
	}
	if (m_pDlgFrameList!=NULL)
	{
		m_pDlgFrameList->UserLineStateChange(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId, pMemberInfo->m_nLineState);
	}
	//CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pMemberInfo->m_sGroupCode);
	//if (pDlgDialog.get()!=NULL)
	//{
	//}

#endif

	return 0;
}

void CDlgMainFrame::SaveCallRecord(eb::bigint sCallId,eb::bigint sGroupCode,const EB_AccountInfo& pFromAccountInfo)
{
	eb::bigint sEmpCode = pFromAccountInfo.m_pFromCardInfo.m_sMemberCode;
	CEBString sDepName = pFromAccountInfo.m_pFromCardInfo.m_sGroupName;
	CEBString sFromName = pFromAccountInfo.m_pFromCardInfo.m_sName;
	int nFromType = (int)pFromAccountInfo.m_pFromCardInfo.m_nAccountType;
	if (nFromType==(int)EB_ACCOUNT_TYPE_VISITOR)
		sFromName = pFromAccountInfo.m_pFromCardInfo.m_sAddress;
	CEBString sCompany = pFromAccountInfo.m_pFromCardInfo.m_sEnterprise;
	CEBString sTitle = pFromAccountInfo.m_pFromCardInfo.m_sTitle;
	eb::bigint sPhone = eb_atoi64(pFromAccountInfo.m_pFromCardInfo.m_sPhone.c_str());
	//CEBString sPhone = pFromAccountInfo.m_pFromCardInfo.m_sPhone;
	CEBString sTel = pFromAccountInfo.m_pFromCardInfo.m_sTel;
	CEBString sEmail = pFromAccountInfo.m_pFromCardInfo.m_sEmail;
	CString sSql;
	if (sGroupCode==0)
	{
#ifdef USES_EBCOM_TEST
		_bstr_t sMemberName = theEBClientCore->EB_GetMemberNameByMemberCode(pFromAccountInfo.m_pFromCardInfo.m_sMemberCode);
		if (sMemberName.length() > 0)
		{
			sFromName = sMemberName.GetBSTR();
			char lpszMemberCode[24];
			sprintf(lpszMemberCode,"%lld",pFromAccountInfo.m_pFromCardInfo.m_sMemberCode);
			sDepName = lpszMemberCode;	// ?
		}
#else
		if (theEBAppClient.EB_GetMemberNameByMemberCode(pFromAccountInfo.m_pFromCardInfo.m_sMemberCode,sFromName))
		{
			char lpszMemberCode[24];
			sprintf(lpszMemberCode,"%lld",pFromAccountInfo.m_pFromCardInfo.m_sMemberCode);
			sDepName = lpszMemberCode;	// ?
			//EB_GroupInfo::pointer pGroupInfo = theEBAppClient.GetGroupInfo(pMemberInfo->m_sGroupCode);
			//if (pGroupInfo.get() == NULL)
			//	sDepName = sGroupCode;
			//else
			//	sDepName = pGroupInfo->m_sGroupName.c_str();
		}
#endif
	}else
	{
#ifdef USES_EBCOM_TEST
		sDepName = theEBClientCore->EB_GetGroupName(sGroupCode).GetBSTR();
		if (sDepName.empty())
		{
			char lpszBuffer[24];
			sprintf(lpszBuffer,"%lld",sGroupCode);
			sDepName = lpszBuffer;
		}
#else
		if (!theEBAppClient.EB_GetGroupName(sGroupCode,sDepName))
		{
			char lpszBuffer[24];
			sprintf(lpszBuffer,"%lld",sGroupCode);
			sDepName = lpszBuffer;
		}
#endif
	}
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		// 删除旧会话
		CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(sGroupCode,pFromAccountInfo.GetUserId());
		if (pOldCallInfo.get()!=NULL)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE call_id=%lld"),pOldCallInfo->m_sCallId);
			theApp.m_pBoUsers->execute(sSql);
			if (sPhone==0 && pOldCallInfo->m_nFromPhone!=0)
				sPhone = pOldCallInfo->m_nFromPhone;
			if (sFromName.empty() && !pOldCallInfo->m_sFromName.empty())
				sFromName = pOldCallInfo->m_sFromName;
			if (sCompany.empty() && !pOldCallInfo->m_sCompany.empty())
				sCompany = pOldCallInfo->m_sCompany;
			if (sTitle.empty() && !pOldCallInfo->m_sTitle.empty())
				sTitle = pOldCallInfo->m_sTitle;
			if (sTel.empty() && !pOldCallInfo->m_sTel.empty())
				sTel = pOldCallInfo->m_sTel;
			if (sEmail.empty() && !pOldCallInfo->m_sEmail.empty())
				sEmail = pOldCallInfo->m_sEmail;
		}
	}

	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,dep_name,emp_code,from_uid,from_phone,from_name,from_type,company,title,tel,email) ")\
		_T("VALUES(%lld,%lld,'%s',%lld,%lld,%lld,'%s',%d,'%s','%s','%s','%s')"),
		sCallId,sGroupCode,libEbc::ACP2UTF8(sDepName.c_str()).c_str(),sEmpCode,pFromAccountInfo.GetUserId(),sPhone,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),nFromType,
		libEbc::ACP2UTF8(sCompany.c_str()).c_str(),libEbc::ACP2UTF8(sTitle.c_str()).c_str(),sTel.c_str(),sEmail.c_str());
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = sCallId;
		pCallRecordInfo->m_sGroupCode = sGroupCode;
		pCallRecordInfo->m_sGroupName = sDepName;
		pCallRecordInfo->m_sMemberCode = sEmpCode;
		pCallRecordInfo->m_nFromUserId = pFromAccountInfo.GetUserId();
		pCallRecordInfo->m_sFromAccount = pFromAccountInfo.GetAccount();
		pCallRecordInfo->m_sFromName = sFromName;
		pCallRecordInfo->m_sCompany = sCompany;
		pCallRecordInfo->m_sTitle = sTitle;
		pCallRecordInfo->m_nFromPhone = sPhone;
		pCallRecordInfo->m_sTel = sTel;
		pCallRecordInfo->m_sEmail = sEmail;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		//struct timeb tbNow;
		//ftime(&tbNow);
		//tbNow.time -= (tbNow.timezone*60);
		//pCallRecordInfo->m_tTime = tbNow.time;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo);
	}
}

LRESULT CDlgMainFrame::OnMessageCallConnected(WPARAM wParam, LPARAM lParam)
{
	sndPlaySound(NULL,SND_NODEFAULT);
#ifdef USES_EBCOM_TEST
	IEB_CallInfo * pCallInfo = (IEB_CallInfo*)wParam;
	long nConnectFlag = lParam;
	const eb::bigint sCallId = pCallInfo->CallId;
	const eb::bigint sGroupCode = pCallInfo->GroupCode;
	const eb::bigint nFromUserId = pCallInfo->FromUserId;
	const CEBString sFromaccount(pCallInfo->FromAccount.GetBSTR());
	CComPtr<IEB_AccountInfo> pFromAccountInfo = theEBClientCore->EB_GetCallAccountInfo(sCallId,nFromUserId);
	if (pFromAccountInfo == NULL)
	{
		return 0;
	}

	if (sGroupCode==0 || sGroupCode==sCallId)// ?? || !pCallInfo->m_sChatId.empty())
	{
		const bool bOffLineUser = (nConnectFlag&EB_CONNECTED_OFFLINE_USER)==EB_CONNECTED_OFFLINE_USER;
		const bool bOwnerCall = (nConnectFlag&EB_CONNECTED_OWNER_CALL)==EB_CONNECTED_OWNER_CALL;
		const bool bConnectMsg = (nConnectFlag&EB_CONNECTED_MSG)==EB_CONNECTED_MSG;
		bool bNewCall = false;
		CEBCCallInfo::pointer pEbCallInfo;
		if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
		{
			bNewCall = true;
			pEbCallInfo = CEBCCallInfo::create();
			pEbCallInfo->m_bOffLineUser = bOffLineUser;
			pEbCallInfo->m_pCallInfo = pCallInfo;
			pEbCallInfo->m_pFromAccountInfo = pFromAccountInfo;
			theApp.m_pCallList.insert(sCallId,pEbCallInfo);
		}else
		{
			pEbCallInfo->m_bOffLineUser = bOffLineUser;
		}
#ifdef USES_EBCOM_TEST
		const int nSettingValue = theEBClientCore->EB_MyAccountSetting;
#else
		const int nSettingValue = theEBAppClient.EB_GetMyAccountSetting();
#endif
		// 对方离线转在线，本端自动呼叫，不需要处理界面
		if (bOwnerCall || bConnectMsg || ((bOffLineUser&&sGroupCode==0) && (nConnectFlag&EB_CONNECTED_AUTO_ACK)!=EB_CONNECTED_AUTO_ACK))
		{
			// 本方发起会话，离线会话
			// 自动打开聊天界面
			const bool bShow = bOwnerCall;
			CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,bShow);
			if (bOwnerCall)
			{
				if (m_pDlgFrameList->IsIconic())
					m_pDlgFrameList->ShowWindow(SW_RESTORE);
				m_pDlgFrameList->SetForegroundWindow();
			//}else if (bConnectMsg && !pDlgDialog->IsWindowVisible())
			//{
			//	pDlgDialog->ShowWindow(SW_SHOWNORMAL);
			}else
			{
				//::FlashWindow(pDlgDialog->GetSafeHwnd(), TRUE);
			}
		}

		// 保存会话记录
		if (bNewCall && !theApp.IsLogonVisitor())
		{
			SaveCallRecord(sCallId,sGroupCode,pEbCallInfo->m_pFromAccountInfo);
		}
	}
	if (pCallInfo->OldCallId>0)
		theApp.m_pCallList.remove(pCallInfo->OldCallId);

	DeleteDlgIncomingCall(nFromUserId);
	DeleteDlgAlertingCall(nFromUserId);
#else
	const EB_CallInfo* pConnectInfo = (const EB_CallInfo*)wParam;
	int nConnectFlag = lParam;

	const eb::bigint sCallId = pConnectInfo->GetCallId();
	EB_AccountInfo pFromAccountInfo;
	if (!theEBAppClient.EB_GetCallAccountInfo(sCallId,pConnectInfo->GetFromUserId(),&pFromAccountInfo))
	{
		return 1;
	}

	if (pConnectInfo->m_sGroupCode==0 || pConnectInfo->m_sGroupCode==sCallId)// ?? || !pCallInfo->m_sChatId.empty())
	{
		const bool bOffLineUser = (nConnectFlag&EB_CONNECTED_OFFLINE_USER)==EB_CONNECTED_OFFLINE_USER;
		const bool bOwnerCall = (nConnectFlag&EB_CONNECTED_OWNER_CALL)==EB_CONNECTED_OWNER_CALL;
		const bool bConnectMsg = (nConnectFlag&EB_CONNECTED_MSG)==EB_CONNECTED_MSG;
		bool bNewCall = false;
		CEBCCallInfo::pointer pEbCallInfo;
		if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
		{
			bNewCall = true;
			pEbCallInfo = CEBCCallInfo::create();
			pEbCallInfo->m_bOffLineUser = bOffLineUser;
			pEbCallInfo->m_pCallInfo = pConnectInfo;
			pEbCallInfo->m_pFromAccountInfo = pFromAccountInfo;
			theApp.m_pCallList.insert(sCallId,pEbCallInfo);
		}else
		{
			pEbCallInfo->m_bOffLineUser = bOffLineUser;
			pEbCallInfo->m_tLastTime = time(0);
		}
		const int nSettingValue = theEBAppClient.EB_GetMyAccountSetting();
		// 对方离线转在线，本端自动呼叫，不需要处理界面
		if (bOwnerCall || bConnectMsg || ((bOffLineUser&&pConnectInfo->m_sGroupCode==0) && (nConnectFlag&EB_CONNECTED_AUTO_ACK)!=EB_CONNECTED_AUTO_ACK))
		{
			// 本方发起会话，离线会话
			// 自动打开聊天界面
			const bool bShow = bOwnerCall;
			CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,bShow);
			if (bOwnerCall)
			{
				// **关闭聊天提示窗口
				CDlgFuncWindow * pOldFuncWindow = NULL;
				if (pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
					theApp.m_pTempGroupMsg.find(pEbCallInfo->m_pCallInfo.m_sGroupCode,pOldFuncWindow,true);
				else
					theApp.m_pTempFromUserMsg.find(pEbCallInfo->m_pFromAccountInfo.GetUserId(),pOldFuncWindow,true);
				// **必须放在前面
				if (pOldFuncWindow!=NULL)
				{
					pOldFuncWindow->ShowWindow(SW_HIDE);	// *必须隐藏不让用户点击；
					pOldFuncWindow->m_pEbCallInfo.reset();	// *必须设为空，否则会清掉后面窗口
					theApp.m_pCloseWnd.add(pOldFuncWindow);
				}

				if (m_pDlgFrameList->IsIconic())
					m_pDlgFrameList->ShowWindow(SW_RESTORE);
				m_pDlgFrameList->SetForegroundWindow();
			//}else if (bConnectMsg && !pDlgDialog->IsWindowVisible())
			//{
			//	pDlgDialog->ShowWindow(SW_SHOWNORMAL);
			}else
			{
				//::FlashWindow(pDlgDialog->GetSafeHwnd(), TRUE);
			}
		}

		// 保存会话记录
		if (bNewCall && !theEBAppClient.EB_IsLogonVisitor())
		{
			SaveCallRecord(sCallId,pConnectInfo->m_sGroupCode,pEbCallInfo->m_pFromAccountInfo);
		}
	}
	if (pConnectInfo->m_sOldCallId>0)
		theApp.m_pCallList.remove(pConnectInfo->m_sOldCallId);

	DeleteDlgIncomingCall(pConnectInfo->GetFromUserId());
	DeleteDlgAlertingCall(pConnectInfo->GetFromUserId());
#endif

	if (m_pDlgFrameList==NULL || !m_pDlgFrameList->ExistCallIdWnd(sCallId))
	{
		// 没有打开聊天，设置为退出标识；
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallExit(sCallId);
#else
		theEBAppClient.EB_CallExit(sCallId);
#endif
	}

	return 0;
}

LRESULT CDlgMainFrame::OnMessageCallHangup(WPARAM wParam, LPARAM lParam)
{
	sndPlaySound(NULL,SND_NODEFAULT);

#ifdef USES_EBCOM_TEST
	IEB_CallInfo * pCallInfo = (IEB_CallInfo*)wParam;
	bool bOwner = lParam==1;
	const eb::bigint sCallId = pCallInfo->CallId;
	const eb::bigint nFromUserId = pCallInfo->FromUserId;
	bool bRemoveCall = bOwner;
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId, bRemoveCall);
	if (pDlgDialog.get()!=NULL)
	{
		if (!bRemoveCall)
			pDlgDialog->UserExitRoom(nFromUserId,false);
	}
	if (bOwner || 
		pDlgDialog.get()==NULL)		// 本地已经关闭聊天界面
	{
		theApp.m_pCallList.remove(sCallId);
	}
	DeleteDlgAlertingCall(nFromUserId);
	DeleteDlgIncomingCall(nFromUserId);
#else
	EB_CallInfo* pCallInfo = (EB_CallInfo*)wParam;
	bool bOwner = (bool)(lParam==1);

	bool bRemoveCall = bOwner;
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pCallInfo->GetCallId(), bRemoveCall);
	if (pDlgDialog.get()!=NULL)
	{
		if (!bRemoveCall)
		{
			CCrAccountInfo pCrAccountInfo;
			pCrAccountInfo.m_sAccount = pCallInfo->GetFromUserId();
			pDlgDialog->OnUserExitRoom(&pCrAccountInfo);
		}
	}
	if (bOwner || 
		pDlgDialog.get()==NULL)		// 本地已经关闭聊天界面
	{
		theApp.m_pCallList.remove(pCallInfo->GetCallId());
	}
	DeleteDlgAlertingCall(pCallInfo->GetFromUserId());
	DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageCallError(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nResultValue = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nResultValue || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nResultValue)
		return 0;
	sndPlaySound(NULL,SND_NODEFAULT);

#ifdef USES_EBCOM_TEST
	IEB_CallInfo * pCallInfo = (IEB_CallInfo*)wParam;
	const eb::bigint sCallId = pCallInfo->CallId;
	const eb::bigint nFromUserId = pCallInfo->FromUserId;
	const CEBString sFromAccount = pCallInfo->FromAccount.GetBSTR();
	CString sErrorText;
	DeleteDlgAlertingCall(nFromUserId);
	if (nResultValue == EB_STATE_ACCOUNT_NOT_EXIST)
		sErrorText = _T("用户不存在！");
	else if (nResultValue == EB_STATE_USER_OFFLINE)
		sErrorText = _T("用户离线状态！");
	else if (nResultValue == EB_STATE_UNAUTH_ERROR)
		sErrorText = _T("该帐号未通过验证！");
	else if (nResultValue == EB_STATE_ACCOUNT_FREEZE)
		sErrorText = _T("帐号已经被临时冻结！");
	else if (nResultValue == EB_STATE_MAX_RETRY_ERROR)
		sErrorText = _T("错误次数太频繁，帐号被临时锁住，请稍候再试！");
	else if (nResultValue == EB_STATE_USER_BUSY)
	{
		DeleteDlgIncomingCall(nFromUserId);
		sErrorText = _T("用户线路忙！");
	}else if (nResultValue == EB_STATE_TIMEOUT_ERROR)
	{
		// 本地太久未响应，也会有该事件
		DeleteDlgIncomingCall(nFromUserId);
		sErrorText = _T("会话超时！");
	}else if (nResultValue == EB_STATE_USER_HANGUP)
	{
		DeleteDlgIncomingCall(nFromUserId);
		sErrorText = _T("用户取消呼叫！");
	}else if (nResultValue == EB_STATE_ACCOUNT_DISABLE_OFFCALL)
	{
		sErrorText = _T("用户不在线，没有开放接收离线信息权限！");
	}else if (nResultValue==EB_STATE_ACCOUNT_DISABLE_EXTCALL)
	{
		sErrorText = _T("用户不接收外部会话！");
	}else if (nResultValue==EB_STATE_OAUTH_FORWARD)
	{
		sErrorText = _T("操作成功，等待对方验证！");
	}else
	{
		sErrorText.Format(_T("呼叫失败 %d"),nResultValue);
	}

	CComPtr<IEB_AccountInfo> pTempFromAccountInfo = theEBClientCore->EB_GetCallAccountInfo(sCallId,nFromUserId);
	if (pTempFromAccountInfo != NULL)
	{
		SaveCallRecord(sCallId,pCallInfo->GroupCode,EB_AccountInfo(pTempFromAccountInfo));
		m_btnMySession.SetWindowText(_T("！"));
	}

	// ??
	//if (!pCallInfo->m_bOwner)
	{
		CDlgMessageBox::EbMessageBox(this,sFromAccount.c_str(),sErrorText,CDlgMessageBox::IMAGE_WARNING,5);
	}
#else
	EB_CallInfo* pCallInfo = (EB_CallInfo*)wParam;
	const eb::bigint sCallId = pCallInfo->GetCallId();
	CString sErrorText;
	DeleteDlgAlertingCall(pCallInfo->GetFromUserId());
	if (nResultValue == EB_STATE_ACCOUNT_NOT_EXIST)
		sErrorText = _T("用户不存在！");
	else if (nResultValue == EB_STATE_USER_OFFLINE)
		sErrorText = _T("用户离线状态！");
	else if (nResultValue == EB_STATE_UNAUTH_ERROR)
		sErrorText = _T("该帐号未通过验证！");
	else if (nResultValue == EB_STATE_ACCOUNT_FREEZE)
		sErrorText = _T("帐号已经被临时冻结！");
	else if (nResultValue == EB_STATE_MAX_RETRY_ERROR)
		sErrorText = _T("错误次数太频繁，帐号被临时锁住，请稍候再试！");
	else if (nResultValue == EB_STATE_USER_BUSY)
	{
		DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
		sErrorText = _T("用户线路忙！");
	}else if (nResultValue == EB_STATE_TIMEOUT_ERROR)
	{
		// 本地太久未响应，也会有该事件
		DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
		sErrorText = _T("会话超时！");
	}else if (nResultValue == EB_STATE_USER_HANGUP)
	{
		DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
		sErrorText = _T("用户取消呼叫！");
	}else if (nResultValue == EB_STATE_ACCOUNT_DISABLE_OFFCALL)
	{
		sErrorText = _T("用户不在线，没有开放接收离线信息权限！");
	}else if (nResultValue==EB_STATE_ACCOUNT_DISABLE_EXTCALL)
	{
		sErrorText = _T("用户不接收外部会话！");
	}else if (nResultValue==EB_STATE_OAUTH_FORWARD)
	{
		sErrorText = _T("操作成功，等待对方验证！");
	}else
	{
		sErrorText.Format(_T("呼叫失败 %d"),nResultValue);
	}

	EB_AccountInfo pFromAccountInfo;
	if (theEBAppClient.EB_GetCallAccountInfo(sCallId,pCallInfo->GetFromUserId(),&pFromAccountInfo))
	{
		SaveCallRecord(sCallId,pCallInfo->m_sGroupCode,pFromAccountInfo);
		m_btnMySession.SetWindowText(_T("！"));
	}

	// ??
	//if (!pCallInfo->m_bOwner)
	{
		CDlgMessageBox::EbMessageBox(this,pCallInfo->GetFromAccount().c_str(),sErrorText,CDlgMessageBox::IMAGE_WARNING,5);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageAlertingCall(WPARAM wParam, LPARAM lParam)
{
	const int const_dlg_width = 380;
	const int const_dlg_height = 262;
	int nScreenWidth = 0;
	int nScreenHeight = 0;
	theApp.GetScreenSize(nScreenWidth, nScreenHeight);
#ifdef USES_EBCOM_TEST
	IEB_CallInfo* pCallInfo = (IEB_CallInfo*)wParam;
	const eb::bigint sCallId = pCallInfo->CallId;
	const eb::bigint nFromUserId = pCallInfo->FromUserId;
	CComPtr<IEB_AccountInfo> pFromAccountInfo = theEBClientCore->EB_GetCallAccountInfo(sCallId,nFromUserId);
	if (pFromAccountInfo == NULL) return 0;

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnAlertingCall();
	}else if (!m_pAlertingCallList.exist(nFromUserId))
	{
		CDlgAlertingCall::pointer pDlgAlertingCall = CDlgAlertingCall::create(this);
		pDlgAlertingCall->m_pFromAccountInfo = pFromAccountInfo;
		pDlgAlertingCall->m_sCallId = sCallId;
		pDlgAlertingCall->Create(CDlgAlertingCall::IDD, this);
		CRect rectDlgIncomingCall;
		pDlgAlertingCall->GetWindowRect(&rectDlgIncomingCall);
		pDlgAlertingCall->MoveWindow(0, nScreenHeight-const_dlg_height, const_dlg_width, const_dlg_height);
		pDlgAlertingCall->ShowWindow(SW_SHOW);
		//pDlgAlertingCall->SetActiveWindow();
		pDlgAlertingCall->SetForegroundWindow();
		m_pAlertingCallList.insert(nFromUserId, pDlgAlertingCall);
	}
#else
	EB_CallInfo* pCallInfo = (EB_CallInfo*)wParam;
	const eb::bigint sCallId = pCallInfo->GetCallId();
	const eb::bigint nFromUserId = pCallInfo->GetFromUserId();
	const CEBString sFromAccount = pCallInfo->GetFromAccount();
	EB_AccountInfo pFromAccountInfo;
	if (!theEBAppClient.EB_GetCallAccountInfo(sCallId,nFromUserId,&pFromAccountInfo)) return 1;

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnAlertingCall();
	}else if (!m_pAlertingCallList.exist(pCallInfo->GetFromUserId()))
	{
		AFX_SWITCH_INSTANCED();
		CDlgAlertingCall::pointer pDlgAlertingCall = CDlgAlertingCall::create(this);
		pDlgAlertingCall->m_pFromAccountInfo = pFromAccountInfo;
		pDlgAlertingCall->m_sCallId = sCallId;
		pDlgAlertingCall->Create(CDlgAlertingCall::IDD, this);
		CRect rectDlgIncomingCall;
		pDlgAlertingCall->GetWindowRect(&rectDlgIncomingCall);
		pDlgAlertingCall->MoveWindow(0, nScreenHeight-const_dlg_height, const_dlg_width, const_dlg_height);
		pDlgAlertingCall->ShowWindow(SW_SHOW);
		//pDlgAlertingCall->SetActiveWindow();
		pDlgAlertingCall->SetForegroundWindow();
		m_pAlertingCallList.insert(pCallInfo->GetFromUserId(), pDlgAlertingCall);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageIncomingCall(WPARAM wParam, LPARAM lParam)
{
	CString sSoundFile;
	sSoundFile.Format(_T("%s/wav/incomingcall.wav"), theApp.GetAppDataPath());
	sndPlaySound(sSoundFile, SND_ASYNC|SND_NOWAIT|SND_LOOP);
	const int const_dlg_width = 380;
	const int const_dlg_height = 262;
	int nScreenWidth = 0;
	int nScreenHeight = 0;
	theApp.GetScreenSize(nScreenWidth, nScreenHeight);
#ifdef USES_EBCOM_TEST
	IEB_CallInfo * pCallInfo = (IEB_CallInfo*)wParam;
	IEB_AccountInfo* pFromAccount = (IEB_AccountInfo*)lParam;
	const eb::bigint sCallId = pCallInfo->CallId;
	const eb::bigint nFromUserId = pFromAccount->UserId;
	//CEBCCallInfo::pointer pCallInfo = theEBAppClient.GetCallInfo(sCallId);;
	//if (pCallInfo.get() == NULL) return 1;

	CDlgIncomingCall::pointer pDlgIncomingCall = CDlgIncomingCall::create(this);
	pDlgIncomingCall->m_pFromAccountInfo = pFromAccount;
	pDlgIncomingCall->m_pCallInfo = pCallInfo;
	pDlgIncomingCall->Create(CDlgIncomingCall::IDD, this);
	pDlgIncomingCall->MoveWindow(nScreenWidth-const_dlg_width, nScreenHeight-const_dlg_height, const_dlg_width, const_dlg_height);
	pDlgIncomingCall->SetCircle();
	pDlgIncomingCall->ShowWindow(SW_SHOW);
	//pDlgIncomingCall->SetActiveWindow();
	pDlgIncomingCall->SetForegroundWindow();
	m_pIncomingCallList.insert(nFromUserId, pDlgIncomingCall);
#else
	const EB_CallInfo* pCallInfo = (const EB_CallInfo*)wParam;
	const EB_AccountInfo* pFromAccount = (const EB_AccountInfo*)lParam;

	const eb::bigint sCallId = pCallInfo->GetCallId();
	//CEBCCallInfo::pointer pCallInfo = theEBAppClient.GetCallInfo(sCallId);;
	//if (pCallInfo.get() == NULL) return 1;

	AFX_SWITCH_INSTANCED();
	CDlgIncomingCall::pointer pDlgIncomingCall = CDlgIncomingCall::create(this);
	pDlgIncomingCall->m_pFromAccountInfo = pFromAccount;
	pDlgIncomingCall->m_pCallInfo = pCallInfo;
	pDlgIncomingCall->Create(CDlgIncomingCall::IDD, this);
	pDlgIncomingCall->MoveWindow(nScreenWidth-const_dlg_width, nScreenHeight-const_dlg_height, const_dlg_width, const_dlg_height);
	pDlgIncomingCall->SetCircle();
	pDlgIncomingCall->ShowWindow(SW_SHOW);
	//pDlgIncomingCall->SetActiveWindow();
	pDlgIncomingCall->SetForegroundWindow();
	m_pIncomingCallList.insert(pCallInfo->GetFromUserId(), pDlgIncomingCall);
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageUGInfo(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_UGInfo * pUGInfo = (IEB_UGInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->UGInfo(pUGInfo);
#else
	const EB_UGInfo* pUGInfo = (const EB_UGInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->UGInfo(pUGInfo);
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageUGDelete(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_UGInfo * pUGInfo = (IEB_UGInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->UGDelete(pUGInfo);
#else
	const EB_UGInfo* pUGInfo = (const EB_UGInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->UGDelete(pUGInfo);
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageContactInfo(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_ContactInfo * pContactInfo = (IEB_ContactInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->ContactInfo(pContactInfo);
#else
	const EB_ContactInfo* pPopContactInfo = (const EB_ContactInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->ContactInfo(pPopContactInfo);
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageContactDelete(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_ContactInfo * pContactInfo = (IEB_ContactInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->DeleteContact(pContactInfo);
#else
	const EB_ContactInfo* pPopContactInfo = (const EB_ContactInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->DeleteContact(pPopContactInfo);
#endif
	return 0;
}
LRESULT CDlgMainFrame::OnMessageContactStateChange(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_ContactInfo * pContactInfo = (IEB_ContactInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->ContactInfo(pContactInfo);
	if (m_pDlgFrameList!=NULL)
	{
		m_pDlgFrameList->UserLineStateChange(0, pContactInfo->ContactUserId, pContactInfo->LineState);
	}

#else
	const EB_ContactInfo* pPopContactInfo = (const EB_ContactInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->ContactInfo(pPopContactInfo);
	if (m_pDlgFrameList!=NULL)
	{
		m_pDlgFrameList->UserLineStateChange(0, pPopContactInfo->m_nContactUserId, pPopContactInfo->m_nLineState);
	}

#endif

	return 0;
}

LRESULT CDlgMainFrame::OnMessageEnterpriseInfo(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_EnterpriseInfo * pEnterpriseInfo = (IEB_EnterpriseInfo*)wParam;
	if (m_pDlgMyEnterprise != NULL)
		m_pDlgMyEnterprise->EnterpriseInfo(pEnterpriseInfo);

	if (m_btnMyEnterprise.GetSafeHwnd() && !m_btnMyEnterprise.IsWindowVisible())
	{
		m_btnMyEnterprise.ShowWindow(SW_SHOW);
		m_btnMyEnterprise.Invalidate();
	}
#else
	const EB_EnterpriseInfo* pEnterpriseInfo = (const EB_EnterpriseInfo*)wParam;
	if (m_pDlgMyEnterprise != NULL)
		m_pDlgMyEnterprise->EnterpriseInfo(pEnterpriseInfo);

	if (m_btnMyEnterprise.GetSafeHwnd() && !m_btnMyEnterprise.IsWindowVisible())
	{
		m_btnMyEnterprise.ShowWindow(SW_SHOW);
		m_btnMyEnterprise.Invalidate();
	}
#endif
	ChangeTrayText();
	return 0;
}

LRESULT CDlgMainFrame::OnMessageGroupInfo(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_GroupInfo * pGroupInfo = (IEB_GroupInfo*)wParam;
	//bool bIsMyGroup = lParam==1;
	if (m_pDlgMyEnterprise != NULL)
		m_pDlgMyEnterprise->DepartmentInfo(pGroupInfo);
	if (pGroupInfo->MyEmpId>0 && m_pDlgMyGroup != NULL)
	{
		m_pDlgMyGroup->MyDepartmentInfo(pGroupInfo);
	}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->GroupCode);
	if (pDlgDialog.get()!=NULL)
	{
		// 群资料已经修改
		pDlgDialog->ChangeDepartmentInfo(pGroupInfo);
	}
#else
	const EB_GroupInfo* pGroupInfo = (const EB_GroupInfo*)wParam;
	//bool bIsMyDepartment = (bool)(lParam==1);
	if (m_pDlgMyEnterprise != NULL)
		m_pDlgMyEnterprise->DepartmentInfo(pGroupInfo);
	if (pGroupInfo->m_nMyEmpId>0 && m_pDlgMyGroup != NULL)
	{
		m_pDlgMyGroup->MyDepartmentInfo(pGroupInfo);
	}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode);
	if (pDlgDialog.get()!=NULL)
	{
		// 群资料已经修改
		pDlgDialog->ChangeDepartmentInfo(pGroupInfo);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageGroupDelete(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_GroupInfo * pGroupInfo = (IEB_GroupInfo*)wParam;
	bool bIsMyGroup = lParam==1;
	const eb::bigint sGroupCode = pGroupInfo->GroupCode;
	const CEBString sGroupName = pGroupInfo->GroupName.GetBSTR();
	if (m_pDlgMyEnterprise != NULL)
	{
		m_pDlgMyEnterprise->DeleteDepartmentInfo(sGroupCode);
	}
	if (m_pDlgMyGroup != NULL)
		m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);

	// 关闭现在会话窗口
	theApp.m_pCallList.remove(sGroupCode);
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->OnRemoveGroup(sGroupCode);

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sGroupCode, true);
	if (pDlgDialog.get()!=NULL)
	{
	}

	CString stext;
	stext.Format(_T("解散%s"), sGroupName.c_str());
	CDlgMessageBox::EbMessageBox(this,_T("管理员"),stext,CDlgMessageBox::IMAGE_WARNING,10);

	// ?? remove
	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(pGroupInfo->m_sGroupCode, pDlgDialog))
	//{
	//	// 群资料已经修改
	//	pDlgDialog->ChangeDepartmentInfo(pGroupInfo);
	//}
#else
	const EB_GroupInfo* pGroupInfo = (const EB_GroupInfo*)wParam;
	const eb::bigint sGroupCode = pGroupInfo->m_sGroupCode;
	bool bIsMyDepartment = (bool)(lParam==1);
	if (m_pDlgMyEnterprise != NULL)
	{
		m_pDlgMyEnterprise->DeleteDepartmentInfo(pGroupInfo->m_sGroupCode);
	}
	if (m_pDlgMyGroup != NULL)
		m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);

	// 关闭现在会话窗口
	theApp.m_pCallList.remove(pGroupInfo->m_sGroupCode);

	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode, true);
	if (pDlgDialog.get()!=NULL)
	{
	}
	CString stext;
	stext.Format(_T("解散%s"), pGroupInfo->m_sGroupName.c_str());
	CDlgMessageBox::EbMessageBox(this,_T("管理员"),stext,CDlgMessageBox::IMAGE_INFORMATION,10);

	// ?? remove
	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(pGroupInfo->m_sGroupCode, pDlgDialog))
	//{
	//	// 群资料已经修改
	//	pDlgDialog->ChangeDepartmentInfo(pGroupInfo);
	//}
#endif
	theApp.DeleteDbRecord(sGroupCode,false);
	return 0;
}
LRESULT CDlgMainFrame::OnMessageGroupEditError(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nErrorCode = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nErrorCode || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nErrorCode)
		return 0;
#ifdef USES_EBCOM_TEST
	IEB_GroupInfo * pGroupInfo = (IEB_GroupInfo*)wParam;
#else
	const EB_GroupInfo* pGroupInfo = (const EB_GroupInfo*)wParam;
#endif
	switch (nErrorCode)
	{
	case EB_STATE_NOT_AUTH_ERROR:
		CDlgMessageBox::EbMessageBox(this,_T("对不起，你没有权限进行操作！"),_T(""),CDlgMessageBox::IMAGE_ERROR,5);
		break;
	default:
		CDlgMessageBox::EbMessageBox(this,_T("编辑失败："),_T("请重试或联系客服！"),CDlgMessageBox::IMAGE_ERROR,5);
		break;
	}
	return 0;
}

LRESULT CDlgMainFrame::OnMessageRemoveGroup(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_GroupInfo * pGroupInfo = (IEB_GroupInfo*)wParam;
	IEB_MemberInfo* pMemberInfo = (IEB_MemberInfo*)lParam;
	const eb::bigint sGroupCode(pGroupInfo->GroupCode);
	const CEBString sGroupName(pGroupInfo->GroupName.GetBSTR());
	const eb::bigint sRemoveMemberCode(pMemberInfo->MemberCode);
	const eb::bigint sRemoveAccount(pMemberInfo->MemberUserId);

	// 删除本地群组聊天记录
	theApp.DeleteDbRecord(sGroupCode,false);

	// 找到现在会话，移除用户数据
	CString stext;
	if (sRemoveAccount == theApp.GetLogonUserId())
	{
		theApp.m_pCallList.remove(sGroupCode);
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveGroup(sGroupCode);

		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sGroupCode,true);
		if (pDlgDialog.get()!=NULL)
		{
		}
		if (m_pDlgMyGroup!=NULL)
			m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);
		stext.Format(_T("你被管理员移除出%s"), sGroupName.c_str());
		CDlgMessageBox::EbMessageBox(this,"",stext,CDlgMessageBox::IMAGE_WARNING,10);
	}else
	{
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveMember(sGroupCode, sRemoveMemberCode);

		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sGroupCode);
		if (pDlgDialog.get()!=NULL)
		{
			pDlgDialog->UserExitRoom(sRemoveAccount,true);
		}
		if (m_pDlgMyEnterprise != NULL)
			m_pDlgMyEnterprise->DeleteEmployeeInfo(pGroupInfo,sRemoveMemberCode);
		if (m_pDlgMyGroup != NULL)// && theEBClientCore->EB_IsMyGroup(pMemberInfo->GroupCode))
			m_pDlgMyGroup->DeleteEmployeeInfo(pGroupInfo,sRemoveMemberCode);
		//if (m_pDlgMyGroup)
		//	m_pDlgMyGroup->ExitDepartment(sGroupCode, sRemoveAccount);
	}
#else
	const EB_GroupInfo* pGroupInfo = (const EB_GroupInfo*)wParam;
	const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)lParam;

	// 删除本地群组聊天记录
	theApp.DeleteDbRecord(pGroupInfo->m_sGroupCode,false);

	const eb::bigint sRemoveUserId(pMemberInfo->m_nMemberUserId);
	// 找到现在会话，移除用户数据
	CString stext;
	if (sRemoveUserId == theEBAppClient.EB_GetLogonUserId())
	{
		theApp.m_pCallList.remove(pGroupInfo->m_sGroupCode);

		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);

		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode,true);
		if (pDlgDialog.get()!=NULL)
		{
		}
		if (m_pDlgMyGroup!=NULL)
		{
			m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);
		}
		stext.Format(_T("你被管理员移除出%s"), pGroupInfo->m_sGroupName.c_str());
		CDlgMessageBox::EbMessageBox(this,"",stext,CDlgMessageBox::IMAGE_INFORMATION,10);
	}else
	{
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);

		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode);
		if (pDlgDialog.get()!=NULL)
		{
			pDlgDialog->UserExitRoom(sRemoveUserId,true);
		}
		if (m_pDlgMyGroup != NULL)// && theEBAppClient.EB_IsMyGroup(pMemberInfo->m_sGroupCode))
		{
			m_pDlgMyGroup->DeleteEmployeeInfo(pGroupInfo,pMemberInfo->m_sMemberCode);
		}
	}
	if (pGroupInfo->m_sEnterpriseCode>0 && m_pDlgMyEnterprise != NULL)
		m_pDlgMyEnterprise->DeleteEmployeeInfo(pGroupInfo,pMemberInfo->m_sMemberCode);
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageExitGroup(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_GroupInfo* pGroupInfo = (IEB_GroupInfo*)wParam;
	IEB_MemberInfo* pMemberInfo = (IEB_MemberInfo*)lParam;
	const eb::bigint sGroupCode(pGroupInfo->GroupCode);
	const CEBString sGroupName(pGroupInfo->GroupName.GetBSTR());
	const eb::bigint nCreateUserId = pGroupInfo->CreateUserId;
	const eb::bigint sExitAccount(pMemberInfo->MemberUserId);
	const CEBString sUserName(pMemberInfo->UserName.GetBSTR());
	const eb::bigint sMemberCode(pMemberInfo->MemberCode);
	const CEBString sMemberAccount(pMemberInfo->MemberAccount.GetBSTR());

	// 删除本地群组聊天记录
	theApp.DeleteDbRecord(sGroupCode,false);
	// 找到现在会话，移除用户数据
	if (sExitAccount == theApp.GetLogonUserId())
	{
		theApp.m_pCallList.remove(sGroupCode);
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveGroup(sGroupCode);

		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sGroupCode,true);
		if (pDlgDialog.get()!=NULL)
		{
		}
		if (m_pDlgMyGroup)
			m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);
		return 0;
	}else
	{
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveMember(sGroupCode, sMemberCode);

		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sGroupCode);
		if (pDlgDialog.get()!=NULL)
		{
			pDlgDialog->UserExitRoom(sExitAccount,true);
		}
		if (m_pDlgMyEnterprise != NULL)
			m_pDlgMyEnterprise->DeleteEmployeeInfo(pGroupInfo, sMemberCode);
		if (m_pDlgMyGroup != NULL)// && theEBClientCore->EB_IsMyGroup(pMemberInfo->GroupCode))
			m_pDlgMyGroup->DeleteEmployeeInfo(pGroupInfo,sMemberCode);
		//if (m_pDlgMyGroup)
		//	m_pDlgMyGroup->ExitDepartment(sGroupCode, sExitAccount);
	}
	if (nCreateUserId==theApp.GetLogonUserId())
	{
		CString swindow;
		swindow.Format(_T("%s(%s)"), sUserName.c_str(),sMemberAccount.c_str());
		CString stext;
		stext.Format(_T("已经退出%s"), sGroupName.c_str());
		CDlgMessageBox::EbMessageBox(this,swindow,stext,CDlgMessageBox::IMAGE_WARNING);
	}
#else
	const EB_GroupInfo* pGroupInfo = (const EB_GroupInfo*)wParam;
	const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)lParam;
	const eb::bigint sExitUserId(pMemberInfo->m_nMemberUserId);

	// 删除本地群组聊天记录
	theApp.DeleteDbRecord(pGroupInfo->m_sGroupCode,false);
	// 找到现在会话，移除用户数据
	if (sExitUserId == theEBAppClient.EB_GetLogonUserId())
	{
		theApp.m_pCallList.remove(pGroupInfo->m_sGroupCode);
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);

		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode,true);
		if (pDlgDialog.get()!=NULL)
		{
		}
		if (m_pDlgMyEnterprise != NULL)
			m_pDlgMyEnterprise->DeleteEmployeeInfo(pGroupInfo,pMemberInfo->m_sMemberCode);
		if (m_pDlgMyGroup)
			m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);
		return 0;
	}else
	{
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);

		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode);
		if (pDlgDialog.get()!=NULL)
		{
			pDlgDialog->UserExitRoom(sExitUserId,true);
		}
		if (m_pDlgMyEnterprise != NULL)
			m_pDlgMyEnterprise->DeleteEmployeeInfo(pGroupInfo,pMemberInfo->m_sMemberCode);
		if (m_pDlgMyGroup != NULL)// && theEBAppClient.EB_IsMyGroup(pMemberInfo->m_sGroupCode))
			m_pDlgMyGroup->DeleteEmployeeInfo(pGroupInfo,pMemberInfo->m_sMemberCode);
		//if (m_pDlgMyGroup)
		//	m_pDlgMyGroup->ExitDepartment(pGroupInfo->m_sGroupCode, sExitUserId);
	}
	if (pGroupInfo->m_nCreateUserId==theEBAppClient.EB_GetLogonUserId())
	{
		CString swindow;
		swindow.Format(_T("%s(%s)"), pMemberInfo->m_sUserName.c_str(),pMemberInfo->m_sMemberAccount.c_str());
		CString stext;
		stext.Format(_T("已经退出%s"), pGroupInfo->m_sGroupName.c_str());
		CDlgMessageBox::EbMessageBox(this,swindow,stext,CDlgMessageBox::IMAGE_INFORMATION, 10);
	}
#endif
	return 0;
}
LRESULT CDlgMainFrame::OnMessageRequestJoin2Group(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_AccountInfo* pRequestAccountInfo = (IEB_AccountInfo*)wParam;
	IEB_APMsgInfo* pApMsgInfo = (IEB_APMsgInfo*)lParam;
	const eb::bigint nRequestUserId = pRequestAccountInfo->UserId;
	const eb::bigint nMsgId = pApMsgInfo->MsgId;
	const eb::bigint nGroupId = pApMsgInfo->GroupId;
	const CEBString sRequestAccount(pRequestAccountInfo->Account.GetBSTR());
	const CEBString sDescription(pApMsgInfo->MsgContent.GetBSTR());
	//theEBClientCore->EB_AckMsg(pApMsgInfo->MsgId,VARIANT_TRUE);

	CString sSql;
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		// 删除旧会话
		CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,nRequestUserId,true);
		if (pOldCallInfo.get()!=NULL)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,nRequestUserId);
			theApp.m_pBoUsers->execute(sSql);
		}
	}
	// emp_code=-1,from_type=msg-type
	const int nMsgType = EBC_MSG_TYPE_REQUEST_JOIN_2_GROUP;
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) ")\
		_T("VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
		nMsgId,nGroupId,nRequestUserId,sRequestAccount.c_str(),nMsgType);
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = nMsgId;
		pCallRecordInfo->m_sGroupCode = nGroupId;
		pCallRecordInfo->m_sMemberCode = -1;
		pCallRecordInfo->m_nFromUserId = nRequestUserId;
		pCallRecordInfo->m_sFromAccount = sRequestAccount;
		pCallRecordInfo->m_nFromType = nMsgType;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}
#else
	const EB_AccountInfo* pRequestAccountInfo = (const EB_AccountInfo*)wParam;
	const EB_APMsgInfo* pApMsgInfo = (const EB_APMsgInfo*)lParam;
	const CEBString sRequestAccount(pRequestAccountInfo->GetAccount());
	const CEBString sDescription(pApMsgInfo->m_sMsgContent);
	const eb::bigint nGroupId = pApMsgInfo->m_nGroupId;

	//CString stext;
	//stext.Format(_T("%lld(%s)申请加入群组%s(%lld):%s"),
	//	pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),pApMsgInfo->m_sMsgName.c_str(),nGroupId,pApMsgInfo->m_sMsgContent.c_str());
	//CString swindow;
	//swindow.Format(_T("MSG-ID:%lld"), pApMsgInfo->m_nMsgId);
	//CDlgMessageBox::EbMessageBox(this,swindow,stext,CDlgMessageBox::IMAGE_INFORMATION);
	//theEBAppClient.EB_AckMsg(pApMsgInfo->m_nMsgId,true);

	CString sSql;
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd()!=NULL)
	{
		// 删除旧会话
		CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,pRequestAccountInfo->GetUserId(),true);
		if (pOldCallInfo.get()!=NULL)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,pRequestAccountInfo->GetUserId());
			theApp.m_pBoUsers->execute(sSql);
		}
	}

	// emp_code=-1,from_type=msg-type
	const int nMsgType = EBC_MSG_TYPE_REQUEST_JOIN_2_GROUP;
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) ")\
		_T("VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
		pApMsgInfo->m_nMsgId,nGroupId,pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),nMsgType);
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = pApMsgInfo->m_nMsgId;
		pCallRecordInfo->m_sGroupCode = nGroupId;
		pCallRecordInfo->m_sMemberCode = -1;
		pCallRecordInfo->m_nFromUserId = pRequestAccountInfo->GetUserId();
		pCallRecordInfo->m_sFromAccount = sRequestAccount;
		pCallRecordInfo->m_nFromType = nMsgType;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}

#endif

	if (theApp.GetGroupMsgSugId()>0)
	{
		CString sContent;
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>申请加入群组：%lld<br/>附加消息：%s</font><br/>\
						   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"),
			sRequestAccount.c_str(),nGroupId,sDescription.substr(0,30).c_str(),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->m_nMsgType = nMsgType;
		pFuncWindow->m_bOpenNewClose = true;
		pFuncWindow->m_sTitle = _T("验证消息");
		pFuncWindow->m_sFuncUrl = libEbc::ACP2UTF8(sContent);
		pFuncWindow->m_nWidth = 250;
		pFuncWindow->m_nHeight = 180;
		CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
		pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
		pEbcMsgInfo->m_pParent = pParent;
		pEbcMsgInfo->m_bSetForegroundWindow = true;
		m_pEbcMsgList.add(pEbcMsgInfo);
	}
	return 0;
}
LRESULT CDlgMainFrame::OnMessageInviteJoin2Group(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_AccountInfo* pRequestAccountInfo = (IEB_AccountInfo*)wParam;
	IEB_APMsgInfo* pApMsgInfo = (IEB_APMsgInfo*)lParam;
	const eb::bigint nRequestUserId = pRequestAccountInfo->UserId;
	const eb::bigint nMsgId = pApMsgInfo->MsgId;
	const eb::bigint nGroupId = pApMsgInfo->GroupId;
	const CEBString sRequestAccount(pRequestAccountInfo->Account.GetBSTR());
	const CEBString sDescription(pApMsgInfo->MsgContent.GetBSTR());
	//theEBClientCore->EB_AckMsg(pApMsgInfo->MsgId,VARIANT_TRUE);

	CString sSql;
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		// 删除旧会话
		CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,nRequestUserId,true);
		if (pOldCallInfo.get()!=NULL)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,nRequestUserId);
			theApp.m_pBoUsers->execute(sSql);
		}
	}

	// emp_code=-1,from_type=msg-type
	const int nMsgType = EBC_MSG_TYPE_INVITE_JOIN_2_GROUP;
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) ")\
		_T("VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
		nMsgId,nGroupId,nRequestUserId,sRequestAccount.c_str(),nMsgType);
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = nMsgId;
		pCallRecordInfo->m_sGroupCode = nGroupId;
		pCallRecordInfo->m_sMemberCode = -1;
		pCallRecordInfo->m_nFromUserId = nRequestUserId;
		pCallRecordInfo->m_sFromAccount = sRequestAccount;
		pCallRecordInfo->m_nFromType = nMsgType;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}
#else
	const EB_AccountInfo* pRequestAccountInfo = (const EB_AccountInfo*)wParam;
	const EB_APMsgInfo* pApMsgInfo = (const EB_APMsgInfo*)lParam;
	const CEBString sRequestAccount(pRequestAccountInfo->GetAccount());
	const eb::bigint nGroupId = pApMsgInfo->m_nGroupId;
	const CEBString sDescription(pApMsgInfo->m_sMsgContent);

	//CString stext;
	//stext.Format(_T("%lld(%s)邀请加入群组%s(%lld):%s"),
	//	pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),pApMsgInfo->m_sMsgName.c_str(),nGroupId,pApMsgInfo->m_sMsgContent.c_str());
	//CString swindow;
	//swindow.Format(_T("MSG-ID:%lld"), pApMsgInfo->m_nMsgId);
	//CDlgMessageBox::EbMessageBox(this,swindow,stext,CDlgMessageBox::IMAGE_INFORMATION);
	//theEBAppClient.EB_AckMsg(pApMsgInfo->m_nMsgId,true);

	CString sSql;
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		// 删除旧会话
		CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,pRequestAccountInfo->GetUserId(),true);
		if (pOldCallInfo.get()!=NULL)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,pRequestAccountInfo->GetUserId());
			theApp.m_pBoUsers->execute(sSql);
		}
	}

	// emp_code=-1,from_type=msg-type
	const int nMsgType = EBC_MSG_TYPE_INVITE_JOIN_2_GROUP;
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) ")\
		_T("VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
		pApMsgInfo->m_nMsgId,nGroupId,pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),nMsgType);
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = pApMsgInfo->m_nMsgId;
		pCallRecordInfo->m_sGroupCode = nGroupId;
		pCallRecordInfo->m_sMemberCode = -1;
		pCallRecordInfo->m_nFromUserId = pRequestAccountInfo->GetUserId();
		pCallRecordInfo->m_sFromAccount = sRequestAccount;
		pCallRecordInfo->m_nFromType = nMsgType;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}
#endif
	if (theApp.GetGroupMsgSugId()>0)
	{
		CString sGroupName;
		CString sMsgContent(sDescription.c_str());
		const int nFind = sMsgContent.Find(";");
		if (nFind>=0)
		{
			sGroupName = sMsgContent.Left(nFind);
			sMsgContent = sMsgContent.Mid(nFind+1);
		}
		if (sGroupName.IsEmpty())
			sGroupName.Format(_T("%lld"),nGroupId);

		CString sContent;
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>邀请你加入群组：%s<br/>附加消息：%s</font><br/>\
						   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"),
			sRequestAccount.c_str(),sGroupName,sMsgContent.Left(20),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->m_nMsgType = nMsgType;
		pFuncWindow->m_bOpenNewClose = true;
		pFuncWindow->m_sTitle = _T("验证消息");
		pFuncWindow->m_sFuncUrl = libEbc::ACP2UTF8(sContent);
		pFuncWindow->m_nWidth = 250;
		pFuncWindow->m_nHeight = 180;
		CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
		pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
		pEbcMsgInfo->m_pParent = pParent;
		pEbcMsgInfo->m_bSetForegroundWindow = true;
		m_pEbcMsgList.add(pEbcMsgInfo);
	}

	return 0;
}

LRESULT CDlgMainFrame::OnMessageRejectJoin2Group(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_AccountInfo* pRequestAccountInfo = (IEB_AccountInfo*)wParam;
	IEB_APMsgInfo* pApMsgInfo = (IEB_APMsgInfo*)lParam;
	const eb::bigint nRequestUserId = pRequestAccountInfo->UserId;
	const eb::bigint nMsgId = pApMsgInfo->MsgId;
	const eb::bigint nGroupId = pApMsgInfo->GroupId;
	const CEBString sRequestAccount(pRequestAccountInfo->Account.GetBSTR());
	const CEBString sDescription(pApMsgInfo->MsgContent.GetBSTR());
	//theEBClientCore->EB_AckMsg(pApMsgInfo->MsgId,VARIANT_TRUE);

	//CString sSql;
	//if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	//{
	//	// 删除旧会话
	//	CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,nRequestUserId,true);
	//	if (pOldCallInfo.get()!=NULL)
	//	{
	//		sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,nRequestUserId);
	//		theApp.m_pBoUsers->execute(sSql);
	//	}
	//}

	//// emp_code=-1,from_type=msg-type
	//const int nMsgType = EBC_MSG_TYPE_INVITE_JOIN_2_GROUP;
	//sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) ")\
	//			   _T("VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
	//			   nMsgId,nGroupId,nRequestUserId,sRequestAccount.c_str(),nMsgType);
	//theApp.m_pBoUsers->execute(sSql);
	//if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	//{
	//	CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
	//	pCallRecordInfo->m_sCallId = nMsgId;
	//	pCallRecordInfo->m_sGroupCode = nGroupId;
	//	pCallRecordInfo->m_sMemberCode = -1;
	//	pCallRecordInfo->m_nFromUserId = nRequestUserId;
	//	pCallRecordInfo->m_sFromAccount = sRequestAccount;
	//	pCallRecordInfo->m_nFromType = nMsgType;
	//	pCallRecordInfo->m_tTime = time(0);
	//	pCallRecordInfo->m_bRead = false;
	//	m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	//}
#else
	const EB_AccountInfo* pRequestAccountInfo = (const EB_AccountInfo*)wParam;
	const EB_APMsgInfo* pApMsgInfo = (const EB_APMsgInfo*)lParam;
	const CEBString sRequestAccount(pRequestAccountInfo->GetAccount());
	const eb::bigint nGroupId = pApMsgInfo->m_nGroupId;
	const CEBString sDescription(pApMsgInfo->m_sMsgContent);

	////CString stext;
	////stext.Format(_T("%lld(%s)邀请加入群组%s(%lld):%s"),
	////	pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),pApMsgInfo->m_sMsgName.c_str(),nGroupId,pApMsgInfo->m_sMsgContent.c_str());
	////CString swindow;
	////swindow.Format(_T("MSG-ID:%lld"), pApMsgInfo->m_nMsgId);
	////CDlgMessageBox::EbMessageBox(this,swindow,stext,CDlgMessageBox::IMAGE_INFORMATION);
	////theEBAppClient.EB_AckMsg(pApMsgInfo->m_nMsgId,true);

	//CString sSql;
	//if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	//{
	//	// 删除旧会话
	//	CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,pRequestAccountInfo->GetUserId(),true);
	//	if (pOldCallInfo.get()!=NULL)
	//	{
	//		sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,pRequestAccountInfo->GetUserId());
	//		theApp.m_pBoUsers->execute(sSql);
	//	}
	//}

	//// emp_code=-1,from_type=msg-type
	//const int nMsgType = EBC_MSG_TYPE_INVITE_JOIN_2_GROUP;
	//sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) \
	//			   VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
	//			   pApMsgInfo->m_nMsgId,nGroupId,pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),nMsgType);
	//theApp.m_pBoUsers->execute(sSql);
	//if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	//{
	//	CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
	//	pCallRecordInfo->m_sCallId = pApMsgInfo->m_nMsgId;
	//	pCallRecordInfo->m_sGroupCode = nGroupId;
	//	pCallRecordInfo->m_sMemberCode = -1;
	//	pCallRecordInfo->m_nFromUserId = pRequestAccountInfo->GetUserId();
	//	pCallRecordInfo->m_sFromAccount = sRequestAccount;
	//	pCallRecordInfo->m_nFromType = nMsgType;
	//	pCallRecordInfo->m_tTime = time(0);
	//	pCallRecordInfo->m_bRead = false;
	//	m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	//}
#endif
	//if (theApp.GetGroupMsgSugId()>0)
	//{
	//	CString sGroupName;
	//	CString sMsgContent(sDescription.c_str());
	//	const int nFind = sMsgContent.Find(";");
	//	if (nFind>=0)
	//	{
	//		sGroupName = sMsgContent.Left(nFind);
	//		sMsgContent = sMsgContent.Mid(nFind+1);
	//	}
	//	if (sGroupName.IsEmpty())
	//		sGroupName.Format(_T("%lld"),nGroupId);

	//	CString sContent;
	//	sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>邀请你加入群组：%s<br/>附加消息：%s</font><br/>\
	//					   <a href=\"eb-open-subid://%lld\">查看我的消息</a>"),
	//		sRequestAccount.c_str(),sGroupName,sMsgContent.Left(20),theApp.GetGroupMsgSugId());
	//	CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
	//	CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
	//	pFuncWindow->m_bDisableContextMenu = true;
	//	pFuncWindow->m_bBroadcastMsg = true;
	//	pFuncWindow->m_nMsgType = nMsgType;
	//	pFuncWindow->m_bOpenNewClose = true;
	//	pFuncWindow->m_sTitle = _T("验证消息");
	//	pFuncWindow->m_sFuncUrl = (LPCTSTR)sContent;
	//	pFuncWindow->m_nWidth = 250;
	//	pFuncWindow->m_nHeight = 180;
	//	pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
	//}

	return 0;
}

LRESULT CDlgMainFrame::OnMessageMemberInfo(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_MemberInfo * pMemberInfo = (IEB_MemberInfo*)wParam;
	bool bIsMyDefaultMember = lParam==1;
	const eb::bigint sGroupCode(pMemberInfo->GroupCode);
	const CEBString sUserName(pMemberInfo->UserName.GetBSTR());
	const CEBString sMemberAccount(pMemberInfo->MemberAccount.GetBSTR());
	if (m_pDlgMyEnterprise != NULL)
		m_pDlgMyEnterprise->EmployeeInfo(pMemberInfo);
	if (m_pDlgMyGroup != NULL)
	{
		if (theEBClientCore->EB_IsMyGroup(sGroupCode))
		{
			m_pDlgMyGroup->MyDepMemberInfo(pMemberInfo,false);
		}
	}
	if (bIsMyDefaultMember)
	{
		this->Invalidate();	// 刷新重画自己头像
		CEBString sSettingEnterprise;
		if (!theApp.GetProductName().IsEmpty())
			sSettingEnterprise = (LPCTSTR)theApp.GetProductName();
		else
			sSettingEnterprise = theEBSetting->Enterprise.GetBSTR();
		CString sWindowText;
		CComPtr<IEB_EnterpriseInfo> pEnterpriseInfo = theEBClientCore->EB_GetEnterpriseInfo(0);
		if (pEnterpriseInfo != NULL)
		{
			sWindowText.Format(_T("%s-%s(%s) %s"),sSettingEnterprise.c_str(),sUserName.c_str(),
				sMemberAccount.c_str(),CEBString(pEnterpriseInfo->EnterpriseName.GetBSTR()).c_str());
		}else
		{
			sWindowText.Format(_T("%s-%s(%s)"),sSettingEnterprise.c_str(),sUserName.c_str(),
				sMemberAccount.c_str());
		}
		this->SetWindowText(sWindowText);
	}
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->OnUserEmpInfo(pMemberInfo);
	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(sGroupCode, pDlgDialog))
	//{
	//	pDlgDialog->OnUserEmpInfo(pMemberInfo);
	//}
#else
	const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)wParam;
	const bool bIsMyDefaultMember = (bool)(lParam==1);
	if (m_pDlgMyEnterprise != NULL)
		m_pDlgMyEnterprise->EmployeeInfo(pMemberInfo);
	if (m_pDlgMyGroup != NULL)
	{
		if (theEBAppClient.EB_IsMyGroup(pMemberInfo->m_sGroupCode))
		{
			m_pDlgMyGroup->MyDepMemberInfo(pMemberInfo,false);
		}
	}
	if (bIsMyDefaultMember)
	{
		this->Invalidate();	// 刷新重画自己头像
		CEBString sSettingEnterprise;
		if (!theApp.GetProductName().IsEmpty())
			sSettingEnterprise = (LPCTSTR)theApp.GetProductName();
		else
			sSettingEnterprise = theSetting.GetEnterprise();
		CString sWindowText;
		EB_EnterpriseInfo pEnterpriseInfo;
		if (theEBAppClient.EB_GetEnterpriseInfo(&pEnterpriseInfo))
		{
			sWindowText.Format(_T("%s-%s(%s) %s"),sSettingEnterprise.c_str(),pMemberInfo->m_sUserName.c_str(),
				pMemberInfo->m_sMemberAccount.c_str(),pEnterpriseInfo.m_sEnterpriseName.c_str());
		}else
		{
			sWindowText.Format(_T("%s-%s(%s)"),sSettingEnterprise.c_str(),pMemberInfo->m_sUserName.c_str(),
				pMemberInfo->m_sMemberAccount.c_str());
		}
		this->SetWindowText(sWindowText);
	}

	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->OnUserEmpInfo(pMemberInfo);
	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(pMemberInfo->m_sGroupCode, pDlgDialog))
	//{
	//	pDlgDialog->OnUserEmpInfo(pMemberInfo);
	//}
#endif
	return 0;
}

//LRESULT CDlgMainFrame::OnMessageMemberDelete(WPARAM wParam, LPARAM lParam)
//{
//	// ***统一在EB_WM_REMOVE_GROUP消息处理
////#ifdef USES_EBCOM_TEST
////	IEB_MemberInfo* pMemberInfo = (IEB_MemberInfo*)wParam;
////	bool bIsMyDefaultMember = (bool)(lParam==1);
////	if (m_pDlgMyEnterprise != NULL)
////		m_pDlgMyEnterprise->DeleteEmployeeInfo(pMemberInfo);
////	// 我的部门
////	if (m_pDlgMyGroup != NULL && theEBClientCore->EB_IsMyGroup(pMemberInfo->GroupCode))
////		m_pDlgMyGroup->DeleteEmployeeInfo(pMemberInfo);
////#else
////	const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)wParam;
////	bool bIsMyDefaultMember = (bool)(lParam==1);
////	if (m_pDlgMyEnterprise != NULL)
////		m_pDlgMyEnterprise->DeleteEmployeeInfo(pMemberInfo);
////	// 我的部门
////	if (m_pDlgMyGroup != NULL && theEBAppClient.EB_IsMyGroup(pMemberInfo->m_sGroupCode))
////		m_pDlgMyGroup->DeleteEmployeeInfo(pMemberInfo);
////#endif
//	return 0;
//}
LRESULT CDlgMainFrame::OnMessageMemberEditError(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nErrorCode = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nErrorCode || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nErrorCode)
		return 0;

#ifdef USES_EBCOM_TEST
	IEB_MemberInfo* pMemberInfo = (IEB_MemberInfo*)wParam;
#else
	const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)wParam;
#endif
	switch (nErrorCode)
	{
	case EB_STATE_OAUTH_FORWARD:
		CDlgMessageBox::EbMessageBox(this,_T("邀请成员成功，等待对方接受！"),_T(""),CDlgMessageBox::IMAGE_INFORMATION,5);
		break;
	case EB_STATE_NOT_AUTH_ERROR:
		CDlgMessageBox::EbMessageBox(this,_T("对不起，你没有权限进行操作！"),_T(""),CDlgMessageBox::IMAGE_ERROR,5);
		break;
	case EB_STATE_ACCOUNT_ALREADY_EXIST:
		CDlgMessageBox::EbMessageBox(this,_T("添加失败："),_T("该帐号已经存在"),CDlgMessageBox::IMAGE_ERROR,5);
		break;
	case EB_STATE_ANOTHER_ENT_ACCOUNT:
		CDlgMessageBox::EbMessageBox(this,_T("这是其他企业员工帐号，请重新输入！"),_T(""),CDlgMessageBox::IMAGE_ERROR,5);
		break;
	default:
		CDlgMessageBox::EbMessageBox(this,_T("编辑失败："),_T("请重试或联系客服！"),CDlgMessageBox::IMAGE_ERROR,5);
		break;
	}
	return 0;
}
LRESULT CDlgMainFrame::OnMessageRequestAddContact(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_AccountInfo* pRequestAccountInfo = (IEB_AccountInfo*)wParam;
	IEB_APMsgInfo* pApMsgInfo = (IEB_APMsgInfo*)lParam;
	const eb::bigint nRequestUserId = pRequestAccountInfo->UserId;
	const eb::bigint nMsgId = pApMsgInfo->MsgId;
	const eb::bigint nGroupId = pApMsgInfo->GroupId;
	const CEBString sRequestAccount(pRequestAccountInfo->Account.GetBSTR());
	const CEBString sDescription(pApMsgInfo->MsgContent.GetBSTR());
	//theEBClientCore->EB_AckMsg(pApMsgInfo->MsgId,VARIANT_TRUE);

	CString sSql;
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		// 删除旧会话
		CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,nRequestUserId,true);
		if (pOldCallInfo.get()!=NULL)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,nRequestUserId);
			theApp.m_pBoUsers->execute(sSql);
		}
	}
	// emp_code=-1,from_type=msg-type
	const int nMsgType = EBC_MSG_TYPE_REQUEST_ADD_CONTACT;
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) ")\
		_T("VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
		nMsgId,nGroupId,nRequestUserId,sRequestAccount.c_str(),nMsgType);
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = nMsgId;
		//pCallRecordInfo->m_sGroupCode = nGroupId;
		pCallRecordInfo->m_sMemberCode = -1;
		pCallRecordInfo->m_nFromUserId = nRequestUserId;
		pCallRecordInfo->m_sFromAccount = sRequestAccount;
		pCallRecordInfo->m_nFromType = nMsgType;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}
#else
	const EB_AccountInfo* pRequestAccountInfo = (const EB_AccountInfo*)wParam;
	const EB_APMsgInfo* pApMsgInfo = (const EB_APMsgInfo*)lParam;
	const CEBString sRequestAccount(pRequestAccountInfo->GetAccount());
	const CEBString sDescription(pApMsgInfo->m_sMsgContent);
	const eb::bigint nGroupId = pApMsgInfo->m_nGroupId;

	CString sSql;
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		// 删除旧会话
		CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,pRequestAccountInfo->GetUserId(),true);
		if (pOldCallInfo.get()!=NULL)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,pRequestAccountInfo->GetUserId());
			theApp.m_pBoUsers->execute(sSql);
		}
	}

	// emp_code=-1,from_type=msg-type
	const int nMsgType = EBC_MSG_TYPE_REQUEST_ADD_CONTACT;
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) ")\
		_T("VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
		pApMsgInfo->m_nMsgId,nGroupId,pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),nMsgType);
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = pApMsgInfo->m_nMsgId;
		//pCallRecordInfo->m_sGroupCode = nGroupId;
		pCallRecordInfo->m_sMemberCode = -1;
		pCallRecordInfo->m_nFromUserId = pRequestAccountInfo->GetUserId();
		pCallRecordInfo->m_sFromAccount = sRequestAccount;
		pCallRecordInfo->m_nFromType = nMsgType;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}
#endif

	if (theApp.GetGroupMsgSugId()>0)
	{
		CString sContent;
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>申请加为好友<br/>附加消息：%s</font><br/>\
						   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"),
			sRequestAccount.c_str(),sDescription.substr(0,30).c_str(),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->m_nMsgType = nMsgType;
		pFuncWindow->m_bOpenNewClose = true;
		pFuncWindow->m_sTitle = _T("验证消息");
		pFuncWindow->m_sFuncUrl = libEbc::ACP2UTF8(sContent);
		pFuncWindow->m_nWidth = 250;
		pFuncWindow->m_nHeight = 180;
		CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
		pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
		pEbcMsgInfo->m_pParent = pParent;
		pEbcMsgInfo->m_bSetForegroundWindow = true;
		m_pEbcMsgList.add(pEbcMsgInfo);
	}
	return 0;
}
LRESULT CDlgMainFrame::OnMessageRejectAddContact(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_AccountInfo* pRequestAccountInfo = (IEB_AccountInfo*)wParam;
	IEB_APMsgInfo* pApMsgInfo = (IEB_APMsgInfo*)lParam;
	const eb::bigint nRequestUserId = pRequestAccountInfo->UserId;
	const eb::bigint nMsgId = pApMsgInfo->MsgId;
	const eb::bigint nGroupId = pApMsgInfo->GroupId;
	const CEBString sRequestAccount(pRequestAccountInfo->Account.GetBSTR());
	const CEBString sDescription(pApMsgInfo->MsgContent.GetBSTR());
	//theEBClientCore->EB_AckMsg(pApMsgInfo->MsgId,VARIANT_TRUE);

	CString sSql;
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		// 删除旧会话
		CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,nRequestUserId,true);
		if (pOldCallInfo.get()!=NULL)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,nRequestUserId);
			theApp.m_pBoUsers->execute(sSql);
		}
	}
	// emp_code=-1,from_type=msg-type
	const int nMsgType = EBC_MSG_TYPE_REJECT_ADD_CONTACT;
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) ")\
		_T("VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
		nMsgId,nGroupId,nRequestUserId,sRequestAccount.c_str(),nMsgType);
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = nMsgId;
		//pCallRecordInfo->m_sGroupCode = nGroupId;
		pCallRecordInfo->m_sMemberCode = -1;
		pCallRecordInfo->m_nFromUserId = nRequestUserId;
		pCallRecordInfo->m_sFromAccount = sRequestAccount;
		pCallRecordInfo->m_nFromType = nMsgType;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}
#else
	const EB_AccountInfo* pRequestAccountInfo = (const EB_AccountInfo*)wParam;
	const EB_APMsgInfo* pApMsgInfo = (const EB_APMsgInfo*)lParam;
	const CEBString sRequestAccount(pRequestAccountInfo->GetAccount());
	const CEBString sDescription(pApMsgInfo->m_sMsgContent);
	const eb::bigint nGroupId = pApMsgInfo->m_nGroupId;

	CString sSql;
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		// 删除旧会话
		CCallRecordInfo::pointer pOldCallInfo = m_pDlgMySession->GetCallRecordInfo(nGroupId,pRequestAccountInfo->GetUserId(),true);
		if (pOldCallInfo.get()!=NULL)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0"),nGroupId,pRequestAccountInfo->GetUserId());
			theApp.m_pBoUsers->execute(sSql);
		}
	}

	// emp_code=-1,from_type=msg-type
	const int nMsgType = EBC_MSG_TYPE_REJECT_ADD_CONTACT;
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) ")\
		_T("VALUES(%lld,%lld,-1,%lld,'%s',%d)"),
		pApMsgInfo->m_nMsgId,nGroupId,pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),nMsgType);
	theApp.m_pBoUsers->execute(sSql);
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
		pCallRecordInfo->m_sCallId = pApMsgInfo->m_nMsgId;
		//pCallRecordInfo->m_sGroupCode = nGroupId;
		pCallRecordInfo->m_sMemberCode = -1;
		pCallRecordInfo->m_nFromUserId = pRequestAccountInfo->GetUserId();
		pCallRecordInfo->m_sFromAccount = sRequestAccount;
		pCallRecordInfo->m_nFromType = nMsgType;
		pCallRecordInfo->m_tTime = time(0);
		pCallRecordInfo->m_bRead = false;
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}
#endif

	if (theApp.GetGroupMsgSugId()>0)
	{
		CString sContent;
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>拒绝加为好友<br/>附加消息：%s</font><br/>\
						   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"),
			sRequestAccount.c_str(),sDescription.substr(0,30).c_str(),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->m_nMsgType = nMsgType;
		pFuncWindow->m_bOpenNewClose = true;
		pFuncWindow->m_sTitle = _T("通知消息");
		pFuncWindow->m_sFuncUrl = libEbc::ACP2UTF8(sContent);
		pFuncWindow->m_nWidth = 250;
		pFuncWindow->m_nHeight = 180;
		CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
		pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
		pEbcMsgInfo->m_pParent = pParent;
		//pEbcMsgInfo->m_bSetForegroundWindow = true;
		m_pEbcMsgList.add(pEbcMsgInfo);
	}
	return 0;
}
LRESULT CDlgMainFrame::OnMessageAcceptAddContact(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_ContactInfo * pContactInfo = (IEB_ContactInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->ContactInfo(pContactInfo);
	const CEBString sName = pContactInfo->Name.GetBSTR();
#else
	const EB_ContactInfo* pPopContactInfo = (const EB_ContactInfo*)wParam;
	if (m_pDlgMyContacts != NULL)
		m_pDlgMyContacts->ContactInfo(pPopContactInfo);
	const CEBString sName = pPopContactInfo->m_sName;
#endif
	if (theApp.GetGroupMsgSugId()>0)
	{
		CString sContent;
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>添加好友成功</font><br/>\
						   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"),
						   sName.c_str(),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->m_nMsgType = EBC_MSG_TYPE_ACCERPT_ADD_CONTACT;
		pFuncWindow->m_bOpenNewClose = true;
		pFuncWindow->m_sTitle = _T("通知消息");
		pFuncWindow->m_sFuncUrl = libEbc::ACP2UTF8(sContent);
		pFuncWindow->m_nWidth = 250;
		pFuncWindow->m_nHeight = 180;
		CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
		pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
		pEbcMsgInfo->m_pParent = pParent;
		//pEbcMsgInfo->m_bSetForegroundWindow = true;
		m_pEbcMsgList.add(pEbcMsgInfo);
	}

	return 0;
}


//LRESULT CDlgMainFrame::OnMessageAppMsgInfo(WPARAM wParam, LPARAM lParam)
//{
//	const EB_AppMsgInfo * pMsgInfo = (const EB_AppMsgInfo*)wParam;
//	if (pMsgInfo == NULL) return 1;
//	switch (pMsgInfo->GetMsgType())
//	{
//	case EB_MSG_REMOVE_GROUP:
//		{
//			//const tstring sRemoveAccount = pMsgInfo->GetMsgContent();
//			//EB_GroupInfo pGroupInfo;
//			//if (!theEBAppClient.EB_GetGroupInfo(pMsgInfo->m_sGroupCode,&pGroupInfo))
//			//	break;
//			//// 找到现在会话，移除用户数据
//			//CString stext;
//			//if (sRemoveAccount == theEBAppClient.EB_GetLogonAccount())
//			//{
//			//	theApp.m_pCallList.remove(pMsgInfo->m_sGroupCode);
//
//			//	CDlgDialog::pointer pDlgDialog;
//			//	if (theApp.m_pDialogList.find(pMsgInfo->m_sGroupCode, pDlgDialog, true))
//			//	{
//			//		//pDlgDialog->ExitRoom(1);
//			//		pDlgDialog->DestroyWindow();
//			//	}
//			//	if (m_pDlgMyGroup)
//			//		m_pDlgMyGroup->DeleteDepartment(pMsgInfo->m_sGroupCode);
//			//	stext.Format(_T("你被管理员移除出%s"), pGroupInfo.m_sGroupName.c_str());
//			//	CDlgMessageBox::EbMessageBox(this,"",stext);
//			//}else
//			//{
//			//	CDlgDialog::pointer pDlgDialog;
//			//	if (theApp.m_pDialogList.find(pMsgInfo->m_sGroupCode, pDlgDialog))
//			//	{
//			//		pDlgDialog->UserExitRoom(sRemoveAccount.c_str(),true);
//			//	}
//			//	if (m_pDlgMyGroup)
//			//		m_pDlgMyGroup->ExitDepartment(pMsgInfo->m_sGroupCode, sRemoveAccount);
//			//	//stext.Format(_T("%s用户被管理员移除出%s"), sRemoveAccount.c_str(), pGroupInfo->m_sGroupName.c_str());
//			//	//CDlgMessageBox::EbMessageBox(this,"",stext);
//			//}
//		}break;
//	case EB_MSG_EXIT_GROUP:
//		{
//			//const tstring sExitAccount = pMsgInfo->GetMsgContent();
//			//// 找到现在会话，移除用户数据
//			//if (sExitAccount == theEBAppClient.EB_GetLogonAccount())
//			//{
//			//	theApp.m_pCallList.remove(pMsgInfo->m_sGroupCode);
//			//	CDlgDialog::pointer pDlgDialog;
//			//	if (theApp.m_pDialogList.find(pMsgInfo->m_sGroupCode, pDlgDialog, true))
//			//	{
//			//		//pDlgDialog->ExitRoom(1);
//			//		pDlgDialog->DestroyWindow();
//			//	}
//			//	if (m_pDlgMyGroup)
//			//		m_pDlgMyGroup->DeleteDepartment(pMsgInfo->m_sGroupCode);
//			//	break;
//			//}else
//			//{
//			//	CDlgDialog::pointer pDlgDialog;
//			//	if (theApp.m_pDialogList.find(pMsgInfo->m_sGroupCode, pDlgDialog))
//			//	{
//			//		pDlgDialog->UserExitRoom(sExitAccount.c_str(),true);
//			//	}
//			//	if (m_pDlgMyGroup)
//			//		m_pDlgMyGroup->ExitDepartment(pMsgInfo->m_sGroupCode, sExitAccount);
//			//}
//
//			//EB_GroupInfo pGroupInfo;
//			//if (!theEBAppClient.EB_GetGroupInfo(pMsgInfo->m_sGroupCode,&pGroupInfo))
//			//	break;
//			//if (pGroupInfo.m_sCreator==theEBAppClient.EB_GetLogonAccount())
//			//{
//			//	CString stext;
//			//	stext.Format(_T("已经退出%s"), pGroupInfo.m_sGroupName.c_str());
//			//	CDlgMessageBox::EbMessageBox(this,sExitAccount.c_str(),stext);
//			//}
//		}break;
//	case EB_MSG_DELETE_GROUP:
//		{
//			if (m_pDlgMyGroup)
//				m_pDlgMyGroup->DeleteDepartment(pMsgInfo->m_sGroupCode);
//			// 关闭现在会话窗口
//			theApp.m_pCallList.remove(pMsgInfo->m_sGroupCode);
//			CDlgDialog::pointer pDlgDialog;
//			if (theApp.m_pDialogList.find(pMsgInfo->m_sGroupCode, pDlgDialog, true))
//			{
//				//pDlgDialog->ExitRoom(1);
//				pDlgDialog->DestroyWindow();
//			}
//			CString stext;
//			if (pMsgInfo->m_sFromAccount == theEBAppClient.EB_GetLogonAccount())
//			{
//				stext.Format(_T("成功解散%s"), pMsgInfo->GetMsgContent().c_str());
//			}else
//			{
//				stext.Format(_T("已经解散%s"), pMsgInfo->GetMsgContent().c_str());
//			}
//			CDlgMessageBox::EbMessageBox(this,pMsgInfo->m_sFromAccount.c_str(),stext);
//		}break;
//	default:
//		break;
//	}
//	return 0;
//}

LRESULT CDlgMainFrame::OnMessageResourceInfo(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_ResourceInfo* pResourceInfo = (IEB_ResourceInfo*)wParam;
	const eb::bigint sGroupCode = pResourceInfo->GroupCode;
	const eb::bigint sEnterpriseCode = pResourceInfo->EnterpriseCode;
	eb::bigint sId = 0;
	if (sGroupCode>0)
	{
		sId = sGroupCode;
	}else if (sEnterpriseCode>0)
	{
		sId = sEnterpriseCode;
	}else
	{
		sId = theApp.GetLogonUserId();
	}
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (theApp.m_pResourceMgr.find(sId,pDlgResourceMgr))
	{
		pDlgResourceMgr->OnResourceInfo(pResourceInfo);
	}
#else
	const EB_ResourceInfo* pResourceInfo = (const EB_ResourceInfo*)wParam;
	eb::bigint sId = 0;
	if (pResourceInfo->m_sGroupCode>0)
	{
		sId = pResourceInfo->m_sGroupCode;
	}else if (pResourceInfo->m_sEnterpriseCode>0)
	{
		sId = pResourceInfo->m_sEnterpriseCode;
	}else
	{
		sId = theEBAppClient.EB_GetLogonUserId();
	}
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (theApp.m_pResourceMgr.find(sId,pDlgResourceMgr))
	{
		pDlgResourceMgr->OnResourceInfo(*pResourceInfo);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageDeleteResource(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_ResourceInfo* pResourceInfo = (IEB_ResourceInfo*)wParam;
	const eb::bigint sGroupCode = pResourceInfo->GroupCode;
	const eb::bigint sEnterpriseCode = pResourceInfo->EnterpriseCode;
	eb::bigint sId = 0;
	if (sGroupCode>0)
	{
		sId = sGroupCode;
	}else if (sEnterpriseCode>0)
	{
		sId = sEnterpriseCode;
	}else
	{
		sId = theApp.GetLogonUserId();
	}
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (theApp.m_pResourceMgr.find(sId,pDlgResourceMgr))
	{
		pDlgResourceMgr->OnResourceDelete(pResourceInfo);
	}
#else
	const EB_ResourceInfo* pResourceInfo = (const EB_ResourceInfo*)wParam;
	eb::bigint sId = 0;
	if (pResourceInfo->m_sGroupCode>0)
	{
		sId = pResourceInfo->m_sGroupCode;
	}else if (pResourceInfo->m_sEnterpriseCode>0)
	{
		sId = pResourceInfo->m_sEnterpriseCode;
	}else
	{
		sId = theEBAppClient.EB_GetLogonUserId();
	}
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (theApp.m_pResourceMgr.find(sId,pDlgResourceMgr))
	{
		pDlgResourceMgr->OnResourceDelete(*pResourceInfo);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnMessageMoveResource(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_ResourceInfo* pResourceInfo = (IEB_ResourceInfo*)wParam;
	const CEBString sOldParentResId = (BSTR)lParam;
	const eb::bigint sGroupCode = pResourceInfo->GroupCode;
	const eb::bigint sEnterpriseCode = pResourceInfo->EnterpriseCode;
	eb::bigint sId = 0;
	if (sGroupCode>0)
	{
		sId = sGroupCode;
	}else if (sEnterpriseCode>0)
	{
		sId = sEnterpriseCode;
	}else
	{
		sId = theApp.GetLogonUserId();
	}
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (theApp.m_pResourceMgr.find(sId,pDlgResourceMgr))
	{
		pDlgResourceMgr->OnResourceMove(pResourceInfo,sOldParentResId);
	}
#else
	const EB_ResourceInfo* pResourceInfo = (const EB_ResourceInfo*)wParam;
	const char* sOldParentResId = (const char*)lParam;
	eb::bigint sId = 0;
	if (pResourceInfo->m_sGroupCode>0)
	{
		sId = pResourceInfo->m_sGroupCode;
	}else if (pResourceInfo->m_sEnterpriseCode>0)
	{
		sId = pResourceInfo->m_sEnterpriseCode;
	}else
	{
		sId = theEBAppClient.EB_GetLogonUserId();
	}
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (theApp.m_pResourceMgr.find(sId,pDlgResourceMgr))
	{
		pDlgResourceMgr->OnResourceMove(*pResourceInfo,sOldParentResId);
	}
#endif
	return 0;
}

CDlgDialog::pointer CDlgMainFrame::GetDlgDialog(const CEBCCallInfo::pointer & pEbCallInfo,bool bShow)
{
	CreateFrameList(bShow);
	const eb::bigint sCallId = pEbCallInfo->m_pCallInfo.GetCallId();
	CDlgDialog::pointer pDlgDialog;
	if (pEbCallInfo->m_pCallInfo.m_sOldCallId>0)
	{
		pDlgDialog = m_pDlgFrameList->GetCallIdDialog(pEbCallInfo->m_pCallInfo.m_sOldCallId);
		if (pDlgDialog.get()!=NULL)
		{
			// 找到old call id
			pDlgDialog->SetCallInfo(pEbCallInfo);
		}
	}
	if (pDlgDialog.get()==NULL)
	{
		pDlgDialog = m_pDlgFrameList->GetCallIdDialog(sCallId);
		if (pDlgDialog.get()==NULL)
		{
			AFX_SWITCH_INSTANCED();
			CWnd * pParent = m_pDlgFrameList;
			pDlgDialog = CDlgDialog::create(pParent);
			pDlgDialog->SetCallInfo(pEbCallInfo);
			pDlgDialog->Create(CDlgDialog::IDD, pParent);
			CFrameWndInfo::pointer pFrameWndInfo = CFrameWndInfo::create(pDlgDialog);
			m_pDlgFrameList->AddWnd(pFrameWndInfo, bShow);
		}else if (pEbCallInfo->m_pCallInfo.m_sOldCallId!=pDlgDialog->GetCallInfo()->m_pCallInfo.m_sOldCallId)
		{
			// 更新old call id
			pDlgDialog->SetCallInfo(pEbCallInfo);
		}
	}
	if (bShow)
	{
		m_pDlgFrameList->ShowWnd(pEbCallInfo->m_pCallInfo.GetCallId());

		// **不能添加，否则会发生异常；
		//CDlgFuncWindow * pOldFuncWindow = NULL;
		//if (pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
		//{
		//	theApp.m_pTempGroupMsg.find(pEbCallInfo->m_pCallInfo.m_sGroupCode,pOldFuncWindow,true);
		//}else
		//{
		//	theApp.m_pTempFromUserMsg.find(pEbCallInfo->m_pFromAccountInfo.GetUserId(),pOldFuncWindow,true);
		//}
		//// **必须放在前面
		//if (pOldFuncWindow!=NULL)
		//{
		//	pOldFuncWindow->ShowWindow(SW_HIDE);	// *必须隐藏不让用户点击；
		//	pOldFuncWindow->m_pEbCallInfo.reset();	// *必须设为空，否则会清掉后面窗口
		//	theApp.m_pCloseWnd.add(pOldFuncWindow);
		//	//pOldFuncWindow->DestroyWindow();
		//}
	}
	return pDlgDialog;
}

#ifndef USES_MIN_UI
LRESULT CDlgMainFrame::OnMessageHotKey(WPARAM wParam, LPARAM lParam)
{
	UINT fuModifiers = (UINT) LOWORD(lParam);  // key-modifier flags   
	UINT uVirtKey = (UINT) HIWORD(lParam);     // virtual-key code   

	//判断响应了什么热键  
	if( (MOD_ALT|MOD_SHIFT) == fuModifiers && 'A' == uVirtKey )  
	{  
		CString sParameter;
		sParameter.Format(_T("\"\" %d %d\""),(int)EB_MSG_EBSC_OK,(int)this->GetForegroundWindow()->GetSafeHwnd());	// OK
		//sParameter.Format(_T("\"\" %d %d\""),(int)EB_MSG_EBSC_OK,(int)this->GetTopWindow()->GetSafeHwnd());
		//CString sWindowText;
		//this->GetForegroundWindow()->GetWindowText(sWindowText);
		//sParameter.Format(_T("\"%s\" %d %d"),sWindowText,(int)EB_MSG_EBSC_OK);
		theApp.RunEBSC(sParameter);
	}else if( (MOD_ALT|MOD_SHIFT) == fuModifiers && 'Z' == uVirtKey ) 
	{
		if (theApp.GetHideMainFrame() && theApp.GetAutoOpenSubId()>0)
		{
			theApp.ShowHideAutoOpenSubscribeFunc(true);
		}else if (this->IsWindowVisible())
		{
			this->ShowWindow(SW_HIDE);
			if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
				m_pPanelSearch->ShowWindow(SW_HIDE);
		}else
		{
			OnOpenMain();
		}
	}else if( (MOD_ALT|MOD_SHIFT) == fuModifiers && 'T' == uVirtKey ) 
	{
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnExitRD();
	}
	return 0;
}
#endif

//LRESULT CDlgMainFrame::OnMessageWindowResize(WPARAM wParam, LPARAM lParam)
//{
//	SetCircle();
//	this->Invalidate();
//	return 0;
//}


//void CDlgMainFrame::OnBnClickedButtonSelectuser()
//{
//	CDlgDiaRecord pDlgCallNumber(this);
//	if (pDlgCallNumber.DoModal() == IDOK)
//	{
//		m_sCallNumber = pDlgCallNumber.m_sCallNumber;
//		UpdateData(FALSE);
//		m_comboNumbers.InsertString(0, m_sCallNumber);
//	}
//}
void CDlgMainFrame::InitData(void)
{
	//
}


//void CDlgMainFrame::OnCbnSelchangeComboNumbers()
//{
//	int ncursel = m_comboNumbers.GetCurSel();
//	if (ncursel >= 0)
//	{
//		CString sAccount;
//		m_comboNumbers.GetLBText(ncursel, sAccount);
//		//m_sCallNumber = sAccount;
//		UpdateData(FALSE);
//	}
//}

void CDlgMainFrame::MoveSize(int cx, int cy)
{
	const int const_text_intever = 1;
	const int const_number_height = 22;
	const int const_leftr_tool_width = 0;
#ifndef USES_MIN_UI
	int y = m_bShowedToolbar?POS_LINESTATE_TOP:3;
	m_btnLineState.MovePoint(POS_LINESTATE_LEFT,y);
	if (m_btnLineState.GetSafeHwnd()!=NULL)
		m_btnLineState.ShowWindow(m_bShowedToolbar?SW_SHOW:SW_HIDE);

	if (m_bShowedToolbar && this->GetDlgItem(IDC_EDIT_DESCRIPTION)->GetSafeHwnd()!=NULL)
	{
		y += 22;
		this->GetDlgItem(IDC_EDIT_DESCRIPTION)->MoveWindow(POS_LINESTATE_LEFT,y,cx-(POS_LINESTATE_LEFT)-3,18);
	}
	if (m_bShowedToolbar)
		y += 20;
	int x = m_bShowedToolbar?(POS_LINESTATE_LEFT+2):7;
	m_btnMyCenter.MovePoint(x,y);
	x += m_btnMyCenter.GetImgWidth();
	m_btnFileMgr.MovePoint(x,y);
	if (!theApp.GetDisableUserCloudDrive() && !theApp.IsLogonVisitor())
	{
		x += m_btnMyCenter.GetImgWidth();
		m_btnMyShare.MovePoint(x,y);
	}
	x += m_btnMyCenter.GetImgWidth();
	m_btnMainFunc.MovePoint(x,y);
	x += m_btnMyCenter.GetImgWidth()+10;
	m_btnGoBack.MovePoint(x,y+2);
	x += m_btnMyCenter.GetImgWidth();
	m_btnGoForward.MovePoint(x,y+2);

	y += (m_btnMainFunc.GetImgHeight()+(m_bShowedToolbar?6:3));
	//y = POS_STA_FIRST_Y;
	if (m_editSearch.GetSafeHwnd()!=0)
		m_editSearch.MoveWindow(const_text_intever+const_leftr_tool_width, y, cx-const_text_intever*2-const_leftr_tool_width, const_number_height);
	//if (m_pPanelBrowserNavigate!=0 && m_pPanelBrowserNavigate->GetSafeHwnd()!=0)
	//{
	//	CRect rect;
	//	rect.left = 2;
	//	rect.right = rect.left + 18*2;
	//	rect.top = y + 2;
	//	rect.bottom = rect.top + 18;
	//	this->ClientToScreen(&rect);
	//	m_pPanelBrowserNavigate->MoveWindow(&rect);
	//}
	if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
	{
		const int nlen = (18+2)*4;
		CRect rect;
		rect.left = cx - nlen-4;
		rect.right = rect.left + nlen;
		rect.top = y + 2;
		rect.bottom = rect.top + 18;
		this->ClientToScreen(&rect);
		m_pPanelSearch->MoveWindow(&rect);
	}
	y += const_number_height;
	x = const_text_intever+const_leftr_tool_width+DEFAULT_RETURN_BTN_WIDTH+1;
	//if (m_btnSwitchFrame.GetSafeHwnd()!=NULL && m_btnSwitchFrame.IsWindowVisible())
	//	x += DEFAULT_RETURN_BTN_WIDTH;
#else
	int x = const_text_intever+const_leftr_tool_width+DEFAULT_RETURN_BTN_WIDTH+1;
	int y = 0;
#endif
	const int const_Tab_Width	= m_btnMyDepartment.GetImgWidth();
	const int const_Tab_intever = 0;
	if (theApp.IsLogonVisitor())
	{
		m_btnMySession.MovePoint(x, y);
	}else
	{
		m_btnMyDepartment.MovePoint(x, y);
		x += (const_Tab_Width+const_Tab_intever);
		//m_btnMyContacts.MovePoint(x, y);
		//x += (const_Tab_Width+const_Tab_intever);
		m_btnMySession.MovePoint(x, y);
		x += (const_Tab_Width+const_Tab_intever);
		m_btnMyEnterprise.MovePoint(x, y);
	}
//#ifndef USES_MIN_UI
	if (m_btnSwitchFrame.GetSafeHwnd()!=NULL)
	{
		m_rectSwitchFrame.left = const_text_intever+1;	// +1是 m_pDlgAppFrame有一个像素偏差
		m_rectSwitchFrame.right = m_rectSwitchFrame.left+DEFAULT_RETURN_BTN_WIDTH;
		m_rectSwitchFrame.top = y;
		m_rectSwitchFrame.bottom = m_rectSwitchFrame.top+m_btnMyDepartment.GetImgHeight();
		m_btnSwitchFrame.MoveWindow(&m_rectSwitchFrame);
		//m_btnSwitchFrame.MoveWindow(cx-16-const_text_intever-1,y,16,m_btnMyDepartment.GetImgHeight());	// -1是 m_pDlgAppFrame有一个像素偏差
	}
//#endif
	y += m_btnMyDepartment.GetImgHeight();
#ifndef USES_MIN_UI
	const int const_left_intever = 1;
	const int const_bottom_intever = 28;
#else
	const int const_left_intever = 0;
	const int const_bottom_intever = 0;
#endif
	CRect rectDlg(const_left_intever+const_leftr_tool_width, y, cx-const_left_intever, cy-const_bottom_intever);
	if (m_pDlgMyGroup != NULL && m_pDlgMyGroup->GetSafeHwnd())
	{
		m_pDlgMyGroup->MoveWindow(&rectDlg);
	}
	if (m_pDlgMyContacts != NULL && m_pDlgMyContacts->GetSafeHwnd())
	{
		m_pDlgMyContacts->MoveWindow(&rectDlg);
	}
	if (m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd())
	{
		m_pDlgMySession->MoveWindow(&rectDlg);
	}
	if (m_pDlgMyEnterprise != NULL && m_pDlgMyEnterprise->GetSafeHwnd())
	{
		m_pDlgMyEnterprise->MoveWindow(&rectDlg);
	}
#ifndef USES_MIN_UI
	if (m_treeSearch.GetSafeHwnd()!=0 && m_treeSearch.IsWindowVisible())
	{
		CRect searchRect;
		m_editSearch.GetWindowRect(&searchRect);
		this->ScreenToClient(&searchRect);
		searchRect.top = searchRect.bottom+1;
		searchRect.bottom = rectDlg.bottom;
		m_treeSearch.MoveWindow(&searchRect);
	}
#endif
	if (m_pDlgAppFrame!=NULL)
	{
		rectDlg.top -= m_btnMyDepartment.GetImgHeight();
		rectDlg.left += 1;
		rectDlg.right -= 1;
		m_pDlgAppFrame->MoveWindow(&rectDlg);
	}

#ifndef USES_MIN_UI
	//if (m_btnSwitchFrame.GetSafeHwnd())
	//	m_btnSwitchFrame.MoveWindow(const_text_intever,cy-60,50,50);
	x = 3;
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		if (pFuncButtonInfo->m_btn.GetSafeHwnd()!=NULL)
		{
			CRect rectFuncButton;
			rectFuncButton.left = 4 + i*(const_func_button_size+const_func_button_intever);
			rectFuncButton.right = rectFuncButton.left + const_func_button_size;
			rectFuncButton.bottom = cy-2;
			rectFuncButton.top = rectFuncButton.bottom-const_func_button_size;
			pFuncButtonInfo->m_btn.MoveWindow(&rectFuncButton);
			x = rectFuncButton.right+const_func_button_intever;
		}
	}
	if (m_btnNewApp.GetSafeHwnd()!=NULL)
	{
		m_rectNewApp.left = x;
		m_rectNewApp.right = m_rectNewApp.left+const_func_button_size;
		m_rectNewApp.bottom = cy-3;
		m_rectNewApp.top = m_rectNewApp.bottom-const_func_button_size;
		m_btnNewApp.MoveWindow(&m_rectNewApp);
	}
#endif
}

void CDlgMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
#ifndef USES_MIN_UI
	const int const_minbtn_width = m_btnMin.GetImgWidth();
	const int const_minbtn_height = m_btnMin.GetImgHeight();
	int btnx = 0;
	btnx = cx-m_btnClose.GetImgWidth()-2;
	m_btnClose.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;
	m_btnMax.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;
	m_btnMin.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;
	if (m_btnSkin.GetSafeHwnd())
		m_btnSkin.MoveWindow(btnx,2,const_minbtn_width,const_minbtn_height);
	if (m_btnSwitchToolbar.GetSafeHwnd()!=NULL)
	{
		btnx -= const_minbtn_width;
		m_rectSwitchToolbar.left = btnx;
		m_rectSwitchToolbar.right = m_rectSwitchToolbar.left+const_minbtn_width;
		m_rectSwitchToolbar.top = 2;
		m_rectSwitchToolbar.bottom = m_rectSwitchToolbar.top+const_minbtn_width;
		m_btnSwitchToolbar.MoveWindow(&m_rectSwitchToolbar);
	}
#endif
	MoveSize(cx,cy);

#ifndef USES_MIN_UI
	if (m_bOpenOk && !this->IsIconic() && cx>0 && cy>0)
	{
		char lpszBuffer[24];
		CRect rect;
		this->GetWindowRect(&rect);
		const int x = rect.left;
		const int y = rect.top;
		if (GetPrivateProfileInt(_T("default"),_T("main-x"),0,theApp.GetUserSettingIniFile())!=x)
		{
			sprintf(lpszBuffer,"%d",x);
			WritePrivateProfileString(_T("default"),_T("main-x"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
		if (GetPrivateProfileInt(_T("default"),_T("main-y"),0,theApp.GetUserSettingIniFile())!=y)
		{
			sprintf(lpszBuffer,"%d",y);
			WritePrivateProfileString(_T("default"),_T("main-y"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
		if (GetPrivateProfileInt(_T("default"),_T("main-w"),LEN_DLG_WIDTH,theApp.GetUserSettingIniFile())!=cx)
		{
			sprintf(lpszBuffer,"%d",cx);
			WritePrivateProfileString(_T("default"),_T("main-w"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
		if (GetPrivateProfileInt(_T("default"),_T("main-h"),LEN_DLG_HEIGHT,theApp.GetUserSettingIniFile())!=cy)
		{
			sprintf(lpszBuffer,"%d",cy);
			WritePrivateProfileString(_T("default"),_T("main-h"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
	}
	if (m_nShrinkageWin)
	{
		this->GetWindowRect(&m_rectWin);
	}
#endif
}

//void CDlgMainFrame::OnBnClickedButtonCalluser()
//{
//	UpdateData();
//	if (m_sCallNumber.IsEmpty())
//	{
//		m_comboNumbers.SetFocus();
//		AfxMessageBox(_T("请输入呼叫用户帐号！"));
//		return;
//	}
//	//m_comboNumbers.InsertString(0, m_sCallNumber);
//	theEBAppClient.CallUser(m_sCallNumber,"","");	// ??
//}

void CDlgMainFrame::RefreshLabelWindow(void)
{
	m_btnMyDepartment.SetChecked(m_btnMyDepartment.GetSafeHwnd()==m_pCurrentLabel);
	if (m_pOldCurrentLabel==m_btnMyDepartment.GetSafeHwnd() || m_pCurrentLabel==m_btnMyDepartment.GetSafeHwnd())
		theApp.InvalidateParentRect(&m_btnMyDepartment);
	m_btnMyContacts.SetChecked(m_btnMyContacts.GetSafeHwnd()==m_pCurrentLabel);
	if (m_pOldCurrentLabel==m_btnMyContacts.GetSafeHwnd() || m_pCurrentLabel==m_btnMyContacts.GetSafeHwnd())
		theApp.InvalidateParentRect(&m_btnMyContacts);
	m_btnMySession.SetChecked(m_btnMySession.GetSafeHwnd()==m_pCurrentLabel);
	if (m_pOldCurrentLabel==m_btnMySession.GetSafeHwnd() || m_pCurrentLabel==m_btnMySession.GetSafeHwnd())
		theApp.InvalidateParentRect(&m_btnMySession);
	m_btnMyEnterprise.SetChecked(m_btnMyEnterprise.GetSafeHwnd()==m_pCurrentLabel);
	if (m_pOldCurrentLabel==m_btnMyEnterprise.GetSafeHwnd() || m_pCurrentLabel==m_btnMyEnterprise.GetSafeHwnd())
		theApp.InvalidateParentRect(&m_btnMyEnterprise);
	m_pDlgMyGroup->ShowWindow(m_btnMyDepartment.GetChecked()?SW_SHOW:SW_HIDE);
	m_pDlgMyContacts->ShowWindow(m_btnMyContacts.GetChecked()?SW_SHOW:SW_HIDE);
	m_pDlgMySession->ShowWindow(m_btnMySession.GetChecked()?SW_SHOW:SW_HIDE);
	m_pDlgMyEnterprise->ShowWindow(m_btnMyEnterprise.GetChecked()?SW_SHOW:SW_HIDE);
}

void CDlgMainFrame::OnBnClickedButtonEnterprise()
{
	//if (m_pCurrentLabel != m_btnMyEnterprise.GetSafeHwnd())
	{
		m_pOldCurrentLabel = m_pCurrentLabel;
		m_pCurrentLabel = m_btnMyEnterprise.GetSafeHwnd();
		RefreshLabelWindow();
	}
}

void CDlgMainFrame::OnBnClickedButtonDepartment()
{
	//if (m_pCurrentLabel != m_btnMyDepartment.GetSafeHwnd())
	{
		m_pOldCurrentLabel = m_pCurrentLabel;
		m_pCurrentLabel = m_btnMyDepartment.GetSafeHwnd();
		RefreshLabelWindow();
	}
}

void CDlgMainFrame::OnBnClickedButtonContact()
{
	//if (m_pCurrentLabel != m_btnMyContacts.GetSafeHwnd())
	{
		m_pOldCurrentLabel = m_pCurrentLabel;
		m_pCurrentLabel = m_btnMyContacts.GetSafeHwnd();
		RefreshLabelWindow();
	}
}

void CDlgMainFrame::OnBnClickedButtonSession()
{
	m_btnMySession.SetWindowText(_T(""));
	m_pOldCurrentLabel = m_pCurrentLabel;
	m_pCurrentLabel = m_btnMySession.GetSafeHwnd();
	RefreshLabelWindow();
}

#ifndef USES_MIN_UI
void CDlgMainFrame::OnBnClickedButtonMax()
{
	if (m_nShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_nShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}

	int nScreenWidth = 0;
	int nScreenHeight = 0;
	theApp.GetScreenSize(nScreenWidth, nScreenHeight);

	//int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	////int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	//int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	//m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);

	static CRect theRestoreRect;
	CRect rect;
	GetWindowRect(&rect);
	if (rect.Width() == nScreenWidth)
	{
		if (theRestoreRect.Width()==0 || theRestoreRect.Height()==0)
		{
			theRestoreRect.left = (nScreenWidth-LEN_DLG_WIDTH)/2;
			theRestoreRect.right = theRestoreRect.left+LEN_DLG_WIDTH;
			//if (m_pDlgAppFrame!=NULL && !m_pDlgAppFrame->IsEmpty())
			//	theRestoreRect.right += DEFAULT_RETURN_BTN_WIDTH;
			theRestoreRect.top = (nScreenHeight-LEN_DLG_HEIGHT)/2;
			theRestoreRect.bottom = theRestoreRect.top+LEN_DLG_HEIGHT;
		}
		this->SetToolTipText(IDC_BUTTON_MAX2,_T("最大化"));
		m_btnMax.Load(IDB_PNG_MAX);
		MoveWindow(&theRestoreRect);
	}else
	{
		this->SetToolTipText(IDC_BUTTON_MAX2,_T("向下还原"));
		m_btnMax.Load(IDB_PNG_RESTORE);
		theRestoreRect = rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = rect.left + nScreenWidth;
		rect.bottom = rect.top + nScreenHeight;
		MoveWindow(&rect);
	}
	if (m_pDlgAppFrame!=NULL && m_bAppFrameShowed && !m_treeSearch.IsWindowVisible())//m_pDlgAppFrame->IsWindowVisible())
	{
		// 解决m_pDlgAppFrame 背景刷新问题（按钮背景）
		SetTimer(TIMERID_REFRESH_APPFRAME,1,NULL);

		//this->Invalidate();
		//m_pDlgAppFrame->ShowWindow(SW_HIDE);
		//m_pDlgAppFrame->ShowWindow(SW_SHOW);
		//this->PostMessage(POP_WM_WINDOW_SIZE);
		//m_pDlgAppFrame->PostMessage(POP_WM_WINDOW_SIZE);
		//m_pDlgAppFrame->RefreshWindow();
	}
}

void CDlgMainFrame::OnBnClickedButtonMin()
{
	if (m_nShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_nShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
	}
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CDlgMainFrame::OnBnClickedButtonClose()
{
	if (m_nShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_nShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
	}
	this->ShowWindow(SW_HIDE);
	if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
		m_pPanelSearch->ShowWindow(SW_HIDE);
	this->SetFocus();	// 解决按钮背景刷新问题；
}
#endif
void CDlgMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMERID_ONE_SECOND:
		{
			CEBCMsgInfo::pointer pEbcMsgInfo;
			if (m_pEbcMsgList.front(pEbcMsgInfo))
			{
				if (pEbcMsgInfo->GetPlayMsgSound())
				{
					CString sSoundFile;
					sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
					sndPlaySound(sSoundFile, SND_ASYNC);
				}
				switch (pEbcMsgInfo->GetMsgType())
				{
				case CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW:
					{
						AFX_SWITCH_INSTANCED();
						CWnd * pParent = pEbcMsgInfo->m_pParent;
						pEbcMsgInfo->m_pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
						if (pEbcMsgInfo->m_bSetForegroundWindow)
							pEbcMsgInfo->m_pFuncWindow->SetForegroundWindow();
					}break;
				default:
					break;
				}
			}
		}break;
	case TIMERID_REFRESH_APPFRAME:
		{
			KillTimer(nIDEvent);
			if (m_pDlgAppFrame!=NULL)
			{
				m_pDlgAppFrame->ShowWindow(SW_HIDE);
				m_pDlgAppFrame->ShowWindow(SW_SHOW);
				//m_pDlgAppFrame->PostMessage(POP_WM_WINDOW_SIZE);	// xx
			}
			//PostMessage(POP_WM_WINDOW_SIZE);	// xx
		}break;
	case TIMERID_CHECK_MOUSE_POS:
		{
			CPoint pos;
			GetCursorPos(&pos);
			this->ScreenToClient(&pos);
#ifndef USES_MIN_UI
			if (m_btnSwitchToolbar.GetSafeHwnd()!=NULL)
			{
				static bool thePtInRect = false;
				const bool bPtInRect = m_rectSwitchToolbar.PtInRect(pos)?true:false;
				if (thePtInRect!=bPtInRect)
				{
					thePtInRect = bPtInRect;
					if (bPtInRect)
					{
						if (m_bShowedToolbar)
							m_btnSwitchToolbar.SetDrawTrianglePic(1,RGB(61,61,61),-1,-1,-1,12,6);
						else
							m_btnSwitchToolbar.SetDrawTrianglePic(2,RGB(61,61,61),-1,-1,-1,10,5);
					}else
					{
						if (m_bShowedToolbar)
							m_btnSwitchToolbar.SetDrawTrianglePic(1,RGB(250,250,250),-1,-1,-1,12,6);
						else
							m_btnSwitchToolbar.SetDrawTrianglePic(2,RGB(250,250,250),-1,-1,-1,10,5);
					}
					m_btnSwitchToolbar.Invalidate();
				}
			}
#endif
			if (m_btnSwitchFrame.GetSafeHwnd()!=NULL)
			{
				static bool thePtInRect = false;
				const bool bPtInRect = m_rectSwitchFrame.PtInRect(pos)?true:false;
				if (thePtInRect!=bPtInRect)
				{
					thePtInRect = bPtInRect;
					if (bPtInRect)
					{
						m_btnSwitchFrame.SetDrawTrianglePic(3,RGB(61,61,61),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
						//m_btnSwitchFrame.SetDrawTrianglePic(3,RGB(237,28,36),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
					}else
					{
						m_btnSwitchFrame.SetDrawTrianglePic(3,RGB(250,250,250),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
					}
					m_btnSwitchFrame.Invalidate();
				}
			}
#ifndef USES_MIN_UI
			if (m_btnNewApp.GetSafeHwnd()!=NULL)
			{
				static bool thePtInRect = false;
				const bool bPtInRect = m_rectNewApp.PtInRect(pos)?true:false;
				if (thePtInRect!=bPtInRect)
				{
					thePtInRect = bPtInRect;
					if (bPtInRect)
					{
						m_btnNewApp.SetDrawNewPic(true,RGB(61,61,61),-1,-1,-1,2,11);
					}else
					{
						m_btnNewApp.SetDrawNewPic(true,RGB(250,250,250),-1,-1,-1,2,11);
					}
					m_btnNewApp.Invalidate();
				}
			}
#endif
		}break;
#ifndef USES_MIN_UI
	case TIMERID_SHRINKAGE_WIN:
		{
			CPoint pos;
			GetCursorPos(&pos);
			CRect rectWin;
			m_pDlgShrinkageBar->GetWindowRect(&rectWin);
			if (pos.y>0 && !m_rectWin.PtInRect(pos))
			{
				// up
				if (m_pDlgShrinkageBar->IsWindowVisible())
					break;
				m_pDlgShrinkageBar->ShowWindow(SW_SHOW);
				this->ShowWindow(SW_HIDE);
				if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
					m_pPanelSearch->ShowWindow(SW_HIDE);
			}else if (rectWin.PtInRect(pos))
			{
				// down
				if (!m_pDlgShrinkageBar->IsWindowVisible())
					break;
				m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
				this->ShowWindow(SW_SHOWNORMAL);
				if (this->m_bAppFrameShowed && m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
					m_pPanelSearch->ShowWindow(SW_SHOW);
			}
		}break;
	case TIMERID_NEW_VERSION:
		{
			KillTimer(nIDEvent);
			const CString sNewVersion = theApp.EBC_GetProfileString(_T("new_version"),_T("version"));
			const CString sNewVersionFile = theApp.EBC_GetProfileString(_T("new_version"),_T("file"));
			if (!sNewVersion.IsEmpty())
			{
#ifdef USES_EBCOM_TEST
				const CEBString sVersion = theEBSetting->Version.GetBSTR();
#else
				const CEBString sVersion = theSetting.GetVersion();
#endif
				if (::PathFileExists(sNewVersionFile) && sNewVersion != sVersion.c_str())
				{
					static bool theRunOnce = false;
					if (!theRunOnce)
					{
						theRunOnce = true;
						CString sText;
						sText.Format(_T("[%s]，是否立即更新？"),sNewVersion);
						CString sCaption;
#ifdef USES_EBCOM_TEST
						const CEBString sSettingEnterprise = theEBSetting->Enterprise.GetBSTR();
#else
						const CEBString sSettingEnterprise = theSetting.GetEnterprise();
#endif
						if (theApp.GetLicenseUser() && !theApp.GetProductName().IsEmpty())
						{
							sCaption.Format(_T("%s检测到新版本："),theApp.GetProductName());
						}else if (sSettingEnterprise.empty())
							sCaption.Format(_T("恩布互联检测到新版本："));
						else
						{
							if (!theApp.GetLicenseUser() && sSettingEnterprise.find("恩布")==std::string::npos)
							{
								sCaption.Format(_T("恩布互联检测到新版本："));
							}else
								sCaption.Format(_T("%s检测到新版本："),sSettingEnterprise.c_str());
						}
						if (CDlgMessageBox::EbDoModal(this,sCaption,sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
							//if (MessageBox(sText, _T("恩布新版本"), MB_YESNO|MB_ICONQUESTION) == IDYES)
						{
							// 执行自动更新过程
							TCHAR lpszModuleFileName[MAX_PATH];
							GetModuleFileName( NULL, lpszModuleFileName, MAX_PATH);
							CString sUpdateExe = theApp.GetAppPath()+_T("\\update\\ebupdate2.exe");
							CString sParameter;
							sParameter.Format(_T("\"%s\" \"%s\""), sNewVersionFile, lpszModuleFileName);
							ShellExecute(NULL,  "open", sUpdateExe, sParameter, theApp.GetAppPath(), SW_SHOW);

							// 退出应用程序
							theWantExitApp = true;
							PostMessage(WM_CLOSE, 0, 0);
						}
						theRunOnce = false;
					}
				}
			}
		}break;
	case TIMERID_CHECK_UPDATE:
		{
			const CString sEBCSetting = theApp.GetAppDataPath()+_T("\\ebc.ini");
			CString sClientVersion;
			if (::PathFileExists(sEBCSetting))
			{
				TCHAR lpszBuffer[256];
				memset(lpszBuffer,0,sizeof(lpszBuffer));
				::GetPrivateProfileString("system","client_version","",lpszBuffer,sizeof(lpszBuffer),sEBCSetting);
				sClientVersion = lpszBuffer;
			}

			if (sClientVersion.IsEmpty())
			{
#ifdef USES_EBCOM_TEST
				theEBClientCore->EB_CheckVersion(theEBSetting->Version);
#else
				theEBAppClient.EB_CheckVersion(theSetting.GetVersion().c_str());
#endif
			}else
			{
#ifdef USES_EBCOM_TEST
				theEBClientCore->EB_CheckVersion((LPCTSTR)sClientVersion);
#else
				theEBAppClient.EB_CheckVersion(sClientVersion);
#endif
			}
			if (!theUpdateResetTimer)
			{
				theUpdateResetTimer = true;
				KillTimer(TIMERID_CHECK_UPDATE);
				SetTimer(TIMERID_CHECK_UPDATE,60*60*1000,NULL);	// 一小时检查一次
			}
		}break;
#endif
	case TIMERID_AUTO_OPEN_SUBID:
		{
			// 打开启动FUNC
			static int theIndex = 0;
			if ((theIndex++) == 30)	// 最多检查30次，一次一秒
			{
				theIndex = 0;
				KillTimer(TIMERID_AUTO_OPEN_SUBID);
			}
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo = theEBClientCore->EB_GetSubscribeFuncInfo(theApp.GetAutoOpenSubId());
			if (pSubscribeFuncInfo!=NULL)
			{
				if (!theApp.GetHideMainFrame())
				{
					static int theWaitSecond = 0;
					if ((theWaitSecond++)<3)
						break;
					theWaitSecond = 0;
				}
				KillTimer(TIMERID_AUTO_OPEN_SUBID);
				theApp.OpenAutoSubscribeFunc(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
			}
#else
			EB_SubscribeFuncInfo pSubscribeFuncInfo;
			if (theEBAppClient.EB_GetSubscribeFuncInfo(theApp.GetAutoOpenSubId(),&pSubscribeFuncInfo))
			{
				if (!theApp.GetHideMainFrame())
				{
					static int theWaitSecond = 0;
					if ((theWaitSecond++)<3)
						break;
					theWaitSecond = 0;
				}

				KillTimer(TIMERID_AUTO_OPEN_SUBID);
				theApp.OpenAutoSubscribeFunc(pSubscribeFuncInfo);
			}
#endif
		}break;
	case TIMERID_CHECK_MF_SUBFUNCINFO:
		{
			// 应用功能
			static int theIndex = 0;
			if ((theIndex++) == 15)	// 最多检查15次，一次2秒
			{
				theIndex = 0;
				KillTimer(TIMERID_CHECK_MF_SUBFUNCINFO);
			}
			std::vector<EB_SubscribeFuncInfo> pMainSubscribeFuncList;	// for mainframe
#ifndef USES_MIN_UI
			// 获取主面板底部导航按钮
#ifdef USES_EBCOM_TEST
			_variant_t pSubscribeFuncList = theEBClientCore->EB_GetSubscribeFuncList(EB_FUNC_LOCATION_MAINFRAME_BTN2);
			if (pSubscribeFuncList.vt!=VT_EMPTY && pSubscribeFuncList.parray != NULL)
			{
				CComSafeArray<VARIANT> m_sa(pSubscribeFuncList.parray);
				for (ULONG i=0;i<m_sa.GetCount();i++)
				{
					CComVariant var = m_sa.GetAt(i);
					if (var.vt != VT_DISPATCH)
						continue;
					CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo;
					var.pdispVal->QueryInterface(__uuidof(IEB_SubscribeFuncInfo),(void**)&pSubscribeFuncInfo);
					if (pSubscribeFuncInfo == NULL) continue;
					pMainSubscribeFuncList.push_back(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
				}
			}
#else
			theEBAppClient.EB_GetSubscribeFuncList(EB_FUNC_LOCATION_MAINFRAME_BTN2,pMainSubscribeFuncList);
#endif
			if (!pMainSubscribeFuncList.empty())
			{
				for (size_t i=0;i<pMainSubscribeFuncList.size();i++)
				{
					const EB_SubscribeFuncInfo & pSubscribeFuncInfo = pMainSubscribeFuncList[i];
					AddSubscribeFuncInfo(pSubscribeFuncInfo);
				}
				pMainSubscribeFuncList.clear();
			}
#endif
			// 获取应用工作区导航按钮
#ifdef USES_EBCOM_TEST
			_variant_t pSubscribeFuncList = theEBClientCore->EB_GetSubscribeFuncList(EB_FUNC_LOCATION_APPFRAME_BTN);
			if (pSubscribeFuncList.vt!=VT_EMPTY && pSubscribeFuncList.parray != NULL)
			{
				CComSafeArray<VARIANT> m_sa(pSubscribeFuncList.parray);
				for (ULONG i=0;i<m_sa.GetCount();i++)
				{
					CComVariant var = m_sa.GetAt(i);
					if (var.vt != VT_DISPATCH)
						continue;
					CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo;
					var.pdispVal->QueryInterface(__uuidof(IEB_SubscribeFuncInfo),(void**)&pSubscribeFuncInfo);
					if (pSubscribeFuncInfo == NULL) continue;
					pMainSubscribeFuncList.push_back(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
				}
			}
#else
			theEBAppClient.EB_GetSubscribeFuncList(EB_FUNC_LOCATION_APPFRAME_BTN,pMainSubscribeFuncList);
#endif
			if (!pMainSubscribeFuncList.empty())
			{
				for (size_t i=0;i<pMainSubscribeFuncList.size();i++)
				{
					const EB_SubscribeFuncInfo & pSubscribeFuncInfo = pMainSubscribeFuncList[i];
					AddAppFrameFuncInfo(pSubscribeFuncInfo);
				}
				pMainSubscribeFuncList.clear();
			}
		}break;
	case TIMERID_CHECK_CLOSE_DIALOG:
		{
			// 定期检查超过５分钟没用会话
			{
				BoostReadLock rdlock(theApp.m_pCallList.mutex());
				//boost::mutex::scoped_lock lockCallList(theApp.m_pCallList.mutex());
				CLockMap<eb::bigint, CEBCCallInfo::pointer>::const_iterator pIterCallList = theApp.m_pCallList.begin();
				for (; pIterCallList!=theApp.m_pCallList.end(); pIterCallList++)
				{
					const CEBCCallInfo::pointer pEbCallInfo = pIterCallList->second;
					const eb::bigint sCallid = pEbCallInfo->m_pCallInfo.GetCallId();
					if ((time(0)-pEbCallInfo->m_tLastTime)>5*60 && m_pDlgFrameList!=NULL && !m_pDlgFrameList->ExistCallIdWnd(sCallid))
					{
						theApp.m_pCallList.erase(pIterCallList);
						// 这里主要用于处理自动响应，没有打开聊天界面会话
						// *其实在OnMessageCallConnected已经有处理；
#ifdef USES_EBCOM_TEST
						theEBClientCore->EB_CallExit(pEbCallInfo->m_pCallInfo.GetCallId());
#else
						theEBAppClient.EB_CallExit(pEbCallInfo->m_pCallInfo.GetCallId());
#endif
						break;
					}
				}
			}

			do
			{
				CDlgDialog::pointer pDialog;
				if (theApp.m_pCloseDialog.front(pDialog))
				{
					try
					{
						pDialog->DestroyWindow();
					}catch(std::exception&)
					{
					}catch(...)
					{}
				}
			}while (theApp.m_pCloseDialog.size()>2);

			do
			{
				CWnd * pWnd = NULL;
				if (theApp.m_pCloseWnd.front(pWnd))
				{
					try
					{
						pWnd->DestroyWindow();
					}catch(std::exception&)
					{
					}catch(...)
					{}
				}
			}while (theApp.m_pCloseWnd.size()>2);

		}break;
	case TIMERID_LOGOUT:
		{
			KillTimer(nIDEvent);
			OnLogout();
		}break;
//	case TIMERID_RELOGIN:
//		{
//			KillTimer(TIMERID_RELOGIN);
//#ifdef USES_EBCOM_TEST
//			if (theEBClientCore.get()!=NULL)
//				theEBClientCore->EB_ReLogon();
//#else
//			theEBAppClient.EB_ReLogon();
//#endif
//			//const time_t tNow = time(0);
//			//FILE * file = fopen("c:\\TIMERID_RELOGIN.txt","a+t");
//			//if (file!=NULL)
//			//{
//			//	char lpszBuffer[100];
//			//	sprintf(lpszBuffer,"time=%lld,%d\n",(eb::bigint)tNow,ret);
//			//	fwrite(lpszBuffer,1,strlen(lpszBuffer),file);
//			//	fclose(file);
//			//}
////			static time_t theReloginTime = 0;
////			if (theReloginTime == 0 || (time(0)-theReloginTime)>30)	// 第一次，或超过30秒才处理一次
////			{
////				theReloginTime = time(0);
////#ifdef USES_EBCOM_TEST
////				if (theEBClientCore.get()!=NULL)
////					theEBClientCore->EB_ReLogon();
////#else
////				theEBAppClient.EB_ReLogon();
////#endif
////			}
//		}break;
//	case TIMERID_LOADINFO:
//		{
//			KillTimer(nIDEvent);
//#ifdef USES_EBCOM_TEST
//			theEBClientCore->EB_LoadInfo();
//#else
//			theEBAppClient.EB_LoadInfo();
//#endif
//		}break;
	case TIMERID_SHOW_EMOTIPN:
		{
			KillTimer(nIDEvent);
			if (theApp.m_pDlgEmotionSelect==NULL)
			{
				theApp.ShowImageWindow(false,NULL,NULL);
			}
		}break;
	//case TIMERID_HIDE_MAIN_FRAME:
	//	{
	//		this->ShowWindow(SW_HIDE);
	//		KillTimer(TIMERID_HIDE_MAIN_FRAME);
	//	}break;
	default:
		break;
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

#ifndef USES_MIN_UI
bool CDlgMainFrame::AddSubscribeFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo)
{
	if (pSubscribeFuncInfo.m_nIconResId==0 || !PathFileExists(pSubscribeFuncInfo.m_sResFile.c_str()))
	{
		if (theApp.m_sDefaultAppImage.empty())
			return false;
	}
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		if (pFuncButtonInfo->GetFuncInfo().m_nSubscribeId==pSubscribeFuncInfo.m_nSubscribeId)
			return true;
	}
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CRect rectFuncButton;
	rectFuncButton.left = 4 + m_pMainFuncButtonList.size()*(const_func_button_size+const_func_button_intever);
	rectFuncButton.right = rectFuncButton.left + const_func_button_size;
	rectFuncButton.bottom = rectClient.bottom-2;
	rectFuncButton.top = rectFuncButton.bottom-const_func_button_size;
	CEBFuncButton::pointer pFuncButton = CEBFuncButton::create();
	const UINT nID = EB_COMMAND_MAINFRAME_FUNC+m_pMainFuncButtonList.size();
	if (pFuncButton->Create(this,rectFuncButton,nID,pSubscribeFuncInfo,theApp.m_sDefaultAppImage))
	{
		m_pMainFuncButtonList.push_back(pFuncButton);

		if (m_btnNewApp.GetSafeHwnd()!=NULL)
		{
			const int x = rectFuncButton.right+const_func_button_intever;
			m_rectNewApp.left = x;
			m_rectNewApp.right = m_rectNewApp.left+const_func_button_size;
			m_rectNewApp.bottom = rectClient.Height()-3;
			m_rectNewApp.top = m_rectNewApp.bottom-const_func_button_size;
			m_btnNewApp.MoveWindow(&m_rectNewApp);
		}

		return true;
	}
	return false;
}
#endif
bool CDlgMainFrame::AddAppFrameFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo)
{
	NewAppFrame();
	return m_pDlgAppFrame->AddSubscribeFuncInfo(pSubscribeFuncInfo);
}

#ifndef USES_MIN_UI
void CDlgMainFrame::DeleteItem(HTREEITEM hItem)
{
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(hItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType!=CTreeItemInfo::ITEM_TYPE_URL) return;

	std::string sUrl(pTreeItemInfo->m_sAccount);
	m_treeSearch.DeleteItem(hItem);
	if (m_pUrlItem.remove(sUrl))
	{
		CTreeItemInfo::pointer pUrlGroupInfo;
		if (m_pUrlItem.find(theUrlGroupName,pUrlGroupInfo))
		{
			if (m_pUrlItem.size()<=1)
				m_treeSearch.DeleteItem(pUrlGroupInfo->m_hItem);
			else
			{
				const int nGroupResultSize = m_pUrlItem.size()-1;
				CString sSearchName;
				sSearchName.Format(_T("%s%d"),theUrlGroupResultName,nGroupResultSize);
				m_treeSearch.SetItemText(pUrlGroupInfo->m_hItem,sSearchName);
			}
		}
	}
	CSqliteCdbc::escape_string_in(sUrl);
	sUrl = libEbc::ACP2UTF8(sUrl.c_str());

	CString sSql;
	sSql.Format(_T("DELETE FROM url_record_t WHERE url='%s'"),sUrl.c_str());
	theApp.m_pBoUsers->execute(sSql);
}
void CDlgMainFrame::CallItem(HTREEITEM hItem,bool bSelChange)
{
	if (!bSelChange)
	{
		HTREEITEM hChildItem = m_treeSearch.GetChildItem(hItem);
		if (hChildItem!=NULL)
			hItem = hChildItem;
	}
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(hItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR) return;
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_URL)
	{
		this->AddAppUrl(pTreeItemInfo->m_sAccount);
	}else if (pTreeItemInfo->m_nUserId>0)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallUserId(pTreeItemInfo->m_nUserId,0);
#else
		theEBAppClient.EB_CallUserId(pTreeItemInfo->m_nUserId,0);
#endif
	}else if (pTreeItemInfo->m_sMemberCode>0)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#else
		theEBAppClient.EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#endif
	}else
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallAccount(pTreeItemInfo->m_sAccount.c_str(),0);
#else
		theEBAppClient.EB_CallAccount(pTreeItemInfo->m_sAccount.c_str(),0);
#endif
	}
}
#endif
//
//LRESULT CDlgMainFrame::OnTreeItemSelChange(WPARAM wp, LPARAM lp)
//{
//	HTREEITEM item = (HTREEITEM)wp;
//	VividTree* pOwner = (VividTree*)lp;
//	if (item!=NULL && pOwner == &m_treeSearch)
//	{
//		CallItem(item,true);
//	}
//	return 0;
//}
LRESULT CDlgMainFrame::OnTreeItemDoubleClick(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	VividTree* pOwner = (VividTree*)lp;
#ifndef USES_MIN_UI
	if (pOwner == &m_treeSearch)
	{
		m_editSearch.SetFocus();
		CallItem(item);
	}
#endif
	return 0;
}

LRESULT CDlgMainFrame::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	HTREEITEM hItem = (HTREEITEM)wp;
	VividTree * pOwner = (VividTree*)lp;
#ifndef USES_MIN_UI
	if (pOwner == &m_treeSearch)
	{
		if (hItem == NULL)
		{
			if (m_pViewContactInfo!=0 && m_pViewContactInfo->IsWindowVisible())
			{
				m_pViewContactInfo->HideReset();
			}
			m_btnSearchTrackCall.ShowWindow(SW_HIDE);
			m_btnSearchTrackDel.ShowWindow(SW_HIDE);
		}else
		{
			const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(hItem);
			if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
			{
				if (m_pViewContactInfo!=0 && m_pViewContactInfo->IsWindowVisible())
				{
					m_pViewContactInfo->HideReset();
				}
				m_btnSearchTrackCall.ShowWindow(SW_HIDE);
				m_btnSearchTrackDel.ShowWindow(SW_HIDE);
				return 0;
			}
			if (m_pViewContactInfo == NULL)
			{
				m_pViewContactInfo = new CDlgViewContactInfo(this);
				m_pViewContactInfo->Create(CDlgViewContactInfo::IDD,this);
			}
			CPoint pos;
			GetCursorPos(&pos);

			const int const_dlg_width = 380;
			const int const_dlg_height = 180;
			CRect rect;
			this->GetWindowRect(&rect);
			CRect rectViewContactInfo;
			rectViewContactInfo.top = pos.y-80;
			rectViewContactInfo.bottom = rectViewContactInfo.top+const_dlg_height;
			if (rect.left-const_dlg_width > 0)
			{
				rectViewContactInfo.right = rect.left-5;
				rectViewContactInfo.left = rectViewContactInfo.right-const_dlg_width;
			}else
			{
				int nScreenWidth = 0;
				int nScreenHeight = 0;
				theApp.GetScreenSize(nScreenWidth, nScreenHeight);
				if (rect.right+const_dlg_width<nScreenWidth)
					rectViewContactInfo.left = min((nScreenWidth-const_dlg_width),(rect.right+5));
				else
					rectViewContactInfo.left = pos.x+30;
				rectViewContactInfo.right = rectViewContactInfo.left+const_dlg_width;
			}
			m_pViewContactInfo->MoveWindow(&rectViewContactInfo);
			m_pViewContactInfo->SetCircle();

			if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
			{
#ifdef USES_EBCOM_TEST
				CComPtr<IEB_MemberInfo> pMemberInfo;
				CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode);
				if (pGroupInfo == NULL) return 1;
				pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
				if (pMemberInfo == NULL) return 1;
				m_pViewContactInfo->SetEmployeeInfo(pMemberInfo,pGroupInfo);
#else
				EB_MemberInfo pMemberInfo;
				EB_GroupInfo pGroupInfo;
				if (!theEBAppClient.EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode,&pGroupInfo)) return 1;
				if (!theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)) return 1;
				m_pViewContactInfo->SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
#endif
			}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
			{
#ifdef USES_EBCOM_TEST
				CComPtr<IEB_ContactInfo> pContactInfo = theEBClientCore->EB_GetContactInfo1(pTreeItemInfo->m_sId);
				if (pContactInfo==NULL)
					return 1;
				m_pViewContactInfo->SetContactInfo(pContactInfo);
#else
				EB_ContactInfo pContactInfo;
				if (!theEBAppClient.EB_GetContactInfo1(pTreeItemInfo->m_sId,&pContactInfo))
					return 1;
				m_pViewContactInfo->SetContactInfo(&pContactInfo);
#endif
			}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_URL)
			{
				if (m_pViewContactInfo->IsWindowVisible())
					m_pViewContactInfo->HideReset();

				const int const_btn_width = m_btnSearchTrackDel.GetImgWidth();
				m_treeSearch.GetItemRect(hItem, &rect, TRUE);
				const int nTop = rect.top;
				m_treeSearch.GetClientRect(&rect);
				const int nRight = rect.right;
				m_btnSearchTrackDel.MovePoint(nRight-const_btn_width, nTop);
				m_btnSearchTrackDel.ShowWindow(SW_SHOW);
				m_btnSearchTrackDel.Invalidate();
				return 0;
			}else if (m_pViewContactInfo->IsWindowVisible())
			{
				m_pViewContactInfo->HideReset();
				return 0;
			}
			m_pViewContactInfo->ShowWindow(SW_SHOW);
			m_treeSearch.SetFocus();

			//const int const_btn_width = m_btnSearchTrackCall.GetImgWidth();
			//m_treeSearch.GetItemRect(hItem, &rect, TRUE);
			//const int nTop = rect.top;
			//m_treeSearch.GetClientRect(&rect);
			//const int nRight = rect.right;
			//m_btnSearchTrackCall.MovePoint(nRight-const_btn_width, nTop);
			//m_btnSearchTrackCall.ShowWindow(SW_SHOW);
			//m_btnSearchTrackCall.Invalidate();
		}
	}
#endif

	return 0;
}
BOOL CDlgMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	
#ifndef USES_MIN_UI
	if (m_bShowedToolbar && pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_editDescription.GetSafeHwnd())
	{
		m_editDescription.SetReadOnly(FALSE);
		m_editDescription.ShowWindow(SW_SHOW);
		// 以上没用
		//if (m_editDescription.GetSafeHwnd() && this->GetFocus() != &m_editDescription)
		//{
		//	m_editDescription.SetFocus();
		//	m_editDescription.SetSel(0,-1);
		//}
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_editSearch.GetSafeHwnd())
	{
		m_bCanSearch = true;
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnSearchTrackDel.GetSafeHwnd())
	{
		DeleteItem(m_treeSearch.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnSearchTrackCall.GetSafeHwnd())
	{
		m_editSearch.SetFocus();
		CallItem(m_treeSearch.GetTrackItem());
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_treeSearch.GetSafeHwnd())
	{
		CallItem(m_treeSearch.GetSelectedItem());
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam==VK_UP && pMsg->hwnd == m_treeSearch.GetSafeHwnd())
	{
		// 已经移到最上面
		if (m_treeSearch.GetCount()>0 && m_treeSearch.GetSelectedItem()!=NULL &&
			m_treeSearch.GetSelectedItem()==m_treeSearch.GetFirstVisibleItem() && m_treeSearch.GetParentItem(m_treeSearch.GetSelectedItem())==NULL)
		{
			m_editSearch.SetFocus();
		}
	}else if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_editSearch.GetSafeHwnd())
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			//if (m_treeSearch.IsWindowVisible() && m_treeSearch.GetCount()>0)
			{
				m_editSearch.SetWindowText(_T(""));
				m_editSearch.SetFocus();
			}
		}else if (pMsg->wParam == VK_DOWN)
		{
			if (m_treeSearch.IsWindowVisible() && m_treeSearch.GetCount()>0)
			{
				m_treeSearch.SetFocus();
				if (m_treeSearch.GetSelectedItem()!=0 && m_treeSearch.GetParentItem(m_treeSearch.GetSelectedItem())==0)
				{
					m_treeSearch.Expand(m_treeSearch.GetSelectedItem(),TVE_EXPAND);
				}

			}
		}else if (pMsg->wParam == VK_RETURN)
		{
			// 回车请求会话
			//if (m_treeSearch.GetCount()>0)
			//{
			//	if (m_treeSearch.GetSelectedItem()==NULL)
			//		CallItem(m_treeSearch.GetFirstVisibleItem());
			//	else
			//		CallItem(m_treeSearch.GetSelectedItem());
			//}else
			{
				CString sSearchString;
				m_editSearch.GetWindowText(sSearchString);
				sSearchString.Trim();
				if (!sSearchString.IsEmpty())
				{
#ifdef USES_APPFRAME
					CString sSearchStringTemp(sSearchString);
					sSearchStringTemp.MakeLower();
					const bool bIsEmail = sSearchStringTemp.Find(_T("@"))>=0?true:false;
					if (sSearchStringTemp.Find(_T("http://"))==0 ||
						sSearchStringTemp.Find(_T("https://"))==0 ||
						sSearchStringTemp.Find(_T("www."))==0 ||
						sSearchStringTemp.Find(_T("/"))>0 ||
						sSearchStringTemp.Find(_T("="))>0 ||
						sSearchStringTemp==_T("about:blank") ||
						(!bIsEmail && (sSearchStringTemp.Find(_T(".com"))>0 || sSearchStringTemp.Find(_T(".cn"))>0))
						)
					{
						AddAppUrl((LPCTSTR)sSearchString);
					}else
#endif
					{
#ifdef USES_EBCOM_TEST
						theEBClientCore->EB_CallAccount((LPCTSTR)sSearchString,0);
#else
						theEBAppClient.EB_CallAccount(sSearchString,0);
#endif
					}
				}
			}
		}else
		{
			m_bCanSearch = true;
		}
	}else if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_editDescription.GetSafeHwnd())
	{
		if (pMsg->wParam == VK_RETURN)
		{
			// 回车，保存
			OnEnKillfocusEditDescription();
//#ifdef USES_EBCOM_TEST
//			EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
//			const CEBString sOutDesc = theEBClientCore->EB_Description.GetBSTR();
//#else
//			EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
//			std::string sOutDesc = theEBAppClient.EB_GetDescription();
//#endif
//			CString sDesc;
//			m_editDescription.GetWindowText(sDesc);
//			if (sOutDesc != (LPCTSTR)sDesc)
//			{
//#ifdef USES_EBCOM_TEST
//				theEBClientCore->EB_Description = (LPCTSTR)sDesc;
//#else
//				theEBAppClient.EB_SetDescription(sDesc);
//#endif
//			}
//			RefreshEditDescription();
		}else if (pMsg->wParam == VK_ESCAPE)
		{
			// 退出，取消
			CancelSaveDescription();
		}
	}
#endif
	return CDialog::PreTranslateMessage(pMsg);
}
#ifndef USES_MIN_UI
LRESULT CDlgMainFrame::OnIconNotification(WPARAM wParam, LPARAM lParam)
{
	if(wParam != m_trayIconData.uID)    // 不是自己的图标发来的消息，退出
		return FALSE;
	switch(lParam)  // 过滤消息
	{
	case WM_RBUTTONDOWN:
		{
			if (m_menuTray.GetSafeHmenu()==NULL)
			{
				m_menuTray.CreatePopupMenu();
				if (!theApp.IsLogonVisitor())
				{
					m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_STATE_ONLINE,_T("在线"));
					m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_STATE_AWAY,_T("离开"));
					m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_STATE_BUSY,_T("忙碌"));
					m_menuTray.AppendMenu(MF_SEPARATOR);
					if (!theApp.GetDisableUserCloudDrive())
					{
						m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MY_SHARE,_T("我的云盘"));
						m_menuTray.AppendMenu(MF_SEPARATOR);
					}
					m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_OPEN_MAIN,_T("打开主面板"));
					m_menuTray.AppendMenu(MF_SEPARATOR);
				}
				m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_LOGOUT,_T("注销"));
				m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EXIT_APP,_T("退出"));
				//CImageEx pBtnImage;
				//HGDIOBJ pBtnBitmap[3];
				//pBtnImage.LoadResource(IDB_BITMAP_BTN_FILE);
				//pBtnImage.Cut(3, pBtnBitmap);
				//CBitmap pBitmap1;
				//pBitmap1.Attach(pBtnBitmap[0]);
				////m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_LOGOUT,&pBitmap1);
				//m_menuTray.SetMenuItemBitmaps(EB_COMMAND_LOGOUT,MF_BYCOMMAND,&pBitmap1,&pBitmap1);
			}
			if (!theApp.IsLogonVisitor())
			{
#ifdef USES_EBCOM_TEST
				EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
#else
				EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
#endif
				m_menuTray.CheckMenuItem(EB_COMMAND_STATE_ONLINE,nOutLineState==EB_LINE_STATE_ONLINE_NEW?MF_CHECKED:MF_UNCHECKED);
				m_menuTray.CheckMenuItem(EB_COMMAND_STATE_AWAY,nOutLineState==EB_LINE_STATE_AWAY?MF_CHECKED:MF_UNCHECKED);
				m_menuTray.CheckMenuItem(EB_COMMAND_LOGOUT,nOutLineState==EB_LINE_STATE_BUSY?MF_CHECKED:MF_UNCHECKED);
			}
			CPoint point;
			GetCursorPos(&point);
			m_menuTray.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);

			//m_menu2.LoadMenu( IDR_MENU_TRAY );
			//CMenu *pMenu = m_menu2.GetSubMenu(0);
			//pMenu->CheckMenuItem(ID_STATE_ONLINE,theEBAppClient.EB_GetLineState()==EB_LINE_STATE_ONLINE_NEW?MF_CHECKED:MF_UNCHECKED);
			//pMenu->CheckMenuItem(ID_STATE_AWAY,theEBAppClient.EB_GetLineState()==EB_LINE_STATE_AWAY?MF_CHECKED:MF_UNCHECKED);
			//pMenu->CheckMenuItem(ID_STATE_BUSY,theEBAppClient.EB_GetLineState()==EB_LINE_STATE_BUSY?MF_CHECKED:MF_UNCHECKED);
			//CPoint point;
			//GetCursorPos(&point);
			//pMenu->TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
		}break;
	case WM_LBUTTONDOWN:
		{
			if (theApp.GetHideMainFrame() && theApp.GetAutoOpenSubId()>0)
			{
				theApp.ShowHideAutoOpenSubscribeFunc(true);
			}else if (this->IsWindowVisible())
			{
				this->ShowWindow(SW_HIDE);
				if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
					m_pPanelSearch->ShowWindow(SW_HIDE);
			}else
			{
				OnOpenMain();
			}
		}break; 
	default:
		break;
	}
	return TRUE;
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgMainFrame::Fire_onSearchMemberInfo(IEB_GroupInfo* pGroupInfo, IEB_MemberInfo* pMemberInfo, ULONG dwParam)
{
	if (dwParam == 1)
	{
		// search
		const eb::bigint sEnterpriseCode(pGroupInfo->EnterpriseCode);
		const eb::bigint sMemberCode(pMemberInfo->MemberCode);
		const eb::bigint sMemberUserid(pMemberInfo->MemberUserId);
		const CEBString sMemberAccount(pMemberInfo->MemberAccount.GetBSTR());
		const CEBString sUserName(pMemberInfo->UserName.GetBSTR());
		if (sMemberUserid==theApp.GetLogonUserId())
			return;

		const CString sGroupResultName = pGroupInfo->m_sEnterpriseCode==0?_T("群组成员搜索结果："):_T("部门员工搜索结果：");
		const eb::bigint nGroupMemberCode = pGroupInfo->m_sEnterpriseCode==0?0:-1;
		CLockMap<eb::bigint, CTreeItemInfo::pointer>& pMemberItemList = pGroupInfo->m_sEnterpriseCode==0?m_pMyGroupMemberItem:m_pEntGroupMemberItem;

		CTreeItemInfo::pointer pMemberGroupInfo;
		if (!pMemberItemList.find(nGroupMemberCode,pMemberGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pMemberGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pMemberGroupInfo->m_dwItemData = 1;
			pMemberItemList.insert(nGroupMemberCode,pMemberGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pMemberGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CEBUIApp::TreeCmpFunc);
			if ((!m_bAppFrameShowed || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hEmpItem,TVGN_CARET);
		}
		if (!pMemberItemList.exist(sMemberCode))
		{
			CString sSearchName;
			sSearchName.Format(_T("%s(%s)"),sUserName.c_str(),sMemberAccount.c_str());
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sSearchName,pMemberGroupInfo->m_hItem);
			CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sMemberCode = sMemberCode;
			pEmpItemInfo->m_nUserId = sMemberUserid;
			pEmpItemInfo->m_sAccount = sMemberAccount;
			pEmpItemInfo->m_sGroupCode = pMemberInfo->GroupCode;
			pEmpItemInfo->m_sName = sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->LineState;
			//pEmpItemInfo->m_pGroupInfo = pGroupInfo;
			//pEmpItemInfo->m_pMemberInfo = pMemberInfo;
			pMemberItemList.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pEmpItemInfo.get());
			m_treeSearch.Sort(pMemberGroupInfo->m_hItem,CEBUIApp::TreeCmpFunc);
			ShowSearchResult();
			const int nGroupResultSize = pMemberItemList.size()-1;
			if (nGroupResultSize==1)
			{
				m_treeSearch.Expand(pMemberGroupInfo->m_hItem,TVE_EXPAND);
			}
			sSearchName.Format(_T("%s%d"),sGroupResultName,nGroupResultSize);
			m_treeSearch.SetItemText(pMemberGroupInfo->m_hItem,sSearchName);
			ScrollTreeSearchToTop();
		}
	}
}
#else
void CDlgMainFrame::onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam)
{
#ifndef USES_MIN_UI
	if (dwParam == 1)
	{
		// search
		if (pMemberInfo->m_nMemberUserId==theApp.GetLogonUserId())
			return;

		const CString sGroupResultName = pGroupInfo->m_sEnterpriseCode==0?_T("群组成员搜索结果："):_T("部门员工搜索结果：");
		const eb::bigint nGroupMemberCode = pGroupInfo->m_sEnterpriseCode==0?0:-1;
		CLockMap<eb::bigint, CTreeItemInfo::pointer>& pMemberItemList = pGroupInfo->m_sEnterpriseCode==0?m_pMyGroupMemberItem:m_pEntGroupMemberItem;

		CTreeItemInfo::pointer pMemberGroupInfo;
		if (!pMemberItemList.find(nGroupMemberCode,pMemberGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pMemberGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pMemberGroupInfo->m_dwItemData = 1;
			pMemberItemList.insert(nGroupMemberCode,pMemberGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pMemberGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CEBUIApp::TreeCmpFunc);
			if ((!m_bAppFrameShowed || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hEmpItem,TVGN_CARET);
		}
		if (!pMemberItemList.exist(pMemberInfo->m_sMemberCode))
		{
			CString sSearchName;
			sSearchName.Format(_T("%s(%s)"),pMemberInfo->m_sUserName.c_str(),pMemberInfo->m_sMemberAccount.c_str());
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sSearchName,pMemberGroupInfo->m_hItem);
			CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
			pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
			pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
			pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
			pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
			//pEmpItemInfo->m_pGroupInfo = pGroupInfo;
			//pEmpItemInfo->m_pMemberInfo = pMemberInfo;
			pMemberItemList.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pEmpItemInfo.get());
			m_treeSearch.Sort(pMemberGroupInfo->m_hItem,CEBUIApp::TreeCmpFunc);
			ShowSearchResult();
			const int nGroupResultSize = pMemberItemList.size()-1;
			if (nGroupResultSize==1)
			{
				m_treeSearch.Expand(pMemberGroupInfo->m_hItem,TVE_EXPAND);
			}
			sSearchName.Format(_T("%s%d"),sGroupResultName,nGroupResultSize);
			m_treeSearch.SetItemText(pMemberGroupInfo->m_hItem,sSearchName);
			ScrollTreeSearchToTop();
		}
	}
#endif
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgMainFrame::Fire_onSearchContactInfo(IEB_ContactInfo* pContactInfo, ULONG dwParam)
{
	 if (dwParam == 1)
	{
		// search
		const CEBString sContact(pContactInfo->Contact.GetBSTR());
		const CEBString sName(pContactInfo->Name.GetBSTR());

		const std::string theGroupContactAccount("[* *]");
		const CString sGroupResultName = _T("联系人搜索结果：");
		CTreeItemInfo::pointer pContactGroupInfo;
		if (!m_pContactItem.find(theGroupContactAccount,pContactGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pContactGroupInfo->m_dwItemData = 1;
			m_pContactItem.insert(theGroupContactAccount,pContactGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CEBUIApp::TreeCmpFunc);
			if ((!m_bAppFrameShowed || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hEmpItem,TVGN_CARET);
		}
		if (!m_pContactItem.exist(sContact))
		{
			CString sSearchName;
			sSearchName.Format(_T("%s(%s)"),sName.c_str(),sContact.c_str());
			HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
			CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
			pContactItemInfo->m_sId = pContactInfo->ContactId;
			pContactItemInfo->m_sAccount = sContact;
			pContactItemInfo->m_sName = sName;
			//pContactItemInfo->m_pContactInfo = pContactInfo;
			m_pContactItem.insert(pContactItemInfo->m_sAccount,pContactItemInfo);
			m_treeSearch.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
			ShowSearchResult();
			const int nGroupResultSize = m_pContactItem.size()-1;
			if (nGroupResultSize==1)
			{
				m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
			}
			sSearchName.Format(_T("%s%d"),sGroupResultName,nGroupResultSize);
			m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
			ScrollTreeSearchToTop();
		}
	}
}
#else
void CDlgMainFrame::onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam)
{
#ifndef USES_MIN_UI
	 if (dwParam == 1)
	{
		// search
		const std::string theGroupContactAccount("[* *]");
		const CString sGroupResultName = _T("联系人搜索结果：");
		CTreeItemInfo::pointer pContactGroupInfo;
		if (!m_pContactItem.find(theGroupContactAccount,pContactGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pContactGroupInfo->m_dwItemData = 1;
			m_pContactItem.insert(theGroupContactAccount,pContactGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CEBUIApp::TreeCmpFunc);
			if ((!m_bAppFrameShowed || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hEmpItem,TVGN_CARET);
		}
		if (!m_pContactItem.exist(pContactInfo->m_sContact))
		{
			CString sSearchName;
			sSearchName.Format(_T("%s(%s)"),pContactInfo->m_sName.c_str(),pContactInfo->m_sContact.c_str());
			HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
			CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
			pContactItemInfo->m_sId = pContactInfo->m_nContactId;
			pContactItemInfo->m_sAccount = pContactInfo->m_sContact;
			pContactItemInfo->m_sName = pContactInfo->m_sName;
			//pContactItemInfo->m_pContactInfo = pContactInfo;
			m_pContactItem.insert(pContactItemInfo->m_sAccount,pContactItemInfo);
			m_treeSearch.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
			ShowSearchResult();
			const int nGroupResultSize = m_pContactItem.size()-1;
			if (nGroupResultSize==1)
			{
				m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
			}
			sSearchName.Format(_T("%s%d"),sGroupResultName,nGroupResultSize);
			m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
			ScrollTreeSearchToTop();
		}
	}
#endif
}
#endif

void CDlgMainFrame::ScrollTreeSearchToTop(void)
{
#ifndef USES_MIN_UI
	const int nScrollPos = m_treeSearch.GetScrollPos(SB_VERT);
	if (nScrollPos>0)
	{
		WPARAM wParam = MAKELONG(SB_TOP,nScrollPos);
		m_treeSearch.SendMessage(WM_VSCROLL,wParam);
	}
#endif
}

void CDlgMainFrame::ShowSearchResult(void)
{
#ifndef USES_MIN_UI
	if (!m_treeSearch.IsWindowVisible())
	{
		m_btnSwitchFrame.ShowWindow(SW_HIDE);
		CRect searchRect;
		m_editSearch.GetWindowRect(&searchRect);
		CRect rect;
		m_pDlgMyContacts->GetWindowRect(&rect);
		searchRect.top = searchRect.bottom+1;
		searchRect.bottom = rect.bottom;
		this->ScreenToClient(&searchRect);
		m_treeSearch.MoveWindow(&searchRect);
		m_treeSearch.ShowWindow(SW_SHOW);
		m_pDlgMyEnterprise->ShowWindow(SW_HIDE);
		m_pDlgMyGroup->ShowWindow(SW_HIDE);
		m_pDlgMyContacts->ShowWindow(SW_HIDE);
		m_pDlgMySession->ShowWindow(SW_HIDE);
		m_btnMyEnterprise.ShowWindow(SW_HIDE);
		m_btnMyDepartment.ShowWindow(SW_HIDE);
		m_btnMyContacts.ShowWindow(SW_HIDE);
		m_btnMySession.ShowWindow(SW_HIDE);
		if (m_pDlgAppFrame!=NULL&&m_bAppFrameShowed&&m_pDlgAppFrame->IsWindowVisible())
		{
			m_pDlgAppFrame->ShowWindow(SW_HIDE);
		}
		//if (m_btnGoBack.GetSafeHwnd()!=0&&m_btnGoBack.IsWindowVisible())
		//	m_btnGoBack.ShowWindow(SW_HIDE);
		//if (m_btnGoForward.GetSafeHwnd()!=0&&m_btnGoForward.IsWindowVisible())
		//	m_btnGoForward.ShowWindow(SW_HIDE);
		//if (m_pPanelBrowserNavigate!=0&&m_pPanelBrowserNavigate->IsWindowVisible())
		//	m_pPanelBrowserNavigate->ShowWindow(SW_HIDE);
		if (m_pPanelSearch!=0&&m_pPanelSearch->IsWindowVisible())
			m_pPanelSearch->ShowWindow(SW_HIDE);
	}
#endif
}
void CDlgMainFrame::OnEnChangeEditSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEbDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
#ifndef USES_MIN_UI
	if (!m_bCanSearch) return;
	CString sSearchString;
	m_editSearch.GetWindowText(sSearchString);
	if (sSearchString==theSearchString)
		return;
	theSearchString = sSearchString;

	if (!sSearchString.IsEmpty())
	{
		sSearchString.Trim();
		m_pUrlItem.clear();
		m_pContactItem.clear();
		m_pEntGroupMemberItem.clear();
		m_pMyGroupMemberItem.clear();
		m_treeSearch.DeleteAllItems();

		CString sSql;
		std::string sSearchKey(sSearchString);
		CSqliteCdbc::escape_string_in(sSearchKey);
		sSearchKey = libEbc::ACP2UTF8(sSearchKey.c_str());
		sSql.Format(_T("select url,title FROM url_record_t WHERE url LIKE '%%%s%%' OR title LIKE '%%%s%%' ORDER BY last_time DESC LIMIT 8"),sSearchKey.c_str(),sSearchKey.c_str());
		int nCookie = 0;
		theApp.m_pBoUsers->select(sSql, nCookie);
		cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
		while (pRecord.get()!=NULL)
		{
			tstring sUrl(pRecord->getVector()[0]->getStrValue());
			tstring sTitle(pRecord->getVector()[1]->getStrValue());
			pRecord = theApp.m_pBoUsers->next(nCookie);
			CSqliteCdbc::escape_string_out(sUrl);
			CSqliteCdbc::escape_string_out(sTitle);
			sUrl = libEbc::UTF82ACP(sUrl.c_str());
			sTitle = libEbc::UTF82ACP(sTitle.c_str());

			// search
			CTreeItemInfo::pointer pContactGroupInfo;
			if (!m_pUrlItem.find(theUrlGroupName,pContactGroupInfo))
			{
				HTREEITEM hEmpItem = m_treeSearch.InsertItem(theUrlGroupResultName);
				pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
				pContactGroupInfo->m_dwItemData = 2;
				m_pUrlItem.insert(theUrlGroupName,pContactGroupInfo);
				m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
				m_treeSearch.Sort(TVI_ROOT, CEBUIApp::TreeCmpFunc);
			}
			if (!m_pUrlItem.exist(sUrl))
			{
				CString sSearchName;
				sSearchName.Format(_T("%s\nCLR=%d;%s"),sTitle.c_str(),(int)RGB(0,128,255),sUrl.c_str());
				HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
				CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_URL,hContactItem);
				pContactItemInfo->m_sAccount = sUrl;
				pContactItemInfo->m_sName = sTitle;
				m_pUrlItem.insert(pContactItemInfo->m_sAccount,pContactItemInfo);
				m_treeSearch.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
				ShowSearchResult();
				const int nGroupResultSize = m_pUrlItem.size()-1;
				if (nGroupResultSize==1)
				{
					if (this->m_bAppFrameShowed && m_treeSearch.GetSelectedItem()==NULL)
						m_treeSearch.Select(hContactItem,TVGN_CARET);
					m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
				}
				sSearchName.Format(_T("%s%d"),theUrlGroupResultName,nGroupResultSize);
				m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
				//ScrollTreeSearchToTop();
			}
		}
		theApp.m_pBoUsers->reset(nCookie);

		if (sSearchString!="about:blank" &&
			sSearchString.Find("http://")<0 &&
			sSearchString.Find("https://")<0 &&
			sSearchString.Find("www.")<0)
		{

#ifdef USES_EBCOM_TEST
			//IDispatch* pCallback;
			//((CEBSearchEventsSink*)this)->QueryInterface(IID_IDispatch,(void**)&pCallback);
			theEBClientCore->EB_SearchUserInfo((IDispatch*)(CEBSearchEventsSink*)this, (LPCTSTR)sSearchString, 1);
#else
			theEBAppClient.EB_SearchUserInfo((CEBSearchCallback*)this, sSearchString, 1);
#endif
		}
		if (m_treeSearch.GetCount() > 0)
		{
			ShowSearchResult();
		}
	}
	if (m_treeSearch.GetCount() == 0 || sSearchString.IsEmpty())
	{
		m_treeSearch.DeleteAllItems();
		m_treeSearch.ShowWindow(SW_HIDE);
		
		if (m_pDlgAppFrame!=NULL&&m_bAppFrameShowed)//m_pDlgAppFrame->IsWindowVisible()))
		{
			if (!m_pDlgAppFrame->IsWindowVisible())
			{
				//m_pDlgAppFrame->SetCurrentFocus();
				m_pDlgAppFrame->ShowWindow(SW_SHOW);
			}
			//if (m_btnGoBack.GetSafeHwnd()!=0&&!m_btnGoBack.IsWindowVisible())
			//	m_btnGoBack.ShowWindow(SW_SHOW);
			//if (m_btnGoForward.GetSafeHwnd()!=0&&!m_btnGoForward.IsWindowVisible())
			//	m_btnGoForward.ShowWindow(SW_SHOW);
			//if (m_pPanelBrowserNavigate!=0 && !m_pPanelBrowserNavigate->IsWindowVisible())
			//	m_pPanelBrowserNavigate->ShowWindow(SW_SHOW);
			if (m_pPanelSearch!=0 && !m_pPanelSearch->IsWindowVisible())
				m_pPanelSearch->ShowWindow(SW_SHOW);
			m_editSearch.SetFocus();
			return;
		}

		if (theApp.IsEnterpriseuserUser())
		{
			m_btnMyEnterprise.ShowWindow(SW_SHOW);
		}
		m_btnSwitchFrame.ShowWindow(SW_SHOW);
		m_btnMyDepartment.ShowWindow(SW_SHOW);
		m_btnMyContacts.ShowWindow(SW_SHOW);
		m_btnMySession.ShowWindow(SW_SHOW);
		if (m_btnMyEnterprise.GetChecked())
			m_pDlgMyEnterprise->ShowWindow(SW_SHOW);
		else if (m_btnMyDepartment.GetChecked())
			m_pDlgMyGroup->ShowWindow(SW_SHOW);
		else if (m_btnMyContacts.GetChecked())
			m_pDlgMyContacts->ShowWindow(SW_SHOW);
		else if (m_btnMySession.GetChecked())
			m_pDlgMySession->ShowWindow(SW_SHOW);
	}
#endif
}
LRESULT CDlgMainFrame::OnMsgRefreshUrl(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->doRefresh();
	return 0;
}
LRESULT CDlgMainFrame::OnMsgGoBack(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->goBack();
	return 0;
}
LRESULT CDlgMainFrame::OnMsgGoForward(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->goForward();
	return 0;
}
LRESULT CDlgMainFrame::OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam)
{
	char * sUrl = (char*)wParam;
	if (sUrl==0 || strlen(sUrl)==0) return 0;
	EB_SubscribeFuncInfo pSubscribeFuncInfo;
	pSubscribeFuncInfo.m_sFunctionName = sUrl;
	pSubscribeFuncInfo.m_bDisableContextMenu = (lParam&0x1)!=0;;
	pSubscribeFuncInfo.m_bDisableScrollBar = (lParam&0x2)!=0;
	pSubscribeFuncInfo.m_bClosePrompt = (lParam&0x4)!=0;
	pSubscribeFuncInfo.m_bWinResizable = true;
	const bool nOpenNewClose = (lParam&0x8)!=0;
	this->AddAppUrl(sUrl,pSubscribeFuncInfo,nOpenNewClose);
	delete[] sUrl;
	return 0;
}

LRESULT CDlgMainFrame::OnMsgExitApp(WPARAM wp, LPARAM lp)
{
	OnExitApp();
	return 0;
}
LRESULT CDlgMainFrame::OnMsgLogout(WPARAM wp, LPARAM lp)
{
	OnLogout();
	return 0;
}
LRESULT CDlgMainFrame::OnMsgShowHideMainFrame(WPARAM wp, LPARAM lp)
{
	const bool bShowMainFrame = wp==1?true:false;
	if (theApp.GetHideMainFrame() && theApp.GetAutoOpenSubId()>0)
	{
		theApp.ShowHideAutoOpenSubscribeFunc(bShowMainFrame);
	}else if (bShowMainFrame)
	{
		OnOpenMain();
	}else
	{
		this->ShowWindow(SW_HIDE);
#ifndef USES_MIN_UI
		if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
			m_pPanelSearch->ShowWindow(SW_HIDE);
#endif
	}

	return 0;
}

void CDlgMainFrame::OnExitApp()
{
	theWantExitApp = true;
	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgMainFrame::OnLogout()
{
	theWantExitApp = true;
	PostMessage(WM_CLOSE, 0, 0);
	return;
	//// 重新启动新应用
	//CString strExe;
	//GetModuleFileName( NULL, strExe.GetBuffer(MAX_PATH), MAX_PATH);
	//strExe.ReleaseBuffer(-1);
	//ShellExecute(m_hWnd,_T("open"),strExe,_T("logout"),theApp.GetAppPath(),SW_SHOW);
}

void CDlgMainFrame::OnBnClickedButtonLinestate()
{
#ifndef USES_MIN_UI
	const CString sAutoRunAccount = theApp.EBC_GetProfileString(_T("system"),_T("auto-run-account"));
	const int const_my_ecards_index = 4;						// ***修改菜单项，需要修改这个地方
	const int const_app_func_index = const_my_ecards_index+1;	// ***修改菜单项，需要修改这个地方
	if (m_menuState.GetSafeHmenu()==NULL)
	{
		m_menuState.CreatePopupMenu();
		if (!theApp.IsLogonVisitor())
		{
			m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_STATE_ONLINE,_T("在线"));
			m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_STATE_AWAY,_T("离开"));
			m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_STATE_BUSY,_T("忙碌"));
			m_menuState.AppendMenu(MF_SEPARATOR);
			if (!sAutoRunAccount.IsEmpty())
				m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_AUTO_LOGIN,_T("自动登录"));
			m_menuState.AppendMenu(MF_SEPARATOR);
		}
		m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_LOGOUT,_T("注销"));
		m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EXIT_APP,_T("退出"));
	}else
	{
		if (m_bRemoveAppFuncIndex)
			m_menuState.RemoveMenu(const_app_func_index,MF_BYPOSITION);
		if (!theApp.IsLogonVisitor())
			m_menuState.RemoveMenu(const_my_ecards_index,MF_BYPOSITION);
	}

	if (!theApp.IsLogonVisitor())
	{
#ifdef USES_EBCOM_TEST
		EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
#else
		EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
#endif
		m_menuState.CheckMenuItem(EB_COMMAND_AUTO_LOGIN,sAutoRunAccount.IsEmpty()?MF_UNCHECKED:MF_CHECKED);
		m_menuState.CheckMenuItem(EB_COMMAND_STATE_ONLINE,nOutLineState==EB_LINE_STATE_ONLINE_NEW?MF_CHECKED:MF_UNCHECKED);
		m_menuState.CheckMenuItem(EB_COMMAND_STATE_AWAY,nOutLineState==EB_LINE_STATE_AWAY?MF_CHECKED:MF_UNCHECKED);
		m_menuState.CheckMenuItem(EB_COMMAND_STATE_BUSY,nOutLineState==EB_LINE_STATE_BUSY?MF_CHECKED:MF_UNCHECKED);
		m_pMyEmployeeInfo.clear();
#ifdef USES_EBCOM_TEST
		_variant_t pMyMemberList = theEBClientCore->EB_GetMyMemberList();
		if (pMyMemberList.vt!=VT_EMPTY && pMyMemberList.parray != NULL)
		{
			CComSafeArray<VARIANT> m_sa(pMyMemberList.parray);
			for (ULONG i=0;i<m_sa.GetCount();i++)
			{
				CComVariant var = m_sa.GetAt(i);
				if (var.vt != VT_DISPATCH)
					continue;
				CComPtr<IEB_MemberInfo> pInterface;
				var.pdispVal->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
				m_pMyEmployeeInfo.push_back(EB_MemberInfo(pInterface));
			}
		}
#else
		theEBAppClient.EB_GetMyMemberList(m_pMyEmployeeInfo);
#endif

#ifdef USES_EBCOM_TEST
		const eb::bigint sDefaultMemberCode = theEBClientCore->EB_MyDefaultMemberCode;
#else
		eb::bigint sDefaultMemberCode;
		theEBAppClient.EB_GetMyDefaultMemberCode(sDefaultMemberCode);
#endif
		CMenu pPopupMenu;
		pPopupMenu.CreatePopupMenu();
		for (size_t i=0;i<m_pMyEmployeeInfo.size();i++)
		{
			EB_MemberInfo pMemberInfo = m_pMyEmployeeInfo[i];
#ifdef USES_EBCOM_TEST
			const CEBString sGroupName = theEBClientCore->EB_GetGroupName(pMemberInfo.m_sGroupCode).GetBSTR();
			if (!sGroupName.empty())
#else
			CEBString sGroupName;
			if (theEBAppClient.EB_GetGroupName(pMemberInfo.m_sGroupCode,sGroupName))
#endif
			{
				CString stext;
				stext.Format(_T("%s-%s"),pMemberInfo.m_sUserName.c_str(),sGroupName.c_str());
				pPopupMenu.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MY_EMPLOYEE+i,stext);
				if (sDefaultMemberCode == pMemberInfo.m_sMemberCode)
				{
					pPopupMenu.CheckMenuItem(EB_COMMAND_MY_EMPLOYEE+i,MF_CHECKED);
				}
			}
		}
		m_menuState.InsertMenu(const_my_ecards_index,MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenu.m_hMenu,_T("我的名片"));
	}

	// 应用功能菜单
	CMenu pPopupMenu;
	pPopupMenu.CreatePopupMenu();
	theApp.ClearSubscribeSelectInfo();
	m_pSubscribeFuncList.clear();
#ifdef USES_EBCOM_TEST
	_variant_t pSubscribeFuncList = theEBClientCore->EB_GetSubscribeFuncList(EB_FUNC_LOCATION_RIGHT_CLICK_MENU_MAINFRAME);
	if (pSubscribeFuncList.vt!=VT_EMPTY && pSubscribeFuncList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pSubscribeFuncList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo;
			var.pdispVal->QueryInterface(__uuidof(IEB_SubscribeFuncInfo),(void**)&pSubscribeFuncInfo);
			if (pSubscribeFuncInfo == NULL) continue;
			m_pSubscribeFuncList.push_back(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
		}
	}
#else
	theEBAppClient.EB_GetSubscribeFuncList(EB_FUNC_LOCATION_RIGHT_CLICK_MENU_MAINFRAME,m_pSubscribeFuncList);
#endif
	if (!m_pSubscribeFuncList.empty())
	{
		for (size_t i=0;i<m_pSubscribeFuncList.size();i++)
		{
			const EB_SubscribeFuncInfo & pSubscribeFuncInfo = m_pSubscribeFuncList[i];
			pPopupMenu.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,pSubscribeFuncInfo.m_sFunctionName.c_str());
		}
		m_bRemoveAppFuncIndex = true;
		m_menuState.InsertMenu(const_app_func_index,MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenu.m_hMenu,_T("应用功能"));
	}

	CPoint point;
	GetCursorPos(&point);
	m_menuState.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);

	//CMenu m_menu2;
	//m_menu2.LoadMenu( IDR_MENU_LINESTATE );
	//CMenu *pMenu = m_menu2.GetSubMenu(0);
	//pMenu->CheckMenuItem(ID_STATE_ONLINE,theEBAppClient.EB_GetLineState()==EB_LINE_STATE_ONLINE_NEW?MF_CHECKED:MF_UNCHECKED);
	//pMenu->CheckMenuItem(ID_STATE_AWAY,theEBAppClient.EB_GetLineState()==EB_LINE_STATE_AWAY?MF_CHECKED:MF_UNCHECKED);
	//pMenu->CheckMenuItem(ID_STATE_BUSY,theEBAppClient.EB_GetLineState()==EB_LINE_STATE_BUSY?MF_CHECKED:MF_UNCHECKED);
	//CPoint point;
	//GetCursorPos(&point);
	//pMenu->TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
#endif
}
void CDlgMainFrame::OnMyEmployeeInfo(UINT nID)
{
#ifndef USES_MIN_UI
	size_t nIndex = nID-EB_COMMAND_MY_EMPLOYEE;
	if (nIndex>=0 && nIndex<m_pMyEmployeeInfo.size())
	{
		EB_MemberInfo pMemberInfo = m_pMyEmployeeInfo[nIndex];
#ifdef USES_EBCOM_TEST
		CIEB_MemberInfo * pIEBMemberInfo = new CIEB_MemberInfo(pMemberInfo);
		CComPtr<IEB_MemberInfo> pOutInterface;
		pIEBMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pOutInterface);
		theApp.EditEmployeeInfo(this,pOutInterface);
#else
		theApp.EditEmployeeInfo(this,&pMemberInfo);
#endif
	}
#endif
}
void CDlgMainFrame::OnSubscribeFunc(UINT nID)
{
#ifndef USES_MIN_UI
	const size_t nIndex = nID-EB_COMMAND_SUBSCRIBE_FUNC;
	if (nIndex>=0 && nIndex<m_pSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = m_pSubscribeFuncList[nIndex];
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo);
	}
#endif
}
void CDlgMainFrame::OnMainFrameFunc(UINT nID)
{
#ifndef USES_MIN_UI
	const size_t nIndex = nID-EB_COMMAND_MAINFRAME_FUNC;
	if (nIndex>=0 && nIndex<m_pMainFuncButtonList.size())
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[nIndex];
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = pFuncButtonInfo->GetFuncInfo();
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo);
	}
#endif
}
void CDlgMainFrame::NewAppFrame(void)
{
	if (m_pDlgAppFrame==NULL)
	{
		//CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		AFX_SWITCH_INSTANCED();
		m_pDlgAppFrame = new CDlgAppFrame(this);
		m_pDlgAppFrame->Create(CDlgAppFrame::IDD,this);
		CRect searchRect;
		if (theApp.IsLogonVisitor())
			m_btnMySession.GetWindowRect(&searchRect);
		else
			m_btnMyDepartment.GetWindowRect(&searchRect);

		CRect rect;
		m_pDlgMyContacts->GetWindowRect(&rect);
		searchRect.left = rect.left+1;
		searchRect.right = rect.right-1;
		searchRect.bottom = rect.bottom;
		this->ScreenToClient(&searchRect);
		m_pDlgAppFrame->MoveWindow(&searchRect);
	}
}
void CDlgMainFrame::AddAppUrl(const std::string& sAppUrl,const EB_SubscribeFuncInfo& pSubscribeFuncInfo, bool nOpenNewClose)
{
	// 应用面板
	if (theApp.GetHideMainFrame() && theApp.GetAutoOpenSubId()>0)
	{
		theApp.ShowHideAutoOpenSubscribeFunc(true);
	}else if (!this->IsWindowVisible() || this->IsIconic())
	{
		OnOpenMain();
	}

	NewAppFrame();
	m_pDlgAppFrame->AddAppUrl(sAppUrl, pSubscribeFuncInfo, nOpenNewClose);
	OnBnClickedButtonSwitchFrame();
}
void CDlgMainFrame::AddAppUrl(const std::string& sAppUrl)
{
	EB_SubscribeFuncInfo pSubscribeFuncInfo;
	pSubscribeFuncInfo.m_sFunctionName = sAppUrl;
	pSubscribeFuncInfo.m_bClosePrompt = false;
	pSubscribeFuncInfo.m_bDisableScrollBar = false;
	pSubscribeFuncInfo.m_bDisableContextMenu = false;
	pSubscribeFuncInfo.m_bWinResizable = true;
	AddAppUrl(sAppUrl,pSubscribeFuncInfo,false);
}

void CDlgMainFrame::SetChatWindowTopMost(bool bTopMost)
{
	if (m_pDlgFrameList!=NULL && m_pDlgFrameList->GetSafeHwnd()!=NULL)
	{
		if (bTopMost)
			m_pDlgFrameList->SetWindowPos(&CWnd::wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		else
			m_pDlgFrameList->SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

void CDlgMainFrame::ChangeTrayText(void)
{
#ifndef USES_MIN_UI
	CString sTrayText;
#ifdef USES_EBCOM_TEST
	const CEBString sUserName = theEBClientCore->EB_UserName.GetBSTR();
	EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
	CEBString sEnterpriseName = theEBClientCore->EB_GetEnterpriseName(0).GetBSTR();
#else
	std::string sUserName = theEBAppClient.EB_GetUserName();
	EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
	std::string sEnterpriseName;
	theEBAppClient.EB_GetEnterpriseName(sEnterpriseName);
#endif
	if (!theApp.GetProductName())
		sEnterpriseName = (LPCTSTR)theApp.GetProductName();
	if (theApp.IsLogonVisitor())
		sTrayText.Format(_T("游客-%s-%s"),theApp.GetLogonAccount(),GetLineStateText(nOutLineState));
	else if (!sEnterpriseName.empty())
		sTrayText.Format(_T("%s(%s)-%s\n%s"),sUserName.c_str(),theApp.GetLogonAccount(),GetLineStateText(nOutLineState),sEnterpriseName.c_str());
	else
		sTrayText.Format(_T("%s(%s)-%s"),sUserName.c_str(),theApp.GetLogonAccount(),GetLineStateText(nOutLineState));
	lstrcpy(m_trayIconData.szTip, sTrayText);
	Shell_NotifyIcon(NIM_MODIFY, &m_trayIconData);
#endif
}

void CDlgMainFrame::OnStateOnline()
{
#ifndef USES_MIN_UI
#ifdef USES_EBCOM_TEST
	EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
#else
	EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
#endif
	if (nOutLineState == EB_LINE_STATE_ONLINE_NEW)
	{
		return;
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_LineState = EB_LINE_STATE_ONLINE_NEW;
#else
	theEBAppClient.EB_SetLineState(EB_LINE_STATE_ONLINE_NEW);
#endif
	m_btnLineState.Load(IDB_PNG_BTN_STATE_ONLINE);
	//m_btnLineState.SetWindowText(_T("在线"));
	m_btnLineState.Invalidate();
	theApp.SetLineState2Db(EB_LINE_STATE_ONLINE_NEW);
	ChangeTrayText();
#endif
}

void CDlgMainFrame::OnStateAway()
{
#ifndef USES_MIN_UI
#ifdef USES_EBCOM_TEST
	EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
#else
	EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
#endif
	if (nOutLineState == EB_LINE_STATE_AWAY)
	{
		return;
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_LineState = EB_LINE_STATE_AWAY;
#else
	theEBAppClient.EB_SetLineState(EB_LINE_STATE_AWAY);
#endif
	m_btnLineState.Load(IDB_PNG_BTN_STATE_AWAY);
	//m_btnLineState.SetWindowText(_T("离开"));
	m_btnLineState.Invalidate();
	theApp.SetLineState2Db(EB_LINE_STATE_AWAY);
	ChangeTrayText();
#endif
}

void CDlgMainFrame::OnStateBusy()
{
#ifndef USES_MIN_UI
#ifdef USES_EBCOM_TEST
	EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
#else
	EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
#endif
	if (nOutLineState == EB_LINE_STATE_BUSY)
	{
		return;
	}
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_LineState = EB_LINE_STATE_BUSY;
#else
	theEBAppClient.EB_SetLineState(EB_LINE_STATE_BUSY);
#endif
	m_btnLineState.Load(IDB_PNG_BTN_STATE_BUSY);
	//m_btnLineState.SetWindowText(_T("忙"));
	m_btnLineState.Invalidate();
	theApp.SetLineState2Db(EB_LINE_STATE_BUSY);
	ChangeTrayText();
#endif
}
void CDlgMainFrame::OnAutoLogin()
{
	if (!theApp.IsLogonVisitor())
	{
		const CString sAutoRunExeName = _T("entboost");
		HKEY hKey = NULL;
		LPCTSTR lpRun = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
		long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE|KEY_READ, &hKey);
		if(lRet == ERROR_SUCCESS)
		{
			char pFileName[MAX_PATH] = {0};
			GetModuleFileName(NULL, pFileName, MAX_PATH);
			const std::string sModuleFileName(pFileName);
			DWORD dwDataSize = sModuleFileName.size();
			lRet = ::RegQueryValueEx(hKey,sAutoRunExeName,0,0,(BYTE*)pFileName,&dwDataSize);
			if(lRet != ERROR_SUCCESS || sModuleFileName != pFileName)
			{
				lRet = RegSetValueEx(hKey,sAutoRunExeName,0,REG_SZ,(BYTE *)sModuleFileName.c_str(),sModuleFileName.size());
			}
			RegCloseKey(hKey);
		}

		const CString sAutoRunAccount = theApp.EBC_GetProfileString(_T("system"),_T("auto-run-account"));
		if (sAutoRunAccount.IsEmpty())
		{
			theApp.EBC_SetProfileString(_T("system"),_T("auto-run-account"),theApp.GetLogonAccount());
		}else
		{
			theApp.EBC_SetProfileString(_T("system"),_T("auto-run-account"),_T(""));
		}
	}
}

void CDlgMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
#ifndef USES_MIN_UI
	CPoint pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	if (m_rectHead.PtInRect(pos))
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
		return;
	}else if (m_editDescription.GetSafeHwnd()!=0 && this->GetFocus() != &m_editDescription)
	{
		CRect rect;
		m_editDescription.GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		if (rect.PtInRect(pos))
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
			return;
		}
	}
#endif
	__super::OnMouseMove(nFlags, point);
}
void CDlgMainFrame::CancelSaveDescription(void)
{
#ifndef USES_MIN_UI
#ifdef USES_EBCOM_TEST
	const CEBString sOutDesc = theEBClientCore->EB_Description.GetBSTR();
#else
	std::string sOutDesc = theEBAppClient.EB_GetDescription();
#endif
	m_editDescription.SetWindowText(sOutDesc.c_str());
	RefreshEditDescription();
#endif
}

void CDlgMainFrame::OnEnKillfocusEditDescription()
{
#ifndef USES_MIN_UI
	CString sNewDesc;
	m_editDescription.GetWindowText(sNewDesc);
#ifdef USES_EBCOM_TEST
	const CEBString sOutDesc = theEBClientCore->EB_Description.GetBSTR();
#else
	std::string sOutDesc = theEBAppClient.EB_GetDescription();
#endif
	if (sOutDesc != (LPCTSTR)sNewDesc)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_Description = (LPCTSTR)sNewDesc;
#else
		theEBAppClient.EB_SetDescription(sNewDesc);
#endif
	}
	//m_editDescription.SetWindowText(sOutDesc.c_str());
	RefreshEditDescription();
#endif
}
void CDlgMainFrame::RefreshEditDescription(void)
{
#ifndef USES_MIN_UI
	//m_editDescription.EnableWindow(FALSE);
	m_editDescription.SetReadOnly(TRUE);
	m_editDescription.ShowWindow(SW_HIDE);
	CRect rect;
	m_editDescription.GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	this->InvalidateRect(rect);
#endif
}

void CDlgMainFrame::OnOK()
{
	//__super::OnOK();
}

void CDlgMainFrame::OnCancel()
{
	if (!theWantExitApp)
	{
		//OnBnClickedButtonClose();
		return;
	}
	__super::OnCancel();
}


void CDlgMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos);
#ifndef USES_MIN_UI
	if (m_rectHead.PtInRect(pos))
	{
		// 
		OnMySetting();
//#ifdef USES_EBCOM_TEST
//		CComPtr<IEB_MemberInfo> pOutInterface = theEBClientCore->EB_GetMyDefaultMemberInfo();
//		theApp.EditEmployeeInfo(this,pOutInterface);
//#else
//		EB_MemberInfo pMemberInfo;
//		theEBAppClient.EB_GetMyDefaultMemberInfo(&pMemberInfo);
//		theApp.EditEmployeeInfo(this,&pMemberInfo);
//#endif
	}else if (m_editDescription.GetSafeHwnd())
	{
		if (this->GetFocus() == &m_editDescription)
		{
			RefreshEditDescription();
			m_editSearch.SetFocus();
		}else
		{
			CRect rect;
			m_editDescription.GetWindowRect(&rect);
			this->ScreenToClient(&rect);
			if (rect.PtInRect(pos))
			{
				m_editDescription.SetReadOnly(FALSE);
				m_editDescription.ShowWindow(SW_SHOW);
				m_editDescription.SetFocus();
				m_editDescription.SetSel(0,-1);
			}
		}
	}
#endif
	__super::OnLButtonDown(nFlags, point);
}

void CDlgMainFrame::OnMySetting()
{
	// TODO: Add your command handler code here
#ifndef USES_MIN_UI
	bool bIsNewWindow = false;
	if (m_pDlgEditInfo == NULL)
	{
		bIsNewWindow = true;
		m_pDlgEditInfo = new CDlgEditInfo(this);
		m_pDlgEditInfo->Create(CDlgEditInfo::IDD, this);
	}else
	{
		m_pDlgEditInfo->CheckData();
		m_pDlgEditInfo->SetCtrlColor();
	}
	if (bIsNewWindow || !m_pDlgEditInfo->IsWindowVisible())
	{
		const int const_dlg_width = 588;	// 508;
		const int const_dlg_height = 452;
		int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
		int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
		m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);
		CRect rect;
		rect.left = (m_nScreenWidth-const_dlg_width)/2;
		rect.top = (m_nScreenHeight-const_dlg_height)/2;
		rect.right = rect.left+const_dlg_width;
		rect.bottom = rect.top+const_dlg_height;
		m_pDlgEditInfo->MoveWindow(&rect);
	}
	m_pDlgEditInfo->ShowWindow(SW_SHOW);
#endif
}
void CDlgMainFrame::OnMyShare()
{
	if (!theApp.GetDisableUserCloudDrive() && !theApp.IsLogonVisitor())
	{
		theApp.OpenMyShareWindow(this);
	}
}

void CDlgMainFrame::OnSkinSelect(UINT nID)
{
	const size_t nIndex = nID-EB_COMMAND_SKIN_SETTING;
	if (nIndex == 0)
	{
		AFX_SWITCH_INSTANCED();
		CColorDialog dlg2(theApp.GetMainColor(), CC_FULLOPEN);
		if (dlg2.DoModal() != IDOK)
		{
			return;
		}
		theApp.SetMainColor(dlg2.GetColor());
	}else if (nIndex<=theColorSkinSize)
	{
		theApp.SetMainColor(theColorSkinsValue[nIndex-1]);
	}else
	{
		return;
	}
	SetCtrlColor();
	if (theApp.m_pBoEB->use("eb"))
	{
		CString sSql;
		sSql.Format(_T("UPDATE sys_value_t SET value2=%d WHERE name='main-color'"),(DWORD)theApp.GetMainColor());
		theApp.m_pBoEB->execsql(sSql);
		theApp.m_pBoEB->close();
	}
	this->Invalidate();
	if ((m_pDlgAppFrame!=NULL&&m_pDlgAppFrame->IsWindowVisible()))
	{
		// 解决m_pDlgAppFrame 背景刷新问题（按钮和IE背景）
		m_pDlgAppFrame->ShowWindow(SW_HIDE);
		m_pDlgAppFrame->ShowWindow(SW_SHOW);
	}
}
void CDlgMainFrame::OnSkinGradient(void)
{
	const bool bColorGradient = theApp.GetColorGradient()?false:true;
	theApp.SetColorGradient(bColorGradient);
	if (theApp.m_pBoEB->use("eb"))
	{
		CString sSql;
		sSql.Format(_T("UPDATE sys_value_t SET value2=%d WHERE name='color-gradient'"),(int)(bColorGradient?1:0));
		theApp.m_pBoEB->execsql(sSql);
		theApp.m_pBoEB->close();
	}
	this->Invalidate();
	if ((m_pDlgAppFrame!=NULL&&m_pDlgAppFrame->IsWindowVisible()))
	{
		// 解决m_pDlgAppFrame 背景刷新问题（按钮和IE背景）
		m_pDlgAppFrame->ShowWindow(SW_HIDE);
		m_pDlgAppFrame->ShowWindow(SW_SHOW);
	}
}

void CDlgMainFrame::OnBnClickedButtonSkin2()
{
#ifndef USES_MIN_UI
	if (m_menuSkin.GetSafeHmenu()==NULL)
	{
		m_menuSkin.CreatePopupMenu();
		for (int i=0; i<theColorSkinSize; i++)
		{
			m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SKIN_1+i,theColorSkinsString[i]);
		}
		m_menuSkin.AppendMenu(MF_SEPARATOR);
		m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SKIN_SETTING,_T("更多色调"));
		m_menuSkin.AppendMenu(MF_SEPARATOR);
		m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SKIN_GRADIENT,_T("渐变效果"));
	}
	bool bFindedChecked = false;
	for (int i=0; i<theColorSkinSize; i++)
	{
		if (theApp.GetMainColor()==theColorSkinsValue[i])
		{
			m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_1+i,MF_CHECKED);
			bFindedChecked = true; // **这里不能break退出
		}else
		{
			m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_1+i,MF_UNCHECKED);
		}
	}
	m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_SETTING,bFindedChecked?MF_UNCHECKED:MF_CHECKED);
	m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_GRADIENT,theApp.GetColorGradient()?MF_CHECKED:MF_UNCHECKED);
	CPoint point;
	GetCursorPos(&point);
	m_menuSkin.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
#endif
}

void CDlgMainFrame::OnOpenMain()
{
#ifndef USES_MIN_UI
	if (m_nShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_nShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
	}
#endif
	if (theApp.GetHideMainFrame() && theApp.GetAutoOpenSubId()>0)
	{
		theApp.ShowAutoOpenSubscribeFunc(SW_SHOW);
	}else
	{
		this->ShowWindow(SW_SHOWNORMAL);
		this->SetForegroundWindow();
#ifndef USES_MIN_UI
		if (this->m_bAppFrameShowed && m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
			m_pPanelSearch->ShowWindow(SW_SHOW);
#endif
	}
}

void CDlgMainFrame::OnRButtonUp(UINT nFlags, CPoint point)
{
	OnBnClickedButtonLinestate();
	__super::OnRButtonUp(nFlags, point);
}

void CDlgMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
#ifndef USES_MIN_UI
	OnBnClickedButtonMax();
#endif
	__super::OnLButtonDblClk(nFlags, point);
}

//#define DEFAULT_DLG_WIDTH	710+120
//#define DEFAULT_DLG_HEIGHT	588

void CDlgMainFrame::CreateFrameList(bool bShow)
{
	if (m_pDlgFrameList==NULL)
	{
		AFX_SWITCH_INSTANCED();
		CWnd * pParent = this;//CWnd::FromHandle(::GetDesktopWindow());
		//CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		m_pDlgFrameList = new CDlgFrameList(pParent);
		m_pDlgFrameList->Create(CDlgFrameList::IDD, pParent);
		SetChatWindowTopMost(theApp.m_bTopMost);
		//int nScreenWidth = 0;
		//int nScreenHeight = 0;
		//theApp.GetScreenSize(nScreenWidth, nScreenHeight);
		//CRect rect;
		//rect.left = (nScreenWidth-DEFAULT_DLG_WIDTH)/2;
		//rect.right = rect.left + DEFAULT_DLG_WIDTH;
		//rect.top = (nScreenHeight-DEFAULT_DLG_HEIGHT)/2;
		//rect.bottom = rect.top + DEFAULT_DLG_HEIGHT;
		//m_pDlgFrameList->MoveWindow(&rect);
	}
	if (bShow)
		m_pDlgFrameList->ShowWindow(SW_SHOW);
}

void CDlgMainFrame::OnBnClickedButtonMyCenter()
{
	OnMySetting();
}
void CDlgMainFrame::OnBnClickedButtonFileMgr()
{
	OnFileManager();
}

void CDlgMainFrame::OnBnClickedButtonMyShare()
{
	OnMyShare();
}

void CDlgMainFrame::OnBnClickedButtonMainFunc()
{
#ifndef USES_MIN_UI
	if (m_menuFunc.GetSafeHmenu()==NULL)
	{
		m_menuFunc.CreatePopupMenu();
	}else
	{
		while (m_menuFunc.GetMenuItemCount()>0)
			m_menuFunc.RemoveMenu(0,MF_BYPOSITION);
	}

	// 应用功能菜单
	theApp.ClearSubscribeSelectInfo();
	m_pSubscribeFuncList.clear();
#ifdef USES_EBCOM_TEST
	_variant_t pSubscribeFuncList = theEBClientCore->EB_GetSubscribeFuncList(EB_FUNC_LOCATION_MAINFRAME_BTN1);
	if (pSubscribeFuncList.vt!=VT_EMPTY && pSubscribeFuncList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pSubscribeFuncList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo;
			var.pdispVal->QueryInterface(__uuidof(IEB_SubscribeFuncInfo),(void**)&pSubscribeFuncInfo);
			if (pSubscribeFuncInfo == NULL) continue;
			m_pSubscribeFuncList.push_back(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
		}
	}
#else
	theEBAppClient.EB_GetSubscribeFuncList(EB_FUNC_LOCATION_MAINFRAME_BTN1,m_pSubscribeFuncList);
#endif
	if (!m_pSubscribeFuncList.empty())
	{
		for (size_t i=0;i<m_pSubscribeFuncList.size();i++)
		{
			const EB_SubscribeFuncInfo & pSubscribeFuncInfo = m_pSubscribeFuncList[i];
			m_menuFunc.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,pSubscribeFuncInfo.m_sFunctionName.c_str());
		}
	}else
	{
		m_menuFunc.AppendMenu(MF_BYCOMMAND,0,_T("没有集成应用"));
		m_menuFunc.EnableMenuItem(0,MF_GRAYED);
	}

	CPoint point;
	GetCursorPos(&point);
	m_menuFunc.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
#endif
}

HBRUSH CDlgMainFrame::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//if (nCtlColor==CTLCOLOR_EDIT && pWnd==&m_editSearch)
	//{
	//	float h,s,l;
	//	::RGBtoHSL(GetRValue(theApp.GetMainColor()), GetGValue(theApp.GetMainColor()), GetBValue(theApp.GetMainColor()),&h,&s,&l);
	//	const COLORREF color2 = ::HSLtoRGB(h, s, 0.85);	// 内框 RGB(196,234,247)
	//	pDC->SetBkColor( color2 );  // yellow
	//	HBRUSH m_hEditBrush = CreateSolidBrush( color2 );
	//	return m_hEditBrush;
	//}
	return CEbDialogBase::OnCtlColor(pDC, pWnd, nCtlColor);
}

//FILE * file = NULL;
LRESULT CDlgMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_ENDSESSION )
	{
		// **Windows关机消息，必须先发送退出命令
		//const time_t tNow = time(0);
		//FILE * file = fopen("c:\\WM_ENDSESSION.txt","a+t");
		//if (file!=NULL)
		//{
		//	char lpszBuffer[100];
		//	sprintf(lpszBuffer,"time=%lld,wp=%d, lp=%d\n",(eb::bigint)tNow,wParam,lParam);
		//	fwrite(lpszBuffer,1,strlen(lpszBuffer),file);
		//	fclose(file);
		//}

#ifdef USES_EBCOM_TEST
		theEBSetting.Release();
		if (theEBClientCore != NULL)
		{
			theEBClientCore->EB_Logout();
			theEBClientCore->EB_UnInit();
			theEBClientCore.Release();
		}
#else
		theEBAppClient.EB_Logout();
		theEBAppClient.EB_UnInit();
#endif
		OnExitApp();
	}else if (message==WM_POWERBROADCAST)
	{
		// 休眠事件
		//const time_t tNow = time(0);
		//FILE * file = fopen("c:\\WM_POWERBROADCAST.txt","a+t");
		//if (file!=NULL)
		//{
		//	char lpszBuffer[100];
		//	sprintf(lpszBuffer,"time=%lld,wp=%d, lp=%d\n",(eb::bigint)tNow,wParam,lParam);
		//	fwrite(lpszBuffer,1,strlen(lpszBuffer),file);
		//	fclose(file);
		//}
		//time=1416619237,wp=4, lp=0	// PBT_APMSUSPEND
		//time=1416619858,wp=18, lp=0	// PBT_APMRESUMEAUTOMATIC
		//time=1416619858,wp=7, lp=0	// PBT_APMRESUMESUSPEND

		switch (wParam)
		{
		case PBT_APMSUSPEND:
		case PBT_APMQUERYSUSPEND:
			{
				//系统即将休眠消息处理 
				//#ifdef USES_EBCOM_TEST
				//			//theEBSetting.Release();
				//			if (theEBClientCore != NULL)
				//			{
				//				theEBClientCore->EB_Logout();
				//				theEBClientCore->EB_UnInit();
				//				//theEBClientCore.Release();
				//			}
				//#else
				//			theEBAppClient.EB_Logout();
				//			theEBAppClient.EB_UnInit();
				//#endif
			}break;
		case PBT_APMRESUMEAUTOMATIC:
		case PBT_APMRESUMESUSPEND:
			{
				// 重新启动
				//SetTimer(TIMERID_RELOGIN,5000,NULL);
#ifdef USES_EBCOM_TEST
				if (theEBClientCore!=NULL)
				{
					theEBClientCore->EB_ReLogon();
				}
#else
				theEBAppClient.EB_ReLogon();
#endif
			}break;
		default:
			break;
		}
	}
	return __super::WindowProc(message, wParam, lParam);
}

void CDlgMainFrame::OnMove(int x, int y)
{
	__super::OnMove(x, y);

	if (!this->IsIconic() && (x>0 || y>0))
	{
		char lpszBuffer[24];
		if (GetPrivateProfileInt(_T("default"),_T("main-x"),0,theApp.GetUserSettingIniFile())!=x)
		{
			sprintf(lpszBuffer,"%d",x);
			WritePrivateProfileString(_T("default"),_T("main-x"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
		if (GetPrivateProfileInt(_T("default"),_T("main-y"),0,theApp.GetUserSettingIniFile())!=y)
		{
			sprintf(lpszBuffer,"%d",y);
			WritePrivateProfileString(_T("default"),_T("main-y"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
	}

	//if (m_pPanelBrowserNavigate!=0 && m_editSearch.GetSafeHwnd()!=0)
	//{
	//	CRect rect;
	//	m_editSearch.GetWindowRect(&rect);
	//	this->ScreenToClient(&rect);
	//	rect.left = 2;
	//	rect.right = rect.left + 18*2;
	//	rect.top = rect.top + 2;
	//	rect.bottom = rect.top + 18;
	//	this->ClientToScreen(&rect);
	//	m_pPanelBrowserNavigate->MoveWindow(&rect);
	//}
#ifndef USES_MIN_UI
	if (m_pPanelSearch!=0 && m_editSearch.GetSafeHwnd()!=0)
	{
		const int nlen = (18+2)*4;
		CRect rect;
		m_editSearch.GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		rect.left = rect.right - nlen-4;
		rect.right = rect.left + nlen;
		rect.top = rect.top + 2;
		rect.bottom = rect.top + 18;
		this->ClientToScreen(&rect);
		m_pPanelSearch->MoveWindow(&rect);
	}

	int nNeedHideWin = 0;
	if (x<=0 && y>0)
		nNeedHideWin = 1;		// left
	else if (x>0 && y<=0)
		nNeedHideWin = 2;		// top
	else
	{
		CRect rect;
		this->GetWindowRect(&rect);
		int nScreenWidth = 0;
		int nScreenHeight = 0;
		theApp.GetScreenSize(nScreenWidth, nScreenHeight);
		if (rect.right>=nScreenWidth && y>0)
			nNeedHideWin = 3;	// right
	}
	if (nNeedHideWin>0)
	{
		CRect rect;
		this->GetWindowRect(&rect);

		if (m_pDlgShrinkageBar==NULL)
		{
			CWnd* pParent = GetDesktopWindow();
			m_pDlgShrinkageBar = new CDlgShrinkageBar(pParent);
			m_pDlgShrinkageBar->Create(CDlgShrinkageBar::IDD,pParent);
			m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
			m_pDlgShrinkageBar->SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
		m_rectWin = rect;
		if (nNeedHideWin==1)
		{
			// left
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
			rect.left = rect.right-2;
		}
		m_pDlgShrinkageBar->MoveWindow(&rect);
		if (!m_nShrinkageWin)
		{
			m_nShrinkageWin = true;
			SetTimer(TIMERID_SHRINKAGE_WIN,100,NULL);
			SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}else if (nNeedHideWin==0 && m_nShrinkageWin && x>0 && y>0)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_nShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		this->ShowWindow(SW_SHOW);
		SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
#endif
}

void CDlgMainFrame::OnMoving(UINT fwSide, LPRECT pRect)
{
#ifndef USES_MIN_UI
	if (pRect!=NULL)
	{
		if (pRect->left<0 && pRect->top>0)
		{
			// 限制左移
			pRect->right -= pRect->left;
			pRect->left = 0;
		}else 
		{
			int nScreenWidth = 0;
			int nScreenHeight = 0;
			theApp.GetScreenSize(nScreenWidth, nScreenHeight);
			if (pRect->left>0 && pRect->right>nScreenWidth)
			{
				// 限制右移
				pRect->left -= (pRect->right-nScreenWidth);
				pRect->right = nScreenWidth;
			}
		}
	}
#endif
	__super::OnMoving(fwSide, pRect);

	// TODO: Add your message handler code here
}

LRESULT CDlgMainFrame::OnMessageReturnMainFrame(WPARAM wParam, LPARAM lParam)
{
	m_btnMyDepartment.ShowWindow(SW_SHOW);
	//m_btnMyContacts.ShowWindow(SW_SHOW);
	m_btnMySession.ShowWindow(SW_SHOW);
	m_btnMyEnterprise.ShowWindow(SW_SHOW);
	m_pDlgMyGroup->ShowWindow(m_btnMyDepartment.GetChecked()?SW_SHOW:SW_HIDE);
	m_pDlgMyContacts->ShowWindow(m_btnMyContacts.GetChecked()?SW_SHOW:SW_HIDE);
	m_pDlgMySession->ShowWindow(m_btnMySession.GetChecked()?SW_SHOW:SW_HIDE);
	m_pDlgMyEnterprise->ShowWindow(m_btnMyEnterprise.GetChecked()?SW_SHOW:SW_HIDE);
	m_btnSwitchFrame.ShowWindow(SW_SHOW);
	//m_btnSwitchFrame.ShowWindow(m_pDlgAppFrame->IsEmpty()?SW_HIDE:SW_SHOW);
	if (m_pDlgAppFrame!=NULL)
	{
		m_bAppFrameShowed = false;
		m_pDlgAppFrame->ShowWindow(SW_HIDE);
	}
#ifndef USES_MIN_UI
	this->m_editSearch.SetWindowText(_T(""));
	//if (m_btnGoBack.GetSafeHwnd()!=0)
	//	m_btnGoBack.ShowWindow(SW_HIDE);
	//if (m_btnGoForward.GetSafeHwnd()!=0)
	//	m_btnGoForward.ShowWindow(SW_HIDE);
	//if (m_pPanelBrowserNavigate!=0)
	//	m_pPanelBrowserNavigate->ShowWindow(SW_HIDE);
	if (m_pPanelSearch!=0)
		m_pPanelSearch->ShowWindow(SW_HIDE);
	if (!m_pDlgAppFrame->IsEmpty())
	{
		CRect rect;
		this->GetWindowRect(&rect);
		if (rect.Width()<LEN_DLG_WIDTH)
		{
			rect.right = rect.left + LEN_DLG_WIDTH;
			this->MoveWindow(&rect);
		}
	}
#endif
	return 0;
}

void CDlgMainFrame::OnBnClickedButtonSwitchFrame()
{
	// TODO: Add your control notification handler code here
//#ifndef USES_MIN_UI
	if (m_pDlgAppFrame==NULL || m_pDlgAppFrame->IsEmpty())
	{
		AddAppUrl("about:blank");
		//m_editSearch.SetFocus();	// ** 没用
		return;
	}

	if (m_pDlgAppFrame!=NULL && !m_pDlgAppFrame->IsEmpty())
	{
		m_bAppFrameShowed = true;
		m_pDlgAppFrame->SetCurrentFocus();
		m_pDlgAppFrame->ShowWindow(SW_SHOW);
		m_btnMyDepartment.ShowWindow(SW_HIDE);
		m_btnMyContacts.ShowWindow(SW_HIDE);
		m_btnMySession.ShowWindow(SW_HIDE);
		m_btnMyEnterprise.ShowWindow(SW_HIDE);
		m_pDlgMyGroup->ShowWindow(SW_HIDE);
		m_pDlgMyContacts->ShowWindow(SW_HIDE);
		m_pDlgMySession->ShowWindow(SW_HIDE);
		m_pDlgMyEnterprise->ShowWindow(SW_HIDE);
		m_btnSwitchFrame.ShowWindow(SW_HIDE);
#ifndef USES_MIN_UI
		m_treeSearch.ShowWindow(SW_HIDE);
#endif
	}
#ifndef USES_MIN_UI
	//if (m_btnGoBack.GetSafeHwnd()!=0)
	//	m_btnGoBack.ShowWindow(SW_SHOW);
	//if (m_btnGoForward.GetSafeHwnd()!=0)
	//	m_btnGoForward.ShowWindow(SW_SHOW);
	//if (m_pPanelBrowserNavigate!=0)
	//	m_pPanelBrowserNavigate->ShowWindow(SW_SHOW);
	if (m_pPanelSearch!=0)
		m_pPanelSearch->ShowWindow(SW_SHOW);
#endif
//#endif
}

void CDlgMainFrame::OnBnClickedButtonNewApp()
{
	if (!theApp.GetEntManagerurl().empty())
	{
		// eb/func/func.csp
		std::string sUrl = theApp.GetEntManagerurl();
		sUrl.append("&redirect_url=/eb/func/edit.csp");
		this->AddAppUrl(sUrl);
	}
}

void CDlgMainFrame::OnBnClickedButtonSwitchToolbar2()
{
#ifndef USES_MIN_UI
	if (m_bShowedToolbar)
	{
		this->SetToolTipText(IDC_BUTTON_SWITCH_TOOLBAR2,_T("展开工具栏"));
		m_btnSwitchToolbar.SetDrawTrianglePic(2,RGB(61,61,61),-1,-1,-1,10,5);
	}else
	{
		this->SetToolTipText(IDC_BUTTON_SWITCH_TOOLBAR2,_T("收缩工具栏"));
		m_btnSwitchToolbar.SetDrawTrianglePic(1,RGB(61,61,61),-1,-1,-1,12,6);
	}
	m_bShowedToolbar = !m_bShowedToolbar;
	this->Invalidate();
	CRect rect;
	this->GetClientRect(&rect);
	MoveSize(rect.Width(),rect.Height());
	m_btnSwitchToolbar.Invalidate();
#endif
}

LRESULT CDlgMainFrame::OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam)
{
	char * sUrl = (char*)wParam;
	if (sUrl==0 || strlen(sUrl)==0) return 0;
#ifndef USES_MIN_UI
	theSearchString = "";
	m_bCanSearch = false;
	m_editSearch.SetWindowText(sUrl);
	m_editSearch.SetFocus();
	m_editSearch.SetSel(0,-1);
#endif
	delete[] sUrl;
	return 0;
}

void CDlgMainFrame::OnNMClickTreeSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
#ifndef USES_MIN_UI
	CallItem(m_treeSearch.GetTrackItem(),true);
#endif
}

void CDlgMainFrame::OnBnClickedButtonGoback()
{
	// TODO: Add your control notification handler code here
}

void CDlgMainFrame::OnBnClickedButtonGoforward()
{
	// TODO: Add your control notification handler code here
}

void CDlgMainFrame::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CEbDialogBase::OnPaint();
		DrawInfo();
	}
}

void CDlgMainFrame::DrawInfo(void)
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);
	this->ClearBgDrawInfo();
	//if (theApp.GetColorGradient())
	//{
	//	this->AddBgDrawInfo(CEbBackDrawInfo(m_bShowedToolbar?120:38,theDefaultBgColorTitleLight1,true));
	//	//this->AddBgDrawInfo(CEbBackDrawInfo(62,theDefaultBgColorBgLight3,false));
	//	this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	//}else
	{
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(120,1.0,theApp.GetColorGradient()));
	}
	this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
//
//	Gdiplus::Graphics graphics(memDC.m_hDC);
//	const FontFamily fontFamily(theFontFamily.c_str());
//
//	if (m_bShowedToolbar)
//	{
//		USES_CONVERSION;
//		const Gdiplus::Font fontEbUserName(&fontFamily, 14, FontStyleRegular, UnitPixel);
//
//		const Gdiplus::Font fontEbTitle(&fontFamily, 13, FontStyleBold, UnitPixel);
//		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
//		//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
//		//graphics.DrawImage(theApp.m_imageEbIcon, Gdiplus::RectF(3,3,20,20));
//		//const Gdiplus::PointF pointTitle(25,7);
//		const Gdiplus::PointF pointTitle(10,10);
//#ifdef USES_EBCOM_TEST
//		CEBString sSettingEnterprise = theEBSetting->Enterprise.GetBSTR();
//		const CEBString sSettingVersion = theEBSetting->Version.GetBSTR();
//#else
//		CEBString sSettingEnterprise = theSetting.GetEnterprise();
//		const CEBString sSettingVersion = theSetting.GetVersion();
//#endif
//		//graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);	// 消除锯齿效果
//		if (theApp.GetLicenseUser() && !theApp.GetProductName().IsEmpty())
//		{
//			graphics.DrawString(A2W_ACP(theApp.GetProductName()),-1,&fontEbTitle,pointTitle,&brushEbTitle);
//		}else if (sSettingEnterprise.empty())
//			graphics.DrawString(L"恩布互联",-1,&fontEbTitle,pointTitle,&brushEbTitle);
//		else
//		{
//			if (!theApp.GetLicenseUser() && sSettingEnterprise.find("恩布")==std::string::npos)
//			{
//				sSettingEnterprise = "恩布互联";
//			}
//			graphics.DrawString(A2W_ACP(sSettingEnterprise.c_str()),-1,&fontEbTitle,pointTitle,&brushEbTitle);
//		}
//
//		// 写标题
//		//CFont pNewFontTitle;
//		//pNewFontTitle.CreatePointFont(115, _T("宋体"));//创建显示文本的字体
//		//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontTitle.m_hObject);
//		//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
//		//CString sOutText = _T("EB2015");//theSetting.GetEnterprise().c_str();//_T("POP-2013");
//		//TextOut(m_hdcMemory, 6, 5, sOutText, sOutText.GetLength());
//		//SelectObject(m_hdcMemory, pOldFond);
//
//		// MY IMG
//#ifdef USES_EBCOM_TEST
//		const CEBString sHeadFile = theEBClientCore->EB_GetMyDefaultMemberHeadFile().GetBSTR();
//		const CEBString sUserName = theEBClientCore->EB_UserName.GetBSTR();
//		const CEBString sOutDesc = theEBClientCore->EB_Description.GetBSTR();
//#else
//		const CEBString sHeadFile = theEBAppClient.EB_GetMyDefaultMemberHeadFile();
//		const CEBString sUserName = theEBAppClient.EB_GetUserName();;
//		const CEBString sOutDesc = theEBAppClient.EB_GetDescription();;
//#endif
//		Gdiplus::Image * pImage;
//		if (theApp.IsLogonVisitor())
//		{
//			pImage = theApp.m_imageDefaultVisitor->Clone();
//		}else
//		{
//			if (PathFileExists(sHeadFile.c_str()))
//			{
//				pImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(sHeadFile.c_str()));
//			}else 
//			{
//				pImage = theApp.m_imageDefaultMember->Clone();
//			}
//		}
//		graphics.DrawImage(pImage, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
//		//graphics.DrawImage(pImage, POS_ADIMG_LEFT, POS_ADIMG_TOP, POS_ADIMG_SIGE, POS_ADIMG_SIGE);
//		delete pImage;
//
//		// 写字
//		const int POS_LINESTATE_WIDTH = m_btnLineState.GetImgWidth();
//		Gdiplus::SolidBrush brushString(Gdiplus::Color::Black);
//		graphics.DrawString(A2W_ACP(sUserName.c_str()),-1,&fontEbUserName,Gdiplus::PointF(POS_LINESTATE_LEFT+POS_LINESTATE_WIDTH+1,POS_LINESTATE_TOP-2),&brushString);
//		// 
//		if (sOutDesc.empty())
//		{
//			const Gdiplus::Font fontEbDescription(&fontFamily, 13, FontStyleRegular, UnitPixel);
//			Gdiplus::SolidBrush brushDescription(Gdiplus::Color(200,128,128,128));
//			graphics.DrawString(L"编辑个性签名",-1,&fontEbDescription,Gdiplus::PointF(POS_LINESTATE_LEFT,POS_LINESTATE_TOP+22),&brushDescription);
//		}else
//		{
//			const Gdiplus::Font fontEbDescription(&fontFamily, 13, FontStyleBold, UnitPixel);
//			Gdiplus::SolidBrush brushDescription(Gdiplus::Color(200,255,255,255));
//			graphics.DrawString(A2W_ACP(sOutDesc.c_str()),-1,&fontEbDescription,Gdiplus::PointF(POS_LINESTATE_LEFT,POS_LINESTATE_TOP+22),&brushDescription);
//		}
//	}
//
//	const Gdiplus::Font fontEbBeta(&fontFamily, 12, FontStyleBold, UnitPixel);
//	const Gdiplus::PointF pointBeta(rectClient.Width()-72,rectClient.Height()-23);	// 60，某些XP环境下，长度不够
//	Gdiplus::Color colorVersion;
//	colorVersion.SetFromCOLORREF(theApp.GetBgColor1());
//	//colorVersion.SetFromCOLORREF(theApp.GetColorGradient()?theApp.GetHotColor2():theDefaultTextWhiteColor);
//	Gdiplus::SolidBrush brushEbVersion(colorVersion);
//	graphics.DrawString(L"1.17.0.352",-1,&fontEbBeta,pointBeta,&brushEbVersion);
}

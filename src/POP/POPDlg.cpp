// POPDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "POP.h"
#include "POPDlg.h"
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
#include "DlgMsgTip.h"
#pragma warning(disable:4018)

//const tstring theAppGroupResultName = _T("集成应用搜索：");
//const tstring theUrlGroupName("[* *]");
//const CString theUrlGroupResultName = _T("最新浏览记录：");

////////////////////////
//bool theWantExitApp = false;
int theYY = 0;
#ifdef USES_NEW_UI_1220
//const int const_border_left = 135;
#else
const int const_border_left = 0;
#endif

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
#define WM_TRAYICON_NOTIFICATION WM_USER+1

#define LIMIT_DLG_WIDTH	540

#ifdef USES_NEW_UI_1220
#ifdef USES_NEW_UI_160111
#define LEN_DLG_WIDTH	1180
//#define LEN_DLG_WIDTH (const_border_left+500)
#else
#define LEN_DLG_WIDTH (const_border_left+274)
#endif
#else
#define LEN_DLG_WIDTH (274+DEFAULT_RETURN_BTN_WIDTH)	// 12*2
#endif
#define LEN_DLG_HEIGHT 668	// 568

#ifdef USES_NEW_UI_160111
#define POS_ADIMG_LEFT (const_border_left+198)
//#define POS_ADIMG_LEFT (const_border_left+230+24+3)
#define POS_ADIMG_TOP 6
#define POS_ADIMG_SIGE 30	// 128
#else
#define POS_ADIMG_LEFT const_border_left+13
#define POS_ADIMG_TOP 43
#define POS_ADIMG_SIGE 60	// 128
#endif
//#define POS_STA_FIRST_Y (POS_ADIMG_TOP+POS_ADIMG_SIGE+10)	// 18
#ifdef USES_NEW_UI_160111
#define POS_LINESTATE_LEFT (POS_ADIMG_LEFT+POS_ADIMG_SIGE+3)
#define POS_LINESTATE_TOP (POS_ADIMG_TOP+12)
#else
#define POS_LINESTATE_LEFT (POS_ADIMG_LEFT+POS_ADIMG_SIGE+10)
#define POS_LINESTATE_TOP (POS_ADIMG_TOP-1)
#endif
//#define POS_LINESTATE_WIDTH 30

#ifdef USES_SUPPORT_UI_STYLE
#define UISTYLE_CHAT_POS_ADIMG_LEFT 15
#define UISTYLE_CHAT_POS_ADIMG_TOP 35
#define UISTYLE_CHAT_POS_ADIMG_SIGE 64
#define UISTYLE_CHAT_POS_LINESTATE_LEFT	(UISTYLE_CHAT_POS_ADIMG_LEFT+UISTYLE_CHAT_POS_ADIMG_SIGE+3)
#define UISTYLE_CHAT_POS_LINESTATE_TOP	(UISTYLE_CHAT_POS_ADIMG_TOP)
#define UISTYLE_CHAT_POS_STA_FIRST_Y		(UISTYLE_CHAT_POS_ADIMG_TOP+UISTYLE_CHAT_POS_ADIMG_SIGE+10)

#endif

//#define TIMERID_LOADINFO			0x121
//#define TIMERID_RELOGIN				0x122
#define TIMERID_LOGOUT				0x123
#define TIMERID_CHECK_CLOSE_DIALOG	0x124
#define TIMERID_CHECK_UPDATE		0x125
bool theUpdateResetTimer = false;
time_t theCheckNewVersionTime = 0;

#define TIMERID_NEW_VERSION				0x126
#define TIMERID_SHOW_EMOTION			0x127
#define TIMERID_CHECK_MF_SUBFUNCINFO	0x128
#define TIMERID_SHRINKAGE_WIN			0x129
#define TIMERID_AUTO_OPEN_SUBID			0x12A
#define TIMERID_CHECK_MOUSE_POS			0x130
//#define TIMERID_REFRESH_APPFRAME		0x131
#define TIMERID_ONE_SECOND				0x132
#define TIMERID_CHECK_SHRINKAGEBAR		0x133
#define TIMERID_CALL_SEARCH_TRACK		0x134
#define TIMERID_CALL_SEARCH_SEL_CHANGE	0x135
#define TIMERID_CALL_SEARCH_SELECTED	0x136
#define TIMERID_EDIT_SEARCH				0x137
#define TIMERID_LOAD_LOCAL_UNREAD_MSG	0x138
#define TIMERID_CLEAR_AUTOCALL_INFO		0x139
#define TIMERID_CALL_SEARCH_SEL_ALL		0x13A
#define TIMERID_SELECT_FIRST_SEARCH		0x13B
//#define TIMERID_ONUISTYLETYPEOFFICE		0x13C
#define TIMERID_APPFRAME_SETFOCUS			0x13D


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

//
//#ifdef USES_EBCOM
//BEGIN_DISPATCH_MAP(CPOPDlg, CEbDialogBase)
//	DISP_FUNCTION_ID(CPOPDlg,"onCallConnected2",25,onCallConnected2,VT_EMPTY,VTS_DISPATCH VTS_I4)
//	//DISP_FUNCTION_ID(CPOPDlg,"onGroupInfo2",41,onGroupInfo2,VT_EMPTY,VTS_DISPATCH VTS_BOOL)
//	//DISP_FUNCTION_ID(CPOPDlg,"onMemberInfo2",45,onMemberInfo2,VT_EMPTY,VTS_DISPATCH VTS_BOOL)
//	//DISP_FUNCTION_ID(CPOPDlg,"OnBytesRecv",0x4,OnBytesRecv,VT_EMPTY,VTS_I4)
//END_DISPATCH_MAP()
//
//BEGIN_INTERFACE_MAP(CPOPDlg, CEbDialogBase)
//	INTERFACE_PART(CPOPDlg, __uuidof(_IEBClientCoreEvents), Dispatch)
//END_INTERFACE_MAP()
//#endif

//BEGIN_CONNECTION_MAP(CPOPDlg, CEbDialogBase)
//    CONNECTION_PART(CPOPDlg, __uuidof(_IEBClientCoreEvents), SampleConnPt)
//END_CONNECTION_MAP()

// CPOPDlg 对话框

//void TrayIcon(UINT state, HWND hWnd, HICON resIcon,const char *message)
//{// begin TrayIcon
//
//}// end TrayIcon

#ifdef _DEBUG
int abc(int a)
{
	return a+1+20;
}
#endif


CPOPDlg::CPOPDlg(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CPOPDlg::IDD, pParent)
	, CFrameWndInfoProxy(this,true,this)
	//, m_sCallNumber(_T(""))
	, m_pDlgEditInfo(NULL)
	, m_pDlgMyEnterprise(NULL)
	, m_pDlgMyGroup(NULL)
	, m_pDlgMyContacts(NULL)
	, m_pDlgMySession(NULL)

{
#ifdef _DEBUG
	//int aa1 = abc(2);
	//int aa2 = abc(3);
	//int aa3 = abc(4);
#endif
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//m_bTimerToSwitchOnUIStyleTypeOffice = false;
	m_bShowedLeft = true;
	m_bShowedToolbar = true;

	m_pCurrentLabel = NULL;
	m_pOldCurrentLabel = NULL;
	m_pDlgFrameList = NULL;
	m_pDlgBroadcastMsg = NULL;
	m_pDlgAppFrame = NULL;
	m_nFrameType = CFrameWndInfo::FRAME_WND_MAIN_FRAME;
	//m_bAppFrameShowed = false;
	//m_bShowAppFrame = false;
	m_pDlgFileManager = NULL;
	m_bRemoveAppFuncIndex1 = false;
	m_bRemoveAppFuncIndex2 = false;
	m_nMainFrameMyCollectionMenuIndex = -1;
	m_nMenuOpenWorkFrameIndex = -1;
	m_nMenuShowHeadOnlyIndex = -1;
	m_nMenuIconOpenWorkFrameIndex = -1;
	m_bOpenOk = false;
	m_bShrinkageWin = false;
	//m_nShrinkageWin = 0;
	m_pDlgShrinkageBar = NULL;
	m_pPanelSearch = NULL;
	//m_pPanelBrowserNavigate = NULL;
	m_pViewContactInfo = NULL;
	m_pDlgMsgTip = NULL;
	m_bCanSearch = true;

	memset(&m_rectAdjustOld,0,sizeof(m_rectAdjustOld));
	m_pPanelStatus = NULL;
}

void CPOPDlg::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SWITCH_LEFT, m_btnSwitchLeft);
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
	DDX_Control(pDX, IDC_BUTTON_ENTERPRISE, m_btnMyEnterprise);
	DDX_Control(pDX, IDC_BUTTON_DEPARTMENT, m_btnMyDepartment);
	DDX_Control(pDX, IDC_BUTTON_CONTACT, m_btnMyContacts);
	DDX_Control(pDX, IDC_BUTTON_SESSION, m_btnMySession);
	DDX_Control(pDX, IDC_TREE_SEARCH, m_treeSearch);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
	DDX_Control(pDX, IDC_BUTTON_SWITCH_FRAME, m_btnSwitchFrame);
	DDX_Control(pDX, IDC_BUTTON_NEW_APP, m_btnNewApp);
}

BEGIN_MESSAGE_MAP(CPOPDlg, CEbDialogBase)
	ON_WM_SYSCOMMAND()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	
	ON_MESSAGE(EB_COMMAND_RETURN_MAINFRAME, OnMessageReturnMainFrame)
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
	ON_MESSAGE(EB_WM_ACCOUNT_INFO_CHANGE, OnMessageAccountInfoChange)
	//ON_MESSAGE(CR_WM_ENTER_ROOM, OnMessageEnterRoom)
	//ON_MESSAGE(CR_WM_EXIT_ROOM, OnMessageExitRoom)
	ON_MESSAGE(CR_WM_USER_ENTER_ROOM, OnMessageUserEnterRoom)
	ON_MESSAGE(CR_WM_USER_EXIT_ROOM, OnMessageUserExitRoom)
	ON_MESSAGE(CR_WM_USER_NOTIFY, OnMessageUserNotify)
	ON_MESSAGE(CR_WM_RECEIVE_RICH, OnMessageReceiveRich)
	ON_MESSAGE(CR_WM_SEND_RICH, OnMessageSendRich)
	ON_MESSAGE(CR_WM_MSG_RECEIPT, OnMessageMsgReceipt)
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

	ON_MESSAGE(EB_WM_USER_STATE_CHANGE, OnMessageMemberStateChange)
	ON_MESSAGE(EB_WM_USER_HEAD_CHANGE, OnMessageMemberHeadChange)
	ON_MESSAGE(EB_WM_CONTACT_HEAD_CHANGE, OnMessageContactHeadChange)
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
	ON_MESSAGE(EB_WM_GROUP_EDIT_RESPONSE, OnMessageGroupEditResponse)
	ON_MESSAGE(EB_WM_REMOVE_GROUP, OnMessageRemoveGroup)
	ON_MESSAGE(EB_WM_EXIT_GROUP, OnMessageExitGroup)
	ON_MESSAGE(EB_WM_REQUEST_ADD2GROUP, OnMessageRequestJoin2Group)
	ON_MESSAGE(EB_WM_INVITE_ADD2GROUP, OnMessageInviteJoin2Group)
	ON_MESSAGE(EB_WM_REJECT_ADD2GROUP, OnMessageRejectJoin2Group)
	ON_MESSAGE(EB_WM_MEMBER_INFO, OnMessageMemberInfo)
	//ON_MESSAGE(EB_WM_MEMBER_DELETE, OnMessageMemberDelete)
	ON_MESSAGE(EB_WM_MEMBER_EDIT_RESPONSE, OnMessageMemberEditResponse)
	ON_MESSAGE(EB_WM_REQUEST_ADDCONTACT, OnMessageRequestAddContact)
	ON_MESSAGE(EB_WM_REJECT_ADDCONTACT, OnMessageRejectAddContact)
	ON_MESSAGE(EB_WM_ACCEPT_ADDCONTACT, OnMessageAcceptAddContact)

	//ON_MESSAGE(EB_WM_APP_MSG, OnMessageAppMsgInfo)

	ON_MESSAGE(EB_WM_RESOURCE_INFO, OnMessageResourceInfo)
	ON_MESSAGE(EB_WM_RESOURCE_DELETE, OnMessageDeleteResource)
	ON_MESSAGE(EB_WM_RESOURCE_MOVE, OnMessageMoveResource)
	ON_MESSAGE(EB_COMMAND_FILE_MANAGER, OnMessageFileManager)

	ON_MESSAGE(WM_HOTKEY, OnMessageHotKey)

	//ON_MESSAGE(POP_WM_WINDOW_SIZE, OnMessageWindowResize)
	//ON_CBN_SELCHANGE(IDC_COMBO_NUMBERS, &CPOPDlg::OnCbnSelchangeComboNumbers)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CONTACT, &CPOPDlg::OnBnClickedButtonContact)
	ON_BN_CLICKED(IDC_BUTTON_ENTERPRISE, &CPOPDlg::OnBnClickedButtonEnterprise)
	ON_BN_CLICKED(IDC_BUTTON_DEPARTMENT, &CPOPDlg::OnBnClickedButtonDepartment)
	ON_BN_CLICKED(IDC_BUTTON_MAX2, &CPOPDlg::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CPOPDlg::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CPOPDlg::OnBnClickedButtonClose)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CPOPDlg::OnEnChangeEditSearch)
	//ON_MESSAGE(WM_ITEM_SEL_CHANGED, OnTreeItemSelChange)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDoubleClick)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
	ON_MESSAGE(WM_TRAYICON_NOTIFICATION, OnIconNotification)

	ON_MESSAGE(EB_COMMAND_SEARCH_SET_FOCUS_SEL, OnMsgSearchSetFocusSel)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MIN, OnFrameWndMin)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MAX, OnFrameWndMax)
	ON_MESSAGE(EB_COMMAND_RAME_WND_CLOSE, OnFrameWndClose)
	ON_MESSAGE(EB_COMMAND_RAME_WND_COUNT, OnFrameWndCount)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MOVE_UP, OnMessageMoveUp)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MOVE_DOWN, OnMessageMoveDown)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MOVE_OFFSET, OnMessageMoveOffset)
	ON_MESSAGE(EB_COMMAND_RAME_WND_ADJUST_WIDTH, OnMessageAdjustWidth)
	//ON_MESSAGE(EB_COMMAND_REFRESH_URL, OnMsgRefreshUrl)
	ON_MESSAGE(EB_COMMAND_REFRESH_OR_STOP_URL, OnMsgRefreshOrStopUrl)
	ON_MESSAGE(EB_COMMAND_REDRAW_APPFRAME, OnMsgRedrawAppFrame)
	ON_MESSAGE(EB_COMMAND_GO_BACK, OnMsgGoBack)
	ON_MESSAGE(EB_COMMAND_GO_FORWARD, OnMsgGoForward)
	ON_MESSAGE(EB_COMMAND_CHANGE_BROWSER_TYPE, OnMsgChangeBrowserType)
	ON_MESSAGE(EB_COMMAND_QUERY_CAN_SAVE_HISTORY, OnMsgQueryCanSaveHistory)
	ON_MESSAGE(EB_COMMAND_QUERY_BROWSER_TYPE, OnMsgQueryBrowserType)
	ON_MESSAGE(EB_COMMAND_SAVE_HISTORY, OnMsgSaveHistory)
	//ON_MESSAGE(EB_COMMAND_OPEN_SUBID, OnMsgOpenSubId)
	ON_MESSAGE(EB_COMMAND_OPEN_APP_URL, OnMsgOpenAppUrl)
	//ON_MESSAGE(EB_COMMAND_SHELLEXECUTE, OnMsgShellExecuteOpen)
	ON_MESSAGE(EB_COMMAND_EXIT_APP, OnMsgExitApp)
	ON_MESSAGE(EB_COMMAND_LOGOUT, OnMsgLogout)
	ON_MESSAGE(EB_COMMAND_SHOWHIDE_MAIN, OnMsgShowHideMainFrame)
	ON_COMMAND(EB_COMMAND_EXIT_APP, &CPOPDlg::OnExitApp)
	ON_COMMAND(EB_COMMAND_LOGOUT, &CPOPDlg::OnLogout)
	ON_BN_CLICKED(IDC_BUTTON_LINESTATE, &CPOPDlg::OnBnClickedButtonLinestate)
	ON_COMMAND(EB_COMMAND_STATE_ONLINE, &CPOPDlg::OnStateOnline)
	ON_COMMAND(EB_COMMAND_STATE_AWAY, &CPOPDlg::OnStateAway)
	ON_COMMAND(EB_COMMAND_STATE_BUSY, &CPOPDlg::OnStateBusy)
	ON_COMMAND(EB_COMMAND_AUTO_LOGIN, &CPOPDlg::OnAutoLogin)
	ON_WM_MOUSEMOVE()
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, &CPOPDlg::OnEnKillfocusEditDescription)
	ON_BN_CLICKED(IDC_BUTTON_SESSION, &CPOPDlg::OnBnClickedButtonSession)
	ON_WM_LBUTTONDOWN()
	//ON_COMMAND(EB_COMMAND_MY_SETTING, &CPOPDlg::OnMySetting)
	//ON_COMMAND(EB_COMMAND_FILE_MANAGER, &CPOPDlg::OnFileManager)
	ON_COMMAND(EB_COMMAND_MY_SHARE, &CPOPDlg::OnMyShare)
	ON_COMMAND(EB_COMMAND_MY_COLLECTION, &CPOPDlg::OnMyCollection)
	ON_BN_CLICKED(IDC_BUTTON_SKIN2, &CPOPDlg::OnBnClickedButtonSkin2)
	ON_COMMAND_RANGE(EB_COMMAND_SKIN_SETTING,EB_COMMAND_SKIN_2,&OnSkinSelect)
	ON_COMMAND(EB_COMMAND_SKIN_FLAT, &OnSkinFlat)
	ON_COMMAND(EB_COMMAND_SKIN_GRADIENT, &OnSkinGradient)
	ON_COMMAND(EB_COMMAND_BROWSER_TYPE_CEF, &OnBrowserTypeCef)
	ON_COMMAND(EB_COMMAND_BROWSER_TYPE_IE, &OnBrowserTypeIe)
	ON_COMMAND(EB_COMMAND_UISTYLE_TYPE_OFFIACE, &OnUIStyleTypeOffice)
	ON_COMMAND(EB_COMMAND_UISTYLE_TYPE_CHAT, &OnUIStyleTypeChat)
	ON_COMMAND(EB_COMMAND_CHECK_NEW_VERSION, &OnSettingCheckNewVersion)
	ON_COMMAND(EB_COMMAND_OPEN_MAIN, &CPOPDlg::OnOpenMain)
	ON_COMMAND(EB_COMMAND_SHOWHEAD_ONLY, OnShowHeadOnly)
	ON_COMMAND(EB_COMMAND_OPEN_WORKFRAME, &CPOPDlg::OnWorkFrame)
	ON_MESSAGE(EB_COMMAND_OPEN_WORKFRAME, OnMsgWorkFrame)
	ON_COMMAND_RANGE(EB_COMMAND_MY_EMPLOYEE,EB_COMMAND_MY_EMPLOYEE+0x20,OnMyEmployeeInfo)
	ON_COMMAND_RANGE(EB_COMMAND_SUBSCRIBE_FUNC,EB_COMMAND_SUBSCRIBE_FUNC+0x20,OnSubscribeFunc)
	ON_COMMAND_RANGE(EB_COMMAND_MAINFRAME_FUNC,EB_COMMAND_MAINFRAME_FUNC+0x20,OnMainFrameFunc)

	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON_MY_CENTER, &CPOPDlg::OnBnClickedButtonMyCenter)
	ON_BN_CLICKED(IDC_BUTTON_FILE_MGR, &CPOPDlg::OnBnClickedButtonFileMgr)
	ON_BN_CLICKED(IDC_BUTTON_MY_SHARE, &CPOPDlg::OnBnClickedButtonMyShare)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_FUNC, &CPOPDlg::OnBnClickedButtonMainFunc)
	ON_WM_MOVE()
	//ON_WM_MOVING()
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_FRAME, &CPOPDlg::OnBnClickedButtonSwitchFrame)
	ON_BN_CLICKED(IDC_BUTTON_NEW_APP, &CPOPDlg::OnBnClickedButtonNewApp)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_TOOLBAR2, &CPOPDlg::OnBnClickedButtonSwitchToolbar2)
	ON_MESSAGE(EB_COMMAND_CHANGE_APP_URL, OnMsgChangeAppUrl)
	ON_MESSAGE(EB_COMMAND_SHOW_REFRESH_OR_STOP, OnMsgShowRefreshOrStop)
	ON_MESSAGE(EB_COMMAND_CLEAR_SUBID_UNREAD_MSG, OnMsgClearSubIdUnReadmsg)
	ON_MESSAGE(EB_COMMAND_DOWNLOAD_RESOURCE, OnMsgDownloadResource)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SEARCH, &CPOPDlg::OnNMClickTreeSearch)
	ON_BN_CLICKED(IDC_BUTTON_GOBACK, &CPOPDlg::OnBnClickedButtonGoback)
	ON_BN_CLICKED(IDC_BUTTON_GOFORWARD, &CPOPDlg::OnBnClickedButtonGoforward)
	ON_COMMAND_RANGE(FRAME_BTN_ID_MIN, FRAME_BTN_ID_MAX, OnFrameBtnClicked)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_LEFT, &CPOPDlg::OnBnClickedButtonSwitchLeft)
	ON_WM_ACTIVATEAPP()
	//ON_EN_SETFOCUS(IDC_EDIT_SEARCH, &CPOPDlg::OnEnSetfocusEditSearch)
	END_MESSAGE_MAP()

// CPOPDlg 消息处理程序
DWORD dwAdvise = 0;
BOOL CALLBACK MyEnumFonts(CONST LOGFONTW* lplf, CONST TEXTMETRICW *lptm,DWORD dwType,LPARAM lparam)
{
	const wchar_t * lpszFine = (const wchar_t*)lparam;
	const std::wstring tempFontName(lplf->lfFaceName);
	if(tempFontName.find(lpszFine)!=std::wstring::npos)
	{
		theFontFamily = lpszFine;
		USES_CONVERSION;
		theFontFace = W2T(theFontFamily.c_str());
		return false;
	}
	return true;
}

void CPOPDlg::SetCtrlColor(void)
{
	if (m_pViewContactInfo!=NULL && m_pViewContactInfo->GetSafeHwnd()!=NULL)
		m_pViewContactInfo->SetCtrlColor();

	m_btnSkin.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnMin.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnMax.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);

#ifdef USES_NEW_UI_160111
	const int const_Tab_Width	= m_btnMyDepartment.GetImgWidth();
	const int nLineWidth = const_Tab_Width-6;
	m_btnMyDepartment.SetDrawLine(2,nLineWidth,2,-1,theApp.GetHotColor(),theApp.GetMainColor(),-1,1,theDefaultFlatLineColor);
	m_btnMyContacts.SetDrawLine(2,nLineWidth,2,-1,theApp.GetHotColor(),theApp.GetMainColor(),-1,1,theDefaultFlatLineColor);
	m_btnMySession.SetDrawLine(2,nLineWidth,2,-1,theApp.GetHotColor(),theApp.GetMainColor(),-1,1,theDefaultFlatLineColor);
	m_btnMyEnterprise.SetDrawLine(2,nLineWidth,2,-1,theApp.GetHotColor(),theApp.GetMainColor(),-1,1,theDefaultFlatLineColor);
#endif
	//m_btnMyCenter.SetDrawLine(5,1,0,-1,theApp.GetHotColor2());
	//m_btnFileMgr.SetDrawLine(5,1,0,-1,theApp.GetHotColor2());
	//m_btnMyShare.SetDrawLine(5,1,0,-1,theApp.GetHotColor2());
	//m_btnMainFunc.SetDrawLine(5,1,0,-1,theApp.GetHotColor2());

	//m_btnSwitchFrame.SetDrawPanel(true,theApp.GetMainColor(),RGB(237,28,36),theApp.GetBgColor3());
#ifdef USES_NEW_UI_1220
	CFrameWndInfoProxy::SetCtrlColor(true);
#ifdef USES_NEW_UI_160111
	m_editSearch.SetRectangleColor(theDefaultFlatLineColor,theApp.GetMainColor());
#else
	m_editSearch.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
#endif
#else
	m_editSearch.SetRectangleColor(theApp.GetHotColor2(),RGB(255,255,255));
#endif
	if (m_pDlgMyGroup!=NULL)
		m_pDlgMyGroup->SetCtrlColor();
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
	//if (m_pDlgBroadcastMsg!=NULL)
	//	m_pDlgBroadcastMsg->SetCt
	if (m_pDlgAppFrame!=NULL)
		m_pDlgAppFrame->SetCtrlColor();
	if (m_pDlgMsgTip!=NULL)
		m_pDlgMsgTip->SetCtrlColor();

	{
		BoostReadLock rdlock(theApp.m_pResourceMgr.mutex());
		CLockMap<eb::bigint, CDlgResourceMgr*>::const_iterator pIterMsgRecord = theApp.m_pResourceMgr.begin();
		for (; pIterMsgRecord!=theApp.m_pResourceMgr.end(); pIterMsgRecord++)
		{
			CDlgResourceMgr * pDlgDialog = pIterMsgRecord->second;
			pDlgDialog->SetCtrlColor();
		}
	}
	{
		BoostReadLock rdlock(theApp.m_pMsgRecord.mutex());
		CLockMap<eb::bigint, CDlgMsgRecord*>::const_iterator pIterMsgRecord = theApp.m_pMsgRecord.begin();
		for (; pIterMsgRecord!=theApp.m_pMsgRecord.end(); pIterMsgRecord++)
		{
			CDlgMsgRecord * pDlgDialog = pIterMsgRecord->second;
			pDlgDialog->SetCtrlColor();
		}
	}

}

void CPOPDlg::SetWindowTextAndTrayInfo(bool bFirstTrayIcon)
{
#ifdef USES_EBCOM_TEST
	EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
	const CEBString sUserName = theEBClientCore->EB_UserName.GetBSTR();
	const CEBString sSettingEnterprise = theEBSetting->Enterprise.GetBSTR();
#else
	EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
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

	// 系统托盘
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
	if (bFirstTrayIcon)
		Shell_NotifyIcon(NIM_ADD, &m_trayIconData);
	else
		Shell_NotifyIcon(NIM_MODIFY, &m_trayIconData);
}
BOOL CPOPDlg::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	//ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);

	CPaintDC dc((CWnd*)this);
  ::EnumFontsW(dc.m_hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily1.c_str());	// 
  ::EnumFontsW(dc.m_hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily0.c_str());

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	RegisterHotKey(this->m_hWnd, 0x2013, MOD_SHIFT|MOD_ALT, 'A'); 
	RegisterHotKey(this->m_hWnd, 0x2014, MOD_SHIFT|MOD_ALT, 'Z'); 
	RegisterHotKey(this->m_hWnd, 0x2015, MOD_SHIFT|MOD_ALT, 'T'); 
	//RegisterHotKey(this->m_hWnd, 0x2013, MOD_CONTROL|MOD_ALT, 'P'); 

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标
	if (GetPrivateProfileInt(_T("default"),_T("show-left"),1,theApp.GetUserSettingIniFile())==0)
	{
		m_bShowedLeft = false;
		CFrameWndInfoProxy::SetShowHideLeft(m_bShowedLeft);
	}
	SetTransparentType(CEbDialogBase::TT_STATIC);
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		SetSplitterBorder(UISTYLE_CHAT_DEFAULT_DLG_WIDTH);
	else
#endif
		SetSplitterBorder(LIMIT_DLG_WIDTH);

	DisableMoveBottom(TRUE);
	this->EnableToolTips();
	this->SetToolTipText(IDC_BUTTON_SWITCH_LEFT,m_bShowedLeft?_T("收起面板"):_T("展开面板"));
	this->SetToolTipText(IDC_BUTTON_SWITCH_TOOLBAR2,_T("收缩工具栏"));
	this->SetToolTipText(IDC_BUTTON_SKIN2,_T("设置"));
	this->SetToolTipText(IDC_BUTTON_MIN,_T("最小化"));
	this->SetToolTipText(IDC_BUTTON_MAX2,_T("最大化"));
	this->SetToolTipText(IDC_BUTTON_CLOSE,_T("关闭"));
	this->SetToolTipText(IDC_BUTTON_LINESTATE,_T("在线状况"));
	this->SetToolTipText(IDC_BUTTON_MY_CENTER,_T("个人中心"));
	this->SetToolTipText(IDC_BUTTON_FILE_MGR,_T("文件管理"));
	this->SetToolTipText(IDC_BUTTON_MY_SHARE,_T("个人云盘"));
	this->SetToolTipText(IDC_BUTTON_MAIN_FUNC,_T("集成应用"));
	this->SetToolTipText(IDC_EDIT_DESCRIPTION,_T("点击编辑个性签名"));
	this->SetToolTipText(IDC_EDIT_SEARCH,_T("搜索用户、帐号、联系人和浏览记录，回车打开；输入/搜索集成应用"));
	//this->SetToolTipText(IDC_BUTTON_CALLUSER,_T("呼叫用户"));
	this->SetToolTipText(IDC_BUTTON_DEPARTMENT,_T("我的部门（群和讨论组）"));
	this->SetToolTipText(IDC_BUTTON_CONTACT,_T("联系人"));
	this->SetToolTipText(IDC_BUTTON_SESSION,_T("最近会话"));
	this->SetToolTipText(IDC_BUTTON_ENTERPRISE,_T("公司组织结构"));

	m_editSearch.SetPromptColor(RGB(234,234,234));
	m_editSearch.SetTextColor(theDefaultTextBlackColor);
	m_editSearch.SetPromptText(_T("搜索用户、浏览记录、集成应用(/)"));
	//m_editSearch.SetPromptText(_T("搜索用户：回车开始聊天；输入网址：回车打开网站"));
	//m_editSearch.SetFocusSelAll(TRUE);

	m_pPanelStatus = new CPanelText(this,16);
	m_pPanelStatus->Create(CPanelText::IDD,this);
	m_pPanelStatus->ShowWindow(SW_HIDE);
	//m_pPanelStatus->SetParent(CWnd::FromHandle(::GetDesktopWindow()));
	m_pPanelStatus->SetBgColor(theDefaultFlatLineColor);
	//m_pPanelStatus->SetCircle();

	m_btnSwitchLeft.SetWindowText(_T(""));
	m_btnSwitchLeft.SetDrawTrianglePic((m_bShowedLeft?3:4),theDefaultFlatLineColor,theDefaultFlatBlackText2Color,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	m_btnSwitchLeft.ShowWindow(SW_HIDE);
	//m_btnSwitchLeft.ShowWindow(SW_SHOW);

	m_btnSwitchToolbar.SetWindowText(_T(""));
#ifdef USES_NEW_UI_1220
	m_btnSwitchToolbar.SetDrawTrianglePic(1,theDefaultFlatLineColor,-1,-1,-1,12,6);
#else
	m_btnSwitchToolbar.SetDrawTrianglePic(1,theDefaultBtnWhiteColor,-1,-1,-1,12,6);
#endif
	//m_btnSkin.Load(IDB_PNG_BTN_SKIN);
	m_btnSkin.SetDrawTrianglePic(2,theDefaultBtnWhiteColor,-1,-1,-1,10,5);
	m_btnSkin.SetDrawPanelRgn(false);
	m_btnMin.SetAutoSize(false);
	m_btnMin.SetDrawToolButtonPic(1,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnMin.SetDrawPanelRgn(false);
	m_btnMax.SetAutoSize(false);
	m_btnMax.SetDrawToolButtonPic(2,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnMax.SetDrawPanelRgn(false);
	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
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
#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		m_btnMyCenter.Load(IDB_PNG_BTN_MY_CENTER_UISTYLE1,24);
		m_btnMyCenter.SetDrawLine(5,1,0,-1,theDefaultBtnWhiteColor);
	}else
#endif
	{
		m_btnMyCenter.Load(IDB_PNG_BTN_MY_CENTER,24);
		m_btnMyCenter.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
	}
	m_btnMyCenter.SetDrawPanelRgn(false);
	m_btnMyCenter.SetWindowText(_T(""));
#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		m_btnFileMgr.Load(IDB_PNG_BTN_FILE_MGR_UISTYLE1,24);
		m_btnFileMgr.SetDrawLine(5,1,0,-1,theDefaultBtnWhiteColor);
	}else
#endif
	{
		m_btnFileMgr.Load(IDB_PNG_BTN_FILE_MGR,24);
		m_btnFileMgr.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
	}
	m_btnFileMgr.SetDrawPanelRgn(false);
	m_btnFileMgr.SetWindowText(_T(""));
#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		m_btnMyShare.Load(IDB_PNG_BTN_MY_SHARE_UISTYLE1,24);
		m_btnMyShare.SetDrawLine(5,1,0,-1,theDefaultBtnWhiteColor);
	}else
#endif
	{
		m_btnMyShare.Load(IDB_PNG_BTN_MY_SHARE,24);
		m_btnMyShare.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
	}
	m_btnMyShare.SetDrawPanelRgn(false);
	m_btnMyShare.SetWindowText(_T(""));

	if (theApp.GetDisableUserCloudDrive() || theApp.IsLogonVisitor())
	{
		m_btnMyShare.ShowWindow(SW_HIDE);
	}
#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		m_btnMainFunc.Load(IDB_PNG_BTN_MAIN_FUNC_UISTYLE1,24);
		m_btnMainFunc.SetDrawLine(5,1,0,-1,theDefaultBtnWhiteColor);
	}else
#endif
	{
		m_btnMainFunc.Load(IDB_PNG_BTN_MAIN_FUNC,24);
		m_btnMainFunc.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
	}
//	m_btnMainFunc.Load(IDB_PNG_BTN_MAIN_FUNC,24);
//#ifdef USES_NEW_UI_160111
//	m_btnMainFunc.SetDrawLine(5,1,0,-1,theDefaultBtnWhiteColor);
//#else
//	m_btnMainFunc.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
//#endif
	m_btnMainFunc.SetDrawPanelRgn(false);
	m_btnMainFunc.SetWindowText(_T(""));
	m_btnGoBack.Load(IDB_PNG_BTN_GOBACK72X18);
	m_btnGoBack.SetWindowText(_T(""));
	m_btnGoBack.SetToolTipText(_T("转到上一页"));
	m_btnGoForward.Load(IDB_PNG_BTN_GOFORWARD72X18);
	m_btnGoForward.SetWindowText(_T(""));
	m_btnGoForward.SetToolTipText(_T("转到下一页"));

#ifdef USES_NEW_UI_160111
	m_btnSwitchToolbar.ShowWindow(SW_HIDE);
	//m_btnMyCenter.ShowWindow(SW_HIDE);
	//m_btnFileMgr.ShowWindow(SW_HIDE);
	//m_btnMyShare.ShowWindow(SW_HIDE);
	m_btnSwitchFrame.ShowWindow(SW_HIDE);
	//m_btnLineState.ShowWindow(SW_HIDE);
#endif

	m_btnMainFunc.ShowWindow(SW_SHOW);
	//m_btnMainFunc.ShowWindow(SW_HIDE);	// 暂时隐藏该功能按钮

	m_pPanelSearch = new CPanelSearch(this);
	m_pPanelSearch->Create(CPanelSearch::IDD,this);
#ifdef USES_SUPPORT_UI_STYLE
	m_pPanelSearch->SetSearchEditShow(true);
#endif
	//m_pPanelSearch->ShowWindow(SW_SHOW);
	//m_pPanelBrowserNavigate = new CPanelBrowserNavigate(this);
	//m_pPanelBrowserNavigate->Create(CPanelBrowserNavigate::IDD,this);

	BuildHeadRect();

	m_editDescription.EnableWindow(TRUE);
	m_editDescription.SetReadOnly(TRUE);
	m_editDescription.ShowWindow(SW_HIDE);
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
#ifdef USES_NEW_UI_1220
	m_btnMyDepartment.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color);
	m_btnMyContacts.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color);
	m_btnMySession.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color);
	m_btnMyEnterprise.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color);
#endif

	m_btnMyDepartment.SetDrawPanelRgn(false);
	m_btnMyDepartment.SetWindowText(_T(""));
	m_btnMyDepartment.SetAutoSize(false);
	m_btnMyDepartment.Load(IDB_PNG_TAB_DEP);
	m_btnMyDepartment.Invalidate();
	m_btnMyContacts.SetDrawPanelRgn(false);
	m_btnMyContacts.SetWindowText(_T(""));
	m_btnMyContacts.SetAutoSize(false);
	m_btnMyContacts.Load(IDB_PNG_TAB_CONTACT);
	m_btnMySession.SetDrawPanelRgn(false);
	m_btnMySession.SetWindowText(_T(""));
	m_btnMySession.SetAutoSize(false);
	m_btnMySession.Load(IDB_PNG_TAB_SES);
	m_btnMyEnterprise.SetDrawPanelRgn(false);
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
	SetCtrlColor();

	m_btnSwitchFrame.SetDrawPanelRgn(false);
	m_btnSwitchFrame.SetWindowText(_T(""));
	m_btnSwitchFrame.SetToolTipText(_T("切换到集成应用工作台（网页浏览器）"));
#ifdef USES_NEW_UI_1220
	m_btnSwitchFrame.SetDrawTrianglePic(3,theDefaultFlatLineColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	m_btnSwitchFrame.ShowWindow(SW_HIDE);
#else
	m_btnSwitchFrame.SetDrawTrianglePic(3,theDefaultBtnWhiteColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	m_btnSwitchFrame.ShowWindow(SW_SHOW);
#endif
	m_btnNewApp.SetDrawPanelRgn(false);
	m_btnNewApp.SetWindowText(_T(""));
	m_btnNewApp.SetToolTipText(_T("添加应用"));
#ifdef USES_NEW_UI_1220
	m_btnNewApp.SetDrawNewPic(true,theDefaultFlatLineColor,-1,-1,-1,2,11);
#else
	m_btnNewApp.SetDrawNewPic(true,RGB(64,64,64),-1,-1,-1,2,11);
#endif
	m_btnNewApp.ShowWindow((theApp.GetSystemAccountFlag()==1 && !theApp.GetEntManagerurl().empty())?SW_SHOW:SW_HIDE);

	m_btnSearchTrackDel.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSearch, 0xffff);
	m_btnSearchTrackDel.SetAutoSize(false);
	m_btnSearchTrackDel.Load(IDB_PNG_HOT_DELETE);
	m_btnSearchTrackDel.SetToolTipText(_T("删除历史记录"));

	m_btnSearchTrackCall.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSearch, 0xffff);
	m_btnSearchTrackCall.SetAutoSize(false);
	m_btnSearchTrackCall.Load(IDB_PNG_HOT_CALL);
	m_btnSearchTrackCall.SetToolTipText(_T("打开会话"));
	m_treeSearch.SetItemHeight(40);
	m_treeSearch.SetIconSize(32,32);
	m_treeSearch.SetCallback(&theApp);
	m_treeSearch.ShowWindow(SW_HIDE);
#ifdef USES_NEW_UI_160111
	m_treeSearch.SetRectangle(theDefaultFlatLineColor);
#endif

	CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
	m_pDlgBroadcastMsg = new CDlgFuncWindow(pParent,false);
	//m_pDlgBroadcastMsg->m_nFuncBrowserType = m_nFuncBrowserType;
	m_pDlgBroadcastMsg->m_nMsgType = EBC_MSG_TYPE_BC_MSG;
	m_pDlgBroadcastMsg->m_bDisableContextMenu = true;
	m_pDlgBroadcastMsg->m_bBroadcastMsg = true;
	m_pDlgBroadcastMsg->m_bOpenNewClose = true;
	m_pDlgBroadcastMsg->m_nWidth = 250;
	m_pDlgBroadcastMsg->m_nHeight = 180;
	m_pDlgBroadcastMsg->Create(CDlgFuncWindow::IDD,pParent);
	m_pDlgBroadcastMsg->ShowWindow(SW_HIDE);
	m_pDlgMsgTip = new CDlgMsgTip(pParent);
	if (!m_pDlgMsgTip->Create(CDlgMsgTip::IDD,pParent))
	{
		delete m_pDlgMsgTip;
		m_pDlgMsgTip = NULL;
		return FALSE;
	}
	m_pDlgMsgTip->SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

#ifdef USES_EBCOM_TEST
	const CEBString sOutDesc = theEBClientCore->EB_Description.GetBSTR();
#else
	const tstring sOutDesc = theEBAppClient.EB_GetDescription();
#endif
	this->GetDlgItem(IDC_EDIT_DESCRIPTION)->SetWindowText(sOutDesc.c_str());

#ifdef USES_EBCOM_TEST
	(((CEBDispatch*)(CEBCoreEventsSink*)this))->AddRef();	// 增加计数，避免删除自己
	//theEBClientCore->EB_SetCallBack((IDispatch*)(CEBCoreEventsSink*)this);
	((CEBSearchEventsSink*)this)->AddRef();	// 增加计数，避免删除自己
	((CEBCoreEventsSink*)this)->m_hFireWnd = this->GetSafeHwnd();
	((CEBCoreEventsSink*)this)->DispEventAdvise(theEBClientCore);
#else
	theEBAppClient.EB_SetMsgHwnd(this->GetSafeHwnd());
#endif
	this->PostMessage(EB_WM_LOGON_SUCCESS,0,0);
	//OnMessageLogonSuccess(0,0);

	//theBtnImage.LoadResource(IDB_BITMAP_BTN_6821);
	//theBtnImage.Cut(3, pBtnBitmap);
	//m_btnCallUser.SetSkin(pBtnBitmap[0], pBtnBitmap[2], pBtnBitmap[1]);
	SetWindowTextAndTrayInfo(true);
//#ifdef USES_EBCOM_TEST
//	const CEBString sUserName = theEBClientCore->EB_UserName.GetBSTR();
//	const CEBString sSettingEnterprise = theEBSetting->Enterprise.GetBSTR();
//#else
//	const CEBString sUserName = theEBAppClient.EB_GetUserName();
//	const CEBString sSettingEnterprise = theSetting.GetEnterprise();
//#endif
//	CString sWindowText;
//	if (theApp.IsLogonVisitor())
//		sWindowText.Format(_T("游客-%s"),sUserName.c_str());
//	else if (!theApp.GetProductName().IsEmpty())
//		sWindowText.Format(_T("%s-%s(%s)"),theApp.GetProductName(),sUserName.c_str(),theApp.GetLogonAccount());
//	else
//		sWindowText.Format(_T("%s-%s(%s)"),sSettingEnterprise.c_str(),sUserName.c_str(),theApp.GetLogonAccount());
//	this->SetWindowText(sWindowText);

	const int nScreenWidth = theApp.GetScreenWidth();
	const int nScreenHeight = theApp.GetScreenHeight();
#ifdef USES_SUPPORT_UI_STYLE
	const int theDefaultWidth = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_WIDTH:LEN_DLG_WIDTH;
	const int theDefaultHeight = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_HEIGHT:LEN_DLG_HEIGHT;
	const int nLimitDefaultWidth = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_WIDTH:LIMIT_DLG_WIDTH;
	const int nLimitDefaultHeight = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_HEIGHT:LEN_DLG_HEIGHT;
#else
	const int theDefaultWidth = LEN_DLG_WIDTH;
	const int theDefaultHeight = LEN_DLG_HEIGHT;
	const int nLimitDefaultWidth = LIMIT_DLG_WIDTH;
	const int nLimitDefaultHeight = LEN_DLG_HEIGHT;
#endif
	UINT nX = max(0,GetPrivateProfileInt(_T("default"),_T("main-x"),(nScreenWidth-theDefaultWidth)/2,theApp.GetUserSettingIniFile()));
	UINT nY = max(0,GetPrivateProfileInt(_T("default"),_T("main-y"),(nScreenHeight-theDefaultHeight)/2,theApp.GetUserSettingIniFile()));
	UINT nW = max(nLimitDefaultWidth,GetPrivateProfileInt(_T("default"),_T("main-w"),theDefaultWidth,theApp.GetUserSettingIniFile()));
	UINT nH = max(nLimitDefaultHeight,GetPrivateProfileInt(_T("default"),_T("main-h"),theDefaultHeight,theApp.GetUserSettingIniFile()));
	//m_nShrinkageWin = GetPrivateProfileInt(_T("default"),_T("shrinkage-win"),0,theApp.GetUserSettingIniFile());
	nX = min((nScreenWidth-nLimitDefaultWidth),nX);
	nY = min((nScreenHeight-nLimitDefaultHeight),nY);
	nW = min(nScreenWidth,nW);
	nH = min(nScreenHeight,nH);
	MoveWindow(nX, nY, nW, nH);
	if (nW>=nScreenWidth && nH>=nScreenHeight)
	{
		this->SetToolTipText(IDC_BUTTON_MAX2,_T("向下还原"));
		m_btnMax.SetDrawToolButtonPic(3,theDefaultBtnWhiteColor,-1,-1,-1,2);
	}
	m_bOpenOk = true;
	SetCircle();

	// 未处理呼叫
	m_btnMySession.SetTextLeft(-40);
	m_btnMySession.SetTextTop(-2);
	m_btnMySession.SetHotTextColor(RGB(255,0,128)); 
	m_btnMySession.SetNorTextColor(RGB(255,0,128)); 
	m_btnMySession.SetPreTextColor(RGB(255,0,128)); 

	SetTimer(TIMERID_SHOW_EMOTION,12*1000,NULL);
	//// 系统托盘
	//CString sTrayText;
	//if (theApp.IsLogonVisitor())
	//	sTrayText.Format(_T("游客-%s-%s"), sUserName.c_str(),GetLineStateText(nOutLineState));
	//else
	//	sTrayText.Format(_T("%s(%s)-%s"), sUserName.c_str(),theApp.GetLogonAccount(),GetLineStateText(nOutLineState));
	//m_trayIconData.cbSize = sizeof(NOTIFYICONDATA);
	//m_trayIconData.hIcon = m_hIcon;
	//m_trayIconData.hWnd = this->GetSafeHwnd();
	//m_trayIconData.uCallbackMessage = WM_TRAYICON_NOTIFICATION;
	//m_trayIconData.uFlags = NIF_TIP|NIF_MESSAGE|NIF_ICON;
	//m_trayIconData.uID = 120;
	//lstrcpy(m_trayIconData.szTip, sTrayText);
	//Shell_NotifyIcon(NIM_ADD, &m_trayIconData);

	//
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	CFrameWndInfoProxy::SetChildFont(theDefaultDialogFontSize,theFontFace.c_str());

#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		m_btnSwitchLeft.ShowWindow(SW_HIDE);
	}else
#endif
	{
		CFrameWndInfoProxy::AddWnd(CFrameWndInfo::FRAME_WND_WORK_FRAME, _T("工作台"), _T("工作台 Ctrl+1"), false, false);
		CFrameWndInfoProxy::AddWnd(CFrameWndInfo::FRAME_WND_MAIN_FRAME, _T("联系人"), _T("联系人 Ctrl+2"), true, false);	// 即时通讯
	}
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
	SetTimer(TIMERID_CHECK_MOUSE_POS,50,NULL);
	SetTimer(TIMERID_CHECK_CLOSE_DIALOG,5000,NULL);
	SetTimer(TIMERID_CHECK_MF_SUBFUNCINFO,2000,NULL);
	SetTimer(TIMERID_ONE_SECOND,500,NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPOPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CString sText;
		sText.Format(_T("当前版本：%s\r\nCopyright (C) 2012-2017"),theSetting.GetVersion().c_str());
		CDlgMessageBox::EbMessageBox(AfxGetMainWnd(),"关于恩布互联 Entboost.com",sText,CDlgMessageBox::IMAGE_ENTBOOST,10);
		//CAboutDlg dlgAbout;
		//dlgAbout.DoModal();
	}
	else if (nID==SC_CLOSE)
	{
			OnBnClickedButtonClose();	// 支持实现，鼠标移动工作栏，点击关闭，隐藏窗口
	}
	else
	{
		CEbDialogBase::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPOPDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPOPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPOPDlg::OnClose()
{
	if (theWantExitApp)
	{
		m_pList.clear();
	}
	CEbDialogBase::OnClose();
}

void CPOPDlg::OnDestroy()
{
	m_pHideList.clear();
	m_pCloseList.clear();

	m_pMainFuncButtonList.clear();
	Shell_NotifyIcon(NIM_DELETE, &m_trayIconData);

	UnregisterHotKey(this->m_hWnd,0x2013); 
	UnregisterHotKey(this->m_hWnd,0x2014); 
	UnregisterHotKey(this->m_hWnd,0x2015); 

	CEbDialogBase::OnDestroy();
	m_menuFunc.DestroyMenu();
	m_menuState.DestroyMenu();
	m_menuTray.DestroyMenu();
	m_menuSkin.DestroyMenu();
#ifdef USES_SUPPORT_UI_STYLE
	m_menuUIStyleType.DestroyMenu();
#endif
	m_menuSetBrowserType.DestroyMenu();
#ifdef USES_EBCOM_TEST
	//AfxConnectionUnadvise(theEBClientCore,__uuidof(_IEBClientCoreEvents), GetIDispatch(FALSE),FALSE,dwAdvise);
	((CEBCoreEventsSink*)this)->DispEventUnadvise(theEBClientCore);
#else
	theEBAppClient.EB_SetMsgHwnd(NULL);
#endif
	m_btnSearchTrackDel.DestroyWindow();
	m_btnSearchTrackCall.DestroyWindow();
	if (m_pDlgEditInfo)
	{
		m_pDlgEditInfo->DestroyWindow();
		delete m_pDlgEditInfo;
		m_pDlgEditInfo = NULL;
	}
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
	if (m_pDlgBroadcastMsg!=NULL)
	{
		m_pDlgBroadcastMsg->DestroyWindow();
		delete m_pDlgBroadcastMsg;
		m_pDlgBroadcastMsg = NULL;
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
	if (m_pDlgMsgTip!=NULL)
	{
		m_pDlgMsgTip->DestroyWindow();
		delete m_pDlgMsgTip;
		m_pDlgMsgTip = NULL;
	}
	if (m_pPanelStatus!=NULL)
	{
		m_pPanelStatus->DestroyWindow();
		delete m_pPanelStatus;
		m_pPanelStatus = NULL;
	}

}

void CPOPDlg::DeleteDlgIncomingCall(eb::bigint sFromAccount)
{
	CDlgIncomingCall::pointer pDlgIncomingCall;
	if (m_pIncomingCallList.find(sFromAccount, pDlgIncomingCall, true))
	{
		pDlgIncomingCall->DestroyWindow();
	}
}
void CPOPDlg::DeleteDlgAlertingCall(eb::bigint sFromAccount)
{
	CDlgAlertingCall::pointer pDlgAlertingCall;
	if (m_pAlertingCallList.find(sFromAccount, pDlgAlertingCall, true))
	{
		pDlgAlertingCall->DestroyWindow();
	}
}

void CPOPDlg::ShowDlgFileManager(void)
{
	if (m_pDlgFileManager == NULL)
	{
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
	if (m_pDlgFileManager->IsEmpty())
	{
		//m_pDlgFileManager->OnBnClickedButtonRefresh();
		m_pDlgFileManager->OnBnClickedButtonTraned();
	}

	m_pDlgFileManager->SetForegroundWindow();
	m_pDlgFileManager->SetCircle();
}
void CPOPDlg::ShowStatusMessage(const wchar_t* sValue)
{
	if (sValue==NULL || wcslen(sValue)==0)
	{
		if (m_pPanelStatus->IsWindowVisible())
		{
			m_pPanelStatus->ShowWindow(SW_HIDE);
		}
		//m_sStatusMessage.clear();
	}else
	{
		const std::wstring m_sStatusMessage(sValue);
		CRect rect;
		m_btnSkin.GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		const int nStringWidth = m_pPanelStatus->MeasureTextWidth(m_sStatusMessage.c_str(),theApp.GetScreenWidth());
		const int const_status_height = 32;
		m_pPanelStatus->MoveWindow(rect.left-nStringWidth,rect.top,nStringWidth+2,const_status_height);
		m_pPanelStatus->SetDrawText(m_sStatusMessage.c_str(),true,5);
		//m_sStatusMessage = sValue;
		//KillTimer(TIMER_HIDE_STATUS_MESSAGE);
		//KillTimer(TIMER_SHOW_STATUS_MESSAGE);
		//SetTimer(TIMER_SHOW_STATUS_MESSAGE,100,NULL);
	}
}

LRESULT CPOPDlg::OnMessageAreaInfo(WPARAM wParam, LPARAM lParam)
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
			if (m_pDlgEditInfo!=NULL)
			{
#ifdef USES_EBCOM_TEST
				EB_AreaInfo pAreaInfoTemp(pAreaInfo);
				m_pDlgEditInfo->OnAreaInfo(&pAreaInfoTemp,nParameter);
#else
				m_pDlgEditInfo->OnAreaInfo(pAreaInfo,nParameter);
#endif
			}
		}break;
	default:
		break;
	}
	return 0;
}

//#define EB_BROADCAST_SUB_TYPE_SUBMSG (EB_BROADCAST_SUB_TYPE_UNREAD_EMAIL+1)//10

LRESULT CPOPDlg::OnMessageBroadcastMsg(WPARAM wParam, LPARAM lParam)
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
	const eb::bigint nMsgId = pApMsgInfo->m_nMsgId;
	const int nBCMsgSubType = pApMsgInfo->m_nMsgType;
	const CEBString sMsgName(pApMsgInfo->m_sMsgName);
	const CEBString sMsgContent(pApMsgInfo->m_sMsgContent);
#endif
	// 0=bc
	// 1=email msg
	switch (nBCMsgSubType)
	{
	case EB_BROADCAST_SUB_TYPE_NEW_EMAIL:
		{
			// 邮件消息通知；
			const tstring theEBSParseString0_from(" ");
			const tstring theEBSParseString0_to("&nbsp;");
			std::vector<tstring> pList;
			if (libEbc::ParseString(sMsgContent.c_str(),theEBSParseString0_from.c_str(),pList)<10)
				return 0;
			const eb::bigint nUserId = eb_atoi64(pList[0].c_str());
			const eb::bigint nMailAddressId = eb_atoi64(pList[1].c_str());
			const eb::bigint nMailContentId = eb_atoi64(pList[2].c_str());
			const int nMailSize = atoi(pList[3].c_str());
			const int nAttachCount = atoi(pList[4].c_str());
			CEBString sMailDate(pList[5]);
			libEbc::replace(sMailDate,theEBSParseString0_to,theEBSParseString0_from);
			CEBString sFromName(pList[6]);
			libEbc::replace(sFromName,theEBSParseString0_to,theEBSParseString0_from);
			const CEBString sFromMail(pList[7]);
			CEBString sSubject(pList[8]);
			libEbc::replace(sSubject,theEBSParseString0_to,theEBSParseString0_from);
			const CEBString sParam(pList[9]);

			if (m_pDlgMsgTip!=NULL && m_pDlgMsgTip->GetSafeHwnd()!=NULL)
			{
				const eb::bigint nEmailSubId = eb_atoi64(sMsgName.c_str());
				CString sMsgTip;
				sMsgTip.Format(_T("新邮件:%s<%s> %s:\n%s"),sFromName.c_str(),sFromMail.c_str(),sMailDate.c_str(),sSubject.c_str());
				m_pDlgMsgTip->AddEmailMsgTip(nMailContentId, nEmailSubId, sMsgTip, sParam.c_str());
				AddSubUnreadMsg(nEmailSubId,true);
			}
			return 0;
		}break;
	case EB_BROADCAST_SUB_TYPE_UNREAD_EMAIL:
		{
			if (m_pDlgMsgTip!=NULL && m_pDlgMsgTip->GetSafeHwnd()!=NULL)
			{
				const eb::bigint nEmailSubId = eb_atoi64(sMsgName.c_str());
				CString sMsgTip;
				sMsgTip.Format(_T("你一共有 %s 封未读邮件！\n点击查看！"),sMsgContent.c_str());
				m_pDlgMsgTip->AddEmailMsgTip(0, nEmailSubId, sMsgTip, "");
				SetSubUnreadMsg(nEmailSubId,atoi(sMsgContent.c_str()),true);
			}
			return 0;
		}break;
	case EB_BROADCAST_SUB_TYPE_TW_MSG:
		{
			// 应用消息通知；
			//const CEBString sMsgTip(sMsgName);
			const tstring theEBSParseString0_from(" ");
			//const tstring theEBSParseString0_to("&nbsp;");
			std::vector<tstring> pList;
			if (libEbc::ParseString(sMsgContent.c_str(),theEBSParseString0_from.c_str(),pList)<2)
			{
				theEBAppClient.EB_AckMsg(nMsgId,5);	// * 删除无用数据
				return 0;
			}
			const eb::bigint nId = eb_atoi64(pList[0].c_str());			// id->planid,taskid,...
			const eb::bigint nSubId = eb_atoi64(pList[1].c_str());		// subid
			const CEBString sParam = pList.size()>=3?pList[2]:"";
			if (!sMsgName.empty() && m_pDlgMsgTip!=NULL && m_pDlgMsgTip->GetSafeHwnd()!=NULL)
			{
				//CString sMsgTip;
				//sMsgTip.Format(_T("新消息:%s<%s> %s:\n%s"),sFromName.c_str(),sFromMail.c_str(),sMailDate.c_str(),sSubject.c_str());
				m_pDlgMsgTip->AddSubMsgTip(nId, nSubId, sMsgName.c_str(), sParam.c_str(), nMsgId);
			}
			AddSubUnreadMsg(nSubId,true);
			return 0;
		}break;
	case EB_BROADCAST_SUB_TYPE_SUBID_UNREAD_MSG:
		{
			std::vector<tstring> pList;
			if (libEbc::ParseString(sMsgContent.c_str(),",",pList)<2)
			{
				theEBAppClient.EB_AckMsg(nMsgId,5);	// * 删除无用数据
				return 0;
			}
			const eb::bigint nSubId = eb_atoi64(pList[0].c_str());		// subid
			const size_t nUnreadMsg = atoi(pList[1].c_str());					// unread msg
			SetSubUnreadMsg(nSubId,nUnreadMsg,true);
			return 0;
		}break;
	default:
		break;
	}

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

	//CString sContent;
	//if (theApp.GetGroupMsgSugId()>0)
	//	sContent.Format(_T("%s<br/><a href=\"eb-open-subid://%lld,tab_type=bc_msg\">查看我的消息</a>"),sMsgContent.c_str(),theApp.GetGroupMsgSugId());
	//if (theApp.GetGroupMsgSugId()>0)
	//	m_pDlgBroadcastMsg->UpdateBroadcastMsg(sMsgName,libEbc::ACP2UTF8(sContent),nMsgId);
	//else
	//	m_pDlgBroadcastMsg->UpdateBroadcastMsg(sMsgName,libEbc::ACP2UTF8(sMsgContent.c_str()),nMsgId);
	if (m_pDlgBroadcastMsg->GetShowMyMsgSubId()==0 && theApp.GetGroupMsgSugId()>0)
	{
		m_pDlgBroadcastMsg->SetShowMyMsgSubId(theApp.GetGroupMsgSugId(),"tab_type=bc_msg");
	}
	m_pDlgBroadcastMsg->UpdateBroadcastMsg(sMsgName,libEbc::ACP2UTF8(sMsgContent.c_str()),nMsgId);
	m_pDlgBroadcastMsg->ShowWindow(SW_SHOW);
	//CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
	//CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
	//pFuncWindow->m_bDisableContextMenu = true;
	//pFuncWindow->m_bBroadcastMsg = true;
	//pFuncWindow->m_nMsgType = nMsgType;
	//pFuncWindow->m_bOpenNewClose = true;
	//pFuncWindow->m_sTitle = sMsgName;
	//if (theApp.GetGroupMsgSugId()>0)
	//	pFuncWindow->m_sFuncUrl = libEbc::ACP2UTF8(sContent);
	//else
	//	pFuncWindow->m_sFuncUrl = libEbc::ACP2UTF8(sMsgContent.c_str());
	//pFuncWindow->m_nWidth = 250;
	//pFuncWindow->m_nHeight = 180;
	//CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW,true);
	//pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
	//pEbcMsgInfo->m_pParent = pParent;
	//pEbcMsgInfo->m_bSetForegroundWindow = true;
	//m_pEbcMsgList.add(pEbcMsgInfo);

	return 0;
}

LRESULT CPOPDlg::OnMessageNewVersion(WPARAM wParam, LPARAM lParam)
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
	UINT nType = pVersionInfo->UpdateType;
	if (nType==-1)
		nType = 101;
	else if (nType==-2)
		nType = 102;
	theApp.EBC_SetProfileInt(_T("new_version"),_T("type"),nType);
	SetTimer(TIMERID_NEW_VERSION,100,NULL);
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
	UINT nType = pNewVersionInfo->m_nUpdateType;
	if (nType==-1)
		nType = 101;
	else if (nType==-2)
		nType = 102;
	theApp.EBC_SetProfileInt(_T("new_version"),_T("type"),nType);
	SetTimer(TIMERID_NEW_VERSION,100,NULL);
#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageServerChange(WPARAM wParam, LPARAM lParam)
{
	// 会话无效，提示重新登录
#ifdef USES_EBCOM_TEST
	const int nServerState = (int)wParam;
#else
	const CEBCallbackInterface::SERVER_STATE nServerState = (CEBCallbackInterface::SERVER_STATE)wParam;
#endif
	if (nServerState == 1)	// CEBCallbackInterface::SERVER_STOPED
	{
		CDlgMessageBox::EbMessageBox(this,"",_T("系统正在更新维护：\r\n稍候会自动重新登录..."),CDlgMessageBox::IMAGE_WARNING,5);
	//}else if (nServerState == 3)	// CEBCallbackInterface::SERVER_RESTART)
	//{
	//	// **SDK内部会自动重新登录；
	//	//CDlgMessageBox::EbMessageBox(this,"更新成功：",_T("正在重新登录..."),CDlgMessageBox::IMAGE_WARNING,8);
	//	//SetTimer(TIMERID_RELOGIN,5000,NULL);
	}
	if (this->m_pDlgAppFrame!=NULL)
	{
		this->m_pDlgAppFrame->OnOffline(nServerState);
	}
#ifdef USES_FRAMELIST_APPFRAME
	if (this->m_pDlgFrameList!=NULL)
	{
		this->m_pDlgFrameList->OnOffline(nServerState);
	}
#endif
	return 0;
}
LRESULT CPOPDlg::OnMessageStateCode(WPARAM wParam, LPARAM lParam)
{
	// 会话无效，在这里重新登录
	const EB_STATE_CODE nStateCode = (EB_STATE_CODE)wParam;
	if (EB_STATE_APPID_KEY_ERROR==nStateCode || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nStateCode)
	{
		return 0;
	}
	switch (nStateCode)
	{
	case EB_STATE_OK:
		break;
	case EB_STATE_RES_NOT_EXIST:
		CDlgMessageBox::EbMessageBox(this,"",_T("资源（文件）不存在：\r\n已经被删除！"),CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_MAX_UG_ERROR:
		CDlgMessageBox::EbMessageBox(this,"",_T("超过最大联系人分组数量：\r\n请检查后重试！"),CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_MAX_CONTACT_ERROR:
		CDlgMessageBox::EbMessageBox(this,"",_T("超过最大联系人数量：\r\n请检查后重试！"),CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_OAUTH_FORWARD:
		CDlgMessageBox::EbMessageBox(this,"",_T("操作成功：\r\n等待对方验证！"),CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_UNAUTH_ERROR:
		CDlgMessageBox::EbMessageBox(this,"","帐号未通过验证：\r\n请联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_ACCOUNT_FREEZE:
		CDlgMessageBox::EbMessageBox(this,"","帐号已被临时冻结：\r\n请联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_ACCOUNT_NOT_EXIST:
		CDlgMessageBox::EbMessageBox(this,"","帐号不存在：\r\n请联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_CONTENT_BIG_LENGTH:
		CDlgMessageBox::EbMessageBox(this,"","字段内容超过最大长度：\r\n请重新输入！",CDlgMessageBox::IMAGE_WARNING,5);
		break;
	case EB_STATE_PARAMETER_ERROR:
		CDlgMessageBox::EbMessageBox(this,"","参数错误：\r\n请重试或联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
		break;
	default:
		{
			CString sText;
			sText.Format(_T("错误代码：%d\r\n请重试或联系公司客服！"),(int)nStateCode);
			CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_WARNING,5);
		}break;
	}
	return 0;
}

LRESULT CPOPDlg::OnMessageOnlineAnother(WPARAM wParam, LPARAM lParam)
{
	// 0=已经在其他地方登录，退出当前连接；
	// 1=修改密码，退出当前连接；
	const int nOnlineAnotherType = (int)wParam;
	SetTimer(TIMERID_LOGOUT,10,NULL);
	return 0;
}
LRESULT CPOPDlg::OnMessageLogoutResponse(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CPOPDlg::OnMessageLogonSuccess(WPARAM wParam, LPARAM lParam)
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
		theEBClientCore->EB_LoadContact();
	}
	const CEBString sHeadFile = theEBClientCore->EB_GetMyDefaultMemberHeadFile().GetBSTR();
#else
	theEBAppClient.EB_LoadOrg();	// 加载组织结构、表情、头像
	if (theApp.IsLogonVisitor())
	{
		//this->GetDlgItem(IDC_BUTTON_MYCONTACTS)->EnableWindow(FALSE);
	}else
	{
		theEBAppClient.EB_LoadContact();
	}
	const CEBString sHeadFile = theEBAppClient.EB_GetMyDefaultMemberHeadFile();
#endif
	if (!sHeadFile.empty())
	{
		this->Invalidate();
	}
	//SetTimer(TIMERID_LOADINFO, 2*1000, NULL);	// 改由SDK内部自动加载；
	theUpdateResetTimer = false;
	KillTimer(TIMERID_CHECK_UPDATE);
	//SetTimer(TIMERID_CHECK_UPDATE,10*1000,NULL);	// 10秒测试检查
	SetTimer(TIMERID_CHECK_UPDATE,(1+rand()%5)*60*1000,NULL);	// 先1-5分钟检查
	SetTimer(TIMERID_LOAD_LOCAL_UNREAD_MSG,6000,NULL);

	if (this->m_pDlgAppFrame!=NULL)
	{
		this->m_pDlgAppFrame->OnLogonSuccess();
	}
#ifdef USES_FRAMELIST_APPFRAME
	if (this->m_pDlgFrameList!=NULL)
	{
		this->m_pDlgFrameList->OnLogonSuccess();
	}
#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageLogonTimeout(WPARAM wParam, LPARAM lParam)
{
	CDlgMessageBox::EbDoModal(this,"",_T("登录超时：\r\n请重新登录。"),CDlgMessageBox::IMAGE_WARNING,true,8);
	OnLogout();
	return 0;
}

LRESULT CPOPDlg::OnMessageLogonError(WPARAM wParam, LPARAM lParam)
{
	int nErrorCode = lParam;
	if (nErrorCode == EB_STATE_UNAUTH_ERROR)
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("该帐号未通过验证：\r\n请查收邮件，点击验证链接，完成注册！"),CDlgMessageBox::IMAGE_WARNING,5);
	else if (nErrorCode == EB_STATE_ACCOUNT_FREEZE)
		CDlgMessageBox::EbMessageBox(this,"","帐号已被临时冻结：\r\n请联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
	else if (nErrorCode == EB_STATE_MAX_RETRY_ERROR)
		CDlgMessageBox::EbMessageBox(this,"","错误次数太频繁：\r\n帐号已被临时锁住，请稍候再试！",CDlgMessageBox::IMAGE_WARNING,5);
	else
	{
		CString sText;
		sText.Format(_T("错误代码：%d\r\n请重新登录！"),nErrorCode);
		CDlgMessageBox::EbDoModal(this,"登录失败",sText,CDlgMessageBox::IMAGE_WARNING,true);
		OnLogout();
	}
	return 0;
}
LRESULT CPOPDlg::OnMessageAccountInfoChange(WPARAM wParam, LPARAM lParam)
{
	//const EB_AccountInfo* pAccountInfo = (const EB_AccountInfo*)wParam;
	//const CString sAccountName = pAccountInfo->GetUserName().c_str();
	SetWindowTextAndTrayInfo(false);
	// 刷新主界面显示
	RECT rect;
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		rect.left = UISTYLE_CHAT_POS_LINESTATE_LEFT+28;
		rect.top = UISTYLE_CHAT_POS_LINESTATE_TOP-3;
	}
	else
#endif
	{
		rect.left = POS_LINESTATE_LEFT+28;
		rect.top = POS_LINESTATE_TOP-3;
	}
	rect.right = rect.left + 200;
	rect.bottom = rect.top + 25;
	this->InvalidateRect(&rect);
	//this->Invalidate();	// 刷新界面显示；

	return 0;
}

LRESULT CPOPDlg::OnMessageSInfoResponse(WPARAM wParam, LPARAM lParam)
{
	const EB_STATE_CODE nState = (EB_STATE_CODE)wParam;
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
	{
		return 0;
	}
	const int nFlag = (int)lParam;
	if (nState==EB_STATE_ACC_PWD_ERROR)
	{
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("帐号或密码错误：\r\n请重新输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		return 0;
	}else if (nFlag==1)	// *** 修改密码
	{
		if (nState==EB_STATE_OK)
		{
			if (theApp.m_pBoEB->use("eb"))
			{
				CString sSql;
				sSql.Format(_T("update user_login_record_t set password='' where user_id=%lld"),theApp.GetLogonUserId());
				theApp.m_pBoEB->execsql(sSql);
				theApp.m_pBoEB->close();
			}
			CDlgMessageBox::EbDoModal(this,"",_T("密码修改成功：\r\n请重新登录。"),CDlgMessageBox::IMAGE_INFORMATION,true,8);
			OnLogout();
			return 0;
			//CDlgMessageBox::EbMessageBox(this,_T(""),_T("密码修改成功！"),CDlgMessageBox::IMAGE_INFORMATION,5);
		}else
		{
			CString sText;
			sText.Format(_T("密码修改失败，错误代码：%d\r\n请重试！"),nState);
			CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_WARNING,5);
		}
	}else if (nState==EB_STATE_OK)
	{
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("个人资料修改成功！"),CDlgMessageBox::IMAGE_INFORMATION,5);
	}else
	{
		CString sText;
		sText.Format(_T("个人资料修改失败，错误代码：%d\r\n请重试！"),nState);
		CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_WARNING,5);
	}
#ifdef USES_EBCOM_TEST
	m_editDescription.SetWindowText(CEBString(theEBClientCore->EB_Description.GetBSTR()).c_str());
	ChangeTrayText();
	this->Invalidate();
#else
	m_editDescription.SetWindowText(theEBAppClient.EB_GetDescription().c_str());
	ChangeTrayText();
	this->Invalidate();
#endif
	return 0;
}

//LRESULT CPOPDlg::OnMessageEnterRoom(WPARAM wParam, LPARAM lParam)
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
//LRESULT CPOPDlg::OnMessageExitRoom(WPARAM wParam, LPARAM lParam)
//{
//	const char * lpszCallId = (const char*)lParam;
//	if (lpszCallId==NULL) return 1;
//	theApp.m_pCallList.remove(lpszCallId);
//	theApp.m_pDialogList.remove(lpszCallId);
//	return 0;
//}

CDlgDialog::pointer CPOPDlg::GetCallIdDialog(eb::bigint nCallId, bool bRemove)
{
	if (theWantExitApp)
	{
		return NullDlgDialog;	// 预防死锁
	}
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// **跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		CDlgDialog::pointer pDlgDialog =  CFrameWndInfoProxy::GetCallIdDialog(nCallId, bRemove, false);
		//if (pDlgDialog.get()!=NULL && pDlgDialog->GetSafeHwnd()!=NULL && m_pDlgMsgTip!=NULL && pDlgDialog->IsWindowVisible() && this->IsWindowVisible())
		//{
		//	CEBCCallInfo::pointer pEbCallInfo = pDlgDialog->GetCallInfo();
		//	if (pEbCallInfo.get()!=NULL)
		//		m_pDlgMsgTip->DelMsgTip(pEbCallInfo->m_pCallInfo.m_sGroupCode,pEbCallInfo->m_pFromAccountInfo.GetUserId());
		//}
		return pDlgDialog;
	}
#endif
	return m_pDlgFrameList!=NULL?m_pDlgFrameList->GetCallIdDialog(nCallId, bRemove, false):NullDlgDialog;
}

LRESULT CPOPDlg::OnMessageUserEnterRoom(WPARAM wParam, LPARAM lParam)
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
	if (pAccountInfo==NULL)
	{
		return 1;
	}

	CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_USER_ENTER_ROOM);
	pEbcMsgInfo->m_pAccountInfo = *pAccountInfo;
	m_pEbcMsgList.add(pEbcMsgInfo);

	//const eb::bigint sCallId = pAccountInfo->GetCallId();
	//CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	//if (pDlgDialog.get()!=NULL)
	//{
	//	pDlgDialog->OnUserEnterRoom(pAccountInfo,true);
	//}
#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageUserExitRoom(WPARAM wParam, LPARAM lParam)
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

// 游客头像变成灰色（未处理）
//#ifdef USES_NEW_UI_1220
//#ifdef USES_SUPPORT_UI_STYLE
//	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
//	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
//	{
//		// **跑下面
//	}else if (!theApp.GetHideMainFrame())
//#else
//	if (!theApp.GetHideMainFrame())
//#endif
//	{
//		CFrameWndInfoProxy::UserLineStateChange(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId, pMemberInfo->m_nLineState);
//		return 0;
//	}
//#endif
//	if (m_pDlgFrameList!=NULL)
//	{
//		m_pDlgFrameList->UserLineStateChange(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId, pMemberInfo->m_nLineState);
//	}
//
//#endif

	return 0;
}
LRESULT CPOPDlg::OnMessageUserNotify(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST

#else
	const CCrNotifyInfo* pNotifyInfo = (const CCrNotifyInfo*)wParam;
	if (pNotifyInfo==NULL)
	{
		return 1;
	}
	const eb::bigint sCallId = pNotifyInfo->GetCallId();
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()==NULL) return 1;
	CString sFirstMsg;
	pDlgDialog->OnUserNotify(pNotifyInfo,&sFirstMsg);
	if (pNotifyInfo->m_nNotifyType==2 && !pDlgDialog->IsWindowVisible())	// 2=群共享文件通知
	{
		//CString sSoundFile;
		//sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
		//sndPlaySound(sSoundFile, SND_ASYNC);

#ifdef USES_NEW_UI_1220
		bool bRet = false;
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			bRet = m_pDlgFrameList->AddUnreadMsg(sCallId,0);
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif	// USES_SUPPORT_UI_STYLE
		{
			const bool bRet = CFrameWndInfoProxy::AddUnreadMsg(sCallId, 0);
			if (bRet && this->IsWindowVisible())
			{
				::FlashWindow(this->GetSafeHwnd(), TRUE);
				return 0;
			}
		}else if (m_pDlgFrameList!=NULL)
		{
			bRet = m_pDlgFrameList->AddUnreadMsg(sCallId,0);
		}
#else
		bRet = m_pDlgFrameList==NULL?false:m_pDlgFrameList->AddUnreadMsg(sCallId,nMsgId);
#endif
		if (bRet && m_pDlgFrameList!=NULL && m_pDlgFrameList->IsWindowVisible())
		{
			::FlashWindow(m_pDlgFrameList->GetSafeHwnd(), TRUE);
			return 0;
		}
		//m_btnMySession.SetWindowText(_T("！"));
		if (m_pDlgMsgTip!=NULL && m_pDlgMsgTip->GetSafeHwnd()!=NULL)
		{
			m_pDlgMsgTip->AddMsgTip(pDlgDialog->GetGroupId(),pNotifyInfo->m_nFromAccount,sFirstMsg);
		}

	}

#endif
	return 0;
}
LRESULT CPOPDlg::OnMessageReceiveRich(WPARAM wParam, LPARAM lParam)
{
	//CString sSoundFile;
	//sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	//sndPlaySound(sSoundFile, SND_ASYNC);

#ifdef USES_EBCOM_TEST
	IEB_ChatRichInfo* pChatRichInfo = (IEB_ChatRichInfo*)wParam;
	const eb::bigint sCallId = pChatRichInfo->CallId;
	const eb::bigint nFromUserid = pChatRichInfo->SendFrom;
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return -1;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,false);
	CString sFirstMsg;
	pDlgDialog->OnReceiveRich(pChatRichInfo,&sFirstMsg);
#else
	const CCrRichInfo * pCrMsgInfo = (const CCrRichInfo*)wParam;
	if (pCrMsgInfo==NULL || pCrMsgInfo->m_pRichMsg==NULL) return -1;
	const eb::bigint nFromUserid = pCrMsgInfo->m_sSendFrom;
	const eb::bigint sCallId = pCrMsgInfo->GetCallId();
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		//theApp.LogMessage("CPOPDlg::OnMessageReceiveRich call not fine error, (callid=%lld,fromuid=%lld,msgid=%lld)\r\n",sCallId,nFromUserid,pCrMsgInfo->m_pRichMsg->GetMsgId());
		return -1;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,false);
	CString sFirstMsg1;
	CString sFirstMsg2;
	pDlgDialog->OnReceiveRich(pCrMsgInfo,&sFirstMsg1,&sFirstMsg2);
	const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();
#endif
	//CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_RECEIVE_RICH);
	//pEbcMsgInfo->m_pEbCallInfo = pEbCallInfo;
	//m_pEbcMsgList.add(pEbcMsgInfo);

	if (!pDlgDialog->IsWindowVisible())
	{
		CString sSoundFile;
		sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
		sndPlaySound(sSoundFile, SND_ASYNC);

#ifdef USES_NEW_UI_1220
		bool bRet = false;
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			bRet = m_pDlgFrameList->AddUnreadMsg(sCallId,nMsgId);
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif	// USES_SUPPORT_UI_STYLE
		{
			const bool bRet = CFrameWndInfoProxy::AddUnreadMsg(sCallId, nMsgId);
			if (bRet && this->IsWindowVisible())
			{
				::FlashWindow(this->GetSafeHwnd(), TRUE);
				return 0;
			}
		}else if (m_pDlgFrameList!=NULL)
		{
			bRet = m_pDlgFrameList->AddUnreadMsg(sCallId,nMsgId);
		}
#else
		bRet = m_pDlgFrameList==NULL?false:m_pDlgFrameList->AddUnreadMsg(sCallId,nMsgId);
#endif
		if (bRet && m_pDlgFrameList!=NULL && m_pDlgFrameList->IsWindowVisible())
		{
			::FlashWindow(m_pDlgFrameList->GetSafeHwnd(), TRUE);
			return 0;
		}
		//m_btnMySession.SetWindowText(_T("！"));
		if (m_pDlgMsgTip!=NULL && m_pDlgMsgTip->GetSafeHwnd()!=NULL)
		{
			m_pDlgMsgTip->AddMsgTip(pEbCallInfo->m_pCallInfo.m_sGroupCode,nFromUserid,sFirstMsg2);
		}

		//// 显示消息
		//CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		//CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		//pFuncWindow->m_pEbCallInfo = pEbCallInfo;
		//pFuncWindow->m_bDisableContextMenu = true;
		//pFuncWindow->m_bBroadcastMsg = true;
		//pFuncWindow->m_bOpenNewClose = true;
		//pFuncWindow->m_sTitle = _T("聊天消息提醒");
		//pFuncWindow->m_sFuncUrl = (LPCTSTR)sFirstMsg1;
		//pFuncWindow->m_nWidth = 250;
		//pFuncWindow->m_nHeight = 180;
		//CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
		//pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
		//pEbcMsgInfo->m_pParent = pParent;
		//m_pEbcMsgList.add(pEbcMsgInfo);
	}
	return 0;
}

LRESULT CPOPDlg::OnMessageSendRich(WPARAM wParam, LPARAM lParam)
{
	//const EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	//if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
	//	return 0;

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
	const EB_STATE_CODE nState = pCrMsgInfo->GetStateCode();
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;

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
	if (m_pDlgMsgTip!=NULL && m_pDlgMsgTip->GetSafeHwnd()!=NULL)
	{
		m_pDlgMsgTip->DelMsgTip(pEbCallInfo->m_pCallInfo.m_sGroupCode,pEbCallInfo->m_pFromAccountInfo.GetUserId());
	}

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

LRESULT CPOPDlg::OnMessageMsgReceipt(WPARAM wParam, LPARAM lParam)
{
//#ifdef USES_EBCOM_TEST
//	IEB_ChatRichInfo* pChatRichInfo = (IEB_ChatRichInfo*)wParam;
//	const eb::bigint sCallId = pChatRichInfo->CallId;
//	CEBCCallInfo::pointer pEbCallInfo;
//	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
//	{
//		return 0;
//	}
//	pEbCallInfo->m_tLastTime = time(0);
//	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
//	if (pDlgDialog.get()==NULL)
//	{
//		return 1;
//	}
//	pDlgDialog->OnSendRich(pChatRichInfo,nState);
//#else
	const CCrRichInfo * pCrMsgInfo = (const CCrRichInfo*)wParam;
	if (pCrMsgInfo==NULL) return 1;
	//const EB_STATE_CODE nState = pCrMsgInfo->GetStateCode();
	const eb::bigint sCallId = pCrMsgInfo->GetCallId();
	const int nAckType = (int)lParam;	// 0:成功 4:撤回消息，6:个人收藏，7:群收藏

	//const eb::bigint nFromUserId = pCrMsgInfo->m_sSendFrom;
	//const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();

	//CEBCCallInfo::pointer pEbCallInfo;
	//if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	//{
	//	return 1;
	//}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()==NULL)
	{
		const eb::bigint nFromUserId = pCrMsgInfo->m_sSendFrom;
		const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();
		if (pCrMsgInfo->GetStateCode()==EB_STATE_OK)
			theApp.UpdateMsgReceiptData(nMsgId, nFromUserId, nAckType);
		return 1;
	}
	pDlgDialog->OnMsgReceipt(pCrMsgInfo, nAckType);

//#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageSendingFile(WPARAM wParam, LPARAM lParam)
{
	//EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	//if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
	//	return 0;
	//switch (nState)
	//{
	//case EB_STATE_FILE_ALREADY_EXIST:
	//	return 0;
	//case EB_STATE_OK:
	//	break;
	//case EB_STATE_NOT_AUTH_ERROR:
	//	CDlgMessageBox::EbMessageBox(this,"","没有权限：\r\n请联系管理员！",CDlgMessageBox::IMAGE_WARNING,5);
	//	return 1;
	//case EB_STATE_EXCESS_QUOTA_ERROR:
	//case EB_STATE_FILE_BIG_LONG:
	//	CDlgMessageBox::EbMessageBox(this,"","文件大小超出系统最大配额：\r\n请发送更小文件！",CDlgMessageBox::IMAGE_WARNING,5);
	//	return 1;
	//case EB_STATE_TIMEOUT_ERROR:
	//	CDlgMessageBox::EbMessageBox(this,"","文件发送超时：\r\n请重新发送！",CDlgMessageBox::IMAGE_WARNING,5);
	//	return 1;
	//default:
	//	{
	//		CString stext;
	//		stext.Format(_T("文件发送失败，错误代码：%d\r\n请检查后重试！"),(int)nState);
	//		CDlgMessageBox::EbMessageBox(this,"",stext,CDlgMessageBox::IMAGE_WARNING,5);
	//		return 1;
	//	}
	//}
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

	EB_STATE_CODE nState = (EB_STATE_CODE)pCrFileInfo->GetStateCode();
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;
	switch (nState)
	{
	case EB_STATE_GROUP_FORBID_SPEECH:
		CDlgMessageBox::EbMessageBox(this,"","群禁言中：\r\n不能发送群文件！",CDlgMessageBox::IMAGE_WARNING,5);
		return 1;
	case EB_STATE_FORBID_SPEECH:
		CDlgMessageBox::EbMessageBox(this,"","你被禁言中：\r\n不能发送群文件！",CDlgMessageBox::IMAGE_WARNING,5);
		return 1;
	case EB_STATE_FILE_ALREADY_EXIST:
		//return 0;	// 不能返回，后面处理
	case EB_STATE_OK:
	case EB_STATE_WAITING_PROCESS:	// ** 等待处理
		break;
	case EB_STATE_NOT_AUTH_ERROR:
		CDlgMessageBox::EbMessageBox(this,"","没有权限：\r\n请联系管理员！",CDlgMessageBox::IMAGE_WARNING,5);
		return 1;
	case EB_STATE_EXCESS_QUOTA_ERROR:
	case EB_STATE_FILE_BIG_LONG:
		{
			if (theApp.IsLicenseType())
				CDlgMessageBox::EbMessageBox(this,"","当前文件大小超出系统最大配额：\r\n请发送更小文件，或联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
			else
				CDlgMessageBox::EbMessageBox(this,"","免费用户，不支持发送大文件：\r\n请发送更小文件，或联系公司客服！",CDlgMessageBox::IMAGE_WARNING,5);
			return 1;
		}
	case EB_STATE_TIMEOUT_ERROR:
		CDlgMessageBox::EbMessageBox(this,"","文件发送超时：\r\n请重新发送！",CDlgMessageBox::IMAGE_WARNING,5);
		return 1;
	case EB_STATE_NOT_MEMBER_ERROR:
		CDlgMessageBox::EbMessageBox(this,"","没有其他群组成员：\r\n不能发送文件！",CDlgMessageBox::IMAGE_WARNING,5);
		return 1;
	default:
		{
			CString stext;
			stext.Format(_T("文件发送失败，错误代码：%d\r\n请检查后重试！"),(int)nState);
			CDlgMessageBox::EbMessageBox(this,"",stext,CDlgMessageBox::IMAGE_WARNING,5);
			return 1;
		}
	}

	const eb::bigint sCallId = pCrFileInfo->GetCallId();
	const eb::bigint sResId = pCrFileInfo->m_sResId;
	if (sResId>0 && sCallId==0)
	//if (sResId>0 && nState!=EB_STATE_FILE_ALREADY_EXIST)
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

LRESULT CPOPDlg::OnMessageSendedFile(WPARAM wParam, LPARAM lParam)
{
	//const EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	//if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
	//	return 0;

	//CString sSoundFile;
	//sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	//sndPlaySound(sSoundFile, SND_ASYNC);

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
	const EB_STATE_CODE nState = (EB_STATE_CODE)pCrFileInfo->GetStateCode();
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;

	const eb::bigint sCallId = pCrFileInfo->GetCallId();
	const eb::bigint sResId = pCrFileInfo->m_sResId;
	if (sResId>0 && sCallId==0)
	//if (sResId>0)
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
			if (pDlgDialog->OnSentFile(pCrFileInfo, nState) && !pDlgDialog->IsWindowVisible())
			{
				CString sSoundFile;
				sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
				sndPlaySound(sSoundFile, SND_ASYNC);
			}
		}else
		{
			if (pCrFileInfo->m_sReceiveAccount!=theEBAppClient.EB_GetLogonUserId())	// * 对方接收消息回执
			{
				const eb::bigint nFromUserId = pCrFileInfo->m_sReceiveAccount;
				const eb::bigint nMsgId = pCrFileInfo->m_nMsgId;
				theApp.UpdateMsgReceiptData(nMsgId, nFromUserId, 0);
			}
		}
	}
#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageCancelFile(WPARAM wParam, LPARAM lParam)
{
	//if (lParam!=10)
	//{
	//	CString sSoundFile;
	//	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	//	sndPlaySound(sSoundFile, SND_ASYNC);
	//}
#ifdef USES_EBCOM_TEST
	if (lParam==10) return 0;	// ???
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
	const bool bChangeP2PSending = lParam==1?true:false;
	const eb::bigint sCallId = pCrFileInfo->GetCallId();
	const eb::bigint sResId = pCrFileInfo->m_sResId;
	const eb::bigint nReceiveAccount = pCrFileInfo->m_sReceiveAccount;
	//const eb::bigint nSendToUid = pCrFileInfo->m_sSendTo;
	if (sResId>0 && sCallId==0)
	//if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL && nReceiveAccount==0)
			m_pDlgFileManager->DeleteDlgTranFile(pCrFileInfo->m_nMsgId);

		if (sResId==pCrFileInfo->m_nMsgId)
		{
			delete pCrFileInfo;
			return 0;
		}
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
		pDlgDialog->OnCancelFile(pCrFileInfo,bChangeP2PSending);
		if (!pDlgDialog->IsWindowVisible())
		{
			if (lParam!=10)
			{
				CString sSoundFile;
				sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
				sndPlaySound(sSoundFile, SND_ASYNC);
			}
		}
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

LRESULT CPOPDlg::OnMessageReceivingFile(WPARAM wParam, LPARAM lParam)
{
	//if (lParam!=10)
	//{
	//	CString sSoundFile;
	//	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	//	sndPlaySound(sSoundFile, SND_ASYNC);
	//}

#ifdef USES_EBCOM_TEST
	if (lParam==10) return 0;	// ???
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
	if (pCrFileInfo==NULL) return -1;
	const eb::bigint sResId = pCrFileInfo->m_sResId;
	if (sResId>0 && pCrFileInfo->GetCallId()==0)
	//if (sResId>0 && (!pCrFileInfo->m_bOffFile || sResId==pCrFileInfo->m_nMsgId))
	{
		ShowDlgFileManager();
		m_pDlgFileManager->OnReceivingFile(pCrFileInfo);
		m_pDlgFileManager->OnBnClickedButtonTraning();
		if (sResId==pCrFileInfo->m_nMsgId)
		{
			delete pCrFileInfo;
			return 0;
		}
	}
	const eb::bigint sCallId = pCrFileInfo->GetCallId();
	CEBCCallInfo::pointer pEbCallInfo;
	if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
	{
		return -1;
	}
	pEbCallInfo->m_tLastTime = time(0);
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,false);
	CString sFirstMsg;
	pDlgDialog->OnReceivingFile(pCrFileInfo,&sFirstMsg);
#endif
	//if (sResId>0) return 0;

	if (!pDlgDialog->IsWindowVisible())
	{
		if (lParam!=10)
		{
			CString sSoundFile;
			sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
			sndPlaySound(sSoundFile, SND_ASYNC);
		}

#ifdef USES_NEW_UI_1220
		bool bRet = false;
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			bRet = m_pDlgFrameList->AddUnreadMsg(sCallId,0);
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			const bool bRet = CFrameWndInfoProxy::AddUnreadMsg(sCallId,0);
			if (bRet && this->IsWindowVisible())
			{
				::FlashWindow(this->GetSafeHwnd(), TRUE);
				return 0;
			}
		}else if (m_pDlgFrameList!=NULL)
		{
			bRet = m_pDlgFrameList->AddUnreadMsg(sCallId,0);
		}
#else
		bRet = m_pDlgFrameList==NULL?false:m_pDlgFrameList->AddUnreadMsg(sCallId,0);
#endif
		if (bRet && m_pDlgFrameList!=NULL && m_pDlgFrameList->IsWindowVisible())
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

LRESULT CPOPDlg::OnMessageReceivedFile(WPARAM wParam, LPARAM lParam)
{
	//if (lParam!=10)
	//{
	//	CString sSoundFile;
	//	sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	//	sndPlaySound(sSoundFile, SND_ASYNC);
	//}

#ifdef USES_EBCOM_TEST
	if (lParam==10) return 0;	// ???
	IEB_ChatFileInfo* pCrFileInfo = (IEB_ChatFileInfo*)wParam;
	const eb::bigint sCallId = pCrFileInfo->CallId;
	const eb::bigint sResId = pCrFileInfo->ResId;
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
	if (sResId>0 && pCrFileInfo->GetCallId()==0)
	//if (sResId>0)
	{
		const bool bIsHttpDownloadFile = sResId==pCrFileInfo->m_nMsgId?true:false;
		if (m_pDlgFileManager!=NULL)
		{
			m_pDlgFileManager->OnReceivedFile(pCrFileInfo);
			if (!bIsHttpDownloadFile && m_pDlgFileManager->IsEmpty())
			{
				m_pDlgFileManager->ShowWindow(SW_HIDE);
			}
		}
		if (bIsHttpDownloadFile)
		{
			tstring sInFileName(pCrFileInfo->m_sFileName);
			theApp.m_pBoUsers->escape_string_in(sInFileName);
			CString sSql;
			sSql.Format(_T("INSERT INTO msg_record_t(msg_id,from_uid,from_name,to_uid,msg_type,msg_text) ")\
				_T("VALUES(%lld,%lld,'',%lld,%d,'%s')"),
				pCrFileInfo->m_nMsgId,pCrFileInfo->m_sSendFrom,
				pCrFileInfo->m_sSendTo,MRT_FILE,libEbc::ACP2UTF8(sInFileName.c_str()).c_str());
			theApp.m_pBoUsers->execute(sSql);
			delete pCrFileInfo;
			if (m_pDlgFileManager->IsWindowVisible() && m_pDlgFileManager->IsEmpty())
			{
				m_pDlgFileManager->OnBnClickedButtonRefresh();
				m_pDlgFileManager->OnBnClickedButtonTraned();
			}
			m_pDlgFileManager->FlashWindow(TRUE);
			return 0;
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
		if (!pDlgDialog->IsWindowVisible())
		{
			if (lParam!=10)
			{
				CString sSoundFile;
				sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
				sndPlaySound(sSoundFile, SND_ASYNC);
			}
		}
	}

#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageFilePercent(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	if (lParam==10) return 0;	// ???
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
	if (sResId>0 && sCallId==0)
	//if (sResId>0)
	{
		if (m_pDlgFileManager!=NULL)
			m_pDlgFileManager->SetFilePercent(pChatRoomFilePercent);

		if (sResId==pChatRoomFilePercent->m_nMsgId)
		{
			delete pChatRoomFilePercent;
			return 0;
		}
	}
	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnFilePercent(pChatRoomFilePercent);
	}

#endif
	return 0;
}
LRESULT CPOPDlg::OnMessageSave2CloudDrive(WPARAM wParam, LPARAM lParam)
{
	//const EB_STATE_CODE nState = (EB_STATE_CODE)lParam;
	//if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
	//	return 0;
	//CString sSoundFile;
	//sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	//sndPlaySound(sSoundFile, SND_ASYNC);

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
	const EB_STATE_CODE nState = (EB_STATE_CODE)pCrFileInfo->GetStateCode();
	if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
		return 0;
	const eb::bigint sCallId = pCrFileInfo->GetCallId();

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
	if (pDlgDialog.get()!=NULL)
	{
		pDlgDialog->OnSave2CloudDrive(pCrFileInfo,nState);
		if (!pDlgDialog->IsWindowVisible())
		{
			CString sSoundFile;
			sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
			sndPlaySound(sSoundFile, SND_ASYNC);
		}
	}

#endif
	return 0;
}
LRESULT CPOPDlg::OnMessageRDRequestResponse(WPARAM wParam, LPARAM lParam)
{
	//CString sSoundFile;
	//sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
	//sndPlaySound(sSoundFile, SND_ASYNC);

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
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("已经在远程桌面中：\r\n请联系对方关闭远程桌面后重试！"),CDlgMessageBox::IMAGE_ERROR,5);
	}
	
	CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo);
	pDlgDialog->RDRequestResponse(pVideoInfo,nState);

	if (!pDlgDialog->IsWindowVisible())
	{
		CString sSoundFile;
		sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
		sndPlaySound(sSoundFile, SND_ASYNC);
	}
#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageRDAckResponse(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageRDRequest(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageRDAccept(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageRDCancel(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageRDEnd(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageVRequestResponse(WPARAM wParam, LPARAM lParam)
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
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("禁止视频会议功能权限：\r\n请联系公司客服！"),CDlgMessageBox::IMAGE_ERROR,5);
	}else if (nState==EB_STATE_EXCESS_QUOTA_ERROR)
	{
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("超过视频会议用户数量：\r\n请联系公司客服！"),CDlgMessageBox::IMAGE_ERROR,5);
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

LRESULT CPOPDlg::OnMessageVAckResponse(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageVideoRequest(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageVideoAccept(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageVideoCancel(WPARAM wParam, LPARAM lParam)
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
LRESULT CPOPDlg::OnMessageVideoTimeout(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageVideoEnd(WPARAM wParam, LPARAM lParam)
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

void CPOPDlg::OnFileManager()
{
	if (m_pDlgFileManager == NULL || !m_pDlgFileManager->IsWindowVisible())
		this->ShowDlgFileManager();
	else if (m_pDlgFileManager->IsWindowVisible())
		m_pDlgFileManager->SetForegroundWindow();

}
LRESULT CPOPDlg::OnMessageFileManager(WPARAM wParam, LPARAM lParam)
{
	OnFileManager();
	return 0;
}

LRESULT CPOPDlg::OnMessageMemberStateChange(WPARAM wParam, LPARAM lParam)
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
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// **跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		CFrameWndInfoProxy::UserLineStateChange(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId, pMemberInfo->m_nLineState);
		return 0;
	}
#endif
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
LRESULT CPOPDlg::OnMessageMemberHeadChange(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_MemberInfo * pEBMemberInfo = (IEB_MemberInfo*)wParam;
	//const bool bIsOwnerMember = lParam==1?true:false;
	// 更新界面用户状况改变
	if (pEBMemberInfo != NULL && m_pDlgMyEnterprise != NULL)
	{
		m_pDlgMyEnterprise->EmployeeInfo(pEBMemberInfo,false);
	}
	if (pEBMemberInfo != NULL && m_pDlgMyGroup != NULL)
	{
		m_pDlgMyGroup->MyDepMemberInfo(pEBMemberInfo,false);
	}
#else
	const EB_MemberInfo * pMemberInfo = (const EB_MemberInfo*)wParam;
	//const bool bIsOwnerMember = lParam==1?true:false;

	// 更新聊天会话列表图标
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		if (m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->MemberHeadChange(pMemberInfo);
		}
	}else if (!theApp.GetHideMainFrame())
	{
		CFrameWndInfoProxy::MemberHeadChange(pMemberInfo);
	}
	// 更新界面用户状况改变
	if (pMemberInfo != NULL && m_pDlgMyEnterprise != NULL)
	{
		m_pDlgMyEnterprise->EmployeeInfo(pMemberInfo,false);
		//m_pDlgMyEnterprise->ChangeEmployeeInfo(pMemberInfo);
	}
	if (pMemberInfo != NULL && m_pDlgMyGroup != NULL)
	{
		m_pDlgMyGroup->MyDepMemberInfo(pMemberInfo,false);
		//m_pDlgMyGroup->ChangeEmployeeInfo(pMemberInfo);
	}
	if (pMemberInfo->m_nMemberUserId==theApp.GetLogonUserId())
	{
		eb::bigint nDefaultMemberCode = 0;
		if (theEBAppClient.EB_GetMyDefaultMemberCode(nDefaultMemberCode) && nDefaultMemberCode==pMemberInfo->m_sMemberCode)
		{
			this->InvalidateRect(&m_rectHead);
		}
	//}else
	//{
	//	// ?
	}
#endif
	return 0;
}
LRESULT CPOPDlg::OnMessageContactHeadChange(WPARAM wParam, LPARAM lParam)
{
	const EB_ContactInfo* pContactInfo = (const EB_ContactInfo*)wParam;
	if (m_pDlgMyContacts!=NULL)
	{
		m_pDlgMyContacts->ContactHeadChange(pContactInfo);
	}
	if (pContactInfo->m_nContactUserId>0)
	{
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			if (m_pDlgFrameList!=NULL)
			{
				m_pDlgFrameList->UserHeadChange(pContactInfo);
			}
		}else if (!theApp.GetHideMainFrame())
		{
			CFrameWndInfoProxy::UserHeadChange(pContactInfo);
		}
	}
	return 0;
}
void CPOPDlg::SaveCallRecord(eb::bigint sCallId,eb::bigint sGroupCode,const EB_AccountInfo& pFromAccountInfo)
{
	eb::bigint sEmpCode = pFromAccountInfo.m_pFromCardInfo.m_sMemberCode;
	CEBString sDepName = pFromAccountInfo.m_pFromCardInfo.m_sGroupName;
	CEBString sFromName = pFromAccountInfo.m_pFromCardInfo.m_sName;
	int nFromType = (int)pFromAccountInfo.m_pFromCardInfo.m_nAccountType;
	if (nFromType==(int)EB_ACCOUNT_TYPE_VISITOR)
	{
		sFromName = pFromAccountInfo.m_pFromCardInfo.m_sAddress;
	}
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
	if (this->m_pDlgMySession != NULL && m_pDlgMySession->GetSafeHwnd() && (sGroupCode>0 || !is_visitor_uid(pFromAccountInfo.GetUserId())))
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
		}else if (sGroupCode>0)
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE call_id=%lld"),sGroupCode);
			theApp.m_pBoUsers->execute(sSql);
		}else
		{
			sSql.Format(_T("DELETE FROM call_record_t WHERE from_uid=%lld AND dep_code=0"),pFromAccountInfo.GetUserId());
			theApp.m_pBoUsers->execute(sSql);
		}
	}

	// ** 必须放前面
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
		m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true);
	}
	// ** 必须放后面
	sSql.Format(_T("INSERT INTO call_record_t(call_id,dep_code,dep_name,emp_code,from_uid,from_phone,from_name,from_type,company,title,tel,email) ")\
		_T("VALUES(%lld,%lld,'%s',%lld,%lld,%lld,'%s',%d,'%s','%s','%s','%s')"),
		sCallId,sGroupCode,libEbc::ACP2UTF8(sDepName.c_str()).c_str(),sEmpCode,pFromAccountInfo.GetUserId(),sPhone,libEbc::ACP2UTF8(sFromName.c_str()).c_str(),nFromType,
		libEbc::ACP2UTF8(sCompany.c_str()).c_str(),libEbc::ACP2UTF8(sTitle.c_str()).c_str(),sTel.c_str(),sEmail.c_str());
	theApp.m_pBoUsers->execute(sSql);
}

LRESULT CPOPDlg::OnMessageCallConnected(WPARAM wParam, LPARAM lParam)
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
			const bool bAutoCall = pConnectInfo->m_sGroupCode>0?theApp.m_pAutoCallGroupIdList.exist(pConnectInfo->m_sGroupCode):theApp.m_pAutoCallFromUserIdList.exist(pConnectInfo->GetFromUserId());
			const bool bShow = bOwnerCall;
			CDlgDialog::pointer pDlgDialog = GetDlgDialog(pEbCallInfo,bShow,bOwnerCall);
			if (bOwnerCall && !bAutoCall)
			{
				//// **关闭聊天提示窗口1
				//if (m_pDlgMsgTip!=NULL)
				//{
				//	m_pDlgMsgTip->DelMsgTip(pEbCallInfo->m_pCallInfo.m_sGroupCode,pEbCallInfo->m_pFromAccountInfo.GetUserId());
				//}
				
				// **关闭聊天提示窗口2
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
	
				if (m_pDlgFrameList!=NULL)
				{
					if (m_pDlgFrameList->IsIconic())
						m_pDlgFrameList->ShowWindow(SW_RESTORE);
					m_pDlgFrameList->SetForegroundWindow();
				//}else if (!theApp.GetHideMainFrame())
				//{
				//	if (this->IsIconic())
				//		this->ShowWindow(SW_RESTORE);
				//	this->SetForegroundWindow();
				}

			//}else if (bConnectMsg && !pDlgDialog->IsWindowVisible())
			//{
			//	pDlgDialog->ShowWindow(SW_SHOWNORMAL);
			}else
			{
				//::FlashWindow(pDlgDialog->GetSafeHwnd(), TRUE);
			}
			
			if (bAutoCall)
			{
				CString sSql;
				if (pConnectInfo->m_sGroupCode>0)
					sSql.Format(_T("SELECT count(msg_id) FROM msg_record_t WHERE dep_code=%lld AND (read_flag&1)=0"),pConnectInfo->m_sGroupCode);
				else
					sSql.Format(_T("SELECT count(msg_id) FROM msg_record_t WHERE from_uid=%lld AND dep_code=0 AND (read_flag&1)=0"),pConnectInfo->GetFromUserId());
				int nCookie = 0;
				theApp.m_pBoUsers->select(sSql,nCookie);
				cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
				if (pRecord.get()!=NULL)
				{
					const int nUnReadMsgCount = pRecord->getVector()[0]->getIntValue();
					theApp.m_pBoUsers->reset(nCookie);

#ifdef USES_SUPPORT_UI_STYLE
					const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
					if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
					{
						if (m_pDlgFrameList!=NULL)
							m_pDlgFrameList->SetUnreadMsg(sCallId,nUnReadMsgCount);
					}else
#endif
					{
						this->SetUnreadMsg(sCallId,nUnReadMsgCount);
					}
				}
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

#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// *跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif	// USES_SUPPORT_UI_STYLE
	{
		if (!CFrameWndInfoProxy::ExistCallIdWnd(sCallId))
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

LRESULT CPOPDlg::OnMessageCallHangup(WPARAM wParam, LPARAM lParam)
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
	const bool bOwner = (bool)(lParam==1);

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

LRESULT CPOPDlg::OnMessageCallError(WPARAM wParam, LPARAM lParam)
{
	sndPlaySound(NULL,SND_NODEFAULT);
	const EB_STATE_CODE nResultValue = (EB_STATE_CODE)lParam;
	if (EB_STATE_APPID_KEY_ERROR==nResultValue ||
		EB_STATE_APP_ONLINE_KEY_TIMEOUT==nResultValue)
		return 0;

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
		if (!pCallInfo->m_bOwner)
			return;
		sErrorText = _T("会话超时！");
	}else if (nResultValue == EB_STATE_USER_HANGUP)
	{
		DeleteDlgIncomingCall(nFromUserId);
		sErrorText = _T("用户取消会话！");
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
		sErrorText.Format(_T("请求会话失败 %d"),nResultValue);
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
		sErrorText.Format(_T("用户：\r\n%s 不存在！"),pCallInfo->GetFromAccount().c_str());
	else if (nResultValue == EB_STATE_USER_OFFLINE)
		sErrorText.Format(_T("用户：\r\n%s 离线状态！"),pCallInfo->GetFromAccount().c_str());
	else if (nResultValue == EB_STATE_UNAUTH_ERROR)
		sErrorText.Format(_T("帐号：\r\n%s 未通过验证！"),pCallInfo->GetFromAccount().c_str());
	else if (nResultValue == EB_STATE_ACCOUNT_FREEZE)
		sErrorText.Format(_T("帐号：\r\n%s 已被临时冻结！"),pCallInfo->GetFromAccount().c_str());
	else if (nResultValue == EB_STATE_MAX_RETRY_ERROR)
		sErrorText = _T("错误次数太频繁：\r\n帐号已被临时锁住，请稍候再试！");
	else if (nResultValue == EB_STATE_USER_BUSY)
	{
		DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
		sErrorText.Format(_T("用户：\r\n%s 忙！"),pCallInfo->GetFromAccount().c_str());
	}else if (nResultValue == EB_STATE_TIMEOUT_ERROR)
	{
		// 本地太久未响应，也会有该事件
		DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
		if (pCallInfo->GetFromUserId() == 0 &&  pCallInfo->m_sGroupCode == 0) {
			return 0;
		}
		sErrorText.Format(_T("用户：\r\n%s 会话超时！\r\n正在自动重连，请稍候再试！"),pCallInfo->GetFromAccount().c_str());
	}else if (nResultValue == EB_STATE_USER_HANGUP)
	{
		DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
		sErrorText.Format(_T("用户：\r\n%s 取消会话！"),pCallInfo->GetFromAccount().c_str());
	}else if (nResultValue == EB_STATE_ACCOUNT_DISABLE_OFFCALL)
	{
		sErrorText.Format(_T("用户：\r\n%s 离线，对方屏蔽接收离线消息功能！"),pCallInfo->GetFromAccount().c_str());
	}else if (nResultValue==EB_STATE_ACCOUNT_DISABLE_EXTCALL)
	{
		sErrorText.Format(_T("用户：\r\n%s 屏蔽外部聊天功能！"),pCallInfo->GetFromAccount().c_str());
	}else if (nResultValue==EB_STATE_OAUTH_FORWARD)
	{
		sErrorText.Format(_T("操作成功，等待对方：\r\n%s 通过验证！"),pCallInfo->GetFromAccount().c_str());
	}else
	{
		sErrorText.Format(_T("请求会话：\r\n%s 失败，错误代码：%d"),pCallInfo->GetFromAccount().c_str(),nResultValue);
	}

	EB_AccountInfo pFromAccountInfo;
	if (theEBAppClient.EB_GetCallAccountInfo(sCallId,pCallInfo->GetFromUserId(),&pFromAccountInfo))
	{
		SaveCallRecord(sCallId,pCallInfo->m_sGroupCode,pFromAccountInfo);
		//m_btnMySession.SetWindowText(_T("！"));
	}

	// ??
	//if (!pCallInfo->m_bOwner)
	{
		CDlgMessageBox::EbMessageBox(this,"",sErrorText,CDlgMessageBox::IMAGE_WARNING,5);
	}
#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageAlertingCall(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageIncomingCall(WPARAM wParam, LPARAM lParam)
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

	CDlgIncomingCall::pointer pDlgIncomingCall = CDlgIncomingCall::create(this);
	pDlgIncomingCall->m_pFromAccountInfo.operator =(pFromAccount);
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

LRESULT CPOPDlg::OnMessageUGInfo(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageUGDelete(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageContactInfo(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageContactDelete(WPARAM wParam, LPARAM lParam)
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
LRESULT CPOPDlg::OnMessageContactStateChange(WPARAM wParam, LPARAM lParam)
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
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// **跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		CFrameWndInfoProxy::UserLineStateChange(0, pPopContactInfo->m_nContactUserId, pPopContactInfo->m_nLineState);
		return 0;
	}
#endif
	if (m_pDlgFrameList!=NULL)
	{
		m_pDlgFrameList->UserLineStateChange(0, pPopContactInfo->m_nContactUserId, pPopContactInfo->m_nLineState);
	}

#endif

	return 0;
}

LRESULT CPOPDlg::OnMessageEnterpriseInfo(WPARAM wParam, LPARAM lParam)
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
	theApp.SetEnterpriseCreateUserId(pEnterpriseInfo->m_nCreateUserId);
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

LRESULT CPOPDlg::OnMessageGroupInfo(WPARAM wParam, LPARAM lParam)
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
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// **跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		CFrameWndInfoProxy::ChangeDepartmentInfo(pGroupInfo);
		return 0;
	}
#endif
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->ChangeDepartmentInfo(pGroupInfo);

	//CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode);
	//if (pDlgDialog.get()!=NULL)
	//{
	//	// 群资料已经修改
	//	pDlgDialog->ChangeDepartmentInfo(pGroupInfo);
	//}
#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageGroupDelete(WPARAM wParam, LPARAM lParam)
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

#ifdef USES_NEW_UI_1220
	if (!theApp.GetHideMainFrame())
	{
		CFrameWndInfoProxy::OnRemoveGroup(sGroupCode);
	}else if (m_pDlgFrameList!=NULL)
	{
		m_pDlgFrameList->OnRemoveGroup(sGroupCode);
	}
#else
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->OnRemoveGroup(sGroupCode);
#endif

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sGroupCode, true);
	if (pDlgDialog.get()!=NULL)
	{
	}

	CString stext;
	stext.Format(_T("解散[%s]"), sGroupName.c_str());
	CDlgMessageBox::EbMessageBox(this,_T("管理员"),stext,CDlgMessageBox::IMAGE_WARNING,8);

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
	//bool bIsMyDepartment = (bool)(lParam==1);
	if (m_pDlgMyEnterprise != NULL)
	{
		m_pDlgMyEnterprise->DeleteDepartmentInfo(pGroupInfo->m_sGroupCode);
	}
	if (m_pDlgMyGroup != NULL)
		m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);

	// 关闭现在会话窗口
	theApp.m_pCallList.remove(pGroupInfo->m_sGroupCode);

#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
	{
		m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		CFrameWndInfoProxy::OnRemoveGroup(pGroupInfo->m_sGroupCode);
	}else if (m_pDlgFrameList!=NULL)
	{
		m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);
	}
#else
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);
#endif

	CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode, true);
	if (pDlgDialog.get()!=NULL)
	{
	}
	CString stext;
	stext.Format(_T("%s\r\n已经被解散！"), pGroupInfo->m_sGroupName.c_str());
	CDlgMessageBox::EbMessageBox(this,_T(""),stext,CDlgMessageBox::IMAGE_INFORMATION,8);

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
LRESULT CPOPDlg::OnMessageGroupEditResponse(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_GroupInfo * pGroupInfo = (IEB_GroupInfo*)wParam;
	EB_STATE_CODE nErrorCode = (EB_STATE_CODE)lParam;
#else
	const EB_GroupInfo* pGroupInfo = (const EB_GroupInfo*)wParam;
	EB_STATE_CODE nErrorCode = (EB_STATE_CODE)lParam;
#endif
	switch (nErrorCode)
	{
	case EB_STATE_OK:
		break;
	case EB_STATE_NOT_AUTH_ERROR:
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("没有权限：\r\n请联系管理员！"),CDlgMessageBox::IMAGE_ERROR,5);
		break;
	default:
		{
			CString sText;
			sText.Format(_T("错误代码：%d\r\n请重试或联系管理员！"),(int)nErrorCode);
			CDlgMessageBox::EbMessageBox(this,_T("编辑失败"),sText,CDlgMessageBox::IMAGE_ERROR,5);
		}break;
	}
	return 0;
}

LRESULT CPOPDlg::OnMessageRemoveGroup(WPARAM wParam, LPARAM lParam)
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
#ifdef USES_NEW_UI_1220
		if (!theApp.GetHideMainFrame())
		{
			CFrameWndInfoProxy::OnRemoveGroup(sGroupCode);
		}else if (m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveGroup(sGroupCode);
		}
#else
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveGroup(sGroupCode);
#endif

		CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sGroupCode,true);
		if (pDlgDialog.get()!=NULL)
		{
		}
		if (m_pDlgMyGroup!=NULL)
			m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);
		stext.Format(_T("你被管理员移除出%s"), sGroupName.c_str());
		CDlgMessageBox::EbMessageBox(this,"",stext,CDlgMessageBox::IMAGE_WARNING,8);
	}else
	{
#ifdef USES_NEW_UI_1220
		if (!theApp.GetHideMainFrame())
		{
			CFrameWndInfoProxy::OnRemoveMember(sGroupCode, sRemoveMemberCode);
		}else if (m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveMember(sGroupCode, sRemoveMemberCode);
		}
#else
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveMember(sGroupCode, sRemoveMemberCode);
#endif

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
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			CFrameWndInfoProxy::OnRemoveGroup(pGroupInfo->m_sGroupCode);
		}else if (m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);
		}
#else
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);
#endif

		//CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode,true);
		//if (pDlgDialog.get()!=NULL)
		//{
		//}
		if (m_pDlgMyGroup!=NULL)
		{
			m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);
		}
		stext.Format(_T("你被管理员移除出：\r\n%s"), pGroupInfo->m_sGroupName.c_str());
		CDlgMessageBox::EbMessageBox(this,"",stext,CDlgMessageBox::IMAGE_INFORMATION,5);
	}else
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			CFrameWndInfoProxy::OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
		}else if (m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
		}
#else
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
#endif

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

LRESULT CPOPDlg::OnMessageExitGroup(WPARAM wParam, LPARAM lParam)
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
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			CFrameWndInfoProxy::OnRemoveGroup(pGroupInfo->m_sGroupCode);
		}else if (m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);
		}
#else
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveGroup(pGroupInfo->m_sGroupCode);
#endif

		///  没用代码
		//GetCallIdDialog(pGroupInfo->m_sGroupCode,true);

		//CDlgDialog::pointer pDlgDialog = GetCallIdDialog(pGroupInfo->m_sGroupCode,true);
		//if (pDlgDialog.get()!=NULL)
		//{
		//	pDlgDialog->UserExitRoom(pMemberInfo->m_nMemberUserId,true);
		//}
		if (m_pDlgMyEnterprise != NULL)
			m_pDlgMyEnterprise->DeleteEmployeeInfo(pGroupInfo,pMemberInfo->m_sMemberCode);
		if (m_pDlgMyGroup)
			m_pDlgMyGroup->DeleteDepartmentInfo(pGroupInfo);
		return 0;
	}else
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			CFrameWndInfoProxy::OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
		}else if (m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
		}
#else
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
#endif

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
		CString sText;
		sText.Format(_T("%s(%s)\r\n已经退出：\r\n%s"),pMemberInfo->m_sUserName.c_str(),pMemberInfo->m_sMemberAccount.c_str(),pGroupInfo->m_sGroupName.c_str());
		CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_INFORMATION, 5);
	}
#endif
	return 0;
}
LRESULT CPOPDlg::OnMessageRequestJoin2Group(WPARAM wParam, LPARAM lParam)
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
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>申请加入群组：%lld<br/>附加消息：%s</font>"), sRequestAccount.c_str(),nGroupId,sDescription.substr(0,60).c_str());
		//sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>申请加入群组：%lld<br/>附加消息：%s</font><br/>\
		//				   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"), sRequestAccount.c_str(),nGroupId,sDescription.substr(0,30).c_str(),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->SetShowMyMsgSubId(theApp.GetGroupMsgSugId(),"tab_type=sys_msg");
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
LRESULT CPOPDlg::OnMessageInviteJoin2Group(WPARAM wParam, LPARAM lParam)
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
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>邀请你加入群组：%s<br/>附加消息：%s</font>"), sRequestAccount.c_str(),sGroupName,sMsgContent.Left(60));
		//sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>邀请你加入群组：%s<br/>附加消息：%s</font><br/>\
		//				   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"), sRequestAccount.c_str(),sGroupName,sMsgContent.Left(20),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->SetShowMyMsgSubId(theApp.GetGroupMsgSugId(),"tab_type=sys_msg");
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

LRESULT CPOPDlg::OnMessageRejectJoin2Group(WPARAM wParam, LPARAM lParam)
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
	//	m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true,true);
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
	//	m_pDlgMySession->InsertCallRecord(pCallRecordInfo,true,true);
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

LRESULT CPOPDlg::OnMessageMemberInfo(WPARAM wParam, LPARAM lParam)
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
#ifdef USES_NEW_UI_160111
		CRect rect(m_rectHead);
		rect.right += 120;
		this->InvalidateRect(rect);	// 刷新重画自己头像
#else
		this->Invalidate();	// 刷新重画自己头像
#endif

		CEBString sSettingEnterprise;
		if (!theApp.GetProductName().IsEmpty())
			sSettingEnterprise = (LPCTSTR)theApp.GetProductName();
		else
			sSettingEnterprise = theSetting.GetEnterprise();
		CString sWindowText;
		tstring sEnterpriseName;
		if (theEBAppClient.EB_GetEnterpriseName(sEnterpriseName))
		{
			sWindowText.Format(_T("%s-%s(%s) %s"),sSettingEnterprise.c_str(),pMemberInfo->m_sUserName.c_str(),
				pMemberInfo->m_sMemberAccount.c_str(),sEnterpriseName.c_str());
		}else
		{
			sWindowText.Format(_T("%s-%s(%s)"),sSettingEnterprise.c_str(),pMemberInfo->m_sUserName.c_str(),
				pMemberInfo->m_sMemberAccount.c_str());
		}
		this->SetWindowText(sWindowText);
	}
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// **跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		CFrameWndInfoProxy::OnUserEmpInfo(pMemberInfo,true);
		return 0;
	}
#endif
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->OnUserEmpInfo(pMemberInfo,true);
	//CDlgDialog::pointer pDlgDialog;
	//if (theApp.m_pDialogList.find(pMemberInfo->m_sGroupCode, pDlgDialog))
	//{
	//	pDlgDialog->OnUserEmpInfo(pMemberInfo);
	//}
#endif
	return 0;
}

//LRESULT CPOPDlg::OnMessageMemberDelete(WPARAM wParam, LPARAM lParam)
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
LRESULT CPOPDlg::OnMessageMemberEditResponse(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_EBCOM_TEST
	IEB_MemberInfo* pMemberInfo = (IEB_MemberInfo*)wParam;
	EB_STATE_CODE nErrorCode = (EB_STATE_CODE)lParam;
#else
	const EB_MemberInfo* pMemberInfo = (const EB_MemberInfo*)wParam;
	const EB_STATE_CODE nErrorCode = (EB_STATE_CODE)lParam;
#endif
	switch (nErrorCode)
	{
	case EB_STATE_OK:
		break;
	case EB_STATE_OAUTH_FORWARD:
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("邀请成员成功：\r\n等待对方通过验证！"),CDlgMessageBox::IMAGE_INFORMATION,5);
		break;
	case EB_STATE_NOT_AUTH_ERROR:
		{
			if (libEbc::IsFullNumber(pMemberInfo->m_sMemberAccount.c_str(),pMemberInfo->m_sMemberAccount.size()))
				CDlgMessageBox::EbMessageBox(this,_T(""),_T("不支持全数字帐号注册：\r\n请更换帐号后重试！"),CDlgMessageBox::IMAGE_ERROR,5);
			else
				CDlgMessageBox::EbMessageBox(this,_T(""),_T("没有权限：\r\n请联系管理员！"),CDlgMessageBox::IMAGE_ERROR,5);
		}break;
	case EB_STATE_ACCOUNT_ALREADY_EXIST:
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("帐号已经存在：\r\n请重新输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		break;
	case EB_STATE_ANOTHER_ENT_ACCOUNT:
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("这是其他企业员工帐号：\r\n请重新输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		break;
	default:
		{
			CString sText;
			sText.Format(_T("错误代码：%d\r\n请重试或联系管理员！"),(int)nErrorCode);
			CDlgMessageBox::EbMessageBox(this,_T("编辑失败"),sText,CDlgMessageBox::IMAGE_ERROR,5);
		}break;
	}
	return 0;
}
LRESULT CPOPDlg::OnMessageRequestAddContact(WPARAM wParam, LPARAM lParam)
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
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>申请加为好友<br/>附加消息：%s</font>"), sRequestAccount.c_str(),sDescription.substr(0,60).c_str());
		//sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>申请加为好友<br/>附加消息：%s</font><br/>\
		//				   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"), sRequestAccount.c_str(),sDescription.substr(0,30).c_str(),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->SetShowMyMsgSubId(theApp.GetGroupMsgSugId(),"tab_type=sys_msg");
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
LRESULT CPOPDlg::OnMessageRejectAddContact(WPARAM wParam, LPARAM lParam)
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
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>拒绝加为好友<br/>附加消息：%s</font>"), sRequestAccount.c_str(),sDescription.substr(0,60).c_str());
		//sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>拒绝加为好友<br/>附加消息：%s</font><br/>\
		//				   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"), sRequestAccount.c_str(),sDescription.substr(0,30).c_str(),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->SetShowMyMsgSubId(theApp.GetGroupMsgSugId(),"tab_type=sys_msg");
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
LRESULT CPOPDlg::OnMessageAcceptAddContact(WPARAM wParam, LPARAM lParam)
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
		sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>添加好友成功</font>"),sName.c_str());
		//sContent.Format(_T("<font color=\"#6c6c6c\">用户：%s<br/>添加好友成功</font><br/>\
		//				   <a href=\"eb-open-subid://%lld,tab_type=sys_msg\">查看我的消息</a>"),sName.c_str(),theApp.GetGroupMsgSugId());
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bBroadcastMsg = true;
		pFuncWindow->SetShowMyMsgSubId(theApp.GetGroupMsgSugId(),"tab_type=sys_msg");
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


//LRESULT CPOPDlg::OnMessageAppMsgInfo(WPARAM wParam, LPARAM lParam)
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

LRESULT CPOPDlg::OnMessageResourceInfo(WPARAM wParam, LPARAM lParam)
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
	if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
	{
		sId = pResourceInfo->m_nFromId;
	}else if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_ENT)
	{
		sId = pResourceInfo->m_nFromId;
	}else
	{
		sId = theEBAppClient.EB_GetLogonUserId();
	}
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (theApp.m_pResourceMgr.find(sId,pDlgResourceMgr))
	{
		pDlgResourceMgr->OnResourceInfo(*pResourceInfo);
	}
	if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
	//if (pResourceInfo->m_sGroupCode>0)
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			// **跑下面
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			CFrameWndInfoProxy::OnResourceInfo(*pResourceInfo);
			return 0;
		}
#endif
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnResourceInfo(*pResourceInfo);
	}
#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageDeleteResource(WPARAM wParam, LPARAM lParam)
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
	if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
	//if (pResourceInfo->m_sGroupCode>0)
	{
		sId = pResourceInfo->m_nFromId;
	}else if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_ENT)
	//}else if (pResourceInfo->m_sEnterpriseCode>0)
	{
		sId = pResourceInfo->m_nFromId;
	}else
	{
		sId = theEBAppClient.EB_GetLogonUserId();
	}
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (theApp.m_pResourceMgr.find(sId,pDlgResourceMgr))
	{
		pDlgResourceMgr->OnResourceDelete(*pResourceInfo);
	}
	if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
	//if (pResourceInfo->m_sGroupCode>0)
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			// **跑下面
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			CFrameWndInfoProxy::OnResourceDelete(*pResourceInfo);
			return 0;
		}
#endif
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnResourceDelete(*pResourceInfo);
	}

#endif
	return 0;
}

LRESULT CPOPDlg::OnMessageMoveResource(WPARAM wParam, LPARAM lParam)
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
	const eb::bigint nOldParentResId = eb_atoi64((const char*)lParam);
	eb::bigint sId = 0;
	if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
	//if (pResourceInfo->m_sGroupCode>0)
	{
		sId = pResourceInfo->m_nFromId;
	}else if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_ENT)
	//}else if (pResourceInfo->m_sEnterpriseCode>0)
	{
		sId = pResourceInfo->m_nFromId;
	}else
	{
		sId = theEBAppClient.EB_GetLogonUserId();
	}
	CDlgResourceMgr * pDlgResourceMgr = NULL;
	if (theApp.m_pResourceMgr.find(sId,pDlgResourceMgr))
	{
		pDlgResourceMgr->OnResourceMove(*pResourceInfo,nOldParentResId);
	}
	if (pResourceInfo->m_nFromId>0 && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
	//if (pResourceInfo->m_sGroupCode>0)
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			// **跑下面
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			CFrameWndInfoProxy::OnResourceMove(*pResourceInfo,nOldParentResId);
			return 0;
		}
#endif
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnResourceMove(*pResourceInfo,nOldParentResId);
	}

#endif
	return 0;
}

CDlgDialog::pointer CPOPDlg::GetDlgDialog(const CEBCCallInfo::pointer & pEbCallInfo,bool bShow,bool bOwnerCall)
{
	//theApp.LogMessage("CPOPDlg::GetDlgDialog... (callid=%lld,show=%d,ownercall=%d)\r\n",pEbCallInfo->m_pCallInfo.GetCallId(),(int)(bShow?1:0),(int)(bOwnerCall?1:0));
	const bool bAutoCall = pEbCallInfo->m_pCallInfo.m_sGroupCode>0?theApp.m_pAutoCallGroupIdList.exist(pEbCallInfo->m_pCallInfo.m_sGroupCode):theApp.m_pAutoCallFromUserIdList.exist(pEbCallInfo->m_pFromAccountInfo.GetUserId());
	CreateFrameList(bShow);
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
#endif
	const eb::bigint sCallId = pEbCallInfo->m_pCallInfo.GetCallId();
	CDlgDialog::pointer pDlgDialog;
	if (pEbCallInfo->m_pCallInfo.m_sOldCallId>0)
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			pDlgDialog = m_pDlgFrameList->GetCallIdDialog(pEbCallInfo->m_pCallInfo.m_sOldCallId,false,bAutoCall);
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			pDlgDialog = CFrameWndInfoProxy::GetCallIdDialog(pEbCallInfo->m_pCallInfo.m_sOldCallId,false,bAutoCall);
		}else if (m_pDlgFrameList!=NULL)
		{
			pDlgDialog = m_pDlgFrameList->GetCallIdDialog(pEbCallInfo->m_pCallInfo.m_sOldCallId,false,bAutoCall);
		}
#else
		pDlgDialog = m_pDlgFrameList->GetCallIdDialog(pEbCallInfo->m_pCallInfo.m_sOldCallId);
#endif
		if (pDlgDialog.get()!=NULL)
		{
			// 找到old call id
			pDlgDialog->SetCallInfo(pEbCallInfo);
		}
	}
	if (pDlgDialog.get()==NULL)
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			pDlgDialog = m_pDlgFrameList->GetCallIdDialog(sCallId,false,bAutoCall);
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			pDlgDialog = CFrameWndInfoProxy::GetCallIdDialog(sCallId,false,bAutoCall);
		}else if (m_pDlgFrameList!=NULL)
		{
			pDlgDialog = m_pDlgFrameList->GetCallIdDialog(sCallId,false,bAutoCall);
		}
#else
		pDlgDialog = m_pDlgFrameList->GetCallIdDialog(sCallId);
#endif
		//theApp.LogMessage("CPOPDlg::GetDlgDialog->CFrameWndInfoProxy::GetCallIdDialog(=%x)... (callid=%lld,show=%d,ownercall=%d)\r\n",(int)pDlgDialog.get(),pEbCallInfo->m_pCallInfo.GetCallId(),(int)(bShow?1:0),(int)(bOwnerCall?1:0));
		if (pDlgDialog.get()==NULL)
		{
#ifdef USES_NEW_UI_1220
			//if (!this->IsIconic())
			{
#ifdef USES_SUPPORT_UI_STYLE
				if (nDefaultUIStyleType!=EB_UI_STYLE_TYPE_CHAT)
#endif
				{
					const int nLimitWidth = const_border_left+DEFAULT_P2P_WIDTH;
					CRect rect;
					this->GetWindowRect(&rect);
					if (rect.Width()< nLimitWidth)
					{
						if (m_rectAdjustOld.Width()==0)
							m_rectAdjustOld = rect;
						rect.right = rect.left + nLimitWidth;
						this->MoveWindow(&rect);
					}
				}
			}
#ifdef USES_SUPPORT_UI_STYLE
			//if (nDefaultUIStyleType!=EB_UI_STYLE_TYPE_CHAT)
			CWnd * pParent = (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT||theApp.GetHideMainFrame())?m_pDlgFrameList:(CWnd*)this;
#else
			CWnd * pParent = theApp.GetHideMainFrame()?m_pDlgFrameList:(CWnd*)this;
#endif
#else
			CWnd * pParent = m_pDlgFrameList;
#endif
			pDlgDialog = CDlgDialog::create(pParent);
			pDlgDialog->SetOwnerCall(bOwnerCall);
			pDlgDialog->SetCallInfo(pEbCallInfo);
			pDlgDialog->Create(CDlgDialog::IDD, pParent);
			//theApp.LogMessage("CPOPDlg::GetDlgDialog->CFrameWndInfo::create and AddWnd(=%x)... (callid=%lld,show=%d,ownercall=%d)\r\n",(int)pDlgDialog.get(),pEbCallInfo->m_pCallInfo.GetCallId(),(int)(bShow?1:0),(int)(bOwnerCall?1:0));
			CFrameWndInfo::pointer pFrameWndInfo = CFrameWndInfo::create(true,pDlgDialog);
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
			if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
			{
				m_pDlgFrameList->AddWnd(pFrameWndInfo, bShow, bAutoCall);
			}else if (!theApp.GetHideMainFrame())
#else
			if (!theApp.GetHideMainFrame())
#endif
			{
				CFrameWndInfoProxy::AddWnd(pFrameWndInfo, bShow,bAutoCall);
			}else
			{
				m_pDlgFrameList->AddWnd(pFrameWndInfo, bShow, bAutoCall);
			}
#else
			m_pDlgFrameList->AddWnd(pFrameWndInfo, bShow, bAutoCall);
#endif
		}else if (pEbCallInfo->m_pCallInfo.m_sOldCallId!=pDlgDialog->GetCallInfo()->m_pCallInfo.m_sOldCallId)
		{
			// 更新old call id
			pDlgDialog->SetCallInfo(pEbCallInfo);
		}
	}
	
	if (bShow && !bAutoCall)
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			m_pDlgFrameList->ShowWnd(pEbCallInfo->m_pCallInfo.GetCallId());
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			CFrameWndInfoProxy::ShowWnd(pEbCallInfo->m_pCallInfo.GetCallId());
			if (bOwnerCall)
			{
				if (this->IsIconic())
					this->ShowWindow(SW_RESTORE);
				else if (!this->IsWindowVisible())
					this->ShowWindow(SW_SHOW);
				this->SetForegroundWindow();
			}
		}else
		{
			m_pDlgFrameList->ShowWnd(pEbCallInfo->m_pCallInfo.GetCallId());
		}
#else
		m_pDlgFrameList->ShowWnd(pEbCallInfo->m_pCallInfo.GetCallId());
#endif

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

LRESULT CPOPDlg::OnMessageHotKey(WPARAM wParam, LPARAM lParam)
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
		if (m_pDlgMsgTip!=NULL && !m_pDlgMsgTip->IsMsgEmpty())
		{
			m_pDlgMsgTip->ViewAllMsg();
			return 0;
		}

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
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnExitRD();
		}else if (!theApp.GetHideMainFrame())
#else
		if (!theApp.GetHideMainFrame())
#endif
		{
			CFrameWndInfoProxy::OnExitRD();
		}else if (m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->OnExitRD();
		}
#else
		if (m_pDlgFrameList!=NULL)
			m_pDlgFrameList->OnExitRD();
#endif
	}
	return 0;
}

//LRESULT CPOPDlg::OnMessageWindowResize(WPARAM wParam, LPARAM lParam)
//{
//	SetCircle();
//	this->Invalidate();
//	return 0;
//}


//void CPOPDlg::OnBnClickedButtonSelectuser()
//{
//	CDlgDiaRecord pDlgCallNumber(this);
//	if (pDlgCallNumber.DoModal() == IDOK)
//	{
//		m_sCallNumber = pDlgCallNumber.m_sCallNumber;
//		UpdateData(FALSE);
//		m_comboNumbers.InsertString(0, m_sCallNumber);
//	}
//}
void CPOPDlg::InitData(void)
{
	//
}


//void CPOPDlg::OnCbnSelchangeComboNumbers()
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

const int const_bottom_intever = 28;
void CPOPDlg::MoveSize(int cx, int cy)
{
	//theApp.LogMessage("CPOPDlg::MoveSize... (cx=%d,cy=%d)\r\n",cx,cy);
	if (cx==0 || cy==0) return;

#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
#endif

#ifdef USES_NEW_UI_160111
	//const bool bShowToolBar = true;
	int nBorderLeft = m_bShowedLeft?const_border_left:const_hide_border_left1;
	int y = NEW_UI_160111_HEIGHT+2;
	int x = nBorderLeft+2;
#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		m_btnLineState.MovePoint(UISTYLE_CHAT_POS_LINESTATE_LEFT,UISTYLE_CHAT_POS_LINESTATE_TOP);
		if (this->GetDlgItem(IDC_EDIT_DESCRIPTION)->GetSafeHwnd()!=NULL)
		{
			y = UISTYLE_CHAT_POS_LINESTATE_TOP+18;
			this->GetDlgItem(IDC_EDIT_DESCRIPTION)->MoveWindow(UISTYLE_CHAT_POS_LINESTATE_LEFT,y,cx-(UISTYLE_CHAT_POS_LINESTATE_LEFT)-3,18);
		}
		nBorderLeft = 1;
		x = UISTYLE_CHAT_POS_LINESTATE_LEFT+2;
		y = UISTYLE_CHAT_POS_ADIMG_TOP+UISTYLE_CHAT_POS_ADIMG_SIGE-m_btnMyCenter.GetImgHeight();
	}else
#endif // USES_SUPPORT_UI_STYLE
		m_btnLineState.MovePoint(POS_LINESTATE_LEFT,POS_LINESTATE_TOP);
#else
	const bool bShowToolBar = m_bShowedToolbar&&GetMainFrameShowed();
	int y = bShowToolBar?POS_LINESTATE_TOP:3;
	m_btnLineState.MovePoint(POS_LINESTATE_LEFT,y);
	//if (m_btnLineState.GetSafeHwnd()!=NULL)
	//	m_btnLineState.ShowWindow(bShowToolBar?SW_SHOW:SW_HIDE);
	if (bShowToolBar && this->GetDlgItem(IDC_EDIT_DESCRIPTION)->GetSafeHwnd()!=NULL)
	{
		y += 22;
		this->GetDlgItem(IDC_EDIT_DESCRIPTION)->MoveWindow(POS_LINESTATE_LEFT,y,cx-(POS_LINESTATE_LEFT)-3,18);
	}
	if (bShowToolBar)
		y += 20;
	int x = (bShowToolBar?(POS_LINESTATE_LEFT):(const_border_left+7));
#endif
	m_btnMyCenter.MovePoint(x,y);
	x += m_btnMyCenter.GetImgWidth();
	m_btnFileMgr.MovePoint(x,y);
	if (!theApp.GetDisableUserCloudDrive() && !theApp.IsLogonVisitor())
	{
		x += m_btnMyCenter.GetImgWidth();
		m_btnMyShare.MovePoint(x,y);
	}
	x += m_btnMyCenter.GetImgWidth();

#ifdef USES_NEW_UI_160111
	m_btnMainFunc.MovePoint(x,y);
	//m_btnMainFunc.MovePoint(nBorderLeft,12);
	y = NEW_UI_160111_HEIGHT+29;
#else
	m_btnMainFunc.MovePoint(x,y);
#endif
	x += m_btnMyCenter.GetImgWidth()+10;
	m_btnGoBack.MovePoint(x,y+2);
	x += m_btnMyCenter.GetImgWidth();
	m_btnGoForward.MovePoint(x,y+2);

	const int const_text_intever = 1;
	const int const_number_height = 22;
	const int const_leftr_tool_width = 0;
#ifndef USES_NEW_UI_160111
	y += (m_btnMainFunc.GetImgHeight()+(bShowToolBar?6:3));
#endif
	//y = POS_STA_FIRST_Y;
	if (m_editSearch.GetSafeHwnd()!=0)
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_NEW_UI_160111
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			y = UISTYLE_CHAT_POS_STA_FIRST_Y;
			m_editSearch.MoveWindow(const_text_intever+const_leftr_tool_width, y, cx-const_text_intever*2-const_leftr_tool_width, const_number_height);
		}else
#endif // USES_SUPPORT_UI_STYLE
			m_editSearch.MoveWindow(const_border_left+const_leftr_tool_width, 13, 190, const_number_height);
#else
		m_editSearch.MoveWindow(const_border_left+const_leftr_tool_width, y, cx-const_text_intever-const_leftr_tool_width-const_border_left, const_number_height);
#endif
#else
		m_editSearch.MoveWindow(const_text_intever+const_leftr_tool_width, y, cx-const_text_intever*2-const_leftr_tool_width, const_number_height);
#endif
	}
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
#ifdef USES_NEW_UI_160111
	if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
	{
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			//m_pPanelSearch->ShowWindow(SW_HIDE);
		}else
#endif // USES_SUPPORT_UI_STYLE
		{
			//const int nlen = (18+2)*4;
			CRect rect;
			rect.left = nBorderLeft+2;
			rect.right = cx-2;
			//rect.right = rect.left + nlen;
			rect.top = NEW_UI_160111_HEIGHT + 5;
			rect.bottom = rect.top + 23;	// 18
			//this->ClientToScreen(&rect);
			m_pPanelSearch->MoveWindow(&rect);
		}
	}
#else
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
#endif

	const int const_Tab_Width	= m_btnMyDepartment.GetImgWidth();
	const int const_Tab_intever = 0;
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		y = UISTYLE_CHAT_POS_STA_FIRST_Y + const_number_height;
		x = const_text_intever+const_leftr_tool_width;
	}else
#endif // USES_SUPPORT_UI_STYLE
		x = nBorderLeft+const_leftr_tool_width;
#else
	x = const_text_intever+const_leftr_tool_width+DEFAULT_RETURN_BTN_WIDTH+1;
#endif
	//if (m_btnSwitchFrame.GetSafeHwnd()!=NULL && m_btnSwitchFrame.IsWindowVisible())
	//	x += DEFAULT_RETURN_BTN_WIDTH;
	if (theApp.IsLogonVisitor())
	{
		m_btnMySession.MovePoint(x, y);
	}else
	{
		m_btnMyDepartment.MovePoint(x, y);
		x += (const_Tab_Width+const_Tab_intever);
		m_btnMyContacts.MovePoint(x, y);
		x += (const_Tab_Width+const_Tab_intever);
		m_btnMySession.MovePoint(x, y);
		x += (const_Tab_Width+const_Tab_intever);
		m_btnMyEnterprise.MovePoint(x, y);
	}
	if (m_btnSwitchFrame.GetSafeHwnd()!=NULL)
	{
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			m_btnSwitchFrame.ShowWindow(SW_HIDE);
		}else
#endif
		{
			m_rectSwitchFrame.left = const_text_intever+1;	// +1是 m_pDlgAppFrame 有一个像素偏差
			m_rectSwitchFrame.right = m_rectSwitchFrame.left+DEFAULT_RETURN_BTN_WIDTH;
			m_rectSwitchFrame.top = y;
			m_rectSwitchFrame.bottom = m_rectSwitchFrame.top+m_btnMyDepartment.GetImgHeight();
			m_btnSwitchFrame.MoveWindow(&m_rectSwitchFrame);
		}
	}
	y += m_btnMyDepartment.GetImgHeight();
#ifdef USES_NEW_UI_160111
	theYY = m_btnMyDepartment.GetImgHeight();
	//y += 1;
#else
#ifdef USES_NEW_UI_1220
	theYY = y;
	y += 1;
#endif
#endif
	const int const_left_intever = 1;
#ifdef USES_NEW_UI_1220
	CRect rectDlg(nBorderLeft+const_leftr_tool_width, y, cx-const_left_intever, cy-const_bottom_intever);
#else
	CRect rectDlg(const_left_intever+const_leftr_tool_width, y, cx-const_left_intever, cy-const_bottom_intever);
#endif
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
	if (m_treeSearch.GetSafeHwnd()!=0 && m_treeSearch.IsWindowVisible())
	{
		CRect searchRect;
		m_editSearch.GetWindowRect(&searchRect);
		this->ScreenToClient(&searchRect);
#ifdef USES_NEW_UI_160111
		searchRect.right = rectDlg.right;
#endif
		searchRect.top = searchRect.bottom+1;
		searchRect.bottom = rectDlg.bottom;
		m_treeSearch.MoveWindow(&searchRect);
	}
#ifndef USES_FRAMELIST_APPFRAME
	if (m_pDlgAppFrame!=NULL)
#endif // USES_FRAMELIST_APPFRAME
	{
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
#ifndef USES_FRAMELIST_APPFRAME
			if (m_pDlgFrameList==NULL)
			{
				//m_pDlgAppFrame->ShowWindow(SW_HIDE);
			}else if (m_pDlgAppFrame!=NULL)
			{
				CRect rectFrameList;
				m_pDlgFrameList->GetClientRect(&rectFrameList);
				const int nBorderLeft = m_pDlgFrameList->GetShowedLeft()?const_border_left:const_hide_border_left2;
				m_pDlgAppFrame->MoveWindow(nBorderLeft,const_border_size+theSearchToolbarHeight,rectFrameList.Width()-nBorderLeft-const_border_size,rectFrameList.Height()-const_border_size*2-theSearchToolbarHeight);
			}
#endif	// USES_FRAMELIST_APPFRAME
		}else
#endif
		{
#ifndef USES_NEW_UI_160111
#else
#ifdef USES_NEW_UI_1220
			rectDlg.top -= (m_btnMyDepartment.GetImgHeight());
#else
			rectDlg.top -= m_btnMyDepartment.GetImgHeight();
#endif
#endif
			//rectDlg.left += 1;
			//rectDlg.right -= 1;
			if (m_pDlgAppFrame!=NULL)
				m_pDlgAppFrame->MoveWindow(&rectDlg);
		}
	}

#ifdef USES_NEW_UI_1220
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CFrameWndInfo::pointer& pFrameWndInfo = *pIter;
			if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
			{
				CFrameWndInfoProxy::ResetUserData(pFrameWndInfo);
				if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_MAIN_FRAME)
					continue;
				else if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)
					continue;
				else if (pFrameWndInfo->GetDialog().get()!=NULL)
				{
					pFrameWndInfo->GetDialog()->SetParent(m_pDlgFrameList);
					pFrameWndInfo->GetDialog()->SetOwner(m_pDlgFrameList);
					pFrameWndInfo->GetDialog()->RefreshBtnShow();
					const bool bShow = pFrameWndInfo->IsChecked()?true:false;
					m_pDlgFrameList->AddWnd(pFrameWndInfo, bShow, false, true);
					if (!m_pDlgFrameList->IsWindowVisible())
						m_pDlgFrameList->ShowFrameList();
				}
				continue;
			}

			if (pFrameWndInfo->GetDialog().get()!=NULL && pFrameWndInfo->GetDialog()->GetSafeHwnd()!=NULL)
			{
#ifdef USES_NEW_UI_160111
				pFrameWndInfo->GetDialog()->MoveWindow(nBorderLeft,NEW_UI_160111_HEIGHT,cx-nBorderLeft-const_border_size,cy-(NEW_UI_160111_HEIGHT+const_border_size));
#else
				pFrameWndInfo->GetDialog()->MoveWindow(const_border_left,const_border_size,cx-const_border_left-const_border_size,cy-const_border_size*2);
#endif
			}
		}
	}
#endif
	//if (m_btnSwitchFrame.GetSafeHwnd())
	//	m_btnSwitchFrame.MoveWindow(const_text_intever,cy-60,50,50);
	x = nBorderLeft+3;
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		CRect rectFuncButton;
#ifdef USES_NEW_UI_1220
		rectFuncButton.left = nBorderLeft+2 + i*(const_func_button_size+const_func_button_intever);
#else
		rectFuncButton.left = nBorderLeft+4 + i*(const_func_button_size+const_func_button_intever);
#endif
		rectFuncButton.right = rectFuncButton.left + const_func_button_size;
		rectFuncButton.bottom = cy-2;
		rectFuncButton.top = rectFuncButton.bottom-const_func_button_size;
		if (pFuncButtonInfo->MoveWindow(&rectFuncButton))
			x = rectFuncButton.right+const_func_button_intever;
		//if (pFuncButtonInfo->m_btn.GetSafeHwnd()!=NULL)
		//{
		//	CRect rectFuncButton;
		//	rectFuncButton.left = nBorderLeft+4 + i*(const_func_button_size+const_func_button_intever);
		//	rectFuncButton.right = rectFuncButton.left + const_func_button_size;
		//	rectFuncButton.bottom = cy-2;
		//	rectFuncButton.top = rectFuncButton.bottom-const_func_button_size;
		//	pFuncButtonInfo->m_btn.MoveWindow(&rectFuncButton);
		//	x = rectFuncButton.right+const_func_button_intever;
		//}
	}
	if (m_btnNewApp.GetSafeHwnd()!=NULL)
	{
		m_rectNewApp.left = x;
		m_rectNewApp.right = m_rectNewApp.left+const_func_button_size;
		m_rectNewApp.bottom = cy-3;
		m_rectNewApp.top = m_rectNewApp.bottom-const_func_button_size;
		m_btnNewApp.MoveWindow(&m_rectNewApp);
	}

#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		CFrameWndInfoProxy::ClearWnd();
	}
#endif

}

void CPOPDlg::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 0,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMax.GetSafeHwnd()!=NULL)
		m_btnMax.MoveWindow(btnx, 0,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMin.GetSafeHwnd()!=NULL)
		m_btnMin.MoveWindow(btnx, 0,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnSkin.GetSafeHwnd())
		m_btnSkin.MoveWindow(btnx,0,const_minbtn_width,const_minbtn_height);
	//if (m_btnSwitchToolbar.GetSafeHwnd()!=NULL)
	//{
	//	btnx -= const_minbtn_width;
	//	m_rectSwitchToolbar.left = btnx;
	//	m_rectSwitchToolbar.right = m_rectSwitchToolbar.left+const_minbtn_width;
	//	m_rectSwitchToolbar.top = 2;
	//	m_rectSwitchToolbar.bottom = m_rectSwitchToolbar.top+const_minbtn_width;
	//	m_btnSwitchToolbar.MoveWindow(&m_rectSwitchToolbar);
	//}
	if (m_btnSwitchLeft.GetSafeHwnd()!=NULL)
	{
		m_rectSwitchLeft.left = 1;
		m_rectSwitchLeft.right = m_rectSwitchLeft.left + const_minbtn_width;
		m_rectSwitchLeft.bottom = cy - 3;
		m_rectSwitchLeft.top = m_rectSwitchLeft.bottom - const_minbtn_width;
		m_btnSwitchLeft.MoveWindow(&m_rectSwitchLeft);
	}
	MoveSize(cx,cy);
	if (nType==SIZE_RESTORED && GetWorkFrameShowed() && m_pDlgAppFrame!=NULL && m_pDlgAppFrame->GetSafeHwnd()!=NULL)
	{
		SetTimer(TIMERID_APPFRAME_SETFOCUS,1,NULL);
		//m_pDlgAppFrame->ShowWindow(SW_SHOW);
		//m_pDlgAppFrame->RefreshAppWnd();	// 设置焦点
	}else if (nType==SIZE_RESTORED && GetCallFrameShowed())
	{
		CFrameWndInfoProxy::RefreshChecked();
	}

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
#ifdef USES_SUPPORT_UI_STYLE
		const int theDefaultWidth = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_WIDTH:LEN_DLG_WIDTH;
		const int theDefaultHeight = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_HEIGHT:LEN_DLG_HEIGHT;
#else
		const int theDefaultWidth = LEN_DLG_WIDTH;
		const int theDefaultHeight = LEN_DLG_HEIGHT;
#endif
		if (GetPrivateProfileInt(_T("default"),_T("main-w"),theDefaultWidth,theApp.GetUserSettingIniFile())!=cx)
		{
			sprintf(lpszBuffer,"%d",cx);
			WritePrivateProfileString(_T("default"),_T("main-w"),lpszBuffer,theApp.GetUserSettingIniFile());
		}
		if (GetPrivateProfileInt(_T("default"),_T("main-h"),theDefaultHeight,theApp.GetUserSettingIniFile())!=cy)
		{
			sprintf(lpszBuffer,"%d",cy);
			WritePrivateProfileString(_T("default"),_T("main-h"),lpszBuffer,theApp.GetUserSettingIniFile());
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

}

//void CPOPDlg::OnBnClickedButtonCalluser()
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

void CPOPDlg::RefreshLabelWindow(void)
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

void CPOPDlg::DrawInfo(void)
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);
	this->ClearBgDrawInfo();
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		const int const_number_height = 22;
		this->AddBgDrawInfo(CEbBackDrawInfo(UISTYLE_CHAT_POS_STA_FIRST_Y+const_number_height,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(29,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		//const int nHeight = const_bottom_intever*-1;
		//this->AddBgDrawInfo(CEbBackDrawInfo(nHeight,1.0,false,false,0,theDefaultFlatLineColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(nHeight+1,1.0,false,false,0,theDefaultFlatBgColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1, UISTYLE_CHAT_POS_STA_FIRST_Y+const_number_height, 2);
	}
#endif

#ifdef USES_NEW_UI_160111
	const bool bShowToolBar = true;
#else
	const bool bShowToolBar = m_bShowedToolbar&&GetMainFrameShowed();
#endif
#ifdef USES_SUPPORT_UI_STYLE
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_OFFICE)
#endif
	//if (theApp.GetColorFlat())
	{
#ifdef USES_NEW_UI_1220
#ifdef USES_NEW_UI_160111
		const int nBorderLeft = m_bShowedLeft?const_border_left:const_hide_border_left1;

		this->AddBgDrawInfo(CEbBackDrawInfo(NEW_UI_160111_HEIGHT,1.0,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,const_border_left));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,nBorderLeft-1,theDefaultFlatBg2Color));	// theDefaultFlatLine2Color
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,1,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(28,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(theYY-1,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));	// 统一灰色，去掉线条（需要考虑工作区左边工具栏整体效果）
#else
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,const_border_left));
		this->AddBgDrawInfo(CEbBackDrawInfo(theYY,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
#endif
#else
		this->AddBgDrawInfo(CEbBackDrawInfo(m_bShowedToolbar?166:82,1.0,false));
#endif
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultBtnWhiteColor));
		const int nHeight = const_bottom_intever*-1;
		this->AddBgDrawInfo(CEbBackDrawInfo(nHeight,1.0,false,false,0,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(nHeight+1,1.0,false,false,0,theDefaultFlatBgColor));
#ifdef USES_NEW_UI_160111
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1, NEW_UI_160111_HEIGHT, 2);
#else
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1, const_border_left, 1);
#endif
	//}else
	//{
	//	if (theApp.GetColorGradient())
	//	{
	//		this->AddBgDrawInfo(CEbBackDrawInfo(m_bShowedToolbar?120:38,theDefaultBgColorTitleLight1,theApp.GetColorGradient()));
	//		//this->AddBgDrawInfo(CEbBackDrawInfo(62,theDefaultBgColorBgLight3,false));
	//		this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	//	}else
	//	{
	//		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,theApp.GetColorGradient()));
	//		//this->AddBgDrawInfo(CEbBackDrawInfo(120,1.0,theApp.GetColorGradient()));
	//	}
	//	this->DrawPopBg(&memDC, theApp.GetMainColor());
	}
	m_editSearch.Invalidate();
	Gdiplus::Graphics graphics(memDC.m_hDC);
	const FontFamily fontFamily(theFontFamily.c_str());

	if (bShowToolBar)
	{
		USES_CONVERSION;
		const Gdiplus::Font fontEbUserName(&fontFamily, 14, FontStyleRegular, UnitPixel);
#ifdef USES_NEW_UI_160111
#ifdef USES_SUPPORT_UI_STYLE
		const int nFontSize = nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?13:15;
		const Gdiplus::Font fontEbTitle(&fontFamily, nFontSize, FontStyleBold, UnitPixel);
#else
		const Gdiplus::Font fontEbTitle(&fontFamily, 15, FontStyleBold, UnitPixel);
#endif // USES_SUPPORT_UI_STYLE
#else
		const Gdiplus::Font fontEbTitle(&fontFamily, 13, FontStyleBold, UnitPixel);
#endif
		Gdiplus::Color pColorTitle;
#ifdef USES_NEW_UI_1220
#ifdef USES_NEW_UI_160111
		pColorTitle.SetFromCOLORREF(theDefaultBtnWhiteColor);
#else
		pColorTitle.SetFromCOLORREF(theApp.GetTrueColor());
#endif
#else
		pColorTitle.SetFromCOLORREF(theDefaultBtnWhiteColor);
#endif
		Gdiplus::SolidBrush brushEbTitle(pColorTitle);
#ifdef USES_NEW_UI_160111
#ifdef USES_SUPPORT_UI_STYLE
		const Gdiplus::PointF pointTitle = nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?Gdiplus::PointF(7,7):Gdiplus::PointF(10,10);
#else
		const Gdiplus::PointF pointTitle(10,10);
#endif // USES_SUPPORT_UI_STYLE
#else
		const Gdiplus::PointF pointTitle(const_border_left+10,10);
#endif
#ifdef USES_EBCOM_TEST
		CEBString sSettingEnterprise = theEBSetting->Enterprise.GetBSTR();
		const CEBString sSettingVersion = theEBSetting->Version.GetBSTR();
#else
		CEBString sSettingEnterprise(theSetting.GetEnterprise());
		const CEBString sSettingVersion(theSetting.GetVersion());
#endif

		//graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);	// 消除锯齿效果
		if (theApp.GetLicenseUser() && !theApp.GetProductName().IsEmpty())
		{
			graphics.DrawString(A2W_ACP(theApp.GetProductName()),-1,&fontEbTitle,pointTitle,&brushEbTitle);
		}else if (sSettingEnterprise.empty())
			graphics.DrawString(L"恩布互联",-1,&fontEbTitle,pointTitle,&brushEbTitle);
		else
		{
			if (!theApp.GetLicenseUser() && sSettingEnterprise.find("恩布")==std::string::npos)
			{
				sSettingEnterprise = "恩布互联";
			}
			graphics.DrawString(A2W_ACP(sSettingEnterprise.c_str()),-1,&fontEbTitle,pointTitle,&brushEbTitle);
		}

		// 写标题
		//CFont pNewFontTitle;
		//pNewFontTitle.CreatePointFont(115, _T("宋体"));//创建显示文本的字体
		//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontTitle.m_hObject);
		//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
		//CString sOutText = _T("EB2015");//theSetting.GetEnterprise().c_str();//_T("POP-2013");
		//TextOut(m_hdcMemory, 6, 5, sOutText, sOutText.GetLength());
		//SelectObject(m_hdcMemory, pOldFond);

		// MY IMG
#ifdef USES_EBCOM_TEST
		const CEBString sHeadFile = theEBClientCore->EB_GetMyDefaultMemberHeadFile().GetBSTR();
		const CEBString sUserName = theEBClientCore->EB_UserName.GetBSTR();
		const CEBString sOutDesc = theEBClientCore->EB_Description.GetBSTR();
#else
		const CEBString sHeadFile = theEBAppClient.EB_GetMyDefaultMemberHeadFile();
		const CEBString sUserName = theEBAppClient.EB_GetUserName();;
		const CEBString sOutDesc = theEBAppClient.EB_GetDescription();;
#endif
		Gdiplus::Image * pImage;
		if (theApp.IsLogonVisitor())
		{
			pImage = theApp.m_imageDefaultVisitor->Clone();
		}else
		{
			if (PathFileExists(sHeadFile.c_str()))
			{
				pImage = libEbc::LoadImageFromFile(sHeadFile.c_str());
				if (pImage==NULL)
					pImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(sHeadFile.c_str()));
			}else 
			{
				pImage = theApp.m_imageDefaultMember->Clone();
			}
		}
		graphics.DrawImage(pImage, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
		//graphics.DrawImage(pImage, POS_ADIMG_LEFT, POS_ADIMG_TOP, POS_ADIMG_SIGE, POS_ADIMG_SIGE);
		delete pImage;
		{
			Gdiplus::Color colorBg;
			colorBg.SetFromCOLORREF(theApp.GetMainColor());
			Gdiplus::Pen penBg(colorBg);
			Gdiplus::SolidBrush brushBg(colorBg);
			Gdiplus::Point polygon[5];
			const int const_draw_len = 2;
			// 上左
			polygon[0].X = m_rectHead.left;			// 左
			polygon[0].Y = m_rectHead.top;
			polygon[1].X = m_rectHead.left+const_draw_len-1;		// 右
			polygon[1].Y = m_rectHead.top;
			polygon[2].X = m_rectHead.left;			// 下
			polygon[2].Y = m_rectHead.top+const_draw_len-1;
			graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// 消除锯齿
			//graphics.DrawLine(&penBg,polygon[0],polygon[1]);
			//graphics.DrawLine(&penBg,polygon[0],polygon[2]);
			//graphics.SetSmoothingMode(SmoothingModeNone);	// 取消消除锯齿
			graphics.DrawLine(&penBg,polygon[0].X,polygon[0].Y,polygon[0].X-1,polygon[0].Y);
			// 上右
			polygon[0].X = m_rectHead.right-const_draw_len;	// 左
			polygon[0].Y = m_rectHead.top;
			polygon[1].X = m_rectHead.right-1;	// 右
			polygon[1].Y = m_rectHead.top;
			polygon[2].X = m_rectHead.right-1;	// 下
			polygon[2].Y = m_rectHead.top+const_draw_len-1;
			//graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// 消除锯齿
			//graphics.DrawLine(&penBg,polygon[1],polygon[0]);
			//graphics.DrawLine(&penBg,polygon[1],polygon[2]);
			//graphics.SetSmoothingMode(SmoothingModeNone);	// 取消消除锯齿
			graphics.DrawLine(&penBg,polygon[1].X,polygon[1].Y,polygon[1].X+1,polygon[1].Y);
			// 下左
			polygon[0].X = m_rectHead.left;			// 左
			polygon[0].Y = m_rectHead.bottom-1;
			polygon[1].X = m_rectHead.left+const_draw_len-1;		// 右
			polygon[1].Y = m_rectHead.bottom-1;
			polygon[2].X = m_rectHead.left;			// 上
			polygon[2].Y = m_rectHead.bottom-const_draw_len;
			//graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// 消除锯齿
			//graphics.DrawLine(&penBg,polygon[0],polygon[1]);
			//graphics.DrawLine(&penBg,polygon[0],polygon[2]);
			//graphics.SetSmoothingMode(SmoothingModeNone);	// 取消消除锯齿
			graphics.DrawLine(&penBg,polygon[0].X,polygon[0].Y,polygon[0].X-1,polygon[0].Y);
			// 下右
			polygon[0].X = m_rectHead.right-const_draw_len;	// 左
			polygon[0].Y = m_rectHead.bottom-1;
			polygon[1].X = m_rectHead.right-1;	// 右
			polygon[1].Y = m_rectHead.bottom-1;
			polygon[2].X = m_rectHead.right-1;	// 上
			polygon[2].Y = m_rectHead.bottom-const_draw_len;
			//graphics.SetSmoothingMode(SmoothingModeAntiAlias);	// 消除锯齿
			//graphics.DrawLine(&penBg,polygon[1],polygon[0]);
			//graphics.DrawLine(&penBg,polygon[1],polygon[2]);
			//graphics.SetSmoothingMode(SmoothingModeNone);	// 取消消除锯齿
			graphics.DrawLine(&penBg,polygon[1].X,polygon[1].Y,polygon[1].X+1,polygon[1].Y);
		}

		// 写字
#ifdef USES_NEW_UI_160111
		Gdiplus::Color pColorUserName;
		pColorUserName.SetFromCOLORREF(theDefaultFlatBgColor);
		Gdiplus::SolidBrush brushString(pColorUserName);
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
			graphics.DrawString(A2W_ACP(sUserName.c_str()),-1,&fontEbUserName,Gdiplus::PointF(UISTYLE_CHAT_POS_LINESTATE_LEFT+28,UISTYLE_CHAT_POS_LINESTATE_TOP-3),&brushString);
		else
#endif
			graphics.DrawString(A2W_ACP(sUserName.c_str()),-1,&fontEbUserName,Gdiplus::PointF(POS_LINESTATE_LEFT+28,POS_LINESTATE_TOP-3),&brushString);
#else
		const int POS_LINESTATE_WIDTH = m_btnLineState.GetImgWidth();
		Gdiplus::SolidBrush brushString(Gdiplus::Color::Black);
		graphics.DrawString(A2W_ACP(sUserName.c_str()),-1,&fontEbUserName,Gdiplus::PointF(POS_LINESTATE_LEFT+POS_LINESTATE_WIDTH+1,POS_LINESTATE_TOP-2),&brushString);
#endif
		// 
#ifdef USES_NEW_UI_160111
		bool bShowUserDescription = false;
		Gdiplus::PointF pPointDescription(POS_LINESTATE_LEFT,POS_LINESTATE_TOP+22);
#ifdef USES_SUPPORT_UI_STYLE
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			bShowUserDescription = true;
			pPointDescription = Gdiplus::PointF(UISTYLE_CHAT_POS_LINESTATE_LEFT,UISTYLE_CHAT_POS_LINESTATE_TOP+18);
		}
#endif	// USES_SUPPORT_UI_STYLE
#else
		bool bShowUserDescription = true;
#endif
		if (bShowUserDescription)
		{
			if (sOutDesc.empty())
			{
				const Gdiplus::Font fontEbDescription(&fontFamily, 13, FontStyleRegular, UnitPixel);
				Gdiplus::SolidBrush brushDescription(Gdiplus::Color(200,255,255,255));
				//Gdiplus::SolidBrush brushDescription(Gdiplus::Color(200,128,128,128));
				graphics.DrawString(L"编辑个性签名",-1,&fontEbDescription,pPointDescription,&brushDescription);
			}else
			{
				const Gdiplus::Font fontEbDescription(&fontFamily, 13, FontStyleRegular, UnitPixel);
				Gdiplus::Color pColorDesc;
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
				if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
					pColorDesc = Gdiplus::Color(200,255,255,255);
				else
#endif
					pColorDesc.SetFromCOLORREF(theDefaultFlatBlackTextColor);
#else
				pColorDesc.SetFromCOLORREF(theDefaultBtnWhiteColor);
#endif
				//if (theApp.GetColorFlat())
				//	pColorDesc.SetFromCOLORREF(theDefaultTextBlackColor);
				//else
				//	pColorDesc.SetFromCOLORREF(RGB(250,250,250));
				Gdiplus::SolidBrush brushDescription(pColorDesc);
				//Gdiplus::SolidBrush brushDescription(Gdiplus::Color(200,255,255,255));
				graphics.DrawString(A2W_ACP(sOutDesc.c_str()),-1,&fontEbDescription,pPointDescription,&brushDescription);
			}
		}
//#endif
	}
//
//	const Gdiplus::Font fontEbBeta(&fontFamily, 12, FontStyleBold, UnitPixel);
//#ifdef USES_SUPPORT_UI_STYLE
//	const Gdiplus::PointF pointBeta(rectClient.Width()-78,rectClient.Height()-23);	// 60，某些XP环境下，长度不够
//#else
//	const Gdiplus::PointF pointBeta(rectClient.Width()-80,rectClient.Height()-23);	// 60，某些XP环境下，长度不够
//#endif
//	Gdiplus::Color colorVersion;
//	colorVersion.SetFromCOLORREF(theDefaultFlatLine2Color);
//	//if (theApp.GetColorGradient() && !theApp.GetColorFlat())
//	//	colorVersion.SetFromCOLORREF(theApp.GetBgColor1());
//	//else
//	//	colorVersion.SetFromCOLORREF(theDefaultFlatLineColor);
//	//colorVersion.SetFromCOLORREF(theApp.GetColorGradient()?theApp.GetHotColor2():theDefaultTextWhiteColor);
//	Gdiplus::SolidBrush brushEbVersion(colorVersion);
//	const tstring sVersion = "V"+theSetting.GetVersion();
//	USES_CONVERSION;
//	graphics.DrawString(T2W(sVersion.c_str()),-1,&fontEbBeta,pointBeta,&brushEbVersion);
//	//graphics.DrawString(L"1.19.0.402",-1,&fontEbBeta,pointBeta,&brushEbVersion);
}

void CPOPDlg::OnBnClickedButtonEnterprise()
{
	//if (m_pCurrentLabel != m_btnMyEnterprise.GetSafeHwnd())
	{
		m_pOldCurrentLabel = m_pCurrentLabel;
		m_pCurrentLabel = m_btnMyEnterprise.GetSafeHwnd();
		RefreshLabelWindow();
	}
}

void CPOPDlg::OnBnClickedButtonDepartment()
{
	//if (m_pCurrentLabel != m_btnMyDepartment.GetSafeHwnd())
	{
		m_pOldCurrentLabel = m_pCurrentLabel;
		m_pCurrentLabel = m_btnMyDepartment.GetSafeHwnd();
		RefreshLabelWindow();
	}
}

void CPOPDlg::OnBnClickedButtonContact()
{
	//if (m_pCurrentLabel != m_btnMyContacts.GetSafeHwnd())
	{
		m_pOldCurrentLabel = m_pCurrentLabel;
		m_pCurrentLabel = m_btnMyContacts.GetSafeHwnd();
		RefreshLabelWindow();
	}
}

void CPOPDlg::OnBnClickedButtonSession()
{
	m_btnMySession.SetWindowText(_T(""));
	m_pOldCurrentLabel = m_pCurrentLabel;
	m_pCurrentLabel = m_btnMySession.GetSafeHwnd();
	RefreshLabelWindow();
}

void CPOPDlg::OnBnClickedButtonMax()
{
	if (m_bShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_bShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}

	const int nScreenWidth = theApp.GetScreenWidth();
	const int nScreenHeight = theApp.GetScreenHeight();

	static CRect theRestoreRect;
	CRect rect;
	GetWindowRect(&rect);
	bool bSetBtnMax = false;
	if (rect.Width() == nScreenWidth)
	{
		if (theRestoreRect.Width()==0 || theRestoreRect.Height()==0)
		{
#ifdef USES_SUPPORT_UI_STYLE
			const int theDefaultWidth = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_WIDTH:LEN_DLG_WIDTH;
			const int theDefaultHeight = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_HEIGHT:LEN_DLG_HEIGHT;
#else
			const int theDefaultWidth = LEN_DLG_WIDTH;
			const int theDefaultHeight = LEN_DLG_HEIGHT;
#endif
			theRestoreRect.left = (nScreenWidth-theDefaultWidth)/2;
			theRestoreRect.right = theRestoreRect.left+theDefaultWidth;
			//if (m_pDlgAppFrame!=NULL && !m_pDlgAppFrame->IsEmpty())
			//	theRestoreRect.right += DEFAULT_RETURN_BTN_WIDTH;
			theRestoreRect.top = (nScreenHeight-theDefaultHeight)/2;
			theRestoreRect.bottom = theRestoreRect.top+theDefaultHeight;
		}
		this->SetToolTipText(IDC_BUTTON_MAX2,_T("最大化"));
		m_btnMax.SetDrawToolButtonPic(2,theDefaultBtnWhiteColor,-1,-1,-1,2);
		MoveWindow(&theRestoreRect);
		bSetBtnMax = true;
	}else
	{
		this->SetToolTipText(IDC_BUTTON_MAX2,_T("向下还原"));
		m_btnMax.SetDrawToolButtonPic(3,theDefaultBtnWhiteColor,-1,-1,-1,2);
		theRestoreRect = rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = rect.left + nScreenWidth;
		rect.bottom = rect.top + nScreenHeight;
		MoveWindow(&rect);
	}
	//if (m_pDlgAppFrame!=NULL && GetWorkFrameShowed() && !m_treeSearch.IsWindowVisible())//m_pDlgAppFrame->IsWindowVisible())
	//{
	//	m_pDlgAppFrame->ShowWindow(SW_SHOW);
	//	m_pDlgAppFrame->RefreshAppWnd();
	//	// 解决m_pDlgAppFrame 背景刷新问题（按钮背景）
	//	//SetTimer(TIMERID_REFRESH_APPFRAME,1,NULL);
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

void CPOPDlg::OnBnClickedButtonMin()
{
	if (m_bShrinkageWin)
	{
		KillTimer(TIMERID_SHRINKAGE_WIN);
		m_bShrinkageWin = false;
		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
		SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
	//if (m_bShrinkageWin && m_pDlgShrinkageBar->IsWindowVisible())
	//{
	//	SetTimer(TIMERID_CHECK_SHRINKAGEBAR,200,NULL);
	//}
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CPOPDlg::OnBnClickedButtonClose()
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
	this->SetFocus();	// 解决按钮背景刷新问题；
}

void CPOPDlg::OnEditSearch(void)
{
	boost::mutex::scoped_lock lockSearch(m_mutexSearch);
	CString sSearchString;
	m_editSearch.GetWindowText(sSearchString);
	sSearchString.Trim();
	if (sSearchString==theSearchString)
	{
		//HideSearchResult();
		return;
	}
	theSearchString = sSearchString;

	if (!sSearchString.IsEmpty())
	{
		m_pSubFuncItem.clear();
		m_pUrlItem.clear();
		m_pContactItem.clear();
		m_pEntGroupMemberItem.clear();
		m_pMyGroupMemberItem.clear();
		m_treeSearch.DeleteAllItems();

		if (sSearchString.Left(1)==_T("/"))
		{
			// *搜索应用
			tstring sAppKey = sSearchString.Mid(1);
			std::transform(sAppKey.begin(), sAppKey.end(), sAppKey.begin(), tolower);
			m_pSubscribeFuncList.clear();
			theEBAppClient.EB_GetSubscribeFuncList((EB_FUNC_LOCATION)0xffffff,m_pSubscribeFuncList);
			for (size_t i=0; i<m_pSubscribeFuncList.size(); i++)
			{
				const EB_SubscribeFuncInfo pFuncInfo = m_pSubscribeFuncList[i];
				tstring sFunctionName(pFuncInfo.m_sFunctionName);
				tstring sDescription(pFuncInfo.m_sDescription);
				std::transform(sFunctionName.begin(), sFunctionName.end(), sFunctionName.begin(), tolower);
				std::transform(sDescription.begin(), sDescription.end(), sDescription.begin(), tolower);

				if (sFunctionName.find(sAppKey)==std::string::npos &&
					sDescription.find(sAppKey)==std::string::npos)
					continue;

				CTreeItemInfo::pointer pContactGroupInfo;
				if (!m_pSubFuncItem.find(0,pContactGroupInfo))
				{
					HTREEITEM hEmpItem = m_treeSearch.InsertItem(theAppGroupResultName.c_str());
					pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
					pContactGroupInfo->m_dwItemData = theApp.GetWorkFrameShowed()?3:0;	// 保证永远排前面
					m_pSubFuncItem.insert(0,pContactGroupInfo);
					m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
					m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
				}

				if (!m_pSubFuncItem.exist(pFuncInfo.m_nSubscribeId))
				{
					const std::string sTitle(pFuncInfo.m_sFunctionName);
					CString sSearchName;
					sSearchName.Format(_T("%s\nCLR=%d;%s"),sTitle.c_str(),(int)RGB(0,128,255),pFuncInfo.m_sDescription.c_str());
					HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
					CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_SUBINFO,hContactItem);
					pContactItemInfo->m_sName = sTitle;
					pContactItemInfo->m_sId = pFuncInfo.m_nSubscribeId;
					pContactItemInfo->m_sGroupName = pFuncInfo.m_sResFile;
					pContactItemInfo->m_sAccount = sFunctionName;
					m_pSubFuncItem.insert(pContactItemInfo->m_sId,pContactItemInfo);
					m_treeSearch.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
					ShowSearchResult();
					const int nGroupResultSize = m_pSubFuncItem.size()-1;
					if (nGroupResultSize==1)
					{
						if (this->GetWorkFrameShowed() && m_treeSearch.GetSelectedItem()==NULL)
							m_treeSearch.Select(hContactItem,TVGN_CARET);
						m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
					}
					sSearchName.Format(_T("%s%d"),theAppGroupResultName.c_str(),nGroupResultSize);
					m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
					if (nGroupResultSize>=15) break;
				}
			}
		}

		CString sSql;
		tstring sSearchKey(sSearchString);
		CSqliteCdbc::escape_string_in(sSearchKey);
		sSearchKey = libEbc::ACP2UTF8(sSearchKey.c_str());
		const int nLimit = GetWorkFrameShowed()?25:20;
		sSql.Format(_T("select url,title,strftime('%%Y-%%m-%%d %%H:%%M',last_time,'localtime') FROM url_record_t WHERE url LIKE '%%%s%%' OR title LIKE '%%%s%%' ORDER BY last_time DESC LIMIT %d"),sSearchKey.c_str(),sSearchKey.c_str(),nLimit);
		int nCookie = 0;
		theApp.m_pBoUsers->select(sSql, nCookie);
		cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
		while (pRecord.get()!=NULL)
		{
			tstring sUrl(pRecord->getVector()[0]->getStrValue());
			tstring sTitle(pRecord->getVector()[1]->getStrValue());
			const tstring sLastTime(pRecord->getVector()[2]->getStrValue());
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
				m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
			}
			if (!m_pUrlItem.exist(sUrl))
			{
				CString sSearchName;
				sSearchName.Format(_T("%s (%s)\nCLR=%d;%s"),sTitle.c_str(),sLastTime.c_str(),(int)RGB(0,128,255),sUrl.c_str());
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
					if (this->GetWorkFrameShowed() && m_treeSearch.GetSelectedItem()==NULL)
						m_treeSearch.Select(hContactItem,TVGN_CARET);
					m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
				}
				sSearchName.Format(_T("%s%d"),theUrlGroupResultName,nGroupResultSize);
				m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
				//ScrollTreeSearchToTop();
			}
		}
		theApp.m_pBoUsers->reset(nCookie);

		CString sSearchStringTemp(sSearchString);
		sSearchStringTemp.MakeLower();
		if (sSearchStringTemp!="about:blank" &&
			sSearchStringTemp.Find("http://")<0 &&
			sSearchStringTemp.Find("https://")<0 &&
			sSearchStringTemp.Find("www.")<0)
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
			SetTimer(TIMERID_SELECT_FIRST_SEARCH,50,NULL);
		}
	}
	if (m_treeSearch.GetCount() == 0 || sSearchString.IsEmpty())
	{
		HideSearchResult();
	}
}

void CPOPDlg::HideSearchResult(void)
{
	if (m_treeSearch.IsWindowVisible())
	{
		m_treeSearch.DeleteAllItems();
		m_treeSearch.ShowWindow(SW_HIDE);

		//SwitchFrameWnd();
		if (m_pDlgAppFrame!=NULL && GetWorkFrameShowed())//m_pDlgAppFrame->IsWindowVisible()))
		{
			//if (!m_pDlgAppFrame->IsWindowVisible())
			//{
			//	m_pDlgAppFrame->ShowWindow(SW_SHOW);
			//}
			//m_pDlgAppFrame->RefreshAppWnd();

#ifdef USES_SUPPORT_UI_STYLE
			const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
			if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
			{
				// *
			}else
#endif
			{
				if (!m_pDlgAppFrame->IsWindowVisible())
				{
					m_pDlgAppFrame->ShowWindow(SW_SHOW);
				}
				m_pDlgAppFrame->RefreshAppWnd();

				if (m_pPanelSearch!=0 && !m_pPanelSearch->IsWindowVisible())
					m_pPanelSearch->ShowWindow(SW_SHOW);
			}
			m_editSearch.SetFocus();
			return;
		}

		if (GetCallFrameShowed())
		{
			CFrameWndInfoProxy::ShowHideCurrent(true);
			return;
		}
		if (theApp.IsEnterpriseuserUser())
		{
			m_btnMyEnterprise.ShowWindow(SW_SHOW);
		}
#ifdef USES_NEW_UI_160111
		m_btnMyCenter.ShowWindow(SW_SHOW);
		m_btnFileMgr.ShowWindow(SW_SHOW);
		m_btnMyShare.ShowWindow(SW_SHOW);
		m_btnMainFunc.ShowWindow(SW_SHOW);
#endif
#ifndef USES_NEW_UI_1220
		m_btnSwitchFrame.ShowWindow(SW_SHOW);
#endif
		if (!theApp.IsLogonVisitor())
		{
			m_btnMyDepartment.ShowWindow(SW_SHOW);
			m_btnMyContacts.ShowWindow(SW_SHOW);
		}
		m_btnMySession.ShowWindow(SW_SHOW);
		if (!theApp.IsLogonVisitor())
		{
			if (m_btnMyEnterprise.GetChecked())
				m_pDlgMyEnterprise->ShowWindow(SW_SHOW);
			else if (m_btnMyDepartment.GetChecked())
				m_pDlgMyGroup->ShowWindow(SW_SHOW);
			else if (m_btnMyContacts.GetChecked())
				m_pDlgMyContacts->ShowWindow(SW_SHOW);
		}
		if (m_btnMySession.GetChecked())
			m_pDlgMySession->ShowWindow(SW_SHOW);
	}
}

void CPOPDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMERID_APPFRAME_SETFOCUS:
		{
			KillTimer(TIMERID_APPFRAME_SETFOCUS);
			m_pDlgAppFrame->RefreshAppWnd();	// 设置焦点
		}break;
	//case TIMERID_ONUISTYLETYPEOFFICE:
	//	{
	//		KillTimer(TIMERID_ONUISTYLETYPEOFFICE);
	//		OnUIStyleTypeOffice();
	//		//m_pDlgAppFrame->DelAll();
	//	}break;
	case TIMERID_SELECT_FIRST_SEARCH:
		{
			KillTimer(TIMERID_SELECT_FIRST_SEARCH);
			theApp.SelectFirstSearch(m_treeSearch, m_editSearch, m_bCanSearch);
		}break;
	case TIMERID_CALL_SEARCH_SEL_ALL:
		{
			KillTimer(TIMERID_CALL_SEARCH_SEL_ALL);
			const int nTextLength = m_editSearch.GetWindowTextLength();
			if (nTextLength>0)
			{
				m_editSearch.SetSel(0,nTextLength);
			}
		}break;
	case TIMERID_CLEAR_AUTOCALL_INFO:
		{
			KillTimer(TIMERID_CLEAR_AUTOCALL_INFO);
			theApp.m_pAutoCallFromUserIdList.clear();
			theApp.m_pAutoCallGroupIdList.clear();
		}break;
	case TIMERID_LOAD_LOCAL_UNREAD_MSG:
		{
			KillTimer(TIMERID_LOAD_LOCAL_UNREAD_MSG);
			std::vector<eb::bigint> pGroupIdList;
			std::vector<eb::bigint> pFromUserIdList;
			CString sSql;
			sSql.Format(_T("SELECT DISTINCT dep_code,from_uid FROM msg_record_t WHERE from_uid<>%lld AND (read_flag&1)=0 LIMIT 30"),theApp.GetLogonUserId());
			int nCookie = 0;
			theApp.m_pBoUsers->select(sSql, nCookie);
			cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
			while (pRecord.get()!=NULL)
			{
				const eb::bigint nGroupId = pRecord->getVector()[0]->getBigIntValue();
				const eb::bigint nFromUserId = pRecord->getVector()[1]->getBigIntValue();

				if (nGroupId>0)
					pGroupIdList.push_back(nGroupId);
				else if (nFromUserId>0 && !is_visitor_uid(nFromUserId))
					pFromUserIdList.push_back(nFromUserId);
				pRecord = theApp.m_pBoUsers->next(nCookie);
			}
			theApp.m_pBoUsers->reset(nCookie);
			for (size_t i=0;i<pGroupIdList.size();i++)
			{
				if (theApp.m_pAutoCallGroupIdList.insert(pGroupIdList[i],true,false))
					theEBAppClient.EB_CallGroup(pGroupIdList[i]);
			}
			for (size_t i=0;i<pFromUserIdList.size();i++)
			{
				if (theApp.m_pAutoCallFromUserIdList.insert(pFromUserIdList[i],true,false))
					theEBAppClient.EB_CallUserId(pFromUserIdList[i]);
			}
			if (!pGroupIdList.empty() || !pFromUserIdList.empty())
			{
				SetTimer(TIMERID_CLEAR_AUTOCALL_INFO,18*1000,NULL);
			}
		}break;
	case TIMERID_EDIT_SEARCH:
		{
			KillTimer(TIMERID_EDIT_SEARCH);
			if (m_bCanSearch)
				OnEditSearch();
			else
				HideSearchResult();
		}break;
	case TIMERID_CALL_SEARCH_SELECTED:
		KillTimer(nIDEvent);
		CallItem(m_treeSearch.GetSelectedItem());
		break;
	case TIMERID_CALL_SEARCH_TRACK:
		KillTimer(nIDEvent);
		CallItem(m_treeSearch.GetTrackItem());
		break;
	case TIMERID_CALL_SEARCH_SEL_CHANGE:
		KillTimer(nIDEvent);
		CallItem(m_treeSearch.GetTrackItem());
		break;
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
	case TIMERID_ONE_SECOND:
		{
			if (m_pDlgMsgTip!=NULL)
			{
				static bool theShowTrayIcon = true;
				const bool bIsMsgEmpty = m_pDlgMsgTip->IsMsgEmpty();
				if (!bIsMsgEmpty || !theShowTrayIcon)
				{
					if (theShowTrayIcon)
					{
						m_trayIconData.hIcon = NULL;

#ifdef USES_SUPPORT_UI_STYLE
						const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
						if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && this->m_pDlgFrameList!=NULL && m_pDlgFrameList->IsShrinkageWin())
						{
							m_pDlgFrameList->CheckUnreadMsgTip(true);
						}else
#endif
						{
							if (m_bShrinkageWin && !this->IsWindowVisible() && m_pDlgShrinkageBar!=NULL && m_pDlgShrinkageBar->IsWindowVisible() && (m_pDlgFrameList==NULL || !m_pDlgFrameList->IsWindowVisible()))
								m_pDlgShrinkageBar->SetBgColor(RGB(255,128,128));
						}
						//m_pDlgShrinkageBar->SetBgColor(theApp.GetHotColor2());
					}else
					{
						m_trayIconData.hIcon = m_hIcon;
#ifdef USES_SUPPORT_UI_STYLE
						const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
						if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && this->m_pDlgFrameList!=NULL)
						{
							m_pDlgFrameList->CheckUnreadMsgTip(false);
						}//else	// *
#endif
						{
							if (m_pDlgShrinkageBar!=NULL)
								m_pDlgShrinkageBar->SetBgColor(-1);
						}
					}
					theShowTrayIcon = !theShowTrayIcon;
					Shell_NotifyIcon(NIM_MODIFY, &m_trayIconData);
				}
				if (m_pDlgMsgTip->IsWindowVisible())
				{
					m_pDlgMsgTip->CheckPosInMsgArea();
				}
			}

			//static unsigned int theIndex = 0;
			//theIndex++;
			//if (theIndex%2==1)
			//	break;

			CEBCMsgInfo::pointer pEbcMsgInfo;
			if (m_pEbcMsgList.front(pEbcMsgInfo))
			{
				bool bCancel = false;
				switch (pEbcMsgInfo->GetMsgType())
				{
				case CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW:
					{
						static time_t theLastCreateWindowTime = 0;
						const time_t tNow = time(0);
						if (theLastCreateWindowTime>0 && (theLastCreateWindowTime+3)>=tNow && (pEbcMsgInfo->GetCreateTime()+2)>=tNow)	// 012 = 3S
						//if ((pEbcMsgInfo->GetCreateTime()+2)>=time(0))	// 012 = 3S
						{
							bCancel = true;
							m_pEbcMsgList.add(pEbcMsgInfo);
							break;
						}
						theLastCreateWindowTime = tNow;

						CWnd * pParent = pEbcMsgInfo->m_pParent;
						//pEbcMsgInfo->m_pFuncWindow->m_nFuncBrowserType = ;
						pEbcMsgInfo->m_pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
						if (pEbcMsgInfo->m_bSetForegroundWindow)
							pEbcMsgInfo->m_pFuncWindow->SetForegroundWindow();
					}break;
				case CEBCMsgInfo::MSG_TYPE_USER_ENTER_ROOM:
					{
						const CCrAccountInfo& pAccountInfo = pEbcMsgInfo->m_pAccountInfo;
						const eb::bigint sCallId = pAccountInfo.GetCallId();
						CDlgDialog::pointer pDlgDialog = GetCallIdDialog(sCallId);
						if (pDlgDialog.get()!=NULL)
						{
							pDlgDialog->OnUserEnterRoom(&pAccountInfo,true);
						}
					}break;
				default:
					break;
				}
				if (!bCancel && pEbcMsgInfo->GetPlayMsgSound())
				{
					CString sSoundFile;
					sSoundFile.Format(_T("%s/wav/msg.wav"), theApp.GetAppDataPath());
					sndPlaySound(sSoundFile, SND_ASYNC);
				}
			}
		}break;
//	case TIMERID_REFRESH_APPFRAME:
//		{
//			KillTimer(nIDEvent);
//			if (m_pDlgAppFrame!=NULL)
//			{
//				//m_pDlgAppFrame->ShowWindow(SW_HIDE);
//				//m_pDlgAppFrame->ShowWindow(SW_SHOW);
//#ifdef USES_LIBCEF
//				m_pDlgAppFrame->RefreshAppWnd();
//#endif
//			}
//		}break;
	case TIMERID_CHECK_MOUSE_POS:
		{
			do
			{
				CFrameWndInfo::pointer pCloseInfo;
				m_pCloseList.front(pCloseInfo);
			}while (m_pCloseList.size()>2);

			CFrameWndInfoProxy::CheckMousePos();
			CPoint pos;
			GetCursorPos(&pos);
			this->ScreenToClient(&pos);
			if (m_btnSwitchLeft.GetSafeHwnd()!=NULL)
			{
				const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
				if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
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

//			if (m_btnSwitchToolbar.GetSafeHwnd()!=NULL)
//			{
//				static bool thePtInRect = false;
//				const bool bPtInRect = m_rectSwitchToolbar.PtInRect(pos)?true:false;
//				if (thePtInRect!=bPtInRect)
//				{
//					thePtInRect = bPtInRect;
//					if (bPtInRect)
//					{
//						if (m_bShowedToolbar)
//							m_btnSwitchToolbar.SetDrawTrianglePic(1,RGB(64,64,64),-1,-1,-1,12,6);
//						else
//							m_btnSwitchToolbar.SetDrawTrianglePic(2,RGB(64,64,64),-1,-1,-1,10,5);
//					}else
//					{
//#ifdef USES_NEW_UI_1220
//						const COLORREF color = theDefaultFlatLineColor;
//#else
//						const COLORREF color = theDefaultBtnWhiteColor;
//#endif
//						if (m_bShowedToolbar)
//							m_btnSwitchToolbar.SetDrawTrianglePic(1,color,-1,-1,-1,12,6);
//						else
//							m_btnSwitchToolbar.SetDrawTrianglePic(2,color,-1,-1,-1,10,5);
//
//					}
//					m_btnSwitchToolbar.Invalidate();
//				}
//			}
//			if (m_btnSwitchFrame.GetSafeHwnd()!=NULL)
//			{
//				static bool thePtInRect = false;
//				const bool bPtInRect = m_rectSwitchFrame.PtInRect(pos)?true:false;
//				if (thePtInRect!=bPtInRect)
//				{
//					thePtInRect = bPtInRect;
//					if (bPtInRect)
//					{
//						m_btnSwitchFrame.SetDrawTrianglePic(3,RGB(64,64,64),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
//					}else
//					{
//#ifdef USES_NEW_UI_1220
//						m_btnSwitchFrame.SetDrawTrianglePic(3,theDefaultFlatLineColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
//#else
//						m_btnSwitchFrame.SetDrawTrianglePic(3,theDefaultBtnWhiteColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
//#endif
//					}
//					m_btnSwitchFrame.Invalidate();
//				}
//			}
			if (m_btnNewApp.GetSafeHwnd()!=NULL)
			{
				static bool thePtInRect = false;
				const bool bPtInRect = m_rectNewApp.PtInRect(pos)?true:false;
				if (thePtInRect!=bPtInRect)
				{
					thePtInRect = bPtInRect;
					if (bPtInRect)
					{
#ifdef USES_NEW_UI_1220
						m_btnNewApp.SetDrawNewPic(true,RGB(64,64,64),-1,-1,-1,2,11);
#else
						m_btnNewApp.SetDrawNewPic(true,theApp.GetHotColor2(),-1,-1,-1,2,11);
#endif
					}else
					{
#ifdef USES_NEW_UI_1220
						m_btnNewApp.SetDrawNewPic(true,theDefaultFlatLineColor,-1,-1,-1,2,11);
#else
						m_btnNewApp.SetDrawNewPic(true,RGB(64,64,64),-1,-1,-1,2,11);
#endif
					}
					m_btnNewApp.Invalidate();
				}
			}
		}break;
	case TIMERID_SHRINKAGE_WIN:
		{
			if (this->IsIconic()) break;
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
					//m_pPanelSearch->ShowWindow(SW_HIDE);
			}else if (rectWin.PtInRect(pos))
			{
				// down
				if (!m_pDlgShrinkageBar->IsWindowVisible())
					break;
				m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
				this->ShowWindow(SW_SHOWNORMAL);
				SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
				this->SetFocus();
				CFrameWndInfoProxy::SetCurrentFocus();
#ifdef USES_SUPPORT_UI_STYLE
				const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
				if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
				{
					// *
				}else
#endif
				{
					if (!m_treeSearch.IsWindowVisible() && this->GetWorkFrameShowed() && m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
						m_pPanelSearch->ShowWindow(SW_SHOW);
				}
				if (m_pDlgAppFrame!=NULL && m_pDlgAppFrame->IsWindowVisible())
				{
					m_pDlgAppFrame->RefreshAppWnd();
				}
				CFrameWndInfoProxy::RefreshChecked();
			}
		}break;
	case TIMERID_NEW_VERSION:
		{
			//ShowStatusMessage(L"正在检查新版本...");
			KillTimer(nIDEvent);
			const CString sNewVersion(theApp.EBC_GetProfileString(_T("new_version"),_T("version")));
			const CString sNewVersionFile(theApp.EBC_GetProfileString(_T("new_version"),_T("file")));
			//const UINT nUpdateType = theApp.EBC_GetProfileInt(_T("new_version"),_T("type"),0);
			if (!sNewVersion.IsEmpty())
			{
				theCheckNewVersionTime = 0;
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
#ifdef USES_EBCOM_TEST
						const CEBString sSettingEnterprise = theEBSetting->Enterprise.GetBSTR();
#else
						const CEBString& sSettingEnterprise = theSetting.GetEnterprise();
#endif
						if (theApp.GetLicenseUser() && !theApp.GetProductName().IsEmpty())
						{
							sText.Format(_T("%s检测到新版本：\r\n%s 是否立即更新？"),theApp.GetProductName(),sNewVersion);
						}else if (sSettingEnterprise.empty())
							sText.Format(_T("恩布互联检测到新版本：\r\n%s 是否立即更新？"),sNewVersion);
						else
						{
							if (!theApp.GetLicenseUser() && sSettingEnterprise.find("恩布")==std::string::npos)
							{
								sText.Format(_T("恩布互联检测到新版本：\r\n%s 是否立即更新？"),sNewVersion);
							}else
								sText.Format(_T("%s检测到新版本：\r\n%s 是否立即更新？"),sSettingEnterprise.c_str(),sNewVersion);
						}
						if (CDlgMessageBox::EbDoModal(this,"版本更新",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
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
			}else if (theCheckNewVersionTime>0)
			{
				const UINT nUpdateType = theApp.EBC_GetProfileInt(_T("new_version"),_T("type"),0);
				// nUpdateType:102 正在下载
				// nUpdateType:101 最新版本
				if (nUpdateType==101)
				{
					theCheckNewVersionTime = 0;
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
						sClientVersion = theEBSetting->Version;	// ?
#else
						sClientVersion = theSetting.GetVersion().c_str();
#endif
					}
					CString sText;
					sText.Format(_T("当前 %s\r\n已经是最新版本！"),sClientVersion);
					CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_ENTBOOST,10);
				}else if (nUpdateType==102)
				{
					CString sText;
					sText.Format(_T("检测到新版本：\r\n正在下载增量更新包..."));
					CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_INFORMATION,10);
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
			unsigned long pGroupMsgSubId = 0;
			theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,&pGroupMsgSubId);
			if (pGroupMsgSubId==NULL) break;
			theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,pGroupMsgSubId);

			EB_SubscribeFuncInfo pSubscribeFuncInfo;
			if (theEBAppClient.EB_GetSubscribeFuncInfo(theApp.GetAutoOpenSubId(),&pSubscribeFuncInfo))
			{
//#ifdef USES_SUPPORT_UI_STYLE
//				const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
//				if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=NULL)
//				{
//#endif
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
			// 集成应用
			static int theIndex = 0;
			if ((theIndex++) == 15)	// 最多检查15次，一次2秒
			{
				theIndex = 0;
				KillTimer(TIMERID_CHECK_MF_SUBFUNCINFO);
			}
			std::vector<EB_SubscribeFuncInfo> pMainSubscribeFuncList;	// for mainframe
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

			pMainSubscribeFuncList.clear();
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
				m_pMainSubscribeFuncList.clear();
				for (size_t i=0;i<pMainSubscribeFuncList.size();i++)
				{
					const EB_SubscribeFuncInfo & pSubscribeFuncInfo = pMainSubscribeFuncList[i];
					m_pMainSubscribeFuncList.add(pSubscribeFuncInfo);
					AddAppFrameFuncInfo(pSubscribeFuncInfo);
				}
			}
		}break;
	case TIMERID_CHECK_CLOSE_DIALOG:
		{
			// 定期检查超过５分钟没用会话
			{
				const time_t tNow = time(0);
				BoostReadLock rdlock(theApp.m_pCallList.mutex());
				//boost::mutex::scoped_lock lockCallList(theApp.m_pCallList.mutex());
				CLockMap<eb::bigint, CEBCCallInfo::pointer>::const_iterator pIterCallList = theApp.m_pCallList.begin();
				for (; pIterCallList!=theApp.m_pCallList.end(); pIterCallList++)
				{
					const CEBCCallInfo::pointer& pEbCallInfo = pIterCallList->second;
					const eb::bigint sCallid = pEbCallInfo->m_pCallInfo.GetCallId();
					if ((tNow-pEbCallInfo->m_tLastTime)<=5*60) continue;

#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
					const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
					if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
					{
						// *跑下面
					}else if (!theApp.GetHideMainFrame())
#else
					if (!theApp.GetHideMainFrame())
#endif
					{
						if (!CFrameWndInfoProxy::ExistCallIdWnd(sCallid))
						//if (CFrameWndInfoProxy::RemoveHideWnd(sCallid) || !CFrameWndInfoProxy::ExistCallIdWnd(sCallid))
						{
							theApp.m_pCallList.erase(pIterCallList);
							// 这里主要用于处理自动响应，没有打开聊天界面会话
							// *其实在OnMessageCallConnected已经有处理；
#ifdef USES_EBCOM_TEST
							theEBClientCore->EB_CallExit(sCallid);
#else
							theEBAppClient.EB_CallExit(sCallid);
#endif
							break;
						}
						continue;
					}
#endif
					if (m_pDlgFrameList!=NULL && !m_pDlgFrameList->ExistCallIdWnd(sCallid))
					{
						theApp.m_pCallList.erase(pIterCallList);
						// 这里主要用于处理自动响应，没有打开聊天界面会话
						// *其实在OnMessageCallConnected已经有处理；
#ifdef USES_EBCOM_TEST
						theEBClientCore->EB_CallExit(sCallid);
#else
						theEBAppClient.EB_CallExit(sCallid);
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
	case TIMERID_SHOW_EMOTION:
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

bool CPOPDlg::AddSubscribeFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo)
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
		{
			// 更新图标
			if (pFuncButtonInfo->GetResFile().empty() && !pSubscribeFuncInfo.m_sResFile.empty())
			{
				pFuncButtonInfo->UpdateResFile(pSubscribeFuncInfo.m_sResFile);
			}
			return true;
		}
	}

	CRect rectClient;
	this->GetClientRect(&rectClient);
	CRect rectFuncButton;
#ifdef USES_NEW_UI_1220
	int nBorderLeft = m_bShowedLeft?const_border_left:const_hide_border_left1;
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		nBorderLeft = 1;
#endif
	rectFuncButton.left = nBorderLeft+2 + m_pMainFuncButtonList.size()*(const_func_button_size+const_func_button_intever);
#else
	rectFuncButton.left = const_border_left+4 + m_pMainFuncButtonList.size()*(const_func_button_size+const_func_button_intever);
#endif
	rectFuncButton.right = rectFuncButton.left + const_func_button_size;
	rectFuncButton.bottom = rectClient.bottom-2;
	rectFuncButton.top = rectFuncButton.bottom-const_func_button_size;
	CEBFuncButton::pointer pFuncButton = CEBFuncButton::create();
	const UINT nID = EB_COMMAND_MAINFRAME_FUNC+m_pMainFuncButtonList.size();
	if (pFuncButton->Create(this,rectFuncButton,nID,pSubscribeFuncInfo,theApp.m_sDefaultAppImage))
	{
		m_pMainFuncButtonList.push_back(pFuncButton);
		if (GetCallFrameShowed())
		{
			pFuncButton->ShowWindow(false);
			//pFuncButton->m_btn.ShowWindow(SW_HIDE);
		}

		if (m_btnNewApp.GetSafeHwnd()!=NULL)
		{
			const int x = rectFuncButton.right;//+const_func_button_intever;
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
bool CPOPDlg::AddAppFrameFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo)
{
#ifdef USES_FRAMELIST_APPFRAME
	CDlgAppFrame* pDlgAppFrame = NewAppFrame(false);
	if (pDlgAppFrame!=NULL)
		return pDlgAppFrame->AddSubscribeFuncInfo(pSubscribeFuncInfo);
	else
		return false;
#else
	NewAppFrame();
	return m_pDlgAppFrame->AddSubscribeFuncInfo(pSubscribeFuncInfo);
#endif
}

void CPOPDlg::DeleteItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(hItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType!=CTreeItemInfo::ITEM_TYPE_URL) return;

	tstring sUrl(pTreeItemInfo->m_sAccount);
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
bool CPOPDlg::CallItem(HTREEITEM hItem)
{
	if (hItem==NULL) return false;
	//if (bFindChild)
	////if (!bSelChange)
	//{
	//	HTREEITEM hChildItem = m_treeSearch.GetChildItem(hItem);
	//	if (hChildItem!=NULL)
	//		hItem = hChildItem;
	//}
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return false;
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
	{
		if (m_treeSearch.GetItemState(hItem,TVIS_EXPANDED)&TVIS_EXPANDED)
			m_treeSearch.Expand(hItem,TVE_COLLAPSE);
		else
			m_treeSearch.Expand(hItem,TVE_EXPAND);
		return false;
	}
	bool bResult = true;
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_SUBINFO)
	{
		EB_SubscribeFuncInfo pSubscribeFuncInfo;
		if (theEBAppClient.EB_GetSubscribeFuncInfo(pTreeItemInfo->m_sId,&pSubscribeFuncInfo))
			bResult = theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
		else
			bResult = false;
	}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_URL)
	{
		this->AddAppUrl(true,pTreeItemInfo->m_sAccount,"",NULL);
	}else if (pTreeItemInfo->m_nUserId>0)
	{
		theApp.m_pAutoCallFromUserIdList.remove(pTreeItemInfo->m_nUserId);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallUserId(pTreeItemInfo->m_nUserId,0);
#else
		theEBAppClient.EB_CallUserId(pTreeItemInfo->m_nUserId,0);
#endif
	}else if (pTreeItemInfo->m_sMemberCode>0)
	{
		theApp.m_pAutoCallFromUserIdList.remove(pTreeItemInfo->m_nUserId);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#else
		theEBAppClient.EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#endif
	}else
	{
		theApp.m_pAutoCallFromUserIdList.remove(pTreeItemInfo->m_nUserId);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallAccount(pTreeItemInfo->m_sAccount.c_str(),0);
#else
		theEBAppClient.EB_CallAccount(pTreeItemInfo->m_sAccount.c_str(),0);
#endif
	}
	//this->ShowWindow(SW_HIDE);
	this->HideSearchResult();
	return bResult;
}
//
//LRESULT CPOPDlg::OnTreeItemSelChange(WPARAM wp, LPARAM lp)
//{
//	HTREEITEM item = (HTREEITEM)wp;
//	VividTree* pOwner = (VividTree*)lp;
//	if (item!=NULL && pOwner == &m_treeSearch)
//	{
//		CallItem(item,true);
//	}
//	return 0;
//}
LRESULT CPOPDlg::OnTreeItemDoubleClick(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	VividTree* pOwner = (VividTree*)lp;
	if (pOwner == &m_treeSearch)
	{
		m_editSearch.SetFocus();
		CallItem(item);
	}
	return 0;
}

LRESULT CPOPDlg::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	try
	{
		//boost::mutex::scoped_lock lockSearch(m_mutexSearch);
		HTREEITEM hItem = (HTREEITEM)wp;
		VividTree * pOwner = (VividTree*)lp;
		if (pOwner == &m_treeSearch)
		{
			if (hItem == NULL)// || !m_treeSearch.IsWindowVisible())
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
				if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_SUBINFO)
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
				const int const_dlg_height = 220;
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
	}catch(const std::exception&)
	{
	}catch(...)
	{
	}
	return 0;
}
BOOL CPOPDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	//if (pMsg->message == WM_KEYDOWN && (GetKeyState(VK_SHIFT)&0x80)==0x80)
	//{
	//	// VK_CONTROL VK_SHIFT VK_MENU（不行）
	//	if (pMsg->wParam == '1')
	//	{
	//		OnBnClickedButtonSwitchFrame();
	//		return TRUE;
	//	}
	//}
	if (pMsg->message == WM_KEYDOWN)
	{
		const bool bShift = (GetKeyState(VK_SHIFT)&0x80)==0x80?true:false;
		const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
		if (bShift && bControl)
		{
			if (pMsg->wParam=='S')
			{
				this->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL,1,1);
				return TRUE;
			}
		}
		
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
	//if ((pMsg->message == WM_KEYDOWN || pMsg->message == WM_LBUTTONUP) && m_bShrinkageWin && this->IsWindowVisible() && m_pDlgShrinkageBar->IsWindowVisible())
	//{
	//	m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
	//}

	const bool bShowToolBar = m_bShowedToolbar&&GetMainFrameShowed();
	if (bShowToolBar && pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_editDescription.GetSafeHwnd())
	{
//#ifndef USES_NEW_UI_160111
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
#else
		if (false)
		{
#endif
			m_editDescription.SetReadOnly(FALSE);
			m_editDescription.ShowWindow(SW_SHOW);
		}
		// 以上没用
		//if (m_editDescription.GetSafeHwnd() && this->GetFocus() != &m_editDescription)
		//{
		//	m_editDescription.SetFocus();
		//	m_editDescription.SetSel(0,-1);
		//}
//#endif
	}else if (pMsg->message == WM_LBUTTONDOWN && pMsg->hwnd == m_editSearch.GetSafeHwnd())
	{
		m_bCanSearch = true;
		if (this->GetFocus()!=&m_editSearch)
		{
			this->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL,1,1);
		}
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_editSearch.GetSafeHwnd())
	{
		m_bCanSearch = true;
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnSearchTrackDel.GetSafeHwnd())
	{
		DeleteItem(m_treeSearch.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnSearchTrackCall.GetSafeHwnd())
	{
		m_editSearch.SetFocus();
		SetTimer(TIMERID_CALL_SEARCH_TRACK,1,NULL);
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_treeSearch.GetSafeHwnd())
	{
		SetTimer(TIMERID_CALL_SEARCH_SELECTED,1,NULL);
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && pMsg->hwnd == m_treeSearch.GetSafeHwnd())
	{
		m_editSearch.SetFocus();
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
		boost::mutex::scoped_lock lockSearch(m_mutexSearch);
		if (pMsg->wParam == VK_ESCAPE)
		{
			//if (m_editSearch.GetWindowTextLength()>0)
			//{
			//	m_editSearch.SetWindowText(_T(""));
			//	m_editSearch.SetFocus();
			//	return TRUE;
			//}else if (m_treeSearch.IsWindowVisible())
			//{
			//	this->HideSearchResult();
			//}
			//if (m_treeSearch.IsWindowVisible() && m_treeSearch.GetCount()>0)
			{
				//m_bCanSearch = false;
				m_editSearch.SetWindowText(_T(""));
				m_editSearch.SetFocus();
			}
		}else if (pMsg->wParam == VK_DOWN)
		{
			if (m_treeSearch.IsWindowVisible() && m_treeSearch.GetCount()>0)
			{
				m_treeSearch.SetFocus();
				if (m_treeSearch.GetSelectedItem()!=0)
				{
					if (m_treeSearch.GetParentItem(m_treeSearch.GetSelectedItem())==0)
						m_treeSearch.Expand(m_treeSearch.GetSelectedItem(),TVE_EXPAND);
					else
						m_treeSearch.SelectItem(m_treeSearch.GetNextItem(m_treeSearch.GetSelectedItem(),TVGN_NEXT));
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
					m_bCanSearch = false;
#ifdef USES_APPFRAME
					CString sSearchStringTemp(sSearchString);
					sSearchStringTemp.MakeLower();
					const bool bIsEmail = sSearchStringTemp.Find(_T("@"))>=0?true:false;
					//const bool bHasPoint = sSearchStringTemp.Find(_T("."))>0?true:false;
					if (sSearchString.Find("(")>0 && sSearchString.Find(")")>0 && CallItem(m_treeSearch.GetSelectedItem()))
					{
						return TRUE;
					}else if (!bIsEmail && sSearchString.Left(1)=="/" && sSearchString.GetLength()>1 && CallItem(m_treeSearch.GetSelectedItem()))
					{
						return TRUE;
					}

					if (!bIsEmail && 
						(sSearchStringTemp.Find(_T("."))>0 ||	// * 非邮件，带 . 默认处理成链接（www.)
						sSearchStringTemp.Find(_T("/"))>0 ||	// (http:// https://)
						sSearchStringTemp.Find(_T("="))>0 ||	// URL
						sSearchStringTemp==_T("about:blank")))
					//if (sSearchStringTemp.Find(_T("http://"))==0 ||
					//	sSearchStringTemp.Find(_T("https://"))==0 ||
					//	sSearchStringTemp.Find(_T("www."))==0 ||
					//	sSearchStringTemp.Find(_T("/"))>0 ||
					//	sSearchStringTemp.Find(_T("="))>0 ||
					//	sSearchStringTemp==_T("about:blank") ||
					//	(!bIsEmail && (sSearchStringTemp.Find(_T(".com"))>0 || sSearchStringTemp.Find(_T(".cn"))>0 || sSearchStringTemp.Find(_T(".net"))>0))
					//	)
					{
						AddAppUrl(true,(LPCTSTR)sSearchString,"",NULL);
					}else
#endif
					{

#ifdef USES_EBCOM_TEST
						theEBClientCore->EB_CallAccount((LPCTSTR)sSearchString,0);
#else
						theEBAppClient.EB_CallAccount(sSearchString,0);
#endif
					}
					this->HideSearchResult();
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
//			tstring sOutDesc = theEBAppClient.EB_GetDescription();
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
	return CEbDialogBase::PreTranslateMessage(pMsg);
}
LRESULT CPOPDlg::OnIconNotification(WPARAM wParam, LPARAM lParam)
{
	if(wParam != m_trayIconData.uID)    // 不是自己的图标发来的消息，退出
		return FALSE;
	switch(lParam)  // 过滤消息
	{
	//case WM_MOUSEHWHEEL:
	//	{
	//		if (m_pDlgMsgTip!=NULL && m_pDlgMsgTip->GetSafeHwnd()!=NULL && m_pDlgMsgTip->IsWindowVisible())
	//		{
	//			m_pDlgMsgTip->ShowWindow(SW_HIDE);
	//		}
	//	}break;
	case WM_MOUSEMOVE:
		{
			// 显示消息
			if (m_pDlgMsgTip!=NULL && !m_pDlgMsgTip->IsMsgEmpty() && (!m_pDlgMsgTip->IsWindowVisible() || m_pDlgMsgTip->IsIconic()))
			{
				m_pDlgMsgTip->SetCurrentTrayIconPos();
				m_pDlgMsgTip->ShowMsgTip();
			}
		}break;
	case WM_RBUTTONDOWN:
		{
			int const_app_func_index = 4;	// ***修改菜单项，需要修改这个地方
			if (!theApp.GetDisableUserCloudDrive())
				const_app_func_index += 1;
			if (m_menuTray.GetSafeHmenu()==NULL)
			{
				m_menuTray.CreatePopupMenu();
				m_menuTray.SetBitmapBackground(RGB(192,192,192));
				if (!theApp.IsLogonVisitor())
				{
					m_menuTray.InsertODMenu(0,_T("在线"),MF_BYPOSITION,EB_COMMAND_STATE_ONLINE,IDB_BITMAP_MENU_ONLINE);
					m_menuTray.InsertODMenu(1,_T("离开"),MF_BYPOSITION,EB_COMMAND_STATE_AWAY,IDB_BITMAP_MENU_AWAY);
					m_menuTray.InsertODMenu(2,_T("忙碌"),MF_BYPOSITION,EB_COMMAND_STATE_BUSY,IDB_BITMAP_MENU_BUSY);
					m_menuTray.AppendMenu(MF_SEPARATOR);
					if (!theApp.GetDisableUserCloudDrive())
					{
						m_menuTray.InsertODMenu(-1,_T("我的云盘"),MF_BYPOSITION,EB_COMMAND_MY_SHARE,IDB_BITMAP_MENU_SHARE);
						m_menuTray.AppendMenu(MF_SEPARATOR);
					}
					m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_OPEN_MAIN,_T("打开主面板"));
#ifdef USES_SUPPORT_UI_STYLE
					if (theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT)
					{
						//m_menuTray.InsertODMenu(-1,"打开工作台",MF_BYCOMMAND,EB_COMMAND_OPEN_WORKFRAME,IDB_PNG_WORKFRAME);
						m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_OPEN_WORKFRAME,_T("打开工作台"));
						m_nMenuIconOpenWorkFrameIndex = (int)m_menuTray.GetMenuItemCount();
					}
#endif // USES_SUPPORT_UI_STYLE
					m_menuTray.AppendMenu(MF_SEPARATOR);
				}
				m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_LOGOUT,_T("注销"));
				m_menuTray.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EXIT_APP,_T("退出"));
			}else
			{
#ifdef USES_SUPPORT_UI_STYLE
				if (theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT)
				{
					if (m_nMenuIconOpenWorkFrameIndex<0)
					{
						m_nMenuIconOpenWorkFrameIndex = m_menuTray.GetMenuPosition("打开主面板");
						if (m_nMenuIconOpenWorkFrameIndex>0)
						{
							m_menuTray.InsertODMenu(m_nMenuIconOpenWorkFrameIndex+1,"打开工作台",MF_BYPOSITION,EB_COMMAND_OPEN_WORKFRAME);
							m_nMenuIconOpenWorkFrameIndex += 1;
						}
					}
				}else if (m_nMenuIconOpenWorkFrameIndex>0)
				{
					m_menuTray.RemoveMenu(m_nMenuIconOpenWorkFrameIndex,MF_BYPOSITION);		// "打开工作台" menu
					m_nMenuIconOpenWorkFrameIndex = -1;
				}
#endif

				if (m_bRemoveAppFuncIndex2)
					m_menuTray.RemoveMenu(const_app_func_index,MF_BYPOSITION);
				m_bRemoveAppFuncIndex2 = false;
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

			// 集成应用菜单
			CNewMenu pPopupMenuApps;
			pPopupMenuApps.CreatePopupMenu();
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
					bool bAppendMenuError = true;
					if (::PathFileExists(pSubscribeFuncInfo.m_sResFile.c_str()))
					{
						CImageEx pImageEx;
						pImageEx.LoadFilename(pSubscribeFuncInfo.m_sResFile.c_str());
						const int nWidth = pImageEx.GetWidth();
						const int nHeight = pImageEx.GetHeight();
						if (nWidth>0 && nHeight>0)
						{
							HGDIOBJ hBitmap = NULL;
							if ((nWidth/nHeight)>2)
							{
								if (nHeight>const_default_menu_image_size)
									hBitmap = pImageEx.Copy(0,0,const_default_menu_image_size,const_default_menu_image_size);
								else
									hBitmap = pImageEx.Copy(0,0,nHeight,nHeight);
							}else
							{
								if (nHeight!=const_default_menu_image_size)
									hBitmap = pImageEx.StrctchCopy(0,0,const_default_menu_image_size,const_default_menu_image_size);
								else
									hBitmap = pImageEx.Copy(0,0,const_default_menu_image_size,const_default_menu_image_size);
							}
							if (hBitmap!=NULL)
							{
								CBitmap pBitmap;
								pBitmap.Attach(hBitmap);
								pPopupMenuApps.AppendODMenu(pSubscribeFuncInfo.m_sFunctionName.c_str(),MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,&pBitmap);
								bAppendMenuError = false;
							}
						}
					}
					if (bAppendMenuError)
					{
						pPopupMenuApps.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,pSubscribeFuncInfo.m_sFunctionName.c_str());
					}
				}
				m_bRemoveAppFuncIndex2 = true;
				m_menuTray.InsertODMenu(const_app_func_index,_T("集成应用"),MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenuApps.m_hMenu,IDB_BITMAP_SELECT_COLOR);
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
			if (m_pDlgMsgTip!=NULL && !m_pDlgMsgTip->IsMsgEmpty())
			{
				m_pDlgMsgTip->ViewAllMsg();
				break;
			}

			if (theApp.GetHideMainFrame() && theApp.GetAutoOpenSubId()>0)
			{
				theApp.ShowHideAutoOpenSubscribeFunc(true);
			}else if (!IsIconic() && this->IsWindowVisible())
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

#ifdef USES_EBCOM_TEST
void CPOPDlg::Fire_onSearchMemberInfo(IEB_GroupInfo* pGroupInfo, IEB_MemberInfo* pMemberInfo, ULONG dwParam)
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
			m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
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
			m_treeSearch.Sort(pMemberGroupInfo->m_hItem,CPOPApp::TreeCmpFunc);
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
void CPOPDlg::onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam)
{
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
			m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
			if ((GetMainFrameShowed() || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
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
			m_treeSearch.Sort(pMemberGroupInfo->m_hItem,CPOPApp::TreeCmpFunc);
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
#endif

#ifdef USES_EBCOM_TEST
void CPOPDlg::Fire_onSearchContactInfo(IEB_ContactInfo* pContactInfo, ULONG dwParam)
{
	 if (dwParam == 1)
	{
		// search
		const CEBString sContact(pContactInfo->Contact.GetBSTR());
		const CEBString sName(pContactInfo->Name.GetBSTR());

		const tstring theGroupContactAccount("[* *]");
		const CString sGroupResultName = _T("联系人搜索结果：");
		CTreeItemInfo::pointer pContactGroupInfo;
		if (!m_pContactItem.find(theGroupContactAccount,pContactGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pContactGroupInfo->m_dwItemData = 1;
			m_pContactItem.insert(theGroupContactAccount,pContactGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
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
void CPOPDlg::onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam)
{
	 if (dwParam == 1)
	{
		// search
		const tstring theGroupContactAccount("[* *]");
		const CString sGroupResultName = _T("联系人搜索结果：");
		CTreeItemInfo::pointer pContactGroupInfo;
		if (!m_pContactItem.find(theGroupContactAccount,pContactGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pContactGroupInfo->m_dwItemData = 1;
			m_pContactItem.insert(theGroupContactAccount,pContactGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
			if ((GetMainFrameShowed() || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hEmpItem,TVGN_CARET);
		}
		if (!m_pContactItem.exist(pContactInfo->m_sContact))
		{
			CString sSearchName;
			sSearchName.Format(_T("%s(%s)"),pContactInfo->m_sName.c_str(),pContactInfo->m_sContact.c_str());
			HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
			CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
			pContactItemInfo->m_sId = pContactInfo->m_nContactId;
			pContactItemInfo->m_nUserId = pContactInfo->m_nContactUserId;
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
}
#endif

void CPOPDlg::ScrollTreeSearchToTop(void)
{
	const int nScrollPos = m_treeSearch.GetScrollPos(SB_VERT);
	if (nScrollPos>0)
	{
		WPARAM wParam = MAKELONG(SB_TOP,nScrollPos);
		m_treeSearch.SendMessage(WM_VSCROLL,wParam);
	}
}
//void CPOPDlg::OnFrameWndEmpty(void)
//{
//	//if (m_bShrinkageWin)
//	//{
//	//	KillTimer(TIMERID_SHRINKAGE_WIN);
//	//	m_bShrinkageWin = false;
//	//	m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
//	//}
//	//this->ShowWindow(SW_HIDE);
//	if (m_rectAdjustOld.Width()>0)
//	{
//		this->MoveWindow(m_rectAdjustOld);
//	}
//}

void CPOPDlg::OnFrameWndShow(const CFrameWndInfo::pointer& pFrameWndInfo, bool bShow)
{
#ifdef USES_FRAMELIST_APPFRAME
	CDlgAppFrame * pDlgAppFrame = NewAppFrame(false);
#else
	NewAppFrame();
	CDlgAppFrame * pDlgAppFrame = m_pDlgAppFrame;
#endif
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)
		{
#ifndef USES_FRAMELIST_APPFRAME
			if (pDlgAppFrame!=0) {
				if (bShow)
				{
					//OnBnClickedButtonSwitchFrame();	// *** 有异常
					if (pDlgAppFrame==NULL || pDlgAppFrame->IsEmpty())
					{
						AddAppUrl(false,"about:blank",NULL);
						return;
					}
					CRect rectFrameList;
					m_pDlgFrameList->GetClientRect(&rectFrameList);
					const int nBorderLeft = this->m_pDlgFrameList->GetShowedLeft()?const_border_left:const_hide_border_left2;
					pDlgAppFrame->MoveWindow(nBorderLeft,const_border_size+theSearchToolbarHeight,rectFrameList.Width()-nBorderLeft-const_border_size,rectFrameList.Height()-const_border_size*2-theSearchToolbarHeight);
					pDlgAppFrame->ShowWindow(SW_SHOW);
					pDlgAppFrame->RefreshAppWnd();
					//SetTimer(TIMERID_REFRESH_APPFRAME,1,NULL);
				}else
				{
					pDlgAppFrame->ShowWindow(SW_HIDE);
				}
			}
#endif
		}else if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG && pFrameWndInfo->GetDialog().get()!=NULL)
		{
			//pFrameWndInfo->GetDialog()->ShowWindow(bShow?SW_SHOW:SW_HIDE);	// ** CDlgFrameList已经处理
			if (bShow && m_pDlgMsgTip!=NULL && m_pDlgMsgTip->GetSafeHwnd()!=NULL)
			{
				m_pDlgMsgTip->DelMsgTip(pFrameWndInfo->GetDialog()->GetGroupId(),pFrameWndInfo->GetDialog()->GetFromUserId());
			}
		}
		return;
	}
#endif
	if (bShow)
	{
		if (m_nFrameType != pFrameWndInfo->GetType())
		{
			m_nFrameType = pFrameWndInfo->GetType();
			SwitchFrameWnd();
			if (m_nFrameType!=CFrameWndInfo::FRAME_WND_CALL_DIALOG && CFrameWndInfoProxy::GetSize()==2 && m_rectAdjustOld.Width()>0)
			{
				if (pDlgAppFrame==NULL || (const_border_left+pDlgAppFrame->GetNeedWidth())<m_rectAdjustOld.Width())
				{
					this->MoveWindow(m_rectAdjustOld);
					memset(&m_rectAdjustOld,0,sizeof(m_rectAdjustOld));
				}
			}
		}
		if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG && pFrameWndInfo->GetDialog().get()!=NULL && m_pDlgMsgTip!=NULL && m_pDlgMsgTip->GetSafeHwnd()!=NULL)
		{
			m_pDlgMsgTip->DelMsgTip(pFrameWndInfo->GetDialog()->GetGroupId(),pFrameWndInfo->GetDialog()->GetFromUserId());
		}
	}else if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME && pDlgAppFrame!=NULL)
	{
		pDlgAppFrame->ShowWindow(SW_HIDE);
		if (m_pPanelSearch!=NULL)
			m_pPanelSearch->ShowWindow(SW_HIDE);
	}
}
	
#ifndef USES_FRAMELIST_APPFRAME
void CPOPDlg::OnFrameSize(const CFrameWndInfo::pointer& pFrameWndInfo, int cx, int cy)
{
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)
			{
				if (m_pDlgAppFrame!=NULL)
				{
					const int nBorderLeft = this->m_pDlgFrameList->GetShowedLeft()?const_border_left:const_hide_border_left2;
					m_pDlgAppFrame->MoveWindow(nBorderLeft,const_border_size+theSearchToolbarHeight,cx-nBorderLeft-const_border_size,cy-const_border_size*2-theSearchToolbarHeight);
				}
			}
			return;
		}
#endif
}
size_t CPOPDlg::OnGetFrameSize(const CFrameWndInfo::pointer& pFrameWndInfo) const
{
	if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME)
	{
		if (m_pDlgAppFrame!=NULL)
		{
			return m_pDlgAppFrame->GetSize();
		}
	}
	return 0;
}
#endif


//void CPOPDlg::OnListEmpty(void)
//{
//	if (m_bShrinkageWin)
//	{
//		KillTimer(TIMERID_SHRINKAGE_WIN);
//		m_bShrinkageWin = false;
//		m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
//	}
//	this->ShowWindow(SW_HIDE);
//	//if (m_rectAdjustOld.Width()>0)
//	//{
//	//	this->MoveWindow(m_rectAdjustOld);
//	//}
//
//}

void CPOPDlg::ShowSearchResult(void)
{
	if (!m_treeSearch.IsWindowVisible())
	{
		m_btnSwitchFrame.ShowWindow(SW_HIDE);
		CRect searchRect;
		m_editSearch.GetWindowRect(&searchRect);
		CRect rect;
		m_pDlgMyContacts->GetWindowRect(&rect);
		searchRect.top = searchRect.bottom+1;
		searchRect.bottom = rect.bottom;
#ifdef USES_NEW_UI_160111
		searchRect.right = rect.right;
		m_btnMyCenter.ShowWindow(SW_HIDE);
		m_btnFileMgr.ShowWindow(SW_HIDE);
		m_btnMyShare.ShowWindow(SW_HIDE);
		m_btnMainFunc.ShowWindow(SW_HIDE);
#endif
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
		if (m_pDlgAppFrame!=NULL && GetWorkFrameShowed() && m_pDlgAppFrame->IsWindowVisible())
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
		if (GetCallFrameShowed())
		{
			CFrameWndInfoProxy::ShowHideCurrent(false);
		}
	}
}
void CPOPDlg::OnEnChangeEditSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEbDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!m_bCanSearch) return;
	//KillTimer(TIMERID_EDIT_SEARCH);
	//SetTimer(TIMERID_EDIT_SEARCH,800,NULL);

	CString sSearchString;
	m_editSearch.GetWindowText(sSearchString);
	if (!sSearchString.IsEmpty())
	{
		KillTimer(TIMERID_EDIT_SEARCH);
		SetTimer(TIMERID_EDIT_SEARCH,500,NULL);
	}else
	{
		theSearchString = "";
		HideSearchResult();
	}
}
LRESULT CPOPDlg::OnMsgRefreshUrl(WPARAM wParam, LPARAM lParam)
{
	//if (m_pDlgAppFrame!=0)
	//	m_pDlgAppFrame->doRefresh();
	return 0;
}
LRESULT CPOPDlg::OnMsgRefreshOrStopUrl(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->doRefreshOrStop();
	return 0;
}
LRESULT CPOPDlg::OnMsgRedrawAppFrame(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_FRAMELIST_APPFRAME
#else
	if (m_pDlgAppFrame!=0)
	{
		m_pDlgAppFrame->ShowWindow(SW_SHOW);
		m_pDlgAppFrame->RefreshAppWnd();
		//SetTimer(TIMERID_REFRESH_APPFRAME,1,NULL);
	}
#endif
	return 0;
}
LRESULT CPOPDlg::OnMsgGoBack(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->goBack();
	return 0;
}
LRESULT CPOPDlg::OnMsgGoForward(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->goForward();
	return 0;
}
LRESULT CPOPDlg::OnMsgChangeBrowserType(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->changeBrowserType();
	return 0;
}
LRESULT CPOPDlg::OnMsgQueryCanSaveHistory(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		return (LRESULT)(m_pDlgAppFrame->canSaveHistory()?1:0);
	return (LRESULT)0;
}
LRESULT CPOPDlg::OnMsgQueryBrowserType(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		return (LRESULT)m_pDlgAppFrame->queryBrowserType();
	return (LRESULT)EB_BROWSER_TYPE_CEF;
}
LRESULT CPOPDlg::OnMsgSaveHistory(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgAppFrame!=0)
		m_pDlgAppFrame->saveHistory();
	return 0;
}
LRESULT CPOPDlg::OnMsgSearchSetFocusSel(WPARAM wParam, LPARAM lParam)
{
	if (wParam==1)	
		m_editSearch.SetFocus();
	if (lParam==1)
	{
		const int nTextLength = m_editSearch.GetWindowTextLength();
		if (nTextLength>0)
		{
			SetTimer(TIMERID_CALL_SEARCH_SEL_ALL,200,NULL);	// ** 解决有时不能全选问题
			//m_editSearch.SetSel(0,nTextLength);
		}
		//m_editSearch.SetSel(0,-1);	// *文本太长，点击前面时，不会选择到后面
	}
	return 0;
}
LRESULT CPOPDlg::OnFrameWndMin(WPARAM wParam, LPARAM lParam)
{
	//const CWnd * pWnd = (CWnd*)wParam;
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	return 0;
}
LRESULT CPOPDlg::OnFrameWndMax(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedButtonMax();
	//int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	////int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	//int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	//m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);

	//bool bSetBtnMax = false;
	//static CRect theRestoreRect;
	//CRect rect;
	//GetWindowRect(&rect);
	//if (rect.Width() == m_nScreenWidth)
	//{
	//	bSetBtnMax = true;
	//	MoveWindow(&theRestoreRect);
	//}else
	//{
	//	theRestoreRect = rect;
	//	rect.left = 0;
	//	rect.top = 0;
	//	rect.right = rect.left + m_nScreenWidth;
	//	rect.bottom = rect.top + m_nScreenHeight;
	//	MoveWindow(&rect);
	//}

	//BoostReadLock rdLock(m_pList.mutex());
	//CLockList<CFrameWndInfo::pointer>::iterator pIter = m_pList.begin();
	//for (; pIter!=m_pList.end(); pIter++)
	//{
	//	CFrameWndInfo::pointer pFrameWndInfo = *pIter;
	//	if (pFrameWndInfo->GetDialog().get()!=NULL)
	//	{
	//		pFrameWndInfo->GetDialog()->SetBtnMax(bSetBtnMax);
	//	}
	//}
	return 0;
}

LRESULT CPOPDlg::OnFrameWndClose(WPARAM wParam, LPARAM lParam)
{
	const CWnd * pWnd = (CWnd*)wParam;
	CFrameWndInfoProxy::DelWnd(pWnd);
	return 0;
}
LRESULT CPOPDlg::OnFrameWndCount(WPARAM wParam, LPARAM lParam)
{
	const HWND pWnd = (HWND)wParam;
	const int nCount = (int)lParam;
	if (pWnd!=NULL && m_pDlgAppFrame!=NULL && pWnd==m_pDlgAppFrame->GetSafeHwnd())
	{
		if (nCount==0)
		{
			CFrameWndInfoProxy::SetTitle(CFrameWndInfo::FRAME_WND_WORK_FRAME, _T("工作台"), _T("工作台 Ctrl+1"));
			if (!m_bShowedLeft)
				CFrameWndInfoProxy::SetUnreadMsg(CFrameWndInfo::FRAME_WND_WORK_FRAME,0);
		}else
		{
			CString sText;
			sText.Format(_T("工作台  %d"),nCount);
			CString sTooltip;
			sTooltip.Format(_T("工作台(%d) Ctrl+1"),nCount);
			CFrameWndInfoProxy::SetTitle(CFrameWndInfo::FRAME_WND_WORK_FRAME, sText, sTooltip);
			if (!m_bShowedLeft)
				CFrameWndInfoProxy::SetUnreadMsg(CFrameWndInfo::FRAME_WND_WORK_FRAME,(size_t)nCount);
		}
	}
	return 0;
}

LRESULT CPOPDlg::OnMessageMoveUp(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// *跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		CFrameWndInfoProxy::ShowUpWnd();
		return 0;
	}
#endif
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
	return 0;
}
LRESULT CPOPDlg::OnMessageMoveDown(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// *跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		CFrameWndInfoProxy::ShowDownWnd();
		return 0;
	}
#endif
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
	return 0;
}
LRESULT CPOPDlg::OnMessageMoveOffset(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// *跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		const int nOffset = (int)wParam;
		CFrameWndInfoProxy::ShowOffsetWnd(nOffset-1);
		return 0;
	}
#endif
	if (m_pDlgFrameList!=NULL)
		m_pDlgFrameList->PostMessage(EB_COMMAND_RAME_WND_MOVE_OFFSET, wParam, 0);
	return 0;
}
LRESULT CPOPDlg::OnMessageAdjustWidth(WPARAM wParam, LPARAM lParam)
{
	//const HWND hAdjustWidthWnd = (HWND)wParam;
	//if (this->IsIconic()) return 0;
	const int nWidthOffset = (int)lParam;
	CRect rectClient;
	this->GetWindowRect(&rectClient);
	if (m_rectAdjustOld.Width()==0 || m_rectAdjustOld.Height()==0)
		m_rectAdjustOld = rectClient;
	const int nScreenWidth = theApp.GetScreenWidth();
	if (m_rectWin.right==nScreenWidth)
	//if (m_bShrinkageWin && m_rectWin.right==nScreenWidth)
		rectClient.left -= nWidthOffset;
	else
		rectClient.right += nWidthOffset;
	if (rectClient.right>nScreenWidth)
	{
		//const int nOffset = rectClient.right-nScreenWidth;
		rectClient.right = nScreenWidth;
		if (rectClient.left==0)
		{
			rectClient.right += 1;
		}
		//if (rectClient.left>nOffset)
		//	rectClient.left -= nOffset;
		//else
		//	rectClient.left = 0;

	}
	this->MoveWindow(rectClient);
	return 0;
}
//LRESULT CPOPDlg::OnMsgOpenSubId(WPARAM wParam, LPARAM lParam)
//{
//	COpenAppUrlInfo * pOpenAppUrlInfo = (COpenAppUrlInfo*)wParam;
//	if (pOpenAppUrlInfo==0) return 0;
//	EB_SubscribeFuncInfo pSubscribeFuncInfo;
//	if (theEBAppClient.EB_GetSubscribeFuncInfo(pOpenAppUrlInfo->m_sOpenSubId,&pSubscribeFuncInfo))
//	{
//		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"",pOpenAppUrlInfo->m_sParam,pOpenAppUrlInfo->m_hwndFrom);
//	}
//	delete pOpenAppUrlInfo;
//}
LRESULT CPOPDlg::OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam)
{
	COpenAppUrlInfo * pOpenAppUrlInfo = (COpenAppUrlInfo*)wParam;
	if (pOpenAppUrlInfo==0) return 0;
	if (pOpenAppUrlInfo->m_sOpenSubId>0)
	{
		EB_SubscribeFuncInfo pSubscribeFuncInfo;
		if (theEBAppClient.EB_GetSubscribeFuncInfo(pOpenAppUrlInfo->m_sOpenSubId,&pSubscribeFuncInfo))
		{
			theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"",pOpenAppUrlInfo->m_sParam,pOpenAppUrlInfo->m_hwndFrom);
		}
	}else if (!pOpenAppUrlInfo->m_sShellExecute.empty())
	{
		ShellExecute(NULL, "Open", pOpenAppUrlInfo->m_sShellExecute.c_str(), pOpenAppUrlInfo->m_sParam.c_str(), pOpenAppUrlInfo->m_sDir.c_str(), SW_SHOW);
	}else 
	{
		EB_SubscribeFuncInfo pSubscribeFuncInfo;
		pSubscribeFuncInfo.m_sFunctionName = pOpenAppUrlInfo->m_sUrl;
		pSubscribeFuncInfo.m_bDisableContextMenu = (pOpenAppUrlInfo->m_nOpenParam&0x1)!=0;;
		pSubscribeFuncInfo.m_bDisableScrollBar = (pOpenAppUrlInfo->m_nOpenParam&0x2)!=0;
		pSubscribeFuncInfo.m_bClosePrompt = (pOpenAppUrlInfo->m_nOpenParam&0x4)!=0;
		pSubscribeFuncInfo.m_bWinResizable = true;
		const bool nOpenNewClose = (pOpenAppUrlInfo->m_nOpenParam&0x8)!=0;
		this->AddAppUrl(pOpenAppUrlInfo->m_bSaveBrowseTitle,pOpenAppUrlInfo->m_sUrl,pOpenAppUrlInfo->m_sPostData,pSubscribeFuncInfo,nOpenNewClose,pOpenAppUrlInfo->m_hwndFrom);
	}
	delete pOpenAppUrlInfo;

	//char * sUrl = (char*)wParam;
	//if (sUrl==0 || strlen(sUrl)==0) return 0;
	//EB_SubscribeFuncInfo pSubscribeFuncInfo;
	//pSubscribeFuncInfo.m_sFunctionName = sUrl;
	//pSubscribeFuncInfo.m_bDisableContextMenu = (lParam&0x1)!=0;;
	//pSubscribeFuncInfo.m_bDisableScrollBar = (lParam&0x2)!=0;
	//pSubscribeFuncInfo.m_bClosePrompt = (lParam&0x4)!=0;
	//pSubscribeFuncInfo.m_bWinResizable = true;
	//const bool nOpenNewClose = (lParam&0x8)!=0;
	//this->AddAppUrl(sUrl,pSubscribeFuncInfo,nOpenNewClose);
	//delete[] sUrl;
	return 0;
}
//LRESULT CPOPDlg::OnMsgShellExecuteOpen(WPARAM wParam, LPARAM lParam)
//{
//	COpenAppUrlInfo * pOpenAppUrlInfo = (COpenAppUrlInfo*)wParam;
//	if (pOpenAppUrlInfo==0) return 0;
//	CString strExe(pOpenAppUrlInfo->m_sUrl.c_str());
//	CString strParm(pOpenAppUrlInfo->m_sParam.c_str());
//	ShellExecute(NULL, "Open", strExe, strParm, NULL, SW_SHOW);
//	delete pOpenAppUrlInfo;
//	return 0;
//}

LRESULT CPOPDlg::OnMsgExitApp(WPARAM wp, LPARAM lp)
{
	OnExitApp();
	return 0;
}
LRESULT CPOPDlg::OnMsgLogout(WPARAM wp, LPARAM lp)
{
	OnLogout();
	return 0;
}
LRESULT CPOPDlg::OnMsgShowHideMainFrame(WPARAM wp, LPARAM lp)
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
		if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
			m_pPanelSearch->ShowWindow(SW_HIDE);
	}

	return 0;
}

void CPOPDlg::OnExitApp()
{
	theWantExitApp = true;
	PostMessage(WM_CLOSE, 0, 0);
	PostMessage(WM_QUIT, 0, 0);
}

void CPOPDlg::OnLogout()
{
	theWantExitApp = true;
	PostMessage(WM_CLOSE, 0, 0);
	// 重新启动新应用
	CString strExe;
	GetModuleFileName( NULL, strExe.GetBuffer(MAX_PATH), MAX_PATH);
	strExe.ReleaseBuffer(-1);
	ShellExecute(m_hWnd,_T("open"),strExe,_T("logout"),theApp.GetAppPath(),SW_SHOW);
}

void CPOPDlg::OnBnClickedButtonLinestate()
{
	const CString sAutoRunAccount = theApp.EBC_GetProfileString(_T("system"),_T("auto-run-account"));
	const mycp::bigint nMyCollectionSubId = theApp.GetMyCollectionSugId();
	 int const_my_ecards_index = nMyCollectionSubId==0?4:5;					// ***修改菜单项，需要修改这个地方
	 int const_app_func_index = const_my_ecards_index+1;						// ***修改菜单项，需要修改这个地方
	if (m_menuState.GetSafeHmenu()==NULL)
	{
		m_menuState.CreatePopupMenu();
		if (!theApp.IsLogonVisitor())
		{
			m_menuState.SetBitmapBackground(RGB(192,192,192));
			m_menuState.InsertODMenu(0,_T("在线"),MF_BYPOSITION,EB_COMMAND_STATE_ONLINE,IDB_BITMAP_MENU_ONLINE);
			m_menuState.InsertODMenu(1,_T("离开"),MF_BYPOSITION,EB_COMMAND_STATE_AWAY,IDB_BITMAP_MENU_AWAY);
			m_menuState.InsertODMenu(2,_T("忙碌"),MF_BYPOSITION,EB_COMMAND_STATE_BUSY,IDB_BITMAP_MENU_BUSY);
			m_menuState.AppendMenu(MF_SEPARATOR);
			if (nMyCollectionSubId>0)
			{
				m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MY_COLLECTION,_T("个人收藏"));
				m_nMainFrameMyCollectionMenuIndex = (int)m_menuState.GetMenuItemCount();
			}
			if (!sAutoRunAccount.IsEmpty())
				m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_AUTO_LOGIN,_T("自动登录"));
			m_menuState.AppendMenu(MF_SEPARATOR);
		}

#ifdef USES_SUPPORT_UI_STYLE
		CreateUIStyleMenu();
		m_menuState.AppendMenu(MF_POPUP,(UINT)m_menuUIStyleType.m_hMenu,_T("界面风格"));
		m_menuState.AppendMenu(MF_SEPARATOR);
		if (theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT)
		{
			m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_OPEN_WORKFRAME,_T("打开工作台"));
			m_nMenuOpenWorkFrameIndex = (int)m_menuState.GetMenuItemCount();
			m_menuState.AppendMenu(MF_SEPARATOR);
		}else
		{
			m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SHOWHEAD_ONLY,_T("仅显示图标"));
			m_nMenuShowHeadOnlyIndex = (int)m_menuState.GetMenuItemCount();
			m_menuState.AppendMenu(MF_SEPARATOR);
		}
#endif // USES_SUPPORT_UI_STYLE
		m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_LOGOUT,_T("注销"));
		m_menuState.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EXIT_APP,_T("退出"));
	}else
	{
		if (m_nMainFrameMyCollectionMenuIndex==-1 && nMyCollectionSubId>0)
		{
			m_nMainFrameMyCollectionMenuIndex = const_my_ecards_index-1;
			m_menuState.InsertMenu(m_nMainFrameMyCollectionMenuIndex,MF_BYPOSITION,EB_COMMAND_MY_COLLECTION,_T("个人收藏"));
		}

#ifdef USES_SUPPORT_UI_STYLE
		if (theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT)
		{
			if (m_nMenuShowHeadOnlyIndex>0)
			{
				m_menuState.RemoveMenu(m_nMenuShowHeadOnlyIndex+1,MF_BYPOSITION);	// MF_SEPARATOR
				m_menuState.RemoveMenu(m_nMenuShowHeadOnlyIndex,MF_BYPOSITION);		// "仅显示头像" menu
				m_nMenuShowHeadOnlyIndex = -1;
			}
			if (m_nMenuOpenWorkFrameIndex<0)
			{
				m_nMenuOpenWorkFrameIndex = m_menuState.GetMenuPosition("界面风格");
				if (m_nMenuOpenWorkFrameIndex>0)
				{
					m_menuState.InsertODMenu(m_nMenuOpenWorkFrameIndex+2,"打开工作台",MF_BYPOSITION,EB_COMMAND_OPEN_WORKFRAME);
					m_menuState.InsertODMenu(m_nMenuOpenWorkFrameIndex+3,NULL,MF_BYPOSITION|MF_SEPARATOR);
					m_nMenuOpenWorkFrameIndex += 2;
				}
			}
		}else
		{
			if (m_nMenuOpenWorkFrameIndex>0)
			{
				m_menuState.RemoveMenu(m_nMenuOpenWorkFrameIndex+1,MF_BYPOSITION);	// MF_SEPARATOR
				m_menuState.RemoveMenu(m_nMenuOpenWorkFrameIndex,MF_BYPOSITION);		// "打开工作台" menu
				m_nMenuOpenWorkFrameIndex = -1;
			}
			if (m_nMenuShowHeadOnlyIndex<0)
			{
				m_nMenuShowHeadOnlyIndex = m_menuState.GetMenuPosition("界面风格");
				if (m_nMenuShowHeadOnlyIndex>0)
				{
					m_menuState.InsertODMenu(m_nMenuShowHeadOnlyIndex+2,"仅显示图标",MF_BYPOSITION,EB_COMMAND_SHOWHEAD_ONLY);
					m_menuState.InsertODMenu(m_nMenuShowHeadOnlyIndex+3,NULL,MF_BYPOSITION|MF_SEPARATOR);
					m_nMenuShowHeadOnlyIndex += 2;
				}
			}
		}
#endif

		if (m_bRemoveAppFuncIndex1)
			m_menuState.RemoveMenu(const_app_func_index,MF_BYPOSITION);
		m_bRemoveAppFuncIndex1 = false;
		if (!theApp.IsLogonVisitor())
			m_menuState.RemoveMenu(const_my_ecards_index,MF_BYPOSITION);
	}

#ifdef USES_SUPPORT_UI_STYLE
	if (theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_OFFICE)
		m_menuState.CheckMenuItem(EB_COMMAND_SHOWHEAD_ONLY,CFrameWndInfoProxy::GetShowedLeft()?MF_UNCHECKED:MF_CHECKED);
#endif

	CNewMenu pPopupMenuCars;
	if (!theApp.IsLogonVisitor())
	{
		pPopupMenuCars.CreatePopupMenu();
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
		for (size_t i=0;i<m_pMyEmployeeInfo.size();i++)
		{
			const EB_MemberInfo& pMemberInfo = m_pMyEmployeeInfo[i];
#ifdef USES_EBCOM_TEST
			const CEBString sGroupName = theEBClientCore->EB_GetGroupName(pMemberInfo.m_sGroupCode).GetBSTR();
			if (!sGroupName.empty())
#else
			CEBString sGroupName;
			if (theEBAppClient.EB_GetGroupName(pMemberInfo.m_sGroupCode,sGroupName))
#endif
			{
				CString stext;
				stext.Format(_T("%s-%s"),sGroupName.c_str(),pMemberInfo.m_sUserName.c_str());

				bool bAppendMenuError = true;
				if (::PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
				{
					CImageEx pImageEx;
					pImageEx.LoadFilename(pMemberInfo.m_sHeadResourceFile.c_str());
					const int nWidth = pImageEx.GetWidth();
					const int nHeight = pImageEx.GetHeight();
					if (nWidth>0 && nHeight>0)
					{
						HGDIOBJ hBitmap = NULL;
						hBitmap = pImageEx.StrctchCopy(0,0,const_default_menu_image_size,const_default_menu_image_size);
						if (hBitmap!=NULL)
						{
							CBitmap pBitmap;
							pBitmap.Attach(hBitmap);
							pPopupMenuCars.AppendODMenu(stext,MF_BYCOMMAND,EB_COMMAND_MY_EMPLOYEE+i,&pBitmap);
							bAppendMenuError = false;
						}
					}
				}
				if (bAppendMenuError)
				{
					pPopupMenuCars.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MY_EMPLOYEE+i,stext);
				}

				if (sDefaultMemberCode == pMemberInfo.m_sMemberCode)
				{
					pPopupMenuCars.CheckMenuItem(EB_COMMAND_MY_EMPLOYEE+i,MF_CHECKED);
				}
			}
		}
		m_menuState.InsertMenu(const_my_ecards_index,MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenuCars.m_hMenu,_T("我的名片"));
	}
#ifdef USES_SUPPORT_UI_STYLE
	m_menuState.CheckMenuItem(EB_COMMAND_UISTYLE_TYPE_OFFIACE,theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_OFFICE?MF_CHECKED:MF_UNCHECKED);
	m_menuState.CheckMenuItem(EB_COMMAND_UISTYLE_TYPE_CHAT,theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?MF_CHECKED:MF_UNCHECKED);
#endif

	// 集成应用菜单
	CNewMenu pPopupMenuApps;
	pPopupMenuApps.CreatePopupMenu();
	{
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
				bool bAppendMenuError = true;
				if (::PathFileExists(pSubscribeFuncInfo.m_sResFile.c_str()))
				{
					CImageEx pImageEx;
					pImageEx.LoadFilename(pSubscribeFuncInfo.m_sResFile.c_str());
					const int nWidth = pImageEx.GetWidth();
					const int nHeight = pImageEx.GetHeight();
					if (nWidth>0 && nHeight>0)
					{
						HGDIOBJ hBitmap = NULL;
						if ((nWidth/nHeight)>2)
						{
							if (nHeight>const_default_menu_image_size)
								hBitmap = pImageEx.Copy(0,0,const_default_menu_image_size,const_default_menu_image_size);
							else
								hBitmap = pImageEx.Copy(0,0,nHeight,nHeight);
						}else
						{
							if (nHeight!=const_default_menu_image_size)
								hBitmap = pImageEx.StrctchCopy(0,0,const_default_menu_image_size,const_default_menu_image_size);
							else
								hBitmap = pImageEx.Copy(0,0,const_default_menu_image_size,const_default_menu_image_size);
						}
						if (hBitmap!=NULL)
						{
							CBitmap pBitmap;
							pBitmap.Attach(hBitmap);
							pPopupMenuApps.AppendODMenu(pSubscribeFuncInfo.m_sFunctionName.c_str(),MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,&pBitmap);
							bAppendMenuError = false;
						}
					}
				}
				if (bAppendMenuError)
				{
					pPopupMenuApps.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,pSubscribeFuncInfo.m_sFunctionName.c_str());
				}
			}
			m_bRemoveAppFuncIndex1 = true;
			m_menuState.InsertODMenu(const_app_func_index,_T("集成应用"),MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenuApps.m_hMenu,IDB_BITMAP_SELECT_COLOR);
			//m_menuState.InsertMenu(const_app_func_index,MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenuApps.m_hMenu,_T("集成应用"));
		}
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
}
void CPOPDlg::OnMyEmployeeInfo(UINT nID)
{
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
}
void CPOPDlg::ClearUnreadMsgBySubId(mycp::bigint nSubscribeId)
{
	if (nSubscribeId==0) return;
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		if (pFuncButtonInfo->GetFuncInfo().m_nSubscribeId==nSubscribeId)
		{
			if (pFuncButtonInfo->ClearUnreadMsg()>0)
			{
#ifdef USES_FRAMELIST_APPFRAME
				CDlgAppFrame * pDlgAppFrame = NewAppFrame(false);
#else
				//NewAppFrame();
				CDlgAppFrame * pDlgAppFrame = m_pDlgAppFrame;
#endif
				if (pDlgAppFrame!=NULL)
				{
					pDlgAppFrame->ClearUnreadMsg(nSubscribeId);
				}
			}
			break;
		}
	}
}
void CPOPDlg::OnSubscribeFunc(UINT nID)
{
	const size_t nIndex = nID-EB_COMMAND_SUBSCRIBE_FUNC;
	if (nIndex>=0 && nIndex<m_pSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = m_pSubscribeFuncList[nIndex];
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
		ClearUnreadMsgBySubId(pSubscribeFuncInfo.m_nSubscribeId);
	}
}
void CPOPDlg::OnMainFrameFunc(UINT nID)
{
	const size_t nIndex = nID-EB_COMMAND_MAINFRAME_FUNC;
	if (nIndex>=0 && nIndex<m_pMainFuncButtonList.size())
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[nIndex];
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = pFuncButtonInfo->GetFuncInfo();
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
		if (pFuncButtonInfo->ClearUnreadMsg()>0)
		{
#ifdef USES_FRAMELIST_APPFRAME
			CDlgAppFrame * pDlgAppFrame = NewAppFrame(false);
#else
			//NewAppFrame();
			CDlgAppFrame * pDlgAppFrame = m_pDlgAppFrame;
#endif
			if (pDlgAppFrame!=NULL)
			{
				pDlgAppFrame->ClearUnreadMsg(pSubscribeFuncInfo.m_nSubscribeId);
			}
		}
	}
}
#ifdef USES_FRAMELIST_APPFRAME
CDlgAppFrame * CPOPDlg::NewAppFrame(bool bCreate)
#else
void CPOPDlg::NewAppFrame(void)
#endif
{
#ifdef USES_FRAMELIST_APPFRAME
	CDlgAppFrame * pResult = NULL;
#else
	if (m_pDlgAppFrame==NULL)
#endif
	{
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
#ifdef USES_FRAMELIST_APPFRAME
			if (!bCreate) return m_pDlgFrameList==NULL?NULL:m_pDlgFrameList->GetDlgAppFrame();
			CreateFrameList(false);
			//m_bTimerToSwitchOnUIStyleTypeOffice = true;
			pResult = m_pDlgFrameList->GetDlgAppFrame();
			//return m_pDlgFrameList->GetDlgAppFrame();
#else
			CreateFrameList(false);
			if (m_pDlgAppFrame==NULL)
			{
				m_pDlgAppFrame = new CDlgAppFrame(this->m_pDlgFrameList);
				m_pDlgAppFrame->Create(CDlgAppFrame::IDD,this->m_pDlgFrameList);
				m_pDlgAppFrame->SetCircle(true);
			}
			//m_bTimerToSwitchOnUIStyleTypeOffice = true;
#endif
		}else
#endif
		{
			if (m_pDlgAppFrame==NULL)
			{
				m_pDlgAppFrame = new CDlgAppFrame(this);
				m_pDlgAppFrame->Create(CDlgAppFrame::IDD,this);
			}
#ifdef USES_FRAMELIST_APPFRAME
			pResult = m_pDlgAppFrame;
#endif
		}
#ifndef USES_FRAMELIST_APPFRAME
		CRect searchRect;
		if (theApp.IsLogonVisitor())
			m_btnMySession.GetWindowRect(&searchRect);
		else
			m_btnMyDepartment.GetWindowRect(&searchRect);
		CRect rect;
		m_pDlgMyContacts->GetWindowRect(&rect);
		searchRect.left = rect.left;
		searchRect.right = rect.right;
		searchRect.bottom = rect.bottom;
		this->ScreenToClient(&searchRect);
		pResult->MoveWindow(&searchRect);
#endif
	}
#ifdef USES_FRAMELIST_APPFRAME
	if (pResult->IsSubscribeFuncInfoEmpty() && !m_pMainSubscribeFuncList.empty())
	{
		// 获取应用工作区导航按钮
		BoostReadLock rdlock(m_pMainSubscribeFuncList.mutex());
		CLockList<EB_SubscribeFuncInfo>::iterator pIter = m_pMainSubscribeFuncList.begin();
		for (; pIter!=m_pMainSubscribeFuncList.end(); pIter++)
		{
			const EB_SubscribeFuncInfo & pSubscribeFuncInfo = *pIter;
			pResult->AddSubscribeFuncInfo(pSubscribeFuncInfo);
		}

		//for (size_t i=0;i<m_pMainSubscribeFuncList.size();i++)
		//{
		//	const EB_SubscribeFuncInfo & pSubscribeFuncInfo = m_pMainSubscribeFuncList[i];
		//	pResult->AddSubscribeFuncInfo(pSubscribeFuncInfo);
		//}
	}
	return pResult;
#endif
}
void CPOPDlg::AddAppUrl(bool bSaveBrowseTitle, const std::string& sAppUrl, const std::string& sPostData, const EB_SubscribeFuncInfo& pSubscribeFuncInfo, bool nOpenNewClose, HWND hwndFrom)
{
	if (m_pDlgFrameList != NULL && m_pDlgFrameList->IsIconic()) {
		/// 解决win10环境下，最小化窗口后，点击应用，导致程序卡死问题；
		m_pDlgFrameList->ShowWindow(SW_RESTORE);
	}

	// 应用面板
	if (theApp.GetHideMainFrame() && theApp.GetAutoOpenSubId()>0)
	{
		theApp.ShowHideAutoOpenSubscribeFunc(true);
	}else if (!this->IsWindowVisible() || this->IsIconic())
	{
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
			// * 跑下面，不主动显示
		}else
#endif
		{
			OnOpenMain();
		}
	}

#ifdef USES_FRAMELIST_APPFRAME
	CDlgAppFrame * pDlgAppFrame = NewAppFrame();
#else
	NewAppFrame();
	CDlgAppFrame * pDlgAppFrame = m_pDlgAppFrame;
#endif
	const int nFromOffsetIndex = pDlgAppFrame->GetOffsetIndexByHwnd(hwndFrom);
	const int nInsertOffset = nFromOffsetIndex!=-1?(nFromOffsetIndex+1):-1;
	pDlgAppFrame->AddAppUrl(bSaveBrowseTitle, sAppUrl, sPostData, pSubscribeFuncInfo, nOpenNewClose, nInsertOffset);
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType!=EB_UI_STYLE_TYPE_CHAT)
#endif
		CFrameWndInfoProxy::ShowWnd((unsigned int)FRAME_BTN_ID_WORKFRAME);
	OnBnClickedButtonSwitchFrame();
}
void CPOPDlg::AddAppUrl(bool bSaveBrowseTitle, const std::string& sAppUrl, const std::string& sPostData, HWND hwndFrom)
{
	EB_SubscribeFuncInfo pSubscribeFuncInfo;
	pSubscribeFuncInfo.m_sFunctionName = sAppUrl;
	pSubscribeFuncInfo.m_bClosePrompt = false;
	pSubscribeFuncInfo.m_bDisableScrollBar = false;
	pSubscribeFuncInfo.m_bDisableContextMenu = false;
	pSubscribeFuncInfo.m_bWinResizable = true;
	AddAppUrl(bSaveBrowseTitle,sAppUrl,sPostData,pSubscribeFuncInfo,false,hwndFrom);
}


void CPOPDlg::ChangeTrayText(void)
{
	CString sTrayText;
#ifdef USES_EBCOM_TEST
	const CEBString sUserName = theEBClientCore->EB_UserName.GetBSTR();
	EB_USER_LINE_STATE nOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_LineState;
	CEBString sEnterpriseName = theEBClientCore->EB_GetEnterpriseName(0).GetBSTR();
#else
	tstring sUserName = theEBAppClient.EB_GetUserName();
	EB_USER_LINE_STATE nOutLineState = theEBAppClient.EB_GetLineState();
	tstring sEnterpriseName;
	theEBAppClient.EB_GetEnterpriseName(sEnterpriseName);
#endif
	if (!theApp.GetProductName().IsEmpty())
		sEnterpriseName = (LPCTSTR)theApp.GetProductName();
	if (theApp.IsLogonVisitor())
		sTrayText.Format(_T("游客-%s-%s"),theApp.GetLogonAccount(),GetLineStateText(nOutLineState));
	else if (!sEnterpriseName.empty())
		sTrayText.Format(_T("%s(%s)-%s\n%s"),sUserName.c_str(),theApp.GetLogonAccount(),GetLineStateText(nOutLineState),sEnterpriseName.c_str());
	else
		sTrayText.Format(_T("%s(%s)-%s"),sUserName.c_str(),theApp.GetLogonAccount(),GetLineStateText(nOutLineState));
	lstrcpy(m_trayIconData.szTip, sTrayText);
	Shell_NotifyIcon(NIM_MODIFY, &m_trayIconData);
}

void CPOPDlg::OnStateOnline()
{
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
}

void CPOPDlg::OnStateAway()
{
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
}

void CPOPDlg::OnStateBusy()
{
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
}
void CPOPDlg::OnAutoLogin()
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
			const tstring sModuleFileName(pFileName);
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

void CPOPDlg::OnMouseMove(UINT nFlags, CPoint point)
{

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

	// EntName LOGO
	const CString& sProductName = theApp.GetProductName();
	if (theApp.GetDefaultUrl().empty() || !theApp.GetLicenseUser() || sProductName.IsEmpty() || sProductName.Find(_T("恩布"))>=0)
	{
		const CRect rectEntName(8,8,76,28);
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		if (rectEntName.PtInRect(pos))
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
			return;
		}
	}

	//  恩布
	__super::OnMouseMove(nFlags, point);
}
void CPOPDlg::CancelSaveDescription(void)
{
#ifdef USES_EBCOM_TEST
	const CEBString sOutDesc = theEBClientCore->EB_Description.GetBSTR();
#else
	tstring sOutDesc = theEBAppClient.EB_GetDescription();
#endif
	m_editDescription.SetWindowText(sOutDesc.c_str());
	RefreshEditDescription();
}

void CPOPDlg::OnEnKillfocusEditDescription()
{
	CString sNewDesc;
	m_editDescription.GetWindowText(sNewDesc);
#ifdef USES_EBCOM_TEST
	const CEBString sOutDesc = theEBClientCore->EB_Description.GetBSTR();
#else
	tstring sOutDesc = theEBAppClient.EB_GetDescription();
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
}
void CPOPDlg::RefreshEditDescription(void)
{
	//m_editDescription.EnableWindow(FALSE);
	m_editDescription.SetReadOnly(TRUE);
	m_editDescription.ShowWindow(SW_HIDE);
	CRect rect;
	m_editDescription.GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	this->InvalidateRect(rect);
}

void CPOPDlg::OnOK()
{
	//__super::OnOK();
}

void CPOPDlg::OnCancel()
{
	if (!theWantExitApp)
	{
		//OnBnClickedButtonClose();	// 支持实现，鼠标移动工作栏，点击关闭，隐藏窗口
		return;
	}
	__super::OnCancel();
}


void CPOPDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// EntName LOGO
	const CString& sProductName = theApp.GetProductName();
	if (theApp.GetDefaultUrl().empty() || !theApp.GetLicenseUser() || sProductName.IsEmpty() || sProductName.Find(_T("恩布"))>=0)
	{
		const CRect rectEntName(8,8,76,28);
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		if (rectEntName.PtInRect(pos))
		{
			if (theApp.GetDefaultUrl().empty())
				ShellExecute(NULL,  "open", "http://www.entboost.com", NULL, NULL, SW_SHOW);
			else
				ShellExecute(NULL,  "open", theApp.GetDefaultUrl().c_str(), NULL, NULL, SW_SHOW);
			return;
		}
	}

	CPoint pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos);
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
//#ifndef USES_NEW_UI_160111
	}else if (m_editDescription.GetSafeHwnd())
	{
#ifdef USES_SUPPORT_UI_STYLE
		const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
		if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		{
#else
		if (false)
		{
#endif
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
//#endif
	}
	__super::OnLButtonDown(nFlags, point);
}

void CPOPDlg::OnMySetting()
{
	// TODO: Add your command handler code here
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
}
void CPOPDlg::OnMyShare()
{
	if (!theApp.GetDisableUserCloudDrive() && !theApp.IsLogonVisitor())
	{
		theApp.OpenMyShareWindow(this);
	}
}
void CPOPDlg::OnMyCollection()
{
	if (!theApp.IsLogonVisitor())
	{
		const mycp::bigint nMyCollectionSubId = theApp.GetMyCollectionSugId();
		EB_SubscribeFuncInfo pSubscribeFuncInfo;
		if (theEBAppClient.EB_GetSubscribeFuncInfo(nMyCollectionSubId,&pSubscribeFuncInfo))
			theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
	}
}

void CPOPDlg::OnSkinSelect(UINT nID)
{
	const size_t nIndex = nID-EB_COMMAND_SKIN_SETTING;
	if (nIndex == 0)
	{
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
	//if ((m_pDlgAppFrame!=NULL&&m_pDlgAppFrame->IsWindowVisible()))
	//{
	//	// 解决m_pDlgAppFrame 背景刷新问题（按钮和IE背景）
	//	//m_pDlgAppFrame->ShowWindow(SW_HIDE);
	//	//m_pDlgAppFrame->ShowWindow(SW_SHOW);
	//	m_pDlgAppFrame->RefreshAppWnd();
	//}
}
void CPOPDlg::OnSkinFlat(void)
{
	const bool bColorFlag = theApp.GetColorFlat()?false:true;
	theApp.SetColorFlat(bColorFlag);
	SetCtrlColor();
	if (theApp.m_pBoEB->use("eb"))
	{
		CString sSql;
		sSql.Format(_T("UPDATE sys_value_t SET value2=%d WHERE name='color-flat'"),(int)(bColorFlag?1:0));
		theApp.m_pBoEB->execsql(sSql);
		theApp.m_pBoEB->close();
	}
	this->Invalidate();
	//if ((m_pDlgAppFrame!=NULL&&m_pDlgAppFrame->IsWindowVisible()))
	//{
	//	m_pDlgAppFrame->RefreshAppWnd();
	//}
}
void CPOPDlg::OnSkinGradient(void)
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
	//if ((m_pDlgAppFrame!=NULL&&m_pDlgAppFrame->IsWindowVisible()))
	//{
	//	m_pDlgAppFrame->RefreshAppWnd();
	//}
}
void CPOPDlg::OnBrowserTypeCef(void)
{
	if (theApp.GetDefaultBrowserType()!=EB_BROWSER_TYPE_CEF)
	{
		theApp.SetDefaultBrowserType(EB_BROWSER_TYPE_CEF);
		if (theApp.m_pBoEB->use("eb"))
		{
			CString sSql;
			sSql.Format(_T("UPDATE sys_value_t SET value2=%d WHERE name='default-browser-type'"),(int)theApp.GetDefaultBrowserType());
			theApp.m_pBoEB->execsql(sSql);
			theApp.m_pBoEB->close();
		}
	}
}
void CPOPDlg::OnBrowserTypeIe(void)
{
	if (theApp.GetDefaultBrowserType()!=EB_BROWSER_TYPE_IE)
	{
		theApp.SetDefaultBrowserType(EB_BROWSER_TYPE_IE);
		if (theApp.m_pBoEB->use("eb"))
		{
			CString sSql;
			sSql.Format(_T("UPDATE sys_value_t SET value2=%d WHERE name='default-browser-type'"),(int)theApp.GetDefaultBrowserType());
			theApp.m_pBoEB->execsql(sSql);
			theApp.m_pBoEB->close();
		}
	}
}
void CPOPDlg::BuildHeadRect(void)
{
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
		m_rectHead = CRect(UISTYLE_CHAT_POS_ADIMG_LEFT, UISTYLE_CHAT_POS_ADIMG_TOP, UISTYLE_CHAT_POS_ADIMG_LEFT+UISTYLE_CHAT_POS_ADIMG_SIGE, UISTYLE_CHAT_POS_ADIMG_TOP+UISTYLE_CHAT_POS_ADIMG_SIGE);
	else
#endif
		m_rectHead = CRect(POS_ADIMG_LEFT, POS_ADIMG_TOP, POS_ADIMG_LEFT+POS_ADIMG_SIGE, POS_ADIMG_TOP+POS_ADIMG_SIGE);
}
void CPOPDlg::OnUIStyleTypeOffice(void)
{
	//if (m_bTimerToSwitchOnUIStyleTypeOffice)
	//{
	//	// ** 优化解决“经典聊天界面”，第一次切换“专业办公界面”时，工作台浏览器界面错乱问题；
	//	m_bTimerToSwitchOnUIStyleTypeOffice = false;
	//	OnBnClickedButtonSwitchFrame();
	//	SetTimer(TIMERID_ONUISTYLETYPEOFFICE,1000,NULL);
	//	return;
	//}
	if (theApp.SetDefaultUIStyleType(EB_UI_STYLE_TYPE_OFFICE))
	{
		BuildHeadRect();
		SetSplitterBorder(LIMIT_DLG_WIDTH);
		m_btnMyCenter.Load(IDB_PNG_BTN_MY_CENTER,24);
		m_btnMyCenter.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
		m_btnFileMgr.Load(IDB_PNG_BTN_FILE_MGR,24);
		m_btnFileMgr.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
		m_btnMyShare.Load(IDB_PNG_BTN_MY_SHARE,24);
		m_btnMyShare.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
		m_btnMainFunc.Load(IDB_PNG_BTN_MAIN_FUNC,24);
		m_btnMainFunc.SetDrawLine(5,1,0,-1,theDefaultButtonColor);

		//if (m_btnSwitchLeft.GetSafeHwnd()!=NULL)
		//{
		//	m_btnSwitchLeft.ShowWindow(SW_SHOW);
		//}

		const CFrameWndInfo::pointer pFrameListCurrentWndInfo = m_pDlgFrameList==NULL?NullFrameWndInfo:m_pDlgFrameList->GetCurrentFrameWndInfo();
		const bool bShowWorkFrame = pFrameListCurrentWndInfo.get()!=NULL && pFrameListCurrentWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME;
#ifdef USES_FRAMELIST_APPFRAME
		CDlgAppFrame* pDlgAppFrame = m_pDlgFrameList==NULL?NULL:m_pDlgFrameList->GetDlgAppFrame();
		if (pDlgAppFrame!=NULL && !pDlgAppFrame->IsEmpty())
		{
			this->NewAppFrame();
			m_pDlgAppFrame->AddFromAppFrame(pDlgAppFrame);
		}
#else
		if (m_pDlgAppFrame!=NULL)
		{
			m_pDlgAppFrame->SetParent(this);
			m_pDlgAppFrame->SetOwner(this);
			m_pDlgAppFrame->SetCircle(false);
			m_pDlgAppFrame->ShowWindow(SW_HIDE);
		}
#endif

		CFrameWndInfoProxy::AddWnd(CFrameWndInfo::FRAME_WND_WORK_FRAME, _T("工作台"), _T("工作台 Ctrl+1"), bShowWorkFrame, false);
		CFrameWndInfoProxy::AddWnd(CFrameWndInfo::FRAME_WND_MAIN_FRAME, _T("联系人"), _T("联系人 Ctrl+2"), !bShowWorkFrame, false);	// 即时通讯
		if (m_pDlgAppFrame!=NULL && m_pDlgAppFrame->GetSize()>0)
		{
			this->PostMessage(EB_COMMAND_RAME_WND_COUNT,(WPARAM)m_pDlgAppFrame->GetSafeHwnd(),(LPARAM)m_pDlgAppFrame->GetSize());
		}

		CRect rectFrameList(0,0,0,0);
		if (m_pDlgFrameList!=NULL)
		{
			m_pDlgFrameList->GetWindowRect(&rectFrameList);
			{
				const CLockList<CFrameWndInfo::pointer>& pList = m_pDlgFrameList->GetList();
				AUTO_CONST_RLOCK(pList);
				CLockList<CFrameWndInfo::pointer>::const_iterator pIter = pList.begin();
				for (; pIter!=pList.end(); pIter++)
				{
					const CFrameWndInfo::pointer& pFrameWndInfo = *pIter;
					m_pDlgFrameList->ResetUserData(pFrameWndInfo);
					if (pFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_CALL_DIALOG && pFrameWndInfo->GetDialog().get()!=NULL)
					{
						pFrameWndInfo->GetDialog()->SetParent(this);
						pFrameWndInfo->GetDialog()->SetOwner(this);
						pFrameWndInfo->GetDialog()->RefreshBtnShow();
						const bool bShow = (!bShowWorkFrame&&pFrameWndInfo->IsChecked())?true:false;
						this->AddWnd(pFrameWndInfo, bShow, false, true);
					}
				}
			}
			m_pDlgFrameList->ClearWnd();
			m_pDlgFrameList->ShowFrameList(false);
			//m_pDlgFrameList->ShowWindow(SW_HIDE);
		}
		CRect rect;
		this->GetWindowRect(&rect);
		const int const_limit_width = max(rectFrameList.Width(),LEN_DLG_WIDTH);
		const int const_limit_height = max(rectFrameList.Height(),LEN_DLG_HEIGHT);
		rect.left = max(10,rect.right - const_limit_width);
		rect.right = rect.left + const_limit_width;
		rect.bottom = rect.top + const_limit_height;
		this->MoveWindow(&rect);
	}
}
void CPOPDlg::OnUIStyleTypeChat(void)
{
	if (theApp.SetDefaultUIStyleType(EB_UI_STYLE_TYPE_CHAT))
	{
		CRect rectWindow;
		this->GetWindowRect(&rectWindow);

		BuildHeadRect();
		SetSplitterBorder(UISTYLE_CHAT_DEFAULT_DLG_WIDTH);
		m_btnMyCenter.Load(IDB_PNG_BTN_MY_CENTER_UISTYLE1,24);
		m_btnMyCenter.SetDrawLine(5,1,0,-1,theDefaultBtnWhiteColor);
		m_btnFileMgr.Load(IDB_PNG_BTN_FILE_MGR_UISTYLE1,24);
		m_btnFileMgr.SetDrawLine(5,1,0,-1,theDefaultBtnWhiteColor);
		m_btnMyShare.Load(IDB_PNG_BTN_MY_SHARE_UISTYLE1,24);
		m_btnMyShare.SetDrawLine(5,1,0,-1,theDefaultBtnWhiteColor);
		m_btnMainFunc.Load(IDB_PNG_BTN_MAIN_FUNC_UISTYLE1,24);
		m_btnMainFunc.SetDrawLine(5,1,0,-1,theDefaultBtnWhiteColor);

		if (m_btnSwitchLeft.GetSafeHwnd()!=NULL && m_btnSwitchLeft.IsWindowVisible())
			m_btnSwitchLeft.ShowWindow(SW_HIDE);

		CreateFrameList(false);
		// *添加工作台
#ifdef USES_FRAMELIST_APPFRAME
		if (m_pDlgAppFrame!=NULL && !m_pDlgAppFrame->IsEmpty())
		{
			CDlgAppFrame* pDlgAppFrame = m_pDlgFrameList->GetDlgAppFrame();
			pDlgAppFrame->AddFromAppFrame(m_pDlgAppFrame);
			const bool bShowWorkFrame = m_pDlgFrameList->GetSize()==0?true:false;
			m_pDlgFrameList->AddWnd(CFrameWndInfo::FRAME_WND_WORK_FRAME, _T("工作台"), _T("工作台 Ctrl+1"), bShowWorkFrame, false);
			m_pDlgFrameList->PostMessage(EB_COMMAND_RAME_WND_COUNT,(WPARAM)pDlgAppFrame->GetSafeHwnd(),(LPARAM)pDlgAppFrame->GetSize());
			m_pDlgFrameList->ShowFrameList();
		}
#else
		if (m_pDlgAppFrame!=NULL)
		{
			m_pDlgAppFrame->SetParent(m_pDlgFrameList);
			m_pDlgAppFrame->SetOwner(m_pDlgFrameList);
			m_pDlgAppFrame->SetCircle(true);
			if (m_pDlgAppFrame->GetSize()>0)
			{
				const bool bShowWorkFrame = m_pDlgFrameList->GetSize()==0?true:false;
				m_pDlgFrameList->AddWnd(CFrameWndInfo::FRAME_WND_WORK_FRAME, _T("工作台"), _T("工作台 Ctrl+1"), bShowWorkFrame, false);
				m_pDlgFrameList->PostMessage(EB_COMMAND_RAME_WND_COUNT,(WPARAM)m_pDlgAppFrame->GetSafeHwnd(),(LPARAM)m_pDlgAppFrame->GetSize());
				m_pDlgFrameList->ShowFrameList();
			}
		}
#endif
		//// *先切换正常显示主界面（必须设置，否则）
		OnMessageReturnMainFrame(1,0);
		
		// * MoveSize 会转换窗口数据
		if (rectWindow.Height()>UISTYLE_CHAT_DEFAULT_DLG_HEIGHT)
		{
			rectWindow.bottom = rectWindow.top + UISTYLE_CHAT_DEFAULT_DLG_HEIGHT;
		}
		if (rectWindow.Width()!=UISTYLE_CHAT_DEFAULT_DLG_WIDTH)
		{
			rectWindow.left = rectWindow.right-UISTYLE_CHAT_DEFAULT_DLG_WIDTH;
			this->MoveWindow(&rectWindow);
		}else
		{
			this->MoveSize(rectWindow.Width(),rectWindow.Height());
			this->Invalidate();
		}
		CFrameWndInfoProxy::ShowWnd((unsigned int)FRAME_BTN_ID_MAINFRAME);
	}
}
void CPOPDlg::OnSettingCheckNewVersion(void)
{
	const time_t tNow = time(0);
	if (theCheckNewVersionTime>0 && (theCheckNewVersionTime+60*3)>tNow)	// 3分钟内，检查未返回，只能检查一次；
	{
		CString sText;
		sText.Format(_T("检测到新版本：\r\n正在下载增量更新包..."));
		CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_INFORMATION,10);
		return;
	}
	theCheckNewVersionTime = tNow;
	theUpdateResetTimer = false;
	KillTimer(TIMERID_CHECK_UPDATE);
	SetTimer(TIMERID_CHECK_UPDATE,100,NULL);
}

void CPOPDlg::CreateUIStyleMenu(void)
{
#ifdef USES_SUPPORT_UI_STYLE
	if (m_menuUIStyleType.GetSafeHmenu()==NULL)
	{
		m_menuUIStyleType.CreatePopupMenu();
		m_menuUIStyleType.InsertODMenu(0,_T("经典聊天风格(&C)"),MF_BYPOSITION,EB_COMMAND_UISTYLE_TYPE_CHAT);
		m_menuUIStyleType.InsertODMenu(1,_T("专业办公风格(&O)"),MF_BYPOSITION,EB_COMMAND_UISTYLE_TYPE_OFFIACE);
	}
#endif
}
void CPOPDlg::OnBnClickedButtonSkin2()
{
	if (m_menuSkin.GetSafeHmenu()==NULL)
	{
		m_menuSkin.CreatePopupMenu();
		m_menuSkin.SetBitmapBackground(RGB(192,192,192));
		m_menuSkin.InsertODMenu(0,_T("选择色调..."),MF_BYPOSITION,EB_COMMAND_SKIN_SETTING,IDB_BITMAP_SELECT_COLOR);
		m_menuSkin.AppendMenu(MF_SEPARATOR);
		for (int i=0; i<theColorSkinSize; i++)
		{
			m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SKIN_1+i,theColorSkinsString[i].c_str());
			m_menuSkin.ModifyODMenu(theColorSkinsString[i].c_str(),EB_COMMAND_SKIN_1+i,theColorSkinsValue[i],theColorSkinsValue[i]);
		}
#ifdef USES_SUPPORT_UI_STYLE
		m_menuSkin.AppendMenu(MF_SEPARATOR);
		CreateUIStyleMenu();
		m_menuSkin.AppendMenu(MF_POPUP,(UINT)m_menuUIStyleType.m_hMenu,_T("界面风格"));
#endif // USES_SUPPORT_UI_STYLE
		m_menuSetBrowserType.CreatePopupMenu();
		m_menuSetBrowserType.InsertODMenu(0,_T("默认使用谷歌&Chrome浏览器"),MF_BYPOSITION,EB_COMMAND_BROWSER_TYPE_CEF,IDB_BITMAP_BROWSER_CHROME);
		if (!theApp.GetIeException())
			m_menuSetBrowserType.InsertODMenu(1,_T("默认使用微软I&E浏览器"),MF_BYPOSITION,EB_COMMAND_BROWSER_TYPE_IE,IDB_BITMAP_BROWSER_IE);
		//m_menuSetBrowserType.AppendMenu(MF_BYCOMMAND,EB_COMMAND_BROWSER_TYPE_CEF,_T("默认使用谷歌&Chrome浏览器"));
		//if (!theApp.GetIeException())
		//	m_menuSetBrowserType.AppendMenu(MF_BYCOMMAND,EB_COMMAND_BROWSER_TYPE_IE,_T("默认使用微软I&E浏览器"));
		m_menuSkin.AppendMenu(MF_SEPARATOR);
		m_menuSkin.AppendMenu(MF_POPUP,(UINT)m_menuSetBrowserType.m_hMenu,_T("内置浏览器"));
		//m_menuSkin.AppendMenu(MF_SEPARATOR);
		//m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SKIN_FLAT,_T("扁平效果"));
		//m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SKIN_GRADIENT,_T("渐变效果"));
		m_menuSkin.AppendMenu(MF_SEPARATOR);
		m_menuSkin.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CHECK_NEW_VERSION,_T("检查版本更新(&U)"));
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
	//m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_FLAT,theApp.GetColorFlat()?MF_CHECKED:MF_UNCHECKED);
	//m_menuSkin.EnableMenuItem(EB_COMMAND_SKIN_GRADIENT,theApp.GetColorFlat()?MF_DISABLED:MF_ENABLED);
	//m_menuSkin.CheckMenuItem(EB_COMMAND_SKIN_GRADIENT,theApp.GetColorGradient()?MF_CHECKED:MF_UNCHECKED);
	m_menuSkin.CheckMenuItem(EB_COMMAND_BROWSER_TYPE_CEF,theApp.GetDefaultBrowserType()==EB_BROWSER_TYPE_CEF?MF_CHECKED:MF_UNCHECKED);
	m_menuSkin.CheckMenuItem(EB_COMMAND_BROWSER_TYPE_IE,theApp.GetDefaultBrowserType()==EB_BROWSER_TYPE_IE?MF_CHECKED:MF_UNCHECKED);
#ifdef USES_SUPPORT_UI_STYLE
	m_menuSkin.CheckMenuItem(EB_COMMAND_UISTYLE_TYPE_OFFIACE,theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_OFFICE?MF_CHECKED:MF_UNCHECKED);
	m_menuSkin.CheckMenuItem(EB_COMMAND_UISTYLE_TYPE_CHAT,theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?MF_CHECKED:MF_UNCHECKED);
#endif

	CPoint point;
	GetCursorPos(&point);
	m_menuSkin.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTALIGN,point.x,point.y,this);
}

void CPOPDlg::OnOpenMain()
{
	//if (m_bShrinkageWin)
	//{
	//	KillTimer(TIMERID_SHRINKAGE_WIN);
	//	m_bShrinkageWin = false;
	//	m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
	//}
	if (m_bShrinkageWin && m_pDlgShrinkageBar->IsWindowVisible())
	{
		SetTimer(TIMERID_CHECK_SHRINKAGEBAR,200,NULL);
	}

	if (theApp.GetHideMainFrame() && theApp.GetAutoOpenSubId()>0)
	{
		theApp.ShowHideAutoOpenSubscribeFunc(true);
	}else if (this->IsIconic())
	{
		this->ShowWindow(SW_RESTORE);
		this->SetForegroundWindow();
	}else
	{
		this->ShowWindow(SW_SHOWNORMAL);
		this->SetForegroundWindow();
		if (this->GetWorkFrameShowed())
		{
#ifdef USES_SUPPORT_UI_STYLE
			const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
			if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
			{
				// *
			}else
#endif
			{
				if (m_pPanelSearch!=0 && m_pPanelSearch->GetSafeHwnd()!=0)
					m_pPanelSearch->ShowWindow(SW_SHOW);
			}
			if (m_pDlgAppFrame!=NULL && m_pDlgAppFrame->GetSafeHwnd()!=NULL)
				m_pDlgAppFrame->RefreshAppWnd();	// 解决Chrome还原窗口，背景显示灰色问题
		}
	}
}

void CPOPDlg::OnShowHeadOnly()
{
	OnBnClickedButtonSwitchLeft();
}
void CPOPDlg::OnWorkFrame()
{
	// for 工作台
	OnBnClickedButtonSwitchFrame();
}
LRESULT CPOPDlg::OnMsgWorkFrame(WPARAM wParam, LPARAM lParam)
{
	// for FrameList 工作台
	OnBnClickedButtonSwitchFrame();
	return 0;
}
void CPOPDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	OnBnClickedButtonLinestate();
	__super::OnRButtonUp(nFlags, point);
}

void CPOPDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnBnClickedButtonMax();
	__super::OnLButtonDblClk(nFlags, point);
}

void CPOPDlg::CreateFrameList(bool bShow)
{
#ifdef USES_NEW_UI_1220
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		// **会跑下面
	}else if (!theApp.GetHideMainFrame())
#else
	if (!theApp.GetHideMainFrame())
#endif
	{
		if (bShow)
		{
			if (!this->IsWindowVisible())
			{
				this->ShowWindow(SW_SHOW);
				if (m_bShrinkageWin && m_pDlgShrinkageBar->IsWindowVisible())
				{
					SetTimer(TIMERID_CHECK_SHRINKAGEBAR,200,NULL);
				}
			}
		}
		return;
	}
#endif
	if (m_pDlgFrameList==NULL)
	{
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		m_pDlgFrameList = new CDlgFrameList(pParent);
//#ifndef USES_FRAMELIST_APPFRAME
//#ifdef USES_SUPPORT_UI_STYLE
		m_pDlgFrameList->SetWorkFrameCallback(this);
//#endif
//#endif
		m_pDlgFrameList->Create(CDlgFrameList::IDD, pParent);
	}
	if (bShow)
	{
		m_pDlgFrameList->ShowFrameList();
	}
}

void CPOPDlg::OnBnClickedButtonMyCenter()
{
	OnMySetting();
}
void CPOPDlg::OnBnClickedButtonFileMgr()
{
	OnFileManager();
}

void CPOPDlg::OnBnClickedButtonMyShare()
{
	OnMyShare();
}

void CPOPDlg::OnBnClickedButtonMainFunc()
{
	if (m_menuFunc.GetSafeHmenu()==NULL)
	{
		m_menuFunc.CreatePopupMenu();
	}else
	{
		while (m_menuFunc.GetMenuItemCount()>0)
			m_menuFunc.RemoveMenu(0,MF_BYPOSITION);
	}

	// 集成应用菜单
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
}

HBRUSH CPOPDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
	HBRUSH hBrush = CEbDialogBase::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->GetDlgCtrlID()==IDC_EDIT_SEARCH)
	{
		pDC->SetBkMode(TRANSPARENT);
#ifdef USES_NEW_UI_160111
		hBrush = CreateSolidBrush(theApp.GetHotColor());   //   背景色
#else
		hBrush = CreateSolidBrush(theDefaultFlatBgColor);   //   背景色
#endif
	}
	return hBrush;
}

//FILE * file = NULL;
LRESULT CPOPDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	}else if (message == WM_MOVING || message == WM_MOVE)
	{
		if (m_pDlgAppFrame!=NULL && m_pDlgAppFrame->GetSafeHwnd()!=NULL)
			m_pDlgAppFrame->NotifyMoveOrResizeStarted();
	}else if ( message == WM_ENDSESSION )
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

void CPOPDlg::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if (m_bOpenOk && !this->IsIconic() && (x>=0 && y>=0))
	//if (!this->IsIconic() && (x>0 || y>0))
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
	if (m_pPanelSearch!=0 && m_editSearch.GetSafeHwnd()!=0)
	{
		const int nlen = (18+2)*4;
#ifdef USES_NEW_UI_160111
		// *** 改成child 不需要 onmove
		//CRect rect;
		//rect.left = const_border_left+2;//cx - nlen-4;
		//rect.right = rect.left + nlen;
		//rect.top = NEW_UI_160111_HEIGHT + 5;
		//rect.bottom = rect.top + 18;
#else
		CRect rect;
		m_editSearch.GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		rect.left = rect.right - nlen-4;
		rect.right = rect.left + nlen;
		rect.top = rect.top + 2;
		rect.bottom = rect.top + 18;
		this->ClientToScreen(&rect);
		m_pPanelSearch->MoveWindow(&rect);
#endif
	}
	if (m_pPanelSearch!=NULL && m_pPanelSearch->GetSafeHwnd()!=NULL)
	{
		m_pPanelSearch->OnMove();
	}

	if (!theApp.GetHideMainFrame())
	{
		CPoint pCursorPos;
		GetCursorPos(&pCursorPos);

		int nNeedHideWin = 0;
		if ((pCursorPos.y<=0 && y<=0) || (!m_bOpenOk && x>0 && y<=0))
			nNeedHideWin = 2;		// top
		/*
		if ((pCursorPos.x<=0 && y>0) || (!m_bOpenOk && x<=0 && y>0))
			nNeedHideWin = 1;		// left
		else if ((pCursorPos.y<=0 && y<=0) || (!m_bOpenOk && x>0 && y<=0))
			nNeedHideWin = 2;		// top
		else
		{
			CRect rect;
			this->GetWindowRect(&rect);
			const int nScreenWidth = theApp.GetScreenWidth();
			if (((pCursorPos.x+1)>=nScreenWidth && y>0) || (!m_bOpenOk && rect.right>=nScreenWidth && y>0))
				nNeedHideWin = 3;	// right

			//CRect rect;
			//this->GetWindowRect(&rect);
			//int nScreenWidth = 0;
			//int nScreenHeight = 0;
			//theApp.GetScreenSize(nScreenWidth, nScreenHeight);
			//if (rect.right>=nScreenWidth && y>0)
			//	nNeedHideWin = 3;	// right
		}
		*/
		if (nNeedHideWin>0)
		{
			CRect rect;
			this->GetWindowRect(&rect);
			const int nWinWidth = rect.Width();
			const int nWinHeight = rect.Height();

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
				const int nScreenWidth = theApp.GetScreenWidth();
				bNeedReMoveWindow = (m_rectWin.right!=nScreenWidth)?true:false;
				m_rectWin.right = nScreenWidth;
				m_rectWin.left = m_rectWin.right-nWinWidth;
				rect.right = nScreenWidth;
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
	}

	//if (m_bOpenOk)
	//{
	//	m_nShrinkageWin = m_bShrinkageWin?1:0;
	//	char lpszBuffer[12];
	//	sprintf(lpszBuffer,"%d",m_nShrinkageWin);
	//	WritePrivateProfileString(_T("default"),_T("shrinkage-win"),lpszBuffer,theApp.GetUserSettingIniFile());
	//}
	//if (m_pDlgAppFrame!=NULL)
	//	m_pDlgAppFrame->OnMove();
	CFrameWndInfoProxy::OnMove();
	if (y<=0 && m_editSearch.GetSafeHwnd()!=NULL)
	{
		m_editSearch.Invalidate();
	}
	if (m_pDlgAppFrame!=NULL && m_pDlgAppFrame->IsWindowVisible())
	{
		m_pDlgAppFrame->OnMove();
	}

}

//void CPOPDlg::OnMoving(UINT fwSide, LPRECT pRect)
//{
//	//if (pRect!=NULL)
//	//{
//	//	const int nScreenHeight = theApp.GetScreenHeight();
//	//	CPoint pCursorPos;
//	//	GetCursorPos(&pCursorPos);
//	//	if (pCursorPos.y>= nScreenHeight)
//	//	{
//	//		// 限制下移
//	//		pRect->top -= (pCursorPos.y-nScreenHeight);
//	//		pRect->bottom -= (pCursorPos.y-nScreenHeight);
//	//		SetCursorPos(pCursorPos.x,nScreenHeight);
//	//	}
//
//	//	//if (pRect->left<0 && pRect->top>0)
//	//	//{
//	//	//	// 限制左移
//	//	//	pRect->right -= pRect->left;
//	//	//	pRect->left = 0;
//	//	//}else 
//	//	//{
//	//	//	int nScreenWidth = 0;
//	//	//	int nScreenHeight = 0;
//	//	//	theApp.GetScreenSize(nScreenWidth, nScreenHeight);
//	//	//	if (pRect->left>0 && pRect->right>nScreenWidth)
//	//	//	{
//	//	//		// 限制右移
//	//	//		pRect->left -= (pRect->right-nScreenWidth);
//	//	//		pRect->right = nScreenWidth;
//	//	//	}
//	//	//}
//	//}
//	__super::OnMoving(fwSide, pRect);
//
//	// TODO: Add your message handler code here
//}

LRESULT CPOPDlg::OnMessageReturnMainFrame(WPARAM wParam, LPARAM lParam)
{
	const bool bShowWnd = wParam==0?true:false;
	if (bShowWnd)
	{
		CFrameWndInfoProxy::ShowWnd((unsigned int)FRAME_BTN_ID_MAINFRAME);
	}
	m_nFrameType = CFrameWndInfo::FRAME_WND_MAIN_FRAME;
	m_btnMyCenter.ShowWindow(SW_SHOW);
	m_btnFileMgr.ShowWindow(SW_SHOW);
	m_btnMyShare.ShowWindow(SW_SHOW);
	m_btnMainFunc.ShowWindow(SW_SHOW);
	m_editSearch.ShowWindow(SW_SHOW);
#ifndef USES_NEW_UI_160111
	m_btnLineState.ShowWindow(SW_SHOW);
	m_btnSwitchToolbar.ShowWindow(SW_SHOW);
#endif
	if (!theApp.IsLogonVisitor())
	{
		//m_btnLineState.ShowWindow(SW_SHOW);
		//m_btnMyCenter.ShowWindow(SW_SHOW);
		//m_btnFileMgr.ShowWindow(SW_SHOW);
		//m_btnMyShare.ShowWindow(SW_SHOW);
		//m_btnMainFunc.ShowWindow(SW_SHOW);

		m_btnMyDepartment.ShowWindow(SW_SHOW);
		m_btnMyContacts.ShowWindow(SW_SHOW);
		if (theApp.IsEnterpriseuserUser())
			m_btnMyEnterprise.ShowWindow(SW_SHOW);
	}
	m_btnMySession.ShowWindow(SW_SHOW);
	m_pDlgMyGroup->ShowWindow(m_btnMyDepartment.GetChecked()?SW_SHOW:SW_HIDE);
	m_pDlgMyContacts->ShowWindow(m_btnMyContacts.GetChecked()?SW_SHOW:SW_HIDE);
	m_pDlgMySession->ShowWindow(m_btnMySession.GetChecked()?SW_SHOW:SW_HIDE);
	m_pDlgMyEnterprise->ShowWindow(m_btnMyEnterprise.GetChecked()?SW_SHOW:SW_HIDE);
#ifndef USES_NEW_UI_1220
	m_btnSwitchFrame.ShowWindow(SW_SHOW);
#endif
	//m_btnSwitchFrame.ShowWindow(m_pDlgAppFrame->IsEmpty()?SW_HIDE:SW_SHOW);
	if (m_pDlgAppFrame!=NULL)
	{
		//m_bAppFrameShowed = false;
		m_pDlgAppFrame->ShowWindow(SW_HIDE);
	}
	this->m_editSearch.SetWindowText(_T(""));
	//if (m_btnGoBack.GetSafeHwnd()!=0)
	//	m_btnGoBack.ShowWindow(SW_HIDE);
	//if (m_btnGoForward.GetSafeHwnd()!=0)
	//	m_btnGoForward.ShowWindow(SW_HIDE);
	//if (m_pPanelBrowserNavigate!=0)
	//	m_pPanelBrowserNavigate->ShowWindow(SW_HIDE);
	if (m_pPanelSearch!=0)
		m_pPanelSearch->ShowWindow(SW_HIDE);

	if (m_pDlgAppFrame!=NULL && !m_pDlgAppFrame->IsEmpty())
	{
#ifdef USES_SUPPORT_UI_STYLE
		const int theDefaultWidth = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_WIDTH:LEN_DLG_WIDTH;
		//const int theDefaultHeight = theApp.GetDefaultUIStyleType()==EB_UI_STYLE_TYPE_CHAT?UISTYLE_CHAT_DEFAULT_DLG_HEIGHT:LEN_DLG_HEIGHT;
#else
		const int theDefaultWidth = LEN_DLG_WIDTH;
		//const int theDefaultHeight = LEN_DLG_HEIGHT;
#endif
		CRect rect;
		this->GetWindowRect(&rect);
		if (rect.Width()<theDefaultWidth)
		{
			rect.right = rect.left + theDefaultWidth;
			this->MoveWindow(&rect);
		}
	}
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		pFuncButtonInfo->ShowWindow(true);
		//if (pFuncButtonInfo->m_btn.GetSafeHwnd()!=NULL)
		//{
		//	pFuncButtonInfo->m_btn.ShowWindow(SW_SHOW);
		//}
	}
	if (m_btnNewApp.GetSafeHwnd()!=NULL)
	{
		m_btnNewApp.ShowWindow((theApp.GetSystemAccountFlag()==1 && !theApp.GetEntManagerurl().empty())?SW_SHOW:SW_HIDE);
		//m_btnNewApp.ShowWindow(theApp.GetEntManagerurl().empty()?SW_HIDE:SW_SHOW);
	}
	CRect rect;
	this->GetClientRect(&rect);
	MoveSize(rect.Width(),rect.Height());
	return 0;
}

void CPOPDlg::SwitchFrameWnd(void)
{
	switch (m_nFrameType)
	{
	case CFrameWndInfo::FRAME_WND_WORK_FRAME:
		{
			OnBnClickedButtonSwitchFrame();
		}break;
	case CFrameWndInfo::FRAME_WND_MAIN_FRAME:
		{
			OnMessageReturnMainFrame(1,0);
		}break;
	case CFrameWndInfo::FRAME_WND_CALL_DIALOG:
		{
#ifndef USES_NEW_UI_160111
			m_btnMainFunc.ShowWindow(SW_HIDE);
			m_editSearch.ShowWindow(SW_HIDE);
			m_btnLineState.ShowWindow(SW_HIDE);
#endif
			m_btnMyCenter.ShowWindow(SW_HIDE);
			m_btnFileMgr.ShowWindow(SW_HIDE);
			m_btnMyShare.ShowWindow(SW_HIDE);
			m_btnMainFunc.ShowWindow(SW_HIDE);
			if (m_pDlgAppFrame!=NULL)
				m_pDlgAppFrame->ShowWindow(SW_HIDE);
			m_btnMyDepartment.ShowWindow(SW_HIDE);
			m_btnMyContacts.ShowWindow(SW_HIDE);
			m_btnMySession.ShowWindow(SW_HIDE);
			m_btnMyEnterprise.ShowWindow(SW_HIDE);
			m_pDlgMyGroup->ShowWindow(SW_HIDE);
			m_pDlgMyContacts->ShowWindow(SW_HIDE);
			m_pDlgMySession->ShowWindow(SW_HIDE);
			m_pDlgMyEnterprise->ShowWindow(SW_HIDE);
			m_btnSwitchFrame.ShowWindow(SW_HIDE);
			m_treeSearch.ShowWindow(SW_HIDE);

			if (m_pPanelSearch!=0)
				m_pPanelSearch->ShowWindow(SW_HIDE);

			for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
			{
				const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
				pFuncButtonInfo->ShowWindow(false);
				//if (pFuncButtonInfo->m_btn.GetSafeHwnd()!=NULL)
				//{
				//	pFuncButtonInfo->m_btn.ShowWindow(SW_HIDE);
				//}
			}
			if (m_btnNewApp.GetSafeHwnd()!=NULL)
			{
				m_btnNewApp.ShowWindow(SW_HIDE);
			}
		}break;
	default:
		break;
	}
}
void CPOPDlg::OnBnClickedButtonSwitchFrame()
{
	// TODO: Add your control notification handler code here
#ifdef USES_FRAMELIST_APPFRAME
	CDlgAppFrame* pDlgAppFrame = NewAppFrame(false);
#else
	CDlgAppFrame* pDlgAppFrame = m_pDlgAppFrame;
#endif
	if (pDlgAppFrame==0 || pDlgAppFrame->IsEmpty())
	{
		const tstring sHomeIndex = theApp.GetHomeIndexUrl();
		if (sHomeIndex.empty())
			AddAppUrl(false,"about:blank","",NULL);
		else
			AddAppUrl(false,sHomeIndex.string(),"",NULL);
		//m_editSearch.SetFocus();	// ** 没用
		return;
	}
#ifdef USES_SUPPORT_UI_STYLE
	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
	{
		CreateFrameList(false);
		pDlgAppFrame->SetParent(m_pDlgFrameList);
		CRect rectFrameList;
		m_pDlgFrameList->GetClientRect(&rectFrameList);
		const int nBorderLeft = m_pDlgFrameList->GetShowedLeft()?const_border_left:const_hide_border_left2;
		pDlgAppFrame->MoveWindow(nBorderLeft,const_border_size+theSearchToolbarHeight,rectFrameList.Width()-nBorderLeft-const_border_size,rectFrameList.Height()-const_border_size*2-theSearchToolbarHeight);
		m_pDlgFrameList->AddWnd(CFrameWndInfo::FRAME_WND_WORK_FRAME, _T("工作台"), _T("工作台 Ctrl+1"), true, false);
		m_pDlgFrameList->ShowFrameList();
		pDlgAppFrame->ShowWindow(SW_SHOW);
		pDlgAppFrame->RefreshAppWnd();
		//SetTimer(TIMERID_REFRESH_APPFRAME,1,NULL);
		return;
	}
#endif
	m_nFrameType = CFrameWndInfo::FRAME_WND_WORK_FRAME;
	//m_btnMainFunc.ShowWindow(SW_SHOW);
	m_editSearch.ShowWindow(SW_SHOW);
	m_editSearch.Invalidate();
#ifdef USES_NEW_UI_160111
	m_btnMyCenter.ShowWindow(SW_HIDE);
	m_btnFileMgr.ShowWindow(SW_HIDE);
	m_btnMyShare.ShowWindow(SW_HIDE);
	m_btnMainFunc.ShowWindow(SW_HIDE);
#else
	m_btnMyCenter.ShowWindow(SW_SHOW);
	m_btnFileMgr.ShowWindow(SW_SHOW);
	m_btnMyShare.ShowWindow(SW_SHOW);
	m_btnLineState.ShowWindow(SW_SHOW);
#endif
	if (pDlgAppFrame!=NULL && !pDlgAppFrame->IsEmpty())
	{
		// 
		m_btnSwitchToolbar.ShowWindow(SW_HIDE);
		//m_btnLineState.ShowWindow(SW_HIDE);
		//m_btnMyCenter.ShowWindow(SW_HIDE);
		//m_btnFileMgr.ShowWindow(SW_HIDE);
		//m_btnMyShare.ShowWindow(SW_HIDE);
		//m_btnMainFunc.ShowWindow(SW_HIDE);

		//m_bAppFrameShowed = true;
		pDlgAppFrame->SetCurrentFocus();
		pDlgAppFrame->ShowWindow(SW_SHOW);
		m_btnMyDepartment.ShowWindow(SW_HIDE);
		m_btnMyContacts.ShowWindow(SW_HIDE);
		m_btnMySession.ShowWindow(SW_HIDE);
		m_btnMyEnterprise.ShowWindow(SW_HIDE);
		m_pDlgMyGroup->ShowWindow(SW_HIDE);
		m_pDlgMyContacts->ShowWindow(SW_HIDE);
		m_pDlgMySession->ShowWindow(SW_HIDE);
		m_pDlgMyEnterprise->ShowWindow(SW_HIDE);
		m_btnSwitchFrame.ShowWindow(SW_HIDE);
		m_treeSearch.ShowWindow(SW_HIDE);
	}
	//if (m_btnGoBack.GetSafeHwnd()!=0)
	//	m_btnGoBack.ShowWindow(SW_SHOW);
	//if (m_btnGoForward.GetSafeHwnd()!=0)
	//	m_btnGoForward.ShowWindow(SW_SHOW);
	//if (m_pPanelBrowserNavigate!=0)
	//	m_pPanelBrowserNavigate->ShowWindow(SW_SHOW);
	if (m_pPanelSearch!=0)
		m_pPanelSearch->ShowWindow(SW_SHOW);
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		pFuncButtonInfo->ShowWindow(true);
		//if (pFuncButtonInfo->m_btn.GetSafeHwnd()!=NULL)
		//{
		//	pFuncButtonInfo->m_btn.ShowWindow(SW_SHOW);
		//}
	}
	if (m_btnNewApp.GetSafeHwnd()!=NULL)
	{
		m_btnNewApp.ShowWindow((theApp.GetSystemAccountFlag()==1 && !theApp.GetEntManagerurl().empty())?SW_SHOW:SW_HIDE);
		//m_btnNewApp.ShowWindow(theApp.GetEntManagerurl().empty()?SW_HIDE:SW_SHOW);
	}
	CRect rect;
	this->GetClientRect(&rect);
	MoveSize(rect.Width(),rect.Height());
	pDlgAppFrame->ShowWindow(SW_SHOW);
	pDlgAppFrame->RefreshAppWnd();
	//SetTimer(TIMERID_REFRESH_APPFRAME,1,NULL);
}

void CPOPDlg::OnBnClickedButtonNewApp()
{
	if (theApp.GetSystemAccountFlag()==1 && !theApp.GetEntManagerurl().empty())
	//if (!theApp.GetEntManagerurl().empty())
	{
		// eb/func/func.csp
		tstring sUrl = theApp.GetEntManagerurl();
		sUrl.append("&redirect_url=/eb/func/edit.csp");
		this->AddAppUrl(false,sUrl,"",NULL);
	}
}

void CPOPDlg::OnBnClickedButtonSwitchToolbar2()
{
	//if (m_bShowedToolbar)
	//{
	//	this->SetToolTipText(IDC_BUTTON_SWITCH_TOOLBAR2,_T("展开工具栏"));
	//	m_btnSwitchToolbar.SetDrawTrianglePic(2,RGB(64,64,64),-1,-1,-1,10,5);
	//}else
	//{
	//	this->SetToolTipText(IDC_BUTTON_SWITCH_TOOLBAR2,_T("收缩工具栏"));
	//	m_btnSwitchToolbar.SetDrawTrianglePic(1,RGB(64,64,64),-1,-1,-1,12,6);
	//}
	//m_bShowedToolbar = !m_bShowedToolbar;
	//this->Invalidate();
	//CRect rect;
	//this->GetClientRect(&rect);
	//MoveSize(rect.Width(),rect.Height());
	//m_btnSwitchToolbar.Invalidate();
}

LRESULT CPOPDlg::OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam)
{
#ifdef USES_SUPPORT_UI_STYLE
	if (this->m_pPanelSearch!=NULL)
		this->m_pPanelSearch->PostMessage(EB_COMMAND_CHANGE_APP_URL,wParam,lParam);
#else
	char * sUrl = (char*)wParam;
	if (sUrl==0 || strlen(sUrl)==0) return 0;
	const bool bSearchFocus = lParam==1?true:false;
	theSearchString = "";
	m_bCanSearch = false;
	boost::mutex::scoped_lock lockSearch(m_mutexSearch);
	CString sUrlTemp(sUrl);
	sUrlTemp.MakeLower();
	if (sUrlTemp==_T("about:blank"))
	{
		m_editSearch.SetWindowText(_T(""));
		if (bSearchFocus)
			m_editSearch.SetFocus();
	}else
	{
		const std::string csURL(libEbc::UTF82ACP(libEbc::URLDecode(sUrl,false).c_str()));
		m_editSearch.SetWindowText(csURL.c_str());
		//m_editSearch.SetWindowText(sUrl);
		if (bSearchFocus)
			this->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL,1,1);
		else
			this->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL,0,1);
	}
	delete[] sUrl;
#endif
	return 0;
}
LRESULT CPOPDlg::OnMsgShowRefreshOrStop(WPARAM wParam, LPARAM lParam)
{
	if (this->m_pPanelSearch!=NULL)
		this->m_pPanelSearch->PostMessage(EB_COMMAND_SHOW_REFRESH_OR_STOP,wParam,lParam);
	return 0;
}

void CPOPDlg::AddSubUnreadMsg(mycp::bigint nSubId, bool bSendToAppFrame)
{
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		if (pFuncButtonInfo->GetFuncInfo().m_nSubscribeId==nSubId)
		{
			pFuncButtonInfo->AddUnreadMsg(true);
			break;
		}
	}

	if (bSendToAppFrame)
	{
#ifdef USES_FRAMELIST_APPFRAME
		CDlgAppFrame * pDlgAppFrame = NewAppFrame(false);
#else
		//NewAppFrame();
		CDlgAppFrame * pDlgAppFrame = m_pDlgAppFrame;
#endif
		if (pDlgAppFrame!=NULL)
		{
			pDlgAppFrame->AddUnreadMsg(nSubId);
		}
	}
}

void CPOPDlg::SetSubUnreadMsg(mycp::bigint nSubId, size_t nUnreadMsgCount, bool bSendToAppFrame)
{
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		if (pFuncButtonInfo->GetFuncInfo().m_nSubscribeId==nSubId)
		{
			pFuncButtonInfo->SetUnreadMsg(nUnreadMsgCount,true);
			break;
		}
	}
	if (bSendToAppFrame)
	{
#ifdef USES_FRAMELIST_APPFRAME
		CDlgAppFrame * pDlgAppFrame = NewAppFrame(false);
#else
		//NewAppFrame();
		CDlgAppFrame * pDlgAppFrame = m_pDlgAppFrame;
#endif
		if (pDlgAppFrame!=NULL)
		{
			pDlgAppFrame->SetUnreadMsg(nSubId, nUnreadMsgCount);
		}
	}
}

LRESULT CPOPDlg::OnMsgClearSubIdUnReadmsg(WPARAM wParam, LPARAM lParam)
{
	char* lpszSubId = (char*)wParam;
	if (lpszSubId==NULL) return 0;
	const mycp::bigint nSubId = cgc_atoi64(lpszSubId);
	const bool bSendToAppFrame = lParam==1?true:false;
	//const mycp::bigint nSubId = MAKEINT64(wParam,lParam);
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		if (pFuncButtonInfo->GetFuncInfo().m_nSubscribeId==nSubId)
		{
			pFuncButtonInfo->ClearUnreadMsg();
			break;
		}
	}
	if (bSendToAppFrame)
	{
#ifdef USES_FRAMELIST_APPFRAME
		CDlgAppFrame * pDlgAppFrame = NewAppFrame(false);
#else
		//NewAppFrame();
		CDlgAppFrame * pDlgAppFrame = m_pDlgAppFrame;
#endif
		if (pDlgAppFrame!=NULL)
		{
			pDlgAppFrame->ClearUnreadMsg(nSubId);
		}
	}
	delete[] lpszSubId;
	return 0;
}

LRESULT CPOPDlg::OnMsgDownloadResource(WPARAM wParam, LPARAM lParam)
{
	// eb-download-resource://[TYPE],[RESOURCEID]
	// eb-download-resource://[TYPE],[RESOURCEID],[FILENAME]
	char * lpszResourceInfo = (char*)wParam;
	if (lpszResourceInfo==NULL) return 1;
	const CString sResourceInfo(lpszResourceInfo);
	delete[] lpszResourceInfo;
	const int nFind1 = sResourceInfo.Find(',');
	if (nFind1<=0) return 1;
	const int nType = atoi(sResourceInfo.Left(nFind1));
	const int nFind2 = sResourceInfo.Find(',',nFind1+1);
	eb::bigint nResourceId = 0;
	CString sDefaultFileName;
	if (nFind2>0)
	{
		nResourceId = eb_atoi64(sResourceInfo.Mid(nFind1+1,nFind2-nFind1));
		const tstring sFileName(libEbc::URLDecode(sResourceInfo.Mid(nFind2+1),true));
		sDefaultFileName = libEbc::UTF82ACP(sFileName.c_str()).c_str();
		//sDefaultFileName = libEbc::UTF82ACP(sResourceInfo.Mid(nFind2+1)).c_str();
	}else
	{
		nResourceId = eb_atoi64(sResourceInfo.Mid(nFind1+1));
		sDefaultFileName.Format(_T("%lld"),nResourceId);
	}

	tstring sFileName;
	tstring sFileExt;
	libEbc::GetFileExt((LPCTSTR)sDefaultFileName,sFileName,sFileExt);
	CFileDialog dlg(FALSE, sFileExt.c_str(), sDefaultFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, theAllFilesFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		const CString sPathName(dlg.GetPathName());
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_DownloadFileRes(nResourceId, (LPCTSTR)sPathName);
#else
		theEBAppClient.EB_DownloadFileRes(nResourceId, sPathName);
#endif
	}
	return 0;
}

void CPOPDlg::OnNMClickTreeSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	SetTimer(TIMERID_CALL_SEARCH_SEL_CHANGE,1,NULL);
}

void CPOPDlg::OnBnClickedButtonGoback()
{
	// TODO: Add your control notification handler code here
}

void CPOPDlg::OnBnClickedButtonGoforward()
{
	// TODO: Add your control notification handler code here
}

void CPOPDlg::OnFrameBtnClicked(UINT nID)
{
	CFrameWndInfoProxy::ClickFrame(nID);
}

void CPOPDlg::OnBnClickedButtonSwitchLeft()
{
	m_bShowedLeft = !m_bShowedLeft;
	if (m_pDlgAppFrame!=NULL && m_pDlgAppFrame->GetSafeHwnd()!=NULL)
	{
		// ** 用于实现，隐藏左边栏时，显示工作区有打开多少个页面页；
		if (m_bShowedLeft)
		{
			CFrameWndInfoProxy::SetUnreadMsg(CFrameWndInfo::FRAME_WND_WORK_FRAME,0);
		}else
		{
			const size_t nSize = m_pDlgAppFrame->GetSize();
			CFrameWndInfoProxy::SetUnreadMsg(CFrameWndInfo::FRAME_WND_WORK_FRAME,nSize);
		}
	}
	WritePrivateProfileString(_T("default"),_T("show-left"),(m_bShowedLeft?_T("1"):_T("0")),theApp.GetUserSettingIniFile());
	this->SetToolTipText(IDC_BUTTON_SWITCH_LEFT,m_bShowedLeft?_T("收起面板"):_T("展开面板"));
	m_btnSwitchLeft.SetDrawTrianglePic((m_bShowedLeft?3:4),theDefaultFlatLineColor,theDefaultFlatBlackText2Color,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	CFrameWndInfoProxy::SetShowHideLeft(m_bShowedLeft);
	this->Invalidate();
	CRect rect;
	this->GetClientRect(&rect);
	MoveSize(rect.Width(),rect.Height());
	m_btnSwitchLeft.Invalidate();
}

void CPOPDlg::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	__super::OnActivateApp(bActive, dwThreadID);

	if (bActive && GetWorkFrameShowed() && m_pDlgAppFrame!=NULL && this->IsWindowVisible())
	{
		m_pDlgAppFrame->RefreshAppWnd();
	}
}

//void CPOPDlg::OnEnSetfocusEditSearch()
//{
//	m_editSearch.SetSel(0,-1);
//}

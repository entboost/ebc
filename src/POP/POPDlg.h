// POPDlg.h : 头文件
//
#pragma once

#define UISTYLE_CHAT_DEFAULT_DLG_WIDTH	288
#define UISTYLE_CHAT_DEFAULT_DLG_HEIGHT	568

#include "stl/lockmap.h"
#include "DlgDialog.h"
#include "DlgIncomingCall.h"
#include "DlgAlertingCall.h"
#include "EBFuncButton.h"
#include "afxwin.h"
#include "FrameWndInfo.h"
#include "PanelText.h"
class CDlgMyEnterprise;
class CDlgMyGroup;
class CDlgMyContacts;
class CDlgMySession;
class CDlgEditInfo;
class CDlgFileManager;
class CDlgVideoConference;
class CDlgShrinkageBar;
class CDlgFrameList;
class CDlgAppFrame;
class CPanelSearch;
//class CPanelBrowserNavigate;
class CDlgViewContactInfo;
class CDlgMsgTip;

#define USES_APPFRAME

class CEBCMsgInfo
{
public:
	typedef enum MSG_TYPE
	{
		MSG_TYPE_UNKNOWN
		//, MSG_TYPE_RECEIVE_RICH
		, MSG_TYPE_CREATE_FUNC_WINDOW
		, MSG_TYPE_USER_ENTER_ROOM
	};
	typedef boost::shared_ptr<CEBCMsgInfo> pointer;
	static CEBCMsgInfo::pointer create(MSG_TYPE nMsgType,bool bPlayMsgSound=false)
	{
		return CEBCMsgInfo::pointer(new CEBCMsgInfo(nMsgType,bPlayMsgSound));
	}
	MSG_TYPE GetMsgType(void) const {return m_nMsgType;}
	bool GetPlayMsgSound(void) const {return m_bPlayMsgSound;}
	time_t GetCreateTime(void) const {return m_tCreateTime;}
	CEBCCallInfo::pointer m_pEbCallInfo;
	CDlgFuncWindow* m_pFuncWindow;
	CWnd* m_pParent;
	bool m_bSetForegroundWindow;
	CCrAccountInfo m_pAccountInfo;

	CEBCMsgInfo(MSG_TYPE nMsgType,bool bPlayMsgSound)
		: m_nMsgType(nMsgType), m_pFuncWindow(NULL), m_pParent(NULL)
		, m_bPlayMsgSound(bPlayMsgSound)
		, m_bSetForegroundWindow(false)
	{
		m_tCreateTime = time(0);
	}
	CEBCMsgInfo(void)
		: m_nMsgType(MSG_TYPE_UNKNOWN), m_pFuncWindow(NULL), m_pParent(NULL)
		, m_bPlayMsgSound(false)
		, m_bSetForegroundWindow(false)
	{
		m_tCreateTime = time(0);
	}
private:
	MSG_TYPE m_nMsgType;
	bool m_bPlayMsgSound;
	time_t m_tCreateTime;
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

// CPOPDlg 对话框
/*
 * ebc程序，主界面
 * 
*//////////////////////////////////////////////////////

class CPOPDlg : public CEbDialogBase
	, public CTreeCallback
#ifdef USES_EBCOM_TEST
	, public CEBCoreEventsSink
	, public CEBSearchEventsSink
#else
	, public CEBSearchCallback
#endif
	, public CFrameWndInfoProxy, public CFrameWndInfoProxyCallback
{
// 构造
public:
	CPOPDlg(CWnd* pParent = NULL);	// 标准构造函数

	void AddAppUrl(bool bSaveBrowseTitle, const std::string& sAppUrl, const std::string& sPostData, const EB_SubscribeFuncInfo& pSubscribeFuncInfo, bool nOpenNewClose, HWND hwndFrom);
	void AddAppUrl(bool bSaveBrowseTitle, const std::string& sAppUrl, const std::string& sPostData, HWND hwndFrom);
	bool GetMainFrameShowed(void) const {return m_nFrameType==CFrameWndInfo::FRAME_WND_MAIN_FRAME?true:false;}
	bool GetWorkFrameShowed(void) const {return m_nFrameType==CFrameWndInfo::FRAME_WND_WORK_FRAME?true:false;}
	bool GetCallFrameShowed(void) const {return m_nFrameType==CFrameWndInfo::FRAME_WND_CALL_DIALOG?true:false;}
	CFrameWndInfo::FRAME_WND_TYPE GetFrameTypeShowed(void) const {return m_nFrameType;}

// 对话框数据
	enum { IDD = IDD_POP_DIALOG };
protected:
	CPanelText * m_pPanelStatus;
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	//bool m_bTimerToSwitchOnUIStyleTypeOffice;
	bool m_bShowedLeft;
	CTraButton m_btnSwitchLeft;
	CRect m_rectSwitchLeft;
	bool m_bShowedToolbar;
	CTraButton m_btnSwitchToolbar;
	CRect m_rectSwitchToolbar;
	CTraButton m_btnSkin;
	CTraButton m_btnMin;
	CTraButton m_btnMax;
	CTraButton m_btnClose;
	CTraButton m_btnLineState;
	CTraButton m_btnMyCenter;
	CTraButton m_btnFileMgr;
	CTraButton m_btnMyShare;
	CTraButton m_btnMainFunc;
	CTraButton m_btnGoBack;
	CTraButton m_btnGoForward;
	//CString m_sCallNumber;
	//CComboBox m_comboNumbers;
	//CxSkinButton m_btnCallUser;
	CTraButton m_btnMyEnterprise;
	CTraButton m_btnMyDepartment;
	CTraButton m_btnMyContacts;
	CTraButton m_btnMySession;
	CTraButton m_btnSwitchFrame;
	CRect m_rectSwitchFrame;
	boost::mutex m_mutexSearch;
	VividTree m_treeSearch;				// itemdata:1=employee;2=contact
	CTraButton m_btnSearchTrackCall;	// search call
	CTraButton m_btnSearchTrackDel;		// search del
	CHoverEdit m_editSearch;
	CString theSearchString;
	bool m_bCanSearch;
	//CPanelBrowserNavigate* m_pPanelBrowserNavigate;
	CPanelSearch* m_pPanelSearch;
	//CHoverEdit m_editDescription;
	CEdit m_editDescription;
	CTraButton m_btnNewApp;
	CRect m_rectNewApp;

	CRect m_rectHead;
	CRect m_rectAdjustOld;

//#ifdef USES_EBCOM
//	DECLARE_INTERFACE_MAP()
//	DECLARE_DISPATCH_MAP()
//
//	//// Connection point for ISample interface
//	//BEGIN_CONNECTION_PART(CPOPDlg, SampleConnPt)
//	//	CONNECTION_IID(__uuidof(_IEBClientCoreEvents))
//	//END_CONNECTION_PART(SampleConnPt)
//	//DECLARE_CONNECTION_MAP()
//
//	void onCallConnected2(IDispatch * pCallInfo, long nConnectFlag)
//	{
//		CComPtr<IEB_CallInfo> pInterface;
//		pCallInfo->QueryInterface(__uuidof(IEB_CallInfo),(void**)&pInterface);
//		if (pInterface != NULL)
//		{
//			::SendMessage(this->GetSafeHwnd(),EB_WM_CALL_CONNECTED,(WPARAM)(IEB_CallInfo*)pInterface,(LPARAM)nConnectFlag);
//		}
//	}
//	void onGroupInfo2(IDispatch * pGroupInfo, VARIANT_BOOL bIsMyGroup)
//	{
//		CComPtr<IEB_GroupInfo> pInterface;
//		pGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface);
//		if (pInterface != NULL)
//		{
//		AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			//if (m_hFireWnd)
//				::SendMessage(GetSafeHwnd(),EB_WM_GROUP_INFO,(WPARAM)(IEB_GroupInfo*)pInterface,bIsMyGroup?1:0);
//			//else
//				//Fire_onGroupInfo(pInterface, bIsMyGroup?true:false);
//		}
//	}
//	void onMemberInfo2(IDispatch * pMemberInfo, VARIANT_BOOL bIsMyDefaultMember)
//	{
//		CComPtr<IEB_MemberInfo> pInterface;
//		pMemberInfo->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
//		if (pInterface != NULL)
//		{
//		AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			//if (m_hFireWnd)
//				::SendMessage(GetSafeHwnd(),EB_WM_MEMBER_INFO,(WPARAM)(IEB_MemberInfo*)pInterface,(LPARAM)0);
//			//else
//				//Fire_onMemberInfo(pInterface, bIsMyDefaultMember?true:false);
//		}
//	}
//#endif

// 实现
protected:
	CNewMenu m_menuFunc;
	CNewMenu m_menuState;
	bool m_bRemoveAppFuncIndex1;	// 主界面
	bool m_bRemoveAppFuncIndex2;	// 程序图标
	int m_nMainFrameMyCollectionMenuIndex;
	int m_nMenuOpenWorkFrameIndex;	// for 经典聊天风格
	int m_nMenuShowHeadOnlyIndex;		// for 专业办公风格
	int m_nMenuIconOpenWorkFrameIndex;
	CNewMenu m_menuTray;
	CNewMenu m_menuSkin;
#ifdef USES_SUPPORT_UI_STYLE
	CNewMenu m_menuUIStyleType;
#endif
	CNewMenu m_menuSetBrowserType;
	//CMenu m_menuSkin;
	//CMenu m_menuSetBrowserType;
	NOTIFYICONDATA m_trayIconData;
	std::vector<EB_MemberInfo> m_pMyEmployeeInfo;			// for menu
	std::vector<EB_SubscribeFuncInfo> m_pSubscribeFuncList;	// for menu
	std::vector<CEBFuncButton::pointer> m_pMainFuncButtonList;	// for mainframe

	bool m_bOpenOk;
	bool m_bShrinkageWin;	// -1=up 1=down
	//int m_nShrinkageWin;
	CDlgShrinkageBar* m_pDlgShrinkageBar;
	CRect m_rectWin;
	//HICON m_hIcon;
	CDlgEditInfo* m_pDlgEditInfo;
	CDlgMyEnterprise* m_pDlgMyEnterprise;
	CDlgMyGroup* m_pDlgMyGroup;
	CDlgMyContacts* m_pDlgMyContacts;
	CDlgMySession* m_pDlgMySession;
	CLockMap<eb::bigint, CDlgIncomingCall::pointer> m_pIncomingCallList;	// fromaccount->
	CLockMap<eb::bigint, CDlgAlertingCall::pointer> m_pAlertingCallList;	// fromaccount->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pSubFuncItem;		// subid->
	CLockMap<std::string, CTreeItemInfo::pointer> m_pUrlItem;		// url->
	CLockMap<std::string, CTreeItemInfo::pointer> m_pContactItem;		// contact->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pEntGroupMemberItem;		// emp_code->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pMyGroupMemberItem;		// emp_code->
	CDlgViewContactInfo* m_pViewContactInfo;
	CDlgMsgTip* m_pDlgMsgTip;
	//CLockMap<HTREEITEM, std::string> m_pSearchItem2;		//
	CLockList<CEBCMsgInfo::pointer> m_pEbcMsgList;

	void SaveCallRecord(eb::bigint nCallId,eb::bigint sGroupCode,const EB_AccountInfo& pFromAccountInfo);
	void DeleteDlgIncomingCall(eb::bigint sFromAccount);
	void DeleteDlgAlertingCall(eb::bigint sFromAccount);

	CDlgDialog::pointer GetDlgDialog(const CEBCCallInfo::pointer& pEbCallInfo,bool bShow=true,bool bOwnerCall=false);
	bool AddSubscribeFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo);
#ifdef USES_FRAMELIST_APPFRAME
	CDlgAppFrame* NewAppFrame(bool bCreate=true);
#else
	void NewAppFrame(void);
#endif
	bool AddAppFrameFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo);

	CDlgFuncWindow * m_pDlgBroadcastMsg;
	CDlgFrameList* m_pDlgFrameList;
	CFrameWndInfo::FRAME_WND_TYPE m_nFrameType;
	//bool m_bAppFrameShowed;
	CDlgAppFrame* m_pDlgAppFrame;
	CLockList<EB_SubscribeFuncInfo> m_pMainSubscribeFuncList;	// for DlgFrame
	//bool m_bShowAppFrame;
	// file manager
	CDlgFileManager* m_pDlgFileManager;
	void ShowDlgFileManager(void);
	void ShowStatusMessage(const wchar_t* sValue);
#ifdef USES_EBCOM_TEST
	virtual void Fire_onSearchGroupInfo(IEB_GroupInfo* pGroupInfo, IEB_EnterpriseInfo* pEnterpriseInfo, ULONG dwParam) {}
	virtual void Fire_onSearchMemberInfo(IEB_GroupInfo* pGroupInfo, IEB_MemberInfo* pMemberInfo, ULONG dwParam);
	virtual void Fire_onSearchContactInfo(IEB_ContactInfo* pContactInfo, ULONG dwParam);
#else
	virtual void onGroupInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo,unsigned long dwParam) {}
	virtual void onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam);
	virtual void onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam);
#endif
	void ScrollTreeSearchToTop(void);
	// CFrameWndInfoProxyCallback
	//virtual void OnFrameWndEmpty(void);
	virtual void OnFrameWndShow(const CFrameWndInfo::pointer& pFrameWndInfo, bool bShow);
#ifndef USES_FRAMELIST_APPFRAME
	virtual void OnFrameSize(const CFrameWndInfo::pointer& pFrameWndInfo, int cx, int cy);
	virtual size_t OnGetFrameSize(const CFrameWndInfo::pointer& pFrameWndInfo) const;
#endif

	// chatroom
	//LRESULT OnMessageEnterRoom(WPARAM wParam, LPARAM lParam);
	//LRESULT OnMessageExitRoom(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageReturnMainFrame(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageUserEnterRoom(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageUserExitRoom(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageUserNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageReceiveRich(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageSendRich(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMsgReceipt(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageSendingFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageSendedFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageCancelFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageReceivingFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageReceivedFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageFilePercent(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageSave2CloudDrive(WPARAM wParam, LPARAM lParam);

	LRESULT OnMessageRDRequestResponse(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRDAckResponse(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRDRequest(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRDAccept(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRDCancel(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRDEnd(WPARAM wParam, LPARAM lParam);

	LRESULT OnMessageVRequestResponse(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageVAckResponse(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageVideoRequest(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageVideoAccept(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageVideoCancel(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageVideoTimeout(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageVideoEnd(WPARAM wParam, LPARAM lParam);

	LRESULT OnMessageAreaInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageBroadcastMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageNewVersion(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageStateCode(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageServerChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageOnlineAnother(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageLogoutResponse(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageLogonSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageLogonTimeout(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageLogonError(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageAccountInfoChange(WPARAM wParam, LPARAM lParam);

	LRESULT OnMessageSInfoResponse(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMemberStateChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMemberHeadChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageContactHeadChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageCallConnected(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageCallHangup(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageAlertingCall(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageCallError(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageIncomingCall(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageUGInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageUGDelete(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageContactInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageContactDelete(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageContactStateChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageEnterpriseInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageGroupInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageGroupDelete(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageGroupEditError(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRemoveGroup(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageExitGroup(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRequestJoin2Group(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageInviteJoin2Group(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRejectJoin2Group(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMemberInfo(WPARAM wParam, LPARAM lParam);
	//LRESULT OnMessageMemberDelete(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMemberEditError(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRequestAddContact(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageRejectAddContact(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageAcceptAddContact(WPARAM wParam, LPARAM lParam);
	//LRESULT OnMessageAppMsgInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageResourceInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageDeleteResource(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMoveResource(WPARAM wParam, LPARAM lParam);
//#endif
	LRESULT OnMessageFileManager(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageHotKey(WPARAM wParam, LPARAM lParam);

	//LRESULT OnMessageWindowResize(WPARAM wParam, LPARAM lParam);
	//LRESULT OnTreeItemSelChange(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemDoubleClick(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);
	LRESULT OnIconNotification(WPARAM wp, LPARAM lp);
	LRESULT OnMsgExitApp(WPARAM wp, LPARAM lp);
	LRESULT OnMsgLogout(WPARAM wp, LPARAM lp);
	LRESULT OnMsgShowHideMainFrame(WPARAM wp, LPARAM lp);
	//LRESULT OnMsgOpenSubId(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam);
	//LRESULT OnMsgShellExecuteOpen(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgRefreshUrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgRefreshOrStopUrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgRedrawAppFrame(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgGoBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgGoForward(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgChangeBrowserType(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgQueryCanSaveHistory(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgQueryBrowserType(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgSaveHistory(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgSearchSetFocusSel(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndMin(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndMax(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndClose(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndCount(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMoveUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMoveDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMoveOffset(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageAdjustWidth(WPARAM wParam, LPARAM lParam);

	void CreateFrameList(bool bShow);
	CDlgDialog::pointer GetCallIdDialog(eb::bigint nCallId, bool bRemove=false);

	void DrawInfo(void);
	void DeleteItem(HTREEITEM hItem);
	bool CallItem(HTREEITEM hItem);
	//void CallItem(HTREEITEM hItem, bool bSelChange=false);
	void CancelSaveDescription(void);

	void InitData(void);
	HWND m_pCurrentLabel;
	HWND m_pOldCurrentLabel;
	void RefreshLabelWindow(void);
	void RefreshEditDescription(void);
	void ChangeTrayText(void);

	void SwitchFrameWnd(void);
	void MoveSize(int cx, int cy);
	void SetCtrlColor(void);
	//void abc(void){};
	void ShowSearchResult(void);
	// 生成的消息映射函数
	void SetWindowTextAndTrayInfo(bool bFirstTrayIcon);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//afx_msg void OnCbnSelchangeComboNumbers();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnBnClickedButtonCalluser();
	afx_msg void OnBnClickedButtonEnterprise();
	afx_msg void OnBnClickedButtonDepartment();
	afx_msg void OnBnClickedButtonContact();
	afx_msg void OnBnClickedButtonMax();
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonClose();
	void OnEditSearch(void);
	void HideSearchResult(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEditSearch();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnExitApp();
	afx_msg void OnLogout();
	afx_msg void OnBnClickedButtonLinestate();
	afx_msg void OnStateOnline();
	afx_msg void OnStateAway();
	afx_msg void OnStateBusy();
	afx_msg void OnAutoLogin();	// 取消自动登录
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnEnKillfocusEditDescription();
	afx_msg void OnBnClickedButtonSession();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMySetting();
	afx_msg void OnFileManager();
	afx_msg void OnMyShare();
	afx_msg void OnMyCollection();
	void CreateUIStyleMenu(void);
	afx_msg void OnBnClickedButtonSkin2();
	afx_msg void OnSkinSelect(UINT nID);
	afx_msg void OnSkinFlat(void);
	afx_msg void OnSkinGradient(void);
	afx_msg void OnBrowserTypeCef(void);
	afx_msg void OnBrowserTypeIe(void);
	void BuildHeadRect(void);
	afx_msg void OnUIStyleTypeOffice(void);
	afx_msg void OnUIStyleTypeChat(void);
	afx_msg void OnSettingCheckNewVersion(void);
	afx_msg void OnOpenMain();
	afx_msg void OnShowHeadOnly();
	afx_msg void OnWorkFrame();
	LRESULT OnMsgWorkFrame(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMyEmployeeInfo(UINT nID);
	void ClearUnreadMsgBySubId(mycp::bigint nSubscribeId);
	afx_msg void OnSubscribeFunc(UINT nID);
	afx_msg void OnMainFrameFunc(UINT nID);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonMyCenter();
	afx_msg void OnBnClickedButtonFileMgr();
	afx_msg void OnBnClickedButtonMyShare();
	afx_msg void OnBnClickedButtonMainFunc();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnMove(int x, int y);
	//afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	//afx_msg void OnBnClickedButtonSwitchFrame();
	afx_msg void OnBnClickedButtonSwitchFrame();
	afx_msg void OnBnClickedButtonNewApp();
	afx_msg void OnBnClickedButtonSwitchToolbar2();
	LRESULT OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgShowRefreshOrStop(WPARAM wParam, LPARAM lParam);
	void AddSubUnreadMsg(mycp::bigint nSubId, bool bSendToAppFrame);
	void SetSubUnreadMsg(mycp::bigint nSubId, size_t nUnreadMsgCount, bool bSendToAppFrame);
	LRESULT OnMsgClearSubIdUnReadmsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgDownloadResource(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMClickTreeSearch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonGoback();
	afx_msg void OnBnClickedButtonGoforward();
	afx_msg void OnFrameBtnClicked(UINT nID);
	afx_msg void OnBnClickedButtonSwitchLeft();
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	//afx_msg void OnEnSetfocusEditSearch();
};

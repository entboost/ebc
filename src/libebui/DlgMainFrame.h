#pragma once
#include "EBUIApp.h"
#include "DlgDialog.h"
#include "DlgIncomingCall.h"
#include "DlgAlertingCall.h"
#include "EBFuncButton.h"
#include "afxwin.h"
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

#define USES_APPFRAME

class CEBCMsgInfo
{
public:
	typedef enum MSG_TYPE
	{
		MSG_TYPE_UNKNOWN
		//, MSG_TYPE_RECEIVE_RICH
		, MSG_TYPE_CREATE_FUNC_WINDOW
	};
	typedef boost::shared_ptr<CEBCMsgInfo> pointer;
	static CEBCMsgInfo::pointer create(MSG_TYPE nMsgType,bool bPlayMsgSound=false)
	{
		return CEBCMsgInfo::pointer(new CEBCMsgInfo(nMsgType,bPlayMsgSound));
	}
	MSG_TYPE GetMsgType(void) const {return m_nMsgType;}
	bool GetPlayMsgSound(void) const {return m_bPlayMsgSound;}
	CEBCCallInfo::pointer m_pEbCallInfo;
	CDlgFuncWindow* m_pFuncWindow;
	CWnd* m_pParent;
	bool m_bSetForegroundWindow;

	CEBCMsgInfo(MSG_TYPE nMsgType,bool bPlayMsgSound)
		: m_nMsgType(nMsgType), m_pFuncWindow(NULL), m_pParent(NULL)
		, m_bPlayMsgSound(bPlayMsgSound)
		, m_bSetForegroundWindow(false)
	{}
	CEBCMsgInfo(void)
		: m_nMsgType(MSG_TYPE_UNKNOWN), m_pFuncWindow(NULL), m_pParent(NULL)
		, m_bPlayMsgSound(false)
		, m_bSetForegroundWindow(false)
	{}
private:
	MSG_TYPE m_nMsgType;
	bool m_bPlayMsgSound;
};

#define USES_MIN_UI
// CDlgMainFrame dialog

class CDlgMainFrame : public CEbDialogBase
//	, public CTreeCallback
//#ifdef USES_EBCOM_TEST
//	, public CEBCoreEventsSink
//	, public CEBSearchEventsSink
//#else
//	, public CEBSearchCallback
//#endif
{
	DECLARE_DYNAMIC(CDlgMainFrame)

public:
	CDlgMainFrame(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMainFrame();

	void SetMsgId(UINT nMsgId) {m_nMsgId = nMsgId;}
	void AddAppUrl(const std::string& sAppUrl,const EB_SubscribeFuncInfo& pSubscribeFuncInfo, bool nOpenNewClose);
	void AddAppUrl(const std::string& sAppUrl);
	bool GetAppFrameShowed(void) const {return m_bAppFrameShowed;}
	void SetChatWindowTopMost(bool bTopMost);

// Dialog Data
	enum { IDD = IDD_DLG_MAINFRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
#ifndef USES_MIN_UI
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
#endif
	//CString m_sCallNumber;
	//CComboBox m_comboNumbers;
	//CxSkinButton m_btnCallUser;
	CTraButton m_btnMyEnterprise;
	CTraButton m_btnMyDepartment;
	CTraButton m_btnMyContacts;
	CTraButton m_btnMySession;
	CTraButton m_btnSwitchFrame;
	CRect m_rectSwitchFrame;
#ifndef USES_MIN_UI
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

	CMenu m_menuFunc;
	CMenu m_menuState;
	bool m_bRemoveAppFuncIndex;
	CMenu m_menuTray;
	CMenu m_menuSkin;
	NOTIFYICONDATA m_trayIconData;
	std::vector<EB_MemberInfo> m_pMyEmployeeInfo;			// for menu
	std::vector<EB_SubscribeFuncInfo> m_pSubscribeFuncList;	// for menu
	std::vector<CEBFuncButton::pointer> m_pMainFuncButtonList;	// for mainframe

	bool m_bOpenOk;
	bool m_nShrinkageWin;	// -1=up 1=down
	CDlgShrinkageBar* m_pDlgShrinkageBar;
	CRect m_rectWin;

	//HICON m_hIcon;
	CDlgEditInfo* m_pDlgEditInfo;
#endif
	CDlgMyEnterprise* m_pDlgMyEnterprise;
	CDlgMyGroup* m_pDlgMyGroup;
	CDlgMyContacts* m_pDlgMyContacts;
	CDlgMySession* m_pDlgMySession;
	CLockMap<eb::bigint, CDlgIncomingCall::pointer> m_pIncomingCallList;	// fromaccount->
	CLockMap<eb::bigint, CDlgAlertingCall::pointer> m_pAlertingCallList;	// fromaccount->
	CLockMap<std::string, CTreeItemInfo::pointer> m_pUrlItem;		// url->
	CLockMap<std::string, CTreeItemInfo::pointer> m_pContactItem;		// contact->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pEntGroupMemberItem;		// emp_code->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pMyGroupMemberItem;		// emp_code->
	CDlgViewContactInfo* m_pViewContactInfo;
	//CLockMap<HTREEITEM, std::string> m_pSearchItem2;		//
	CLockList<CEBCMsgInfo::pointer> m_pEbcMsgList;

	void SaveCallRecord(eb::bigint nCallId,eb::bigint sGroupCode,const EB_AccountInfo& pFromAccountInfo);
	void DeleteDlgIncomingCall(eb::bigint sFromAccount);
	void DeleteDlgAlertingCall(eb::bigint sFromAccount);

	CDlgDialog::pointer GetDlgDialog(const CEBCCallInfo::pointer& pEbCallInfo,bool bShow=true);
#ifndef USES_MIN_UI
	bool AddSubscribeFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo);
#endif
	void NewAppFrame(void);
	bool AddAppFrameFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo);

	UINT m_nMsgId;
	CDlgFrameList* m_pDlgFrameList;
	bool m_bAppFrameShowed;
	CDlgAppFrame* m_pDlgAppFrame;
	//bool m_bShowAppFrame;
	// file manager
	CDlgFileManager* m_pDlgFileManager;
	void ShowDlgFileManager(void);
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
	// chatroom
	//LRESULT OnMessageEnterRoom(WPARAM wParam, LPARAM lParam);
	//LRESULT OnMessageExitRoom(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageReturnMainFrame(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageUserEnterRoom(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageUserExitRoom(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageReceiveRich(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageSendRich(WPARAM wParam, LPARAM lParam);
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

	LRESULT OnMessageSInfoResponse(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageUserStateChange(WPARAM wParam, LPARAM lParam);
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
#ifndef USES_MIN_UI
	LRESULT OnMessageHotKey(WPARAM wParam, LPARAM lParam);
#endif
	//LRESULT OnMessageWindowResize(WPARAM wParam, LPARAM lParam);
	//LRESULT OnTreeItemSelChange(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemDoubleClick(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);
#ifndef USES_MIN_UI
	LRESULT OnIconNotification(WPARAM wp, LPARAM lp);
#endif
	LRESULT OnMsgExitApp(WPARAM wp, LPARAM lp);
	LRESULT OnMsgLogout(WPARAM wp, LPARAM lp);
	LRESULT OnMsgShowHideMainFrame(WPARAM wp, LPARAM lp);
	LRESULT OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgRefreshUrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgGoBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgGoForward(WPARAM wParam, LPARAM lParam);

	void CreateFrameList(bool bShow);
	CDlgDialog::pointer GetCallIdDialog(eb::bigint nCallId, bool bRemove = false);

	void DrawInfo(void);
#ifndef USES_MIN_UI
	void DeleteItem(HTREEITEM hItem);
	void CallItem(HTREEITEM hItem, bool bSelChange=false);
#endif
	void CancelSaveDescription(void);

	void InitData(void);
	HWND m_pCurrentLabel;
	HWND m_pOldCurrentLabel;
	void RefreshLabelWindow(void);
	void RefreshEditDescription(void);
	void ChangeTrayText(void);

	void MoveSize(int cx, int cy);
	void SetCtrlColor(void);
	//void abc(void){};
	void ShowSearchResult(void);
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	//afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//afx_msg void OnCbnSelchangeComboNumbers();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnBnClickedButtonCalluser();
	afx_msg void OnBnClickedButtonEnterprise();
	afx_msg void OnBnClickedButtonDepartment();
	afx_msg void OnBnClickedButtonContact();
#ifndef USES_MIN_UI
	afx_msg void OnBnClickedButtonMax();
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonClose();
#endif
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
	afx_msg void OnBnClickedButtonSkin2();
	afx_msg void OnSkinSelect(UINT nID);
	afx_msg void OnSkinGradient(void);
	afx_msg void OnOpenMain();
	afx_msg void OnMyEmployeeInfo(UINT nID);
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
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
public:
	//afx_msg void OnBnClickedButtonSwitchFrame();
	afx_msg void OnBnClickedButtonSwitchFrame();
	afx_msg void OnBnClickedButtonNewApp();
	afx_msg void OnBnClickedButtonSwitchToolbar2();
	LRESULT OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMClickTreeSearch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonGoback();
	afx_msg void OnBnClickedButtonGoforward();
};

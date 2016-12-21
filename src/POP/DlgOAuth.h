#pragma once
#include "Core/Explorer.h"

// CDlgOAuth dialog
/*
 * 开放验证窗口类
 * 用于在CPOP::InitInstance程序启动时，调用弹开，自动启动eblogin.exe程序，等待用户验证登录成功后，
 * 自动关闭当前窗口，打开主界面。
 * 
*//////////////////////////////////////////////////////

class CDlgOAuth : public CDialog
#ifdef USES_EBCOM_TEST
	, public CEBCoreEventsSink
#endif
{
	DECLARE_DYNAMIC(CDlgOAuth)

public:
	CDlgOAuth(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOAuth();

	static void ExecuteEBLogonServerError(void);
	tstring m_sAppParameter0;
	const CString& GetAccountPrefix(void) const {return m_sAccountPrefix;}
	const CString& GetProductName(void) const {return m_sProductName;}
	bool GetLicenseUser(void) const {return m_bLicenseUser;}
	bool GetSendRegMail(void) const {return m_bSendRegMail;}
	bool GetSaveConversationLocal(void) const {return (m_nSaveConversations&1)==1;}
	bool GetSaveConversationServer(void) const {return (m_nSaveConversations&2)==2;}
	bool GetAuthContact(void) const {return m_bAuthContact;}
	eb::bigint GetDeployId(void) const {return m_nDeployId;}
	int GetLicenseType(void) const {return m_nLicenstType;}
	//eb::bigint GetGroupMsgSugId(void) const {return m_nGroupMsgSubId;}
	eb::bigint GetAutoOpenSubId(void) const {return m_nAutoOpenSubId;}
	bool GetAutoHideMainFrame(void) const {return m_bAutoHideMainFrame;}
	bool GetHideMainFrame(void) const {return m_bHideMainFrame;}
	bool GetDisableUserCloudDrive(void) const {return m_bDisableUserCloudDrive;}
	bool GetDisableGroupSharedCloud(void) const {return m_bDisableGroupSharedCloud;}
	bool GetDisableModifyPasswd(void) const {return m_bDisableModifyPasswd;}
	const tstring& GetEntManagerurl(void) const {return m_sEntManagerUrl;}
	bool GetDisableVideo(void) const {return m_bDisableVideo;}
	bool GetDisableRD(void) const {return m_bDisableRemoteDesktop;}
	bool GetDisableAccountEdit(void) const {return m_bDisableAccountEdit;}
	EB_BROWSER_TYPE GetDefaultBrowserType(void) const {return m_nDefaultBrowserType;}
	bool GetDisableMsgReceipt(void) const {return m_bDisableMsgReceipt;}
	bool GetStatSubGroupMember(void) const {return m_bStatSubGroupMember;}
	bool GetIeException(void) const {return m_bIeException;}

// Dialog Data
	enum { IDD = IDD_DLG_OAUTH };

protected:
	CString m_sAccountPrefix;
	CString m_sProductName;
	bool m_bLicenseUser;
	bool m_bSendRegMail;
	int m_nSaveConversations;
	bool m_bAuthContact;
	eb::bigint m_nDeployId;
	int m_nLicenstType;	// 许可类型；0=未授权；1=终身授权；2=时间授权
	//eb::bigint m_nGroupMsgSubId;
	eb::bigint m_nAutoOpenSubId;
	bool m_bAutoHideMainFrame;
	bool m_bHideMainFrame;
	bool m_bDisableUserCloudDrive;
	bool m_bDisableGroupSharedCloud;
	bool m_bDisableModifyPasswd;
	tstring m_sEntManagerUrl;
	bool m_bDisableVideo;
	bool m_bDisableRemoteDesktop;
	bool m_bDisableAccountEdit;
	EB_BROWSER_TYPE m_nDefaultBrowserType;
	bool m_bDisableMsgReceipt;
	bool m_bStatSubGroupMember;
	bool m_bIeException;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	static void RunEBC(void);
	//CCefBrowser m_pCefBrowser;

	int m_nWaitCloseIndex;
	bool m_bDocumentComplete;
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

//#ifdef USES_EBCOM_TEST
//	virtual void Fire_onOAuthForward(BSTR sOAuthUrl);
//	virtual void Fire_onLogonSuccess(IEB_AccountInfo * pAccountInfo);
//	virtual void Fire_onLogonTimeout(IEB_AccountInfo * pAccountInfo);
//	virtual void Fire_onLogonError(IEB_AccountInfo * pAccountInfo, EB_STATE_CODE nState);
//	virtual void Fire_onAppIdSuccess(eb::bigint sAppId, const CEBString& sAppOnlineKey);
//#else
//#endif
	LRESULT OnMessageOAuthForward(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageLogonSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageLogonTimeout(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageLogonError(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageAppIdSuccess(WPARAM wParam, LPARAM lParam);
	bool StartClient(void);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CExplorer m_pExplorer;
};

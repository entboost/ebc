#pragma once
#include "afxwin.h"
#include "Tools/TraButton.h"
#include "Tools/VividTree.h"

class CLoginInfo
{
public:
	typedef boost::shared_ptr<CLoginInfo> pointer;
	static CLoginInfo::pointer create(const CString& sAccount, const CString& sPassword, bool bSafePwd)
	{
		return CLoginInfo::pointer(new CLoginInfo(sAccount, sPassword, bSafePwd));
	}

	CLoginInfo(void)
		: m_bSafePwd(false)
	{
	}
	CLoginInfo(const CString& sAccount, const CString& sPassword, bool bSafePwd)
		: m_sAccount(sAccount)
		, m_sPassword(sPassword)
		, m_bSafePwd(bSafePwd)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
		, m_nUserId(0), m_nPhone(0)
		, m_hItem(NULL)
	{
	}
	CString m_sAccount;
	CString m_sPassword;
	bool m_bSafePwd;
	EB_USER_LINE_STATE m_nLineState;
	mycp::bigint m_nUserId;
	mycp::bigint m_nPhone;
	HTREEITEM m_hItem;
};
// CDlgLogin dialog
#define USES_NEW_USER_CTRL

class CDlgLogin : public CEbDialogBase
#ifdef USES_EBCOM_TEST
	, public CEBCoreEventsSink
#endif
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLogin();

	std::string m_sReqCode;
	std::string m_sAppParameter1;		// logout:×¢Ïú
	std::string m_sMainExe;
	CLabelEx m_labelAutoLogin;
	bool m_bReload;

	bool GetAutoLogSuccess(void) const {return m_bAutoLogSuccess;}

// Dialog Data
	enum { IDD = IDD_DLG_LOGIN };

protected:
	eb::bigint m_nDeployId;
	CString m_sProductName;
	CString m_sOrgServer;
	CMenu m_menuState;
	CNewMenu m_menuSkin;
	//CMenu m_menuSkin;
	bool m_bAutoLogSuccess;

	HICON m_hIcon;
	CLockMap<CString, CLoginInfo::pointer> m_pLoginInfoList;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
#ifdef USES_NEW_USER_CTRL
	bool m_bCanSearch;
	CHoverEdit m_editUser;
	CTraButton m_btnDel;
	VividTree m_treeUsers;
#endif
	//CBitComboBox m_comboUsers;
	CHoverEdit	m_editPassword;
	CLabelEx m_labelSavePwd;
	CTraButton m_btnSkin;
	CTraButton m_btnMin;
	CTraButton m_btnClose;
	CTraButton m_btnLineState;
	CTraButton m_btnForgetPwd;
	CTraButton m_btnSetting;
	CTraButton m_btnReg;
	CTraButton m_btnLogin;
	CTraButton m_btnVisitor;
	EB_USER_LINE_STATE m_nOutLineState;
	CLabelEx m_labelError;

	void DrawInfo(const CString & sAdText);
	int GetExistAppCount(void) const;
	bool DownloadHttpFile(const CString & strUrl,const CString& strFile,const CString& strOldLastModified,CString& pOutNewLastModified) const;

//#ifdef USES_EBCOM_TEST
//	virtual void Fire_onOAuthForward(BSTR sOAuthUrl);
//	virtual void Fire_onLogonSuccess(IEB_AccountInfo * pAccountInfo);
//	virtual void Fire_onLogonTimeout(IEB_AccountInfo * pAccountInfo);
//	virtual void Fire_onLogonError(IEB_AccountInfo * pAccountInfo, EB_STATE_CODE nState);
//	virtual void Fire_onAppIdError(EB_STATE_CODE nState);
//	virtual void Fire_onAppIdSuccess(eb::bigint sAppId, const CEBString& sAppOnlineKey);
//#else
//#endif
	LRESULT OnMessageOAuthForward(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageLogonSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageLogonTimeout(WPARAM wParam, LPARAM lParam);
	void ShowError(const CString& sErrorString);
	LRESULT OnMessageLogonError(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageAppIdError(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageAppIdSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageServerChange(WPARAM wParam, LPARAM lParam);
	bool StartClient(void);
	void EbSetWindowText(void);
	void SetCtrlColor(void);

	void ShowTreeCtrl(bool bShow);
	void DeleteItem(HTREEITEM hItem);
	void SelectItem(HTREEITEM hItem, bool bHideUserCtrl=true);

	DECLARE_MESSAGE_MAP()
	LRESULT OnTreeItemSelChange(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);
	afx_msg void OnBnClickedButtonLogin();
	virtual BOOL OnInitDialog();
	CString m_sUserAccount;
	CString m_sUserPassword;
	afx_msg void OnBnClickedButtonVisitor();
	afx_msg void OnBnClickedButtonSetting();
	afx_msg void OnDestroy();
	void MoveSize(int cx, int cy);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnCbnSelchangeComboUsers();
	//afx_msg void OnCbnEditchangeComboUsers();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonReg();
	//afx_msg void OnBnClickedButtonLinestate();
	afx_msg void OnStateOnline();
	afx_msg void OnStateAway();
	afx_msg void OnStateBusy();
	//afx_msg void OnDeleteAccount();
	afx_msg void OnBnClickedButtonSkin();
	afx_msg void OnSkinSelect(UINT nID);
	afx_msg void OnSkinFlat(void);
	afx_msg void OnSkinGradient(void);
	afx_msg void OnServerSetting(void);
	afx_msg void OnStnClickedStaticAutologin();
	afx_msg void OnBnClickedButtonForgetPwd();
	virtual void OnCancel();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMKillfocusTreeUsers(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTreeUsers(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditUsers();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

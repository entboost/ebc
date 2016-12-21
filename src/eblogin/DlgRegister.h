#pragma once

// CDlgRegister dialog
/*
 * 注册窗口类
 * 实现弹出注册窗口，输入注册内容，支持“注册个人帐号”和“注册企业帐号”二种；
 * 
*//////////////////////////////////////////////////////

class CDlgRegister : public CEbDialogBase
#ifdef USES_EBCOM_TEST
	, public CEBCoreEventsSink
#endif

{
	DECLARE_DYNAMIC(CDlgRegister)

public:
	CDlgRegister(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRegister();

	CString m_sAccount;
	CString m_sUserName;
	CString m_sPassword;
	CString m_sPwdConfirm;
	CString m_sEnterpriseName;
// Dialog Data
	enum { IDD = IDD_DLG_REG };

protected:
#ifdef USES_EBCOM_TEST
	IEBClientCorePtr m_pEBAppClient;
#else
	CEBAppClient m_pEBAppClient;
#endif
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit	m_editAccount;
	CHoverEdit	m_editUserName;
	CHoverEdit	m_editPassword;
	CHoverEdit	m_editPwdConfirm;
	CHoverEdit	m_editEnterpriseName;
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;
	//bool m_bIsEntboostServer;

	void DrawInfo(void);
//
//#ifdef USES_EBCOM_TEST
//	virtual void Fire_onRegisterResponse(EB_STATE_CODE nState,eb::bigint nUserId);
//#else
//#endif
	LRESULT OnMessageRegResponse(WPARAM wParam, LPARAM lParam);
	void OnRegResponse(EB_STATE_CODE nState,eb::bigint nUserId);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#pragma once
#include "afxwin.h"
#include "DlgSelectUser.h"


// CDlgGroupInfo dialog

class CDlgGroupInfo : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgGroupInfo)

public:
	CDlgGroupInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGroupInfo();

	eb::bigint m_nGroupId;
	eb::bigint m_nManagerUserId;
	EB_GROUP_TYPE m_nGroupType;
	CString m_sGroupName;
	CString m_sPhone;
	CString m_sFax;
	CString m_sEmail;
	CString m_sUrl;
	CString m_sAddress;
	int m_nDisplayIndex;
	CString m_sDescription;
// Dialog Data
	enum { IDD = IDD_DLG_GROUP_INFO };

protected:
	tstring m_sManagerAccount;
	mycp::bigint m_nManagerEmpId;
	CDlgSelectUser m_pDlgSelectUser;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CLabelEx m_staManagerName;
	CHoverEdit	m_editGroupName;
	CHoverEdit	m_editPhone;
	CHoverEdit	m_editFax;
	CHoverEdit	m_editEmail;
	CHoverEdit	m_editUrl;
	CHoverEdit	m_editAddress;
	CHoverEdit	m_editDisplayIndex;
	CHoverEdit	m_editDescription;
	CTraButton m_btnManager;
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
public:
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonManagerName();
	afx_msg void OnStnClickedStaticManagerName();
};

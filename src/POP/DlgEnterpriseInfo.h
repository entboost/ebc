#pragma once

// CDlgEnterpriseInfo dialog

/*
 * 企业资料信息显示类
 * 用于显示（或修改）企业资料信息。
 * 
*//////////////////////////////////////////////////////

class CDlgEnterpriseInfo : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgEnterpriseInfo)

public:
	CDlgEnterpriseInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEnterpriseInfo();

	CString m_sEnterpriseName;
	CString m_sPhone;
	CString m_sFax;
	CString m_sEmail;
	CString m_sUrl;
	CString m_sAddress;
	CString m_sDescription;
// Dialog Data
	enum { IDD = IDD_DLG_ENTERPRISE_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit	m_editEnterpriseName;
	CHoverEdit	m_editPhone;
	CHoverEdit	m_editFax;
	CHoverEdit	m_editEmail;
	CHoverEdit	m_editUrl;
	CHoverEdit	m_editAddress;
	CHoverEdit	m_editDescription;
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnPaint();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonClose();
};

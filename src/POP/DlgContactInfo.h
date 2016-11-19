#pragma once
#include "afxwin.h"


// CDlgContactInfo dialog
/*
 * 联系人窗口类
 * 用于显示（或修改）联系人资料。
 * 
*//////////////////////////////////////////////////////

class CDlgContactInfo : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgContactInfo)

public:
	CDlgContactInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgContactInfo();

	//eb::bigint m_nContactId;
	eb::bigint m_nContactUserId;
	CString m_sContact;
	eb::bigint m_nUGId;
	CString m_sName;
	CString m_sPhone;
	CString m_sCompany;
	CString m_sJobTitle;
	CString m_sUrl;
	CString m_sTel;
	CString m_sFax;
	CString m_sEmail;
	CString m_sAddress;
	CString m_sDescription;

// Dialog Data
	enum { IDD = IDD_DLG_CONTACT_INFO };

protected:
	CLockMap<int,eb::bigint> m_pUGInfoList;		// item->ugid
	//CLockMap<int,eb::bigint> m_pContactList;	// item->con_id
	//CLockMap<int,std::string> m_pContactList;	// item->contact
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit	m_editContact;
	CHoverEdit	m_editName;
	CHoverEdit	m_editPhone;
	CHoverEdit	m_editCompany;
	CHoverEdit	m_editJobTitle;
	CHoverEdit	m_editUrl;
	CHoverEdit	m_editTel;
	CHoverEdit	m_editFax;
	CHoverEdit	m_editEmail;
	CHoverEdit	m_editAddress;
	CHoverEdit	m_editDescription;
	//CBitComboBox m_comboContact;
	CBitComboBox m_comboGroup;
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;

//#ifdef USES_EBCOM_TEST
//	void Data2Ctrl(IEB_ContactInfo* pPopContactInfo);
//#else
//	void Data2Ctrl(const EB_ContactInfo* pPopContactInfo);
//#endif

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	//afx_msg void OnCbnSelchangeComboContact();
	//afx_msg void OnCbnEditchangeComboContact();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

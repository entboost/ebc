#pragma once


// CDlgRootPassword dialog

class CDlgRootPassword : public CDialog
{
	DECLARE_DYNAMIC(CDlgRootPassword)

public:
	CDlgRootPassword(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRootPassword();

// Dialog Data
	enum { IDD = IDD_DLG_MYSQL_ROOTPWD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_sRootPassword;
};

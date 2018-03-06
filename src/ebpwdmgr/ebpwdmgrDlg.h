
// ebpwdmgrDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CebpwdmgrDlg dialog
class CebpwdmgrDlg : public CDialog
{
// Construction
public:
	CebpwdmgrDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EBPWDMGR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_comboDBType;
	CString m_sDbIp;
	int m_nDbPort;
	CString m_sDbName;
	CString m_sDbAccount;
	CString m_sDbSecure;
	CComboBox m_comboPWDMode;

public:
	afx_msg void OnBnClickedButtonSetBinPath();
	afx_msg void OnBnClickedBtnTestConnect();
	afx_msg void OnBnClickedBtnChangePwd();
private:
	CString m_sNewSecure;
};

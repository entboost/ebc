// ebs_config_winDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <vector>


// Cebs_config_winDlg dialog
class Cebs_config_winDlg : public CDialog
{
// Construction
public:
	Cebs_config_winDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EBS_CONFIG_WIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	static void WinExecX(const char * lpszFile,const char* lpParameters,UINT uCmdShow,const char* lpDirectory=NULL);

// Implementation
protected:
	HICON m_hIcon;
	bool m_bIntegrateDb;	// ¼¯³É pgsql
	bool m_bGetRootPassword;
	CString m_sRootPassword;
	std::vector<std::string> m_pLocalHostIp;
	bool IsLocalIp(const char* sIp);

	void GetRootPassword(void);
	bool CheckRunProgram(const CString& sProgramName) const;
	void InstallDbFirst(void);
	// pgsql
	void RebuildPgBatPath(const CString& sRebuildFile);
	void RebuildPgHBAFile(void);
	// mysql
	void RebuildMySQLPath(const CString& sDBPath);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int m_nHttpPort;
	int m_nHttpsPort;
	int m_nEbPort;
	CString m_sDbName;
	CString m_sDbIp;
	int m_nDbPort;
	CString m_sDbAccount;
	CString m_sDbSecure;
	CString m_sDbBackupFile;
	CString m_sDbBackupPath;
	//afx_msg void OnBnClickedButtonSaveCurpath();
	afx_msg void OnBnClickedButtonSaveHttpport();
	bool SaveDbinfo(void);
	afx_msg void OnBnClickedButtonSaveDbinfo();
	CString m_sServerAddress;
	bool SaveClientInfo(bool bChangeIpOnly);
	void TarClientZipFile(void);
	afx_msg void OnBnClickedButtonSaveClientinfo();
	afx_msg void OnBnClickedButtonNewDbaccount();
	CString m_sDBDir;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg void OnBnClickedButtonSetPgBinPath();
	afx_msg void OnBnClickedButtonSetBackupFile();
	CString m_sCompanyName;
	CString m_sAdminAccount;
	CString m_sAdminSecure;
	bool InitDb(bool bChangeIpOnly);
	afx_msg void OnBnClickedButtonInitDb();
	CString m_sTel;
	afx_msg void OnBnClickedButtonSaveCsinfo();
	CComboBox m_comboDBType;
	afx_msg void OnCbnSelchangeComboDbtype();
	afx_msg void OnEnChangeEditDbAccount();
	afx_msg void OnBnClickedButtonDbTest();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedButtonChangeIp();
	afx_msg void OnBnClickedCheckAutoChangeip();
	BOOL m_bAutoChangeIp;
	CComboBox m_pServerIpList;
	//CString m_sServerIp;
};

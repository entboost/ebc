
// ebpwdmgrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ebpwdmgr.h"
#include "ebpwdmgrDlg.h"
#include <vector>
#include "../include/md5.h"
//#include "../include/rsa.h"
#include "../include/pbkdf2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CebpwdmgrDlg dialog




CebpwdmgrDlg::CebpwdmgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CebpwdmgrDlg::IDD, pParent)
	, m_sDbName(_T(""))
	, m_sDbIp(_T(""))
	, m_nDbPort(0)
	, m_sDbAccount(_T(""))
	, m_sDbSecure(_T(""))
	, m_sNewSecure(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CebpwdmgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DBTYPE, m_comboDBType);
	DDX_Text(pDX, IDC_EDIT_DB_IP, m_sDbIp);
	DDX_Text(pDX, IDC_EDIT_DB_PORT, m_nDbPort);
	DDX_Text(pDX, IDC_EDIT_DB_NAME, m_sDbName);
	DDX_Text(pDX, IDC_EDIT_DB_ACCOUNT, m_sDbAccount);
	DDX_Text(pDX, IDC_EDIT_DB_SECURE, m_sDbSecure);
	DDX_Control(pDX, IDC_COMBO_PWDMODE, m_comboPWDMode);
	DDX_Text(pDX, IDC_EDIT_NEW_SECURE, m_sNewSecure);
}

BEGIN_MESSAGE_MAP(CebpwdmgrDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SET_BIN_PATH, &CebpwdmgrDlg::OnBnClickedButtonSetBinPath)
	ON_BN_CLICKED(IDC_BTN_TEST_CONNECT, &CebpwdmgrDlg::OnBnClickedBtnTestConnect)
	ON_BN_CLICKED(IDC_BTN_CHANGE_PWD, &CebpwdmgrDlg::OnBnClickedBtnChangePwd)
END_MESSAGE_MAP()


// CebpwdmgrDlg message handlers

BOOL CebpwdmgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_comboDBType.AddString(_T("PostgreSQL"));
	m_comboDBType.AddString(_T("MySQL"));
	//m_comboDBType.AddString(_T("恩布内置数据库（SQLite）"));
	//m_comboDBType.SetCurSel(0);
	m_comboDBType.EnableWindow(FALSE);
	m_comboPWDMode.AddString(_T("默认恩布验证方式"));
	m_comboPWDMode.AddString(_T("MD5验证方式"));
	m_comboPWDMode.SetCurSel(0);

	//// postgresql test data
	//this->GetDlgItem(IDC_STATIC_DB_BINPATH)->SetWindowText(_T("F:\\git\\oschina\\entboost-1.16.0-win-pgsql-w32\\pgsql\\bin"));
	//m_comboDBType.SetCurSel(0);
	//m_sDbIp = _T("114.215.181.249");
	//m_nDbPort = 5432;
	//m_sDbName = _T("entboost");
	//m_sDbAccount = _T("postgres");
	//m_sDbSecure = _T("postgres1");
	//UpdateData(FALSE);

	//// mysql test data
	//this->GetDlgItem(IDC_STATIC_DB_BINPATH)->SetWindowText(_T("F:\\git\\oschina\\entboost-1.5.0-win-mysql-x64\\mysql-5.6.20-winx64\\bin"));
	//m_comboDBType.SetCurSel(1);
	//m_sDbIp = _T("localhost");
	//m_nDbPort = 3306;
	//m_sDbName = _T("entboost");
	//m_sDbAccount = _T("root");
	//m_sDbSecure = _T("");
	//UpdateData(FALSE);

	// mysql test data
	//this->GetDlgItem(IDC_STATIC_DB_BINPATH)->SetWindowText(_T("F:\\git\\oschina\\entboost-1.5.0-win-mysql-x64\\mysql-5.6.20-winx64\\bin"));
	//m_comboDBType.SetCurSel(1);
	//m_sDbIp = _T("192.168.1.198");
	//m_nDbPort = 3306;
	//m_sDbName = _T("entboost1.12");
	//m_sDbAccount = _T("root");
	//m_sDbSecure = _T("mysql");
	//m_sNewSecure = _T("111111");
	//UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CebpwdmgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CebpwdmgrDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CebpwdmgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString GetBrowseFolder(HWND pParent,const CString& wndTitle)
{
    char        szDir[MAX_PATH] = {0};
    BROWSEINFO    bi;
    ITEMIDLIST    *pidl = NULL;

    bi.hwndOwner = pParent;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szDir;
    bi.lpszTitle = wndTitle;
    bi.ulFlags = BIF_RETURNONLYFSDIRS;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;

    pidl = SHBrowseForFolder(&bi);
    if(NULL == pidl)
        return "";
    if(!SHGetPathFromIDList(pidl,szDir))  
        return "";

    return CString(szDir);
}
void CebpwdmgrDlg::OnBnClickedButtonSetBinPath()
{
	const CString sDBBinDir = GetBrowseFolder(this->GetSafeHwnd(),_T("请选择PostgreSQL或MySQL数据库bin目录："));
	if (sDBBinDir.IsEmpty())
		return;
	//CString sDbFilePath;
	if (::PathFileExists(sDBBinDir+_T("\\createuser.exe")))
	{
		m_comboDBType.SetCurSel(0);
		//sDbFilePath = sDBBinDir+_T("\\createuser.exe");
		m_nDbPort = 5432;
	}else if (::PathFileExists(sDBBinDir+_T("\\mysql.exe")))
	{
		m_comboDBType.SetCurSel(1);
		//sDbFilePath = sDBBinDir+_T("\\mysql.exe");
		m_nDbPort = 3306;
	//}else if (::PathFileExists(sDBBinDir+_T("\\sqlite3.exe")))
	//{
	//	m_comboDBType.SetCurSel(2);
	//	//sDbFilePath = sDBBinDir+_T("\\mysql.exe");
	//	//m_nDbPort = 3306;
	}else
	{
		CString stext;
		stext.Format(_T("错误数据库目录，请重新选择！"));
		AfxMessageBox(stext);
		return;
	}
	this->GetDlgItem(IDC_STATIC_DB_BINPATH)->SetWindowText(sDBBinDir);
	//this->GetDlgItem(IDC_STATIC_DB_BINPATH)->SetWindowText(sDbFilePath);
	UpdateData(FALSE);
	this->GetDlgItem(IDC_EDIT_DB_IP)->SetFocus();
}

void WinExecX(const char * lpszFile,const char* lpParameters,UINT uCmdShow,const char* lpDirectory)
{
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	//ShExecInfo.hwnd = ::GetDesktopWindow();	// 不种子,可以用
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpFile = lpszFile;
	ShExecInfo.lpParameters = lpParameters;
	ShExecInfo.lpDirectory = lpDirectory;
	ShExecInfo.nShow = uCmdShow;
	ShExecInfo.hInstApp = NULL;	
	if (ShellExecuteEx(&ShExecInfo))
	{
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	}
}

BOOL ExecDosCmd(LPCSTR sAppName,LPSTR sCommandLine,CString& sOutString,bool bShow=true)
{
	SECURITY_ATTRIBUTES sa;  
	HANDLE hRead,hWrite;  
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);  
	sa.lpSecurityDescriptor = NULL;  
	sa.bInheritHandle = TRUE;  
	if (!CreatePipe(&hRead,&hWrite,&sa,0))   
	{  
		return FALSE;  
	}

	STARTUPINFO si;  
	PROCESS_INFORMATION pi;   
	si.cb = sizeof(STARTUPINFO);  
	GetStartupInfo(&si);   
	si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入  
	si.hStdOutput = hWrite;           //把创建进程的标准输出重定向到管道输入  
	si.wShowWindow = bShow?SW_SHOW:SW_HIDE;  
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;  
	//关键步骤，CreateProcess函数参数意义请查阅MSDN  
	if (!CreateProcess(sAppName, sCommandLine,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))   
	{  
		CloseHandle(hWrite);  
		CloseHandle(hRead);  
		return FALSE;  
	}  
	CloseHandle(hWrite);  
	char buffer[4096] = {0};          //用4K的空间来存储输出的内容，只要不是显示文件内容，一般情况下是够用了。  
	DWORD bytesRead;   
	while (true)   
	{  
		memset(buffer,0,sizeof(buffer));
		if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)  
			break;
		sOutString.Append(buffer);
		//sOutString = buffer;
		//buffer中就是执行的结果，可以保存到文本，也可以直接输出  
		//AfxMessageBox(buffer);   //这里是弹出对话框显示  
	}  
	CloseHandle(hRead);   
	return TRUE;
}  

void CebpwdmgrDlg::OnBnClickedBtnTestConnect()
{
	if (m_comboDBType.GetCurSel()<0)
	{
		AfxMessageBox(_T("请先选择数据库bin目录！"));
		return;
	}
	UpdateData();
	if (m_sDbIp.IsEmpty())
	{
		AfxMessageBox(_T("请输入数据库地址！"));
		this->GetDlgItem(IDC_EDIT_DB_IP)->SetFocus();
		return;
	}else if (m_nDbPort<=0)
	{
		AfxMessageBox(_T("请输入数据库端口！"));
		this->GetDlgItem(IDC_EDIT_DB_PORT)->SetFocus();
		return;
	}else if (m_sDbName.IsEmpty())
	{
		AfxMessageBox(_T("请输入数据库名称！"));
		this->GetDlgItem(IDC_EDIT_DB_NAME)->SetFocus();
		return;
	}else if (m_sDbAccount.IsEmpty())
	{
		AfxMessageBox(_T("请输入数据库帐号！"));
		this->GetDlgItem(IDC_EDIT_DB_ACCOUNT)->SetFocus();
		return;
	//}else if (m_sDbSecure.IsEmpty())
	//{
	//	AfxMessageBox(_T("请输入数据库密码！"));
	//	this->GetDlgItem(IDC_EDIT_DB_SECURE)->SetFocus();
	//	return;
	}

	CString sDBBinDir;
	this->GetDlgItem(IDC_STATIC_DB_BINPATH)->GetWindowText(sDBBinDir);
	const int nDBType = m_comboDBType.GetCurSel();
	if (nDBType==0)	// pgsql
	{
		const CString sSqlExe = sDBBinDir+_T("\\psql.exe");
		if (!::PathFileExists(sSqlExe))
		{
			CString stext;
			stext.Format(_T("%s文件不存在，请重新设置PostgreSQL数据库bin目录！"),sSqlExe);
			AfxMessageBox(stext);
			return;
		}

		CString sSQL = _T("SELECT user_id FROM user_account_t WHERE user_id=80;");
		char lpszCommand[1024*5];
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		else
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --password --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		CString sString;
		ExecDosCmd(NULL,lpszCommand,sString);
		if (sString.Find(_T("user_id"))>=0 && sString.Find(_T("80"),5)>0)
		{
			AfxMessageBox(_T("数据库连接测试成功！"));
		}else
		{
			AfxMessageBox(sString);
			//AfxMessageBox(_T("数据库连接测试失败！"));
		}
	}else if (nDBType==1)	// mysql
	{
		const CString sSqlExe = sDBBinDir+_T("\\mysql.exe");
		if (!::PathFileExists(sSqlExe))
		{
			CString stext;
			stext.Format(_T("%s文件不存在，请重新设置MySQL数据库bin目录！"),sSqlExe);
			AfxMessageBox(stext);
			return;
		}

		const CString sSQL = _T("SELECT user_id FROM user_account_t WHERE user_id=80");
		char lpszCommand[1024*5];
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s -uroot -h%s -D%s -e\"%s\"",sSqlExe,m_sDbIp,m_sDbName,sSQL);
		else
			sprintf(lpszCommand,"%s -uroot -p%s -h%s -D%s -e\"%s\"",sSqlExe,m_sDbSecure,m_sDbIp,m_sDbName,sSQL);
		CString sString;
		ExecDosCmd(NULL,lpszCommand,sString,false);
		if (sString.Find(_T("user_id"))>=0 && sString.Find(_T("80"),5)>0)
		{
			AfxMessageBox(_T("数据库连接测试成功！"));
		}else
		{
			AfxMessageBox(sString);
			//AfxMessageBox(_T("数据库连接测试失败！"));
		}
	}else
	{
		// ???

	}
}

#define EB_SYS_EXT_KEY_PASSWD_AUTH_MODE 26
#define EB_SYS_EXT_KEY_ENCRYPTION_SALT	57
#define EB_SYS_EXT_KEY_PBKDF2_COUNT		58
#define EB_SYS_EXT_KEY_PBKDF2_LENGTH	59

int ParseString(const char * lpszString, const char * lpszInterval, std::vector<std::string> & pOut)
{
	std::string sIn(lpszString);
	pOut.clear();
	while (!sIn.empty())
	{
		std::string::size_type find = sIn.find(lpszInterval);
		if (find == std::string::npos)
		{
			pOut.push_back(sIn);
			break;
		}
		if (find==0)
			pOut.push_back("");	// 空
		else
			pOut.push_back(sIn.substr(0, find));
		sIn = sIn.substr(find+strlen(lpszInterval));
	}
	return (int)pOut.size();
}
inline std::string GetSaltString(void)
{
	char lpszBuffer[64];
	const int nRandValue1 = GetTickCount();
	int nRandValue2 = 0;
	for (int i=0;i<(nRandValue1%100);i++)
	{
		nRandValue2 += rand();
	}
	sprintf(lpszBuffer,"%d%d%d%d",(int)time(0),nRandValue1,nRandValue2,(int)GetTickCount());
	//sprintf(lpszBuffer,"111111");
	entboost::MD5 md5;
	md5.update((const unsigned char*)lpszBuffer,strlen(lpszBuffer));
	md5.finalize();
	return md5.hex_digest();
}

bool ExecCmdPgSql(LPSTR lpszCommand,CString& pOutString)
{
//            sys_value             
//----------------------------------
// C74026FC36070A8C22E9E549F98E5911
//(1 行记录)
	CString sString;
	ExecDosCmd(NULL,lpszCommand,sString);
	if (sString.IsEmpty())
	{
		//AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		return false;
	}else if (sString.Find(_T("sys_value"))>=0)
	{
		std::vector<std::string> pList;
		if (ParseString(sString,"\r\n",pList)==0)
			ParseString(sString,"\n",pList);
		if (pList.size()!=5)
		{
			//AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
			pOutString = sString;
			return false;
		}
		pOutString = pList[2].c_str();
		pOutString.Trim();
		//if (m_sEncryptionSalt.GetLength()!=32)
		//{
		//	AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		//	return;
		//}
		return true;
	}else
	{
		pOutString = sString;
		//AfxMessageBox(sString);
		return false;
	}
}
bool ExecCmdMySql(LPSTR lpszCommand,CString& pOutString)
{
	CString sString;
	ExecDosCmd(NULL,lpszCommand,sString,false);
	if (sString.IsEmpty())
	{
		//AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		return false;
	}else if (sString.Find(_T("sys_value"))>=0)
	{
//Warning: Using a password on the command line interface can be insecure.
//sys_value
//A192553B017A74A9EFFE50475E2235C1

		std::vector<std::string> pList;
		if (ParseString(sString,"\r\n",pList)==0)
			ParseString(sString,"\n",pList);
		if (pList.size()<2)	// 支持5.0,5.1
		{
			pOutString = sString;
			//AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
			return false;
		}
		pOutString = pList[1].c_str();
		pOutString.Trim();
		if (pOutString=="sys_value" && pList.size()>=3)
		{
			pOutString = pList[2].c_str();
			pOutString.Trim();
		}
		//if (m_sEncryptionSalt.GetLength()!=32)
		//{
		//	AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		//	return;
		//}
		return true;
	}else
	{
		pOutString = sString;
		return false;
	}
}

void CebpwdmgrDlg::OnBnClickedBtnChangePwd()
{
	//unsigned long x = 10;
	//int y = 90;
	//unsigned long n1 = (x << y);
	//unsigned long n2 = (x >> -y);
	//CString stext;
	//stext.Format(_T("%d-%d"),n1,n2);
	//AfxMessageBox(stext);
	//return;
	//CPBKDF2 pbkdf;
	////pbkdf.SHA1("1234567890123456",16);
	//pbkdf.HMAC("1234567890123456",16,"1234567890123456",16);
	//AfxMessageBox(pbkdf.GetHString().c_str());
	//return;

	if (m_comboDBType.GetCurSel()<0)
	{
		AfxMessageBox(_T("请先选择数据库bin目录！"));
		return;
	}
	UpdateData();
	if (m_sDbIp.IsEmpty())
	{
		AfxMessageBox(_T("请输入数据库地址！"));
		this->GetDlgItem(IDC_EDIT_DB_IP)->SetFocus();
		return;
	}else if (m_nDbPort<=0)
	{
		AfxMessageBox(_T("请输入数据库端口！"));
		this->GetDlgItem(IDC_EDIT_DB_PORT)->SetFocus();
		return;
	}else if (m_sDbName.IsEmpty())
	{
		AfxMessageBox(_T("请输入数据库名称！"));
		this->GetDlgItem(IDC_EDIT_DB_NAME)->SetFocus();
		return;
	}else if (m_sDbAccount.IsEmpty())
	{
		AfxMessageBox(_T("请输入数据库帐号！"));
		this->GetDlgItem(IDC_EDIT_DB_ACCOUNT)->SetFocus();
		return;
	}else if (m_sNewSecure.IsEmpty())
	{
		AfxMessageBox(_T("请输入管理员新密码！"));
		this->GetDlgItem(IDC_EDIT_NEW_SECURE)->SetFocus();
		return;
	}else if (m_sNewSecure.GetLength()<6)
	{
		AfxMessageBox(_T("管理员密码必须六位以上，请重新输入！"));
		this->GetDlgItem(IDC_EDIT_NEW_SECURE)->SetFocus();
		return;
	}

	// 先计算出原始密码
	entboost::MD5 md5;
	char lpszBuffer[128];
	if (m_comboPWDMode.GetCurSel()==1)	// MD5验证方式
		sprintf(lpszBuffer,"%s",m_sNewSecure);
	else
		sprintf(lpszBuffer,"80%s",m_sNewSecure);
	md5.update((const unsigned char*)lpszBuffer,strlen(lpszBuffer));
	md5.finalize();
	const std::string sPwdU = md5.hex_digest();
	//AfxMessageBox(sPwdU.c_str());

	char lpszCommand[1024*5];

	int m_nPBKDF2Count = 1;
	int m_nPBKDF2Length = 32;
	CString m_sEncryptionSalt;
	CString sSQL;
	CString sDBBinDir;
	this->GetDlgItem(IDC_STATIC_DB_BINPATH)->GetWindowText(sDBBinDir);
	const int nDBType = m_comboDBType.GetCurSel();
	if (nDBType==0)	// pgsql
	{
		const CString sSqlExe = sDBBinDir+_T("\\psql.exe");
		if (!::PathFileExists(sSqlExe))
		{
			CString stext;
			stext.Format(_T("%s文件不存在，请重新设置PostgreSQL数据库bin目录！"),sSqlExe);
			AfxMessageBox(stext);
			return;
		}

		// 先取出系统密码盐
		//sSQL.Format(_T("SELECT sys_value FROM eb_system_info_t WHERE (sys_key=%d OR sys_key=%d AND sys_key=%d AND sys_key=%d) AND enable=1 ORDER BY sys_key;"),
		//	(int)EB_SYS_EXT_KEY_ENCRYPTION_SALT,(int)EB_SYS_EXT_KEY_PASSWD_AUTH_MODE,(int)EB_SYS_EXT_KEY_PBKDF2_COUNT,(int)EB_SYS_EXT_KEY_PBKDF2_LENGTH);
		sSQL.Format(_T("SELECT sys_value FROM eb_system_info_t WHERE sys_key=%d AND enable=1 AND length(sys_value)>0;"),(int)EB_SYS_EXT_KEY_ENCRYPTION_SALT);
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		else
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --password --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		if (!ExecCmdPgSql(lpszCommand,m_sEncryptionSalt))
		{
			if (!m_sEncryptionSalt.IsEmpty())
				AfxMessageBox(m_sEncryptionSalt);
			else
				AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
			return;
		}
		if (m_sEncryptionSalt.GetLength()!=32)
		{
			AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
			return;
		}
		//CString sString;
		//ExecDosCmd(NULL,lpszCommand,sString);
		//if (sString.IsEmpty())
		//{
		//	AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		//	return;
		//}else if (sString.Find(_T("sys_value"))>=0)
		//{
		//	std::vector<std::string> pList;
		//	if (ParseString(sString,"\r\n",pList)==0)
		//		ParseString(sString,"\n",pList);
		//	if (pList.size()!=5)
		//	{
		//		AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		//		return;
		//	}
		//	m_sEncryptionSalt = pList[2].c_str();
		//	m_sEncryptionSalt.Trim();
		//	if (m_sEncryptionSalt.GetLength()!=32)
		//	{
		//		AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		//		return;
		//	}
		//}else
		//{
		//	AfxMessageBox(sString);
		//	return;
		//}

		// 取出系统加密类型
		sSQL.Format(_T("SELECT sys_value FROM eb_system_info_t WHERE sys_key=%d AND enable=1;"),(int)EB_SYS_EXT_KEY_PASSWD_AUTH_MODE);
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		else
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --password --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		int m_nPasswdAuthMode = 0;
		CString sString;
		if (ExecCmdPgSql(lpszCommand,sString) && !sString.IsEmpty())
		{
			m_nPasswdAuthMode = atoi(sString);
		}
		// 取出计算次数
		sSQL.Format(_T("SELECT sys_value FROM eb_system_info_t WHERE sys_key=%d AND enable=1;"),(int)EB_SYS_EXT_KEY_PBKDF2_COUNT);
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		else
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --password --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		sString = "";
		if (ExecCmdPgSql(lpszCommand,sString) && !sString.IsEmpty())
		{
			m_nPBKDF2Count = atoi(sString);
		}
		// 取出生成字节
		sSQL.Format(_T("SELECT sys_value FROM eb_system_info_t WHERE sys_key=%d AND enable=1;"),(int)EB_SYS_EXT_KEY_PBKDF2_LENGTH);
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		else
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --password --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		sString = "";
		if (ExecCmdPgSql(lpszCommand,sString) && !sString.IsEmpty())
		{
			m_nPBKDF2Length = atoi(sString);
		}

		// 生成数据库安全密码；
		CPBKDF2 pbkdf2;
		const std::string sUserPwdSalt = GetSaltString();
		if (m_nPasswdAuthMode==2)
			sprintf(lpszBuffer,"%s",sUserPwdSalt.c_str());
		else
			sprintf(lpszBuffer,"%s%s",m_sEncryptionSalt,sUserPwdSalt.c_str());
		const std::string sRealPassword = pbkdf2.PBKDF2(sPwdU.c_str(),sPwdU.size(),lpszBuffer,strlen(lpszBuffer),m_nPBKDF2Count,m_nPBKDF2Length);

		sString = "";
		sSQL.Format(_T("UPDATE user_account_t SET pwd_s='%s',pwd_u='%s' WHERE user_id=80"),sUserPwdSalt.c_str(),sRealPassword.c_str());
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		else
			sprintf(lpszCommand,"%s --host %s --port %d --username %s --password --command \"%s\" %s",sSqlExe,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		ExecDosCmd(NULL,lpszCommand,sString);
		if (sString.Find(_T("UPDATE 1"))>=0)
		{
			AfxMessageBox(_T("密码修改成功！"));
		}else
		{
			AfxMessageBox(sString);
		}
	}else if (nDBType==1)	// mysql
	{
		const CString sSqlExe = sDBBinDir+_T("\\mysql.exe");
		if (!::PathFileExists(sSqlExe))
		{
			CString stext;
			stext.Format(_T("%s文件不存在，请重新设置MySQL数据库bin目录！"),sSqlExe);
			AfxMessageBox(stext);
			return;
		}

		// 先取出系统密码盐
		sSQL.Format(_T("SELECT sys_value FROM eb_system_info_t WHERE sys_key=%d AND enable=1 AND length(sys_value)>0;"),(int)EB_SYS_EXT_KEY_ENCRYPTION_SALT);
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s -uroot -h%s -D%s -e\"%s\"",sSqlExe,m_sDbIp,m_sDbName,sSQL);
		else
			sprintf(lpszCommand,"%s -uroot -p%s -h%s -D%s -e\"%s\"",sSqlExe,m_sDbSecure,m_sDbIp,m_sDbName,sSQL);
		if (!ExecCmdMySql(lpszCommand,m_sEncryptionSalt))
		{
			if (!m_sEncryptionSalt.IsEmpty())
				AfxMessageBox(m_sEncryptionSalt);
			else
				AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
			return;
		}
		if (m_sEncryptionSalt.GetLength()!=32)
		{
			AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
			return;
		}
		//CString sString;
		//ExecDosCmd(NULL,lpszCommand,sString,false);
		//if (sString.IsEmpty())
		//{
		//	AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		//	return;
		//}else if (sString.Find(_T("sys_value"))>=0)
		//{
		//	std::vector<std::string> pList;
		//	if (ParseString(sString,"\r\n",pList)==0)
		//		ParseString(sString,"\n",pList);
		//	if (pList.size()<2)	// 支持5.0,5.1
		//	{
		//		AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		//		return;
		//	}
		//	m_sEncryptionSalt = pList[1].c_str();
		//	m_sEncryptionSalt.Trim();
		//	if (m_sEncryptionSalt.GetLength()!=32)
		//	{
		//		AfxMessageBox(_T("系统未安装成功，请安装成功后再试！"));
		//		return;
		//	}
		//}else
		//{
		//	AfxMessageBox(sString);
		//	return;
		//}
		// 取出系统验证类型
		sSQL.Format(_T("SELECT sys_value FROM eb_system_info_t WHERE sys_key=%d AND enable=1;"),(int)EB_SYS_EXT_KEY_PASSWD_AUTH_MODE);
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s -uroot -h%s -D%s -e\"%s\"",sSqlExe,m_sDbIp,m_sDbName,sSQL);
		else
			sprintf(lpszCommand,"%s -uroot -p%s -h%s -D%s -e\"%s\"",sSqlExe,m_sDbSecure,m_sDbIp,m_sDbName,sSQL);
		int m_nPasswdAuthMode = 0;
		CString sString;
		if (ExecCmdMySql(lpszCommand,sString) && !sString.IsEmpty())
		{
			m_nPasswdAuthMode = atoi(sString);
		}
		// 取出系统验证次数
		sSQL.Format(_T("SELECT sys_value FROM eb_system_info_t WHERE sys_key=%d AND enable=1;"),(int)EB_SYS_EXT_KEY_PBKDF2_COUNT);
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s -uroot -h%s -D%s -e\"%s\"",sSqlExe,m_sDbIp,m_sDbName,sSQL);
		else
			sprintf(lpszCommand,"%s -uroot -p%s -h%s -D%s -e\"%s\"",sSqlExe,m_sDbSecure,m_sDbIp,m_sDbName,sSQL);
		if (ExecCmdMySql(lpszCommand,sString) && !sString.IsEmpty())
		{
			m_nPBKDF2Count = atoi(sString);
		}
		// 取出系统验证字节
		sSQL.Format(_T("SELECT sys_value FROM eb_system_info_t WHERE sys_key=%d AND enable=1;"),(int)EB_SYS_EXT_KEY_PBKDF2_LENGTH);
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s -uroot -h%s -D%s -e\"%s\"",sSqlExe,m_sDbIp,m_sDbName,sSQL);
		else
			sprintf(lpszCommand,"%s -uroot -p%s -h%s -D%s -e\"%s\"",sSqlExe,m_sDbSecure,m_sDbIp,m_sDbName,sSQL);
		if (ExecCmdMySql(lpszCommand,sString) && !sString.IsEmpty())
		{
			m_nPBKDF2Length = atoi(sString);
		}

		// 生成数据库安全密码；
		CPBKDF2 pbkdf2;
		const std::string sUserPwdSalt = GetSaltString();
		if (m_nPasswdAuthMode==2)
			sprintf(lpszBuffer,"%s",sUserPwdSalt.c_str());
		else
			sprintf(lpszBuffer,"%s%s",m_sEncryptionSalt,sUserPwdSalt.c_str());
		const std::string sRealPassword = pbkdf2.PBKDF2(sPwdU.c_str(),sPwdU.size(),lpszBuffer,strlen(lpszBuffer),m_nPBKDF2Count,m_nPBKDF2Length);
		//AfxMessageBox(sRealPassword.c_str());

		sString = "";
		sSQL.Format(_T("UPDATE user_account_t SET pwd_s='%s',pwd_u='%s' WHERE user_id=80"),sUserPwdSalt.c_str(),sRealPassword.c_str());
		if (m_sDbSecure.IsEmpty())
			sprintf(lpszCommand,"%s -uroot -h%s -D%s -e\"%s\"",sSqlExe,m_sDbIp,m_sDbName,sSQL);
		else
			sprintf(lpszCommand,"%s -uroot -p%s -h%s -D%s -e\"%s\"",sSqlExe,m_sDbSecure,m_sDbIp,m_sDbName,sSQL);
		ExecDosCmd(NULL,lpszCommand,sString,false);
		if (sString.IsEmpty() || sString.Find(_T("Warning:"))>=0 || sString.Find(_T("UPDATE 1"))>=0)
		{
			AfxMessageBox(_T("密码修改成功！"));
		}else
		{
			AfxMessageBox(sString);
		}
	}else
	{
		// ???

	}
}

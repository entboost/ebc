// ebs_config_winDlg.cpp : implementation file
//
#include "stdafx.h"
#include "ebs_config_win.h"
#include "ebs_config_winDlg.h"
#include <CGCBase/cgcSmartString.h>
using namespace mycp;
//#include <string>
#include "../include/md5.h"
#include "tlhelp32.h"
#include "DlgRootPassword.h"
#include <stl/lockmap.h>

#define USES_NEW_RESOURCE_INFO

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const __int64 DEFAULT_900_NUMBER = 9009301234;
#define TIMER_FIND_DB_TYPE		110

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


// Cebs_config_winDlg dialog

inline std::string str_convert(const char * strSource, int sourceCodepage, int targetCodepage)
{
	int unicodeLen = MultiByteToWideChar(sourceCodepage, 0, strSource, -1, NULL, 0);
	if (unicodeLen <= 0) return "";

	wchar_t * pUnicode = new wchar_t[unicodeLen];
	memset(pUnicode,0,(unicodeLen)*sizeof(wchar_t));
	MultiByteToWideChar(sourceCodepage, 0, strSource, -1, (wchar_t*)pUnicode, unicodeLen);

	char * pTargetData = 0;
	int targetLen = WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);
	if (targetLen <= 0)
	{
		delete[] pUnicode;
		return "";
	}

	pTargetData = new char[targetLen];
	memset(pTargetData, 0, targetLen);

	WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char *)pTargetData, targetLen, NULL, NULL);

	std::string result(pTargetData);
	//	tstring result(pTargetData, targetLen);
	delete[] pUnicode;
	delete[] pTargetData;
	return   result;
}
inline std::string ACP2UTF8(const char* sString)
{
	return str_convert(sString,CP_ACP,CP_UTF8);
}
inline std::string UTF82ACP(const char* sString)
{
	return str_convert(sString,CP_UTF8,CP_ACP);
}

inline std::string GetHostIp(const char * lpszHostName,const char* lpszDefault)
{
	struct hostent *host_entry;
	//struct sockaddr_in addr;
	/* 即要解析的域名或主机名 */
	host_entry=gethostbyname(lpszHostName);
	//printf("%s\n", dn_or_ip);
	char lpszIpAddress[50];
	memset(lpszIpAddress, 0, sizeof(lpszIpAddress));
	if (host_entry!=0)
	{
		//printf("解析IP地址: ");
		sprintf(lpszIpAddress, "%d.%d.%d.%d",
			(host_entry->h_addr_list[0][0]&0x00ff),
			(host_entry->h_addr_list[0][1]&0x00ff),
			(host_entry->h_addr_list[0][2]&0x00ff),
			(host_entry->h_addr_list[0][3]&0x00ff));
		return lpszIpAddress;
	}else
	{
		return lpszDefault;
	}
}

CString theLetter;
Cebs_config_winDlg::Cebs_config_winDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cebs_config_winDlg::IDD, pParent)
	, m_nHttpPort(0)
	, m_nHttpsPort(0)
	, m_nEbPort(0)
	, m_sDbName(_T(""))
	, m_sDbIp(_T(""))
	, m_nDbPort(0)
	, m_sDbAccount(_T(""))
	, m_sDbSecure(_T(""))
	, m_sDbBackupFile(_T(""))
	, m_sServerAddress(_T(""))
	//, m_sServerAddress(_T("192.168.1.100:18012"))
	, m_sDBDir(_T(""))
	, m_sCompanyName(_T(""))
	, m_sAdminAccount(_T(""))
	, m_sAdminSecure(_T("12345678abc"))
	, m_sTel(_T(""))
	, m_bAutoChangeIp(FALSE)
	//, m_sServerIp(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bIntegrateDb = false;
	m_bGetRootPassword = false;

	for (int i=0; i<26; i++)
	{
		theLetter.AppendChar('a'+i);
	}

}

void Cebs_config_winDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HTTP_PORT, m_nHttpPort);
	DDX_Text(pDX, IDC_EDIT_HTTPS_PORT, m_nHttpsPort);
	DDX_Text(pDX, IDC_EDIT_EB_PORT, m_nEbPort);
	DDX_Text(pDX, IDC_EDIT_DB_NAME, m_sDbName);
	DDX_Text(pDX, IDC_EDIT_DB_IP, m_sDbIp);
	DDX_Text(pDX, IDC_EDIT_DB_PORT, m_nDbPort);
	DDX_Text(pDX, IDC_EDIT_DB_ACCOUNT, m_sDbAccount);
	DDX_Text(pDX, IDC_EDIT_DB_SECURE, m_sDbSecure);
	DDX_Text(pDX, IDC_EDIT_DB_BACKUP, m_sDbBackupFile);
	//DDX_Text(pDX, IDC_EDIT_SERVER_IP, m_sServerAddress);
	//DDX_Text(pDX, IDC_EDIT_PG_BIN_PATH, m_sDBDir);
	DDX_Text(pDX, IDC_EDIT_COMPANY_NAME, m_sCompanyName);
	DDX_Text(pDX, IDC_EDIT_ADMIN_ACCOUNT, m_sAdminAccount);
	DDX_Text(pDX, IDC_EDIT_ADMIN_SECURE, m_sAdminSecure);
	DDX_Text(pDX, IDC_EDIT_CS_TEL, m_sTel);
	DDX_Control(pDX, IDC_COMBO_DBTYPE, m_comboDBType);
	DDX_Check(pDX, IDC_CHECK_AUTO_CHANGEIP, m_bAutoChangeIp);
	DDX_Control(pDX, IDC_COMBO_SERVER_IPS, m_pServerIpList);
	DDX_CBString(pDX, IDC_COMBO_SERVER_IPS, m_sServerAddress);
	//DDX_CBString(pDX, IDC_COMBO_SERVER_IPS, m_sServerIp);
}

BEGIN_MESSAGE_MAP(Cebs_config_winDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDC_BUTTON_SAVE_CURPATH, &Cebs_config_winDlg::OnBnClickedButtonSaveCurpath)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_HTTPPORT, &Cebs_config_winDlg::OnBnClickedButtonSaveHttpport)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_DBINFO, &Cebs_config_winDlg::OnBnClickedButtonSaveDbinfo)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CLIENTINFO, &Cebs_config_winDlg::OnBnClickedButtonSaveClientinfo)
	//ON_BN_CLICKED(IDC_BUTTON_NEW_DBACCOUNT, &Cebs_config_winDlg::OnBnClickedButtonNewDbaccount)
	ON_WM_TIMER()
	//ON_BN_CLICKED(IDC_BUTTON_SET_PG_BIN_PATH, &Cebs_config_winDlg::OnBnClickedButtonSetPgBinPath)
	ON_BN_CLICKED(IDC_BUTTON_SET_BACKUP_FILE, &Cebs_config_winDlg::OnBnClickedButtonSetBackupFile)
	ON_BN_CLICKED(IDC_BUTTON_INIT_DB, &Cebs_config_winDlg::OnBnClickedButtonInitDb)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CSINFO, &Cebs_config_winDlg::OnBnClickedButtonSaveCsinfo)
	ON_CBN_SELCHANGE(IDC_COMBO_DBTYPE, &Cebs_config_winDlg::OnCbnSelchangeComboDbtype)
	ON_EN_CHANGE(IDC_EDIT_DB_ACCOUNT, &Cebs_config_winDlg::OnEnChangeEditDbAccount)
	ON_BN_CLICKED(IDC_BUTTON_DB_TEST, &Cebs_config_winDlg::OnBnClickedButtonDbTest)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_IP, &Cebs_config_winDlg::OnBnClickedButtonChangeIp)
	ON_BN_CLICKED(IDC_CHECK_AUTO_CHANGEIP, &Cebs_config_winDlg::OnBnClickedCheckAutoChangeip)
END_MESSAGE_MAP()


// Cebs_config_winDlg message handlers

bool FindPGDir(LPCTSTR lpszDir, CString& sOutDir)
{
	if (!PathFileExists(lpszDir))
	{
		return false;
	}
	bool result = false;
	CString sFindDir = lpszDir;
	if (sFindDir.Right(1) == _T("\\"))
		sFindDir += _T("*.*");
	else
		sFindDir += _T("\\*.*");
	CFileFind pFileFind;
	BOOL ret = pFileFind.FindFile(sFindDir);
	while (ret)
	{
		ret = pFileFind.FindNextFile();
		if (pFileFind.IsDots()) continue;
		if (pFileFind.IsDirectory())
		{
			if (FindPGDir(pFileFind.GetFilePath(),sOutDir))
			{
				result = true;
				break;
			}
		}else if (pFileFind.GetFileName() == _T("createuser.exe"))
		{
			sOutDir = pFileFind.GetFilePath();
			sOutDir = sOutDir.Mid(0, sOutDir.ReverseFind('\\'));
			result = true;
			break;
		}
	}
	pFileFind.Close();
	return result;
}
bool FindMySQLDir(LPCTSTR lpszDir, CString& sOutDir)
{
	if (!PathFileExists(lpszDir))
	{
		return false;
	}

	bool result = false;
	CString sFindDir = lpszDir;
	if (sFindDir.Right(1) == _T("\\"))
		sFindDir += _T("*.*");
	else
		sFindDir += _T("\\*.*");
	CFileFind pFileFind;
	BOOL ret = pFileFind.FindFile(sFindDir);
	while (ret)
	{
		ret = pFileFind.FindNextFile();
		if (pFileFind.IsDots()) continue;
		if (pFileFind.IsDirectory())
		{
			if (FindMySQLDir(pFileFind.GetFilePath(),sOutDir))
			{
				result = true;
				break;
			}
		}else if (pFileFind.GetFileName() == _T("mysql.exe"))
		{
			sOutDir = pFileFind.GetFilePath();
			sOutDir = sOutDir.Mid(0, sOutDir.ReverseFind('\\'));
			result = true;
			break;
		}
	}
	pFileFind.Close();
	return result;
}

BOOL ExecDosCmd(LPCSTR sAppName,LPSTR sCommandLine,CString& sOutString,bool bShow = true)
{
	TCHAR               CommandLine[1024*10]   = {0};
	GetSystemDirectory(CommandLine, MAX_PATH);
	//sprintf(CommandLine,"%s\\cmd.exe /c %s",CommandLine,sCommandLine);
	_tcscat_s(CommandLine, MAX_PATH, _T("\\cmd.exe /c "));  
	_tcscat_s(CommandLine, 8*1024, sCommandLine);  
	//_tcscat_s(CommandLine, MAX_PATH, sCommandLine);  

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
	if (!CreateProcess(sAppName, CommandLine,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))   
	//if (!CreateProcess(sAppName, sCommandLine,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))   
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

		//buffer中就是执行的结果，可以保存到文本，也可以直接输出  
		//AfxMessageBox(buffer);   //这里是弹出对话框显示  
	}  
	CloseHandle(hRead);   
	return TRUE;
}
typedef struct tagNetworkCfg 
{
	char szIP[18];
	char szNetmask[18];
	char szGateway[18];
	//char szDns1[18];
	//char szDns2[18];
}NetworkCfg;
inline bool GetNetworkCfg(std::vector<NetworkCfg>& pOutNetworkCfg,bool bGetEmptyGateway)
{
	//log_printf("Get network config");
	//获取网卡名称 网卡名称，网卡别名
	std::vector<std::string> strAdapterAliasList;
	//std::string strAdapterName;
	HKEY hKey, hSubKey, hNdiIntKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"System\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}",
		0,
		KEY_READ,
		&hKey) != ERROR_SUCCESS)
		return false;

	DWORD dwIndex = 0;
	DWORD dwBufSize = 256;
	DWORD dwDataType;
	char szSubKey[256];
	unsigned char szData[256];

	while(RegEnumKeyEx(hKey, dwIndex++, szSubKey, &dwBufSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		if(RegOpenKeyEx(hKey, szSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
		{    
			if(RegOpenKeyEx(hSubKey, "Ndi\\Interfaces", 0, KEY_READ, &hNdiIntKey) == ERROR_SUCCESS)
			{
				dwBufSize = 256;
				if(RegQueryValueEx(hNdiIntKey, "LowerRange", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
				{
					if(strstr((char*)szData, "ethernet") != NULL)//  判断是不是以太网卡
					{
						dwBufSize = 256;
						if(RegQueryValueEx(hSubKey, "DriverDesc", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
						{
							//strAdapterName = (LPCTSTR)szData;
							dwBufSize = 256;
							if(RegQueryValueEx(hSubKey, "NetCfgInstanceID", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
							{
								strAdapterAliasList.push_back((LPCTSTR)szData);
								//break;
							}
						}
					}
				}
				RegCloseKey(hNdiIntKey);
			}
			RegCloseKey(hSubKey);
		}

		dwBufSize = 256;
	}  /* end of while */

	RegCloseKey(hKey);
	if (strAdapterAliasList.empty())
	{
		//log_printf("failed to get network config");
		return false;
	}
	for (size_t i=0;i<strAdapterAliasList.size();i++)
	{
		std::string strKeyName = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
		strKeyName += strAdapterAliasList[i];
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			strKeyName.c_str(),
			0,
			KEY_READ,
			&hKey) != ERROR_SUCCESS)
			continue;
		NetworkCfg cfg;
		memset(&cfg,0,sizeof(cfg));
		dwBufSize = 256;
		if(RegQueryValueEx(hKey, "DhcpIPAddress", 0,&dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
			strcpy(cfg.szIP,(LPCTSTR)szData);
		else{
			if(RegQueryValueEx(hKey, "IPAddress", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
				strcpy(cfg.szIP,(LPCTSTR)szData);
		}
		if (strstr(cfg.szIP,"0.0.0.0")!=NULL)
		{
			RegCloseKey(hKey);
			continue;
		}

		dwBufSize = 256;
		if(RegQueryValueEx(hKey, "DhcpSubnetMask", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
			strcpy(cfg.szNetmask,(LPCTSTR)szData);
		else
		{
			if(RegQueryValueEx(hKey, "SubnetMask", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
				strcpy(cfg.szNetmask,(LPCTSTR)szData);
		}

		dwBufSize = 256;
		if(RegQueryValueEx(hKey, "DhcpDefaultGateway", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
			strcpy(cfg.szGateway,(LPCTSTR)szData);
		else
		{
			if(RegQueryValueEx(hKey, "DefaultGateway", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
				strcpy(cfg.szGateway,(LPCSTR)szData);
		}
		RegCloseKey(hKey);

		if (!bGetEmptyGateway && strlen(cfg.szGateway)==0)
		{
			continue;
		}

		////获取DNS服务器信息
		//FIXED_INFO *fi = (FIXED_INFO *)GlobalAlloc(GPTR,sizeof( FIXED_INFO));
		//ULONG ulOutBufLen = sizeof(FIXED_INFO);
		//DWORD ret = ::GetNetworkParams(fi, &ulOutBufLen);
		//if(ret != ERROR_SUCCESS) 
		//{ 
		//	GlobalFree(fi);
		//	fi = (FIXED_INFO *) GlobalAlloc( GPTR, ulOutBufLen );
		//	ret = ::GetNetworkParams(fi, &ulOutBufLen);
		//	if(ret != ERROR_SUCCESS) 
		//	{
		//		//log_printf("Get Dns server failed"); 
		//		GlobalFree(fi);
		//		continue;
		//	}
		//}
		//strcpy(cfg.szDns1,fi->DnsServerList.IpAddress.String);
		//IP_ADDR_STRING *pIPAddr = fi->DnsServerList.Next;
		//if(pIPAddr != NULL) 
		//	strcpy(cfg.szDns2, pIPAddr->IpAddress.String);
		//GlobalFree(fi);

		pOutNetworkCfg.push_back(cfg);
	}
	return true;
}
inline int GetLocalHostIp(std::vector<std::string> & pOutIps)
{
	/*
	char host_name[255];
	//获取本地主机名称
	if (gethostname(host_name, sizeof(host_name)) == -1) {
		printf("Error when getting local host name.n\n" );
		return 1;
	}
	//printf("Host name is: %s\n", host_name);
	//从主机名数据库中得到对应的“主机”
	struct hostent *phe = gethostbyname(host_name);
	if (phe == 0) {
		printf("Yow! Bad host lookup.");
		return 1;
	}
	//循环得出本地机器所有IP地址
	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		pOutIps.push_back(inet_ntoa(addr));
		printf("Address %d : %s\n" , i, inet_ntoa(addr));
	}
	return 0;
	*/

	int count=0;
#ifdef _WIN32
	char host_name[255];
	//获取本地主机名称
	if (gethostname(host_name, sizeof(host_name)) == -1) {
		//DWORD dwError = GetLastError();
		printf("Error when getting local host name.n\n" );
		return 1;
	}
	//printf("Host name is: %s\n", host_name);
	//从主机名数据库中得到对应的“主机”
	struct hostent *phe = gethostbyname(host_name);
	if (phe == 0) {
		printf("Yow! Bad host lookup.");
		return 1;
	}
	//循环得出本地机器所有IP地址
	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		pOutIps.push_back(inet_ntoa(addr));
		printf("Address %d : %s\n" , i, inet_ntoa(addr));
	}
#else

	// **只取到127.0.0.1
	//char hname[128];
	//struct hostent *hent;
	//int i;

	//gethostname(hname, sizeof(hname));
	////hent = gethostent();
	//hent = gethostbyname(hname);
	//printf("hostname: %s/naddress list: ", hent->h_name);
	//for(i = 0; hent->h_addr_list[i]; i++) {
	//	pOutIps.push_back(inet_ntoa(*(struct in_addr*)(hent->h_addr_list[i])));
	//	printf("%s\n", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[i])));
	//}

	CLockMap<std::string,bool> pIpList;

	struct ifaddrs * ifAddrStruct=NULL;
	void * tmpAddrPtr=NULL;
	getifaddrs(&ifAddrStruct);
	char addressBuffer[INET_ADDRSTRLEN];
	while (ifAddrStruct!=NULL) {
		if (ifAddrStruct->ifa_addr->sa_family==AF_INET) { // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
		} else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6) { // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
		}else
		{
			ifAddrStruct=ifAddrStruct->ifa_next;
			continue;
		}

		if (!pIpList.exist(addressBuffer) && strstr(addressBuffer,"127.0.0.1")==NULL)
		{
			pIpList.insert(addressBuffer,true);
			count++;
			pOutIps.push_back(addressBuffer);
			printf("%s IP Address : %s\n", ifAddrStruct->ifa_name, addressBuffer); 
		}
		ifAddrStruct=ifAddrStruct->ifa_next;
	}

	//int sock;
	//struct sockaddr_in sin;
	//struct ifreq ifr;
	//sock = socket(AF_INET, SOCK_STREAM, 0);
	//if(sock>=0)//!<0
	//{
	//	for(int i=0;i<10;i++)
	//	{
	//		memset(&ifr, 0, sizeof(ifr)); 
	//		sprintf(ifr.ifr_name,"eth%d",i);
	//		if(ioctl(sock,SIOCGIFADDR,&ifr)<0) break;
	//		::memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	//		pOutIps.push_back((const char*)inet_ntoa(sin.sin_addr));
	//		printf("Address %d : %s\n", i, (const char*)inet_ntoa(sin.sin_addr));

	//		memset(&ifr, 0, sizeof(ifr)); 
	//		sprintf(ifr.ifr_name,"eth%d:1",i);
	//		if(ioctl(sock,SIOCGIFADDR,&ifr)<0) continue;
	//		::memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	//		pOutIps.push_back((const char*)inet_ntoa(sin.sin_addr));
	//		printf("Address %d : %s\n", i, (const char*)inet_ntoa(sin.sin_addr));
	//	}
	//	close(sock);
	//}else
	//{
	//	printf("socket error\n");
	//}
#endif
	return count;
}

inline int GetLocalHostIp2(std::vector<std::string> & pOutIps)
{
#ifdef _WIN32
	std::vector<tagNetworkCfg> pNetworkCfg;
	GetNetworkCfg(pNetworkCfg,false);	// **有可能取到二个以上，其中有一个是不对的
	if (pNetworkCfg.size()>1)
	{
		// 做交叉比较，去掉没用的IP地址，取出真实IP地址；
		CLockMap<std::string,bool> pIpList;
		for (int i=0;i<pNetworkCfg.size();i++)
			pIpList.insert(pNetworkCfg[i].szIP,true);
		std::vector<std::string> pLocalHostIp;
		GetLocalHostIp(pLocalHostIp);
		for (int i=0;i<pLocalHostIp.size();i++)
		{
			const tstring sIp = pLocalHostIp[i];
			if (pIpList.exist(sIp))
			{
				pOutIps.push_back(sIp);
				break;
			}
		}
		if (pOutIps.empty())
			GetLocalHostIp(pOutIps);
	}else if (pNetworkCfg.size()==1)
	{
		pOutIps.push_back(pNetworkCfg[0].szIP);
	}else
	{
		GetLocalHostIp(pOutIps);
	}
#else
	GetLocalHostIp(pOutIps);
#endif
	return pOutIps.size();
}


bool Cebs_config_winDlg::IsLocalIp(const char* sIp)
{
	for (size_t i=0; i<m_pLocalHostIp.size(); i++)
	{
		if (m_pLocalHostIp[i]==sIp)
			return true;
	}
	return false;
}

BOOL Cebs_config_winDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_pServerIpList.LimitText(
	WSADATA wsaData;
	int err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );

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

	m_comboDBType.AddString(_T("PostgreSQL"));
	m_comboDBType.AddString(_T("MySQL"));
	m_comboDBType.AddString(_T("恩布内置数据库SQLite"));
	m_comboDBType.SetCurSel(0);

	const int const_max_buffer_size = 1024*10;
	char lpszBuffer[const_max_buffer_size];
	memset(lpszBuffer,0,const_max_buffer_size);

	SetTimer(TIMER_FIND_DB_TYPE,10,NULL);

	const CString sParamsFile = theApp.GetAppUpPath()+"/ebs/conf/params.xml";
	//const CString sPOPCMParamsFile = theApp.GetAppUpPath()+"/ebs/conf/POPChatManager/params.xml";
	FILE * f = fopen(sParamsFile,"r");
	if (f != NULL)
	{
		fread(lpszBuffer,1,const_max_buffer_size,f);
		fclose(f);
		//const CString sCmdataPath = theApp.GetAppUpPath()+"/cmdata";
		//if (strstr(lpszBuffer,sCmdataPath) != NULL)
		//{
		//	this->GetDlgItem(IDC_STATIC_PATH_STATE)->SetWindowText(_T("*当前配置路径正确"));
		//	this->GetDlgItem(IDC_BUTTON_SAVE_CURPATH)->EnableWindow(FALSE);
		//}else
		//{
		//	this->GetDlgItem(IDC_STATIC_PATH_STATE)->SetWindowText(_T("*当前配置路径错误，请修改路径"));
		//	this->GetDlgItem(IDC_BUTTON_SAVE_CURPATH)->EnableWindow(TRUE);
		//}
		if (strstr(lpszBuffer,"<value>ds_pop_mysql</value>")!=NULL)
		{
			m_comboDBType.SetCurSel(1);
			OnCbnSelchangeComboDbtype();
		}else if (strstr(lpszBuffer,"<value>ds_pop_sqlite</value>")!=NULL)
		{
			m_comboDBType.SetCurSel(2);
			OnCbnSelchangeComboDbtype();
		}
		// <parameter><name>lc-domain</name><type>string</type><value>192.168.1.103</value></parameter>
		const std::string sFindStart("lc-domain</name><type>string</type><value>");
		char * find1 = strstr(lpszBuffer,sFindStart.c_str());
		if (find1 != NULL)
		{
			char * find2 = strstr(find1,"</value>");
			if (find2 != NULL)
			{
				m_sServerAddress = CString(find1+sFindStart.size(),find2-find1-sFindStart.size());
				UpdateData(FALSE);
			}
		}
	}
	// modules.xml
	const CString sModuleXmlFile = theApp.GetAppUpPath()+"/ebs/conf/modules.xml";
	f = fopen(sModuleXmlFile,"r");
	if (f != NULL)
	{
		fread(lpszBuffer,1,const_max_buffer_size,f);
		fclose(f);
		char * find = strstr(lpszBuffer,"<name>Comm80Server</name>");
		if (find != NULL)
		{
			find = strstr(find,"<commport>");
			m_nHttpPort = atoi(find+10);
			UpdateData(FALSE);
		}
		find = strstr(lpszBuffer,"<name>CommSslServer</name>");
		if (find != NULL)
		{
			find = strstr(find,"<commport>");
			m_nHttpsPort = atoi(find+10);
			UpdateData(FALSE);
		}
		find = strstr(lpszBuffer,"<name>CommUdpServer</name>");
		if (find != NULL)
		{
			find = strstr(find,"<commport>");
			m_nEbPort = atoi(find+10);
			UpdateData(FALSE);
		}
	}
	// cdbcs.xml
	const CString sCdbcXmlFile = theApp.GetAppUpPath()+"/ebs/conf/cdbcs.xml";
	f = fopen(sCdbcXmlFile,"r");
	if (f != NULL)
	{
		fread(lpszBuffer,1,const_max_buffer_size,f);
		fclose(f);
		char * find = strstr(lpszBuffer,"<name>cdbc_eb</name>");
		if (find != NULL)
		{
			char * find1 = strstr(find,"<database>");
			char * find2 = strstr(find1,"</database>");
			m_sDbName = CString(find1+10,find2-find1-10);
			find1 = strstr(find,"<host>");
			find2 = strstr(find1,":");
			if (find2==0 || find2-find1>30)
			{
				// 找不到端口，有可能是MySQL
				find2 = strstr(find1,"</host>");
				m_sDbIp = CString(find1+6,find2-find1-6);
				//m_nDbPort = 3306; //
			}else
			{
				m_sDbIp = CString(find1+6,find2-find1-6);
				m_nDbPort = atoi(find2+1);
			}
			find1 = strstr(find,"<account>");
			find2 = strstr(find1,"</account>");
			m_sDbAccount = CString(find1+9,find2-find1-9);
			find1 = strstr(find,"<secure>");
			find2 = strstr(find1,"</secure>");
			m_sDbSecure = CString(find1+8,find2-find1-8);
			UpdateData(FALSE);
		}
	}
	const CString sOnlineCallJSFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/webim/js/onlinecall.js";
	//const CString sOnlineCallJSFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/js/onlinecall.js";
	f = fopen(sOnlineCallJSFile,"r");
	if (f != NULL)
	{
		fread(lpszBuffer,1,const_max_buffer_size,f);
		fclose(f);
		char * find = strstr(lpszBuffer,"class=\"sp-tel yahei\"><span>");
		if (find != NULL)
		{
			char * find1 = strstr(find,"&nbsp;");
			char * find2 = strstr(find1,"</span>");
			m_sTel = CString(find1+6,find2-find1-6);
			UpdateData(FALSE);
		}
	}
	GetLocalHostIp2(m_pLocalHostIp);
	for (size_t i=0; i<m_pLocalHostIp.size(); i++)
	{
		m_pServerIpList.AddString(m_pLocalHostIp[i].c_str());
	}

	// ebc.ini
	memset(lpszBuffer,0,sizeof(lpszBuffer));
	//const CString sEbcIniFile = theApp.GetAppUpPath()+"/ebc/datas/ebc.ini";
	//::GetPrivateProfileString(_T("system"),_T("server"),m_sServerAddress,lpszBuffer,100,sEbcIniFile);
	//m_sServerAddress = lpszBuffer;
	//m_sServerAddress = "www.entboost.com:18012";
	//CString sServerIp;
	bool bIsDomainName = false;
	if (!m_pLocalHostIp.empty() && m_sServerAddress.IsEmpty())
	{
		m_sServerAddress = m_pLocalHostIp[0].c_str();
		//sServerIp = pLocalHostIp[0].c_str();
		//m_sServerAddress.Format(_T("%s:%d"),sServerIp,m_nEbPort);
		this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(_T("*默认获取本机IP地址，请确认后保存"));
	}else
	{
		//const int nfind = m_sServerAddress.Find(_T(":"));
		//if (nfind<0)
		//{
		//	this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(_T("*服务地址格式错误，请修改！"));
		//	if (pLocalHostIp.empty())
		//		sServerIp = _T("192.168.1.100");
		//	else
		//		sServerIp = pLocalHostIp[0].c_str();
		//	m_sServerAddress.Format(_T("%s:%d"),sServerIp,m_nEbPort);
		//	//m_sServerAddress.Format(_T("%s:%d"),sServerIp,m_nEbPort);
		//}else
		{
			CString sServerIp(m_sServerAddress);
			//sServerIp = m_sServerAddress.Left(nfind);
			sServerIp.MakeLower();
			bIsDomainName = sServerIp.FindOneOf(theLetter)>=0?true:false;
			if (bIsDomainName)
			{
				const std::string sIp = GetHostIp(sServerIp,"");
				if (sIp.empty())
				{
					this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(_T("*IP地址错误，请修改！"));
				}else if (IsLocalIp(sIp.c_str()))
				{
					this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(_T(""));
				}else
				{
					CString stext;
					stext.Format(_T("*域名[%s]IP[%s]不是本机IP，请修改后保存"),m_sServerAddress,sIp.c_str());
					this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(stext);
				}
			}else
			{
				this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(_T("正在检查服务地址..."));
				char lpszCommand[256];
				sprintf(lpszCommand,"ping %s -n 1",sServerIp);
				CString sString;
				ExecDosCmd(NULL,lpszCommand,sString,false);
				//AfxMessageBox(lpszCommand);
				if (sString.Find("字节=32 时间")<0 && sString.Find("bytes=32 time")<0)
				{
					sString.Format(_T("*错误地址[%s]，请修改后保存"),sServerIp);
					this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(sString);
				}else if (IsLocalIp(m_sServerAddress))
				{
					this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(_T(""));
				}else
				{
					sString.Format(_T("[%s]不是本机IP，请修改后保存"),sServerIp);
					this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(sString);
				}
			}
		}
	}

	m_sCompanyName = _T("请修改公司名称！");
	if (bIsDomainName)
	{
		std::string sServerIpTemp = (LPCTSTR)m_sServerAddress;
		//std::string sServerIpTemp = (LPCTSTR)sServerIp;
		std::string::size_type nfind = sServerIpTemp.rfind(".");
		if (nfind!=std::string::npos)
		{
			nfind = sServerIpTemp.rfind(".",nfind-1);
		}
		if (nfind==std::string::npos)
			m_sAdminAccount.Format(_T("admin@%s"),sServerIpTemp.c_str());
		else
			m_sAdminAccount.Format(_T("admin@%s"),sServerIpTemp.substr(nfind+1).c_str());
	}else
	{
		m_sAdminAccount.Format(_T("admin@%s"),m_sServerAddress);
		//m_sAdminAccount.Format(_T("admin@%s"),sServerIp);
	}
	//m_sAdminAccount = _T("admin@192.168.1.100");
	//m_sAdminSecure.Format(_T("%d%04d"),(int)(time(0)%10000),::GetTickCount()%10000);

	CString sSettingFile;
	sSettingFile.Format(_T("%s/ebs/conf/POPLogonCenter/enable_auto_detect_server_ip"),theApp.GetAppUpPath());
	if (::PathFileExists(sSettingFile))
		m_bAutoChangeIp = TRUE;
	UpdateData(FALSE);

	// postgresql test data
	//m_sDBDir = _T("F:\\git\\oschina\\entboost-1.12.1-win-pgsql-x64\\pgsql\\bin");
	//m_comboDBType.SetCurSel(0);
	//m_sDbIp = _T("114.215.181.249");
	//m_nDbPort = 5432;
	//m_sDbName = _T("entboost-1");
	//m_sDbAccount = _T("postgres");
	//m_sDbSecure = _T("postgres1");
	//UpdateData(FALSE);

	//m_comboDBType.SetCurSel(0);
	//m_sDbIp = _T("192.168.1.198");
	//m_nDbPort = 5432;
	//m_sDbName = _T("entboost-10");
	//m_sDbAccount = _T("postgres");
	//m_sDbSecure = _T("postgres");
	//UpdateData(FALSE);

	//// mysql test data
	//m_sDBDir = _T("F:\\git\\oschina\\entboost-1.5.0-win-mysql-x64\\mysql-5.6.20-winx64\\bin");
	//m_comboDBType.SetCurSel(1);
	//m_sDbIp = _T("localhost");
	//m_nDbPort = 3306;
	//m_sDbName = _T("entboost");
	//m_sDbAccount = _T("root");
	//m_sDbSecure = _T("");
	//UpdateData(FALSE);

	//// mysql test data
	//m_sDBDir = _T("F:\\git\\oschina\\entboost-1.5.0-win-mysql-x64\\mysql-5.6.20-winx64\\bin");
	//m_comboDBType.SetCurSel(1);
	//m_sDbIp = _T("192.168.1.198");
	//m_nDbPort = 3306;
	//m_sDbName = _T("entboostb");
	//m_sDbAccount = _T("root");
	//m_sDbSecure = _T("mysql");
	////m_sDbSecure = _T("111111");
	//UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cebs_config_winDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cebs_config_winDlg::OnPaint()
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
HCURSOR Cebs_config_winDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//void Cebs_config_winDlg::OnBnClickedButtonSaveCurpath()
//{
//	const int const_max_buffer_size = 1024*10;
//	char lpszBuffer[const_max_buffer_size];
//
//	const CString sParamsFile = theApp.GetAppUpPath()+"/ebs/conf/params.xml";
//	//const CString sPOPCMParamsFile = theApp.GetAppUpPath()+"/ebs/conf/POPChatManager/params.xml";
//	FILE * f = fopen(sParamsFile,"r+");
//	if (f != NULL)
//	{
//		memset(lpszBuffer,0,const_max_buffer_size);
//		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
//		lpszBuffer[r] = '\0';
//		fclose(f);
//		char * find = strstr(lpszBuffer,"<parameter><name>cm-msg-path");
//		if (find == NULL)
//		{
//			return;
//		}
//		// 重建XML文件
//		f = fopen(sParamsFile,"w");
//		fwrite(lpszBuffer,1,find-lpszBuffer,f);
//		CString sPathString;
//		sPathString.Format(_T("<parameter><name>cm-msg-path</name><type>string</type><value>%s/cmdata/msg</value></parameter>\n"),theApp.GetAppUpPath());
//		fwrite(sPathString,1,sPathString.GetLength(),f);
//		sPathString.Format(_T("\t<parameter><name>cm-res-path</name><type>string</type><value>%s/cmdata/res</value></parameter>\n"),theApp.GetAppUpPath());
//		fwrite(sPathString,1,sPathString.GetLength(),f);
//		sPathString.Format(_T("\t<parameter><name>cm-file-path</name><type>string</type><value>%s/cmdata/file</value></parameter>\n"),theApp.GetAppUpPath());
//		fwrite(sPathString,1,sPathString.GetLength(),f);
//		sPathString.Format(_T("\t<parameter><name>push-ssl-path</name><type>string</type><value>%s/cmdata/push</value></parameter>\n"),theApp.GetAppUpPath());
//		fwrite(sPathString,1,sPathString.GetLength(),f);
//		sPathString.Format(_T("\t<parameter><name>cm-online-path</name><type>string</type><value>%s/cmdata/online</value></parameter>"),theApp.GetAppUpPath());
//		fwrite(sPathString,1,sPathString.GetLength(),f);
//		find = strstr(find,"online</value></parameter>");
//		if (find != NULL)
//		{
//			// 保存后面内容
//			find += 26;
//			fwrite(find,1,strlen(find),f);
//		}else
//		{
//			fwrite("</root>\n",1,9,f);
//		}
//		fclose(f);
//	}
//	const CString sWebCMParamsFile = theApp.GetAppUpPath()+"/ebs/conf/ebwebcm/params.xml";
//	f = fopen(sWebCMParamsFile,"r+");
//	if (f != NULL)
//	{
//		memset(lpszBuffer,0,const_max_buffer_size);
//		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
//		lpszBuffer[r] = '\0';
//		fclose(f);
//		char * find = strstr(lpszBuffer,"<parameter><name>temp-res-path");
//		//char * find = strstr(lpszBuffer,"<parameter><name>system-res-path");
//		if (find == NULL)
//		{
//			return;
//		}
//		// 重建XML文件
//		f = fopen(sWebCMParamsFile,"w");
//		fwrite(lpszBuffer,1,find-lpszBuffer,f);
//		CString sPathString;
//		//sPathString.Format(_T("<parameter><name>system-res-path</name><type>string</type><value>%s/cmdata/res</value></parameter>\n"),theApp.GetAppUpPath());
//		//fwrite(sPathString,1,sPathString.GetLength(),f);
//		sPathString.Format(_T("<parameter><name>temp-res-path</name><type>string</type><value>%s/tempres</value></parameter>"),theApp.GetAppUpPath());
//		fwrite(sPathString,1,sPathString.GetLength(),f);
//		find = strstr(find,"tempres</value></parameter>");
//		if (find != NULL)
//		{
//			// 保存后面内容
//			find += 27;
//			fwrite(find,1,strlen(find),f);
//		}else
//		{
//			fclose(f);
//			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sWebCMParamsFile);
//			this->GetDlgItem(IDC_STATIC_PATH_STATE)->SetWindowText(sPathString);
//			return;
//		}
//		fclose(f);
//	}
//
//	this->GetDlgItem(IDC_STATIC_PATH_STATE)->SetWindowText(_T("*当前配置路径正确"));
//	this->GetDlgItem(IDC_BUTTON_SAVE_CURPATH)->EnableWindow(FALSE);
//}

void Cebs_config_winDlg::OnBnClickedButtonSaveHttpport()
{
	const int const_max_buffer_size = 1024*10;
	char lpszBuffer[const_max_buffer_size];
	UpdateData();

	const CString sModuleXmlFile = theApp.GetAppUpPath()+"/ebs/conf/modules.xml";
	FILE * f = fopen(sModuleXmlFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"<name>Comm80Server</name>");
		if (find == NULL)
		{
			return;
		}
		// 重建XML文件
		f = fopen(sModuleXmlFile,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		sPathString.Format(_T("<name>Comm80Server</name>\n\t\t<protocol>1</protocol>\n\t\t<commport>%d</commport>"),m_nHttpPort);
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"</commport>");
		if (find != NULL)
		{
			// 保存后面内容
			find += 11;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sModuleXmlFile);
			this->SetWindowText(sPathString);
			return;
		}
		fclose(f);
	}
	f = fopen(sModuleXmlFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"<name>CommSslServer</name>");
		if (find == NULL)
		{
			return;
		}
		// 重建XML文件
		f = fopen(sModuleXmlFile,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		sPathString.Format(_T("<name>CommSslServer</name>\n\t\t<protocol>5</protocol>\n\t\t<commport>%d</commport>"),m_nHttpsPort);
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"</commport>");
		if (find != NULL)
		{
			// 保存后面内容
			find += 11;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sModuleXmlFile);
			this->SetWindowText(sPathString);
			return;
		}
		fclose(f);
	}
	f = fopen(sModuleXmlFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"<name>CommUdpServer</name>");
		if (find == NULL)
		{
			return;
		}
		// 重建XML文件
		f = fopen(sModuleXmlFile,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		sPathString.Format(_T("<name>CommUdpServer</name>\n\t\t<commport>%d</commport>"),m_nEbPort);
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"</commport>");
		if (find != NULL)
		{
			// 保存后面内容
			find += 11;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sModuleXmlFile);
			this->SetWindowText(sPathString);
			return;
		}
		fclose(f);
	}
	const CString sSystemParamsXmlFile = theApp.GetAppUpPath()+"/ebs/conf/params.xml";
	// 修改 http-port
	f = fopen(sSystemParamsXmlFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		const char * find = strstr(lpszBuffer,"<parameter><name>http-port</name><type>int</type><value>");
		if (find != NULL)
		{
			// 重建XML文件
			f = fopen(sSystemParamsXmlFile,"w");
			fwrite(lpszBuffer,1,find-lpszBuffer,f);
			CString sPathString;
			sPathString.Format(_T("<parameter><name>http-port</name><type>int</type><value>%d"),m_nHttpPort);
			fwrite(sPathString,1,sPathString.GetLength(),f);
			find = strstr(find,"</value></parameter>");
			if (find != NULL)
			{
				// 保存后面内容
				//find += 11;
				fwrite(find,1,strlen(find),f);
			}else
			{
				fclose(f);
				sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sSystemParamsXmlFile);
				this->SetWindowText(sPathString);
				return;
			}
			fclose(f);
		}
	}
	// 修改 port
	f = fopen(sSystemParamsXmlFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		const char * find = strstr(lpszBuffer,"<parameter><name>port</name><type>int</type><value>");
		if (find != NULL)
		{
			// 重建XML文件
			f = fopen(sSystemParamsXmlFile,"w");
			fwrite(lpszBuffer,1,find-lpszBuffer,f);
			CString sPathString;
			sPathString.Format(_T("<parameter><name>port</name><type>int</type><value>%d"),m_nEbPort);
			fwrite(sPathString,1,sPathString.GetLength(),f);
			find = strstr(find,"</value></parameter>");
			if (find != NULL)
			{
				// 保存后面内容
				//find += 11;
				fwrite(find,1,strlen(find),f);
			}else
			{
				fclose(f);
				sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sSystemParamsXmlFile);
				this->SetWindowText(sPathString);
				return;
			}
			fclose(f);
		}
	}

}

bool Cebs_config_winDlg::SaveDbinfo(void)
{
	if (m_bIntegrateDb)
	{
		InstallDbFirst();
	}
	UpdateData();
	const int nDBType = m_comboDBType.GetCurSel();
	if (nDBType==1)		// msyql
	{
		if (m_sDbName.Find(_T("."))>=0 || m_sDbName.Find(_T("-"))>=0)
		{
			this->GetDlgItem(IDC_EDIT_DB_NAME)->SetFocus();
			AfxMessageBox(_T("数据库名称带有特殊字符[.-]，会导致导入数据失败，请更换数据库名称！"));
			return false;
		}
	}

	const int const_max_buffer_size = 1024*10;
	char lpszBuffer[const_max_buffer_size];

	const CString sCdbcXmlFile = theApp.GetAppUpPath()+"/ebs/conf/cdbcs.xml";
	FILE * f = fopen(sCdbcXmlFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"<name>cdbc_eb</name>");
		if (find == NULL)
		{
			return false;
		}
		// 重建XML文件
		f = fopen(sCdbcXmlFile,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		if (nDBType==0)	// pgsql
		{
			sPathString.Format(_T("<name>cdbc_eb</name>\n\t\t<database>%s</database>\n\t\t<host>%s:%d</host>\n\t\t<account>%s</account>\n\t\t<secure>%s</secure>"),
				m_sDbName,m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbSecure);
		}else if (nDBType==1)	// mysql
		{
			sPathString.Format(_T("<name>cdbc_eb</name>\n\t\t<database>%s</database>\n\t\t<host>%s:%d</host>\n\t\t<account>%s</account>\n\t\t<secure>%s</secure>"),
				m_sDbName,m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbSecure);
		}else
		{
			sPathString.Format(_T("<name>cdbc_eb</name>\n\t\t<database>%s</database>\n\t\t<host></host>\n\t\t<account></account>\n\t\t<secure></secure>"),m_sDbName);
		}
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"</secure>");
		if (find != NULL)
		{
			// 保存后面内容
			find += 9;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sCdbcXmlFile);
			this->SetWindowText(sPathString);
			return false;
		}
		fclose(f);
	}
	const CString sMYCPParams = theApp.GetAppUpPath()+"/ebs/conf/params.xml";
	f = fopen(sMYCPParams,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"<parameter><name>eb-datasource</name><type>string</type><value>");
		if (find == NULL)
		{
			/*AfxMessageBox(_T("abc"));*/
			return false;
		}
		// 重建XML文件
		f = fopen(sMYCPParams,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		if (nDBType==0)	// pgsql
			sPathString.Format(_T("<parameter><name>eb-datasource</name><type>string</type><value>ds_pop</value>"));
		else if (nDBType==1)	// mysql
			sPathString.Format(_T("<parameter><name>eb-datasource</name><type>string</type><value>ds_pop_mysql</value>"));
		else
			sPathString.Format(_T("<parameter><name>eb-datasource</name><type>string</type><value>ds_pop_sqlite</value>"));
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"</value>");
		if (find != NULL)
		{
			// 保存后面内容
			find += 8;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sMYCPParams);
			this->SetWindowText(sPathString);
			return false;
		}
		fclose(f);
	}
	f = fopen(sMYCPParams,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"<parameter><name>eb-online-ds</name><type>string</type><value>");
		if (find == NULL)
		{
			/*AfxMessageBox(_T("abc"));*/
			return false;
		}
		// 重建XML文件
		f = fopen(sMYCPParams,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		if (nDBType==0)	// pgsql
			sPathString.Format(_T("<parameter><name>eb-online-ds</name><type>string</type><value>ds_online</value>"));
		else if (nDBType==1)	// mysql
			sPathString.Format(_T("<parameter><name>eb-online-ds</name><type>string</type><value>ds_online_mysql</value>"));
		else
			sPathString.Format(_T("<parameter><name>eb-online-ds</name><type>string</type><value>ds_online_sqlite</value>"));
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"</value>");
		if (find != NULL)
		{
			// 保存后面内容
			find += 8;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sMYCPParams);
			this->SetWindowText(sPathString);
			return false;
		}
		fclose(f);
	}
	return true;
}
void Cebs_config_winDlg::OnBnClickedButtonSaveDbinfo()
{
	SaveDbinfo();
}


bool Cebs_config_winDlg::SaveClientInfo(bool bChangeIpOnly)
{
	const int const_max_buffer_size = 1024*30;
	char lpszBuffer[const_max_buffer_size];

	//const int nfind = m_sServerAddress.Find(_T(":"));
	//if (nfind<0)
	//{
	//	this->GetDlgItem(IDC_EDIT_SERVER_IP)->SetFocus();
	//	AfxMessageBox(_T("服务地址格式错误！"));
	//	return false;
	//}
	if (m_sServerAddress.IsEmpty())
	{
		m_pServerIpList.SetFocus();
		//this->GetDlgItem(IDC_EDIT_SERVER_IP)->SetFocus();
		AfxMessageBox(_T("请输入服务器地址！"));
		return false;
	}

	CString sString;
	bool bFindIP = false;
	CString sServerIp(m_sServerAddress);
	//CString sServerIp = m_sServerAddress.Left(nfind);
	sServerIp.MakeLower();
	const bool bIsDomainName = sServerIp.FindOneOf(theLetter)>=0?true:false;
	if (bIsDomainName)
	{
		const std::string sIp = GetHostIp(sServerIp,"");
		bFindIP = sIp.empty()?false:true;
	}else
	{
		sString.Format(_T("正在检查服务地址[%s]..."),sServerIp);
		this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(sString);

		char lpszCommand[256];
		sprintf(lpszCommand,"ping %s -n 1",sServerIp);
		CString sString;
		ExecDosCmd(NULL,lpszCommand,sString,false);
		if (sString.Find("字节=32 时间")>0 || sString.Find("bytes=32 time")>0)
		{
			bFindIP = true;
		}
	}
	if (bIsDomainName)
	{
		std::string sServerIpTemp = (LPCTSTR)sServerIp;
		std::string::size_type nfind = sServerIpTemp.rfind(".");
		if (nfind!=std::string::npos)
		{
			nfind = sServerIpTemp.rfind(".",nfind-1);
		}
		if (nfind==std::string::npos)
			m_sAdminAccount.Format(_T("admin@%s"),sServerIpTemp.c_str());
		else
			m_sAdminAccount.Format(_T("admin@%s"),sServerIpTemp.substr(nfind+1).c_str());
	}else
	{
		m_sAdminAccount.Format(_T("admin@%s"),sServerIp);
	}
	UpdateData(FALSE);

	if (!bFindIP)
	{
		m_pServerIpList.SetFocus();
		//this->GetDlgItem(IDC_EDIT_SERVER_IP)->SetFocus();
		sString.Format(_T("*错误地址[%s]，请修改后保存"),sServerIp);
		this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(sString);
		CString stext;
		stext.Format(_T("服务地址[%s]检查错误，是否忽略错误继续保存？"),sServerIp);
		if (MessageBox(stext,_T("地址错误"),MB_YESNO|MB_ICONQUESTION)!=IDYES)
		{
			return false;
		}
	}else if (bIsDomainName || IsLocalIp(m_sServerAddress))
	{
		this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(_T(""));
	}else
	{
		CString sString;
		sString.Format(_T("[%s]不是本机IP，请修改后保存"),m_sServerAddress);
		this->GetDlgItem(IDC_STATIC_IP_STATE)->SetWindowText(_T(""));
	}

	const CString sMYCPParams = theApp.GetAppUpPath()+"/ebs/conf/params.xml";
	FILE * f = fopen(sMYCPParams,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"<parameter><name>lc-domain</name><type>string</type><value>");
		if (find == NULL)
		{
			/*AfxMessageBox(_T("abc"));*/
			return false;
		}
		// 重建XML文件
		f = fopen(sMYCPParams,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		sPathString.Format(_T("<parameter><name>lc-domain</name><type>string</type><value>%s</value>"),sServerIp);
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"</value>");
		if (find != NULL)
		{
			// 保存后面内容
			find += 8;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sMYCPParams);
			this->SetWindowText(sPathString);
			return false;
		}
		fclose(f);
	}
	const CString sWebCMParamsFile = theApp.GetAppUpPath()+"/ebs/conf/ebwebcm/params.xml";
	f = fopen(sWebCMParamsFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"<parameter><name>service-domain</name><type>pt.string</type><value>");
		if (find == NULL)
		{
			return false;
		}
		// 重建XML文件
		f = fopen(sWebCMParamsFile,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		sPathString.Format(_T("<parameter><name>service-domain</name><type>pt.string</type><value>%s</value>"),sServerIp);
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"</value>");
		if (find != NULL)
		{
			// 保存后面内容
			find += 8;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sWebCMParamsFile);
			this->SetWindowText(sPathString);
			return false;
		}
		fclose(f);
	}
	// 修改900客服1
	const CString sEBMessengerUIJSFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/webim/client.html";
	//const CString sEBMessengerUIJSFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/js/jqEBMessengerUI.min.js";
	f = fopen(sEBMessengerUIJSFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find1 = strstr(lpszBuffer,"$.ebMsg.options.DOMAIN_URL=");
		if (find1 == NULL)
		{
			return false;
		}
		char * find2 = strstr(find1,"$.ebMsg.options.CUS_ACCOUNT=");
		if (find2 == NULL)
		{
			return false;
		}
		//$.ebMsg.options.DOMAIN_URL="entboost.entboost.com";
		//$.ebMsg.options.WEBIM_URL="http://webim.entboost.com";
		//$.ebMsg.options.CUS_ACCOUNT="9009301111";

		// 重建文件
		f = fopen(sEBMessengerUIJSFile,"w");
		//CString s;
		//s.Format(_T("%d"),(int)f);
		//AfxMessageBox(s);
		fwrite(lpszBuffer,1,find1-lpszBuffer,f);
		CString sPathString;
		if (m_nHttpPort==80)
		{
			sPathString.Format(_T("$.ebMsg.options.DOMAIN_URL=\"%s\";\r\n\t\t$.ebMsg.options.WEBIM_URL=\"http://%s/webim\";\r\n\t\t$.ebMsg.options.CUS_ACCOUNT=\"%lld\";"),
				sServerIp,sServerIp,DEFAULT_900_NUMBER);
		}else
		{
			sPathString.Format(_T("$.ebMsg.options.DOMAIN_URL=\"%s:%d\";\r\n\t\t$.ebMsg.options.WEBIM_URL=\"http://%s:%d/webim\";\r\n\t\t$.ebMsg.options.CUS_ACCOUNT=\"%lld\";"),
				sServerIp,m_nHttpPort,sServerIp,m_nHttpPort,DEFAULT_900_NUMBER);
		}
		//AfxMessageBox(sPathString);

		fwrite(sPathString,1,sPathString.GetLength(),f);
		find1 = strstr(find2,"\";");
		if (find1 != NULL)
		{
			// 保存后面内容
			find1 += 2;
			fwrite(find1,1,strlen(find1),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sEBMessengerUIJSFile);
			this->SetWindowText(sPathString);
			return false;
		}
		fclose(f);
	}
	// 修改900客服2
	const CString sOnlineCallJSFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/webim/js/onlinecall.js";
	//const CString sOnlineCallJSFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/js/onlinecall.js";
	f = fopen(sOnlineCallJSFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"webimUrl=");
		if (find == NULL)
		{
			return false;
		}
		// 重建JS文件
		f = fopen(sOnlineCallJSFile,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		if (m_nHttpPort==80)
			sPathString.Format(_T("webimUrl=\"http://%s/webim\";"),sServerIp);
		else
			sPathString.Format(_T("webimUrl=\"http://%s:%d/webim\";"),sServerIp,m_nHttpPort);
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,";");
		if (find != NULL)
		{
			// 保存后面内容
			find += 1;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sOnlineCallJSFile);
			this->SetWindowText(sPathString);
			return false;
		}
		fclose(f);
	}
	//// 修改ANDROID 地址
	//const CString sSrcConfigPropertiesFile = theApp.GetAppUpPath()+"/Android_SDK_1.0/lib/EBOnlineCustomerService/src/com/entboost/api/config.properties";
	//f = fopen(sSrcConfigPropertiesFile,"r");
	//if (f != NULL)
	//{
	//	memset(lpszBuffer,0,const_max_buffer_size);
	//	size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
	//	lpszBuffer[r] = '\0';
	//	fclose(f);
	//	char * find = strstr(lpszBuffer,"logoncenteraddr=");
	//	if (find == NULL)
	//	{
	//		return;
	//	}
	//	// 重建文件
	//	f = fopen(sSrcConfigPropertiesFile,"w");
	//	fwrite(lpszBuffer,1,find-lpszBuffer,f);
	//	CString sPathString;
	//	sPathString.Format(_T("logoncenteraddr=%s\n"),m_sServerAddress);
	//	fwrite(sPathString,1,sPathString.GetLength(),f);
	//	find = strstr(find,"\n");
	//	if (find != NULL)
	//	{
	//		// 保存后面内容
	//		find += 1;
	//		fwrite(find,1,strlen(find),f);
	//	}
	//	fclose(f);
	//}
	//// 修改ANDROID 客服号码
	//f = fopen(sSrcConfigPropertiesFile,"r");
	//if (f != NULL)
	//{
	//	memset(lpszBuffer,0,const_max_buffer_size);
	//	size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
	//	lpszBuffer[r] = '\0';
	//	fclose(f);
	//	char * find = strstr(lpszBuffer,"csaid=");
	//	if (find == NULL)
	//	{
	//		return;
	//	}
	//	// 重建文件
	//	f = fopen(sSrcConfigPropertiesFile,"w");
	//	fwrite(lpszBuffer,1,find-lpszBuffer,f);
	//	CString sPathString;
	//	sPathString.Format(_T("csaid=%lld\n"),DEFAULT_900_NUMBER);
	//	fwrite(sPathString,1,sPathString.GetLength(),f);
	//	find = strstr(find,"\n");
	//	if (find != NULL)
	//	{
	//		// 保存后面内容
	//		find += 1;
	//		fwrite(find,1,strlen(find),f);
	//	}
	//	fclose(f);
	//}

	// ebc.ini
	const CString sEbcIniFile = theApp.GetAppUpPath()+"/ebc/datas/ebc.ini";
	CString stext;
	stext.Format(_T("%s:%d"),m_sServerAddress,m_nEbPort);
	::WritePrivateProfileString(_T("system"),_T("server"),stext,sEbcIniFile);
	//::WritePrivateProfileString(_T("system"),_T("server"),m_sServerAddress,sEbcIniFile);
	// ebc_$DATE.zip

	if (!bChangeIpOnly)
	{
		TarClientZipFile();
		//const time_t tNow = time(0);
		//struct tm *newtime = localtime(&tNow);
		//char lpszEBCZipFile[60];
		//_snprintf(lpszEBCZipFile,60,"ebc_%04d%02d%02d.zip",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday);

		//stext.Format(_T("服务器地址保存成功，是否打包客户端程序 %s 吗？"),lpszEBCZipFile);
		//if (MessageBox(stext,_T("打包客户端"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
		////if (bChangeIpOnly || MessageBox(stext,_T("打包客户端"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
		//{
		//	// Rar.exe a -k -r -s -m1 d:\web.rar d:\web\\\//
		//	bool bOk = true;
		//	if (PathFileExists(lpszEBCZipFile))
		//	{
		//		bOk = false;
		//		stext.Format(_T("存在 %s 文件，确定替换更新吗？"),lpszEBCZipFile);
		//		if (MessageBox(stext,_T("打包客户端"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
		//		{
		//			bOk = true;
		//			DeleteFile(lpszEBCZipFile);
		//		}
		//	}
		//	if (bOk)
		//	{
		//		char lpszCommand[1024];
		//		sprintf(lpszCommand,"a -k -r -s %s ../ebc",lpszEBCZipFile);
		//		WinExecX("Rar.exe",lpszCommand,SW_SHOW);
		//		// 复制到下载目录；
		//		CString sCopyToFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/entboost/ebc_install.zip";
		//		CopyFile(lpszEBCZipFile,sCopyToFile,FALSE);
		//		//// 打开浏览器地址
		//		//if (m_nHttpPort==80)
		//		//	sprintf(lpszCommand,"http://%s/entboost/index.html",sServerIp);
		//		//else
		//		//	sprintf(lpszCommand,"http://%s:%d/entboost/index.html",sServerIp,m_nHttpPort);
		//		//ShellExecute(NULL, "open", lpszCommand, NULL, NULL, SW_SHOW);
		//	}
		//}
	}

	const CString s900CodeFile = _T("900_code.txt");
	f = fopen(s900CodeFile,"w");
	CString s900CodeWriteString;
	if (m_nHttpPort==80)
	{
		s900CodeWriteString.Format(_T("<script type=\"text/javascript\" src=\"http://%s/webim/js/onlinecall.js?to_account=%lld\" charset=\"UTF-8\"></script>"),
			sServerIp,DEFAULT_900_NUMBER);
	}else
	{
		s900CodeWriteString.Format(_T("<script type=\"text/javascript\" src=\"http://%s:%d/webim/js/onlinecall.js?to_account=%lld\" charset=\"UTF-8\"></script>"),
			sServerIp,m_nHttpPort,DEFAULT_900_NUMBER);
	}
	fwrite(s900CodeWriteString,1,s900CodeWriteString.GetLength(),f);
	fclose(f);
	//ShellExecute(NULL, "open", s900CodeFile, NULL, NULL, SW_SHOW);

	// 修改首页在线客服
	const CString sEntboostIndexFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/entboost/index.html";
	f = fopen(sEntboostIndexFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find1 = strstr(lpszBuffer,"<script type=\"text/javascript\"");
		if (find1 != NULL)
		{
			// 重建文件
			f = fopen(sEntboostIndexFile,"w");
			fwrite(lpszBuffer,1,find1-lpszBuffer,f);
			fwrite(s900CodeWriteString,1,s900CodeWriteString.GetLength(),f);
			const CString sPathString(_T("\r\n</body>\r\n</html>"));
			fwrite(sPathString,1,sPathString.GetLength(),f);
			fclose(f);
		}
	}

	return true;
}
void Cebs_config_winDlg::TarClientZipFile(void)
{
	const time_t tNow = time(0);
	struct tm *newtime = localtime(&tNow);
	char lpszEBCZipFile[60];
	_snprintf(lpszEBCZipFile,60,"ebc_%04d%02d%02d.zip",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday);

	CString stext;
	stext.Format(_T("服务器地址保存成功，是否打包客户端程序 %s 吗？"),lpszEBCZipFile);
	if (MessageBox(stext,_T("打包客户端"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
		//if (bChangeIpOnly || MessageBox(stext,_T("打包客户端"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
	{
		// Rar.exe a -k -r -s -m1 d:\web.rar d:\web\\\//
		bool bOk = true;
		if (PathFileExists(lpszEBCZipFile))
		{
			bOk = false;
			stext.Format(_T("存在 %s 文件，确定替换更新吗？"),lpszEBCZipFile);
			if (MessageBox(stext,_T("打包客户端"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
			{
				bOk = true;
				DeleteFile(lpszEBCZipFile);
			}
		}
		if (bOk)
		{
			char lpszCommand[1024];
			sprintf(lpszCommand,"a -k -r -s %s ../ebc",lpszEBCZipFile);
			WinExecX("Rar.exe",lpszCommand,SW_SHOW);
			// 复制到下载目录；
			CString sCopyToFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/entboost/ebc_install.zip";
			CopyFile(lpszEBCZipFile,sCopyToFile,FALSE);
			//// 打开浏览器地址
			//if (m_nHttpPort==80)
			//	sprintf(lpszCommand,"http://%s/entboost/index.html",sServerIp);
			//else
			//	sprintf(lpszCommand,"http://%s:%d/entboost/index.html",sServerIp,m_nHttpPort);
			//ShellExecute(NULL, "open", lpszCommand, NULL, NULL, SW_SHOW);
		}
	}
}

void Cebs_config_winDlg::OnBnClickedButtonSaveClientinfo()
{
	//UpdateData();
	OnBnClickedButtonSaveHttpport();
	SaveClientInfo(false);
}


void Cebs_config_winDlg::OnBnClickedButtonNewDbaccount()
{
	// TODO: Add your control notification handler code here
	// createuser -s -d -r --host $dbhost --port $dbport --username postgres $dbaccount
	UpdateData();
	if (m_sDbIp.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_DB_IP)->SetFocus();
		AfxMessageBox(_T("请输入数据库地址！"));
		return;
	}
	if (m_nDbPort<=0)
	{
		this->GetDlgItem(IDC_EDIT_DB_PORT)->SetFocus();
		AfxMessageBox(_T("请输入数据库端口！"));
		return;
	}
	if (m_sDbAccount.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_DB_ACCOUNT)->SetFocus();
		AfxMessageBox(_T("请输入数据库帐号！"));
		return;
	}

	char lpszCurrentDirectory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,lpszCurrentDirectory);
	const int nDBType = m_comboDBType.GetCurSel();
	if (nDBType==0)	// pgsql
	{
		const CString sEXEPath = m_sDBDir.IsEmpty()?theApp.GetAppPath()+"\\pgsql":m_sDBDir;
		const CString sPGCreateUserExe(sEXEPath+_T("\\createuser.exe"));
		if (!PathFileExists(sPGCreateUserExe))
		{
			CString stext;
			stext.Format(_T("%s文件不存在，请重新设置PostgreSQL数据库bin目录！"),sPGCreateUserExe);
			AfxMessageBox(stext);
			return;
		}
		InstallDbFirst();

		CString stext;
		stext.Format(_T("你确定新建数据库帐号 %s 吗？"),m_sDbAccount);
		if (MessageBox(stext,_T("新建数据库帐号"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
			return;
		SetCurrentDirectory(sEXEPath);
		char lpszCommand[1024*2];
		//if (m_sDbSecure.IsEmpty())
		//{
		//	sprintf(lpszCommand,"%s -s -d -r --host %s --port %d --username postgres %s",sPGCreateUserExe,m_sDbIp,m_nDbPort,m_sDbAccount);
		//}else
		{
			sprintf(lpszCommand,"createuser.exe -s -d -r -P --host %s --port %d --username postgres %s",m_sDbIp,m_nDbPort,m_sDbAccount);
			//sprintf(lpszCommand,"%s -s -d -r -P --host %s --port %d --username postgres %s",sPGCreateUserExe,m_sDbIp,m_nDbPort,m_sDbAccount);
		}
		//WinExec(lpszCommand,SW_SHOW);
		CString sString;
		ExecDosCmd(NULL,lpszCommand,sString);
		if (sString.IsEmpty())
		{
			AfxMessageBox(_T("数据库帐号创建成功！"));
		}else
		{
			AfxMessageBox(UTF82ACP((LPCTSTR)sString).c_str());
		}
	}else if (nDBType==1)	// mysql
	{
		if (m_sDbName.IsEmpty())
		{
			this->GetDlgItem(IDC_EDIT_DB_NAME)->SetFocus();
			AfxMessageBox(_T("请输入数据库名称！"));
			return;
		}else if (m_sDbName.Find(_T("."))>=0 || m_sDbName.Find(_T("-"))>=0)
		{
			this->GetDlgItem(IDC_EDIT_DB_NAME)->SetFocus();
			AfxMessageBox(_T("数据库名称带有特殊字符[.-]，会导致导入数据失败，请更换数据库名称！"));
			return;
		}
		if (m_sDbSecure.IsEmpty())
		{
			this->GetDlgItem(IDC_EDIT_DB_SECURE)->SetFocus();
			AfxMessageBox(_T("请输入数据库密码！"));
			return;
		}

		GetRootPassword();
		const CString sEXEPath = m_sDBDir.IsEmpty()?theApp.GetAppPath()+"\\mysql":m_sDBDir;
		const CString sMySQLExeFile(sEXEPath+_T("\\mysql.exe"));
		if (!PathFileExists(sMySQLExeFile))
		{
			CString stext;
			stext.Format(_T("%s文件不存在，请重新设置MySQL数据库bin目录！"),sMySQLExeFile);
			AfxMessageBox(stext);
			return;
		}
		InstallDbFirst();
		CString stext;
		stext.Format(_T("你确定新建数据库帐号 %s 吗？"),m_sDbAccount);
		if (MessageBox(stext,_T("新建数据库帐号"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
			return;
		// CREATE USER 'username'@'host' IDENTIFIED BY 'password';
		// GRANT ALL ON picture.* TO 'username'@'host' IDENTIFIED BY "password";
		SetCurrentDirectory(sEXEPath);
		char lpszCommand[1024*2];
		if (m_sRootPassword.IsEmpty())
		{
			sprintf(lpszCommand,"mysql.exe -uroot -h%s -P%d -e\"GRANT ALL ON %s.* TO '%s'@'%%' IDENTIFIED BY '%s'\"",m_sDbIp,m_nDbPort,m_sDbName,m_sDbAccount,m_sDbSecure);
			//sprintf(lpszCommand,"%s -uroot -h%s -e\"GRANT ALL ON %s.* TO '%s'@'%%' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbSecure);
			//WinExec(lpszCommand,SW_HIDE);

			//sprintf(lpszCommand,"%s -uroot -h%s -e\"GRANT ALL ON %s.* TO '%s'@'localhost' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbSecure);
			//WinExec(lpszCommand,SW_HIDE);
			//sprintf(lpszCommand,"%s -uroot -h%s -e\"GRANT ALL ON %s.* TO '%s'@'%s' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbIp,m_sDbSecure);
			//WinExec(lpszCommand,SW_SHOW);
		}else
		{
			sprintf(lpszCommand,"mysql.exe -uroot -p%s -h%s -P%d -e\"GRANT ALL ON %s.* TO '%s'@'%%' IDENTIFIED BY '%s'\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,m_sDbAccount,m_sDbSecure);
			//sprintf(lpszCommand,"%s -uroot -p%s -h%s -e\"GRANT ALL ON %s.* TO '%s'@'%%' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sRootPassword,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbSecure);
			//WinExec(lpszCommand,SW_HIDE);

			//sprintf(lpszCommand,"%s -uroot -p%s -h%s -e\"GRANT ALL ON %s.* TO '%s'@'localhost' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sRootPassword,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbSecure);
			//WinExec(lpszCommand,SW_HIDE);
			//sprintf(lpszCommand,"%s -uroot -p%s -h%s -e\"GRANT ALL ON %s.* TO '%s'@'%s' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sRootPassword,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbIp,m_sDbSecure);
			//WinExec(lpszCommand,SW_SHOW);
		}
		CString sString;
		ExecDosCmd(NULL,lpszCommand,sString,false);
		if (sString.IsEmpty() || sString.Find(_T("Warning:"))>=0)
		{
			AfxMessageBox(_T("数据库帐号创建成功！"));
		}else
		{
			AfxMessageBox(sString);
		}
	}
	SetCurrentDirectory(lpszCurrentDirectory);
}
void Cebs_config_winDlg::RebuildPgBatPath(const CString& sRebuildFile)
{
	const int const_max_buffer_size = 1024*10;
	char lpszBuffer[const_max_buffer_size];
	FILE * f = fopen(sRebuildFile,"r+");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"PGHOME=");
		if (find == NULL)
		{
			return;
		}
		// 重建文件
		f = fopen(sRebuildFile,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		sPathString.Format(_T("PGHOME=%s\\pgsql\nset PATH="),theApp.GetAppUpPath());
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"PATH=");
		if (find != NULL)
		{
			// 保存后面内容
			find += 5;
			fwrite(find,1,strlen(find),f);
		}
		fclose(f);
	}
}
void Cebs_config_winDlg::RebuildPgHBAFile(void)
{
	const int const_max_buffer_size = 1024*10;
	char lpszBuffer[const_max_buffer_size];
	const CString sPgHBAFile = theApp.GetAppUpPath()+_T("/pgsql/data/pg_hba.conf");
	const CString sAddContent = _T("host    all    all    0.0.0.0/0    md5");
	FILE * f = fopen(sPgHBAFile,"r+");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"sAddContent");
		if (find != NULL)
		{
			// 已经存在；
			return;
		}

		// 重建文件
		f = fopen(sPgHBAFile,"w");
		fwrite(lpszBuffer,1,r,f);
		fwrite(sAddContent,1,sAddContent.GetLength(),f);
		fclose(f);
	}
}
void Cebs_config_winDlg::RebuildMySQLPath(const CString& sDBPath)
{
	const int const_max_buffer_size = 1024*10;
	char lpszBuffer[const_max_buffer_size];
	FILE * f = fopen(sDBPath+"\\my.ini","r+");
	if (f == NULL)
		return;
	else
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"basedir=");
		if (find == NULL)
		{
			return;
		}
		// 重建文件
		f = fopen(sDBPath+"\\my.ini","w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		sPathString.Format(_T("basedir=%s\ndatadir=%s\\data"),sDBPath,sDBPath);
		fwrite(sPathString,1,sPathString.GetLength(),f);
		find = strstr(find,"\\data");
		if (find != NULL)
		{
			// 保存后面内容
			find += 5;
			fwrite(find,1,strlen(find),f);
		}
		fclose(f);
	}
	// 直接重写文件内容
	f = fopen(sDBPath+"\\service-inst.bat","w");
	if (f != NULL)
	{
		CString sPathString;
		sPathString.Format(_T("set MYSQL_HOME=%s\nset PATH=%%MYSQL_HOME%%\\bin;%%path%%\nmysqld install EB-MySQL --defaults-file=\"%s\\my.ini\""),
			sDBPath,sDBPath);
		fwrite(sPathString,1,sPathString.GetLength(),f);
		fclose(f);
	}
	// 直接重写文件内容
	f = fopen(sDBPath+"\\service-uninst.bat","w");
	if (f != NULL)
	{
		CString sPathString;
		sPathString.Format(_T("set MYSQL_HOME=%s\nset PATH=%%MYSQL_HOME%%\\bin;%%path%%\nmysqld remove EB-MySQL"),sDBPath);
		fwrite(sPathString,1,sPathString.GetLength(),f);
		fclose(f);
	}
}

void Cebs_config_winDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_FIND_DB_TYPE)
	{
		KillTimer(nIDEvent);

		//if (FindPGDir(theApp.GetAppUpPath(),m_sDBDir))
		//{
		//	m_bIntegrateDb = true;
		//	m_comboDBType.SetCurSel(0);
		//	OnEnChangeEditDbAccount();
		//	RebuildPgBatPath(theApp.GetAppUpPath()+_T("/pgsql/inst.bat"));
		//	RebuildPgBatPath(theApp.GetAppUpPath()+_T("/pgsql/service-inst.bat"));
		//	RebuildPgBatPath(theApp.GetAppUpPath()+_T("/pgsql/service-uninst.bat"));
		//	RebuildPgBatPath(theApp.GetAppUpPath()+_T("/pgsql/start.bat"));
		//	RebuildPgBatPath(theApp.GetAppUpPath()+_T("/pgsql/stop.bat"));
		//}else if (FindMySQLDir(theApp.GetAppUpPath(),m_sDBDir))
		//{
		//	m_bIntegrateDb = true;
		//	m_comboDBType.SetCurSel(1);
		//	OnCbnSelchangeComboDbtype();
		//	OnEnChangeEditDbAccount();
		//	const CString sDBPath = m_sDBDir.Left(m_sDBDir.GetLength()-4);	// 4='\\bin'
		//	RebuildMySQLPath(sDBPath);
		////}else if (!FindPGDir("c:\\",m_sDBDir))
		////{
		////	//FindPGDir("d:\\",m_sDBDir);
		////	//{
		////	//	FindPGDir("e:\\",m_sDBDir);
		////	//}
		//}
		//if (!m_sDBDir.IsEmpty())
		//{
		//	if (m_nDbPort==0)
		//		m_nDbPort = 5432;
		//	UpdateData(FALSE);
		//}
		// 
		//const CString sFindDir = "F:\\git\\oschina\\entboost-1.17.0-win\\install\\*.*";
		const CString sFindDir = theApp.GetAppPath()+"\\*.*";
		CFileFind pFileFind;
		BOOL ret = pFileFind.FindFile(sFindDir);
		while (ret)
		{
			ret = pFileFind.FindNextFile();
			if (pFileFind.IsDots()) continue;
			if (pFileFind.IsDirectory()) continue;

			const CString sFileName = pFileFind.GetFileName();
			if ((m_comboDBType.GetCurSel()==0&&sFileName.Find(".backup")>0) || 
				(m_comboDBType.GetCurSel()==1&&sFileName.Find(".sql")>0) ||
				(m_comboDBType.GetCurSel()==2&&sFileName=="entboost-sqlite"))
			{
				m_sDbBackupFile = sFileName;
				m_sDbBackupPath = pFileFind.GetFilePath();
				UpdateData(FALSE);
				break;
			}
		}
		pFileFind.Close();
	}

	CDialog::OnTimer(nIDEvent);
}

CString GetBrowseFolder(HWND pParent,CString wndTitle)
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
//void Cebs_config_winDlg::OnBnClickedButtonSetPgBinPath()
//{
//	const CString sDBBinDir = GetBrowseFolder(this->GetSafeHwnd(),_T("请选择PostgreSQL或MySQL数据库安装bin目录"));
//	if (PathFileExists(sDBBinDir+_T("\\createuser.exe")))
//	{
//		m_comboDBType.SetCurSel(0);
//	}else if (PathFileExists(sDBBinDir+_T("\\mysql.exe")))
//	{
//		m_comboDBType.SetCurSel(1);
//	}else
//	{
//		m_comboDBType.SetCurSel(2);
//		//AfxMessageBox(_T("使用恩布默认内置数据库！"));
//		//return;
//	}
//	m_sDBDir = sDBBinDir;
//	UpdateData(FALSE);
//	OnCbnSelchangeComboDbtype();
//
//	//CFileDialog dlg(TRUE, m_sDBDir, "createuser.exe", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, this);
//	////dlg.m_sf
//	//if (dlg.DoModal() == IDOK)
//	//{
//	//	m_sDBDir = dlg.GetFileName();
//	//	UpdateData(FALSE);
//
//	//	//POSITION pos = dlg.GetStartPosition(); 
//	//	//while (pos)
//	//	//{
//	//	//	CString sPathName = dlg.GetNextPathName(pos);
//	//	//	if (this->m_pDlgChatInput != NULL)
//	//	//		m_pDlgChatInput->SendFile(sPathName);
//	//	//	//if (pos != NULL)
//	//	//	//{
//	//	//	//	Sleep(1000);
//	//	//	//}
//	//	//}
//	//}
//}

void Cebs_config_winDlg::OnBnClickedButtonSetBackupFile()
{
	CFileDialog dlg(TRUE, 0, m_sDbBackupPath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, this);
	if (dlg.DoModal() == IDOK)
	{
		m_sDbBackupFile = dlg.GetFileName();
		m_sDbBackupPath = dlg.GetPathName();
		UpdateData(FALSE);

		//m_sDBDir = dlg.GetFileName();
		//UpdateData(FALSE);

		//POSITION pos = dlg.GetStartPosition(); 
		//while (pos)
		//{
		//	CString sPathName = dlg.GetNextPathName(pos);
		//	if (this->m_pDlgChatInput != NULL)
		//		m_pDlgChatInput->SendFile(sPathName);
		//	//if (pos != NULL)
		//	//{
		//	//	Sleep(1000);
		//	//}
		//}
	}
}

void Cebs_config_winDlg::WinExecX(const char * lpszFile,const char* lpParameters,UINT uCmdShow,const char* lpDirectory)
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

bool Cebs_config_winDlg::CheckRunProgram(const CString& sProgramName) const
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  
	if (hProcessSnap==NULL) return false;
	PROCESSENTRY32 pe32;  
	memset(&pe32,0,sizeof(pe32));
	pe32.dwSize=sizeof(PROCESSENTRY32);  
	bool bExistApp = false;
	if(::Process32First(hProcessSnap,&pe32))  
	{  
		do  
		{ 
			if (sProgramName==pe32.szExeFile)
			{
				bExistApp = true;
				break;
			}
		}  
		while(::Process32Next(hProcessSnap,&pe32));   
	}
	CloseHandle(hProcessSnap);
	return bExistApp;
}

void Cebs_config_winDlg::InstallDbFirst(void)
{
	return;
	//if (m_bIntegrateDb)
	//{
	//	const int nDBType = m_comboDBType.GetCurSel();
	//	if (nDBType==0)	// pgsql
	//	{
	//		// 集成PostgreSQL数据库
	//		const CString sPgDataPath = theApp.GetAppUpPath()+_T("\\pgsql\\data");
	//		if (!PathFileExists(sPgDataPath))
	//		{
	//			CString stext;
	//			stext.Format(_T("当前版本集成PostgreSQL数据库，是否立即安装并启动PostgreSQL数据库服务？"));
	//			if (MessageBox(stext,_T("安装数据库服务"),MB_YESNO|MB_ICONQUESTION)==IDYES)
	//			{
	//				const CString sPgInstFile = theApp.GetAppUpPath()+_T("\\pgsql\\inst.bat");
	//				WinExecX(sPgInstFile,"",SW_SHOW);
	//				RebuildPgHBAFile();
	//				//const CString sPgStartFile = theApp.GetAppUpPath()+_T("\\pgsql\\service-start.bat");
	//				// 集成数据库，必须用命令行形式打开；因为Windows服务，需要设置登录帐号，初始时会启动失败；
	//				const CString sPgStartFile = theApp.GetAppUpPath()+_T("\\pgsql\\start.bat");
	//				WinExecX(sPgStartFile,"",SW_SHOW);

	//				// createuser -s postgres
	//				const CString sPGCreateUserExe = m_sDBDir+_T("\\createuser.exe");
	//				char lpszCommand[1024];
	//				sprintf(lpszCommand,"-s postgres");
	//				WinExecX(sPGCreateUserExe,lpszCommand,SW_SHOW);

	//				//2. 通过命令行添加 postgres 用户，如下：
	//				//C:\>net user postgres pass /add /expires:never /passwordchg:no
	//				//上面的命令，创建 postgres 用户，密码为 pass，用户帐号不过期，用户不能修改密码。
	//				//
	//				//用户创建成功后，将用户从Users组里删除，如下：
	//				//C:\>net localgroup users postgres /delete
	//				//
	//				//3. 创建 PostgreSQL 服务，命令如下：（假设我们的数据库创建在 E:\pgsql\data下，创建数据库用 createdb 命令）
	//				//D:\Program Files\PostgreSQL\8.2\bin>pg_ctl register -N PostgreSQL -U postgres -P pass -D E:\pgsql\data
	//				//
	//				//4. 指定 postgres 用户的访问权限，如下：
	//				//D:\Program Files\PostgreSQL\8.2>cacls . /T /E /P postgres:R
	//				//上面的命令，设置 postgres 用户对 D:\Program Files\PostgreSQL\8.2 目录可读。
	//				//
	//				//E:\pgsql>cacls . /T /E /P postgres:R
	//				//E:\pgsql>cacls data /T /E /P postgres:C
	//				//上面的命令，设置 postgres 用户对 E:\pgsql\data 目录可更改（可写）

	//			}
	//		}else if (!CheckRunProgram("postgres.exe"))
	//		{
	//			CString stext;
	//			stext.Format(_T("PostgreSQL数据库服务未启动，是否立即启动？"));
	//			if (MessageBox(stext,_T("启动数据库服务"),MB_YESNO|MB_ICONQUESTION)==IDYES)
	//			{
	//				//const CString sPgStartFile = theApp.GetAppUpPath()+_T("\\pgsql\\service-start.bat");
	//				const CString sPgStartFile = theApp.GetAppUpPath()+_T("\\pgsql\\start.bat");
	//				WinExecX(sPgStartFile,"",SW_SHOW);
	//			}
	//		}
	//	}else if (nDBType==1)	// mysql
	//	{
	//		// 集成MYSQL数据库
	//		const CString sDBPath = m_sDBDir.Left(m_sDBDir.GetLength()-4);	// 4='\\bin'
	//		if (!CheckRunProgram("mysqld.exe"))
	//		{
	//			CString stext;
	//			stext.Format(_T("当前版本集成MySQL数据库，是否立即安装数据库服务并立即启动？"));
	//			if (MessageBox(stext,_T("启动数据库服务"),MB_YESNO|MB_ICONQUESTION)==IDYES)
	//			{
	//				//const CString sUnInstFile = sDBPath+_T("\\service-uninst.bat");
	//				//WinExecX(sUnInstFile,"",SW_HIDE);
	//				const CString sInstFile = sDBPath+_T("\\service-inst.bat");
	//				WinExecX(sInstFile,"",SW_HIDE);
	//				const CString sStartFile = sDBPath+_T("\\service-start.bat");
	//				WinExecX(sStartFile,"",SW_SHOW);
	//			}
	//		}
	//	}
	//}
}

void Cebs_config_winDlg::GetRootPassword(void)
{
	if (!m_bGetRootPassword)
	{
		CString sDatabaseAccount;
		this->GetDlgItem(IDC_EDIT_DB_ACCOUNT)->GetWindowText(sDatabaseAccount);
		const int nDBType = m_comboDBType.GetCurSel();
		if (nDBType==0)	// pgsql
		{
			// PostgreSQL
			if (sDatabaseAccount==_T("postgres"))
			{
				this->GetDlgItem(IDC_EDIT_DB_SECURE)->GetWindowText(m_sRootPassword);
				m_bGetRootPassword = true;
				return;
			}
		}else if (nDBType==1)	// mysql
		{
			// MySQL
			if (sDatabaseAccount==_T("root"))
			{
				this->GetDlgItem(IDC_EDIT_DB_SECURE)->GetWindowText(m_sRootPassword);
				m_bGetRootPassword = true;
				return;
			}
		}else
		{
			return;
		}

		CDlgRootPassword dlg;
		if (dlg.DoModal()==IDOK)
		{
			m_sRootPassword = dlg.m_sRootPassword;
			m_bGetRootPassword = true;
		}
	}
}

#define USES_NEW_PG

bool Cebs_config_winDlg::InitDb(bool bChangeIpOnly)
{
	const int nDBType = m_comboDBType.GetCurSel();
	if (nDBType!=2)	// pgsql,mysql
	{
		if (!bChangeIpOnly)
		{
			if (!PathFileExists(m_sDbBackupPath))
			{
				this->GetDlgItem(IDC_EDIT_DB_BACKUP)->SetFocus();
				AfxMessageBox(_T("请输入（或选择）备份数据库文件！"));
				return false;
			//}else if (nDBType==1 && m_sDbBackupPath.Find(_T(" "))>=0)
			//{
			//	this->GetDlgItem(IDC_EDIT_DB_BACKUP)->SetFocus();
			//	CString stext;
			//	stext.Format(_T("备份文件目录存在空格，会导致导入数据失败，请重新选择！\n%s"),m_sDbBackupPath);
			//	AfxMessageBox(stext);
			//	return false;
			}
		}
		if (m_sDbName.IsEmpty())
		{
			this->GetDlgItem(IDC_EDIT_DB_NAME)->SetFocus();
			AfxMessageBox(_T("请输入数据库名称！"));
			return false;
		}
		if (m_sDbAccount.IsEmpty())
		{
			this->GetDlgItem(IDC_EDIT_DB_ACCOUNT)->SetFocus();
			AfxMessageBox(_T("请输入数据库帐号！"));
			return false;
		}
	}
	if (!bChangeIpOnly)
	{
		if (m_sCompanyName.IsEmpty())
		{
			this->GetDlgItem(IDC_EDIT_COMPANY_NAME)->SetFocus();
			AfxMessageBox(_T("请输入公司名称！"));
			return false;
		}
		if (m_sAdminAccount.IsEmpty())
		{
			this->GetDlgItem(IDC_EDIT_ADMIN_ACCOUNT)->SetFocus();
			AfxMessageBox(_T("请输入公司管理员邮箱帐号！"));
			return false;
		}else if (m_sAdminAccount.Find(_T("@"))<0)
		{
			this->GetDlgItem(IDC_EDIT_ADMIN_ACCOUNT)->SetFocus();
			AfxMessageBox(_T("管理员邮箱格式错误，请重新输入！"));
			return false;
		}
		if (m_sAdminSecure.IsEmpty())
		{
			this->GetDlgItem(IDC_EDIT_ADMIN_SECURE)->SetFocus();
			AfxMessageBox(_T("请输入公司管理员帐号密码！"));
			return false;
		}
	}

	//CString sServerAddress;
	//int nFind = m_sServerAddress.Find(_T(":"));
	//if (nFind > 0)
	//{
	//	sServerAddress = m_sServerAddress.Left(nFind);
	//}
	//if (sServerAddress.IsEmpty())
	if (m_sServerAddress.IsEmpty())
	{
		m_pServerIpList.SetFocus();
		//this->GetDlgItem(IDC_EDIT_SERVER_IP)->SetFocus();
		AfxMessageBox(_T("请输入服务器地址！"));
		return false;
	}

	const std::string sServerIp((LPCTSTR)m_sServerAddress);
	//const std::string sServerIp((LPCTSTR)sServerAddress);
	//const std::string sServerIp = GetHostIp(sServerAddress,sServerAddress);
	if (!bChangeIpOnly)
		InstallDbFirst();
	
	//AfxMessageBox(sServerIp.c_str());
	//return;

	//mycp::MD5 md51;
	//std::string strtemp(m_sAdminAccount);
	//strtemp.append(m_sAdminSecure);
	//md51.update((const unsigned char*)strtemp.c_str(),strtemp.size());
	//md51.finalize();
	//const std::string sPwd1 = md51.hex_digest();
	entboost::MD5 md52;
	std::string strtemp = "80";
	strtemp.append(m_sAdminSecure);
	md52.update((const unsigned char*)strtemp.c_str(),strtemp.size());
	md52.finalize();
	const std::string sPwd2 = md52.hex_digest();

	char lpszCurrentDirectory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,lpszCurrentDirectory);

	char lpszCommand[1024*5];
	CString sString;
	if (nDBType==0)	// pgsql
	{
		//if (m_sDbName.Find(_T("."))>=0 || m_sDbName.Find(_T("-"))>=0)
		//{
		//	this->GetDlgItem(IDC_EDIT_DB_NAME)->SetFocus();
		//	AfxMessageBox(_T("数据库名称带有特殊字符[.-]，会导致导入数据失败，请更换数据库名称！"));
		//	return;
		//}
		const CString sEXEPath = m_sDBDir.IsEmpty()?theApp.GetAppPath()+"\\pgsql":m_sDBDir;
		const CString sPGCreateDbExe(sEXEPath+_T("\\createdb.exe"));
		if (!PathFileExists(sPGCreateDbExe))
		{
			CString stext;
			stext.Format(_T("%s文件不存在，请重新设置PostgreSQL数据库安装bin目录！"),sPGCreateDbExe);
			AfxMessageBox(stext);
			return false;
		}

		CString stext;
		if (bChangeIpOnly)
		{
			stext.Format(_T("你确定修改 %s 数据库，服务器IP地址：%s 吗？"),m_sDbName,m_sServerAddress);
			if (MessageBox(stext,_T("修改服务器IP地址"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
				return false;
		}else
		{
			stext.Format(_T("你确定新建 %s 数据库，设置服务器IP地址：%s，公司名称：%s，管理员帐号：%s，并导入初始化数据吗？"),
				m_sDbName,m_sServerAddress,m_sCompanyName,m_sAdminAccount);
			if (MessageBox(stext,_T("初始化数据库"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
				return false;
		}
		SetCurrentDirectory(sEXEPath);

		if (!bChangeIpOnly)
		{
#ifndef USES_NEW_PG
			//if (!m_sDbSecure.IsEmpty())
			{
				sprintf(lpszCommand,"set PGPASSWORD=%s",m_sDbSecure);
				ExecDosCmd(NULL,lpszCommand,sString);
			}
#endif
#ifdef USES_NEW_PG
			sprintf(lpszCommand,"set PGPASSWORD=%s&createdb.exe --host %s --port %d --encoding=UTF8 --username %s --owner %s %s",m_sDbSecure,m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbAccount,m_sDbName);
#else
			sprintf(lpszCommand,"createdb.exe --host %s --port %d --encoding=UTF8 --username %s --owner %s %s",m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbAccount,m_sDbName);
#endif
			//if (m_sDbSecure.IsEmpty())
			//	sprintf(lpszCommand,"createdb.exe --host %s --port %d --encoding=UTF8 --username %s --owner %s %s",m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbAccount,m_sDbName);
			//else
			//{
			//	//sprintf(lpszCommand,"createdb.exe --host %s --port %d --encoding=UTF8 --username %s --password --owner %s %s",m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbAccount,m_sDbName);
			//	sprintf(lpszCommand,"set PGPASSWORD=%s & createdb.exe --host %s --port %d --encoding=UTF8 --username %s  --password --owner %s %s",
			//		m_sDbSecure,m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbAccount,m_sDbName);
			//}
			ExecDosCmd(NULL,lpszCommand,sString,false);
			if (sString.IsEmpty())
			{
			}else
			{
				SetCurrentDirectory(lpszCurrentDirectory);
				AfxMessageBox(UTF82ACP(sString).c_str());
				return false;
			}

#ifdef USES_NEW_PG
			sprintf(lpszCommand,"set PGPASSWORD=%s&pg_restore.exe --host %s --port %d --username %s --dbname %s --verbose -O \"%s\"",m_sDbSecure,m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbName,m_sDbBackupPath);
			//sString = "";
			ExecDosCmd(NULL,lpszCommand,sString,false);
			//if (sString.IsEmpty())
			//{
			//}else
			//{
			//	SetCurrentDirectory(lpszCurrentDirectory);
			//	AfxMessageBox(UTF82ACP(sString).c_str());
			//	return false;
			//}
#else
			// C:/Program Files (x86)/PostgreSQL/8.4/bin\pg_restore.exe --host 14.17.65.113 --port 5432 --username postgres --dbname entboost7 --verbose "F:\tn\POP\back\pgdata\entboost-173-full.backup"
			const CString sPGRestoreExe(sEXEPath+_T("\\pg_restore.exe"));
			sprintf(lpszCommand,"--host %s --port %d --username %s --dbname %s --verbose -O \"%s\"",m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbName,m_sDbBackupPath);
			//if (m_sDbSecure.IsEmpty())
			//	sprintf(lpszCommand,"--host %s --port %d --username %s --dbname %s --verbose -O \"%s\"",m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbName,m_sDbBackupPath);
			//else
			//	sprintf(lpszCommand,"--host %s --port %d --username %s --password --dbname %s --verbose -O \"%s\"",m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbName,m_sDbBackupPath);
			WinExecX(sPGRestoreExe,lpszCommand,SW_SHOW);
			//if (m_sDbSecure.IsEmpty())
			//	sprintf(lpszCommand,"%s --host %s --port %d --username %s --dbname %s --verbose \"%s\"",sPGRestoreExe,m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbName,m_sDbBackupPath);
			//else
			//	sprintf(lpszCommand,"%s --host %s --port %d --username %s --password --dbname %s --verbose \"%s\"",sPGRestoreExe,m_sDbIp,m_nDbPort,m_sDbAccount,m_sDbName,m_sDbBackupPath);
			//ExecDosCmd(NULL,lpszCommand,sString);
			//if (sString.IsEmpty())
			//{
			//}else
			//{
			//	AfxMessageBox(sString);
			//	return;
			//}
#endif
		}

#ifndef USES_NEW_PG
		const CString sPGSqlExe(sEXEPath+_T("\\psql.exe"));
#endif
		CString sSQL;
		if (!bChangeIpOnly)
		{
			sprintf(lpszCommand,"UPDATE eb_system_info_t SET sys_value='%d' where sys_key=8;",nDBType);
			sSQL += lpszCommand;
		}
		sprintf(lpszCommand,"UPDATE service_info_t SET sdomain='%s' where enable=1;",sServerIp.c_str());
		sSQL += lpszCommand;
		sprintf(lpszCommand,"UPDATE service_info_t SET sdomain='%s:17022,17032' where ser_type=13;",sServerIp.c_str());
		//sprintf(lpszCommand,"UPDATE service_info_t SET address='%s:3101;%s:3501' where ser_type=13;",sServerIp.c_str(),sServerIp.c_str());
		sSQL += lpszCommand;
		sprintf(lpszCommand,"UPDATE service_info_t SET sdomain='%s:17012' where ser_type=14;",sServerIp.c_str());
		sSQL += lpszCommand;
		if (bChangeIpOnly)
		{
			sprintf(lpszCommand,"UPDATE enterprise_emo_t SET ver=ver+1;");
			sSQL += lpszCommand;
		}

#ifndef USES_NEW_RESOURCE_INFO
		sprintf(lpszCommand,"UPDATE resource_info_t SET cm_server='%s';",m_sServerAddress);
		sSQL += lpszCommand;
#endif
		if (!bChangeIpOnly)
		{
//#ifdef USES_NEW_PG
//			sprintf(lpszCommand,"UPDATE enterprise_info_t SET ent_name='%s',email='%s' where create_uid=80;",ACP2UTF8(m_sCompanyName).c_str(),m_sAdminAccount);
//#else
			sprintf(lpszCommand,"UPDATE enterprise_info_t SET ent_name='%s',email='%s' where create_uid=80;",m_sCompanyName,m_sAdminAccount);
//#endif
			sSQL += lpszCommand;
			sprintf(lpszCommand,"UPDATE enterprise_cs_t SET cs_id=%lld,cs_max=1 WHERE ent_id=1000000000000030;",DEFAULT_900_NUMBER);
			sSQL += lpszCommand;
			sprintf(lpszCommand,"UPDATE cs_number_t SET cs_id=%lld,emp_id=5444748691720001 WHERE cs_id=9009301111;",DEFAULT_900_NUMBER);
			sSQL += lpszCommand;
			sprintf(lpszCommand,"UPDATE user_account_t SET account='%s',pwd_u='%s',default_emp_id=5444748691720001,email='%s' where user_id=80;",m_sAdminAccount,sPwd2.c_str(),m_sAdminAccount);
			//sprintf(lpszCommand,"UPDATE user_account_t SET account='%s',password='%s',pwd_u='%s',default_emp_id=5444748691720001 where user_id=80;",m_sAdminAccount,sPwd1.c_str(),sPwd2.c_str());
			sSQL += lpszCommand;
		}
#ifdef USES_NEW_PG
		// set PGCLIENTENCODING=GBK	** 解决中文公司名称长，导入失败问题
		sprintf(lpszCommand,"set PGCLIENTENCODING=GBK&set PGPASSWORD=%s&psql.exe --host %s --port %d --username %s --command \"%s\" %s",m_sDbSecure,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		//sprintf(lpszCommand,"set PGPASSWORD=%s&psql.exe --host %s --port %d --username %s --command \"%s\" %s",m_sDbSecure,m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		sString = "";
		ExecDosCmd(NULL,lpszCommand,sString,false);
		if (sString.IsEmpty() || sString.Find("UPDATE")>=0)
		{
		}else
		{
			SetCurrentDirectory(lpszCurrentDirectory);
			AfxMessageBox(UTF82ACP(sString).c_str());
			return false;
		}
#else
		sprintf(lpszCommand,"--host %s --port %d --username %s --command \"%s\" %s",m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		//if (m_sDbSecure.IsEmpty())
		//	sprintf(lpszCommand,"--host %s --port %d --username %s --command \"%s\" %s",m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		//else
		//	sprintf(lpszCommand,"--host %s --port %d --username %s --password --command \"%s\" %s",m_sDbIp,m_nDbPort,m_sDbAccount,sSQL,m_sDbName);
		WinExecX(sPGSqlExe,lpszCommand,SW_SHOW);
#endif
	}else if (nDBType==1)	// msyql
	{
		if (m_sDbName.Find(_T("."))>=0 || m_sDbName.Find(_T("-"))>=0)
		{
			this->GetDlgItem(IDC_EDIT_DB_NAME)->SetFocus();
			if (bChangeIpOnly)
				AfxMessageBox(_T("数据库名称带有特殊字符[.-]，会导致执行失败，请更换数据库名称！"));
			else
				AfxMessageBox(_T("数据库名称带有特殊字符[.-]，会导致导入数据失败，请更换数据库名称！"));
			return false;
		}

		GetRootPassword();
		// MySQL
		const CString sEXEPath = m_sDBDir.IsEmpty()?theApp.GetAppPath()+"\\mysql":m_sDBDir;
		const CString sMySQLExeFile(sEXEPath+_T("\\mysql.exe"));
		if (!PathFileExists(sMySQLExeFile))
		{
			CString stext;
			stext.Format(_T("%s文件不存在，请重新设置MySQL数据库bin目录！"),sMySQLExeFile);
			AfxMessageBox(stext);
			return false;
		}
		InstallDbFirst();

		CString stext;
		if (bChangeIpOnly)
		{
			stext.Format(_T("你确定修改 %s 数据库，服务器IP地址：%s 吗？"),m_sDbName,m_sServerAddress);
			if (MessageBox(stext,_T("修改服务器IP地址"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
				return false;
		}else
		{
			stext.Format(_T("你确定新建 %s 数据库，设置服务器IP地址：%s，公司名称：%s，管理员帐号：%s，并导入初始化数据吗？"),
				m_sDbName,m_sServerAddress,m_sCompanyName,m_sAdminAccount);
			if (MessageBox(stext,_T("初始化数据库"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
				return false;
		}

		SetCurrentDirectory(sEXEPath);
		// GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,DROP,ALTER ON 数据库名.* TO 数据库名@localhost IDENTIFIED BY '密码';
		// CREATE DATABASE `test2` DEFAULT CHARACTER SET utf8
		// 以下三行，用于增加MYSQL帐号数据库操作权限
		if (m_sRootPassword.IsEmpty())
		{
			if (!bChangeIpOnly)
			{
				sprintf(lpszCommand,"mysql.exe -uroot -h%s -P%d -e\"CREATE DATABASE %s DEFAULT CHARACTER SET utf8\"",m_sDbIp,m_nDbPort,m_sDbName);
				//WinExecX(NULL,lpszCommand,SW_SHOW);
				ExecDosCmd(NULL,lpszCommand,sString,false);
				if (sString.IsEmpty() || 
					(sString.Find(_T("Warning:"))>=0 && sString.Find(_T("ERROR"))<0))
				{
				}else
				{
					SetCurrentDirectory(lpszCurrentDirectory);
					AfxMessageBox(sString);
					return false;
				}

				sprintf(lpszCommand,"mysql.exe -uroot -h%s -P%d -e\"GRANT ALL ON %s.* TO '%s'@'%%' IDENTIFIED BY '%s'\"",m_sDbIp,m_nDbPort,m_sDbName,m_sDbAccount,m_sDbSecure);
				//WinExec(lpszCommand,SW_HIDE);
				ExecDosCmd(NULL,lpszCommand,sString,false);
				if (sString.IsEmpty() || 
					(sString.Find(_T("Warning:"))>=0 && sString.Find(_T("ERROR"))<0))
				{
				}else
				{
					SetCurrentDirectory(lpszCurrentDirectory);
					AfxMessageBox(sString);
					return false;
				}

				//sprintf(lpszCommand,"%s -uroot -h%s -e\"GRANT ALL ON %s.* TO '%s'@'localhost' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbSecure);
				//WinExec(lpszCommand,SW_HIDE);
				//sprintf(lpszCommand,"%s -uroot -h%s -e\"GRANT ALL ON %s.* TO '%s'@'%s' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbIp,m_sDbSecure);
				//WinExec(lpszCommand,SW_HIDE);
				//sprintf(lpszCommand,"mysql.exe -uroot -h%s -P%d -e\"CREATE DATABASE %s DEFAULT CHARACTER SET utf8\"",m_sDbIp,m_nDbPort,m_sDbName);
				////WinExecX(NULL,lpszCommand,SW_SHOW);
				//ExecDosCmd(NULL,lpszCommand,sString,false);
				//if (sString.IsEmpty() || 
				//	(sString.Find(_T("Warning:"))>=0 && sString.Find(_T("ERROR"))<0))
				//{
				//}else
				//{
				//	SetCurrentDirectory(lpszCurrentDirectory);
				//	AfxMessageBox(sString);
				//	return;
				//}

				//sprintf(lpszCommand,"-uroot -h%s -D%s<%s",m_sDbIp,m_sDbName,m_sDbBackupPath);
				//WinExec(lpszCommand,SW_SHOW);
				// ShellExecute
				//ShellExecute(this->GetSafeHwnd(),"open",sMySQLExeFile,lpszCommand,sMySQLExeFile,SW_SHOW);
				//CreateProcess(sMySQLExeFile,lpszCommand,NULL,NULL,FALSE,0,NULL,NULL,NULL,NULL);
				// 可以导入数据库
				//sprintf(lpszCommand,"\"%s\" -uroot -h%s %s<%s",sMySQLExeFile,m_sDbIp,m_sDbName,m_sDbBackupPath);
				//system(lpszCommand);
				sprintf(lpszCommand,"mysql.exe -uroot -h%s -P%d %s < \"%s\"\n",m_sDbIp,m_nDbPort,m_sDbName,m_sDbBackupPath);
				//sprintf(lpszCommand,"mysql.exe -uroot -h%s %s < %s\n",m_sDbIp,m_sDbName,m_sDbBackupPath);
				ExecDosCmd(NULL,lpszCommand,sString,false);
				if (sString.IsEmpty() || 
					(sString.Find(_T("Warning:"))>=0 && sString.Find(_T("ERROR"))<0))
				{
				}else
				{
					SetCurrentDirectory(lpszCurrentDirectory);
					AfxMessageBox(sString);
					return false;
				}

				sprintf(lpszCommand,"-uroot -h%s -P%d -D%s -e\"UPDATE eb_system_info_t SET sys_value='%d' where sys_key=8\"",m_sDbIp,m_nDbPort,m_sDbName,nDBType);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			}
			sprintf(lpszCommand,"-uroot -h%s -P%d -D%s -e\"UPDATE service_info_t SET sdomain='%s' where enable=1\"",
				m_sDbIp,m_nDbPort,m_sDbName,sServerIp.c_str());
			WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			sprintf(lpszCommand,"-uroot -h%s -P%d -D%s -e\"UPDATE service_info_t SET sdomain='%s:17022,17032' where ser_type=13\"",
				m_sDbIp,m_nDbPort,m_sDbName,sServerIp.c_str());
			WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			sprintf(lpszCommand,"-uroot -h%s -P%d -D%s -e\"UPDATE service_info_t SET sdomain='%s:17012' where ser_type=14\"",m_sDbIp,m_nDbPort,m_sDbName,sServerIp.c_str());
			WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			if (bChangeIpOnly)
			{
				sprintf(lpszCommand,"-uroot -h%s -P%d -D%s -e\"UPDATE enterprise_emo_t SET ver=ver+1\"",m_sDbIp,m_nDbPort,m_sDbName);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			}
#ifndef USES_NEW_RESOURCE_INFO
			sprintf(lpszCommand,"-uroot -h%s -P%d -D%s -e\"UPDATE resource_info_t SET cm_server='%s'\"",
				m_sDbIp,m_nDbPort,m_sDbName,m_sServerAddress);
			WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
#endif
			if (!bChangeIpOnly)
			{
				sprintf(lpszCommand,"-uroot -h%s -P%d --default-character-set=utf8 -D%s -e\"UPDATE enterprise_info_t SET ent_name='%s',email='%s' where create_uid=80\"",
					m_sDbIp,m_nDbPort,m_sDbName,m_sCompanyName,m_sAdminAccount);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
				sprintf(lpszCommand,"-uroot -h%s -P%d -D%s -e\"UPDATE enterprise_cs_t SET cs_id=%lld,cs_max=1 WHERE ent_id=1000000000000030\"",
					m_sDbIp,m_nDbPort,m_sDbName,DEFAULT_900_NUMBER);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
				sprintf(lpszCommand,"-uroot -h%s -P%d -D%s -e\"UPDATE cs_number_t SET cs_id=%lld,emp_id=5444748691720001 WHERE cs_id=9009301111\"",
					m_sDbIp,m_nDbPort,m_sDbName,DEFAULT_900_NUMBER);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);

				sprintf(lpszCommand,"-uroot -h%s -P%d -D%s -e\"UPDATE user_account_t SET account='%s',pwd_u='%s',default_emp_id=5444748691720001,email='%s' where user_id=80\"",
					m_sDbIp,m_nDbPort,m_sDbName,m_sAdminAccount,sPwd2.c_str(),m_sAdminAccount);
				//sprintf(lpszCommand,"-uroot -h%s -D%s -e\"UPDATE user_account_t SET account='%s',password='%s',pwd_u='%s',default_emp_id=5444748691720001 where user_id=80\"",
				//	m_sDbIp,m_sDbName,m_sAdminAccount,sPwd1.c_str(),sPwd2.c_str());
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			}
		}else
		{
			if (!bChangeIpOnly)
			{
				sprintf(lpszCommand,"mysql.exe -uroot -p%s -h%s -P%d -e\"CREATE DATABASE %s DEFAULT CHARACTER SET utf8\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName);
				ExecDosCmd(NULL,lpszCommand,sString,false);
				if (sString.IsEmpty() || 
					(sString.Find(_T("Warning:"))>=0 && sString.Find(_T("ERROR"))<0))
				{
				}else
				{
					SetCurrentDirectory(lpszCurrentDirectory);
					AfxMessageBox(sString);
					return false;
				}

				//sprintf(lpszCommand,"\"%s\" -uroot -p%s -h%s -e\"GRANT ALL ON %s.* TO '%s'@'%%' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sRootPassword,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbSecure);
				//WinExec(lpszCommand,SW_HIDE);
				sprintf(lpszCommand,"mysql.exe -uroot -p%s -h%s -P%d -e\"GRANT ALL ON %s.* TO '%s'@'%%' IDENTIFIED BY '%s'\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,m_sDbAccount,m_sDbSecure);
				ExecDosCmd(NULL,lpszCommand,sString,false);
				if (sString.IsEmpty() || 
					(sString.Find(_T("Warning:"))>=0 && sString.Find(_T("ERROR"))<0))
				{
				}else
				{
					SetCurrentDirectory(lpszCurrentDirectory);
					AfxMessageBox(sString);
					return false;
				}
				//sprintf(lpszCommand,"%s -uroot -p%s -h%s -e\"GRANT ALL ON %s.* TO '%s'@'localhost' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sRootPassword,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbSecure);
				//WinExec(lpszCommand,SW_HIDE);
				//sprintf(lpszCommand,"%s -uroot -p%s -h%s -e\"GRANT ALL ON %s.* TO '%s'@'%s' IDENTIFIED BY '%s'\"",sMySQLExeFile,m_sRootPassword,m_sDbIp,m_sDbName,m_sDbAccount,m_sDbIp,m_sDbSecure);
				//WinExec(lpszCommand,SW_HIDE);
				//sprintf(lpszCommand,"-uroot -p%s -h%s -e\"CREATE DATABASE %s DEFAULT CHARACTER SET utf8\"",m_sRootPassword,m_sDbIp,m_sDbName);
				//WinExecX(sMySQLExeFile,lpszCommand,SW_SHOW);
				//sprintf(lpszCommand,"mysql.exe -uroot -p%s -h%s -P%d -e\"CREATE DATABASE %s DEFAULT CHARACTER SET utf8\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName);
				//ExecDosCmd(NULL,lpszCommand,sString,false);
				//if (sString.IsEmpty() || 
				//	(sString.Find(_T("Warning:"))>=0 && sString.Find(_T("ERROR"))<0))
				//{
				//}else
				//{
				//	SetCurrentDirectory(lpszCurrentDirectory);
				//	AfxMessageBox(sString);
				//	return;
				//}

				//sprintf(lpszCommand,"-uroot -h%s -D%s<%s",m_sDbIp,m_sDbName,m_sDbBackupPath);
				//WinExec(lpszCommand,SW_SHOW);
				// ShellExecute
				//ShellExecute(this->GetSafeHwnd(),"open",sMySQLExeFile,lpszCommand,sMySQLExeFile,SW_SHOW);
				//CreateProcess(sMySQLExeFile,lpszCommand,NULL,NULL,FALSE,0,NULL,NULL,NULL,NULL);
				// 可以导入数据库
				//sprintf(lpszCommand,"\"%s\" -uroot -p%s -h%s %s < %s",sMySQLExeFile,m_sRootPassword,m_sDbIp,m_sDbName,m_sDbBackupPath);
				//system(lpszCommand);
				sprintf(lpszCommand,"mysql.exe -uroot -p%s -h%s -P%d %s < \"%s\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,m_sDbBackupPath);
				ExecDosCmd(NULL,lpszCommand,sString,false);
				if (sString.IsEmpty() || 
					(sString.Find(_T("Warning:"))>=0 && sString.Find(_T("ERROR"))<0))
				{
				}else
				{
					SetCurrentDirectory(lpszCurrentDirectory);
					AfxMessageBox(sString);
					return false;
				}

				sprintf(lpszCommand,"-uroot -p%s -h%s -P%d -D%s -e\"UPDATE eb_system_info_t SET sys_value='%d' where sys_key=8\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,nDBType);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			}
			sprintf(lpszCommand,"-uroot -p%s -h%s -P%d -D%s -e\"UPDATE service_info_t SET sdomain='%s' where enable=1\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,sServerIp.c_str());
			WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			sprintf(lpszCommand,"-uroot -p%s -h%s -P%d -D%s -e\"UPDATE service_info_t SET sdomain='%s:17022,17032' where ser_type=13\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,sServerIp.c_str());
			WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			sprintf(lpszCommand,"-uroot -p%s -h%s -P%d -D%s -e\"UPDATE service_info_t SET sdomain='%s:17012' where ser_type=14\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,sServerIp.c_str());
			WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			if (bChangeIpOnly)
			{
				sprintf(lpszCommand,"-uroot -p%s -h%s -P%d -D%s -e\"UPDATE enterprise_emo_t SET ver=ver+1\"",m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			}
#ifndef USES_NEW_RESOURCE_INFO
			sprintf(lpszCommand,"-uroot -p%s -h%s -P%d -D%s -e\"UPDATE resource_info_t SET cm_server='%s'\"",
				m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,m_sServerAddress);
			WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
#endif
			if (!bChangeIpOnly)
			{
				sprintf(lpszCommand,"-uroot -p%s -h%s -P%d --default-character-set=utf8 -D%s -e\"UPDATE enterprise_info_t SET ent_name='%s',email='%s' where create_uid=80\"",
					m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,m_sCompanyName,m_sAdminAccount);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
				sprintf(lpszCommand,"-uroot -p%s -h%s -P%d -D%s -e\"UPDATE enterprise_cs_t SET cs_id=%lld,cs_max=1 WHERE ent_id=1000000000000030\"",
					m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,DEFAULT_900_NUMBER);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
				sprintf(lpszCommand,"-uroot -p%s -h%s -P%d -D%s -e\"UPDATE cs_number_t SET cs_id=%lld,emp_id=5444748691720001 WHERE cs_id=9009301111\"",
					m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,DEFAULT_900_NUMBER);
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);

				sprintf(lpszCommand,"-uroot -p%s -h%s -P%d -D%s -e\"UPDATE user_account_t SET account='%s',pwd_u='%s',default_emp_id=5444748691720001,email='%s' where user_id=80\"",
					m_sRootPassword,m_sDbIp,m_nDbPort,m_sDbName,m_sAdminAccount,sPwd2.c_str(),m_sAdminAccount);
				//sprintf(lpszCommand,"-uroot -p%s -h%s -D%s -e\"UPDATE user_account_t SET account='%s',password='%s',pwd_u='%s',default_emp_id=5444748691720001 where user_id=80\"",
				//	m_sRootPassword,m_sDbIp,m_sDbName,m_sAdminAccount,sPwd1.c_str(),sPwd2.c_str());
				WinExecX(sMySQLExeFile,lpszCommand,SW_HIDE);
			}
		}
	}else
	{
		const CString sSQLiteExeFile(theApp.GetAppPath()+"\\sqlite3\\sqlite3.exe");
		CString sDestToFile;
		sDestToFile.Format(_T("%s/ebs/conf/SqliteService/%s"),theApp.GetAppUpPath(),m_sDbName);

		CString stext;
		if (bChangeIpOnly)
		{
			if (!::PathFileExists(sDestToFile))
			{
				stext.Format(_T("%s 数据库文件不存在，请重新安装！"),m_sDbName);
				AfxMessageBox(stext);
				return false;
			}
			stext.Format(_T("你确定修改 %s 数据库，服务器IP地址：%s 吗？"),m_sDbName,m_sServerAddress);
			if (MessageBox(stext,_T("修改服务器IP地址"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
				return false;

			sprintf(lpszCommand,"\"%s\" \"UPDATE service_info_t SET sdomain='%s' where enable=1\"",sDestToFile,sServerIp.c_str());
			WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
			sprintf(lpszCommand,"\"%s\" \"UPDATE service_info_t SET sdomain='%s:17022,17032' where ser_type=13\"",sDestToFile,sServerIp.c_str());
			WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
			sprintf(lpszCommand,"\"%s\" \"UPDATE service_info_t SET sdomain='%s:17012' where ser_type=14\"",sDestToFile,sServerIp.c_str());
			WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
			if (bChangeIpOnly)
			{
				sprintf(lpszCommand,"\"%s\" \"UPDATE enterprise_emo_t SET ver=ver+1\"",sDestToFile);
				WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
			}
//#ifndef USES_NEW_RESOURCE_INFO
//			sprintf(lpszCommand,"%s \"UPDATE resource_info_t SET cm_server='%s'\"",m_sDbName,m_sServerAddress);
//			WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
//#endif
			return true;
		}
		stext.Format(_T("你确定新建 %s 数据库，设置服务器IP地址：%s，公司名称：%s，管理员帐号：%s，初始化恩布内置数据库数据吗？"),
			m_sDbName,m_sServerAddress,m_sCompanyName,m_sAdminAccount);
		if (MessageBox(stext,_T("初始化数据库"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
			return false;

		//SetCurrentDirectory(m_sDBDir);

		//sprintf(lpszCommand,"mysql.exe -uroot -h%s -P%d %s < %s\n",m_sDbIp,m_nDbPort,m_sDbName,m_sDbBackupPath);
		////sprintf(lpszCommand,"mysql.exe -uroot -h%s %s < %s\n",m_sDbIp,m_sDbName,m_sDbBackupPath);
		//ExecDosCmd(NULL,lpszCommand,sString,false);
		//if (sString.IsEmpty() || sString.Find(_T("Warning:"))>=0)
		//{
		//}else
		//{
		//	SetCurrentDirectory(lpszCurrentDirectory);
		//	AfxMessageBox(sString);
		//	return;
		//}
		
		//const CString sDefaulSqlitetDatabaseFile = m_sDbBackupFile;//_T("entboost-sqlite");
		//AfxMessageBox(m_sDbBackupPath);
		if (!::PathFileExists(m_sDbBackupPath))
		{
			AfxMessageBox(_T("内置默认数据库文件 entboost-sqlite 不存在！"));
			return false;
		}
		if (!CopyFile(m_sDbBackupPath,m_sDbName,TRUE))
		{
			stext.Format(_T("%s 数据库文件已经存在，是否确定替换数据库？"),m_sDbName);
			if (MessageBox(stext,_T("替换数据库"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
			{
				return false;
			}
			CopyFile(m_sDbBackupPath,m_sDbName,FALSE);
		}

		sprintf(lpszCommand,"%s \"UPDATE eb_system_info_t SET sys_value='%d' where sys_key=8\"",m_sDbName,nDBType);
		WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
		sprintf(lpszCommand,"%s \"UPDATE service_info_t SET sdomain='%s' where enable=1\"",m_sDbName,sServerIp.c_str());
		WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
		sprintf(lpszCommand,"%s \"UPDATE service_info_t SET sdomain='%s:17022,17032' where ser_type=13\"",m_sDbName,sServerIp.c_str());
		WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
		sprintf(lpszCommand,"%s \"UPDATE service_info_t SET sdomain='%s:17012' where ser_type=14\"",m_sDbName,sServerIp.c_str());
		WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
#ifndef USES_NEW_RESOURCE_INFO
		sprintf(lpszCommand,"%s \"UPDATE resource_info_t SET cm_server='%s'\"",m_sDbName,m_sServerAddress);
		WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
#endif
		std::string sUTF8CompanyName = ACP2UTF8(m_sCompanyName);
		if (sUTF8CompanyName==(LPCTSTR)m_sCompanyName)
		{
			sprintf(lpszCommand,"%s \"UPDATE enterprise_info_t SET ent_name='%s',email='%s' where create_uid=80\"",m_sDbName,m_sCompanyName,m_sAdminAccount);
			sUTF8CompanyName.clear();
		}else
			sprintf(lpszCommand,"%s \"UPDATE enterprise_info_t SET email='%s' where create_uid=80\"",m_sDbName,m_sAdminAccount);
		//sprintf(lpszCommand,"%s \"UPDATE enterprise_info_t SET ent_name='%s',email='%s' where create_uid=80\"",m_sDbName,ACP2UTF8(m_sCompanyName).c_str(),m_sAdminAccount);
		//sprintf(lpszCommand,"%s \"UPDATE enterprise_info_t SET ent_name='%s',email='%s' where create_uid=80\"",m_sDbName,m_sCompanyName,m_sAdminAccount);
		WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
		sprintf(lpszCommand,"%s \"UPDATE enterprise_cs_t SET cs_id=%lld,cs_max=1 WHERE ent_id=1000000000000030\"",m_sDbName,DEFAULT_900_NUMBER);
		WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
		sprintf(lpszCommand,"%s \"UPDATE cs_number_t SET cs_id=%lld,emp_id=5444748691720001 WHERE cs_id=9009301111\"",m_sDbName,DEFAULT_900_NUMBER);
		WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);

		sprintf(lpszCommand,"%s \"UPDATE user_account_t SET account='%s',pwd_u='%s',default_emp_id=5444748691720001,email='%s' where user_id=80\"",m_sDbName,m_sAdminAccount,sPwd2.c_str(),m_sAdminAccount);
		WinExecX(sSQLiteExeFile,lpszCommand,SW_HIDE);
		if (!CopyFile(m_sDbName,sDestToFile,TRUE))
		{
			stext.Format(_T("%s 数据库文件已经存在，是否确定替换数据库？"),m_sDbName);
			if (MessageBox(stext,_T("替换数据库"),MB_OKCANCEL|MB_ICONQUESTION)!=IDOK)
			{
				DeleteFile(m_sDbName);
				return false;
			}
			const time_t tNow = time(0);
			struct tm *newtime = localtime(&tNow);
			CString sBkFile;
			sBkFile.Format(_T("%s.bk.%04d%02d%02d"),sDestToFile,newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday);
			CopyFile(sDestToFile,sBkFile,FALSE);	// 做一个备份
			if (!CopyFile(m_sDbName,sDestToFile,FALSE))
			{
				DeleteFile(m_sDbName);
				MessageBox(_T("数据导入成功，但复制数据库文件失败，请停止服务器后再试一次！"),_T("复制文件失败"),MB_OK);
				return false;
			}
		}
		DeleteFile(m_sDbName);
		if (!sUTF8CompanyName.empty())
		{
			CString sCopyToFile;
			sCopyToFile.Format(_T("%s/ebs/conf/POPLogonCenter/company_name_utf8_temp"),theApp.GetAppUpPath());
			FILE * f = fopen(sCopyToFile,"w");
			if (f!=NULL)
			{
				fwrite(sUTF8CompanyName.c_str(),1,sUTF8CompanyName.size(),f);
				fclose(f);
			}
		}
	}

	SetCurrentDirectory(lpszCurrentDirectory);
	return true;
}

void Cebs_config_winDlg::OnBnClickedButtonInitDb()
{
	//UpdateData();
	OnBnClickedButtonSaveHttpport();
	if (!SaveDbinfo())
		return;
	if (InitDb(false))
	{
		// .ini
		const CString sEbcSettingIniFile = theApp.GetAppUpPath()+"/ebc/datas/setting.ini";
		char lpszBuffer[128];
		memset(lpszBuffer,0,sizeof(lpszBuffer));
		::GetPrivateProfileString(_T("system"),_T("auto-run-account"),"",lpszBuffer,128,sEbcSettingIniFile);
		if (strlen(lpszBuffer)==0)
		{
			::WritePrivateProfileString(_T("system"),_T("auto-run-account"),m_sAdminAccount,sEbcSettingIniFile);
		}
		CString sString;
		sString.Format(_T("成功导入 %s 数据库！"),m_sDbName);
		MessageBox(sString,_T("导入数据"),MB_OK);
	}
}

void Cebs_config_winDlg::OnBnClickedButtonSaveCsinfo()
{
	const int const_max_buffer_size = 1024*20;
	char lpszBuffer[const_max_buffer_size];
	UpdateData();
	//if (m_sTel.

	// 修改900客服：电话号码
	const CString sOnlineCallJSFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/webim/js/onlinecall.js";
	//const CString sOnlineCallJSFile = theApp.GetAppUpPath()+"/ebs/conf/web/samples/js/onlinecall.js";
	FILE * f = fopen(sOnlineCallJSFile,"r");
	if (f != NULL)
	{
		memset(lpszBuffer,0,const_max_buffer_size);
		size_t r = fread(lpszBuffer,1,const_max_buffer_size,f);
		lpszBuffer[r] = '\0';
		fclose(f);
		char * find = strstr(lpszBuffer,"class=\"sp-tel yahei\"><span>");
		if (find == NULL)
		{
			return;
		}
		// 重建JS文件
		f = fopen(sOnlineCallJSFile,"w");
		fwrite(lpszBuffer,1,find-lpszBuffer,f);
		CString sPathString;
		sPathString.Format(_T("class=\"sp-tel yahei\"><span>电话&nbsp;%s</span>"),m_sTel);
		const std::string sWriteString = ACP2UTF8(sPathString);
		fwrite(sWriteString.c_str(),1,sWriteString.size(),f);
		find = strstr(find,"</span>");
		if (find != NULL)
		{
			// 保存后面内容
			find += 7;
			fwrite(find,1,strlen(find),f);
		}else
		{
			fclose(f);
			sPathString.Format(_T("%s文件格式错误，请替换该文件后再试！"),sOnlineCallJSFile);
			this->SetWindowText(sPathString);
			return;
		}
		fclose(f);
	}

}

void Cebs_config_winDlg::OnCbnSelchangeComboDbtype()
{
	const int nDBType = m_comboDBType.GetCurSel();
	if (nDBType==0)	// pgsql
	{
		//if (m_sDbBackupPath.Replace("entboost.sql","entboost.backup")==0)
		{
			m_sDbBackupPath = theApp.GetAppPath() + "\\entboost.backup";
		}
		m_nDbPort = 5432;
		m_sDbBackupFile = _T("entboost.backup");
		if (m_sDbAccount==_T("root"))
			m_sDbAccount = _T("postgres");
	}else if (nDBType==1)	// mysql
	{
		//if (m_sDbBackupPath.Replace("entboost.backup","entboost.sql")==0)
		{
			m_sDbBackupPath = theApp.GetAppPath() + "\\entboost.sql";
		}
		m_nDbPort = 3306;
		m_sDbBackupFile = _T("entboost.sql");
		if (m_sDbAccount==_T("postgres"))
			m_sDbAccount = _T("root");
	}else if (nDBType==2)	// sqlite
	{
		// 使用默认sqlite数据库；
		m_sDbBackupFile = _T("entboost-sqlite");
		m_sDbBackupPath = theApp.GetAppPath() + "\\entboost-sqlite";
	}
	this->GetDlgItem(IDC_EDIT_DB_IP)->EnableWindow(nDBType!=2?TRUE:FALSE);
	//this->GetDlgItem(IDC_EDIT_DB_NAME)->EnableWindow(nDBType!=2?TRUE:FALSE);
	this->GetDlgItem(IDC_EDIT_DB_PORT)->EnableWindow(nDBType!=2?TRUE:FALSE);
	this->GetDlgItem(IDC_EDIT_DB_ACCOUNT)->EnableWindow(nDBType!=2?TRUE:FALSE);
	//this->GetDlgItem(IDC_BUTTON_SET_PG_BIN_PATH)->EnableWindow(nDBType!=2?TRUE:FALSE);
	//this->GetDlgItem(IDC_BUTTON_NEW_DBACCOUNT)->EnableWindow(nDBType!=2?TRUE:FALSE);
	this->GetDlgItem(IDC_EDIT_DB_BACKUP)->EnableWindow(nDBType!=2?TRUE:FALSE);
	this->GetDlgItem(IDC_BUTTON_SET_BACKUP_FILE)->EnableWindow(nDBType!=2?TRUE:FALSE);
	this->GetDlgItem(IDC_EDIT_DB_SECURE)->EnableWindow(nDBType!=2?TRUE:FALSE);
	//this->GetDlgItem(IDC_BUTTON_INIT_DB)->EnableWindow(nDBType==0?TRUE:FALSE);
	UpdateData(FALSE);
}

void Cebs_config_winDlg::OnEnChangeEditDbAccount()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//CString sDatabaseAccount;
	//this->GetDlgItem(IDC_EDIT_DB_ACCOUNT)->GetWindowText(sDatabaseAccount);
	//const int nDBType = m_comboDBType.GetCurSel();
	//if (nDBType==0)	// pgsql
	//{
	//	// PostgreSQL
	//	this->GetDlgItem(IDC_BUTTON_NEW_DBACCOUNT)->EnableWindow(sDatabaseAccount!=_T("postgres"));
	//}else if (nDBType==1)	// mysql
	//{
	//	// MySQL
	//	this->GetDlgItem(IDC_BUTTON_NEW_DBACCOUNT)->EnableWindow(sDatabaseAccount!=_T("root"));
	//}
}

void Cebs_config_winDlg::OnBnClickedButtonDbTest()
{
	UpdateData();
	if (m_sDbIp.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_DB_IP)->SetFocus();
		AfxMessageBox(_T("请输入数据库地址！"));
		return;
	}
	if (m_nDbPort<=0)
	{
		this->GetDlgItem(IDC_EDIT_DB_PORT)->SetFocus();
		AfxMessageBox(_T("请输入数据库端口！"));
		return;
	}
	if (m_sDbAccount.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_DB_ACCOUNT)->SetFocus();
		AfxMessageBox(_T("请输入数据库帐号！"));
		return;
	}
	if (m_sDbSecure.IsEmpty())
	{
		this->GetDlgItem(IDC_EDIT_DB_SECURE)->SetFocus();
		AfxMessageBox(_T("请输入数据库密码！"));
		return;
	}

}

void Cebs_config_winDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

void Cebs_config_winDlg::OnBnClickedButtonChangeIp()
{
	//UpdateData();
	OnBnClickedButtonSaveHttpport();
	if (!SaveClientInfo(true))
		return;

	if (InitDb(true))
	{
		TarClientZipFile();
		CString sString;
		sString.Format(_T("成功修改服务器IP地址！"));
		MessageBox(sString,_T("修改服务器IP地址"),MB_OK);
	}
}

void Cebs_config_winDlg::OnBnClickedCheckAutoChangeip()
{
	m_bAutoChangeIp = !m_bAutoChangeIp;

	CString sSettingFile;
	sSettingFile.Format(_T("%s/ebs/conf/POPLogonCenter/enable_auto_detect_server_ip"),theApp.GetAppUpPath());
	if (m_bAutoChangeIp)
	{
		if (!::PathFileExists(sSettingFile))
		{
			FILE * f = fopen(sSettingFile,"w");
			if (f==NULL)
			{
				m_bAutoChangeIp = FALSE;
			}else
			{
				fclose(f);
			}
		}
	}else
	{
		if (::PathFileExists(sSettingFile))
			DeleteFile(sSettingFile);
	}
	UpdateData(FALSE);
}

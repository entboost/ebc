// chatroom.cpp : Defines the exported functions for the DLL application.
#ifdef _MSC_VER
#pragma warning(disable:4018 4819)
#endif // WIN32

//#ifdef WIN32
#include "stdafx.h"
//#endif

#include "UserManagerApp.h"
#include "../include/md5.h"
#include "../include/EBParseSetting.h"
#ifndef _QT_MAKE_
//#ifdef _MSC_VER //WIN32
#include "MediaDevice.h"
#endif
#ifdef _QT_MAKE_
#include <QFileInfo>
#include <QThread>
#else
#include "ShlObj.h"
#include "shlwapi.h"
#endif
#include "EBAppClient.h"
//#include <CGCLib/CGCLib.h>

#ifdef USES_VIDEOROOM
#include "ebmm.h"
#endif

#ifndef WIN32
#include "../include/boost_ini.h"
#endif

#ifdef _QT_MAKE_
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QDesktopServices>
#include <QImage>
#include <QDir>
#include <QFile>
const int thePostEventPriority = Qt::NormalEventPriority;
#else
#include "atlconv.h"
#include <Gdiplus.h>
#endif

#ifndef _QT_MAKE_
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
#endif

#ifdef _MSC_VER
#pragma comment(lib, "shlwapi.lib")

/// 在CGCClass.lib 已经包含
//#ifdef _DEBUG
//#pragma comment(lib, "zlibstatd.lib")
//#else
//#pragma comment(lib, "zlibstat.lib")
//#endif

#ifndef _QT_MAKE_
#ifdef _DLL
#ifdef _DEBUG
//#pragma comment(lib, "videoroomd.lib")
#pragma comment(lib, "libebcmd.lib")
#else
//#pragma comment(lib, "videoroom.lib")
#pragma comment(lib, "libebcm.lib")
#endif

#ifdef USES_REMOTE_DESKTOP
#ifndef USES_RD_COM
#ifdef _DEBUG
#pragma comment(lib, "libebrdd.lib")
#else
#pragma comment(lib, "libebrd.lib")
#endif
#endif
#endif /// USES_REMOTE_DESKTOP

#ifdef USES_VIDEOROOM
#ifdef _DEBUG
#pragma comment(lib, "libebmmd.lib")
#else
#pragma comment(lib, "libebmm.lib")
#endif
#endif /// USES_VIDEOROOM

#else
#ifdef _DEBUG
#pragma comment(lib, "libebcmsd.lib")
#else
#pragma comment(lib, "libebcm.lib")
#endif

#ifndef USES_RD_COM
#ifdef _DEBUG
#pragma comment(lib, "libebrdd.lib")
#else
#pragma comment(lib, "libebrd.lib")
#endif
#endif

#ifdef _DEBUG
#pragma comment(lib, "libebmmd.lib")
#else
#pragma comment(lib, "libebmm.lib")
#endif

#endif  /// _DLL

#endif  /// _QT_MAKE_


#endif // _MSC_VER


namespace entboost {

mycp::bigint EbGetFileSize(const EBFileString &sFilePath)
{
#ifdef _QT_MAKE_
    QFile file(sFilePath);
    if ( !file.open(QFile::ReadOnly) ) {
        return -1;
    }
    const mycp::bigint fileSize = file.size();
    file.close();
    return fileSize;
#else
    FILE * f = fopen(sFilePath.c_str(), "rb");
    if (f == NULL) {
        return -1;
    }
#ifdef WIN32
    _fseeki64(f, 0, SEEK_END);
    const mycp::bigint fileSize = _ftelli64(f);
#else
    fseeko(f, 0, SEEK_END);
    const mycp::bigint fileSize = ftello(f);
#endif
    fclose(f);
    return fileSize;
#endif
}

const unsigned int theOneMB = 1024*1024;
bool GetFileMd5(const EBFileString &sFilePath, mycp::bigint& pOutFileSize, tstring& pOutFileMd5)
{
#ifdef _QT_MAKE_
    QFile file(sFilePath);
    if ( !file.open(QFile::ReadOnly) ) {
        return false;
    }
    pOutFileSize = file.size();
    file.seek(0);
    MD5 md5;
    const unsigned int nPackSize = (unsigned int)(pOutFileSize>theOneMB?theOneMB:pOutFileSize);
    char * lpszBuffer = new char[nPackSize+1];
    while (true) {
        const size_t nReadSize = file.read(lpszBuffer,nPackSize);
        if (nReadSize==0) {
            break;
        }
        md5.update((const unsigned char*)lpszBuffer, nReadSize);
    }
    file.close();
    md5.finalize();
    pOutFileMd5 = md5.hex_digest();
    delete[] lpszBuffer;
#else
    FILE * f = fopen(sFilePath.c_str(), "rb");
	if (f == NULL) return false;
#ifdef WIN32
	_fseeki64(f, 0, SEEK_END);
	pOutFileSize = _ftelli64(f);
#else
	fseeko(f, 0, SEEK_END);
	pOutFileSize = ftello(f);
#endif
	// 获取文件MD5
	MD5 md5;
	const unsigned int nPackSize = (unsigned int)(pOutFileSize>theOneMB?theOneMB:pOutFileSize);
	unsigned char * lpszBuffer = new unsigned char[nPackSize+1];
#ifdef WIN32
	_fseeki64(f, 0, SEEK_SET);
#else
	fseeko(f, 0, SEEK_SET);
#endif
	while (true) {
		const size_t nReadSize = fread(lpszBuffer,1,nPackSize,f);
		if (nReadSize==0) break;
		md5.update(lpszBuffer, nReadSize);
	}
	fclose(f);
	md5.finalize();
	pOutFileMd5 = md5.hex_digest();
	delete[] lpszBuffer;
#endif
	return true;
}

//#pragma comment(lib,"Ws2_32.lib")
//#include <Iphlpapi.h>
//#pragma comment(lib, "Iphlpapi.lib")
//using namespace std;
typedef struct tagNetworkCfg 
{
	char szIP[18];
#ifndef _QT_MAKE_
	char szNetmask[18];
	char szGateway[18];
	//char szDns1[18];
	//char szDns2[18];
#endif
}NetworkCfg;

bool GetNetworkCfg(std::vector<NetworkCfg>& pOutNetworkCfg,bool bGetEmptyGateway)
{
#ifdef _QT_MAKE_
	QList<QHostAddress> list = QNetworkInterface::allAddresses();  
	foreach (QHostAddress address, list)  
	{  
        if ( address.protocol() == QAbstractSocket::IPv4Protocol &&
             address != QHostAddress::Null &&
             address != QHostAddress::LocalHost)
        {
            /// IPv4地址
            /// 10.0.2.15 ?
            const QString sIp = address.toString();
            if (sIp.contains("127.0.") ||
//        #ifdef Q_OS_ANDROID
                    sIp.contains("10.0.") ||
//        #endif
                    sIp.contains("0.0.0.0"))
//            if (sIp.contains("127.0.") || sIp.contains("0.0.0.0"))
			{  
				continue;  
			}  
            NetworkCfg cfg;
            strcpy(cfg.szIP, sIp.toStdString().c_str());
            pOutNetworkCfg.push_back(cfg);
		}  
    }
//    if (pOutNetworkCfg.empty()) {
//        NetworkCfg cfg;
//        strcpy(cfg.szIP, QHostAddress(QHostAddress::LocalHost).toString().toStdString().c_str());
//        pOutNetworkCfg.push_back(cfg);
//    }
#else

//#ifdef _MSC_VER
	//log_printf("Get network config");
	//获取网卡名称 网卡名称，网卡别名
	std::vector<tstring> strAdapterAliasList;
	//tstring strAdapterName;
	HKEY hKey, hSubKey, hNdiIntKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"System\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}",
		0,
		KEY_READ,
		&hKey) != ERROR_SUCCESS)
	{
		return false;
	}
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
		tstring strKeyName = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
		strKeyName += strAdapterAliasList[i];
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			strKeyName.c_str(),
			0,
			KEY_READ,
			&hKey) != ERROR_SUCCESS)
		{
			continue;
		}
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
//#endif

#endif //_QT_MAKE_
	return true;
}
int GetLocalHostIp(std::vector<tstring> & pOutIps)
{
	int count=0;
#ifdef _WIN32
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
	for (int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		pOutIps.push_back(inet_ntoa(addr));
		printf("Address %d : %s\n" , i, inet_ntoa(addr));
	}
#else
	int sock;
	struct sockaddr_in sin;
	struct ifreq ifr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock>=0)//!<0
	{
		for(int i=0;i<10;i++)
		{
			memset(&ifr, 0, sizeof(ifr)); 
			sprintf(ifr.ifr_name,"eth%d",i);
			if(ioctl(sock,SIOCGIFADDR,&ifr)<0) break;
			::memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
			pOutIps.push_back((const char*)inet_ntoa(sin.sin_addr));
			printf("Address %d : %s\n", i, (const char*)inet_ntoa(sin.sin_addr));

			memset(&ifr, 0, sizeof(ifr)); 
			sprintf(ifr.ifr_name,"eth%d:1",i);
			if(ioctl(sock,SIOCGIFADDR,&ifr)<0) continue;
			::memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
			pOutIps.push_back((const char*)inet_ntoa(sin.sin_addr));
			printf("Address %d : %s\n", i, (const char*)inet_ntoa(sin.sin_addr));
		}
		close(sock);
	}else
	{
		printf("socket error\n");
	}
#endif
	return count;
}

//int abc(int aba)
//{
//	return 1;
//}
//CPOPCLogonCenter::pointer theDeletePOPCLogonCenter;		// 无用临时数据
int ParseString(const tstring& sIn, const char * lpszInterval, bool bClear, std::vector<tstring> & pOut)
{
	//const tstring sIn(lpszString);
	const size_t nInLen = sIn.size();
	const size_t nIntervalLen = strlen(lpszInterval);
	if (bClear)
		pOut.clear();
	std::string::size_type nFindBegin = 0;
	while (nFindBegin<nInLen)
	{
		std::string::size_type find = sIn.find(lpszInterval,nFindBegin);
		if (find == std::string::npos)
		{
			pOut.push_back(sIn.substr(nFindBegin));
			break;
		}
		if (find==nFindBegin)
		{
			pOut.push_back("");	// 空
		}else
		{
			pOut.push_back(sIn.substr(nFindBegin, (find-nFindBegin)));
		}
		nFindBegin = find+nIntervalLen;
	}
	return (int)pOut.size();

	//tstring sIn(lpszString);
	//if (bClear)
	//	pOut.clear();
	//while (!sIn.empty())
	//{
	//	std::string::size_type find = sIn.find(lpszInterval);
	//	if (find == std::string::npos)
	//	{
	//		pOut.push_back(sIn);
	//		break;
	//	}
	//	if (find==0)
	//		pOut.push_back("");	// 空
	//	else
	//		pOut.push_back(sIn.substr(0, find));
	//	sIn = sIn.substr(find+strlen(lpszInterval));
	//}
	//return (int)pOut.size();
}
bool SplitString(const tstring& sSource, const char* sStart, const char* sEnd, tstring & sOut)
{
	sOut.empty();
	std::string::size_type find1 = sSource.find(sStart);
	if (find1 == std::string::npos) return false;
	std::string::size_type find2 = sSource.find(sEnd, find1+strlen(sStart));
	if (find2 == std::string::npos) return false;
	sOut = sSource.substr(find1+strlen(sStart),find2-find1-strlen(sStart));
	return true;
}
mycp::bigint SplitBigIntValue(const tstring& sSource, const char* sStart, const char* sEnd, mycp::bigint nDefault)
{
	mycp::bigint nResult(nDefault);
	tstring sOut;
	if (SplitString(sSource,sStart,sEnd,sOut))
	{
		nResult = cgc_atoi64(sOut.c_str());
	}
	return nResult;
}
inline int SplitIntValue(const tstring& sSource, const char* sStart, const char* sEnd, int nDefault)
{
	int nResult(nDefault);
	tstring sOut;
	if (SplitString(sSource,sStart,sEnd,sOut))
	{
		nResult = atoi(sOut.c_str());
	}
	return nResult;
}
#ifdef _DEBUG
int abctest3(int a) {return 22+a;}
tstring abctest2(void) {return "abc";}
#endif
#ifdef _QT_MAKE_
inline bool checkCreateDir(const QString &dirName)
{
    QDir pDir1(dirName);
    if (!pDir1.exists()) {
        return pDir1.mkdir(dirName);
    }
    return true;
}
#else
ULONG_PTR theGdiplusToken=0;
#endif
CUserManagerApp::CUserManagerApp(void)
: m_callback(NULL)
, m_pHwnd(NULL)
, m_bKilled(false)
, m_userStatus(US_Logout)
//, m_serverStatus(SS_Unknown)
, m_nLcServerState(EB_SS_INIT)
, m_nUmServerState(EB_SS_INIT)
, m_nApServerState(EB_SS_INIT)
, m_nLastStateCode(EB_STATE_OK)
, m_tReLogonTime(0)
, m_tReLoadInfo(0)
{
#ifdef _DEBUG
	abctest3(1);
//	abctest6();
#endif

#ifndef _QT_MAKE_
//#ifdef _MSC_VER
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&theGdiplusToken, &gdiplusStartupInput, NULL);
#endif
	m_tOrgBeginOfStart = 0;
	m_nOrgServerIndex = 0;
	m_nCurrentIndex = -1;
	m_tBeginOfStart = 0;
	m_sDevAppId = 0;
	m_nOpenRegister = 0;
	m_nSystemSetting = 0;
	m_bOpenVisitor = false;
	m_bLicenseUser = false;
	//m_nSendRegMail = 0;
	m_nPasswdAuthMode = PASSWD_AUTH_MODE_ENTBOOST;
	m_nOnlineExtData = 0;
	m_nDeployId = 0;
	m_nLicenstType = 0;
	m_nEBServerVersion = 0;
	m_nAutoOpenSubId = 0;
	m_nGroupMsgSubId = 0;
	m_nFindAppSubId = 0;
	m_nMyCollectionSubId = 0;
	m_nSaveConversations = 3;
	m_nSDKVersion = 0;
	m_tLogonCenter = 0;
	m_pMyAccountInfo = CEBAccountInfo::create(0,"");
#ifdef _QT_MAKE_
#ifdef Q_OS_ANDROID
    const QString writablePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    checkCreateDir(writablePath);
    m_sAppPath = QDir(writablePath).absolutePath();
#else
    const QString pApplicationDirPath = QCoreApplication::applicationDirPath();
    m_sAppPath = pApplicationDirPath;
#endif
    m_sImgPath = m_sAppPath+"/img";
    checkCreateDir(m_sImgPath);
    m_sEbResourcePath = m_sAppPath+"/res";
    checkCreateDir(m_sEbResourcePath);
    m_sEbDataPath = m_sAppPath+"/datas";
    checkCreateDir(m_sEbDataPath);

    /// app data path
    /// C:/Users/pc/AppData/Local/ebcd
//    const QString sStringTemp = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
//    const QString sStringTemp = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    /// C:/Users/pc/AppData/Roaming/ebcd
    const QString sStringTemp = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_sAppDataPath = sStringTemp;
//    m_sAppDataPath += _T("/entboost");
#else
	char lpszBuffer[MAX_PATH];
    GetModuleFileNameA(NULL, lpszBuffer, MAX_PATH);
	m_sAppPath = lpszBuffer;
	m_sAppPath = m_sAppPath.substr(0,m_sAppPath.rfind('\\'));
	m_sImgPath = m_sAppPath+_T("/img");
    if (!::PathFileExistsA(m_sImgPath.c_str()))
	{
        ::CreateDirectoryA(m_sImgPath.c_str(), NULL);
	}
	m_sEbResourcePath = m_sAppPath+_T("\\res");
    if (!::PathFileExistsA(m_sEbResourcePath.c_str()))
	{
        ::CreateDirectoryA(m_sEbResourcePath.c_str(), NULL);
	}
	m_sEbDataPath = m_sAppPath+_T("\\datas");
    if (!::PathFileExistsA(m_sEbDataPath.c_str()))
	{
        ::CreateDirectoryA(m_sEbDataPath.c_str(), NULL);
	}
	// app data path
    SHGetSpecialFolderPathA(::GetDesktopWindow(),lpszBuffer,CSIDL_APPDATA,FALSE);
	m_sAppDataPath = lpszBuffer;
	m_sAppDataPath += _T("\\entboost");
#endif
	

#ifdef _QT_MAKE_
    checkCreateDir(m_sAppDataPath);
    m_sAppDataTempPath = m_sAppDataPath+"/temp";
    checkCreateDir(m_sAppDataTempPath);
#else
    if (!::PathFileExistsA(m_sAppDataPath.c_str()))
	{
        ::CreateDirectoryA(m_sAppDataPath.c_str(), NULL);
	}
	m_sAppDataTempPath = m_sAppDataPath+_T("\\temp");
    if (!::PathFileExistsA(m_sAppDataTempPath.c_str()))
	{
        ::CreateDirectoryA(m_sAppDataTempPath.c_str(), NULL);
	}
#endif
	//m_pVideoStreamCallback = NULL;
	//m_dwVideoCallbackData = 0;
#ifdef USES_VIDEOROOM
	Cebmm::GetVideoDevices(m_pVideoDevices);
#endif
	m_nLocalVideoIndex = 0;
	m_bExistLocalImage = false;
	m_bExistLocalContact = false;

	m_nMyDepInfoSize = -1;
	m_nReturnMyGroupInfoSize = 0;
	m_nEntDepInfoSize = -1;
	m_nReturnEntGroupInfoSize = 0;
	m_nLoadUGCount = -1;
	m_nLoadContactCount = -1;

	m_nLoadSubFunc = 1;
	m_bNeedLoadVerInfo = true;
	m_nServerEntDepInfoVer = -1;
	theServerEntDepInfoSize = 0;
	m_nServerMyDepInfoVer = -1;
	m_nServerContactInfoVer = -1;

	for (int i=0;i<20;i++)
	{
		m_pRsa.SetPrivatePwd(GetSaltString());
		m_pRsa.rsa_generatekey_mem(1024);
		if (m_pRsa.GetPublicKey().empty() || m_pRsa.GetPrivateKey().empty())
		{
			m_pRsa.SetPrivatePwd("");
			continue;
		}
		if (!m_pRsa.rsa_open_public_mem())
		{
			m_pRsa.SetPrivatePwd("");
			continue;
		}
		const tstring sFrom("eb");
		unsigned char* pTo = NULL;
		m_pRsa.rsa_public_encrypt((const unsigned char*)sFrom.c_str(),sFrom.size(),&pTo);
		//m_pRsa.rsa_private_encrypt((const unsigned char*)sFrom.c_str(),sFrom.size(),&pTo);
		if (pTo!=NULL)
		{
			delete[] pTo;
			m_pRsa.rsa_close_public();
			break;
		}
		m_pRsa.rsa_close_public();
		m_pRsa.SetPrivatePwd("");
	}
	//m_file = NULL;
#ifdef _QT_MAKE_
    const std::string sSettingIniFile = m_sEbDataPath.toLocal8Bit().toStdString()+"/setting.ini";
#else
	const tstring sSettingIniFile = m_sEbDataPath+_T("\\setting.ini");
#endif
#ifdef WIN32
    m_bLogDebug = GetPrivateProfileIntA("system","debug",1,sSettingIniFile.c_str())==1?true:false;
#else
    m_bLogDebug = GetPrivateProfileIntABoost("system","debug",1,sSettingIniFile.c_str())==1?true:false;
#endif
	m_pLogDebug = NULL;
}

CUserManagerApp::~CUserManagerApp(void)
{
//#ifdef _DEBUG
//	abctest1(12);
//#endif
	LogoutAppCenter();
	Logout();
	m_pUserManager.reset();
	m_pLogonCenter.reset();
	m_pAppCenter.reset();
	m_pMyAccountInfo.reset();
#ifndef _QT_MAKE_
//#ifdef _MSC_VER
	Gdiplus::GdiplusShutdown(theGdiplusToken);
#endif

	m_bKilled = true;
	if (m_pProcessThread.get() != NULL)
	{
		m_pProcessThread->join();
		m_pProcessThread.reset();
	}

	for (size_t i=0;i<m_pExit2DeleteFileList.size(); i++)
	{
#ifdef _QT_MAKE_
        QFile::remove(m_pExit2DeleteFileList[i]);
#else
        DeleteFileA(m_pExit2DeleteFileList[i].c_str());
#endif
	}
	//if (m_file!=NULL)
	//	fclose(m_file);
}

void CUserManagerApp::process_thread_svr(void)
{
	while (!m_bKilled)
	{
		try
		{
			DoProcess();
		}catch(std::exception&)
		{}catch(...)
		{}
		if (!m_bKilled)
		{
#ifdef WIN32
			Sleep(1000);
#else
			usleep(1000000);
#endif
		}
	}
}
//#define PROCESS_MSG_TYPE_RESET_DEVAPPID			1
//#define PROCESS_MSG_TYPE_SAVE_LOAD_MEMBERINFO	2

void CUserManagerApp::DoProcess(void)
{
	time_t theRelogonTime = 0;
	unsigned int nIndex = 0;
	while (!m_bKilled)
	{
#ifdef WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
		if (((nIndex++)%100)!=99)
		{
			continue;
		}
		// 1 秒

		const time_t tNow = time(0);
		if (m_tReLogonTime > 0 && m_tReLogonTime<tNow)
		{
			if (theRelogonTime!=m_tReLogonTime)	// 避免异常跳出 DoProcess() 重进打印太多
			{
				theRelogonTime = m_tReLogonTime;
				//LogMessage("DoProcess Relogon... (m_sDevAppOnlineKey=%s,m_sDevAppId=%lld,m_sDevAppKey=%s,m_userStatus=%d)\r\n",m_sDevAppOnlineKey.c_str(),m_sDevAppId,m_sDevAppKey.c_str(),(int)m_userStatus);
			}
			if (m_sDevAppOnlineKey.empty())
			{
				m_tReLogonTime = tNow+20;					// 30S后继续处理；（**开发者ID验证成功返回，也会立即申请重新登录；）
				SetDevAppId(m_sDevAppId,m_sDevAppKey,true);	// 登录前，需要重新验证appkey
			}else
			{
				m_tReLogonTime = tNow+30;					// 30S后会重新登录；（如果登录成功，会设为0；）
				ReLogon();
			}
		}

		bool bBreak = false;
		CProcessMsgInfo::pointer pProcessMsgInfo;
        while (!m_bKilled && !bBreak && m_pProcessMsgList.front(pProcessMsgInfo))
		{
			if (pProcessMsgInfo->m_tProcessTime>0 && pProcessMsgInfo->m_tProcessTime>=time(0))
			{
				m_pProcessMsgList.add(pProcessMsgInfo);
				if (m_pProcessMsgList.size()==1)
					break;
#ifdef WIN32
				Sleep(100);
#else
				usleep(100000);
#endif
				continue;
//			}else
//			{
//#ifdef WIN32
//				Sleep(2);
//#else
//				usleep(2000);
//#endif
			}

            switch (pProcessMsgInfo->GetProcessMsgType()) {
			//case CProcessMsgInfo::PROCESS_MSG_TYPE_CHECK_MEMBER_HEAD:
			//	{
			//		const CEBMemberInfo::pointer& pEmployeeInfo = pProcessMsgInfo->m_pEmployeeInfo;
			//		//const tstring& sHeadResourceFile = pProcessMsgInfo->m_sString1;
			//		ProcessMemberHeadInfo(pEmployeeInfo);
			//	}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_CHECK_RESOURCE_REFRESH:
				{
					std::vector<CEBResourceInfo::pointer> pDeleteList;
					{
						AUTO_CONST_RLOCK(theResourceList);
						CLockMap<mycp::bigint, CEBResourceInfo::pointer>::const_iterator pIter = theResourceList.begin();
						for (; pIter!=theResourceList.end(); pIter++)
						{
							const CEBResourceInfo::pointer& pResourceInfo = pIter->second;
							if ((pResourceInfo->m_nItemData & EB_RESOURCE_DATA_REFRESH)==EB_RESOURCE_DATA_REFRESH)
							{
								// 文件删除；
								pDeleteList.push_back(pResourceInfo);
							}
						}
					}
					for (size_t i=0; i<pDeleteList.size(); i++)
					{
						const CEBResourceInfo::pointer pResourceInfo = pDeleteList[i];
						if (this->m_callback)
							m_callback->onResourceDelete(pResourceInfo.get());
						if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
							EB_ResourceInfo * pEvent = new EB_ResourceInfo(pResourceInfo.get());
                            pEvent->SetQEventType((QEvent::Type)EB_WM_RESOURCE_DELETE);
                            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
							::SendMessage(m_pHwnd, EB_WM_RESOURCE_DELETE, (WPARAM)(const EB_ResourceInfo*)pResourceInfo.get(), 0);
#endif
						}
						theResourceList.remove(pResourceInfo->m_sResId);
					}
					m_pProcessCheckResourceRefresh.reset();
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_P2P_REQUEST:
				{
					CEBCallInfo::pointer pCallInfo = pProcessMsgInfo->m_pCallInfo;
					const CEBAccountInfo::pointer pAccountInfo = pProcessMsgInfo->m_pFromAccount;
					LocalChatRoomP2PRequest(pCallInfo,pAccountInfo,1);	// 离线重试
				}break;
#ifdef USES_RESEND_FILE_2
			case CProcessMsgInfo::PROCESS_MSG_TYPE_RESEND_FILE:
				{
					const mycp::bigint nMsgId = pProcessMsgInfo->m_nBigInt1;
					CToSendInfo::pointer pCallToSendInfo;
					if (theREsendIdList.find(nMsgId,pCallToSendInfo,true))
					{
						const mycp::bigint sCallId = pProcessMsgInfo->m_nBigInt2;
                        SendCrFile(sCallId,pCallToSendInfo->m_sFilePath,pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile);
					}
				}break;
#endif
			case CProcessMsgInfo::PROCESS_MSG_TYPE_WAIT_LOCAL_P2P_OK:
				{
#ifdef USES_CHATROOM
                    const CEBAccountInfo::pointer pFromAccount = pProcessMsgInfo->m_pFromAccount;
                    BoostReadLock rdLock2(pFromAccount->m_mutexLocalChatRoom);
					BoostReadLock rdLock1(pFromAccount->m_mutexRemoteChatRoom);
					if (!pFromAccount->m_bLocalP2POk && !pFromAccount->m_bRemoteP2POk)
					{
						pFromAccount->m_bRemoteP2POk = true;
						pFromAccount->m_pRemoteChatRoom->SetDestAddress(1,pFromAccount->GetRemoteAddress());
						if (pFromAccount->m_pLocalChatRoom.get()!=NULL)
						{
							m_pClearChatRoom.add(pFromAccount->m_pLocalChatRoom);
							pFromAccount->m_pLocalChatRoom.reset();
						}
						CEBCallInfo::pointer pCallInfo = pProcessMsgInfo->m_pCallInfo;
						const mycp::bigint sCallId = pProcessMsgInfo->m_pCallInfo->GetCallId();
						pCallInfo->m_tP2POkTime = time(0);
						pCallInfo->m_nCallData |= EB_CALL_DATA_P2P_OK;
						LogMessage("PROCESS_MSG_TYPE_WAIT_LOCAL_P2P_OK->m_pP2POkList.add... (callid=%lld,from_uid=%lld)\r\n",sCallId,pFromAccount->GetUserId());
						m_pP2POkList.add(sCallId);
					}
#endif
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_SEND_GROUP_FILE:
				{
					const mycp::bigint sCallId = pProcessMsgInfo->m_nCallGroupId;
					const mycp::bigint sResourceId = pProcessMsgInfo->m_nBigInt1;
                    const EBFileString sFileName(pProcessMsgInfo->m_sString1);
                    SendCrFile(sCallId,sFileName,sResourceId);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_LOAD_RESOURCE_INFO:
				{
					const mycp::bigint sResourceId = pProcessMsgInfo->m_nBigInt1;
					this->LoadRes(1,sResourceId,0,0,EB_RESOURCE_FROM_TYPE_UNKNOWN);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_TO_SEND_LIST:
				{
					const mycp::bigint sCallId = pProcessMsgInfo->m_pCallInfo->GetCallId();
					if (!m_pWaitList1.remove(sCallId))
						break;	// *已经处理过
					const bool bSendGroupCall = pProcessMsgInfo->m_nBigInt1==1?true:false;
					ProcessToSendList(sCallId, bSendGroupCall, true);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_WAIT_ONCANCEL_FILE:
				{
					const mycp::bigint sCallId = pProcessMsgInfo->m_pCallInfo->GetCallId();
					const mycp::bigint nMsgId = pProcessMsgInfo->m_nBigInt1;
					if (!m_pWaitList1.remove(nMsgId))
						break;	// *已经处理过

					CCrFileInfo pFileInfo(0,sCallId,sCallId,EB_STATE_OK);
					pFileInfo.m_nMsgId = nMsgId;
					//pFileInfo.m_sFileName = "";
					pFileInfo.m_sSendFrom = m_pMyAccountInfo->GetUserId();
					//pFileInfo.m_sSendTo = 0;
					pFileInfo.m_sReceiveAccount = m_pMyAccountInfo->GetUserId();
					OnCancelFile(pFileInfo);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_LOG_MESSAGE:
				{
					if (m_pLogDebug==NULL)
					{
						char sDebugFile[260];
#ifdef _QT_MAKE_
                        if (is_visitor_uid(m_pMyAccountInfo->GetUserId()))
                            sprintf(sDebugFile, "%s/debug_visitor.txt", m_sAppPath.toLocal8Bit().constData());
                        else
                            sprintf(sDebugFile, "%s/debug_%lld.txt", m_sAppPath.toLocal8Bit().constData(), m_pMyAccountInfo->GetUserId());
#else
                        if (is_visitor_uid(m_pMyAccountInfo->GetUserId()))
                            sprintf(sDebugFile, "%s\\debug_visitor.txt", m_sAppPath.c_str());
                        else
                            sprintf(sDebugFile, "%s\\debug_%lld.txt", m_sAppPath.c_str(), m_pMyAccountInfo->GetUserId());
#endif
                        m_pLogDebug = fopen(sDebugFile,"w");
						if (m_pLogDebug==NULL)
							break;
					}
					const time_t tNow = pProcessMsgInfo->m_tProcessTime+1;
					const struct tm *newtime = localtime(&tNow);
					char lpszDateTime[64];
					sprintf(lpszDateTime, "%02d-%02d %02d:%02d:%02d ",newtime->tm_mon+1,newtime->tm_mday, newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
					fwrite(lpszDateTime,1,strlen(lpszDateTime),m_pLogDebug);
#ifdef _QT_MAKE_
                    const std::string sData = pProcessMsgInfo->m_sString1.toStdString();
                    fwrite(sData.c_str(), 1, sData.size(), m_pLogDebug);
#else
                    fwrite(pProcessMsgInfo->m_sString1.c_str(),1,pProcessMsgInfo->m_sString1.size(),m_pLogDebug);
#endif
					//fwrite("\r\n",1,2,m_pLogDebug);
					fflush(m_pLogDebug);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE:
				{
					//LogMessage("PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE... IsEnterOk=%d\r\n",(int)(pProcessMsgInfo->m_pChatRoom->IsEnterOk()?1:0));
					if (!pProcessMsgInfo->m_pChatRoom->IsEnterOk())
					{
						pProcessMsgInfo->m_pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
						m_pProcessMsgList.add(pProcessMsgInfo);
						if (m_pProcessMsgList.size()==1)
						{
							bBreak = true;
							break;
						}
#ifdef WIN32
						Sleep(100);
#else
						usleep(100000);
#endif
						continue;
					}
					//LogMessage("PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE... RecviveResource\r\n");
                    pProcessMsgInfo->m_pChatRoom->RecviveResource(pProcessMsgInfo->m_nBigInt1,pProcessMsgInfo->m_sString1);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_RESET_DEVAPPID:
				SetDevAppId(m_sDevAppId,m_sDevAppKey,true);	// 登录前，需要重新验证appkey
				break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_F_GROUP_INFO:
				{
					OnFUMEMDepInfo(pProcessMsgInfo->m_pDepartmentInfo,pProcessMsgInfo->m_nReturnVer,pProcessMsgInfo->m_nCallGroupId);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_DELETE_HEAD_RESFILE:
				{
					DeleteOldHeadFile(pProcessMsgInfo->m_nBigInt1);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_F_MEMBER_INFO:
				{
					const bool bUserHeadChange = pProcessMsgInfo->m_nBigInt1==1?true:false;
					OnFUMEMEmpInfo(pProcessMsgInfo->m_pEmployeeInfo,pProcessMsgInfo->m_bNewMemberInfo,pProcessMsgInfo->m_nReturnVer,bUserHeadChange);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_LOAD_GROUP_OLS:
				{
					m_pWaitList2.remove(pProcessMsgInfo->m_nCallGroupId,0,true);
					LoadInfo(0,0,0,0,0,0,pProcessMsgInfo->m_nCallGroupId);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_CALL_ACCEPT:
				{
					if (pProcessMsgInfo->m_pCallInfo.get()!=NULL)
						CallAck(pProcessMsgInfo->m_pCallInfo, EB_CAT_ACCEPT);
				}break;
			case CProcessMsgInfo::PROCESS_MSG_TYPE_INVITE_CALL:
				{
					if (pProcessMsgInfo->m_pCallInfo.get()!=NULL)
					{
						const bool bSendFile = pProcessMsgInfo->m_nBigInt1==1?true:false;
						InviteCall(pProcessMsgInfo->m_pCallInfo, 0,pProcessMsgInfo->m_nFromUserId,true,bSendFile);
					}
				}break;
			default:
				break;
			}
		}
	}
	if (m_pLogDebug!=NULL)
	{
		fclose(m_pLogDebug);
		m_pLogDebug = NULL;
    }
}

#ifdef _QT_MAKE_
long CUserManagerApp::postWaitEventResult(QObject *receiver, EB_Event *event)
{
    static unsigned long theIndex = 0;
    const unsigned long resultKey = (++theIndex)+(unsigned long)event;
    event->setReceiver(this, resultKey);
    QCoreApplication::postEvent(receiver, event, thePostEventPriority);
    /// 等待最长 60 秒
    return waitEventResult(resultKey, 60);
}

long CUserManagerApp::postWaitEventResult(QObject *receiver, CCrInfo *event)
{
    static unsigned long theIndex = 0;
    const unsigned long resultKey = (++theIndex)+(unsigned long)event;
    event->setReceiver(this, resultKey);
    QCoreApplication::postEvent(receiver, event, thePostEventPriority);
    /// 等待最长 60 秒
    return waitEventResult(resultKey, 60);
}

long CUserManagerApp::waitEventResult(unsigned long resultKey, int waitMaxSecond, long defaultResult)
{
    long result = defaultResult;
    for (int i=0; i<waitMaxSecond*200; i++) {
        QCoreApplication::processEvents();
        QThread::msleep (5);
        if (m_eventResult.find(resultKey,result)) {
            break;
        }
    }
    return result;
}

class EBUM_Event : public QEvent
{
public:
    enum EventType {
        Event_None,
        Event_Open_Video,
        Event_VRequestResponse,
        Event_VAckResponse,
        Event_FVRequest,
        Event_FVAck,
        Event_FVEnd
    };
    EBUM_Event(EventType type, QEvent::Type e=QEvent::User)
        : QEvent(e)
        , m_type(type)
    {
    }
    CPOPSotpRequestInfo::pointer m_pSotpRequestInfo;
    CPOPSotpResponseInfo::pointer m_pSotpResponseInfo;
    CEBCallInfo::pointer m_pCallInfo;
    EventType type(void) const {return m_type;}
private:
    EventType m_type;
};
void CUserManagerApp::customEvent(QEvent *e)
{
    const QEvent::Type eventType = e->type();
    if (eventType==CR_WM_EVENT_RESULT) {
        const CCrInfo * event = (CCrInfo*)e;
        m_eventResult.insert(event->receiveKey(),event->GetEventParameter());
    }
    else if (eventType==EB_WM_EVENT_RESULT) {
        const EB_Event * event = (EB_Event*)e;
        m_eventResult.insert(event->receiveKey(),event->GetEventParameter());
    }
    else if (eventType==QEvent::User) {
        doEBUMEvent(e);
    }
}

void CUserManagerApp::doEBUMEvent(QEvent *e)
{
    EBUM_Event *event = (EBUM_Event*)e;
    switch (event->type()) {
    case EBUM_Event::Event_Open_Video: {
        const CEBCallInfo::pointer &pCallInfo = event->m_pCallInfo;
        if (pCallInfo.get()==0) {
            break;
        }
        if (event->m_pSotpRequestInfo.get()==0) {
            break;
        }
        const mycp::bigint userId = event->m_pSotpRequestInfo->m_pRequestList.getParameterValue("user-id",(mycp::bigint)0);
        if (userId==0) break;
        const bool localVideo = userId==m_pMyAccountInfo->GetUserId()?true:false;
        QWidget *hVideoWndParent = (QWidget*)event->m_pSotpRequestInfo->m_pRequestList.getParameterValue("video-parent",(mycp::bigint)0);
        void *pAudioParam = (void*)event->m_pSotpRequestInfo->m_pRequestList.getParameterValue("audio-param",(mycp::bigint)0);
        const unsigned long  resultKey = (unsigned long)event->m_pSotpRequestInfo->m_pRequestList.getParameterValue("result-key",(mycp::bigint)0);
        bool bVideoError = false;
        OpenUserVideo(pCallInfo, userId, localVideo, hVideoWndParent, pAudioParam, bVideoError);
        if (resultKey!=0) {
            m_eventResult.insert(resultKey, bVideoError?0:1);
        }
        break;
    }
    case EBUM_Event::Event_VAckResponse:
        OnVAckResponse(event->m_pSotpRequestInfo, event->m_pSotpResponseInfo);
        break;
    case EBUM_Event::Event_VRequestResponse:
        OnVRequestResponse(event->m_pSotpRequestInfo, event->m_pSotpResponseInfo);
        break;
    case EBUM_Event::Event_FVRequest:
        OnFVRequest(event->m_pSotpRequestInfo, event->m_pSotpResponseInfo);
        break;
    case EBUM_Event::Event_FVAck:
        OnFVAck(event->m_pSotpRequestInfo, event->m_pSotpResponseInfo);
        break;
    case EBUM_Event::Event_FVEnd:
        OnFVEnd(event->m_pSotpRequestInfo, event->m_pSotpResponseInfo);
        break;
    default:
        break;
    }

}
#endif

int CUserManagerApp::LogonAppCenter(void)
{
	if (m_sDevAppId==0 || m_sAPServer.empty())
	{
		return -1;
	}
	if (m_pAppCenter.get()!=NULL && m_pAppCenter->GetAddress().address()!=m_sServerAddress)
	{
		m_pAppCenter->Stop();
		m_pAppCenter.reset();
	}
	if (m_pAppCenter.get()==NULL)
	{
		m_pAppCenter = CEBCAppCenter::create(this);
		m_pAppCenter->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
		const int ret = m_pAppCenter->Start(m_sAPServer,m_sAPAppName,0,1);
		if (ret!=0)
		{
			m_pAppCenter.reset();
			return ret;
		}
	}
	return m_pAppCenter->SendAOn(0,m_sDevAppId,m_sDevAppOnlineKey);
}
int CUserManagerApp::LogoutAppCenter(void)
{
	if (m_pAppCenter.get() == NULL)
	{
		return -1;
	}
	return m_pAppCenter->SendAOff(0);
}
int CUserManagerApp::SendAPMsg(const EB_APMsgInfo& pAPMsgInfo, bool bSaveOffMsg)
{
	if (m_pAppCenter.get() == NULL)
	{
		return -1;
	}
	return m_pAppCenter->SendAMsg(0,pAPMsgInfo,bSaveOffMsg);
}
int CUserManagerApp::APMsgAck(mycp::bigint nMsgId)
{
	if (m_pAppCenter.get() == NULL)
	{
		return -1;
	}
	return m_pAppCenter->SendAMAck(nMsgId);
}
int CUserManagerApp::UmMsgAck(mycp::bigint nMsgId,EB_CALL_ACK_TYPE nAckType)
{
	if (m_pUserManager.get() == NULL)
	{
		return -1;
	}
	return m_pUserManager->SendUMMack(0,nMsgId,nAckType);
}

void CUserManagerApp::SaveLCAddressIndex(int nIndex)
{
#ifdef _QT_MAKE_
    if (m_sEbDataPath.isEmpty()) return;
#else
    if (m_sEbDataPath.empty()) return;
#endif
	char sTempIniFile[260];
#ifdef _QT_MAKE_
    sprintf(sTempIniFile,"%s/temp.ini",m_sEbDataPath.toLocal8Bit().constData());
#else
	sprintf(sTempIniFile,"%s\\temp.ini",m_sEbDataPath.c_str());
#endif
	char lpszBuffer[24];
	if (m_nCurrentIndex>=0)
	{
		sprintf(lpszBuffer,"%d",m_nCurrentIndex);
#ifdef WIN32
        WritePrivateProfileStringA(m_sServerAddress.c_str(),_T("lc_index"),lpszBuffer,sTempIniFile);
#else
        WritePrivateProfileStringABoost(m_sServerAddress.c_str(),_T("lc_index"),lpszBuffer,sTempIniFile);
#endif
	}
	//sprintf(lpszBuffer,"%lld",(mycp::bigint)time(0));
    //WritePrivateProfileStringA(m_sServerAddress.c_str(),_T("last_time"),lpszBuffer,sTempIniFile);
}
void CUserManagerApp::SaveLCAddressList(const tstring& sFullAddress)
{
	char sTempIniFile[260];
#ifdef _QT_MAKE_
    sprintf(sTempIniFile,"%s/temp.ini",m_sEbDataPath.toLocal8Bit().constData());
#else
    sprintf(sTempIniFile,"%s\\temp.ini",m_sEbDataPath.c_str());
#endif
#ifdef WIN32
    WritePrivateProfileStringA(m_sServerAddress.c_str(),_T("lc_list"),sFullAddress.c_str(),sTempIniFile);
#else
    WritePrivateProfileStringABoost(m_sServerAddress.c_str(),_T("lc_list"),sFullAddress.c_str(),sTempIniFile);
#endif
	char lpszBuffer[24];
	sprintf(lpszBuffer,"%lld",(mycp::bigint)time(0));
#ifdef WIN32
    WritePrivateProfileStringA(m_sServerAddress.c_str(),_T("last_time"),lpszBuffer,sTempIniFile);
#else
    WritePrivateProfileStringABoost(m_sServerAddress.c_str(),_T("last_time"),lpszBuffer,sTempIniFile);
#endif
}
tstring CUserManagerApp::GetLCAddressList(int& nOutLCIndex, time_t& pOutLastTime)
{
	char lpszBuffer[1024];
	char sTempIniFile[260];
#ifdef _QT_MAKE_
    sprintf(sTempIniFile,"%s/temp.ini",m_sEbDataPath.toLocal8Bit().constData());
#else
	sprintf(sTempIniFile,"%s\\temp.ini",m_sEbDataPath.c_str());
#endif
	
#ifdef WIN32
    nOutLCIndex = GetPrivateProfileIntA(m_sServerAddress.c_str(),_T("lc_index"),0,sTempIniFile);
#else
    nOutLCIndex = GetPrivateProfileIntABoost(m_sServerAddress.c_str(),_T("lc_index"),0,sTempIniFile);
#endif
	memset(lpszBuffer,0,1024);
#ifdef WIN32
    GetPrivateProfileStringA(m_sServerAddress.c_str(),_T("last_time"),"",lpszBuffer,1023,sTempIniFile);
#else
    GetPrivateProfileStringABoost(m_sServerAddress.c_str(),_T("last_time"),"",lpszBuffer,1023,sTempIniFile);
#endif
	pOutLastTime = cgc_atoi64(lpszBuffer);

	memset(lpszBuffer,0,1024);
#ifdef WIN32
    GetPrivateProfileStringA(m_sServerAddress.c_str(),_T("lc_list"),"",lpszBuffer,1023,sTempIniFile);
#else
    GetPrivateProfileStringABoost(m_sServerAddress.c_str(),_T("lc_list"),"",lpszBuffer,1023,sTempIniFile);
#endif
	return lpszBuffer;
}
void CUserManagerApp::SaveAddressList(const tstring& sFullAddress)
{
	std::vector<tstring> pList;
	if (ParseString(sFullAddress,":",false,pList)==2)
	{
		const tstring sAddress(pList[0]);
		const tstring sIp = entboost::GetHostIp(sAddress.c_str(),sAddress.c_str());
		if (sAddress!=sIp)
		{
			char sTempIniFile[260];
#ifdef _QT_MAKE_
            sprintf(sTempIniFile,"%s/temp.ini",m_sAppDataTempPath.toLocal8Bit().constData());
#else
            sprintf(sTempIniFile,"%s\\temp.ini",m_sAppDataTempPath.c_str());
#endif
#ifdef WIN32
            WritePrivateProfileStringA(_T("address_list"),sAddress.c_str(),sIp.c_str(),sTempIniFile);
#else
            WritePrivateProfileStringABoost(_T("address_list"),sAddress.c_str(),sIp.c_str(),sTempIniFile);
#endif
		}
	}
}
tstring CUserManagerApp::GetAddressList(const tstring& sFullAddress)
{
	tstring sAddress = sFullAddress;
	std::vector<tstring> pList;
	if (ParseString(sFullAddress,":",false,pList)==2)
	{
		sAddress = pList[0];
	}
	char sTempIniFile[260];
#ifdef _QT_MAKE_
    sprintf(sTempIniFile,"%s/temp.ini",m_sAppDataTempPath.toLocal8Bit().constData());
#else
    sprintf(sTempIniFile,"%s\\temp.ini",m_sAppDataTempPath.c_str());
#endif
	char lpszIp[101];
	memset(lpszIp,0,101);
#ifdef WIN32
    GetPrivateProfileStringA(_T("address_list"),sAddress.c_str(),"",lpszIp,100,sTempIniFile);
#else
    GetPrivateProfileStringABoost(_T("address_list"),sAddress.c_str(),"",lpszIp,100,sTempIniFile);
#endif
	if (strlen(lpszIp)==0)
	{
		return "";
	}

	tstring result = lpszIp;
	if (pList.size()==2)
	{
		result.append(":");
		result.append(pList[1]);
	}
#ifdef _QT_MAKE_
    LogMessage("GetAddressList... m_sAppDataTempPath=%s,sFullAddres%s,result=%s,size=%d\r\n",m_sAppDataTempPath.toStdString().c_str(),sFullAddress.c_str(),result.c_str(),(int)pList.size());
#else
    LogMessage("GetAddressList... m_sAppDataTempPath=%s,sFullAddres%s,result=%s,size=%d\r\n",m_sAppDataTempPath.c_str(),sFullAddress.c_str(),result.c_str(),(int)pList.size());
#endif
	return result;
}

void CUserManagerApp::GetLocalIp(void)
{
	if (m_pLocalHostIp.empty())
	{
		std::vector<tagNetworkCfg> pNetworkCfg;
		GetNetworkCfg(pNetworkCfg,false);	// **有可能取到二个以上，其中有一个是不对的
		if (pNetworkCfg.size()>1)
        {
#ifdef Q_OS_ANDROID
            for (int i=0;i<(int)pNetworkCfg.size();i++) {
                m_pLocalHostIp.push_back(pNetworkCfg[i].szIP);
            }
            /// for test
            /// 192.168.1.123;10.0.2.15
//            std::string sIp;
//            for (int i=0;i<(int)pNetworkCfg.size();i++) {
//                sIp += pNetworkCfg[i].szIP;
//                sIp += ";";
//            }
//            m_pLocalHostIp.push_back(sIp);
#else
			// 做交叉比较，去掉没用的IP地址，取出真实IP地址；
			CLockMap<tstring,bool> pIpList;
			for (int i=0;i<(int)pNetworkCfg.size();i++)
				pIpList.insert(pNetworkCfg[i].szIP,true);
			std::vector<tstring> pLocalHostIp;
			GetLocalHostIp(pLocalHostIp);
			for (int i=0;i<(int)pLocalHostIp.size();i++)
			{
				const tstring sIp = pLocalHostIp[i];
				if (pIpList.exist(sIp))
				{
					m_pLocalHostIp.push_back(sIp);
					break;
				}
			}
			if (m_pLocalHostIp.empty())
			{
				GetLocalHostIp(m_pLocalHostIp);
            }
#endif
		}else if (pNetworkCfg.size()==1)
		{
			m_pLocalHostIp.push_back(pNetworkCfg[0].szIP);
		}else
		{
			GetLocalHostIp(m_pLocalHostIp);
		}
	}
}

int CUserManagerApp::Logon(const CPOPLogonInfo::pointer & pLogonInfo, bool bReLogon)
{
	GetLocalIp();

	if ((pLogonInfo->m_nLogonType&EB_LOGON_TYPE_EMAIL)==EB_LOGON_TYPE_EMAIL && !m_sAccountPrefix.empty() && !IsFullNumber(pLogonInfo->m_sAccount.c_str(),pLogonInfo->m_sAccount.size()) && !pLogonInfo->m_sAccount.empty() && pLogonInfo->m_sAccount.find(m_sAccountPrefix)==std::string::npos)	// "@"非邮箱才处理
	//if (!m_sAccountPrefix.empty() && !pLogonInfo->m_sAccount.empty() && pLogonInfo->m_sAccount.substr(0,m_sAccountPrefix.size())!=m_sAccountPrefix)
	{
		pLogonInfo->m_sAccount = m_sAccountPrefix+pLogonInfo->m_sAccount;
	}
	pLogonInfo->m_nPasswdAuthMode = this->m_nPasswdAuthMode;

	if (m_nSDKVersion==0 || m_sCallKey.empty())
	{
		CEBParseSetting theSetting;
#ifdef _QT_MAKE_
        const std::string sSettingFile = m_sAppPath.toLocal8Bit().toStdString()+"/datas/setting";
#else
		const tstring sSettingFile = m_sAppPath + _T("\\datas\\setting");
#endif
		theSetting.load(sSettingFile.c_str());
		m_sCallKey = theSetting.GetCallKey();
		m_nSDKVersion = theSetting.GetSDKVersion();
	}

	if (bReLogon || m_userStatus != US_Logoned)//m_userStatus == US_Invalidate || m_userStatus == US_Logout || m_userStatus == US_LogonError)
	{
		m_tLogonCenter = time(0);
		if (m_pLogonCenter.get()!=NULL &&
			(m_nLcServerState!=EB_SS_INIT || m_tReLogonTime>0 || m_pLogonCenter->GetAddress().address()!=m_sServerAddress))
		{
			m_pLogonCenter->Stop();
			m_pLogonCenter.reset();
		}
		m_nLcServerState = EB_SS_INIT;
		if (m_pLogonCenter.get()==NULL)
		{
			bool bRetry = false;
			m_pLogonCenter = CPOPCLogonCenter::create(this);
			m_pLogonCenter->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
			int ret = m_pLogonCenter->Start(CCgcAddress(m_sServerAddress, CCgcAddress::ST_UDP),0,1);
			if (ret==-1)	
			{
				// 域名解析问题，再试一次；
				m_pLogonCenter.reset();
				const tstring sServerAddress = GetAddressList(m_sServerAddress);
				if (sServerAddress.empty()) return ret;
				m_pLogonCenter = CPOPCLogonCenter::create(this);
				m_pLogonCenter->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
				ret = m_pLogonCenter->Start(CCgcAddress(sServerAddress, CCgcAddress::ST_UDP),0,1);
				bRetry = true;
			}
			if (ret!=0)
			{
				m_pLogonCenter.reset();
				return ret;
			}
			// 成功记下，域名和IP地址；
			if (!bRetry)
			{
				SaveAddressList(m_sServerAddress);
			}
		}

		// 实现游客重新登录，使用同一个帐号??
		//if (m_userStatus == US_Invalidate && pLogonInfo->m_nLogonType==EB_LOGON_TYPE_VISITOR)
		//{
		//	pLogonInfo->m_sAccount = m_pLogonCenter->GetLogonAccount();
		//}
		m_userStatus = US_Logging;
		if ((!pLogonInfo->m_sOAuthKey.empty() || !pLogonInfo->m_sPassword.empty()) && (pLogonInfo->m_nLogonType&EB_LOGON_TYPE_UID)==0)
		{
			// 正常密码登录，需要二次登录；（UID登录除外）
			CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_LOGON);
			pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("logon-info", (mycp::cgcObject::pointer)pLogonInfo));
			return m_pLogonCenter->SendLCQuery(0,m_sDevAppId,m_sDevAppOnlineKey,EB_REQ_TYPE_LOGON,pLogonInfo->m_nLogonType,0,"",0,0,pLogonInfo->m_sAccount,pSotpRequestInfo);
		}else
		{
			// relogon,oauth
			return m_pLogonCenter->SendLCLogon(m_sLogonUserData,m_nSDKVersion,0,pLogonInfo,m_sDevAppId,m_sDevAppOnlineKey);
		}
	}else
	{
		return 1;
	}
}

int CUserManagerApp::ReLogon(void)
{
	if (m_userStatus == US_Logout)
		return -1;

	CPOPLogonInfo::pointer pLogonInfo = CPOPLogonInfo::create(m_pMyAccountInfo->GetLogonType());
	pLogonInfo->m_nLineState = m_pMyAccountInfo->GetLineState();
	pLogonInfo->m_nUserId = m_pMyAccountInfo->GetUserId();
	pLogonInfo->m_sAccount = m_pMyAccountInfo->GetAccount();
	pLogonInfo->m_sACMKey = m_pMyAccountInfo->GetACMKey("");
	return Logon(pLogonInfo,true);
}

int CUserManagerApp::SetDevAppId(mycp::bigint sAppId, const tstring& sAppKey,bool bReLoadAppOnlineKey)
{
	if (!bReLoadAppOnlineKey && sAppId==m_sDevAppId && !m_sDevAppOnlineKey.empty())
	{
		return 0;
	}
	if (m_pProcessThread.get() == NULL)
	{
		m_bKilled = false;
        m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CUserManagerApp::process_thread_svr, this)));
//		boost::thread_attributes attrs;
//		attrs.set_stack_size(CGC_THREAD_STACK_MIN);
//		m_pProcessThread = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CUserManagerApp::process_thread_svr, this)));
	}

	if (bReLoadAppOnlineKey || m_userStatus == US_Invalidate || m_userStatus == US_Logout || m_userStatus == US_LogonError)
	{
		m_tLogonCenter = time(0);
		if (m_pLogonCenter.get()!=NULL &&
			(m_tReLogonTime>0 || m_pLogonCenter->GetAddress().address()!=m_sServerAddress))
		{
			m_pLogonCenter->Stop();
			m_pLogonCenter.reset();
		}
//		LogMessage("SetDevAppId, m_sServerAddress=%s,(m_pLogonCenter=0x%x)\r\n",m_sServerAddress.c_str(),(int)m_pLogonCenter.get());

		if (m_pLogonCenter.get()==NULL)
		{
			bool bRetry = false;
			m_pLogonCenter = CPOPCLogonCenter::create(this);
			m_pLogonCenter->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
			int ret = m_pLogonCenter->Start(CCgcAddress(m_sServerAddress, CCgcAddress::ST_UDP),0,1);
			if (ret==-1)	
			{
                /// 域名解析问题，再试一次；
				m_pLogonCenter.reset();
				const tstring sServerAddress = GetAddressList(m_sServerAddress);
                if (sServerAddress.empty()) {
                    if (m_pHwnd!=0) {
#ifdef _QT_MAKE_
                        EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_APPID_ERROR);
                        pEvent->SetEventParameter((long)EB_STATE_ERROR);
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
                        ::PostMessage(m_pHwnd, EB_WM_APPID_ERROR, EB_STATE_ERROR, 0);
#endif
                    }
                    return ret;
                }
				m_pLogonCenter = CPOPCLogonCenter::create(this);
				m_pLogonCenter->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
				ret = m_pLogonCenter->Start(CCgcAddress(sServerAddress, CCgcAddress::ST_UDP),0,1);
				bRetry = true;
				//LogMessage("SetDevAppId2... m_sServerAddress=%s(%s),ret=%d\r\n",m_sServerAddress.c_str(),sServerAddress.c_str(),ret);
			}
            if (ret!=0) {
				m_pLogonCenter.reset();
                if (m_pHwnd!=0) {
    #ifdef _QT_MAKE_
                    EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_APPID_ERROR);
                    pEvent->SetEventParameter((long)EB_STATE_ERROR);
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
    #else
                    ::PostMessage(m_pHwnd, EB_WM_APPID_ERROR, EB_STATE_ERROR, 0);
    #endif
                }
				return ret;
			}
            /// 成功记下，域名和IP地址；
			if (!bRetry)
			{
				SaveAddressList(m_sServerAddress);
			}
			//m_pLogonCenter = CPOPCLogonCenter::create(this);
			//m_pLogonCenter->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
			//int ret = m_pLogonCenter->Start(CCgcAddress(m_sServerAddress, CCgcAddress::ST_UDP));
			//if (ret!=0)
			//{
			//	m_pLogonCenter.reset();
			//	return ret;
			//}
		}

		CPOPLogonInfo::pointer pLogonInfo = CPOPLogonInfo::create(EB_LOGON_TYPE_APPID);
		pLogonInfo->m_sAppId = sAppId;
		pLogonInfo->m_sPassword = sAppKey;
		m_userStatus = US_DevAppIdLogoning;
		CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_LOGON);
		pSotpRequestInfo->SetRequestTimeout(6);
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("app-id", sAppId));
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("app-key", sAppKey));
//		LogMessage("SetDevAppId, m_pLogonCenter->SendLCLogon...\r\n");
        const int ret = m_pLogonCenter->SendLCLogon(m_sLogonUserData,m_nSDKVersion,0,pLogonInfo,0,"",pSotpRequestInfo);
        if (ret!=0) {
#ifdef _QT_MAKE_
            EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_APPID_ERROR);
            pEvent->SetEventParameter((long)EB_STATE_ERROR);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#endif
        }
		return ret;
    }else
	{
		return 1;
	}
}
void CUserManagerApp::SetDevAppOnlineKey(mycp::bigint sAppId, const tstring& sAppOnlineKey)
{
	m_sDevAppId = sAppId;
	m_sDevAppOnlineKey = sAppOnlineKey;
}

int CUserManagerApp::Register(const tstring& sAccount, const tstring& sPwd, const tstring& sUserName, const tstring& sEntName)
{
	tstring sFullAccount(sAccount);
	if (!sFullAccount.empty() && // * 支持空可选参数；
		!m_sAccountPrefix.empty() && !IsFullNumber(sFullAccount.c_str(),sFullAccount.size()) && sFullAccount.find(m_sAccountPrefix)==std::string::npos)	// "@"非邮箱才处理
	{
		sFullAccount = m_sAccountPrefix+sAccount;
	}
	//MessageBox(NULL,m_sAccountPrefix.c_str(),sFullAccount.c_str(),MB_OK);

	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_REG);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from", sFullAccount));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("pwd", sPwd));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("user-name", sUserName));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ent-name", sEntName));
	return QueryUser(EB_REQ_TYPE_REG,0,0,0,0,sFullAccount,pSotpRequestInfo);
}

void CUserManagerApp::LoadInfo(int nLoadSubFunc,int nLoadMsg,int nLoadGroupVer,mycp::bigint nLoadOLSGroupId, int nLoadEntGroupOLS,int nLoadUserGroupOLS,
							   mycp::bigint nLoadGroupOLSGid, const tstring& sLoadGroupOLSGid)
{
	if (m_pUserManager.get() == NULL) return;
	if (m_userStatus != US_Logoned)
	{
		return;
	}

	//try
	//{
	//	// ?在这里处理，网络掉线，会话失效，重新登录成功后，重新呼叫
	//	BoostReadLock lockCallInfoList(theCallInfoList.mutex());
	//	CLockMap<mycp::bigint, CEBCallInfo::pointer>::const_iterator pIter = theCallInfoList.begin();
	//	for (;pIter!=theCallInfoList.end(); pIter++)
	//	{
	//		CEBCallInfo::pointer pCallInfo = pIter->second;
	//		if (pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE)
	//		{
	//			// 会话失效，需要重新呼叫
	//			if (pCallInfo->m_pChatRoom.get() != NULL)
	//			{
	//				tstring sUMServer;
	//				tstring sCMServer;
	//				int nPort = 0;
	//				entboost::GetAddressPort(m_pUserManager->GetAddress().address().c_str(),sUMServer,nPort);
	//				entboost::GetAddressPort(pCallInfo->m_sCmServer.c_str(),sCMServer,nPort);
	//				bool bSameServer = entboost::GetHostIp(sUMServer.c_str(),sUMServer.c_str()) == entboost::GetHostIp(sCMServer.c_str(),sCMServer.c_str());
	//				if (bSameServer)
	//				{
	//					// UM，CM同一台，清空后，重新连接
	//					// 不同服务器，不需要重新连接
	//					//pCallInfo->m_pChatRoom->Stop();
	//					m_pClearChatRoom.add(pCallInfo->m_pChatRoom);
	//					pCallInfo->m_pChatRoom.reset();
	//				}
	//			}
	//			lockCallInfoList.unlock();
	//			this->InviteCall(pCallInfo,0);
	//			lockCallInfoList.lock();
	//		}
	//	}
	//}catch(std::exception&)
	//{
	//}catch(...)
	//{
	//}
	// ** 游客也可以加载，应该订购
	//if (!this->m_pMyAccountInfo->IsLogonVisitor())
	{
		if (!sLoadGroupOLSGid.empty())
			m_pUserManager->SendUMSMLoad(nLoadSubFunc,nLoadMsg,nLoadGroupVer,nLoadOLSGroupId,"",nLoadEntGroupOLS,nLoadUserGroupOLS,sLoadGroupOLSGid);
		else
		{
			char lpszBuffer[24];
			if (nLoadGroupOLSGid>0)
				sprintf(lpszBuffer,"%lld",nLoadGroupOLSGid);
			else
				memset(lpszBuffer,0,sizeof(lpszBuffer));
			m_pUserManager->SendUMSMLoad(nLoadSubFunc,nLoadMsg,nLoadGroupVer,nLoadOLSGroupId,"",nLoadEntGroupOLS,nLoadUserGroupOLS,lpszBuffer);
		}
	}
}
void CUserManagerApp::Logout(void)
{
	// 必须放在前面处理
	if (m_userStatus == US_Logging || m_userStatus == US_Logoned)
	{
		if (m_pUserManager.get() != NULL)
		{
			m_pUserManager->SendUMSMOffline();
		}
		m_userStatus = US_Logout;	// ?
	}

	{
		BoostWriteLock wtLock(m_mutexEBDatas);
		if (m_pEBDatas.get()!=NULL)
		{
			m_pEBDatas->close();
			m_pEBDatas.reset();
		}
	}
    m_pProcessMsgList.clear();
	theChatRoomList.clear();
	theResourceChatRoom.clear();

	//m_pVideoStreamCallback = NULL;
	//m_dwVideoCallbackData = 0;
	m_pDefaultEntEmployeeInfo.reset();
	m_pClearChatRoom.clear();
	m_pHangupUm.clear();
	theUserManagerList.clear();
	//theCallInfoList.clear();
	ClearAllCallInfo();
	theOwnerCallUser.clear();
	theOwnerCallGroup.clear();
	//theOffLineUser.clear();
	//theAcceptUser.clear()
	theUGInfoList.clear();
	theContactList1.clear();
	theContactList2.clear();
	theEditContactList.clear();
	theResourceList.clear();
	theTempResourceList.clear();
	m_pDefaultEnterpriseInfo.reset();
	theEnterpriseList.clear();
	theDepartmentList.clear();
	theEmployeeList.clear();
	theMyEmployeeList.clear();
	theEditDepartmentList.clear();
	theTempDepartmentList.clear();
	theEditEmployeeList.clear();
	theEmotionList.clear();
	theSubscribeFuncInfolist.clear();
	theAreaInfoList.clear();
	m_nLastStateCode = EB_STATE_OK;
	m_pDesktopHwnd.clear();
	m_pVideoHwnd.clear();
}

int CUserManagerApp::GetSubGroupMemberSize(eb::bigint nParentGroupId) const
{
	if (nParentGroupId==0) return 0;
	int nResult = 0;
	AUTO_CONST_RLOCK(theDepartmentList);
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
	for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++)
	{
		const CEBGroupInfo::pointer pDepartmentInfo = pIterDepartment->second;
		if (pDepartmentInfo->m_sParentCode==nParentGroupId)
		{
			// 加上子部门数量
			nResult += pDepartmentInfo->m_nEmpCount;
			// 再统计子部门的子部门
			nResult += GetSubGroupMemberSize(pDepartmentInfo->m_sGroupCode);
		}
	}
	return nResult;
}
bool CUserManagerApp::GetSubGroupMemberSize(eb::bigint nParentGroupId, int& pOutMemberSize, int& pOutOnlineSize) const
{
	if (nParentGroupId==0) return false;
	AUTO_CONST_RLOCK(theDepartmentList);
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
	for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++)
	{
		const CEBGroupInfo::pointer pDepartmentInfo = pIterDepartment->second;
		if (pDepartmentInfo->m_sParentCode==nParentGroupId)
		{
			// 加上子部门数量
			pOutMemberSize += pDepartmentInfo->m_nEmpCount;
			pOutOnlineSize += pDepartmentInfo->m_nOnlineSize;
			// 再统计子部门的子部门
			GetSubGroupMemberSize(pDepartmentInfo->m_sGroupCode,pOutMemberSize,pOutOnlineSize);
		}
	}
	return true;
}

int CUserManagerApp::GetGroupMemberSize(eb::bigint nGroupId, int nGetType) const
{
	// 参数：nGetType 0=按照系统配置是否统计子部门数量；1=只统计当前部门数量；2=统计子部门数量
	int nResult = -1;
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(nGroupId, pDepartmentInfo))
	{
		nResult = pDepartmentInfo->m_nEmpCount;
		if(nGetType==2 || (nGetType==0 && (m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_STATSUB_GROUP_MEMBER)==0))
		{
			// 统计子部门人数
			nResult += GetSubGroupMemberSize(nGroupId);
		}
	}
	return nResult;
}
bool CUserManagerApp::GetGroupMemberSize(eb::bigint nGroupId, int nGetType,int& pOutMemberSize, int& pOutOnlineSize) const
{
	pOutMemberSize = -1;
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(nGroupId, pDepartmentInfo))
	{
		pOutMemberSize = pDepartmentInfo->m_nEmpCount;
		pOutOnlineSize = pDepartmentInfo->m_nOnlineSize;
		if(nGetType==2 || (nGetType==0 && (m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_STATSUB_GROUP_MEMBER)==0))
		{
            /// 统计子部门人数
			GetSubGroupMemberSize(nGroupId, pOutMemberSize, pOutOnlineSize);
		}
		return true;
	}
	return false;
}

int CUserManagerApp::GetSubGroupOnlineSize(eb::bigint nParentGroupId) const
{
	if (nParentGroupId==0) return 0;
	int nResult = 0;
	AUTO_CONST_RLOCK(theDepartmentList);
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
	for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++)
	{
		const CEBGroupInfo::pointer pDepartmentInfo = pIterDepartment->second;
		if (pDepartmentInfo->m_sParentCode==nParentGroupId)
		{
			// 加上子部门在线人数数量
			nResult += pDepartmentInfo->m_nOnlineSize;
			// 再统计子部门的子部门
			nResult += GetSubGroupOnlineSize(pDepartmentInfo->m_sGroupCode);
		}
	}
	return nResult;
}
int CUserManagerApp::GetGroupOnlineSize(eb::bigint nGroupId, int nGetType) const
{
	// 参数：nGetType 0=按照系统配置是否统计子部门数量；1=只统计当前部门数量；2=统计子部门数量
	int nResult = -1;
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(nGroupId, pDepartmentInfo))
	{
		nResult = pDepartmentInfo->m_nOnlineSize;
		if(nGetType==2 || (nGetType==0 && (m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_DISABLE_STATSUB_GROUP_MEMBER)==0))
		{
			// 统计子部门人数
			nResult += GetSubGroupOnlineSize(nGroupId);
		}
	}
	return nResult;
}

int CUserManagerApp::GetAreaInfo(int nParent, unsigned int nParameter)
{
	CEBAreaInfo::pointer pParentAreaInfo;
	if (theAreaInfoList.find(nParent,pParentAreaInfo) && pParentAreaInfo->m_pAreaInfoList.size()>=(size_t)pParentAreaInfo->m_nSize)
	{
		// 
		BoostReadLock rdlock(pParentAreaInfo->m_pAreaInfoList.mutex());
		CLockMap<int,CEBAreaInfo::pointer>::iterator pIter = pParentAreaInfo->m_pAreaInfoList.begin();
		for (; pIter!=pParentAreaInfo->m_pAreaInfoList.end(); pIter++)
		{
			const CEBAreaInfo::pointer& pAreaInfo = pIter->second;
			if (m_callback)
				m_callback->onAreaInfo(pAreaInfo.get(),nParameter);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_AreaInfo * pEvent = new EB_AreaInfo(pAreaInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_AREA_INFO);
				pEvent->SetEventParameter((long)nParameter);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_AREA_INFO, (WPARAM)(const EB_AreaInfo*)pAreaInfo.get(), (LPARAM)nParameter);
#endif
			}
		}
		return 0;
	}else
	{
		if (m_pUserManager.get() == NULL) return -1;
		CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("type", (int)EB_DICT_AREA_DICT_LIST));
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("parameter", (mycp::bigint)nParameter));
		const mycp::bigint nValue = nParent;
		return m_pUserManager->SendDictLoad(EB_DICT_AREA_DICT_LIST,nValue,pSotpRequestInfo);
	}
}

int CUserManagerApp::CheckVersion(const tstring& sClientVersion)
{
	if (m_pUserManager.get() == NULL) return -1;
	if (GetUserStatus() != CUserManagerApp::US_Logoned)
	{
		return -2;
	}
	m_pVersionInfo.m_sVersion = sClientVersion;
	return m_pUserManager->SendVersinoCheck(m_sDevAppId,sClientVersion,m_pMyAccountInfo->GetLogonType());
}

int CUserManagerApp::SetDepHead(mycp::bigint sDepCode,const tstring& sFilePath)
{
	if (m_pUserManager.get() == NULL) return -1;
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("dep-code", sDepCode));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("file-path", sFilePath));
	return m_pUserManager->SendUMSHead(0,sDepCode,0,pSotpRequestInfo);
}
int CUserManagerApp::SetDepResHead(mycp::bigint sDepCode,mycp::bigint sResourceId)
{
	if (m_pUserManager.get() == NULL) return -1;
#ifdef _QT_MAKE_
    const QString sResourceFile = QString("%1/%2").arg(m_sEbResourcePath).arg(sResourceId);
	if (!QFileInfo::exists(sResourceFile)) {
#else
    char sResourceFile[260];
    sprintf(sResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),sResourceId);
	if (!::PathFileExistsA(sResourceFile)) {
#endif
		return -1;
	}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("dep-code", sDepCode));
	return m_pUserManager->SendUMSHead(0,sDepCode,sResourceId,pSotpRequestInfo);
}
int CUserManagerApp::LoadRes(int nVerifyValid,mycp::bigint nResourceId,int nType,mycp::bigint nFromId,EB_RESOURCE_FROM_TYPE nFromType,int nOffset,int nLimit)
{
	if (m_pUserManager.get() == NULL) return -1;
	if ((nFromId>0 && nFromType==EB_RESOURCE_FROM_TYPE_GROUP) ||	// * 加载群共享
		(nFromId==0 && nFromType==EB_RESOURCE_FROM_TYPE_USER))			// * 加载个人云享
	{
		bool bAddCheckResourceProcess = false;
		AUTO_CONST_RLOCK(theResourceList);
		CLockMap<mycp::bigint, CEBResourceInfo::pointer>::const_iterator pIter = theResourceList.begin();
		for (; pIter!=theResourceList.end(); pIter++)
		{
			const CEBResourceInfo::pointer& pResourceInfo = pIter->second;
			if ((pResourceInfo->m_nFromType == nFromType && pResourceInfo->m_nFromId==nFromId) ||
				(pResourceInfo->m_nFromType == EB_RESOURCE_FROM_TYPE_USER && (pResourceInfo->m_nFromId==0 || pResourceInfo->m_nFromId==m_pMyAccountInfo->GetUserId())))	// ?
			{
				// 记录刷新标识
				pResourceInfo->m_nItemData |= EB_RESOURCE_DATA_REFRESH;
				if (!bAddCheckResourceProcess)
					bAddCheckResourceProcess = true;
			}
		}
		// 启动一检查线程
		if (bAddCheckResourceProcess)
		{
			CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_CHECK_RESOURCE_REFRESH);
			pProcessMsgInfo->m_tProcessTime = time(0)+5;
			m_pProcessMsgList.add(pProcessMsgInfo);
			m_pProcessCheckResourceRefresh = pProcessMsgInfo;
		}
	}
	return m_pUserManager->SendRLoad(nVerifyValid,nResourceId,nType,nFromId,nFromType,nOffset,nLimit,0,0);
}
int CUserManagerApp::AddDirRes(const tstring& sDirName,mycp::bigint sParentResId,mycp::bigint nFromId, EB_RESOURCE_FROM_TYPE nFromType)
{
	if (m_pUserManager.get() == NULL) return -1;
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("type", (int)EB_RESOURCE_DIR));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("res-name", sDirName));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("pr_id", sParentResId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from-id", nFromId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from-type", (int)nFromType));
	return m_pUserManager->SendRNew(sDirName,EB_RESOURCE_DIR,sParentResId,"",EB_RESOURCE_SHARE_UNKNOWN,nFromId,nFromType,pSotpRequestInfo);
	//pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("dep-code", sDepCode));
	//pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("end-code", sEntCode));
	//return m_pUserManager->SendRNew(sDirName,EB_RESOURCE_DIR,sParentResId,"",sDepCode,sEntCode,0,0,0,pSotpRequestInfo);
}
int CUserManagerApp::AddFileRes(const tstring& sFileName,const tstring& sFilePath,const tstring& sDescription,mycp::bigint sParentResId,EB_RESOURCE_SHARE_TYPE nShare, mycp::bigint nFromId, EB_RESOURCE_FROM_TYPE nFromType)
{
	if (m_pUserManager.get() == NULL) return -1;
	if (sParentResId>0)
	{
		if (!theResourceList.exist(sParentResId)) return -2;
	}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("type", (int)EB_RESOURCE_FILE));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("res-name", sFileName));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("file-path", sFilePath));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("description", sDescription));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("pr_id", sParentResId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("share", (int)nShare));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from-id", nFromId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from-type", (int)nFromType));
	return m_pUserManager->SendRNew(sFileName,EB_RESOURCE_FILE,sParentResId,sDescription,nShare,nFromId,nFromType,pSotpRequestInfo);
	//pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("dep-code", sDepCode));
	//pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("end-code", sEntCode));
	//return m_pUserManager->SendRNew(sFileName,EB_RESOURCE_FILE,sParentResId,sDescription,sDepCode,sEntCode,nShare,0,0,pSotpRequestInfo);
}
int CUserManagerApp::AddNoteRes(const tstring& sNoteName,const tstring& sDescription,mycp::bigint sParentResId,mycp::bigint nFromId, EB_RESOURCE_FROM_TYPE nFromType)
{
	if (m_pUserManager.get() == NULL) return -1;
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	return m_pUserManager->SendRNew(sNoteName,EB_RESOURCE_NOTE,sParentResId,sDescription,EB_RESOURCE_SHARE_UNKNOWN,nFromId,nFromType,pSotpRequestInfo);
}
int CUserManagerApp::EditRes(mycp::bigint sResourceId,const tstring& sName,const tstring& sDescription)
{
	if (m_pUserManager.get() == NULL)
	{
		return -1;
	}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("res-name", sName));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("description", sDescription));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("edit", true));
	return m_pUserManager->SendREdit(sResourceId,sName,sDescription,0,pSotpRequestInfo);
}
int CUserManagerApp::MoveRes2Dir(mycp::bigint sResourceId,mycp::bigint sParentResId)
{
	if (m_pUserManager.get() == NULL) return -1;
	CEBResourceInfo::pointer pResourceInfo;
	if (!theResourceList.find(sResourceId,pResourceInfo)) return -1;
	if (pResourceInfo->m_sParentResId == sParentResId) return 1;	// 不需要更新
	if (sParentResId>0)
	{
		if (!theResourceList.exist(sParentResId)) return -2;
	}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("old-parent-res-id", pResourceInfo->m_sParentResId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("new-parent-res-id", sParentResId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("move", true));
	return m_pUserManager->SendREdit(sResourceId,"","",sParentResId,pSotpRequestInfo);
}

bool CUserManagerApp::HasSubRes(mycp::bigint sParentResId) const
{
	AUTO_CONST_RLOCK(theResourceList);
	CLockMap<mycp::bigint, CEBResourceInfo::pointer>::const_iterator pIter = theResourceList.begin();
	for (; pIter!=theResourceList.end(); pIter++)
	{
		const CEBResourceInfo::pointer& pResourceInfo = pIter->second;
		if (pResourceInfo->m_sParentResId == sParentResId)
		{
			return true;
		}
	}
	return false;
}
int CUserManagerApp::DeleteRes(mycp::bigint sResourceId)
{
	if (m_pUserManager.get() == NULL) return -1;
	// 外面已经判断
	//CEBResourceInfo::pointer pResourceInfo;
	//if (!theResourceList.find(sResourceId,pResourceInfo)) return -1;
	//if (pResourceInfo->m_nResType == EB_RESOURCE_DIR)
	//{
	//	if (HasSubRes(pResourceInfo->m_sResId)) return 1;	// 不能删除非空目录
	//}

	CFilePathInfo::pointer pFilePathInfo;
	if (theResourceFilePath.find(sResourceId,pFilePathInfo,true) && pFilePathInfo->m_nMsgId>0)
	{
		CancelFileRes(sResourceId,pFilePathInfo->m_nMsgId);
	}
	//Cchatroom::pointer pChatRoom;
	//if (theResourceChatRoom.find(sResourceId,pChatRoom))
	//{
	//	CFilePathInfo::pointer pFilePathInfo;
	//	if (theResourceFilePath.find(sResourceId,pFilePathInfo))
	//	{
	//		pChatRoom->CancelSendingFile(pFilePathInfo->m_nMsgId);
	//	}
	//}
	//
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("r_id", sResourceId));
	return m_pUserManager->SendRDelete(0,sResourceId,pSotpRequestInfo);
}
int CUserManagerApp::DownloadFileRes(mycp::bigint sResourceId,const EBFileString& sSaveTo)
{
	//LogMessage("DownloadFileRes ResourceId=%lld\r\n",sResourceId);
	CEBResourceInfo::pointer pResourceInfo;
	if (!theResourceList.find(sResourceId,pResourceInfo) || pResourceInfo->m_sCmServer.empty())
	{
		if (sResourceId==0 || m_pUserManager.get() == NULL) return -1;
		CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("download_resource_id", sResourceId));
#ifdef _QT_MAKE_
        pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("download_save_to", sSaveTo.toStdString()));
#else
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("download_save_to", sSaveTo));
#endif
		//LogMessage("DownloadFileRes SendRLoad()...\r\n");
		m_pUserManager->SendRLoad(1,sResourceId,0,0,EB_RESOURCE_FROM_TYPE_UNKNOWN,-1,1,0,0,-1,0,pSotpRequestInfo);
		return 1;
	}
	if (pResourceInfo->m_nResType != EB_RESOURCE_FILE) return -2;
	if (theResourceChatRoom.exist(sResourceId) && theResourceFilePath.exist(sResourceId))
	{
		return -3;	// 正在下载，直接返回
	}
	
	// 请求下载资源文件
	const tstring sCmServerKey = GetCmServerKey(pResourceInfo->m_sCmServer);
	Cchatroom::pointer pChatRoom;
	if (!theChatRoomList.find(sCmServerKey,pChatRoom) || !pChatRoom->IsEnterOk())
	{
		//LogMessage("DownloadFileRes Cchatroom::create(), server=%s\r\n",pResourceInfo->m_sCmServer.c_str());
		pChatRoom = Cchatroom::create();
		pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
		pChatRoom->Start(pResourceInfo->m_sCmServer.c_str(), "",0,true,1);
		pChatRoom->SetCallBack(this);
		theChatRoomList.insert(sCmServerKey,pChatRoom);
	}
	theResourceChatRoom.insert(sResourceId,pChatRoom);
	theResourceFilePath.insert(sResourceId,CFilePathInfo::create(CFilePathInfo::FILE_PAHT_RESOURCE,sSaveTo,true));
	//pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
	//pChatRoom->RecviveResource(sResourceId,sSaveTo.c_str());
	CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE);
	pProcessMsgInfo->m_pChatRoom = pChatRoom;
	pProcessMsgInfo->m_nBigInt1 = sResourceId;
	pProcessMsgInfo->m_sString1 = sSaveTo;
	//pProcessMsgInfo->m_sString1.append(".ebtemp");
	m_pProcessMsgList.add(pProcessMsgInfo);
	return 0;
}
int CUserManagerApp::CancelFileRes(mycp::bigint sResourceId,mycp::bigint nMsgId)
{
	CEBResourceInfo::pointer pResourceInfo;
	if (!theResourceList.find(sResourceId,pResourceInfo))
	{
		return -1;
	}
	if (pResourceInfo->m_nResType != EB_RESOURCE_FILE) return -2;
	
	// 请求下载资源文件
	Cchatroom::pointer pChatRoom;
	if (theResourceChatRoom.find(sResourceId,pChatRoom))
	{
		pChatRoom->CancelSendingFile(nMsgId);
	}
	return 0;
}

mycp::bigint CUserManagerApp::GetUserId(void) const
{
	return m_pMyAccountInfo->GetUserId();
}
tstring CUserManagerApp::GetOnlineKey(void) const
{
	return m_pMyAccountInfo->m_pLogonList.GetOnlineKey("");
}

void CUserManagerApp::SendMyLineState(EB_USER_LINE_STATE nNewLineState)
{
	try
	{
		BoostReadLock lockMemberList(theMyEmployeeList.mutex());
		CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = theMyEmployeeList.begin();
		for (; pIter!=theMyEmployeeList.end(); pIter++)
		{
			const mycp::bigint sDepCode = pIter->first;
			const CEBMemberInfo::pointer& pMyEmployeeInfo = pIter->second;
			pMyEmployeeInfo->m_nLineState = nNewLineState;
			CEBGroupInfo::pointer pMyDepartmentInfo;
			if (theDepartmentList.find(sDepCode,pMyDepartmentInfo))
			{
				if (this->m_callback)
					m_callback->onUserStateChange(pMyEmployeeInfo.get(),true);
				if (m_pHwnd != NULL) {
#ifdef _QT_MAKE_
					EB_MemberInfo * pEvent = new EB_MemberInfo(pMyEmployeeInfo.get());
					pEvent->SetQEventType((QEvent::Type)EB_WM_USER_STATE_CHANGE);
                    pEvent->SetEventParameter(1);
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
					::SendMessage(m_pHwnd, EB_WM_USER_STATE_CHANGE,(WPARAM)(const EB_MemberInfo*)pMyEmployeeInfo.get(),1);
#endif
				}
			}
		}
	}catch(std::exception&)
	{
	}catch(...)
	{}
}
bool CUserManagerApp::ChangeLineState(EB_USER_LINE_STATE nNewLineState)
{
	if (m_pUserManager.get() == NULL) return false;
	if (m_pMyAccountInfo->GetLineState() == nNewLineState)
	{
		return true;
	}

	// 通知改变自己状况
	SendMyLineState(nNewLineState);
	int nRealLineState = nNewLineState;
	nRealLineState &= ~EB_USER_CHANGE_STATE;
	m_pMyAccountInfo->SetLineState((EB_USER_LINE_STATE)nRealLineState);
    m_pUserManager->SendUMSMOnline(m_nSDKVersion,
                                   m_pMyAccountInfo->GetLogonType(),
                                   m_pMyAccountInfo->GetUserId(),
                                   m_pMyAccountInfo->m_pLogonList.GetOnlineKey(""),
                                   nRealLineState|EB_USER_CHANGE_STATE,
                                   0,
                                   m_pMyAccountInfo->m_pLogonList.GetUserSignId(""),
                                   "");
	return true;
}
bool CUserManagerApp::UpdateInfo(const CPopParameterList& pInfoList)
{
	if (m_pUserManager.get() == NULL) return false;
	cgcParameter::pointer pSInfoParameter = pInfoList.getParameter("user-name");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetUserName(pSInfoParameter->getStr());
	}
	pSInfoParameter = pInfoList.getParameter("desc");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetDescription(pSInfoParameter->getStr());
	}
	pSInfoParameter = pInfoList.getParameter("old_pwd");	// 旧密码
	if (pSInfoParameter.get()!=NULL && m_nPasswdAuthMode!=PASSWD_AUTH_MODE_PBKDF2)
	{
		// 密码需要加密
		MD5 md5;
		char lpszBuffer[128];
		if (m_nPasswdAuthMode==PASSWD_AUTH_MODE_MD5)
		{
			// md5(passwd)
			sprintf(lpszBuffer,"%s",pSInfoParameter->getStr().c_str());
		}else
		{
			sprintf(lpszBuffer,"%lld%s",m_pMyAccountInfo->GetUserId(),pSInfoParameter->getStr().c_str());
		}
		md5.update((const unsigned char*)lpszBuffer,strlen(lpszBuffer));
		md5.finalize();
		const tstring sMD5Password = md5.hex_digest();
		pSInfoParameter->setStr(sMD5Password);
	}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo;
	//pSInfoParameter = pInfoList.getParameter("pwd");	// 新密码
	//if (pSInfoParameter.get()!=NULL)
	//{
	//	pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	//	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("new_pwd", pSInfoParameter->getStr()));
	//	// 密码需要加密
	//	MD5 md5;
	//	char lpszBuffer[128];
	//	sprintf(lpszBuffer,"%lld%s",m_pMyAccountInfo->GetUserId(),pSInfoParameter->getStr().c_str());
	//	md5.update((const unsigned char*)lpszBuffer,strlen(lpszBuffer));
	//	md5.finalize();
	//	const tstring sMD5Password = md5.hex_digest();
	//	pSInfoParameter->setStr(sMD5Password);
	//}else
	{

		pSInfoParameter = pInfoList.getParameter("passwd");	// 新密码
		if (pSInfoParameter.get()!=NULL)
		{
			pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
			pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("new_pwd", pSInfoParameter->getStr()));
			pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("change-pwd", 1));
			//m_pMyAccountInfo->SetPasswd(pSInfoParameter->getStr());
			if (m_nPasswdAuthMode!=PASSWD_AUTH_MODE_PBKDF2)
			{
				MD5 md5;
				char lpszBuffer[128];
				if (m_nPasswdAuthMode==PASSWD_AUTH_MODE_MD5)
					sprintf(lpszBuffer,"%s",pSInfoParameter->getStr().c_str());
				else
					sprintf(lpszBuffer,"%lld%s",m_pMyAccountInfo->GetUserId(),pSInfoParameter->getStr().c_str());
				md5.update((const unsigned char*)lpszBuffer,strlen(lpszBuffer));
				md5.finalize();
				const tstring sMD5Password = md5.hex_digest();
				pSInfoParameter->setStr(sMD5Password);
			}
		}
	}
	pSInfoParameter = pInfoList.getParameter("setting");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetSetting(pSInfoParameter->getInt());
	}
	pSInfoParameter = pInfoList.getParameter("default_emp");
	if (pSInfoParameter.get()!=NULL)
	{
		if (pSotpRequestInfo.get()==NULL)
			pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("default-emp-id", pSInfoParameter->getBigInt()));
		//m_pMyAccountInfo->SetDefaultEmpId(pSInfoParameter->getBigInt());
	}
	pSInfoParameter = pInfoList.getParameter("area1");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetArea1(pSInfoParameter->getInt());
		m_pMyAccountInfo->SetArea1String("");
		if (m_pMyAccountInfo->GetArea1()>0)
		{
			CEBAreaInfo::pointer pAreaInfo;
			if (theAreaInfoList.find(m_pMyAccountInfo->GetArea1(),pAreaInfo))
			{
				m_pMyAccountInfo->SetArea1String(pAreaInfo->m_sAreaName);
			}
		}
	}
	pSInfoParameter = pInfoList.getParameter("area2");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetArea2(pSInfoParameter->getInt());
		m_pMyAccountInfo->SetArea2String("");
		if (m_pMyAccountInfo->GetArea2()>0)
		{
			CEBAreaInfo::pointer pAreaInfo;
			if (theAreaInfoList.find(m_pMyAccountInfo->GetArea2(),pAreaInfo))
			{
				m_pMyAccountInfo->SetArea2String(pAreaInfo->m_sAreaName);
			}
		}
	}
	pSInfoParameter = pInfoList.getParameter("area3");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetArea3(pSInfoParameter->getInt());
		m_pMyAccountInfo->SetArea3String("");
		if (m_pMyAccountInfo->GetArea3()>0)
		{
			CEBAreaInfo::pointer pAreaInfo;
			if (theAreaInfoList.find(m_pMyAccountInfo->GetArea3(),pAreaInfo))
			{
				m_pMyAccountInfo->SetArea3String(pAreaInfo->m_sAreaName);
			}
		}
	}
	pSInfoParameter = pInfoList.getParameter("area4");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetArea4(pSInfoParameter->getInt());
		m_pMyAccountInfo->SetArea4String("");
		if (m_pMyAccountInfo->GetArea4()>0)
		{
			CEBAreaInfo::pointer pAreaInfo;
			if (theAreaInfoList.find(m_pMyAccountInfo->GetArea4(),pAreaInfo))
			{
				m_pMyAccountInfo->SetArea4String(pAreaInfo->m_sAreaName);
			}
		}
	}
	pSInfoParameter = pInfoList.getParameter("add");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetAddress(pSInfoParameter->getStr());
	}
	pSInfoParameter = pInfoList.getParameter("url");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetUrl(pSInfoParameter->getStr());
	}
	pSInfoParameter = pInfoList.getParameter("gender");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetGender((EB_GENDER_TYPE)pSInfoParameter->getInt());
	}
	pSInfoParameter = pInfoList.getParameter("tel");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetTel(pSInfoParameter->getStr());
	}
	pSInfoParameter = pInfoList.getParameter("mobile");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetMobile(pSInfoParameter->getStr());
	}
	pSInfoParameter = pInfoList.getParameter("email");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetEmail(pSInfoParameter->getStr());
	}
	pSInfoParameter = pInfoList.getParameter("birthday");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetBirthday(pSInfoParameter->getInt());
	}
	pSInfoParameter = pInfoList.getParameter("zipcode");
	if (pSInfoParameter.get()!=NULL)
	{
		m_pMyAccountInfo->SetZipCode(pSInfoParameter->getStr());
	}

	m_pUserManager->SendUMSInfo(0,0,pInfoList,pSotpRequestInfo);
	//m_pUserManager->SendUMSInfo(m_pMyAccountInfo->m_pLogonList.GetOnlineKey(m_pMyAccountInfo->GetAccount()),pInfoList);
	return true;
}
bool CUserManagerApp::SetDefaltMemberCode(mycp::bigint sNewDefaultMemberCode)
{
	if (sNewDefaultMemberCode>0)
	{
		CEBMemberInfo::pointer pEmployeeInfo;
		if (!theEmployeeList.find(sNewDefaultMemberCode,pEmployeeInfo))
		{
			return false;
		}
		if (pEmployeeInfo->m_nMemberUserId!=m_pMyAccountInfo->GetUserId())
		{
			return false;
		}
		//m_pDefaultEntEmployeeInfo = pEmployeeInfo;
	}else
	{
		m_pDefaultEntEmployeeInfo.reset();
	}
	CPopParameterList pParameter;
	pParameter.SetParameter(CGC_PARAMETER("default_emp",sNewDefaultMemberCode));
	return UpdateInfo(pParameter);
}

//#define USES_DISABLE_P2P

//const unsigned int theLocalP2PPort1		= 9100;
//const unsigned int theLocalP2PPort2		= 9300;
//const unsigned int theRemoveP2PPort1	= 9500;
void CUserManagerApp::LocalChatRoomP2PRequest(const CEBCallInfo::pointer& pCallInfo,const CEBAccountInfo::pointer& pAccountInfo,int nRequestData)
{
#ifdef Q_OS_ANDROID
    return;
#endif

#ifdef USES_DISABLE_P2P
	return;
#endif
	if (pAccountInfo->IsOnLineState() && !pAccountInfo->m_bRemoteP2POk)// && pAccountInfo->m_pLocalChatRoom.get()==NULL)
	{
		LogMessage("LocalChatRoomP2PRequest... (uid=%lld,RequestData=%d)\r\n",pAccountInfo->GetUserId(),nRequestData);
		BoostWriteLock wtLock(pAccountInfo->m_mutexLocalChatRoom);
		if (pAccountInfo->m_pLocalChatRoom.get()==NULL)
		{
			pAccountInfo->m_pLocalChatRoom = Cchatroom::create();
			pAccountInfo->m_pLocalChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
			pAccountInfo->m_pLocalChatRoom->Start(pCallInfo->m_pChatRoom->GetAddress().c_str(),"",0,true,0,1);
			pAccountInfo->m_pLocalChatRoom->SetCallId(pCallInfo->m_sChatId);
			pAccountInfo->m_pLocalChatRoom->SetCallBack(this);
			pAccountInfo->m_pLocalChatRoom->SetParam(1);	// 这是P2P
		}
		if (!m_pLocalHostIp.empty())
		{
			if (!pAccountInfo->m_bLocalP2POk)	// **本地未打通
			{
				if (!m_pP2PTaskInfo.exist(pCallInfo->GetCallId()))
				{
					CP2PTaskInfo::pointer pP2PTaskInfo = CP2PTaskInfo::create();
					pP2PTaskInfo->m_pP2PInfo.SetCallId(pCallInfo->GetCallId());
					pP2PTaskInfo->m_pP2PInfo.m_sFromAccount = pAccountInfo->GetUserId();
					pP2PTaskInfo->m_tP2PTry = time(0);
					pP2PTaskInfo->m_bP2PResponse = false;
					m_pP2PTaskInfo.insert(pCallInfo->GetCallId(),pP2PTaskInfo,false);
				}
			}
			char lpszLocalIp[64];
			sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pAccountInfo->m_pLocalChatRoom->GetLocalPort());
			LogMessage("LocalChatRoomP2PRequest->SendP2PRequest... (uid=%lld,RequestData=%d,LocalIp=%s)\r\n",pAccountInfo->GetUserId(),nRequestData,lpszLocalIp);
			pAccountInfo->m_pLocalChatRoom->SetDestAddress(0,pCallInfo->m_pChatRoom->GetAddress());
			pAccountInfo->m_pLocalChatRoom->SendP2PRequest(m_pMyAccountInfo->GetUserId(),pCallInfo->m_sChatId,pAccountInfo->GetUserId(),lpszLocalIp,nRequestData);
			//LogMessage("LocalChatRoomP2PRequest->SendP2PRequest ok\r\n");

			if (pAccountInfo->m_bLocalP2POk && pAccountInfo->m_pLocalChatRoom.get()!=NULL)
			{
				pAccountInfo->m_pLocalChatRoom->SetDestAddress(1,pAccountInfo->GetLocalAddress());
				pAccountInfo->m_pLocalChatRoom->SendP2PTest(3);
			}else
			{
				BoostReadLock rdLock(pAccountInfo->m_mutexRemoteChatRoom);
				if (pAccountInfo->m_bRemoteP2POk && pAccountInfo->m_pRemoteChatRoom.get()!=NULL)
				{
					pAccountInfo->m_pRemoteChatRoom->SetDestAddress(1,pAccountInfo->GetRemoteAddress());
					pAccountInfo->m_pRemoteChatRoom->SendP2PTest(3);
				}
			}
		}
	}
}
bool CUserManagerApp::EnterChat(mycp::bigint sCallId, mycp::bigint sAccount, bool bOffLine)
{
	LogMessage("EnterChat... (callid=%lld,uid=%lld,offline=%d)\r\n",sCallId,sAccount,(int)(bOffLine?1:0));
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		CEBAccountInfo::pointer pAccountInfo;
		if (pCallInfo->m_pCallUserList.find(sAccount, pAccountInfo))
		{
			pCallInfo->m_tLastTime = time(0);
			//if (!bOffLine)
			//	pAccountInfo->SetLineState(EB_LINE_STATE_ONLINE_NEW);	// **离线进入不处理；
			pAccountInfo->SetLineState(bOffLine?EB_LINE_STATE_OFFLINE:EB_LINE_STATE_ONLINE_NEW);

			// 在线用户尝试P2P
			if (!bOffLine && pCallInfo->m_sGroupCode==0 && pAccountInfo->GetUserId() != m_pMyAccountInfo->GetUserId())
			{
				// LOCAL P2P
				LocalChatRoomP2PRequest(pCallInfo,pAccountInfo,0);
			}
			return true;
		}
	}
	return false;
}

bool CUserManagerApp::ExitChat(mycp::bigint sCallId, mycp::bigint sAccount, bool bHangup,EB_USER_LINE_STATE& pOutLineState)
{
	LogMessage("ExitChat... (uid=%lld)\r\n",sAccount);
	if (m_nUmServerState == EB_SS_STOPED)
		return false;
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		CEBAccountInfo::pointer pAccountInfo;
		if (pCallInfo->m_pCallUserList.find(sAccount, pAccountInfo))
		{
			if (pAccountInfo->IsUnknownLineState())	// 前面已经处理；
				return false;
			pOutLineState = pAccountInfo->GetLineState();
			pAccountInfo->m_dwAccountData &= ~EB_DATA_SEND_FILE_CALL;
			pAccountInfo->SetLineState(EB_LINE_STATE_UNKNOWN);
			//pAccountInfo->SetLineState(EB_LINE_STATE_OFFLINE);
			if (pCallInfo->m_sGroupCode==0 && pAccountInfo->GetUserId() != m_pMyAccountInfo->GetUserId())
			{
				// 清空P2P连接，下次重进时，会重新邀请；
				pAccountInfo->m_bLocalP2POk = false;
				{
					BoostWriteLock wtLock(pAccountInfo->m_mutexLocalChatRoom);
					if (pAccountInfo->m_pLocalChatRoom.get()!=NULL)
					{
						m_pClearChatRoom.add(pAccountInfo->m_pLocalChatRoom);
						pAccountInfo->m_pLocalChatRoom.reset();
					}
				}
				pAccountInfo->m_bRemoteP2POk = false;
				{
					BoostWriteLock wtLock(pAccountInfo->m_mutexRemoteChatRoom);
					if (pAccountInfo->m_pRemoteChatRoom.get()!=NULL)
					{
						m_pClearChatRoom.add(pAccountInfo->m_pRemoteChatRoom);
						pAccountInfo->m_pRemoteChatRoom.reset();
					}
				}
			}

			if (pCallInfo->m_nCallState == EB_CALL_STATE_HANGUP ||
				pCallInfo->m_nCallState == EB_CALL_STATE_EXIT)	// **群组，本端关掉窗口也退出；
				//(pCallInfo->m_sGroupCode==0 && pCallInfo->m_nCallState == EB_CALL_STATE_EXIT))
			{
				// 一对一会话，并且本端已经退出会话，清除会话消息；
				this->ClearCallInfo(sCallId,true,false);
			}else if (pCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED && 
				(pCallInfo->m_sGroupCode>0 || pAccountInfo->m_pFromCardInfo.m_nAccountType!=EB_ACCOUNT_TYPE_VISITOR))
			{
				//// 找到第一个在线成员，并且该成员为本端用户，邀请成员进会话（保证群组成员，只邀请一次）
				//BoostReadLock rdlock(pCallInfo->m_pCallUserList.mutex());
				//CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
				//for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
				//{
				//	pAccountInfo = pIter->second;
				//	if (pAccountInfo->IsOnLineState())
				//	{
				//		if (pAccountInfo->GetUserId()==m_pMyAccountInfo->GetUserId())
				//		{
				//			rdlock.unlock();
				//			pCallInfo->m_nCallState = EB_CALL_STATE_AUTO_ACK;
				//			// 需要呼叫一次，否则如果对方在线状态会收不到消息
				//			if (pCallInfo->m_sGroupCode==0)
				//				this->InviteCall(pCallInfo,0,sAccount);	
				//			else
				//				this->InviteCall(pCallInfo,0);	// 群组需要呼叫全部一次；
				//			//EnterRoom(pCallInfo,sAccount);
				//			break;
				//		}
				//		break;	// **这里也需要退出
				//	}
				//}
			}
			return true;
		}
	}
	return false;
}

void CallbackGroupAndMemberInfo(const CEBEnterpriseInfo::pointer& pEnterpriseInfo, const CEBGroupInfo::pointer& pDepartmentInfo, CEBSearchCallback * pCallback, unsigned long dwParam)
{
	pCallback->onGroupInfo(pDepartmentInfo.get(),pEnterpriseInfo.get(),dwParam);
	{
		BoostReadLock lockMember(pDepartmentInfo->m_pMemberList.mutex());
		CLockMap<eb::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		{
			const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
			pCallback->onMemberInfo(pDepartmentInfo.get(), pEmployeeInfo.get(), dwParam);
		}
	}
}

void CUserManagerApp::LoadDepartmentInfo(mycp::bigint sEntCode, CEBSearchCallback * pCallback, unsigned long dwParam)
{
	CLockMap<mycp::bigint,bool> pCallBackList;	// group_id->
	CLockMultiMap<mycp::bigint, CEBGroupInfo::pointer>	pTempDepartmentList;	// parent-depcode->

	CEBEnterpriseInfo::pointer pEnterpriseInfo;
	//BOOST_ASSERT(pCallback != NULL);
	BoostReadLock lockDepartment(theDepartmentList.mutex());
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
	for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++)
	{
        const CEBGroupInfo::pointer &groupInfo = pIterDepartment->second;
        if (sEntCode==0 || groupInfo->m_sEnterpriseCode == sEntCode)
		{
			if (pEnterpriseInfo.get()==NULL)
			{
                theEnterpriseList.find(groupInfo->m_sEnterpriseCode,pEnterpriseInfo);
			}
            if (groupInfo->m_sParentCode>0 && !pCallBackList.exist(groupInfo->m_sParentCode))
			{
                pTempDepartmentList.insert(groupInfo->m_sParentCode, groupInfo);
				continue;
			}
            const mycp::bigint sGroupCode = groupInfo->m_sGroupCode;
			pCallBackList.insert(sGroupCode,true,false);

            CallbackGroupAndMemberInfo(pEnterpriseInfo, groupInfo, pCallback, dwParam);
            /// 处理未处理下一级
			std::vector<CEBGroupInfo::pointer> pSubDepartmentInfos; 
			pTempDepartmentList.find(sGroupCode, pSubDepartmentInfos);
			for (size_t i=0; i<pSubDepartmentInfos.size(); i++)
			{
				CallbackGroupAndMemberInfo(pEnterpriseInfo, pSubDepartmentInfos[i], pCallback, dwParam);
			}
			theTempDepartmentList.remove(sGroupCode);
		}
	}
}
bool CUserManagerApp::IsMyContact(mycp::bigint nContactUserId)
{
	if (nContactUserId==0) return false;
	return theContactList2.exist(nContactUserId);
	//BoostReadLock lockContact(theContactList.mutex());
	//CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList.begin();
	//for (; pIterContact!=theContactList.end(); pIterContact++)
	//{
	//	const CEBContactInfo::pointer pContactInfo = pIterContact->second;
	//	if (nContactUserId==pContactInfo->m_nContactUserId)
	//	{
	//		return true;
	//	}
	//}
	//return false;
}
bool CUserManagerApp::GetContactInfo(mycp::bigint nContactUserId,EB_ContactInfo* pOutContactInfo) const
{
	if (nContactUserId==0)
	{
		return false;
	}
	CEBContactInfo::pointer pContactInfo;
	if (theContactList2.find(nContactUserId,pContactInfo))
	{
		pOutContactInfo->operator =(pContactInfo.get());
		return true;
	}
	//AUTO_CONST_RLOCK(theContactList);
	//CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList.begin();
	//for (; pIterContact!=theContactList.end(); pIterContact++)
	//{
	//	const CEBContactInfo::pointer pContactInfo = pIterContact->second;
	//	if (nContactUserId==pContactInfo->m_nContactUserId)
	//	{
	//		pOutContactInfo->operator =(pContactInfo.get());
	//		return true;
	//	}
	//}
    return false;
}

bool CUserManagerApp::GetContactName(bigint nContactUserId, cgcSmartString &pOutContactName) const
{
    if (nContactUserId==0) {
        return false;
    }
    CEBContactInfo::pointer pContactInfo;
    if (theContactList2.find(nContactUserId,pContactInfo)) {
        pOutContactName = pContactInfo->m_sName;
        return true;
    }
    return false;
}
CEBContactInfo::pointer CUserManagerApp::GetContactInfo(mycp::bigint nContactUserId) const
{
	if (nContactUserId==0) return EB_Null_ContactInfo;
	CEBContactInfo::pointer pContactInfo;
	theContactList2.find(nContactUserId,pContactInfo);
	return pContactInfo;
	//AUTO_CONST_RLOCK(theContactList);
	//CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList.begin();
	//for (; pIterContact!=theContactList.end(); pIterContact++)
	//{
	//	CEBContactInfo::pointer pContactInfo = pIterContact->second;
	//	if (nContactUserId==pContactInfo->m_nContactUserId)
	//	{
	//		return pContactInfo;
	//	}
	//}
	//return EB_Null_ContactInfo;
}

void CUserManagerApp::LoadContactInfo(CEBSearchCallback * pCallback, unsigned long dwParam)
{
	//BOOST_ASSERT(pCallback != NULL);
	BoostReadLock lockContact(theContactList1.mutex());
	CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList1.begin();
	for (; pIterContact!=theContactList1.end(); pIterContact++)
	{
		const CEBContactInfo::pointer& pContactInfo = pIterContact->second;
		//lockContact.unlock();
		pCallback->onContactInfo(pContactInfo.get(), dwParam);
		//lockContact.lock();
	}
}
void CUserManagerApp::SearchUserInfo(const tstring& sSearchKey, CEBSearchCallback * pCallback, unsigned long dwParam)
{
	//BOOST_ASSERT(pCallback != NULL);
	int nCallbackCount = 0;
	const mycp::bigint nSearchId = cgc_atoi64(sSearchKey.c_str());
	bool bNeedOnlineSearch = false;
	CLockMap<mycp::bigint,bool> pExistUserIdList;
	CLockMap<tstring,bool> pExistAccountList;
	{
		BoostReadLock lockDepartment(theDepartmentList.mutex());
		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
		for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++)
		{
			const CEBGroupInfo::pointer& pDepartmentInfo = pIterDepartment->second;
			if (!bNeedOnlineSearch && pDepartmentInfo->m_nEmpCount>0 && (pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER)==0)
			{
				bNeedOnlineSearch = true;
			}
			//if (pDepartmentInfo->m_sGroupCode==1000573)
			//	int i=0;

			//lockDepartment.unlock();
			BoostReadLock lockMember(pDepartmentInfo->m_pMemberList.mutex());
			CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
			for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
			{
				const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
				if (pExistUserIdList.exist(pEmployeeInfo->m_nMemberUserId,false) || pExistAccountList.exist(pEmployeeInfo->m_sMemberAccount,false))
					continue;
				if (sSearchKey.empty() ||
					(nSearchId>0 && pEmployeeInfo->m_nMemberUserId == nSearchId) ||
					pEmployeeInfo->m_sMemberAccount.find(sSearchKey) != std::string::npos ||
					pEmployeeInfo->m_sUserName.find(sSearchKey) != std::string::npos ||
					pEmployeeInfo->m_sJobTitle.find(sSearchKey) != std::string::npos ||
					pEmployeeInfo->m_sCellPhone.find(sSearchKey) != std::string::npos ||
					//pEmployeeInfo->m_sEmail.find(sSearchKey) != std::string::npos ||
					//pEmployeeInfo->m_sAddress.find(sSearchKey) != std::string::npos ||
					pEmployeeInfo->m_sDescription.find(sSearchKey) != std::string::npos
					)
				{
					nCallbackCount++;
					//lockMember.unlock();
					pExistUserIdList.insert(pEmployeeInfo->m_nMemberUserId,true,false,NULL,false);
					pExistAccountList.insert(pEmployeeInfo->m_sMemberAccount,true,false,NULL,false);
					pCallback->onMemberInfo(pDepartmentInfo.get(), pEmployeeInfo.get(), dwParam);
					if (nCallbackCount>=20)
						break;
					//lockMember.lock();
				}
			}
			//lockDepartment.lock();
		}
	}

	// contact info
	{
		BoostReadLock lockContact(theContactList1.mutex());
		CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList1.begin();
		for (; pIterContact!=theContactList1.end(); pIterContact++)
		{
			const CEBContactInfo::pointer& pContactInfo = pIterContact->second;
			if ((pContactInfo->m_nContactUserId>0 && pExistUserIdList.exist(pContactInfo->m_nContactUserId,false)) ||
				pExistAccountList.exist(pContactInfo->m_sContact,false) || pExistAccountList.exist(pContactInfo->m_sEmail,false))
			{
				continue;
			}

			if (sSearchKey.empty() ||
				pContactInfo->m_sContact.find(sSearchKey) != std::string::npos ||
				pContactInfo->m_sName.find(sSearchKey) != std::string::npos ||
				//pContactInfo->m_sPhone.find(sSearchKey) != std::string::npos ||
				pContactInfo->m_sEmail.find(sSearchKey) != std::string::npos ||
				pContactInfo->m_sAddress.find(sSearchKey) != std::string::npos ||
				pContactInfo->m_sDescription.find(sSearchKey) != std::string::npos
				)
			{
				nCallbackCount++;
				//lockContact.unlock();
				if (pContactInfo->m_nContactUserId>0)
					pExistUserIdList.insert(pContactInfo->m_nContactUserId,true,false,NULL,false);
				if (!pContactInfo->m_sContact.empty())
					pExistAccountList.insert(pContactInfo->m_sContact,true,false,NULL,false);
				if (!pContactInfo->m_sEmail.empty())
					pExistAccountList.insert(pContactInfo->m_sEmail,true,false,NULL,false);
				pCallback->onContactInfo(pContactInfo.get(), dwParam);
				//lockContact.lock();
				if (nCallbackCount>=30)
					break;
			}
		}
	}

	if (bNeedOnlineSearch && nCallbackCount<20 && !sSearchKey.empty() && (m_sSearchNullKey.empty() || sSearchKey.find(m_sSearchNullKey)==std::string::npos))
	{
		CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("search-callback", (const void*)pCallback));
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("search-key", sSearchKey));
		pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("search-param", (mycp::bigint)dwParam));
		EnterpriseLoad(0,0,0,1,0,0,false,sSearchKey,false,pSotpRequestInfo);
	}

}

void CUserManagerApp::SearchGroupInfo(const cgcSmartString &sSearchKey, CEBSearchCallback *pCallback, unsigned long dwParam)
{
    const mycp::bigint nSearchId = cgc_atoi64(sSearchKey.c_str());
    int nCallbackCount = 0;

    //BOOST_ASSERT(pCallback != NULL);
    BoostReadLock lockDepartment(theDepartmentList.mutex());
    CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
    for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++) {
        const CEBGroupInfo::pointer &groupInfo = pIterDepartment->second;

        if (sSearchKey.empty() ||
            (nSearchId>0 && groupInfo->m_sGroupCode == nSearchId) ||
            groupInfo->m_sGroupName.find(sSearchKey) != std::string::npos ||
            groupInfo->m_sDescription.find(sSearchKey) != std::string::npos
            )
        {
            nCallbackCount++;
            CEBEnterpriseInfo::pointer pEnterpriseInfo;
            if (groupInfo->m_sEnterpriseCode>0 &&
                    m_pDefaultEnterpriseInfo.get()!=0 &&
                    m_pDefaultEnterpriseInfo->m_sEnterpriseCode==groupInfo->m_sEnterpriseCode) {
                pCallback->onGroupInfo(groupInfo.get(), m_pDefaultEnterpriseInfo.get(), dwParam);
            }
            else {
                pCallback->onGroupInfo(groupInfo.get(), 0, dwParam);
            }
            if (nCallbackCount>=20)
                break;
        }
    }
}

CEBMemberInfo::pointer CUserManagerApp::GetEmployeeInfo(mycp::bigint sAccount) const
{
	AUTO_CONST_RLOCK(theDepartmentList);
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIter = theDepartmentList.begin();
	for (; pIter!=theDepartmentList.end(); pIter++)
	{
		const CEBGroupInfo::pointer& pDepartmentInfo = pIter->second;
		CEBMemberInfo::pointer pEmployeeInfo;
		if (pDepartmentInfo->m_pMemberList.find(sAccount, pEmployeeInfo))
		{
			return pEmployeeInfo;
		}
	}
	return EB_Null_MemberInfo;
}
CEBMemberInfo::pointer CUserManagerApp::GetEmployeeInfo(const tstring& sAccount) const
{
	AUTO_CONST_RLOCK(theDepartmentList);
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIter = theDepartmentList.begin();
	for (; pIter!=theDepartmentList.end(); pIter++)
	{
		const CEBGroupInfo::pointer& pDepartmentInfo = pIter->second;
		BoostReadLock lock2(pDepartmentInfo->m_pMemberList.mutex());
		CLockMap<eb::bigint, CEBMemberInfo::pointer>::const_iterator pIter2 = pDepartmentInfo->m_pMemberList.begin();
		for (; pIter2!=pDepartmentInfo->m_pMemberList.end(); pIter2++)
		{
			CEBMemberInfo::pointer pEmployeeInfo = pIter2->second;
			if (pEmployeeInfo->m_sMemberAccount == sAccount)
			{
				return pEmployeeInfo;
			}
		}
	}
	return EB_Null_MemberInfo;
}

CEBMemberInfo::pointer CUserManagerApp::GetEmployeeInfo2(mycp::bigint sResourceId) const
{
	AUTO_CONST_RLOCK(theEmployeeList);
	CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = theEmployeeList.begin();
	for (; pIter!=theEmployeeList.end(); pIter++)
	{
		const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
		if (pEmployeeInfo->m_sHeadResourceId==sResourceId)
			return pEmployeeInfo;
	}
	return EB_Null_MemberInfo;
}
CEBMemberInfo::pointer CUserManagerApp::GetMyEntEmployeeInfo(void) const
{
	AUTO_CONST_RLOCK(theMyEmployeeList);
	CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = theMyEmployeeList.begin();
	for (; pIter!=theMyEmployeeList.end(); pIter++)
	{
		const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!theDepartmentList.find(pEmployeeInfo->m_sGroupCode,pDepartmentInfo))
		{
			continue;
		}
		if (pDepartmentInfo->m_nGroupType == EB_GROUP_TYPE_DEPARTMENT)
		{
			return pEmployeeInfo;
		}
	}
	return EB_Null_MemberInfo;
}

bool CUserManagerApp::GetGroupEmployeeInfo(mycp::bigint sGroupCode,std::vector<EB_MemberInfo>& pOutEmployeeInfo) const
{
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(sGroupCode,pDepartmentInfo))
	{
		BoostReadLock lock(pDepartmentInfo->m_pMemberList.mutex());
		CLockMap<eb::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		{
			const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
			pOutEmployeeInfo.push_back(EB_MemberInfo(pEmployeeInfo.get()));
		}
		return true;
	}
	return false;
}
bool CUserManagerApp::GetGroupEmployeeInfo(mycp::bigint sGroupCode,std::vector<mycp::bigint>& pOutEmployeeInfo) const
{
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(sGroupCode,pDepartmentInfo))
	{
		BoostReadLock lock(pDepartmentInfo->m_pMemberList.mutex());
		CLockMap<eb::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		{
			const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
			pOutEmployeeInfo.push_back(pEmployeeInfo->m_nMemberUserId);
		}
		return true;
	}
	return false;
}
bool CUserManagerApp::GetGroupMemberCode(mycp::bigint sGroupCode,std::vector<mycp::bigint>& pOutEmployeeInfo) const
{
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(sGroupCode,pDepartmentInfo))
	{
		BoostReadLock lock(pDepartmentInfo->m_pMemberList.mutex());
		CLockMap<eb::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		{
			const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
			pOutEmployeeInfo.push_back(pEmployeeInfo->m_sMemberCode);
		}
		return true;
	}
	return false;
}

bool CUserManagerApp::GetGroupEmployeeInfo(mycp::bigint sGroupCode,std::vector<tstring>& pOutEmployeeInfo) const
{
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(sGroupCode,pDepartmentInfo))
	{
		BoostReadLock lock(pDepartmentInfo->m_pMemberList.mutex());
		CLockMap<eb::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		{
			const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
			pOutEmployeeInfo.push_back(pEmployeeInfo->m_sMemberAccount);
		}
		return true;
	}
	return false;
}
void CUserManagerApp::GetMyEmployeeInfo(std::vector<EB_MemberInfo>& pOutEmployeeInfo) const
{
	pOutEmployeeInfo.clear();
	AUTO_CONST_RLOCK(theMyEmployeeList);
	CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = theMyEmployeeList.begin();
	for (; pIter!=theMyEmployeeList.end(); pIter++)
	{
		const CEBMemberInfo::pointer& pMyEmployeeInfo = pIter->second;
		pOutEmployeeInfo.push_back(EB_MemberInfo(pMyEmployeeInfo.get()));
	}
}
void CUserManagerApp::GetMyDepartment(std::vector<CEBGroupInfo::pointer>& pOutDepartment) const
{
	pOutDepartment.clear();
	AUTO_CONST_RLOCK(theMyEmployeeList);
	CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = theMyEmployeeList.begin();
	for (; pIter!=theMyEmployeeList.end(); pIter++)
	{
		const mycp::bigint sDepCode = pIter->first;
		const CEBMemberInfo::pointer& pMyEmployeeInfo = pIter->second;
		// 
	}
}
void CUserManagerApp::GetEntManagerLevel(mycp::bigint sEnterpriseCode,std::vector<int>& pOutManagerLevelList) const
{
	AUTO_CONST_RLOCK(theDepartmentList);
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIter = theDepartmentList.begin();
	for (; pIter!=theDepartmentList.end(); pIter++)
	{
		const CEBGroupInfo::pointer& pDepartmentInfo = pIter->second;
		if (pDepartmentInfo->m_sEnterpriseCode == sEnterpriseCode)
		{
			CEBMemberInfo::pointer pMyEmployeeInfo;
			if (pDepartmentInfo->m_pMemberList.find(m_pMyAccountInfo->GetUserId(),pMyEmployeeInfo))
			{
				pOutManagerLevelList.push_back(pMyEmployeeInfo->m_nManagerLevel);
			}
		}
	}
}
bool CUserManagerApp::IsEnterpriseCreator(mycp::bigint sEnterpriseCode) const
{
	if (sEnterpriseCode==0 && m_pDefaultEnterpriseInfo.get()!=NULL)
		return (bool)(m_pDefaultEnterpriseInfo->m_nCreateUserId == m_pMyAccountInfo->GetUserId());	// 默认企业
	CEBEnterpriseInfo::pointer pEnterpriseInfo;
	if (!theEnterpriseList.find(sEnterpriseCode,pEnterpriseInfo))
	{
		return false;
	}
	return (bool)(m_pMyAccountInfo->GetUserId()==pEnterpriseInfo->m_nCreateUserId)?true:false;
}
bool CUserManagerApp::IsEnterpriseCreator(mycp::bigint sEnterpriseCode, mycp::bigint nMemberUserId) const
{
	if (sEnterpriseCode==0 && m_pDefaultEnterpriseInfo.get()!=NULL)
		return (bool)(m_pDefaultEnterpriseInfo->m_nCreateUserId == nMemberUserId);	// 默认企业
	CEBEnterpriseInfo::pointer pEnterpriseInfo;
	if (!theEnterpriseList.find(sEnterpriseCode,pEnterpriseInfo))
	{
		return false;
	}
	return (bool)(nMemberUserId==pEnterpriseInfo->m_nCreateUserId)?true:false;
}
bool compare_func_display_index(const EB_SubscribeFuncInfo& t1,const EB_SubscribeFuncInfo& t2){  
	return t1.m_nDisplayIndex > t2.m_nDisplayIndex;  
}
bool CUserManagerApp::GetFuncInfo(eb::bigint nSubId,EB_SubscribeFuncInfo* pOutFuncInfo) const
{
	CEBSubscribeFuncInfo::pointer pSubscrieFuncInfo;
	if (theSubscribeFuncInfolist.find(nSubId,pSubscrieFuncInfo))
	{
		pOutFuncInfo->operator =(pSubscrieFuncInfo.get());
		return true;
	}
    return false;
}

bool CUserManagerApp::IsExistFuncInfo(eb::bigint nSubId) const
{
    return theSubscribeFuncInfolist.exist(nSubId);
}
int CUserManagerApp::GetFuncInfo(EB_FUNC_LOCATION nFuncLocation,std::vector<EB_SubscribeFuncInfo>& pOutFuncInfo) const
{
	AUTO_CONST_RLOCK(theSubscribeFuncInfolist);
	CLockMap<mycp::bigint,CEBSubscribeFuncInfo::pointer>::const_iterator pIter = theSubscribeFuncInfolist.begin();
	for (; pIter!=theSubscribeFuncInfolist.end(); pIter++)
	{
		const CEBSubscribeFuncInfo::pointer& pSubscrieFuncInfo = pIter->second;
		if ((pSubscrieFuncInfo->m_nLocation&nFuncLocation)!=0)
		//if ((pSubscrieFuncInfo->m_nLocation&nFuncLocation)==nFuncLocation)
		{
			EB_SubscribeFuncInfo pFuncInfo(pSubscrieFuncInfo.get());
#ifdef _QT_MAKE_
            if (pFuncInfo.m_sResFile.isEmpty() || !QFileInfo::exists(pFuncInfo.m_sResFile))
#else
            if (pFuncInfo.m_sResFile.empty() || !::PathFileExistsA(pFuncInfo.m_sResFile.c_str()))
#endif
			{

#ifdef _QT_MAKE_
                QString lpszSubIdFile = QString("%1/subid_%2.png").arg(m_sImgPath).arg(pFuncInfo.m_nSubscribeId);
                if (QFileInfo::exists(lpszSubIdFile)) {
                    pFuncInfo.m_sResFile = lpszSubIdFile;
                }
                else if (pFuncInfo.m_nFromSubscribeId>0)
                {
                    lpszSubIdFile = QString("%1/subid_%2.png").arg(m_sImgPath).arg(pFuncInfo.m_nFromSubscribeId);
                    if (QFileInfo::exists(lpszSubIdFile)) {
                        pFuncInfo.m_sResFile = lpszSubIdFile;
                    }
                }
#else
                char lpszSubIdFile[260];
                sprintf(lpszSubIdFile,"%s/subid_%lld.png",m_sImgPath.c_str(),pFuncInfo.m_nSubscribeId);
                if (::PathFileExistsA(lpszSubIdFile))
				{
					pFuncInfo.m_sResFile = lpszSubIdFile;
				}else if (pFuncInfo.m_nFromSubscribeId>0)
				{
					sprintf(lpszSubIdFile,"%s/subid_%lld.png",m_sImgPath.c_str(),pFuncInfo.m_nFromSubscribeId);
                    if (::PathFileExistsA(lpszSubIdFile))
						pFuncInfo.m_sResFile = lpszSubIdFile;
				}
#endif
			}
			pOutFuncInfo.push_back(pFuncInfo);
		}
	}
	if (pOutFuncInfo.empty()) return 0;
	std::sort(pOutFuncInfo.begin(),pOutFuncInfo.end(),compare_func_display_index);
	return pOutFuncInfo.size();
}
int CUserManagerApp::GetFuncSize(EB_FUNC_LOCATION nFuncLocation) const
{
	int nResult = 0;
	AUTO_CONST_RLOCK(theSubscribeFuncInfolist);
	CLockMap<mycp::bigint,CEBSubscribeFuncInfo::pointer>::const_iterator pIter = theSubscribeFuncInfolist.begin();
	for (; pIter!=theSubscribeFuncInfolist.end(); pIter++)
	{
		const CEBSubscribeFuncInfo::pointer& pSubscrieFuncInfo = pIter->second;
		if ((pSubscrieFuncInfo->m_nLocation&nFuncLocation)==nFuncLocation)
		{
			nResult++;
		}
	}
	return nResult;
}
tstring CUserManagerApp::GetSubscribeFuncUrl(eb::bigint nSubscribeId,eb::bigint nCallId,eb::bigint nFromUserId,const tstring& sFromAccount,eb::bigint nGroupId) const
{
	if (m_pUserManager.get()==NULL) return "";
	if (m_pMyAccountInfo.get()==NULL) return "";
	if (m_pFuncUrl.empty()) return "";
	CEBSubscribeFuncInfo::pointer pSubscrieFuncInfo;
	if (theSubscribeFuncInfolist.find(nSubscribeId,pSubscrieFuncInfo))
	{
		//char lpszCallParams[128];
		//memset(lpszCallParams,0,sizeof(lpszCallParams));
		//if (nCallId>0 && (pSubscrieFuncInfo->m_nLocation&EB_FUNC_LOCATION_CALL_CHAT)!=0)
		//{
		//	// 获取呼叫信息
		//	CEBCallInfo::pointer pCallInfo;
		//	if (!theCallInfoList.find(nCallId, pCallInfo))
		//	{
		//		return "";
		//	}
		//	boost::mutex::scoped_lock lock(pCallInfo->m_pCallUserList.mutex());
		//	CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
		//	for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
		//	{
		//		CEBAccountInfo::pointer pCallAccountInfo = pIter->second;
		//		if (pCallAccountInfo->GetUserId() != m_pMyAccountInfo->GetUserId())
		//		{
		//			sprintf(lpszCallParams,"&cid=%lld&fuid=%lld&fusid=%lld",nCallId,pCallAccountInfo->GetUserId(),pCallAccountInfo->GetUserSignId());
		//			break;
		//		}
		//	}
		//}
		if (pSubscrieFuncInfo->m_nFunctionMode==EB_FUNC_MODE_PROGRAM)
		{
			if (pSubscrieFuncInfo->m_nFunctionParam==EB_FUNC_PARAM_USER_INFO)
			{
				char lpszBuffer[320];
                sprintf(lpszBuffer,"%s %lld %s",pSubscrieFuncInfo->m_sFunctionUrl.c_str(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetAccount().c_str());
				return lpszBuffer;
			}else
			{
				return pSubscrieFuncInfo->m_sFunctionUrl;
			}
		}

		tstring sUMServer(m_sHttpServer);
		if (sUMServer.empty())
		{
			int nPort = 0;
			entboost::GetAddressPort(m_pUserManager->GetAddress().address().c_str(),sUMServer,nPort);
		}
		char lpszBuffer[1024];
		sprintf(lpszBuffer,"[fk_v1]%lld;%s;%lld",m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetAccessToken().c_str(),pSubscrieFuncInfo->m_nSubscribeId);
		//sprintf(lpszBuffer,"[fk_v1]%lld;%s;%lld",m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),pSubscrieFuncInfo->m_nSubscribeId);
		MD5 md5;
		md5.update((const unsigned char*)lpszBuffer,(unsigned int)strlen(lpszBuffer));
		md5.finalize();
		const tstring sFunctionKey = md5.hex_digest();
		sprintf(lpszBuffer,"http://%s%s?uid=%lld&ok=%s&sub_id=%lld&fk=%s&cid=%lld&fuid=%lld&facc=%s&gid=%lld",
			sUMServer.c_str(),m_pFuncUrl.c_str(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->m_pLogonList.GetOnlineKey("").c_str(),
			pSubscrieFuncInfo->m_nSubscribeId,sFunctionKey.c_str(),nCallId,nFromUserId,sFromAccount.c_str(),nGroupId);
		return lpszBuffer;
	}
	return "";
}

tstring CUserManagerApp::GetConversationsUrl(eb::bigint nFromUserId,eb::bigint nGroupId) const
{
	if (m_pUserManager.get()==NULL) return "";
	if (m_pMyAccountInfo.get()==NULL) return "";
	if (m_sConversationsUrl.empty()) return "";
	tstring sUMServer(m_sHttpServer);
	if (sUMServer.empty())
	{
		int nPort = 0;
		entboost::GetAddressPort(m_pUserManager->GetAddress().address().c_str(),sUMServer,nPort);
	}
	char lpszBuffer[1024];
	if (m_sConversationsUrl.find("?")==std::string::npos)
	{
		sprintf(lpszBuffer,"http://%s%s?uid=%lld&ok=%s&fuid=%lld&gid=%lld",sUMServer.c_str(),m_sConversationsUrl.c_str(),
			m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->m_pLogonList.GetOnlineKey("").c_str(),nFromUserId,nGroupId);
	}else
	{
		sprintf(lpszBuffer,"http://%s%s&uid=%lld&ok=%s&fuid=%lld&gid=%lld",sUMServer.c_str(),m_sConversationsUrl.c_str(),
			m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->m_pLogonList.GetOnlineKey("").c_str(),nFromUserId,nGroupId);
	}
	return lpszBuffer;
}

tstring CUserManagerApp::GetEntManagerUrl(void) const
{
	if (m_pUserManager.get()==NULL) return "";
	if (m_pMyAccountInfo.get()==NULL) return "";
	if (m_sEntManagerUrl.empty()) return "";

	tstring sUMServer(m_sHttpServer);
	if (sUMServer.empty())
	{
		int nPort = 0;
		entboost::GetAddressPort(m_pUserManager->GetAddress().address().c_str(),sUMServer,nPort);
	}
	char lpszBuffer[1024];
	if (m_sEntManagerUrl.find("?",1)==std::string::npos)
		sprintf(lpszBuffer,"http://%s%s?uid=%lld&ok=%s",sUMServer.c_str(),m_sEntManagerUrl.c_str(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->m_pLogonList.GetOnlineKey("").c_str());
	else
		sprintf(lpszBuffer,"http://%s%s&uid=%lld&ok=%s",sUMServer.c_str(),m_sEntManagerUrl.c_str(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->m_pLogonList.GetOnlineKey("").c_str());
	return lpszBuffer;
}

tstring CUserManagerApp::GetHomeIndexUrl(void) const
{
	if (m_pUserManager.get()==NULL) return "";
	if (m_pMyAccountInfo.get()==NULL) return "";

	tstring sUMServer(m_sHttpServer);
	if (sUMServer.empty())
	{
		int nPort = 0;
		entboost::GetAddressPort(m_pUserManager->GetAddress().address().c_str(),sUMServer,nPort);
	}
	char lpszBuffer[1024];
	if (m_sEntManagerUrl.find("?",1)==std::string::npos)
		sprintf(lpszBuffer,"http://%s/ebtw/index.php",sUMServer.c_str());
	else
		sprintf(lpszBuffer,"http://%s/ebtw/index.php",sUMServer.c_str());
	return lpszBuffer;
}

tstring CUserManagerApp::GetLogonHttpReqUrl(void) const
{
	if (m_pUserManager.get()==NULL) return "";
	if (m_pMyAccountInfo.get()==NULL) return "";

	tstring sUMServer(m_sHttpServer);
	if (sUMServer.empty())
	{
		int nPort = 0;
		entboost::GetAddressPort(m_pUserManager->GetAddress().address().c_str(),sUMServer,nPort);
	}
	char lpszBuffer[1024];
	sprintf(lpszBuffer,"http://%s/rest.v03.ebwebum.logonhttpreq?uid=%lld&online_key=%s&logon_type=%d",
		sUMServer.c_str(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->m_pLogonList.GetOnlineKey("").c_str(),m_pMyAccountInfo->GetLogonType());
	return lpszBuffer;

}

int CUserManagerApp::RDRequest(mycp::bigint sCallId,EB_REMOTE_DESKTOP_TYPE nRDType)
{
	if (m_pUserManager.get() == NULL) return -1;

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return -1;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return -1;
	}

	CPOPCUserManager::pointer pPOPCUserManager;
	if (!theUserManagerList.find(sCallId,pPOPCUserManager))
	{
		return 1;
	}
	if (pCallInfo->m_nRDType!=EB_RD_UNKNOWN)
	{
		EB_RemoteDesktopInfo* pVideoInfo = new EB_RemoteDesktopInfo();
#ifdef _QT_MAKE_
		pVideoInfo->SetQEventType((QEvent::Type)EB_WM_RD_REQUEST_RESPONSE);
		pVideoInfo->SetEventParameter((long)EB_STATE_ALREADY_IN_REMOTE_DESKTOP);
#endif
		pVideoInfo->m_sCallId = sCallId;
		pVideoInfo->m_nRDType = pCallInfo->m_nRDType;
		pVideoInfo->m_nFromUserId = m_pMyAccountInfo->GetUserId();
		//if (pCallInfo->m_nRDType==EB_RD_DESKTOP_SRC)
		//	pVideoInfo.m_bOnDesktop = true;
		if (m_callback)
			m_callback->onRDRequestResponse(*pVideoInfo,(EB_STATE_CODE)EB_STATE_ALREADY_IN_REMOTE_DESKTOP);

#ifdef _QT_MAKE_
		QObject* hWnd=NULL;
#else
		HWND hWnd=NULL;
#endif
		if (m_pDesktopHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(hWnd, EB_WM_RD_REQUEST_RESPONSE, (WPARAM)pVideoInfo, EB_STATE_ALREADY_IN_REMOTE_DESKTOP);
			delete pVideoInfo;
#endif
		}
		else if (m_pHwnd != NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(m_pHwnd, EB_WM_RD_REQUEST_RESPONSE, (WPARAM)pVideoInfo, EB_STATE_ALREADY_IN_REMOTE_DESKTOP);
			delete pVideoInfo;
#endif
		}
		else {
			delete pVideoInfo;
		}
		return 1;
	}

	//if (pFromAccountInfo->m_nVideoUserId>0)
	//{
	//	// 当前已经有视频用户ＩＤ
	//	return 1;
	//}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-id", sCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("rd-type", (int)nRDType));
	pPOPCUserManager->SendRDRequest(sCallId,nRDType,pSotpRequestInfo);
	return 0;
}
int CUserManagerApp::RDAck(mycp::bigint sCallId,bool bAccept,mycp::bigint nToUserId)
{
	if (m_pUserManager.get() == NULL) return -1;

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return -1;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return -1;
	}
	// ?
	//if (pFromAccountInfo->m_nVideoUserId>0)
	//{
	//	// 当前已经有视频用户ＩＤ
	//	return 1;
	//}
	CPOPCUserManager::pointer pPOPCUserManager;
	if (!theUserManagerList.find(sCallId,pPOPCUserManager))
	{
		//pPOPCUserManager = m_pUserManager;
		return 1;
	}
	const int nAckType = bAccept?EB_CAT_ACCEPT:EB_CAT_REJECT;
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", nToUserId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-id", sCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ack-type", nAckType));
	pPOPCUserManager->SendRDAck(sCallId,nAckType,nToUserId,pSotpRequestInfo);
	return 0;
}
int CUserManagerApp::RDEnd(mycp::bigint sCallId)
{
	if (m_pUserManager.get() == NULL) return -1;

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return -1;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return -1;
	}
	if (!pCallInfo->m_pRDUserIdList.exist(pFromAccountInfo->GetUserId()))
	{
		// 当前没有视频用户ＩＤ
		return 1;
	}
	//DoRDDisonnecte(pCallInfo);
	// ?
	if (!pCallInfo->m_pRDUserIdList.remove(pFromAccountInfo->GetUserId()))
	{
		// 当前没有上视频
		return 1;
	}
	pCallInfo->m_nRDType = EB_RD_UNKNOWN;
	pCallInfo->m_pRDUserIdList.clear();
	pCallInfo->m_pOnRDUserList.clear();

	CPOPCUserManager::pointer pPOPCUserManager;
	if (!theUserManagerList.find(sCallId,pPOPCUserManager))
	{
		//pPOPCUserManager = m_pUserManager;
		return 1;
	}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-id", sCallId));
	pPOPCUserManager->SendRDEnd(sCallId,pSotpRequestInfo);
	// 通知自己事件
	EB_RemoteDesktopInfo* pVideoInfo = new EB_RemoteDesktopInfo();
#ifdef _QT_MAKE_
	pVideoInfo->SetQEventType((QEvent::Type)EB_WM_RD_CLOSE);
#endif
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nRDType = pCallInfo->m_nRDType;
	pVideoInfo->m_nFromUserId = pFromAccountInfo->GetUserId();
	pVideoInfo->m_bOnDesktop = true;
	if (m_callback)
		m_callback->onRDClose(*pVideoInfo);

	// ?
#ifdef _QT_MAKE_
	QObject* hWnd=NULL;
#else
	HWND hWnd=NULL;
#endif
	if (m_pDesktopHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(hWnd, EB_WM_RD_CLOSE, (WPARAM)pVideoInfo, (LPARAM)0);
		delete pVideoInfo;
#endif
	}
	else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(m_pHwnd, EB_WM_RD_CLOSE, (WPARAM)pVideoInfo, (LPARAM)0);
		delete pVideoInfo;
#endif
	}
	else {
		delete pVideoInfo;
	}
	return 0;
}
void CUserManagerApp::CreateDesktop(const CEBCallInfo::pointer& pCallInfo)
{
#ifdef USES_REMOTE_DESKTOP
	const tstring sRDServer = pCallInfo->m_sRDServer;
#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop == NULL)
	{
		::CoInitialize(NULL);
		HRESULT hr = CoCreateInstance(__uuidof(EBRemoteDesktop),NULL,CLSCTX_INPROC_SERVER,__uuidof(IEBRemoteDesktop),(void**)&pCallInfo->m_pEBRemoteDesktop);
		if (FAILED(hr))
		{
			//char lpszBuffer[50];
			//sprintf(lpszBuffer,"error=0x%x",hr);
			//MessageBox(NULL,lpszBuffer,lpszBuffer,MB_OK);
			return;
		}
		//MessageBox(NULL,"ok","ok",MB_OK);
		if (pCallInfo->m_pEBRemoteDesktop == NULL)
			return;
	}
	pCallInfo->m_pEBRemoteDesktop->RD_Init(sRDServer.c_str(),0);
	const VARIANT_BOOL bIsGroup = pCallInfo->m_sGroupCode==0?VARIANT_FALSE:VARIANT_TRUE;
	pCallInfo->m_pEBRemoteDesktop->RD_Register(pCallInfo->GetCallId(),m_pMyAccountInfo->GetUserId(),pCallInfo->m_nRemoteDesktopParam,bIsGroup);
#else
	if (pCallInfo->m_pEBRemoteDesktop.get() == NULL)
	{
		pCallInfo->m_pEBRemoteDesktop = Cebrd::create();
	}
	pCallInfo->m_pEBRemoteDesktop->Start(sRDServer.c_str(),0);
	const bool bIsGroup = pCallInfo->m_sGroupCode==0?false:true;
	pCallInfo->m_pEBRemoteDesktop->Register(pCallInfo->GetCallId(),m_pMyAccountInfo->GetUserId(),pCallInfo->m_nRemoteDesktopParam,bIsGroup);
#endif
#endif
}

int CUserManagerApp::OpenLocalDesktop(eb::bigint sCallId, int quant, int fps)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return -1;
	}
	if (pCallInfo->m_nRDType == EB_RD_UNKNOWN) return 0;
	CreateDesktop(pCallInfo);
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop==NULL)
		return -1;
	pCallInfo->m_pEBRemoteDesktop->RD_StartLocalDesktop(quant,fps);
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()==NULL)
		return -1;
	pCallInfo->m_pEBRemoteDesktop->StartLocalDesktop(quant,fps);
#endif
#endif
	return 1;
}
void CUserManagerApp::EnableControlLocalDesktop(eb::bigint sCallId, eb::bigint nUserId)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop!=NULL)
		pCallInfo->m_pEBRemoteDesktop->RD_EnableControlLocalDesktop = nUserId;
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
		pCallInfo->m_pEBRemoteDesktop->EnableControlLocalDesktop(nUserId);
#endif
#endif
}
eb::bigint CUserManagerApp::EnableControlLocalDesktop(eb::bigint sCallId) const
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return 0;
	}
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop!=NULL)
		return pCallInfo->m_pEBRemoteDesktop->RD_EnableControlLocalDesktop;
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
		return pCallInfo->m_pEBRemoteDesktop->EnableControlLocalDesktop();
#endif
#endif
	return 0;
}
void CUserManagerApp::CloseLocalDesktop(eb::bigint sCallId)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop!=NULL)
		pCallInfo->m_pEBRemoteDesktop->RD_StopLocalDesktop();
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
		pCallInfo->m_pEBRemoteDesktop->StopLocalDesktop();
#endif
#endif
}
#ifdef _QT_MAKE_
bool CUserManagerApp::OpenRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId, QObject* hWndParent)
#else
bool CUserManagerApp::OpenRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId, HWND hWndParent)
#endif
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return false;
	}
	if (pCallInfo->m_nRDType == EB_RD_UNKNOWN) return false;
	CreateDesktop(pCallInfo);
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop==NULL) return false;
	pCallInfo->m_pEBRemoteDesktop->RD_AddSink(nUserId, (ULONG)hWndParent);
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()==NULL) return false;
	pCallInfo->m_pEBRemoteDesktop->AddSink(nUserId, hWndParent);
#endif
#endif
	return true;
}
bool CUserManagerApp::ControlRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId, int nMouseMoveFPS)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return false;
	}
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop==NULL) return false;
	pCallInfo->m_pEBRemoteDesktop->PutRD_ControlSinkDesktop(nUserId, nMouseMoveFPS);
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()==NULL) return false;
	pCallInfo->m_pEBRemoteDesktop->ControlSinkDesktop(nUserId, nMouseMoveFPS);
#endif
#endif
	return true;
}
int CUserManagerApp::ControlRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId) const
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return 0;
	}
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop!=NULL)
		return pCallInfo->m_pEBRemoteDesktop->GetRD_ControlSinkDesktop(nUserId);
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
		return pCallInfo->m_pEBRemoteDesktop->ControlSinkDesktop(nUserId);
#endif
#endif
	return 0;
}
void CUserManagerApp::MoveRemoteDesktopWindow(eb::bigint sCallId,mycp::bigint nUserId, int x, int y, int width, int height, bool bRepaint) const
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop!=NULL)
		pCallInfo->m_pEBRemoteDesktop->RD_MoveSinkWindow(nUserId, x, y, width, height, bRepaint?VARIANT_TRUE:VARIANT_FALSE);
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
		pCallInfo->m_pEBRemoteDesktop->MoveSinkWindow(nUserId, x, y, width, height, bRepaint);
#endif
#endif
}
void CUserManagerApp::CloseRemoteDesktop(eb::bigint sCallId,mycp::bigint nUserId)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop!=NULL)
		pCallInfo->m_pEBRemoteDesktop->RD_DelSink(nUserId);
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
		pCallInfo->m_pEBRemoteDesktop->DelSink(nUserId);
#endif
#endif
}
void CUserManagerApp::CloseAllRemoteDesktop(eb::bigint sCallId)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop!=NULL)
		pCallInfo->m_pEBRemoteDesktop->RD_DelAllSink();
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
		pCallInfo->m_pEBRemoteDesktop->DelAllSink();
#endif
#endif
}
void CUserManagerApp::CloseAllDesktop(eb::bigint sCallId)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
	if (pCallInfo->m_pEBRemoteDesktop!=NULL)
	{
		pCallInfo->m_pEBRemoteDesktop->RD_DelAllSink();
		pCallInfo->m_pEBRemoteDesktop->RD_StopLocalDesktop();
		pCallInfo->m_pEBRemoteDesktop->RD_UnInit();
		pCallInfo->m_pEBRemoteDesktop.Release();
	}
#else
	if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
	{
		pCallInfo->m_pEBRemoteDesktop->DelAllSink();
		pCallInfo->m_pEBRemoteDesktop->StopLocalDesktop();
		pCallInfo->m_pEBRemoteDesktop->Stop();
		pCallInfo->m_pEBRemoteDesktop.reset();
	}
#endif
#endif
}

bool CUserManagerApp::GetCallRDUserInfo(mycp::bigint sCallId, std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return false;
	}
	GetCallRDUserInfo(pCallInfo,pOutVideoUserInfo);
	return true;
}
void CUserManagerApp::GetCallRDUserInfo(const CEBCallInfo::pointer& pCallInfo, std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const
{
	BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
	CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
	for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
	{
		const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
		if (pCallInfo->m_pRDUserIdList.exist(pAccountInfo->GetUserId()))
		{
			const bool bOnVideo = pCallInfo->m_pOnRDUserList.exist(pAccountInfo->GetUserId());
			pOutVideoUserInfo.push_back(EB_UserVideoInfo(pAccountInfo->GetUserId(),bOnVideo));
		}
	}
}

int CUserManagerApp::VideoRequest(mycp::bigint sCallId,EB_VIDEO_TYPE nVideoType)
{
	if (m_pUserManager.get() == NULL) return -1;

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return -1;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return -1;
	}

	CPOPCUserManager::pointer pPOPCUserManager;
	if (!theUserManagerList.find(sCallId,pPOPCUserManager))
	{
		return 1;
	}

	//if (pFromAccountInfo->m_nVideoUserId>0)
	//{
	//	// 当前已经有视频用户ＩＤ
	//	return 1;
	//}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-id", sCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("v-type", (int)nVideoType));
	pPOPCUserManager->SendVRequest(sCallId,nVideoType,pSotpRequestInfo);
	return 0;
}
int CUserManagerApp::VideoAck(mycp::bigint sCallId,bool bAccept,mycp::bigint nToUserId)
{
	if (m_pUserManager.get() == NULL) return -1;

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return -1;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return -1;
	}
	//if (pCallInfo->m_sGroupCode>0 && !bAccept && nToUserId==0)
	//{
	//	// 退出群组视频，要清空所有视频数据，否则会有异常
	//	DoVideoDisonnecte(pCallInfo,false);
	//}

	// ?
	//if (pFromAccountInfo->m_nVideoUserId>0)
	//{
	//	// 当前已经有视频用户ＩＤ
	//	return 1;
	//}
	CPOPCUserManager::pointer pPOPCUserManager;
	if (!theUserManagerList.find(sCallId,pPOPCUserManager))
	{
		//LogMessage("VideoAck... error (sCallId=%lld)\r\n", sCallId);
		//pPOPCUserManager = m_pUserManager;
		return 1;
	}

	const int nAckType = bAccept?EB_CAT_ACCEPT:EB_CAT_REJECT;
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", nToUserId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-id", sCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ack-type", nAckType));
	pPOPCUserManager->SendVAck(sCallId,nAckType,nToUserId,pSotpRequestInfo);
	//m_pUserManager->SendVAck(sCallId,nAckType,nToUserId,pSotpRequestInfo);
	return 0;
}
int CUserManagerApp::VideoEnd(mycp::bigint sCallId)
{
	if (m_pUserManager.get() == NULL) return -1;

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return -1;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return -1;
	}
	if (!pCallInfo->m_pVideoUserIdList.exist(pFromAccountInfo->GetUserId()))
	{
		// 当前没有视频用户ＩＤ
		return 1;
	}
    if (pCallInfo->m_sGroupCode==0) {
        /// 一对一，关闭所有
		DoVideoDisonnecte(pCallInfo,true);
    }
    else {
#ifdef USES_VIDEOROOM

		BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
		if (pCallInfo->m_pVideoRoom.get()!=NULL)
		{
			// 关闭本端视频；
			pCallInfo->m_pVideoUserParam.remove(pFromAccountInfo->GetUserId());
			pCallInfo->m_pVideoRoom->StopVideoCapture();
			pCallInfo->m_pVideoRoom->StopAudioCapture();
		}
#endif
	}
	// ?
	if (!pCallInfo->m_pOnVideoUserList.remove(pFromAccountInfo->GetUserId()))
	{
		// 当前没有上视频
		return 1;
	}
	//pCallInfo->m_pVideoUserIdList.insert(pFromAccountInfo->GetUserId(),true);
	//HWND hWnd=NULL;
	//if (m_pVideoHwnd.find(sCallId,hWnd))
	//{
	//	::PostMessage(hWnd, POP_WM_VIDEO_END, (WPARAM)pCallInfo.get(), (LPARAM)pFromAccountInfo.get());
	//}else if (m_pHwnd!=NULL)
	//{
	//	::PostMessage(m_pHwnd, POP_WM_VIDEO_END, (WPARAM)pCallInfo.get(), (LPARAM)pFromAccountInfo.get());
	//}
	CPOPCUserManager::pointer pPOPCUserManager;
	if (!theUserManagerList.find(sCallId,pPOPCUserManager))
	{
		//pPOPCUserManager = m_pUserManager;
		return 1;
	}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-id", sCallId));
	pPOPCUserManager->SendVEnd(sCallId,pSotpRequestInfo);
	//m_pUserManager->SendVEnd(sCallId,pSotpRequestInfo);
	// 通知自己事件
	EB_UserVideoInfo pUserVideoInfo(pFromAccountInfo->GetUserId(),true);
	EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
#ifdef _QT_MAKE_
	pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_CLOSE);
	//pVideoInfo->SetEventParameter((long)EB_STATE_ALREADY_IN_REMOTE_DESKTOP);
	pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nVideoType = pCallInfo->m_nVideoType;
	if (m_callback)
		m_callback->onVideoClose(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
	QObject* hWnd=NULL;
#else
	HWND hWnd=NULL;
#endif
	if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
		::SendMessage(hWnd, EB_WM_VIDEO_CLOSE, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
		delete pVideoInfo;
#endif
	}
	else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
		::SendMessage(m_pHwnd, EB_WM_VIDEO_CLOSE, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
		delete pVideoInfo;
#endif
	}
	else {
		delete pVideoInfo;
	}
	return 0;
}
bool CUserManagerApp::GetCallVideoUserInfo(mycp::bigint sCallId, std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return false;
	}
	GetCallVideoUserInfo(pCallInfo,pOutVideoUserInfo);
    return true;
}

bool CUserManagerApp::IsExistCallVideoUserInfo(bigint sCallId) const
{
    CEBCallInfo::pointer pCallInfo;
    if (!theCallInfoList.find(sCallId,pCallInfo)) {
        return false;
    }
    return pCallInfo->m_pVideoUserIdList.empty()?false:true;
}
void CUserManagerApp::GetCallVideoUserInfo(const CEBCallInfo::pointer& pCallInfo, std::vector<EB_UserVideoInfo>& pOutVideoUserInfo) const
{
	BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
	CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
	for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
	{
		const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
		if (pCallInfo->m_pVideoUserIdList.exist(pAccountInfo->GetUserId()))
		{
			const bool bOnVideo = pCallInfo->m_pOnVideoUserList.exist(pAccountInfo->GetUserId());
			pOutVideoUserInfo.push_back(EB_UserVideoInfo(pAccountInfo->GetUserId(),bOnVideo));
		}
	}
}

#define MAX_LOG_SIZE 1024
void CUserManagerApp::LogMessage(const char * format,...)
{
	if (!m_bLogDebug) return;
	//if (is_visitor_uid(m_pMyAccountInfo->GetUserId())) return;

	try
	{
		char debugmsg[MAX_LOG_SIZE];
		memset(debugmsg, 0, MAX_LOG_SIZE);
		va_list   vl;
		va_start(vl, format);
		int len = vsnprintf(debugmsg, MAX_LOG_SIZE-1, format, vl);
		va_end(vl);
		if (len > MAX_LOG_SIZE)
			len = MAX_LOG_SIZE;
		debugmsg[len] = '\0';
		CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_LOG_MESSAGE);
		pProcessMsgInfo->m_tProcessTime = time(0)-1;
		pProcessMsgInfo->m_sString1 = debugmsg;
		m_pProcessMsgList.add(pProcessMsgInfo);
	}catch(std::exception const &)
	{
	}catch(...)
	{
	}
}
void CUserManagerApp::RichBufferAddText(eb::bigint sCallId,const char* sText)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return;
	}
	if(pCallInfo->m_pChatRoom.get()==NULL) return;
	return pCallInfo->m_pChatRoom->RichBufferAddText(sText);
}
void CUserManagerApp::RichBufferAddObject(eb::bigint sCallId,const char* pData,unsigned long nSize)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return;
	}
	if(pCallInfo->m_pChatRoom.get()==NULL) return;
	return pCallInfo->m_pChatRoom->RichBufferAddObject(pData,nSize);
}
void CUserManagerApp::RichBufferAddResource(eb::bigint sCallId,const char* sResource)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return;
	}
	if(pCallInfo->m_pChatRoom.get()==NULL) return;
	return pCallInfo->m_pChatRoom->RichBufferAddResource(sResource);
}
void CUserManagerApp::RichBufferClear(eb::bigint sCallId)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return;
	}
	if(pCallInfo->m_pChatRoom.get()==NULL) return;
	return pCallInfo->m_pChatRoom->RichBufferClear();
}
int CUserManagerApp::SendCrRichBuffer(eb::bigint sCallId,mycp::bigint sTo,bool bPrivate)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return 1;
	}
	if(pCallInfo->m_pChatRoom.get()==NULL)
	{
		return 2;
	}
	if (sTo==0 || !bPrivate)
	{
		if (InviteCall(pCallInfo,0) != 0) return 3;
	}else
	{
		// 群组，私聊
		if (InviteCall(pCallInfo,0, sTo) != 0) return 3;
	}
	return pCallInfo->m_pChatRoom->SendRichBuffser(sTo,bPrivate);
}

int CUserManagerApp::SendCrRich(eb::bigint sCallId,const EB_ChatRoomRichMsg* pRichMsg,mycp::bigint sTo,bool bPrivate, bool bFromToSendList)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return 1;
	}
	//LogMessage("SendCrRich... (callid=%lld,touid=%lld)\r\n",sCallId,sTo);
	if (sTo==0 || !bPrivate)
	{
		if (InviteCall(pCallInfo,0) != 0 && !bFromToSendList)
		{
			CToSendList::pointer pCallToSendList;
			if (!theCallToSendList.find(sCallId,pCallToSendList))
			{
				pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
				theCallToSendList.insert(sCallId,pCallToSendList);
			}
			//LogMessage("SendCrRich->pCallToSendList->m_list.add... (callid=%lld,touid=%lld)\r\n",sCallId,sTo);
			pCallToSendList->m_list.add(CToSendInfo::create(CEBChatRoomRichMsg::create(*pRichMsg),sTo,bPrivate));
			return 0;
			//return 3;
		}
	}else
	{
		// 群组，私聊
		if (InviteCall(pCallInfo,0, sTo) != 0 && !bFromToSendList)
		{
			CToSendList::pointer pCallToSendList;
			if (!theCallToSendList.find(sCallId,pCallToSendList))
			{
				pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
				theCallToSendList.insert(sCallId,pCallToSendList);
			}
			pCallToSendList->m_list.add(CToSendInfo::create(CEBChatRoomRichMsg::create(*pRichMsg),sTo,bPrivate));
			return 0;
			//return 3;
		}
	}
	if(pCallInfo->m_pChatRoom.get()==NULL)
	{
        this->InviteCall(pCallInfo, 0);
		return 2;
	}
	//LogMessage("SendCrRich->m_pChatRoom->SendRich... (callid=%lld,touid=%lld)\r\n",sCallId,sTo);
	return pCallInfo->m_pChatRoom->SendRich(pRichMsg,sTo,bPrivate);
}
int CUserManagerApp::SendUserCard(eb::bigint sCallId,const tstring& sCardUserAccount,mycp::bigint sTo,bool bPrivate)
{
	if (m_pUserManager.get() == NULL)
	{
		return -1;
	}
	if (!theCallInfoList.exist(sCallId))
	{
		return -2;
	}

	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(EB_REQ_TYPE_USER_INFO);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("send-usercard-call-id", sCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("send-usercard-to-uid", sTo));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("send-usercard-private", bPrivate));
	return m_pUserManager->SendUQuery(0,sCardUserAccount,pSotpRequestInfo);
}

int CUserManagerApp::RequestWithdrawMsg(eb::bigint sCallId,mycp::bigint nMsgId)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return 1;
	}
	if(pCallInfo->m_pChatRoom.get()==NULL)
	{
		return 2;
	}
	return pCallInfo->m_pChatRoom->MsgAck(nMsgId,(int)EB_MAT_WITHDRAW);
}
int CUserManagerApp::RequestCollectMsg(eb::bigint sCallId,mycp::bigint nMsgId, bool bGroupCollection)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return 1;
	}
	if(pCallInfo->m_pChatRoom.get()==NULL)
	{
		return 2;
	}
	return pCallInfo->m_pChatRoom->MsgAck(nMsgId,(int)(bGroupCollection?EB_MAT_GROUP_COLLECT:EB_MAT_SELF_COLLECT));
}

bool CUserManagerApp::SendWaitingProcessCallback(eb::bigint sCallId, const EBFileString &sFilePath,mycp::bigint sTo,bool bPrivate,bool bOffFile)
{
    if (m_pWaitingProcessFileList.exist(sFilePath))
    {
        return false;
    }
    const unsigned long nWaitingProcessMsgId = this->GetNextId();
    m_pWaitingProcessFileList.insert(sFilePath,nWaitingProcessMsgId);
    CCrFileInfo * pCrFileInfo = new CCrFileInfo(0,sCallId,sCallId,EB_STATE_WAITING_PROCESS);
#ifdef _QT_MAKE_
    pCrFileInfo->SetQEventType((QEvent::Type)CR_WM_SENDING_FILE);
    //pVideoInfo->SetEventParameter((long)EB_STATE_ALREADY_IN_REMOTE_DESKTOP);
    //pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif
    pCrFileInfo->m_sFileName = sFilePath;
    pCrFileInfo->m_nFileSize = EbGetFileSize(sFilePath);
    pCrFileInfo->m_bOffFile = bOffFile;
    pCrFileInfo->m_sSendFrom = this->m_pMyAccountInfo->GetUserId();
    pCrFileInfo->m_sSendTo = sTo;
    pCrFileInfo->m_bPrivate = bPrivate;
    pCrFileInfo->m_nMsgId = (mycp::bigint)nWaitingProcessMsgId;
    if (m_callback)
        m_callback->OnSendingFile(*pCrFileInfo);
    if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pCrFileInfo, thePostEventPriority);
#else
        ::SendMessage(m_pHwnd, CR_WM_SENDING_FILE, (WPARAM)pCrFileInfo, 0);
        delete pCrFileInfo;
#endif
    }
    else {
        delete pCrFileInfo;
    }
    return true;
}
int CUserManagerApp::SendCrFile(eb::bigint sCallId,const EBFileString &sFilePath,mycp::bigint nResourceId)
{
#ifdef _QT_MAKE_
    return SendCrFile(sCallId, sFilePath.toStdString().c_str(), nResourceId);
#else
    return SendCrFile(sCallId, sFilePath.c_str(), nResourceId);
#endif
}
int CUserManagerApp::SendCrFile(eb::bigint sCallId,const char* sFilePath,mycp::bigint nResourceId)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return 1;
	}
	if (InviteCall(pCallInfo,0,0,true,false) != 0)
	{
		//if (bNeedWaitingCallback && pCallInfo->m_sGroupCode==0)
		//{
		//	if (!SendWaitingProcessCallback(sCallId, sFilePath, sTo, bPrivate, bOffFile))
		//		return 0;
		//}
		//if (pOutInviteCall!=NULL)
		//	*pOutInviteCall = true;
		CToSendList::pointer pCallToSendList;
		if (!theCallToSendList.find(sCallId,pCallToSendList))
		{
			pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
			theCallToSendList.insert(sCallId,pCallToSendList);
		}
		LogMessage("SendCrFile->InviteCall->AddFilePath... (%s)\r\n",sFilePath);
		pCallToSendList->AddFileResource(sFilePath,nResourceId);
		return 0;
		//return 3;
	}

	if(pCallInfo->m_pChatRoom.get()==NULL)
		return 2;
	return pCallInfo->m_pChatRoom->SendResource(nResourceId,sFilePath,false);
}
int CUserManagerApp::SendCrFile(eb::bigint sCallId,const EBFileString &sFilePath,mycp::bigint sTo,bool bPrivate,bool bOffFile, bool bFromToSendList,bool* pOutInviteCall,bool bNeedWaitingCallback)
{
#ifdef _QT_MAKE_
    return SendCrFile(sCallId, sFilePath.toStdString().c_str(), sTo, bPrivate, bOffFile, bFromToSendList, pOutInviteCall, bNeedWaitingCallback);
#else
    return SendCrFile(sCallId, sFilePath.c_str(), sTo, bPrivate, bOffFile, bFromToSendList, pOutInviteCall, bNeedWaitingCallback);
#endif
}
int CUserManagerApp::SendCrFile(eb::bigint sCallId,const char* sFilePath,mycp::bigint sTo,bool bPrivate,bool bOffFile, bool bFromToSendList,bool* pOutInviteCall,bool bNeedWaitingCallback)
{
	LogMessage("SendCrFile... (%s)\r\n",sFilePath);
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return 1;
	}
	//if (bNeedWaitingCallback && pCallInfo->m_sGroupCode==0)
	//{
	//	if (!SendWaitingProcessCallback(sCallId, sFilePath, sTo, bPrivate, bOffFile))
	//		return 0;
	//}
	bool bCanSendFileCall = false;
	if (sTo==0 || !bPrivate)
	{
		if (InviteCall(pCallInfo,0,0,true,false,&bCanSendFileCall) != 0 && !bFromToSendList && !bOffFile)	// * 离线文件不跑里面，直接跑后面发送
		{
#ifndef Q_OS_ANDROID
			if (bNeedWaitingCallback && pCallInfo->m_sGroupCode==0)
			{
				if (!SendWaitingProcessCallback(sCallId, sFilePath, sTo, bPrivate, bOffFile))
					return 0;
			}
#endif  /// Q_OS_ANDROID
			if (pOutInviteCall!=NULL)
				*pOutInviteCall = true;
			CToSendList::pointer pCallToSendList;
			if (!theCallToSendList.find(sCallId,pCallToSendList))
			{
				pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
				theCallToSendList.insert(sCallId,pCallToSendList);
			}
			LogMessage("SendCrFile->InviteCall->AddFilePath... (%s)\r\n",sFilePath);
			pCallToSendList->AddFilePath(sFilePath,sTo,bPrivate,bOffFile);
			//pCallToSendList->m_list.add(CToSendInfo::create(sFilePath,sTo,bPrivate,bOffFile));
			return 0;
			//return 3;
		}
	}else
	{
		// 群组，私聊
		if (InviteCall(pCallInfo,0, sTo) != 0 && !bFromToSendList)
		{
			//if (bNeedWaitingCallback && pCallInfo->m_sGroupCode==0)
			//{
			//	if (!SendWaitingProcessCallback(sCallId, sFilePath, sTo, bPrivate, bOffFile))
			//		return 0;
			//}

			if (pOutInviteCall!=NULL)
				*pOutInviteCall = true;
			CToSendList::pointer pCallToSendList;
			if (!theCallToSendList.find(sCallId,pCallToSendList))
			{
				pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
				theCallToSendList.insert(sCallId,pCallToSendList);
			}
			pCallToSendList->AddFilePath(sFilePath,sTo,bPrivate,bOffFile);
			//pCallToSendList->m_list.add(CToSendInfo::create(sFilePath,sTo,bPrivate,bOffFile));
			return 0;
			//return 3;
		}
	}
#ifndef Q_OS_ANDROID
    /// 安卓手机不做P2P
    /// 一对一，在线发送文件才处理，离线发送不处理；
	if (pCallInfo->m_sGroupCode==0 && !bOffFile)	// 离线文件不处理；
	{
		// 查找P2P发送
		{
			BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
			CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
			for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
			{
				const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
				if (pAccountInfo->GetUserId() == m_pMyAccountInfo->GetUserId())
					continue;

				{
					BoostReadLock rdLock(pAccountInfo->m_mutexLocalChatRoom);
					if (pAccountInfo->m_pLocalChatRoom.get()!=NULL && pAccountInfo->m_bLocalP2POk)
					{
						CToSendList::pointer pCallToSendList;
						if (!theP2PFileList.find(sCallId,pCallToSendList))
						{
							pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
							theP2PFileList.insert(sCallId,pCallToSendList);
						}
						pCallToSendList->AddFilePath(sFilePath,sTo,bPrivate,bOffFile);

						pAccountInfo->m_pLocalChatRoom->SetDestAddress(1,pAccountInfo->GetLocalAddress());
						return pAccountInfo->m_pLocalChatRoom->SendFile(sFilePath,sTo,bPrivate,bOffFile);
					}
				}
				{
					BoostReadLock rdLock(pAccountInfo->m_mutexRemoteChatRoom);
					if (pAccountInfo->m_pRemoteChatRoom.get()!=NULL && pAccountInfo->m_bRemoteP2POk)
					{
						CToSendList::pointer pCallToSendList;
						if (!theP2PFileList.find(sCallId,pCallToSendList))
						{
							pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
							theP2PFileList.insert(sCallId,pCallToSendList);
						}
						pCallToSendList->AddFilePath(sFilePath,sTo,bPrivate,bOffFile);

						pAccountInfo->m_pRemoteChatRoom->SetDestAddress(1,pAccountInfo->GetRemoteAddress());
						return pAccountInfo->m_pRemoteChatRoom->SendFile(sFilePath,sTo,bPrivate,bOffFile);
					}
				}
				if (m_pP2PTaskInfo.exist(sCallId))// || pAccountInfo->m_pLocalChatRoom.get()!=NULL)
				{
					if (bNeedWaitingCallback && pCallInfo->m_sGroupCode==0)
					{
						if (!SendWaitingProcessCallback(sCallId, sFilePath, sTo, bPrivate, bOffFile))
							return 0;
					}

					// 正在做P2P尝试，先保存，P2P成功或者超时会发送；
					CToSendList::pointer pCallToSendList;
					if (!theCallToSendList.find(sCallId,pCallToSendList))
					{
						pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
						theCallToSendList.insert(sCallId,pCallToSendList,false);
					}
					LogMessage("SendCrFile->m_pP2PTaskInfo.exist->AddFilePath... (%s)\r\n",sFilePath);
					pCallToSendList->AddFilePath(sFilePath,sTo,bPrivate,bOffFile);
					//pCallToSendList->m_list.add(CToSendInfo::create(sFilePath,sTo,bPrivate,bOffFile));
					return 0;
				}
			}
		}
		if (bCanSendFileCall)
		{
			if (InviteCall(pCallInfo,0,0,true,true) != 0 && !bFromToSendList)
			{
				if (bNeedWaitingCallback && pCallInfo->m_sGroupCode==0)
				{
					if (!SendWaitingProcessCallback(sCallId, sFilePath, sTo, bPrivate, bOffFile))
						return 0;
				}

				CToSendList::pointer pCallToSendList;
				if (!theCallToSendList.find(sCallId,pCallToSendList))
				{
					pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
					theCallToSendList.insert(sCallId,pCallToSendList);
				}
				LogMessage("SendCrFile->bCanSendFileCall(true)->InviteCall->AddFilePath... (%s)\r\n",sFilePath);
				pCallToSendList->AddFilePath(sFilePath,sTo,bPrivate,bOffFile);
				//pCallToSendList->m_list.add(CToSendInfo::create(sFilePath,sTo,bPrivate,bOffFile));
				return 0;
				//return 3;
			}
		}
	}
#endif  /// Q_OS_ANDROID
	if(pCallInfo->m_pChatRoom.get()==NULL)
		return 2;
	// **保存在线发送的文件，用于取消，或者对方上线P2P成功后，重新发送；
	CToSendList::pointer pCallToSendList;
	if (!theOnlineFileList.find(sCallId,pCallToSendList))
	{
		pCallToSendList = CToSendList::create(sCallId,pCallInfo->m_sGroupCode);
		theOnlineFileList.insert(sCallId,pCallToSendList);
	}
	LogMessage("SendCrFile->m_pChatRoom->SendFile->AddFilePath... (%s)\r\n",sFilePath);
	//LogMessage("SendCrFile->m_pChatRoom->SendFile->InviteCall->AddFilePath... (%s)\r\n",sFilePath);
	pCallToSendList->AddFilePath(sFilePath,sTo,bPrivate,bOffFile);
	//pCallToSendList->m_list.add(CToSendInfo::create(sFilePath,sTo,bPrivate,bOffFile));
    return pCallInfo->m_pChatRoom->SendFile(sFilePath,sTo,bPrivate,bOffFile);
}

int CUserManagerApp::AcceptCrFile(eb::bigint sCallId, bigint nMsgId, const EBFileString &sSaveTo)
{
#ifdef _QT_MAKE_
    return AcceptCrFile(sCallId, nMsgId, sSaveTo.toStdString().c_str());
#else
    return AcceptCrFile(sCallId, nMsgId, sSaveTo.c_str());
#endif

}
int CUserManagerApp::AcceptCrFile(eb::bigint sCallId,mycp::bigint nMsgId,const char* sSaveTo)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return 1;
	}
	if(pCallInfo->m_pChatRoom.get()==NULL)
		return 2;
	const tstring sResCmInfo = pCallInfo->m_pResCmInfo[nMsgId];
	std::vector<tstring> pList;
	if (!sResCmInfo.empty() && ParseString(sResCmInfo,",",false,pList)>=3)
	{
		//pCallInfo->m_pChatRoom->CancelSendingFile(nMsgId);	// 增加这个会有异常，不增加，文件传输界面会有一个没有清除（已经处理好；）
		const mycp::bigint sResourceId = cgc_atoi64(pList[0].c_str());
		const tstring sCmServer = pList[1];
		//tstring sCmAppName = pList[2];
		// 资源文件不存在，请求下载
		const tstring sCmServerKey = GetCmServerKey(sCmServer);
		Cchatroom::pointer pChatRoom;
		if (!theChatRoomList.find(sCmServerKey,pChatRoom))
		{
			pChatRoom = Cchatroom::create();
			pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
			pChatRoom->Start(sCmServer.c_str(), "",0,true,1);
			pChatRoom->SetCallBack(this);
			theChatRoomList.insert(sCmServerKey,pChatRoom);
		}
		theResourceChatRoom.insert(sResourceId,pChatRoom);
		//pChatRoom->SetParam(pCallInfo->GetCallId());		// 用callid会响应到会话中
		// 记录下resid对应callid
		theResource2CallId.remove(sResourceId);
		theResource2MsgId.remove(sResourceId);
		theResource2CallId.insert(sResourceId,pCallInfo->GetCallId());
		theResource2MsgId.insert(sResourceId,nMsgId);
		if (!pChatRoom->IsEnterOk())
			pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
		return pChatRoom->RecviveResource(sResourceId,sSaveTo);
	}else
	{
		if (pCallInfo->m_sGroupCode==0 && !pCallInfo->m_pOfflineFile.exist(nMsgId))	// 接收离线文件不处理；
		{
			// 查找P2P发送
			BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
			CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
			for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
			{
				const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
				if (pAccountInfo->GetUserId() != m_pMyAccountInfo->GetUserId())
				{
					{
						BoostReadLock rdLock(pAccountInfo->m_mutexLocalChatRoom);
						if (pAccountInfo->m_pLocalChatRoom.get()!=NULL && pAccountInfo->m_bLocalP2POk)
						{
							pAccountInfo->m_pLocalChatRoom->SetDestAddress(1,pAccountInfo->GetLocalAddress());
							return pAccountInfo->m_pLocalChatRoom->ReceiveFile(nMsgId,sSaveTo);
						}
					}
					{
						BoostReadLock rdLock(pAccountInfo->m_mutexRemoteChatRoom);
						if (pAccountInfo->m_pRemoteChatRoom.get()!=NULL && pAccountInfo->m_bRemoteP2POk)
						{
							pAccountInfo->m_pRemoteChatRoom->SetDestAddress(1,pAccountInfo->GetRemoteAddress());
							return pAccountInfo->m_pRemoteChatRoom->ReceiveFile(nMsgId,sSaveTo);
						}
					}
				}
			}
		}

		return pCallInfo->m_pChatRoom->ReceiveFile(nMsgId,sSaveTo);
	}
}

int CUserManagerApp::Save2CloudDrive(eb::bigint sCallId,mycp::bigint nMsgId)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		return 1;
	}
	if (pCallInfo->m_pChatRoom.get()==NULL)
		return 2;
	return pCallInfo->m_pChatRoom->SaveFile2CloudDrive(nMsgId);
}

//int CUserManagerApp::AcceptCrFileRes(eb::bigint sCallId,const char* sResId,const char* sSaveTo)
//{
//	CEBCallInfo::pointer pCallInfo;
//	if (!theCallInfoList.find(sCallId,pCallInfo))
//	{
//		return 1;
//	}
//	const tstring sResourceId(sResId);
//	const tstring sResCmInfo = pCallInfo->m_pResCmInfo[sResourceId];
//	std::vector<tstring> pList;
//	if (ParseString(sResCmInfo.c_str(),",",pList)==2)
//	{
//		tstring sCmServer = pList[0];
//		tstring sCmAppName = pList[1];
//		// 资源文件不存在，请求下载
//		const tstring sKey = sCmServer+sCmAppName;
//		Cchatroom::pointer pChatRoom;
//		if (!theChatRoomList.find(sKey,pChatRoom))
//		{
//			pChatRoom = Cchatroom::create();
//			pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
//			pChatRoom->Start(sCmServer.c_str(), sCmAppName.c_str(),0,false,1);
//			theChatRoomList.insert(sKey,pChatRoom);
//		}
//		theResourceChatRoom.insert(sResourceId,pChatRoom);
//		pChatRoom->SetCallBack(this);
//		pChatRoom->SetParam(pCallInfo->GetCallId());
//		pChatRoom->EnterRoom(m_pMyAccountInfo->GetAccount().c_str(),m_pMyAccountInfo->GetAccount().c_str(),
//			m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
//		pChatRoom->RecviveResource(sResId,sSaveTo);
//	}else
//	{
//		if(pCallInfo->m_pChatRoom.get()==NULL)
//			return 2;
//		return pCallInfo->m_pChatRoom->RecviveResource(sResId,sSaveTo);
//	}
//}
int CUserManagerApp::CancelCrFile(eb::bigint sCallId,mycp::bigint nMsgId)
{
	LogMessage("CancelCrFile... (callid=%lld)\r\n",sCallId);
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		theOnlineFileList.remove(sCallId);
		theP2PFileList.remove(sCallId);
		return 1;
	}

	{
		CToSendList::pointer pOnlineFileList;
		if (pCallInfo->m_pChatRoom.get()!=NULL && theOnlineFileList.find(sCallId,pOnlineFileList))
		{
			// 找到前面在线发送文件MSGID，取消；
			BoostWriteLock lock(pOnlineFileList->m_list.mutex());
			CLockList<CToSendInfo::pointer>::iterator pIter = pOnlineFileList->m_list.begin();
			for (; pIter!=pOnlineFileList->m_list.end(); pIter++)
			{
				const CToSendInfo::pointer& pCallToSendInfo = *pIter;
				if (pCallToSendInfo->m_nMsgId==nMsgId)
				{
					pOnlineFileList->m_list.erase(pIter);
					if (pOnlineFileList->m_list.empty())
						theOnlineFileList.remove(sCallId);
					return pCallInfo->m_pChatRoom->CancelSendingFile(nMsgId);
				}
			}
		}
	}

	if (pCallInfo->m_sGroupCode==0 && !pCallInfo->m_pOfflineFile.exist(nMsgId))	// 接收离线文件不处理；
	//if (pCallInfo->m_sGroupCode==0)
	{
		// 查找P2P发送
		BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
		CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
		for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
		{
			const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
			if (pAccountInfo->GetUserId() != m_pMyAccountInfo->GetUserId())
			{
				{
					BoostReadLock rdLock(pAccountInfo->m_mutexLocalChatRoom);
					if (pAccountInfo->m_pLocalChatRoom.get()!=NULL && pAccountInfo->m_bLocalP2POk)
					{
						m_pWaitList1.insert(nMsgId,0,false);
						CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_WAIT_ONCANCEL_FILE);
						pProcessMsgInfo->m_tProcessTime = time(0)+3;
						pProcessMsgInfo->m_pCallInfo = pCallInfo;
						pProcessMsgInfo->m_nBigInt1 = nMsgId;
						m_pProcessMsgList.add(pProcessMsgInfo);

						DeleteP2PFileList(sCallId, nMsgId);
						pAccountInfo->m_pLocalChatRoom->SetDestAddress(1,pAccountInfo->GetLocalAddress());
						return pAccountInfo->m_pLocalChatRoom->CancelSendingFile(nMsgId);
					}
				}
				{
					BoostReadLock rdLock(pAccountInfo->m_mutexRemoteChatRoom);
					if (pAccountInfo->m_pRemoteChatRoom.get()!=NULL && pAccountInfo->m_bRemoteP2POk)
					{
						m_pWaitList1.insert(nMsgId,0,false);
						CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_WAIT_ONCANCEL_FILE);
						pProcessMsgInfo->m_tProcessTime = time(0)+3;
						pProcessMsgInfo->m_pCallInfo = pCallInfo;
						pProcessMsgInfo->m_nBigInt1 = nMsgId;
						m_pProcessMsgList.add(pProcessMsgInfo);

						DeleteP2PFileList(sCallId, nMsgId);
						pAccountInfo->m_pRemoteChatRoom->SetDestAddress(1,pAccountInfo->GetRemoteAddress());
						return pAccountInfo->m_pRemoteChatRoom->CancelSendingFile(nMsgId);
					}
				}
			}
		}
	}

	if(pCallInfo->m_pChatRoom.get()==NULL)
		return 2;

	if (pCallInfo->m_sGroupCode==0)	// 记录 cancel msgid ，预防，该msgid是P2P消息，后来P2P失败，不能取消文件；
	{
		m_pWaitList1.insert(nMsgId,0,false);
		CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_WAIT_ONCANCEL_FILE);
		pProcessMsgInfo->m_tProcessTime = time(0)+3;
		pProcessMsgInfo->m_pCallInfo = pCallInfo;
		pProcessMsgInfo->m_nBigInt1 = nMsgId;
		m_pProcessMsgList.add(pProcessMsgInfo);
	}
	DeleteOnlineFileList(sCallId, nMsgId);
	return pCallInfo->m_pChatRoom->CancelSendingFile(nMsgId);
}

//int CUserManagerApp::SendMsg(const tstring& sToAccount,const tstring& sEmpCode,EB_MSG_TYPE nMsgType, const tstring& sMsgContent, mycp::bigint sDepCode, bool bOffSave)//, const tstring& sFromInfo)
//{
//	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_USER_MSG);
//	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from", sToAccount));
//	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("emp-code", sEmpCode));
//	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("msg-type", (int)nMsgType));
//	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("msg-content", sMsgContent));
//	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("dep-code", sDepCode));
//	//pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("f-info", sFromInfo));
//	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("off-save", bOffSave));
//	return QueryUser(sToAccount, 0, "", pSotpRequestInfo);
//}

int CUserManagerApp::CallMember(mycp::bigint sMemberCode,mycp::bigint sExistCallId,mycp::bigint sOldCallId,bool bOwnerCall)
{
	CEBCallInfo::pointer pCallInfo;
	if (sExistCallId>0 && !theCallInfoList.find(sExistCallId,pCallInfo))
	{
		return 1;
	}

	CEBMemberInfo::pointer pEmployeeInfo;
	if (!theEmployeeList.find(sMemberCode, pEmployeeInfo))
	{
		// 找不到成员信息
		return 1;
	}else if (pEmployeeInfo->m_nMemberUserId==m_pMyAccountInfo->GetUserId())
	{
		// 不能呼叫自己
		return 1;
	}
	//mycp::bigint nToUserId = pEmployeeInfo->m_nMemberUserId;
	const mycp::bigint nToUserId(pEmployeeInfo->m_nMemberUserId);
	if (sExistCallId==0)
	{
		tstring pToAccount;
		EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
		CEBCallInfo::pointer pInCallInfo = GetCallInfoByUser(nToUserId,pToAccount,pOutLineState);	// 查找在线用户，不在线无效，需要重新邀请
		if (pInCallInfo.get() != NULL && pOutLineState == EB_LINE_STATE_ONLINE_NEW &&
			(pInCallInfo->m_nCallState == EB_CALL_STATE_EXIT || pInCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED || pInCallInfo->m_nCallState==EB_CALL_STATE_INVALIDATE))
		{
			// 呼叫或者通话中...
			if (pInCallInfo->m_nCallState==EB_CALL_STATE_INVALIDATE)
				this->InviteCall(pInCallInfo,0);
			else
				EnterRoom(pInCallInfo);
			SendConnectMsg(pInCallInfo,nToUserId,pToAccount,EB_CONNECTED_OWNER_CALL);
			return 0;
		}
	}
	if (pCallInfo.get()!=NULL)
		pCallInfo->m_sOldCallId = sOldCallId==pCallInfo->GetCallId()?0:sOldCallId;
	// **全部通过LC查询；
	//if (!pEmployeeInfo->IsOfflineState() && !pEmployeeInfo->m_sUmServer.empty())
	//{
	//	// 用户在线状态
	//	CPOPCUserManager::pointer pPOPCUserManager;
	//	if (m_pUserManager.get() != NULL && m_pUserManager->GetAddress().address() == pEmployeeInfo->m_sUmServer)
	//		pPOPCUserManager = m_pUserManager;
	//	else if (!theUserManagerList.find(nToUserId, pPOPCUserManager))
	//	{
	//		pPOPCUserManager = CPOPCUserManager::create(this,0);
	//		pPOPCUserManager->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
	//		int ret = pPOPCUserManager->Start(pEmployeeInfo->m_sUmServer, pEmployeeInfo->m_sUmAppName,0,true,1);
	//		if (ret != 0)
	//		{
	//			EB_CallInfo pEbCallInfo(nToUserId,pEmployeeInfo->m_sMemberAccount,pCallInfo->GetCallId(),pCallInfo->m_sGroupCode);
	//			if (m_callback)
	//				m_callback->onCallError(pEbCallInfo,EB_STATE_ERROR);
	//			if (m_pHwnd!=NULL)
	//				::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)&pEbCallInfo, EB_STATE_ERROR);
	//			return -1;
	//		}
	//		pPOPCUserManager->SetUserId(this->m_pMyAccountInfo->GetUserId());
	//	}
	//	theUserManagerList.insert(nToUserId, pPOPCUserManager);	// 统一放在这里
	//	theOwnerCallUser.insert(nToUserId,bOwnerCall);
	//	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_INVITE);
	//	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("member-code", sMemberCode));
	//	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", nToUserId));
	//	return pPOPCUserManager->SendUMIUUser(sMemberCode,nToUserId,pCallInfo,0,pSotpRequestInfo);
	//}
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_INVITE);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from_uid", nToUserId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("member-code", sMemberCode));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("exist-call-id", sExistCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("old-cid", sOldCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("owner-call", bOwnerCall));
	return QueryUser(EB_REQ_TYPE_INVITE,0,nToUserId,sMemberCode,0, "",pSotpRequestInfo);
}

int CUserManagerApp::CallAccount(const tstring & sToAccount,mycp::bigint sExistCallId, mycp::bigint sOldCallId,bool bOwnerCall)
{
	if (sToAccount.empty())
	{
		return 1;
	}
	tstring sToFullAccount(sToAccount);
	if (!m_sAccountPrefix.empty() && !IsFullNumber(sToFullAccount.c_str(), sToFullAccount.size()) && sToFullAccount.find(m_sAccountPrefix)==std::string::npos)	// "@"，非邮箱才处理；
	//if (!m_sAccountPrefix.empty() && sToFullAccount.substr(0,m_sAccountPrefix.size())!=m_sAccountPrefix)
	{
		sToFullAccount = m_sAccountPrefix+sToAccount;
	}
	if (m_pMyAccountInfo->GetAccount() == sToFullAccount ||
		(IsFullNumber(sToFullAccount.c_str(),sToFullAccount.size()) && cgc_atoi64(sToFullAccount.c_str())==m_pMyAccountInfo->GetUserId()))
	{
		// 不能呼叫自己！
		return 2;
	}
	CEBMemberInfo::pointer pCallMemberInfo = GetEmployeeInfo(sToFullAccount);
	if (pCallMemberInfo.get()!=NULL)
	{
		return CallMember(pCallMemberInfo->m_sMemberCode,sExistCallId,sOldCallId,bOwnerCall);
	}

	//if (!sExistCallId.empty() && !theCallInfoList.exist(sExistCallId))
	//{
	//	return 1;
	//}
	if (sExistCallId==0)
	{
		mycp::bigint nToUserId = 0;
		EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
		CEBCallInfo::pointer pInCallInfo = GetCallInfoByUser(sToFullAccount,nToUserId,pOutLineState);	// 查找在线用户，不在线无效，需要重新邀请
		if (pInCallInfo.get() != NULL && pOutLineState == EB_LINE_STATE_ONLINE_NEW &&
			(pInCallInfo->m_nCallState == EB_CALL_STATE_EXIT || pInCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED || pInCallInfo->m_nCallState==EB_CALL_STATE_INVALIDATE))
		{
			// 呼叫或者通话中...
			if (pInCallInfo->m_nCallState==EB_CALL_STATE_INVALIDATE)
				this->InviteCall(pInCallInfo,0);
			else
				EnterRoom(pInCallInfo);
			SendConnectMsg(pInCallInfo,nToUserId,sToFullAccount,EB_CONNECTED_OWNER_CALL);
			return 0;
		}
	}

	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_INVITE);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from", sToFullAccount));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("exist-call-id", sExistCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("old-cid", sOldCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("owner-call", bOwnerCall));
	return QueryUser(EB_REQ_TYPE_INVITE,0,0,0,0,sToFullAccount,pSotpRequestInfo);
}
int CUserManagerApp::CallUserId(mycp::bigint nToUserId,mycp::bigint sExistCallId,mycp::bigint sOldCallId,bool bOwnerCall)
{
	if (nToUserId==0) return 1;
	if (m_pMyAccountInfo->GetUserId() == nToUserId)
	{
		// 不能呼叫自己！
		return 2;
	}
	//CEBMemberInfo::pointer pCallMemberInfo = GetEmployeeInfo(sToAccount);
	//if (pCallMemberInfo.get()!=NULL)
	//{
	//	return CallMember(pCallMemberInfo->m_sMemberCode,sExistCallId,sOldCallId);
	//}

	//if (!sExistCallId.empty() && !theCallInfoList.exist(sExistCallId))
	//{
	//	return 1;
	//}
	if (sExistCallId==0)
	{
		CEBString sToAccount;
		EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
		CEBCallInfo::pointer pInCallInfo = GetCallInfoByUser(nToUserId,sToAccount,pOutLineState);	// 查找在线用户，不在线无效，需要重新邀请
		if (pInCallInfo.get() != NULL && pOutLineState == EB_LINE_STATE_ONLINE_NEW &&
			(pInCallInfo->m_nCallState == EB_CALL_STATE_EXIT || pInCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED || pInCallInfo->m_nCallState==EB_CALL_STATE_INVALIDATE))
		{
			// 呼叫或者通话中...
			if (pInCallInfo->m_nCallState==EB_CALL_STATE_INVALIDATE)
				this->InviteCall(pInCallInfo,0);
			else
				EnterRoom(pInCallInfo);
			SendConnectMsg(pInCallInfo,nToUserId,sToAccount,EB_CONNECTED_OWNER_CALL);
			return 0;
		}
	}
	LogMessage("CallUserId... (uid=%lld)\r\n",nToUserId);
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_INVITE);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from_uid", nToUserId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("exist-call-id", sExistCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("old-cid", sOldCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("owner-call", bOwnerCall));
	return QueryUser(EB_REQ_TYPE_INVITE,0,nToUserId,0,0,"",pSotpRequestInfo);
}

int CUserManagerApp::QueryUser(EB_REQEUST_TYPE nReqType,int nSubType,mycp::bigint nUserId,mycp::bigint nMemberId,mycp::bigint nGroupId,
							   const tstring & sToAccount, const CPOPSotpRequestInfo::pointer& pSotpRequestInfo)
{
	const tstring sAddress(m_sServerAddress);
	//if (!entboost::GetAccountAddress(sToAccount.c_str(), sAddress))
	//	return -1;

	m_tLogonCenter = time(0);
	// 地址不相同，需要更换
	// 地址相同，不需要更换，用于在类似群部门连续请求会话中，提高效率
	if (m_pLogonCenter.get()!=NULL && m_pLogonCenter->GetAddress().address()!=sAddress)	
	{
		m_pLogonCenter->Stop();
		m_pLogonCenter.reset();
	}
	if (m_pLogonCenter.get()==NULL)
	{
		bool bRetry = false;
		m_pLogonCenter = CPOPCLogonCenter::create(this);
		m_pLogonCenter->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
		int ret = m_pLogonCenter->Start(CCgcAddress(sAddress, CCgcAddress::ST_UDP),0,1);
		if (ret==-1)	
		{
			// 域名解析问题，再试一次；
			m_pLogonCenter.reset();
			const tstring sServerAddress = GetAddressList(sAddress);
			if (sServerAddress.empty()) return ret;
			m_pLogonCenter = CPOPCLogonCenter::create(this);
			m_pLogonCenter->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
			ret = m_pLogonCenter->Start(CCgcAddress(sServerAddress, CCgcAddress::ST_UDP),0,1);
			bRetry = true;
		}
		if (ret!=0)
		{
			m_pLogonCenter.reset();
			return ret;
		}
		// 成功记下，域名和IP地址；
		if (!bRetry)
		{
			SaveAddressList(sAddress);
		}
		//m_pLogonCenter = CPOPCLogonCenter::create(this);
		//m_pLogonCenter->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
		//int ret = m_pLogonCenter->Start(CCgcAddress(sAddress, CCgcAddress::ST_UDP));
		//if (ret != 0)
		//{
		//	m_pLogonCenter.reset();
		//	return -1;
		//}
	}
#ifdef WIN32
	Sleep(2);	// ** 预防添加多用户
#else
	usleep(2000);	// ** 预防添加多用户
#endif
	if (nUserId > 0)
	{
		char lpszAccount[24];
		sprintf(lpszAccount,"%lld",nUserId);
		return m_pLogonCenter->SendLCQuery(0,m_sDevAppId,m_sDevAppOnlineKey,nReqType,nSubType,this->m_pMyAccountInfo->GetUserId(),m_sCallKey,nMemberId,nGroupId,lpszAccount,pSotpRequestInfo);
	}else
	{
		return m_pLogonCenter->SendLCQuery(0,m_sDevAppId,m_sDevAppOnlineKey,nReqType,nSubType,this->m_pMyAccountInfo->GetUserId(),m_sCallKey,nMemberId,nGroupId,sToAccount,pSotpRequestInfo);
	}
}

int CUserManagerApp::Call2Dep(mycp::bigint sCallId,const tstring& sToAccount)
{
	if (m_pUserManager.get() == NULL) return -1;
	if (m_pMyAccountInfo.get() == NULL) return -1;
	if (!sToAccount.empty() && sToAccount == m_pMyAccountInfo->GetAccount())
		return -1;	// 不能邀请自己

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId,pCallInfo))
	{
		// 不存在会话；
		return 1;
	}
	const int bCall2Dep = pCallInfo->m_sGroupCode==0?1:0;
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_INVITE);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("from", sToAccount));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("exist-call-id", sCallId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call2group", bCall2Dep));
	return QueryUser(EB_REQ_TYPE_INVITE,0,0,0,0,sToAccount,pSotpRequestInfo);

	//CEBCallInfo::pointer pCallInfo;
	//if (!theCallInfoList.find(sCallId, pCallInfo))
	//{
	//	// 不存在会话；
	//	return 1;
	//}
	//CPOPCUserManager::pointer pPOPCUserManager;
	//if (!theUserManagerList.find(sCallId,pPOPCUserManager))
	//{
	//	return 1;
	//}
	//int bCall2Dep = pCallInfo->m_sGroupCode==0?1:0;
	//CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_INVITE);
	//pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-account", sToAccount));
	//pPOPCUserManager->SendUMIUUser(0,0,sToAccount,pCallInfo,bCall2Dep,pSotpRequestInfo);
	//return 0;
}

int CUserManagerApp::CallGroup(mycp::bigint sDepCode,bool bOwnerCall,bool bQueryForce)
{
	//if (!sToAccount.empty() && sToAccount == m_pMyAccountInfo->GetAccount())
	//	return -1;	// 不能邀请自己
	if (!bQueryForce)
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		theDepartmentList.find(sDepCode, pDepartmentInfo);
        if (pDepartmentInfo.get() == NULL || pDepartmentInfo->m_nMyEmpId==0)
//		if (pDepartmentInfo.get() == NULL || !pDepartmentInfo->m_pMemberList.exist(m_pMyAccountInfo->GetUserId()))
		{
			// 群组不存在
			// 不属于本群组，不能发起会话
			return 1;
		}
	}
	//tstring sFromInfo;
	//if (!sToAccount.empty())
	//{
	//	if (pDepartmentInfo.get() != NULL)
	//	{
	//		EB_ACCOUNT_TYPE nAccountType = pDepartmentInfo->m_pMemberList.exist(sToAccount)?EB_ACCOUNT_TYPE_IN_ENT:EB_ACCOUNT_TYPE_OUT_ENT;
	//		sFromInfo = GetDepFromInfo(nAccountType,pDepartmentInfo);
	//	}else
	//	{
	//		CEBECardInfo::pointer pPopECardInfo = GetMyECardInfo(sEmpCode);
	//		if (pPopECardInfo.get() == NULL) return -1;
	//		sFromInfo = GetECardFromInfo(pPopECardInfo);
	//	}
	//}
	// 先生成一个模拟会话
	const mycp::bigint sExistCallId = sDepCode;
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sExistCallId, pCallInfo))
	{
		CEBAccountInfo::pointer pFromAccountInfo = CEBAccountInfo::create(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetAccount());
		pCallInfo = CEBCallInfo::create(sExistCallId,sDepCode,pFromAccountInfo);
		theCallInfoList.insert(sExistCallId, pCallInfo);
		BuildGroupCallUserList(pCallInfo,sDepCode);
		//pCallInfo->m_sGroupCode = sDepCode;
		//pCallInfo->m_pCallUserList.insert(m_pMyAccountInfo->GetUserId(), pFromAccountInfo);
	}else if (!pCallInfo->m_pCallUserList.exist(m_pMyAccountInfo->GetUserId()))
	{
		CEBAccountInfo::pointer pFromAccountInfo = CEBAccountInfo::create(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetAccount());
		pCallInfo->m_pCallUserList.insert(m_pMyAccountInfo->GetUserId(), pFromAccountInfo);
	}else if (!bQueryForce && (pCallInfo->m_nCallState == EB_CALL_STATE_EXIT || pCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED))
	{
		// 呼叫或者通话中...
		this->InviteCall(pCallInfo,0);
		EnterRoom(pCallInfo);
		SendConnectMsg(pCallInfo,m_pMyAccountInfo->GetUserId(), m_pMyAccountInfo->GetAccount(),EB_CONNECTED_OWNER_CALL);
		return 0;
	}

	//if (!sToAccount.empty() && !pCallInfo->m_pCallUserList.exist(sToAccount))
	//{
	//	CEBAccountInfo::pointer pToAccountInfo = CEBAccountInfo::create(sToAccount);
	//	pToAccountInfo->m_nAccountType = nAccountType;
	//	pCallInfo->m_pCallUserList.insert(sToAccount, pToAccountInfo);
	//}

	// 查询可用聊天服务器
	CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0,EB_REQ_TYPE_INVITE);
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("owner-call", bOwnerCall));
	return QueryUser(EB_REQ_TYPE_INVITE,0,m_pMyAccountInfo->GetUserId(),0,sDepCode, "",pSotpRequestInfo);
}
//CEBGroupInfo::pointer CUserManagerApp::Call2Dep(const CEBCallInfo::pointer& pCallInfo)
//{
//	if (m_pLogonCenter.get() == NULL) return NullPopDepartmentInfo;
//	if (!pCallInfo->m_sGroupCode.empty())
//	{
//		CEBGroupInfo::pointer pDepartmentInfo;
//		theDepartmentList.find(pCallInfo->m_sGroupCode, pDepartmentInfo);
//		return pDepartmentInfo;
//	}else
//	{
//		pCallInfo->m_sGroupCode = pCallInfo->GetCallId();
//		CEBGroupInfo::pointer pDepartmentInfo;
//		if (!theDepartmentList.find(pCallInfo->m_sGroupCode, pDepartmentInfo))
//		{
//			pDepartmentInfo = CEBGroupInfo::create("", pCallInfo->m_sGroupCode);
//			pDepartmentInfo->m_sGroupName = "call group";
//			pDepartmentInfo->m_sCreator = m_pLogonCenter->GetLogonAccount();
//			pDepartmentInfo->m_nGroupType = EB_GROUP_TYPE_TEMP;
//			theDepartmentList.insert(pCallInfo->m_sGroupCode, pDepartmentInfo);
//
//		}
//		return pDepartmentInfo;
//	}
//}

int CUserManagerApp::InviteCall(mycp::bigint sExistCallId, mycp::bigint sOldCallId, mycp::bigint sOnlyAccount,bool bOffReCall)
{
	if (m_pUserManager.get() == NULL) return -1;
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sExistCallId, pCallInfo))
	{
		// not callinfo
		return -1;
	}
	return InviteCall(pCallInfo,sOldCallId,sOnlyAccount,bOffReCall);
}
int CUserManagerApp::GetOffLineCount(const CEBCallInfo::pointer & pCallInfo) const
{
	int result = 0;
	BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
	CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
	for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
	{
		const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
		//if (pAccountInfo->IsOffLineState())	// 用户离线（不需要呼叫）
		if (pAccountInfo->IsUnknownLineState())	// 未知状态（需要呼叫）
			result++;
	}
	return result;
}
int CUserManagerApp::InviteCall(const CEBCallInfo::pointer & pCallInfo, mycp::bigint sOldCallId, mycp::bigint sOnlyAccount,bool bOffReCall,bool bSendFile,bool* pOutCanSendFileCall)
{
	if (pCallInfo->m_nCallState == EB_CALL_STATE_ALERTING) return 0;	// 已经在邀请中...
	//if (!pCallInfo->m_sGroupCode.empty())
	//{
	//	if (pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE)
	//	{
	//		// 会话失效，需要重新呼叫
	//		CallGroup(pCallInfo->m_sGroupCode);
	//		return 1;
	//	}
	//	return 0;						// 群组不需要重新邀请
	//}

	if (pCallInfo->m_sGroupCode>0)
	{
		if (pCallInfo->m_pChatRoom.get()==NULL)
		{
			CallGroup(pCallInfo->m_sGroupCode,false,true);
			return 1;
		}else if (pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE || pCallInfo->m_nCallState == EB_CALL_STATE_HANGUP)
		{
			// 群组会话失效，需要重新呼叫
			// >=3个，主动发起新呼叫；
			if (bOffReCall)
				CallGroup(pCallInfo->m_sGroupCode,false,true);
			return 1;
		}else if (pCallInfo->m_pChatRoom.get()!=NULL && pCallInfo->m_nCallState == EB_CALL_STATE_EXIT)// || pCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED)
		{
			// 呼叫或者通话中...
			//this->InviteCall(pCallInfo,0);
			//EnterRoom(pCallInfo);
			SendConnectMsg(pCallInfo,m_pMyAccountInfo->GetUserId(), m_pMyAccountInfo->GetAccount(),EB_CONNECTED_OWNER_CALL);
		}

		// 群组本端正常，直接发消息；
		return 0;
	}

	// ?
	try
	{
		// ** 预防发送多文件，连续呼叫多次
		if (pCallInfo->m_nCallTime>0 && pCallInfo->m_nCallTime+2>=time(0))	// 012 = 3秒
		{
			return 1;	// 当请求呼叫成功
		}
		BoostReadLock rdlock(pCallInfo->m_pCallUserList.mutex());
		CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
		for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
		{
			const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
			if (pAccountInfo->GetUserId() == m_pMyAccountInfo->GetUserId())
			{
				continue;
			}
			if ((pAccountInfo->m_dwAccountData&EB_DATA_SEND_FILE_CALL)==0)
			{
				if (pOutCanSendFileCall!=NULL)
					*pOutCanSendFileCall = true;
				if (bSendFile)
				{
					pAccountInfo->m_dwAccountData |= EB_DATA_SEND_FILE_CALL;
					const mycp::bigint nAccountUserId = pAccountInfo->GetUserId();
					rdlock.unlock();
					theSendFileCallTime.insert(pCallInfo->GetCallId(),true,true);
					pCallInfo->m_nCallTime = time(0);
					CallUserId(nAccountUserId, pCallInfo->GetCallId(), sOldCallId);
					return 1;
				}
			}
            if (pCallInfo->m_sGroupCode>0 ||
                    pAccountInfo->m_pFromCardInfo.m_nAccountType!=EB_ACCOUNT_TYPE_VISITOR ||
                    pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE)
			{
				if (pCallInfo->m_nCallState == EB_CALL_STATE_HANGUP ||
					pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE ||	// 会话失效
					//pAccountInfo->IsOffLineState() ||						// 离线
					pAccountInfo->IsUnknownLineState() ||					// （未知状态）需要呼叫
					//!pAccountInfo->IsOnLineState() ||						// 离线
					sOldCallId>0)									// sOldCallId 不为空，说明是对方新会话，要求合并
				{
					const mycp::bigint nAccountUserId = pAccountInfo->GetUserId();
					rdlock.unlock();
					if (bOffReCall)
					{
						pCallInfo->m_nCallTime = time(0);
						CallUserId(nAccountUserId, pCallInfo->GetCallId(), sOldCallId);
					}
					return 1;
				}
			}
		}
	}catch(std::exception&)
	{
	}catch(...)
	{}
	return 0;

	//if (pCallInfo->m_sGroupCode>0 &&
	//	(pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE || pCallInfo->m_nCallState == EB_CALL_STATE_HANGUP || GetOffLineCount(pCallInfo)>=3))
	//{
	//	// 群组会话失效，需要重新呼叫
	//	// >=3个，主动发起新呼叫；
	//	if (bOffReCall)
	//		CallGroup(pCallInfo->m_sGroupCode,false,true);
	//	return 1;
	//}

	//// 只自带某个用户，用于在群组聊天中
	//if (sOnlyAccount>0)
	//{
	//	CEBAccountInfo::pointer pAccountInfo;
	//	if (!pCallInfo->m_pCallUserList.find(sOnlyAccount,pAccountInfo))
	//	{
	//		return -1;
	//	}
	//	if ((pCallInfo->m_sGroupCode>0 || pAccountInfo->m_pFromCardInfo.m_nAccountType!=EB_ACCOUNT_TYPE_VISITOR)
	//		&& pAccountInfo->GetUserId() != m_pMyAccountInfo->GetUserId())
	//	{
	//		if (pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE ||	// 会话失效
	//			//pAccountInfo->IsUnknownLineState() ||					// 未知状态
	//			!pAccountInfo->IsOnLineState() ||						// **没有在线（包括离线和未知状态）；只呼叫某人，用这个更好一些；
	//			sOldCallId>0)									// sOldCallId 不为空，说明是对方新会话，要求合并
	//		{
	//			// **不能用这里的MemberCode做呼叫，因为有可能对方的MemberCode是其他组
	//			//if (pAccountInfo->m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_IN_ENT && !pAccountInfo->m_pFromCardInfo.m_sMemberCode.empty())
	//			//	CallMember(pAccountInfo->m_pFromCardInfo.m_sMemberCode, pCallInfo->GetCallId(), sOldCallId);
	//			//else
	//			if (bOffReCall)
	//				CallUserId(pAccountInfo->GetUserId(), pCallInfo->GetCallId(), sOldCallId);
	//			//const tstring sEmpCode = pAccountInfo->m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_IN_ENT?pAccountInfo->m_pFromCardInfo.m_sMemberCode:"";
	//			//CallUser(pAccountInfo->GetAccount().c_str(), sEmpCode, pCallInfo->GetCallId(), sOldCallId);
	//			return 1;
	//		}
	//	}
	//	return 0;
	//}

	//// ?
	//int ret = 0;
	//try
	//{
	//	BoostReadLock rdlock(pCallInfo->m_pCallUserList.mutex());
	//	CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
	//	for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
	//	{
	//		CEBAccountInfo::pointer pAccountInfo = pIter->second;
	//		if (pAccountInfo->GetUserId() == m_pMyAccountInfo->GetUserId())
	//			continue;
	//		if (pCallInfo->m_sGroupCode>0 || pAccountInfo->m_pFromCardInfo.m_nAccountType!=EB_ACCOUNT_TYPE_VISITOR || pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE)
	//		{
	//			if (pCallInfo->m_nCallState == EB_CALL_STATE_HANGUP ||
	//				pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE ||	// 会话失效
	//				//pAccountInfo->IsOffLineState() ||						// 离线
	//				pAccountInfo->IsUnknownLineState() ||					// （未知状态）需要呼叫
	//				//!pAccountInfo->IsOnLineState() ||						// 离线
	//				sOldCallId>0)									// sOldCallId 不为空，说明是对方新会话，要求合并
	//			{
	//				//rdlock.unlock();
	//				ret = 1;
	//				// **不能用这里的MemberCode做呼叫，因为有可能对方的MemberCode是其他组
	//				//if (pAccountInfo->m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_IN_ENT && !pAccountInfo->m_pFromCardInfo.m_sMemberCode.empty())
	//				//	CallMember(pAccountInfo->m_pFromCardInfo.m_sMemberCode, pCallInfo->GetCallId(), sOldCallId);
	//				//else

	//				//if (pCallInfo->m_sGroupCode!=0)
	//				//{
	//				//	// 群组会话，直接呼叫会话，然后退出；
	//				//	CallGroup(pCallInfo->m_sGroupCode,false,true);
	//				//	break;
	//				//}else
	//				if (bOffReCall)
	//				{
	//					CallUserId(pAccountInfo->GetUserId(), pCallInfo->GetCallId(), sOldCallId);
	//				}else
	//				{
	//					break;
	//				}
	//				//const tstring sEmpCode = pAccountInfo->m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_IN_ENT?pAccountInfo->m_pFromCardInfo.m_sMemberCode:"";
	//				//CallUser(pAccountInfo->GetAccount().c_str(), sEmpCode, pCallInfo->GetCallId(), sOldCallId);
	//				//rdlock.lock();
	//				Sleep(5);
	//			}
	//		}
	//	}
	//}catch(std::exception&)
	//{
	//}catch(...)
	//{}
	//return ret;
}

int CUserManagerApp::CallAck(mycp::bigint sCallId,EB_CALL_ACK_TYPE nAckType)
{
	if (m_pUserManager.get() == NULL) return -1;
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		// not callinfo
		return 1;
	}
	return CallAck(pCallInfo,nAckType);
}
int CUserManagerApp::CallAck(const CEBCallInfo::pointer & pCallInfo, EB_CALL_ACK_TYPE nAckType)
{
	//BOOST_ASSERT (pCallInfo.get() != NULL);
	if (m_pUserManager.get() == NULL)
	{
		return -1;
	}

	CPOPCUserManager::pointer pPOPCUserManager;
	if (!theUserManagerList.find(pCallInfo->GetCallId(),pPOPCUserManager))
	{
		return 1;
	}
	if (nAckType == EB_CAT_ACCEPT)
	{
		if (pCallInfo->m_nCallState!=EB_CALL_STATE_AUTO_ACK)
		{
			// 非自动应答；
			if (pCallInfo->m_sGroupCode>0)
				theOwnerCallGroup.insert(pCallInfo->m_sGroupCode,true);
			else
				theOwnerCallUser.insert(pCallInfo->GetFromUserId(),true);
		}
		//theAcceptUser.insert(pCallInfo->GetFromUserId(),true);
	}
	//else if (nAckType == EB_CAT_REJECT)
	//	theAcceptUser.remove(pCallInfo->GetFromUserId());
	pCallInfo->m_tLastTime = time(0);
	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_INVITE);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-id", pCallInfo->GetCallId()));
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", pCallInfo->GetFromUserId()));
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ack-type", (int)nAckType));
	return pPOPCUserManager->SendUMIUAck(pCallInfo->GetCallId(),nAckType,pRequestInfo);
}

int CUserManagerApp::CallHangup(mycp::bigint sCallId)
{
	//if (m_pUserManager.get() == NULL) return -1;
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		// not callinfo
		return 1;
	}

	CPOPCUserManager::pointer pPOPCUserManager;
	if (!theUserManagerList.find(sCallId,pPOPCUserManager))
	{
		return 1;
	}
	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-id", sCallId));
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("hangup", true));
	return pPOPCUserManager->SendUMIUHangup(sCallId,true,pRequestInfo);
}
int CUserManagerApp::CallNotify(eb::bigint nCallId, eb::bigint nToUserId, int nNotifyType, eb::bigint nNotifyId, const char* sNotifyData)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(nCallId, pCallInfo))
	{
		// not callinfo
		return 1;
	}else if (nToUserId>0 && !pCallInfo->m_pCallUserList.exist(nToUserId))
	{
		return 2;
	}

	if (pCallInfo->m_pChatRoom.get()==NULL) return 3;
	return pCallInfo->m_pChatRoom->Notify(nToUserId, nNotifyType, nNotifyId, sNotifyData);
}
void CUserManagerApp::ClearAllCallInfo(void)
{
	{
		BoostReadLock lock(theCallInfoList.mutex());
		CLockMap<mycp::bigint, CEBCallInfo::pointer>::const_iterator pIter = theCallInfoList.begin();
		for (;pIter!=theCallInfoList.end(); pIter++)
		{
			const CEBCallInfo::pointer& pCallInfo = pIter->second;
			if (pCallInfo->m_pChatRoom.get()!=NULL)
			{
				pCallInfo->m_pChatRoom->ExitRoom(0);
			}
			DoVideoDisonnecte(pCallInfo,true);
		}
	}
	theCallInfoList.clear();
}
bool CUserManagerApp::ClearCallInfo(mycp::bigint sCallId,bool bHangup, bool bSendHangup, bool bExitSes)
{
	theOnlineFileList.remove(sCallId);
	theP2PFileList.remove(sCallId);

	CPOPCUserManager::pointer pPOPCUserManager;
	if (bHangup)
	{
		//LogMessage("ClearCallInfo... hangup=true, (callid=%lld)\r\n",sCallId);
		if (theUserManagerList.find(sCallId, pPOPCUserManager,true))
		{
			if (bSendHangup)
				pPOPCUserManager->SendUMIUHangup(sCallId, true);
			else	// 实现退出清空call info
				pPOPCUserManager->SendUMIUHangup(sCallId, false);
			m_pHangupUm.add(pPOPCUserManager);
		}
	}
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo,bHangup))
	{
		DoVideoDisonnecte(pCallInfo,true);

		if (bHangup)
		{
			BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
			CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
			for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
			{
				const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
				{
					pAccountInfo->m_bLocalP2POk = false;
					BoostWriteLock wtLock(pAccountInfo->m_mutexLocalChatRoom);
					if (pAccountInfo->m_pLocalChatRoom.get()!=NULL)
					{
						m_pClearChatRoom.add(pAccountInfo->m_pLocalChatRoom);
						pAccountInfo->m_pLocalChatRoom.reset();
					}
				}
				{
					pAccountInfo->m_bRemoteP2POk = false;
					BoostWriteLock wtLock(pAccountInfo->m_mutexRemoteChatRoom);
					if (pAccountInfo->m_pRemoteChatRoom.get()!=NULL)
					{
						m_pClearChatRoom.add(pAccountInfo->m_pRemoteChatRoom);
						pAccountInfo->m_pRemoteChatRoom.reset();
					}
				}

				CEBCallInfo::pointer pExistCallInfo = GetExistCallInfo(pAccountInfo->GetUserId(),0);
				if (pExistCallInfo.get()==NULL)
				{
					// 没有在用UM，清空数据。
					//LogMessage("ClearCallInfo... bHangup=true,pExistCallInfo.get()==NULL, (callid=%lld)\r\n",sCallId);
					if (theUserManagerList.find(pAccountInfo->GetUserId(), pPOPCUserManager,true))
					{
						m_pHangupUm.add(pPOPCUserManager);
					}
				}
			}

			if (pCallInfo->m_pChatRoom.get()!=NULL)
			{
				pCallInfo->m_pChatRoom->ExitRoom((bExitSes&&(pCallInfo->m_sGroupCode==0))?1:0);	// 群组不能真正退出
				//m_pClearChatRoom.add(pCallInfo->m_pChatRoom);
				//pCallInfo->m_pChatRoom.reset();
			}
			pCallInfo->m_nCallState = EB_CALL_STATE_HANGUP;
		}else if (pCallInfo->m_nCallState == EB_CALL_STATE_EXIT || pCallInfo->m_nCallState == EB_CALL_STATE_HANGUP)
		{
			pCallInfo->m_nCallState = EB_CALL_STATE_HANGUP;
		}else
		{
			// 只是退出会话，超过一定时间后，再完全退出
			pCallInfo->m_nCallState = EB_CALL_STATE_EXIT;
		}
		return true;
	}
	return false;
}
bool CUserManagerApp::GetCallUserList(mycp::bigint sCallId, std::vector<tstring> & pOutUserList) const
{
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
		CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
		for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
		{
			const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
			pOutUserList.push_back(pAccountInfo->GetAccount());
		}
		return true;
	}
	return false;
}
bool CUserManagerApp::GetCallUserList(mycp::bigint sCallId, std::vector<mycp::bigint> & pOutUserList) const
{
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
		CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
		for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
		{
			const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
			pOutUserList.push_back(pAccountInfo->GetUserId());
		}
		return true;
	}
	return false;
}
bool CUserManagerApp::GetCallUserList(mycp::bigint sCallId, std::vector<EB_AccountInfo> & pOutUserList) const
{
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
		CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
		for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
		{
			const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
			pOutUserList.push_back(EB_AccountInfo(pAccountInfo.get()));
		}
		return true;
	}
	return false;
}
int CUserManagerApp::GetCallOnlineSize(mycp::bigint sCallId) const
{
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		int result = 0;
		BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
		CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
		for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
		{
			const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
			if (pAccountInfo->IsOnLineState())
				result++;
		}
		return result;
	}
	return -1;
}

CEBCallInfo::pointer CUserManagerApp::GetCallInfo(mycp::bigint sCallId) const
{
	CEBCallInfo::pointer pCallInfo;
	theCallInfoList.find(sCallId, pCallInfo);
	return pCallInfo;
}
CEBAccountInfo::pointer CUserManagerApp::GetCallInfo(mycp::bigint sCallId, mycp::bigint nFromUserId) const
{
	CEBAccountInfo::pointer result;
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		//if (sFromAccount == pCallInfo->GetFromAccount()->GetAccount())
		//	result = pCallInfo->GetFromAccount();
		//else
			pCallInfo->m_pCallUserList.find(nFromUserId, result);
	}
	return result;
}


//CEBECardInfo::pointer CUserManagerApp::GetMyECardInfo(const tstring& sEmpCode, mycp::bigint sDepCode) const
//{
//	CEBECardInfo::pointer pPopECardInfo = CEBECardInfo::create();
//
//	if (!sDepCode.empty())
//	{
//		CEBGroupInfo::pointer pDepartmentInfo;
//		if (!theDepartmentList.find(sDepCode, pDepartmentInfo))
//		{
//			return EB_Null_ECardInfo;
//		}
//
//		CEBMemberInfo::pointer pMyEmployeeInfo;
//		if (pDepartmentInfo->m_pMemberList.find(m_pMyAccountInfo->GetAccount(),pMyEmployeeInfo))
//		{
//			pPopECardInfo->m_nAccountType = EB_ACCOUNT_TYPE_OUT_ENT;
//			pMyEmployeeInfo->GetECardInfo(pPopECardInfo.get());
//			if (m_pDefaultEnterpriseInfo.get() != NULL && pDepartmentInfo->m_sEnterpriseCode==m_pDefaultEnterpriseInfo->m_sEnterpriseCode)
//				pPopECardInfo->m_sEnterprise = m_pDefaultEnterpriseInfo->m_sEnterpriseName;
//			pPopECardInfo->m_sGroupName = pDepartmentInfo->m_sGroupName;
//		}
//		if (!sEmpCode.empty())
//		{
//			boost::mutex::scoped_lock lock(const_cast<boost::mutex&>(pDepartmentInfo->m_pMemberList.mutex()));
//			CLockMap<tstring, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
//			for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
//			{
//				CEBMemberInfo::pointer pEmployeeInfo = pIter->second;
//				if (pEmployeeInfo->m_sMemberCode == sEmpCode)
//				{
//					pPopECardInfo->m_nAccountType = EB_ACCOUNT_TYPE_IN_ENT;
//					break;
//				}
//			}
//		}
//		return pPopECardInfo;
//	}
//	if (!sEmpCode.empty())
//	{
//		CEBMemberInfo::pointer pEmployeeInfo;
//		if (theEmployeeList.find(sEmpCode, pEmployeeInfo))
//		{
//			CEBGroupInfo::pointer pDepartmentInfo;
//			if (!theDepartmentList.find(pEmployeeInfo->m_sGroupCode, pDepartmentInfo))
//			{
//				return EB_Null_ECardInfo;
//			}
//			CEBMemberInfo::pointer pMyEmployeeInfo;
//			if (pDepartmentInfo->m_pMemberList.find(m_pMyAccountInfo->GetAccount(),pMyEmployeeInfo))
//			{
//				// 同群组（部门、项目...）
//				pPopECardInfo->m_nAccountType = EB_ACCOUNT_TYPE_IN_ENT;
//				pMyEmployeeInfo->GetECardInfo(pPopECardInfo.get());
//				if (m_pDefaultEnterpriseInfo.get())
//					pPopECardInfo->m_sEnterprise = m_pDefaultEnterpriseInfo->m_sEnterpriseName;
//				pPopECardInfo->m_sGroupName = pDepartmentInfo->m_sGroupName;
//			}else if (!pDepartmentInfo->m_sEnterpriseCode.empty()&&theEnterpriseList.exist(pDepartmentInfo->m_sEnterpriseCode)&&m_pDefaultEntEmployeeInfo.get()!=NULL)
//			{
//				// 同企业不同部门
//				if (!theDepartmentList.find(m_pDefaultEntEmployeeInfo->m_sGroupCode, pDepartmentInfo))
//				{
//					return EB_Null_ECardInfo;
//				}
//				pPopECardInfo->m_nAccountType = EB_ACCOUNT_TYPE_IN_ENT;
//				m_pDefaultEntEmployeeInfo->GetECardInfo(pPopECardInfo.get());
//				pPopECardInfo->m_sGroupName = pDepartmentInfo->m_sGroupName;
//				if (m_pDefaultEnterpriseInfo.get())
//					pPopECardInfo->m_sEnterprise = m_pDefaultEnterpriseInfo->m_sEnterpriseName;
//			}
//		}
//	}
//
//	if (sEmpCode.empty() || pPopECardInfo->m_nAccountType == EB_ACCOUNT_TYPE_VISITOR)
//	{
//		CEBMemberInfo::pointer pMyDefaultEmployeeInfo;
//		if (!m_pMyAccountInfo->GetDefaultEmp().empty())
//			theEmployeeList.find(m_pMyAccountInfo->GetDefaultEmp(),pMyDefaultEmployeeInfo);		// 不是企业成员，取默认群组名片
//		if (pMyDefaultEmployeeInfo.get()==NULL)
//			pMyDefaultEmployeeInfo = m_pDefaultEntEmployeeInfo;
//		if (pMyDefaultEmployeeInfo.get() != NULL)
//		{
//			// 对对方来讲，我是外部企业员工（或群组成员）
//			pPopECardInfo->m_nAccountType = EB_ACCOUNT_TYPE_OUT_ENT;
//			pMyDefaultEmployeeInfo->GetECardInfo(pPopECardInfo.get());
//			CEBGroupInfo::pointer pDepartmentInfo;
//			if (theDepartmentList.find(pMyDefaultEmployeeInfo->m_sGroupCode, pDepartmentInfo))
//			{
//				pPopECardInfo->m_sGroupName = pDepartmentInfo->m_sGroupName;
//			}
//			if (m_pDefaultEnterpriseInfo.get())
//				pPopECardInfo->m_sEnterprise = m_pDefaultEnterpriseInfo->m_sEnterpriseName;
//		}else if (m_pMyAccountInfo->IsLogonVisitor())
//		{
//			// 我是游客
//			pPopECardInfo->m_nAccountType = EB_ACCOUNT_TYPE_VISITOR;
//			pPopECardInfo->m_sName = m_pMyAccountInfo->GetUserName();
//		}else
//		{
//			// 我是注册用户
//			pPopECardInfo->m_nAccountType = EB_ACCOUNT_TYPE_USER;
//			pPopECardInfo->m_sName = m_pMyAccountInfo->GetUserName();
//			//pPopECardInfo->m_sUserName = m_pMyAccountInfo->GetDescription();
//		}
//	}
//	return pPopECardInfo;
//}

//tstring CUserManagerApp::GetECardFromInfo(const CEBECardInfo::pointer& pECardInfo) const
//{
//	if (pECardInfo.get() == NULL) return "";
//	char lpszBuffer[1024];
//	sprintf(lpszBuffer, "t=%d;ec=%s;na=%s;ph=%s;tel=%s;em=%s;ti=%s;de=%s;en=%s;",pECardInfo->m_nAccountType,pECardInfo->m_sMemberCode.c_str(),
//		pECardInfo->m_sName.c_str(),pECardInfo->m_sPhone.c_str(),pECardInfo->m_sTel.c_str(),pECardInfo->m_sEmail.c_str(),
//		pECardInfo->m_sTitle.c_str(),pECardInfo->m_sGroupName.c_str(),pECardInfo->m_sEnterprise.c_str());
//	return (tstring)lpszBuffer;
//}
//tstring CUserManagerApp::GetDepFromInfo(EB_ACCOUNT_TYPE nAccountType,const CEBGroupInfo::pointer& pDepartmentInfo) const
//{
//	if (pDepartmentInfo.get() == NULL) return "";
//	CEBMemberInfo::pointer pEmployeeInfo;
//	if (!pDepartmentInfo->m_pMemberList.find(m_pMyAccountInfo->GetAccount(), pEmployeeInfo))
//	{
//		return "";
//	}
//	char lpszBuffer[1024];
//	sprintf(lpszBuffer, "t=%d;ec=%s;na=%s;ph=%s;tel=%s;em=%s;ti=%s;de=%s;",nAccountType,pEmployeeInfo->m_sMemberCode.c_str(),
//		pEmployeeInfo->m_sUserName.c_str(),pEmployeeInfo->m_sCellPhone.c_str(),pEmployeeInfo->m_sWorkPhone.c_str(),pEmployeeInfo->m_sEmail.c_str(),
//		pEmployeeInfo->m_sJobTitle.c_str(),pDepartmentInfo->m_sGroupName.c_str());
//	tstring sFromInfo(lpszBuffer);
//	if (m_pDefaultEnterpriseInfo.get() != NULL)
//	{
//		sprintf(lpszBuffer, "en=%s;",m_pDefaultEnterpriseInfo->m_sEnterpriseName.c_str());
//		sFromInfo.append(lpszBuffer);
//	}
//	return sFromInfo;
//}

//EB_ACCOUNT_TYPE CUserManagerApp::GetAccountTypeFromAccount(const tstring& sFromAccount) const
//{
//	boost::mutex::scoped_lock lock(const_cast<boost::mutex&>(theDepartmentList.mutex()));
//	CLockMap<tstring, CEBGroupInfo::pointer>::const_iterator pIter = theDepartmentList.begin();
//	for (; pIter!=theDepartmentList.end(); pIter++)
//	{
//		CEBGroupInfo::pointer pDepartmentInfo = pIter->second;
//		if ((pDepartmentInfo->m_nGroupType == EB_GROUP_TYPE_DEPARTMENT || pDepartmentInfo->m_nGroupType == EB_GROUP_TYPE_PROJECT)
//			&& pDepartmentInfo->m_pMemberList.exist(sFromAccount))
//		{
//			// 本企业部门或项目组成员，属于内部员工
//			return EB_ACCOUNT_TYPE_IN_ENT;
//		}
//	}
//	if (m_pDefaultEnterpriseInfo.get() != NULL)
//		return EB_ACCOUNT_TYPE_OUT_ENT;	// 外部企业员工
//	else
//		return EB_ACCOUNT_TYPE_USER;
//}
inline const tstring & replace(tstring & strSource, const tstring & strFind, const tstring &strReplace)
{
	std::string::size_type pos=0;
	std::string::size_type findlen=strFind.size();
	std::string::size_type replacelen=strReplace.size();
	while ((pos=strSource.find(strFind, pos)) != std::string::npos)
	{
		strSource.replace(pos, findlen, strReplace);
		pos += replacelen;
	}
	return strSource;
}
#define USES_REPLACE_EQUAL_SIGN
bool CUserManagerApp::GetECardByFromInfo(const tstring& sFromInfo,EB_ECardInfo* pOutECardInfo)
{
	bool result = false;
	std::vector<tstring> pList;
	ParseString(sFromInfo,";",false,pList);
	for (size_t i=0; i<pList.size(); i++)
	{
		const std::string sOut = pList[i];
		const std::string::size_type nFind = sOut.find("=");
		if (nFind==std::string::npos) continue;
		const std::string sKey = sOut.substr(0,nFind);
		const std::string sValue = sOut.substr(nFind+1);
		if (sKey=="t")
		{
			result = true;
			pOutECardInfo->m_nAccountType = (EB_ACCOUNT_TYPE)atoi(sValue.c_str());
		}
		else if (sKey=="uid")
		{
			pOutECardInfo->m_nMemberUserId = cgc_atoi64(sValue.c_str());
		}
		else if (sKey=="ec")
		{
			pOutECardInfo->m_sMemberCode = cgc_atoi64(sValue.c_str());
		}
		else if (sKey=="na")
		{
			pOutECardInfo->m_sName = sValue;
#ifdef USES_REPLACE_EQUAL_SIGN
			replace(pOutECardInfo->m_sName,"%3d","=");
#endif
			replace(pOutECardInfo->m_sName,"%3b",";");
			if (pOutECardInfo->m_sAccountName.empty())
				pOutECardInfo->m_sAccountName = pOutECardInfo->m_sName;	// ??? 兼容旧版本服务端
		}
		else if (sKey=="ph")
		{
			pOutECardInfo->m_sPhone = sValue;
#ifdef USES_REPLACE_EQUAL_SIGN
			replace(pOutECardInfo->m_sPhone,"%3d","=");
#endif
			replace(pOutECardInfo->m_sPhone,"%3b",";");
		}
		else if (sKey=="tel")
		{
			pOutECardInfo->m_sTel = sValue;
#ifdef USES_REPLACE_EQUAL_SIGN
			replace(pOutECardInfo->m_sTel,"%3d","=");
#endif
			replace(pOutECardInfo->m_sTel,"%3b",";");
		}
		else if (sKey=="em")
		{
			pOutECardInfo->m_sEmail = sValue;
#ifdef USES_REPLACE_EQUAL_SIGN
			replace(pOutECardInfo->m_sEmail,"%3d","=");
#endif
			replace(pOutECardInfo->m_sEmail,"%3b",";");
		}
		else if (sKey=="ti")
		{
			pOutECardInfo->m_sTitle = sValue;
#ifdef USES_REPLACE_EQUAL_SIGN
			replace(pOutECardInfo->m_sTitle,"%3d","=");
#endif
			replace(pOutECardInfo->m_sTitle,"%3b",";");
		}
		else if (sKey=="de")
		{
			pOutECardInfo->m_sGroupName = sValue;
#ifdef USES_REPLACE_EQUAL_SIGN
			replace(pOutECardInfo->m_sGroupName,"%3d","=");
#endif
			replace(pOutECardInfo->m_sGroupName,"%3b",";");
		}
		else if (sKey=="en")
		{
			pOutECardInfo->m_sEnterprise = sValue;
#ifdef USES_REPLACE_EQUAL_SIGN
			replace(pOutECardInfo->m_sEnterprise,"%3d","=");
#endif
			replace(pOutECardInfo->m_sEnterprise,"%3b",";");
		}
		else if (sKey=="ad")
		{
			pOutECardInfo->m_sAddress = sValue;
#ifdef USES_REPLACE_EQUAL_SIGN
			replace(pOutECardInfo->m_sAddress,"%3d","=");
#endif
			replace(pOutECardInfo->m_sAddress,"%3b",";");
		}
		else if (sKey=="usid")
		{
			pOutECardInfo->m_nUserSignId = cgc_atoi64(sValue.c_str());
		}
		else if (sKey=="hid")
		{
			pOutECardInfo->m_nHeadResId = cgc_atoi64(sValue.c_str());
		}
		else if (sKey=="hcms")
		{
			pOutECardInfo->m_sHeadCmServer = sValue;
		}
		else if (sKey=="hhs")
		{
			pOutECardInfo->m_sHeadHttpServer = sValue;
		}
		else if (sKey=="hmd5")
		{
			pOutECardInfo->m_sHeadMd5 = sValue;
		}
		else if (sKey=="acn")
		{
			pOutECardInfo->m_sAccountName = sValue;
#ifdef USES_REPLACE_EQUAL_SIGN
			replace(pOutECardInfo->m_sAccountName,"%3d","=");
#endif
			replace(pOutECardInfo->m_sAccountName,"%3b",";");
		}
	}
	return result;

	//tstring sOut;
	//if (!SplitString(sFromInfo, "t=", ";", sOut)) return false;
	//pOutECardInfo->m_nAccountType = (EB_ACCOUNT_TYPE)atoi(sOut.c_str());
	//pOutECardInfo->m_sMemberCode = SplitBigIntValue(sFromInfo, "ec=", ";",0);
	//SplitString(sFromInfo, "na=", ";", pOutECardInfo->m_sName);
	//replace(pOutECardInfo->m_sName,"%3d","=");
	//replace(pOutECardInfo->m_sName,"%3b",";");
	//SplitString(sFromInfo, "ph=", ";", pOutECardInfo->m_sPhone);
	//replace(pOutECardInfo->m_sPhone,"%3d","=");
	//replace(pOutECardInfo->m_sPhone,"%3b",";");
	//SplitString(sFromInfo, "tel=", ";", pOutECardInfo->m_sTel);
	//replace(pOutECardInfo->m_sTel,"%3d","=");
	//replace(pOutECardInfo->m_sTel,"%3b",";");
	//SplitString(sFromInfo, "em=", ";", pOutECardInfo->m_sEmail);
	//replace(pOutECardInfo->m_sEmail,"%3d","=");
	//replace(pOutECardInfo->m_sEmail,"%3b",";");
	//SplitString(sFromInfo, "ti=", ";", pOutECardInfo->m_sTitle);
	//replace(pOutECardInfo->m_sTitle,"%3d","=");
	//replace(pOutECardInfo->m_sTitle,"%3b",";");
	//SplitString(sFromInfo, "de=", ";", pOutECardInfo->m_sGroupName);
	//replace(pOutECardInfo->m_sGroupName,"%3d","=");
	//replace(pOutECardInfo->m_sGroupName,"%3b",";");
	//SplitString(sFromInfo, "en=", ";", pOutECardInfo->m_sEnterprise);
	//replace(pOutECardInfo->m_sEnterprise,"%3d","=");
	//replace(pOutECardInfo->m_sEnterprise,"%3b",";");
	//SplitString(sFromInfo, "ad=", ";", pOutECardInfo->m_sAddress);
	//replace(pOutECardInfo->m_sAddress,"%3d","=");
	//replace(pOutECardInfo->m_sAddress,"%3b",";");
	//pOutECardInfo->m_nUserSignId = SplitBigIntValue(sFromInfo, "usid=", ";",0);
	//pOutECardInfo->m_nHeadResId = SplitBigIntValue(sFromInfo, "hid=", ";",0);
	//if (pOutECardInfo->m_nHeadResId>0)
	//{
	//	SplitString(sFromInfo, "hcms=", ";", pOutECardInfo->m_sHeadCmServer);
	//	SplitString(sFromInfo, "hhs=", ";", pOutECardInfo->m_sHeadHttpServer);
	//	SplitString(sFromInfo, "hmd5=", ";", pOutECardInfo->m_sHeadMd5);
	//}
	//SplitString(sFromInfo, "acn=", ";", pOutECardInfo->m_sAccountName);
	//if (pOutECardInfo->m_sAccountName.empty())
	//{
	//	pOutECardInfo->m_sAccountName = pOutECardInfo->m_sName;	// ??? 兼容旧版本服务端
	//}else
	//{
	//	replace(pOutECardInfo->m_sAccountName,"%3d","=");
	//	replace(pOutECardInfo->m_sAccountName,"%3b",";");
	//}
	//return true;
}
//bool CUserManagerApp::GetAccountInfoByFromInfo(const tstring& sFromInfo, CEBAccountInfo::pointer & pAccountInfo) const
//{
//	pAccountInfo->m_pFromCardInfo = GetECardByFromInfo(sFromInfo);
//	return pAccountInfo->m_pFromCardInfo.get()!=NULL?true:false;
//}

CEBCallInfo::pointer CUserManagerApp::GetCallInfoByUser(mycp::bigint sAccount, tstring& pOutAccount, EB_USER_LINE_STATE& pOutLineState)
{
	BoostReadLock rdlock(theCallInfoList.mutex());
	CLockMap<eb::bigint, CEBCallInfo::pointer>::const_iterator pIter = theCallInfoList.begin();
	for (;pIter!=theCallInfoList.end(); pIter++)
	{
		const CEBCallInfo::pointer& pCallInfo = pIter->second;
		if (pCallInfo->m_sGroupCode>0) continue;	// 群会话不算
		if (IsInCallUser(sAccount, pCallInfo,pOutAccount,pOutLineState))
			return pCallInfo;
	}
	return NullCallInfo;
}
CEBCallInfo::pointer CUserManagerApp::GetCallInfoByUser(const tstring& sAccount, mycp::bigint& pOutUserId, EB_USER_LINE_STATE& pOutLineState)
{
	BoostReadLock lock(theCallInfoList.mutex());
	CLockMap<eb::bigint, CEBCallInfo::pointer>::const_iterator pIter = theCallInfoList.begin();
	for (;pIter!=theCallInfoList.end(); pIter++)
	{
		const CEBCallInfo::pointer& pCallInfo = pIter->second;
		if (pCallInfo->m_sGroupCode>0) continue;	// 群会话不算
		if (IsInCallUser(sAccount, pCallInfo,pOutUserId,pOutLineState))
			return pCallInfo;
	}
	return NullCallInfo;
}
bool CUserManagerApp::IsInCallUser(mycp::bigint sAccount, const CEBCallInfo::pointer & pCallInfo, tstring& pOutAccount,EB_USER_LINE_STATE& pOutLineState) const
{
	BOOST_ASSERT (pCallInfo.get() != 0);
	//if (pCallInfo->GetFromAccount()->GetAccount() == sAccount)
	//	return true;
	BoostReadLock rdlock(pCallInfo->m_pCallUserList.mutex());
	CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
	for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
	{
		const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
		if (pAccountInfo->GetUserId() == sAccount)
		{
			pOutAccount = pAccountInfo->GetAccount();
			pOutLineState = pAccountInfo->GetLineState();
			return true;
		}
	}
	return false;
}
bool CUserManagerApp::IsInCallUser(const tstring& sAccount, const CEBCallInfo::pointer & pCallInfo, mycp::bigint& pOutUserId,EB_USER_LINE_STATE& pOutLineState) const
{
	BOOST_ASSERT (pCallInfo.get() != 0);
	//if (pCallInfo->GetFromAccount()->GetAccount() == sAccount)
	//	return true;
	BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
	CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
	for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
	{
		const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
		if (pAccountInfo->GetAccount() == sAccount)
		{
			pOutUserId = pAccountInfo->GetUserId();
			pOutLineState = pAccountInfo->GetLineState();
			return true;
		}
	}
	return false;
}

int CUserManagerApp::UGInfoEit(mycp::bigint nUGId, const tstring& sGroupName)
{
	if (sGroupName.empty()) return -1;
	if (m_pUserManager.get() == NULL) return -1;

	CEBUGInfo::pointer pEditUGInfo = CEBUGInfo::create(nUGId,sGroupName);
	bool bNewUGInfo = false;
	if (pEditUGInfo->m_nUGId==0)
	{
		bNewUGInfo = true;
		pEditUGInfo->m_nUGId = this->GetNextBigId();
	}else if (!theUGInfoList.exist(pEditUGInfo->m_nUGId))
	{
		return -1;
	}
	theEitUGInfoList.insert(pEditUGInfo->m_nUGId, pEditUGInfo);

	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ug-id", pEditUGInfo->m_nUGId));
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("new-uginfo", bNewUGInfo));
	return m_pUserManager->SendUGEdit(nUGId,sGroupName,pRequestInfo);
}
int CUserManagerApp::UGInfoDelete(mycp::bigint nUGId)
{
	if (nUGId == 0) return -1;
	if (m_pUserManager.get() == NULL) return -1;
	if (!theUGInfoList.exist(nUGId))
	{
		return 1;
	}
	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("ug-id", nUGId));
    return m_pUserManager->SendUGDelete(nUGId, pRequestInfo);
}

bool CUserManagerApp::HasUGContact(eb::bigint nUGId) const
{
    const bool bAuthContact = (m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT;	// 需要验证好友
    AUTO_CONST_RLOCK(theContactList1);
    CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList1.begin();
    for (; pIterContact!=theContactList1.end(); pIterContact++) {
        const CEBContactInfo::pointer &pContactInfo = pIterContact->second;
        if (pContactInfo->m_nUGId==nUGId) {
            return true;
        }
    }
    return false;
}
int CUserManagerApp::GetUGContactSize(eb::bigint nUGId, int& pOutContactSize, int& pOutOnlineSize) const
{
	const bool bAuthContact = (m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT;	// 需要验证好友

    AUTO_CONST_RLOCK(theContactList1);
	CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList1.begin();
    for (; pIterContact!=theContactList1.end(); pIterContact++) {
        const CEBContactInfo::pointer &pContactInfo = pIterContact->second;
        if (pContactInfo->m_nUGId==nUGId) {
			pOutContactSize++;
            if (bAuthContact && pContactInfo->m_nLineState>=EB_LINE_STATE_BUSY)	{
                /// 需要验证好友
				pOutOnlineSize++;
			}
		}
	}
    return 0;
}

int CUserManagerApp::GetUGContactList(eb::bigint nUGId, std::vector<EB_ContactInfo> &pOutUGContactList) const
{
    AUTO_CONST_RLOCK(theContactList1);
    CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList1.begin();
    for (; pIterContact!=theContactList1.end(); pIterContact++) {
        const CEBContactInfo::pointer &pContactInfo = pIterContact->second;
        if (pContactInfo->m_nUGId==nUGId) {
            pOutUGContactList.push_back(EB_ContactInfo(pContactInfo.get()));
        }
    }
    return (int)pOutUGContactList.size();
}

int CUserManagerApp::ContactEdit(const EB_ContactInfo* pPopContactEditInfo)
{
	if (pPopContactEditInfo == 0) return -1;
	if (m_pUserManager.get() == NULL) return -1;

	//if ((m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)
	//{
	//	// 好友验证模式
	//}
	//if (pPopContactEditInfo->m_nContactId==0 && m_nSystemSetting

	CEBContactInfo::pointer pEditContactInfo = CEBContactInfo::create(pPopContactEditInfo);
	bool bNewContact = false;
	if (pEditContactInfo->m_nContactId==0)
	{
		if (pPopContactEditInfo->m_nContactUserId>0)
		{
			EB_ContactInfo pContactInfoTemp;
			if (GetContactInfo(pPopContactEditInfo->m_nContactUserId,&pContactInfoTemp))
			{
				//if (pContactInfoTemp.m_nContactType==2 ||									// 已经是通过验证好友
				if ((pContactInfoTemp.m_nContactType&EB_CONTACT_TYPE_AUTH)==EB_CONTACT_TYPE_AUTH ||									// 已经是通过验证好友
					(m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==0)	// 不需要验证好友
				{
					// 已经是好友
					if (m_callback)
						m_callback->onContactInfo(&pContactInfoTemp);
					if (m_pHwnd != NULL) {
#ifdef _QT_MAKE_
						EB_ContactInfo * pEvent = new EB_ContactInfo(&pContactInfoTemp);
						pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_INFO);
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::SendMessage(m_pHwnd, EB_WM_CONTACT_INFO, (WPARAM)&pContactInfoTemp, 0);
#endif
					}
					return 1;
				}
			}
		}else if (pPopContactEditInfo->m_nContactUserId==0 && pPopContactEditInfo->m_sContact.empty())
		{
			return 1;
			//if ((m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)!=0)	// 需要验证好友
			//{
			//	return 1;
			//}
		}
		bNewContact = true;
		pEditContactInfo->m_nContactId = this->GetNextBigId();
	}else if (!theContactList1.exist(pEditContactInfo->m_nContactId))
	{
		return -1;
	}
	theEditContactList.remove(pEditContactInfo->m_nContactId);
	theEditContactList.insert(pEditContactInfo->m_nContactId, pEditContactInfo);

	//CEBContactInfo::pointer pPopContactInfo;
	//if (!theContactList.find(pEditContactInfo->m_nContactId, pPopContactInfo))
	//{
	//	bNewContact = true;
	//	theContactList.insert(pEditContactInfo->m_sContact, pEditContactInfo);
	//}else
	//{
	//	theEditContactList.remove(pEditContactInfo->m_sContact);
	//	theEditContactList.insert(pEditContactInfo->m_sContact, pEditContactInfo);
	//}
	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("con-id", pEditContactInfo->m_nContactId));
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("new-contact", bNewContact));
	//return m_pUserManager->SendUMCMEdit(pEditContactInfo.get(), pRequestInfo);
	CPopParameterList pParameter;
	pParameter.SetParameter(CGC_PARAMETER("con_uid",pPopContactEditInfo->m_nContactUserId));
	pParameter.SetParameter(CGC_PARAMETER("description",pPopContactEditInfo->m_sDescription));
	if (!bNewContact ||
		(m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==0)	// 不验证联系人，也可以随便添加
	{
		const bool bSetForce = !bNewContact;	// 新建不需要强制添加，修改时需要强制添加
		pParameter.SetParameter(CGC_PARAMETER("con_id",pPopContactEditInfo->m_nContactId));
		pParameter.SetParameter(CGC_PARAMETER("ugid",pPopContactEditInfo->m_nUGId));
		pParameter.SetParameter(CGC_PARAMETER("name",pPopContactEditInfo->m_sName),bSetForce);
		pParameter.SetParameter(CGC_PARAMETER("phone",pPopContactEditInfo->m_sPhone),bSetForce);
		pParameter.SetParameter(CGC_PARAMETER("email",pPopContactEditInfo->m_sEmail),bSetForce);
		pParameter.SetParameter(CGC_PARAMETER("address",pPopContactEditInfo->m_sAddress),bSetForce);
		pParameter.SetParameter(CGC_PARAMETER("company",pPopContactEditInfo->m_sCompany),bSetForce);
		pParameter.SetParameter(CGC_PARAMETER("job_title",pPopContactEditInfo->m_sJobTitle),bSetForce);
		pParameter.SetParameter(CGC_PARAMETER("tel",pPopContactEditInfo->m_sTel),bSetForce);
		pParameter.SetParameter(CGC_PARAMETER("fax",pPopContactEditInfo->m_sFax),bSetForce);
		pParameter.SetParameter(CGC_PARAMETER("url",pPopContactEditInfo->m_sUrl),bSetForce);
	}
	return m_pUserManager->SendUMCMEdit(0,pParameter,pRequestInfo);
}
int CUserManagerApp::ContactDelete(mycp::bigint nContactId,bool bDeleteDest)
{
	if (nContactId == 0) return -1;
	if (m_pUserManager.get() == NULL) return -1;
	if (!theContactList1.exist(nContactId))
	{
		return 1;
	}
	if (bDeleteDest && (m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==0)
		bDeleteDest = false;	// **普通联系人模式，不通删除对方；
	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("con-id", nContactId));
	return m_pUserManager->SendUMCMDelete(nContactId,0,bDeleteDest,pRequestInfo);
}
int CUserManagerApp::ContactQuery(mycp::bigint nOnlyContactId,mycp::bigint nOnlyContactUserId,int nLoadFlag,bool bAcceptContact)
{
	if (m_pUserManager.get() == NULL) return -1;
	if (nOnlyContactId==0 && nOnlyContactUserId==0 && theUGInfoList.empty())
		return m_pUserManager->SendUGLoad();
	else
	{
		if (bAcceptContact)
		{
			CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
			pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("accept-contact", bAcceptContact));
			return m_pUserManager->SendUMCMQuery(nOnlyContactId,nOnlyContactUserId,0,pRequestInfo);
		}else
		{
			return m_pUserManager->SendUMCMQuery(nOnlyContactId,nOnlyContactUserId,nLoadFlag);
		}
	}
}

int CUserManagerApp::EntEdit(const EB_EnterpriseInfo* pEnterpriseInfo)
{
	if (m_pUserManager.get() == NULL)
	{
		return -1;
	}
	if (pEnterpriseInfo == NULL) return -1;
	m_pEditEnterpriseInfo = CEBEnterpriseInfo::create();
	m_pEditEnterpriseInfo->operator =(pEnterpriseInfo);
	return m_pUserManager->SendUMEMEntEdit(m_pEditEnterpriseInfo.get());
}
int CUserManagerApp::DepEdit(const EB_GroupInfo* pDepartmentEditInfo)
{
	if (pDepartmentEditInfo==NULL)
	{
		return -1;
	}
	if (m_pUserManager.get() == NULL) return -1;
	if (pDepartmentEditInfo->m_sParentCode>0 && !theDepartmentList.exist(pDepartmentEditInfo->m_sParentCode))
	{
		return 1;
	}
	mycp::bigint sEditDepCode = pDepartmentEditInfo->m_sGroupCode;
	mycp::bigint nGroupCreateUserId = 0;
	mycp::bigint nMyEmpId = 0;
	int nEmpCount = 0;
	mycp::bigint nGroupVer = 0;
	if (sEditDepCode==0)
	{
		sEditDepCode = this->GetNextBigId();
		//static unsigned short static_group_code_index = 0;
		//char lpszBuffer[24];
		//sprintf(lpszBuffer, "%d%d%d",rand()%1000,(int)time(0),(++static_group_code_index)%1000);
		//sEditDepCode = cgc_atoi64(lpszBuffer);
		nGroupCreateUserId = this->m_pMyAccountInfo->GetUserId();
	}else
	{
		CEBGroupInfo::pointer pTempGroupInfo;
		if (!theDepartmentList.find(sEditDepCode,pTempGroupInfo))
		{
			return -1;
		}
		nGroupCreateUserId = pTempGroupInfo->m_nCreateUserId;
		nMyEmpId = pTempGroupInfo->m_nMyEmpId;
		nEmpCount = pTempGroupInfo->m_nEmpCount;
		nGroupVer = pTempGroupInfo->m_nGroupVer;
		theEditDepartmentList.remove(sEditDepCode);
	}
	CEBGroupInfo::pointer pEditGroupInfo = CEBGroupInfo::create();
	pEditGroupInfo->operator = (pDepartmentEditInfo);
	pEditGroupInfo->m_nCreateUserId = nGroupCreateUserId;
	pEditGroupInfo->m_nMyEmpId = nMyEmpId;
	pEditGroupInfo->m_nEmpCount = nEmpCount;
	pEditGroupInfo->m_nGroupVer = nGroupVer;
	theEditDepartmentList.insert(sEditDepCode, pEditGroupInfo);
	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("edit_dep_code", sEditDepCode));
	return m_pUserManager->SendUMEMDepEdit(0,pEditGroupInfo.get(), pRequestInfo);
}

int CUserManagerApp::SetGroupForbid(mycp::bigint nGroupId, bool bForbidSpeech, int nForbidMinutes)
{
	// ** nForbidMinutes
	// -1: 解决禁言 0:永久禁言 >0 保留用于以后实现按分钟禁言；
	CPopParameterList pParameter;
	pParameter.SetParameter(CGC_PARAMETER("dep_code",nGroupId));
	if (bForbidSpeech)
	{
		pParameter.SetParameter(CGC_PARAMETER("forbid_minutes",nForbidMinutes));
	}
	else
	{
		nForbidMinutes = -1;
		pParameter.SetParameter(CGC_PARAMETER("forbid_minutes",nForbidMinutes));
	}

	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("edit_dep_code", nGroupId));
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("edit_forbid_minutes", nForbidMinutes));
	return m_pUserManager->SendUMEMDepEdit(0,pParameter, pRequestInfo);
}

int CUserManagerApp::DepDelete(mycp::bigint sDepCode)
{
	if (m_pUserManager.get() == NULL) return -1;
	CEBGroupInfo::pointer pDepartmentInfo;
	if (!theDepartmentList.find(sDepCode, pDepartmentInfo))
	{
		// not exist error
		return -1;
	}

	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("dep_code", sDepCode));
	return m_pUserManager->SendUMEMDepDelete(0,sDepCode, pRequestInfo);
}
int CUserManagerApp::JoinGroup(mycp::bigint nGroupId,const tstring& sDescription)
{
	if (m_pUserManager.get() == NULL) return -1;
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(nGroupId, pDepartmentInfo))
	{
		// not exist error
		if (pDepartmentInfo->m_pMemberList.exist(m_pMyAccountInfo->GetUserId()))
		{
			return 1;
		}
	}
	// 申请加入群组；
	EB_MemberInfo pMemberInfo;
	pMemberInfo.m_sGroupCode = nGroupId;
	pMemberInfo.m_sDescription = sDescription;
	return m_pUserManager->SendUMEMEmpEdit(&pMemberInfo,0,0);
}
int CUserManagerApp::DepExit(mycp::bigint sDepCode)
{
	if (m_pUserManager.get() == NULL) return -1;
	CEBGroupInfo::pointer pDepartmentInfo;
	if (!theDepartmentList.find(sDepCode, pDepartmentInfo))
	{
		// not exist error
		return -1;
	}
	CEBMemberInfo::pointer pEmployeeInfo;
	if (!pDepartmentInfo->m_pMemberList.find(m_pMyAccountInfo->GetUserId(), pEmployeeInfo))
	{
		return -1;
	}
//    CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
//    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("dep_code", pEmployeeInfo->m_sGroupCode));
//    //return m_pUserManager->SendUMEMEmpDelete(0,pEmployeeInfo->m_sGroupCode,lpszBuffer,0,pRequestInfo);
//    return m_pUserManager->SendUMEMEmpDelete(pEmployeeInfo->m_sMemberCode,0,"",0,pRequestInfo);
    return m_pUserManager->SendUMEMEmpDelete(pEmployeeInfo->m_sMemberCode,0,"");
}
bool CUserManagerApp::HasSubDepartment(eb::bigint sParentCode) const
{
	AUTO_CONST_RLOCK(theDepartmentList);
	CLockMap<eb::bigint, CEBGroupInfo::pointer>::const_iterator pIter = theDepartmentList.begin();
	for (; pIter!=theDepartmentList.end(); pIter++)
	{
		const CEBGroupInfo::pointer& pDepartmentInfo = pIter->second;
		if (pDepartmentInfo->m_sParentCode == sParentCode)
		{
			return true;
		}
	}
	return false;
}
int CUserManagerApp::EmpEdit(const CEBMemberInfo* pEmployeeInfo,int nForbidMinutes,int nNeedEmpInfo)
{
	if (pEmployeeInfo == NULL)
	{
		return -1;
	}
	if (m_pUserManager.get() == NULL)
	{
		return -1;
	}
	CEBGroupInfo::pointer pDepartmentInfo;
	if (!theDepartmentList.find(pEmployeeInfo->m_sGroupCode, pDepartmentInfo))
	{
		// not exist department error
		return -1;
	}
	CEBMemberInfo::pointer pEditMemberInfo = CEBMemberInfo::create();
	pEditMemberInfo->operator =(pEmployeeInfo);

	if (!m_sAccountPrefix.empty() && !IsFullNumber(pEditMemberInfo->m_sMemberAccount.c_str(),pEditMemberInfo->m_sMemberAccount.size()) && !pEditMemberInfo->m_sMemberAccount.empty() && pEditMemberInfo->m_sMemberAccount.find(m_sAccountPrefix)==std::string::npos)	// "@"非邮箱才处理
	{
		pEditMemberInfo->m_sMemberAccount = m_sAccountPrefix+pEditMemberInfo->m_sMemberAccount;
	}else if (m_sAccountPrefix.empty())
	{
		//const std::string::size_type find = pEditMemberInfo->m_sMemberAccount.find("@");
		//if (find!=std::string::npos)
		//{
		//	// 邮箱
		//}else if (pEditMemberInfo->m_sMemberAccount.size()==11 && pEditMemberInfo->m_sMemberAccount.substr(0,1)=="1" && cgc_atoi64(pEditMemberInfo->m_sMemberAccount.c_str())>10000000000)
		//{
		//	// 手机
		//}else
		//{
		//	// 格式错误
		//	return -2;
		//}
	}
	const bool bNewEmployeeInfo = pEditMemberInfo->m_sMemberCode==0?true:false;
	mycp::bigint nTempMemberUserId = 0;
	if (bNewEmployeeInfo)
	{
		static unsigned short theIndex = 0;
		nTempMemberUserId = (time(0)*100)+(theIndex%100)+rand();
		pDepartmentInfo->m_pMemberList.insert(nTempMemberUserId, pEditMemberInfo);
	}else
	{
		theEditEmployeeList.remove(pEditMemberInfo->m_nMemberUserId);
		theEditEmployeeList.insert(pEditMemberInfo->m_nMemberUserId, pEditMemberInfo);
	}
	//bool bNewEmployeeInfo = false;
	//if (!pDepartmentInfo->m_pMemberList.exist(pEditMemberInfo->m_nMemberUserId))
	//{
	//	bNewEmployeeInfo = true;
	//	pDepartmentInfo->m_pMemberList.insert(pEditMemberInfo->m_nMemberUserId, pEditMemberInfo);
	//}else
	//{
	//	theEditEmployeeList.remove(pEditMemberInfo->m_nMemberUserId);
	//	theEditEmployeeList.insert(pEditMemberInfo->m_nMemberUserId, pEditMemberInfo);
	//}
	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("edit_dep_code", pEditMemberInfo->m_sGroupCode));
	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("emp_uid", pEditMemberInfo->m_nMemberUserId));
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("new_employee", bNewEmployeeInfo));
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("new_temp_member_uid", nTempMemberUserId));
	//pEditMemberInfo->m_sMemberCode = 0;
	return m_pUserManager->SendUMEMEmpEdit(pEditMemberInfo.get(),nForbidMinutes,nNeedEmpInfo,"",0,0,0,"",pRequestInfo);
}

int CUserManagerApp::SetMemberForbidSpeech(mycp::bigint nGroupId, mycp::bigint nMemberUserId,int nForbidMinutes)
{
	// -1: 解除禁言 0:永久禁言 >0 保留用于以后实现按分钟禁言；
	CPopParameterList pParameter;
	pParameter.SetParameter(CGC_PARAMETER("dep_code",nGroupId));
	pParameter.SetParameter(CGC_PARAMETER("emp_uid",nMemberUserId));
	pParameter.SetParameter(CGC_PARAMETER("forbid_minutes",nForbidMinutes));

	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("edit_dep_code", nGroupId));
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("edit_forbid_minutes", nForbidMinutes));
	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("new_employee", false));
	//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("new_temp_member_uid", nTempMemberUserId));
	return m_pUserManager->SendUMEMEmpEdit(0, pParameter, pRequestInfo);
}

int CUserManagerApp::EmpDelete(mycp::bigint sEmpCode, bool bDeleteAccount)
{
	if (m_pUserManager.get() == NULL) return -1;
	CEBMemberInfo::pointer pEmployeeInfo;
	if (!theEmployeeList.find(sEmpCode, pEmployeeInfo))
	{
		// not exist error
		return -1;
	}
	CEBGroupInfo::pointer pDepartmentInfo;
	if (!theDepartmentList.find(pEmployeeInfo->m_sGroupCode, pDepartmentInfo))
	{
		// error
		return -1;
	}else if (!pDepartmentInfo->m_pMemberList.exist(pEmployeeInfo->m_nMemberUserId))
	{
		// error
		// == EDT_DELETE_EMP不需要判断，否则会死锁
		return -1;
	}
	CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("dep_code", pEmployeeInfo->m_sGroupCode));
	//char lpszBuffer[24];
	//sprintf(lpszBuffer,"%lld",pEmployeeInfo->m_nMemberUserId);
	//return m_pUserManager->SendUMEMEmpDelete(0,pEmployeeInfo->m_sGroupCode,lpszBuffer,0,pRequestInfo);
    return m_pUserManager->SendUMEMEmpDelete(pEmployeeInfo->m_sMemberCode,0,"",0,pRequestInfo);
}

int CUserManagerApp::EmpDelete(bigint groupId, bigint userId, bool bDeleteAccount)
{
    if (m_pUserManager.get() == NULL) return -1;

    CEBGroupInfo::pointer pDepartmentInfo;
    if (!theDepartmentList.find(groupId, pDepartmentInfo))
    {
        // error
        return -1;
    }else if (!pDepartmentInfo->m_pMemberList.exist(userId))
    {
        // error
        // == EDT_DELETE_EMP不需要判断，否则会死锁
        return -1;
    }
    CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0, EB_REQ_TYPE_UNKNOWN);
    pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("dep_code", groupId));
    char lpszBuffer[24];
    sprintf(lpszBuffer,"%lld", userId);
    return m_pUserManager->SendUMEMEmpDelete(0,groupId,lpszBuffer,0,pRequestInfo);
//    return m_pUserManager->SendUMEMEmpDelete(pEmployeeInfo->m_sMemberCode,0,"",0,pRequestInfo);
}

int CUserManagerApp::EnterpriseLoad(mycp::bigint sDepCode,
									int nLoadEntDep,
									int nLoadMyGroup,
									int nLoadEmp,
									mycp::bigint nMemberCode,
									mycp::bigint nMemberUid,
									bool bLoadImage,
									const tstring& sSearchKey,
									bool bLoadFromEBC,
									const CPOPSotpRequestInfo::pointer& pRequestInfo)
{
	if (m_pUserManager.get() == NULL) return -1;
	if (m_userStatus != US_Logoned) return -2;
	if (sDepCode>0 && nLoadEmp==1 && (nMemberCode==0 || nMemberUid==0))
	{
		CEBGroupInfo::pointer pDepartmentInfo;
		if (theDepartmentList.find(sDepCode, pDepartmentInfo))
		{
			// *本地已经存在
			if (pDepartmentInfo->m_nServerGroupVer==pDepartmentInfo->m_nGroupVer && (size_t)pDepartmentInfo->m_nEmpCount<=pDepartmentInfo->m_pMemberList.size())
			{
				if (pDepartmentInfo->m_nEmpCount>0 && (pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER_STATE)==0)
				{
					pDepartmentInfo->m_nGroupData |= EB_GROUP_DATA_LOAD_MEMBER_STATE;
					if (pDepartmentInfo->m_nMyEmpId==0)
					{
						theNeedLoadOLSList.add(pDepartmentInfo->m_sGroupCode);
					}else
					{
						theNeedLoadOLSList.pushfront(pDepartmentInfo->m_sGroupCode);
					}
				}
				if (bLoadFromEBC)
				{
					AUTO_RLOCK(pDepartmentInfo->m_pMemberList);
					CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
					for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
					{
						const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
						ProcessMemberInfo(pDepartmentInfo,pEmployeeInfo);
					}
				}
				//const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
				//if (m_callback)
				//	m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
				//if (m_pHwnd!=NULL)
				//	::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)pDepartmentInfo.get(),bIsMyDepartment?1:0);
				return 0;
			}
			if ((pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER)==EB_GROUP_DATA_LOAD_MEMBER &&
				pDepartmentInfo->m_pMemberList.size()==pDepartmentInfo->m_nEmpCount)
			{
				return 1;
			}
			pDepartmentInfo->m_nGroupData |= EB_GROUP_DATA_LOAD_MEMBER;
		}
	}
	if (sDepCode>0 && (nMemberCode==0 || nMemberUid==0) && !theNeedLoadGroupMemberList.empty())
	{
		BoostWriteLock lock(theNeedLoadGroupMemberList.mutex());
		CLockList<mycp::bigint>::iterator pIter = theNeedLoadGroupMemberList.begin();
		for (; pIter!=theNeedLoadGroupMemberList.end(); pIter++)
		{
			const mycp::bigint nGroupId = *pIter;
			if (nGroupId==sDepCode)
			{
				theNeedLoadGroupMemberList.erase(pIter);
				break;
			}
		}
	}
	if (bLoadImage && m_bExistLocalImage)
	{
		bLoadImage = false;
	}
	//if (sDepCode==0 && (nLoadEntDep==1 || nLoadEmp==1))
	//{
	//	int i=0;
	//}
	//if (nLoadEmp==1)
	//{
	//	int i=0;
	//}
	return m_pUserManager->SendUMEMLoad(sDepCode,nLoadEntDep,nLoadMyGroup,nLoadEmp,bLoadImage,nMemberCode,nMemberUid,sSearchKey,pRequestInfo);
}

bool CUserManagerApp::DeleteOnlineFileList(eb::bigint sCallId,eb::bigint nMsgId)
{
	CToSendList::pointer pOnlineFileToSendList;
	if (!theOnlineFileList.find(sCallId,pOnlineFileToSendList))
	{
		return false;
	}
	BoostWriteLock lock(pOnlineFileToSendList->m_list.mutex());
	CLockList<CToSendInfo::pointer>::iterator pIter;
	for (pIter=pOnlineFileToSendList->m_list.begin(); pIter!=pOnlineFileToSendList->m_list.end(); pIter++)
	{
		const CToSendInfo::pointer& pCallToSendInfo = *pIter;
		if (pCallToSendInfo->m_nMsgId==nMsgId)
		{
			pOnlineFileToSendList->m_list.erase(pIter);
			if (pOnlineFileToSendList->m_list.empty())
				theOnlineFileList.remove(sCallId);
			return true;
		}
	}
	return false;
}
bool CUserManagerApp::DeleteP2PFileList(eb::bigint sCallId,eb::bigint nMsgId)
{
	CToSendList::pointer pOnlineFileToSendList;
	if (!theP2PFileList.find(sCallId,pOnlineFileToSendList))
	{
		return false;
	}
	BoostWriteLock lock(pOnlineFileToSendList->m_list.mutex());
	CLockList<CToSendInfo::pointer>::iterator pIter;
	for (pIter=pOnlineFileToSendList->m_list.begin(); pIter!=pOnlineFileToSendList->m_list.end(); pIter++)
	{
		const CToSendInfo::pointer& pCallToSendInfo = *pIter;
		if (pCallToSendInfo->m_nMsgId==nMsgId)
		{
			pOnlineFileToSendList->m_list.erase(pIter);
			if (pOnlineFileToSendList->m_list.empty())
				theP2PFileList.remove(sCallId);
			return true;
		}
	}
	return false;
}

void CUserManagerApp::OnProcessTimer(const CPOPCUserManager* pUMOwner)
{
	mycp::bigint sCallId = 0;
	if (m_pP2POkList.front(sCallId))	// ** while 改成 if，处理慢一些，预计发送文件，第一次太快不能处理
	{
		// **
		LogMessage("OnProcessTimer->m_pP2POkList.front TRUE... (callid=%lld)\r\n",sCallId);
		m_pP2PTaskInfo.remove(sCallId);
		ProcessToSendList(sCallId,false,false);
		//ProcessToSendList(sCallId,false);

		// 打通P2P，检查在线文件，重新发送；
		CToSendList::pointer pOnlineFileToSendList;
		if (theOnlineFileList.find(sCallId,pOnlineFileToSendList))
		{
			LogMessage("OnProcessTimer->theOnlineFileList.find TRUE... (callid=%lld)\r\n",sCallId);
			std::vector<CToSendInfo::pointer> pResendFileList;
			{
				// **找到前面在线发送文件；
				BoostReadLock lock(pOnlineFileToSendList->m_list.mutex());
				CLockList<CToSendInfo::pointer>::iterator pIter;
				for (pIter=pOnlineFileToSendList->m_list.begin(); pIter!=pOnlineFileToSendList->m_list.end(); pIter++)
				{
					const CToSendInfo::pointer& pCallToSendInfo = *pIter;
					if (!pCallToSendInfo->m_bOffFile && pCallToSendInfo->m_nMsgId>0)
					{
						pResendFileList.push_back(pCallToSendInfo);
					}
				}
			}
			// ** 重新发送；
			//CEBCallInfo::pointer pCallInfo;
			//theCallInfoList.find(sCallId,pCallInfo);
			for (size_t i=0;i<pResendFileList.size(); i++)
			{
				CToSendInfo::pointer pCallToSendInfo = pResendFileList[i];
				if (pCallToSendInfo->m_nMsgId>0)
				{
					theChangeP2PMsgList.insert(pCallToSendInfo->m_nMsgId,true,false);
#ifdef USES_RESEND_FILE_2
					theREsendIdList.insert(pCallToSendInfo->m_nMsgId,pCallToSendInfo,false);
					// ** 改用线程发送；
					CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RESEND_FILE);
					pProcessMsgInfo->m_tProcessTime = time(0)+2;	// 012，等待三秒
					pProcessMsgInfo->m_nBigInt1 = pCallToSendInfo->m_nMsgId;
					pProcessMsgInfo->m_nBigInt2 = sCallId;
					m_pProcessMsgList.add(pProcessMsgInfo);
#endif
					// 先取消文件
					this->CancelCrFile(sCallId,pCallToSendInfo->m_nMsgId);
#ifdef USES_RESEND_FILE_2
					continue;
#else
					Sleep(1000);	// 800
#endif
				}
				//// 在线文件，需要重新发送；
				//if (pCallInfo.get()!=NULL && pCallInfo->m_pChatRoom.get()!=NULL)
				//{
				//	// 先取消在线文件；（对方未接收）
				//	pCallInfo->m_pChatRoom->CancelSendingFile(pCallToSendInfo->m_nMsgId);
				//}
				//DeleteOnlineFileList(sCallId,pCallToSendInfo->m_nMsgId);
				// 重新发送
                SendCrFile(sCallId,pCallToSendInfo->m_sFilePath,pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile);
			}
		}
	}
	const time_t tNow = time(0);
	if ((this->m_pMyAccountInfo->GetOnlineTime()+6)<tNow && m_nMyDepInfoSize>m_nReturnMyGroupInfoSize)
	{
		this->m_pMyAccountInfo->SetOnlineTime(tNow);
		EnterpriseLoad(0,0,1,0);	// 加载所有我的个人群组
		m_nMyDepInfoSize = -2;
		return;
	}
	//if ((this->m_pMyAccountInfo->GetOnlineTime()+6)<tNow && m_nEntDepInfoSize>m_nReturnEntGroupInfoSize)
	//{
	//	this->m_pMyAccountInfo->SetOnlineTime(tNow);
	//	EnterpriseLoad(0,1,0,0);	// 加载所有我的企业
	//	m_nEntDepInfoSize = -2;
	//	return;
	//}

	{
		// 检查P2P穿透任务；
		BoostWriteLock lock(m_pP2PTaskInfo.mutex());
		CLockMap<mycp::bigint,CP2PTaskInfo::pointer>::iterator pIter = m_pP2PTaskInfo.begin();
		for (; pIter!=m_pP2PTaskInfo.end();pIter++)
		{
			const CP2PTaskInfo::pointer pP2PTaskInfo = pIter->second;
			//const CP2PTaskInfo::pointer& pP2PTaskInfo = pIter->second;
			const mycp::bigint sCallId = pP2PTaskInfo->m_pP2PInfo.GetCallId();
			bool bP2POk = false;
			if (pP2PTaskInfo->m_bP2PResponse && !DoP2PTask(pP2PTaskInfo->m_pP2PInfo,bP2POk))
			{
				// 打通成功，或其他失败
				m_pP2PTaskInfo.erase(pIter);
				lock.unlock();
				ProcessToSendList(sCallId,false,false);
				//ProcessToSendList(sCallId,false);
				break;
			}
			if (pP2PTaskInfo->m_tP2PTry>0 && (tNow-pP2PTaskInfo->m_tP2PTry)>=10)
			{
				// 超过8秒，当超时处理；

				// 清空P2P连接资源
				CEBCallInfo::pointer pCallInfo;
				if (theCallInfoList.find(sCallId, pCallInfo))
				{
					pCallInfo->m_nCallData &= ~EB_CALL_DATA_P2P_OK;	// ?

					CEBAccountInfo::pointer pFromAccount;
					if (pCallInfo->m_pCallUserList.find(pP2PTaskInfo->m_pP2PInfo.m_sFromAccount, pFromAccount))
					{
						//MessageBox(NULL,"OnProcessTimer","",MB_OK);
						pFromAccount->m_bLocalP2POk = false;
						{
							BoostWriteLock wtLock(pFromAccount->m_mutexLocalChatRoom);
							if (pFromAccount->m_pLocalChatRoom.get()!=NULL)
							{
								m_pClearChatRoom.add(pFromAccount->m_pLocalChatRoom);
								pFromAccount->m_pLocalChatRoom.reset();
							}
						}
						pFromAccount->m_bRemoteP2POk = false;
						{
							BoostWriteLock wtLock(pFromAccount->m_mutexRemoteChatRoom);
							if (pFromAccount->m_pRemoteChatRoom.get()!=NULL)
							{
								m_pClearChatRoom.add(pFromAccount->m_pRemoteChatRoom);
								pFromAccount->m_pRemoteChatRoom.reset();
							}
						}
					}
				}
				m_pP2PTaskInfo.erase(pIter);
				lock.unlock();
				ProcessToSendList(sCallId,false,false);
				//ProcessToSendList(sCallId,false);

				// 检查在线文件，重新发送；
				if (pCallInfo.get()!=NULL && pCallInfo->m_pChatRoom.get()!=NULL)
				{
					CToSendList::pointer pOnlineFileToSendList;
					if (theOnlineFileList.find(sCallId,pOnlineFileToSendList))
					{
						//LogMessage("OnProcessTimer->theOnlineFileList.find TRUE... (callid=%lld)\r\n",sCallId);
						//std::vector<CToSendInfo::pointer> pResendFileList;
						{
							// **找到前面在线发送文件；
							BoostReadLock lock(pOnlineFileToSendList->m_list.mutex());
							CLockList<CToSendInfo::pointer>::iterator pIter;
							for (pIter=pOnlineFileToSendList->m_list.begin(); pIter!=pOnlineFileToSendList->m_list.end(); pIter++)
							{
								const CToSendInfo::pointer& pCallToSendInfo = *pIter;
								if (!pCallToSendInfo->m_bOffFile && pCallToSendInfo->m_nMsgId>0)
								{
#ifdef _QT_MAKE_
                                    LogMessage("OnProcessTimer->resend file... (msgid=%lld,%s)\r\n",pCallToSendInfo->m_nMsgId,pCallToSendInfo->m_sFilePath.toStdString().c_str());
#else
                                    LogMessage("OnProcessTimer->resend file... (msgid=%lld,%s)\r\n",pCallToSendInfo->m_nMsgId,pCallToSendInfo->m_sFilePath.c_str());
#endif
									pCallInfo->m_pChatRoom->ResendMsg(pCallToSendInfo->m_nMsgId);
									//pResendFileList.push_back(pCallToSendInfo);
								}
							}
						}
						//// ** 重新发送；
						//CEBCallInfo::pointer pCallInfo;
						//theCallInfoList.find(sCallId,pCallInfo);
						//for (size_t i=0;i<pResendFileList.size(); i++)
						//{
						//	CToSendInfo::pointer pCallToSendInfo = pResendFileList[i];
						//	// 发送多一次
						//	pCallInfo->m_pChatRoom->SendFile(pCallToSendInfo->m_sFilePath.c_str(),pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile);
						//	//SendCrFile(sCallId,pCallToSendInfo->m_sFilePath.c_str(),pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile,true);
						//}
					}
				}
				break;
			}
		}
	}
	// ?检查超过太长时间，没用会话
	static time_t theCheckTime = 0;
	if (theCheckTime == 0 || (tNow-theCheckTime)>30)	// 30S检查一次
	{
		theCheckTime = tNow;
		BoostReadLock lockCallInfoList(theCallInfoList.mutex());
		CLockMap<eb::bigint, CEBCallInfo::pointer>::const_iterator pIter = theCallInfoList.begin();
		for (;pIter!=theCallInfoList.end(); pIter++)
		{
			const CEBCallInfo::pointer& pCallInfo = pIter->second;
			if (pCallInfo->m_tLastTime==0 || (pCallInfo->m_nCallState != EB_CALL_STATE_EXIT && pCallInfo->m_nCallState != EB_CALL_STATE_HANGUP)) continue;
			//if ((tNow-pCallInfo->m_tLastTime)>20)			// 测试20秒
			if ((tNow-pCallInfo->m_tLastTime)>5*60)			// 5*60
			{
				const mycp::bigint nCallId = pCallInfo->GetCallId();
				lockCallInfoList.unlock();
				ClearCallInfo(nCallId,true,false);
				break;
			}
		}
	}
	// 超过30秒没有用，自动关闭LC
	static CPOPCLogonCenter::pointer theLastLogonCenter;
	if (m_userStatus != US_OAuthForward &&
		m_pLogonCenter.get()!=NULL && (tNow-m_tLogonCenter>30))
	{
		theLastLogonCenter = m_pLogonCenter;
		m_pLogonCenter.reset();
	}else if (theLastLogonCenter.get()!=0)
	{
		theLastLogonCenter->Stop();
		theLastLogonCenter.reset();
	}
	//if (m_tReLogonTime > 0 && m_tReLogonTime<tNow)
	//{
	//	if (m_sDevAppOnlineKey.empty())
	//	{
	//		SetDevAppId(m_sDevAppId,m_sDevAppKey,true);	// 登录前，需要重新验证appkey
	//		m_tReLogonTime = tNow+20;				// 30S后继续处理；（**开发者ID验证成功返回，也会立即申请重新登录；）
	//	}else
	//	{
	//		m_tReLogonTime = tNow+30;				// 30S后会重新登录；（如果登录成功，会设为0；）
	//		ReLogon();
	//	}
	//}
	
	static int theLoadOLSCount = 0;
	mycp::bigint nLoadGroupId = 0;
	if (m_nLoadSubFunc==1 && m_tReLoadInfo>0 && m_tReLoadInfo<tNow)
	{
		LoadInfo(m_nLoadSubFunc,0,0);		// 先加载集成应用
		m_nLoadSubFunc = 0;
		m_tReLoadInfo = tNow+5;				// 5秒后再加载离线消息
		return;
	}
	if (m_pHwnd==NULL)
	{
		return;
	}else
	{
		if (!m_pLogonToSendContactLineState.empty())
		{
			BoostWriteLock lock(m_pLogonToSendContactLineState.mutex());
			CLockList<CEBContactInfo::pointer>::iterator pIter = m_pLogonToSendContactLineState.begin();
			for (; pIter!=m_pLogonToSendContactLineState.end(); pIter++) {
				const CEBContactInfo::pointer& pContactInfo = *pIter;
#ifdef _QT_MAKE_
				EB_ContactInfo * pEvent = new EB_ContactInfo(pContactInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_STATE_CHANGE);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_CONTACT_STATE_CHANGE, (WPARAM)(const EB_ContactInfo*)pContactInfo.get(), 0);
#endif
			}
			m_pLogonToSendContactLineState.clear(false);
		}
		if (!m_pLogonToSendMemberLineState.empty())
		{
			BoostWriteLock lock(m_pLogonToSendMemberLineState.mutex());
			CLockList<CEBMemberInfo::pointer>::iterator pIter = m_pLogonToSendMemberLineState.begin();
			for (; pIter!=m_pLogonToSendMemberLineState.end(); pIter++) {
				const CEBMemberInfo::pointer& pEmployeeInfo = *pIter;
				const bool nIsMyMember = (bool)(m_pMyAccountInfo->GetUserId() == pEmployeeInfo->m_nMemberUserId);
#ifdef _QT_MAKE_
				EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_USER_STATE_CHANGE);
				pEvent->SetEventParameter((long)nIsMyMember?1:0);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_USER_STATE_CHANGE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), nIsMyMember?1:0);
#endif
			}
			m_pLogonToSendMemberLineState.clear(false);
		}
	}
	if (theNeedLoadGroupMemberList.front(nLoadGroupId))
	{
		int nIndex = 0;
		do
		{
			// 加载我的部门成员，一次加载5个；
			theCheckGroupMemberList.insert(nLoadGroupId,time(0),true);
			this->EnterpriseLoad(nLoadGroupId,0,0,1);
		}while((nIndex++)<2 && theNeedLoadGroupMemberList.front(nLoadGroupId));
	}else if (theNeedLoadGroupInfoList.front(nLoadGroupId))
	{
		int nIndex = 0;
		do
		{
			// 加载部门信息，一次加载15个；
			theCheckGroupInfoList.insert(nLoadGroupId,true,false);
			this->EnterpriseLoad(nLoadGroupId,1,1,0);
		}while((nIndex++)<15 && theNeedLoadGroupInfoList.front(nLoadGroupId));
	}else if (theLoadOLSCount<6 && theNeedLoadOLSList.front(nLoadGroupId))
	{
		// 加载在线用户状态
		theLoadOLSCount++;	// **通过这个，限制，最多一次加载N个群组部门在线状态，可以留一次机会给后面（如加载离线）
		int nIndex = 0;
		do
		{
			this->LoadInfo(0,0,0,nLoadGroupId);
		}while ((nIndex++)<5 && theNeedLoadOLSList.front(nLoadGroupId));
	}else if (m_tReLoadInfo>0 && m_tReLoadInfo<tNow)
	{
		theLoadOLSCount = 0;
		LoadInfo(0,1,0);
		m_tReLoadInfo = tNow+120+(rand()%30);	// 2分钟加载一次
	}else if (theLoadOLSCount>0)
	{
		theLoadOLSCount = 0;
	}

	static unsigned int theIndex = 0;
	theIndex++;

	if ((theIndex%(10*3600))==9*3600)	// 10小时重新验证一次；
	{
		m_pProcessMsgList.add(CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RESET_DEVAPPID));
		//SetDevAppId(m_sDevAppId,m_sDevAppKey,true);
	}

	if ((theIndex%5)==4)
	{
		if (theNeedLoadGroupInfoList.empty() && !theCheckGroupInfoList.empty())
		{
			int nIndex = 0;
			BoostReadLock rdLock(theCheckGroupInfoList.mutex());
			CLockMap<mycp::bigint,bool>::iterator pIter = theCheckGroupInfoList.begin();
			for (; pIter!=theCheckGroupInfoList.end(); pIter++)
			{
				nLoadGroupId = pIter->first;
				//time_t tGroupMemberDataTime = 0;
				//if (theGroupMemberDataTime.find(nLoadGroupId,tGroupMemberDataTime) && tGroupMemberDataTime+3>tNow)
				//{
				//	continue;
				//}
				this->EnterpriseLoad(nLoadGroupId,1,1,0);
				if ((nIndex++)>=3)
				{
					break;
				}
			}
		}
		if (theNeedLoadGroupMemberList.empty() && !theCheckGroupMemberList.empty())
		{
			int nIndex = 0;
			BoostReadLock rdLock(theCheckGroupMemberList.mutex());
			CLockMap<mycp::bigint,time_t>::iterator pIter = theCheckGroupMemberList.begin();
			for (; pIter!=theCheckGroupMemberList.end(); pIter++)
			{
				nLoadGroupId = pIter->first;
				const time_t tSendTime = pIter->second;
				if (tSendTime+4>tNow)
					continue;
				time_t tGroupMemberDataTime = 0;
				if (theGroupMemberDataTime.find(nLoadGroupId,tGroupMemberDataTime) && (tGroupMemberDataTime+4)>tNow)
				{
					continue;
				}
				this->EnterpriseLoad(nLoadGroupId,0,0,1);
				if ((nIndex++)>=3)
				{
					break;
				}
			}
		}

		static Cchatroom::pointer theLastClearChatRoom;
		Cchatroom::pointer pChatRoom;
		if (m_pClearChatRoom.front(pChatRoom))
		{
			// * 无效P2P，预防 OnP2POk 返回
			// * 无效CM，预防 OnInvalidateSession 返回
			pChatRoom->SetParam(2);
			theLastClearChatRoom = pChatRoom;
			//pChatRoom->Stop();	// ***不能stop
		}else if (theLastClearChatRoom.get()!=NULL)
		{
			theLastClearChatRoom.reset();
		}

		static CPOPCUserManager::pointer theLastHangupUm;
		CPOPCUserManager::pointer pHangupUm;
		if (m_pHangupUm.front(pHangupUm))
		{
			theLastHangupUm = pHangupUm;
			//pHangupUm->Stop();
		}else if (theLastHangupUm.get()!=NULL)
		{
			theLastHangupUm.reset();
		}

		if (m_pAppCenterTemp.get() != NULL)
		{
			m_pAppCenterTemp->Stop();
			m_pAppCenterTemp.reset();
		}
	}

}

tstring CUserManagerApp::GetNextLCAddress(void)
{
	if (m_pOrgServerList.size()>=2)
	{
		if ((m_nOrgServerIndex+1)<(int)m_pOrgServerList.size())
		{
			//m_tOrgBeginOfStart = time(0);
			m_nOrgServerIndex++;
			tstring result = m_pOrgServerList[m_nOrgServerIndex];
			if (result.rfind(":")==std::string::npos)
				result.append(":18012");
			return result;
		}else// if (m_nOrgServerIndex>=1 && m_pOrgServerList.size()>=2)
			//}else if ((m_tOrgBeginOfStart==0 || (m_tOrgBeginOfStart+3*60)<time(0)) && m_nOrgServerIndex>=1 && m_pOrgServerList.size()>=2)
		{
			//m_tOrgBeginOfStart = time(0);
			m_nOrgServerIndex = 0;
			tstring result = m_pOrgServerList[m_nOrgServerIndex];
			if (result.rfind(":")==std::string::npos)
				result.append(":18012");
			return result;
		}
	}

	if (m_pLCList.empty())
	{
		int nCurentIndex = 0;
		time_t tLastTime = 0;
		const tstring sLCList = GetLCAddressList(nCurentIndex,tLastTime);
		ParseString(sLCList,";",true,m_pLCList);
		if (nCurentIndex>0)
		//if (tLastTime>0 && (tLastTime+1*3600)>time(0))	// 一小时内
		{
			if ((nCurentIndex+1)<=(int)sLCList.size())
			{
				m_nCurrentIndex = nCurentIndex;
				//m_tBeginOfStart = time(0);
			}
		}
	}
	if (m_nCurrentIndex == -1)
	{
		m_tBeginOfStart = time(0);
		m_nCurrentIndex = 0;
	}else if (m_nCurrentIndex>0 && !m_pLCList.empty() && (m_nCurrentIndex+1)>=(int)m_pLCList.size())
	//}else if (m_nCurrentIndex>0 && !m_pLCList.empty() && (m_tBeginOfStart+3*60)<time(0) && (m_nCurrentIndex+1)>=m_pLCList.size())	// 3分钟内，可以重新取一次；
	{
		m_nOrgServerIndex = 0;	//

		m_tBeginOfStart = time(0);
		m_nCurrentIndex = 0;
		SaveLCAddressIndex(this->m_nCurrentIndex);
		const tstring sLCAddress(m_pLCList[0]);
		return sLCAddress;
	}

	tstring sFindLCAddress;
	for (int i=m_nCurrentIndex;i<(int)m_pLCList.size();i++)
	{
		const tstring sLCAddress(m_pLCList[i]);
		if (sLCAddress==m_sServerAddress)
		{
			continue;
		}
		sFindLCAddress = sLCAddress;
		m_nCurrentIndex = i;
		break;
	}
	if (!sFindLCAddress.empty())
		SaveLCAddressIndex(this->m_nCurrentIndex);
	else if (!m_pOrgServerList.empty())
	{
		// 找不到
		m_nOrgServerIndex = 0;
		sFindLCAddress = m_pOrgServerList[m_nOrgServerIndex];
	}
	if (sFindLCAddress.rfind(":")==std::string::npos)
		sFindLCAddress.append(":18012");
	return sFindLCAddress;
}
void CUserManagerApp::ProcessTimeout(const CPOPSotpRequestInfo::pointer& pSotpRequestInfo)
{
	if (m_userStatus == US_DevAppIdLogoning)
	{
		if (pSotpRequestInfo.get()==NULL)
		{
			return;
		}
		const tstring sFindLCAddress(GetNextLCAddress());
#ifdef _QT_MAKE_
        if (sFindLCAddress.empty() || m_sServerAddress==sFindLCAddress) {
            static unsigned int theIndex = 0;
            if ((theIndex++)%10==0) {
                EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_APPID_ERROR);
                pEvent->SetEventParameter((long)EB_STATE_TIMEOUT_ERROR);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
            }
        }
#endif
		if (!sFindLCAddress.empty())
		{
            m_sServerAddress = sFindLCAddress;
			//LogMessage("ProcessTimeout... (sFindLCAddress=%s)\r\n",sFindLCAddress.c_str());

			if (m_sDevAppId==0)
			{
				m_sDevAppId  = pSotpRequestInfo->m_pRequestList.getParameterValue("app-id",(mycp::bigint)0);
				m_sDevAppKey = pSotpRequestInfo->m_pRequestList.getParameterValue("app-key");
			}
			m_pProcessMsgList.add(CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RESET_DEVAPPID));
		}
		return;
	}else if (m_userStatus == US_Logging)
	{
		//char lpszBuffer[20];
		//sprintf(lpszBuffer,"%d",m_nUmServerState);
		//MessageBox(NULL,lpszBuffer,"ProcessTimeout loggint",MB_OK);
		LogMessage("ProcessTimeout... (m_userStatus=%d,m_nUmServerState=%d)\r\n",(int)m_userStatus,(int)m_nUmServerState);
		if (m_nUmServerState!=EB_SS_INIT)
		{
			// 可能是系统重启，再登录一次；
			theDepartmentList.clear();
			m_sDevAppOnlineKey.clear();	// 清空app_online_key，登录前重新验证APP KEY
            m_tReLogonTime = time(0)+5+(((long)this)%10);
		}else
		{
			m_userStatus = US_LogonError;
			const EB_AccountInfo* pAccountInfo = m_pMyAccountInfo.get(); 
			if (m_callback)
				m_callback->onLogonTimeout(*pAccountInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
				pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_TIMEOUT);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_LOGON_TIMEOUT, (WPARAM)pAccountInfo, 0);
#endif
			}
		}
		return;
	}

	if (pSotpRequestInfo.get() != NULL)
	{
		if (pSotpRequestInfo->GetRequestType() == EB_REQ_TYPE_INVITE)
		{
			if (m_nUmServerState == EB_SS_STOPED)
			{

			}else
			{
				const mycp::bigint nGroupId = pSotpRequestInfo->m_pRequestList.getParameterValue("group-id",(mycp::bigint)0);
				const mycp::bigint nFromUserId = pSotpRequestInfo->m_pRequestList.getParameterValue("from_uid",(mycp::bigint)0);
				const tstring sFromAccount = pSotpRequestInfo->m_pRequestList.getParameterValue("from");
				if (nGroupId==0)
				{
					EB_CallInfo* pEbCallInfo = new EB_CallInfo(nFromUserId,sFromAccount,0,nGroupId);
					if (m_callback)
						m_callback->onCallError(*pEbCallInfo,EB_STATE_TIMEOUT_ERROR);
					if (m_pHwnd != NULL) {
#ifdef _QT_MAKE_
						pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
						pEbCallInfo->SetEventParameter((long)EB_STATE_TIMEOUT_ERROR);
                        QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
						::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, EB_STATE_TIMEOUT_ERROR);
						delete pEbCallInfo;
#endif
					}
					else {
						delete pEbCallInfo;
					}
				}
			}
		}else if (pSotpRequestInfo->GetRequestType() == EB_REQ_TYPE_LOGON)
		{
			//const tstring sFindLCAddress(GetNextLCAddress());
			//if (!sFindLCAddress.empty())
			//{
			//	m_sServerAddress = sFindLCAddress;
			//	m_sDevAppId  = pSotpRequestInfo->m_pRequestList.getParameterValue("app-id",(mycp::bigint)0);
			//	m_sDevAppKey = pSotpRequestInfo->m_pRequestList.getParameterValue("app-key");
			//	m_pProcessMsgList.add(PROCESS_MSG_TYPE_RESET_DEVAPPID);
			//}
			//return;

			// 这是online超时；
			LogMessage("ProcessTimeout->EB_REQ_TYPE_LOGON... (m_nUmServerState=%d)\r\n",(int)m_nUmServerState);
			if (m_nUmServerState!=EB_SS_INIT)
			{
				// 可能是系统重启，再登录一次；
                m_tReLogonTime = time(0)+5+(((long)this)%10);
			}else
			{
				const EB_AccountInfo* pAccountInfo = m_pMyAccountInfo.get(); 
				if (m_callback)
					m_callback->onLogonTimeout(*pAccountInfo);
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
					pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_TIMEOUT);
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
					::PostMessage(m_pHwnd, EB_WM_LOGON_TIMEOUT, (WPARAM)pAccountInfo, 0);
#endif
				}
			}
		}
	}
}
void CUserManagerApp::OnTimeout(const CPOPSotpRequestInfo::pointer& pSotpRequestInfo,const CPOPCUserManager* pUMOwner)
{
	ProcessTimeout(pSotpRequestInfo);
}
void CUserManagerApp::OnTimeout(const CPOPSotpRequestInfo::pointer& pSotpRequestInfo,const CPOPCLogonCenter* pLCOwner)
{
	ProcessTimeout(pSotpRequestInfo);
}
void CUserManagerApp::OnTimeout(const CPOPSotpRequestInfo::pointer& pSotpRequestInfo,const CEBCAppCenter* pAPOwner)
{
	ProcessTimeout(pSotpRequestInfo);
}

void CUserManagerApp::OnInvalidateSession(int nResultCode,const CPOPCUserManager* pUMOwner)
{
	//if (m_userStatus == US_Logoned)
	if (pUMOwner == this->m_pUserManager.get())
	{
		LogMessage("OnInvalidateSession UM ... (nResultCode=%d,m_nUmServerState=%d)\r\n",(int)nResultCode,(int)m_nUmServerState);
		// 只处理本帐号UM
		CEBCallbackInterface::SERVER_STATE nServerState = CEBCallbackInterface::SERVER_UNKNOWN;
		if (nResultCode == -102 && m_nUmServerState != EB_SS_MOVED)
		{
			m_nUmServerState = EB_SS_MOVED;
			nServerState = CEBCallbackInterface::SERVER_MOVED;
		}else if (nResultCode == -103 && m_nUmServerState != EB_SS_RESTART)
		{
			m_nUmServerState = EB_SS_RESTART;
			nServerState = CEBCallbackInterface::SERVER_RESTART;
			theUserManagerList.clear();
		}else if (nResultCode == -117 && m_nUmServerState != EB_SS_STOPED)
		{
			m_nUmServerState = EB_SS_STOPED;
			nServerState = CEBCallbackInterface::SERVER_STOPED;
			theUserManagerList.clear();
		}else if (nResultCode == -200 && m_nUmServerState != EB_SS_TIMEOUT)
		{
			m_nUmServerState = EB_SS_TIMEOUT;
			nServerState = CEBCallbackInterface::SERVER_TIMEOUT;
			theUserManagerList.clear();
		}else
		{
			if (nResultCode == -200)
			{
				// **第二次超时，检查ＵＭ，ＬＣ是否相同服务器，如果不同直接重新登录
				tstring sUMServer;
				tstring sLCServer;
				int nPort = 0;
				entboost::GetAddressPort(pUMOwner->GetAddress().address().c_str(),sUMServer,nPort);
				entboost::GetAddressPort(m_sServerAddress.c_str(),sLCServer,nPort);
				const bool bSameServer = entboost::GetHostIp(sUMServer.c_str(),sUMServer.c_str()) == entboost::GetHostIp(sLCServer.c_str(),sLCServer.c_str());
				LogMessage("OnInvalidateSession UM->bSameServer=%d... (nResultCode=%d)\r\n",(int)(bSameServer?1:0),(int)nResultCode);
				if (!bSameServer)
				{
					// ＵＭ跟ＬＣ不同服务器，直接重新登录；
					m_sDevAppOnlineKey.clear();	// 清空app_online_key，登录前重新验证APP KEY
                    m_tReLogonTime = time(0)+5+(((long)this)%10);
				}
			}
			return;
		}
		m_pLocalHostIp.clear();
		GetLocalIp();

		m_userStatus = US_Invalidate;
		{
			BoostWriteLock wtLock(m_mutexEBDatas);
			if (m_pEBDatas.get()!=NULL)
			{
				m_pEBDatas->close();
				m_pEBDatas.reset();
			}
		}
		theChatRoomList.clear();
		theResourceChatRoom.clear();
		theNeedLoadOLSList.clear();
		theNeedLoadGroupMemberList.clear();
		theCheckGroupMemberList.clear();
		theNeedLoadGroupInfoList.clear();
		theCheckGroupInfoList.clear();
		//theContactList.clear();
		//theResourceList.clear();
		//theEnterpriseList.clear();
		//m_pDefaultEntEmployeeInfo.reset();
		//m_pEditEnterpriseInfo.reset();
		//theDepartmentList.clear();
		//theEmployeeList.clear();
		//theMyEmployeeList.clear();
		// 设置所有成员离线状态
		{
			BoostReadLock lock(theEmployeeList.mutex());
			CLockMap<eb::bigint, CEBMemberInfo::pointer>::const_iterator pIter = theEmployeeList.begin();
			for (; pIter!=theEmployeeList.end(); pIter++)
			{
				const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
				pEmployeeInfo->m_nLineState = EB_LINE_STATE_UNKNOWN;	// 未知状态，需要呼叫
				pEmployeeInfo->m_sUmServer.clear();
			}
		}
		// 设置所有群组状态
		{
			AUTO_CONST_RLOCK(theDepartmentList);
			CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
			for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++)
			{
				CEBGroupInfo::pointer pEntDepartmentInfo = pIterDepartment->second;
				pEntDepartmentInfo->m_nGroupData &= ~EB_GROUP_DATA_LOAD_MEMBER;
				pEntDepartmentInfo->m_nGroupData &= ~EB_GROUP_DATA_LOAD_MEMBER_STATE;
				pEntDepartmentInfo->m_nOnlineSize = 0;
			}
		}

		// 设置连接会话离线状态
		while (!theCallInfoList.empty())
		{
			bool bEraseIter = false;
			BoostWriteLock lock(theCallInfoList.mutex());
			CLockMap<eb::bigint, CEBCallInfo::pointer>::const_iterator pIter = theCallInfoList.begin();
			for (;pIter!=theCallInfoList.end(); pIter++)
			{
				const CEBCallInfo::pointer& pCallInfo = pIter->second;
				if (pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE)
					continue;
				if (pCallInfo->m_nCallState != EB_CALL_STATE_EXIT)
					pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
				else
				{
					bEraseIter = true;
					theCallInfoList.erase(pIter);
					break;
				}
			}
			if (!bEraseIter)
			{
				break;
			}
		}
		
		tstring sUMServer;
		int nPort = 0;
		entboost::GetAddressPort(pUMOwner->GetAddress().address().c_str(),sUMServer,nPort);

		//std::vector<mycp::bigint> pRemoveList;
		//{
		//	BoostReadLock lock(theUserManagerList.mutex());
		//	CLockMap<mycp::bigint, CPOPCUserManager::pointer>::iterator pIter = theUserManagerList.begin();
		//	for (; pIter!=theUserManagerList.end(); pIter++)
		//	{
		//		const CPOPCUserManager::pointer pUmTemp = pIter->second;
		//		if (pUmTemp.get()==pUMOwner)
		//		{
		//			pRemoveList.push_back(pIter->first);
		//		}else
		//		{
		//			tstring sUMTempServer;
		//			entboost::GetAddressPort(pUmTemp->GetAddress().address().c_str(),sUMTempServer,nPort);
		//			const bool bSameUmServer = entboost::GetHostIp(sUMServer.c_str(),sUMServer.c_str()) == entboost::GetHostIp(sUMTempServer.c_str(),sUMTempServer.c_str());
		//			if (bSameUmServer)
		//			{
		//				pRemoveList.push_back(pIter->first);
		//			}
		//		}
		//	}
		//}
		//for (size_t i=0;i<pRemoveList.size();i++)
		//{
		//	theUserManagerList.remove(pRemoveList[i]);
		//}

		//char lpszBuffer[20];
		//sprintf(lpszBuffer,"%d",nResultCode);
		//MessageBox(NULL,lpszBuffer,"OnInvalidateSession",MB_OK);
		if (m_callback)
			m_callback->onServerChange(nServerState);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_SERVER_CHENGE);
//			pEvent->SetQEventType((QEvent::Type)EB_WM_SERVER_CHENGE);
			pEvent->SetEventParameter((long)nServerState);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::PostMessage(m_pHwnd, EB_WM_SERVER_CHENGE, (WPARAM)nServerState, 0);
#endif
		}
		//MessageBox(NULL,"msg after","OnInvalidateSession",MB_OK);
		LogMessage("OnInvalidateSession UM->nServerState=%d... (nResultCode=%d)\r\n",(int)nServerState,(int)nResultCode);
		if (nServerState == CEBCallbackInterface::SERVER_TIMEOUT)
		{
			tstring sLCServer;
			entboost::GetAddressPort(m_sServerAddress.c_str(),sLCServer,nPort);
			// 
			const bool bSameServer = entboost::GetHostIp(sUMServer.c_str(),sUMServer.c_str()) == entboost::GetHostIp(sLCServer.c_str(),sLCServer.c_str());
			LogMessage("OnInvalidateSession UM->SERVER_TIMEOUT bSameServer=%d ... (m_sServerAddress=%s,nResultCode=%d)\r\n",(int)(bSameServer?1:0),m_sServerAddress.c_str(),(int)nResultCode);
			if (bSameServer)
			{
				m_sLastErrorServerAddress = m_sServerAddress;
				const tstring sFindLCAddress(GetNextLCAddress());
				if (!sFindLCAddress.empty())
				{
					m_sServerAddress = sFindLCAddress;
					m_sDevAppOnlineKey.clear();	// 清空app_online_key，登录前重新验证APP KEY
                    m_tReLogonTime = time(0)+3+(((long)this)%10);	// 可以让当前服务完全退出，重新登录，分配到其他服务上；
					LogMessage("OnInvalidateSession UM->SERVER_TIMEOUT Set ReloginTime... (m_sLastErrorServerAddress=%s,m_sServerAddress=%s,nResultCode=%d)\r\n",m_sLastErrorServerAddress.c_str(),m_sServerAddress.c_str(),(int)nResultCode);
					return;
				}
			}
			m_tReLogonTime = time(0)+10;
			LogMessage("OnInvalidateSession UM->SERVER_TIMEOUT Set ReloginTime... (nResultCode=%d)\r\n",(int)nResultCode);
		}else if (nServerState == CEBCallbackInterface::SERVER_RESTART)
		{
			m_sDevAppOnlineKey.clear();						// 清空app_online_key，登录前重新验证APP KEY
            m_tReLogonTime = time(0)+10+(((long)this)%10);	// 可以让当前服务完全启动；
		}else
		{
			tstring sLCServer;
			int nPort = 0;
			entboost::GetAddressPort(m_sServerAddress.c_str(),sLCServer,nPort);
			const bool bSameServer = entboost::GetHostIp(sUMServer.c_str(),sUMServer.c_str()) == entboost::GetHostIp(sLCServer.c_str(),sLCServer.c_str());
			LogMessage("OnInvalidateSession UM->bSameServer=%d... (nResultCode=%d)\r\n",(int)(bSameServer?1:0),(int)nResultCode);
			if (!bSameServer)
			{
				// ＵＭ跟ＬＣ不同服务器，直接重新登录；
				m_sDevAppOnlineKey.clear();	// 清空app_online_key，登录前重新验证APP KEY
                m_tReLogonTime = time(0)+10+(((long)this)%20);	// 可以让当前服务完全退出，重新登录，分配到其他服务上；
			}else
			{
				m_sLastErrorServerAddress = m_sServerAddress;
				const tstring sFindLCAddress(GetNextLCAddress());
				LogMessage("OnInvalidateSession UM->sFindLCAddress=%s... (nResultCode=%d)\r\n",sFindLCAddress.c_str(),(int)nResultCode);
				if (!sFindLCAddress.empty())
				{
					m_sServerAddress = sFindLCAddress;
					m_sDevAppOnlineKey.clear();	// 清空app_online_key，登录前重新验证APP KEY
                    m_tReLogonTime = time(0)+3+(((long)this)%10);	// 可以让当前服务完全退出，重新登录，分配到其他服务上；
				}
			}
		}
	}
}

void CUserManagerApp::OnInvalidateSession(int nResultCode,const CPOPCLogonCenter* pLCOwner)
{
	if (pLCOwner == this->m_pLogonCenter.get())
	{
		// 只处理本帐号LC
		CEBCallbackInterface::SERVER_STATE nServerState = CEBCallbackInterface::SERVER_UNKNOWN;
		if (nResultCode == -102 && m_nLcServerState != EB_SS_MOVED)
		{
			m_nLcServerState = EB_SS_MOVED;
			nServerState = CEBCallbackInterface::SERVER_MOVED;
		}else if (nResultCode == -103 && m_nLcServerState != EB_SS_RESTART)
		{
			m_nLcServerState = EB_SS_RESTART;
			nServerState = CEBCallbackInterface::SERVER_RESTART;
		}else if (nResultCode == -117 && m_nLcServerState != EB_SS_STOPED)
		{
			m_nLcServerState = EB_SS_STOPED;
			nServerState = CEBCallbackInterface::SERVER_STOPED;
		}else if (nResultCode == -200 && m_nLcServerState != EB_SS_TIMEOUT)
		{
			m_nLcServerState = EB_SS_TIMEOUT;
			nServerState = CEBCallbackInterface::SERVER_TIMEOUT;
		}else
		{
			return;
		}
		// ** LC 不回调，由 UM 统一回调即可
		//if (m_callback)
		//	m_callback->onServerChange(nServerState);
		//if (m_pHwnd!=NULL)
		//	::PostMessage(m_pHwnd, EB_WM_SERVER_CHENGE, (WPARAM)nServerState, 0);
	}

	//if (m_userStatus == US_Logoned)
	//{
	//	m_userStatus = US_Invalidate;
	//	if (m_callback)
	//		m_callback->onServerDisconnect();

	//	if (m_pHwnd!=NULL)
	//		::PostMessage(m_pHwnd, EB_WM_SERVER_DISCONNECT, 0, 0);
	//}
}
void CUserManagerApp::OnInvalidateSession(int nResultCode,const CEBCAppCenter* pAPOwner)
{
	//if (m_userStatus == US_Logoned)
	if (pAPOwner == this->m_pAppCenter.get())
	{
		CEBCallbackInterface::SERVER_STATE nServerState = CEBCallbackInterface::SERVER_UNKNOWN;
		if (nResultCode == -102 && m_nApServerState != EB_SS_MOVED)
		{
			m_nApServerState = EB_SS_MOVED;
			nServerState = CEBCallbackInterface::SERVER_MOVED;
		}else if (nResultCode == -103 && m_nApServerState != EB_SS_RESTART)
		{
			m_nApServerState = EB_SS_RESTART;
			nServerState = CEBCallbackInterface::SERVER_RESTART;
		}else if (nResultCode == -117 && m_nApServerState != EB_SS_STOPED)
		{
			m_nApServerState = EB_SS_STOPED;
			nServerState = CEBCallbackInterface::SERVER_STOPED;
		}else if (nResultCode == -200 && m_nApServerState != EB_SS_TIMEOUT)
		{
			m_nApServerState = EB_SS_TIMEOUT;
			nServerState = CEBCallbackInterface::SERVER_TIMEOUT;
		}else
		{
			return;
		}
		m_userStatus = US_Invalidate;
		if (m_callback)
			m_callback->onServerChange(nServerState);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_SERVER_CHENGE);
//			pEvent->SetQEventType((QEvent::Type)EB_WM_SERVER_CHENGE);
			pEvent->SetEventParameter((long)nServerState);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::PostMessage(m_pHwnd, EB_WM_SERVER_CHENGE, (WPARAM)nServerState, 0);
#endif
		}
	}
}
void CUserManagerApp::ProcessResultValue(int nResultValue)
{
	if (m_nLastStateCode==(EB_STATE_CODE)nResultValue) return;	// 预防重复；
	m_nLastStateCode = (EB_STATE_CODE)nResultValue;
	if (m_nLastStateCode == EB_STATE_APP_ONLINE_KEY_TIMEOUT ||
		m_nLastStateCode==EB_STATE_APPID_KEY_ERROR)	// 跨服务器时候有用；
	{
		// app online key超时、错误，重新验证；
		m_pProcessMsgList.add(CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RESET_DEVAPPID));
		//SetDevAppId(m_sDevAppId,m_sDevAppKey,true);
	}
}
void CUserManagerApp::OnResultValue(int nResultValue,const CPOPCUserManager* pUMOwner)
{
	ProcessResultValue(nResultValue);
}
void CUserManagerApp::OnResultValue(int nResultValue,const CPOPCLogonCenter* pLCOwner)
{
	ProcessResultValue(nResultValue);
}
void CUserManagerApp::OnResultValue(int nResultValue,const CEBCAppCenter* pAPOwner)
{
	ProcessResultValue(nResultValue);
}

void CUserManagerApp::OnAPAOnResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CEBCAppCenter* pAPOwner)
{
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	if (nResultValue == EB_STATE_OK)
	{
		if (m_nUmServerState!=EB_SS_INIT)
		{
			m_nUmServerState=EB_SS_INIT;
		}
		m_userStatus = US_Logoned;
	}
	if (m_callback)
		m_callback->onLogonAppCenter((EB_STATE_CODE)nResultValue);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_LOGON_APPCENTER);
//		pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_APPCENTER);
		pEvent->SetEventParameter((long)nResultValue);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::SendMessage(m_pHwnd, EB_WM_LOGON_APPCENTER,(WPARAM)nResultValue,0);
#endif
	}
}
void CUserManagerApp::OnAPAOffResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CEBCAppCenter* pAPOwner)
{
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	if (m_callback)
		m_callback->onLogoutAppCenter((EB_STATE_CODE)nResultValue);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_LOGOUT_APPCENTER);
//		pEvent->SetQEventType((QEvent::Type)EB_WM_LOGOUT_APPCENTER);
		pEvent->SetEventParameter((long)nResultValue);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::SendMessage(m_pHwnd, EB_WM_LOGOUT_APPCENTER,(WPARAM)nResultValue,0);
#endif
	}
	m_pAppCenterTemp = m_pAppCenter;
	m_pAppCenter.reset();
	m_userStatus = US_Logout;
}
void CUserManagerApp::OnAPMsgResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CEBCAppCenter* pAPOwner)
{
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	// ???
	//if (m_callback)
	//	m_callback->onLogoutAppCenter((EB_STATE_CODE)nResultValue);
	//if (m_pHwnd!=NULL)
	//	::SendMessage(m_pHwnd, EB_WM_LOGOUT_APPCENTER,(WPARAM)nResultValue,0);
}
void CUserManagerApp::OnAPFAMsg(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CEBCAppCenter* pAPOwner)
{
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	EB_APMsgInfo* pAPMsgInfo = new EB_APMsgInfo();
	pAPMsgInfo->m_sFromAppId = pSotpResponseInfo->m_pResponseList.getParameterValue("from_app_id",(mycp::bigint)0);
	pAPMsgInfo->m_nUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("uid",(mycp::bigint)0);
	pAPMsgInfo->m_sAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("account");
	const mycp::bigint nMsgId = pSotpResponseInfo->m_pResponseList.getParameterValue("msg_id",(mycp::bigint)0);
	pAPMsgInfo->m_nMsgId = nMsgId;
	pAPMsgInfo->m_nMsgType = pSotpResponseInfo->m_pResponseList.getParameterValue("type",(mycp::bigint)0);
	pAPMsgInfo->m_sMsgName = pSotpResponseInfo->m_pResponseList.getParameterValue("name");
	pAPMsgInfo->m_sMsgContent = pSotpResponseInfo->m_pResponseList.getParameterValue("content");
	pAPMsgInfo->m_sMsgTime = pSotpResponseInfo->m_pResponseList.getParameterValue("time");
	pAPMsgInfo->m_nExtData = pSotpResponseInfo->m_pResponseList.getParameterValue("ext_data",(mycp::bigint)0);
	int ret = 0;
	if (m_callback)
		ret = m_callback->onAPMsgInfo(*pAPMsgInfo);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		pAPMsgInfo->SetQEventType((QEvent::Type)EB_WM_AP_MSG);
        QCoreApplication::postEvent(m_pHwnd, pAPMsgInfo, thePostEventPriority);
#else
		ret = ::SendMessage(m_pHwnd, EB_WM_AP_MSG,(WPARAM)pAPMsgInfo,0);
		delete pAPMsgInfo;
#endif
	}
	else {
		delete pAPMsgInfo;
	}

	if (ret == 1 && m_pAppCenter.get() != NULL)
	{
		// 自动响应
		m_pAppCenter->SendAMAck(nMsgId);
	}
}
void CUserManagerApp::OnAPAMackResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CEBCAppCenter* pAPOwner)
{
	//const int nResultValue = pSotpResponseInfo->GetResultValue();
}

/////////////////////////////////////
// logon center
void CUserManagerApp::OnLCULQueryResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCLogonCenter* pLCOwner)
{
	//BOOST_ASSERT (pSotpRequestInfo.get() != 0);
	if (pSotpRequestInfo.get() == NULL)
	{
		// may be timeout
		return;
	}
	BOOST_ASSERT (pSotpResponseInfo.get() != 0);
	//const mycp::bigint nFromUserId = pSotpRequestInfo->m_pRequestList.getParameterValue("from_uid");
	const tstring sFromAccount = pSotpRequestInfo->m_pRequestList.getParameterValue("from");
	const bool bOwnerCall = pSotpRequestInfo->m_pRequestList.getParameterValue("owner-call",false);
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	const mycp::bigint nUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("uid",(mycp::bigint)0);
	const tstring sAccount(pSotpResponseInfo->m_pResponseList.getParameterValue("account"));
	const int nAccountType = pSotpResponseInfo->m_pResponseList.getParameterValue("account-type",(int)0);
	//const tstring sFromInfo = pSotpResponseInfo->m_pResponseList.getParameterValue("f-info");	// ***
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", nUserId));
	pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-account", sAccount));
	const mycp::bigint sDepCode = pSotpResponseInfo->m_pResponseList.getParameterValue("dep_code",(mycp::bigint)0);
	//const tstring sUmAppName = pSotpResponseInfo->m_pResponseList.getParameterValue("um-appname");
	tstring sUmServer(pSotpResponseInfo->m_pResponseList.getParameterValue("um-server"));
	const tstring sCallUmKey = pSotpResponseInfo->m_pResponseList.getParameterValue("um-key");

	if (bOwnerCall)
	{
		// 群组会话，uid==owner_uid
		if (sDepCode>0)
			theOwnerCallGroup.insert(sDepCode,true,false);
		else if (nUserId>0)
		{
			if (m_pMyAccountInfo->GetUserId()==nUserId) return;
			theOwnerCallUser.insert(nUserId,true,false);
		}
	}
	if (sUmServer.empty() && m_pUserManager.get() != NULL)
	{
		sUmServer = m_pUserManager->GetAddress().address();
	}

	CEBCallInfo::pointer pGroupCallInfo;
	if (sDepCode>0)
	{
		// 群聊天请求
		const mycp::bigint sCallId = sDepCode;
		const mycp::bigint sChatId = sDepCode;
		if (nResultValue != EB_STATE_OK)
		{
			EB_CallInfo * pEbCallInfo = new EB_CallInfo(nUserId,sFromAccount,sCallId,sDepCode);
			if (m_callback)
				m_callback->onCallError(*pEbCallInfo,(EB_STATE_CODE)nResultValue);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
                pEbCallInfo->SetEventParameter((long)nResultValue);
                QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
				::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, nResultValue);
				delete pEbCallInfo;
#endif
			}
			else {
				delete pEbCallInfo;
			}
			return;
		}

		if (!theCallInfoList.find(sCallId, pGroupCallInfo))
		{
			return;
		}
		//pGroupCallInfo->m_sUmAppName = sUmAppName;
		pGroupCallInfo->m_sUmServer = sUmServer;
		pGroupCallInfo->m_sChatId = sChatId;

		if (!BuildGroupCallUserList(pGroupCallInfo,pGroupCallInfo->m_sGroupCode))
		{
			return;
		}
		//CEBGroupInfo::pointer pDepartmentInfo;
		//if (!theDepartmentList.find(pGroupCallInfo->m_sGroupCode, pDepartmentInfo))
		//	return;

		//// 先添加成员
		//{
		//	boost::mutex::scoped_lock lock(const_cast<boost::mutex&>(pDepartmentInfo->m_pMemberList.mutex()));
		//	CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		//	for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		//	{
		//		CEBMemberInfo::pointer pEmployeeInfo = pIter->second;
		//		const mycp::bigint nEmpUserId = pEmployeeInfo->m_nMemberUserId;
		//		if (nEmpUserId == m_pMyAccountInfo->GetUserId()) continue;
		//		if (!pGroupCallInfo->m_pCallUserList.exist(nEmpUserId))
		//		{
		//			CEBAccountInfo::pointer pEmpAccountInfo = CEBAccountInfo::create(nEmpUserId,pEmployeeInfo->m_sMemberAccount);
		//			pEmpAccountInfo->m_pFromCardInfo.m_nAccountType = EB_ACCOUNT_TYPE_IN_ENT;
		//			pEmployeeInfo->GetECardInfo(&pEmpAccountInfo->m_pFromCardInfo);
		//			pGroupCallInfo->m_pCallUserList.insert(nEmpUserId, pEmpAccountInfo);
		//		}
		//	}
		//}
	}
	if (EB_REQ_TYPE_LOGON == pSotpRequestInfo->GetRequestType())
	{
		// 登录请求；
		if (nResultValue==EB_STATE_APP_ONLINE_KEY_TIMEOUT || nResultValue==EB_STATE_APPID_KEY_ERROR)
		{
			// app online key超时、错误，重新验证；
			m_pProcessMsgList.add(CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RESET_DEVAPPID));
			return;
		}
		const cgcParameter::pointer pLogonInfoTemp = pSotpRequestInfo->m_pRequestList.getParameter("logon-info");
		if (nUserId==0 || pLogonInfoTemp.get() == NULL || pLogonInfoTemp->getObject().get() == NULL)
		{
			const EB_AccountInfo* pAccountInfo = m_pMyAccountInfo.get(); 
			if (m_callback)
				m_callback->onLogonError(*pAccountInfo,(EB_STATE_CODE)nResultValue);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
				pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_ERROR);
				pEvent->SetEventParameter((long)nResultValue);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_LOGON_ERROR, (WPARAM)pAccountInfo, nResultValue);
#endif
			}
			m_userStatus = US_LogonError;
			if (m_tReLogonTime>0 && nResultValue==EB_STATE_ACC_PWD_ERROR)
				m_tReLogonTime = 0;
			return;
		}
		CPOPLogonInfo::pointer pLogonInfo = CGC_OBJECT_CAST<CPOPLogonInfo>(pLogonInfoTemp->getObject());
		if (m_pLogonCenter.get()!=NULL)
		{
			if (nAccountType==1)
				pLogonInfo->m_nLogonType |= EB_LOGON_TYPE_MAIL_TEST;

			pLogonInfo->m_nUserId = nUserId;
			m_pLogonCenter->SendLCLogon(m_sLogonUserData,m_nSDKVersion,0,pLogonInfo,m_sDevAppId,m_sDevAppOnlineKey);
		}else
		{
			const EB_AccountInfo* pAccountInfo = m_pMyAccountInfo.get(); 
			if (m_callback)
				m_callback->onLogonError(*pAccountInfo,EB_STATE_ERROR);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
				pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_ERROR);
				pEvent->SetEventParameter((long)EB_STATE_ERROR);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_LOGON_ERROR, (WPARAM)pAccountInfo, EB_STATE_ERROR);
#endif
			}
			m_userStatus = US_LogonError;
			if (m_tReLogonTime>0 && nResultValue==EB_STATE_ACC_PWD_ERROR)
				m_tReLogonTime = 0;
		}
	}else if (EB_REQ_TYPE_REG == pSotpRequestInfo->GetRequestType() ||
		EB_REQ_TYPE_INVITE == pSotpRequestInfo->GetRequestType()/* ||
		EB_REQ_TYPE_USER_MSG == pSotpRequestInfo->GetRequestType()*/
		)
	{
		if (nResultValue == EB_STATE_OK || nResultValue == EB_STATE_USER_OFFLINE)	// 3:offline
		{
			LogMessage("OnLCULQueryResponse... (uid=%lld,nResultValue=%d)\r\n",nUserId,(int)nResultValue);
			if (sUmServer.empty())// || sUmAppName.empty())
			{
				if (EB_REQ_TYPE_REG == pSotpRequestInfo->GetRequestType())
				{
					if (this->m_callback)
						m_callback->onRegisterResponse(EB_STATE_ERROR,0);
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                        EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_REGISTER_RESPONSE);
//						pEvent->SetQEventType((QEvent::Type)EB_WM_REGISTER_RESPONSE);
						pEvent->SetEventParameter((long)EB_STATE_ERROR);
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::PostMessage(m_pHwnd, EB_WM_REGISTER_RESPONSE, (WPARAM)EB_STATE_ERROR, 0);
#endif
					}
				}else
				{
					EB_CallInfo * pEbCallInfo = new EB_CallInfo(nUserId,sAccount,0,sDepCode);
					if (m_callback)
						m_callback->onCallError(*pEbCallInfo,EB_STATE_ERROR);
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
						pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
                        pEbCallInfo->SetEventParameter((long)EB_STATE_ERROR);
                        QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
						::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, EB_STATE_ERROR);
						delete pEbCallInfo;
#endif
					}
					else {
						delete pEbCallInfo;
					}
				}
				return;
			}
			// ??
			// 
			const mycp::bigint sCallId = sDepCode;

			CPOPCUserManager::pointer pPOPCUserManager;
			//if (sCallId==0 && nResultValue == EB_STATE_OK && EB_REQ_TYPE_INVITE == pSotpRequestInfo->GetRequestType())
			//{
			//	CEBCallInfo::pointer pCallInfo = GetExistCallInfo(nUserId, 0);	// depcode:群组，为空表示单对单会话
			//	if (pCallInfo.get()!=NULL && pCallInfo->m_nCallState==EB_CALL_STATE_CONNECTED)
			//	{
			//		theUserManagerList.find(pCallInfo->GetCallId(), pPOPCUserManager);
			//	}
			//}

			//if (pPOPCUserManager.get()==NULL && m_pUserManager.get() != NULL && m_pUserManager->GetAddress().address() == sUmServer)
			//	pPOPCUserManager = m_pUserManager;
			//else
			if (pPOPCUserManager.get()==NULL)// if (!theUserManagerList.find(sAccount, pPOPCUserManager))
			{
				if (sCallId>0)
					theUserManagerList.find(sCallId, pPOPCUserManager);
				if (pPOPCUserManager.get()==NULL)
					theUserManagerList.find(nUserId, pPOPCUserManager);
				if (pPOPCUserManager.get()!=NULL && !sUmServer.empty() && sUmServer!=pPOPCUserManager->GetAddress().address())
				{
					if (sCallId>0)
						theUserManagerList.remove(sCallId);
					theUserManagerList.remove(nUserId);
					pPOPCUserManager.reset();
				}
				if (pPOPCUserManager.get()==NULL)
				{
					bool bRetry = false;
					pPOPCUserManager = CPOPCUserManager::create(this,0);
					pPOPCUserManager->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
					int ret = pPOPCUserManager->Start(sUmServer,"",0,true,true,1);
					if (ret==-1)
					{
						// 域名解析问题，再试一次；
						const tstring sServerAddress = GetAddressList(sUmServer);
						if (!sServerAddress.empty())
						{
							pPOPCUserManager.reset();
							pPOPCUserManager = CPOPCUserManager::create(this,0);
							pPOPCUserManager->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
							ret = pPOPCUserManager->Start(sServerAddress,"",0,true,true,1);
							bRetry = true;
						}
					}
					if (ret != 0)
					{
						EB_CallInfo * pEbCallInfo = new EB_CallInfo(nUserId,sAccount,0,sDepCode);
						if (m_callback)
							m_callback->onCallError(*pEbCallInfo,EB_STATE_ERROR);
						if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
							pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
                            pEbCallInfo->SetEventParameter((long)EB_STATE_ERROR);
                            QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
							::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, EB_STATE_ERROR);
							delete pEbCallInfo;
#endif
						}
						else {
							delete pEbCallInfo;
						}
						return;
					}
					pPOPCUserManager->SetUserId(this->m_pMyAccountInfo->GetUserId());
					if (!bRetry)
						SaveAddressList(sUmServer);
				}
			}
            if (pGroupCallInfo.get() != 0) {
                /// 查询群组
				theUserManagerList.remove(sCallId);
				theUserManagerList.insert(sCallId, pPOPCUserManager);	// 统一放在这里
				pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("um-key", sCallUmKey));
				pPOPCUserManager->SendUMIUUser(0,0,pGroupCallInfo,0,pSotpRequestInfo);
				return;
            }
            else {
				theUserManagerList.remove(nUserId);
				theUserManagerList.insert(nUserId, pPOPCUserManager);	// 统一放在这里
			}

			switch (pSotpRequestInfo->GetRequestType())
			{
			case EB_REQ_TYPE_REG:
				{
					// 注册新用户
					//MessageBox(NULL,sFromAccount.c_str(),"EB_REQ_TYPE_REG",MB_OK);
					const tstring sPassword = pSotpRequestInfo->m_pRequestList.getParameterValue("pwd");
					const tstring sUserName = pSotpRequestInfo->m_pRequestList.getParameterValue("user-name");
					const tstring sEntName = pSotpRequestInfo->m_pRequestList.getParameterValue("ent-name");
					pPOPCUserManager->SetDevAppId(m_sDevAppId,m_sDevAppOnlineKey);
					pPOPCUserManager->SendUMReg(0,sFromAccount, 0,sPassword, sUserName, sEntName);
				}break;
			//case EB_REQ_TYPE_USER_MSG:
			//	{
			//		//const tstring sEmpCode = pSotpRequestInfo->m_pRequestList.getParameterValue("emp-code");
			//		//CEBECardInfo::pointer pPopECardInfo = GetMyECardInfo(sEmpCode);
			//		//if (pPopECardInfo.get() == NULL) return -1;
			//		const EB_MSG_TYPE nMsgType = (EB_MSG_TYPE)pSotpRequestInfo->m_pRequestList.getParameterValue("msg-type", (int)0);
			//		const tstring sMsgContent = pSotpRequestInfo->m_pRequestList.getParameterValue("msg-content");
			//		const tstring sDepCode = pSotpRequestInfo->m_pRequestList.getParameterValue("dep-code");
			//		const bool bOffSave = pSotpRequestInfo->m_pRequestList.getParameterValue("off-save", false);
			//		CEBAppMsgInfo::pointer pMsgInfo = CEBAppMsgInfo::create(nMsgType, sMsgContent);
			//		pMsgInfo->m_sGroupCode = sDepCode;
			//		//pMsgInfo->m_sFromInfo = sFromInfo;
			//		pMsgInfo->m_bOffSave = bOffSave;
			//		pPOPCUserManager->SendUMIUMsg(m_pMyAccountInfo->GetAccount(), sFromAccount, pMsgInfo);//, pPopECardInfo);
			//	}break;
			case EB_REQ_TYPE_INVITE:
				{
					// 邀请用户会话
					const mycp::bigint sToMemberCode = pSotpRequestInfo->m_pRequestList.getParameterValue("member-code",(mycp::bigint)0);
					const mycp::bigint sExistCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("exist-call-id",(mycp::bigint)0);
					const mycp::bigint sOldCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("old-cid",(mycp::bigint)0);
					const int bCall2Dep = pSotpRequestInfo->m_pRequestList.getParameterValue("call2group",(int)0);
					//if (nResultValue == EB_STATE_USER_OFFLINE)
					//{
					//	//theOffLineUser.remove(nUserId);
					//	//theOffLineUser.insert(nUserId, true);
					//}
					// 记录接通会话用户
					//if (sDepCode==0)
					//	theAcceptUser.insert(nUserId,true);

					if (sExistCallId==0)
					{
						pPOPCUserManager->SendUMIUUser(sToMemberCode,nUserId,NullCallInfo,bCall2Dep,pSotpRequestInfo);
					}else
					{
						// 邀请用户进现有会议室
						CEBCallInfo::pointer pCallInfo;
						if (!theCallInfoList.find(sExistCallId, pCallInfo))
						{
							// **有可能会话无效，使用旧ID进入
							pCallInfo = CEBCallInfo::create(sExistCallId,0,this->m_pMyAccountInfo);
							//EB_CallInfo pEbCallInfo(sAccount,"",sDepCode);
							//if (m_callback)
							//	m_callback->onCallError(pEbCallInfo,EB_STATE_ERROR);
							//if (m_pHwnd!=NULL)
							//	::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)&pEbCallInfo, EB_STATE_ERROR);
							//return;
						}
						if (pSotpRequestInfo.get()!=NULL)
							pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("group-id", pCallInfo->m_sGroupCode));
						pCallInfo->m_sOldCallId = sOldCallId==pCallInfo->GetCallId()?0:sOldCallId;
						pPOPCUserManager->SendUMIUUser(sToMemberCode,nUserId,pCallInfo,bCall2Dep,pSotpRequestInfo);
					}
				}break;
			default:
				break;
			}
		}else
		{
			if (EB_REQ_TYPE_REG == pSotpRequestInfo->GetRequestType())
			{
				if (this->m_callback)
					m_callback->onRegisterResponse((EB_STATE_CODE)nResultValue,0);
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                    EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_REGISTER_RESPONSE);
//					pEvent->SetQEventType((QEvent::Type)EB_WM_REGISTER_RESPONSE);
					pEvent->SetEventParameter((long)nResultValue);
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
					::PostMessage(m_pHwnd, EB_WM_REGISTER_RESPONSE, nResultValue, 0);
#endif
				}
			}else
			{
				EB_CallInfo * pEbCallInfo = new EB_CallInfo(nUserId,sAccount,0,sDepCode);
				if (m_callback)
					m_callback->onCallError(*pEbCallInfo,(EB_STATE_CODE)nResultValue);
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
					pEbCallInfo->SetEventParameter((long)nResultValue);
                    QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
					::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, nResultValue);
					delete pEbCallInfo;
#endif
				}
				else {
					delete pEbCallInfo;
				}
			}
		}
	}
}
//bool DeleteDirectory(const char* strDirName)
//{
//	CFileFind tempFind;
//	char strTempFileFind[MAX_PATH];
//	sprintf(strTempFileFind,"%s\\*.*", strDirName);
//	BOOL IsFinded = tempFind.FindFile(strTempFileFind);
//	while (IsFinded)
//	{
//		IsFinded = tempFind.FindNextFile();
//		if (!tempFind.IsDots()) 
//		{
//			char strFoundFileName[MAX_PATH];
//			strcpy(strFoundFileName, tempFind.GetFileName().GetBuffer(MAX_PATH));
//			if (tempFind.IsDirectory())
//			{
//				char strTempDir[MAX_PATH];
//				sprintf(strTempDir,"%s\\%s", strDirName, strFoundFileName);
//				if (!DeleteDirectory(strTempDir))
//					return false;
//			}
//			else
//			{
//				char strTempFileName[MAX_PATH];
//				sprintf(strTempFileName,"%s\\%s", strDirName, strFoundFileName);
//				if (!DeleteFileA(strTempFileName))
//					return false;
//			}
//		}
//	}
//	tempFind.Close();
//	if(!RemoveDirectoryA(strDirName))
//	{
//		return false;
//	}
//	return true;
//}
void CUserManagerApp::SetServerAddress(const tstring& sAddress,const tstring& sOrgAddress)
{
	m_sServerAddress = sAddress;
	m_nOrgServerIndex = 0;
	m_sOrgServerAddress = sOrgAddress;
	//if (m_sOrgServerAddress.rfind(":")==std::string::npos)
	//	m_sOrgServerAddress.append(":18012");
	ParseString(m_sOrgServerAddress,";",true,m_pOrgServerList);
	if (m_pOrgServerList.size()>=2)
	{
		char sTempIniFile[260];
#ifdef _QT_MAKE_
        sprintf(sTempIniFile,"%s/temp.ini",m_sEbDataPath.toLocal8Bit().constData());
#else
        sprintf(sTempIniFile,"%s\\temp.ini",m_sEbDataPath.c_str());
#endif

#ifdef WIN32
        const int nOutLCIndex = GetPrivateProfileIntA(m_sOrgServerAddress.c_str(),_T("lc_org_index"),-1,sTempIniFile);
#else
        const int nOutLCIndex = GetPrivateProfileIntABoost(m_sOrgServerAddress.c_str(),_T("lc_org_index"),-1,sTempIniFile);
#endif
		if (nOutLCIndex>=0 && nOutLCIndex<(int)m_pOrgServerList.size())
		{
			m_nOrgServerIndex = nOutLCIndex;
			m_sServerAddress = m_pOrgServerList[nOutLCIndex];
			if (m_sServerAddress.rfind(":")==std::string::npos)
				m_sServerAddress.append(":18012");
		}
	}
	if (m_pLCList.empty())
	{
		int nCurentIndex = 0;
		time_t tLastTime = 0;
		const tstring sLCList = GetLCAddressList(nCurentIndex,tLastTime);
		ParseString(sLCList,";",true,m_pLCList);
		//if (tLastTime>0 && (tLastTime+1*3600)>time(0))	// 一小时内
		if (nCurentIndex>0)
		{
			if ((nCurentIndex+1)<=(int)sLCList.size())
			{
				m_sServerAddress = m_pLCList[nCurentIndex];
				m_nCurrentIndex = nCurentIndex;
				//m_tBeginOfStart = time(0);
				return;
			}
		}
	}
	if (!m_pLCList.empty() && m_pLCList[0]!=m_sServerAddress)
	{
		//if (m_pLCList.front(m_sServerAddress)==std::string::npos)
		//{
		//	// ** 配置IP地址，不正确

		//}else
		{
			m_sServerAddress = m_pLCList[0];
			m_nCurrentIndex = 0;
			m_tBeginOfStart = time(0);
		}
	}
}

void CUserManagerApp::OnLCULLogonResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner)
{
	if (m_pLogonCenter.get()==NULL)
	{
		return;
	}
	//if (m_userStatus != US_Logging)
	//	return -1;
	const int nResultValue = pResponseInfo->GetResultValue();
	if (nResultValue == EB_STATE_OAUTH_FORWARD)
	{
		const tstring sOAuthUrl(pResponseInfo->m_pResponseList.getParameterValue("oauth_url"));
		m_userStatus = US_OAuthForward;
		if (m_callback!=NULL)
			m_callback->onOAuthForward(sOAuthUrl);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_OAUTH_FORWARD);
//			pEvent->SetQEventType((QEvent::Type)EB_WM_OAUTH_FORWARD);
			pEvent->SetEventString(sOAuthUrl.string());
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_OAUTH_FORWARD, (WPARAM)sOAuthUrl.c_str(), 0);
#endif
		}
	}else if (nResultValue == EB_STATE_OK)
	{
		//const tstring sAppName = pResponseInfo->m_pResponseList.getParameterValue("app-name");
		const tstring sServerAddress(pResponseInfo->m_pResponseList.getParameterValue("address"));
		const tstring sHttpServer(pResponseInfo->m_pResponseList.getParameterValue("http-address"));
		//m_sHttpServer = pResponseInfo->m_pResponseList.getParameterValue("http-address");
//		LogMessage("OnLCULLogonResponse... (m_userStatus=%d,m_sHttpServer=%s)\r\n",(int)m_userStatus,m_sHttpServer.c_str());
		mycp::bigint nUserId = pResponseInfo->m_pResponseList.getParameterValue("uid",(mycp::bigint)0);
		const tstring sAppOnlineKey(pResponseInfo->m_pResponseList.getParameterValue("app-online-key"));
		tstring sLCList(pResponseInfo->m_pResponseList.getParameterValue("lc-list"));
		const tstring sAccountPrefix(pResponseInfo->m_pResponseList.getParameterValue("account-prefix"));
		const int nSystemSetting = pResponseInfo->m_pResponseList.getParameterValue("system-setting",(int)0);
		const int nOpenRegister = pResponseInfo->m_pResponseList.getParameterValue("open-register",(int)0);
		const int nLicenseUser = pResponseInfo->m_pResponseList.getParameterValue("license-user",(int)0);
		const int nOpenVisitor = pResponseInfo->m_pResponseList.getParameterValue("open-visitor",(int)0);
		const tstring sForgetPwdUrl(pResponseInfo->m_pResponseList.getParameterValue("forget-pwd-url"));
		const tstring sRegisterUrl(pResponseInfo->m_pResponseList.getParameterValue("register-url"));
		const tstring sEntLogoUrl(pResponseInfo->m_pResponseList.getParameterValue("ent-logo-url"));
		const tstring sProductName(pResponseInfo->m_pResponseList.getParameterValue("product-name"));
		//const int nSendRegMail = pResponseInfo->m_pResponseList.getParameterValue("send-reg-mail",(int)0);
		const PASSWD_AUTH_MODE nPasswdAuthMode = (PASSWD_AUTH_MODE)pResponseInfo->m_pResponseList.getParameterValue("passwd-auth-mode",(int)PASSWD_AUTH_MODE_ENTBOOST);
		const tstring sOnlineKey(pResponseInfo->m_pResponseList.getParameterValue("online-key"));
		const tstring sAccountCmKey(pResponseInfo->m_pResponseList.getParameterValue("acm-key"));
		int nSetting = pResponseInfo->m_pResponseList.getParameterValue("setting", (int)0);
		const mycp::bigint sDefaultEmp = pResponseInfo->m_pResponseList.getParameterValue("default_emp", (mycp::bigint)0);
		const mycp::bigint nDeployId = pResponseInfo->m_pResponseList.getParameterValue("deploy-id",(mycp::bigint)0);
		const int nLicenstType = pResponseInfo->m_pResponseList.getParameterValue("license-type",(int)0);
		const mycp::bigint nAutoOpenSubId = pResponseInfo->m_pResponseList.getParameterValue("auto-open-subid",(mycp::bigint)0);
		//const mycp::bigint nGroupMsgSubId = pResponseInfo->m_pResponseList.getParameterValue("group-msg-subid",(mycp::bigint)0);
		const int nSaveConversations = pResponseInfo->m_pResponseList.getParameterValue("save-conversations",(int)0);
		const tstring sConversationsUrl(pResponseInfo->m_pResponseList.getParameterValue("conversations-url"));
		const int nEBServerVersion = pResponseInfo->m_pResponseList.getParameterValue("server-version",(int)0);
		const tstring sDefaultUrl(pResponseInfo->m_pResponseList.getParameterValue("default-url"));

		if (m_userStatus == US_DevAppIdLogoning)
		{
			// 开发者ID验证返回
			//m_sHttpServer = sHttpServer;
			m_sDevAppId = m_pLogonCenter->GetAppId();
			m_sDevAppKey = m_pLogonCenter->GetLogonPassword();
			m_sDevAppOnlineKey = sAppOnlineKey;
			//LogMessage("US_DevAppIdLogoning return m_sDevAppOnlineKey=%s,sLCList=%s\r\n",m_sDevAppOnlineKey.c_str(),sLCList.c_str());
			if (sLCList.empty())
				sLCList = m_sServerAddress;
			//sLCList = "192.168.1.100:18012;test-lc.entboost.com:18012";

			if (m_pOrgServerList.size()>=2 && m_nOrgServerIndex>=0 && m_nOrgServerIndex<(int)m_pOrgServerList.size())
			{
				char sTempIniFile[260];
#ifdef _QT_MAKE_
                sprintf(sTempIniFile,"%s/temp.ini",m_sEbDataPath.toLocal8Bit().constData());
#else
                sprintf(sTempIniFile,"%s\\temp.ini",m_sEbDataPath.c_str());
#endif
				char lpszBuffer[24];
				sprintf(lpszBuffer,"%d",m_nOrgServerIndex);
#ifdef WIN32
                WritePrivateProfileStringA(m_sOrgServerAddress.c_str(),_T("lc_org_index"),lpszBuffer,sTempIniFile);
#else
                WritePrivateProfileStringABoost(m_sOrgServerAddress.c_str(),_T("lc_org_index"),lpszBuffer,sTempIniFile);
#endif
			}

			SaveLCAddressList(sLCList);
			ParseString(sLCList,";",true,m_pLCList);
			if (!m_pLCList.empty() && m_pLCList[0]!=m_sServerAddress && m_pLCList[0]!=m_sLastErrorServerAddress)	// 不处理上一个出错服务器
			{
				m_nCurrentIndex = 0;
				//SaveLCAddressList(sLCList);						// 保存多一次；
				SaveLCAddressIndex(m_nCurrentIndex);		// * 更新index=0
				m_sServerAddress = m_pLCList[0];				// * 放后面
			}else
			{
				for (size_t i=0;i<m_pLCList.size();i++)
				{
					const tstring sLCAddress(m_pLCList[i]);
					if (sLCAddress==m_sServerAddress)
					{
						m_nCurrentIndex = i;
						SaveLCAddressIndex(m_nCurrentIndex);
						break;
					}
				}
			}
			m_sLastErrorServerAddress.clear();
			m_sAccountPrefix = sAccountPrefix;
			//if (!m_sAccountPrefix.empty())
			//	MessageBox(NULL,m_sAccountPrefix.c_str(),m_sAccountPrefix.c_str(),MB_OK);
			m_nOpenRegister = nOpenRegister;
			m_nSystemSetting = nSystemSetting;
			m_bOpenVisitor = nOpenVisitor==1?true:false;
			m_bLicenseUser = nLicenseUser==1?true:false;
			m_sForgetPwdUrl = sForgetPwdUrl;
			m_sRegisterUrl = sRegisterUrl;
			m_sEntLogoUrl = sEntLogoUrl;
			m_sProductName = sProductName;
			//m_nSendRegMail = nSendRegMail;
			m_nPasswdAuthMode = nPasswdAuthMode;
			m_nDeployId = nDeployId;
			m_nLicenstType = nLicenstType;
			m_nEBServerVersion = nEBServerVersion;
			m_nAutoOpenSubId = nAutoOpenSubId;
			//m_nGroupMsgSubId = nGroupMsgSubId;
			m_nSaveConversations = nSaveConversations;
			m_sConversationsUrl = sConversationsUrl;
			m_sDefaultUrl = sDefaultUrl;
			m_sAPServer = sServerAddress;
			//m_sAPAppName = sAppName;
			if (m_callback!=NULL)
				m_callback->onAppIdSuccess(m_sDevAppId,m_sDevAppOnlineKey.c_str());
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_APPID_SUCCESS);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_APPID_SUCCESS, 0, 0);
#endif
			}
			if (m_tReLogonTime>0)
				m_tReLogonTime = time(0)+3;	// 请求立即重新登录；
			//m_userStatus = US_Logout;
			return;
		}else if (m_userStatus != US_Logging)
		{
			if (m_userStatus == US_OAuthForward)	// OAUTH转发，重新设置正在登录状态
				m_userStatus = US_Logging;
			else
				return;
		}
		m_sHttpServer = sHttpServer;
		const int nArea1 = pResponseInfo->m_pResponseList.getParameterValue("area1", (int)0);
		const tstring sArea1(pResponseInfo->m_pResponseList.getParameterValue("area1s", ""));
		const int nArea2 = pResponseInfo->m_pResponseList.getParameterValue("area2", (int)0);
		const tstring sArea2(pResponseInfo->m_pResponseList.getParameterValue("area2s", ""));
		const int nArea3 = pResponseInfo->m_pResponseList.getParameterValue("area3", (int)0);
		const tstring sArea3(pResponseInfo->m_pResponseList.getParameterValue("area3s", ""));
		const int nArea4 = pResponseInfo->m_pResponseList.getParameterValue("area4", (int)0);
		const tstring sArea4(pResponseInfo->m_pResponseList.getParameterValue("area4s", ""));
		const tstring sMyAddress(pResponseInfo->m_pResponseList.getParameterValue("add", ""));
		const tstring sUrl(pResponseInfo->m_pResponseList.getParameterValue("url", ""));
		const EB_GENDER_TYPE nGender = (EB_GENDER_TYPE)pResponseInfo->m_pResponseList.getParameterValue("gender", (int)EB_GENDER_UNKNOWN);
		const tstring sTel(pResponseInfo->m_pResponseList.getParameterValue("tel", ""));
		const tstring sMobile(pResponseInfo->m_pResponseList.getParameterValue("mobile", ""));
		const tstring sEmail(pResponseInfo->m_pResponseList.getParameterValue("email", ""));
		const int nBirthday = pResponseInfo->m_pResponseList.getParameterValue("birthday", (int)0);
		const tstring sZipCode(pResponseInfo->m_pResponseList.getParameterValue("zipcode", ""));

		const tstring sDescription(pResponseInfo->m_pResponseList.getParameterValue("description", ""));
		const mycp::bigint nHeadResId = pResponseInfo->m_pResponseList.getParameterValue("head-rid", (mycp::bigint)0);
		const tstring sHeadCmServer(pResponseInfo->m_pResponseList.getParameterValue("head-cm-server", ""));
		const tstring sHeadHttpServer(pResponseInfo->m_pResponseList.getParameterValue("head-http-server", ""));
		const tstring sHeadMd5(pResponseInfo->m_pResponseList.getParameterValue("head-md5", ""));

        unsigned int nLogonType = m_pLogonCenter->GetLogonType();
		EB_USER_LINE_STATE nLineState = m_pLogonCenter->GetLineState();
		m_pMyAccountInfo->SetAccount(m_pLogonCenter->GetLogonAccount());
		m_pMyAccountInfo->SetPasswd(m_pLogonCenter->GetLogonPassword());	// 取消会导致不能正常修改密码；
		//MessageBox(NULL,m_pMyAccountInfo->GetPasswd().c_str(),"",MB_OK);
		//m_sUserPassword = m_pLogonCenter->GetLogonPassword();
		m_pMyAccountInfo->SetUserId(nUserId);
		m_pMyAccountInfo->SetLogonType(nLogonType);
		m_pMyAccountInfo->SetLineState(nLineState);
		m_pMyAccountInfo->m_pLogonList.clear();
		m_pMyAccountInfo->m_pLogonList.SetSotpSessionId(nLogonType,m_pMyAccountInfo->GetAccount(),"");
		m_pMyAccountInfo->m_pLogonList.SetOnlineKey("",sOnlineKey);
		m_pMyAccountInfo->m_pLogonList.SetACMKey("",sAccountCmKey);
		m_pMyAccountInfo->SetUserName(m_pLogonCenter->GetUserName());
		m_pMyAccountInfo->SetArea1(nArea1);
		m_pMyAccountInfo->SetArea2(nArea2);
		m_pMyAccountInfo->SetArea3(nArea3);
		m_pMyAccountInfo->SetArea4(nArea4);
		m_pMyAccountInfo->SetArea1String(sArea1);
		m_pMyAccountInfo->SetArea2String(sArea2);
		m_pMyAccountInfo->SetArea3String(sArea3);
		m_pMyAccountInfo->SetArea4String(sArea4);
		m_pMyAccountInfo->SetAddress(sMyAddress);
		m_pMyAccountInfo->SetUrl(sUrl);
		m_pMyAccountInfo->SetGender(nGender);
		m_pMyAccountInfo->SetTel(sTel);
		m_pMyAccountInfo->SetMobile(sMobile);
		m_pMyAccountInfo->SetEmail(sEmail);
		m_pMyAccountInfo->SetBirthday(nBirthday);
		m_pMyAccountInfo->SetZipCode(sZipCode);
		m_pMyAccountInfo->SetDescription(sDescription);
		m_pMyAccountInfo->SetSetting(nSetting);
		m_pMyAccountInfo->SetDefaultEmpId(sDefaultEmp);
		m_nLoadSubFunc = 1;
		m_bNeedLoadVerInfo = true;
		m_nServerEntDepInfoVer = -1;
		theServerEntDepInfoSize = 0;
		m_nServerMyDepInfoVer = -1;
		m_nServerContactInfoVer = -1;
		if (!m_pLogonCenter->GetReqCode().empty())
		{
			// 这是OAUTH验证
			const EB_AccountInfo* pAccountInfo = m_pMyAccountInfo.get(); 
			m_userStatus = US_Logoned;
			m_nApServerState = EB_SS_INIT;
			if (this->m_callback)
				m_callback->onLogonSuccess(*pAccountInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
				pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_SUCCESS);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_LOGON_SUCCESS, (WPARAM)pAccountInfo, 0);
#endif
			}
			return;
		}
        /// 下面是普通登录
		if (sDefaultEmp>0)
		{
			if (m_pDefaultEntEmployeeInfo.get()==NULL)
				m_pDefaultEntEmployeeInfo = CEBMemberInfo::create();
			m_pDefaultEntEmployeeInfo->m_sHeadResourceId = nHeadResId;
			m_pDefaultEntEmployeeInfo->m_sMemberCode = sDefaultEmp;
			m_pDefaultEntEmployeeInfo->m_sHeadCmServer = sHeadCmServer;
			m_pDefaultEntEmployeeInfo->m_sHeadHttpServer = sHeadHttpServer;
			m_pDefaultEntEmployeeInfo->m_sHeadMd5 = sHeadMd5;
			ProcessMemberHeadInfo(m_pDefaultEntEmployeeInfo);
		}
		if (m_nDeployId>0 && !m_pMyAccountInfo->IsLogonVisitor())
		{
            /// **检查本地数据文件；
#ifdef Q_OS_ANDROID
            const EBFileString sDefaultEBDatasFile = "assets:/datas/bodb/syncdatas";
#else
            const EBFileString sDefaultEBDatasFile = GetAppPath()+"/datas/bodb/syncdatas";
#endif
#ifdef _QT_MAKE_
            const bool bExistDefaultEBDatasFile = QFileInfo::exists(sDefaultEBDatasFile);
            QString lpszBuffer = QString("%1/users").arg(GetAppPath());
#else
            const BOOL bExistDefaultEBDatasFile = ::PathFileExistsA(sDefaultEBDatasFile.c_str());
            char lpszBuffer[260];
            sprintf(lpszBuffer,"%s/users",GetAppPath().c_str());
#endif
            /// *数据库路径
            /// \users
            EBFileString sCurrentLogonServerBoPath(lpszBuffer);
#ifdef _QT_MAKE_
            if (!QFileInfo::exists(sCurrentLogonServerBoPath) && bExistDefaultEBDatasFile)
#else
            if (!::PathFileExistsA(sCurrentLogonServerBoPath.c_str()) && bExistDefaultEBDatasFile)
#endif
			{
#ifdef _QT_MAKE_
				QDir pDir;
                pDir.mkdir(sCurrentLogonServerBoPath);
#else
                ::CreateDirectoryA(sCurrentLogonServerBoPath.c_str(), NULL);
#endif
			}
            ///  ANDROID：/users/uid
            /// \users\[account]
#ifdef _QT_MAKE_
#ifdef Q_OS_ANDROID
            sCurrentLogonServerBoPath = QString("%1/%2").arg(sCurrentLogonServerBoPath).arg(m_pMyAccountInfo->GetUserId());
#else   /// Q_OS_ANDROID
            sCurrentLogonServerBoPath = QString("%1/%2").arg(sCurrentLogonServerBoPath).arg(m_pMyAccountInfo->GetAccount().c_str());
#endif  /// Q_OS_ANDROID
            if (!QFileInfo::exists(sCurrentLogonServerBoPath) && bExistDefaultEBDatasFile)
#else
            sprintf(lpszBuffer,"%s\\%s",sCurrentLogonServerBoPath.c_str(),m_pMyAccountInfo->GetAccount().c_str());
            sCurrentLogonServerBoPath = lpszBuffer;
            if (!::PathFileExistsA(sCurrentLogonServerBoPath.c_str()) && bExistDefaultEBDatasFile)
#endif
			{
#ifdef _QT_MAKE_
				QDir pDir;
                pDir.mkdir(sCurrentLogonServerBoPath);
#else
                ::CreateDirectoryA(sCurrentLogonServerBoPath.c_str(), NULL);
#endif
			}
            /// ANDROID: /users/[user_id]/[deployid]
            /// \users\[account]\[deployid]
#ifdef _QT_MAKE_
            sCurrentLogonServerBoPath = QString("%1/%2").arg(sCurrentLogonServerBoPath).arg(m_nDeployId);
            if (!QFileInfo::exists(sCurrentLogonServerBoPath) && bExistDefaultEBDatasFile)
#else
            sprintf(lpszBuffer,"%s\\%lld",sCurrentLogonServerBoPath.c_str(),m_nDeployId);
            sCurrentLogonServerBoPath = lpszBuffer;
            if (!::PathFileExistsA(sCurrentLogonServerBoPath.c_str()) && bExistDefaultEBDatasFile)
#endif
			{
#ifdef _QT_MAKE_
				QDir pDir;
                pDir.mkdir(sCurrentLogonServerBoPath);
#else
                ::CreateDirectoryA(sCurrentLogonServerBoPath.c_str(), NULL);
#endif
			}
            /// 删除过期无用文件
            const EBFileString sDelPath = sCurrentLogonServerBoPath+"/bodb/ebdatas";
#ifdef _QT_MAKE_
            if (QFileInfo::exists(sDelPath)) {
#else
            if (::PathFileExistsA(sDelPath.c_str())) {
#endif
                EBFileString sDelFile = sDelPath+"/ebdatas.bdf";
#ifdef _QT_MAKE_
                QFile::remove(sDelFile);
                sDelFile = sDelPath+"/ebdatas.bdf.bk";
                QFile::remove(sDelFile);
				QDir pDir;
                pDir.rmdir(sDelPath);
				//QDir pDir(sDelPath.c_str());
				//pDir.rmdir(QDir::toNativeSeparators(pDir.path()));
#else
                DeleteFileA(sDelFile.c_str());
				sDelFile = sDelPath + _T("\\ebdatas.bdf.bk");
                DeleteFileA(sDelFile.c_str());
                RemoveDirectoryA(sDelPath.c_str());
#endif
			}

            /// ANDROID: /users/[user_id]\[deployid]\bodb
            /// \users\[account]\[deployid]\bodb
            sCurrentLogonServerBoPath = sCurrentLogonServerBoPath+"/bodb";
#ifdef _QT_MAKE_
            if (!QFileInfo::exists(sCurrentLogonServerBoPath) && bExistDefaultEBDatasFile) {
				QDir pDir;
                pDir.mkdir(sCurrentLogonServerBoPath);
			}
#else
            if (!::PathFileExistsA(sCurrentLogonServerBoPath.c_str()) && bExistDefaultEBDatasFile) {
                ::CreateDirectoryA(sCurrentLogonServerBoPath.c_str(), NULL);
			}
#endif

            /// *数据库文件
            /// ANDROID: /users/[user_id]/[deployid]/bodb/ebdatas
            /// \users\[account]\[deployid]\bodb\ebdatas
            const EBFileString sCurrentLogonServerBoFile = sCurrentLogonServerBoPath+"/syncdatas";
            //if (!::PathFileExistsA(sCurrentLogonServerBoFile.c_str()) && bExistDefaultEBDatasFile)
			//{
            //	::CreateDirectoryA(sCurrentLogonServerBoFile.c_str(), NULL);
			//}
			//// \users\[account]\[deployid]\bodb\ebdatas\ebdatas.bdf
			//sCurrentLogonServerBoFile = sCurrentLogonServerBoFile + _T("\\ebdatas.bdf");
			//const tstring sCurrentLogonServerBoFileBk = sCurrentLogonServerBoFile + _T(".bk");
			//bool bNewCopyFile = false;
            if (EbGetFileSize(sCurrentLogonServerBoFile)<=0 && bExistDefaultEBDatasFile)
            //if (!::PathFileExistsA(sCurrentLogonServerBoFile.c_str()) && bExistDefaultEBDatasFile)
			{
				//bNewCopyFile = true;
#ifdef _QT_MAKE_
                QFile::remove(sCurrentLogonServerBoFile);
                QFile::copy(sDefaultEBDatasFile, sCurrentLogonServerBoFile);
#else
                CopyFileA(sDefaultEBDatasFile.c_str(),sCurrentLogonServerBoFile.c_str(),FALSE);
#endif
			}
            if (m_pEBDatas.get()!=0)
			{
				try
				{
					m_pEBDatas->close();
				}
				catch(std::exception&)
				{
				}
				catch(...)
				{
				}
				m_pEBDatas.reset();
			}
#ifdef _QT_MAKE_
            if (QFileInfo::exists(sCurrentLogonServerBoFile))
#else
            if (::PathFileExistsA(sCurrentLogonServerBoFile.c_str()))
#endif
			{
				m_pEBDatas = CSqliteCdbc::create();
#ifdef _QT_MAKE_
                if (!m_pEBDatas->open(sCurrentLogonServerBoFile.toStdString().c_str()))
#else
				if (!m_pEBDatas->open(CEBParseSetting::str_convert(sCurrentLogonServerBoFile.c_str(),CP_ACP,CP_UTF8).c_str()))
#endif
				{
					m_pEBDatas.reset();
#ifdef _QT_MAKE_
                    QFile::remove(sCurrentLogonServerBoFile);
#else
                    DeleteFileA(sCurrentLogonServerBoFile.c_str());
#endif
				}else
				{
                    /// ** 检查 eb_contact_info_t 表 1.25 版本代码，1.28以后版本删除下面代码
					if (m_pEBDatas.get()!=NULL && m_pEBDatas->select("SELECT con_id FROM eb_contact_info_t WHERE con_id=0 LIMIT 1")<0)
					{
						const char* pCreateDatabaseTable = "CREATE TABLE eb_contact_info_t \
						( \
							con_id BIGINT DEFAULT 0 PRIMARY KEY, \
							con_uid BIGINT DEFAULT 0, \
							ug_id BIGINT DEFAULT 0, \
							con_account VARCHAR(64) DEFAULT '', \
							name VARCHAR(64) DEFAULT '', \
							phone VARCHAR(32) DEFAULT '', \
							email VARCHAR(120) DEFAULT '', \
							address VARCHAR(250) DEFAULT '', \
							description VARCHAR(500) DEFAULT '', \
							company VARCHAR(128) DEFAULT '', \
							job_title VARCHAR(64) DEFAULT '', \
							tel VARCHAR(32) DEFAULT '', \
							fax VARCHAR(32) DEFAULT '', \
							url VARCHAR(64) DEFAULT '', \
							ntype SMALLINT DEFAULT 0, \
							head_ver BIGINT DEFAULT 0, \
							head_rid BIGINT DEFAULT 0, \
							head_cm_server VARCHAR(32) DEFAULT '', \
							head_http_server VARCHAR(32) DEFAULT '', \
							head_md5 VARCHAR(32) DEFAULT '', \
							flag INT DEFAULT 0 \
						); \
						CREATE INDEX eb_contact_info_t_idx_con_uid ON eb_contact_info_t(con_uid); \
						CREATE INDEX eb_contact_info_t_idx_ug_id ON eb_contact_info_t(ug_id);";
						if (m_pEBDatas->execute(pCreateDatabaseTable)==-1)
						{
							// 错误
							m_pEBDatas->close();
							m_pEBDatas.reset();
							// **删除文件，下次登录会重新创建
#ifdef _QT_MAKE_
                            QFile::remove(sCurrentLogonServerBoFile);
                            QFile::copy(sDefaultEBDatasFile,sCurrentLogonServerBoFile);
#else
                            DeleteFileA(sCurrentLogonServerBoFile.c_str());
                            CopyFileA(sDefaultEBDatasFile.c_str(),sCurrentLogonServerBoFile.c_str(),FALSE);
#endif
							m_pEBDatas = CSqliteCdbc::create();
#ifdef _QT_MAKE_
                            if (!m_pEBDatas->open(sCurrentLogonServerBoFile.toStdString().c_str()))
#else
							if (!m_pEBDatas->open(CEBParseSetting::str_convert(sCurrentLogonServerBoFile.c_str(),CP_ACP,CP_UTF8).c_str()))
#endif
							{
								m_pEBDatas.reset();
#ifdef _QT_MAKE_
                                QFile::remove(sCurrentLogonServerBoFile);
#else
                                DeleteFileA(sCurrentLogonServerBoFile.c_str());
#endif
							}
						}
					}

                    /// ** 检查 eb_emp_info_t 表 1.25 版本代码，1.28 以后版本删除下面代码
					if (m_pEBDatas.get()!=NULL && m_pEBDatas->select("SELECT display_index FROM eb_emp_info_t LIMIT 1")<0)
					{
						const char* pAlterTable = "ALTER TABLE eb_emp_info_t ADD display_index smallint DEFAULT 0;";
						if (m_pEBDatas->execute(pAlterTable)==-1)
						{
							// 错误
							m_pEBDatas->close();
							m_pEBDatas.reset();
							// **删除文件，下次登录会重新创建
#ifdef _QT_MAKE_
                            QFile::remove(sCurrentLogonServerBoFile);
                            QFile::copy(sDefaultEBDatasFile,sCurrentLogonServerBoFile);
#else
                            DeleteFileA(sCurrentLogonServerBoFile.c_str());
                            CopyFileA(sDefaultEBDatasFile.c_str(),sCurrentLogonServerBoFile.c_str(),FALSE);
#endif
							m_pEBDatas = CSqliteCdbc::create();
#ifdef _QT_MAKE_
                            if (!m_pEBDatas->open(sCurrentLogonServerBoFile.toStdString().c_str()))
#else
							if (!m_pEBDatas->open(CEBParseSetting::str_convert(sCurrentLogonServerBoFile.c_str(),CP_ACP,CP_UTF8).c_str()))
#endif
							{
								m_pEBDatas.reset();
#ifdef _QT_MAKE_
                                QFile::remove(sCurrentLogonServerBoFile);
#else
                                DeleteFileA(sCurrentLogonServerBoFile.c_str());
#endif
							}
						}
					}
					// ** 检查 eb_emp_info_t 表 forbid_minutes 字段 1.25 版本代码，1.28 以后版本删除下面代码 2017-03-30
					if (m_pEBDatas.get()!=NULL && m_pEBDatas->select("SELECT forbid_minutes FROM eb_emp_info_t LIMIT 1")<0)
					{
						const char* pAlterTable = "ALTER TABLE eb_emp_info_t ADD forbid_minutes int DEFAULT 0;";
						if (m_pEBDatas->execute(pAlterTable)==-1)
						{
							// 错误
							m_pEBDatas->close();
							m_pEBDatas.reset();
							// **删除文件，下次登录会重新创建
#ifdef _QT_MAKE_
                            QFile::remove(sCurrentLogonServerBoFile);
                            QFile::copy(sDefaultEBDatasFile,sCurrentLogonServerBoFile);
#else
                            DeleteFileA(sCurrentLogonServerBoFile.c_str());
                            CopyFileA(sDefaultEBDatasFile.c_str(),sCurrentLogonServerBoFile.c_str(),FALSE);
#endif
							m_pEBDatas = CSqliteCdbc::create();
#ifdef _QT_MAKE_
                            if (!m_pEBDatas->open(sCurrentLogonServerBoFile.toStdString().c_str()))
#else
							if (!m_pEBDatas->open(CEBParseSetting::str_convert(sCurrentLogonServerBoFile.c_str(),CP_ACP,CP_UTF8).c_str()))
#endif
							{
								m_pEBDatas.reset();
#ifdef _QT_MAKE_
                                QFile::remove(sCurrentLogonServerBoFile);
#else
                                DeleteFileA(sCurrentLogonServerBoFile.c_str());
#endif
							}
						}
					}

                    /// ** 检查 eb_dep_info_t 表 1.25 版本代码，1.28 以后版本删除下面代码 (2017-03-29)
					if (m_pEBDatas.get()!=NULL && m_pEBDatas->select("SELECT ext_data FROM eb_dep_info_t LIMIT 1")<0)
					{
						const char* pAlterTable = "ALTER TABLE eb_dep_info_t ADD ext_data int DEFAULT 0;";
						if (m_pEBDatas->execute(pAlterTable)==-1)
						{
							// 错误
							m_pEBDatas->close();
							m_pEBDatas.reset();
                            /// **删除文件，下次登录会重新创建
#ifdef _QT_MAKE_
                            QFile::remove(sCurrentLogonServerBoFile);
                            QFile::copy(sDefaultEBDatasFile,sCurrentLogonServerBoFile);
#else
                            DeleteFileA(sCurrentLogonServerBoFile.c_str());
                            CopyFileA(sDefaultEBDatasFile.c_str(),sCurrentLogonServerBoFile.c_str(),FALSE);
#endif
							m_pEBDatas = CSqliteCdbc::create();
#ifdef _QT_MAKE_
                            if (!m_pEBDatas->open(sCurrentLogonServerBoFile.toStdString().c_str()))
#else
							if (!m_pEBDatas->open(CEBParseSetting::str_convert(sCurrentLogonServerBoFile.c_str(),CP_ACP,CP_UTF8).c_str()))
#endif
							{
								m_pEBDatas.reset();
#ifdef _QT_MAKE_
                                QFile::remove(sCurrentLogonServerBoFile);
#else
                                DeleteFileA(sCurrentLogonServerBoFile.c_str());
#endif
							}
						}
					}
					//// ** 检查 eb_dep_info_t 表 1.19 版本代码，1.21 以后版本删除下面代码
					//if (m_pEBDatas.get()!=NULL && m_pEBDatas->select("SELECT manager_uid FROM eb_dep_info_t LIMIT 1")<0)
					//{
					//	const char* pAlterTable = "ALTER TABLE eb_dep_info_t ADD manager_uid bigint DEFAULT 0;";
					//	if (m_pEBDatas->execute(pAlterTable)==-1)
					//	{
					//		// 错误
					//		m_pEBDatas->close();
					//		m_pEBDatas.reset();
					//		// **删除文件，下次登录会重新创建
                    //		DeleteFileA(sCurrentLogonServerBoFile.c_str());
                    //		CopyFileA(sDefaultEBDatasFile.c_str(),sCurrentLogonServerBoFile.c_str(),FALSE);

					//		m_pEBDatas = CSqliteCdbc::create();
					//		if (!m_pEBDatas->open(CEBParseSetting::str_convert(sCurrentLogonServerBoFile.c_str(),CP_ACP,CP_UTF8).c_str()))
					//		{
					//			m_pEBDatas.reset();
                    //			DeleteFileA(sCurrentLogonServerBoFile.c_str());
					//		}
					//	}
					//}

					// 检查 eb_emo_info_t 表
					// *** 1.16 版本代码，1.18以后版本删除下面代码
					if (m_pEBDatas.get()!=NULL && m_pEBDatas->select("SELECT emo_id FROM eb_emo_info_t WHERE emo_id=0 LIMIT 1")<0)
					{
						const char* pCreateDatabaseTable = "CREATE TABLE eb_emo_info_t \
						( \
							emo_id BIGINT DEFAULT 0, \
							res_id BIGINT DEFAULT 0, \
							cm_server VARCHAR(64) DEFAULT '', \
							nindex INT DEFAULT 0, \
							ntype INT DEFAULT 0, \
							ent_id BIGINT DEFAULT 0, \
							res_type INT DEFAULT 0, \
							res_md5 VARCHAR(32) DEFAULT '', \
							desc VARCHAR(64) DEFAULT '', \
							cm_sid INT DEFAULT 0, \
							PRIMARY KEY(emo_id,res_id) \
						); \
						CREATE INDEX eb_emo_info_t_idx ON eb_emo_info_t(emo_id); \
						CREATE TABLE eb_emo_ver_t \
						( \
							emo_id BIGINT DEFAULT 0 PRIMARY KEY, \
							ver INT DEFAULT 0 \
						);";
						if (m_pEBDatas->execute(pCreateDatabaseTable)==-1)
						{
							// 错误
							m_pEBDatas->close();
							m_pEBDatas.reset();
							// **删除文件，下次登录会重新创建
#ifdef _QT_MAKE_
                            QFile::remove(sCurrentLogonServerBoFile);
                            QFile::copy(sDefaultEBDatasFile,sCurrentLogonServerBoFile);
#else
                            DeleteFileA(sCurrentLogonServerBoFile.c_str());
                            CopyFileA(sDefaultEBDatasFile.c_str(),sCurrentLogonServerBoFile.c_str(),FALSE);
#endif
							m_pEBDatas = CSqliteCdbc::create();
#ifdef _QT_MAKE_
                            if (!m_pEBDatas->open(sCurrentLogonServerBoFile.toStdString().c_str()))
#else
							if (!m_pEBDatas->open(CEBParseSetting::str_convert(sCurrentLogonServerBoFile.c_str(),CP_ACP,CP_UTF8).c_str()))
#endif
							{
								m_pEBDatas.reset();
#ifdef _QT_MAKE_
                                QFile::remove(sCurrentLogonServerBoFile);
#else
                                DeleteFileA(sCurrentLogonServerBoFile.c_str());
#endif
							}
						}
					}
					// ** 检查 eb_emo_info_t 表 cm_sid 字段，1.20 版本代码，1.22 以后版本删除下面代码
					if (m_pEBDatas.get()!=NULL && m_pEBDatas->select("SELECT cm_sid FROM eb_emo_info_t LIMIT 1")<0)
					{
						const char* pAlterTable = "ALTER TABLE eb_emo_info_t ADD cm_sid int DEFAULT 0;";
						if (m_pEBDatas->execute(pAlterTable)==-1)
						{
							// 错误
							m_pEBDatas->close();
							m_pEBDatas.reset();
							// **删除文件，下次登录会重新创建
#ifdef _QT_MAKE_
                            QFile::remove(sCurrentLogonServerBoFile);
                            QFile::copy(sDefaultEBDatasFile,sCurrentLogonServerBoFile);
#else
                            DeleteFileA(sCurrentLogonServerBoFile.c_str());
                            CopyFileA(sDefaultEBDatasFile.c_str(),sCurrentLogonServerBoFile.c_str(),FALSE);
#endif
							m_pEBDatas = CSqliteCdbc::create();
#ifdef _QT_MAKE_
                            if (!m_pEBDatas->open(sCurrentLogonServerBoFile.toStdString().c_str()))
#else
							if (!m_pEBDatas->open(CEBParseSetting::str_convert(sCurrentLogonServerBoFile.c_str(),CP_ACP,CP_UTF8).c_str()))
#endif
							{
								m_pEBDatas.reset();
#ifdef _QT_MAKE_
                                QFile::remove(sCurrentLogonServerBoFile);
#else
                                DeleteFileA(sCurrentLogonServerBoFile.c_str());
#endif
							}
						}
					}
				}
			}
		}

		if (m_pUserManager.get() != NULL)
		{
			m_pUserManager->Stop();
			m_pUserManager.reset();
		}
		bool bRetry = false;
		m_pUserManager = CPOPCUserManager::create(this,1);
		m_pUserManager->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
		int ret = m_pUserManager->Start(sServerAddress, "",0,true,true,1);
		if (ret==-1)
		{
			// 域名解析问题，再试一次；
			const tstring sServerAddressTemp = GetAddressList(sServerAddress);
			if (!sServerAddressTemp.empty())
			{
				m_pUserManager.reset();
				m_pUserManager = CPOPCUserManager::create(this,1);
				m_pUserManager->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
				ret = m_pUserManager->Start(sServerAddressTemp,"",0,true,true,1);
				bRetry = true;
			}
		}
		if (ret != 0)
		{
			m_userStatus = US_LogonError;
			m_pUserManager.reset();
			const EB_AccountInfo* pAccountInfo = m_pMyAccountInfo.get(); 
			if (m_callback)
				m_callback->onLogonError(*pAccountInfo,EB_STATE_ERROR);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
				pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_ERROR);
				pEvent->SetEventParameter((long)EB_STATE_ERROR);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_LOGON_ERROR,(WPARAM)pAccountInfo,EB_STATE_ERROR);
#endif
			}
			return;
		}
		if (!bRetry)
			SaveAddressList(sServerAddress);

		// get usersignid
		char sTempFile[260];
#ifdef _QT_MAKE_
        sprintf(sTempFile,"%s/ebc.temp",m_sAppDataTempPath.toLocal8Bit().constData());
#else
        sprintf(sTempFile,"%s\\ebc.temp",m_sAppDataTempPath.c_str());
#endif
		char lpszBuffer[60];
		memset(lpszBuffer,0,sizeof(lpszBuffer));
#ifdef WIN32
        ::GetPrivateProfileStringA(_T("temp"),_T("us_id"),NULL,lpszBuffer,sizeof(lpszBuffer),sTempFile);
#else
        ::GetPrivateProfileStringABoost(_T("temp"),_T("us_id"),NULL,lpszBuffer,sizeof(lpszBuffer),sTempFile);
#endif
		m_pMyAccountInfo->m_pLogonList.SetUserSignId("",cgc_atoi64(lpszBuffer));
		//m_pMyAccountInfo->m_dwAccountData |= EB_DATA_NEED_CHANGE_LINESTATE;

		int nRealLineState = nLineState;
		if (m_nUmServerState!=EB_SS_INIT)
		{
			// 重启登录，直接通知在线状态；
			//m_nUmServerState=EB_SS_INIT;
			//nRealLineState |= EB_USER_CHANGE_STATE;
			m_pMyAccountInfo->m_dwAccountData |= EB_DATA_NEED_CHANGE_LINESTATE;
		}
        m_pUserManager->SendUMSMOnline(m_nSDKVersion,nLogonType,m_pMyAccountInfo->GetUserId(),sOnlineKey,nRealLineState,0,m_pMyAccountInfo->m_pLogonList.GetUserSignId(""),m_sLogonUserData);
	}else if (m_tReLogonTime>0 && nResultValue==EB_STATE_APPID_KEY_ERROR)
	{
		m_sDevAppOnlineKey.clear();
		m_tReLogonTime = time(0)+5+(rand()%10);	// 重新登录错误，继续重新登录；
	}else
	{
		const EB_AccountInfo* pAccountInfo = m_pMyAccountInfo.get(); 
		if (m_callback!=NULL)
		{
			if (m_userStatus == US_DevAppIdLogoning)
				m_callback->onAppIdError((EB_STATE_CODE)nResultValue);
			else
				m_callback->onLogonError(*pAccountInfo,(EB_STATE_CODE)nResultValue);
		}
		if (m_pHwnd!=NULL) {
			if (m_userStatus == US_DevAppIdLogoning) {
#ifdef _QT_MAKE_
                EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_APPID_ERROR);
//				pEvent->SetQEventType((QEvent::Type)EB_WM_APPID_ERROR);
				pEvent->SetEventParameter((long)nResultValue);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_APPID_ERROR, nResultValue, 0);
#endif
			}
			else {
#ifdef _QT_MAKE_
				EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
				pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_ERROR);
				pEvent->SetEventParameter((long)nResultValue);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_LOGON_ERROR,(WPARAM)pAccountInfo,nResultValue);
#endif
			}
		}
		m_userStatus = US_LogonError;
		if (m_tReLogonTime>0 && nResultValue==EB_STATE_ACC_PWD_ERROR)
			m_tReLogonTime = 0;
	}
}

#ifdef _QT_MAKE_
static void MY_MMAudioVoiceCallBack(mycp::bigint nUserId, int nCallBackType, unsigned long lParam, unsigned int wParam, void* pUserData)
#else
static void MY_MMAudioVoiceCallBack(mycp::bigint nUserId, int nCallBackType, unsigned int lParam, unsigned int wParam, void* pUserData)
#endif
{
#ifdef USES_VIDEOROOM
	const tag_VideoStreamData * pAudiooStreamData = (const tag_VideoStreamData*)pUserData;
	if (pAudiooStreamData != NULL && pAudiooStreamData->pApp != NULL)
	{
		try
		{
			CUserManagerApp* pApp = (CUserManagerApp*)pAudiooStreamData->pApp;
			pApp->ProcessAudioStream(pAudiooStreamData->m_nCallId, nUserId, nCallBackType, lParam, wParam);
		}catch(std::exception&)
		{
		}catch(...)
		{}
	}
#endif
}
void CUserManagerApp::ProcessAudioStream(mycp::bigint nCallId, mycp::bigint nUserId, int nCallBackType, unsigned int lParam, unsigned int wParam)
{
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(nCallId, pCallInfo))
	{
#ifdef USES_VIDEOROOM
		if (nUserId==this->m_pMyAccountInfo->GetUserId())
        {
			BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
			if (pCallInfo->m_pVideoRoom.get()!=NULL && !pCallInfo->m_pVideoRoom->GetLocalAudioSend())	// 本地静音
                return;
		}
		void* pUserdwParam = 0;
		if (pCallInfo->m_pVideoUserParam.find(nUserId,pUserdwParam))
		{
			PEBVideoCallBack cb = (PEBVideoCallBack)pCallInfo->m_pAudioStreamData.pCallback;
			if (cb)
				cb(nUserId, nCallBackType, lParam, wParam, pUserdwParam);
		}
#endif
	}
}

void CUserManagerApp::SetVideoCallback(eb::bigint sCallId, PEBVideoCallBack cbAudio)
{
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
#ifdef USES_VIDEOROOM
		pCallInfo->m_pAudioStreamData.pApp = this;
        pCallInfo->m_pAudioStreamData.pCallback = (void*)cbAudio;
		pCallInfo->m_pAudioStreamData.m_nCallId = sCallId;
#endif
	}
}

void CUserManagerApp::CreateVideoRoom(const CEBCallInfo::pointer& pCallInfo)
{
	{
		// 设置视频服务器
		//m_pVideoRoom->SetVideoServer(L"183.61.237.142", 3101);
		// 3100:3500
		// 60.184.66.221:3100;60.184.66.221:3500
		// 183.61.237.142:3102;183.61.237.142:3202
		//sVideoServers = _T("60.184.66.221:3101;60.184.66.221:3501");
		tstring sRealVideoServer;
		tstring sRealAudioServer;
		const tstring sVideoServers = pCallInfo->m_sVideoServer;
		std::vector<tstring> pList;
		if (ParseString(sVideoServers,";",false,pList)==2)
		{
			sRealVideoServer = pList[0];
			sRealAudioServer = pList[1];
//#ifdef USES_VIDEO_EBMM
//#else
//			// video server
//			if (ParseString(sRealVideoServer.c_str(),":",pList)==2)
//			{
//				tstring sIp = pList[0];
//				//sIp = "test-lc.entboost.com";	// for test
//				const int nPort = atoi(pList[1].c_str());
//				sIp = entboost::GetHostIp(sIp.c_str(),"");
//				if (sIp.empty())
//				{
//					sIp = GetAddressList(sIp);
//					if (sIp.empty())
//						sIp = pList[0];
//				}
//
//				pCallInfo->m_pVideoRoom->VR_SetVideoServer(sIp.c_str(),nPort);
//			}
//			// audio server
//			if (ParseString(sRealAudioServer.c_str(),":",pList)==2)
//			{
//				tstring sIp = pList[0];
//				const int nPort = atoi(pList[1].c_str());
//				sIp = entboost::GetHostIp(sIp.c_str(),"");
//				if (sIp.empty())
//				{
//					sIp = GetAddressList(sIp);
//					if (sIp.empty())
//						sIp = pList[0];
//				}
//				pCallInfo->m_pVideoRoom->VR_SetAudioServer(sIp.c_str(),nPort);
//			}
//#endif
		}
		try
		{
#ifdef USES_VIDEOROOM

			BoostWriteLock wtLock(pCallInfo->m_mutexVideoRoom);
			if (pCallInfo->m_pVideoRoom.get() == NULL)
			{
#ifdef _QT_MAKE_
#else
                SetMicroBoostLevel(0.0);			/// 设置麦克风加强为0
                SetStereoMix(TRUE);					/// 设置混音器静音
#endif
				pCallInfo->m_pVideoRoom = Cebmm::create();
				pCallInfo->m_pVideoRoom->Start(sRealAudioServer.c_str(), sRealVideoServer.c_str(), 0);
				pCallInfo->m_pVideoRoom->SetCallBack(MY_MMAudioVoiceCallBack,&pCallInfo->m_pAudioStreamData);
				pCallInfo->m_pVideoRoom->Register(pCallInfo->GetCallId(), this->m_pMyAccountInfo->GetUserId(), pCallInfo->m_nVideoParam, pCallInfo->m_sGroupCode>0?true:false);
				pCallInfo->m_pVideoRoom->SetLocalVideoSend(true);
				pCallInfo->m_pVideoRoom->SetLocalAudioSend(true);
			}
#endif
		}catch(std::exception&)
		{
		}catch(...)
		{}
	}

}
#ifdef _QT_MAKE_
void CUserManagerApp::OpenUserVideo(const CEBCallInfo::pointer& pCallInfo,
                                    mycp::bigint nVideoUserId, bool bLocalUser,
                                    QWidget *hVideoWndParent, void* pAudioParam, bool& pOutVideoError)
#else
void CUserManagerApp::OpenUserVideo(const CEBCallInfo::pointer& pCallInfo,mycp::bigint nVideoUserId, bool bLocalUser,HWND hVideoWndParent, void* pAudioParam, bool& pOutVideoError)
#endif
{
	CreateVideoRoom(pCallInfo);
#ifdef USES_VIDEOROOM

	if (pAudioParam==0)
		pCallInfo->m_pVideoUserParam.remove(nVideoUserId);
	else
		pCallInfo->m_pVideoUserParam.insert(nVideoUserId,pAudioParam);
	try
	{
		BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
		if (bLocalUser)// && !m_pVideoDevices.empty())
		{
			if (pCallInfo->m_nVideoType == EB_VIDEO_AUDIO)
			{
				pCallInfo->m_pVideoRoom->InitAudioCapture();
			}else if (pCallInfo->m_nVideoType == EB_VIDEO_BOTH)
			{
				if (m_nLocalVideoIndex >= (int)m_pVideoDevices.size())
					m_nLocalVideoIndex = 0;
				if (!pCallInfo->m_pVideoRoom->InitVideoCapture(m_nLocalVideoIndex))
					pOutVideoError = true;
				pCallInfo->m_pVideoRoom->InitAudioCapture();
			}
		}
		if (!bLocalUser)
		{
			//pCallInfo->m_pVideoRoom->VR_AddMeLooks(nVideoUserId);
            if (pCallInfo->m_nVideoType == EB_VIDEO_AUDIO) {
				pCallInfo->m_pVideoRoom->AddAudioSink(nVideoUserId);
            }
            else if (pCallInfo->m_nVideoType == EB_VIDEO_BOTH) {
				pCallInfo->m_pVideoRoom->AddAudioSink(nVideoUserId);
				pCallInfo->m_pVideoRoom->AddVideoSink(nVideoUserId, hVideoWndParent);
                if (hVideoWndParent!=0) {
					pCallInfo->m_pDestUidVideo.insert(nVideoUserId, hVideoWndParent);
					pCallInfo->m_pVideoRoom->StartVideoGraph(nVideoUserId);
				}
			}
        }
        else if (bLocalUser)    /// && !m_pVideoDevices.empty())
		{
            if (pCallInfo->m_nVideoType == EB_VIDEO_AUDIO) {
				pCallInfo->m_pVideoRoom->StartAudioCapture();
            }
            else if (pCallInfo->m_nVideoType == EB_VIDEO_BOTH) {
				pCallInfo->m_pVideoRoom->StartVideoCapture(hVideoWndParent);
				pCallInfo->m_pVideoRoom->StartVideoGraph(nVideoUserId);
				pCallInfo->m_pVideoRoom->StartAudioCapture();
			}
		}
	}catch(std::exception&)
	{
	}catch(...)
	{}
#endif
}

bool CUserManagerApp::SetVideoOpenClose(const CEBCallInfo::pointer& pCallInfo,mycp::bigint nVideoUserId, bool bOpen)
{
#ifdef USES_VIDEOROOM

	BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
	if (pCallInfo->m_pVideoRoom.get()==NULL || pCallInfo->m_nVideoType!=EB_VIDEO_BOTH) return false;
	const bool bLocalUser = nVideoUserId==this->m_pMyAccountInfo->GetUserId();
	if (bLocalUser)
	{
		pCallInfo->m_pVideoRoom->SetLocalVideoSend(bOpen);
		pCallInfo->m_pVideoRoom->SetLocalVideoPlay(bOpen);
	}else
	{
		pCallInfo->m_pVideoRoom->SetSinkVideoPlay(nVideoUserId, bOpen);
		//if (bOpen)
		//{
		//	HWND hVideoWndParent = NULL;
		//	if (!pCallInfo->m_pDestUidVideo.find(nVideoUserId, hVideoWndParent))
		//		return false;
		//	pCallInfo->m_pVideoRoom->AddVideoSink(nVideoUserId, hVideoWndParent);
		//}else
		//{
		//	pCallInfo->m_pVideoRoom->DelVideoSink(nVideoUserId);
		//}
	}
#endif
	return true;
}
bool CUserManagerApp::SetAudioOpenClose(const CEBCallInfo::pointer& pCallInfo,mycp::bigint nVideoUserId, bool bOpen)
{
#ifdef USES_VIDEOROOM

	BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
	if (pCallInfo->m_pVideoRoom.get()==NULL || pCallInfo->m_nVideoType==EB_VIDEO_UNKNOWN) return false;
	const bool bLocalUser = nVideoUserId==this->m_pMyAccountInfo->GetUserId();
	if (bLocalUser)
	{
#ifdef _QT_MAKE_
#else
		if (bOpen)
			SetMicrophoneValue(FALSE,-1.0);		// 取消麦克风静音
#endif

		pCallInfo->m_pVideoRoom->SetLocalAudioPlay(bOpen);
		pCallInfo->m_pVideoRoom->SetLocalAudioSend(bOpen);
		//if (bOpen)
		//{
		//	pCallInfo->m_pVideoRoom->VR_AddAudioStream(nVideoUserId, (vr::AudioStreamCallBack)ProcessAudioStream, &pCallInfo->m_pAudioStreamData);
		//}else
		//{
		//	pCallInfo->m_pVideoRoom->VR_DelAudioStream(nVideoUserId);
		//}
	}else
	{
		// **OK
		pCallInfo->m_pVideoRoom->SetSinkAudioPlay(nVideoUserId,bOpen);
        if (bOpen) {
#ifdef _QT_MAKE_
#else
			SetSystemVolume(FALSE,-1.0);		// 取消播放器静音
#endif
			//pCallInfo->m_pVideoRoom->AddAudioSink(nVideoUserId);
		}else
		{
			//pCallInfo->m_pVideoRoom->DelAudioSink(nVideoUserId);
		}
	}
#endif
	return true;
}

void CUserManagerApp::DoVideoDisonnecte(eb::bigint sCallId, bool bSendVAck)
{
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		DoVideoDisonnecte(pCallInfo, bSendVAck);
	}
}

void CUserManagerApp::DoVideoDisonnecte(const CEBCallInfo::pointer& pCallInfo, bool bSendVAck)
{
#ifdef USES_VIDEOROOM
	pCallInfo->reset_video();
#endif
	//if (!bSendVAck) return;
	const bool bMyInVideo = pCallInfo->m_pVideoUserIdList.exist(m_pMyAccountInfo->GetUserId());
	if (bMyInVideo && pCallInfo->m_sGroupCode>0)
	{
		this->VideoAck(pCallInfo->GetCallId(),false,0);
	}
}
void CUserManagerApp::DoVideoConnected(const CEBCallInfo::pointer& pCallInfo)
{
#ifdef USES_VIDEOROOM

	BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
	if (pCallInfo->m_pVideoRoom.get() == NULL) return;
	std::vector<EB_UserVideoInfo> pVideoUserInfo;
	GetCallVideoUserInfo(pCallInfo,pVideoUserInfo);
	//pCallInfo->m_nLocalVideoId = this->m_pMyAccountInfo->GetUserId();

	// 生成上麦视频
	pCallInfo->m_nLocalOnVideo = false;
	for (size_t i=0; i<pVideoUserInfo.size(); i++)
	{
		if (!pVideoUserInfo[i].m_bOnVideo)
		{
			continue;
		}
		const mycp::bigint nVideoUserId = pVideoUserInfo[i].m_sUserAccount;
		const bool bLocalVideo = (bool)(nVideoUserId==m_pMyAccountInfo->GetUserId());
		if (bLocalVideo)
		{
			pCallInfo->m_nLocalOnVideo = true;
			pCallInfo->m_pVideoRoom->SetLocalVideoPlay(true);
		}else
		{
#ifdef _QT_MAKE_
            QWidget* hVideoWndParent = NULL;
#else
			HWND hVideoWndParent = NULL;
#endif
			if (pCallInfo->m_pDestUidVideo.find(nVideoUserId,hVideoWndParent))
			{
				pCallInfo->m_pVideoRoom->AddVideoSink(nVideoUserId, hVideoWndParent);
			}
		}
    }
#endif
}
#ifdef _QT_MAKE_
int CUserManagerApp::OpenLocalVideo(eb::bigint sCallId, QWidget *hVideoWndParent,void* pAudioParam)
#else
int CUserManagerApp::OpenLocalVideo(eb::bigint sCallId,HWND hVideoWndParent,void* pAudioParam)
#endif
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return -1;
	}
	//if (pCallInfo->m_nLocalVideoId == 0) return 0;
	if (pCallInfo->m_nVideoType == EB_VIDEO_UNKNOWN) return 0;
#ifdef _QT_MAKE_
    /// 使用统一线程处理
    EBUM_Event * event = new EBUM_Event(EBUM_Event::Event_Open_Video, QEvent::User);
    event->m_pCallInfo = pCallInfo;
    event->m_pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
    event->m_pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("user-id", m_pMyAccountInfo->GetUserId()));
    event->m_pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("video-parent", (mycp::bigint)hVideoWndParent));
    event->m_pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("audio-param", (mycp::bigint)pAudioParam));
    const unsigned long resultKey = (unsigned long)event+(unsigned long)hVideoWndParent;
    event->m_pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("result-key", (mycp::bigint)resultKey));
    QCoreApplication::postEvent(this, event);
    const long result = waitEventResult(resultKey,60,0);
    /// 返回1成功，10视频失败
    return result==1?1:10;
#else
    bool bVideoError = false;
	OpenUserVideo(pCallInfo,m_pMyAccountInfo->GetUserId(),true,hVideoWndParent,pAudioParam,bVideoError);
	if (bVideoError)
		return 10;
	return 1;
	//return pCallInfo->m_nLocalVideoId;
#endif
}
#ifdef _QT_MAKE_
bool CUserManagerApp::OpenUserVideo(eb::bigint sCallId,mycp::bigint sAccount, QWidget* hVideoWndParent,void* pAudioParam)
#else
bool CUserManagerApp::OpenUserVideo(eb::bigint sCallId,mycp::bigint sAccount,HWND hVideoWndParent,void* pAudioParam)
#endif
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return false;
	}
	//const bool nVideoUserId = pCallInfo->m_pVideoUserIdList[sAccount];
	if (!pCallInfo->m_pVideoUserIdList.exist(sAccount)) return false;
#ifdef _QT_MAKE_
    /// 使用统一线程处理
    EBUM_Event * event = new EBUM_Event(EBUM_Event::Event_Open_Video, QEvent::User);
    event->m_pCallInfo = pCallInfo;
    event->m_pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
    event->m_pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("user-id", sAccount));
    event->m_pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("video-parent", (mycp::bigint)hVideoWndParent));
    event->m_pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("audio-param", (mycp::bigint)pAudioParam));
    const unsigned long resultKey = (unsigned long)event+(unsigned long)hVideoWndParent;
    event->m_pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("result-key", (mycp::bigint)resultKey));
    QCoreApplication::postEvent(this, event);
    const long result = waitEventResult(resultKey,60,0);
    return result==1?true:false;
#else
	const bool bLocalUser = sAccount==this->m_pMyAccountInfo->GetUserId()?true:false;
	bool bVideoError = false;
	OpenUserVideo(pCallInfo,sAccount,bLocalUser,hVideoWndParent,pAudioParam,bVideoError);
	if (bVideoError)
		return false;
	//OpenUserVideo(pCallInfo,sAccount,false,cbAudio,pData);
	return true;
#endif
}
bool CUserManagerApp::SetVideoOpenClose(eb::bigint sCallId,mycp::bigint sAccount,bool bOpen)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return false;
	}
	//const bool nVideoUserId = pCallInfo->m_pVideoUserIdList[sAccount];
	//if (!nVideoUserId) return false;
	if (!pCallInfo->m_pVideoUserIdList.exist(sAccount)) return false;
	return SetVideoOpenClose(pCallInfo,sAccount,bOpen);
}
bool CUserManagerApp::SetAudioOpenClose(eb::bigint sCallId,mycp::bigint sAccount,bool bOpen)
{
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return false;
	}
	//const bool nVideoUserId = pCallInfo->m_pVideoUserIdList[sAccount];
	//if (!nVideoUserId) return false;
	if (!pCallInfo->m_pVideoUserIdList.exist(sAccount)) return false;
	return SetAudioOpenClose(pCallInfo,sAccount,bOpen);
}

//bool CUserManagerApp::OpenUserVideo(eb::bigint sCallId,int nUserVideoId,PAudioDataCallBack cbAudio,void* pData)
//{
//	CEBCallInfo::pointer pCallInfo;
//	if (!theCallInfoList.find(sCallId, pCallInfo))
//	{
//		return false;
//	}
//	OpenUserVideo(pCallInfo,nUserVideoId,false,cbAudio,pData);
//	return true;
//}

///////////////////////////////////////////
//// user manager
void CUserManagerApp::OnFuncReqResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner)
{

}
void CUserManagerApp::OnFuncAuthResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCUserManager* pUMOwner)
{

}

void CUserManagerApp::OnVersionCheckResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	if (nResultValue == EB_STATE_NEW_VERSION)
	{
		const tstring sServerVersion = pSotpResponseInfo->m_pResponseList.getParameterValue("server_ver","");
		const tstring sDescription = pSotpResponseInfo->m_pResponseList.getParameterValue("description","");
		const tstring sDownloadUrl = pSotpResponseInfo->m_pResponseList.getParameterValue("download_url","");
		const int nUpdateType = pSotpResponseInfo->m_pResponseList.getParameterValue("update_type",(int)0);
		const tstring sUpdateTime = pSotpResponseInfo->m_pResponseList.getParameterValue("update_time","");
		const tstring sCmServer = pSotpResponseInfo->m_pResponseList.getParameterValue("cm_server","");
		//const tstring sHttpServer = pSotpResponseInfo->m_pResponseList.getParameterValue("http_server","");	// * 参数有效
		//const tstring sCmAppName = pSotpResponseInfo->m_pResponseList.getParameterValue("cm_appname","");
		const mycp::bigint sResourceId = pSotpResponseInfo->m_pResponseList.getParameterValue("res_id",(mycp::bigint)0);
		const tstring sExt = pSotpResponseInfo->m_pResponseList.getParameterValue("ext","");
		const tstring sMd5 = pSotpResponseInfo->m_pResponseList.getParameterValue("md5","");

		const std::string::size_type find1 = m_pVersionInfo.m_sVersion.rfind(".");
		const std::string::size_type find2 = sServerVersion.rfind(".");
		if (find1!=std::string::npos && find2!=std::string::npos)
		{
			const int nClientVersion = atoi(m_pVersionInfo.m_sVersion.substr(find1+1).c_str());
			const int nServerVersion = atoi(sServerVersion.substr(find2+1).c_str());
			if (nClientVersion>=nServerVersion)
			{
				return;
			}
		}

#ifdef _QT_MAKE_
        QString sVersionResourceFile = QString("%1/%2").arg(m_sAppDataTempPath).arg(m_nDeployId);
		if (!QFileInfo::exists(sVersionResourceFile)) {
			QDir pDir;
            pDir.mkdir(sVersionResourceFile);
		}
        sVersionResourceFile = QString("%1/%2/%3%4").arg(m_sAppDataTempPath).arg(m_nDeployId).arg(sResourceId).arg(sExt.c_str());
#else
        char sVersionResourceFile[260];
        sprintf(sVersionResourceFile,"%s\\%lld",m_sAppDataTempPath.c_str(),m_nDeployId);
        if (!::PathFileExistsA(sVersionResourceFile)) {
            ::CreateDirectoryA(sVersionResourceFile, NULL);
		}
        sprintf(sVersionResourceFile,"%s\\%lld\\%lld%s",m_sAppDataTempPath.c_str(),m_nDeployId,sResourceId,sExt.c_str());
#endif
		if (!sServerVersion.empty() && m_pVersionInfo.m_sVersion != sServerVersion)
		{
			m_pVersionInfo.m_sVersion = sServerVersion;
			m_pVersionInfo.m_sDescription = sDescription;
			m_pVersionInfo.m_sDownloadUrl = sDownloadUrl;
			m_pVersionInfo.m_sUpdateTime = sUpdateTime;
			m_pVersionInfo.m_nUpdateType = nUpdateType;
			m_pVersionInfo.m_sVersionFile = sVersionResourceFile;
			m_pVersionInfo.m_sResId = sResourceId;
			m_pVersionInfo.m_sMD5 = sMd5;
#ifdef Q_OS_ANDROID
            EB_VersionInfo * pEvent = new EB_VersionInfo(m_pVersionInfo);
            pEvent->SetQEventType((QEvent::Type)EB_WM_NEW_VERSION);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
            return;
#endif

#ifdef _QT_MAKE_
            if (QFileInfo::exists(sVersionResourceFile))
#else
            if (::PathFileExistsA(sVersionResourceFile))
#endif
			{
				// 判断MD5
				mycp::bigint nFileSize = 0;
				tstring sFileMd5String;
				GetFileMd5(sVersionResourceFile,nFileSize,sFileMd5String);
				if (sFileMd5String!=sMd5)
				{
					// 删除后重新下载；
#ifdef _QT_MAKE_
                    QFile::remove(sVersionResourceFile);
#else
                    DeleteFileA(sVersionResourceFile);
#endif
				}
			}

#ifdef _QT_MAKE_
            if (!QFileInfo::exists(sVersionResourceFile))
#else
            if (!::PathFileExistsA(sVersionResourceFile))
#endif
			{
				// 资源文件不存在，请求下载
				const tstring sCmServerKey = GetCmServerKey(sCmServer);
				Cchatroom::pointer pChatRoom;
				if (!theChatRoomList.find(sCmServerKey,pChatRoom))
				{
					pChatRoom = Cchatroom::create();
					pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
					pChatRoom->Start(sCmServer.c_str(), "",0,true,1);
					pChatRoom->SetCallBack(this);
					theChatRoomList.insert(sCmServerKey,pChatRoom);
				}
				theResourceChatRoom.insert(sResourceId,pChatRoom);
				if (!pChatRoom->IsEnterOk())
					pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
                pChatRoom->RecviveResource(sResourceId,sVersionResourceFile);
			}else
			{
				// 资源文件已经存在，并且文件MD5正确，直接回调
				if (m_callback)
					m_callback->onNewVersion(m_pVersionInfo);
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					EB_VersionInfo * pEvent = new EB_VersionInfo(m_pVersionInfo);
					pEvent->SetQEventType((QEvent::Type)EB_WM_NEW_VERSION);
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
					::SendMessage(m_pHwnd, EB_WM_NEW_VERSION, (WPARAM)&m_pVersionInfo, 0);
#endif
				}
			}
		}
	}else if (nResultValue == EB_STATE_OK)
	{
		EB_VersionInfo* pVersionInfo = new EB_VersionInfo();
		pVersionInfo->m_nUpdateType = -1;
		if (m_callback)
			m_callback->onNewVersion(*pVersionInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			pVersionInfo->SetQEventType((QEvent::Type)EB_WM_NEW_VERSION);
            QCoreApplication::postEvent(m_pHwnd, pVersionInfo, thePostEventPriority);
#else
			::SendMessage(m_pHwnd, EB_WM_NEW_VERSION, (WPARAM)pVersionInfo, 0);
			delete pVersionInfo;
#endif
		}
		else {
			delete pVersionInfo;
		}

	}
}

void CUserManagerApp::OnVRequestResponse(const CPOPSotpRequestInfo::pointer &pSotpRequestInfo,
                                         const CPOPSotpResponseInfo::pointer &pSotpResponseInfo)
{
    if (m_pMyAccountInfo.get() == NULL) return;
    if (pSotpRequestInfo.get() == NULL) return;

    const mycp::bigint sCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("call-id",(mycp::bigint)0);
    const int nVideoType = pSotpRequestInfo->m_pRequestList.getParameterValue("v-type",(int)0);
    const tstring sVideoServer = pSotpResponseInfo->m_pResponseList.getParameterValue("v-server");
    const mycp::bigint nVideoParam = pSotpResponseInfo->m_pResponseList.getParameterValue("v-param",(mycp::bigint)0);
    //const int nVideoUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("vu-id",(int)0);
    int nResultValue = pSotpResponseInfo->GetResultValue();

    CEBCallInfo::pointer pCallInfo;
    if (!theCallInfoList.find(sCallId, pCallInfo))
    {
        return ;
    }
    CEBAccountInfo::pointer pFromAccountInfo;
    if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
    {
        return ;
    }

    //MessageBox(NULL,"OnVRequestResponse","",MB_OK);
    if (nResultValue == EB_STATE_OK)
    {
        //pCallInfo->m_nLocalVideoId = nVideoUserId;
        pCallInfo->m_sVideoServer = sVideoServer;
        pCallInfo->m_nVideoParam = nVideoParam;
        pCallInfo->m_nVideoType = (EB_VIDEO_TYPE)nVideoType;
        pCallInfo->m_pVideoUserIdList.insert(pFromAccountInfo->GetUserId(),true);
        pCallInfo->m_pOnVideoUserList.insert(pFromAccountInfo->GetUserId(),true);

        if (pCallInfo->m_sGroupCode==0)
        {
        }else
        {
            CreateVideoRoom(pCallInfo);
            DoVideoConnected(pCallInfo);
        }
    }

    EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
#ifdef _QT_MAKE_
    pVideoInfo->SetQEventType((QEvent::Type)EB_WM_V_REQUEST_RESPONSE);
    pVideoInfo->SetEventParameter((long)nResultValue);
#endif
    pVideoInfo->m_sCallId = sCallId;
    pVideoInfo->m_nVideoType = (EB_VIDEO_TYPE)nVideoType;
    if (m_callback)
        m_callback->onVRequestResponse(*pVideoInfo,(EB_STATE_CODE)nResultValue);

#ifdef _QT_MAKE_
    QObject* hWnd=NULL;
#else
    HWND hWnd=NULL;
#endif
    if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
        ::SendMessage(hWnd, EB_WM_V_REQUEST_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
        delete pVideoInfo;
#endif
    }
    else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
        ::SendMessage(m_pHwnd, EB_WM_V_REQUEST_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
        delete pVideoInfo;
#endif
    }
    else {
        delete pVideoInfo;
    }
}

void CUserManagerApp::OnVRequestResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get() == NULL) return;
	if (pSotpRequestInfo.get() == NULL) return;
#ifdef _QT_MAKE_
    /// 使用统一线程处理
    EBUM_Event * event = new EBUM_Event(EBUM_Event::Event_VRequestResponse, QEvent::User);
    event->m_pSotpRequestInfo = pSotpRequestInfo;
    event->m_pSotpResponseInfo = pSotpResponseInfo;
    QCoreApplication::postEvent(this, event);
#else
	const mycp::bigint sCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("call-id",(mycp::bigint)0);
	const int nVideoType = pSotpRequestInfo->m_pRequestList.getParameterValue("v-type",(int)0);
	const tstring sVideoServer = pSotpResponseInfo->m_pResponseList.getParameterValue("v-server");
	const mycp::bigint nVideoParam = pSotpResponseInfo->m_pResponseList.getParameterValue("v-param",(mycp::bigint)0);
	//const int nVideoUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("vu-id",(int)0);
	int nResultValue = pSotpResponseInfo->GetResultValue();

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return ;
	}

	//MessageBox(NULL,"OnVRequestResponse","",MB_OK);
	if (nResultValue == EB_STATE_OK)
	{
		//pCallInfo->m_nLocalVideoId = nVideoUserId;
		pCallInfo->m_sVideoServer = sVideoServer;
		pCallInfo->m_nVideoParam = nVideoParam;
		pCallInfo->m_nVideoType = (EB_VIDEO_TYPE)nVideoType;
		pCallInfo->m_pVideoUserIdList.insert(pFromAccountInfo->GetUserId(),true);
		pCallInfo->m_pOnVideoUserList.insert(pFromAccountInfo->GetUserId(),true);

		if (pCallInfo->m_sGroupCode==0)
		{
		}else
		{
			CreateVideoRoom(pCallInfo);
			DoVideoConnected(pCallInfo);
		}
	}

	EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
#ifdef _QT_MAKE_
	pVideoInfo->SetQEventType((QEvent::Type)EB_WM_V_REQUEST_RESPONSE);
	pVideoInfo->SetEventParameter((long)nResultValue);
#endif
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nVideoType = (EB_VIDEO_TYPE)nVideoType;
	if (m_callback)
		m_callback->onVRequestResponse(*pVideoInfo,(EB_STATE_CODE)nResultValue);

#ifdef _QT_MAKE_
	QObject* hWnd=NULL;
#else
	HWND hWnd=NULL;
#endif
	if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(hWnd, EB_WM_V_REQUEST_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
		delete pVideoInfo;
#endif
	}
	else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(m_pHwnd, EB_WM_V_REQUEST_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
		delete pVideoInfo;
#endif
	}
	else {
		delete pVideoInfo;
    }
#endif
}

void CUserManagerApp::OnVAckResponse(const CPOPSotpRequestInfo::pointer &pSotpRequestInfo,
                                     const CPOPSotpResponseInfo::pointer &pSotpResponseInfo)
{
    if (m_pMyAccountInfo.get() == NULL) return;
    if (pSotpRequestInfo.get() == NULL) return ;
    const mycp::bigint nToUserId = pSotpRequestInfo->m_pRequestList.getParameterValue("to-uid",(mycp::bigint)0);
    const mycp::bigint sCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("call-id",(mycp::bigint)0);
    const int nAckType = pSotpRequestInfo->m_pRequestList.getParameterValue("ack-type",(int)0);
    const tstring sVideoServer = pSotpResponseInfo->m_pResponseList.getParameterValue("v-server");
    const mycp::bigint nVideoParam = pSotpResponseInfo->m_pResponseList.getParameterValue("v-param",(mycp::bigint)0);
    //const int nVideoUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("vu-id",(int)0);
    int nResultValue = pSotpResponseInfo->GetResultValue();

    CEBCallInfo::pointer pCallInfo;
    if (!theCallInfoList.find(sCallId, pCallInfo))
    {
        return ;
    }
    CEBAccountInfo::pointer pFromAccountInfo;
    if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
    {
        return ;
    }
    if (nResultValue == EB_STATE_OK && nAckType==EB_CAT_ACCEPT)
    {
        pCallInfo->m_sVideoServer = sVideoServer;
        pCallInfo->m_nVideoParam = nVideoParam;
        pCallInfo->m_pVideoUserIdList.insert(pFromAccountInfo->GetUserId(),true);
        if (pCallInfo->m_sGroupCode==0)
        {
            pCallInfo->m_pOnVideoUserList.insert(pFromAccountInfo->GetUserId(),true);
        }
        CreateVideoRoom(pCallInfo);
        DoVideoConnected(pCallInfo);

        //BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
        //if (pCallInfo->m_sGroupCode>0 && pCallInfo->m_pVideoRoom!=NULL && nToUserId>0)
        //{
        //	//char lpBuffer[64];
        //	//sprintf(lpBuffer,"打开群组成员视频：uid=%lld",nToUserId);
        //	//MessageBox(NULL, lpBuffer,"",MB_OK);
        //	pCallInfo->m_pVideoRoom->VR_AddMeLooks(nToUserId);
        //	// 这里需要先打开，等上层收到数据后，再打开界面和音频
        //	pCallInfo->m_pVideoRoom->VR_AddVideoStream(nToUserId, (DWORD)&pCallInfo->m_pVideoStreamData, false);
        //}
    }else if (pCallInfo->m_sGroupCode>0 && nAckType==2 && nToUserId!=EB_CAT_TIMEOUT && !pCallInfo->m_pOnVideoUserList.exist(m_pMyAccountInfo->GetUserId()))
    {
        /// 退出群组视频，要清空所有视频数据，否则会有异常
        pCallInfo->m_pOnVideoUserList.clear();
        pCallInfo->m_pVideoUserIdList.clear();
        DoVideoDisonnecte(pCallInfo,false);
    }
    if (nAckType==EB_CAT_ACCEPT)
    {
        SendConnectMsg(pCallInfo, pFromAccountInfo->GetUserId(),pFromAccountInfo->GetAccount(),0);	// ?
        // 接受才发送窗口消息
        EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
#ifdef _QT_MAKE_
        pVideoInfo->SetQEventType((QEvent::Type)EB_WM_V_ACK_RESPONSE);
        pVideoInfo->SetEventParameter((long)nResultValue);
#endif
        pVideoInfo->m_sCallId = sCallId;
        pVideoInfo->m_nVideoType = pCallInfo->m_nVideoType;
        if (m_callback)
            m_callback->onVAckResponse(*pVideoInfo,(EB_STATE_CODE)nResultValue);

#ifdef _QT_MAKE_
        QObject* hWnd=NULL;
#else
        HWND hWnd=NULL;
#endif
        if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
            ::SendMessage(hWnd, EB_WM_V_ACK_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
            delete pVideoInfo;
#endif
        }
        else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
            ::SendMessage(m_pHwnd, EB_WM_V_ACK_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
            delete pVideoInfo;
#endif
        }
        else {
            delete pVideoInfo;
        }
    }
}

void CUserManagerApp::OnVAckResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get() == NULL) return;
	if (pSotpRequestInfo.get() == NULL) return ;
#ifdef _QT_MAKE_
    /// 使用统一线程处理
    EBUM_Event * event = new EBUM_Event(EBUM_Event::Event_VAckResponse, QEvent::User);
    event->m_pSotpRequestInfo = pSotpRequestInfo;
    event->m_pSotpResponseInfo = pSotpResponseInfo;
    QCoreApplication::postEvent(this, event);
#else
	const mycp::bigint nToUserId = pSotpRequestInfo->m_pRequestList.getParameterValue("to-uid",(mycp::bigint)0);
	const mycp::bigint sCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("call-id",(mycp::bigint)0);
	const int nAckType = pSotpRequestInfo->m_pRequestList.getParameterValue("ack-type",(int)0);
	const tstring sVideoServer = pSotpResponseInfo->m_pResponseList.getParameterValue("v-server");
	const mycp::bigint nVideoParam = pSotpResponseInfo->m_pResponseList.getParameterValue("v-param",(mycp::bigint)0);
	//const int nVideoUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("vu-id",(int)0);
	int nResultValue = pSotpResponseInfo->GetResultValue();

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return ;
	}
	if (nResultValue == EB_STATE_OK && nAckType==EB_CAT_ACCEPT)
	{
		pCallInfo->m_sVideoServer = sVideoServer;
		pCallInfo->m_nVideoParam = nVideoParam;
		pCallInfo->m_pVideoUserIdList.insert(pFromAccountInfo->GetUserId(),true);
		if (pCallInfo->m_sGroupCode==0)
		{
			pCallInfo->m_pOnVideoUserList.insert(pFromAccountInfo->GetUserId(),true);
		}
		CreateVideoRoom(pCallInfo);
		DoVideoConnected(pCallInfo);

		//BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
		//if (pCallInfo->m_sGroupCode>0 && pCallInfo->m_pVideoRoom!=NULL && nToUserId>0)
		//{
		//	//char lpBuffer[64];
		//	//sprintf(lpBuffer,"打开群组成员视频：uid=%lld",nToUserId);
		//	//MessageBox(NULL, lpBuffer,"",MB_OK);
		//	pCallInfo->m_pVideoRoom->VR_AddMeLooks(nToUserId);
		//	// 这里需要先打开，等上层收到数据后，再打开界面和音频
		//	pCallInfo->m_pVideoRoom->VR_AddVideoStream(nToUserId, (DWORD)&pCallInfo->m_pVideoStreamData, false);
		//}
	}else if (pCallInfo->m_sGroupCode>0 && nAckType==2 && nToUserId!=EB_CAT_TIMEOUT && !pCallInfo->m_pOnVideoUserList.exist(m_pMyAccountInfo->GetUserId()))
	{
        /// 退出群组视频，要清空所有视频数据，否则会有异常
		pCallInfo->m_pOnVideoUserList.clear();
		pCallInfo->m_pVideoUserIdList.clear();
		DoVideoDisonnecte(pCallInfo,false);
	}
	if (nAckType==EB_CAT_ACCEPT)
	{
		SendConnectMsg(pCallInfo, pFromAccountInfo->GetUserId(),pFromAccountInfo->GetAccount(),0);	// ?
		// 接受才发送窗口消息
		EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
#ifdef _QT_MAKE_
		pVideoInfo->SetQEventType((QEvent::Type)EB_WM_V_ACK_RESPONSE);
		pVideoInfo->SetEventParameter((long)nResultValue);
#endif
		pVideoInfo->m_sCallId = sCallId;
		pVideoInfo->m_nVideoType = pCallInfo->m_nVideoType;
		if (m_callback)
			m_callback->onVAckResponse(*pVideoInfo,(EB_STATE_CODE)nResultValue);

#ifdef _QT_MAKE_
		QObject* hWnd=NULL;
#else
		HWND hWnd=NULL;
#endif
		if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(hWnd, EB_WM_V_ACK_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
			delete pVideoInfo;
#endif
		}
		else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(m_pHwnd, EB_WM_V_ACK_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
			delete pVideoInfo;
#endif
		}
		else {
			delete pVideoInfo;
		}
	}
#endif
}

void CUserManagerApp::OnRDRequestResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get() == NULL) return;
    if (pSotpRequestInfo.get() == NULL) return;
	const mycp::bigint sCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("call-id",(mycp::bigint)0);
	const int nVideoType = pSotpRequestInfo->m_pRequestList.getParameterValue("rd-type",(int)0);
	const tstring sRDServer = pSotpResponseInfo->m_pResponseList.getParameterValue("rd-server");
	const mycp::bigint nRDParam = pSotpResponseInfo->m_pResponseList.getParameterValue("rd-param",(mycp::bigint)0);
	int nResultValue = pSotpResponseInfo->GetResultValue();

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return ;
	}

	if (nResultValue == EB_STATE_OK)
	{
		pCallInfo->m_nRemoteDesktopParam = nRDParam;
		pCallInfo->m_sRDServer = sRDServer;
		pCallInfo->m_nRDType = (EB_REMOTE_DESKTOP_TYPE)nVideoType;
		pCallInfo->m_pRDUserIdList.insert(pFromAccountInfo->GetUserId(),true,false);
		if (pCallInfo->m_nRDType==EB_RD_DESKTOP_SRC)
			pCallInfo->m_pOnRDUserList.insert(pFromAccountInfo->GetUserId(),true,false);

		if (pCallInfo->m_sGroupCode==0)
		{
		}else
		{
			// 
			CreateDesktop(pCallInfo);
			//DoRDConnected(pCallInfo);
		}
	}

	EB_RemoteDesktopInfo* pVideoInfo = new EB_RemoteDesktopInfo();
#ifdef _QT_MAKE_
	pVideoInfo->SetQEventType((QEvent::Type)EB_WM_RD_REQUEST_RESPONSE);
	pVideoInfo->SetEventParameter((long)nResultValue);
#endif
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nRDType = (EB_REMOTE_DESKTOP_TYPE)nVideoType;
	pVideoInfo->m_nFromUserId = m_pMyAccountInfo->GetUserId();
	if (pCallInfo->m_nRDType==EB_RD_DESKTOP_SRC)
		pVideoInfo->m_bOnDesktop = true;
	if (m_callback)
		m_callback->onRDRequestResponse(*pVideoInfo,(EB_STATE_CODE)nResultValue);

#ifdef _QT_MAKE_
	QObject* hWnd=NULL;
#else
	HWND hWnd=NULL;
#endif
	if (m_pDesktopHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(hWnd, EB_WM_RD_REQUEST_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
		delete pVideoInfo;
#endif
	}else if (m_pHwnd != NULL) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(m_pHwnd, EB_WM_RD_REQUEST_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
		delete pVideoInfo;
#endif
	}
	else {
		delete pVideoInfo;
    }
}
void CUserManagerApp::OnRDAckResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get() == NULL) return;
	if (pSotpRequestInfo.get() == NULL) return ;
	const mycp::bigint nToUserId = pSotpRequestInfo->m_pRequestList.getParameterValue("to-uid",(mycp::bigint)0);
	const mycp::bigint sCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("call-id",(mycp::bigint)0);
	const int nAckType = pSotpRequestInfo->m_pRequestList.getParameterValue("ack-type",(int)0);
	const tstring sRDServer = pSotpResponseInfo->m_pResponseList.getParameterValue("rd-server");
	const mycp::bigint nRDParam = pSotpResponseInfo->m_pResponseList.getParameterValue("rd-param",(mycp::bigint)0);
	int nResultValue = pSotpResponseInfo->GetResultValue();

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(),pFromAccountInfo))
	{
		return ;
	}
	if (nResultValue == EB_STATE_OK && nAckType==EB_CAT_ACCEPT)
	{
		pCallInfo->m_nRemoteDesktopParam = nRDParam;
		pCallInfo->m_sRDServer = sRDServer;
		pCallInfo->m_pRDUserIdList.insert(pFromAccountInfo->GetUserId(),true,false);
		if (pCallInfo->m_sGroupCode==0 && pCallInfo->m_nRDType==EB_RD_DESKTOP_DEST)
		{
			pCallInfo->m_pOnRDUserList.insert(pFromAccountInfo->GetUserId(),true,false);
		}
		CreateDesktop(pCallInfo);
		//DoRDConnected(pCallInfo);
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
		if (pCallInfo->m_sGroupCode==0 && pCallInfo->m_nRDType==EB_RD_DESKTOP_DEST && pCallInfo->m_pEBRemoteDesktop!=NULL)
		{
			pCallInfo->m_pEBRemoteDesktop->RD_EnableControlLocalDesktop = nToUserId;
		}
#else
		if (pCallInfo->m_sGroupCode==0 && pCallInfo->m_nRDType==EB_RD_DESKTOP_DEST && pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
		{
			pCallInfo->m_pEBRemoteDesktop->EnableControlLocalDesktop(nToUserId);
		}
#endif
#endif
	}
	if (nAckType==EB_CAT_ACCEPT)
	{
		// 接受才发送窗口消息
		SendConnectMsg(pCallInfo, pFromAccountInfo->GetUserId(),pFromAccountInfo->GetAccount(),0);	// ?

		EB_RemoteDesktopInfo * pVideoInfo = new EB_RemoteDesktopInfo();
#ifdef _QT_MAKE_
		pVideoInfo->SetQEventType((QEvent::Type)EB_WM_RD_ACK_RESPONSE);
		pVideoInfo->SetEventParameter((long)nResultValue);
#endif
		pVideoInfo->m_sCallId = sCallId;
		pVideoInfo->m_nRDType = pCallInfo->m_nRDType;
		pVideoInfo->m_nFromUserId = nToUserId;
		if (pCallInfo->m_nRDType==EB_RD_DESKTOP_SRC)
			pVideoInfo->m_bOnDesktop = true;
		if (m_callback)
			m_callback->onRDAckResponse(*pVideoInfo,(EB_STATE_CODE)nResultValue);
#ifdef _QT_MAKE_
		QObject* hWnd=NULL;
#else
		HWND hWnd=NULL;
#endif
		if (m_pDesktopHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(hWnd, EB_WM_RD_ACK_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
			delete pVideoInfo;
#endif
		}
		else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(m_pHwnd, EB_WM_RD_ACK_RESPONSE, (WPARAM)pVideoInfo, nResultValue);
			delete pVideoInfo;
#endif
		}
		else {
			delete pVideoInfo;
		}
	}else if (pCallInfo->m_sGroupCode==0)
	{
		pCallInfo->m_nRDType = EB_RD_UNKNOWN;
		pCallInfo->m_pRDUserIdList.clear();
		pCallInfo->m_pOnRDUserList.clear();
	}
}

void CUserManagerApp::SendConnectMsg(const CEBCallInfo::pointer& pCallInfo,mycp::bigint nFromUserId,const tstring& sFromAccount, int nConnectFlag, bool bSetCallStateConnection)
{
	//if ((nConnectFlag&EB_CONNECTED_AUTO_ACK)==EB_CONNECTED_AUTO_ACK && pCallInfo->m_sGroupCode>0)
	//{
	//	// 清空自动应答群组
	//	theOwnerCallGroup.remove(pCallInfo->m_sGroupCode);
	//}
	if (pCallInfo->m_nCallState != EB_CALL_STATE_CONNECTED || (nConnectFlag&EB_CONNECTED_OWNER_CALL)==EB_CONNECTED_OWNER_CALL || (nConnectFlag&EB_CONNECTED_MSG)==EB_CONNECTED_MSG)
	{
		//theAcceptUser.insert(nFromUserId,true);
		tstring sCallFromAccount(sFromAccount);
		if (nFromUserId==m_pMyAccountInfo->GetUserId()) {
			BoostReadLock rdLock(pCallInfo->m_pCallUserList.mutex());
			CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter = pCallInfo->m_pCallUserList.begin();
			for (; pIter!=pCallInfo->m_pCallUserList.end(); pIter++) {
				const CEBAccountInfo::pointer& pFromAccountInfo = pIter->second;
				if (pFromAccountInfo->GetUserId() != m_pMyAccountInfo->GetUserId()) {
					nFromUserId = pFromAccountInfo->GetUserId();
					sCallFromAccount = pFromAccountInfo->GetAccount();
					break;
				}
			}
		}
		else if (sCallFromAccount.empty()) {
			CEBAccountInfo::pointer pFromAccountInfo;
			if (pCallInfo->m_pCallUserList.find(nFromUserId,pFromAccountInfo)) {
				sCallFromAccount = pFromAccountInfo->GetAccount();
			}
		}
		if (bSetCallStateConnection)
			pCallInfo->m_nCallState = EB_CALL_STATE_CONNECTED;	// 这个必须放前面，因为后面有可能会调用退出窗口；
		EB_CallInfo* pConnectInfo = new EB_CallInfo(nFromUserId,sCallFromAccount,pCallInfo->GetCallId(),pCallInfo->m_sGroupCode);
		if (pCallInfo->m_sOldCallId != pCallInfo->GetCallId())
			pConnectInfo->m_sOldCallId = pCallInfo->m_sOldCallId;
		if (pConnectInfo->m_sOldCallId>0) {
			nConnectFlag &= ~EB_CONNECTED_AUTO_ACK;
			nConnectFlag |= EB_CONNECTED_OWNER_CALL;
		}

		if (m_callback!=NULL)
			m_callback->onCallConnected(*pConnectInfo,nConnectFlag);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			pConnectInfo->SetQEventType((QEvent::Type)EB_WM_CALL_CONNECTED);
			pConnectInfo->SetEventParameter((long)nConnectFlag);
            QCoreApplication::postEvent(m_pHwnd, pConnectInfo, thePostEventPriority);
#else
			::SendMessage(m_pHwnd, EB_WM_CALL_CONNECTED,(WPARAM)pConnectInfo,(LPARAM)nConnectFlag);
			delete pConnectInfo;
#endif
		}
		else {
			delete pConnectInfo;
		}
	}
}

size_t CUserManagerApp::GetEntGroupSize(void) const
{
	size_t result = 0;
	AUTO_CONST_RLOCK(theDepartmentList);
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
	for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++)
	{
		CEBGroupInfo::pointer pEntDepartmentInfo = pIterDepartment->second;
		if (pEntDepartmentInfo->m_sEnterpriseCode>0)
			result++;
	}
	return result;
}
size_t CUserManagerApp::GetUnEntGroupSize(void) const
{
	size_t result = 0;
	AUTO_CONST_RLOCK(theDepartmentList);
	CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
	for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++)
	{
		CEBGroupInfo::pointer pEntDepartmentInfo = pIterDepartment->second;
		if (pEntDepartmentInfo->m_sEnterpriseCode==0)
			result++;
	}
	return result;
}
void CUserManagerApp::LoadLocalDepInfo(mycp::bigint nGroupId,mycp::bigint nServerGroupVer, bool bNeedLoadGroupInfo)
{
	//if (999121==nGroupId)
	//	int i=0;
	BoostReadLock rdLock(m_mutexEBDatas);
	if (m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[512];
		sprintf(lpszBuffer,"SELECT group_id,dep_name,parent_gid,ent_id,description,phone,fax,email,url,address,create_time,create_uid,type,ver,my_emp_id,emp_count,display_index,manager_uid,ext_data FROM eb_dep_info_t WHERE group_id=%lld",nGroupId);
		int nCookie = 0;
		m_pEBDatas->select(lpszBuffer, nCookie);
		CEBGroupInfo::pointer pDepartmentInfo;
		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		if (pRecord.get()!=0)
		{
			const mycp::bigint nLocalGroupId = pRecord->getVector()[0]->getBigIntValue();
			const tstring sLocalGroupName(pRecord->getVector()[1]->getStrValue());
			const mycp::bigint nLocalParentGroupId = pRecord->getVector()[2]->getBigIntValue();
			const mycp::bigint nLocalEntId = pRecord->getVector()[3]->getBigIntValue();
			tstring sLocalDescription(pRecord->getVector()[4]->getStrValue());
			CSqliteCdbc::escape_string_out(sLocalDescription);
			const tstring sLocalPhone(pRecord->getVector()[5]->getStrValue());
			const tstring sLocalFax(pRecord->getVector()[6]->getStrValue());
			const tstring sLocalEmail(pRecord->getVector()[7]->getStrValue());
			const tstring sLocalUrl(pRecord->getVector()[8]->getStrValue());
			tstring sLocalAddress(pRecord->getVector()[9]->getStrValue());
			CSqliteCdbc::escape_string_out(sLocalAddress);
			const tstring sLocalCreateTime(pRecord->getVector()[10]->getStrValue());
			const mycp::bigint nLocalCreateUid = pRecord->getVector()[11]->getBigIntValue();
			const int nLocalType = pRecord->getVector()[12]->getIntValue();
			const mycp::bigint nLocalGroupVer = pRecord->getVector()[13]->getBigIntValue();
			const mycp::bigint nLocalMyEmpId = pRecord->getVector()[14]->getBigIntValue();
			const int nLocalEmpCount = pRecord->getVector()[15]->getIntValue();
			const int nDisplayIndex = pRecord->getVector()[16]->getIntValue();
			const mycp::bigint nManagerUserId = pRecord->getVector()[17]->getBigIntValue();
			const int nExtData = pRecord->getVector()[18]->getIntValue();

			if (!theDepartmentList.find(nLocalGroupId, pDepartmentInfo))
			{
				pDepartmentInfo = CEBGroupInfo::create(nLocalEntId, nLocalGroupId);
				theDepartmentList.insert(nLocalGroupId, pDepartmentInfo);
			}
			pDepartmentInfo->m_sGroupName = sLocalGroupName;
			pDepartmentInfo->m_sParentCode = nLocalParentGroupId;
			pDepartmentInfo->m_sDescription = sLocalDescription;
			pDepartmentInfo->m_sPhone = sLocalPhone;
			pDepartmentInfo->m_sFax = sLocalFax;
			pDepartmentInfo->m_sEmail = sLocalEmail;
			pDepartmentInfo->m_sUrl = sLocalUrl;
			pDepartmentInfo->m_sAddress = sLocalAddress;
			pDepartmentInfo->m_sCreateTime = sLocalCreateTime;
			pDepartmentInfo->m_nCreateUserId = nLocalCreateUid;
			pDepartmentInfo->m_nManagerUserId = nManagerUserId;
			pDepartmentInfo->m_nGroupType = (EB_GROUP_TYPE)nLocalType;
			pDepartmentInfo->m_nMyEmpId = nLocalMyEmpId;
			pDepartmentInfo->m_nEmpCount = max(0,nLocalEmpCount);
			pDepartmentInfo->m_nGroupVer = nLocalGroupVer;
			pDepartmentInfo->m_nDisplayIndex = nDisplayIndex;
			pDepartmentInfo->m_nGroupExtData = nExtData;
			if (nLocalParentGroupId>0 && !theDepartmentList.exist(nLocalParentGroupId))
			{
				// 暂时找不到上一级部门
				theTempDepartmentList.insert(nLocalParentGroupId, pDepartmentInfo);
			}
		}
		m_pEBDatas->reset(nCookie);
		if (pDepartmentInfo.get()!=NULL)
		{
			pDepartmentInfo->m_nServerGroupVer = nServerGroupVer;	// 记录下服务器最新的版本号；
			if (pDepartmentInfo->m_nGroupVer!=nServerGroupVer)
			{
				ProcessGroupInfo(pDepartmentInfo);	// 这里只是为了通知群组消息而已；
				if (bNeedLoadGroupInfo)
					theNeedLoadGroupInfoList.add(nGroupId);		// 加载一次部门数据
				// 部门版本不一致，需要重新加载；
				if (pDepartmentInfo->m_nMyEmpId>0)	// ** 属于我的部门，加载成员
					theNeedLoadGroupMemberList.pushfront(nGroupId);
				else
				{
					const int nLocalGroupMemberSize = GetLocalGroupMemberSize(nGroupId);
					if (nLocalGroupMemberSize>0)	// 本地已经有员工信息，需要重新加载
						theNeedLoadGroupMemberList.add(nGroupId);	// ** 加载
					//else if (bNeedLoadGroupInfo)
					//	theNeedLoadGroupInfoList.add(nGroupId);		// 加载一次部门数据
				}

				//// 部门版本不一致，需要重新加载成员；
				//if (pDepartmentInfo->m_nMyEmpId==0)
				//	theNeedLoadGroupMemberList.add(nGroupId);
				//else
				//	theNeedLoadGroupMemberList.pushfront(nGroupId);
			}else if (pDepartmentInfo->m_sParentCode==0 || theDepartmentList.exist(pDepartmentInfo->m_sParentCode))
			{
				ProcessGroupInfo(pDepartmentInfo);
				// 加载一次在线用户状态
				if (!pDepartmentInfo->m_pMemberList.empty())
				//if (!pDepartmentInfo->m_pMemberList.empty() && pDepartmentInfo->m_nMyEmpId>0)
				{
					pDepartmentInfo->m_nGroupData |= EB_GROUP_DATA_LOAD_MEMBER_STATE;
					if (pDepartmentInfo->m_nMyEmpId==0)
						theNeedLoadOLSList.add(nGroupId);
					else
						theNeedLoadOLSList.pushfront(nGroupId);
				}
			}
		}else if (bNeedLoadGroupInfo)
		{
			// 本地没有部门数据，加载一次部门数据
			theNeedLoadGroupInfoList.add(nGroupId);
			//this->EnterpriseLoad(nGroupId,1,1,0);
		}
	}
}

void CUserManagerApp::OnFVRequest(const CPOPSotpRequestInfo::pointer &pReqeustInfo,
                                  const CPOPSotpResponseInfo::pointer &pSotpResponseInfo)
{
    if (m_pMyAccountInfo.get() == NULL) return;

    const mycp::bigint sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
    const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
    const int nVideoType = pSotpResponseInfo->m_pResponseList.getParameterValue("v-type",(int)0);
    //const int nVideoUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("vu-id",(int)0);

    CEBCallInfo::pointer pCallInfo;
    if (!theCallInfoList.find(sCallId, pCallInfo))
    {
        return ;
    }
    CEBAccountInfo::pointer pFromAccountInfo;
    if (!pCallInfo->m_pCallUserList.find(sFromAccount,pFromAccountInfo))
    {
        return ;
    }
    pCallInfo->m_nVideoType = (EB_VIDEO_TYPE)nVideoType;
    pCallInfo->m_pVideoUserIdList.insert(pFromAccountInfo->GetUserId(),true);
    pCallInfo->m_pOnVideoUserList.insert(pFromAccountInfo->GetUserId(),true);

    //MessageBox(NULL,"OnFVRequest","",MB_OK);
    SendConnectMsg(pCallInfo, pFromAccountInfo->GetUserId(),pFromAccountInfo->GetAccount(),0);	// ?

    if (pCallInfo->m_sGroupCode==0)
    {
    }else
    {
        //if (this->GetVideoCount()>0)
        //{
        //	// 自动接收视频
        //	OnBnClickedButtonVideoAccept();
        //}
    }

    EB_UserVideoInfo pUserVideoInfo(sFromAccount,true);
    EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
#ifdef _QT_MAKE_
    pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_REQUEST);
    pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif
    pVideoInfo->m_sCallId = sCallId;
    pVideoInfo->m_nVideoType = pCallInfo->m_nVideoType;
    if (m_callback)
        m_callback->onVideoRequest(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
    QObject* hWnd=NULL;
#else
    HWND hWnd=NULL;
#endif
    if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
        ::SendMessage(hWnd, EB_WM_VIDEO_REQUEST, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
        delete pVideoInfo;
#endif
    }
    else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
        ::SendMessage(m_pHwnd, EB_WM_VIDEO_REQUEST, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
        delete pVideoInfo;
#endif
    }
    else {
        delete pVideoInfo;
    }
}

void CUserManagerApp::OnFVRequest(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo,
                                  const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,
                                  const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get() == NULL) return;
#ifdef _QT_MAKE_
    /// 使用统一线程处理
    EBUM_Event * event = new EBUM_Event(EBUM_Event::Event_FVRequest, QEvent::User);
    event->m_pSotpRequestInfo = pSotpRequestInfo;
    event->m_pSotpResponseInfo = pSotpResponseInfo;
    QCoreApplication::postEvent(this, event);
#else
	const mycp::bigint sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	const int nVideoType = pSotpResponseInfo->m_pResponseList.getParameterValue("v-type",(int)0);
	//const int nVideoUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("vu-id",(int)0);

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(sFromAccount,pFromAccountInfo))
	{
		return ;
	}
	pCallInfo->m_nVideoType = (EB_VIDEO_TYPE)nVideoType;
	pCallInfo->m_pVideoUserIdList.insert(pFromAccountInfo->GetUserId(),true);
	pCallInfo->m_pOnVideoUserList.insert(pFromAccountInfo->GetUserId(),true);

	//MessageBox(NULL,"OnFVRequest","",MB_OK);
	SendConnectMsg(pCallInfo, pFromAccountInfo->GetUserId(),pFromAccountInfo->GetAccount(),0);	// ?

	if (pCallInfo->m_sGroupCode==0)
	{
	}else
	{
		//if (this->GetVideoCount()>0)
		//{
		//	// 自动接收视频
		//	OnBnClickedButtonVideoAccept();
		//}
	}

	EB_UserVideoInfo pUserVideoInfo(sFromAccount,true);
	EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
#ifdef _QT_MAKE_
    pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_REQUEST);
    pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nVideoType = pCallInfo->m_nVideoType;
	if (m_callback)
		m_callback->onVideoRequest(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
	QObject* hWnd=NULL;
#else
	HWND hWnd=NULL;
#endif
	if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
		::SendMessage(hWnd, EB_WM_VIDEO_REQUEST, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
		delete pVideoInfo;
#endif
	}
	else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
		::SendMessage(m_pHwnd, EB_WM_VIDEO_REQUEST, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
		delete pVideoInfo;
#endif
	}
	else {
		delete pVideoInfo;
	}
#endif
}

void CUserManagerApp::OnFVAck(const CPOPSotpRequestInfo::pointer &pReqeustInfo,
                              const CPOPSotpResponseInfo::pointer &pSotpResponseInfo)
{
    if (m_pMyAccountInfo.get() == NULL) return;

    const mycp::bigint sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
    const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
    const int nAckType = pSotpResponseInfo->m_pResponseList.getParameterValue("ack-type",(int)0);
    const tstring sVideoServer = pSotpResponseInfo->m_pResponseList.getParameterValue("v-server");
    const mycp::bigint nVideoParam = pSotpResponseInfo->m_pResponseList.getParameterValue("v-param",(mycp::bigint)0);
    //const int nVideoUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("vu-id",(int)0);

    CEBCallInfo::pointer pCallInfo;
    if (!theCallInfoList.find(sCallId, pCallInfo))
    {
        return ;
    }
    CEBAccountInfo::pointer pFromAccountInfo;
    if (!pCallInfo->m_pCallUserList.find(sFromAccount,pFromAccountInfo))
    {
        return ;
    }
    //MessageBox(NULL,"OnFVAck","",MB_OK);
    if (!pCallInfo->m_pOnVideoUserList.exist(this->m_pMyAccountInfo->GetUserId()))
    {
        // 本人未上视频；
        return;
    }

    const bool bFromOnVideo = pCallInfo->m_pOnVideoUserList.exist(sFromAccount);
    EB_UserVideoInfo pUserVideoInfo(sFromAccount,bFromOnVideo);
    EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
    pVideoInfo->m_sCallId = sCallId;
    pVideoInfo->m_nVideoType = pCallInfo->m_nVideoType;
    if (nAckType == EB_CAT_ACCEPT) {
        // accept
        pCallInfo->m_sVideoServer = sVideoServer;
        pCallInfo->m_nVideoParam = nVideoParam;
        pCallInfo->m_pVideoUserIdList.insert(pFromAccountInfo->GetUserId(),true);
        if (pCallInfo->m_sGroupCode==0)
        {
            pCallInfo->m_pOnVideoUserList.insert(pFromAccountInfo->GetUserId(),true);
        }
        if (pCallInfo->m_sGroupCode==0)
        {
            CreateVideoRoom(pCallInfo);
            DoVideoConnected(pCallInfo);
        }else
        {
            // 有人打开我的视频；
            //BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
            //if (pCallInfo->m_pVideoRoom.get()!=NULL)
            //{
            //	//MessageBox(NULL,"有人打开我的视频","",MB_OK);
            //	//pCallInfo->m_pVideoRoom->VR_AddLookMes(sFromAccount);
            //}
        }

        SendConnectMsg(pCallInfo, pFromAccountInfo->GetUserId(),pFromAccountInfo->GetAccount(),0);	// ?
        if (m_callback)
            m_callback->onVideoAccept(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
        pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_ACCEPT);
        //pConnectInfo->SetEventParameter((long)nConnectFlag);
        pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif

#ifdef _QT_MAKE_
        QObject* hWnd=NULL;
#else
        HWND hWnd=NULL;
#endif
        if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
            postWaitEventResult(m_pHwnd, pVideoInfo);
#else
            ::SendMessage(hWnd, EB_WM_VIDEO_ACCEPT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
            delete pVideoInfo;
#endif
        }
        else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            postWaitEventResult(m_pHwnd, pVideoInfo);
#else
            ::SendMessage(m_pHwnd, EB_WM_VIDEO_ACCEPT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
            delete pVideoInfo;
#endif
        }
        else {
            delete pVideoInfo;
        }
    }
    else {
        // cancel
        if (pCallInfo->m_sGroupCode==0) {
        }
        else {
            // 有人关闭我的视频；
//#ifdef USES_VIDEO_EBMM
//#else
//			if (pCallInfo->m_pVideoRoom)
//			{
//				pCallInfo->m_pVideoRoom->VR_DelLookMes(sFromAccount);
//			}
//#endif
        }

        if (nAckType == EB_CAT_TIMEOUT) {
            if (m_callback)
                m_callback->onVideoTimeout(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
            pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_TIMEOUT);
            pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif

#ifdef _QT_MAKE_
            QObject* hWnd=NULL;
#else
            HWND hWnd=NULL;
#endif
            if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
                postWaitEventResult(m_pHwnd, pVideoInfo);
#else
                ::SendMessage(hWnd, EB_WM_VIDEO_TIMEOUT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
                delete pVideoInfo;
#endif
            }
            else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                postWaitEventResult(m_pHwnd, pVideoInfo);
#else
                ::SendMessage(m_pHwnd, EB_WM_VIDEO_TIMEOUT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
                delete pVideoInfo;
#endif
            }
            else {
                delete pVideoInfo;
            }

        }else
        {
            if (m_callback)
                m_callback->onVideoReject(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
            pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_REJECT);
            pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif

#ifdef _QT_MAKE_
            QObject* hWnd=NULL;
#else
            HWND hWnd=NULL;
#endif
            if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
                postWaitEventResult(m_pHwnd, pVideoInfo);
#else
                ::SendMessage(hWnd, EB_WM_VIDEO_REJECT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
                delete pVideoInfo;
#endif
            }
            else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                postWaitEventResult(m_pHwnd, pVideoInfo);
#else
                ::SendMessage(m_pHwnd, EB_WM_VIDEO_REJECT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
                delete pVideoInfo;
#endif
            }
            else {
                delete pVideoInfo;
            }
        }
        pCallInfo->m_pVideoUserIdList.remove(pFromAccountInfo->GetUserId());
    }
}
void CUserManagerApp::OnFVAck(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo,
                              const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,
                              const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get() == NULL) return;
#ifdef _QT_MAKE_
    /// 使用统一线程处理
    EBUM_Event * event = new EBUM_Event(EBUM_Event::Event_FVAck, QEvent::User);
    event->m_pSotpRequestInfo = pSotpRequestInfo;
    event->m_pSotpResponseInfo = pSotpResponseInfo;
    QCoreApplication::postEvent(this, event);
#else
	const mycp::bigint sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	const int nAckType = pSotpResponseInfo->m_pResponseList.getParameterValue("ack-type",(int)0);
	const tstring sVideoServer = pSotpResponseInfo->m_pResponseList.getParameterValue("v-server");
	const mycp::bigint nVideoParam = pSotpResponseInfo->m_pResponseList.getParameterValue("v-param",(mycp::bigint)0);
	//const int nVideoUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("vu-id",(int)0);

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(sFromAccount,pFromAccountInfo))
	{
		return ;
	}
	//MessageBox(NULL,"OnFVAck","",MB_OK);
	if (!pCallInfo->m_pOnVideoUserList.exist(this->m_pMyAccountInfo->GetUserId()))
	{
		// 本人未上视频；
		return;
	}

	const bool bFromOnVideo = pCallInfo->m_pOnVideoUserList.exist(sFromAccount);
	EB_UserVideoInfo pUserVideoInfo(sFromAccount,bFromOnVideo);
	EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nVideoType = pCallInfo->m_nVideoType;
	if (nAckType == EB_CAT_ACCEPT) {
		// accept
		pCallInfo->m_sVideoServer = sVideoServer;
		pCallInfo->m_nVideoParam = nVideoParam;
		pCallInfo->m_pVideoUserIdList.insert(pFromAccountInfo->GetUserId(),true);
		if (pCallInfo->m_sGroupCode==0)
		{
			pCallInfo->m_pOnVideoUserList.insert(pFromAccountInfo->GetUserId(),true);
		}
		if (pCallInfo->m_sGroupCode==0)
		{
            CreateVideoRoom(pCallInfo);
			DoVideoConnected(pCallInfo);
		}else
		{
			// 有人打开我的视频；
			//BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
			//if (pCallInfo->m_pVideoRoom.get()!=NULL)
			//{
			//	//MessageBox(NULL,"有人打开我的视频","",MB_OK);
			//	//pCallInfo->m_pVideoRoom->VR_AddLookMes(sFromAccount);
			//}
		}

		SendConnectMsg(pCallInfo, pFromAccountInfo->GetUserId(),pFromAccountInfo->GetAccount(),0);	// ?
		if (m_callback)
			m_callback->onVideoAccept(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
		pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_ACCEPT);
		//pConnectInfo->SetEventParameter((long)nConnectFlag);
		pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif

#ifdef _QT_MAKE_
		QObject* hWnd=NULL;
#else
		HWND hWnd=NULL;
#endif
		if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
            postWaitEventResult(m_pHwnd, pVideoInfo);
#else
			::SendMessage(hWnd, EB_WM_VIDEO_ACCEPT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
			delete pVideoInfo;
#endif
		}
		else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            postWaitEventResult(m_pHwnd, pVideoInfo);
#else
			::SendMessage(m_pHwnd, EB_WM_VIDEO_ACCEPT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
			delete pVideoInfo;
#endif
		}
		else {
			delete pVideoInfo;
		}
	}
	else {
		// cancel
		if (pCallInfo->m_sGroupCode==0) {
		}
		else {
			// 有人关闭我的视频；
//#ifdef USES_VIDEO_EBMM
//#else
//			if (pCallInfo->m_pVideoRoom)
//			{
//				pCallInfo->m_pVideoRoom->VR_DelLookMes(sFromAccount);
//			}
//#endif
		}

		if (nAckType == EB_CAT_TIMEOUT) {
			if (m_callback)
				m_callback->onVideoTimeout(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
			pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_TIMEOUT);
			pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif

#ifdef _QT_MAKE_
			QObject* hWnd=NULL;
#else
			HWND hWnd=NULL;
#endif
			if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
                postWaitEventResult(m_pHwnd, pVideoInfo);
#else
				::SendMessage(hWnd, EB_WM_VIDEO_TIMEOUT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
				delete pVideoInfo;
#endif
			}
			else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                postWaitEventResult(m_pHwnd, pVideoInfo);
#else
				::SendMessage(m_pHwnd, EB_WM_VIDEO_TIMEOUT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
				delete pVideoInfo;
#endif
			}
			else {
				delete pVideoInfo;
			}

		}else
		{
			if (m_callback)
				m_callback->onVideoReject(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
			pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_REJECT);
			pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif

#ifdef _QT_MAKE_
			QObject* hWnd=NULL;
#else
			HWND hWnd=NULL;
#endif
			if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
                postWaitEventResult(m_pHwnd, pVideoInfo);
#else
				::SendMessage(hWnd, EB_WM_VIDEO_REJECT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
				delete pVideoInfo;
#endif
			}
			else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                postWaitEventResult(m_pHwnd, pVideoInfo);
#else
				::SendMessage(m_pHwnd, EB_WM_VIDEO_REJECT, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
				delete pVideoInfo;
#endif
			}
			else {
				delete pVideoInfo;
			}
		}
		pCallInfo->m_pVideoUserIdList.remove(pFromAccountInfo->GetUserId());
	}
#endif
}

void CUserManagerApp::OnFVEnd(const CPOPSotpRequestInfo::pointer &pReqeustInfo,
                              const CPOPSotpResponseInfo::pointer &pSotpResponseInfo)
{
    const mycp::bigint sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
    const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
    CEBCallInfo::pointer pCallInfo;
    if (!theCallInfoList.find(sCallId, pCallInfo))
    {
        return ;
    }
    CEBAccountInfo::pointer pFromAccountInfo;
    if (!pCallInfo->m_pCallUserList.find(sFromAccount,pFromAccountInfo))
    {
        return ;
    }
#ifdef USES_VIDEOROOM
    pCallInfo->m_pVideoUserParam.remove(pFromAccountInfo->GetUserId());

    /// **需要先停止视频流播放
    if (pCallInfo->m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
    {
        // 一对一会话，或者本端关闭
        BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
        if (pCallInfo->m_pVideoRoom.get()!=NULL)
        {
            pCallInfo->m_pVideoRoom->SetLocalVideoPlay(false);
            pCallInfo->m_pVideoRoom->SetLocalAudioPlay(false);
            pCallInfo->m_pVideoRoom->SetSinkVideoPlay(sFromAccount,false);
            pCallInfo->m_pVideoRoom->SetSinkAudioPlay(sFromAccount,false);
        }
    }else
    {
        // 群组，有人退出视频；
        pCallInfo->m_pDestUidVideo.remove(sFromAccount);
        pCallInfo->m_pVideoUserIdList.remove(sFromAccount);
        BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
        if (pCallInfo->m_pVideoRoom.get()!=NULL)
        {
            pCallInfo->m_pVideoRoom->SetSinkVideoPlay(sFromAccount,false);
            pCallInfo->m_pVideoRoom->SetSinkAudioPlay(sFromAccount,false);
        }
    }
#endif
    // ** 放在后面处理，避免关闭时，视频窗口灰色；
    //if (pCallInfo->m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
    //{
    //	// 一对一会话，或者本端关闭
    //	DoVideoDisonnecte(pCallInfo);
    //	pCallInfo->m_pVideoUserIdList.clear();
    //}else
    //{
    //	pCallInfo->m_pVideoUserIdList.remove(pFromAccountInfo->GetUserId());
    //	// 群组，有人退出视频；
    //	if (pCallInfo->m_pVideoUserIdList.empty())
    //	{
    //		DoVideoDisonnecte(pCallInfo);
    //	}
    //}

    const bool bFromOnVideo = pCallInfo->m_pOnVideoUserList.exist(sFromAccount);
    EB_UserVideoInfo pUserVideoInfo(sFromAccount,bFromOnVideo);
    EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
#ifdef _QT_MAKE_
    pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_CLOSE);
    pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif
    pVideoInfo->m_sCallId = sCallId;
    pVideoInfo->m_nVideoType = pCallInfo->m_nVideoType;
    if (m_callback)
        m_callback->onVideoClose(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
    QObject* hWnd=NULL;
#else
    HWND hWnd=NULL;
#endif
    if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
        ::SendMessage(hWnd, EB_WM_VIDEO_CLOSE, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
        delete pVideoInfo;
#endif
    }
    else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
        ::SendMessage(m_pHwnd, EB_WM_VIDEO_CLOSE, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
        delete pVideoInfo;
#endif
    }
    else {
        delete pVideoInfo;
    }

    if (pCallInfo->m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
    {
        /// 一对一会话，或者本端关闭
//#ifdef _QT_MAKE_
//        /// 由外面主界面线程去处理
//#else
        DoVideoDisonnecte(pCallInfo, true);
        pCallInfo->m_pVideoUserIdList.clear();
//#endif
    }else
    {
        // 群组，有人退出视频；
#ifdef USES_VIDEOROOM

        {
            BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
            if (pCallInfo->m_pVideoRoom.get()!=NULL)
            {
                pCallInfo->m_pVideoRoom->DelAudioSink(sFromAccount);
                pCallInfo->m_pVideoRoom->DelVideoSink(sFromAccount);
            }
        }
#endif
        //pCallInfo->m_pDestUidVideo.remove(sFromAccount);
        //pCallInfo->m_pVideoUserIdList.remove(sFromAccount);
        if (pCallInfo->m_pVideoUserIdList.empty())
        {
            DoVideoDisonnecte(pCallInfo, true);
        }
    }
    pCallInfo->m_pOnVideoUserList.remove(pFromAccountInfo->GetUserId());
}

void CUserManagerApp::OnFVEnd(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo,
                              const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,
                              const CPOPCUserManager* pUMOwner)
{
#ifdef _QT_MAKE_
    /// 使用统一线程处理
    EBUM_Event * event = new EBUM_Event(EBUM_Event::Event_FVEnd, QEvent::User);
    event->m_pSotpRequestInfo = pSotpRequestInfo;
    event->m_pSotpResponseInfo = pSotpResponseInfo;
    QCoreApplication::postEvent(this, event);
#else
	const mycp::bigint sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(sFromAccount,pFromAccountInfo))
	{
		return ;
	}
#ifdef USES_VIDEOROOM
	pCallInfo->m_pVideoUserParam.remove(pFromAccountInfo->GetUserId());

    /// **需要先停止视频流播放
	if (pCallInfo->m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
	{
		// 一对一会话，或者本端关闭
		BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
		if (pCallInfo->m_pVideoRoom.get()!=NULL)
		{
			pCallInfo->m_pVideoRoom->SetLocalVideoPlay(false);
			pCallInfo->m_pVideoRoom->SetLocalAudioPlay(false);
			pCallInfo->m_pVideoRoom->SetSinkVideoPlay(sFromAccount,false);
			pCallInfo->m_pVideoRoom->SetSinkAudioPlay(sFromAccount,false);
		}
	}else
	{
		// 群组，有人退出视频；
		pCallInfo->m_pDestUidVideo.remove(sFromAccount);
		pCallInfo->m_pVideoUserIdList.remove(sFromAccount);
		BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
		if (pCallInfo->m_pVideoRoom.get()!=NULL)
		{
			pCallInfo->m_pVideoRoom->SetSinkVideoPlay(sFromAccount,false);
			pCallInfo->m_pVideoRoom->SetSinkAudioPlay(sFromAccount,false);
		}
	}
#endif
	// ** 放在后面处理，避免关闭时，视频窗口灰色；
	//if (pCallInfo->m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
	//{
	//	// 一对一会话，或者本端关闭
	//	DoVideoDisonnecte(pCallInfo);
	//	pCallInfo->m_pVideoUserIdList.clear();
	//}else
	//{
	//	pCallInfo->m_pVideoUserIdList.remove(pFromAccountInfo->GetUserId());
	//	// 群组，有人退出视频；
	//	if (pCallInfo->m_pVideoUserIdList.empty())
	//	{
	//		DoVideoDisonnecte(pCallInfo);
	//	}
	//}

	const bool bFromOnVideo = pCallInfo->m_pOnVideoUserList.exist(sFromAccount);
	EB_UserVideoInfo pUserVideoInfo(sFromAccount,bFromOnVideo);
	EB_VideoInfo * pVideoInfo = new EB_VideoInfo();
#ifdef _QT_MAKE_
	pVideoInfo->SetQEventType((QEvent::Type)EB_WM_VIDEO_CLOSE);
	pVideoInfo->SetEventData((void*)&pUserVideoInfo);
#endif
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nVideoType = pCallInfo->m_nVideoType;
	if (m_callback)
		m_callback->onVideoClose(*pVideoInfo,pUserVideoInfo);

#ifdef _QT_MAKE_
	QObject* hWnd=NULL;
#else
	HWND hWnd=NULL;
#endif
	if (m_pVideoHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
		::SendMessage(hWnd, EB_WM_VIDEO_CLOSE, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
		delete pVideoInfo;
#endif
	}
	else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        postWaitEventResult(m_pHwnd, pVideoInfo);
#else
		::SendMessage(m_pHwnd, EB_WM_VIDEO_CLOSE, (WPARAM)pVideoInfo, (LPARAM)&pUserVideoInfo);
		delete pVideoInfo;
#endif
	}
	else {
		delete pVideoInfo;
	}

	if (pCallInfo->m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
	{
        /// 一对一会话，或者本端关闭
//#ifdef _QT_MAKE_
//        /// 由外面主界面线程去处理
//#else
        DoVideoDisonnecte(pCallInfo, true);
        pCallInfo->m_pVideoUserIdList.clear();
//#endif
	}else
	{
		// 群组，有人退出视频；
#ifdef USES_VIDEOROOM

		{
			BoostReadLock rdLock(pCallInfo->m_mutexVideoRoom);
			if (pCallInfo->m_pVideoRoom.get()!=NULL)
			{
				pCallInfo->m_pVideoRoom->DelAudioSink(sFromAccount);
				pCallInfo->m_pVideoRoom->DelVideoSink(sFromAccount);
			}
		}
#endif
		//pCallInfo->m_pDestUidVideo.remove(sFromAccount);
		//pCallInfo->m_pVideoUserIdList.remove(sFromAccount);
		if (pCallInfo->m_pVideoUserIdList.empty())
		{
			DoVideoDisonnecte(pCallInfo, true);
		}
	}
	pCallInfo->m_pOnVideoUserList.remove(pFromAccountInfo->GetUserId());
#endif
}

void CUserManagerApp::OnFRDRequest(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get() == NULL) return;
	const mycp::bigint sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	const int nVideoType = pSotpResponseInfo->m_pResponseList.getParameterValue("rd-type",(int)0);
	//const int nVideoUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("vu-id",(int)0);

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(sFromAccount,pFromAccountInfo))
	{
		return ;
	}
	pCallInfo->m_nRDType = (EB_REMOTE_DESKTOP_TYPE)nVideoType;
	pCallInfo->m_pRDUserIdList.insert(pFromAccountInfo->GetUserId(),true,false);
	if (pCallInfo->m_nRDType == EB_RD_DESKTOP_SRC)
		pCallInfo->m_pOnRDUserList.insert(pFromAccountInfo->GetUserId(),true,false);

	SendConnectMsg(pCallInfo, pFromAccountInfo->GetUserId(),pFromAccountInfo->GetAccount(),0);	// ?

	//if (pCallInfo->m_sGroupCode==0)
	//{
	//}else
	//{
	//	//if (this->GetVideoCount()>0)
	//	//{
	//	//	// 自动接收视频
	//	//	OnBnClickedButtonVideoAccept();
	//	//}
	//}

	EB_RemoteDesktopInfo * pVideoInfo = new EB_RemoteDesktopInfo();
#ifdef _QT_MAKE_
	pVideoInfo->SetQEventType((QEvent::Type)EB_WM_RD_REQUEST);
#endif
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nRDType = pCallInfo->m_nRDType;
	pVideoInfo->m_nFromUserId = sFromAccount;
	if (pCallInfo->m_nRDType == EB_RD_DESKTOP_SRC)
		pVideoInfo->m_bOnDesktop = true;
	if (m_callback)
		m_callback->onRDRequest(*pVideoInfo);

#ifdef _QT_MAKE_
	QObject* hWnd=NULL;
#else
	HWND hWnd=NULL;
#endif
	if (m_pDesktopHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(hWnd, EB_WM_RD_REQUEST, (WPARAM)pVideoInfo, (LPARAM)0);
		delete pVideoInfo;
#endif
	}
	else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(m_pHwnd, EB_WM_RD_REQUEST, (WPARAM)pVideoInfo, (LPARAM)0);
		delete pVideoInfo;
#endif
	}
	else {
		delete pVideoInfo;
	}
}
void CUserManagerApp::OnFRDAck(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get() == NULL) return;
	const mycp::bigint sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	const int nAckType = pSotpResponseInfo->m_pResponseList.getParameterValue("ack-type",(int)0);
	const tstring sRDServer = pSotpResponseInfo->m_pResponseList.getParameterValue("rd-server");
	const mycp::bigint nRDParam = pSotpResponseInfo->m_pResponseList.getParameterValue("rd-param",(mycp::bigint)0);

	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(sFromAccount,pFromAccountInfo))
	{
		return ;
	}
	if ((pCallInfo->m_sGroupCode==0 && !pCallInfo->m_pRDUserIdList.exist(this->m_pMyAccountInfo->GetUserId())) ||
		(pCallInfo->m_sGroupCode>0 && !pCallInfo->m_pOnRDUserList.exist(this->m_pMyAccountInfo->GetUserId())))
	{
		// 本人未上视频；
		return;
	}

	const bool bFromOnVideo = pCallInfo->m_pOnRDUserList.exist(sFromAccount);
	EB_RemoteDesktopInfo * pVideoInfo = new EB_RemoteDesktopInfo();
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nRDType = pCallInfo->m_nRDType;
	pVideoInfo->m_nFromUserId = sFromAccount;
	pVideoInfo->m_bOnDesktop = bFromOnVideo;
	if (nAckType == EB_CAT_ACCEPT)
	{
		// accept
		pCallInfo->m_nRemoteDesktopParam = nRDParam;
		pCallInfo->m_sRDServer = sRDServer;
		pCallInfo->m_pRDUserIdList.insert(pFromAccountInfo->GetUserId(),true,false);
		if (pCallInfo->m_sGroupCode==0 && pCallInfo->m_nRDType==EB_RD_DESKTOP_DEST)
		{
			pCallInfo->m_pOnRDUserList.insert(pFromAccountInfo->GetUserId(),true,false);
		}
		if (pCallInfo->m_sGroupCode==0)
		{
			CreateDesktop(pCallInfo);
			//DoRDConnected(pCallInfo);
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
			if (pCallInfo->m_nRDType==EB_RD_DESKTOP_SRC && pCallInfo->m_pEBRemoteDesktop != NULL)
			{
				pCallInfo->m_pEBRemoteDesktop->RD_EnableControlLocalDesktop = sFromAccount;
			}
#else
			if (pCallInfo->m_nRDType==EB_RD_DESKTOP_SRC && pCallInfo->m_pEBRemoteDesktop.get() != NULL)
			{
				pCallInfo->m_pEBRemoteDesktop->EnableControlLocalDesktop(sFromAccount);
			}
#endif
#endif
		}else
		{
			//// 有人打开我的视频；
			//if (pCallInfo->m_pEBRemoteDesktop)
			//{
			//	//MessageBox(NULL,"有人打开我的视频","",MB_OK);
			//	pCallInfo->m_pEBRemoteDesktop->VR_AddLookMes(sFromAccount);
			//}
		}

		SendConnectMsg(pCallInfo, pFromAccountInfo->GetUserId(),pFromAccountInfo->GetAccount(),0);	// ?
		if (m_callback)
			m_callback->onRDAccept(*pVideoInfo);

#ifdef _QT_MAKE_
		pVideoInfo->SetQEventType((QEvent::Type)EB_WM_RD_ACCEPT);
		QObject* hWnd=NULL;
#else
		HWND hWnd=NULL;
#endif
		if (m_pDesktopHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(hWnd, EB_WM_RD_ACCEPT, (WPARAM)pVideoInfo, (LPARAM)0);
			delete pVideoInfo;
#endif
		}
		else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(m_pHwnd, EB_WM_RD_ACCEPT, (WPARAM)pVideoInfo, (LPARAM)0);
			delete pVideoInfo;
#endif
		}
		else {
			delete pVideoInfo;
		}
	}else
	{
		// cancel
		if (pCallInfo->m_sGroupCode==0)
		{
			pCallInfo->m_nRDType = EB_RD_UNKNOWN;
			pCallInfo->m_pRDUserIdList.clear();
			pCallInfo->m_pOnRDUserList.clear();
		}else
		{
			// 有人关闭我的视频；
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
			if (pCallInfo->m_pEBRemoteDesktop!=NULL)
			{
				pCallInfo->m_pEBRemoteDesktop->RD_DelSink(sFromAccount);
			}
#else
			if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
			{
				pCallInfo->m_pEBRemoteDesktop->DelSink(sFromAccount);
			}
#endif
#endif
			pCallInfo->m_pRDUserIdList.remove(pFromAccountInfo->GetUserId());
		}

		if (m_callback)
			m_callback->onRDReject(*pVideoInfo);

#ifdef _QT_MAKE_
		pVideoInfo->SetQEventType((QEvent::Type)EB_WM_RD_REJECT);
		QObject* hWnd=NULL;
#else
		HWND hWnd=NULL;
#endif
		if (m_pDesktopHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(hWnd, EB_WM_RD_REJECT, (WPARAM)pVideoInfo, (LPARAM)0);
			delete pVideoInfo;
#endif
		}
		else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
			::SendMessage(m_pHwnd, EB_WM_RD_REJECT, (WPARAM)pVideoInfo, (LPARAM)0);
			delete pVideoInfo;
#endif
		}
		else {
			delete pVideoInfo;
		}
	}
}
void CUserManagerApp::OnFRDEnd(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const mycp::bigint sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!pCallInfo->m_pCallUserList.find(sFromAccount,pFromAccountInfo))
	{
		return ;
	}
	if (pCallInfo->m_sGroupCode==0)// || pFromAccountInfo->GetAccount() == theEBAppClient.EB_GetLogonAccount())
	{
		// 一对一会话，或者本端关闭
		//DoRDDisonnecte(pCallInfo);
		pCallInfo->m_nRDType = EB_RD_UNKNOWN;
		pCallInfo->m_pRDUserIdList.clear();
		pCallInfo->m_pOnRDUserList.clear();
	}else
	{
		pCallInfo->m_pRDUserIdList.remove(pFromAccountInfo->GetUserId());
		// 群组，有人退出视频；
		if (pCallInfo->m_pRDUserIdList.empty())
		{
			//DoRDDisonnecte(pCallInfo);
			pCallInfo->m_pOnRDUserList.clear();
#ifdef USES_REMOTE_DESKTOP

#ifdef USES_RD_COM
		}else if (pCallInfo->m_pEBRemoteDesktop!=NULL)
		{
			pCallInfo->m_pEBRemoteDesktop->RD_DelSink(sFromAccount);
#else
		}else if (pCallInfo->m_pEBRemoteDesktop.get()!=NULL)
		{
			pCallInfo->m_pEBRemoteDesktop->DelSink(sFromAccount);
#endif
#endif
		}
	}
	const bool bFromOnVideo = pCallInfo->m_pOnRDUserList.exist(sFromAccount);
	EB_RemoteDesktopInfo * pVideoInfo = new EB_RemoteDesktopInfo();
#ifdef _QT_MAKE_
	pVideoInfo->SetQEventType((QEvent::Type)EB_WM_RD_CLOSE);
#endif
	pVideoInfo->m_sCallId = sCallId;
	pVideoInfo->m_nRDType = pCallInfo->m_nRDType;
	pVideoInfo->m_nFromUserId = sFromAccount;
	pVideoInfo->m_bOnDesktop = bFromOnVideo;
	if (m_callback)
		m_callback->onRDClose(*pVideoInfo);

#ifdef _QT_MAKE_
	QObject* hWnd=NULL;
#else
	HWND hWnd=NULL;
#endif
	if (m_pDesktopHwnd.find(sCallId,hWnd)) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(hWnd, EB_WM_RD_CLOSE, (WPARAM)pVideoInfo, (LPARAM)0);
		delete pVideoInfo;
#endif
	}
	else if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        QCoreApplication::postEvent(m_pHwnd, pVideoInfo, thePostEventPriority);
#else
		::SendMessage(m_pHwnd, EB_WM_RD_CLOSE, (WPARAM)pVideoInfo, (LPARAM)0);
		delete pVideoInfo;
#endif
	}
	else {
		delete pVideoInfo;
	}
    pCallInfo->m_pOnRDUserList.remove(pFromAccountInfo->GetUserId());
}

void CUserManagerApp::UpdateLocalGroupVer(mycp::bigint nGroupId,mycp::bigint nNewGroupVer) const
{
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(nGroupId, pDepartmentInfo))
	{
		pDepartmentInfo->m_nServerGroupVer = nNewGroupVer;
		if (nNewGroupVer==pDepartmentInfo->m_nGroupVer+1)
		{
			pDepartmentInfo->m_nGroupVer = nNewGroupVer;
		}
	}
	const mycp::bigint nLocalGroupVer = GetLocalGroupVer(nGroupId);
	if (nNewGroupVer==nLocalGroupVer+1)
	{
		SetLocalGroupVer(nGroupId,nNewGroupVer);
	}
}
void CUserManagerApp::DeleteMemberInfo(const CEBGroupInfo::pointer& pDepartmentInfo, const CEBMemberInfo::pointer& pEmployeeInfo)
{
	if (pDepartmentInfo.get()==NULL || pEmployeeInfo.get()==NULL)
		return;

	const mycp::bigint sRemoveExitGroupId = pDepartmentInfo->m_sGroupCode;
	const mycp::bigint sRemoveExitEmpUserId = pEmployeeInfo->m_nMemberUserId;
	if (pDepartmentInfo->m_nManagerUserId==sRemoveExitEmpUserId)
	{
		pDepartmentInfo->m_nManagerUserId = 0;
		BoostReadLock rdLock(m_mutexEBDatas);
		if (m_pEBDatas.get()!=NULL)
		{
			char lpszBuffer[128];
			sprintf(lpszBuffer,"UPDATE eb_dep_info_t SET manager_uid=0 WHERE group_id=%lld",pDepartmentInfo->m_sGroupCode);
			m_pEBDatas->execute(lpszBuffer);
		}
	}

	if (sRemoveExitEmpUserId == m_pMyAccountInfo->GetUserId())
	{
		// 本人退出该群组（部门）
		theCheckGroupMemberList.remove(sRemoveExitGroupId);
		theCheckGroupInfoList.remove(sRemoveExitGroupId);

		if (pDepartmentInfo->m_nMyEmpId>0)
			pDepartmentInfo->m_nMyEmpId = 0;

		if (m_pDefaultEntEmployeeInfo.get()!=NULL && m_pDefaultEntEmployeeInfo->m_sGroupCode==sRemoveExitGroupId)
		{
			this->SetDefaltMemberCode(0);

			//m_pDefaultEntEmployeeInfo.reset();
			//m_pDefaultEntEmployeeInfo = GetMyEntEmployeeInfo();
			//if (m_pDefaultEntEmployeeInfo.get()!=NULL)
			//{
			//	if (m_callback)
			//		m_callback->onMemberInfo(pEmployeeInfo.get(),true);
			//	if (m_pHwnd!=NULL)
			//		::SendMessage(m_pHwnd, EB_WM_MEMBER_INFO, (WPARAM)pEmployeeInfo.get(), 1);
			//	// 重设置一次默认群组；
			//	this->SetDefaltMemberCode(m_pDefaultEntEmployeeInfo->m_sMemberCode);
			//}
		}

		if (pDepartmentInfo->m_sEnterpriseCode==0)
		{
			// 个人群组，删除全部数据；
			ClearGroupAllData(pDepartmentInfo);
		}else
		{
			// 企业部门，删除部分数据；
			ClearCallInfo(pDepartmentInfo->m_sGroupCode,true,true,true);			// 清除会话数据
			pDepartmentInfo->m_pMemberList.remove(m_pMyAccountInfo->GetUserId());	// 移除成员数据
			theMyEmployeeList.remove(pDepartmentInfo->m_sGroupCode);				// 移除我的成员数据
		}
	}else
	{
		// 删除该成员
		pDepartmentInfo->m_pMemberList.remove(sRemoveExitEmpUserId);
		theEmployeeList.remove(pEmployeeInfo->m_sMemberCode);
		CEBCallInfo::pointer pCallInfo;
		if (theCallInfoList.find(sRemoveExitGroupId, pCallInfo))
		{
			pCallInfo->m_pCallUserList.remove(sRemoveExitEmpUserId);
		}
	}
	BoostReadLock rdLock(m_mutexEBDatas);
	if (m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[128];
		sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE group_id=%lld AND emp_uid=%lld",pEmployeeInfo->m_sGroupCode,pEmployeeInfo->m_nMemberUserId);
		//sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE emp_id=%lld",pEmployeeInfo->m_sMemberCode);
		m_pEBDatas->execute(lpszBuffer);
	}
}
void CUserManagerApp::OnUMEMEmpDeleteResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return ;
	const mycp::bigint sEmpGroupId = pSotpRequestInfo->m_pRequestList.getParameterValue("dep_code",(mycp::bigint)0);
	const mycp::bigint nMemberCode = pSotpResponseInfo->m_pResponseList.getParameterValue("emp_code",(mycp::bigint)0);	// **该参数有效；
	const mycp::bigint nNewGroupVer = pSotpResponseInfo->m_pResponseList.getParameterValue("new_group_ver",(mycp::bigint)0);
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	// ** nResultValue == EB_STATE_OK 统一在 EB_MSG_REMOVE_GROUP 处理；
	if (EB_STATE_DEP_ACC_ERROR==nResultValue)
	//if (nResultValue == EB_STATE_OK || EB_STATE_DEP_ACC_ERROR==nResultValue)
	{
		// 更新本地群组成员版本号
		if (nNewGroupVer>0)
			UpdateLocalGroupVer(sEmpGroupId,nNewGroupVer);

		CEBMemberInfo::pointer pEmployeeInfo;
		if (!theEmployeeList.find(nMemberCode, pEmployeeInfo))
		{
			return;
		}
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!theDepartmentList.find(sEmpGroupId, pDepartmentInfo)) return;
		// 重新加载一次群组在线人数；
		LoadInfo(0,0,0,0,0,0,sEmpGroupId);

		//pEmployeeInfo->m_bDeleteFlag=true;
		pDepartmentInfo->m_nEmpCount = max(0,(pDepartmentInfo->m_pMemberList.size()-1));
		SetLocalGroupEmpCount(pDepartmentInfo->m_sGroupCode,pDepartmentInfo->m_nEmpCount);
		if (m_callback)
			m_callback->onRemoveGroup(pDepartmentInfo.get(),pEmployeeInfo.get());
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_REMOVE_GROUP);
			pEvent->SetEventData((void*)(const EB_MemberInfo*)pEmployeeInfo.get());
            postWaitEventResult(m_pHwnd, pEvent);
#else
			::SendMessage(m_pHwnd, EB_WM_REMOVE_GROUP, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(), (LPARAM)(const EB_MemberInfo*)pEmployeeInfo.get());
#endif
		}
		DeleteMemberInfo(pDepartmentInfo,pEmployeeInfo);
	}
}
void CUserManagerApp::OnUMEMEmpEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get()==NULL) return ;
	if (pSotpRequestInfo.get() == NULL) return ;
	BOOST_ASSERT (pSotpResponseInfo.get() != 0);
	const mycp::bigint sDepCode = pSotpRequestInfo->m_pRequestList.getParameterValue("edit_dep_code",(mycp::bigint)0);
	const int nEditForbidMinutes = pSotpRequestInfo->m_pRequestList.getParameterValue("edit_forbid_minutes",(int)-2);
	const bool bNewEmployeeInfo = pSotpRequestInfo->m_pRequestList.getParameterValue("new_employee", false);
	const mycp::bigint nTempMemberUserId = pSotpRequestInfo->m_pRequestList.getParameterValue("new_temp_member_uid", (mycp::bigint)0);
	const mycp::bigint sEmpCode = pSotpResponseInfo->m_pResponseList.getParameterValue("emp_code",(mycp::bigint)0);
	const mycp::bigint sEmpUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("emp_uid",(mycp::bigint)0);
	const mycp::bigint nNewGroupVer = pSotpResponseInfo->m_pResponseList.getParameterValue("new_group_ver",(mycp::bigint)0);
	const int nLineState = pSotpResponseInfo->m_pResponseList.getParameterValue("line_state",(int)EB_LINE_STATE_UNKNOWN);
	int nResultValue = pSotpResponseInfo->GetResultValue();
	CEBGroupInfo::pointer pDepartmentInfo;
	if (!theDepartmentList.find(sDepCode, pDepartmentInfo))
	{
		return ;
	}
	
	CEBMemberInfo::pointer pEmployeeInfo;
	if (bNewEmployeeInfo && nTempMemberUserId > 0)
	{
		if (!pDepartmentInfo->m_pMemberList.find(nTempMemberUserId, pEmployeeInfo,true))
		{
			return;
		}
		pEmployeeInfo->m_nMemberUserId = sEmpUserId;
		pEmployeeInfo->m_nLineState = (EB_USER_LINE_STATE)nLineState;
		pDepartmentInfo->m_pMemberList.insert(sEmpUserId, pEmployeeInfo);
	}else if (!pDepartmentInfo->m_pMemberList.find(sEmpUserId, pEmployeeInfo))
	{
		return ;
	}
	CEBMemberInfo::pointer pEditEmployeeInfo;
	if (nEditForbidMinutes>=-1)
	{
		// 禁言设置返回
		if (!pDepartmentInfo->m_pMemberList.find(sEmpUserId, pEditEmployeeInfo))
		{
			return;
		}
	}
	else if (!bNewEmployeeInfo)
	{
		if (!theEditEmployeeList.find(sEmpUserId, pEditEmployeeInfo, true))
		{
			return ;
		}
	}else
	{
		pDepartmentInfo->m_nEmpCount = pDepartmentInfo->m_pMemberList.size();
		//// 新建成员
		//if (!pDepartmentInfo->m_pMemberList.exist(sEmpUserId))
		//	pDepartmentInfo->m_pMemberList.insert(sEmpUserId, pEmployeeInfo);
	}
	if (nResultValue == EB_STATE_OK)
	{
		// -1: 解决禁言 0:永久禁言 >0 保留用于以后实现按分钟禁言；
		if (nEditForbidMinutes==-1)
		{
			pEditEmployeeInfo->m_nForbidMinutes = 0;
			pEditEmployeeInfo->m_nManagerLevel &= ~EB_GROUP_EXT_DATA_FORBID_SPEECH;
		}else if (nEditForbidMinutes>=0)
		{
			pEditEmployeeInfo->m_nForbidMinutes = nEditForbidMinutes;
			pEditEmployeeInfo->m_nManagerLevel |= EB_LEVEL_FORBID_SPEECH;
		}
		if (!theEmployeeList.exist(sEmpCode))
			theEmployeeList.insert(sEmpCode, pEmployeeInfo);
		if (pEditEmployeeInfo.get() != NULL)
		{
			if (pEditEmployeeInfo->m_sHeadResourceId==0)
			{
				pEditEmployeeInfo->m_sHeadResourceId = pEmployeeInfo->m_sHeadResourceId;
				pEditEmployeeInfo->m_sHeadResourceFile = pEmployeeInfo->m_sHeadResourceFile;
			}
			if (pEmployeeInfo.get()!=pEditEmployeeInfo.get())
				pEmployeeInfo->operator =(pEditEmployeeInfo.get());
		}
		pEmployeeInfo->m_sMemberCode = sEmpCode;

		// 更新本地群组成员版本号
		UpdateLocalGroupVer(sDepCode,nNewGroupVer);
		// 保存本地数据
		SaveLocalMemberInfo(pEmployeeInfo);

		// 添加成员，需要发送一次群组资料事件；
		this->ProcessMemberInfo(pDepartmentInfo,pEmployeeInfo,bNewEmployeeInfo);
		// 重新加载一次群组在线人数；
		LoadInfo(0,0,0,0,0,0,pEmployeeInfo->m_sGroupCode);
		//if (m_callback)
		//	m_callback->onMemberInfo(pEmployeeInfo.get(),false);
		//if (m_pHwnd!=NULL)
		//	::SendMessage(m_pHwnd, EB_WM_MEMBER_INFO, (WPARAM)pEmployeeInfo.get(), 0);

		//if (pEditEmployeeInfo.get() != NULL && pEditEmployeeInfo.get() != pEmployeeInfo.get())
		//{
		//	pDepartmentInfo->m_pMemberList.remove(sEmpUserId);
		//	pDepartmentInfo->m_pMemberList.insert(sEmpUserId, pEditEmployeeInfo);
		//}
#ifdef _QT_MAKE_
        if (m_callback!=0)
            m_callback->onMemberEditResponse(pEmployeeInfo.get(),(EB_STATE_CODE)nResultValue);
        if (m_pHwnd!=0) {
            EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
            pEvent->SetQEventType((QEvent::Type)EB_WM_MEMBER_EDIT_RESPONSE);
            pEvent->SetEventParameter((long)nResultValue);
            pEvent->SetEventBigParameter(bNewEmployeeInfo?1:0);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
        }
#endif

	}else
	{
		if (m_callback)
            m_callback->onMemberEditResponse(pEmployeeInfo.get(),(EB_STATE_CODE)nResultValue);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
            pEvent->SetQEventType((QEvent::Type)EB_WM_MEMBER_EDIT_RESPONSE);
			pEvent->SetEventParameter((long)nResultValue);
			//pEvent->SetEventData((void*)(const EB_MemberInfo*)pEmployeeInfo.get());
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
            ::SendMessage(m_pHwnd, EB_WM_MEMBER_EDIT_RESPONSE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), (LPARAM)nResultValue);
#endif
		}
		if (bNewEmployeeInfo)
			pDepartmentInfo->m_pMemberList.remove(sEmpUserId);
	}
}
void CUserManagerApp::ClearGroupAllData(const CEBGroupInfo::pointer& pDepartmentInfo)
{
	const mycp::bigint sDeleteDepCode = pDepartmentInfo->m_sGroupCode;
	{
		BoostReadLock lock(pDepartmentInfo->m_pMemberList.mutex());
		CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		{
			const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
			theEmployeeList.remove(pEmployeeInfo->m_sMemberCode);
		}
	}
	ClearCallInfo(sDeleteDepCode,true,true,true);
	theDepartmentList.remove(sDeleteDepCode);
	theMyEmployeeList.remove(sDeleteDepCode);

	BoostReadLock rdLock(m_mutexEBDatas);
	if (m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[128];
		sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE group_id=%lld",sDeleteDepCode);
		m_pEBDatas->execute(lpszBuffer);
		sprintf(lpszBuffer,"DELETE FROM eb_dep_info_t WHERE group_id=%lld",sDeleteDepCode);
		m_pEBDatas->execute(lpszBuffer);
	}
}

void CUserManagerApp::UpdateLocalGroupInfoVer(mycp::bigint nEntId,mycp::bigint nNewGroupInfoVer, bool bSaveForce) const
{
	//BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_mutexEBDatas));
	if (m_pEBDatas.get()!=NULL)
	{
		if (nNewGroupInfoVer>0)
		{
			// 判断是否更新本地版本号
			if (nEntId>0)
			{
				const mycp::bigint nLocalEntDepInfoVer = bSaveForce?(nNewGroupInfoVer-1):GetLocalEntGroupInfoVer(nEntId);
				if (nNewGroupInfoVer==nLocalEntDepInfoVer+1)
				{
					// 更新本地企业部门资料版本号
					char lpszBuffer[128];
					sprintf(lpszBuffer,"UPDATE eb_ent_info_t SET ver=%lld WHERE ent_id=%lld",nNewGroupInfoVer,nEntId);
					m_pEBDatas->execute(lpszBuffer);
				}
			}else
			{
				const mycp::bigint nLocalMyDepInfoVer = bSaveForce?(nNewGroupInfoVer-1):GetLocalMyGroupInfoVer(m_pMyAccountInfo->GetUserId());
				if (nNewGroupInfoVer==nLocalMyDepInfoVer+1)
				{
					// 更新本地企业部门资料版本号
					char lpszBuffer[128];
					sprintf(lpszBuffer,"UPDATE eb_user_ext_t SET int_value=%lld WHERE uid=%lld AND ext_key=%d",nNewGroupInfoVer,m_pMyAccountInfo->GetUserId(),EB_USER_EXT_KEY_GROUP_INFO_VERSION);
					m_pEBDatas->execute(lpszBuffer);
				}
			}
		}
	}
}
void CUserManagerApp::OnUMEMDepDeleteResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	// ** 统一在 EB_MSG_DELETE_GROUP 处理；
	//if (pSotpRequestInfo.get() == NULL) return ;
	////const tstring sDeleteEntCode = pSotpRequestInfo->m_pRequestList.getParameterValue("ent_code");
	//const mycp::bigint sDeleteDepCode = pSotpRequestInfo->m_pRequestList.getParameterValue("dep_code",(mycp::bigint)0);
	//const mycp::bigint nNewGroupInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("new_group_info_ver",(mycp::bigint)0);
	//int nResultValue = pSotpResponseInfo->GetResultValue();
	//CEBGroupInfo::pointer pDepartmentInfo;
	//if (!theDepartmentList.find(sDeleteDepCode, pDepartmentInfo))
	//{
	//	return ;
	//}
	//if (nResultValue == EB_STATE_OK)
	//{
	//	pDepartmentInfo->m_nGroupData |= EB_GROUP_DATA_DELETE;
	//	const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
	//	if (m_callback)
	//		m_callback->onGroupDelete(pDepartmentInfo.get(),bIsMyDepartment);
	//	if (m_pHwnd!=NULL)
	//		::SendMessage(m_pHwnd, EB_WM_GROUP_DELETE, (WPARAM)pDepartmentInfo.get(), bIsMyDepartment?1:0);

	//	UpdateLocalGroupInfoVer(pDepartmentInfo->m_sEnterpriseCode,nNewGroupInfoVer);
	//	ClearGroupAllData(pDepartmentInfo);
	//}else
	//{
	//	//if (m_pHwnd!=NULL)
	//	//	::PostMessage(m_pHwnd, POP_WM_DEP_DELETE_RESPONSE, (WPARAM)pDepartmentInfo.get(), 0);
	//}
	//return ;
}
void CUserManagerApp::OnUMEMDepEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_pMyAccountInfo.get()==NULL) return;
	if (pSotpRequestInfo.get() == NULL) return ;
	const mycp::bigint sEditDepCode = pSotpRequestInfo->m_pRequestList.getParameterValue("edit_dep_code",(mycp::bigint)0);
	const int nEditForbidMinutes = pSotpRequestInfo->m_pRequestList.getParameterValue("edit_forbid_minutes",(int)-2);
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	const mycp::bigint s_dep_code = pSotpResponseInfo->m_pResponseList.getParameterValue("dep_code",(mycp::bigint)0);
	const mycp::bigint nNewGroupInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("new_group_info_ver",(mycp::bigint)0);
	CEBGroupInfo::pointer pEditDepartmentInfo;
	if (nEditForbidMinutes>=-1)	// 禁言
	{
		if (!theDepartmentList.find(sEditDepCode, pEditDepartmentInfo))
		{
			return;
		}
	}else
	{
		// 普通修改更新
		if (!theEditDepartmentList.find(sEditDepCode, pEditDepartmentInfo, true))
		{
			return ;
		}
	}
	pEditDepartmentInfo->m_sGroupCode = s_dep_code;
	if (nResultValue == EB_STATE_OK)
	{
		// -1: 解决禁言 0:永久禁言 >0 保留用于以后实现按分钟禁言；
		if (nEditForbidMinutes==-1)
			pEditDepartmentInfo->m_nGroupExtData &= ~EB_GROUP_EXT_DATA_FORBID_SPEECH;
		else if (nEditForbidMinutes==0)
			pEditDepartmentInfo->m_nGroupExtData |= EB_GROUP_EXT_DATA_FORBID_SPEECH;

		bool bNewDepartment = false;
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!theDepartmentList.find(s_dep_code, pDepartmentInfo))
		{
			bNewDepartment = true;
			pDepartmentInfo = pEditDepartmentInfo;
			theDepartmentList.insert(s_dep_code, pDepartmentInfo);
		}else if (pDepartmentInfo.get() != pEditDepartmentInfo.get())
		{
			pDepartmentInfo->operator =(pEditDepartmentInfo.get());
		}

		// 更新本地数据
		SaveLocalGroupInfo(pDepartmentInfo);
		// 判断更新本地版本数据
		UpdateLocalGroupInfoVer(pDepartmentInfo->m_sEnterpriseCode,nNewGroupInfoVer);

		const bool bIsMyDepartment = pEditDepartmentInfo->m_nMyEmpId>0;
		if (m_callback)
			m_callback->onGroupInfo(pEditDepartmentInfo.get(),bIsMyDepartment);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_GroupInfo * pEvent = new EB_GroupInfo(pEditDepartmentInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
			pEvent->SetEventParameter((long)bIsMyDepartment?1:0);
			//pEvent->SetEventData((void*)(const EB_MemberInfo*)pEmployeeInfo.get());
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pEditDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
		}

		// 处理前面未处理群组成员
		if (bNewDepartment)
		{
			std::vector<CEBMemberInfo::pointer> pDepMemberList; 
			theTempMemberList.find(sEditDepCode,pDepMemberList);
			for (size_t i=0; i<pDepMemberList.size(); i++)
			{
				CEBMemberInfo::pointer pMemberInfo = pDepMemberList[i];
				ProcessMemberInfo(pDepartmentInfo,pMemberInfo);
			}
			theTempMemberList.remove(sEditDepCode);
		}

		//if (!bNewDepartment)
		//{
		//	pOldDepartmentInfo->operator =(pEditDepartmentInfo.get());
		//}
#ifdef _QT_MAKE_
        if (m_callback)
            m_callback->onGroupEditResponse(pEditDepartmentInfo.get(),(EB_STATE_CODE)nResultValue);
        if (m_pHwnd!=0) {
            EB_GroupInfo * pEvent = new EB_GroupInfo(pEditDepartmentInfo.get());
            pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_EDIT_RESPONSE);
            pEvent->SetEventParameter((long)EB_STATE_OK);
            pEvent->SetEventBigParameter(bNewDepartment?1:0);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
        }
#endif
	}else
	{
		if (m_callback)
            m_callback->onGroupEditResponse(pEditDepartmentInfo.get(),(EB_STATE_CODE)nResultValue);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_GroupInfo * pEvent = new EB_GroupInfo(pEditDepartmentInfo.get());
            pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_EDIT_RESPONSE);
			pEvent->SetEventParameter((long)nResultValue);
			//pEvent->SetEventData((void*)(const EB_MemberInfo*)pEmployeeInfo.get());
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
            ::SendMessage(m_pHwnd, EB_WM_GROUP_EDIT_RESPONSE, (WPARAM)(const EB_GroupInfo*)pEditDepartmentInfo.get(),(LPARAM)nResultValue);
#endif
		}
		//if (m_pHwnd!=NULL)
		//	::SendMessage(m_pHwnd, POP_WM_DEP_EDIT_RESPONSE, (WPARAM)pEditDepartmentInfo.get(), 0);
	}
}
void CUserManagerApp::OnUMEMDepOpResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
}
void CUserManagerApp::OnUMEMEntEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_pDefaultEnterpriseInfo.get() == NULL || m_pEditEnterpriseInfo.get() == NULL) return ;
	int nResultValue = pSotpResponseInfo->GetResultValue();
	if (nResultValue == EB_STATE_OK)
	{
		mycp::bigint nCreateUserId = m_pDefaultEnterpriseInfo->m_nCreateUserId;
		m_pDefaultEnterpriseInfo->operator =(m_pEditEnterpriseInfo.get());
		m_pDefaultEnterpriseInfo->m_nCreateUserId = nCreateUserId;
		if (m_callback) {
			m_callback->onEnterpriseInfo(m_pDefaultEnterpriseInfo.get());
		}

		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            EB_EnterpriseInfo * pEvent = new EB_EnterpriseInfo(m_pDefaultEnterpriseInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_ENTERPRISE_INFO);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_ENTERPRISE_INFO, (WPARAM)(const EB_EnterpriseInfo*)m_pDefaultEnterpriseInfo.get(), 0);
#endif
		}
	}else
	{
		//if (m_pHwnd!=NULL)
		//	::SendMessage(m_pHwnd, POP_WM_ENT_EDIT_RESPONSE, (WPARAM)m_pDefaultEnterpriseInfo.get(), 0);
	}
	m_pEditEnterpriseInfo.reset();
}
void CUserManagerApp::OnFUMEMEntInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const mycp::bigint s_ent_code = pSotpResponseInfo->m_pResponseList.getParameterValue("ent_code",(mycp::bigint)0);
	const tstring s_ent_name = pSotpResponseInfo->m_pResponseList.getParameterValue("ent_name");
	const tstring s_description = pSotpResponseInfo->m_pResponseList.getParameterValue("description");
	const tstring s_phone = pSotpResponseInfo->m_pResponseList.getParameterValue("phone");
	const tstring s_fax = pSotpResponseInfo->m_pResponseList.getParameterValue("fax");
	const tstring s_email = pSotpResponseInfo->m_pResponseList.getParameterValue("email");
	const tstring s_url = pSotpResponseInfo->m_pResponseList.getParameterValue("url");
	const tstring s_address = pSotpResponseInfo->m_pResponseList.getParameterValue("address");
	const tstring s_create_time = pSotpResponseInfo->m_pResponseList.getParameterValue("create_time", "");
	const mycp::bigint nCreateUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("create_uid",(mycp::bigint)0);
	const int nEntExt = pSotpResponseInfo->m_pResponseList.getParameterValue("ent_ext",(int)0);
	//const tstring s_creator = pSotpResponseInfo->m_pResponseList.getParameterValue("creator");
	const int nEntCount = pSotpResponseInfo->m_pResponseList.getParameterValue("ent-count",(int)-1);
	if (m_bNeedLoadVerInfo && nEntCount<=0)	// -1=企业资料返回；0=没有企业资料
	{
		m_bNeedLoadVerInfo = false;
		LoadInfo(0,0,1);
	}
	if (nEntCount >= 0)
	{
		// *加载企业成功，加载部门版本号；
		return;
	}

	CEBEnterpriseInfo::pointer pEnterpriseInfo;
	if (!theEnterpriseList.find(s_ent_code,pEnterpriseInfo))
	{
		pEnterpriseInfo = CEBEnterpriseInfo::create(s_ent_code, s_ent_name);
		theEnterpriseList.insert(s_ent_code,pEnterpriseInfo);
	}else
	{
		pEnterpriseInfo->m_sEnterpriseName = s_ent_name;
	}
	pEnterpriseInfo->m_sDescription = s_description;
	pEnterpriseInfo->m_sPhone = s_phone;
	pEnterpriseInfo->m_sFax = s_fax;
	pEnterpriseInfo->m_sEmail = s_email;
	pEnterpriseInfo->m_sUrl = s_url;
	pEnterpriseInfo->m_sAddress = s_address;
	pEnterpriseInfo->m_sCreateTime = s_create_time;
	//pEnterpriseInfo->m_sCreator = s_creator;
	pEnterpriseInfo->m_nCreateUserId = nCreateUserId;
	pEnterpriseInfo->m_nEntExt = nEntExt;
	if (m_pDefaultEnterpriseInfo.get() == NULL)
		m_pDefaultEnterpriseInfo = pEnterpriseInfo;

	//if (m_pEBDatas.get()!=NULL)
	//{
	//	tstring sDescription(s_description);
	//	CSqliteCdbc::escape_string_in(sDescription);
	//	tstring sAddress(s_address);
	//	CSqliteCdbc::escape_string_in(sAddress);
	//	char lpszBuffer[1024*5];
	//	sprintf(lpszBuffer,"SELECT ent_id FROM eb_ent_info_t WHERE ent_id=%lld",s_ent_code);
	//	int ret = m_pEBDatas->execsql(lpszBuffer);
	//	if (ret == 1)
	//	{
	//		sprintf(lpszBuffer,"UPDATE eb_ent_info_t SET \
	//						   ent_name='%s',description='%s',phone='%s',fax='%s',email='%s',url='%s',address='%s' WHERE ent_id=%lld",
	//						   s_ent_name.c_str(),sDescription.c_str(),s_phone.c_str(),s_fax.c_str(),s_email.c_str(),s_url.c_str(),sAddress.c_str(),s_ent_code);
	//	}else
	//	{
	//		sprintf(lpszBuffer,"INSERT INTO eb_ent_info_t(ent_id,ent_name,description,phone,fax,email,url,address) VALUES(%lld,'%s','%s','%s','%s','%s','%s','%s')",
	//						   s_ent_code,s_ent_name.c_str(),sDescription.c_str(),s_phone.c_str(),s_fax.c_str(),s_email.c_str(),s_url.c_str(),sAddress.c_str());
	//	}
	//	m_pEBDatas->execsql(lpszBuffer);
	//}
	if (m_callback)
		m_callback->onEnterpriseInfo(m_pDefaultEnterpriseInfo.get());
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        /// for test
//        char lpszLocalIp[128];
////        sprintf(lpszLocalIp,"ip-size=%d", (int)m_pLocalHostIp.size());
//        sprintf(lpszLocalIp,"%d-%s", (int)m_pLocalHostIp.size(), m_pLocalHostIp[0].c_str());
//        m_pDefaultEnterpriseInfo->m_sEnterpriseName = lpszLocalIp;
        EB_EnterpriseInfo * pEvent = new EB_EnterpriseInfo(m_pDefaultEnterpriseInfo.get());
		pEvent->SetQEventType((QEvent::Type)EB_WM_ENTERPRISE_INFO);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::SendMessage(m_pHwnd, EB_WM_ENTERPRISE_INFO, (WPARAM)(const EB_EnterpriseInfo*)m_pDefaultEnterpriseInfo.get(), 0);
#endif
	}
}

void CUserManagerApp::SaveLocalMemberInfo(const CEBMemberInfo::pointer& pEmployeeInfo)
{
	BoostReadLock rdLock(m_mutexEBDatas);
	if (m_pEBDatas.get()!=NULL)
	{
		// 保存本地数据库
		tstring sDescription(pEmployeeInfo->m_sDescription);
		CSqliteCdbc::escape_string_in(sDescription);
		tstring sAddress(pEmployeeInfo->m_sAddress);
		CSqliteCdbc::escape_string_in(sAddress);
		char lpszBuffer[1024*5];
		sprintf(lpszBuffer,"SELECT emp_id FROM eb_emp_info_t WHERE group_id=%lld AND emp_uid=%lld LIMIT 1",pEmployeeInfo->m_sGroupCode,pEmployeeInfo->m_nMemberUserId);
		//sprintf(lpszBuffer,"SELECT emp_id FROM eb_emp_info_t WHERE emp_id=%lld",pEmployeeInfo->m_sMemberCode);
		if (m_pEBDatas->select(lpszBuffer) == 1)
		{
			sprintf(lpszBuffer,"UPDATE eb_emp_info_t SET "\
				"group_id=%lld,username='%s',gender=%d,birthday=%d,job_title='%s',job_position=%d,work_phone='%s',cell_phone='%s',"\
				"fax='%s',email='%s',address='%s',description='%s',manager_level=%d,head_rid=%lld,head_cm_server='%s',head_http_server='%s',emp_id=%lld,display_index=%d,forbid_minutes=%d WHERE group_id=%lld AND emp_uid=%lld",
				pEmployeeInfo->m_sGroupCode,pEmployeeInfo->m_sUserName.c_str(),pEmployeeInfo->m_nGender,pEmployeeInfo->m_nBirthday,pEmployeeInfo->m_sJobTitle.c_str(),
				pEmployeeInfo->m_nJobPosition,pEmployeeInfo->m_sWorkPhone.c_str(),pEmployeeInfo->m_sCellPhone.c_str(),pEmployeeInfo->m_sFax.c_str(),
				pEmployeeInfo->m_sEmail.c_str(),sAddress.c_str(),sDescription.c_str(),pEmployeeInfo->m_nManagerLevel,pEmployeeInfo->m_sHeadResourceId,
				pEmployeeInfo->m_sHeadCmServer.c_str(),pEmployeeInfo->m_sHeadHttpServer.c_str(),pEmployeeInfo->m_sMemberCode,pEmployeeInfo->m_nDisplayIndex,
				pEmployeeInfo->m_nForbidMinutes,pEmployeeInfo->m_sGroupCode,pEmployeeInfo->m_nMemberUserId);
		}else
		{
			sprintf(lpszBuffer,"INSERT INTO eb_emp_info_t(emp_id,emp_account,emp_uid,group_id,username,gender,birthday,job_title,job_position,work_phone,cell_phone,fax,email,address,description,manager_level,head_rid,head_cm_server,head_http_server,display_index,forbid_minutes) "\
				"VALUES(%lld,'%s',%lld,%lld,'%s',%d,%d,'%s',%d,'%s','%s','%s','%s','%s','%s',%d,%lld,'%s','%s',%d,%d)",
				pEmployeeInfo->m_sMemberCode,pEmployeeInfo->m_sMemberAccount.c_str(),pEmployeeInfo->m_nMemberUserId,pEmployeeInfo->m_sGroupCode,pEmployeeInfo->m_sUserName.c_str(),pEmployeeInfo->m_nGender,pEmployeeInfo->m_nBirthday,pEmployeeInfo->m_sJobTitle.c_str(),pEmployeeInfo->m_nJobPosition,pEmployeeInfo->m_sWorkPhone.c_str(),pEmployeeInfo->m_sCellPhone.c_str(),
				pEmployeeInfo->m_sFax.c_str(),pEmployeeInfo->m_sEmail.c_str(),sAddress.c_str(),sDescription.c_str(),pEmployeeInfo->m_nManagerLevel,pEmployeeInfo->m_sHeadResourceId,pEmployeeInfo->m_sHeadCmServer.c_str(),pEmployeeInfo->m_sHeadHttpServer.c_str(),pEmployeeInfo->m_nDisplayIndex,pEmployeeInfo->m_nForbidMinutes);
		}
		m_pEBDatas->execute(lpszBuffer);
	}
}

void CUserManagerApp::SaveLocalGroupInfo(const CEBGroupInfo::pointer& pDepartmentInfo)
{
	BoostReadLock rdLock(m_mutexEBDatas);
	if (m_pEBDatas.get()!=NULL)
	{
		// 保存数据库
		tstring sDescription(pDepartmentInfo->m_sDescription);
		CSqliteCdbc::escape_string_in(sDescription);
		tstring sAddress(pDepartmentInfo->m_sAddress);
		CSqliteCdbc::escape_string_in(sAddress);
		char lpszBuffer[1024*5];
		sprintf(lpszBuffer,"SELECT group_id FROM eb_dep_info_t WHERE group_id=%lld",pDepartmentInfo->m_sGroupCode);
		if (m_pEBDatas->select(lpszBuffer) == 1)
		{
			sprintf(lpszBuffer,"UPDATE eb_dep_info_t SET "\
				"dep_name='%s',parent_gid=%lld,ent_id=%lld,description='%s',phone='%s',fax='%s',email='%s',url='%s',address='%s',create_time='%s',"\
				"create_uid=%lld,type=%d,my_emp_id=%lld,emp_count=%d,display_index=%d,manager_uid=%lld,ext_data=%d WHERE group_id=%lld",
				pDepartmentInfo->m_sGroupName.c_str(),pDepartmentInfo->m_sParentCode,pDepartmentInfo->m_sEnterpriseCode,sDescription.c_str(),
				pDepartmentInfo->m_sPhone.c_str(),pDepartmentInfo->m_sFax.c_str(),pDepartmentInfo->m_sEmail.c_str(),pDepartmentInfo->m_sUrl.c_str(),
				sAddress.c_str(),pDepartmentInfo->m_sCreateTime.c_str(),pDepartmentInfo->m_nCreateUserId,(int)pDepartmentInfo->m_nGroupType,
				pDepartmentInfo->m_nMyEmpId,pDepartmentInfo->m_nEmpCount,pDepartmentInfo->m_nDisplayIndex,pDepartmentInfo->m_nManagerUserId,pDepartmentInfo->m_nGroupExtData,pDepartmentInfo->m_sGroupCode);
		}else if (pDepartmentInfo->m_nEmpCount==0 || pDepartmentInfo->m_nMyEmpId==0)
		{
			// 空部门（群组）和不属于我的部门，直接设置版本号
			sprintf(lpszBuffer,"INSERT INTO eb_dep_info_t(group_id,dep_name,parent_gid,ent_id,description,phone,fax,email,url,address,create_time,create_uid,type,my_emp_id,emp_count,display_index,ver,manager_uid,ext_data) "\
				"VALUES(%lld,'%s',%lld,%lld,'%s','%s','%s','%s','%s','%s','%s',%lld,%d,%lld,%d,%d,%lld,%lld,%d)",
				pDepartmentInfo->m_sGroupCode,pDepartmentInfo->m_sGroupName.c_str(),pDepartmentInfo->m_sParentCode,pDepartmentInfo->m_sEnterpriseCode,
				sDescription.c_str(),pDepartmentInfo->m_sPhone.c_str(),pDepartmentInfo->m_sFax.c_str(),pDepartmentInfo->m_sEmail.c_str(),pDepartmentInfo->m_sUrl.c_str(),
				sAddress.c_str(),pDepartmentInfo->m_sCreateTime.c_str(),pDepartmentInfo->m_nCreateUserId,pDepartmentInfo->m_nGroupType,pDepartmentInfo->m_nMyEmpId,pDepartmentInfo->m_nEmpCount,
				pDepartmentInfo->m_nDisplayIndex,pDepartmentInfo->m_nServerGroupVer,pDepartmentInfo->m_nManagerUserId,pDepartmentInfo->m_nGroupExtData);
		}else
		{
			// 非空部门（群组），不设置版本号，加载成员后再设置
			sprintf(lpszBuffer,"INSERT INTO eb_dep_info_t(group_id,dep_name,parent_gid,ent_id,description,phone,fax,email,url,address,create_time,create_uid,type,my_emp_id,emp_count,display_index,manager_uid,ext_data) "\
				"VALUES(%lld,'%s',%lld,%lld,'%s','%s','%s','%s','%s','%s','%s',%lld,%d,%lld,%d,%d,%lld,%d)",
				pDepartmentInfo->m_sGroupCode,pDepartmentInfo->m_sGroupName.c_str(),pDepartmentInfo->m_sParentCode,pDepartmentInfo->m_sEnterpriseCode,
				sDescription.c_str(),pDepartmentInfo->m_sPhone.c_str(),pDepartmentInfo->m_sFax.c_str(),pDepartmentInfo->m_sEmail.c_str(),pDepartmentInfo->m_sUrl.c_str(),
				sAddress.c_str(),pDepartmentInfo->m_sCreateTime.c_str(),pDepartmentInfo->m_nCreateUserId,pDepartmentInfo->m_nGroupType,pDepartmentInfo->m_nMyEmpId,pDepartmentInfo->m_nEmpCount,
				pDepartmentInfo->m_nDisplayIndex,pDepartmentInfo->m_nManagerUserId,pDepartmentInfo->m_nGroupExtData);
		}
		m_pEBDatas->execute(lpszBuffer);
	}
}
void CUserManagerApp::SaveLocalContactInfo(const CEBContactInfo::pointer& pContactInfo)
{
	BoostReadLock rdLock(m_mutexEBDatas);
	if (m_pEBDatas.get()!=NULL)
	{
		// 保存本地数据库
		tstring sDescription(pContactInfo->m_sDescription);
		CSqliteCdbc::escape_string_in(sDescription);
		tstring sAddress(pContactInfo->m_sAddress);
		CSqliteCdbc::escape_string_in(sAddress);
		tstring sContact(pContactInfo->m_sContact);
		CSqliteCdbc::escape_string_in(sContact);
		tstring sName(pContactInfo->m_sName);
		CSqliteCdbc::escape_string_in(sName);
		tstring sCompany(pContactInfo->m_sCompany);
		CSqliteCdbc::escape_string_in(sCompany);
		char lpszBuffer[1024*5];
		sprintf(lpszBuffer,"SELECT con_id FROM eb_contact_info_t WHERE con_id=%lld",pContactInfo->m_nContactId);
		if (m_pEBDatas->select(lpszBuffer) == 1)
		{
			sprintf(lpszBuffer,"UPDATE eb_contact_info_t SET "\
				"con_uid=%lld,ug_id=%lld,con_account='%s',name='%s',phone='%s',email='%s',address='%s',description='%s',company='%s',job_title='%s',"\
				"tel='%s',fax='%s',url='%s',ntype=%d,head_ver=%lld,head_rid=%lld,head_cm_server='%s',head_http_server='%s',head_md5='%s',flag=0 WHERE con_id=%lld",
				pContactInfo->m_nContactUserId,pContactInfo->m_nUGId,sContact.c_str(),sName.c_str(),pContactInfo->m_sPhone.c_str(),pContactInfo->m_sEmail.c_str(),
				sAddress.c_str(),sDescription.c_str(),sCompany.c_str(),pContactInfo->m_sJobTitle.c_str(),pContactInfo->m_sTel.c_str(),pContactInfo->m_sFax.c_str(),
				pContactInfo->m_sUrl.c_str(),pContactInfo->m_nContactType,pContactInfo->m_nHeadVer,pContactInfo->m_sHeadResourceId,pContactInfo->m_sHeadCmServer.c_str(),
				pContactInfo->m_sHeadHttpServer.c_str(),pContactInfo->m_sHeadMd5.c_str(),pContactInfo->m_nContactId);
		}else
		{
			sprintf(lpszBuffer,"INSERT INTO eb_contact_info_t(con_id,con_uid,ug_id,con_account,name,phone,email,address,description,company,job_title,tel,fax,url,ntype,head_ver,head_rid,head_cm_server,head_http_server,head_md5) "\
				"VALUES(%lld,%lld,%lld,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%lld,%lld,'%s','%s','%s')",
				pContactInfo->m_nContactId,pContactInfo->m_nContactUserId,pContactInfo->m_nUGId,sContact.c_str(),sName.c_str(),pContactInfo->m_sPhone.c_str(),pContactInfo->m_sEmail.c_str(),
				sAddress.c_str(),sDescription.c_str(),sCompany.c_str(),pContactInfo->m_sJobTitle.c_str(),pContactInfo->m_sTel.c_str(),pContactInfo->m_sFax.c_str(),
				pContactInfo->m_sUrl.c_str(),pContactInfo->m_nContactType,pContactInfo->m_nHeadVer,pContactInfo->m_sHeadResourceId,pContactInfo->m_sHeadCmServer.c_str(),
				pContactInfo->m_sHeadHttpServer.c_str(),pContactInfo->m_sHeadMd5.c_str());
		}
		m_pEBDatas->execute(lpszBuffer);
	}
}

void CUserManagerApp::ProcessGroupInfo(const CEBGroupInfo::pointer& pDepartmentInfo)
{
	if (pDepartmentInfo.get()==NULL)
		return;
	// *这里版本不一致，不加载成员
	{
		BoostReadLock rdLock(m_mutexEBDatas);
		if (m_pEBDatas.get()!=NULL && pDepartmentInfo->m_nGroupVer==pDepartmentInfo->m_nServerGroupVer)
		{
			// 加载本地群组成员；
			bool bHasLocalMemberInfo = false;
			char lpszBuffer[512];
			sprintf(lpszBuffer,"SELECT emp_id,emp_account,emp_uid,group_id,username,gender,birthday,job_title,job_position,work_phone,cell_phone,fax,email,address,description,"\
				"manager_level,head_rid,head_cm_server,display_index,forbid_minutes FROM eb_emp_info_t WHERE group_id=%lld",pDepartmentInfo->m_sGroupCode);

			int nCookie = 0;
			m_pEBDatas->select(lpszBuffer, nCookie);
			int nIndex = 0;
			cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
			while (pRecord.get()!=0)
			{
				bHasLocalMemberInfo = true;
				const mycp::bigint nLocalEmpId = pRecord->getVector()[0]->getBigIntValue();
				const tstring sLocalEmpAccount = pRecord->getVector()[1]->getStrValue();
				const mycp::bigint nLocalEmpUserId = pRecord->getVector()[2]->getBigIntValue();
				const mycp::bigint nLocalGroupId = pRecord->getVector()[3]->getBigIntValue();
				const tstring sLocalUserName = pRecord->getVector()[4]->getStrValue();
				const EB_GENDER_TYPE nLocalGender = (EB_GENDER_TYPE)pRecord->getVector()[5]->getIntValue();
				const int nLocalBirthday = pRecord->getVector()[6]->getIntValue();
				const tstring sLocalJobTitle = pRecord->getVector()[7]->getStrValue();
				const int nLocalJobPosition = pRecord->getVector()[8]->getIntValue();
				const tstring sLocalWorkPhone = pRecord->getVector()[9]->getStrValue();
				const tstring sLocalCellPhone = pRecord->getVector()[10]->getStrValue();
				const tstring sLocalFax = pRecord->getVector()[11]->getStrValue();
				const tstring sLocalEmail = pRecord->getVector()[12]->getStrValue();
				tstring sLocalAddress = pRecord->getVector()[13]->getStrValue();
				CSqliteCdbc::escape_string_out(sLocalAddress);
				tstring sLocalDescription = pRecord->getVector()[14]->getStrValue();
				CSqliteCdbc::escape_string_out(sLocalDescription);
				const int nLocalManagerLevel = pRecord->getVector()[15]->getIntValue();
				const mycp::bigint nLocalHeadResId = pRecord->getVector()[16]->getBigIntValue();
				const tstring sLocalHeadCmServer = pRecord->getVector()[17]->getStrValue();
				const int nDisplayIndex = pRecord->getVector()[18]->getIntValue();
				int nForbidMinutes = pRecord->getVector()[19]->getIntValue();
				if ((nLocalManagerLevel&EB_LEVEL_FORBID_SPEECH)==0)
				{
					nForbidMinutes = 0;
				}

				CEBMemberInfo::pointer pEmployeeInfo;
				if (!theEmployeeList.find(nLocalEmpId, pEmployeeInfo))
				{
					pEmployeeInfo = CEBMemberInfo::create(nLocalGroupId,nLocalEmpUserId);
					theEmployeeList.insert(nLocalEmpId, pEmployeeInfo);
				}
				pEmployeeInfo->m_sMemberAccount = sLocalEmpAccount;
				pEmployeeInfo->m_nMemberUserId = nLocalEmpUserId;
				pEmployeeInfo->m_sMemberCode = nLocalEmpId;
				pEmployeeInfo->m_sUserName = sLocalUserName;
				pEmployeeInfo->m_sJobTitle = sLocalJobTitle;
				pEmployeeInfo->m_nJobPosition = nLocalJobPosition;
				pEmployeeInfo->m_sCellPhone = sLocalCellPhone;
				pEmployeeInfo->m_sFax = sLocalFax;
				pEmployeeInfo->m_sWorkPhone = sLocalWorkPhone;
				pEmployeeInfo->m_sEmail = sLocalEmail.empty()?sLocalEmpAccount:sLocalEmail;
				pEmployeeInfo->m_sAddress = sLocalAddress;
				pEmployeeInfo->m_sDescription = sLocalDescription;
				pEmployeeInfo->m_nManagerLevel = nLocalManagerLevel;
				pEmployeeInfo->m_nGender = nLocalGender;
				pEmployeeInfo->m_nBirthday = nLocalBirthday;
				//pEmployeeInfo->m_nLineState = n_line_state;
				//pEmployeeInfo->m_sUmServer = s_um_server;
				//pEmployeeInfo->m_sUmAppName = s_um_app_name;
				pEmployeeInfo->m_sHeadResourceId = nLocalHeadResId;
				pEmployeeInfo->m_sHeadCmServer = sLocalHeadCmServer;
				//pEmployeeInfo->m_nCSId = nCSId;
				//pEmployeeInfo->m_nCSExt = nCSExt;
				pEmployeeInfo->m_nDisplayIndex = nDisplayIndex;
				pEmployeeInfo->m_nForbidMinutes = nForbidMinutes;
				ProcessMemberHeadInfo(pEmployeeInfo);
				const bool bSendGroupInfoEvent = (nIndex==0)?true:false;
				nIndex++;
				ProcessMemberInfo(pDepartmentInfo,pEmployeeInfo,bSendGroupInfoEvent);
				pRecord = m_pEBDatas->next(nCookie);
			}
			m_pEBDatas->reset(nCookie);
			if (bHasLocalMemberInfo && !pDepartmentInfo->m_pMemberList.empty() &&	// 群组成员不为空，表示前面已经加载过，否则有可能是未加载部门
				pDepartmentInfo->m_nEmpCount!=pDepartmentInfo->m_pMemberList.size() &&
				//pDepartmentInfo->m_nEmpCount>pDepartmentInfo->m_pMemberList.size() &&
				(pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER)!=EB_GROUP_DATA_LOAD_MEMBER)
			{
				// 群组成员数量不多，加载一次成员；
				if (pDepartmentInfo->m_nMyEmpId>0)
					theNeedLoadGroupMemberList.pushfront(pDepartmentInfo->m_sGroupCode);
				else
					theNeedLoadGroupMemberList.add(pDepartmentInfo->m_sGroupCode);
			}else if (pDepartmentInfo->m_sEnterpriseCode==0 && pDepartmentInfo->m_nMyEmpId==0)
			{
				// ?不属于自己的个人群组，加载一次群组资料
				theNeedLoadGroupInfoList.add(pDepartmentInfo->m_sGroupCode);
				//this->EnterpriseLoad(pDepartmentInfo->m_sGroupCode,1,1,0);
			}
		}
	}

	const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
	if (m_callback)
		m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
		pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
		pEvent->SetEventParameter((long)bIsMyDepartment?1:0);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);		// 企业部门/项目组
#endif
	}
	// 处理前面未处理，下一级部门数据
	ProcessSubGroupInfo(pDepartmentInfo->m_sGroupCode);
	// 默认加载我的群组成员
	if (bIsMyDepartment && pDepartmentInfo->m_nEmpCount>(int)pDepartmentInfo->m_pMemberList.size() &&
		(pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER)!=EB_GROUP_DATA_LOAD_MEMBER)
	{
		theNeedLoadGroupMemberList.pushfront(pDepartmentInfo->m_sGroupCode);
	}

	// 处理前面未处理群组成员
	std::vector<CEBMemberInfo::pointer> pDepMemberList; 
	theTempMemberList.find(pDepartmentInfo->m_sGroupCode,pDepMemberList);
	for (size_t i=0; i<pDepMemberList.size(); i++)
	{
		CEBMemberInfo::pointer pMemberInfo = pDepMemberList[i];
		ProcessMemberInfo(pDepartmentInfo,pMemberInfo);
	}
	theTempMemberList.remove(pDepartmentInfo->m_sGroupCode);
}
void CUserManagerApp::ProcessSubGroupInfo(mycp::bigint sGroupCode)
{
	CEBGroupInfo::pointer pDepartmentInfo;
	std::vector<CEBGroupInfo::pointer> pSubDepartmentInfos; 
	theTempDepartmentList.find(sGroupCode, pSubDepartmentInfos);
	for (size_t i=0; i<pSubDepartmentInfos.size(); i++)
	{
		// 找到下一级部门，需要处理
		pDepartmentInfo = pSubDepartmentInfos[i];
		if ((pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_NEED_SAVE2LOCAL)==EB_GROUP_DATA_NEED_SAVE2LOCAL)
		{
			pDepartmentInfo->m_nGroupData &= ~EB_GROUP_DATA_NEED_SAVE2LOCAL;
			SaveLocalGroupInfo(pDepartmentInfo);
		}

		BoostReadLock rdLock(m_mutexEBDatas);
		if (m_pEBDatas.get()!=NULL && pDepartmentInfo->m_pMemberList.empty())
		{
			// 本地缓存数据，如果我的部门不是根目录，不能自动加载，所以需要在这里加载一次；
			// **里面会通知群组资料事件，所以做一个if else；不通知二次；
			ProcessGroupInfo(pDepartmentInfo);
		}else
		{
			// **原来的OK
			const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
			if (m_callback)
			{
				m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
			}
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
				pEvent->SetEventParameter((long)bIsMyDepartment?1:0);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
			}
			ProcessSubGroupInfo(pDepartmentInfo->m_sGroupCode);
		}
	}
	theTempDepartmentList.remove(sGroupCode);
}
mycp::bigint CUserManagerApp::GetLocalEntGroupInfoVer(mycp::bigint nEntId) const
{
	mycp::bigint nLocalEntDepInfoVer = -1;
	//BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_mutexEBDatas));
	if (m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[128];
		sprintf(lpszBuffer,"SELECT ver FROM eb_ent_info_t WHERE ent_id=%lld",nEntId);
		int nCookie = 0;
		m_pEBDatas->select(lpszBuffer, nCookie);
		int nIndex = 0;
		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		if (pRecord.get()!=0)
		{
			nLocalEntDepInfoVer = pRecord->getVector()[0]->getBigIntValue();
			if (nLocalEntDepInfoVer==-1)
				nLocalEntDepInfoVer = 0;
		}
		m_pEBDatas->reset(nCookie);
	}
	return nLocalEntDepInfoVer;
}
mycp::bigint CUserManagerApp::GetLocalMyGroupInfoVer(mycp::bigint nUserId) const
{
	mycp::bigint nLocalMyDepInfoVer = -1;
	//BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_mutexEBDatas));
	if (m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[128];
		sprintf(lpszBuffer,"SELECT int_value FROM eb_user_ext_t WHERE uid=%lld AND ext_key=%d",nUserId,EB_USER_EXT_KEY_GROUP_INFO_VERSION);
		int nCookie = 0;
		m_pEBDatas->select(lpszBuffer, nCookie);
		int nIndex = 0;
		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		if (pRecord.get()!=0)
		{
			nLocalMyDepInfoVer = pRecord->getVector()[0]->getBigIntValue();
		}
		m_pEBDatas->reset(nCookie);
	}
	return nLocalMyDepInfoVer;
}
mycp::bigint CUserManagerApp::GetLocalMyContactInfoVer(mycp::bigint nUserId) const
{
	mycp::bigint nLocalMyContactInfoVer = -1;
	//BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_mutexEBDatas));
	if (m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[128];
		sprintf(lpszBuffer,"SELECT int_value FROM eb_user_ext_t WHERE uid=%lld AND ext_key=%d",nUserId,EB_USER_EXT_KEY_CONTACT_INFO_VERSION);
		int nCookie = 0;
		m_pEBDatas->select(lpszBuffer, nCookie);
		int nIndex = 0;
		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		if (pRecord.get()!=0)
		{
			nLocalMyContactInfoVer = pRecord->getVector()[0]->getBigIntValue();
		}
		m_pEBDatas->reset(nCookie);
	}
	return nLocalMyContactInfoVer;
}
void CUserManagerApp::SetLocalMyContactInfoVer(mycp::bigint nUserId, mycp::bigint nNewContactInfoVer) const
{
	if (m_pEBDatas.get()!=NULL)
	{
		// 更新本地部门成员资料版本号；
		char lpszBuffer[256];
		sprintf(lpszBuffer,"UPDATE eb_user_ext_t SET int_value=%lld WHERE uid=%lld AND ext_key=%d",nNewContactInfoVer,nUserId,(int)EB_USER_EXT_KEY_CONTACT_INFO_VERSION);
		if (m_pEBDatas->execute(lpszBuffer)!=1)
		{
			sprintf(lpszBuffer,"INSERT INTO eb_user_ext_t(id,uid,ext_key,int_value) VALUES(%lld,%lld,%d,%lld)",GetNextBigId(),nUserId,(int)EB_USER_EXT_KEY_CONTACT_INFO_VERSION,nNewContactInfoVer);
			m_pEBDatas->execute(lpszBuffer);
		}
	}
}

//void CUserManagerApp::SaveEntGroupInfo(mycp::bigint nEntId,mycp::bigint nNewGroupInfoVer)
//{
//	if (m_nServerEntDepInfoVer!=-1)
//	{
//		if (m_pEBDatas.get()!=NULL && m_pDefaultEnterpriseInfo.get()!=NULL)
//		{
//			char lpszBuffer[128];
//			if (GetLocalEntGroupInfoVer(m_pDefaultEnterpriseInfo->m_sEnterpriseCode)==-1)
//			{
//				sprintf(lpszBuffer,"INSERT INTO eb_ent_info_t(ent_id,ver) VALUES(%lld,%lld)",m_pDefaultEnterpriseInfo->m_sEnterpriseCode,m_nServerEntDepInfoVer);
//			}else
//			{
//				sprintf(lpszBuffer,"UPDATE eb_ent_info_t SET ver=%lld WHERE ent_id=%lld",m_nServerEntDepInfoVer,m_pDefaultEnterpriseInfo->m_sEnterpriseCode);
//			}
//			m_pEBDatas->execsql(lpszBuffer);
//		}
//		m_nServerEntDepInfoVer = -1;
//	}
//	//if (m_pEBDatas.get()!=NULL && nEntId>0)
//	//{
//	//	// 这里处理删除本地企业部门
//	//	char lpszBuffer[128];
//	//	sprintf(lpszBuffer,"SELECT group_id FROM eb_dep_info_t WHERE ent_id=%lld",nEntId);
//	//	bo::PRESULTSET pResltSet = NULL;
//	//	m_pEBDatas->execsql(lpszBuffer,&pResltSet);
//	//	if (pResltSet != NULL && pResltSet->rscount>0)
//	//	{
//	//		for (int i=0; i<pResltSet->rscount; i++)
//	//		{
//	//			const mycp::bigint nLocalGroupId = pResltSet->rsvalues[i]->fieldvalues[0]->v.bigintVal;
//	//			// *** 在这里判断删除本地无用部门数据
//	//			if (!this->theDepartmentList.exist(nLocalGroupId))
//	//			{
//	//				// 删除部门资料
//	//				sprintf(lpszBuffer,"DELETE FROM eb_dep_info_t WHERE group_id=%lld",nLocalGroupId);
//	//				m_pEBDatas->execsql(lpszBuffer);
//	//				// 删除部门成员
//	//				sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE group_id=%lld",nLocalGroupId);
//	//				m_pEBDatas->execsql(lpszBuffer);
//	//			}
//	//		}
//	//	}
//	//	bo::bodb_free(pResltSet);
//	//}
//}

//void CUserManagerApp::SaveMyGroupInfo(mycp::bigint nNewGroupInfoVer)
//{
//	if (m_nServerMyDepInfoVer!=-1)
//	{
//		if (m_pEBDatas.get()!=NULL && m_pMyAccountInfo.get()!=NULL)
//		{
//			// *更新我的个人群组资料版本号
//			char lpszBuffer[128];
//			if (GetLocalMyGroupInfoVer(m_pMyAccountInfo->GetUserId())==-1)
//			{
//				sprintf(lpszBuffer,"INSERT INTO eb_user_ext_t(id,uid,ext_key,int_value) VALUES(%lld,%lld,%d,%lld)",GetNextBigId(), m_pMyAccountInfo->GetUserId(),EB_USER_EXT_KEY_GROUP_INFO_VERSION,m_nServerMyDepInfoVer);
//			}else
//			{
//				sprintf(lpszBuffer,"UPDATE eb_user_ext_t SET int_value=%lld WHERE uid=%lld AND ext_key=%d",m_nServerMyDepInfoVer,m_pMyAccountInfo->GetUserId(),EB_USER_EXT_KEY_GROUP_INFO_VERSION);
//			}
//			m_pEBDatas->execsql(lpszBuffer);
//		}
//		m_nServerMyDepInfoVer = -1;
//	}
//	//if (m_pEBDatas.get()!=NULL)
//	//{
//	//	// 这里处理删除本地个人群组
//	//	char lpszBuffer[128];
//	//	sprintf(lpszBuffer,"SELECT group_id FROM eb_dep_info_t WHERE ent_id=0");
//	//	bo::PRESULTSET pResltSet = NULL;
//	//	m_pEBDatas->execsql(lpszBuffer,&pResltSet);
//	//	if (pResltSet != NULL && pResltSet->rscount>0)
//	//	{
//	//		for (int i=0; i<pResltSet->rscount; i++)
//	//		{
//	//			const mycp::bigint nLocalGroupId = pResltSet->rsvalues[i]->fieldvalues[0]->v.bigintVal;
//	//			// *** 在这里判断删除本地无用部门数据
//	//			if (!this->theDepartmentList.exist(nLocalGroupId))
//	//			{
//	//				// 删除群组资料
//	//				sprintf(lpszBuffer,"DELETE FROM eb_dep_info_t WHERE group_id=%lld",nLocalGroupId);
//	//				m_pEBDatas->execsql(lpszBuffer);
//	//				// 删除群组成员
//	//				sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE group_id=%lld",nLocalGroupId);
//	//				m_pEBDatas->execsql(lpszBuffer);
//	//			}
//	//		}
//	//	}
//	//	bo::bodb_free(pResltSet);
//	//}
//}

void CUserManagerApp::OnFUMEMDepInfo(const CEBGroupInfo::pointer& pDepartmentInfo,mycp::bigint nGroupInfoVer,mycp::bigint nCallGroupId)
{
    /// for test
//    if (pDepartmentInfo->m_sGroupCode==999121) {
//        int i=0;
//    }
    if (pDepartmentInfo->m_sEnterpriseCode>0 && m_nServerEntDepInfoVer!=-1 && (int)GetEntGroupSize()>=theServerEntDepInfoSize)
	{
		BoostReadLock rdLock(m_mutexEBDatas);
		if (m_pEBDatas.get()!=NULL && m_pDefaultEnterpriseInfo.get()!=NULL)
		{
			char lpszBuffer[256];
			if (this->GetLocalEntGroupInfoVer(m_pDefaultEnterpriseInfo->m_sEnterpriseCode)==-1)
				sprintf(lpszBuffer,"INSERT INTO eb_ent_info_t(ent_id,ver) VALUES(%lld,%lld)",m_pDefaultEnterpriseInfo->m_sEnterpriseCode,m_nServerEntDepInfoVer);
			else
				sprintf(lpszBuffer,"UPDATE eb_ent_info_t SET ver=%lld WHERE ent_id=%lld",m_nServerEntDepInfoVer,m_pDefaultEnterpriseInfo->m_sEnterpriseCode);
			m_pEBDatas->execute(lpszBuffer);
		}
		m_nServerEntDepInfoVer = -1;
		// 这里加载一次所有企业部门在线人数；
		LoadInfo(0,0,0,0,1);
	}

	const mycp::bigint s_dep_code = pDepartmentInfo->m_sGroupCode;
	const mycp::bigint s_parent_code = pDepartmentInfo->m_sParentCode;
	if (s_parent_code>0 && !theDepartmentList.exist(s_parent_code))
	{
		// 暂时找不到上一级部门
		pDepartmentInfo->m_nGroupData |= EB_GROUP_DATA_NEED_SAVE2LOCAL;
		theTempDepartmentList.insert(s_parent_code, pDepartmentInfo);
		return ;
	}
	//if (s_dep_code==999004)
	//{
	//	int i=0;
	//}
	// 保存数据库
	SaveLocalGroupInfo(pDepartmentInfo);

	//if (pDepartmentInfo->m_sEnterpriseCode>0 && GetEntGroupSize()>=this->m_nEntDepInfoSize)
	//{
	//	SaveEntGroupInfo(pDepartmentInfo->m_sEnterpriseCode,m_nServerEntDepInfoVer);
	//}else if (pDepartmentInfo->m_sEnterpriseCode==0 && GetUnEntGroupSize()>=this->m_nMyDepInfoSize)
	//{
	//	SaveMyGroupInfo(m_nServerMyDepInfoVer);
	//}

	if (nGroupInfoVer>0)
	{
		// 单独加载某群组资料时，返回版本号，自动判断并更新；
		UpdateLocalGroupInfoVer(pDepartmentInfo->m_sEnterpriseCode,nGroupInfoVer);
		// 加载一次群组在线人数；
		if (pDepartmentInfo->m_nOnlineSize<=0)	// 可能是群组（讨论组），需要加载一次在线人数；
			LoadInfo(0,0,0,0,0,0,pDepartmentInfo->m_sGroupCode);

		if ((int)pDepartmentInfo->m_pMemberList.size()>=pDepartmentInfo->m_nEmpCount)
		{
			theCheckGroupMemberList.remove(s_dep_code);
			theGroupMemberDataTime.remove(s_dep_code);
			SaveGroupMemberInfo(pDepartmentInfo);
		}
	}
	ProcessGroupInfo(pDepartmentInfo);

	if (nCallGroupId>0 && nCallGroupId==s_dep_code)
	{
		CallGroup(nCallGroupId,true);
	}
}
void CUserManagerApp::OnFUMEMDepInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	// 是否请求呼叫该群组；
	const mycp::bigint nCallGroupId = pSotpRequestInfo.get()==NULL?0:pSotpRequestInfo->m_pRequestList.getParameterValue("call-group-id",(mycp::bigint)0);
	const mycp::bigint s_ent_code = pSotpResponseInfo->m_pResponseList.getParameterValue("ent_code",(mycp::bigint)0);
	const mycp::bigint s_dep_code = pSotpResponseInfo->m_pResponseList.getParameterValue("dep_code",(mycp::bigint)0);
	const tstring s_dep_name = pSotpResponseInfo->m_pResponseList.getParameterValue("dep_name");
	const mycp::bigint s_parent_code = pSotpResponseInfo->m_pResponseList.getParameterValue("parent_code",(mycp::bigint)0);
	const tstring s_description = pSotpResponseInfo->m_pResponseList.getParameterValue("description");
	const tstring s_phone = pSotpResponseInfo->m_pResponseList.getParameterValue("phone");
	const tstring s_fax = pSotpResponseInfo->m_pResponseList.getParameterValue("fax");
	const tstring s_email = pSotpResponseInfo->m_pResponseList.getParameterValue("email");
	const tstring s_url = pSotpResponseInfo->m_pResponseList.getParameterValue("url");
	const tstring s_address = pSotpResponseInfo->m_pResponseList.getParameterValue("address");
	const tstring s_create_time = pSotpResponseInfo->m_pResponseList.getParameterValue("create_time", "");
	const mycp::bigint nMyEmpId = pSotpResponseInfo->m_pResponseList.getParameterValue("my_emp_id", (mycp::bigint)0);
	const int nEmpCount = pSotpResponseInfo->m_pResponseList.getParameterValue("emp_count", (int)0);
	const mycp::bigint nGroupVer = pSotpResponseInfo->m_pResponseList.getParameterValue("dep_ver", (mycp::bigint)0);
	const int nExtData = pSotpResponseInfo->m_pResponseList.getParameterValue("ext_data", (int)0);
	const tstring s_creator = pSotpResponseInfo->m_pResponseList.getParameterValue("creator");
	const mycp::bigint nCreateUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("create_uid",(mycp::bigint)0);
	const mycp::bigint nManagerUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("manager_uid",(mycp::bigint)0);
	const int nDisplayIndex = pSotpResponseInfo->m_pResponseList.getParameterValue("display_index",(int)0);
	const int n_type = pSotpResponseInfo->m_pResponseList.getParameterValue("type", (int)0);
	const int nEntDepOffset = pSotpResponseInfo->m_pResponseList.getParameterValue("ent-dep-offset", (int)-1);
	const int nEntDepCount = pSotpResponseInfo->m_pResponseList.getParameterValue("ent-dep-count", (int)-1);
	const int nMyDepOffset = pSotpResponseInfo->m_pResponseList.getParameterValue("my-dep-offset", (int)-1);
	const int nMyDepCount = pSotpResponseInfo->m_pResponseList.getParameterValue("my-dep-count", (int)-1);
	const mycp::bigint nGroupInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("group_info_ver", (mycp::bigint)-1);
	if (s_dep_code==0 || nEntDepCount>=0 || nMyDepCount>=0)
	{
		if (nMyDepCount>=0 && m_nMyDepInfoSize==-1)
		{
			m_nMyDepInfoSize = nMyDepCount;
		}else if (nEntDepCount>=0 && m_nEntDepInfoSize==-1)
		{
			m_nEntDepInfoSize = nEntDepCount;
		}


		if (m_nServerEntDepInfoVer!=-1 && nEntDepCount>=0)
		{
			// *更新企业部门资料版本号
			BoostReadLock rdLock(m_mutexEBDatas);
			if (m_pEBDatas.get()!=NULL && m_pDefaultEnterpriseInfo.get()!=NULL)
			{
				char lpszBuffer[256];
				if (this->GetLocalEntGroupInfoVer(m_pDefaultEnterpriseInfo->m_sEnterpriseCode)==-1)
					sprintf(lpszBuffer,"INSERT INTO eb_ent_info_t(ent_id,ver) VALUES(%lld,%lld)",m_pDefaultEnterpriseInfo->m_sEnterpriseCode,m_nServerEntDepInfoVer);
				else
					sprintf(lpszBuffer,"UPDATE eb_ent_info_t SET ver=%lld WHERE ent_id=%lld",m_nServerEntDepInfoVer,m_pDefaultEnterpriseInfo->m_sEnterpriseCode);
				m_pEBDatas->execute(lpszBuffer);
			}
			m_nServerEntDepInfoVer = -1;
			// 这里加载一次所有企业部门在线人数；
			LoadInfo(0,0,0,0,1);
		}else if (m_nServerMyDepInfoVer!=-1 && nMyDepCount>=0)
		{
			BoostReadLock rdLock(m_mutexEBDatas);
			if (m_pEBDatas.get()!=NULL && m_pMyAccountInfo.get()!=NULL)
			{
				// *更新我的个人群组资料版本号
				char lpszBuffer[256];
				if (this->GetLocalMyGroupInfoVer(m_pMyAccountInfo->GetUserId())==-1)
				{
					sprintf(lpszBuffer,"INSERT INTO eb_user_ext_t(id,uid,ext_key,int_value) VALUES(%lld,%lld,%d,%lld)",GetNextBigId(), m_pMyAccountInfo->GetUserId(),EB_USER_EXT_KEY_GROUP_INFO_VERSION,m_nServerMyDepInfoVer);
				}else
				{
					sprintf(lpszBuffer,"UPDATE eb_user_ext_t SET int_value=%lld WHERE uid=%lld AND ext_key=%d",m_nServerMyDepInfoVer,m_pMyAccountInfo->GetUserId(),EB_USER_EXT_KEY_GROUP_INFO_VERSION);
				}
				m_pEBDatas->execute(lpszBuffer);
			}
			m_nServerMyDepInfoVer = -1;
			// 这里加载一次所有个人群组在线人数；
			LoadInfo(0,0,0,0,0,1);
		}
		return;
	}

	theCheckGroupInfoList.remove(s_dep_code);
	CEBGroupInfo::pointer pDepartmentInfo;
	if (!theDepartmentList.find(s_dep_code, pDepartmentInfo))
	{
		pDepartmentInfo = CEBGroupInfo::create(s_ent_code, s_dep_code);
		theDepartmentList.insert(s_dep_code, pDepartmentInfo);
		if (s_ent_code==0)
		{
			m_nReturnMyGroupInfoSize++;
		}else
		{
			m_nReturnEntGroupInfoSize++;
		}
	}else if (pDepartmentInfo->m_sEnterpriseCode!=s_ent_code)
	{
		pDepartmentInfo->m_sEnterpriseCode = s_ent_code;
	}
    /// for test
//    if (s_dep_code==999121) {
//        int i=0;
//    }
	pDepartmentInfo->m_sGroupName = s_dep_name;
	pDepartmentInfo->m_sParentCode = s_parent_code;
	pDepartmentInfo->m_sDescription = s_description;
	pDepartmentInfo->m_sPhone = s_phone;
	pDepartmentInfo->m_sFax = s_fax;
	pDepartmentInfo->m_sEmail = s_email;
	pDepartmentInfo->m_sUrl = s_url;
	pDepartmentInfo->m_sAddress = s_address;
	pDepartmentInfo->m_sCreateTime = s_create_time;
	pDepartmentInfo->m_nCreateUserId = nCreateUserId;
	pDepartmentInfo->m_nManagerUserId = nManagerUserId;
	pDepartmentInfo->m_nDisplayIndex = nDisplayIndex;
	pDepartmentInfo->m_nGroupType = (EB_GROUP_TYPE)n_type;
	pDepartmentInfo->m_nMyEmpId = nMyEmpId;
	pDepartmentInfo->m_nEmpCount = nEmpCount;
	pDepartmentInfo->m_nServerGroupVer = nGroupVer;
	pDepartmentInfo->m_nGroupExtData = nExtData;

	if (nEmpCount==0)
	{
		//theCheckGroupMemberList.remove(s_dep_code);
		//theGroupMemberDataTime.remove(s_dep_code);
	}
	//pDepartmentInfo->m_nGroupVer = nGroupVer;
	//if (m_file==NULL)
	//{
	//	m_file = fopen("d:\\eb_group_info.txt","w+");
	//}
	//static int theEmpCount = 0;
	//theEmpCount += nEmpCount;
	//char lpszBuffer[256];
	//sprintf(lpszBuffer,"new groupid=%lld, group__ver=%lld, my_emp_id=%lld,emp_count=%d(%d)\n",s_dep_code,nGroupVer,nMyEmpId,nEmpCount,theEmpCount);
	//fwrite(lpszBuffer,1,strlen(lpszBuffer),m_file);

	CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_F_GROUP_INFO);
	pProcessMsgInfo->m_pDepartmentInfo = pDepartmentInfo;
	pProcessMsgInfo->m_nReturnVer = nGroupInfoVer;
	pProcessMsgInfo->m_nCallGroupId = nCallGroupId;
	m_pProcessMsgList.add(pProcessMsgInfo);

	//if (pDepartmentInfo->m_sEnterpriseCode>0 && m_nServerEntDepInfoVer!=-1 && GetEntGroupSize()>=theServerEntDepInfoSize)
	//{
	//	BoostReadLock rdLock(m_mutexEBDatas);
	//	if (m_pEBDatas.get()!=NULL && m_pDefaultEnterpriseInfo.get()!=NULL)
	//	{
	//		char lpszBuffer[256];
	//		if (this->GetLocalEntGroupInfoVer(m_pDefaultEnterpriseInfo->m_sEnterpriseCode)==-1)
	//			sprintf(lpszBuffer,"INSERT INTO eb_ent_info_t(ent_id,ver) VALUES(%lld,%lld)",m_pDefaultEnterpriseInfo->m_sEnterpriseCode,m_nServerEntDepInfoVer);
	//		else
	//			sprintf(lpszBuffer,"UPDATE eb_ent_info_t SET ver=%lld WHERE ent_id=%lld",m_nServerEntDepInfoVer,m_pDefaultEnterpriseInfo->m_sEnterpriseCode);
	//		m_pEBDatas->execute(lpszBuffer);
	//	}
	//	m_nServerEntDepInfoVer = -1;
	//	// 这里加载一次所有企业部门在线人数；
	//	LoadInfo(0,0,0,0,1);
	//}

	//if (s_parent_code>0 && !theDepartmentList.exist(s_parent_code))
	//{
	//	// 暂时找不到上一级部门
	//	pDepartmentInfo->m_nGroupData |= EB_GROUP_DATA_NEED_SAVE2LOCAL;
	//	theTempDepartmentList.insert(s_parent_code, pDepartmentInfo);
	//	return ;
	//}
	////if (s_dep_code==999004)
	////{
	////	int i=0;
	////}
	//// 保存数据库
	//SaveLocalGroupInfo(pDepartmentInfo);

	////if (pDepartmentInfo->m_sEnterpriseCode>0 && GetEntGroupSize()>=this->m_nEntDepInfoSize)
	////{
	////	SaveEntGroupInfo(pDepartmentInfo->m_sEnterpriseCode,m_nServerEntDepInfoVer);
	////}else if (pDepartmentInfo->m_sEnterpriseCode==0 && GetUnEntGroupSize()>=this->m_nMyDepInfoSize)
	////{
	////	SaveMyGroupInfo(m_nServerMyDepInfoVer);
	////}

	//if (nGroupInfoVer>0)
	//{
	//	// 单独加载某群组资料时，返回版本号，自动判断并更新；
	//	UpdateLocalGroupInfoVer(pDepartmentInfo->m_sEnterpriseCode,nGroupInfoVer);
	//	// 加载一次群组在线人数；
	//	if (pDepartmentInfo->m_nOnlineSize<=0)	// 可能是群组（讨论组），需要加载一次在线人数；
	//		LoadInfo(0,0,0,0,0,0,pDepartmentInfo->m_sGroupCode);
	//}
	//ProcessGroupInfo(pDepartmentInfo);

	//if (nCallGroupId>0 && nCallGroupId==s_dep_code)
	//{
	//	CallGroup(nCallGroupId,true);
	//}
}

void CUserManagerApp::ProcessToSendList(mycp::bigint nCallId, bool bSendGroupCall, bool bCheckCallTime)
//void CUserManagerApp::ProcessToSendList(mycp::bigint nCallId, bool bSendGroupCall, mycp::bigint nWaitFromUidP2PState)
{
	LogMessage("ProcessToSendList... (callid=%lld,bCheckCallTime=%d)\r\n",nCallId,(int)(bCheckCallTime?1:0));
	if (m_pP2PTaskInfo.exist(nCallId))	// ** 正在做P2P，先不处理；
	{
		//LogMessage("ProcessToSendList->m_pP2PTaskInfo.exist TRUE... (callid=%lld,bCheckCallTime=%d)\r\n",nCallId,(int)(bCheckCallTime?1:0));
		return;
	}else if (bCheckCallTime)
	{
		CEBCallInfo::pointer pCallInfo;
		if (!theCallInfoList.find(nCallId,pCallInfo))
		{
			return ;
		}
		if (pCallInfo->m_nCallTime>0 && pCallInfo->m_nCallTime+3>=time(0))	// 0123 = 4秒
		{
			if (m_pWaitList1.insert(nCallId,0,false))
			{
				CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_TO_SEND_LIST);
				pProcessMsgInfo->m_tProcessTime = time(0)+10;
				pProcessMsgInfo->m_pCallInfo = pCallInfo;
				pProcessMsgInfo->m_nBigInt1 = bSendGroupCall?1:0;
				m_pProcessMsgList.add(pProcessMsgInfo);
			}
			return;	// 正在呼叫，不处理
		}
	}
	m_pWaitList1.remove(nCallId);

	CToSendList::pointer pCallToSendList;
	if (!theCallToSendList.find(nCallId,pCallToSendList)) return;

	if ((bSendGroupCall && pCallToSendList->m_nGroupCode>0) ||
		(!bSendGroupCall && pCallToSendList->m_nGroupCode==0))
	{
#ifdef _QT_MAKE_
        CLockMap<QString,bool> pSendFileList;		// **预防发送多次；
#else
        CLockMap<tstring,bool> pSendFileList;		// **预防发送多次；
#endif
		CToSendInfo::pointer pCallToSendInfoTemp;	// 用于记录文件是否已经处理；
		while(!pCallToSendList->m_list.empty())
		{
			CToSendInfo::pointer pCallToSendInfo;
			if (!pCallToSendList->m_list.front(pCallToSendInfo))
			{
				// 发送列表为空；
				break;
			}
			if (pCallToSendInfoTemp.get()!=NULL && pCallToSendInfoTemp.get()==pCallToSendInfo.get())
			{
				// *剩下文件等待发送；
				return;
			}
			if (pCallToSendInfo->IsRich())
			{
				SendCrRich(nCallId,pCallToSendInfo->m_rich.get(),pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,true);
			}else
			{
				//if (pCallToSendList->m_nGroupCode==0 && nWaitFromUidP2PState!=0 && nWaitFromUidP2PState!=m_pMyAccountInfo->GetUserId())
				//{
				//	// 一对一聊天，发送文件需要等待P2P状态，重新加入列表（处理到最后，列表只剩下所有未等待发送文件。）
				//	if (pCallToSendInfoTemp.get()==NULL)
				//	{
				//		// 增加这一个，是为了最后检查时，删除掉没用；
				//		pCallToSendInfoTemp = pCallToSendInfo;
				//		pCallToSendList->m_list.add(pCallToSendInfo);
				//	}
				//	pCallToSendList->m_list.add(pCallToSendInfo);
				//	continue;
				//}
				if (!pSendFileList.exist(pCallToSendInfo->m_sFilePath))
				{
					pSendFileList.insert(pCallToSendInfo->m_sFilePath,true,false);
                    if (pCallToSendInfo->m_nResourceId>0)
                        SendCrFile(nCallId,pCallToSendInfo->m_sFilePath,pCallToSendInfo->m_nResourceId);
                    else
                        SendCrFile(nCallId,pCallToSendInfo->m_sFilePath,pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile,true);
				}
			}
		}
		theCallToSendList.remove(nCallId);
	}
}

void CUserManagerApp::OnEnterRoom(const CCrInfo& pParameter)
{
	if (m_pMyAccountInfo.get()==NULL) return;
	const mycp::bigint sCallId = pParameter.GetCallId();
	if (pParameter.GetStateCode() == EB_STATE_OK || pParameter.GetStateCode() == EB_STATE_NEED_RESEND)
	{
		this->EnterChat(sCallId,m_pMyAccountInfo->GetUserId(),false);
		ProcessToSendList(sCallId,true,true);
		if (pParameter.GetStateCode() == EB_STATE_NEED_RESEND)
		{
			// 
			int i=0;
		}
		//ProcessToSendList(sCallId,true);

		//CToSendList::pointer pCallToSendList;
		//if (theCallToSendList.find(sCallId,pCallToSendList))
		//{
		//	if (pCallToSendList->m_nGroupCode>0)
		//	{
		//		while(!pCallToSendList->m_list.empty())
		//		{
		//			CToSendInfo::pointer pCallToSendInfo;
		//			if (!pCallToSendList->m_list.front(pCallToSendInfo))
		//			{
		//				break;
		//			}
		//			if (pCallToSendInfo->IsRich())
		//				SendCrRich(sCallId,pCallToSendInfo->m_rich.get(),pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate);
		//			else
		//				SendCrFile(sCallId,pCallToSendInfo->m_sFilePath.c_str(),pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile);
		//		}
		//		theCallToSendList.remove(sCallId);
		//	}
		//}

	}
}
void CUserManagerApp::OnUserEnterRoom(const CCrAccountInfo& pAccountInfo)
{
	const mycp::bigint sCallId = pAccountInfo.GetCallId();
	LogMessage("OnUserEnterRoom... (uid=%lld,offline=%d)\r\n",pAccountInfo.m_sAccount,(int)(pAccountInfo.m_bOffLine?1:0));
	this->EnterChat(sCallId,pAccountInfo.m_sAccount,pAccountInfo.m_bOffLine);

	if (m_callback)
		m_callback->OnUserEnterRoom(pAccountInfo);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		CCrAccountInfo * pEvent = new CCrAccountInfo(pAccountInfo);
		pEvent->SetQEventType((QEvent::Type)CR_WM_USER_ENTER_ROOM);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::SendMessage(m_pHwnd, CR_WM_USER_ENTER_ROOM, (WPARAM)&pAccountInfo, 0);
#endif
	}
	const bool bCheckCallTime = pAccountInfo.m_bOffLine?false:true;
	ProcessToSendList(sCallId,false,bCheckCallTime);
	//ProcessToSendList(sCallId,false,pAccountInfo.m_sAccount);

	//CToSendList::pointer pCallToSendList;
	//if (theCallToSendList.find(sCallId,pCallToSendList))
	//{
	//	if (pCallToSendList->m_nGroupCode==0)
	//	{
	//		while(!pCallToSendList->m_list.empty())
	//		{
	//			CToSendInfo::pointer pCallToSendInfo;
	//			if (!pCallToSendList->m_list.front(pCallToSendInfo))
	//			{
	//				break;
	//			}
	//			if (pCallToSendInfo->IsRich())
	//				SendCrRich(sCallId,pCallToSendInfo->m_rich.get(),pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate);
	//			else
	//				SendCrFile(sCallId,pCallToSendInfo->m_sFilePath.c_str(),pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile);
	//		}
	//		theCallToSendList.remove(sCallId);
	//	}
	//}
}
void CUserManagerApp::OnUserExitRoom(const CCrAccountInfo& pAccountInfo, bool bHangup)
{
	const mycp::bigint sCallId = pAccountInfo.GetCallId();
	EB_USER_LINE_STATE pLineState = EB_LINE_STATE_UNKNOWN;
	this->ExitChat(sCallId,pAccountInfo.m_sAccount,bHangup,pLineState);
	if (pLineState != EB_LINE_STATE_UNKNOWN)	// 避免重复处理
	//if (pLineState != EB_LINE_STATE_OFFLINE)	// 避免重复处理
	{
		if (m_callback)
			m_callback->OnUserExitRoom(pAccountInfo,bHangup);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrAccountInfo * pEvent = new CCrAccountInfo(pAccountInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_USER_EXIT_ROOM);
            pEvent->SetEventParameter((long)(bHangup?1:0));
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_USER_EXIT_ROOM, (WPARAM)&pAccountInfo, bHangup?1:0);
#endif
		}
	}
}
void CUserManagerApp::OnUserNotify(const CCrNotifyInfo& pNotifyInfo)
{
	const mycp::bigint sCallId = pNotifyInfo.GetCallId();
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}else if (!pCallInfo->m_pCallUserList.exist(pNotifyInfo.m_nFromAccount))
		return;

	if (pNotifyInfo.m_nNotifyType == 2)
	{
		SendConnectMsg(pCallInfo, pNotifyInfo.m_nFromAccount,"",EB_CONNECTED_MSG);	// ?
	}

	if (m_callback)
		m_callback->OnUserNotify(pNotifyInfo);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		CCrNotifyInfo * pEvent = new CCrNotifyInfo(pNotifyInfo);
		pEvent->SetQEventType((QEvent::Type)CR_WM_USER_NOTIFY);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::SendMessage(m_pHwnd, CR_WM_USER_NOTIFY, (WPARAM)&pNotifyInfo, 0);
#endif
	}

	//if (pNotifyInfo.m_nNotityId>0 && pNotifyInfo.GetStateCode()==EB_STATE_NEED_DELETE_MSG)
	//{
	//	UmMsgAck(pNotifyInfo.m_nNotityId, EB_CAT_DELETE);
	//}
}
void CUserManagerApp::OnSendRich(const CCrRichInfo& pCrMsgInfo)
{
	const mycp::bigint sCallId = pCrMsgInfo.GetCallId();
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}
	pCallInfo->m_tLastTime = time(0);
	const EB_STATE_CODE nState = pCrMsgInfo.GetStateCode();
	switch (nState)
	{
	case EB_STATE_ACCOUNT_NOT_EXIST:
	case EB_STATE_NOT_MEMBER_ERROR:
	case EB_STATE_CHAT_NOT_EXIST:
	case EB_STATE_TIMEOUT_ERROR:
		{
			// 54：会话成员为空，重新呼叫一次
			pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
			this->InviteCall(pCallInfo,0);
		}break;
	default:
		break;
	}
	//}else if (nState == EB_STATE_TIMEOUT_ERROR)
	//{
	//	pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
	//	this->InviteCall(pCallInfo,0);
	//}
	if (m_callback)
		m_callback->OnSendRich(pCrMsgInfo);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		CCrRichInfo * pEvent = new CCrRichInfo(pCrMsgInfo);
		pEvent->SetQEventType((QEvent::Type)CR_WM_SEND_RICH);
        postWaitEventResult(m_pHwnd, pEvent);
#else
		::SendMessage(m_pHwnd, CR_WM_SEND_RICH, (WPARAM)&pCrMsgInfo, 0);	// nState
#endif
	}
}
int CUserManagerApp::OnReceiveRich(const CCrRichInfo& pCrMsgInfo)
{
	const mycp::bigint sCallId = pCrMsgInfo.GetCallId();
	//MessageBox(NULL,_T("OnReceiveRich"),_T(""),MB_OK);
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		LogMessage("CUserManagerApp::OnReceiveRich not find callinfo, callid=%lld, msgid=%lld\r\n",sCallId,pCrMsgInfo.m_pRichMsg->GetMsgId());
		//// ?这里不知有没有用
		//const tstring sSender(pCrMsgInfo.m_pSendInfo.m_sFrom);
		//EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
		//CEBCallInfo::pointer pInCallInfo = GetCallInfoByUser(sSender,pOutLineState);
		//if (pInCallInfo.get() != NULL && pOutLineState == EB_LINE_STATE_ONLINE_NEW &&
		//	(pInCallInfo->m_nCallState == EB_CALL_STATE_EXIT || pInCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED))
		//{
		//	pCallInfo = pInCallInfo;
		//}else
		//MessageBox(NULL,_T("OnReceiveRich"),_T("error"),MB_OK);
		{
			return -1;
		}
	}else
	{
		pCallInfo->m_tLastTime = time(0);
		CEBAccountInfo::pointer pSendFromAccountInfo;
		if (pCallInfo->m_pCallUserList.find(pCrMsgInfo.m_sSendFrom, pSendFromAccountInfo))
		{
			pSendFromAccountInfo->SetLineState(EB_LINE_STATE_ONLINE_NEW);
		}else if (pCallInfo->m_sGroupCode>0)
		{
			EnterpriseLoad(pCallInfo->m_sGroupCode,0,0,0,0,pCrMsgInfo.m_sSendFrom);	// 预防万一
		}
	}
    CLockMap<EBFileString,bool> pDownloadFileList;	/// 这里是企业图标文件，先简单做，用延时等待下载完成，加超时处理
	// 检查资源文件，是否存在
	const EB_ChatRoomRichMsg * pRichMsg = pCrMsgInfo.m_pRichMsg;
	const std::vector<EB_ChatRoomMsgItem*>& pRichMsgList = pRichMsg->GetList();
	for (size_t i=0; i<pRichMsgList.size(); i++)
	{
		const EB_ChatRoomMsgItem* pMsgItem = pRichMsgList[i];
		if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_RESOURCE)
		{
			const tstring& sResourceInfo = pMsgItem->GetText();
			std::vector<tstring> pList;
			if (ParseString(sResourceInfo,";",false,pList)>=3)
			{
				const tstring lpszResourceId(pList[0]);
				const mycp::bigint sResourceId = cgc_atoi64(lpszResourceId.c_str());
				const tstring sCmServer(pList[1]);
				//if (sCmServer=="cm-static1.entboost.com:80")	// ???测试
				//{
				//	sCmServer="cm-static1.entboost.com:18012";
				//}
				//const tstring sCmAppName = pList[2];
				//const tstring sDescription = pList.size()>3?pList[3]:"";
#ifdef _QT_MAKE_
                const QString sResourceFileTemp = QString("%1/%2").arg(m_sEbResourcePath).arg(lpszResourceId.c_str());
                if (!QFileInfo::exists(sResourceFileTemp))
#else
                tstring sResourceFile = m_sEbResourcePath + "\\";
                sResourceFile.append(lpszResourceId);
                if (!::PathFileExistsA(sResourceFile.c_str()))
#endif
				{
#ifdef _QT_MAKE_
                    const EBFileString sResourceFile = sResourceFileTemp;
#endif
					// 资源文件不存在
					if (pDownloadFileList.exist(sResourceFile) || theResourceChatRoom.exist(sResourceId))
					{
						// 前面已经在下载
					}else
					{
						pDownloadFileList.insert(sResourceFile,true);
						// 请求下载...
						const tstring sCmServerKey = GetCmServerKey(sCmServer);
						Cchatroom::pointer pChatRoom;
						if (!theChatRoomList.find(sCmServerKey,pChatRoom))
						{
							pChatRoom = Cchatroom::create();
							pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
							pChatRoom->Start(sCmServer.c_str(), "",0,true,1);
							pChatRoom->SetCallBack(this);
							theChatRoomList.insert(sCmServerKey,pChatRoom);
						}
						theResourceChatRoom.insert(sResourceId,pChatRoom);
						CFilePathInfo::pointer pFilePathInfo = CFilePathInfo::create(CFilePathInfo::FILE_ENT_IMAGE,sResourceFile,false);
						theResourceFilePath.insert(sResourceId,pFilePathInfo);
						if (!pChatRoom->IsEnterOk())
							pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
#ifdef _QT_MAKE_
                        pChatRoom->RecviveResource(sResourceId,sResourceFile);
#else
                        pChatRoom->RecviveResource(sResourceId,sResourceFile.c_str());
#endif
					}
				}
			}
		}
	}
	// 等待文件下载完成
	if (!pDownloadFileList.empty())
	{
		int nWaitSecond = 0;
		while (nWaitSecond<4)	// 4秒超时处理
		{
			bool bFileNotExist = false;
            CLockMap<EBFileString,bool>::iterator pIter = pDownloadFileList.begin();
			for (;pIter!=pDownloadFileList.end();pIter++)
			{
                const EBFileString& sResourceFile = pIter->first;
#ifdef _QT_MAKE_
                if (QFileInfo::exists(sResourceFile))
#else
                if (::PathFileExistsA(sResourceFile.c_str()))
#endif
				{
					pDownloadFileList.erase(pIter);
					if (!pDownloadFileList.empty())
					{
                        /// 处理下一个
						bFileNotExist = true;
					}
					break;
				}else
				{
					bFileNotExist = true;
					nWaitSecond++;
#ifdef WIN32
					Sleep(1000);
#else
                    sleep(1);
#endif
					break;
				}
			}
			if (!bFileNotExist)
			{
                /// 文件下载完成
				break;
			}
		}
	}

	//char lpszBuffer[200];
	//sprintf(lpszBuffer,"OnReceiveRich from=%lld,callid=%lld,groupid=%lld",pCrMsgInfo.m_sSendFrom,pCallInfo->GetCallId(),pCallInfo->m_sGroupCode);
	//MessageBox(NULL,lpszBuffer,_T(""),MB_OK);
	SendConnectMsg(pCallInfo, pCrMsgInfo.m_sSendFrom,"",EB_CONNECTED_MSG);	// ?
	int ret = -1;	// -1错误，外面不响应
	if (pCrMsgInfo.m_sSendFrom==m_pMyAccountInfo->GetUserId())
	{
		ret = 0;
		const_cast<CCrRichInfo&>(pCrMsgInfo).SetStateCode(EB_STATE_FORWARD_MSG);
		if (m_callback)
			m_callback->OnSendRich(pCrMsgInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrRichInfo * pEvent = new CCrRichInfo(pCrMsgInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_SEND_RICH);
            postWaitEventResult(m_pHwnd, pEvent);
#else
			::SendMessage(m_pHwnd, CR_WM_SEND_RICH, (WPARAM)&pCrMsgInfo, 0);	// EB_STATE_FORWARD_MSG
#endif
		}
	}else
	{
		if (m_callback)
			ret = m_callback->OnReceiveRich(pCrMsgInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            CCrRichInfo * pEvent = new CCrRichInfo(pCrMsgInfo);
            pEvent->SetQEventType((QEvent::Type)CR_WM_RECEIVE_RICH);
            ret = (int)postWaitEventResult(m_pHwnd, pEvent);
#else
			ret = (int)::SendMessage(m_pHwnd, CR_WM_RECEIVE_RICH, (WPARAM)&pCrMsgInfo,0);
#endif
		}
		//LogMessage("CUserManagerApp::OnReceiveRich SendMessage() = %d, callid=%lld, msgid=%lld,groupid=%lld(InviteCall...)\r\n",ret,sCallId,pCrMsgInfo.m_pRichMsg->GetMsgId(),pCallInfo->m_sGroupCode);
		if (ret==-1)
		{
			LogMessage("CUserManagerApp::OnReceiveRich SendMessage() -1 error, callid=%lld, msgid=%lld,groupid=%lld(InviteCall...)\r\n",sCallId,pCrMsgInfo.m_pRichMsg->GetMsgId(),pCallInfo->m_sGroupCode);
			CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_INVITE_CALL);
			//pProcessMsgInfo->m_tProcessTime = time(0)+3;
			pProcessMsgInfo->m_pCallInfo = pCallInfo;
			pProcessMsgInfo->m_nBigInt1 = 0;														// bSendFile false
			if (pCallInfo->m_sGroupCode==0)
				pProcessMsgInfo->m_nFromUserId = pCrMsgInfo.m_sSendFrom;	// bCallOnlyAccount
			m_pProcessMsgList.add(pProcessMsgInfo);
			//this->InviteCall(pCallInfo,0);
		}
	}
	return ret;
}
void CUserManagerApp::OnMsgReceipt(const CCrRichInfo& pRichInfo, int nAckType)
{
	const mycp::bigint sCallId = pRichInfo.GetCallId();
	//const mycp::bigint nMsgId = pRichInfo.m_pRichMsg->GetMsgId();
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		SendConnectMsg(pCallInfo, pRichInfo.m_sSendFrom,"",0);	// ?
		if (m_callback)
			m_callback->OnMsgReceipt(pRichInfo, nAckType);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrRichInfo * pEvent = new CCrRichInfo(pRichInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_MSG_RECEIPT);
			pEvent->SetEventParameter((long)nAckType);
            postWaitEventResult(m_pHwnd, pEvent);
#else
			::SendMessage(m_pHwnd, CR_WM_MSG_RECEIPT, (WPARAM)&pRichInfo, (LPARAM)nAckType);
#endif
		}
		if (pRichInfo.GetStateCode()==EB_STATE_NEED_DELETE_MSG)
		{
			UmMsgAck(pRichInfo.m_pRichMsg->GetMsgId(), EB_CAT_DELETE);
		}
		return;
	}

}

void CUserManagerApp::OnSendingFile(const CCrFileInfo& pFileInfo)
{
	const mycp::bigint sCallId = pFileInfo.GetCallId();
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		bool bFindFile = false;
		{
			CToSendList::pointer pOnlineFileToSendList;
			if (theOnlineFileList.find(sCallId,pOnlineFileToSendList))
			{
				// **找到前面在线发送文件，记录msgid
				BoostReadLock lock(pOnlineFileToSendList->m_list.mutex());
				CLockList<CToSendInfo::pointer>::iterator pIter;
				for (pIter=pOnlineFileToSendList->m_list.begin(); pIter!=pOnlineFileToSendList->m_list.end(); pIter++)
				{
					const CToSendInfo::pointer& pCallToSendInfo = *pIter;
					if (pCallToSendInfo->m_sFilePath==pFileInfo.m_sFileName)
					{
						pCallToSendInfo->m_nMsgId = pFileInfo.m_nMsgId;
						bFindFile = true;
						break;
					}
				}
			}
		}
		if (!bFindFile)
		{
			CToSendList::pointer pOnlineFileToSendList;
			if (theP2PFileList.find(sCallId,pOnlineFileToSendList))
			{
				// **找到前面P2P发送文件，记录msgid
				BoostReadLock lock(pOnlineFileToSendList->m_list.mutex());
				CLockList<CToSendInfo::pointer>::iterator pIter;
				for (pIter=pOnlineFileToSendList->m_list.begin(); pIter!=pOnlineFileToSendList->m_list.end(); pIter++)
				{
					const CToSendInfo::pointer& pCallToSendInfo = *pIter;
					if (pCallToSendInfo->m_sFilePath==pFileInfo.m_sFileName)
					{
						pCallToSendInfo->m_nMsgId = pFileInfo.m_nMsgId;
						bFindFile = true;
						break;
					}
				}
			}
		}

		if (pFileInfo.m_bOffFile && pFileInfo.GetStateCode()==EB_STATE_OK)
		{
			// 如果是接收离线文件，这里要记下来，接收或者拒绝的时候可以判断的到；
			pCallInfo->m_pOfflineFile.insert(pFileInfo.m_nMsgId,true);
		}
		if (EB_STATE_TIMEOUT_ERROR==pFileInfo.GetStateCode() && pCallInfo->m_sGroupCode==0)
		{
			LogMessage("OnSendingFile EB_STATE_TIMEOUT_ERROR... (callid=%lld)\r\n",sCallId);
			bool bResendFile = false;
			// 点对点超时，清空P2P资源后，尝试重新打通
			BoostReadLock rdLock(pCallInfo->m_pCallUserList.mutex());
			CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
			for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
			{
				const CEBAccountInfo::pointer pAccountInfo = pIter->second;
				//const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
				if (pAccountInfo->GetUserId() == m_pMyAccountInfo->GetUserId())
					continue;

				if (pCallInfo->m_tP2POkTime>0 && (pCallInfo->m_tP2POkTime+5)>=time(0))
				{
					bResendFile = true;
				}else
				{
					if ((pCallInfo->m_nCallData&EB_CALL_DATA_P2P_OK)!=0)
						//if (pAccountInfo->m_bLocalP2POk || pAccountInfo->m_bRemoteP2POk)
					{
						//pCallInfo->m_nCallData &= ~EB_CALL_DATA_P2P_OK;
						pAccountInfo->m_dwAccountData &= ~EB_DATA_SEND_FILE_CALL;
						bResendFile = true;
					}
					//pToAccountInfo = pAccountInfo;
					{
						pAccountInfo->m_bLocalP2POk = false;
						{
							BoostWriteLock wtLock(pAccountInfo->m_mutexLocalChatRoom);
							if (pAccountInfo->m_pLocalChatRoom.get()!=NULL)
							{
								//pAccountInfo->m_pLocalChatRoom->SetParam(2);	// * 无效P2P，预防 OnP2POk 返回
								m_pClearChatRoom.add(pAccountInfo->m_pLocalChatRoom);
								pAccountInfo->m_pLocalChatRoom.reset();
							}
						}
						pAccountInfo->m_bRemoteP2POk = false;
						{
							BoostWriteLock wtLock(pAccountInfo->m_mutexRemoteChatRoom);
							if (pAccountInfo->m_pRemoteChatRoom.get()!=NULL)
							{
								//pAccountInfo->m_pRemoteChatRoom->SetParam(2);	// * 无效P2P，预防 OnP2POk 返回
								m_pClearChatRoom.add(pAccountInfo->m_pRemoteChatRoom);
								pAccountInfo->m_pRemoteChatRoom.reset();
							}
						}
					}
				}
				rdLock.unlock();
				bool bP2Pequest = true;
				if (bResendFile)
				{
					bool bInviteCall = false;
                    this->SendCrFile(sCallId,pFileInfo.m_sFileName,pFileInfo.m_sSendTo,pFileInfo.m_bPrivate,pFileInfo.m_bOffFile,false,&bInviteCall);
					if (bInviteCall)
						bP2Pequest = false;
				}
				if (bP2Pequest)
				{
					// ** 改用线程发送；
					CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_P2P_REQUEST);
					pProcessMsgInfo->m_pCallInfo = pCallInfo;
					pProcessMsgInfo->m_pFromAccount = pAccountInfo;
					m_pProcessMsgList.add(pProcessMsgInfo);
					//LocalChatRoomP2PRequest(pCallInfo,pAccountInfo,1);	// 离线重试
				}
				break;
			}
			if (bResendFile)
			{
				return;
			}
		}
		
		if (pFileInfo.GetParam()!=0)
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(0);
		unsigned long nWaitingProcessMsgId = 0;
        m_pWaitingProcessFileList.find(pFileInfo.m_sFileName,nWaitingProcessMsgId,true);
		const_cast<CCrFileInfo&>(pFileInfo).SetParam(nWaitingProcessMsgId);
		if (m_callback)
			m_callback->OnSendingFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_SENDING_FILE);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_SENDING_FILE, (WPARAM)&pFileInfo, 0);	// nState
#endif
		}
		return;
	}
	const mycp::bigint sResourceId = pFileInfo.m_sResId;
	if (theResourceChatRoom.exist(sResourceId))
	{
		if (pFileInfo.GetStateCode()!=EB_STATE_OK && pFileInfo.GetStateCode()!=EB_STATE_FILE_ALREADY_EXIST)
		{
			CFilePathInfo::pointer pFilePathInfo;
			if (theResourceFilePath.find(sResourceId,pFilePathInfo,true))
			{
				//if (pFilePathInfo->m_bDownload)
				//{
                //	DeleteFileA(pFilePathInfo->m_sFilePath.c_str());
				//}else
				{
					// 取消上传
					this->DeleteRes(sResourceId);
				}
			}
		}else
		{
			CFilePathInfo::pointer pFilePathInfo;
			if (theResourceFilePath.find(sResourceId,pFilePathInfo))
				pFilePathInfo->m_nMsgId = pFileInfo.m_nMsgId;
		}
		CEBResourceInfo::pointer pResourceInfo;
		if (theResourceList.find(sResourceId,pResourceInfo) && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP && theCallInfoList.find(pResourceInfo->m_nFromId, pCallInfo))
		{
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(1);
			const_cast<CCrFileInfo&>(pFileInfo).SetCallId(pCallInfo->GetCallId());
			const_cast<CCrFileInfo&>(pFileInfo).SetChatId(pCallInfo->m_sChatId);
			//const_cast<CCrFileInfo&>(pFileInfo).m_sResId = 0;
		}
		unsigned long nWaitingProcessMsgId = 0;
        m_pWaitingProcessFileList.find(pFileInfo.m_sFileName,nWaitingProcessMsgId,true);
		const_cast<CCrFileInfo&>(pFileInfo).SetParam(nWaitingProcessMsgId);
		if (m_callback)
			m_callback->OnSendingFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_SENDING_FILE);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_SENDING_FILE, (WPARAM)&pFileInfo, 0);	// nState
#endif
		}
		return;
	}
}
void CUserManagerApp::OnSentFile(const CCrFileInfo& pFileInfo)
{
	const mycp::bigint sCallId = pFileInfo.GetCallId();
	bool bFineFile = DeleteOnlineFileList(sCallId, pFileInfo.m_nMsgId);
	if (!bFineFile)
		bFineFile = DeleteP2PFileList(sCallId, pFileInfo.m_nMsgId);

    m_pWaitingProcessFileList.remove(pFileInfo.m_sFileName);
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		SendConnectMsg(pCallInfo, pFileInfo.m_sReceiveAccount,"",0);	// ?
		pCallInfo->m_pOfflineFile.remove(pFileInfo.m_nMsgId);
		pCallInfo->m_tLastTime = time(0);
		if (pFileInfo.GetParam()!=0)
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(0);
		if (m_callback)
			m_callback->OnSentFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_SENT_FILE);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_SENT_FILE, (WPARAM)&pFileInfo, 0);	// nState
#endif
		}
		if (pFileInfo.GetStateCode()==EB_STATE_NEED_DELETE_MSG)
		{
			UmMsgAck(pFileInfo.m_nMsgId, EB_CAT_DELETE);
		}
		return;
	}

	const mycp::bigint sResourceId = pFileInfo.m_sResId;
	Cchatroom::pointer pChatRoom;
	if (theResourceChatRoom.find(sResourceId,pChatRoom,true))
	{
		//pChatRoom->ExitRoom(1);
		if (theResourceChatRoom.empty())
			theChatRoomList.clear();
		else
			theChatRoomList.remove(GetCmServerKey(pChatRoom->GetAddress()));
		m_pClearChatRoom.add(pChatRoom);

		CEBResourceInfo::pointer pResourceInfo;
		if (theResourceList.find(sResourceId,pResourceInfo) && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP && theCallInfoList.find(pResourceInfo->m_nFromId, pCallInfo))
		{
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(1);
			const_cast<CCrFileInfo&>(pFileInfo).SetCallId(pCallInfo->GetCallId());
			const_cast<CCrFileInfo&>(pFileInfo).SetChatId(pCallInfo->m_sChatId);
			const_cast<CCrFileInfo&>(pFileInfo).m_sResId = 0;
		}
		if (m_callback)
			m_callback->OnSentFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_SENT_FILE);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_SENT_FILE, (WPARAM)&pFileInfo, 0);	// nState
#endif
		}

		CFilePathInfo::pointer pFilePathInfo;
		if (theResourceFilePath.find(sResourceId,pFilePathInfo,true))
		{
			if (pFilePathInfo->m_nType == CFilePathInfo::FILE_PAHT_HEAD)
			{
				CEBMemberInfo::pointer pEmployeeInfo = GetEmployeeInfo2(sResourceId);
				if (pEmployeeInfo.get()!=NULL)
				{
#ifdef _QT_MAKE_
                    const QString lpszHeadResourceFile = QString("%1/%2").arg(m_sEbResourcePath).arg(sResourceId);
                    pEmployeeInfo->m_sHeadResourceFile = lpszHeadResourceFile;
                    if (lpszHeadResourceFile!=pFilePathInfo->m_sFilePath) {
                        QFile::remove(lpszHeadResourceFile);
                        QFile::copy(pFilePathInfo->m_sFilePath,lpszHeadResourceFile);
                    }
#else
                    char lpszHeadResourceFile[260];
                    sprintf(lpszHeadResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),sResourceId);
                    //const tstring sHeadResourceFile = m_sEbResourcePath + "\\" + sResourceId;
                    pEmployeeInfo->m_sHeadResourceFile = lpszHeadResourceFile;
                    CopyFileA(pFilePathInfo->m_sFilePath.c_str(),lpszHeadResourceFile,FALSE);
#endif
                    mycp::bigint nFileSize = 0;
                    GetFileMd5(lpszHeadResourceFile,nFileSize,pEmployeeInfo->m_sHeadMd5);

					if (this->m_callback)
						m_callback->onUserHeadChange(pEmployeeInfo.get(),true);

					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
						EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
						pEvent->SetQEventType((QEvent::Type)EB_WM_USER_HEAD_CHANGE);
						pEvent->SetEventParameter((long)1);
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::SendMessage(m_pHwnd, EB_WM_USER_HEAD_CHANGE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(),1);
#endif
					}
				}
			}else if (pFilePathInfo->m_nType == CFilePathInfo::FILE_PAHT_RESOURCE)
			{
				CEBResourceInfo::pointer pResourceInfo;
				if (theResourceList.find(sResourceId,pResourceInfo))
				{
					// 新建资源上传文件成功
					pResourceInfo->m_nSize = pFileInfo.m_nFileSize;
					// 请求加载多一次，可以加载“在线浏览”相关信息
					CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_LOAD_RESOURCE_INFO);
					pProcessMsgInfo->m_tProcessTime = time(0)+3;
					pProcessMsgInfo->m_nBigInt1 = sResourceId;
					m_pProcessMsgList.add(pProcessMsgInfo);
					//this->LoadRes(sResourceId,0,0,EB_RESOURCE_FROM_TYPE_UNKNOWN);
					if (this->m_callback)
						m_callback->onResourceInfo(pResourceInfo.get());
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
						EB_ResourceInfo * pEvent = new EB_ResourceInfo(pResourceInfo.get());
						pEvent->SetQEventType((QEvent::Type)EB_WM_RESOURCE_INFO);
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::SendMessage(m_pHwnd, EB_WM_RESOURCE_INFO, (WPARAM)(const EB_ResourceInfo*)pResourceInfo.get(), 0);
#endif
					}
					// *
					if (pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP)
					{
						// * 改用cm_msg
						const mycp::bigint nGroupId = pResourceInfo->m_nFromId;
						//const mycp::bigint sCallId = nGroupId;
						//SendCrFile(sCallId,pFileInfo.m_sFileName.c_str(),pResourceInfo->m_sResId);
						CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_SEND_GROUP_FILE);
						pProcessMsgInfo->m_tProcessTime = time(0);
						pProcessMsgInfo->m_nCallGroupId = nGroupId;
						pProcessMsgInfo->m_nBigInt1 = pResourceInfo->m_sResId;
						pProcessMsgInfo->m_sString1 = pFileInfo.m_sFileName;
						m_pProcessMsgList.add(pProcessMsgInfo);

						//const mycp::bigint nGroupId = pResourceInfo->m_nFromId;
						//const mycp::bigint sCallId = nGroupId;
						//const std::string::size_type find = pFileInfo.m_sFileName.rfind('\\');
						//const std::string sFileName = find==std::string::npos?pFileInfo.m_sFileName:pFileInfo.m_sFileName.substr(find+1);
						//char lpszNotifyData[400];
						//sprintf(lpszNotifyData,"%lld*%d*%d*%lld*%s",pResourceInfo->m_sResId,(int)pResourceInfo->m_nResType,(int)pResourceInfo->m_nShare,pFileInfo.m_nFileSize,sFileName.c_str());
						//CCrNotifyInfo pNotifyInfo(0,sCallId,sCallId,EB_STATE_OK);
						//pNotifyInfo.m_nFromAccount = m_pMyAccountInfo->GetUserId();
						//pNotifyInfo.m_nNotifyType = 2;
						//pNotifyInfo.m_nNotityId = 0;
						//pNotifyInfo.m_sNotifyData = lpszNotifyData;
						//OnUserNotify(pNotifyInfo);
						//CallNotify(sCallId,0,2,0,lpszNotifyData);
					}

				}
			}
		}
	}
}
void CUserManagerApp::OnCancelFile(const CCrFileInfo& pFileInfo)
{
	const mycp::bigint sResourceId = pFileInfo.m_sResId;
	mycp::bigint sCallId = pFileInfo.GetCallId();
	bool bCallIdResourceFile = false;
	if (sCallId == 0 && sResourceId>0)
	{
		bCallIdResourceFile = theResource2CallId.find(sResourceId,sCallId);
	}
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		pCallInfo->m_pOfflineFile.remove(pFileInfo.m_nMsgId);
		//m_pWaitList1.remove(pFileInfo.m_nMsgId);
		int nWaitType = 0;
		if (m_pWaitList1.find(pFileInfo.m_nMsgId,nWaitType,true) && pCallInfo->m_sGroupCode>0 && nWaitType==1)
		{
			// 主动响应取消群共享，后面不处理
			return;
		}
		SendConnectMsg(pCallInfo, pFileInfo.m_sReceiveAccount,"",0);	// ?
		pCallInfo->m_tLastTime = time(0);
		const bool bChangeP2PMsgId = theChangeP2PMsgList.remove(pFileInfo.m_nMsgId);
		if (pFileInfo.GetParam()!=0)
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(0);
		if (m_callback)
			m_callback->OnCancelFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_CANCEL_FILE);
			pEvent->SetEventParameter((long)(bChangeP2PMsgId?1:0));
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_CANCEL_FILE, (WPARAM)&pFileInfo, (LPARAM)(bChangeP2PMsgId?1:0));
#endif
		}

		if (pFileInfo.GetStateCode()==EB_STATE_NEED_DELETE_MSG)
		{
			UmMsgAck(pFileInfo.m_nMsgId, EB_CAT_DELETE);
		}

		if (bCallIdResourceFile&&sResourceId>0)
		{
			// 这里是接收离线文件
			Cchatroom::pointer pChatRoom;
			if (theResourceChatRoom.find(sResourceId,pChatRoom,true))
			{
				if (theResourceChatRoom.empty())
					theChatRoomList.clear();
				else
					theChatRoomList.remove(GetCmServerKey(pChatRoom->GetAddress()));
				m_pClearChatRoom.add(pChatRoom);
			}
			theResource2CallId.remove(sResourceId);
			theResource2MsgId.remove(sResourceId);
		}

		//pFileInfo.m_nMsgId
#ifdef USES_RESEND_FILE_2
		CToSendInfo::pointer pCallToSendInfo;
		if (theREsendIdList.find(pFileInfo.m_nMsgId,pCallToSendInfo,true))
		{
            SendCrFile(sCallId,pCallToSendInfo->m_sFilePath,pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile);
		}
#endif
		return;
	}

	Cchatroom::pointer pChatRoom;
	if (theResourceChatRoom.find(sResourceId,pChatRoom,true)) {
		//pChatRoom->ExitRoom(1);
		if (theResourceChatRoom.empty())
			theChatRoomList.clear();
		else
			theChatRoomList.remove(GetCmServerKey(pChatRoom->GetAddress()));
		m_pClearChatRoom.add(pChatRoom);
		CFilePathInfo::pointer pFilePathInfo;
		if (theResourceFilePath.find(sResourceId,pFilePathInfo,true)) {
			if (pFilePathInfo->m_bDownload) {
                /// ** 因为 libebcm 保存下载文件，会保存到 .ebtemp临时文件，所以只删除 .ebtemp 临时文件
                /// ** 原正式文件不删除，避免删除替换下载文件问题；
#ifdef _QT_MAKE_
                if (pFilePathInfo->m_sFilePath.indexOf(".ebtemp")>0) {
                    QFile::remove(pFilePathInfo->m_sFilePath);
                }
#else
                if (pFilePathInfo->m_sFilePath.find(".ebtemp")!=std::string::npos) {
                    DeleteFileA(pFilePathInfo->m_sFilePath.c_str());
                }
#endif
			}
			else {
				// 取消上传
				this->DeleteRes(sResourceId);
			}
		}
		CEBResourceInfo::pointer pResourceInfo;
		if (theResourceList.find(sResourceId,pResourceInfo) && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP && theCallInfoList.find(pResourceInfo->m_nFromId, pCallInfo))
		{
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(1);
			const_cast<CCrFileInfo&>(pFileInfo).SetCallId(pCallInfo->GetCallId());
			const_cast<CCrFileInfo&>(pFileInfo).SetChatId(pCallInfo->m_sChatId);
			//const_cast<CCrFileInfo&>(pFileInfo).m_sResId = 0;
		}
		if (m_callback)
			m_callback->OnCancelFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_CANCEL_FILE);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_CANCEL_FILE, (WPARAM)&pFileInfo, 0);
#endif
		}
	}
}
int CUserManagerApp::OnReceivingFile(const CCrFileInfo& pFileInfo)
{
	const mycp::bigint sResourceId = pFileInfo.m_sResId;
	mycp::bigint sCallId = pFileInfo.GetCallId();
	bool bCallIdResourceFile = false;
	if (sCallId == 0 && sResourceId>0)
	{
		if (theResource2CallId.exist(sResourceId))
			return 0;
	}
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		if (pFileInfo.m_bOffFile)
		{
			// 如果是接收离线文件，这里要记下来，接收或者拒绝的时候可以判断的到；
			pCallInfo->m_pOfflineFile.insert(pFileInfo.m_nMsgId,true);
		}

		// 这里要记录离线文件
		const mycp::bigint nMsgId(pFileInfo.m_nMsgId);
		if (sResourceId>0)
		{
			//if (!pCallInfo->m_pResCmInfo.empty())
			//{
			//	// 这里面是接收离线文件，需要清除旧消息
			//	boost::mutex::scoped_lock lock(pCallInfo->m_pResCmInfo.mutex());
			//	CLockMap<mycp::bigint,tstring>::iterator pIter = pCallInfo->m_pResCmInfo.begin();
			//	for (;pIter!=pCallInfo->m_pResCmInfo.end(); pIter++)
			//	{
			//		const mycp::bigint nMsgId = pIter->first;
			//		const tstring sResCmInfo = pIter->second;
			//		const mycp::bigint nResCmInfoResId = cgc_atoi64(sResCmInfo.c_str());
			//		if (nResCmInfoResId==sResourceId)
			//		//if (sResCmInfo.find(sResourceId) == 0)
			//		{
			//			pCallInfo->m_pChatRoom->CancelSendingFile(nMsgId);
			//		}
			//	}
			//}
			if (!pFileInfo.m_sResCmServer.empty() && 
				(pCallInfo->m_pChatRoom.get()==NULL || pCallInfo->m_pChatRoom->GetAddress() != pFileInfo.m_sResCmServer))
			{
				char sResCmInfo[120];
				sprintf(sResCmInfo,"%lld,%s,%s",pFileInfo.m_sResId,pFileInfo.m_sResCmServer.c_str(),pFileInfo.m_sResCmAppName.c_str());
				pCallInfo->m_pResCmInfo.insert(nMsgId, sResCmInfo);
			}
			if (pCallInfo->m_sGroupCode==0)	// 群共享资源，保留resid
				const_cast<CCrFileInfo&>(pFileInfo).m_sResId = 0;
		}
		//if (pCallInfo->m_sGroupCode>0 && !pCallInfo->m_pCallUserList.exist(pFileInfo.m_sSendFrom))
		//{
		//	EnterpriseLoad(pCallInfo->m_sGroupCode,0,0,0,0,pFileInfo.m_sSendFrom);	// 预防万一
		//}

		SendConnectMsg(pCallInfo, pFileInfo.m_sSendFrom,"",EB_CONNECTED_MSG);	// ?
		pCallInfo->m_tLastTime = time(0);
		if (pFileInfo.GetParam()!=0)
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(0);
		int ret = -1;
		if (m_callback)
			ret = m_callback->OnReceivingFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_RECEIVING_FILE);
            ret = (int)postWaitEventResult(m_pHwnd, pEvent);
#else
			ret = (int)::SendMessage(m_pHwnd, CR_WM_RECEIVING_FILE, (WPARAM)&pFileInfo, 0);
#endif
		}
		if (ret==-1)
		{
			LogMessage("CUserManagerApp::OnReceivingFile SendMessage() -1 error, callid=%lld, msgid=%lld,groupid=%lld(InviteCall...)\r\n",sCallId,pFileInfo.m_nMsgId,pCallInfo->m_sGroupCode);
			CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_INVITE_CALL);
			//pProcessMsgInfo->m_tProcessTime = time(0)+3;
			pProcessMsgInfo->m_pCallInfo = pCallInfo;
			pProcessMsgInfo->m_nBigInt1 = 0;														// bSendFile false
			if (pCallInfo->m_sGroupCode==0)
				pProcessMsgInfo->m_nFromUserId = pFileInfo.m_sSendFrom;		// bCallOnlyAccount
			m_pProcessMsgList.add(pProcessMsgInfo);
			//this->InviteCall(pCallInfo,0);
		}else if (pCallInfo->m_sGroupCode>0)
		{
			m_pWaitList1.insert(pFileInfo.m_nMsgId,1,false);	// 1=
			CancelCrFile(sCallId,pFileInfo.m_nMsgId);
		}
		return ret;
	}

	if (m_pVersionInfo.m_sResId == sResourceId)
	{
		// 版本更新，不需要下载
		if (!pFileInfo.m_sMD5.empty() && pFileInfo.m_sMD5!=m_pVersionInfo.m_sMD5)
			m_pVersionInfo.m_sMD5 = pFileInfo.m_sMD5;
		EB_VersionInfo * pVersionInfo = new EB_VersionInfo();
		pVersionInfo->m_nUpdateType = -2;
		if (m_callback)
			m_callback->onNewVersion(*pVersionInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			pVersionInfo->SetQEventType((QEvent::Type)EB_WM_NEW_VERSION);
            QCoreApplication::postEvent(m_pHwnd, pVersionInfo, thePostEventPriority);
#else
			::SendMessage(m_pHwnd, EB_WM_NEW_VERSION, (WPARAM)pVersionInfo, 0);
			delete pVersionInfo;
#endif
		}
		else {
			delete pVersionInfo;
		}
		return 0;
	}
	if (theResourceChatRoom.exist(sResourceId))
	{
		CFilePathInfo::pointer pFilePathInfo;
		if (theResourceFilePath.find(sResourceId,pFilePathInfo))
		{
			if (pFilePathInfo->m_nType == CFilePathInfo::FILE_ENT_IMAGE)
			{
				// 下载企业表情，不需要显示
				return 0;
			}else if (pFilePathInfo->m_nType == CFilePathInfo::FILE_PAHT_HEAD)
			{
				// 下载成员头像，不需要显示
				return 0;
			}else if (pFilePathInfo->m_nType == CFilePathInfo::FILE_FUNC_IMAGE)
			{
				// 下载功能图标，不需要显示
				return 0;
			}
		}

		CEBResourceInfo::pointer pResourceInfo;
		if (theResourceList.find(sResourceId,pResourceInfo) && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP && theCallInfoList.find(pResourceInfo->m_nFromId, pCallInfo))
		{
			SendConnectMsg(pCallInfo, pFileInfo.m_sSendFrom,"",EB_CONNECTED_MSG);	// ?
			pCallInfo->m_tLastTime = time(0);
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(1);
			const_cast<CCrFileInfo&>(pFileInfo).SetCallId(pCallInfo->GetCallId());
			const_cast<CCrFileInfo&>(pFileInfo).SetChatId(pCallInfo->m_sChatId);
			//const_cast<CCrFileInfo&>(pFileInfo).m_sResId = 0;
		}
		int ret = -1;
		if (m_callback)
			ret = m_callback->OnReceivingFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_RECEIVING_FILE);
            ret = (int)postWaitEventResult(m_pHwnd, pEvent);
#else
			ret = (int)::SendMessage(m_pHwnd, CR_WM_RECEIVING_FILE, (WPARAM)&pFileInfo, 0);
#endif
		}
		return ret;
	}
	return 0;
}
void CUserManagerApp::OnReceivedFile(const CCrFileInfo& pFileInfo)
{
	const mycp::bigint sResourceId = pFileInfo.m_sResId;
	mycp::bigint sCallId = pFileInfo.GetCallId();
	bool bCallIdResourceFile = false;
	if (sCallId == 0 && sResourceId>0)
	{
		bCallIdResourceFile = theResource2CallId.find(sResourceId,sCallId,true);
	}
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		pCallInfo->m_pOfflineFile.remove(pFileInfo.m_nMsgId);

		pCallInfo->m_tLastTime = time(0);
		if (bCallIdResourceFile && sResourceId>0)
		{
			// 这里是接收离线文件
			Cchatroom::pointer pChatRoom;
			if (theResourceChatRoom.find(sResourceId,pChatRoom,true))
			{
				if (theResourceChatRoom.empty())
					theChatRoomList.clear();
				else
					theChatRoomList.remove(GetCmServerKey(pChatRoom->GetAddress()));
				m_pClearChatRoom.add(pChatRoom);
			}
			// 重设msgid
			mycp::bigint nTempMsgId = 0;
			theResource2MsgId.find(sResourceId,nTempMsgId,true);
			const_cast<CCrFileInfo&>(pFileInfo).SetCallId(sCallId);
			const_cast<CCrFileInfo&>(pFileInfo).SetChatId(pCallInfo->m_sChatId);
			const_cast<CCrFileInfo&>(pFileInfo).m_nMsgId = nTempMsgId;
		}
		SendConnectMsg(pCallInfo, pFileInfo.m_sSendFrom,"",EB_CONNECTED_MSG);	// ?
		if (pFileInfo.GetParam()!=0)
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(0);
		if (m_callback)
			m_callback->OnReceivedFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_RECEIVED_FILE);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_RECEIVED_FILE, (WPARAM)&pFileInfo, 0);
#endif
		}
		return;
	}
	if (m_pVersionInfo.m_sResId == sResourceId)
	{
		// 已经下载最新版本打包文件
		// 判断下载文件MD5是否正确
		mycp::bigint nFileSize = 0;
		tstring sFileMd5String;
		if (m_pVersionInfo.m_sVersionFile!=pFileInfo.m_sFileName)
		{
			//　临时文件有问题，删除临时文件
            const EBFileString sTempFile = m_pVersionInfo.m_sVersionFile+".ebtemp";
#ifdef _QT_MAKE_
            QFile::remove(sTempFile);
#else
            DeleteFileA(sTempFile.c_str());
#endif
			// 更新升级文件
			m_pVersionInfo.m_sVersionFile = pFileInfo.m_sFileName;
		}
        GetFileMd5(m_pVersionInfo.m_sVersionFile,nFileSize,sFileMd5String);
		if (sFileMd5String!=m_pVersionInfo.m_sMD5)
		{
            /// 删除后重新下载；
#ifdef _QT_MAKE_
            QFile::remove(m_pVersionInfo.m_sVersionFile);
#else
            DeleteFileA(m_pVersionInfo.m_sVersionFile.c_str());
#endif
            const EBFileString sTempFile = m_pVersionInfo.m_sVersionFile+".ebtemp";
#ifdef _QT_MAKE_
            QFile::remove(sTempFile);
#else
            DeleteFileA(sTempFile.c_str());
#endif
		}else
		{
			if (m_callback)
				m_callback->onNewVersion(m_pVersionInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_VersionInfo * pEvent = new EB_VersionInfo(m_pVersionInfo);
				pEvent->SetQEventType((QEvent::Type)EB_WM_NEW_VERSION);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_NEW_VERSION, (WPARAM)&m_pVersionInfo, 0);
#endif
			}
		}
	}

	Cchatroom::pointer pChatRoom;
	if (theResourceChatRoom.find(sResourceId,pChatRoom,true))
	{
		if (theResourceChatRoom.empty())
		{
			theChatRoomList.clear();
			m_pClearChatRoom.add(pChatRoom);
		}else
		{
			const tstring sCmServerKey = GetCmServerKey(pChatRoom->GetAddress());
			char lpszKey[200];
			sprintf(lpszKey,"%s%lld",sCmServerKey.c_str(),sResourceId);
			//sprintf(lpszKey,"%s%lld",pChatRoom->GetAddress().c_str(),sResourceId);
			if (!theChatRoomList.remove(lpszKey))
			{
				theChatRoomList.remove(sCmServerKey);
				//theChatRoomList.remove(pChatRoom->GetAddress());
				m_pClearChatRoom.add(pChatRoom);
			}
		}

		CFilePathInfo::pointer pFilePathInfo;
		if (theResourceFilePath.find(sResourceId,pFilePathInfo,true)) {
            if (pFilePathInfo->m_sFilePath!=pFileInfo.m_sFileName) {
#ifdef _QT_MAKE_
                QFile::remove(pFilePathInfo->m_sFilePath);
                QFile::rename(pFileInfo.m_sFileName,pFilePathInfo->m_sFilePath);
#else
                CopyFileA(pFileInfo.m_sFileName.c_str(),pFilePathInfo->m_sFilePath.c_str(),FALSE);
                DeleteFileA(pFileInfo.m_sFileName.c_str());
#endif
			}

			if (pFilePathInfo->m_nType == CFilePathInfo::FILE_ENT_IMAGE)
			{
				// 自动下载企业表情/头像，不需要显示
				const mycp::bigint sEntCode = pFilePathInfo->m_nBigData;
				CEBEmotionInfo::pointer pEmotionInfo;
				char sEnterpriseImageKey[48];
				sprintf(sEnterpriseImageKey,"%lld%lld",sResourceId,sEntCode);
				if (theEmotionList.find(sEnterpriseImageKey,pEmotionInfo))
				{
					pEmotionInfo->m_sResFile = pFilePathInfo->m_sFilePath;
					SaveLocalEmotionInfo(pEmotionInfo);
				}
				return;
			}else if (pFilePathInfo->m_nType == CFilePathInfo::FILE_PAHT_HEAD)
			{
				// 自动下载成员头像，不需要显示
				if (pFilePathInfo->m_pContactInfo.get()!=NULL)
				{
					pFilePathInfo->m_pContactInfo->m_sHeadResourceFile = pFilePathInfo->m_sFilePath;
					const bool bUserHeadChange = pFilePathInfo->m_nBigData==1?true:false;
					if (bUserHeadChange)
					{
						if (this->m_callback)
							m_callback->onContactHeadChange(pFilePathInfo->m_pContactInfo.get());
						if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
							EB_ContactInfo * pEvent = new EB_ContactInfo(pFilePathInfo->m_pContactInfo.get());
							pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_HEAD_CHANGE);
                            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
							::SendMessage(m_pHwnd, EB_WM_CONTACT_HEAD_CHANGE, (WPARAM)(const EB_ContactInfo*)pFilePathInfo->m_pContactInfo.get(), 0);
#endif
						}
					}
				}else if (pFilePathInfo->m_pAccountInfo.get()!=NULL)
				{
					pFilePathInfo->m_pAccountInfo->m_pFromCardInfo.m_sHeadResourceFile = pFilePathInfo->m_sFilePath;
				}else if (pFilePathInfo->m_pEmployeeInfo.get()!=NULL)
				{
					pFilePathInfo->m_pEmployeeInfo->m_sHeadResourceFile = pFilePathInfo->m_sFilePath;
					const bool bUserHeadChange = pFilePathInfo->m_nBigData==1?true:false;
                    if (bUserHeadChange) {
                        mycp::bigint nFileSize = 0;
                        GetFileMd5(pFilePathInfo->m_sFilePath,
                                   nFileSize,
                                   pFilePathInfo->m_pEmployeeInfo->m_sHeadMd5);

						const bool bIsOwnerMember = pFilePathInfo->m_pEmployeeInfo->m_nMemberUserId==m_pMyAccountInfo->GetUserId()?true:false;
						if (this->m_callback)
							m_callback->onUserHeadChange(pFilePathInfo->m_pEmployeeInfo.get(),bIsOwnerMember);
						if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
							EB_MemberInfo * pEvent = new EB_MemberInfo(pFilePathInfo->m_pEmployeeInfo.get());
							pEvent->SetQEventType((QEvent::Type)EB_WM_USER_HEAD_CHANGE);
							pEvent->SetEventParameter((long)(bIsOwnerMember?1:0));
                            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
							::SendMessage(m_pHwnd, EB_WM_USER_HEAD_CHANGE, (WPARAM)(const EB_MemberInfo*)pFilePathInfo->m_pEmployeeInfo.get(),(LPARAM)(bIsOwnerMember?1:0));
#endif
						}
					}
					//const mycp::bigint sMemberCode = pFilePathInfo->m_nBigData;
					//CEBMemberInfo::pointer pEmployeeInfo;
					//if (theEmployeeList.find(sMemberCode, pEmployeeInfo))
					//{
					//	if (pFilePathInfo->m_sFilePath != pFileInfo.m_sFileName)
					//	{
                    //		CopyFileA(pFileInfo.m_sFileName.c_str(),pFilePathInfo->m_sFilePath.c_str(),FALSE);
                    //		DeleteFileA(pFileInfo.m_sFileName.c_str());
					//	}
					//	pEmployeeInfo->m_sHeadResourceFile = pFilePathInfo->m_sFilePath;
					//}
				}
				return;
			}else if (pFilePathInfo->m_nType == CFilePathInfo::FILE_FUNC_IMAGE)
			{
				// 自动功能图标，不需要显示
				const mycp::bigint nSubscribeId = pFilePathInfo->m_nBigData;
				CEBSubscribeFuncInfo::pointer pSubscrieFuncInfo;
				if (theSubscribeFuncInfolist.find(nSubscribeId,pSubscrieFuncInfo))
				{
					pSubscrieFuncInfo->m_sResFile = pFilePathInfo->m_sFilePath;
				}
				return;
			}
		}
		if (m_pVersionInfo.m_sResId == sResourceId)
		{
			// 下载版本文件，不需要显示
			return;
		}
		CEBResourceInfo::pointer pResourceInfo;
		if (theResourceList.find(sResourceId,pResourceInfo) && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP && theCallInfoList.find(pResourceInfo->m_nFromId, pCallInfo))
		{
			pCallInfo->m_tLastTime = time(0);
			const_cast<CCrFileInfo&>(pFileInfo).SetParam(1);
			const_cast<CCrFileInfo&>(pFileInfo).SetCallId(pCallInfo->GetCallId());
			const_cast<CCrFileInfo&>(pFileInfo).SetChatId(pCallInfo->m_sChatId);
			//const_cast<CCrFileInfo&>(pFileInfo).m_sResId = 0;
		}

		if (m_callback)
			m_callback->OnReceivedFile(pFileInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
			pEvent->SetQEventType((QEvent::Type)CR_WM_RECEIVED_FILE);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_RECEIVED_FILE, (WPARAM)&pFileInfo, 0);
#endif
		}
		return;
	}
}
void CUserManagerApp::OnSave2CloudDrive(const CCrFileInfo& pFileInfo)
{
	const mycp::bigint sCallId = pFileInfo.GetCallId();
	//MessageBox(NULL,_T("OnReceiveRich"),_T(""),MB_OK);
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return;
	}
	pCallInfo->m_tLastTime = time(0);
	if (m_callback)
		m_callback->OnSave2CloudDrive(pFileInfo);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		CCrFileInfo * pEvent = new CCrFileInfo(pFileInfo);
		pEvent->SetQEventType((QEvent::Type)CR_WM_SAVE2CLOUD_DRIVE);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::SendMessage(m_pHwnd, CR_WM_SAVE2CLOUD_DRIVE, (WPARAM)&pFileInfo,(LPARAM)0);	// nState
#endif
	}
}

void CUserManagerApp::OnFilePercent(const CChatRoomFilePercent& pFilePercent)
{
	const mycp::bigint sResourceId = pFilePercent.m_sResId;
	mycp::bigint sCallId = pFilePercent.GetCallId();
	bool bCallIdResourceFile = false;
	if (sCallId == 0 && sResourceId>0)
		bCallIdResourceFile = theResource2CallId.find(sResourceId,sCallId);
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		if (bCallIdResourceFile && sResourceId>0)
		{
			// 重设msgid
			mycp::bigint nTempMsgId = 0;
			theResource2MsgId.find(sResourceId,nTempMsgId);
			const_cast<CChatRoomFilePercent&>(pFilePercent).SetCallId(sCallId);
			const_cast<CChatRoomFilePercent&>(pFilePercent).SetChatId(pCallInfo->m_sChatId);
			const_cast<CChatRoomFilePercent&>(pFilePercent).m_nMsgId = nTempMsgId;
		}
		if (pFilePercent.GetParam()!=0)
			const_cast<CChatRoomFilePercent&>(pFilePercent).SetParam(0);
		if (m_callback)
			m_callback->OnFilePercent(pFilePercent);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            CChatRoomFilePercent * pEvent = new CChatRoomFilePercent(pFilePercent);
			pEvent->SetQEventType((QEvent::Type)CR_WM_FILE_PERCENT);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_FILE_PERCENT, (WPARAM)&pFilePercent, 0);
#endif
		}
		return;
	}
	if (theResourceChatRoom.exist(sResourceId))
	{
		CEBResourceInfo::pointer pResourceInfo;
		if (theResourceList.find(sResourceId,pResourceInfo) && pResourceInfo->m_nFromType==EB_RESOURCE_FROM_TYPE_GROUP && theCallInfoList.find(pResourceInfo->m_nFromId, pCallInfo))
		{
			const_cast<CChatRoomFilePercent&>(pFilePercent).SetParam(1);
			const_cast<CChatRoomFilePercent&>(pFilePercent).SetCallId(pCallInfo->GetCallId());
			const_cast<CChatRoomFilePercent&>(pFilePercent).SetChatId(pCallInfo->m_sChatId);
			//const_cast<CChatRoomFilePercent&>(pFilePercent).m_sResId = 0;
		}

		if (m_callback)
			m_callback->OnFilePercent(pFilePercent);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			CChatRoomFilePercent * pEvent = new CChatRoomFilePercent(pFilePercent);
			pEvent->SetQEventType((QEvent::Type)CR_WM_FILE_PERCENT);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, CR_WM_FILE_PERCENT, (WPARAM)&pFilePercent, 0);
#endif
		}
		return;
	}
}
void CUserManagerApp::OnInvalidateSession(const CCrInfo& pParameter)
{
	// 某个会话SESSION无效，需要重新进入会话
	const unsigned long nCmParameter = pParameter.GetParam();
	if (nCmParameter==2) return;	// 无效CM，直接返回
	const int nResultCode = pParameter.GetStateCode();
	EB_SERVER_STATE nCmServerState = EB_SS_INIT;
	CEBCallbackInterface::SERVER_STATE nServerState = CEBCallbackInterface::SERVER_UNKNOWN;
	LogMessage("OnInvalidateSession CM ... (nResultCode=%d,nServerState=%d)\r\n",(int)nResultCode,(int)nServerState);
	if (nResultCode == -102)
	{
		nCmServerState = EB_SS_MOVED;
		nServerState = CEBCallbackInterface::SERVER_MOVED;
	}else if (nResultCode == -103)
	{
		nCmServerState = EB_SS_RESTART;
		nServerState = CEBCallbackInterface::SERVER_RESTART;
	}else if (nResultCode == -117)
	{
		nCmServerState = EB_SS_STOPED;
		nServerState = CEBCallbackInterface::SERVER_STOPED;
	}else if (nResultCode == -200)
	{
		nCmServerState = EB_SS_TIMEOUT;
		nServerState = CEBCallbackInterface::SERVER_TIMEOUT;
	}else
	{
		return;
	}

	const mycp::bigint sCallId = pParameter.GetCallId();
	LogMessage("OnInvalidateSession CM ... (nResultCode=%d,nServerState=%d) callid=%lld,CmParameter=%d, CmServerState=%d\r\n",(int)nResultCode,(int)nServerState,sCallId,(int)nCmParameter,(int)nCmServerState);
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		// 清空P2P状态；
		if (pCallInfo->m_sGroupCode==0)
		{
			// 查找P2P发送
			BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
			CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
			for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
			{
				const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
				if (pAccountInfo->GetUserId() != m_pMyAccountInfo->GetUserId())
				{
					if (nCmParameter==0 ||					// 正常CM退出需要清空（非P2P）
					//if (nCmParameter!=1 ||					// 正常CM退出需要清空（非P2P）
						!pAccountInfo->m_bLocalP2POk)		// P2P打通失败也需要清空
					{
						pAccountInfo->m_bLocalP2POk = false;
						BoostWriteLock wtLock(pAccountInfo->m_mutexLocalChatRoom);
						if (pAccountInfo->m_pLocalChatRoom.get()!=NULL)
						{
							m_pClearChatRoom.add(pAccountInfo->m_pLocalChatRoom);
							pAccountInfo->m_pLocalChatRoom.reset();
						}
					}
					if (nCmParameter==0 ||					// 正常CM退出需要清空（非P2P）
					//if (nCmParameter!=1 ||					// 正常CM退出需要清空（非P2P）
						!pAccountInfo->m_bRemoteP2POk)		// P2P打通失败也需要清空
					{
						pAccountInfo->m_bRemoteP2POk = false;
						BoostWriteLock wtLock(pAccountInfo->m_mutexRemoteChatRoom);
						if (pAccountInfo->m_pRemoteChatRoom.get()!=NULL)
						{
							m_pClearChatRoom.add(pAccountInfo->m_pRemoteChatRoom);
							pAccountInfo->m_pRemoteChatRoom.reset();
						}
					}
				}
			}
			if (nCmParameter>=1)	// 0=正常；1=P2P；2=无效P2P
			{
				// 这是P2P，不需要处理；
				return;
			}
		}

		// 判断CM和UM是不是同一台服务器
		bool bSameUMServer = false;
		tstring sCMServer;
		int nPort = 0;
		entboost::GetAddressPort(pCallInfo->m_sCmServer.c_str(),sCMServer,nPort);
		if (m_pUserManager.get() != NULL)
		{
			tstring sUMServer;
			entboost::GetAddressPort(m_pUserManager->GetAddress().address().c_str(),sUMServer,nPort);
			bSameUMServer = entboost::GetHostIp(sUMServer.c_str(),sUMServer.c_str()) == entboost::GetHostIp(sCMServer.c_str(),sCMServer.c_str());
		}
		if (nServerState==CEBCallbackInterface::SERVER_RESTART)
		{
			if (pCallInfo->m_nCallState == EB_CALL_STATE_EXIT)
			{
				pCallInfo->m_nCallState = EB_CALL_STATE_HANGUP;
				ClearCallInfo(sCallId,true,false);
			}else
			{
				if (pCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED)
					pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
				if (!bSameUMServer)
				{
					// UM和CM不是同一台，需要处理重新直接会话；
					//if (pCallInfo->m_nCmServerState != EB_SS_STOPED && pCallInfo->m_pChatRoom.get()!=NULL)
					//{
					//	// 前面收不到stop事件，清空CM信息
					//	//pCallInfo->m_pChatRoom->ExitRoom(0);
					//	pCallInfo->m_pChatRoom.reset();
					//}
					if (pCallInfo->m_pChatRoom.get()!=NULL)
					{
						pCallInfo->m_pChatRoom->Stop();
						m_pClearChatRoom.add(pCallInfo->m_pChatRoom);
						pCallInfo->m_pChatRoom.reset();
					}

					pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
					this->InviteCall(pCallInfo,0);
				}else
				{
					if (pCallInfo->m_pChatRoom.get()!=NULL)
						//}else if (bSameUMServer && pCallInfo->m_pChatRoom.get()!=NULL)
					{
						pCallInfo->m_pChatRoom->ExitRoom(0);
						m_pClearChatRoom.add(pCallInfo->m_pChatRoom);
						pCallInfo->m_pChatRoom.reset();
					}
					if (pCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED)
						pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
					else if (pCallInfo->m_nCallState != EB_CALL_STATE_INVALIDATE)
					{
						pCallInfo->m_nCallState = EB_CALL_STATE_HANGUP;
						ClearCallInfo(sCallId,true,false);
					}
				}
			}
			pCallInfo->m_nCmServerState = nCmServerState;
		}else
		{
			tstring sLCServer;
			entboost::GetAddressPort(this->m_sServerAddress.c_str(),sLCServer,nPort);
			bool bSameLCServer = entboost::GetHostIp(sCMServer.c_str(),sCMServer.c_str()) == entboost::GetHostIp(sLCServer.c_str(),sLCServer.c_str());
			pCallInfo->m_nCmServerState = nCmServerState;
			// UM，CM同一台才清空；清空后UM会自动处理；
			// 不同服务器清空会导致CM收不到-103(restart)事件
			if (!bSameLCServer && !bSameUMServer && pCallInfo->m_pChatRoom.get()!=NULL)
			{
				// 跟LC不同服务器，直接重新呼叫；
				if (pCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED)
				{
					pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
					if (pCallInfo->m_pChatRoom.get()!=NULL)
					{
						pCallInfo->m_pChatRoom->ExitRoom(0);
						m_pClearChatRoom.add(pCallInfo->m_pChatRoom);
						pCallInfo->m_pChatRoom.reset();
					}
#ifdef WIN32
					Sleep(6000);	// 延时，避免后台未完全退出；
#else
					sleep(6);	// 延时，避免后台未完全退出；
#endif
					this->InviteCall(pCallInfo,0);
					return;
				}
			}else if (pCallInfo->m_pChatRoom.get()!=NULL)
			//}else if (bSameUMServer && pCallInfo->m_pChatRoom.get()!=NULL)
			{
				pCallInfo->m_pChatRoom->ExitRoom(0);
				m_pClearChatRoom.add(pCallInfo->m_pChatRoom);
				pCallInfo->m_pChatRoom.reset();
			}
			if (pCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED)
				pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
			else if (pCallInfo->m_nCallState != EB_CALL_STATE_INVALIDATE)
			{
				pCallInfo->m_nCallState = EB_CALL_STATE_HANGUP;
				ClearCallInfo(sCallId,true,false);
			}
		}
		//if (nServerState==CEBCallbackInterface::SERVER_MOVED)
		//	pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
		//else if (nServerState==CEBCallbackInterface::SERVER_STOPED)
		//	pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
		//else
		//{
		//	if (pCallInfo->m_sGroupCode.empty())
		//	{
		//		// 单对单会话
		//		tstring sFromAccount;
		//		{
		//			boost::mutex::scoped_lock lock(pCallInfo->m_pCallUserList.mutex());
		//			CLockMap<tstring, CEBAccountInfo::pointer>::iterator pIter;
		//			for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
		//			{
		//				CEBAccountInfo::pointer pAccountInfo = pIter->second;
		//				if (pAccountInfo->GetAccount() != m_pMyAccountInfo->GetAccount())
		//				{
		//					sFromAccount = pAccountInfo->GetAccount();
		//					break;
		//				}
		//			}
		//			lock.unlock();
		//		}
		//		EnterRoom(pCallInfo,sFromAccount.c_str());	// **同时邀请对方，保证对方能收到信息
		//	}else
		//	{
		//		// **群组会话，后台会重新加载群组成员
		//		EnterRoom(pCallInfo);
		//		//if (pCallInfo->m_nCallState == EB_CALL_STATE_EXIT)
		//		//{
		//		//	EnterRoom(pCallInfo);
		//		//}else
		//		//{
		//		//	pCallInfo->m_nCallState = EB_CALL_STATE_INVALIDATE;
		//		//	this->CallGroup(pCallInfo->m_sGroupCode);
		//		//}
		//	}
		//}
	}
}
void CUserManagerApp::OnEnterAnother(const CCrInfo& pParameter)
{
	// ??进入了另外一个会话，当前会话已经无效
	//const mycp::bigint sCallId = pParameter.GetCallId();
	//CEBCallInfo::pointer pCallInfo;
	//if (theCallInfoList.find(sCallId, pCallInfo))
	//{
	//}
}
void CUserManagerApp::OnP2PRequest(const CCrP2PInfo& pP2PInfo)
{
	// 服务端转发P2P请求；
	const mycp::bigint sCallId = pP2PInfo.GetCallId();
	const mycp::bigint sChatId = pP2PInfo.GetChatId();
	const int nP2PData = pP2PInfo.m_nP2PData;	// nP2PData=1：表示对方离线超时，重试

	LogMessage("OnP2PRequest... (callid=%lld,from_uid=%lld,P2PData=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nP2PData);
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		if (pCallInfo->m_sGroupCode>0 || pCallInfo->m_pChatRoom.get()==NULL)
			return;
		CEBAccountInfo::pointer pFromAccount;
		if (pCallInfo->m_pCallUserList.find(pP2PInfo.m_sFromAccount, pFromAccount))
		{
			// 找到对方；
			if (nP2PData==1)	// 对方发文件超时离线，请求重连
			{
				pFromAccount->m_bLocalP2POk = false;
				pFromAccount->m_bRemoteP2POk = false;
				//MessageBox(NULL,"nP2PData==1","",MB_OK);
			}
			DoCheckP2PState(pP2PInfo,pFromAccount);			// **已经打通也不返回，可以再处理一次；
			//if (DoCheckP2PState(pP2PInfo,pFromAccount))	// 已经打通，直接返回
			//{
			//	return;
			//}

			//char lpszBuffer[128];
			//sprintf(lpszBuffer,"%s-%s",pP2PInfo.m_sFromLocalAddress.c_str(),pP2PInfo.m_sFromRemoteAddress.c_str());
			//MessageBox(NULL,lpszBuffer,"OnP2PRequest",MB_OK);
			// LOCAL P2P，这个可以打通，connect&send也可以打通
			if (!pP2PInfo.m_sFromLocalAddress.empty())
			{
				BoostWriteLock wtLock(pFromAccount->m_mutexLocalChatRoom);
				if (pFromAccount->m_pLocalChatRoom.get()==NULL)
				{
					pFromAccount->m_pLocalChatRoom = Cchatroom::create();
					pFromAccount->m_pLocalChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
					pFromAccount->m_pLocalChatRoom->Start(pCallInfo->m_pChatRoom->GetAddress().c_str(),"",0,true,0,1);
					pFromAccount->m_pLocalChatRoom->SetCallId(pCallInfo->GetCallId());
					pFromAccount->m_pLocalChatRoom->SetCallBack(this);
					pFromAccount->m_pLocalChatRoom->SetParam(1);	// 这是P2P
					if (!m_pLocalHostIp.empty())
					{
						// *这边也主动请求一次；
						char lpszLocalIp[64];
						sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pFromAccount->m_pLocalChatRoom->GetLocalPort());
						LogMessage("OnP2PRequest->m_pLocalChatRoom->SendP2PRequest... (callid=%lld,from_uid=%lld,LocalIp=%s)\r\n",sCallId,pP2PInfo.m_sFromAccount,lpszLocalIp);
						pFromAccount->m_pLocalChatRoom->SetDestAddress(0,pCallInfo->m_pChatRoom->GetAddress());
						pFromAccount->m_pLocalChatRoom->SendP2PRequest(m_pMyAccountInfo->GetUserId(),pCallInfo->m_sChatId,pFromAccount->GetUserId(),lpszLocalIp,0);
					}
				}
				if (!m_pLocalHostIp.empty())
				{
					char lpszLocalIp[64];
					sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pFromAccount->m_pLocalChatRoom->GetLocalPort());
					//if (!pFromAccount->m_bLocalP2POk)
					//{
					//	// *这边也主动请求一次；
					//	pFromAccount->m_pLocalChatRoom->SetDestAddress(0,pCallInfo->m_pChatRoom->GetAddress());
					//	pFromAccount->m_pLocalChatRoom->SendP2PRequest(m_pMyAccountInfo->GetUserId(),pCallInfo->m_sChatId,pFromAccount->GetUserId(),lpszLocalIp,0);
					//}

					LogMessage("OnP2PRequest->m_pLocalChatRoom->SendP2PResponse... (callid=%lld,from_uid=%lld,LocalIp=%s)\r\n",sCallId,pP2PInfo.m_sFromAccount,lpszLocalIp);
					pFromAccount->m_pLocalChatRoom->SetDestAddress(0,pCallInfo->m_pChatRoom->GetAddress());
					pFromAccount->m_pLocalChatRoom->SendP2PResponse(m_pMyAccountInfo->GetUserId(),sChatId,pP2PInfo.m_sFromAccount,lpszLocalIp,0);

					if (pFromAccount->m_bLocalP2POk && pFromAccount->m_pLocalChatRoom.get()!=NULL)
					{
						pFromAccount->m_pLocalChatRoom->SetDestAddress(1,pFromAccount->GetLocalAddress());
						pFromAccount->m_pLocalChatRoom->SendP2PTest(3);
					}
				}
			}

			// REMOTE P2P
			if (!pFromAccount->m_bLocalP2POk)
			{
				BoostWriteLock wtLock(pFromAccount->m_mutexRemoteChatRoom);
				if (pFromAccount->m_pRemoteChatRoom.get()==NULL)
				{
					pFromAccount->m_pRemoteChatRoom = Cchatroom::create();
					pFromAccount->m_pRemoteChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
					pFromAccount->m_pRemoteChatRoom->Start(pCallInfo->m_pChatRoom->GetAddress().c_str(),"",0,true,0,1);
					pFromAccount->m_pRemoteChatRoom->SetCallId(pCallInfo->GetCallId());
					pFromAccount->m_pRemoteChatRoom->SetCallBack(this);
					pFromAccount->m_pRemoteChatRoom->SetParam(1);	// 这是P2P
					pFromAccount->m_pRemoteChatRoom->SetDestAddress(0,pCallInfo->m_pChatRoom->GetAddress());
					LogMessage("OnP2PRequest->m_pRemoteChatRoom->SendP2PRequest... (callid=%lld,from_uid=%lld)\r\n",sCallId,pP2PInfo.m_sFromAccount);
					pFromAccount->m_pRemoteChatRoom->SendP2PRequest(m_pMyAccountInfo->GetUserId(),pCallInfo->m_sChatId,pFromAccount->GetUserId(),"",0);
				}
				//if (!pFromAccount->m_bRemoteP2POk)
				//{
				//	pFromAccount->m_pRemoteChatRoom->SetDestAddress(0,pCallInfo->m_pChatRoom->GetAddress());
				//	pFromAccount->m_pRemoteChatRoom->SendP2PRequest(m_pMyAccountInfo->GetUserId(),pCallInfo->m_sChatId,pFromAccount->GetUserId(),"",0);
				//}
				LogMessage("OnP2PRequest->m_pRemoteChatRoom->SendP2PResponse... (callid=%lld,from_uid=%lld)\r\n",sCallId,pP2PInfo.m_sFromAccount);
				pFromAccount->m_pRemoteChatRoom->SetDestAddress(0,pCallInfo->m_pChatRoom->GetAddress());
				pFromAccount->m_pRemoteChatRoom->SendP2PResponse(m_pMyAccountInfo->GetUserId(),sChatId,pP2PInfo.m_sFromAccount,"",0);

				if (pFromAccount->m_bRemoteP2POk && pFromAccount->m_pRemoteChatRoom.get()!=NULL)
				{
					pFromAccount->m_pRemoteChatRoom->SetDestAddress(1,pFromAccount->GetRemoteAddress());
					pFromAccount->m_pRemoteChatRoom->SendP2PTest(3);
				}
			}
		}
	}
}
bool CUserManagerApp::DoCheckP2PState(const CCrP2PInfo& pP2PInfo,const CEBAccountInfo::pointer& pFromAccount)
{
	if (pFromAccount->m_bLocalP2POk || pFromAccount->m_bRemoteP2POk)
	{
		bool bResult = true;
		{
			BoostWriteLock wtLock(pFromAccount->m_mutexLocalChatRoom);
			if (pFromAccount->m_pLocalChatRoom.get()!=NULL)
			{
				//bool bChangeIp = false;
				if (!pP2PInfo.m_sFromLocalAddress.empty() && pP2PInfo.m_sFromLocalAddress!=pFromAccount->GetLocalAddress())
				{
					bResult = false;
					pFromAccount->m_bLocalP2POk = false;
					LogMessage("DoCheckP2PState LocalAddress Change... (from_uid=%lld, %s -> %s)\r\n",pP2PInfo.m_sFromAccount,pFromAccount->GetLocalAddress().c_str(),pP2PInfo.m_sFromLocalAddress.c_str());

					{
						const mycp::bigint sCallId = pP2PInfo.GetCallId();
						CToSendList::pointer pOnlineFileToSendList;
						if (theP2PFileList.find(sCallId,pOnlineFileToSendList,true))
						{
							// **找到前面P2P发送文件，放到 calltosend 列表中，重新发送
							CToSendList::pointer pCallToSendList;
							if (!theCallToSendList.find(sCallId,pCallToSendList))
							{
								pCallToSendList = CToSendList::create(sCallId,0);
								theCallToSendList.insert(sCallId,pCallToSendList);
							}
							BoostReadLock lock(pOnlineFileToSendList->m_list.mutex());
							CLockList<CToSendInfo::pointer>::iterator pIter;
							for (pIter=pOnlineFileToSendList->m_list.begin(); pIter!=pOnlineFileToSendList->m_list.end(); pIter++)
							{
								const CToSendInfo::pointer& pCallToSendInfo = *pIter;
								pCallToSendList->AddFilePath(pCallToSendInfo->m_sFilePath,pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile);
								if (pCallToSendInfo->m_nMsgId>0)
								{
									pFromAccount->m_pLocalChatRoom->SetDestAddress(1,pFromAccount->GetLocalAddress());
									pFromAccount->m_pLocalChatRoom->CancelSendingFile(pCallToSendInfo->m_nMsgId);
#ifdef WIN32
									Sleep(100);
#else
									usleep(100000);
#endif
								}
							}
						}
					}
					pFromAccount->SetLocalAddress(pP2PInfo.m_sFromLocalAddress);		// 更新地址
					//bChangeIp = true;
					//pFromAccount->m_pLocalChatRoom->SetParam(2);	// * 无效P2P，预防 OnP2POk 返回
					m_pClearChatRoom.add(pFromAccount->m_pLocalChatRoom);
					pFromAccount->m_pLocalChatRoom.reset();

					return false;
				}
				pFromAccount->m_pLocalChatRoom->SetDestAddress(1,pFromAccount->GetLocalAddress());
				//if (bChangeIp)
				//{
				//	const mycp::bigint sCallId = pP2PInfo.GetCallId();
				//	const mycp::bigint sChatId = pP2PInfo.GetChatId();
				//	pFromAccount->m_pLocalChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),1);	// 1=内网地址
				//}else
				{
					pFromAccount->m_pLocalChatRoom->SendP2PTest(3);
				}
			}
		}
		{
			BoostWriteLock wtLock(pFromAccount->m_mutexRemoteChatRoom);
			if (pFromAccount->m_pRemoteChatRoom.get()!=NULL)
			{
				//bool bChangeIp = false;
				if (!pP2PInfo.m_sFromRemoteAddress.empty() && pP2PInfo.m_sFromRemoteAddress!=pFromAccount->GetRemoteAddress())
				{
					bResult = false;
					pFromAccount->m_bRemoteP2POk = false;
					LogMessage("DoCheckP2PState RemoteAddress Change... (from_uid=%lld, %s -> %s)\r\n",pP2PInfo.m_sFromAccount,pFromAccount->GetRemoteAddress().c_str(),pP2PInfo.m_sFromRemoteAddress.c_str());

					{
						const mycp::bigint sCallId = pP2PInfo.GetCallId();
						CToSendList::pointer pOnlineFileToSendList;
						if (theP2PFileList.find(sCallId,pOnlineFileToSendList,true))
						{
							// **找到前面P2P发送文件，放到在线列表中，重新发送
							CToSendList::pointer pCallToSendList;
							if (!theCallToSendList.find(sCallId,pCallToSendList))
							{
								pCallToSendList = CToSendList::create(sCallId,0);
								theCallToSendList.insert(sCallId,pCallToSendList);
							}
							BoostReadLock lock(pOnlineFileToSendList->m_list.mutex());
							CLockList<CToSendInfo::pointer>::iterator pIter;
							for (pIter=pOnlineFileToSendList->m_list.begin(); pIter!=pOnlineFileToSendList->m_list.end(); pIter++)
							{
								const CToSendInfo::pointer& pCallToSendInfo = *pIter;
								pCallToSendList->AddFilePath(pCallToSendInfo->m_sFilePath,pCallToSendInfo->m_sTo,pCallToSendInfo->m_bPrivate,pCallToSendInfo->m_bOffFile);
								if (pCallToSendInfo->m_nMsgId>0)
								{
									pFromAccount->m_pRemoteChatRoom->SetDestAddress(1,pFromAccount->GetRemoteAddress());
									pFromAccount->m_pRemoteChatRoom->CancelSendingFile(pCallToSendInfo->m_nMsgId);
#ifdef WIN32
									Sleep(100);
#else
									usleep(100000);
#endif
								}
							}
						}
					}
					pFromAccount->SetRemoteAddress(pP2PInfo.m_sFromRemoteAddress);		// 更新地址
					//bChangeIp = true;
					//pFromAccount->m_pRemoteChatRoom->SetParam(2);	// * 无效P2P，预防 OnP2POk 返回
					m_pClearChatRoom.add(pFromAccount->m_pRemoteChatRoom);
					pFromAccount->m_pRemoteChatRoom.reset();
					return false;
				}
				pFromAccount->m_pRemoteChatRoom->SetDestAddress(1,pFromAccount->GetRemoteAddress());
				//if (bChangeIp)
				//{
				//	const mycp::bigint sCallId = pP2PInfo.GetCallId();
				//	const mycp::bigint sChatId = pP2PInfo.GetChatId();
				//	pFromAccount->m_pRemoteChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),2);	// 2=NAT公网地址
				//}else
				{
					pFromAccount->m_pRemoteChatRoom->SendP2PTest(3);
				}
			}
		}
		return bResult;
	}
	if (!pP2PInfo.m_sFromLocalAddress.empty())
		pFromAccount->SetLocalAddress(pP2PInfo.m_sFromLocalAddress);		// 这个内网地址，可以打通P2P；connect&send也可以打通
	//else
	if (!pP2PInfo.m_sFromRemoteAddress.empty())
		pFromAccount->SetRemoteAddress(pP2PInfo.m_sFromRemoteAddress);		// 这个外网地址，可以打通P2P；（大部分路由器支持）
	return false;
}

bool CUserManagerApp::DoP2PTask(const CCrP2PInfo& pP2PInfo,bool& pOutP2POk)
{
	const mycp::bigint sCallId = pP2PInfo.GetCallId();
	const mycp::bigint sChatId = pP2PInfo.GetChatId();
	//const int nP2PData = pP2PInfo.m_nP2PData;

	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		if (pCallInfo->m_pChatRoom.get()==NULL)
			return false;
		CEBAccountInfo::pointer pFromAccount;
		if (!pCallInfo->m_pCallUserList.find(pP2PInfo.m_sFromAccount, pFromAccount))
			return false;

		if (DoCheckP2PState(pP2PInfo,pFromAccount))
		{
			pOutP2POk = true;
			return false;	// 已经打通P2P
		}

		//CCgcAddress pAddress(pP2PInfo.m_sFromRemoteAddress);
		//char lpszBuffer[128];
		//sprintf(lpszBuffer,"%s: n:%d<->h:%d",pAddress.address().c_str(),::htons(pAddress.getport()),pAddress.getport());
		//MessageBox(NULL,lpszBuffer,"OnP2PMsg remote-address",MB_OK);
		// LOCAL P2P
		{
			BoostReadLock rdLock(pFromAccount->m_mutexLocalChatRoom);
			if (!pFromAccount->GetLocalAddress().empty() && pFromAccount->m_pLocalChatRoom.get()!=NULL)
			{
				Cchatroom::pointer pChatRoom = pFromAccount->m_pLocalChatRoom;	// 使用这个，不会导致OnP2POk清空异常
				pChatRoom->SetDestAddress(1,pFromAccount->GetLocalAddress());
				pChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),1);	// 1=内网地址
			}
		}
		// REMOTE P2P
		{
			BoostReadLock rdLock(pFromAccount->m_mutexRemoteChatRoom);
			if (!pFromAccount->GetRemoteAddress().empty() && pFromAccount->m_pRemoteChatRoom.get()!=NULL)
			{
				Cchatroom::pointer pChatRoom = pFromAccount->m_pRemoteChatRoom;	// 使用这个，不会导致OnP2POk清空异常
				pChatRoom->SetDestAddress(1,pFromAccount->GetRemoteAddress());
				pChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),2);	// 2=NAT公网地址
			}
		}
		return true;
	}
	return false;
}

void CUserManagerApp::OnP2PResponse(const CCrP2PInfo& pP2PInfo,int nFlag)
{
	// nFlag==0服务端转发P2P请求；
	// nFlag==1:响应返回；
	const mycp::bigint sCallId = pP2PInfo.GetCallId();
	const mycp::bigint sChatId = pP2PInfo.GetChatId();
	LogMessage("OnP2PResponse... (callid=%lld,from_uid=%lld,flag=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nFlag);
	if (nFlag==0)
	{
		// nFlag==0服务端转发P2P响应；
		const int nP2PData = pP2PInfo.m_nP2PData;
		bool bP2POk = false;
		if (DoP2PTask(pP2PInfo,bP2POk))
		{
			// 未打通P2P，或其他错误，需要保存做超时检查
			CP2PTaskInfo::pointer pP2PTaskInfo;
			if (!m_pP2PTaskInfo.find(sCallId,pP2PTaskInfo))
			{
				pP2PTaskInfo = CP2PTaskInfo::create();
				pP2PTaskInfo->m_bP2PResponse = true;
				m_pP2PTaskInfo.insert(sCallId,pP2PTaskInfo,false);
			}
			pP2PTaskInfo->m_pP2PInfo = pP2PInfo;
			pP2PTaskInfo->m_tP2PTry = time(0);
			pP2PTaskInfo->m_bP2PResponse = true;
			LogMessage("OnP2PResponse->P2P State FALSE... (callid=%lld,from_uid=%lld,flag=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nFlag);
		}else if (bP2POk)
		{
			LogMessage("OnP2PResponse->P2P State TRUE... (callid=%lld,from_uid=%lld,flag=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nFlag);
			m_pP2POkList.add(sCallId);	// 使用这个，让定时器有机会，重新发送文件列表；
			//m_pP2PTaskInfo.remove(sCallId);
		}
	}else if (nFlag==1)
	{
		// nFlag==1:响应返回；
		CEBCallInfo::pointer pCallInfo;
		if (theCallInfoList.find(sCallId, pCallInfo))
		{
			if (pCallInfo->m_sGroupCode>0 || pCallInfo->m_pChatRoom.get()==NULL)
				return;
			CEBAccountInfo::pointer pFromAccount;
			if (pCallInfo->m_pCallUserList.find(pP2PInfo.m_sFromAccount, pFromAccount))
			{
				if (!pFromAccount->GetLocalAddress().empty())
				//if (!pFromAccount->m_bLocalP2POk && !pFromAccount->GetLocalAddress().empty())
				{
					BoostReadLock rdLock(pFromAccount->m_mutexLocalChatRoom);
					if (pFromAccount->m_pLocalChatRoom.get()!=NULL)
					{
						pFromAccount->m_pLocalChatRoom->SetDestAddress(1,pFromAccount->GetLocalAddress());
						if (pFromAccount->m_bLocalP2POk)
							pFromAccount->m_pLocalChatRoom->SendP2PTest(3);
						else
						{
							CP2PTaskInfo::pointer pP2PTaskInfo;
							if (m_pP2PTaskInfo.find(sCallId,pP2PTaskInfo))
							{
								pP2PTaskInfo->m_tP2PTry = time(0);
							}
							LogMessage("OnP2PResponse->m_pLocalChatRoom->SendP2PTry... (callid=%lld,from_uid=%lld,flag=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nFlag);
							pFromAccount->m_pLocalChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),1);	// 1=内网地址
						}
					}
				}

				if (!pFromAccount->GetRemoteAddress().empty())
				//if (!pFromAccount->m_bRemoteP2POk && !pFromAccount->GetRemoteAddress().empty())
				{
					BoostReadLock rdLock(pFromAccount->m_mutexRemoteChatRoom);
					if (pFromAccount->m_pRemoteChatRoom.get()!=NULL)
					{
						pFromAccount->m_pRemoteChatRoom->SetDestAddress(1,pFromAccount->GetRemoteAddress());
						if (pFromAccount->m_bRemoteP2POk)
							pFromAccount->m_pRemoteChatRoom->SendP2PTest(3);
						else
						{
							CP2PTaskInfo::pointer pP2PTaskInfo;
							if (m_pP2PTaskInfo.find(sCallId,pP2PTaskInfo))
							{
								pP2PTaskInfo->m_tP2PTry = time(0);
							}
							LogMessage("OnP2PResponse->m_pRemoteChatRoom->SendP2PTry... (callid=%lld,from_uid=%lld,flag=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nFlag);
							pFromAccount->m_pRemoteChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),2);	// 2=NAT公网地址
						}
					}
				}
			}
		}
	}
}

void CUserManagerApp::OnP2POk(const CCrP2PInfo& pP2PInfo)
{
	// 已经打通P2P通道；
	const mycp::bigint sCallId = pP2PInfo.GetCallId();
	const mycp::bigint sChatId = pP2PInfo.GetChatId();
	const int nP2PData = pP2PInfo.m_nP2PData;
	if (pP2PInfo.GetParam()!=1)
	{
		LogMessage("OnP2POk error param (callid=%lld,from_uid=%lld,P2PData=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nP2PData);
		return;
	}
	LogMessage("OnP2POk... (callid=%lld,from_uid=%lld,P2PData=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nP2PData);

	bool bRealP2POk = false;
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		//MessageBox(NULL,"OnP2POk","local",MB_OK);
		CEBAccountInfo::pointer pFromAccount;
		if (pCallInfo->m_pCallUserList.find(pP2PInfo.m_sFromAccount, pFromAccount))
		{
			{
				BoostReadLock rdLock(pFromAccount->m_mutexLocalChatRoom);
				if (nP2PData==1 && pFromAccount->m_pLocalChatRoom.get()!=NULL)
				{
					bRealP2POk = true;
					//MessageBox(NULL,"P2P","内网P2POK！",MB_OK);
					// 找到对方；
					LogMessage("OnP2POk->LocalP2POk TRUE... (callid=%lld,from_uid=%lld,P2PData=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nP2PData);
					if (!pFromAccount->m_bLocalP2POk)
					{
						pFromAccount->m_bLocalP2POk = true;
						pFromAccount->m_pLocalChatRoom->SetDestAddress(1,pFromAccount->GetLocalAddress());
						//pFromAccount->m_pLocalChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),1);	// 1=NAT内网地址
					}
					// 清空外网P2P，否则会有问题
					pFromAccount->m_bRemoteP2POk = false;
					BoostWriteLock wtLock(pFromAccount->m_mutexRemoteChatRoom);
					if (pFromAccount->m_pRemoteChatRoom.get()!=NULL)
					{
						m_pClearChatRoom.add(pFromAccount->m_pRemoteChatRoom);
						pFromAccount->m_pRemoteChatRoom.reset();
					}
				}
			}
			{
				BoostReadLock rdLock(pFromAccount->m_mutexRemoteChatRoom);
				if (nP2PData==2 && pFromAccount->m_pRemoteChatRoom.get()!=NULL && !pFromAccount->m_bLocalP2POk)
				{
					// 找到对方；
					LogMessage("OnP2POk->RemoteP2POk TRUE... (callid=%lld,from_uid=%lld,P2PData=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nP2PData);
					// ** 改成等待3秒后才处理，避免这边先打通远端，再打通本地；而对方先打通远端，再打通本地，导致二方不能正常P2P通讯；
					CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_WAIT_LOCAL_P2P_OK);
					pProcessMsgInfo->m_tProcessTime = time(0)+2;	// 012，等待三秒
					pProcessMsgInfo->m_pCallInfo = pCallInfo;
					pProcessMsgInfo->m_pFromAccount = pFromAccount;
					m_pProcessMsgList.add(pProcessMsgInfo);
					//bRealP2POk = true;
					////MessageBox(NULL,"P2P","外网P2POK！",MB_OK);
					//if (!pFromAccount->m_bRemoteP2POk)
					//{
					//	pFromAccount->m_bRemoteP2POk = true;
					//	pFromAccount->m_pRemoteChatRoom->SetDestAddress(1,pFromAccount->GetRemoteAddress());
					//	//pFromAccount->m_pRemoteChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),2);	// 2=NAT公网地址
					//}
					// 清空内网P2P，否则会有问题
					//pFromAccount->m_bLocalP2POk = false;
					//BoostWriteLock wtLock(pFromAccount->m_mutexLocalChatRoom);
					//if (pFromAccount->m_pLocalChatRoom.get()!=NULL)
					//{
					//	m_pClearChatRoom.add(pFromAccount->m_pLocalChatRoom);
					//	pFromAccount->m_pLocalChatRoom.reset();
					//}
				}
			}
		}
		if (bRealP2POk)
		{
			pCallInfo->m_tP2POkTime = time(0);
			pCallInfo->m_nCallData |= EB_CALL_DATA_P2P_OK;
		}
	}
	if (bRealP2POk)
	{
		LogMessage("OnP2POk->m_pP2POkList.add... (callid=%lld,from_uid=%lld,P2PData=%d)\r\n",sCallId,pP2PInfo.m_sFromAccount,nP2PData);
		m_pP2POkList.add(sCallId);
	}
}

void CUserManagerApp::ProcessMemberInfo(const CEBGroupInfo::pointer& pDepartmentInfo,const CEBMemberInfo::pointer& pEmployeeInfo,bool bSendGroupInfoEvent)
{
	const mycp::bigint s_emp_code = pEmployeeInfo->m_sMemberCode;
	const mycp::bigint nMemberUserId = pEmployeeInfo->m_nMemberUserId;
	const tstring s_emp_account = pEmployeeInfo->m_sMemberAccount;
	const mycp::bigint s_dep_code = pDepartmentInfo->m_sGroupCode;
	//bool bNewEmployeeInfo = false;
	if (!pDepartmentInfo->m_pMemberList.exist(nMemberUserId))
	{
		//bNewEmployeeInfo = true;
		if ((pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER)==0 &&
			pDepartmentInfo->m_sEnterpriseCode>0 &&
			pDepartmentInfo->m_nServerGroupVer>pDepartmentInfo->m_nGroupVer && pDepartmentInfo->m_nGroupVer==0 && pDepartmentInfo->m_pMemberList.empty())
		{
			// 未加载成员
			pDepartmentInfo->m_nEmpCount += 1;
			SetLocalGroupEmpCount(pDepartmentInfo->m_sGroupCode,pDepartmentInfo->m_nEmpCount);
			const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
			if (m_callback)
				m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
				pEvent->SetEventParameter((long)(bIsMyDepartment?1:0));
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
			}
			return;	// 不走后面；
		}else
		{
			pDepartmentInfo->m_pMemberList.insert(nMemberUserId, pEmployeeInfo);
			if (pDepartmentInfo->m_nEmpCount<(int)pDepartmentInfo->m_pMemberList.size())
			{
				pDepartmentInfo->m_nEmpCount = pDepartmentInfo->m_pMemberList.size();
				SetLocalGroupEmpCount(pDepartmentInfo->m_sGroupCode,pDepartmentInfo->m_nEmpCount);
				bSendGroupInfoEvent = true;
			}
		}
	}
	bool bSendCallback = false;
	if (m_pMyAccountInfo->GetUserId() == nMemberUserId)
	{
		if (pDepartmentInfo->m_nMyEmpId==0)
			pDepartmentInfo->m_nMyEmpId = pEmployeeInfo->m_sMemberCode;
		if (!theMyEmployeeList.exist(s_dep_code))
		{
			pEmployeeInfo->m_nLineState = m_pMyAccountInfo->GetLineState();
			theMyEmployeeList.insert(s_dep_code,pEmployeeInfo);
			bSendGroupInfoEvent = false;
			if (m_callback)
				m_callback->onGroupInfo(pDepartmentInfo.get(),true);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
				pEvent->SetEventParameter((long)1);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_GROUP_INFO,(WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),1);
#endif
			}
		}
		if (s_emp_code==m_pMyAccountInfo->GetDefaultEmpId() &&
			(m_pDefaultEntEmployeeInfo.get()==NULL || m_pDefaultEntEmployeeInfo.get()!=pEmployeeInfo.get() || (m_pDefaultEntEmployeeInfo.get() != NULL &&  m_pDefaultEntEmployeeInfo->m_sMemberCode!=s_emp_code)))
			//(m_pDefaultEntEmployeeInfo.get()==NULL || (m_pDefaultEntEmployeeInfo.get() != NULL &&  m_pDefaultEntEmployeeInfo->m_sMemberCode!=s_emp_code)))
		{
			m_pDefaultEntEmployeeInfo = pEmployeeInfo;
			bSendCallback = true;
			//if (m_pDefaultEnterpriseInfo.get()!=NULL && m_pDefaultEnterpriseInfo->m_sEnterpriseCode>0)
			//{
			//	m_pMyAccountInfo->SetUserName(pEmployeeInfo->m_sUserName);
			//}

			if (m_callback)
				m_callback->onMemberInfo(pEmployeeInfo.get(),true);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_MEMBER_INFO);
				pEvent->SetEventParameter((long)1);
                pEvent->SetEventBigParameter(1);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_MEMBER_INFO, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), 1);
#endif
			}

			//if (m_pMyAccountInfo->GetDefaultEmpId()==0)
			//{
			//	// 设置一次默认群组；
			//	this->SetDefaltMemberCode(m_pDefaultEntEmployeeInfo->m_sMemberCode);
			//}
		}
	}

	if (bSendGroupInfoEvent)
	{
		// 添加用户，需要通知一次群组事件；
		const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
		if (m_callback)
			m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
			pEvent->SetEventParameter((long)(bIsMyDepartment?1:0));
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
		}
	}

	if (!bSendCallback &&
		!pDepartmentInfo->m_pMemberList.empty())	// ** 不为空表示已经加载成员；
	{
		const bool bMyDefaultMember = m_pMyAccountInfo->GetDefaultEmpId()==s_emp_code?true:false;
		//if (bMyDefaultMember && m_pDefaultEnterpriseInfo.get()!=NULL && m_pDefaultEnterpriseInfo->m_sEnterpriseCode>0)
		//{
		//	m_pMyAccountInfo->SetUserName(pEmployeeInfo->m_sUserName);
		//}
		if (m_callback)
			m_callback->onMemberInfo(pEmployeeInfo.get(),bMyDefaultMember);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_MEMBER_INFO);
            pEvent->SetEventParameter((long)(bMyDefaultMember?1:0));
            pEvent->SetEventBigParameter(theMyEmployeeList.exist(pEmployeeInfo->m_sGroupCode)?1:0);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_MEMBER_INFO, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), bMyDefaultMember?1:0);
#endif
		}
	}
	// 检查成员是否属于某个会话，加入会话处理（在离线请求进入群组，接受有用）
	CEBCallInfo::pointer pCallInfo;
	const mycp::bigint sCallId = s_dep_code;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		// 一对一会话转讨论组时，加载组成员信息在后，用于更新界面信息
		//if (bNewEmployeeInfo)
		//{
		//	// ? 群会话已经分开，这里不需要处理
		//	//if (m_pHwnd!=NULL)
		//	//{
		//	//	::SendMessage(m_pHwnd, POP_WM_CALL_EMP_INFO, (WPARAM)pCallInfo.get(), (LPARAM)pEmployeeInfo.get());
		//	//}
		//}

		CEBAccountInfo::pointer pEmpAccount;
		if (!pCallInfo->m_pCallUserList.find(nMemberUserId, pEmpAccount))
		{
			pEmpAccount = pEmployeeInfo->GetAccountInfo();
			pEmpAccount->m_pFromCardInfo.m_nAccountType = EB_ACCOUNT_TYPE_IN_ENT;
			pEmployeeInfo->GetECardInfo(&pEmpAccount->m_pFromCardInfo);
			pCallInfo->m_pCallUserList.insert(nMemberUserId,pEmpAccount);
			if (pCallInfo->m_sChatId>0)
			{
				EnterRoom(pCallInfo);
				SendConnectMsg(pCallInfo,nMemberUserId,s_emp_account,EB_CONNECTED_OFFLINE_USER);
			}
		}
	}
}

tstring CUserManagerApp::GetCmServerKey(const tstring& sCmServer)
{
	tstring sAddress;
	int nPort = 0;
	if (!entboost::GetAddressPort(sCmServer.c_str(),sAddress,nPort))
		return "";
	sAddress = entboost::GetHostIp(sAddress.c_str(),sAddress.c_str());
	char lpszBuffer[64];
	sprintf(lpszBuffer,"%s:%d",sAddress.c_str(),nPort);
	return lpszBuffer;
}

void CUserManagerApp::ProcessContactHeadInfo(const CEBContactInfo::pointer& pContactInfo,bool bUserHeadChange)
{
	if (pContactInfo.get()==NULL) return;
	const mycp::bigint sHeadResourceId = pContactInfo->m_sHeadResourceId;
	const tstring sHeadCmServer(pContactInfo->m_sHeadCmServer);
	if (sHeadResourceId>0 && !sHeadCmServer.empty())
	{
#ifdef _QT_MAKE_
        const EBFileString sHeadResourceFile = QString("%1/%2").arg(m_sEbResourcePath).arg(sHeadResourceId);
        if (QFileInfo::exists(sHeadResourceFile))
#else
        char sHeadResourceFile[260];
        sprintf(sHeadResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),sHeadResourceId);
        if (::PathFileExistsA(sHeadResourceFile))
#endif
		{
			if (pContactInfo->m_sHeadMd5.empty())
			{
#ifdef _QT_MAKE_
				QImage pImage;
				if (!pImage.load(sHeadResourceFile) || pImage.width()<=0) {
                    QFile::remove(sHeadResourceFile);
				}else
#else
//#ifdef _MSC_VER
				USES_CONVERSION;
                Gdiplus::Image pImage(A2W(sHeadResourceFile));
                const bool bIsImage = (bool)(pImage.GetType()!= Gdiplus::ImageTypeUnknown);
				if (!bIsImage)
				{
					// 前面文件下载错误
#ifdef _QT_MAKE_
					QFile::remove(sHeadResourceFile);
#else
					DeleteFileA(sHeadResourceFile);
#endif
				}else
#endif
				{
					pContactInfo->m_sHeadResourceFile = sHeadResourceFile;
				}
			}else
			{
                /// 判断MD5值
				mycp::bigint nFileSize = 0;
				tstring sFileMd5String;
				GetFileMd5(sHeadResourceFile,nFileSize,sFileMd5String);
				if (sFileMd5String==pContactInfo->m_sHeadMd5)
				{
					pContactInfo->m_sHeadResourceFile = sHeadResourceFile;
				}else
				{
					// MD5已经变了，删除后重新下载
#ifdef _QT_MAKE_
                    QFile::remove(sHeadResourceFile);
#else
                    DeleteFileA(sHeadResourceFile);
#endif
				}
			}
		}
#ifdef _QT_MAKE_
        if (bUserHeadChange && QFileInfo::exists(sHeadResourceFile))
#else
        if (bUserHeadChange && ::PathFileExistsA(sHeadResourceFile))
#endif
		{
			if (this->m_callback)
				m_callback->onContactHeadChange(pContactInfo.get());
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_ContactInfo * pEvent = new EB_ContactInfo(pContactInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_HEAD_CHANGE);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_CONTACT_HEAD_CHANGE, (WPARAM)(const EB_ContactInfo*)pContactInfo.get(), 0);
#endif
			}
#ifdef _QT_MAKE_
        }else if (!QFileInfo::exists(sHeadResourceFile) && !theResourceFilePath.exist(sHeadResourceId))
#else
        }else if (!::PathFileExistsA(sHeadResourceFile) && !theResourceFilePath.exist(sHeadResourceId))
#endif
		{
			// 资源文件不存在，请求下载
			const tstring sCmServerKey = GetCmServerKey(sHeadCmServer);
			Cchatroom::pointer pChatRoom;
			if (!theChatRoomList.find(sCmServerKey,pChatRoom))
			{
				pChatRoom = Cchatroom::create();
				pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
				pChatRoom->Start(sHeadCmServer.c_str(), "",0,true,1);
				pChatRoom->SetCallBack(this);
				theChatRoomList.insert(sCmServerKey,pChatRoom);
			}
			theResourceChatRoom.insert(sHeadResourceId,pChatRoom);
			CFilePathInfo::pointer pFilePathInfo = CFilePathInfo::create(CFilePathInfo::FILE_PAHT_HEAD,sHeadResourceFile,true);
			pFilePathInfo->m_pContactInfo = pContactInfo;
			pFilePathInfo->m_nBigData = bUserHeadChange?1:0;
			theResourceFilePath.insert(sHeadResourceId,pFilePathInfo);

			CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE);
			pProcessMsgInfo->m_pChatRoom = pChatRoom;
			pProcessMsgInfo->m_nBigInt1 = sHeadResourceId;
			//pProcessMsgInfo->m_nBigInt2 = bUserHeadChange?1:0;
			pProcessMsgInfo->m_sString1 = sHeadResourceFile;
			//pProcessMsgInfo->m_sString1.append(".ebtemp");
			m_pProcessMsgList.add(pProcessMsgInfo);

			//if (!pChatRoom->IsEnterOk())
			//	pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
			//pChatRoom->RecviveResource(sHeadResourceId,sHeadResourceFile);
			////char sTempFile[260];
			////sprintf(sTempFile,"%s.ebtemp",sHeadResourceFile);
			////pChatRoom->RecviveResource(sHeadResourceId,sTempFile);
		}
	}
}
void CUserManagerApp::ProcessAccountHeadInfo(const CEBAccountInfo::pointer& pAccountInfo)
{
	if (pAccountInfo.get()==NULL)
		return;
	const mycp::bigint sHeadResourceId = pAccountInfo->m_pFromCardInfo.m_nHeadResId;
	const tstring sHeadCmServer(pAccountInfo->m_pFromCardInfo.m_sHeadCmServer);
	if (sHeadResourceId>0 && !sHeadCmServer.empty())
	{
#ifdef _QT_MAKE_
        const EBFileString sHeadResourceFile = QString("%1/%2").arg(m_sEbResourcePath).arg(sHeadResourceId);
        if (QFileInfo::exists(sHeadResourceFile))
#else
        char sHeadResourceFile[260];
        sprintf(sHeadResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),sHeadResourceId);
        if (::PathFileExistsA(sHeadResourceFile))
#endif
		{
			if (pAccountInfo->m_pFromCardInfo.m_sHeadMd5.empty())
			{
#ifdef _QT_MAKE_
                QImage pImage;
                if (!pImage.load(sHeadResourceFile) || pImage.width()<=0) {
                    QFile::remove(sHeadResourceFile);
                }else
#else
//#ifdef _MSC_VER
				USES_CONVERSION;
				Gdiplus::Image pImage(A2W(sHeadResourceFile));
				const bool bIsImage = (bool)(pImage.GetType()!= Gdiplus::ImageTypeUnknown);
				if (!bIsImage)
				{
					// 前面文件下载错误
					DeleteFileA(sHeadResourceFile);
				}else
#endif
				{
					pAccountInfo->m_pFromCardInfo.m_sHeadResourceFile = sHeadResourceFile;
				}
			}else
			{
				// 判断MD5值
				mycp::bigint nFileSize = 0;
				tstring sFileMd5String;
				GetFileMd5(sHeadResourceFile,nFileSize,sFileMd5String);
				if (sFileMd5String==pAccountInfo->m_pFromCardInfo.m_sHeadMd5)
				{
					pAccountInfo->m_pFromCardInfo.m_sHeadResourceFile = sHeadResourceFile;
				}else
				{
					// MD5已经变了，删除后重新下载
#ifdef _QT_MAKE_
                    QFile::remove(sHeadResourceFile);
#else
                    DeleteFileA(sHeadResourceFile);
#endif
				}
			}
		}
#ifdef _QT_MAKE_
        if (!QFileInfo::exists(sHeadResourceFile) && !theResourceFilePath.exist(sHeadResourceId))
#else
        if (!::PathFileExistsA(sHeadResourceFile) && !theResourceFilePath.exist(sHeadResourceId))
#endif
		{
			// 资源文件不存在，请求下载
			const tstring sCmServerKey = GetCmServerKey(sHeadCmServer);
			Cchatroom::pointer pChatRoom;
			if (!theChatRoomList.find(sCmServerKey,pChatRoom))
			{
				pChatRoom = Cchatroom::create();
				pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
				pChatRoom->Start(sHeadCmServer.c_str(), "",0,true,1);
				pChatRoom->SetCallBack(this);
				theChatRoomList.insert(sCmServerKey,pChatRoom);
			}
			theResourceChatRoom.insert(sHeadResourceId,pChatRoom);
			CFilePathInfo::pointer pFilePathInfo = CFilePathInfo::create(CFilePathInfo::FILE_PAHT_HEAD,sHeadResourceFile,true);
			pFilePathInfo->m_pAccountInfo = pAccountInfo;
			theResourceFilePath.insert(sHeadResourceId,pFilePathInfo);

			if (!pChatRoom->IsEnterOk())
				pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
            pChatRoom->RecviveResource(sHeadResourceId,sHeadResourceFile);
            //char sTempFile[260];
			//sprintf(sTempFile,"%s.ebtemp",sHeadResourceFile);
			//pChatRoom->RecviveResource(sHeadResourceId,sTempFile);
		}
	}
}
void CUserManagerApp::ProcessMemberHeadInfo(const CEBMemberInfo::pointer& pEmployeeInfo,bool bUserHeadChange)
//void CUserManagerApp::ProcessMemberHeadInfo(const CEBMemberInfo::pointer& pEmployeeInfo,bool bUserHeadChange,bool bAddToCheckMemberHead)
{
	if (pEmployeeInfo.get()==NULL)
		return;
	const mycp::bigint sHeadResourceId = pEmployeeInfo->m_sHeadResourceId;
	const tstring sHeadCmServer(pEmployeeInfo->m_sHeadCmServer);
	if (sHeadResourceId>0 && !sHeadCmServer.empty())
	{
		bool bNeedReDownload = false;
#ifdef _QT_MAKE_
        const EBFileString sHeadResourceFile = QString("%1/%2").arg(m_sEbResourcePath).arg(sHeadResourceId);
        if (QFileInfo::exists(sHeadResourceFile))
#else
        char sHeadResourceFile[260];
        sprintf(sHeadResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),sHeadResourceId);
        if (::PathFileExistsA(sHeadResourceFile))
#endif
		{
            /// ??检查头像是否正确，后期删除这个检查
			if (pEmployeeInfo->m_sHeadMd5.empty())
			{
#ifdef _QT_MAKE_
                QImage pImage;
                if (!pImage.load(sHeadResourceFile) || pImage.width()<=0) {
                    bNeedReDownload = true;
                    QFile::remove(sHeadResourceFile);
                }else
#else
//#ifdef _MSC_VER
				USES_CONVERSION;
				Gdiplus::Image pImage(A2W(sHeadResourceFile));
				const bool bIsImage = (bool)(pImage.GetType()!= Gdiplus::ImageTypeUnknown);
				if (!bIsImage)
				{
					// 前面文件下载错误，重新下载
					bNeedReDownload = true;
					DeleteFileA(sHeadResourceFile);
				}else
#endif
				{
					pEmployeeInfo->m_sHeadResourceFile = sHeadResourceFile;
				}
			}else
			{
				// 判断MD5值
				mycp::bigint nFileSize = 0;
				tstring sFileMd5String;
				GetFileMd5(sHeadResourceFile,nFileSize,sFileMd5String);
				if (sFileMd5String==pEmployeeInfo->m_sHeadMd5)
				{
					pEmployeeInfo->m_sHeadResourceFile = sHeadResourceFile;
				}else
				{
					// MD5已经变了，删除后重新下载
					bNeedReDownload = true;
#ifdef _QT_MAKE_
                    QFile::remove(sHeadResourceFile);
#else
                    DeleteFileA(sHeadResourceFile);
#endif
				}
			}
		}
		
#ifdef _QT_MAKE_
        if (bUserHeadChange && !bNeedReDownload && QFileInfo::exists(sHeadResourceFile))
#else
        if (bUserHeadChange && !bNeedReDownload && ::PathFileExistsA(sHeadResourceFile))
#endif
		{
			const bool bIsOwnerMember = pEmployeeInfo->m_nMemberUserId==m_pMyAccountInfo->GetUserId()?true:false;
			if (this->m_callback)
				m_callback->onUserHeadChange(pEmployeeInfo.get(),bIsOwnerMember);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_USER_HEAD_CHANGE);
				pEvent->SetEventParameter((long)(bIsOwnerMember?1:0));
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_USER_HEAD_CHANGE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(),(LPARAM)(bIsOwnerMember?1:0));
#endif
			}
        //}else if (bNeedReDownload || !::PathFileExistsA(sHeadResourceFile))
#ifdef _QT_MAKE_
        }else if ((bNeedReDownload || !QFileInfo::exists(sHeadResourceFile)) && !theResourceFilePath.exist(sHeadResourceId))
#else
        }else if ((bNeedReDownload || !::PathFileExistsA(sHeadResourceFile)) && !theResourceFilePath.exist(sHeadResourceId))
#endif
		{
			// 资源文件不存在，请求下载
			//if (!theResourceFilePath.exist(sHeadResourceId))
			//{
				const tstring sCmServerKey = GetCmServerKey(sHeadCmServer);
				Cchatroom::pointer pChatRoom;
				if (!theChatRoomList.find(sCmServerKey,pChatRoom))
				{
					pChatRoom = Cchatroom::create();
					pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
					pChatRoom->Start(sHeadCmServer.c_str(), "",0,true,1);
					pChatRoom->SetCallBack(this);
					theChatRoomList.insert(sCmServerKey,pChatRoom);
				}
				char lpszKey[200];
				sprintf(lpszKey,"%s%lld",sCmServerKey.c_str(),sHeadResourceId);
				//sprintf(lpszKey,"%s%lld",sHeadCmServer.c_str(),sHeadResourceId);
				theChatRoomList.insert(lpszKey,pChatRoom,false);

				theResourceChatRoom.insert(sHeadResourceId,pChatRoom);
				CFilePathInfo::pointer pFilePathInfo = CFilePathInfo::create(CFilePathInfo::FILE_PAHT_HEAD,sHeadResourceFile,true);
				pFilePathInfo->m_pEmployeeInfo = pEmployeeInfo;
				pFilePathInfo->m_nBigData = bUserHeadChange?1:0;
				//pFilePathInfo->m_nBigData = pEmployeeInfo->m_sMemberCode;
				theResourceFilePath.insert(sHeadResourceId,pFilePathInfo);

				//const tstring sTempFile = sHeadResourceFile+".temp";
				//pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
				//pChatRoom->RecviveResource(sHeadResourceId,sTempFile);
				CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE);
				pProcessMsgInfo->m_pChatRoom = pChatRoom;
				pProcessMsgInfo->m_nBigInt1 = sHeadResourceId;
				//pProcessMsgInfo->m_nBigInt2 = bUserHeadChange?1:0;
				pProcessMsgInfo->m_sString1 = sHeadResourceFile;
				//pProcessMsgInfo->m_sString1.append(".ebtemp");
				m_pProcessMsgList.add(pProcessMsgInfo);
			//}else if (bUserHeadChange)
			//{
			//	CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_CHECK_MEMBER_HEAD);
			//	pProcessMsgInfo->m_tProcessTime = time(0)+3;
			//	pProcessMsgInfo->m_pEmployeeInfo = pEmployeeInfo;
			//	//pProcessMsgInfo->m_sString1 = sHeadResourceFile;
			//	m_pProcessMsgList.add(pProcessMsgInfo);
			//}
		}
	}
}
mycp::bigint CUserManagerApp::GetLocalGroupVer(mycp::bigint nGroupId) const
{
	mycp::bigint nLocalGroupVer = -1;
	//BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_mutexEBDatas));
	if (m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[128];
		sprintf(lpszBuffer,"SELECT ver FROM eb_dep_info_t WHERE group_id=%lld",nGroupId);
		int nCookie = 0;
		m_pEBDatas->select(lpszBuffer, nCookie);
		int nIndex = 0;
		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		if (pRecord.get()!=0)
		{
			nLocalGroupVer = pRecord->getVector()[0]->getBigIntValue();
		}
		m_pEBDatas->reset(nCookie);
	}
	return nLocalGroupVer;
}
void CUserManagerApp::SetLocalGroupEmpCount(mycp::bigint nGroupId,int nEmpCount) const
{
	//BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_mutexEBDatas));
	if (m_pEBDatas.get()!=NULL)
	{
		// 更新本地部门成员数量
		if (nEmpCount<0)
			nEmpCount = 0;
		char lpszBuffer[128];
		sprintf(lpszBuffer,"UPDATE eb_dep_info_t SET emp_count=%d WHERE group_id=%lld",nEmpCount,nGroupId);
		m_pEBDatas->execute(lpszBuffer);
	}
}
void CUserManagerApp::SetLocalGroupVer(mycp::bigint nGroupId,mycp::bigint nNewGroupVer) const
{
	//BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_mutexEBDatas));
	if (m_pEBDatas.get()!=NULL)
	{
		// 更新本地部门成员资料版本号；
		char lpszBuffer[128];
		sprintf(lpszBuffer,"UPDATE eb_dep_info_t SET ver=%lld WHERE group_id=%lld",nNewGroupVer,nGroupId);
		m_pEBDatas->execute(lpszBuffer);
	}
}

void CUserManagerApp::SaveGroupMemberInfo(const CEBGroupInfo::pointer& pDepartmentInfo)
{
	// * 保存群组版本号和判断删除本地成员
	if (pDepartmentInfo.get()==NULL)
		return;
	if (pDepartmentInfo->m_nServerGroupVer!=pDepartmentInfo->m_nGroupVer)
	{
		SetLocalGroupVer(pDepartmentInfo->m_sGroupCode,pDepartmentInfo->m_nServerGroupVer);
		pDepartmentInfo->m_nGroupVer = pDepartmentInfo->m_nServerGroupVer;
		SetLocalGroupEmpCount(pDepartmentInfo->m_sGroupCode,pDepartmentInfo->m_nEmpCount);
	}
	bool bDeleteLocalMemberInfo = false;
	{
		BoostReadLock rdLock(m_mutexEBDatas);
		if (m_pEBDatas.get()!=NULL)
		{
			// 这里处理删除本地成员
			char lpszBuffer[128];
			sprintf(lpszBuffer,"SELECT emp_uid FROM eb_emp_info_t WHERE group_id=%lld",pDepartmentInfo->m_sGroupCode);
			int nCookie = 0;
			m_pEBDatas->select(lpszBuffer, nCookie);
			int nIndex = 0;
			cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
			while (pRecord.get()!=0)
			{
				const mycp::bigint nLocalEmpUserId = pRecord->getVector()[0]->getBigIntValue();
				// *** 在这里判断删除本地无用成员数据
				if (!pDepartmentInfo->m_pMemberList.exist(nLocalEmpUserId))
				{
					bDeleteLocalMemberInfo = true;
					sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE group_id=%lld AND emp_uid=%lld",pDepartmentInfo->m_sGroupCode,nLocalEmpUserId);
					m_pEBDatas->execute(lpszBuffer);
				}
				pRecord = m_pEBDatas->next(nCookie);
			}
			m_pEBDatas->reset(nCookie);
		}
	}
	if (bDeleteLocalMemberInfo && pDepartmentInfo->m_nEmpCount!=pDepartmentInfo->m_pMemberList.size())
	{
		// 本地删除无用成员，处理一次数据，并发送一次部门事件
		pDepartmentInfo->m_nEmpCount = pDepartmentInfo->m_pMemberList.size();
		SetLocalGroupEmpCount(pDepartmentInfo->m_sGroupCode,pDepartmentInfo->m_nEmpCount);
		const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
		if (m_callback)
			m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
			pEvent->SetEventParameter((long)(bIsMyDepartment?1:0));
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
		}
	}
}
int CUserManagerApp::GetLocalGroupMemberSize(mycp::bigint nGroupId) const
{
	int nResult = -1;
	//BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_mutexEBDatas));
	if (m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[128];
		sprintf(lpszBuffer,"SELECT count(emp_uid) FROM eb_emp_info_t WHERE group_id=%lld",nGroupId);
		int nCookie = 0;
		m_pEBDatas->select(lpszBuffer, nCookie);
		int nIndex = 0;
		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		if (pRecord.get()!=0)
		{
			nResult = pRecord->getVector()[0]->getIntValue();
		}
		m_pEBDatas->reset(nCookie);
	}
	return nResult;
}

void CUserManagerApp::OnFUMEMEmpInfo(const CEBMemberInfo::pointer& pEmployeeInfo,bool bNewMemberInfo,mycp::bigint nGroupVer,bool bUserHeadChange)
{
	const mycp::bigint s_dep_code = pEmployeeInfo->m_sGroupCode;
	SaveLocalMemberInfo(pEmployeeInfo);
	if (nGroupVer>0)
	{
		// 单独加载成员，自动判断更新部门成员版本号
		UpdateLocalGroupVer(pEmployeeInfo->m_sGroupCode,nGroupVer);
	}

	//const tstring 
	ProcessMemberHeadInfo(pEmployeeInfo,bUserHeadChange);

	CEBGroupInfo::pointer pDepartmentInfo;
	if (!theDepartmentList.find(s_dep_code, pDepartmentInfo))
	{
		// 暂时找不到群组（部门）信息
		theTempMemberList.insert(s_dep_code,pEmployeeInfo);
		return ;
	}
	// 添加成员，需要发送一次群组资料事件；
	const bool bSendGroupInfoEvent = (bNewMemberInfo && (int)pDepartmentInfo->m_pMemberList.size()>=pDepartmentInfo->m_nEmpCount)?true:false;
	ProcessMemberInfo(pDepartmentInfo,pEmployeeInfo,bSendGroupInfoEvent);

	if ((int)pDepartmentInfo->m_pMemberList.size()>=pDepartmentInfo->m_nEmpCount)
	{
		theCheckGroupMemberList.remove(s_dep_code);
		theGroupMemberDataTime.remove(s_dep_code);

		// * 保存群组版本号和判断删除本地成员
		//if (s_dep_code==999004)
		//{
		//	int i=0;
		//}
		SaveGroupMemberInfo(pDepartmentInfo);
		if (bNewMemberInfo)
		{
			// 加载一次成员状态
			if (pDepartmentInfo->m_nMyEmpId==0)
				theNeedLoadOLSList.add(pDepartmentInfo->m_sGroupCode);
			else
				theNeedLoadOLSList.pushfront(pDepartmentInfo->m_sGroupCode);
		}
	}
}
void CUserManagerApp::OnFUMEMEmpInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const mycp::bigint s_emp_code = pSotpResponseInfo->m_pResponseList.getParameterValue("emp_code",(mycp::bigint)0);
	const mycp::bigint s_dep_code = pSotpResponseInfo->m_pResponseList.getParameterValue("dep_code",(mycp::bigint)0);
	const mycp::bigint sMemberUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("emp_uid",(mycp::bigint)0);
	const tstring s_emp_account = pSotpResponseInfo->m_pResponseList.getParameterValue("emp_account");
	const tstring s_username = pSotpResponseInfo->m_pResponseList.getParameterValue("username");
	const tstring s_job_title = pSotpResponseInfo->m_pResponseList.getParameterValue("job_title");
	const int n_job_position = pSotpResponseInfo->m_pResponseList.getParameterValue("job_position", (int)0);
	const tstring s_cell_phone = pSotpResponseInfo->m_pResponseList.getParameterValue("cell_phone");
	const tstring s_fax = pSotpResponseInfo->m_pResponseList.getParameterValue("fax");
	const tstring s_work_phone = pSotpResponseInfo->m_pResponseList.getParameterValue("work_phone");
	const tstring s_email = pSotpResponseInfo->m_pResponseList.getParameterValue("email");
	const tstring s_address = pSotpResponseInfo->m_pResponseList.getParameterValue("address");
	const tstring s_description = pSotpResponseInfo->m_pResponseList.getParameterValue("description");
	const int n_manager_level = pSotpResponseInfo->m_pResponseList.getParameterValue("manager_level", (int)0);
	const EB_GENDER_TYPE n_gender = (EB_GENDER_TYPE)pSotpResponseInfo->m_pResponseList.getParameterValue("gender", (int)EB_GENDER_UNKNOWN);
	const int n_birthday = pSotpResponseInfo->m_pResponseList.getParameterValue("birthday", (int)0);
	const EB_USER_LINE_STATE n_line_state = (EB_USER_LINE_STATE)pSotpResponseInfo->m_pResponseList.getParameterValue("state", (int)EB_LINE_STATE_UNKNOWN);
	const tstring s_um_server = pSotpResponseInfo->m_pResponseList.getParameterValue("um_server");
	const tstring s_um_app_name = pSotpResponseInfo->m_pResponseList.getParameterValue("um_app_name");
	const tstring sHeadCmServer = pSotpResponseInfo->m_pResponseList.getParameterValue("h_cm_server");
	const tstring sHeadMd5 = pSotpResponseInfo->m_pResponseList.getParameterValue("h_md5");
	const tstring sHeadCmHttpServer = pSotpResponseInfo->m_pResponseList.getParameterValue("h_cm_httpserver");	// **该参数是有的
	//const tstring sHeadCmAppName = pSotpResponseInfo->m_pResponseList.getParameterValue("h_cm_app_name");
	const mycp::bigint sHeadResourceId = pSotpResponseInfo->m_pResponseList.getParameterValue("h-r-id",(mycp::bigint)0);
	const mycp::bigint nCSId = pSotpResponseInfo->m_pResponseList.getParameterValue("cs_id",(mycp::bigint)0);
	const int nCSExt = pSotpResponseInfo->m_pResponseList.getParameterValue("cs_ext",(int)0);
	const int nDisplayIndex = pSotpResponseInfo->m_pResponseList.getParameterValue("display_index",(int)0);
	const int nForbidMinutes = pSotpResponseInfo->m_pResponseList.getParameterValue("forbid_minutes",(int)0);
	const int nEmpCount = pSotpResponseInfo->m_pResponseList.getParameterValue("emp-count", (int)-1);
	const int nEmpOffset = pSotpResponseInfo->m_pResponseList.getParameterValue("emp-offset", (int)-1);
	const mycp::bigint nGroupVer = pSotpResponseInfo->m_pResponseList.getParameterValue("group_ver",(mycp::bigint)-1);

	// fro search key
	cgcParameter::pointer pCallback;
	if (pSotpRequestInfo.get()!=NULL)
		pCallback = pSotpRequestInfo->m_pRequestList.getParameter("search-callback");
	if (pCallback.get()!=NULL && pCallback->getPointer()!=NULL)
	{
		if (s_emp_code==0 || sMemberUserId==0)
		{
			const tstring sSearchKey = pSotpRequestInfo->m_pRequestList.getParameterValue("search-key");
			if (!IsFullNumber(sSearchKey.c_str(),sSearchKey.size()) && (m_sSearchNullKey.empty() || sSearchKey.find(m_sSearchNullKey)==std::string::npos))
			{
				m_sSearchNullKey = sSearchKey;
			}
			return;
		}
		//const tstring sSearchkey = pSotpRequestInfo->m_pRequestList.getParameterValue("search-key");
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!theDepartmentList.find(s_dep_code, pDepartmentInfo))
		{
			pDepartmentInfo = CEBGroupInfo::create(0,s_dep_code);
		}
		const unsigned long nSearchParam = (unsigned long)pSotpRequestInfo->m_pRequestList.getParameterValue("search-param",(mycp::bigint)0);
		CEBSearchCallback* pSearchCallback = (CEBSearchCallback*)pCallback->getPointer();
		try
		{
			bool bNewMemberInfo = !pDepartmentInfo->m_pMemberList.exist(sMemberUserId);
			//bool bNewMemberInfo = false;
			CEBMemberInfo::pointer pEmployeeInfo;
			if (!theEmployeeList.find(s_emp_code, pEmployeeInfo))
			{
				//bNewMemberInfo = true;
				pEmployeeInfo = CEBMemberInfo::create(s_dep_code,sMemberUserId);
				pEmployeeInfo->m_nMemberData |= EB_MEMBER_DATA_SEARCH_INFO;
				theEmployeeList.insert(s_emp_code, pEmployeeInfo,false);
			}else if (bNewMemberInfo)
			{
				bNewMemberInfo = false;
			}

			pEmployeeInfo->m_sMemberAccount = s_emp_account;
			pEmployeeInfo->m_nMemberUserId = sMemberUserId;
			pEmployeeInfo->m_sMemberCode = s_emp_code;
			pEmployeeInfo->m_sUserName = s_username;
			pEmployeeInfo->m_sJobTitle = s_job_title;
			pEmployeeInfo->m_nJobPosition = n_job_position;
			pEmployeeInfo->m_sCellPhone = s_cell_phone;
			pEmployeeInfo->m_sFax = s_fax;
			pEmployeeInfo->m_sWorkPhone = s_work_phone;
			pEmployeeInfo->m_sEmail = s_email.empty()?s_emp_account:s_email;
			pEmployeeInfo->m_sAddress = s_address;
			pEmployeeInfo->m_sDescription = s_description;
			pEmployeeInfo->m_nManagerLevel = n_manager_level;
			pEmployeeInfo->m_nGender = n_gender;
			pEmployeeInfo->m_nBirthday = n_birthday;
			pEmployeeInfo->m_nLineState = n_line_state;
			pEmployeeInfo->m_sUmServer = s_um_server;
			//pEmployeeInfo->m_sUmAppName = s_um_app_name;
			pEmployeeInfo->m_sHeadResourceId = sHeadResourceId;
			pEmployeeInfo->m_nCSId = nCSId;
			pEmployeeInfo->m_nCSExt = nCSExt;
			pEmployeeInfo->m_nDisplayIndex = nDisplayIndex;
			pEmployeeInfo->m_nForbidMinutes = nForbidMinutes;
			if (bNewMemberInfo)
			{
				pDepartmentInfo->m_pMemberList.insert(sMemberUserId,pEmployeeInfo,false);
				pSearchCallback->onMemberInfo(pDepartmentInfo.get(), pEmployeeInfo.get(), nSearchParam);
			}
		}catch(std::exception&)
		{
		}catch(...)
		{}

		return;
	}
	if (s_emp_code==0 || nEmpCount>=0)
	{
		//const mycp::bigint sGroupCode = pSotpResponseInfo->m_pResponseList.getParameterValue("dep-code",(mycp::bigint)0);
		//theCheckGroupMemberList.remove(sGroupCode);
		////if (sGroupCode==999004)
		////{
		////	int i=0;
		////}

		// *** 下面会加载一次成员在线状态；
		//CEBGroupInfo::pointer pDepartmentInfo;
		//if (theDepartmentList.find(sGroupCode, pDepartmentInfo))
		//{
		//	if (pDepartmentInfo->m_pMemberList.size()>=nEmpCount)
		//	{
		//		SaveGroupMemberInfo(pDepartmentInfo);
		//		// 加载一次成员状态
		//		if (nEmpCount>0)
		//		{
		//			if (pDepartmentInfo->m_nMyEmpId==0)
		//				theNeedLoadOLSList.add(pDepartmentInfo->m_sGroupCode);
		//			else
		//				theNeedLoadOLSList.pushfront(pDepartmentInfo->m_sGroupCode);
		//		}
		//	}
		//}
		return;
	}

	bool bNewMemberInfo = false;
	CEBMemberInfo::pointer pEmployeeInfo;
	if (!theEmployeeList.find(s_emp_code, pEmployeeInfo))
	{
		bNewMemberInfo = true;
		pEmployeeInfo = CEBMemberInfo::create(s_dep_code,sMemberUserId);
		theEmployeeList.insert(s_emp_code, pEmployeeInfo);
	}
	pEmployeeInfo->m_sMemberAccount = s_emp_account;
	pEmployeeInfo->m_nMemberUserId = sMemberUserId;
	pEmployeeInfo->m_sMemberCode = s_emp_code;
	pEmployeeInfo->m_sUserName = s_username;
	pEmployeeInfo->m_sJobTitle = s_job_title;
	pEmployeeInfo->m_nJobPosition = n_job_position;
	pEmployeeInfo->m_sCellPhone = s_cell_phone;
	pEmployeeInfo->m_sFax = s_fax;
	pEmployeeInfo->m_sWorkPhone = s_work_phone;
	pEmployeeInfo->m_sEmail = s_email.empty()?s_emp_account:s_email;
	pEmployeeInfo->m_sAddress = s_address;
	pEmployeeInfo->m_sDescription = s_description;
	pEmployeeInfo->m_nManagerLevel = n_manager_level;
	pEmployeeInfo->m_nGender = n_gender;
	pEmployeeInfo->m_nBirthday = n_birthday;
	pEmployeeInfo->m_nLineState = n_line_state;
	pEmployeeInfo->m_sUmServer = s_um_server;
	//pEmployeeInfo->m_sUmAppName = s_um_app_name;
	bool bUserHeadChange = false;
	if (!bNewMemberInfo && pEmployeeInfo->m_sHeadResourceId>0 && pEmployeeInfo->m_sHeadResourceId!=sHeadResourceId)
	{
		bUserHeadChange = true;
		CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_DELETE_HEAD_RESFILE);
		pProcessMsgInfo->m_tProcessTime = time(0)+8;
		pProcessMsgInfo->m_pEmployeeInfo = pEmployeeInfo;
		pProcessMsgInfo->m_nBigInt1 = pEmployeeInfo->m_sHeadResourceId;
		m_pProcessMsgList.add(pProcessMsgInfo);
	}
	pEmployeeInfo->m_sHeadResourceId = sHeadResourceId;
	pEmployeeInfo->m_sHeadMd5 = sHeadMd5;
	pEmployeeInfo->m_sHeadCmServer = sHeadCmServer;
	pEmployeeInfo->m_sHeadHttpServer = sHeadCmHttpServer;
	pEmployeeInfo->m_nCSId = nCSId;
	pEmployeeInfo->m_nCSExt = nCSExt;
	pEmployeeInfo->m_nDisplayIndex = nDisplayIndex;
	pEmployeeInfo->m_nForbidMinutes = nForbidMinutes;
	if ((pEmployeeInfo->m_nMemberData&EB_MEMBER_DATA_SEARCH_INFO)==EB_MEMBER_DATA_SEARCH_INFO)
	{
		bNewMemberInfo = true;
		pEmployeeInfo->m_nMemberData &= ~EB_MEMBER_DATA_SEARCH_INFO;
	}

	if (theCheckGroupMemberList.exist(s_dep_code))
		theGroupMemberDataTime.insert(s_dep_code,time(0));

	//static int theMemberSize = 0;
	//theMemberSize++;
	CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_F_MEMBER_INFO);
	pProcessMsgInfo->m_pEmployeeInfo = pEmployeeInfo;
	pProcessMsgInfo->m_nReturnVer = nGroupVer;
	pProcessMsgInfo->m_nBigInt1 = bUserHeadChange?1:0;
	m_pProcessMsgList.add(pProcessMsgInfo);
	////SaveLocalMemberInfo(pEmployeeInfo);
	//if (nGroupVer>0)
	//{
	//	// 单独加载成员，自动判断更新部门成员版本号
	//	UpdateLocalGroupVer(pEmployeeInfo->m_sGroupCode,nGroupVer);
	//}

	////const tstring 
	//ProcessMemberHeadInfo(pEmployeeInfo);

	//CEBGroupInfo::pointer pDepartmentInfo;
	//if (!theDepartmentList.find(s_dep_code, pDepartmentInfo))
	//{
	//	// 暂时找不到群组（部门）信息
	//	theTempMemberList.insert(s_dep_code,pEmployeeInfo);
	//	return ;
	//}
	//// 添加成员，需要发送一次群组资料事件；
	//const bool bSendGroupInfoEvent = bNewMemberInfo&&pDepartmentInfo->m_pMemberList.size()>=pDepartmentInfo->m_nEmpCount;
	//ProcessMemberInfo(pDepartmentInfo,pEmployeeInfo,bSendGroupInfoEvent);

	//if (pDepartmentInfo->m_pMemberList.size()>=pDepartmentInfo->m_nEmpCount)
	//{
	//	theCheckGroupMemberList.remove(s_dep_code);
	//	theGroupMemberDataTime.remove(s_dep_code);

	//	// * 保存群组版本号和判断删除本地成员
	//	//if (s_dep_code==999004)
	//	//{
	//	//	int i=0;
	//	//}
	//	SaveGroupMemberInfo(pDepartmentInfo);
	//	if (bNewMemberInfo)
	//	{
	//		// 加载一次成员状态
	//		if (pDepartmentInfo->m_nMyEmpId==0)
	//			theNeedLoadOLSList.add(pDepartmentInfo->m_sGroupCode);
	//		else
	//			theNeedLoadOLSList.pushfront(pDepartmentInfo->m_sGroupCode);
	//	}
	//}
}

void CUserManagerApp::OnUGEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return ;
	const mycp::bigint nEditUGId = pSotpRequestInfo->m_pRequestList.getParameterValue("ug-id",(mycp::bigint)0);
	const bool bNewUGInfo = pSotpRequestInfo->m_pRequestList.getParameterValue("new-uginfo", false);
	const mycp::bigint nUGId = pSotpResponseInfo->m_pResponseList.getParameterValue("ugid",(mycp::bigint)0);
	const mycp::bigint nNewContactInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("new_contact_info_ver",(mycp::bigint)0);
	const int nResultValue = pSotpResponseInfo->GetResultValue();

	CEBUGInfo::pointer pEditUGInfo;
	if (!theEitUGInfoList.find(nEditUGId, pEditUGInfo, true))
	{
		return;
	}

	if (nResultValue==EB_STATE_OK && nUGId>0)
	{
		CEBUGInfo::pointer pUGInfo;
		if (bNewUGInfo)
		{
			// 使用新内容
			pEditUGInfo->m_nUGId = nUGId;
			pUGInfo = pEditUGInfo;
			theUGInfoList.insert(nUGId, pUGInfo);
		}else if (!theUGInfoList.find(nUGId, pUGInfo))
		{
			return ;
		}else
		{
			// 修改新内容；
			pUGInfo->operator = (pEditUGInfo.get());
		}

		if (nNewContactInfoVer==m_pMyAccountInfo->GetContactInfoVer()+1)
		{
			m_pMyAccountInfo->SetContactInfoVer(nNewContactInfoVer);
			SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),nNewContactInfoVer);
		}

		if (m_callback)
			m_callback->onUGInfo(pUGInfo.get());
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_UGInfo * pEvent = new EB_UGInfo(pUGInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_UG_INFO);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_UG_INFO, (WPARAM)(const EB_UGInfo*)pUGInfo.get(), 0);
#endif
		}
	}else
	{
		if (m_callback)
			m_callback->onStateCode((EB_STATE_CODE)nResultValue,0);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_STATE_CODE);
//			pEvent->SetQEventType((QEvent::Type)EB_WM_STATE_CODE);
			pEvent->SetEventParameter((long)nResultValue);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::PostMessage(m_pHwnd, EB_WM_STATE_CODE, (WPARAM)nResultValue, 0);
#endif
		}
	}
}
void CUserManagerApp::DeleteUG(mycp::bigint nUGId)
{
	CEBUGInfo::pointer pUGInfo;
	if (!theUGInfoList.find(nUGId, pUGInfo))
	{
		return ;
	}
	if (m_pEBDatas.get()!=NULL)
	{
		// 先设置删除标识
		char lpszBuffer[128];
		sprintf(lpszBuffer,"UPDATE eb_contact_info_t SET ug_id=0 WHERE ug_id=%lld",nUGId);
		m_pEBDatas->execute(lpszBuffer);
	}

	if (m_callback)
		m_callback->onUGDelete(pUGInfo.get());
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		EB_UGInfo * pEvent = new EB_UGInfo(pUGInfo.get());
		pEvent->SetQEventType((QEvent::Type)EB_WM_UG_DELETE);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::SendMessage(m_pHwnd, EB_WM_UG_DELETE, (WPARAM)(const EB_UGInfo*)pUGInfo.get(), 0);
#endif
	}

	{
		// 清除所有联系人UGID为0
		BoostReadLock lockContact(theContactList1.mutex());
		CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList1.begin();
		for (; pIterContact!=theContactList1.end(); pIterContact++)
		{
			CEBContactInfo::pointer pContactInfo = pIterContact->second;
			if (pContactInfo->m_nUGId==nUGId)
			{
				pContactInfo->m_nUGId = 0;
				pContactInfo->m_sGroupName = "";
			}
		}
	}
	theUGInfoList.remove(nUGId);
}
void CUserManagerApp::OnUGDeleteResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return ;
	const mycp::bigint nUGId = pSotpRequestInfo->m_pRequestList.getParameterValue("ug-id",(mycp::bigint)0);
	const mycp::bigint nNewContactInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("new_contact_info_ver",(mycp::bigint)0);
	int nResultValue = pSotpResponseInfo->GetResultValue();
	if (nResultValue==EB_STATE_OK && nUGId>0)
	{
		DeleteUG(nUGId);
		//if (nNewContactInfoVer==m_pMyAccountInfo->GetContactInfoVer()+1)
		//{
		//	m_pMyAccountInfo->SetContactInfoVer(nNewContactInfoVer);
		//	SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),nNewContactInfoVer);
		//}
	}else
	{
		//CEBContactInfo::pointer pPopContactInfo;
		//if (!theContactList.find(nContactId, pPopContactInfo))
		//{
		//	return ;
		//}
		// ??
		//if (m_pHwnd!=NULL)
		//{
		//	::PostMessage(m_pHwnd, POP_WM_CONTACT_DELETE_RESPONSE, (WPARAM)pPopContactInfo.get(), 0);
		//}
	}
}
void CUserManagerApp::LoadLocalContactData(void)
{
	BoostReadLock rdLock(m_mutexEBDatas);
	// 加载本地联系人数据
	if (this->m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[512];
		int nCookie = 0;
		//sprintf(lpszBuffer,"SELECT ug_id,group_name FROM eb_user_group_t");
		//m_pEBDatas->select(lpszBuffer, nCookie);
		//cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		//while (pRecord.get()!=0)
		//{
		//	const mycp::bigint nUGId = pRecord->getVector()[0]->getBigIntValue();
		//	const tstring sGroupName = pRecord->getVector()[1]->getStrValue();
		//	CEBUGInfo::pointer pUGInfo;
		//	if (!theUGInfoList.find(nUGId, pUGInfo))
		//	{
		//		pUGInfo = CEBUGInfo::create(nUGId,sGroupName);
		//		theUGInfoList.insert(nUGId, pUGInfo);
		//	}else
		//	{
		//		pUGInfo->m_sGroupName = sGroupName;
		//	}
		//}
		//m_pEBDatas->reset(nCookie);

		sprintf(lpszBuffer,"SELECT con_id,con_uid,ug_id,con_account,name,phone,email,address,description,company,job_title,tel,fax,url,ntype,head_ver,head_rid,head_cm_server,head_http_server,head_md5 FROM eb_contact_info_t");
		m_pEBDatas->select(lpszBuffer, nCookie);
		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		while (pRecord.get()!=0)
		{
			const mycp::bigint nContactId = pRecord->getVector()[0]->getBigIntValue();
			const mycp::bigint nContactUserId = pRecord->getVector()[1]->getBigIntValue();
			const mycp::bigint nUGId = pRecord->getVector()[2]->getBigIntValue();
			const tstring sContact = pRecord->getVector()[3]->getStrValue();
			const tstring sName = pRecord->getVector()[4]->getStrValue();
			const tstring sPhone = pRecord->getVector()[5]->getStrValue();
			const tstring sEmail = pRecord->getVector()[6]->getStrValue();
			const tstring sAddress = pRecord->getVector()[7]->getStrValue();
			const tstring sDescription = pRecord->getVector()[8]->getStrValue();
			const tstring sCompany = pRecord->getVector()[9]->getStrValue();
			const tstring sJobTitle = pRecord->getVector()[10]->getStrValue();
			const tstring sTel = pRecord->getVector()[11]->getStrValue();
			const tstring sFax = pRecord->getVector()[12]->getStrValue();
			const tstring sUrl = pRecord->getVector()[13]->getStrValue();
			const int nContactType = pRecord->getVector()[14]->getIntValue();
			const mycp::bigint nHeadVer = pRecord->getVector()[15]->getBigIntValue();
			const mycp::bigint nHeadResId = pRecord->getVector()[16]->getBigIntValue();
			const tstring sHeadCmServer = pRecord->getVector()[17]->getStrValue();
			const tstring sHeadHttpServer = pRecord->getVector()[18]->getStrValue();
			const tstring sHeadMd5 = pRecord->getVector()[19]->getStrValue();
			pRecord = m_pEBDatas->next(nCookie);

			CEBContactInfo::pointer pPopContactInfo;
			if (!theContactList1.find(nContactId, pPopContactInfo))
			{
				pPopContactInfo = CEBContactInfo::create(nContactId);
				theContactList1.insert(nContactId, pPopContactInfo);

				if (nContactUserId>0)
					theContactList2.insert(nContactUserId, pPopContactInfo);
			}
			//pPopContactInfo->m_sGroup = sGroup;
			pPopContactInfo->m_nContactUserId = nContactUserId;

			CEBUGInfo::pointer pUGInfo;
			if (theUGInfoList.find(nUGId,pUGInfo))
			{
				pPopContactInfo->m_nUGId = nUGId;
                pPopContactInfo->m_sGroupName = pUGInfo->m_sGroupName;
            }
            else {
                pPopContactInfo->m_nUGId = nUGId;
            }
			pPopContactInfo->m_sName = sName;
			pPopContactInfo->m_sPhone = sPhone;
			pPopContactInfo->m_sEmail = sEmail;
			pPopContactInfo->m_sAddress = sAddress;
			pPopContactInfo->m_sDescription = sDescription;
			pPopContactInfo->m_sCompany = sCompany;
			pPopContactInfo->m_sJobTitle = sJobTitle;
			pPopContactInfo->m_sTel = sTel;
			pPopContactInfo->m_sFax = sFax;
			pPopContactInfo->m_sUrl = sUrl;
			pPopContactInfo->m_nContactType = nContactType;
			pPopContactInfo->m_sContact = sContact;
			//pPopContactInfo->m_nLineState = nLineState;

			pPopContactInfo->m_nHeadVer = nHeadVer;
			pPopContactInfo->m_sHeadResourceId = nHeadResId;
			pPopContactInfo->m_sHeadCmServer = sHeadCmServer;
			pPopContactInfo->m_sHeadHttpServer = sHeadHttpServer;
			pPopContactInfo->m_sHeadMd5 = sHeadMd5;
			ProcessContactHeadInfo(pPopContactInfo,false);
			if (m_callback)
				m_callback->onContactInfo(pPopContactInfo.get());
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_ContactInfo * pEvent = new EB_ContactInfo(pPopContactInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_INFO);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_CONTACT_INFO, (WPARAM)(const EB_ContactInfo*)pPopContactInfo.get(), 0);
#endif
			}
		}
		m_pEBDatas->reset(nCookie);
	}
	m_bExistLocalContact = theContactList1.empty()?false:true;
	if (m_bExistLocalContact && m_pUserManager.get()!=NULL)
	{
		// 加载联系人在线状态
		m_pUserManager->SendUMSMLoad(0,0,0,0,"",0,0,"",1);
		// 加载联系人头像版本
		m_pUserManager->SendUMSMLoad2(EB_UM_LOAD2_TYPE_CONTACT_HEAD_VER);
	}
}
void CUserManagerApp::ProcLoadContactData(void)
{
	// 需要判断本地联系人版本
	if (GetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId())==m_pMyAccountInfo->GetContactInfoVer())
	{
		LoadLocalContactData();
	}else
	{
		m_bExistLocalContact = false;
	}
	if (m_bExistLocalContact && GetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId())==-1)
	{
		SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetContactInfoVer());
	}else if (!m_bExistLocalContact)
	{
		if (m_pEBDatas.get()!=NULL)
		{
			// 先设置删除标识
			m_pEBDatas->execute("UPDATE eb_contact_info_t SET flag=1");
		}
		m_pUserManager->SendUMCMQuery(0,0,0);
	}
}
void CUserManagerApp::OnUGInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const int nCount = pSotpResponseInfo->m_pResponseList.getParameterValue("ug-count",(int)-1);
	if (nCount>=0)
	{
		// 分组加载完成，要加载一次
		m_nLoadUGCount = nCount;
		if ((int)theUGInfoList.size()>=m_nLoadUGCount)
		{
			m_nLoadUGCount = -1;
			ProcLoadContactData();
		}
		return ;
	}
	const int nOffset = pSotpResponseInfo->m_pResponseList.getParameterValue("ug-offset",(int)-1);
	const mycp::bigint nUGId = pSotpResponseInfo->m_pResponseList.getParameterValue("ugid",(mycp::bigint)0);
	if (nUGId==0)
		return;
	const tstring sGroupName = pSotpResponseInfo->m_pResponseList.getParameterValue("groupname");

	CEBUGInfo::pointer pUGInfo;
	if (!theUGInfoList.find(nUGId, pUGInfo))
	{
		pUGInfo = CEBUGInfo::create(nUGId,sGroupName);
		theUGInfoList.insert(nUGId, pUGInfo);
	}else
	{
		pUGInfo->m_sGroupName = sGroupName;
	}
	if ((int)theUGInfoList.size()>=m_nLoadUGCount)
	{
		m_nLoadUGCount = -1;
		ProcLoadContactData();
	}

	if (m_callback)
		m_callback->onUGInfo(pUGInfo.get());
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		EB_UGInfo * pEvent = new EB_UGInfo(pUGInfo.get());
		pEvent->SetQEventType((QEvent::Type)EB_WM_UG_INFO);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::PostMessage(m_pHwnd, EB_WM_UG_INFO, (WPARAM)(const EB_UGInfo*)pUGInfo.get(), 0);
#endif
	}
}

void CUserManagerApp::OnUMCMEdit(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return ;
	const mycp::bigint nEditContactId = pSotpRequestInfo->m_pRequestList.getParameterValue("con-id",(mycp::bigint)0);
	const bool bNewContact = pSotpRequestInfo->m_pRequestList.getParameterValue("new-contact", false);
	const mycp::bigint nContactId = pSotpResponseInfo->m_pResponseList.getParameterValue("con_id",(mycp::bigint)0);
	const tstring sAddContactAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("add_contact");
	//const mycp::bigint nContactUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("contact_uid",(mycp::bigint)0);
	const mycp::bigint nNewContactInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("new_contact_info_ver",(mycp::bigint)0);
	int nResultValue = pSotpResponseInfo->GetResultValue();

	CEBContactInfo::pointer pPopContactEditInfo;
	if (!theEditContactList.find(nEditContactId, pPopContactEditInfo, true))
	{
		return ;
	}

	if (nResultValue==EB_STATE_OK && nContactId>0)
	{
		CEBContactInfo::pointer pPopContactInfo;
		if (bNewContact)
		{
			// 使用新内容
			pPopContactEditInfo->m_nContactId = nContactId;
			pPopContactEditInfo->m_sContact = sAddContactAccount;
			pPopContactInfo = pPopContactEditInfo;
			theContactList1.insert(nContactId,pPopContactInfo);
		}else if (!theContactList1.find(nContactId, pPopContactInfo))
		{
			return ;
		}else
		{
			// 修改新内容；
			pPopContactInfo->operator = (pPopContactEditInfo.get());
		}
		if (pPopContactEditInfo->m_nContactUserId>0)
			theContactList2.insert(pPopContactEditInfo->m_nContactUserId,pPopContactInfo,false);

		if (pPopContactInfo->m_nUGId>0 && pPopContactInfo->m_sGroupName.empty())
		{
			CEBUGInfo::pointer pUGInfo;
			if (theUGInfoList.find(pPopContactInfo->m_nUGId,pUGInfo))
			{
				pPopContactInfo->m_sGroupName = pUGInfo->m_sGroupName;
			}
		}
		//SaveLocalContactInfo(pPopContactInfo);
		//if (nNewContactInfoVer==m_pMyAccountInfo->GetContactInfoVer()+1)
		//{
		//	m_pMyAccountInfo->SetContactInfoVer(nNewContactInfoVer);
		//	SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),nNewContactInfoVer);
		//}
		if (m_callback)
			m_callback->onContactInfo(pPopContactInfo.get());
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_ContactInfo * pEvent = new EB_ContactInfo(pPopContactInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_INFO);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_CONTACT_INFO, (WPARAM)(const EB_ContactInfo*)pPopContactInfo.get(), 0);
#endif
		}

		//if (m_pHwnd!=NULL)
		//{
		//	if (bNewContact)
		//		::PostMessage(m_pHwnd, POP_WM_CONTACT_INFO, (WPARAM)pPopContactInfo.get(), 0);
		//	else
		//		::PostMessage(m_pHwnd, POP_WM_CONTACT_EDIT_RESPONSE, (WPARAM)pPopContactInfo.get(), 1);
		//}
		//if (pPopContactEditInfo.get() != NULL && pPopContactEditInfo.get() != pPopContactInfo.get())
		//{
		//	theContactList.remove(sContactAccount);
		//	theContactList.insert(sContactAccount, pPopContactEditInfo);
		//}
	}else
	{
		if (m_callback)
			m_callback->onStateCode((EB_STATE_CODE)nResultValue,0);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
            EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_STATE_CODE);
//			pEvent->SetQEventType((QEvent::Type)EB_WM_STATE_CODE);
			pEvent->SetEventParameter((long)nResultValue);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::PostMessage(m_pHwnd, EB_WM_STATE_CODE, (WPARAM)nResultValue, 0);
#endif
		}
	}
}
void CUserManagerApp::OnUMCMDelete(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return ;
	const mycp::bigint nContactId = pSotpRequestInfo->m_pRequestList.getParameterValue("con-id",(mycp::bigint)0);
	const mycp::bigint nNewContactInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("new_contact_info_ver",(mycp::bigint)0);
	int nResultValue = pSotpResponseInfo->GetResultValue();
	// ** nResultValue == EB_STATE_OK 统一在 EB_MSG_DELETE_CONTACT 处理；（V1.22版本，1.23版本删除以下代码）
	if (nResultValue==EB_STATE_OK && nContactId>0)
	{
		//CEBContactInfo::pointer pPopContactInfo;
		//if (!theContactList1.find(nContactId, pPopContactInfo, true))
		//{
		//	return ;
		//}
		//if (pPopContactInfo->m_nContactUserId>0)
		//	theContactList2.remove(pPopContactInfo->m_nContactUserId);

		//// *** 删除本地无用联系人数据
		//if (m_pEBDatas.get()!=NULL)
		//{
		//	// 删除部门资料
		//	char lpszBuffer[128];
		//	sprintf(lpszBuffer,"DELETE FROM eb_contact_info_t WHERE con_id=%lld",nContactId);
		//	m_pEBDatas->execute(lpszBuffer);
		//}
		//if (nNewContactInfoVer==m_pMyAccountInfo->GetContactInfoVer()+1)
		//{
		//	m_pMyAccountInfo->SetContactInfoVer(nNewContactInfoVer);
		//	SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),nNewContactInfoVer);
		//}

		//if (m_callback)
		//	m_callback->onContactDelete(pPopContactInfo.get());
		//if (m_pHwnd!=NULL)
		//	::SendMessage(m_pHwnd, EB_WM_CONTACT_DELETE, (WPARAM)(const EB_ContactInfo*)pPopContactInfo.get(), 0);
		////theContactList.remove(nContactId);
	}else
	{
		//CEBContactInfo::pointer pPopContactInfo;
		//if (!theContactList.find(nContactId, pPopContactInfo))
		//{
		//	return ;
		//}
		// ??
		//if (m_pHwnd!=NULL)
		//{
		//	::PostMessage(m_pHwnd, POP_WM_CONTACT_DELETE_RESPONSE, (WPARAM)pPopContactInfo.get(), 0);
		//}
	}
}
void CUserManagerApp::OnUMCMQuery(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const int nCount = pSotpResponseInfo->m_pResponseList.getParameterValue("ab-count",(int)-1);
	const mycp::bigint nMyContactInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("my_contact_info_ver",(mycp::bigint)-1);
	if (nCount>=0)
	{
		m_nLoadContactCount = nCount;
		if (theContactList1.size()==m_nLoadContactCount)
		{
			m_nLoadContactCount = -1;
			const mycp::bigint nLocalContactInfoVer = GetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId());
			if (nLocalContactInfoVer==-1 || nLocalContactInfoVer!=m_pMyAccountInfo->GetContactInfoVer())
			{
				SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetContactInfoVer());
			}
			if (m_pEBDatas.get()!=NULL)
			{
				// 删除无用未本地数据
				m_pEBDatas->execute("DELETE FROM eb_contact_info_t WHERE flag=1");
			}
		}
		return ;
	}
	const int nOffset = pSotpResponseInfo->m_pResponseList.getParameterValue("ab-offset",(int)-1);
	//const tstring sContact = pSotpResponseInfo->m_pResponseList.getParameterValue("contact");
	//if (sContact.empty())
	//	return ;
	const mycp::bigint nContactId = pSotpResponseInfo->m_pResponseList.getParameterValue("con_id",(mycp::bigint)0);
	if (nContactId==0) return;
	const mycp::bigint nContactUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("con_uid",(mycp::bigint)0);
	const int nLoadFlag = pSotpResponseInfo->m_pResponseList.getParameterValue("load_flag",(int)0);
	const bool bLoadOnlyHeadInfo = (nLoadFlag&0x4)==0x4?true:false;
	const mycp::bigint nUGId = pSotpResponseInfo->m_pResponseList.getParameterValue("ugid",(mycp::bigint)0);
	//const tstring sGroup = pSotpResponseInfo->m_pResponseList.getParameterValue("groupname");
	const tstring sName = pSotpResponseInfo->m_pResponseList.getParameterValue("name");
	const tstring sPhone = pSotpResponseInfo->m_pResponseList.getParameterValue("phone");
	const tstring sEmail = pSotpResponseInfo->m_pResponseList.getParameterValue("email");
	const tstring sAddress = pSotpResponseInfo->m_pResponseList.getParameterValue("address");
	const tstring sDescription = pSotpResponseInfo->m_pResponseList.getParameterValue("description");
	const tstring sCompany = pSotpResponseInfo->m_pResponseList.getParameterValue("company");
	const tstring sJobTitle = pSotpResponseInfo->m_pResponseList.getParameterValue("job_title");
	const tstring sTel = pSotpResponseInfo->m_pResponseList.getParameterValue("tel");
	const tstring sFax = pSotpResponseInfo->m_pResponseList.getParameterValue("fax");
	const tstring sUrl = pSotpResponseInfo->m_pResponseList.getParameterValue("url");
	const int nContactType = pSotpResponseInfo->m_pResponseList.getParameterValue("type",(int)0);
	const EB_USER_LINE_STATE nLineState = (EB_USER_LINE_STATE)pSotpResponseInfo->m_pResponseList.getParameterValue("state",(int)EB_LINE_STATE_UNKNOWN);
	const tstring sContact = pSotpResponseInfo->m_pResponseList.getParameterValue("contact");
	const bool bAcceptContact = pSotpRequestInfo.get()==NULL?false:pSotpRequestInfo->m_pRequestList.getParameterValue("accept-contact",false);
	const mycp::bigint nHeadVer = pSotpResponseInfo->m_pResponseList.getParameterValue("head-ver", (mycp::bigint)0);
	const mycp::bigint nHeadResId = pSotpResponseInfo->m_pResponseList.getParameterValue("head-rid", (mycp::bigint)0);
	const tstring sHeadCmServer(pSotpResponseInfo->m_pResponseList.getParameterValue("head-cm-server", ""));
	const tstring sHeadHttpServer(pSotpResponseInfo->m_pResponseList.getParameterValue("head-http-server", ""));
	const tstring sHeadMd5(pSotpResponseInfo->m_pResponseList.getParameterValue("head-md5", ""));

	CEBContactInfo::pointer pPopContactInfo;
	if (!theContactList1.find(nContactId, pPopContactInfo))
	{
		pPopContactInfo = CEBContactInfo::create(nContactId);
		theContactList1.insert(nContactId, pPopContactInfo);
	}
	//pPopContactInfo->m_sGroup = sGroup;
	pPopContactInfo->m_nContactUserId = nContactUserId;
	if (pPopContactInfo->m_nContactUserId>0)
		theContactList2.insert(pPopContactInfo->m_nContactUserId, pPopContactInfo,false);

	if (!bLoadOnlyHeadInfo)
	{
		CEBUGInfo::pointer pUGInfo;
		if (theUGInfoList.find(nUGId,pUGInfo))
		{
			pPopContactInfo->m_nUGId = nUGId;
			pPopContactInfo->m_sGroupName = pUGInfo->m_sGroupName;
		}
        else {
            pPopContactInfo->m_nUGId = nUGId;
        }
		pPopContactInfo->m_sName = sName;
		pPopContactInfo->m_sPhone = sPhone;
		pPopContactInfo->m_sEmail = sEmail;
		pPopContactInfo->m_sAddress = sAddress;
		pPopContactInfo->m_sDescription = sDescription;
		pPopContactInfo->m_sCompany = sCompany;
		pPopContactInfo->m_sJobTitle = sJobTitle;
		pPopContactInfo->m_sTel = sTel;
		pPopContactInfo->m_sFax = sFax;
		pPopContactInfo->m_sUrl = sUrl;
		pPopContactInfo->m_nContactType = nContactType;
		pPopContactInfo->m_sContact = sContact;
		pPopContactInfo->m_nLineState = nLineState;
	}
	pPopContactInfo->m_nHeadVer = nHeadVer;
	pPopContactInfo->m_sHeadResourceId = nHeadResId;
	pPopContactInfo->m_sHeadCmServer = sHeadCmServer;
	pPopContactInfo->m_sHeadHttpServer = sHeadHttpServer;
	pPopContactInfo->m_sHeadMd5 = sHeadMd5;
	ProcessContactHeadInfo(pPopContactInfo,false);
	SaveLocalContactInfo(pPopContactInfo);
	if (!m_bExistLocalContact)
		m_bExistLocalContact = true;

	if (!bLoadOnlyHeadInfo)
	{
		if (m_callback)
			m_callback->onContactInfo(pPopContactInfo.get());
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_ContactInfo * pEvent = new EB_ContactInfo(pPopContactInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_INFO);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::PostMessage(m_pHwnd, EB_WM_CONTACT_INFO, (WPARAM)(const EB_ContactInfo*)pPopContactInfo.get(), 0);
#endif
		}
	}
	if (nMyContactInfoVer>0 && nMyContactInfoVer==m_pMyAccountInfo->GetContactInfoVer()+1)
	{
		m_pMyAccountInfo->SetContactInfoVer(nMyContactInfoVer);
		SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),nMyContactInfoVer);
	}
	if (theContactList1.size()==m_nLoadContactCount)
	{
		m_nLoadContactCount = -1;
		const mycp::bigint nLocalContactInfoVer = GetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId());
		if (nLocalContactInfoVer==-1 || nLocalContactInfoVer!=m_pMyAccountInfo->GetContactInfoVer())
		{
			SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetContactInfoVer());
		}
		if (m_pEBDatas.get()!=NULL)
		{
			// 删除无用未本地数据
			m_pEBDatas->execute("DELETE FROM eb_contact_info_t WHERE flag=1");
		}
	}
	if (bAcceptContact && (m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)
	{
		// 好友验证模式
		if (m_callback)
			m_callback->onAcceptAddContact(pPopContactInfo.get());
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_ContactInfo * pEvent = new EB_ContactInfo(pPopContactInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_ACCEPT_ADDCONTACT);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::PostMessage(m_pHwnd, EB_WM_ACCEPT_ADDCONTACT, (WPARAM)(const EB_ContactInfo*)pPopContactInfo.get(), (LPARAM)0);
#endif
		}
	}
}
void CUserManagerApp::OnFUMIUHangup(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const mycp::bigint nFromUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	const int nHangup = pSotpResponseInfo->m_pResponseList.getParameterValue("hangup",(int)0);
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId,pCallInfo))
	{
		if (nHangup == 1 && pCallInfo->m_sGroupCode==0)
		{
			CEBAccountInfo::pointer pFromAccountInfo;
			if (!pCallInfo->m_pCallUserList.find(nFromUserId,pFromAccountInfo))
			{
				return;
			}
			if (pFromAccountInfo->GetLineState()==EB_LINE_STATE_UNKNOWN)
			{
				// 未进入，或者已经挂机一次；
				return;
			}
			pFromAccountInfo->SetLineState(EB_LINE_STATE_UNKNOWN);
			EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,"",sCallId,pCallInfo->m_sGroupCode);
			if (m_callback)
				m_callback->onCallHangup(*pEbCallInfo,false);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_HANGUP);
                QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
				::SendMessage(m_pHwnd, EB_WM_CALL_HANGUP, (WPARAM)pEbCallInfo, 0);
				delete pEbCallInfo;
#endif
			}
			else {
				delete pEbCallInfo;
			}
			// 判断
			if (pCallInfo->m_nCallState==EB_CALL_STATE_CONNECTED)	// 本端在线，没有退出窗口
			{
				pCallInfo->m_nCallState = EB_CALL_STATE_HANGUP;
				//ExitChat(sCallId.c_str(),nFromUserId.c_str());
			}else
			{
				ClearCallInfo(sCallId,true,false);
			}
		}else
		{
			// 用户退出
			CCrAccountInfo pCrAccountInfo(0,pCallInfo->GetCallId(),pCallInfo->m_sChatId,(EB_STATE_CODE)pSotpResponseInfo->GetResultValue());
			pCrAccountInfo.m_sAccount = nFromUserId;
			OnUserExitRoom(pCrAccountInfo,(pCallInfo->m_sGroupCode==0&&nHangup==1)?true:false);
			//OnUserExitRoom(pCrAccountInfo,true);
		}
	}
}

void CUserManagerApp::OnUMIUHangup(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == 0) return ;
	if (m_pMyAccountInfo.get() == 0) return ;
	//const tstring sFromAccount = pSotpRequestInfo->m_pRequestList.getParameterValue("from");
	const mycp::bigint sCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("call-id",(mycp::bigint)0);
	const bool bHangup = pSotpRequestInfo->m_pRequestList.getParameterValue("hangup",false);
	CEBCallInfo::pointer pCallInfo;
	if (theCallInfoList.find(sCallId, pCallInfo))
	{
		// ??这里是否用
		//tstring sFromAccount = m_pMyAccountInfo->GetAccount();
		if (bHangup)
		{
			mycp::bigint nFromUserId = 0;
			tstring sFromAccount;
			{
				BoostReadLock lock(pCallInfo->m_pCallUserList.mutex());
				CLockMap<mycp::bigint, CEBAccountInfo::pointer>::iterator pIter;
				for (pIter=pCallInfo->m_pCallUserList.begin(); pIter!=pCallInfo->m_pCallUserList.end(); pIter++)
				{
					const CEBAccountInfo::pointer& pAccountInfo = pIter->second;
					if (pAccountInfo->GetUserId() != m_pMyAccountInfo->GetUserId())
					{
						sFromAccount = pAccountInfo->GetAccount();
						nFromUserId = pAccountInfo->GetUserId();
						//if (bHangup)
						//	theAcceptUser.remove(nFromUserId);
						//break;
					}
				}
			}

			EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,sFromAccount,sCallId,pCallInfo->m_sGroupCode);
			if (m_callback)
				m_callback->onCallHangup(*pEbCallInfo,true);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_HANGUP);
				pEbCallInfo->SetEventParameter((long)1);
                QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
				::SendMessage(m_pHwnd, EB_WM_CALL_HANGUP, (WPARAM)pEbCallInfo, 1);
				delete pEbCallInfo;
#endif
			}
			else {
				delete pEbCallInfo;
			}
			ClearCallInfo(sCallId,true,false,bHangup);
			//ClearCallInfo(sCallId,true,false,true);
		//}else
		//{
		//	//CCrAccountInfo pCrAccountInfo(0,pCallInfo->GetCallId(),pCallInfo->m_sChatId);
		//	//pCrAccountInfo.m_sAccount = m_pMyAccountInfo->GetUserId();
		//	//OnUserExitRoom(pCrAccountInfo,false);
		}
	}
}

void CUserManagerApp::OnFUMIUAck(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	//BOOST_ASSERT (pSotpResponseInfo.get() != 0);
	//const bool bOwnerCall = pSotpRequestInfo.get()==NULL?false:pSotpRequestInfo->m_pRequestList.getParameterValue("owner-call",false);
	const int bCall2Dep = pSotpRequestInfo.get()==NULL?0:pSotpRequestInfo->m_pRequestList.getParameterValue("call2group",(int)0);
	const mycp::bigint nCallFromUserId = pSotpRequestInfo.get()==NULL?0:pSotpRequestInfo->m_pRequestList.getParameterValue("to-uid",(mycp::bigint)0);
	const mycp::bigint nFromUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const tstring sFromAccount(pSotpResponseInfo->m_pResponseList.getParameterValue("from_account"));
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	const tstring sFromInfo(pSotpResponseInfo->m_pResponseList.getParameterValue("f-info"));

	if (bCall2Dep==1) return;
	// 处理UM
	if (!ProcUm(sCallId, nFromUserId))
	{
		return ;
	}

	const EB_CALL_ACK_TYPE nAckType = (EB_CALL_ACK_TYPE)pSotpResponseInfo->m_pResponseList.getParameterValue("ack-type", (int)EB_CAT_UNKNOWN);
	const bool bOffLineUser = (bool)(nAckType == EB_CAT_OFF_ACCEPT);
	LogMessage("OnFUMIUAck... (uid=%lld,offline=%d)\r\n",nFromUserId,(int)(bOffLineUser?1:0));
	CEBAccountInfo::pointer pFromAccountInfo;
	CEBCallInfo::pointer pCallInfo;
	if (bOffLineUser && nCallFromUserId==nFromUserId)
	//if (bOffLineUser || theOffLineUser.remove(nFromUserId))	// **感觉上没什么用
	{
		if (!theCallInfoList.find(sCallId, pCallInfo))
		{
			// 不存在呼叫
			pFromAccountInfo = CEBAccountInfo::create(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetAccount());
			pCallInfo = CEBCallInfo::create(sCallId,0,pFromAccountInfo);
			theCallInfoList.insert(sCallId, pCallInfo);
			pCallInfo->m_pCallUserList.insert(m_pMyAccountInfo->GetUserId(), pFromAccountInfo);
			pFromAccountInfo = CEBAccountInfo::create(nFromUserId,sFromAccount);
			pCallInfo->m_pCallUserList.insert(nFromUserId, pFromAccountInfo);
			//pFromAccountInfo = CEBAccountInfo::create(m_pMyAccountInfo->GetUserId(),"");
			//pCallInfo->m_pCallUserList.insert(m_pMyAccountInfo->GetUserId(), pFromAccountInfo);
		}
	}else if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		if (nCallFromUserId!=nFromUserId)
			return;
		// 有可能服务器自动应答；
		pCallInfo = CEBCallInfo::create(sCallId,0,m_pMyAccountInfo);
		theCallInfoList.insert(sCallId, pCallInfo);
		pCallInfo->m_pCallUserList.insert(m_pMyAccountInfo->GetUserId(), m_pMyAccountInfo);
		pFromAccountInfo = CEBAccountInfo::create(nFromUserId,"");
		pCallInfo->m_pCallUserList.insert(nFromUserId, pFromAccountInfo);
	}
	//if (!bOffLineUser)
	//	pCallInfo->m_nCallTime = time(0);

	const tstring sAccountAddress = pSotpResponseInfo->m_pResponseList.getParameterValue("address");
	const tstring sCallUmKey = pSotpResponseInfo->m_pResponseList.getParameterValue("um-key");

	if (pFromAccountInfo.get()==NULL && !pCallInfo->m_pCallUserList.find(nFromUserId, pFromAccountInfo))
	{
		if (nAckType == EB_CAT_ACCEPT || nAckType == EB_CAT_OFF_ACCEPT)
		{
			// 群组聊天，第三方加入
			pFromAccountInfo = CEBAccountInfo::create(nFromUserId,sFromAccount);
			pCallInfo->m_pCallUserList.insert(nFromUserId, pFromAccountInfo);
		}else
		{
			return ;
		}
	}

	//if (pCallInfo->m_sGroupCode==0 && (pFromAccountInfo->IsUnknownLineState() || bOffLineUser))	// 群组离线用户，还是设为unknown
	if (pFromAccountInfo->IsUnknownLineState() || bOffLineUser)
		pFromAccountInfo->SetLineState(bOffLineUser?EB_LINE_STATE_OFFLINE:EB_LINE_STATE_ONLINE_NEW);

	pFromAccountInfo->m_sFromInfo = sFromInfo;
    if (pSotpResponseInfo->GetResultValue() == EB_STATE_OK &&
            (nAckType == EB_CAT_ACCEPT || nAckType == EB_CAT_OFF_ACCEPT))
	{
        // 在线用户，需要更新一次；
        if (!sFromAccount.empty() && pFromAccountInfo->GetAccount()!=sFromAccount)
		{
			pFromAccountInfo->SetAccount(sFromAccount);
		}
        // 群组会话，不需要名片；
        if (pCallInfo->m_sGroupCode==0 &&
			!GetECardByFromInfo(sFromInfo,&pFromAccountInfo->m_pFromCardInfo))
		{
			return ;
		}
		this->ProcessAccountHeadInfo(pFromAccountInfo);
		if (bOffLineUser && pFromAccountInfo->m_pFromCardInfo.m_sMemberCode>0)
		{
			// 离线状态，检查成员是否非离线状态
			CEBMemberInfo::pointer pEmployeeInfo;
			if (theEmployeeList.find(pFromAccountInfo->m_pFromCardInfo.m_sMemberCode,pEmployeeInfo) && !pEmployeeInfo->IsOfflineState())
			{
				// 非离线状态，通知界面
				pEmployeeInfo->m_nLineState = EB_LINE_STATE_OFFLINE;
				if (this->m_callback)
					m_callback->onUserStateChange(pEmployeeInfo.get(),false);
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
					pEvent->SetQEventType((QEvent::Type)EB_WM_USER_STATE_CHANGE);
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
					::SendMessage(m_pHwnd, EB_WM_USER_STATE_CHANGE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), 0);
#endif
				}
			}
		}

		if (pCallInfo->m_sGroupCode>0)
		{
			// 群组会话
			CEBGroupInfo::pointer pDepartmentInfo;
			if (!theDepartmentList.find(pCallInfo->m_sGroupCode, pDepartmentInfo))
			{
				// 不存在该群组信息，需要加载
				EnterpriseLoad(pCallInfo->m_sGroupCode);
			}else if (pDepartmentInfo->m_pMemberList.size() < pCallInfo->m_pCallUserList.size())
			{
				// 有新成员加入，需要重新加载
				EnterpriseLoad(pCallInfo->m_sGroupCode);
			}
		}
        const bool bNeedEnter = (pCallInfo->m_sChatId==0 ||
                                 pCallInfo->m_nCallState==EB_CALL_STATE_INVALIDATE)?true:false;
		int nConnectFlag = 0;
		if (bOffLineUser)
			nConnectFlag |= EB_CONNECTED_OFFLINE_USER;
        // 自动响应，或对方下线，自动呼叫
        if(pCallInfo->m_nCallState==EB_CALL_STATE_AUTO_ACK)
			nConnectFlag |= EB_CONNECTED_AUTO_ACK;
        // ** 群组聊天不会跑这里，不过这里的代码不影响业务，暂时保留
        if (pCallInfo->m_sGroupCode>0)
		{
            // *** 不可以 remove
            // *** 需要 remove
            if ((bNeedEnter && theOwnerCallGroup.exist(nFromUserId)) ||
                (!bNeedEnter && theOwnerCallGroup.remove(nFromUserId)))
			{
					nConnectFlag |= EB_CONNECTED_OWNER_CALL;
			}
		}else// if (pCallInfo->m_sGroupCode==0)
		{
            // *** 不可以 remove
            // *** 需要 remove
            if ((bNeedEnter && theOwnerCallUser.exist(nFromUserId)) ||
                (!bNeedEnter && theOwnerCallUser.remove(nFromUserId)))
			{
					nConnectFlag |= EB_CONNECTED_OWNER_CALL;
			}
		}
		SendConnectMsg(pCallInfo,nFromUserId,sFromAccount,nConnectFlag,false);

		if (pCallInfo->m_sChatId==0 || pCallInfo->m_nCallState==EB_CALL_STATE_INVALIDATE)
		{
			// eb_c_enter
			//if (!pCallInfo->m_sGroupCode.empty()) // && pCallInfo->m_nCallState==EB_CALL_STATE_AUTO_ACK && (time(0)-pCallInfo->m_tLastTime)<=1)
			//{
			//	return ; // 群组会话，前面刚响应，而且这是别人进入，所以不需要处理
			//}
			CPOPCUserManager::pointer pPOPCUserManager;
			if (!theUserManagerList.find(sCallId, pPOPCUserManager))
			{
				EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,sFromAccount, sCallId,pCallInfo->m_sGroupCode);
				if (m_callback)
					m_callback->onCallError(*pEbCallInfo,EB_STATE_ERROR);
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
					pEbCallInfo->SetEventParameter((long)EB_STATE_ERROR);
                    QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
					::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, EB_STATE_ERROR);
					delete pEbCallInfo;
#endif
				}
				else {
					delete pEbCallInfo;
				}
				return ;
			}
			if (pCallInfo->m_sGroupCode==0 || !sCallUmKey.empty())
				pCallInfo->m_sUmKey = sCallUmKey;
			CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
			pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", nFromUserId));
			pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("off-line-user", bOffLineUser));
			//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("owner-call", bOwnerCall));
			pPOPCUserManager->SendUMIUEnter(sCallId, pCallInfo->m_sUmKey,pCallInfo->m_sGroupCode, pRequestInfo);
		}else
		{
			EnterRoom(pCallInfo);
			//EnterRoom(pCallInfo,bOffLineUser?nFromUserId:0);
			if (!theSendFileCallTime.remove(pCallInfo->GetCallId()) || bOffLineUser)
			{
				const bool bCheckCallTime = bOffLineUser?false:true;
				ProcessToSendList(pCallInfo->GetCallId(),false,bCheckCallTime);
				//ProcessToSendList(pCallInfo->GetCallId(),false,true);
				//ProcessToSendList(pCallInfo->GetCallId(),false);
			}

			//int nConnectFlag = 0;
			//// 关闭窗口，会话存在，第二次呼叫；
			////int nConnectFlag = (pCallInfo->m_sGroupCode==0&&pCallInfo->m_nCallState!=EB_CALL_STATE_AUTO_ACK&&)?EB_CONNECTED_OWNER_CALL:0; // 群组会话设为0
			//if (bOffLineUser)
			//	nConnectFlag |= EB_CONNECTED_OFFLINE_USER;
			//if(pCallInfo->m_nCallState==EB_CALL_STATE_AUTO_ACK)		// 自动响应，或对方下线，自动呼叫
			//	nConnectFlag |= EB_CONNECTED_AUTO_ACK;
			//else if (pCallInfo->m_sGroupCode==0 && theOwnerCallUser.remove(nFromUserId))	// ?
			//	nConnectFlag |= EB_CONNECTED_OWNER_CALL;
			//SendConnectMsg(pCallInfo,nFromUserId,sFromAccount,nConnectFlag);
		}
	}else if (pCallInfo->m_nCallState != EB_CALL_STATE_CONNECTED || nAckType != EB_CAT_TIMEOUT)
	{
		// EB_CALL_STATE_CONNECTED && EB_CAT_TIMEOUT有可能是服务器重启，对方下线后，自动呼叫返回，所以不处理
		if (pCallInfo->m_sGroupCode==0)
		{
			EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,sFromAccount, sCallId,pCallInfo->m_sGroupCode);
			const EB_STATE_CODE nStateCode = nAckType == EB_CAT_TIMEOUT?EB_STATE_TIMEOUT_ERROR:EB_STATE_USER_BUSY;
			if (m_callback)
				m_callback->onCallError(*pEbCallInfo,nStateCode);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
				pEbCallInfo->SetEventParameter((long)nStateCode);
                QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
				::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, nStateCode);
				delete pEbCallInfo;
#endif
			}
			else {
				delete pEbCallInfo;
			}
		}
		if (pCallInfo->m_sGroupCode==0)
		{
			// 一对一会话，清除数据
			ClearCallInfo(sCallId,true,true);
			theOwnerCallUser.remove(nFromUserId);
		}else
		{
			theOwnerCallGroup.remove(pCallInfo->m_sGroupCode);
		}
		// 用户忙或其他错误
		//theUserChatManagerList.remove(sFromAccount);
		//theLogonCenterList.remove(sFromAccount);
		//theUserManagerList.find(sFromAccount, pDeletePOPCUserManager, true);
	}
}
void CUserManagerApp::OnUMIUAck(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	//BOOST_ASSERT (pSotpRequestInfo.get() != 0);
	BOOST_ASSERT (pSotpResponseInfo.get() != 0);
	if (pSotpRequestInfo.get() == NULL)
	{
		// may be timeout
		return ;
	}
	const mycp::bigint sCallId = pSotpRequestInfo->m_pRequestList.getParameterValue("call-id",(mycp::bigint)0);
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	const mycp::bigint nFromUserId = pSotpRequestInfo->m_pRequestList.getParameterValue("to-uid",(mycp::bigint)0);
	const EB_CALL_ACK_TYPE nAckType = (EB_CALL_ACK_TYPE)pSotpRequestInfo->m_pRequestList.getParameterValue("ack-type", (int)EB_CAT_UNKNOWN);
	pSotpResponseInfo->m_pResponseList.SetParameter(CGC_PARAMETER("ack-type", (int)nAckType));
	//const tstring sUmAppName = pSotpResponseInfo->m_pResponseList.getParameterValue("um-appname");
	const tstring sUmServer(pSotpResponseInfo->m_pResponseList.getParameterValue("um-server"));
	const tstring sCallUmKey = pSotpResponseInfo->m_pResponseList.getParameterValue("um-key");

	const int nResultValue = pSotpResponseInfo->GetResultValue();
	if (nResultValue == EB_STATE_OK && nAckType == EB_CAT_ACCEPT)
	{
		if (pCallInfo->m_sGroupCode>0)
		{
			// 群组会话
			CEBGroupInfo::pointer pDepartmentInfo;
			if (!theDepartmentList.find(pCallInfo->m_sGroupCode, pDepartmentInfo))
			{
				// 不存在该群组信息，需要加载
				EnterpriseLoad(pCallInfo->m_sGroupCode);
			}else if (pDepartmentInfo->m_pMemberList.size() < pCallInfo->m_pCallUserList.size())
			{
				// 有新成员加入，需要重新加载
				EnterpriseLoad(pCallInfo->m_sGroupCode);
			}
		}
        const tstring sMyUmAddress = m_pUserManager->GetAddress().address();
        const bool bUMServerChange = (!sUmServer.empty() && sUmServer!=pCallInfo->m_sUmServer && sUmServer!=sMyUmAddress)?true:false;
//        const bool bUMServerChange = pCallInfo->m_sUmServer==sUmServer?false:true;
        //pCallInfo->m_sUmAppName = sUmAppName;
        if (bUMServerChange) {
            pCallInfo->m_sUmServer = sUmServer;		/// 空为本UM
        }
        if (pCallInfo->m_sChatId==0 || bUMServerChange || pCallInfo->m_sUmServer.empty() )
		{
			CPOPCUserManager::pointer pPOPCUserManager;
			if (pCallInfo->m_sUmServer.empty())
			{
                pCallInfo->m_sUmServer = sMyUmAddress;
				//// 空为本UM，这里不需要 theUserManagerList.insert，因为在fc_call已经保存
				//pPOPCUserManager = m_pUserManager;
			//}else
			}
			{
				theUserManagerList.remove(sCallId);
				//if (m_pUserManager.get() != NULL && m_pUserManager->GetAddress().address() == pCallInfo->m_sUmServer)
				//	pPOPCUserManager = m_pUserManager;
				//else// if (!theUserManagerList.find(sCallId, pPOPCUserManager))
				{
					bool bRetry = false;
					pPOPCUserManager = CPOPCUserManager::create(this,0);
					pPOPCUserManager->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
					int ret = pPOPCUserManager->Start(pCallInfo->m_sUmServer, pCallInfo->m_sUmAppName,0,true,true,1);
					if (ret==-1)
					{
						// 域名解析问题，再试一次；
						const tstring sServerAddress = GetAddressList(pCallInfo->m_sUmServer);
						if (!sServerAddress.empty())
						{
							pPOPCUserManager.reset();
							pPOPCUserManager = CPOPCUserManager::create(this,0);
							pPOPCUserManager->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
							ret = pPOPCUserManager->Start(sServerAddress,pCallInfo->m_sUmAppName,0,true,true,1);
							bRetry = true;
						}
					}
					if (ret != 0)
					{
						EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,"", sCallId,pCallInfo->m_sGroupCode);
						if (m_callback)
							m_callback->onCallError(*pEbCallInfo,EB_STATE_ERROR);
						if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
							pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
							pEbCallInfo->SetEventParameter((long)EB_STATE_ERROR);
                            QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
							::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, EB_STATE_ERROR);
							delete pEbCallInfo;
#endif
						}
						else {
							delete pEbCallInfo;
						}
						return ;
					}
					pPOPCUserManager->SetUserId(this->m_pMyAccountInfo->GetUserId());
					if (!bRetry)
						SaveAddressList(pCallInfo->m_sUmServer);
				}
				theUserManagerList.insert(sCallId, pPOPCUserManager);	// 统一放在这里
			}
			if (pCallInfo->m_sGroupCode==0 || !sCallUmKey.empty())
				pCallInfo->m_sUmKey = sCallUmKey;
			CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
			pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", nFromUserId));
			//if (pCallInfo->m_sGroupCode==0 && pCallInfo->m_nCallState!=EB_CALL_STATE_AUTO_ACK)	// 不是自动应答，手动主动应答
			//	pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("owner-call", true));
			pPOPCUserManager->SendUMIUEnter(sCallId, pCallInfo->m_sUmKey,pCallInfo->m_sGroupCode, pRequestInfo);
		}else
		{
			EnterRoom(pCallInfo);
			int nConnectFlag = pCallInfo->m_nCallState==EB_CALL_STATE_AUTO_ACK?EB_CONNECTED_AUTO_ACK:0;
			if(pCallInfo->m_nCallState==EB_CALL_STATE_EXIT)
				nConnectFlag |= EB_CONNECTED_AUTO_ACK;
			SendConnectMsg(pCallInfo,nFromUserId,"",nConnectFlag);
		}
		// 感觉没用
		//if (!pCallInfo->m_sGroupCode.empty() && sCallId != pCallInfo->m_sGroupCode)
		//{
		//	// 这是响应进入群组会话
		//	ClearCallInfo(sCallId,true);
		//}
		// 感觉没用
		//if (!pCallInfo->m_sGroupCode.empty() && !theCallInfoList.exist(pCallInfo->m_sGroupCode))
		//{
		//	// 响应邀请进入群组，需要呼叫
		//	CallGroup(pCallInfo->m_sGroupCode);
		//}
	}else
	{
		if (theCallInfoList.exist(sCallId))
		{
			EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,"", sCallId,pCallInfo->m_sGroupCode);
			if (m_callback)
				m_callback->onCallHangup(*pEbCallInfo,true);

			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_HANGUP);
				pEbCallInfo->SetEventParameter((long)1);
                QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
				::SendMessage(m_pHwnd, EB_WM_CALL_HANGUP, (WPARAM)pEbCallInfo, 1);
				delete pEbCallInfo;
#endif
			}
			else {
				delete pEbCallInfo;
			}
			ClearCallInfo(sCallId,true,true);
		}
		//theUserChatManagerList.remove(sFromAccount);
		//theLogonCenterList.remove(sFromAccount);
		//theUserManagerList.find(sFromAccount, pDeletePOPCUserManager, true);
	}
}

void CUserManagerApp::EnterRoom(const CEBCallInfo::pointer& pCallInfo)
{
	// **加了这段代码，群组会有问题；
	//if (pCallInfo->m_pChatRoom.get()!=NULL && !pCallInfo->m_pChatRoom->IsEnterOk())
	//{
	//	// 前面进入房间失败，清空后重新创建；
	//	pCallInfo->m_pChatRoom->Stop();
	//	pCallInfo->m_pChatRoom.reset();
	//}
	if (pCallInfo->m_pChatRoom.get()!=NULL && !pCallInfo->m_sCmServer.empty() && pCallInfo->m_pChatRoom->GetAddress()!=pCallInfo->m_sCmServer)
	{
		pCallInfo->m_pChatRoom.reset();
	}
	if (pCallInfo->m_pChatRoom.get()==NULL && !pCallInfo->m_sCmServer.empty())
	{
		pCallInfo->m_pChatRoom = Cchatroom::create();
		pCallInfo->m_pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
		pCallInfo->m_pChatRoom->Start(pCallInfo->m_sCmServer.c_str(), pCallInfo->m_sCmAppName.c_str(),0,true,1);
		pCallInfo->m_pChatRoom->SetCallBack(this);
	}

	if (pCallInfo->m_pChatRoom.get()!=NULL)
	{
		int nLogonType = m_pMyAccountInfo->GetLogonType();
		pCallInfo->m_pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(), pCallInfo->m_sChatId,pCallInfo->m_sChatKey.c_str(),
			nLogonType,pCallInfo->GetCallId(), pCallInfo->m_sGroupCode);
	}
}

void CUserManagerApp::OnUMIUEnter(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return ;
	if (pSotpResponseInfo.get() == NULL) return ;
	const mycp::bigint sFromAccount = pSotpRequestInfo->m_pRequestList.getParameterValue("to-uid",(mycp::bigint)0);
	const bool bOffLineUser = pSotpRequestInfo->m_pRequestList.getParameterValue("off-line-user",false);
	//const bool bOwnerCall = pSotpRequestInfo->m_pRequestList.getParameterValue("owner-call",false);
	const bool bInviteCall = pSotpRequestInfo->m_pRequestList.getParameterValue("invite-call",false);

	const int nResultValue = pSotpResponseInfo->GetResultValue();
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	//LogMessage("OnUMIUUser... (sFromAccount=%lld,sCallId=%lld)\r\n", sFromAccount,(mycp::bigint)sCallId);
	CEBCallInfo::pointer pCallInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo))
	{
		return ;
	}
	const tstring sCmServer(pSotpResponseInfo->m_pResponseList.getParameterValue("cm-server"));
	if (pCallInfo->m_sGroupCode==0 || pCallInfo->m_sChatKey.empty() || !sCmServer.empty())
	{
		//pCallInfo->m_sCmAppName = pSotpResponseInfo->m_pResponseList.getParameterValue("cm-appname");
		pCallInfo->m_sCmServer = sCmServer;
		pCallInfo->m_sChatId = pSotpResponseInfo->m_pResponseList.getParameterValue("cm-id",(mycp::bigint)0);
		pCallInfo->m_sChatKey = pSotpResponseInfo->m_pResponseList.getParameterValue("cm-key");
	}

	bool bOwnerCall = false;
	if (pCallInfo->m_sGroupCode>0)
		bOwnerCall = theOwnerCallGroup.remove(pCallInfo->m_sGroupCode);
	else
		bOwnerCall = theOwnerCallUser.remove(sFromAccount);

	bool bAutoAck = pCallInfo->m_nCallState==EB_CALL_STATE_AUTO_ACK;
	//bool bOnlineInCall = pCallInfo->m_nCallState==EB_CALL_STATE_ONLINE_INCALL;
	// 先通知事件
	int nConnectFlag = 0;
	if (bAutoAck)
		nConnectFlag |= EB_CONNECTED_AUTO_ACK;
	else if (bOwnerCall)
		nConnectFlag |= EB_CONNECTED_OWNER_CALL;
	//else
	//	nConnectFlag |= EB_CONNECTED_OWNER_ACK;
	if (bOffLineUser)
		nConnectFlag |= EB_CONNECTED_OFFLINE_USER;
	SendConnectMsg(pCallInfo,sFromAccount,"",nConnectFlag);

	EnterRoom(pCallInfo);
	//EnterRoom(pCallInfo,bOffLineUser?sFromAccount:0);

	// 主要用于接收一对一离线消息，eb_c_enter后呼叫对方
	if (bInviteCall)
	{
		this->InviteCall(sCallId,0);
	}else
	{
		// *** 服务器重启，聊天界面没有关闭情况下，发送会返回 51，或54错误
		//const bool bCheckCallTime = bOffLineUser?false:true;
		//ProcessToSendList(sCallId,false,bCheckCallTime);
		ProcessToSendList(sCallId,false,true);
		//ProcessToSendList(sCallId,false);
	}
}
bool CUserManagerApp::ProcUm(eb::bigint sCallId, mycp::bigint nFromUserId)
{
	CPOPCUserManager::pointer pPOPCUserManager;
	if (!theUserManagerList.exist(sCallId))
	{
		if (!theUserManagerList.find(nFromUserId,pPOPCUserManager))
		{
			return false;
		}
		theUserManagerList.insert(sCallId,pPOPCUserManager);
	}
	return true;
}

void CUserManagerApp::OnUMIUUser(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return ;
	if (pSotpResponseInfo.get() == NULL) return ;
	const int bCall2Dep = pSotpRequestInfo->m_pRequestList.getParameterValue("call2group",(int)0);
	const mycp::bigint nFromUserId = pSotpRequestInfo->m_pRequestList.getParameterValue("to-uid",(mycp::bigint)0);
	const tstring sFromAccount(pSotpRequestInfo->m_pRequestList.getParameterValue("to-account"));
	//const bool bOwnerCall = pSotpRequestInfo->m_pRequestList.getParameterValue("owner-call",false);
	//const int nCall2Dep = pSotpRequestInfo->m_pRequestList.getParameterValue("c2d",(int)0);
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	LogMessage("OnUMIUUser... (nFromUserId=%lld,sCallId=%lld)\r\n", nFromUserId,(mycp::bigint)sCallId);
	const EB_ACCOUNT_TYPE nFromType = (EB_ACCOUNT_TYPE)pSotpResponseInfo->m_pResponseList.getParameterValue("f-type", (int)EB_ACCOUNT_TYPE_VISITOR);
	const int nAutoCallAccept = pSotpResponseInfo->m_pResponseList.getParameterValue("auto-accept", (int)0);

	// 处理UM
	if (!ProcUm(sCallId, nFromUserId))
	{
		return ;
	}

	//LogMessage("OnUMIUUser... (uid=%lld,nResultValue=%d)\r\n",nUserId,(int)nResultValue);
	if (nResultValue == EB_STATE_OK)
	{
		if (bCall2Dep==1) return;

		bool bNewCall = false;
		CEBCallInfo::pointer pCallInfo;
		if (!theCallInfoList.find(sCallId, pCallInfo))
		{
			bNewCall = true;
			pCallInfo = CEBCallInfo::create(sCallId,0,m_pMyAccountInfo);
			theCallInfoList.insert(sCallId, pCallInfo);
			pCallInfo->m_pCallUserList.insert(m_pMyAccountInfo->GetUserId(), m_pMyAccountInfo);
		}
		if (!pCallInfo->m_pCallUserList.exist(m_pMyAccountInfo->GetUserId()))
		{
			pCallInfo->m_pCallUserList.insert(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo);
		}
		if (pCallInfo->m_sGroupCode>0)
		{
			// 群组会话，直接进入
			const tstring sCallUmKey(pSotpRequestInfo->m_pRequestList.getParameterValue("um-key"));
			if (!sCallUmKey.empty())
				pCallInfo->m_sUmKey = sCallUmKey;
			CPOPCUserManager::pointer pPOPCUserManager;
			if (!theUserManagerList.find(sCallId,pPOPCUserManager))
			{
				return ;
			}
			CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
			pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", nFromUserId));
			//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("owner-call", bOwnerCall));
			pPOPCUserManager->SendUMIUEnter(sCallId, pCallInfo->m_sUmKey,pCallInfo->m_sGroupCode,pRequestInfo);
			return ;
		}
		CEBAccountInfo::pointer pFromAccountInfo;
		if (!pCallInfo->m_pCallUserList.find(nFromUserId, pFromAccountInfo))
		{
			pFromAccountInfo = CEBAccountInfo::create(nFromUserId,sFromAccount);
			pFromAccountInfo->m_pFromCardInfo.m_nAccountType = nFromType;
			pCallInfo->m_pCallUserList.insert(nFromUserId, pFromAccountInfo);
		}else if (pFromAccountInfo->m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_VISITOR)
			pFromAccountInfo->m_pFromCardInfo.m_nAccountType = nFromType;
		//const EB_ACCOUNT_TYPE nFromType = (EB_ACCOUNT_TYPE)pSotpRequestInfo->m_pRequestList.getParameterValue("f-type", (int)EB_ACCOUNT_TYPE_VISITOR);
		if (bNewCall)
			pCallInfo->m_nCallState = EB_CALL_STATE_ALERTING;
		if (nAutoCallAccept!=1)
		//if (nFromType != EB_ACCOUNT_TYPE_IN_ENT)
		{
			// 不是公司内部员工，需要等待接通
			if (pCallInfo->m_sOldCallId==0)	// 非合并会话，合并会话不发送事件
			//if (bNewCall)
			{
				EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,sFromAccount, sCallId,pCallInfo->m_sGroupCode);
				if (m_callback)
					m_callback->onCallAlerting(*pEbCallInfo);
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ALERTING);
                    QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
					::SendMessage(m_pHwnd, EB_WM_CALL_ALERTING, (WPARAM)pEbCallInfo, 0);
					delete pEbCallInfo;
#endif
				}
				else {
					delete pEbCallInfo;
				}
			}
		}
	}else
	{
		mycp::bigint sGroupCode=0;
		CEBCallInfo::pointer pCallInfo;
		if (theCallInfoList.find(sCallId, pCallInfo))
		{
			sGroupCode = pCallInfo->m_sGroupCode;
		}
		EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,"", sCallId,sGroupCode);
		if (m_callback)
			m_callback->onCallError(*pEbCallInfo,(EB_STATE_CODE)nResultValue);

		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
			pEbCallInfo->SetEventParameter((long)nResultValue);
            QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
			::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, nResultValue);
			delete pEbCallInfo;
#endif
		}
		else {
			delete pEbCallInfo;
		}
	}
}
CEBCallInfo::pointer CUserManagerApp::GetExistCallInfo(mycp::bigint sFromAccount, mycp::bigint sDepCode)
{
	BoostReadLock lock(theCallInfoList.mutex());
	CLockMap<mycp::bigint, CEBCallInfo::pointer>::const_iterator pIter = theCallInfoList.begin();
	for (;pIter!=theCallInfoList.end(); pIter++)
	{
		const CEBCallInfo::pointer& pCallInfo = pIter->second;
		//if (pCallInfo->m_pCallUserList.size() <= 2)
		if (pCallInfo->m_sGroupCode == sDepCode)			// 同一个群，或一对一会话
		{
			if (pCallInfo->m_pCallUserList.exist(sFromAccount))
			{
				return pCallInfo;
			}
		}
	}
	return NullCallInfo;
}

//void CUserManagerApp::OnFUMIUGroup(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
//{
//	//theDeletePOPCLogonCenter.reset();
//	const tstring sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from");
//	const tstring sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id");
//	const int nMulti = pSotpResponseInfo->m_pResponseList.getParameterValue("multi", (int)0);
//	const tstring sEntCode = pSotpResponseInfo->m_pResponseList.getParameterValue("ent-code");
//	const tstring sDepCode = pSotpResponseInfo->m_pResponseList.getParameterValue("dep-code");
//	const tstring sChatId = pSotpResponseInfo->m_pResponseList.getParameterValue("chat-id");
//	const tstring sChatKey = pSotpResponseInfo->m_pResponseList.getParameterValue("chat-key");
//	const tstring sChatAppName = pSotpResponseInfo->m_pResponseList.getParameterValue("chat-app-name");
//	const tstring sChatServer = pSotpResponseInfo->m_pResponseList.getParameterValue("chat-server");
//	const EB_ACCOUNT_TYPE nFromType = (EB_ACCOUNT_TYPE)pSotpResponseInfo->m_pResponseList.getParameterValue("f-type", (int)EB_ACCOUNT_TYPE_VISITOR);
//
//}
void CUserManagerApp::OnFUMIUEnter(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{

}

void CUserManagerApp::OnFUMIUUser(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const mycp::bigint nFromUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const tstring sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from_account");
	//const mycp::bigint sOldCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("old-cid",(mycp::bigint)0);	// 对方要求合并会话
	const mycp::bigint sCallId = pSotpResponseInfo->m_pResponseList.getParameterValue("call-id",(mycp::bigint)0);
	//LogMessage("OnFUMIUUser... (nFromUserId=%lld,sCallId=%lld)\r\n", nFromUserId,(mycp::bigint)sCallId);
	const mycp::bigint sDepCode = pSotpResponseInfo->m_pResponseList.getParameterValue("dep-code",(mycp::bigint)0);
	const tstring sFromInfo = pSotpResponseInfo->m_pResponseList.getParameterValue("f-info");
	const int nAutoCallAccept = pSotpResponseInfo->m_pResponseList.getParameterValue("auto-accept",(int)0);

	//MessageBox(NULL,_T("收到会话请求！"),"",MB_OK);
	//bool bExistOldCall = false;
	//if (sOldCallId>0)
	//{
	//	bExistOldCall = ClearCallInfo(sOldCallId,true,false);	// ?true:bSendHangup
	//	//if (!ClearCallInfo(sOldCallId))
	//	//{
	//	//	return ;
	//	//}
	//}
	// ***先查找本地是否有现存会话，（预防万一，例如服务器重启时）
	//CEBCallInfo::pointer pCallInfo;
	CEBCallInfo::pointer pCallInfo = GetExistCallInfo(nFromUserId, 0);	// depcode:群组，为空表示单对单会话
	if (pCallInfo.get() != NULL
		&& sDepCode==0						// 只处理一对一会话
		&& pCallInfo->GetCallId() != sCallId
		/*&& sOldCallId==0*/)					// 为空，表示新会话，不为空，是对方要求合并会话
	{
		// 找到之前的现存会话（聊天界面没有关闭）
		// 响应进入旧会话 （合并二个会话）
		//CallAck(pCallInfo, EB_CAT_ACCEPT);
		// 直接响应，并更新old call id
		pCallInfo->m_nCallState = EB_CALL_STATE_AUTO_ACK;
		pCallInfo->m_sOldCallId = pCallInfo->GetCallId();
		pCallInfo->SetCallId(sCallId);
		pCallInfo->m_sChatId = 0;	// **设为空，会重新enter
		if (pCallInfo->m_pChatRoom.get()!=NULL)
		{
			int nLogonType = m_pMyAccountInfo->GetLogonType();
			pCallInfo->m_pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(), sCallId,pCallInfo->m_sChatKey.c_str(),nLogonType,sCallId, pCallInfo->m_sGroupCode);
			//m_pClearChatRoom.add(pCallInfo->m_pChatRoom);
			//pCallInfo->m_pChatRoom.reset();
		}
		theCallInfoList.remove(pCallInfo->m_sOldCallId);
		theCallInfoList.insert(pCallInfo->GetCallId(), pCallInfo,false);
		if (!theUserManagerList.exist(pCallInfo->GetCallId()))
		{
			CPOPCUserManager::pointer pPOPCUserManager;
			if (theUserManagerList.find(pCallInfo->m_sOldCallId,pPOPCUserManager))
			//if (theUserManagerList.find(pCallInfo->m_sOldCallId,pPOPCUserManager,true))
			{
				theUserManagerList.insert(sCallId,pPOPCUserManager,false);
			}else
			{
				theUserManagerList.insert(sCallId, m_pUserManager);
			}
		}
		CallAck(pCallInfo, EB_CAT_ACCEPT);
		// **成功
		//pCallInfo->m_sOldCallId = sCallId;
		//InviteCall(pCallInfo, sCallId);	// 需要重新呼叫，对方过来的callid不同；（服务器已经做了处理）
		return ;
	}
	bool bNewCallInfo = false;
	CEBAccountInfo::pointer pFromAccountInfo;
	if (!theCallInfoList.find(sCallId, pCallInfo)) {
		bNewCallInfo = true;
		pFromAccountInfo = CEBAccountInfo::create(nFromUserId,sFromAccount);
		pCallInfo = CEBCallInfo::create(sCallId,sDepCode,pFromAccountInfo);
		pCallInfo->m_sGroupCode = sDepCode;
		theCallInfoList.insert(sCallId, pCallInfo);
		if (pCallInfo->m_sGroupCode>0)
			BuildGroupCallUserList(pCallInfo,pCallInfo->m_sGroupCode);
		else
			pCallInfo->m_pCallUserList.insert(nFromUserId, pFromAccountInfo);
	}
	//pCallInfo->m_sOldCallId = sOldCallId==pCallInfo->GetCallId()?0:sOldCallId;
	// 本方应答，保存该呼叫ID对应UM，方便应答（必须放在 CallAck 前面）
	theUserManagerList.insert(sCallId, m_pUserManager);
	if (!pCallInfo->m_pCallUserList.exist(m_pMyAccountInfo->GetUserId())) {
		pCallInfo->m_pCallUserList.insert(m_pMyAccountInfo->GetUserId(), m_pMyAccountInfo);
	}
	if (!pCallInfo->m_pCallUserList.find(nFromUserId, pFromAccountInfo)) {
		pFromAccountInfo = CEBAccountInfo::create(nFromUserId,sFromAccount);
		pCallInfo->m_pCallUserList.insert(nFromUserId, pFromAccountInfo);
		if (pCallInfo->m_sGroupCode>0) {
			EnterpriseLoad(pCallInfo->m_sGroupCode,0,0,0,0,nFromUserId);	// 预防万一
		}
	}
	else if (!bNewCallInfo) {
		//LogMessage("OnFUMIUUser... bNewCallInfo=false,(nFromUserId=%lld,sCallId=%lld)\r\n", nFromUserId,(mycp::bigint)sCallId);
		// 已经在会话中
		// 自动接收
		if (pCallInfo->m_nCallState == EB_CALL_STATE_INVALIDATE) {
			InviteCall(pCallInfo, sCallId);	// 有可能是服务重启，需要重新呼叫一次
		}
		else if (pCallInfo->m_nCallState != EB_CALL_STATE_INCOMING) {	// 前面未应答，第二次CALL
			pCallInfo->m_nCallState = EB_CALL_STATE_AUTO_ACK;
			if (pCallInfo->GetFromUserId()!=nFromUserId) {
				pCallInfo->SetFromAccountInfo(pFromAccountInfo);
			}
			CallAck(pCallInfo, EB_CAT_ACCEPT);
		}
		return;
	}
	//pCallInfo->SetCallId(12345678);
	//InviteCall(pCallInfo, sCallId);	// 需要重新呼叫，对方过来的callid不同；（服务器已经做了处理）
	//return;

	// 电子名片
	pFromAccountInfo->m_sFromInfo = sFromInfo;
	if (!GetECardByFromInfo(sFromInfo,&pFromAccountInfo->m_pFromCardInfo))
		return;
	pFromAccountInfo->SetUserSignId(pFromAccountInfo->m_pFromCardInfo.m_nUserSignId);
	this->ProcessAccountHeadInfo(pFromAccountInfo);

	// 检查个人设置，是否自动接通会话
	const int nSettingValue = m_pMyAccountInfo->GetSetting();
	EB_ACCOUNT_TYPE nFromType = (EB_ACCOUNT_TYPE)pFromAccountInfo->m_pFromCardInfo.m_nAccountType;
	if (nAutoCallAccept==1 || /*bExistOldCall ||*/ // 由本方发起会话，对方合并呼叫，直接响应
		nFromType == EB_ACCOUNT_TYPE_IN_ENT ||	// 内部成员
		(sDepCode>0 && theDepartmentList.exist(sDepCode)) || // 正常群组会话
		(nFromType == EB_ACCOUNT_TYPE_OUT_ENT && (nSettingValue&EB_SETTING_AUTO_OUTENT_ACCEPT)==EB_SETTING_AUTO_OUTENT_ACCEPT) ||
		(nFromType == EB_ACCOUNT_TYPE_USER && (nSettingValue&EB_SETTING_AUTO_USER_ACCEPT)==EB_SETTING_AUTO_USER_ACCEPT) ||
		(nFromType == EB_ACCOUNT_TYPE_VISITOR && (nSettingValue&EB_SETTING_AUTO_VISITOR_ACCEPT)==EB_SETTING_AUTO_VISITOR_ACCEPT)// ||
		/*theAcceptUser.exist(nFromUserId)*/)
	{
		// 自动接收
		//MessageBox(NULL,_T("自动应答会话请求！"),"",MB_OK);
		pCallInfo->m_nCallState = EB_CALL_STATE_AUTO_ACK;
		if (pCallInfo->GetFromUserId()!=nFromUserId)
			pCallInfo->SetFromAccountInfo(pFromAccountInfo);

		CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_CALL_ACCEPT);
		pProcessMsgInfo->m_tProcessTime = m_pMyAccountInfo->GetOnlineTime()+12;
		pProcessMsgInfo->m_pCallInfo = pCallInfo;
		m_pProcessMsgList.add(pProcessMsgInfo);
		//CallAck(pCallInfo, EB_CAT_ACCEPT);
		return ;
	}
	// 实现联系人自动应答；
	// **其实服务端已经自动应答，不需要客户端应答；
	if ((nSettingValue&EB_SETTING_AUTO_CONTACT_ACCEPT)==EB_SETTING_AUTO_CONTACT_ACCEPT)
	{
		// 自动应答联系人；
		if (IsMyContact(nFromUserId))
		{
			pCallInfo->m_nCallState = EB_CALL_STATE_AUTO_ACK;
			if (pCallInfo->GetFromUserId()!=nFromUserId)
				pCallInfo->SetFromAccountInfo(pFromAccountInfo);
			CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_CALL_ACCEPT);
			pProcessMsgInfo->m_tProcessTime = m_pMyAccountInfo->GetOnlineTime()+12;
			pProcessMsgInfo->m_pCallInfo = pCallInfo;
			m_pProcessMsgList.add(pProcessMsgInfo);
			//CallAck(pCallInfo, EB_CAT_ACCEPT);
			return;
		}

		//BoostReadLock lockContact(theContactList.mutex());
		//CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList.begin();
		//for (; pIterContact!=theContactList.end(); pIterContact++)
		//{
		//	CEBContactInfo::pointer pContactInfo = pIterContact->second;
		//	if (pContactInfo->m_nContactUserId==nFromUserId)
		//	{
		//		lockContact.unlock();
		//		pCallInfo->m_nCallState = EB_CALL_STATE_AUTO_ACK;
		//		if (pCallInfo->GetFromUserId()!=nFromUserId)
		//			pCallInfo->SetFromAccountInfo(pFromAccountInfo);
		//		CallAck(pCallInfo, EB_CAT_ACCEPT);
		//		return;
		//	}
		//}
	}

	//MessageBox(NULL,_T("手工应答会话请求！"),"",MB_OK);

	pCallInfo->m_nCallState = EB_CALL_STATE_INCOMING;

	EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,sFromAccount, sCallId,pCallInfo->m_sGroupCode);
	if (m_callback)
		m_callback->onCallIncoming(*pEbCallInfo,*pFromAccountInfo.get());
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_INCOMING);
		pEbCallInfo->SetEventData((void*)(const EB_AccountInfo*)pFromAccountInfo.get());
        postWaitEventResult(m_pHwnd, pEbCallInfo);
#else
		::SendMessage(m_pHwnd, EB_WM_CALL_INCOMING, (WPARAM)pEbCallInfo,(LPARAM)(const EB_AccountInfo*)pFromAccountInfo.get());
		delete pEbCallInfo;
#endif
	}
	else {
		delete pEbCallInfo;
	}
}
void CUserManagerApp::ProcessEmotionCount(int nEmoCount, const tstring& sEmoVerString)
{
	if (nEmoCount<0 || sEmoVerString.empty()) return;
	int nLocalEmoCount = -1;
	BoostReadLock rdLock(m_mutexEBDatas);
	if (this->m_pEBDatas.get()!=0)
	{
		tstring sEmoList;
		char lpszBuffer[512];
		std::vector<tstring> pList;
		std::vector<tstring> pVerList;
		const int nCount = ParseString(sEmoVerString,";",false,pList);
		for (int i=0; i<nCount; i++)
		{
			const tstring sEmoVer = pList[i];
			if (ParseString(sEmoVer,",",true,pVerList)==2)
			{
				const mycp::bigint nEmoId = cgc_atoi64(pVerList[0].c_str());
				const int nVersion = atoi(pVerList[1].c_str());
				if (sEmoList.empty())
					sprintf(lpszBuffer,"%lld",nEmoId);
				else
					sprintf(lpszBuffer,",%lld",nEmoId);
				sEmoList.append(lpszBuffer);
			}
		}

		sprintf(lpszBuffer,"SELECT count(emo_id) FROM eb_emo_info_t WHERE emo_id IN (%s)",sEmoList.c_str());
		int nCookie = 0;
		m_pEBDatas->select(lpszBuffer, nCookie);
		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		if (pRecord.get()!=0)
		{
			nLocalEmoCount = pRecord->getVector()[0]->getIntValue();
			m_pEBDatas->reset(nCookie);
		}
	}

	if (nLocalEmoCount>=nEmoCount)
	{
		std::vector<tstring> pList;
		std::vector<tstring> pVerList;
		const int nCount = ParseString(sEmoVerString,";",false,pList);
		for (int i=0; i<nCount; i++)
		{
			const tstring sEmoVer = pList[i];
			if (ParseString(sEmoVer,",",true,pVerList)==2)
			{
				const mycp::bigint nEmoId = cgc_atoi64(pVerList[0].c_str());
				const int nVersion = atoi(pVerList[1].c_str());

				SetLocalEmoVer(nEmoId,nVersion);
			}
		}
	}
}
void CUserManagerApp::OnFUMIUMsg(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const EB_MSG_TYPE nMsgType = (EB_MSG_TYPE)pSotpResponseInfo->m_pResponseList.getParameterValue("msg-type", (int)EB_MSG_UNKNOWN);
	if (nMsgType==EB_MSG_UNKNOWN)
	{
		const int nEmoCount = pSotpResponseInfo->m_pResponseList.getParameterValue("emo-count",(int)-1);
		const tstring sEmoVerString = pSotpResponseInfo->m_pResponseList.getParameterValue("emo_ver");
		if (nEmoCount>=0)	// ??? 1.23.0.506 兼容旧版本，后期再删除；
		{
			ProcessEmotionCount(nEmoCount, sEmoVerString);
			return;
		}
		//if (!sEmoVerString.empty())
		//{
		//	int nLocalEmoCount = -1;
		//	BoostReadLock rdLock(m_mutexEBDatas);
		//	if (this->m_pEBDatas.get()!=0)
		//	{
		//		tstring sEmoList;
		//		char lpszBuffer[512];
		//		std::vector<tstring> pList;
		//		std::vector<tstring> pVerList;
		//		const int nCount = ParseString(sEmoVerString.c_str(),";",false,pList);
		//		for (int i=0; i<nCount; i++)
		//		{
		//			const tstring sEmoVer = pList[i];
		//			if (ParseString(sEmoVer.c_str(),",",true,pVerList)==2)
		//			{
		//				const mycp::bigint nEmoId = cgc_atoi64(pVerList[0].c_str());
		//				const int nVersion = atoi(pVerList[1].c_str());
		//				if (sEmoList.empty())
		//					sprintf(lpszBuffer,"%lld",nEmoId);
		//				else
		//					sprintf(lpszBuffer,",%lld",nEmoId);
		//				sEmoList.append(lpszBuffer);
		//			}
		//		}

		//		sprintf(lpszBuffer,"SELECT count(emo_id) FROM eb_emo_info_t WHERE emo_id IN (%s)",sEmoList.c_str());
		//		int nCookie = 0;
		//		m_pEBDatas->select(lpszBuffer, nCookie);
		//		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		//		if (pRecord.get()!=0)
		//		{
		//			nLocalEmoCount = pRecord->getVector()[0]->getIntValue();
		//			m_pEBDatas->reset(nCookie);
		//		}
		//	}

		//	if (nLocalEmoCount>=nEmoCount)
		//	{
		//		std::vector<tstring> pList;
		//		std::vector<tstring> pVerList;
		//		const int nCount = ParseString(sEmoVerString.c_str(),";",false,pList);
		//		for (int i=0; i<nCount; i++)
		//		{
		//			const tstring sEmoVer = pList[i];
		//			if (ParseString(sEmoVer.c_str(),",",true,pVerList)==2)
		//			{
		//				const mycp::bigint nEmoId = cgc_atoi64(pVerList[0].c_str());
		//				const int nVersion = atoi(pVerList[1].c_str());
		//				SetLocalEmoVer(nEmoId,nVersion);
		//			}
		//		}
		//	}
		//}
		//return;
	}
	const mycp::bigint nMsgId = pSotpResponseInfo->m_pResponseList.getParameterValue("msg_id",(mycp::bigint)0);
	const tstring sFromAccount = pSotpResponseInfo->m_pResponseList.getParameterValue("from");
	const mycp::bigint nFromUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("from_uid",(mycp::bigint)0);
	const int nMsgSubType = pSotpResponseInfo->m_pResponseList.getParameterValue("msg-subtype", (int)0);
	const tstring sMsgName = pSotpResponseInfo->m_pResponseList.getParameterValue("msg-name");
	const tstring sMsgContent = pSotpResponseInfo->m_pResponseList.getParameterValue("msg-content");
	const tstring sMsgTime = pSotpResponseInfo->m_pResponseList.getParameterValue("msg-time");
	const mycp::bigint sDepCode = pSotpResponseInfo->m_pResponseList.getParameterValue("dep-code",(mycp::bigint)0);
	const tstring sFromInfo = pSotpResponseInfo->m_pResponseList.getParameterValue("f-info");
	CEBAppMsgInfo::pointer pMsgInfo = CEBAppMsgInfo::create(nMsgId,nMsgType, sMsgContent);
	pMsgInfo->SetMsgSubType(nMsgSubType);
	pMsgInfo->SetMsgName(sMsgName);
	pMsgInfo->m_sMsgTime = sMsgTime;
	pMsgInfo->m_sFromAccount = sFromAccount;
	pMsgInfo->m_nFromUserId = nFromUserId;
	pMsgInfo->m_sGroupCode = sDepCode;
	pMsgInfo->m_sFromInfo = sFromInfo;
	bool bAutoAck = true;
	EB_CALL_ACK_TYPE nAckType = EB_CAT_UNKNOWN;
	ProcessP2PMsgInfo(pMsgInfo,bAutoAck,nAckType,pReqeustInfo);
	if (bAutoAck && nMsgId>0)
	{
		UmMsgAck(nMsgId, nAckType);
	}
}
void CUserManagerApp::OnUMSMOfflineResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	m_userStatus = US_Logout;
	m_tReLoadInfo = 0;
	if (m_callback)
		m_callback->onLogout();
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_LOGOUT);
//		pEvent->SetQEventType((QEvent::Type)EB_WM_LOGOUT);
        try {
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
        }catch (...) {
            int i=0;
        }
#else
		::PostMessage(m_pHwnd, EB_WM_LOGOUT, 0, 0);
#endif
	}
}

void CUserManagerApp::DeleteOldHeadFile(const CEBMemberInfo::pointer& pEmployeeInfo)
{
	if (pEmployeeInfo->m_sHeadResourceId>0)
	{
		CEBEmotionInfo::pointer pEmotionInfo = GetEmotionInfo(pEmployeeInfo->m_sHeadResourceId);
		if (pEmotionInfo.get()==NULL)
		{
			// 不属于系统头像
			const mycp::bigint sTempResourceId(pEmployeeInfo->m_sHeadResourceId);
			pEmployeeInfo->m_sHeadResourceId = 0;
			CEBMemberInfo::pointer pTemoEmployeeInfo = GetEmployeeInfo2(sTempResourceId);
			if (pTemoEmployeeInfo.get()==NULL)
			{
				// 不是其他成员头像，删除该头像文件
#ifdef _QT_MAKE_
                if (!QFile::remove(pEmployeeInfo->m_sHeadResourceFile) && QFileInfo::exists(pEmployeeInfo->m_sHeadResourceFile)) {
#else
				if (!DeleteFileA(pEmployeeInfo->m_sHeadResourceFile.c_str()) && ::PathFileExistsA(pEmployeeInfo->m_sHeadResourceFile.c_str())) {
#endif
					m_pExit2DeleteFileList.push_back(pEmployeeInfo->m_sHeadResourceFile);
				}
			}
		}
	}
}
void CUserManagerApp::DeleteOldHeadFile(mycp::bigint nHeadResourceId)
{
	if (nHeadResourceId==0) return;
	CEBEmotionInfo::pointer pEmotionInfo = GetEmotionInfo(nHeadResourceId);
	if (pEmotionInfo.get()!=NULL) return;				// 系统头像

	// 不属于系统头像
	CEBMemberInfo::pointer pTemoEmployeeInfo = GetEmployeeInfo2(nHeadResourceId);
	if (pTemoEmployeeInfo.get()!=NULL) return;	// 其他成员头像
    /// 不是其他成员头像，删除该头像文件
#ifdef _QT_MAKE_
    const EBFileString lpszHeadResourceFile = QString("%1/%2").arg(m_sEbResourcePath).arg(nHeadResourceId);
	if (!QFile::remove(lpszHeadResourceFile) && QFileInfo::exists(lpszHeadResourceFile)) {
#else
    char lpszHeadResourceFile[260];
    sprintf(lpszHeadResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),nHeadResourceId);
	if (!DeleteFileA(lpszHeadResourceFile) && ::PathFileExistsA(lpszHeadResourceFile)) {
#endif
		m_pExit2DeleteFileList.push_back(lpszHeadResourceFile);
	}
}

CEBEmotionInfo::pointer CUserManagerApp::GetEmotionInfo(mycp::bigint sResourceId) const
{
	AUTO_CONST_RLOCK(theEmotionList);
	CLockMap<tstring,CEBEmotionInfo::pointer>::const_iterator pIter = theEmotionList.begin();
	for (; pIter!=theEmotionList.end(); pIter++)
	{
		if (pIter->second->m_sResId == sResourceId)
		{
			return pIter->second;
		}
	}
	return NullEBEmotionInfo;
}

bool CUserManagerApp::BuildGroupCallUserList(const CEBCallInfo::pointer& pCallInfo, mycp::bigint sGroupCode) const
{
	pCallInfo->m_sGroupCode = sGroupCode;
	if (pCallInfo->m_sGroupCode==0) return false;
	CEBGroupInfo::pointer pDepartmentInfo;
	if (theDepartmentList.find(sGroupCode, pDepartmentInfo))
	{
		// 加载群成员
		BoostReadLock lock(pDepartmentInfo->m_pMemberList.mutex());
		CLockMap<mycp::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
		for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
		{
			const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
			const mycp::bigint nEmpUserId = pEmployeeInfo->m_nMemberUserId;
			if (!pCallInfo->m_pCallUserList.exist(nEmpUserId))
			{
				CEBAccountInfo::pointer pEmpAccountInfo = CEBAccountInfo::create(nEmpUserId,pEmployeeInfo->m_sMemberAccount);
				pEmpAccountInfo->m_pFromCardInfo.m_nAccountType = EB_ACCOUNT_TYPE_IN_ENT;
				pEmployeeInfo->GetECardInfo(&pEmpAccountInfo->m_pFromCardInfo);
				pCallInfo->m_pCallUserList.insert(nEmpUserId, pEmpAccountInfo);
			}
		}
		return true;
	}
	return false;
}
class CBigInfo
{
public:
	typedef boost::shared_ptr<CBigInfo> pointer;
	static CBigInfo::pointer create(void)
	{
		return CBigInfo::pointer(new CBigInfo());
	}
	CLockMap<mycp::bigint,bool> theList;
	CBigInfo(void)
	{}
	virtual ~CBigInfo(void)
	{}
};

void CUserManagerApp::SaveLocalEmotionInfo(const CEBEmotionInfo::pointer& pEmotionInfo)
{
	BoostReadLock rdLock(m_mutexEBDatas);
	if (m_pEBDatas.get()!=NULL)
	{
        /// 保存本地数据库
		tstring sDescription(pEmotionInfo->m_sDescription);
		CSqliteCdbc::escape_string_in(sDescription);
		char lpszBuffer[1024*2];
#ifdef _QT_MAKE_
        const std::string sDescriptionString(sDescription.string());
#else
		const std::string sDescriptionString(CEBParseSetting::str_convert(sDescription.c_str(),CP_ACP,CP_UTF8));
#endif
		sprintf(lpszBuffer,"REPLACE INTO eb_emo_info_t(emo_id,res_id,nindex,ntype,ent_id,res_type,cm_server,res_md5,desc,cm_sid) VALUES(%lld,%lld,%d,%d,%lld,%d,'%s','%s','%s',%d)",
						   pEmotionInfo->m_nEmoId,pEmotionInfo->m_sResId,pEmotionInfo->m_nIndex,pEmotionInfo->m_nType,pEmotionInfo->m_sEnterpriseCode,
						   pEmotionInfo->m_nResType,pEmotionInfo->m_sCmServer.c_str(),pEmotionInfo->m_sResMd5.c_str(),sDescriptionString.c_str(),
						   pEmotionInfo->m_nCmServiceId);
		m_pEBDatas->execute(lpszBuffer);
	}
}
void CUserManagerApp::SetLocalEmoVer(mycp::bigint nEmoId,int nVersion)
{
	BoostReadLock rdLock(m_mutexEBDatas);
	if (m_pEBDatas.get()!=NULL)
	{
		// 保存本地数据库
		char lpszBuffer[512];
		sprintf(lpszBuffer,"REPLACE INTO eb_emo_ver_t(emo_id,ver) VALUES(%lld,%d)",nEmoId,nVersion);
		m_pEBDatas->execute(lpszBuffer);
	}
}
int CUserManagerApp::GetLocalEmoVer(mycp::bigint nEmoId) const
{
	int nVersion = -1;
	//BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_mutexEBDatas));
	if (m_pEBDatas.get()!=NULL)
	{
		char lpszBuffer[512];
		sprintf(lpszBuffer,"SELECT ver FROM eb_emo_ver_t WHERE emo_id=%lld",nEmoId);
		int nCookie = 0;
		m_pEBDatas->select(lpszBuffer, nCookie);
		cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
		if (pRecord.get()!=0)
		{
			nVersion = pRecord->getVector()[0]->getIntValue();
		}
		m_pEBDatas->reset(nCookie);
	}
	return nVersion;
}
inline unsigned char fromHex(const unsigned char &x)
{
	return isdigit(x) ? x-'0' : x-'A'+10;
}
//inline tstring URLDecode(const char *sIn)
//{
//	tstring sOut;
//	const size_t nLen = strlen(sIn);
//	for( size_t ix = 0; ix < nLen; ix++ )
//	{
//		unsigned char ch = 0;
//		if(sIn[ix]=='%')
//		{
//			ch = (fromHex(sIn[ix+1])<<4);
//			ch |= fromHex(sIn[ix+2]);
//			ix += 2;
//		}
//		else if(sIn[ix] == '+')
//		{
//			ch = ' ';
//		}
//		else
//		{
//			ch = sIn[ix];
//		}
//		sOut += (char)ch;
//	}
//	return sOut;
//}
inline tstring EBDecode1(const char *sIn)
{
	tstring sOut;
	const size_t nLen = strlen(sIn);
	for( size_t ix = 0; ix < nLen; ix++ )
	{
		unsigned char ch = 0;
		if (sIn[ix]=='/')
		{
			if (sIn[ix+1]=='/')
			{ 
				sOut += (char)ch;
				ix += 1;
				continue;
			}
			ch = (fromHex(sIn[ix+1])<<4);
			ch |= fromHex(sIn[ix+2]);
			ix += 2;
		}
		//else if(sIn[ix] == '+')
		//{
		//	ch = ' ';
		//}
		else
		{
			ch = sIn[ix];
		}
		sOut += (char)ch;
	}
	return sOut;
}
void CUserManagerApp::ProcessP2PMsgInfo(const CEBAppMsgInfo::pointer& pMsgInfo,bool & pOutAutoAck,EB_CALL_ACK_TYPE& pOutAckType,const CPOPSotpRequestInfo::pointer & pSotpRequestInfo)
{
	// 有些在前面做
	pOutAutoAck = true;
	switch ((int)pMsgInfo->GetMsgType())
	{
	case EB_MSG_FILE:
	case EB_MSG_RICH:
		{
			std::vector<tstring> pList;
			if (ParseString(pMsgInfo->GetMsgContent(),";",false,pList)!=5)
			{
				return;
			}
			const mycp::bigint s_dep_code = pMsgInfo->m_sGroupCode;
			const tstring s_s_info = pMsgInfo->m_sFromInfo;
			const mycp::bigint nFromUserId = pMsgInfo->m_nFromUserId;
			const tstring s_sender(pMsgInfo->m_sFromAccount);
			const mycp::bigint s_callid = cgc_atoi64(pList[0].c_str());
			const mycp::bigint s_chatid = cgc_atoi64(pList[1].c_str());
			const tstring sUmServer(pList[2]);
			//const tstring sUmAppName = pList[3];
			const tstring sCallUmKey(pList[4]);

			//tstring s_chatid = s_sender;					// 实现，同发送用户，同一台服务器，同一台窗口接收
			const mycp::bigint sCallId = s_callid==0?s_chatid:s_callid;	// callid=chatid，用于在离线情况下，另外有人上线，可以进入同一个呼叫
			CEBCallInfo::pointer pCallInfo;
			//// 这里判断是否存在指定CALLINFO
			//EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
			//CEBCallInfo::pointer pInCallInfo = GetCallInfoByUser(s_sender,pOutLineState);	// 查找在线用户，不在线无效，需要重新邀请
			//if (pInCallInfo.get() != NULL && pOutLineState == EB_LINE_STATE_ONLINE_NEW &&
			//	(pInCallInfo->m_nCallState == EB_CALL_STATE_EXIT || pInCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED))
			//{
			//	// 呼叫或者通话中...
			//	EnterRoom(pInCallInfo);
			//	SendConnectMsg(pInCallInfo,s_sender,EB_CONNECTED_OWNER_CALL);
			//	return ;
			//}
			CEBAccountInfo::pointer pFromAccountInfo;
			if (!theCallInfoList.find(sCallId, pCallInfo))
			{
				pFromAccountInfo = CEBAccountInfo::create(nFromUserId,s_sender);
				//pFromAccountInfo->m_sFromInfo = s_s_info;
				pCallInfo = CEBCallInfo::create(sCallId,pMsgInfo->m_sGroupCode,pFromAccountInfo);
				theCallInfoList.insert(sCallId, pCallInfo);
				pCallInfo->m_pCallUserList.insert(nFromUserId, pFromAccountInfo);
			}else if (!pCallInfo->m_pCallUserList.find(nFromUserId, pFromAccountInfo))
			{
				return;
			}
            const tstring sMyUmAddress = m_pUserManager->GetAddress().address();
            const bool bUMServerChange = (!sUmServer.empty() && sUmServer!=pCallInfo->m_sUmServer && sUmServer!=sMyUmAddress)?true:false;
            if (bUMServerChange)
                pCallInfo->m_sUmServer = sUmServer;
//			pCallInfo->m_sUmServer = sUmServer;
            if (pCallInfo->m_sUmServer.empty())
                pCallInfo->m_sUmServer = sMyUmAddress;

			//pCallInfo->m_sUmAppName = sUmAppName;
			GetECardByFromInfo(s_s_info,&pFromAccountInfo->m_pFromCardInfo);
			pFromAccountInfo->m_sFromInfo = s_s_info;
			CEBAccountInfo::pointer pToAccountInfo;
			if (!pCallInfo->m_pCallUserList.find(m_pMyAccountInfo->GetUserId(), pToAccountInfo))
			{
				pToAccountInfo = CEBAccountInfo::create(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetAccount());
				pCallInfo->m_pCallUserList.insert(m_pMyAccountInfo->GetUserId(), pToAccountInfo);
			}
			pCallInfo->m_bOffLineCall = true;
			pCallInfo->m_sChatId = s_chatid;

			if (s_dep_code>0)
			{
				BuildGroupCallUserList(pCallInfo,s_dep_code);
			}

			// rich & file msg
			CPOPCUserManager::pointer pPOPCUserManager;
			//if (m_pUserManager.get() != NULL && (m_pUserManager->GetAddress().address() == pCallInfo->m_sUmServer) || pCallInfo->m_sUmServer.empty())
			//	pPOPCUserManager = m_pUserManager;
			//else// if (!theUserManagerList.find(sCallId, pPOPCUserManager))
			{
				bool bRetry = false;
				pPOPCUserManager = CPOPCUserManager::create(this,0);
				pPOPCUserManager->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
				int ret = pPOPCUserManager->Start(pCallInfo->m_sUmServer, pCallInfo->m_sUmAppName,0,true,true,1);
				if (ret==-1)
				{
					// 域名解析问题，再试一次；
					const tstring sServerAddress = GetAddressList(pCallInfo->m_sUmServer);
					if (!sServerAddress.empty())
					{
						pPOPCUserManager.reset();
						pPOPCUserManager = CPOPCUserManager::create(this,0);
						pPOPCUserManager->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
						ret = pPOPCUserManager->Start(sServerAddress,pCallInfo->m_sUmAppName,0,true,true,1);
						bRetry = true;
					}
				}
				if (ret != 0)
				{
					EB_CallInfo * pEbCallInfo = new EB_CallInfo(nFromUserId,s_sender,s_callid,pCallInfo->m_sGroupCode);
					if (m_callback)
						m_callback->onCallError(*pEbCallInfo,EB_STATE_ERROR);
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
						pEbCallInfo->SetQEventType((QEvent::Type)EB_WM_CALL_ERROR);
						pEbCallInfo->SetEventParameter((long)EB_STATE_ERROR);
                        QCoreApplication::postEvent(m_pHwnd, pEbCallInfo, thePostEventPriority);
#else
						::SendMessage(m_pHwnd, EB_WM_CALL_ERROR, (WPARAM)pEbCallInfo, EB_STATE_ERROR);
						delete pEbCallInfo;
#endif
					}
					else {
						delete pEbCallInfo;
					}
					return;
				}
				pPOPCUserManager->SetUserId(this->m_pMyAccountInfo->GetUserId());
				if (!bRetry)
					SaveAddressList(pCallInfo->m_sUmServer);
			}
			theUserManagerList.insert(sCallId, pPOPCUserManager);	// 统一放在这里
			CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
			pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", nFromUserId));
			pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-account", s_sender));
			//pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("owner-call", false));	// true
			if (pCallInfo->m_sGroupCode==0)
			{
				// 主要用于进入会话后，呼叫对方，（目前同群组企业才自动呼叫，其他发消息前再呼叫）
				if (pFromAccountInfo->m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_IN_ENT)
					pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("invite-call", true));
				pPOPCUserManager->SendUMIUEnter(sCallId, sCallUmKey,s_dep_code,pRequestInfo);
			}else
			{
				// 群组会话，在呼叫成功后，再进入会话
				pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("um-key", sCallUmKey));
				pPOPCUserManager->SendUMIUUser(0,0,pCallInfo,0,pRequestInfo);
			}
			return;
		}break;
	case EB_MSG_SUBSCRIBE_FUNC_INFO:
		{
			const int nFunctionCount = SplitIntValue(pMsgInfo->GetMsgContent(), "func_info_size=", ";",-1);
			if (nFunctionCount >= 0)
			{
				// 结束；
				//MessageBox(NULL,pMsgInfo->GetMsgContent().c_str(),"",MB_OK);
				SplitString(pMsgInfo->GetMsgContent(), "func_url=", ";", m_pFuncUrl);
				this->m_nGroupMsgSubId = SplitBigIntValue(pMsgInfo->GetMsgContent(), "group_msg_subid=", ";",0);
				this->m_nFindAppSubId = SplitBigIntValue(pMsgInfo->GetMsgContent(), "find_app_subid=", ";",0);
				this->m_nMyCollectionSubId = SplitBigIntValue(pMsgInfo->GetMsgContent(), "my_collection_subid=", ";",0);
				break;
			}
			const mycp::bigint nSubscribeId = SplitBigIntValue(pMsgInfo->GetMsgContent(), "sub_id=", ";",0);
			//const mycp::bigint nFunctionId = SplitBigIntValue(pMsgInfo->GetMsgContent(), "func_id=", ";",0);
			if (nSubscribeId==0)
				break;

			CEBSubscribeFuncInfo::pointer pSubscrieFuncInfo;
			if (!theSubscribeFuncInfolist.find(nSubscribeId,pSubscrieFuncInfo))
			{
				pSubscrieFuncInfo = CEBSubscribeFuncInfo::create(nSubscribeId);
				theSubscribeFuncInfolist.insert(nSubscribeId,pSubscrieFuncInfo);
			}
			pSubscrieFuncInfo->m_nFromSubscribeId = SplitBigIntValue(pMsgInfo->GetMsgContent(), "fsub_id=", ";",0);
			//pSubscrieFuncInfo->m_nSubscribeLevel = SplitIntValue(pMsgInfo->GetMsgContent(), "sub_level=", ";",0);
			pSubscrieFuncInfo->m_nDisplayIndex = SplitIntValue(pMsgInfo->GetMsgContent(), "display_index=", ";",0);
			pSubscrieFuncInfo->m_nIconResId = SplitBigIntValue(pMsgInfo->GetMsgContent(), "icon_res_id=", ";",0);
			pSubscrieFuncInfo->m_nLocation = SplitIntValue(pMsgInfo->GetMsgContent(), "location=", ";",0);
			tstring sFunctionName;
			SplitString(pMsgInfo->GetMsgContent(), "func_name=", ";", sFunctionName);
			pSubscrieFuncInfo->m_sFunctionName = sFunctionName;
			pSubscrieFuncInfo->m_nFunctionMode = (EB_FUNC_MODE)SplitIntValue(pMsgInfo->GetMsgContent(), "func_mode=", ";",0);
			pSubscrieFuncInfo->m_nFunctionParam = (EB_FUNC_PARAM)SplitIntValue(pMsgInfo->GetMsgContent(), "func_param=", ";",(int)EB_FUNC_PARAM_NONE);
			pSubscrieFuncInfo->m_nWindowWidth = SplitIntValue(pMsgInfo->GetMsgContent(), "win_w=", ";",0);
			pSubscrieFuncInfo->m_nWindowHeight = SplitIntValue(pMsgInfo->GetMsgContent(), "win_h=", ";",0);
			//if (pSubscrieFuncInfo->m_nFunctionMode == EB_FUNC_MODE_WINDOW ||
			//	pSubscrieFuncInfo->m_nFunctionMode == EB_FUNC_MODE_MODAL)
			{
				if (pSubscrieFuncInfo->m_nWindowWidth==0)
					pSubscrieFuncInfo->m_nWindowWidth = 480;
				if (pSubscrieFuncInfo->m_nWindowHeight==0)
					pSubscrieFuncInfo->m_nWindowHeight = 420;
			}
			if (pSubscrieFuncInfo->m_nFunctionMode == EB_FUNC_MODE_PROGRAM)
			{
				tstring sFunctionUrl;
				SplitString(pMsgInfo->GetMsgContent(), "func_url=", ";", sFunctionUrl);
				pSubscrieFuncInfo->m_sFunctionUrl = sFunctionUrl;
			}
			tstring sDescription;
			SplitString(pMsgInfo->GetMsgContent(), "desc=", ";", sDescription);
			pSubscrieFuncInfo->m_sDescription = EBDecode1(sDescription.c_str());
			const int nFuncExt = SplitIntValue(pMsgInfo->GetMsgContent(), "func_ext=", ";",(int)EB_FUNC_EXT_DISABLE_CONTEXTMENU);
			pSubscrieFuncInfo->m_bDisableContextMenu = (nFuncExt&EB_FUNC_EXT_DISABLE_CONTEXTMENU)==EB_FUNC_EXT_DISABLE_CONTEXTMENU?true:false;
			pSubscrieFuncInfo->m_bDisableScrollBar = (nFuncExt&EB_FUNC_EXT_DISABLE_SCROLLBAR)==EB_FUNC_EXT_DISABLE_SCROLLBAR?true:false;
			pSubscrieFuncInfo->m_bClosePrompt = (nFuncExt&EB_FUNC_EXT_CLOSE_PROMPT)==EB_FUNC_EXT_CLOSE_PROMPT?true:false;
			pSubscrieFuncInfo->m_bWinResizable = (nFuncExt&EB_FUNC_EXT_WIN_RESIZABLE)==EB_FUNC_EXT_WIN_RESIZABLE?true:false;
			if ((nFuncExt&EB_FUNC_EXT_BROWSER_SET_CEF)==EB_FUNC_EXT_BROWSER_SET_CEF)
				pSubscrieFuncInfo->m_nBrowserType = EB_FUNC_BROWSER_TYPE_CEF;
			else if ((nFuncExt&EB_FUNC_EXT_BROWSER_SET_IE)==EB_FUNC_EXT_BROWSER_SET_IE)
				pSubscrieFuncInfo->m_nBrowserType = EB_FUNC_BROWSER_TYPE_IE;
			else
				pSubscrieFuncInfo->m_nBrowserType = EB_FUNC_BROWSER_TYPE_DEFAULT;
			tstring sResMd5;
			SplitString(pMsgInfo->GetMsgContent(), "res_md5=", ";", sResMd5);

			if (pSubscrieFuncInfo->m_nIconResId > 0)
			{
				tstring sIconCmServer;
				SplitString(pMsgInfo->GetMsgContent(), "icon_cm_server=", ";", sIconCmServer);
				if (!sIconCmServer.empty())
				{
#ifdef _QT_MAKE_
                    const EBFileString sResourceFile = QString("%1/%2").arg(m_sEbResourcePath).arg(pSubscrieFuncInfo->m_nIconResId);
                    if (QFileInfo::exists(sResourceFile))
#else
                    char sResourceFile[260];
                    sprintf(sResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),pSubscrieFuncInfo->m_nIconResId);
                    if (::PathFileExistsA(sResourceFile))
#endif
					{
						if (!sResMd5.empty())
						{
                            /// 检查文件MD5是否相同；
							mycp::bigint nFileSize = 0;
							tstring sFileMd5String;
							GetFileMd5(sResourceFile,nFileSize,sFileMd5String);
							if (sFileMd5String==sResMd5)
							{
								pSubscrieFuncInfo->m_sResFile = sResourceFile;
							}else
							{
								// 删除后重新下载
#ifdef _QT_MAKE_
                                QFile::remove(sResourceFile);
#else
                                DeleteFileA(sResourceFile);
#endif
							}
						}else
						{
							pSubscrieFuncInfo->m_sResFile = sResourceFile;
						}
					}
#ifdef _QT_MAKE_
                    if (pSubscrieFuncInfo->m_sResFile.isEmpty())
#else
					if (pSubscrieFuncInfo->m_sResFile.empty())
#endif
					{
						//pEmotionInfo->m_sResFile = sResourceFile;	// 下载成功再保存
						// 资源文件不存在（或MD5不正确），请求下载
						const tstring sCmServerKey = GetCmServerKey(sIconCmServer);
						Cchatroom::pointer pChatRoom;
						if (!theChatRoomList.find(sIconCmServer,pChatRoom))
						{
							pChatRoom = Cchatroom::create();
							pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
							pChatRoom->Start(sIconCmServer.c_str(), "",0,true,1);
							pChatRoom->SetCallBack(this);
							theChatRoomList.insert(sIconCmServer,pChatRoom);
						}
						theResourceChatRoom.insert(pSubscrieFuncInfo->m_nIconResId,pChatRoom);
						CFilePathInfo::pointer pFilePathInfo = CFilePathInfo::create(CFilePathInfo::FILE_FUNC_IMAGE,sResourceFile,false);
						pFilePathInfo->m_nBigData = nSubscribeId;
						theResourceFilePath.insert(pSubscrieFuncInfo->m_nIconResId,pFilePathInfo);
						//pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
						//pChatRoom->RecviveResource(pSubscrieFuncInfo->m_nIconResId,sResourceFile);
						//Sleep(50);
						CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE);
						pProcessMsgInfo->m_pChatRoom = pChatRoom;
						pProcessMsgInfo->m_nBigInt1 = pSubscrieFuncInfo->m_nIconResId;
						pProcessMsgInfo->m_sString1 = sResourceFile;
						//pProcessMsgInfo->m_sString1.append(".ebtemp");
						m_pProcessMsgList.add(pProcessMsgInfo);
					}
				}
			}
		}break;
	case EB_MSG_ENTGROUP_VER_INFO:
		{
			// 企业部门版本信息；
			// msg-name=版本号；格式ver,size
			// msg-content=版本列表；格式groupid:ver;...
			if (m_pDefaultEnterpriseInfo.get()==0) {
				break;
			}
			bool bNeedLoadGroupInfo = true;
			bool bFirstData = false;
			//static int theServerEntDepInfoSize = 0;
			static CLockMap<mycp::bigint,bool> theServerEntDepList;
			if (m_nServerEntDepInfoVer==-1) {
				std::vector<tstring> pList;
				if (ParseString(pMsgInfo->GetMsgName(),",",false,pList)!=2) {
					return;
				}
				m_nServerEntDepInfoVer = cgc_atoi64(pList[0].c_str());
				theServerEntDepInfoSize = atoi(pList[1].c_str());
				//m_nEntDepInfoSize = atoi(pList[1].c_str());
				theServerEntDepList.clear();

				// *判断企业部门资料版本号
				const mycp::bigint nLocalEntDepInfoVer = GetLocalEntGroupInfoVer(m_pDefaultEnterpriseInfo->m_sEnterpriseCode);
				if (nLocalEntDepInfoVer==-1)
				{
					// -1是新加载数据，只需要加一次企业信息，部门信息，会自动下载加载一次
					bNeedLoadGroupInfo = false;
					bFirstData = true;
					EnterpriseLoad(0,1,0,0);	// 会加载二次，但会快
					//EnterpriseLoad(0,0,0,0);	// 只会加载一次，但有点慢
				}else if (nLocalEntDepInfoVer!=m_nServerEntDepInfoVer)
				{
					// 版本不对，需要重新加载企业所有组织结构部门资料；
					EnterpriseLoad(0,1,0,0);
				}else
				{
					m_nServerEntDepInfoVer = -1;
					// 这里加载一次所有企业部门在线人数；
					LoadInfo(0,0,0,0,1);
				}
			}
			std::vector<tstring> pGroupList;
			ParseString(pMsgInfo->GetMsgContent(),";",false,pGroupList);
			for (size_t i=0;i<pGroupList.size();i++)
			{
				const tstring sGroupVerInfo = pGroupList[i];
				// 取出群组版本信息 groupid:groupver
				std::vector<tstring> pList;
				if (ParseString(sGroupVerInfo,":",false,pList)!=2)
				{
					continue;
				}
				const mycp::bigint nGroupId = cgc_atoi64(pList[0].c_str());
				const mycp::bigint nGroupVer = cgc_atoi64(pList[1].c_str());
				//if (nGroupId==999004)
				//{
				//	int i=0;
				//}
				if (bFirstData)
					theCheckGroupInfoList.insert(nGroupId,true,false);
				else
					LoadLocalDepInfo(nGroupId,nGroupVer,bNeedLoadGroupInfo);
				theServerEntDepList.insert(nGroupId,true);
			}
			if ((int)theServerEntDepList.size()>=theServerEntDepInfoSize)
			{
				BoostReadLock rdLock(m_mutexEBDatas);
				if (m_pEBDatas.get()!=NULL && m_pDefaultEnterpriseInfo.get()!=NULL)
				{
					// 这里处理删除本地企业部门
					char lpszBuffer[128];
					sprintf(lpszBuffer,"SELECT group_id FROM eb_dep_info_t WHERE ent_id=%lld",m_pDefaultEnterpriseInfo->m_sEnterpriseCode);
					int nCookie = 0;
					m_pEBDatas->select(lpszBuffer, nCookie);
					CEBGroupInfo::pointer pDepartmentInfo;
					cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
					while (pRecord.get()!=0)
					{
						const mycp::bigint nLocalGroupId = pRecord->getVector()[0]->getBigIntValue();
						// *** 在这里判断删除本地无用部门数据
						if (!theServerEntDepList.exist(nLocalGroupId))
						{
							// 删除部门资料
							sprintf(lpszBuffer,"DELETE FROM eb_dep_info_t WHERE group_id=%lld",nLocalGroupId);
							m_pEBDatas->execute(lpszBuffer);
							// 删除部门成员
							sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE group_id=%lld",nLocalGroupId);
							m_pEBDatas->execute(lpszBuffer);
						}
						pRecord = m_pEBDatas->next(nCookie);
					}
					m_pEBDatas->reset(nCookie);
				}
				theServerEntDepList.clear();
			}
		}break;
	case EB_MSG_MYGROUP_VER_INFO:
		{
			// 个人群组企业信息；
			// msg-name=版本号；格式ver,size
			// msg-content=版本列表；格式groupid:ver;...
			if (m_pMyAccountInfo.get()==NULL)
			{
				break;
			}
			bool bNeedLoadGroupInfo = true;
			static int theServerMyDepInfoSize = 0;
			static CLockMap<mycp::bigint,bool> theServerMyDepInfoList;
			if (m_nServerMyDepInfoVer==-1)
			{
				std::vector<tstring> pList;
				if (ParseString(pMsgInfo->GetMsgName(),",",false,pList)!=2)
				{
					return;
				}
				m_nServerMyDepInfoVer = cgc_atoi64(pList[0].c_str());
				theServerMyDepInfoSize = atoi(pList[1].c_str());
				//m_nMyDepInfoSize = atoi(pList[1].c_str());
				theServerMyDepInfoList.clear();

				// *判断我的个人群组资料版本号
				//const mycp::bigint nLocalMyGroupInfoVer = GetLocalMyGroupInfoVer(m_pMyAccountInfo->GetUserId());
				//if (nLocalMyGroupInfoVer!=-1 && nLocalMyGroupInfoVer!=m_nServerMyDepInfoVer)
				if (GetLocalMyGroupInfoVer(m_pMyAccountInfo->GetUserId())!=m_nServerMyDepInfoVer)
				{
					// 加载
					bNeedLoadGroupInfo = false;
					EnterpriseLoad(0,0,1,0);	// 加载所有我的个人群组
				}else
				{
					m_nServerMyDepInfoVer = -1;
					// 这里加载一次所有个人群组在线人数；
					LoadInfo(0,0,0,0,0,1);
				}
			}
			std::vector<tstring> pGroupList;
			ParseString(pMsgInfo->GetMsgContent(),";",false,pGroupList);
			for (size_t i=0;i<pGroupList.size();i++)
			{
				const tstring sGroupVerInfo = pGroupList[i];
				// 取出群组版本信息 groupid:groupver
				std::vector<tstring> pList;
				if (ParseString(sGroupVerInfo,":",false,pList)!=2)
				{
					continue;
				}
				const mycp::bigint nGroupId = cgc_atoi64(pList[0].c_str());
				const mycp::bigint nGroupVer = cgc_atoi64(pList[1].c_str());
				//if (nGroupId==999004)
				//{
				//	int i=0;
				//}
				LoadLocalDepInfo(nGroupId,nGroupVer,bNeedLoadGroupInfo);
				theServerMyDepInfoList.insert(nGroupId,true);
			}
			if ((int)theServerMyDepInfoList.size()>=theServerMyDepInfoSize)
			{
				BoostReadLock rdLock(m_mutexEBDatas);
				if (m_pEBDatas.get()!=NULL)
				{
					// 这里处理删除本地个人群组
					char lpszBuffer[128];
					sprintf(lpszBuffer,"SELECT group_id FROM eb_dep_info_t WHERE ent_id=0");
					int nCookie = 0;
					m_pEBDatas->select(lpszBuffer, nCookie);
					CEBGroupInfo::pointer pDepartmentInfo;
					cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
					while (pRecord.get()!=0)
					{
						const mycp::bigint nLocalGroupId = pRecord->getVector()[0]->getBigIntValue();
						// *** 在这里判断删除本地无用群组数据
						if (!theServerMyDepInfoList.exist(nLocalGroupId))
						{
							// 删除部门资料
							sprintf(lpszBuffer,"DELETE FROM eb_dep_info_t WHERE group_id=%lld",nLocalGroupId);
							m_pEBDatas->execute(lpszBuffer);
							// 删除部门成员
							sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE group_id=%lld",nLocalGroupId);
							m_pEBDatas->execute(lpszBuffer);
						}
						pRecord = m_pEBDatas->next(nCookie);
					}
					m_pEBDatas->reset(nCookie);
				}
				theServerMyDepInfoList.clear();
			}
		}break;
	case EB_MSG_GROUP_LINESTATE_COUNT:
		{
			// ** 群组在线人数；
			// **gid=0表示是联系人在线状态
			// msg-name=[e|m|g];size
			// msg-content=group-id:online-size;...
			if (m_pMyAccountInfo.get()==NULL)
			{
				break;
			}
			std::vector<tstring> pGroupList;
			if (ParseString(pMsgInfo->GetMsgContent(),";",false,pGroupList)==0)
			{
				break;
			}
			std::vector<tstring> pList;
			for (size_t i=0; i<pGroupList.size(); i++)
			{
				const tstring sGroupInfo = pGroupList[i];
				if (ParseString(sGroupInfo,":",true,pList)!=2)
				{
					continue;
				}
				const mycp::bigint nGroupId = cgc_atoi64(pList[0].c_str());
				const int nOnlineSize = atoi(pList[1].c_str());
				CEBGroupInfo::pointer pDepartmentInfo;
				if (theDepartmentList.find(nGroupId,pDepartmentInfo))
				{
					if (pDepartmentInfo->m_nOnlineSize!=nOnlineSize)
					{
						if (pDepartmentInfo->m_nEmpCount<nOnlineSize)
						//if (!pDepartmentInfo->m_pMemberList.empty() && pDepartmentInfo->m_pMemberList.size()<nOnlineSize)
						{
							// 预防万一
							// 在线人数比成员多，加载一次；
							pDepartmentInfo->m_nEmpCount = nOnlineSize;
							if (pDepartmentInfo->m_pMemberList.empty())
								EnterpriseLoad(nGroupId,1,1,0);
							else
								EnterpriseLoad(nGroupId,0,0,1);
						}

						pDepartmentInfo->m_nOnlineSize = nOnlineSize;
						const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
						if (m_callback)
							m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
						if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
							EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
							pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
							pEvent->SetEventParameter((long)(bIsMyDepartment?1:0));
                            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
							::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
						}
					}
				}
			}
		}break;
	case EB_MSG_GROUP_MEMBER_LINESTATE:
		{
			// **群组成员（或联系人）在线状态
			// **gid=0表示是联系人在线状态
			// msg-name=gid,size
			// msg-content=uid:state;...
			if (m_pMyAccountInfo.get()==NULL)
			{
				break;
			}
			std::vector<tstring> pList;
			if (ParseString(pMsgInfo->GetMsgName(),",",false,pList)!=2)
			{
				return;
			}
			const mycp::bigint nGroupId = cgc_atoi64(pList[0].c_str());
			const int nMemberSize = atoi(pList[1].c_str());
			CEBGroupInfo::pointer pDepartmentInfo;
			if (nGroupId>0)
			{
				if (!theDepartmentList.find(nGroupId,pDepartmentInfo))
				{
					return;
				}
			}

			static CLockMap<mycp::bigint,CBigInfo::pointer> theGroupLineState;
			CBigInfo::pointer pBigInfo;
			if (!theGroupLineState.find(nGroupId,pBigInfo))
			{
				pBigInfo = CBigInfo::create();
				theGroupLineState.insert(nGroupId,pBigInfo);
			}

			std::vector<tstring> pMemberList;
			ParseString(pMsgInfo->GetMsgContent(),";",false,pMemberList);
			for (size_t i=0;i<pMemberList.size();i++)
			{
				const tstring sOnlineStateInfo = pMemberList[i];
				if (ParseString(sOnlineStateInfo,":",true,pList)!=2)
				{
					continue;
				}
				const mycp::bigint nUserId = cgc_atoi64(pList[0].c_str());
				pBigInfo->theList.insert(nUserId,true);
				const EB_USER_LINE_STATE nLineState = (EB_USER_LINE_STATE)atoi(pList[1].c_str());

				if (nGroupId==0)
				{
					// 联系人在线状；
					CEBContactInfo::pointer pContactInfo = GetContactInfo(nUserId);
					if (pContactInfo.get()!=NULL)
					{
						if (pContactInfo->m_nLineState!=nLineState)
						{
							pContactInfo->m_nLineState = nLineState;
							if (this->m_callback)
								m_callback->onContactStateChange(pContactInfo.get());
							if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
								EB_ContactInfo * pEvent = new EB_ContactInfo(pContactInfo.get());
								pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_STATE_CHANGE);
                                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
								::SendMessage(m_pHwnd, EB_WM_CONTACT_STATE_CHANGE, (WPARAM)(const EB_ContactInfo*)pContactInfo.get(), 0);
#endif
							}
						}
					}else
					{
						// 联系人不存在，加载一次；
						this->ContactQuery(0,nUserId);
					}
				}else
				{
					// 群组成员在线状态
					CEBMemberInfo::pointer pEmployeeInfo;
					if (pDepartmentInfo->m_pMemberList.find(nUserId,pEmployeeInfo))
					{
						if (pEmployeeInfo->m_nLineState != nLineState)
						{
							// *有可能是自己的状态
							pEmployeeInfo->m_nLineState = nLineState;
							const bool nIsMyMember = (bool)(m_pMyAccountInfo->GetUserId() == pEmployeeInfo->m_nMemberUserId);
							if (this->m_callback)
								m_callback->onUserStateChange(pEmployeeInfo.get(),nIsMyMember);
							if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
								EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
								pEvent->SetQEventType((QEvent::Type)EB_WM_USER_STATE_CHANGE);
								pEvent->SetEventParameter((long)(nIsMyMember?1:0));
                                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
								::SendMessage(m_pHwnd, EB_WM_USER_STATE_CHANGE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), nIsMyMember?1:0);
#endif
							}
							//pBigInfo->m_bResendGroupInfo = true;
						}
					}else
					{
						// ?成员不存在，单独加载一次（这里是预防万一）
						this->EnterpriseLoad(nGroupId,0,0,1,0,nUserId);
					}
				}
			}
			// 这里处理离线用户
			if ((int)pBigInfo->theList.size()>=nMemberSize)
			{
				// 全部数据已经返回
				theGroupLineState.remove(nGroupId);
				if (nGroupId==0)
				{
					// 联系人在线状态
					BoostReadLock lockContact(theContactList1.mutex());
					CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList1.begin();
					for (; pIterContact!=theContactList1.end(); pIterContact++)
					{
						const CEBContactInfo::pointer pContactInfo = pIterContact->second;
						if (!pBigInfo->theList.exist(pContactInfo->m_nContactUserId) && pContactInfo->m_nLineState!=EB_LINE_STATE_UNKNOWN)
						{
							pContactInfo->m_nLineState=EB_LINE_STATE_UNKNOWN;
							if (this->m_callback)
								m_callback->onContactStateChange(pContactInfo.get());
							if (m_pHwnd!=NULL ){
#ifdef _QT_MAKE_
								EB_ContactInfo * pEvent = new EB_ContactInfo(pContactInfo.get());
								pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_STATE_CHANGE);
                                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
								::SendMessage(m_pHwnd, EB_WM_CONTACT_STATE_CHANGE, (WPARAM)(const EB_ContactInfo*)pContactInfo.get(), 0);
#endif
							}
						}
					}
				}else
				{
					CEBGroupInfo::pointer pDepartmentInfo;
					if (theDepartmentList.find(nGroupId,pDepartmentInfo))
					{
						if (pDepartmentInfo->m_nOnlineSize!=nMemberSize)
						{
							pDepartmentInfo->m_nOnlineSize = nMemberSize;
							const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
							if (m_callback)
								m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
							if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
								EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
								pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
								pEvent->SetEventParameter((long)(bIsMyDepartment?1:0));
                                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
								::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
							}
						}
					}

					// 群组成员在线状态
					BoostReadLock lockMember(pDepartmentInfo->m_pMemberList.mutex());
					CLockMap<eb::bigint, CEBMemberInfo::pointer>::const_iterator pIter = pDepartmentInfo->m_pMemberList.begin();
					for (; pIter!=pDepartmentInfo->m_pMemberList.end(); pIter++)
					{
						const CEBMemberInfo::pointer& pEmployeeInfo = pIter->second;
						if (!pBigInfo->theList.exist(pEmployeeInfo->m_nMemberUserId))
						{
							if (pEmployeeInfo->m_nLineState!=EB_LINE_STATE_UNKNOWN)
							{
								// 用户已经离线
								pEmployeeInfo->m_nLineState = EB_LINE_STATE_UNKNOWN;
								const bool nIsMyMember = (bool)(m_pMyAccountInfo->GetUserId() == pEmployeeInfo->m_nMemberUserId);
								if (this->m_callback)
									m_callback->onUserStateChange(pEmployeeInfo.get(),nIsMyMember);
								if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
									EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
									pEvent->SetQEventType((QEvent::Type)EB_WM_USER_STATE_CHANGE);
									pEvent->SetEventParameter((long)(nIsMyMember?1:0));
                                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
									::SendMessage(m_pHwnd, EB_WM_USER_STATE_CHANGE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), nIsMyMember?1:0);
#endif
								}
							}
						}
					}
				}
			}
		}break;
	case EB_MSG_AREA_DICT_INFO:
		{
			//msgname=[上级地区代码],[返回数量],[地区字典数据版本号]
			//content=[地区代码]:[地区名称]:[电话区号];... 

			if (m_pMyAccountInfo.get()==NULL)
				break;

			const EB_DICT_TYPE nDictType = pSotpRequestInfo.get()==NULL?EB_DICT_UNKNOWN:(EB_DICT_TYPE)pSotpRequestInfo->m_pRequestList.getParameterValue("type",(int)EB_DICT_UNKNOWN);
			std::vector<tstring> pList;
			if (ParseString(pMsgInfo->GetMsgName(),",",false,pList)!=3)
			{
				return;
			}
			const int nParent = atoi(pList[0].c_str());
			const int nSize = atoi(pList[1].c_str());
			//const mycp::bigint nVersion = cgc_atoi64(pList[2].c_str());
			CEBAreaInfo::pointer pParentAreaInfo;
			if (!theAreaInfoList.find(nParent,pParentAreaInfo))
			{
				pParentAreaInfo = CEBAreaInfo::create(nParent);
				theAreaInfoList.insert(nParent,pParentAreaInfo);
			}
			if (nDictType==EB_DICT_AREA_DICT_LIST)
				pParentAreaInfo->m_nSize = nSize;
			std::vector<tstring> pAreaDictList;
			ParseString(pMsgInfo->GetMsgContent(),";",false,pAreaDictList);
			for (size_t i=0;i<pAreaDictList.size();i++)
			{
				const tstring sAreaDictInfo = pAreaDictList[i];
				if (ParseString(sAreaDictInfo,":",true,pList)<2)
				{
					continue;
				}
				const int nAreaDictCode = atoi(pList[0].c_str());
				const tstring sName = pList[1];
				const tstring sDigit = pList.size()<3?"":pList[2];

				CEBAreaInfo::pointer pAreaInfo;
				if (!theAreaInfoList.find(nAreaDictCode,pAreaInfo))
				{
					pAreaInfo = CEBAreaInfo::create(nParent);
					theAreaInfoList.insert(nAreaDictCode,pAreaInfo);
				}
				pAreaInfo->m_nAreaCode = nAreaDictCode;
				pAreaInfo->m_sAreaDigit = sDigit;
				pAreaInfo->m_sAreaName = sName;
				if (!pParentAreaInfo->m_pAreaInfoList.exist(nAreaDictCode))
				{
					pParentAreaInfo->m_pAreaInfoList.insert(nAreaDictCode,pAreaInfo);
				}
			}
			if (pSotpRequestInfo.get()==NULL)
			{
				break;
			}
			const cgcParameter::pointer pParameter = pSotpRequestInfo->m_pRequestList.getParameter("parameter");
			if (pParameter.get()==NULL)
				break;

			if (nSize<=(int)pParentAreaInfo->m_pAreaInfoList.size())
			{
				const unsigned int nParameter = (unsigned int)pParameter->getBigInt();
				BoostReadLock rdlock(pParentAreaInfo->m_pAreaInfoList.mutex());
				CLockMap<int,CEBAreaInfo::pointer>::iterator pIter = pParentAreaInfo->m_pAreaInfoList.begin();
				for (; pIter!=pParentAreaInfo->m_pAreaInfoList.end(); pIter++)
				{
					const CEBAreaInfo::pointer& pAreaInfo = pIter->second;
					if (m_callback)
						m_callback->onAreaInfo(pAreaInfo.get(),nParameter);
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
						EB_AreaInfo * pEvent = new EB_AreaInfo(pAreaInfo.get());
						pEvent->SetQEventType((QEvent::Type)EB_WM_AREA_INFO);
						pEvent->SetEventParameter((long)nParameter);
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::SendMessage(m_pHwnd, EB_WM_AREA_INFO, (WPARAM)(const EB_AreaInfo*)pAreaInfo.get(), (LPARAM)nParameter);
#endif
					}
				}
			}
		}break;
	case EB_MSG_EMOTION_COUNT:
		{
			const int nEmoCount = atoi(pMsgInfo->GetMsgName());
			const tstring& sEmoVerString = pMsgInfo->GetMsgContent();
			ProcessEmotionCount(nEmoCount, sEmoVerString);
		}break;
	case EB_MSG_EMOTION_INFO:
		{
			// 表情资源信息
			std::vector<tstring> pList;
			if (ParseString(pMsgInfo->GetMsgContent(),";",false,pList)<10)	// 7,5
			{
				return;
			}

			const mycp::bigint sResourceId = cgc_atoi64(pList[0].c_str());
            /// for test
//            if (5433726958570064LL==sResourceId) {
//                int i=0;
//            }
			const tstring sCmServer(pList[1]);
			//const tstring sCmAppName = pList[2];
			const int nIndex = atoi(pList[3].c_str());
			const int nType = atoi(pList[4].c_str());
			const mycp::bigint sEntCode = cgc_atoi64(pList[5].c_str());						// entcode为空，是个人表情
			//const tstring sEntCode = pList.size()>5?pList[5]:"";	// entcode为空，是个人表情
			const int nResType = atoi(pList[6].c_str());
			const tstring sResMd5(pList[7]);
			const tstring sDescription(pList[9]);
			const mycp::bigint sEmoId = pList.size()>=11?cgc_atoi64(pList[10].c_str()):0;
			const int sCmServiceId = pList.size()>=12?atoi(pList[11].c_str()):0;
			//const int nEmoVer = pList.size()>=12?atoi(pList[11].c_str()):0;

			CEBEmotionInfo::pointer pEmotionInfo;
			char sEnterpriseImageKey[48];
			sprintf(sEnterpriseImageKey,"%lld%lld",sResourceId,sEntCode);
			if (!theEmotionList.find(sEnterpriseImageKey,pEmotionInfo))
			{
				pEmotionInfo = CEBEmotionInfo::create(sResourceId,nIndex,nType,m_pMyAccountInfo->GetUserId(),sEntCode,(EB_RESOURCE_TYPE)nResType);
				theEmotionList.insert(sEnterpriseImageKey,pEmotionInfo);
			}else
			{
				pEmotionInfo->m_nIndex = nIndex;
				pEmotionInfo->m_nType = nType;
				pEmotionInfo->m_nResType = (EB_RESOURCE_TYPE)nResType;
			}
			pEmotionInfo->m_nCmServiceId = sCmServiceId;
			pEmotionInfo->m_nEmoId = sEmoId;
			pEmotionInfo->m_sResMd5 = sResMd5;
			pEmotionInfo->m_sDescription = sDescription;
			pEmotionInfo->m_sCmServer = sCmServer;
			//pEmotionInfo->m_sCmAppName = sCmAppName;
#ifdef _QT_MAKE_
            const EBFileString sResourceFile = QString("%1/%2").arg(m_sEbResourcePath).arg(sResourceId);
            if (QFileInfo::exists(sResourceFile))
#else
            char sResourceFile[260];
            sprintf(sResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),sResourceId);
            if (::PathFileExistsA(sResourceFile))
#endif
			{
				if (!sResMd5.empty())
				{
                    /// 检查文件MD5是否相同；
					mycp::bigint nFileSize = 0;
					tstring sFileMd5String;
					GetFileMd5(sResourceFile,nFileSize,sFileMd5String);
					if (sFileMd5String==sResMd5)
					{
						pEmotionInfo->m_sResFile = sResourceFile;
					}
				}else
				{
					pEmotionInfo->m_sResFile = sResourceFile;
				}
			}
#ifdef _QT_MAKE_
            if (pEmotionInfo->m_sResFile.isEmpty())
#else
            if (pEmotionInfo->m_sResFile.empty())
#endif
			{
				//pEmotionInfo->m_sResFile = sResourceFile;	// 下载成功再保存
				// 资源文件不存在（或MD5不对，下载不完整），下载
				if (!theResourceChatRoom.exist(sResourceId))
				{
					const tstring sCmServerKey = GetCmServerKey(sCmServer);
					Cchatroom::pointer pChatRoom;
					if (!theChatRoomList.find(sCmServerKey,pChatRoom))
					{
						pChatRoom = Cchatroom::create();
						pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
						pChatRoom->Start(sCmServer.c_str(), "",0,true,1);
						pChatRoom->SetCallBack(this);
						theChatRoomList.insert(sCmServerKey,pChatRoom);
					}
					char lpszKey[200];
					sprintf(lpszKey,"%s%lld",sCmServerKey.c_str(),sResourceId);
					//sprintf(lpszKey,"%s%lld",sCmServer.c_str(),sResourceId);
					theChatRoomList.insert(lpszKey,pChatRoom,false);
					theResourceChatRoom.insert(sResourceId,pChatRoom);
					CFilePathInfo::pointer pFilePathInfo = CFilePathInfo::create(CFilePathInfo::FILE_ENT_IMAGE,sResourceFile,false);
					pFilePathInfo->m_nBigData = sEntCode;
					theResourceFilePath.insert(sResourceId,pFilePathInfo);
					//pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
					//pChatRoom->RecviveResource(sResourceId,sResourceFile);
					CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE);
					pProcessMsgInfo->m_pChatRoom = pChatRoom;
					pProcessMsgInfo->m_nBigInt1 = sResourceId;
					pProcessMsgInfo->m_sString1 = sResourceFile;
					//pProcessMsgInfo->m_sString1.append(".ebtemp");
					m_pProcessMsgList.add(pProcessMsgInfo);
					//Sleep(50);
				}
			}else
			{
				// 保存本地
				SaveLocalEmotionInfo(pEmotionInfo);
			}
		}break;
	case EB_MSG_DELETE_RESOURCE:
		{
			const mycp::bigint sDeleteResourceId = cgc_atoi64(pMsgInfo->GetMsgContent().c_str());
			CEBResourceInfo::pointer pResourceInfo;
			if (!theResourceList.find(sDeleteResourceId,pResourceInfo,true))
			{
				return;
			}
			if (this->m_callback)
				m_callback->onResourceDelete(pResourceInfo.get());
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_ResourceInfo * pEvent = new EB_ResourceInfo(pResourceInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_RESOURCE_DELETE);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_RESOURCE_DELETE, (WPARAM)(const EB_ResourceInfo*)pResourceInfo.get(), 0);
#endif
			}
			return;
		}break;
	case EB_MSG_USER_INFO_CHANGE:
		{
			if (m_pUserManager.get()!=NULL)
			{
				char lpszUserId[24];
				sprintf(lpszUserId,"%lld",m_pMyAccountInfo->GetUserId());
				//CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
				//pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-group-id", pMsgInfo->m_sGroupCode));
				m_pUserManager->SendUQuery(0,lpszUserId);
			}
		}break;
	case EB_MSG_CONTACT_HEAD_VER_INFO:
		{
			// *** 加载所有好友/联系人头像版本
			// 格式: msg_name=[ver_size] msg_content=[con1_uid]:[con1_head_ver];[con2_uid]:[con2_head_ver];...
			// 未设置或者无头像版本号数据不返回

			const int nSize = atoi(pMsgInfo->GetMsgName().c_str());
			if (nSize<=0) return;

			// 企业部门版本信息；
			// msg-name=版本号；格式ver,size
			// msg-content=版本列表；格式groupid:ver;...
			if (m_pDefaultEnterpriseInfo.get()==NULL) return;
			
			std::vector<tstring> pContactHeadList;
			ParseString(pMsgInfo->GetMsgContent(),";",false,pContactHeadList);
			for (size_t i=0;i<pContactHeadList.size();i++)
			{
				const tstring sGroupVerInfo = pContactHeadList[i];
				// 取出联系人头像版本信息 con_uid:con_head_ver

				std::vector<tstring> pList;
				if (ParseString(sGroupVerInfo,":",false,pList)!=2)
				{
					continue;
				}
				const mycp::bigint nContactUserId = cgc_atoi64(pList[0].c_str());
				const mycp::bigint nContactUserHeadVer = cgc_atoi64(pList[1].c_str());
				CEBContactInfo::pointer pContactInfo = GetContactInfo(nContactUserId);
				if (pContactInfo.get()==NULL) continue;
				if (pContactInfo->m_nHeadVer != nContactUserHeadVer)
				{
					// 0x4=只加载头像信息
					this->ContactQuery(pContactInfo->m_nContactId,0,0x4);
				}
			}

		}break;
	case EB_MSG_USER_HEAD_CHANGE:
		{
			pOutAutoAck = false;
			//pOutAckType = EB_CAT_DELETE;
			std::vector<tstring> pList;
			if (ParseString(pMsgInfo->GetMsgContent().c_str(),";",false,pList)!=5)
			{
				return;
			}
			const mycp::bigint nContactHeadVer = cgc_atoi64(pList[0].c_str());
			const mycp::bigint nHeadResId = cgc_atoi64(pList[1].c_str());
			const tstring sHeadCmServer(pList[2]);
			const tstring sHeadHttpServer(pList[3]);
			const tstring sHeadMd5(pList[4]);
			if (pMsgInfo->m_nFromUserId == m_pMyAccountInfo->GetUserId())
			{
				// *** 头像已经在 EB_MSG_GROUP_MEMBER_CHANGE 处理
				//char lpszHeadResourceFile[260];
				//sprintf(lpszHeadResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),sHeadResourceId);
				////const tstring sHeadResourceFile = m_sEbResourcePath + "\\" + sHeadResourceId;
				//pEmployeeInfo->m_sHeadResourceId = sHeadResourceId;

				//// ???
				//if (m_pDefaultEntEmployeeInfo.get()!=NULL && m_pDefaultEntEmployeeInfo.get()!=pEmployeeInfo.get() &&
				//	m_pDefaultEntEmployeeInfo->m_sMemberCode==pEmployeeInfo->m_sMemberCode && m_pDefaultEntEmployeeInfo->m_sHeadResourceId!=sHeadResourceId)
				//{
				//	m_pDefaultEntEmployeeInfo->m_sHeadResourceId = sHeadResourceId;
				//	m_pDefaultEntEmployeeInfo->m_sHeadMd5 = sHeadResourceMd5;
				//	ProcessMemberHeadInfo(m_pDefaultEntEmployeeInfo);
				//}

				//pEmployeeInfo->m_sHeadResourceFile = lpszHeadResourceFile;
				//pEmployeeInfo->m_sHeadMd5 = sHeadResourceMd5;
				//if (this->m_callback)
				//	m_callback->onUserHeadChange(pEmployeeInfo.get(),true);
				//if (m_pHwnd!=NULL)
				//	::SendMessage(m_pHwnd, EB_WM_USER_HEAD_CHANGE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(),1);
			}else
			{
				CEBContactInfo::pointer pContactInfo = GetContactInfo(pMsgInfo->m_nFromUserId);
				if (pContactInfo.get()!=NULL)
				{
					pContactInfo->m_nHeadVer = nContactHeadVer;
					pContactInfo->m_sHeadResourceId = nHeadResId;
					pContactInfo->m_sHeadCmServer = sHeadCmServer;
					pContactInfo->m_sHeadHttpServer = sHeadHttpServer;
					pContactInfo->m_sHeadMd5 = sHeadMd5;
					ProcessContactHeadInfo(pContactInfo,true);
					SaveLocalContactInfo(pContactInfo);
				}
			}
			return;
		}break;
	//case EB_MSG_USER_HEAD_CHANGE:
	//	{
	//		std::vector<tstring> pList;
	//		if (ParseString(pMsgInfo->GetMsgContent().c_str(),";",pList)!=3)
	//		{
	//			return;
	//		}
	//		const tstring sHeadResourceId = pList[0];
	//		const mycp::bigint nHeadResourceId = cgc_atoi64(sHeadResourceId.c_str());
	//		const tstring sHeadCmServer = pList[1];
	//		const tstring sHeadCmAppName = pList[2];
	//		CEBGroupInfo::pointer pDepartmentInfo;
	//		if (!theDepartmentList.find(pMsgInfo->m_sGroupCode, pDepartmentInfo))
	//		{
	//			return;
	//		}
	//		CEBMemberInfo::pointer pEmployeeInfo;
	//		if (!pDepartmentInfo->m_pMemberList.find(pMsgInfo->m_nFromUserId,pEmployeeInfo))
	//		{
	//			return;
	//		}
	//		DeleteOldHeadFile(pEmployeeInfo);
	//		const tstring sHeadResourceFile = m_sEbResourcePath + "\\" + sHeadResourceId;
	//		pEmployeeInfo->m_sHeadResourceId = nHeadResourceId;
	//		pEmployeeInfo->m_sHeadResourceFile = sHeadResourceFile;
	//		CEBEmotionInfo::pointer pEmotionInfo = GetEmotionInfo(pEmployeeInfo->m_sHeadResourceId);
    //		if (pEmotionInfo.get() == NULL || !::PathFileExistsA(sHeadResourceFile.c_str()))
	//		{
	//			// 非系统头像，或者头像文件不存在，请求下载
	//			Cchatroom::pointer pChatRoom;
	//			if (!theChatRoomList.find(sHeadCmServer,pChatRoom))
	//			{
	//				pChatRoom = Cchatroom::create();
	//				pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
	//				pChatRoom->Start(sHeadCmServer.c_str(), "",0,true,1);
	//				theChatRoomList.insert(sHeadCmServer,pChatRoom);
	//			}
	//			theResourceChatRoom.insert(nHeadResourceId,pChatRoom);
	//			theResourceFilePath.insert(nHeadResourceId,CFilePathInfo::create(CFilePathInfo::FILE_PAHT_HEAD,sHeadResourceFile,true));
	//			pChatRoom->SetCallBack(this);
	//			//pChatRoom->SetParam(sHeadResourceId);
	//			pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),
	//				m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
	//			pChatRoom->RecviveResource(nHeadResourceId,sHeadResourceFile.c_str());
	//		}
	//		return;
	//	}break;
	case EB_MSG_ONLINE_ANOTHER:
		{
			// 在其他地方登录，清空数据，保证新登录不会又掉线出来；
			const int nOnlineAnotherType = atoi(pMsgInfo->GetMsgContent().c_str());
            m_userStatus = US_OnlineAnother;
			if (this->m_callback)
				m_callback->onOnlineAnother(nOnlineAnotherType);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_ONLINE_ANOTHER);
//				pEvent->SetQEventType((QEvent::Type)EB_WM_ONLINE_ANOTHER);
				pEvent->SetEventParameter((long)nOnlineAnotherType);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_ONLINE_ANOTHER, (LPARAM)nOnlineAnotherType, 0);
#endif
			}
			m_pMyAccountInfo = CEBAccountInfo::create(0,"");
			//m_pMyAccountInfo.reset();

            // 清空几个重要数据列表
            theEmployeeList.clear();
            theCallInfoList.clear();
            this->theDepartmentList.clear();
            BoostWriteLock wtLock(m_mutexEBDatas);
			if (m_pEBDatas.get()!=NULL)
			{
				m_pEBDatas->close();
				m_pEBDatas.reset();
			}
			//m_pUserManager->Stop();
			//m_pUserManager.reset();
			return;
		}break;
	case EB_MSG_UPDATE_GROUP:
		{
			theNeedLoadGroupInfoList.add(pMsgInfo->m_sGroupCode);	// 群组信息更新，加载群信息
			//CEBGroupInfo::pointer pDepartmentInfo;
			//if (!theDepartmentList.find(pMsgInfo->m_sGroupCode, pDepartmentInfo) || !pDepartmentInfo->m_pMemberList.exist(m_pMyAccountInfo->GetUserId()))
			//{
			//	EnterpriseLoad(pMsgInfo->m_sGroupCode,1,1,1);			// 不存在群组，加载群信息和成员
			//}else
			//{
			//	theNeedLoadGroupInfoList.add(pMsgInfo->m_sGroupCode);	// 已经存在群组，加载群信息
			//}
			return;
		}break;
	case EB_MSG_USER_LINE_STATE:
		{
			// 用户在线状况改变
			// line_state;um_server;us_app_name;ent-groupid,...;user-groupid,...
			const tstring sMsgContent = pMsgInfo->GetMsgContent();
			std::vector<tstring> pList;
			if (ParseString(sMsgContent,";",false,pList)<3)
				return;
			const EB_USER_LINE_STATE nLineState = (EB_USER_LINE_STATE)atoi(pList[0].c_str());
			const tstring sUmServer = pList[1];
			//const tstring sUmAppName = pList[2]
			const tstring sEntGroupList = pList.size()>=4?pList[3]:"";
			const tstring sUserGroupList = pList.size()>=5?pList[4]:"";
			if ((m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)
			{
				// 好友验证模式，通知联系人在线状态
				CEBContactInfo::pointer pContactInfo = GetContactInfo(pMsgInfo->m_nFromUserId);
				if (pContactInfo.get()!=NULL && pContactInfo->m_nLineState!=nLineState)
				{
					pContactInfo->m_nLineState = nLineState;
					if (this->m_callback)
						m_callback->onContactStateChange(pContactInfo.get());
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
						EB_ContactInfo * pEvent = new EB_ContactInfo(pContactInfo.get());
						pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_STATE_CHANGE);
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::SendMessage(m_pHwnd, EB_WM_CONTACT_STATE_CHANGE, (WPARAM)(const EB_ContactInfo*)pContactInfo.get(), 0);
#endif
					}
					else {
						m_pLogonToSendContactLineState.add(pContactInfo);
					}
				}
				//break;
			}

			char lpszBuffer[24];
			tstring sLoadGroupOLSGid;
			ParseString(sEntGroupList,",",true,pList);
			ParseString(sUserGroupList,",",false,pList);
			for (size_t i=0;i<pList.size(); i++)
			{
				const mycp::bigint nGroupId = cgc_atoi64(pList[i].c_str());
				CEBGroupInfo::pointer pDepartmentInfo;
				if (!theDepartmentList.find(nGroupId, pDepartmentInfo))
				{
					continue;
				}
				if ((pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER)==0)
				{
					// 需要重新加载在线人数；
					if (sLoadGroupOLSGid.empty())
						sprintf(lpszBuffer,"%lld",pDepartmentInfo->m_sGroupCode);
					else
						sprintf(lpszBuffer,",%lld",pDepartmentInfo->m_sGroupCode);
					sLoadGroupOLSGid.append(lpszBuffer);
				}

				CEBMemberInfo::pointer pEmployeeInfo;
				if (pDepartmentInfo->m_pMemberList.find(pMsgInfo->m_nFromUserId,pEmployeeInfo))
				{
					const EB_USER_LINE_STATE nOldLineState = pEmployeeInfo->m_nLineState;
					if (nOldLineState<EB_LINE_STATE_BUSY && nLineState>=nOldLineState)
					{
						pDepartmentInfo->m_nOnlineSize += 1;
					}else if (pDepartmentInfo->m_nOnlineSize>0 && nOldLineState>=EB_LINE_STATE_BUSY && nLineState<nOldLineState)
					{
						pDepartmentInfo->m_nOnlineSize -= 1;
					}
					pEmployeeInfo->m_nLineState = nLineState;
					pEmployeeInfo->m_sUmServer = sUmServer;
					//pEmployeeInfo->m_sUmAppName = sUmAppName;
					if (nOldLineState != nLineState)
					{
						const bool nIsMyMember = (bool)(m_pMyAccountInfo->GetUserId() == pEmployeeInfo->m_nMemberUserId);
						if (this->m_callback)
							m_callback->onUserStateChange(pEmployeeInfo.get(),nIsMyMember);
						if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
							EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
							pEvent->SetQEventType((QEvent::Type)EB_WM_USER_STATE_CHANGE);
							pEvent->SetEventParameter((long)(nIsMyMember?1:0));
                            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
							::SendMessage(m_pHwnd, EB_WM_USER_STATE_CHANGE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), nIsMyMember?1:0);
#endif
						}
						else {
							m_pLogonToSendMemberLineState.add(pEmployeeInfo);
						}
					}
				}
			}
			if (!sLoadGroupOLSGid.empty())
				LoadInfo(0,0,0,0,0,0,0,sLoadGroupOLSGid);
			if (nLineState!=EB_LINE_STATE_OFFLINE)
			{
				// 用户上线，检查是否有正在发送文件，呼叫用户
				CEBCallInfo::pointer pCallInfo = GetExistCallInfo(pMsgInfo->m_nFromUserId, 0);	// depcode:群组，为空表示单对单会话
				if (pCallInfo.get()!=NULL)
				{
					CEBAccountInfo::pointer pFromAccountInfo;
					if (theOnlineFileList.exist(pCallInfo->GetCallId()) &&
						pCallInfo->m_pCallUserList.find(pMsgInfo->m_nFromUserId, pFromAccountInfo))
					{
						pFromAccountInfo->m_dwAccountData &= ~EB_DATA_SEND_FILE_CALL;
						CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_INVITE_CALL);
						pProcessMsgInfo->m_tProcessTime = time(0)+3;
						pProcessMsgInfo->m_pCallInfo = pCallInfo;
						pProcessMsgInfo->m_nBigInt1 = 1;													// bSendFile true
						pProcessMsgInfo->m_nFromUserId = pMsgInfo->m_nFromUserId;	// bCallOnlyAccount
						m_pProcessMsgList.add(pProcessMsgInfo);
						//this->InviteCall(pCallInfo,0,pMsgInfo->m_nFromUserId,true,true);
					}
				}
			}
			return;

			//CEBGroupInfo::pointer pDepartmentInfo;
			//if (!theDepartmentList.find(pMsgInfo->m_sGroupCode, pDepartmentInfo))
			//{
			//	return;
			//}

			//CEBMemberInfo::pointer pEmployeeInfo;
			//if (pDepartmentInfo->m_pMemberList.find(pMsgInfo->m_nFromUserId,pEmployeeInfo))
			//{
			//	const EB_USER_LINE_STATE nOldLineState = pEmployeeInfo->m_nLineState;
			//	pEmployeeInfo->m_nLineState = nLineState;
			//	pEmployeeInfo->m_sUmServer = sUmServer;
			//	//pEmployeeInfo->m_sUmAppName = sUmAppName;
			//	if (nOldLineState != nLineState)
			//	{
			//		const bool nIsMyMember = (bool)(m_pMyAccountInfo->GetUserId() == pEmployeeInfo->m_nMemberUserId);
			//		if (this->m_callback)
			//			m_callback->onUserStateChange(pEmployeeInfo.get(),nIsMyMember);
			//		if (m_pHwnd!=NULL)
			//			::SendMessage(m_pHwnd, EB_WM_USER_STATE_CHANGE, (WPARAM)pEmployeeInfo.get(), nIsMyMember?1:0);
			//	}
			//}

			////if (pDepartmentInfo->m_sEnterpriseCode>0)
			//{
			//	// 企业部门，需要同时检查其他部门（项目组）成员
			//	try
			//	{
			//		BoostReadLock lockDepartment(theDepartmentList.mutex());
			//		CLockMap<mycp::bigint, CEBGroupInfo::pointer>::const_iterator pIterDepartment = theDepartmentList.begin();
			//		for (; pIterDepartment!=theDepartmentList.end(); pIterDepartment++)
			//		{
			//			CEBGroupInfo::pointer pEntDepartmentInfo = pIterDepartment->second;
			//			if (pEntDepartmentInfo->m_sGroupCode == pDepartmentInfo->m_sGroupCode) continue;	// 前面已经处理
			//			if (pEntDepartmentInfo->m_pMemberList.find(pMsgInfo->m_nFromUserId,pEmployeeInfo))
			//			{
			//				const EB_USER_LINE_STATE nOldLineState = pEmployeeInfo->m_nLineState;
			//				pEmployeeInfo->m_nLineState = nLineState;
			//				pEmployeeInfo->m_sUmServer = sUmServer;
			//				//pEmployeeInfo->m_sUmAppName = sUmAppName;
			//				if (nOldLineState != nLineState)
			//				{
			//					//lockDepartment.unlock();
			//					const bool nIsMyMember = (bool)(m_pMyAccountInfo->GetUserId() == pEmployeeInfo->m_nMemberUserId);
			//					if (this->m_callback)
			//						m_callback->onUserStateChange(pEmployeeInfo.get(),nIsMyMember);
			//					if (m_pHwnd!=NULL)
			//						::SendMessage(m_pHwnd, EB_WM_USER_STATE_CHANGE, (WPARAM)pEmployeeInfo.get(), nIsMyMember?1:0);
			//					//lockDepartment.lock();
			//				}
			//			}
			//		}
			//	}catch(std::exception&)
			//	{
			//	}catch(...)
			//	{}
			//}
			//if ((m_nSystemSetting&CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)==CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)
			//{
			//	// 好友验证模式，通知联系人在线状态
			//	AUTO_CONST_RLOCK(theContactList);
			//	CLockMap<mycp::bigint, CEBContactInfo::pointer>::const_iterator pIterContact = theContactList.begin();
			//	for (; pIterContact!=theContactList.end(); pIterContact++)
			//	{
			//		const CEBContactInfo::pointer pContactInfo = pIterContact->second;
			//		if (pContactInfo->m_nContactUserId!=pMsgInfo->m_nFromUserId || pContactInfo->m_nLineState==nLineState)
			//		{
			//			continue;
			//		}
			//		pContactInfo->m_nLineState = nLineState;
			//		if (this->m_callback)
			//			m_callback->onContactStateChange(pContactInfo.get());
			//		if (m_pHwnd!=NULL)
			//			::PostMessage(m_pHwnd, EB_WM_CONTACT_STATE_CHANGE, (WPARAM)pContactInfo.get(), 0);
			//	}
			//}
			//return;
		}break;
	//case EB_MSG_USER_ONLINE_INCALL:
	//	{
	//		//const mycp::bigint sCallId = cgc_atoi64(pMsgInfo->GetMsgContent().c_str());
	//		//CEBCallInfo::pointer pCallInfo;
	//		//if (theCallInfoList.find(sCallId, pCallInfo))
	//		//{
	//		//	if (pCallInfo->m_nCallState != EB_CALL_STATE_HANGUP)
	//		//	//if (pCallInfo->m_nCallState == EB_CALL_STATE_CONNECTED || pCallInfo->m_nCallState == EB_CALL_STATE_EXIT)
	//		//	{
	//		//		mycp::bigint sEmpCode = 0;
	//		//		if (pCallInfo->m_sGroupCode>0)
	//		//		{
	//		//			CEBGroupInfo::pointer pDepartmentInfo;
	//		//			if (theDepartmentList.find(pCallInfo->m_sGroupCode, pDepartmentInfo))
	//		//			{
	//		//				CEBMemberInfo::pointer pEmployeeInfo;
	//		//				if (pDepartmentInfo->m_pMemberList.find(pMsgInfo->m_nFromUserId, pEmployeeInfo))
	//		//				{
	//		//					sEmpCode = pEmployeeInfo->m_sMemberCode;
	//		//				}
	//		//			}
	//		//		}else
	//		//		{
	//		//			CEBMemberInfo::pointer pEmployeeInfo = GetEmployeeInfo(pMsgInfo->m_sFromAccount);
	//		//			if (pEmployeeInfo.get()!=NULL)
	//		//			{
	//		//				sEmpCode = pEmployeeInfo->m_sMemberCode;
	//		//			}
	//		//		}
	//		//		pCallInfo->m_nCallState = EB_CALL_STATE_AUTO_ACK;
	//		//		//pCallInfo->m_nCallState = EB_CALL_STATE_ONLINE_INCALL;
	//		//		if (sEmpCode==0)
	//		//			CallUserId(pMsgInfo->m_nFromUserId,sCallId,0);
	//		//		else
	//		//			CallMember(sEmpCode,sCallId,0);
	//		//	}
	//		//}
	//		return;
	//	}break;
	case EB_MSG_CALL_2_GROUP:
		{
			const mycp::bigint sCallId = cgc_atoi64(pMsgInfo->GetMsgContent().c_str());
			//CEBCallInfo::pointer pCallInfo;
			//if (theCallInfoList.find(sCallId, pCallInfo))
			//{
			//	pCallInfo->m_sGroupCode = pMsgInfo->m_sGroupCode;
			//}
			if (!theDepartmentList.exist(pMsgInfo->m_sGroupCode))
			{
				// 正常情况下不会跑这里，这是预防网络问题；
				CPOPSotpRequestInfo::pointer pSotpRequestInfo = CPOPSotpRequestInfo::create(0);
				pSotpRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("call-group-id", pMsgInfo->m_sGroupCode));
                EnterpriseLoad(pMsgInfo->m_sGroupCode,1,1,1,0,0,false,"",false,pSotpRequestInfo);
				break;
			}
			// 所有成员，主动发起一个群组呼叫
			CallGroup(pMsgInfo->m_sGroupCode,true,true);
			return;
		}break;
	case EB_MSG_INVITE_ADD_2_GROUP:
		{
			// 邀请（添加）我进群组（部门）
			EB_AccountInfo * pAccountInfo = new EB_AccountInfo(pMsgInfo->m_nFromUserId,pMsgInfo->m_sFromAccount,0);
			EB_APMsgInfo pApMsgInfo;
			pApMsgInfo.m_nMsgId = pMsgInfo->GetMsgId();
			pApMsgInfo.m_nGroupId = pMsgInfo->m_sGroupCode;
			pApMsgInfo.m_nUserId = pMsgInfo->m_nFromUserId;
			pApMsgInfo.m_sAccount = pMsgInfo->m_sFromAccount;
			pApMsgInfo.m_sMsgContent = pMsgInfo->GetMsgContent();	// description
			if (m_callback)
				m_callback->onInviteAdd2Group(pAccountInfo,&pApMsgInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pAccountInfo->SetQEventType((QEvent::Type)EB_WM_INVITE_ADD2GROUP);
				pAccountInfo->SetEventData((void*)(const EB_APMsgInfo*)&pApMsgInfo);
                postWaitEventResult(m_pHwnd, pAccountInfo);
#else
				::SendMessage(m_pHwnd, EB_WM_INVITE_ADD2GROUP, (WPARAM)pAccountInfo, (LPARAM)&pApMsgInfo);
				delete pAccountInfo;
#endif
			}
			else {
				delete pAccountInfo;
			}
			pOutAutoAck = false;
		}break;
	case EB_MSG_REJECT_ADD_2_GROUP:
		{
			// 拒绝加入群组
			EB_AccountInfo * pAccountInfo = new EB_AccountInfo(pMsgInfo->m_nFromUserId,pMsgInfo->m_sFromAccount,0);
			EB_APMsgInfo pApMsgInfo;
			pApMsgInfo.m_nMsgId = pMsgInfo->GetMsgId();
			pApMsgInfo.m_nGroupId = pMsgInfo->m_sGroupCode;
			pApMsgInfo.m_nUserId = pMsgInfo->m_nFromUserId;
			pApMsgInfo.m_sAccount = pMsgInfo->m_sFromAccount;
			pApMsgInfo.m_sMsgContent = pMsgInfo->GetMsgContent();	// group-name
			if (m_callback)
				m_callback->onRejectAdd2Group(pAccountInfo,&pApMsgInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pAccountInfo->SetQEventType((QEvent::Type)EB_WM_REJECT_ADD2GROUP);
				pAccountInfo->SetEventData((void*)(const EB_APMsgInfo*)&pApMsgInfo);
                postWaitEventResult(m_pHwnd, pAccountInfo);
#else
				::SendMessage(m_pHwnd, EB_WM_REJECT_ADD2GROUP, (WPARAM)pAccountInfo, (LPARAM)&pApMsgInfo);
				delete pAccountInfo;
#endif
			}
			else {
				delete pAccountInfo;
			}
		}break;
	case EB_MSG_UPDATE_USER_GROUP2:
		{
			//sprintf(lpszBuffer,"%lld,%lld,%s",nNewContactInfoVer,nUGId,sGroupName.c_str());
			const tstring & sMsgContent = pMsgInfo->GetMsgContent();
			const std::string::size_type findVer = sMsgContent.find(",");
			if (findVer == std::string::npos) break;
			const mycp::bigint nNewContactInfoVer = cgc_atoi64(sMsgContent.c_str());
			const std::string::size_type findUGId = sMsgContent.find(",",findVer+1);
			if (findUGId == std::string::npos) break;
			const mycp::bigint nUGId = cgc_atoi64(sMsgContent.substr(findVer+1,findUGId-findVer).c_str());
			if (nUGId==0) break;
			const tstring sGroupName(sMsgContent.substr(findUGId+1));

			CEBUGInfo::pointer pUGInfo;
			if (!theUGInfoList.find(nUGId, pUGInfo))
			{
				//if (pMsgInfo->GetMsgType()==EB_MSG_UPDATE_USER_GROUP)
				//	break;
				pUGInfo = CEBUGInfo::create(nUGId,sGroupName);
				theUGInfoList.insert(nUGId, pUGInfo);
			}else
			{
				//pUGInfo->m_nUGId = nUGId;
				pUGInfo->m_sGroupName = sGroupName;
			}
			if (nNewContactInfoVer==m_pMyAccountInfo->GetContactInfoVer()+1)
			{
				m_pMyAccountInfo->SetContactInfoVer(nNewContactInfoVer);
				SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),nNewContactInfoVer);
			}
			if (m_callback)
				m_callback->onUGInfo(pUGInfo.get());
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_UGInfo * pEvent = new EB_UGInfo(pUGInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_UG_INFO);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_UG_INFO, (WPARAM)(const EB_UGInfo*)pUGInfo.get(), 0);
#endif
			}
		}break;
	//case EB_MSG_ADD_USER_GROUP:
	case EB_MSG_UPDATE_USER_GROUP:
		{
			//sprintf(lpszBuffer,"%lld,%s",nUGId,sGroupName.c_str());
			const tstring & sMsgContent = pMsgInfo->GetMsgContent();
			const std::string::size_type findUGId = sMsgContent.find(",");
			if (findUGId == std::string::npos) break;
			const mycp::bigint nUGId = cgc_atoi64(sMsgContent.c_str());
			if (nUGId==0) break;
			const tstring sGroupName(sMsgContent.substr(findUGId+1));

			////sprintf(lpszBuffer,"%lld,%lld,%s",nNewContactInfoVer,nUGId,sGroupName.c_str());
			//const tstring & sMsgContent = pMsgInfo->GetMsgContent();
			//const std::string::size_type findVer = sMsgContent.find(",");
			//if (findVer == std::string::npos) break;
			//const mycp::bigint nNewContactInfoVer = cgc_atoi64(sMsgContent.c_str());
			//const std::string::size_type findUGId = sMsgContent.find(",",findVer+1);
			//if (findUGId == std::string::npos) break;
			//const mycp::bigint nUGId = cgc_atoi64(sMsgContent.substr(findVer+1,findUGId-findVer).c_str());
			//if (nUGId==0) break;
			//const tstring sGroupName(sMsgContent.substr(findUGId+1));

			CEBUGInfo::pointer pUGInfo;
			if (!theUGInfoList.find(nUGId, pUGInfo))
			{
				//if (pMsgInfo->GetMsgType()==EB_MSG_UPDATE_USER_GROUP)
				//	break;
				pUGInfo = CEBUGInfo::create(nUGId,sGroupName);
				theUGInfoList.insert(nUGId, pUGInfo);
			}else
			{
				//pUGInfo->m_nUGId = nUGId;
				pUGInfo->m_sGroupName = sGroupName;
			}
			if (m_callback)
				m_callback->onUGInfo(pUGInfo.get());
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_UGInfo * pEvent = new EB_UGInfo(pUGInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_UG_INFO);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_UG_INFO, (WPARAM)(const EB_UGInfo*)pUGInfo.get(), 0);
#endif
			}
		}break;
	case EB_MSG_DELETE_USER_GROUP2:
		{
			//sprintf(lpszBuffer,"%lld,%lld",nNewContactInfoVer,nUGId);
			const tstring & sMsgContent = pMsgInfo->GetMsgContent();
			const std::string::size_type findVer = sMsgContent.find(",");
			if (findVer == std::string::npos) break;
			const mycp::bigint nNewContactInfoVer = cgc_atoi64(sMsgContent.c_str());
			const mycp::bigint nUGId = cgc_atoi64(sMsgContent.substr(findVer+1).c_str());
			if (nUGId==0) break;
			DeleteUG(nUGId);
			if (nNewContactInfoVer==m_pMyAccountInfo->GetContactInfoVer()+1)
			{
				m_pMyAccountInfo->SetContactInfoVer(nNewContactInfoVer);
				SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),nNewContactInfoVer);
			}
		}break;
	case EB_MSG_DELETE_USER_GROUP:
		{
			//sprintf(lpszBuffer,"%lld",nUGId);
			const tstring & sMsgContent = pMsgInfo->GetMsgContent();
			const mycp::bigint nUGId = cgc_atoi64(sMsgContent.c_str());
			if (nUGId==0) break;
			DeleteUG(nUGId);
			//sprintf(lpszBuffer,"%lld,%lld",nNewContactInfoVer,nUGId);
			//const tstring & sMsgContent = pMsgInfo->GetMsgContent();
			//const std::string::size_type findVer = sMsgContent.find(",");
			//if (findVer == std::string::npos) break;
			//const mycp::bigint nNewContactInfoVer = cgc_atoi64(sMsgContent.c_str());
			//const mycp::bigint nUGId = cgc_atoi64(sMsgContent.substr(findVer+1).c_str());
			//if (nUGId==0) break;
			//DeleteUG(nUGId);
		}break;
	case EB_MSG_REQ_ADD_CONTACT:
		{
			// 申请加为好友
			EB_AccountInfo * pAccountInfo = new EB_AccountInfo(pMsgInfo->m_nFromUserId,pMsgInfo->m_sFromAccount,0);
			GetECardByFromInfo(pMsgInfo->m_sFromInfo,&pAccountInfo->m_pFromCardInfo);
			EB_APMsgInfo pApMsgInfo;
			pApMsgInfo.m_nMsgId = pMsgInfo->GetMsgId();
			pApMsgInfo.m_nUserId = pMsgInfo->m_nFromUserId;
			pApMsgInfo.m_sAccount = pMsgInfo->m_sFromAccount;
			pApMsgInfo.m_sMsgContent = pMsgInfo->GetMsgContent();	// description
			if (m_callback)
				m_callback->onRequestAddContact(pAccountInfo,&pApMsgInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pAccountInfo->SetQEventType((QEvent::Type)EB_WM_REQUEST_ADDCONTACT);
                pAccountInfo->SetEventData((void*)(const EB_APMsgInfo*)&pApMsgInfo);
                postWaitEventResult(m_pHwnd, pAccountInfo);
#else
				::SendMessage(m_pHwnd, EB_WM_REQUEST_ADDCONTACT, (WPARAM)pAccountInfo, (LPARAM)&pApMsgInfo);
				delete pAccountInfo;
#endif
			}
			else {
				delete pAccountInfo;
			}
			pOutAutoAck = false;
		}break;
	case EB_MSG_UPDATE_CONTACT:
	case EB_MSG_ACCEPT_ADD_CONTACT:
		{
			// 好友联系人信息更新
			// 添加好友成功
			const tstring & sMsgContent = pMsgInfo->GetMsgContent();
			const std::string::size_type find = sMsgContent.find(",",3);
			if (find == std::string::npos) break;
			const mycp::bigint nContactId = cgc_atoi64(sMsgContent.c_str());
			if (nContactId==0) break;
			const bool bAcceptContact = pMsgInfo->GetMsgType()==EB_MSG_ACCEPT_ADD_CONTACT?true:false;
			ContactQuery(nContactId,0,0,bAcceptContact);
			//const mycp::bigint nNewContactInfoVer = cgc_atoi64(sMsgContent.substr(find+1).c_str());
		}break;
	case EB_MSG_DELETE_CONTACT:
		{
			int nDeleteDest = 0;
			CEBContactInfo::pointer pContactInfo;
			mycp::bigint nMyContactInfoVer = 0;
			if (pMsgInfo->m_nFromUserId==m_pMyAccountInfo->GetUserId())
			{
				const tstring & sMsgContent = pMsgInfo->GetMsgContent();
				std::vector<tstring> pList;
				if (ParseString(sMsgContent,",",false,pList)<3)
					break;
				//nDeleteDest = atoi(pList[0].c_str());
				nDeleteDest = 1;
				nMyContactInfoVer = cgc_atoi64(pList[1].c_str());
				const mycp::bigint nContactId = cgc_atoi64(pList[2].c_str());
				if (!theContactList1.find(nContactId,pContactInfo))
				{
					// 找不到 contact_id
					break;
				}
			}else
			{
				pContactInfo = GetContactInfo(pMsgInfo->m_nFromUserId);
				if (pContactInfo.get()==NULL) break;
				const tstring & sMsgContent = pMsgInfo->GetMsgContent();
				nDeleteDest = atoi(sMsgContent.substr(0,1).c_str());
				nMyContactInfoVer = cgc_atoi64(sMsgContent.substr(2).c_str());
			}

			if (nMyContactInfoVer>0 && nMyContactInfoVer==m_pMyAccountInfo->GetContactInfoVer()+1)
			{
				m_pMyAccountInfo->SetContactInfoVer(nMyContactInfoVer);
				SetLocalMyContactInfoVer(m_pMyAccountInfo->GetUserId(),nMyContactInfoVer);
			}

			if (nDeleteDest==1)
			{
				// 删除联系人资料；
				if (m_pEBDatas.get()!=NULL)
				{
					// 先设置删除标识
					char lpszBuffer[128];
					sprintf(lpszBuffer,"DELETE FROM eb_contact_info_t WHERE con_id=%lld",pContactInfo->m_nContactId);
					m_pEBDatas->execute(lpszBuffer);
				}
				theContactList1.remove(pContactInfo->m_nContactId);
				if (pContactInfo->m_nContactUserId>0)
					theContactList2.remove(pContactInfo->m_nContactUserId);
				if (m_callback)
					m_callback->onContactDelete(pContactInfo.get());
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					EB_ContactInfo * pEvent = new EB_ContactInfo(pContactInfo.get());
					pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_DELETE);
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
					::SendMessage(m_pHwnd, EB_WM_CONTACT_DELETE, (WPARAM)(const EB_ContactInfo*)pContactInfo.get(), 0);
#endif
				}
			}else if ((pContactInfo->m_nContactType&EB_CONTACT_TYPE_AUTH)==EB_CONTACT_TYPE_AUTH)
			{
				pContactInfo->m_nContactType &= ~EB_CONTACT_TYPE_AUTH;
				//pContactInfo->m_nContactType = 0;
				pContactInfo->m_nLineState = EB_LINE_STATE_OFFLINE;
				if (m_callback)
					m_callback->onContactInfo(pContactInfo.get());
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					EB_ContactInfo * pEvent = new EB_ContactInfo(pContactInfo.get());
					pEvent->SetQEventType((QEvent::Type)EB_WM_CONTACT_INFO);
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
					::SendMessage(m_pHwnd, EB_WM_CONTACT_INFO, (WPARAM)(const EB_ContactInfo*)pContactInfo.get(), 0);
#endif
				}
			}
		}break;
	case EB_MSG_REJECT_ADD_CONTACT:
		{
			// 对方拒绝加为好友
			EB_AccountInfo * pAccountInfo = new EB_AccountInfo(pMsgInfo->m_nFromUserId,pMsgInfo->m_sFromAccount,0);
			GetECardByFromInfo(pMsgInfo->m_sFromInfo,&pAccountInfo->m_pFromCardInfo);
			EB_APMsgInfo pApMsgInfo;
			pApMsgInfo.m_nMsgId = pMsgInfo->GetMsgId();
			pApMsgInfo.m_nUserId = pMsgInfo->m_nFromUserId;
			pApMsgInfo.m_sAccount = pMsgInfo->m_sFromAccount;
			pApMsgInfo.m_sMsgContent = pMsgInfo->GetMsgContent();	// description
			if (m_callback)
				m_callback->onRejectAddContact(pAccountInfo,&pApMsgInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pAccountInfo->SetQEventType((QEvent::Type)EB_WM_REJECT_ADDCONTACT);
				pAccountInfo->SetEventData((void*)(const EB_APMsgInfo*)&pApMsgInfo);
                postWaitEventResult(m_pHwnd, pAccountInfo);
#else
				::SendMessage(m_pHwnd, EB_WM_REJECT_ADDCONTACT, (WPARAM)pAccountInfo, (LPARAM)&pApMsgInfo);
				delete pAccountInfo;
#endif
			}
			else {
				delete pAccountInfo;
			}
		}break;
	case EB_MSG_RESET_ACCOUNT:
		{
			// 重置帐号
			std::vector<tstring> pList;
			if (ParseString(pMsgInfo->GetMsgContent(),",",false,pList)!=2)
				break;
			const mycp::bigint nResetUserId = cgc_atoi64(pList[0].c_str());
			const EB_RESET_TYPE nResetType = (EB_RESET_TYPE)atoi(pList[1].c_str());
			if (nResetUserId!=m_pMyAccountInfo->GetUserId())
			{
				break;
			}
			switch (nResetType)
			{
			case EB_RESET_FREEZE_ACCOUNT:
				{
					if (this->m_callback)
						m_callback->onStateCode(EB_STATE_ACCOUNT_FREEZE,0);
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                        EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_STATE_CODE);
//						pEvent->SetQEventType((QEvent::Type)EB_WM_STATE_CODE);
						pEvent->SetEventParameter((long)EB_STATE_ACCOUNT_FREEZE);
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::PostMessage(m_pHwnd, EB_WM_STATE_CODE, EB_STATE_ACCOUNT_FREEZE, 0);
#endif
					}
				}break;
			case EB_RESET_DELETE_ACCOUNT:
				{
					if (this->m_callback)
						m_callback->onStateCode(EB_STATE_ACCOUNT_NOT_EXIST,1);
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                        EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_STATE_CODE);
//						pEvent->SetQEventType((QEvent::Type)EB_WM_STATE_CODE);
						pEvent->SetEventParameter((long)EB_STATE_ACCOUNT_FREEZE);
                        pEvent->SetEventBigParameter(1);
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::PostMessage(m_pHwnd, EB_WM_STATE_CODE, EB_STATE_ACCOUNT_NOT_EXIST, 1);
#endif
					}
				}break;
			default:
				break;
			}
			m_userStatus = US_Logout;
			if (this->m_callback)
				m_callback->onLogout();
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_LOGOUT);
//				pEvent->SetQEventType((QEvent::Type)EB_WM_LOGOUT);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_LOGOUT, 0, 0);
#endif
			}
			m_pMyAccountInfo = CEBAccountInfo::create(0,"");

			// 清空几个重要数据列表
			theEmployeeList.clear();
			theCallInfoList.clear();
			this->theDepartmentList.clear();
			BoostWriteLock wtLock(m_mutexEBDatas);
			if (m_pEBDatas.get()!=NULL)
			{
				m_pEBDatas->close();
				m_pEBDatas.reset();
			}
		}break;
	case EB_MSG_REQ_ADD_2_GROUP:
		{
			// 申请进入群组（部门）
			CEBGroupInfo::pointer pDepartmentInfo;
			if (!theDepartmentList.find(pMsgInfo->m_sGroupCode, pDepartmentInfo)) return;
			EB_AccountInfo * pAccountInfo = new EB_AccountInfo(pMsgInfo->m_nFromUserId,pMsgInfo->m_sFromAccount,0);
			GetECardByFromInfo(pMsgInfo->m_sFromInfo,&pAccountInfo->m_pFromCardInfo);
			EB_APMsgInfo pApMsgInfo;
			pApMsgInfo.m_nMsgId = pMsgInfo->GetMsgId();
			pApMsgInfo.m_nGroupId = pMsgInfo->m_sGroupCode;
			pApMsgInfo.m_nUserId = pMsgInfo->m_nFromUserId;
			pApMsgInfo.m_sAccount = pMsgInfo->m_sFromAccount;
			pApMsgInfo.m_sMsgName = pDepartmentInfo->m_sGroupName;
			pApMsgInfo.m_sMsgContent = pMsgInfo->GetMsgContent();	// description
			if (m_callback)
				m_callback->onRequestAdd2Group(pAccountInfo,&pApMsgInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pAccountInfo->SetQEventType((QEvent::Type)EB_WM_REQUEST_ADD2GROUP);
				pAccountInfo->SetEventData((void*)(const EB_APMsgInfo*)&pApMsgInfo);
                postWaitEventResult(m_pHwnd, pAccountInfo);
#else
				::SendMessage(m_pHwnd, EB_WM_REQUEST_ADD2GROUP, (WPARAM)pAccountInfo, (LPARAM)&pApMsgInfo);
				delete pAccountInfo;
#endif
			}
			else {
				delete pAccountInfo;
			}
			pOutAutoAck = false;
		}break;
	case EB_MSG_ADD_2_GROUP:
		{
			//////////// 添加进部门
			if (m_pMyAccountInfo.get()==NULL)
				break;

			// 某人加入部门，需要加载成员信息；
			CEBGroupInfo::pointer pDepartmentInfo;
			if (!theDepartmentList.find(pMsgInfo->m_sGroupCode, pDepartmentInfo))
			{
				// 加进部门，群组不存在，重新加载
				EnterpriseLoad(pMsgInfo->m_sGroupCode,1,1,1);
				return;
			}

			const std::string::size_type find = pMsgInfo->GetMsgContent().find(";");
			if (find==std::string::npos)
				break;
			const mycp::bigint nUserId = cgc_atoi64(pMsgInfo->GetMsgContent().substr(find+1).c_str());

			// 重新加载一次群组在线人数；
			//LoadInfo(0,0,0,0,0,0,pMsgInfo->m_sGroupCode);
			if (m_pWaitList2.insert(pMsgInfo->m_sGroupCode,0,false))
			{
				CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_LOAD_GROUP_OLS);
				pProcessMsgInfo->m_nCallGroupId = pMsgInfo->m_sGroupCode;
				pProcessMsgInfo->m_tProcessTime = time(0)+3;
				m_pProcessMsgList.add(pProcessMsgInfo);
			}
			if ((pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER)==0 &&
				pDepartmentInfo->m_sEnterpriseCode>0 &&
				pDepartmentInfo->m_nGroupVer==0 && pDepartmentInfo->m_pMemberList.empty())
			{
				if (nUserId==m_pMyAccountInfo->GetUserId())
				{
					EnterpriseLoad(pMsgInfo->m_sGroupCode,0,0,1);
				}else
				{
					pDepartmentInfo->m_nEmpCount += 1;
					const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
					if (m_callback)
						m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
						EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
						pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
						pEvent->SetEventParameter((long)(bIsMyDepartment?1:0));
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
					}
				}
				break;
			}

			if (!pDepartmentInfo->m_pMemberList.exist(nUserId))
			{
				// 成员不存在，加载该成员信息；
				const mycp::bigint nMemberId = cgc_atoi64(pMsgInfo->GetMsgContent().substr(0,find).c_str());
				if (pDepartmentInfo->m_nMyEmpId==0 && nUserId==m_pMyAccountInfo->GetUserId())
				{
					pDepartmentInfo->m_nMyEmpId = nMemberId;
				}
				EnterpriseLoad(0,0,0,0,nMemberId);
			}
		}break;
	case EB_MSG_GROUP_MEMBER_CHANGE:
		{
			// 群成员资料修改，事件通知
			CEBGroupInfo::pointer pDepartmentInfo;
			if (!theDepartmentList.find(pMsgInfo->m_sGroupCode, pDepartmentInfo))
			{
				break;
			}
			std::vector<tstring> pList;
			if (ParseString(pMsgInfo->GetMsgContent(),";",false,pList)<2) break;
			const mycp::bigint nMemberId = cgc_atoi64(pList[0].c_str());
			const mycp::bigint nUserId = cgc_atoi64(pList[1].c_str());
			//const int nType = pList.size()>=3?atoi(pList[2].c_str()):0;
			//const int nData = pList.size()>=4?atoi(pList[3].c_str()):0;

			//const std::string::size_type find = pMsgInfo->GetMsgContent().find(";");
			//if (find==std::string::npos)
			//	break;
			//const mycp::bigint nUserId = cgc_atoi64(pMsgInfo->GetMsgContent().substr(find+1).c_str());
			if (pDepartmentInfo->m_pMemberList.exist(nUserId))
			{
				// 成员存在，重新加载该成员信息；
				//const mycp::bigint nMemberId = cgc_atoi64(pMsgInfo->GetMsgContent().substr(0,find).c_str());
				EnterpriseLoad(0,0,0,0,nMemberId);
			}
		}break;
	case EB_MSG_REMOVE_GROUP:
	case EB_MSG_EXIT_GROUP:
		{
			std::vector<tstring> pList;
			const int nCount = ParseString(pMsgInfo->GetMsgContent(),";",false,pList);
			if (nCount==0) return;
			const mycp::bigint sRemoveExitEmpUserId = cgc_atoi64(pList[0].c_str());
			const mycp::bigint nNewGroupVer = nCount>=2?cgc_atoi64(pList[1].c_str()):0;
			CEBGroupInfo::pointer pDepartmentInfo;
			if (!theDepartmentList.find(pMsgInfo->m_sGroupCode, pDepartmentInfo)) return;

			if (nNewGroupVer>0)
				UpdateLocalGroupVer(pMsgInfo->m_sGroupCode,nNewGroupVer);

			// 重新加载一次群组在线人数；
			//LoadInfo(0,0,0,0,0,0,pMsgInfo->m_sGroupCode);
			if (pDepartmentInfo->m_sEnterpriseCode>0											// 企业
				|| sRemoveExitEmpUserId!=m_pMyAccountInfo->GetUserId())			// 或者删除的别人，需要加载一次在线人数
			{
				if (m_pWaitList2.insert(pMsgInfo->m_sGroupCode,0,false))
				{
					CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_LOAD_GROUP_OLS);
					pProcessMsgInfo->m_nCallGroupId = pMsgInfo->m_sGroupCode;
					pProcessMsgInfo->m_tProcessTime = time(0)+3;
					m_pProcessMsgList.add(pProcessMsgInfo);
				}
			}
			CEBMemberInfo::pointer pEmployeeInfo;
			if (!pDepartmentInfo->m_pMemberList.find(sRemoveExitEmpUserId, pEmployeeInfo))
			{
				if ((pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER)==0 &&
					pDepartmentInfo->m_nGroupVer==0 && pDepartmentInfo->m_pMemberList.empty() && pDepartmentInfo->m_nEmpCount>0)
				{
					// 未加载成员
					pDepartmentInfo->m_nEmpCount -= 1;
					const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
					if (m_callback)
						m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
					if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
						EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
						pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
						pEvent->SetEventParameter((long)(bIsMyDepartment?1:0));
                        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
						::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
					}
					return;
				}
				return;
			}

			// **记下删除标识，统计成员数量时，不会计算
			//pEmployeeInfo->m_bDeleteFlag = true;
            pDepartmentInfo->m_nEmpCount = max(0,(pDepartmentInfo->m_pMemberList.size()-1));
            if (pEmployeeInfo->IsFullOnineState() && pDepartmentInfo->m_nOnlineSize>0) {
                pDepartmentInfo->m_nOnlineSize -= 1;
            }
            SetLocalGroupEmpCount(pDepartmentInfo->m_sGroupCode,pDepartmentInfo->m_nEmpCount);
			if (pMsgInfo->GetMsgType()==EB_MSG_REMOVE_GROUP)
			{
				if (m_callback)
					m_callback->onRemoveGroup(pDepartmentInfo.get(),pEmployeeInfo.get());
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
					pEvent->SetQEventType((QEvent::Type)EB_WM_REMOVE_GROUP);
					pEvent->SetEventData((void*)(const EB_MemberInfo*)pEmployeeInfo.get());
                    postWaitEventResult(m_pHwnd, pEvent);
#else
					::SendMessage(m_pHwnd, EB_WM_REMOVE_GROUP, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(), (LPARAM)(const EB_MemberInfo*)pEmployeeInfo.get());
#endif
				}
            }
            else {
				if (m_callback)
					m_callback->onExitGroup(pDepartmentInfo.get(),pEmployeeInfo.get());
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
					pEvent->SetQEventType((QEvent::Type)EB_WM_EXIT_GROUP);
					pEvent->SetEventData((void*)(const EB_MemberInfo*)pEmployeeInfo.get());
                    postWaitEventResult(m_pHwnd, pEvent);
#else
					::SendMessage(m_pHwnd, EB_WM_EXIT_GROUP, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(), (LPARAM)(const EB_MemberInfo*)pEmployeeInfo.get());
#endif
				}
			}
			DeleteMemberInfo(pDepartmentInfo,pEmployeeInfo);

			//if (sRemoveExitEmpUserId == m_pMyAccountInfo->GetUserId())
			//{
			//	// 本人退出该群组（部门）
			//	if (m_pDefaultEntEmployeeInfo.get()!=NULL && m_pDefaultEntEmployeeInfo->m_sGroupCode==pMsgInfo->m_sGroupCode)
			//	{
			//		m_pDefaultEntEmployeeInfo.reset();
			//		m_pDefaultEntEmployeeInfo = GetMyEntEmployeeInfo();
			//		
			//		if (m_pDefaultEntEmployeeInfo.get()!=NULL)
			//		{
			//			if (m_callback)
			//				m_callback->onMemberInfo(pEmployeeInfo.get(),true);
			//			if (m_pHwnd!=NULL)
			//				::SendMessage(m_pHwnd, EB_WM_MEMBER_INFO, (WPARAM)pEmployeeInfo.get(), 1);
			//			// 重设置一次默认群组；
			//			this->SetDefaltMemberCode(0);
			//			//this->SetDefaltMemberCode(m_pDefaultEntEmployeeInfo->m_sMemberCode);
			//		}
			//	}

			//	if (pDepartmentInfo->m_sEnterpriseCode==0)
			//	{
			//		// 个人群组，删除全部数据；
			//		ClearGroupAllData(pDepartmentInfo);
			//	}else
			//	{
			//		// 企业部门，删除部分数据；
			//		ClearCallInfo(pDepartmentInfo->m_sGroupCode,true,true,true);			// 清除会话数据
			//		pDepartmentInfo->m_pMemberList.remove(m_pMyAccountInfo->GetUserId());	// 移除成员数据
			//		theMyEmployeeList.remove(pDepartmentInfo->m_sGroupCode);				// 移除我的成员数据

			//		if (m_pEBDatas.get()!=NULL)
			//		{
			//			char lpszBuffer[128];
			//			sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE emp_id=%lld",pEmployeeInfo->m_sMemberCode);
			//			m_pEBDatas->execsql(lpszBuffer);
			//		}
			//	}
			//}else
			//{
			//	// 删除该成员
			//	pDepartmentInfo->m_pMemberList.remove(sRemoveExitEmpUserId);
			//	theEmployeeList.remove(pEmployeeInfo->m_sMemberCode);
			//	CEBCallInfo::pointer pCallInfo;
			//	if (theCallInfoList.find(pMsgInfo->m_sGroupCode, pCallInfo))
			//	{
			//		pCallInfo->m_pCallUserList.remove(sRemoveExitEmpUserId);
			//	}

			//	if (m_pEBDatas.get()!=NULL)
			//	{
			//		char lpszBuffer[128];
			//		sprintf(lpszBuffer,"DELETE FROM eb_emp_info_t WHERE emp_id=%lld",pEmployeeInfo->m_sMemberCode);
			//		m_pEBDatas->execsql(lpszBuffer);
			//	}
			//}
		}break;
	case EB_MSG_DELETE_GROUP:
		{
			theCheckGroupMemberList.remove(pMsgInfo->m_sGroupCode);
			theCheckGroupInfoList.remove(pMsgInfo->m_sGroupCode);

			const mycp::bigint nNewGroupInfoVer = cgc_atoi64(pMsgInfo->GetMsgContent().c_str());
			CEBGroupInfo::pointer pDepartmentInfo;
			if (!theDepartmentList.find(pMsgInfo->m_sGroupCode, pDepartmentInfo)) return;

			pDepartmentInfo->m_nGroupData |= EB_GROUP_DATA_DELETE;
			const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
			if (m_callback)
				m_callback->onGroupDelete(pDepartmentInfo.get(),bIsMyDepartment);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_DELETE);
				pEvent->SetEventParameter((long)(bIsMyDepartment?1:0));
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_GROUP_DELETE, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(), bIsMyDepartment?1:0);
#endif
			}
			UpdateLocalGroupInfoVer(pDepartmentInfo->m_sEnterpriseCode,nNewGroupInfoVer);
			ClearGroupAllData(pDepartmentInfo);
		}break;
	case EB_MSG_BROADCAST_MESSAGE:
		{
			//pOutAutoAck = false;// ???
			// 收到一条广播消息；
			EB_AccountInfo * pAccountInfo = new EB_AccountInfo(pMsgInfo->m_nFromUserId,pMsgInfo->m_sFromAccount,0);

            EB_APMsgInfo pApMsgInfo;
            pApMsgInfo.m_sFromUserId = pMsgInfo->m_nFromUserId;
            pApMsgInfo.m_nMsgId = pMsgInfo->GetMsgId();	// 自动响应即可
            pApMsgInfo.m_nMsgType = pMsgInfo->GetMsgSubType();
            pApMsgInfo.m_sMsgName = pMsgInfo->GetMsgName();
            pApMsgInfo.m_sMsgContent = pMsgInfo->GetMsgContent();
            pApMsgInfo.m_nUserId = pMsgInfo->m_nFromUserId;
            pApMsgInfo.m_sAccount = pMsgInfo->m_sFromAccount;
			if (m_callback)
                m_callback->onBroadcastMsg(pAccountInfo,&pApMsgInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				pAccountInfo->SetQEventType((QEvent::Type)EB_WM_BROADCAST_MSG);
                pAccountInfo->SetEventData((void*)(const EB_APMsgInfo*)&pApMsgInfo);
                postWaitEventResult(m_pHwnd, pAccountInfo);
#else
                ::SendMessage(m_pHwnd, EB_WM_BROADCAST_MSG, (WPARAM)pAccountInfo, (LPARAM)&pApMsgInfo);
				delete pAccountInfo;
#endif
			}
			else {
				delete pAccountInfo;
            }
		}break;
	case EB_MSG_ENT_GROUP_INFO_VERSION:
		{
			// 企业部门资料版本
			const mycp::bigint nServerEntDepInfoVer = cgc_atoi64(pMsgInfo->GetMsgContent().c_str());
			if (m_nServerEntDepInfoVer!=nServerEntDepInfoVer)
			{
				m_bNeedLoadVerInfo = false;
				LoadInfo(0,0,1);
			}
		}break;
	case EB_MSG_GROUP_MEMBER_VERSION:
		{
			// 部门成员版本；
			std::vector<tstring> pList;
			if (ParseString(pMsgInfo->GetMsgContent(),",",false,pList)!=2)
				break;
			const mycp::bigint nServerGroupVer = cgc_atoi64(pList[0].c_str());
			const int nGroupMemberCount = atoi(pList[1].c_str());

			CEBGroupInfo::pointer pDepartmentInfo;
			if (!theDepartmentList.find(pMsgInfo->m_sGroupCode, pDepartmentInfo))
				break;

			const mycp::bigint nLocalOldGroupVer = GetLocalGroupVer(pMsgInfo->m_sGroupCode);
			pDepartmentInfo->m_nServerGroupVer = nServerGroupVer;
			UpdateLocalGroupInfoVer(pDepartmentInfo->m_sEnterpriseCode,nServerGroupVer,true);
			if (pDepartmentInfo->m_nEmpCount != nGroupMemberCount)
			{
				pDepartmentInfo->m_nEmpCount = nGroupMemberCount;
				SaveLocalGroupInfo(pDepartmentInfo);
				const bool bIsMyDepartment = pDepartmentInfo->m_nMyEmpId>0;
				if (m_callback)
					m_callback->onGroupInfo(pDepartmentInfo.get(),bIsMyDepartment);
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					EB_GroupInfo * pEvent = new EB_GroupInfo(pDepartmentInfo.get());
					pEvent->SetQEventType((QEvent::Type)EB_WM_GROUP_INFO);
					pEvent->SetEventParameter((long)(bIsMyDepartment?1:0));
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
					::SendMessage(m_pHwnd, EB_WM_GROUP_INFO, (WPARAM)(const EB_GroupInfo*)pDepartmentInfo.get(),bIsMyDepartment?1:0);
#endif
				}
			}else
			{
				SaveLocalGroupInfo(pDepartmentInfo);
			}

			// 重新加载一次群组在线人数；
			//LoadInfo(0,0,0,0,0,0,pMsgInfo->m_sGroupCode);
			if (m_pWaitList2.insert(pMsgInfo->m_sGroupCode,0,false))
			{
				CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_LOAD_GROUP_OLS);
				pProcessMsgInfo->m_nCallGroupId = pMsgInfo->m_sGroupCode;
				pProcessMsgInfo->m_tProcessTime = time(0)+2;
				m_pProcessMsgList.add(pProcessMsgInfo);
			}
			if ((pDepartmentInfo->m_nGroupData&EB_GROUP_DATA_LOAD_MEMBER)==0)
			{
				// 本地没有缓存数据，不需要加载成员
				break;
			}
			if (nLocalOldGroupVer!=nServerGroupVer)
			{
				// 群组（部门）成员版本不对，重新加载该群组成员
				if (pDepartmentInfo->m_nMyEmpId>0)	// ** 属于我的部门
					theNeedLoadGroupMemberList.pushfront(pMsgInfo->m_sGroupCode);
				else
					theNeedLoadGroupMemberList.add(pMsgInfo->m_sGroupCode);
			}
		}break;
	default:
		break;
	}
}

void CUserManagerApp::OnUMSMLoadResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	if (nResultValue == EB_STATE_OK)
	{
		m_pSysInfo.m_sAuthMsgUrl = pSotpResponseInfo->m_pResponseList.getParameterValue("auth-msg-url");
		if ((m_pMyAccountInfo->m_dwAccountData&EB_DATA_NEED_CHANGE_LINESTATE)==EB_DATA_NEED_CHANGE_LINESTATE)
		{
			// 更改状态通知
			m_pMyAccountInfo->m_dwAccountData &= ~EB_DATA_NEED_CHANGE_LINESTATE;
			int nRealLineState = m_pMyAccountInfo->GetLineState();
            m_pUserManager->SendUMSMOnline(m_nSDKVersion,
                                           m_pMyAccountInfo->GetLogonType(),
                                           m_pMyAccountInfo->GetUserId(),
                                           m_pMyAccountInfo->m_pLogonList.GetOnlineKey(""),
                                           nRealLineState|EB_USER_CHANGE_STATE,
                                           0,
                                           m_pMyAccountInfo->m_pLogonList.GetUserSignId(""),
                                           m_sLogonUserData);
		}
	}
}

void CUserManagerApp::OnUMSMOnlineResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (m_userStatus != US_Logging)
		return ;

	const EB_AccountInfo* pAccountInfo = m_pMyAccountInfo.get(); 
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	const tstring sAccessToken(pSotpResponseInfo->m_pResponseList.getParameterValue("access_token"));
	const mycp::bigint nUserSignId = pSotpResponseInfo->m_pResponseList.getParameterValue("us_id",(mycp::bigint)0);
	const mycp::bigint nDefaultEntId = pSotpResponseInfo->m_pResponseList.getParameterValue("ent_id",(mycp::bigint)0);
	const tstring sEmoVerString(pSotpResponseInfo->m_pResponseList.getParameterValue("emo_ver"));
	const mycp::bigint nMyGroupInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("my_group_info_ver",(mycp::bigint)0);
	const mycp::bigint nMyContactInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("my_contact_info_ver",(mycp::bigint)0);
	const mycp::bigint nEntDepInfoVer = pSotpResponseInfo->m_pResponseList.getParameterValue("ent_dep_info_ver",(mycp::bigint)0);
	const tstring sEntManagerUrl(pSotpResponseInfo->m_pResponseList.getParameterValue("ent_manager_url"));
	const int nOnlineExtData = pSotpResponseInfo->m_pResponseList.getParameterValue("ext_data",(int)0);
	//const int nEntExt = pSotpResponseInfo->m_pResponseList.getParameterValue("ent_ext",(int)0);
	//LogMessage("OnUMSMOnlineResponse... (m_tReLogonTime=%lld,nResultValue=%d)\r\n",(mycp::bigint)m_tReLogonTime,(int)nResultValue);
	if (nResultValue == EB_STATE_OK)
	{
		if (m_nUmServerState!=EB_SS_INIT)
		{
			m_nUmServerState=EB_SS_INIT;
		}

		BoostReadLock rdLock(m_mutexEBDatas);
		// 加载本地表情数据
		if (!sEmoVerString.empty() && this->m_pEBDatas.get()!=0)
		{
			tstring sEmoList;
			char lpszBuffer[512];
			bool bSameEmoVersion = true;
			std::vector<tstring> pList;
			std::vector<tstring> pVerList;
			const int nCount = ParseString(sEmoVerString,";",false,pList);
			for (int i=0; i<nCount; i++)
			{
				const tstring sEmoVer = pList[i];
				if (ParseString(sEmoVer,",",true,pVerList)==2)
				{
					const mycp::bigint nEmoId = cgc_atoi64(pVerList[0].c_str());
					const int nVersion = atoi(pVerList[1].c_str());
					if (this->GetLocalEmoVer(nEmoId)!=nVersion)
					{
						bSameEmoVersion = false;
						break;
					}else
					{
						if (sEmoList.empty())
							sprintf(lpszBuffer,"%lld",nEmoId);
						else
							sprintf(lpszBuffer,",%lld",nEmoId);
						sEmoList.append(lpszBuffer);
					}
				}else
				{
					bSameEmoVersion = false;
					break;
				}
			}

			if (bSameEmoVersion)
			{
				// ***
				sprintf(lpszBuffer,"SELECT emo_id,res_id,cm_server,nindex,ntype,ent_id,res_type,res_md5,desc,cm_sid FROM eb_emo_info_t WHERE emo_id IN (%s)",sEmoList.c_str());
				int nCookie = 0;
				m_pEBDatas->select(lpszBuffer, nCookie);
				cgcValueInfo::pointer pRecord = m_pEBDatas->first(nCookie);
				while (pRecord.get()!=0)
				{
					const mycp::bigint sEmoId = pRecord->getVector()[0]->getBigIntValue();
					const mycp::bigint sResourceId = pRecord->getVector()[1]->getBigIntValue();
                    /// for test
//                   if (5433726958570064LL==sResourceId) {
//                        int i=0;
//                    }
					const tstring sCmServer = pRecord->getVector()[2]->getStrValue();
					const int nIndex = pRecord->getVector()[3]->getIntValue();
					const int nType = pRecord->getVector()[4]->getIntValue();
					const mycp::bigint sEntCode = pRecord->getVector()[5]->getBigIntValue();
					const int nResType = pRecord->getVector()[6]->getIntValue();
					const tstring sResMd5 = pRecord->getVector()[7]->getStrValue();
#ifdef _QT_MAKE_
                    tstring sDescription(pRecord->getVector()[8]->getStrValue());
//                    tstring sDescription(CEBParseSetting::utf82str(pRecord->getVector()[8]->getStrValue().c_str(),"BGK"));
#else
					tstring sDescription(CEBParseSetting::str_convert(pRecord->getVector()[8]->getStrValue().c_str(),CP_UTF8,CP_ACP));
#endif
					CSqliteCdbc::escape_string_out(sDescription);
					const int nCmServiceId = pRecord->getVector()[9]->getIntValue();
					pRecord = m_pEBDatas->next(nCookie);

					CEBEmotionInfo::pointer pEmotionInfo;
					char sEnterpriseImageKey[48];
					sprintf(sEnterpriseImageKey,"%lld%lld",sResourceId,sEntCode);
					if (!theEmotionList.find(sEnterpriseImageKey,pEmotionInfo))
					{
						pEmotionInfo = CEBEmotionInfo::create(sResourceId,nIndex,nType,m_pMyAccountInfo->GetUserId(),sEntCode,(EB_RESOURCE_TYPE)nResType);
						theEmotionList.insert(sEnterpriseImageKey,pEmotionInfo);
					}else
					{
						pEmotionInfo->m_nIndex = nIndex;
						pEmotionInfo->m_nType = nType;
						pEmotionInfo->m_nResType = (EB_RESOURCE_TYPE)nResType;
					}
					pEmotionInfo->m_nCmServiceId = nCmServiceId;
					pEmotionInfo->m_nEmoId = sEmoId;
					pEmotionInfo->m_sResMd5 = sResMd5;
					pEmotionInfo->m_sDescription = sDescription;
					pEmotionInfo->m_sCmServer = sCmServer;
					//pEmotionInfo->m_sCmAppName = sCmAppName;
#ifdef _QT_MAKE_
                    const QString fileTemp = QString("%1/%2").arg(m_sEbResourcePath).arg(sResourceId);
                    strcpy(lpszBuffer,fileTemp.toStdString().c_str());
                    if (QFileInfo::exists(fileTemp)) {

                        if (!sResMd5.empty()) {
                            // 检查文件MD5是否相同；
                            mycp::bigint nFileSize = 0;
                            tstring sFileMd5String;
                            GetFileMd5(lpszBuffer,nFileSize,sFileMd5String);
                            if (sFileMd5String==sResMd5)
                            {
                                pEmotionInfo->m_sResFile = fileTemp;
                            }
                        }
                        else {
                            pEmotionInfo->m_sResFile = fileTemp;
                        }
                    }
#else
                    sprintf(lpszBuffer,"%s\\%lld",m_sEbResourcePath.c_str(),sResourceId);
                    const tstring fileTemp = lpszBuffer;
                    if (::PathFileExistsA(lpszBuffer))
					{
						if (!sResMd5.empty())
						{
							// 检查文件MD5是否相同；
							mycp::bigint nFileSize = 0;
							tstring sFileMd5String;
							GetFileMd5(lpszBuffer,nFileSize,sFileMd5String);
							if (sFileMd5String==sResMd5)
							{
								pEmotionInfo->m_sResFile = lpszBuffer;
							}
						}else
						{
							pEmotionInfo->m_sResFile = lpszBuffer;
						}
					}
#endif

#ifdef _QT_MAKE_
                    if (pEmotionInfo->m_sResFile.isEmpty() && !theResourceChatRoom.exist(sResourceId))
#else
                    if (pEmotionInfo->m_sResFile.empty() && !theResourceChatRoom.exist(sResourceId))
#endif
					{
						//pEmotionInfo->m_sResFile = sResourceFile;	// 下载成功再保存
						// 资源文件不存在（或MD5不对，下载不完整），下载
						const tstring sCmServerKey = GetCmServerKey(sCmServer);
						Cchatroom::pointer pChatRoom;
						if (!theChatRoomList.find(sCmServerKey,pChatRoom))
						{
							pChatRoom = Cchatroom::create();
							pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
							pChatRoom->Start(sCmServer.c_str(), "",0,true,1);
							pChatRoom->SetCallBack(this);
							theChatRoomList.insert(sCmServerKey,pChatRoom);
						}
						char lpszKey[200];
						sprintf(lpszKey,"%s%lld",sCmServerKey.c_str(),sResourceId);
						//sprintf(lpszKey,"%s%lld",sCmServer.c_str(),sResourceId);
						theChatRoomList.insert(lpszKey,pChatRoom,false);
						theResourceChatRoom.insert(sResourceId,pChatRoom);
                        CFilePathInfo::pointer pFilePathInfo = CFilePathInfo::create(CFilePathInfo::FILE_ENT_IMAGE,fileTemp,false);
						pFilePathInfo->m_nBigData = sEntCode;
						theResourceFilePath.insert(sResourceId,pFilePathInfo);
						//pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
						//pChatRoom->RecviveResource(sResourceId,lpszBuffer);
						CProcessMsgInfo::pointer pProcessMsgInfo = CProcessMsgInfo::create(CProcessMsgInfo::PROCESS_MSG_TYPE_RECEIVE_RESOURCE_FILE);
						pProcessMsgInfo->m_pChatRoom = pChatRoom;
						pProcessMsgInfo->m_nBigInt1 = sResourceId;
                        pProcessMsgInfo->m_sString1 = fileTemp;
						//pProcessMsgInfo->m_sString1.append(".ebtemp");
						m_pProcessMsgList.add(pProcessMsgInfo);
						//Sleep(50);
					}
				}
				m_pEBDatas->reset(nCookie);
				m_bExistLocalImage = theEmotionList.empty()?false:true;
			}else if (!sEmoList.empty())
			{
				sprintf(lpszBuffer,"DELETE FROM eb_emo_info_t WHERE emo_id NOT IN (%s)",sEmoList.c_str());
				m_pEBDatas->execute(lpszBuffer);
				sprintf(lpszBuffer,"DELETE FROM eb_emo_ver_t WHERE emo_id NOT IN (%s)",sEmoList.c_str());
				m_pEBDatas->execute(lpszBuffer);
			}
		}

		m_sEntManagerUrl = sEntManagerUrl;
		m_nOnlineExtData = nOnlineExtData;
        // write usersignid
		char sTempFile[260];
#ifdef _QT_MAKE_
        sprintf(sTempFile,"%s/ebc.temp",m_sAppDataTempPath.toLocal8Bit().constData());
#else
        sprintf(sTempFile,"%s\\ebc.temp",m_sAppDataTempPath.c_str());
#endif
		char lpszBuffer[24];
		sprintf(lpszBuffer,"%lld",nUserSignId);
#ifdef WIN32
        ::WritePrivateProfileStringA(_T("temp"),_T("us_id"),lpszBuffer,sTempFile);
#else
        WritePrivateProfileStringABoost(_T("temp"),_T("us_id"),lpszBuffer,sTempFile);
#endif
		m_pMyAccountInfo->m_pLogonList.SetUserSignId("",nUserSignId);
		m_pMyAccountInfo->SetUserId(pUMOwner->GetUserId());
		m_pMyAccountInfo->SetDefaultEntId(nDefaultEntId);
		m_pMyAccountInfo->SetMyGroupInfoVer(nMyGroupInfoVer);
		m_pMyAccountInfo->SetEntDepInfoVer(nEntDepInfoVer);
		m_pMyAccountInfo->SetContactInfoVer(nMyContactInfoVer);
		m_pMyAccountInfo->SetOnlineTime(time(0));
		m_pMyAccountInfo->SetAccessToken(sAccessToken);
		m_userStatus = US_Logoned;
	
		if (this->m_callback)
			m_callback->onLogonSuccess(*pAccountInfo);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
			pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_SUCCESS);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::PostMessage(m_pHwnd, EB_WM_LOGON_SUCCESS, (WPARAM)pAccountInfo, 0);
#endif
		}
		m_tReLoadInfo = time(0)-1;	// 立即开始加载系统消息
		if (m_tReLogonTime>0)
		{
			m_tReLogonTime = 0;	// 登录成功，设为0；
			SendMyLineState(m_pMyAccountInfo->GetLineState());
		}
	}else if (m_tReLogonTime>0)
	{
		if (nResultValue==EB_STATE_APPID_KEY_ERROR)
			m_sDevAppOnlineKey.clear();
		m_tReLogonTime = time(0)+5+(rand()%10);	// 重新登录错误，继续重新登录；
	}else
	{
		m_userStatus = US_LogonError;
		//if (m_tReLogonTime>0 && nResultValue==EB_STATE_ACC_PWD_ERROR)
		//	m_tReLogonTime = 0;
		if (m_callback)
			m_callback->onLogonError(*pAccountInfo,(EB_STATE_CODE)nResultValue);
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
			pEvent->SetQEventType((QEvent::Type)EB_WM_LOGON_ERROR);
			pEvent->SetEventParameter((long)nResultValue);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::PostMessage(m_pHwnd, EB_WM_LOGON_ERROR,(WPARAM)pAccountInfo,nResultValue);
#endif
		}
	}
}
void CUserManagerApp::OnUMReg(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const mycp::bigint nUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("uid",(mycp::bigint)0);
	//const int nRegCode = pSotpResponseInfo->m_pResponseList.getParameterValue("reg_code",(int)0);
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	if (this->m_callback)
		m_callback->onRegisterResponse((EB_STATE_CODE)nResultValue,nUserId);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_REGISTER_RESPONSE);
//		pEvent->SetQEventType((QEvent::Type)EB_WM_REGISTER_RESPONSE);
		pEvent->SetEventParameter((long)nResultValue);
        pEvent->SetEventBigParameter(nUserId);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
        char lpszUserId[24];
        sprintf(lpszUserId,"%lld",nUserId);
        ::SendMessage(m_pHwnd, EB_WM_REGISTER_RESPONSE, nResultValue, (LPARAM)&lpszUserId);
#endif
	}
}
void CUserManagerApp::OnUMSInfo(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const int nChangePwd = pSotpRequestInfo.get()==NULL?0:pSotpRequestInfo->m_pRequestList.getParameterValue("change-pwd",(int)0);
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	if (nResultValue==EB_STATE_OK && pSotpRequestInfo.get()!=NULL)
	{
		const cgcParameter::pointer pNewPassword = pSotpRequestInfo->m_pRequestList.getParameter("new_pwd");
		if (pNewPassword.get()!=NULL)
		{
			m_pMyAccountInfo->SetPasswd(pNewPassword->getStr());
			//MessageBox(NULL,m_pMyAccountInfo->GetPasswd().c_str(),"",MB_OK);
		}
		const cgcParameter::pointer pDefaultEmp = pSotpRequestInfo->m_pRequestList.getParameter("default-emp-id");
		if (pDefaultEmp.get()!=NULL && pDefaultEmp->getBigInt()!=m_pMyAccountInfo->GetDefaultEmpId())
		{
			m_pMyAccountInfo->SetDefaultEmpId(pDefaultEmp->getBigInt());
			if (m_pMyAccountInfo->GetDefaultEmpId()==0)
			{
				m_pDefaultEntEmployeeInfo.reset();
			}else
			{
				CEBMemberInfo::pointer pEmployeeInfo;
				if (!theEmployeeList.find(m_pMyAccountInfo->GetDefaultEmpId(),pEmployeeInfo))
				{
					return;
				}
				if (pEmployeeInfo->m_nMemberUserId!=m_pMyAccountInfo->GetUserId())
				{
					return;
				}
				m_pDefaultEntEmployeeInfo = pEmployeeInfo;
				//if (m_pDefaultEnterpriseInfo.get()!=NULL && m_pDefaultEnterpriseInfo->m_sEnterpriseCode>0)	// 企业员工，切换默认名片，同步更新到个人中心名称
				//{
				//	m_pMyAccountInfo->SetUserName(pEmployeeInfo->m_sUserName);
				//}
				if (m_callback)
					m_callback->onMemberInfo(pEmployeeInfo.get(),true);
				if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
					EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
					pEvent->SetQEventType((QEvent::Type)EB_WM_MEMBER_INFO);
					pEvent->SetEventParameter((long)1);
                    pEvent->SetEventBigParameter(theMyEmployeeList.exist(pEmployeeInfo->m_sGroupCode)?1:0);
                    QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
					::SendMessage(m_pHwnd, EB_WM_MEMBER_INFO, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), 1);
#endif
				}
			}
		}
	}
	if (this->m_callback)
		m_callback->onEditInfoResponse((EB_STATE_CODE)nResultValue, nChangePwd);
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
        EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_EDITINFO_RESPONSE);
//		pEvent->SetQEventType((QEvent::Type)EB_WM_EDITINFO_RESPONSE);
		pEvent->SetEventParameter((long)nResultValue);
        pEvent->SetEventBigParameter((mycp::bigint)nChangePwd);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::PostMessage(m_pHwnd, EB_WM_EDITINFO_RESPONSE, nResultValue, nChangePwd);
#endif
	}
}
void CUserManagerApp::OnUMSHead(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return ;
	const mycp::bigint sDepCode = pSotpRequestInfo->m_pRequestList.getParameterValue("dep-code",(mycp::bigint)0);
    const EBFileString sFilePath(pSotpRequestInfo->m_pRequestList.getParameterValue("file-path"));
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	const tstring sHeadCmServer(pSotpResponseInfo->m_pResponseList.getParameterValue("cm-server"));
	//const tstring sHeadCmAppName = pSotpResponseInfo->m_pResponseList.getParameterValue("cm-appname");
	const mycp::bigint sHeadResourceId = pSotpResponseInfo->m_pResponseList.getParameterValue("h-r-id",(mycp::bigint)0);
	const tstring sHeadResourceMd5(pSotpResponseInfo->m_pResponseList.getParameterValue("h-r-md5"));

	if (nResultValue == EB_STATE_OK)
	{
		// 请求上传资源文件
		CEBGroupInfo::pointer pDepartmentInfo;
		if (!theDepartmentList.find(sDepCode, pDepartmentInfo))
		{
			return ;
		}
		CEBMemberInfo::pointer pEmployeeInfo;
		if (!pDepartmentInfo->m_pMemberList.find(m_pMyAccountInfo->GetUserId(),pEmployeeInfo))
		{
			return ;
		}
		DeleteOldHeadFile(pEmployeeInfo);

#ifdef _QT_MAKE_
        const QString lpszHeadResourceFile = QString("%1/%2").arg(m_sEbResourcePath).arg(sHeadResourceId);
#else
        char lpszHeadResourceFile[260];
		sprintf(lpszHeadResourceFile,"%s\\%lld",m_sEbResourcePath.c_str(),sHeadResourceId);
#endif
		pEmployeeInfo->m_sHeadResourceId = sHeadResourceId;

		if (sHeadCmServer.empty())// || sHeadCmAppName.empty())
		{
            /// 不需要上传头像
			if (m_pDefaultEntEmployeeInfo.get()!=NULL && m_pDefaultEntEmployeeInfo.get()!=pEmployeeInfo.get() &&
				m_pDefaultEntEmployeeInfo->m_sMemberCode==pEmployeeInfo->m_sMemberCode && m_pDefaultEntEmployeeInfo->m_sHeadResourceId!=sHeadResourceId)
			{
				m_pDefaultEntEmployeeInfo->m_sHeadResourceId = sHeadResourceId;
				m_pDefaultEntEmployeeInfo->m_sHeadMd5 = sHeadResourceMd5;
				ProcessMemberHeadInfo(m_pDefaultEntEmployeeInfo);
			}

			pEmployeeInfo->m_sHeadResourceFile = lpszHeadResourceFile;
			pEmployeeInfo->m_sHeadMd5 = sHeadResourceMd5;
			if (this->m_callback)
				m_callback->onUserHeadChange(pEmployeeInfo.get(),true);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_MemberInfo * pEvent = new EB_MemberInfo(pEmployeeInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_USER_HEAD_CHANGE);
				pEvent->SetEventParameter((long)1);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_USER_HEAD_CHANGE, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(),1);
#endif
			}
		}else
		{
			// 需要上传头像文件
			const tstring sCmServerKey = GetCmServerKey(sHeadCmServer);
			Cchatroom::pointer pChatRoom;
			if (!theChatRoomList.find(sCmServerKey,pChatRoom))
			{
				pChatRoom = Cchatroom::create();
				pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
				pChatRoom->Start(sHeadCmServer.c_str(), "",0,true,1);
				theChatRoomList.insert(sCmServerKey,pChatRoom);
			}
			theResourceChatRoom.insert(sHeadResourceId,pChatRoom);
			theResourceFilePath.insert(sHeadResourceId,CFilePathInfo::create(CFilePathInfo::FILE_PAHT_HEAD,sFilePath,false));
			pChatRoom->SetCallBack(this);
			//pChatRoom->SetParam(sHeadResourceId);
			pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),
				m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
#ifdef _QT_MAKE_
            pChatRoom->SendResource(sHeadResourceId,sFilePath,true);
#else
			pChatRoom->SendResource(sHeadResourceId,sFilePath.c_str(),true);
#endif
		}
	}else
	{
	}
	//if (m_pHwnd!=NULL)
	//	::PostMessage(m_pHwnd, POP_WM_SINFO_RESPONSE, nResultValue, 0);
}
void CUserManagerApp::OnUMQuery(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const mycp::bigint nFromUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("f-uid",(mycp::bigint)0);
	const tstring sFromUserAccount(pSotpResponseInfo->m_pResponseList.getParameterValue("f-account"));
	const tstring sFromInfo(pSotpResponseInfo->m_pResponseList.getParameterValue("f-info"));
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	const mycp::bigint nSendCardCallId = pSotpRequestInfo.get()==NULL?0:pSotpRequestInfo->m_pRequestList.getParameterValue("send-usercard-call-id",(mycp::bigint)0);
	if (nSendCardCallId>0)
	{
		if (nResultValue!=EB_STATE_OK)
		{
			if (m_callback)
				m_callback->onStateCode((EB_STATE_CODE)nResultValue,0);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_STATE_CODE);
//				pEvent->SetQEventType((QEvent::Type)EB_WM_STATE_CODE);
				pEvent->SetEventParameter((long)nResultValue);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_STATE_CODE, (WPARAM)nResultValue, 0);
#endif
			}
			return;
		}
		const mycp::bigint nSendCardToUid = pSotpRequestInfo->m_pRequestList.getParameterValue("send-usercard-to-uid",(mycp::bigint)0);
		const bool bSendCardPrivate = pSotpRequestInfo->m_pRequestList.getParameterValue("send-usercard-private",false);
#ifdef _QT_MAKE_
        const std::string sUserECardString(sFromInfo.string());
#else
		const std::string sUserECardString = CEBParseSetting::str_convert(sFromInfo.c_str(),CP_ACP,CP_UTF8);
#endif
		EB_ChatRoomRichMsg pRichMsg;
		pRichMsg.SetSubType(EB_RICH_SUB_TYPE_CARD_INFO);
		const size_t nDataSize = sUserECardString.size();
		char * pObjectData = new char[nDataSize+16];
        const int n = sprintf(pObjectData,"%d,",1);			/// 1=用户名片
		memcpy(pObjectData+n,sUserECardString.c_str(),nDataSize);
		pRichMsg.AddObject(pObjectData,n+nDataSize);
		SendCrRich(nSendCardCallId,&pRichMsg,nSendCardToUid,bSendCardPrivate);
		return;
	}

	if (nFromUserId==m_pMyAccountInfo->GetUserId())
	{
		EB_ECardInfo pECardInfo;
		if (GetECardByFromInfo(sFromInfo,&pECardInfo))
		{
			m_pMyAccountInfo->SetUserName(pECardInfo.m_sAccountName);
			if (pECardInfo.m_nHeadResId==0)
			{
				m_pMyAccountInfo->SetMobile(pECardInfo.m_sPhone);
				m_pMyAccountInfo->SetTel(pECardInfo.m_sTel);
				m_pMyAccountInfo->SetEmail(pECardInfo.m_sEmail);
			}
			const EB_AccountInfo* pAccountInfo = m_pMyAccountInfo.get(); 
			if (this->m_callback)
				m_callback->onAccountInfoChange(*pAccountInfo);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_AccountInfo * pEvent = new EB_AccountInfo(pAccountInfo);
				pEvent->SetQEventType((QEvent::Type)EB_WM_ACCOUNT_INFO_CHANGE);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_ACCOUNT_INFO_CHANGE, (WPARAM)pAccountInfo, 0);
#endif
			}
			//m_pMyAccountInfo->SetDescription(pECardInfo.m_sPhone);
			if (pECardInfo.m_nHeadResId>0 && m_pDefaultEntEmployeeInfo.get()!=NULL &&
				(m_pDefaultEntEmployeeInfo->m_sHeadResourceId!=pECardInfo.m_nHeadResId || m_pDefaultEntEmployeeInfo->m_sHeadMd5!=pECardInfo.m_sHeadMd5))
			{
				//SplitString(sFromInfo, "hcms=", ";", pOutECardInfo->m_sHeadCmServer);
				//SplitString(sFromInfo, "hhs=", ";", pOutECardInfo->m_sHeadHttpServer);
				//SplitString(sFromInfo, "hmd5=", ";", pOutECardInfo->m_sHeadMd5);

				//if (m_callback)
				//	m_callback->onMemberInfo(pEmployeeInfo.get(),true);
				//if (m_pHwnd!=NULL)
				//	::SendMessage(m_pHwnd, EB_WM_MEMBER_INFO, (WPARAM)(const EB_MemberInfo*)pEmployeeInfo.get(), 1);

			}
		}
	}
}

void CUserManagerApp::OnResDeleteResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return;
	const mycp::bigint sResId = pSotpRequestInfo->m_pRequestList.getParameterValue("r_id",(mycp::bigint)0);
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	if (nResultValue == EB_STATE_OK || nResultValue==EB_STATE_RES_NOT_EXIST)
	{
		CEBResourceInfo::pointer pResourceInfo;
		if (!theResourceList.find(sResId,pResourceInfo,true))
		{
			return;
		}
		if (this->m_callback)
			m_callback->onResourceDelete(pResourceInfo.get());
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_ResourceInfo * pEvent = new EB_ResourceInfo(pResourceInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_RESOURCE_DELETE);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_RESOURCE_DELETE, (WPARAM)(const EB_ResourceInfo*)pResourceInfo.get(), 0);
#endif
		}
	}
}
void CUserManagerApp::OnResEditResponse(const CPOPSotpRequestInfo::pointer & pSotpRequestInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	if (pSotpRequestInfo.get() == NULL) return;
	const int nResType = pSotpRequestInfo->m_pRequestList.getParameterValue("type",(int)EB_RESOURCE_UNKNOWN);
	const tstring sResName(pSotpRequestInfo->m_pRequestList.getParameterValue("res-name"));
	const tstring sDescription(pSotpRequestInfo->m_pRequestList.getParameterValue("description"));
	const int nShare = pSotpRequestInfo->m_pRequestList.getParameterValue("share",(int)EB_RESOURCE_SHARE_UNKNOWN);
	const bool bEdit = pSotpRequestInfo->m_pRequestList.getParameterValue("edit",false);
	const bool bMove = pSotpRequestInfo->m_pRequestList.getParameterValue("move",false);

	// move
	const mycp::bigint sOldParentResId = pSotpRequestInfo->m_pRequestList.getParameterValue("old-parent-res-id",(mycp::bigint)0);
	const mycp::bigint sNewParentResId = pSotpRequestInfo->m_pRequestList.getParameterValue("new-parent-res-id",(mycp::bigint)0);
	
    const EBFileString sFilePath(pSotpRequestInfo->m_pRequestList.getParameterValue("file-path"));
	const mycp::bigint sParentResId = pSotpRequestInfo->m_pRequestList.getParameterValue("pr_id",(mycp::bigint)0);
	const mycp::bigint nFromId = pSotpRequestInfo->m_pRequestList.getParameterValue("from-id",(mycp::bigint)0);
	const EB_RESOURCE_FROM_TYPE nFromType = (EB_RESOURCE_FROM_TYPE)pSotpRequestInfo->m_pRequestList.getParameterValue("from-type",(int)EB_RESOURCE_FROM_TYPE_UNKNOWN);
	//const mycp::bigint sDepCode = pSotpRequestInfo->m_pRequestList.getParameterValue("dep-code",(mycp::bigint)0);
	//const mycp::bigint sEntCode = pSotpRequestInfo->m_pRequestList.getParameterValue("ent-code",(mycp::bigint)0);
	const int nResultValue = pSotpResponseInfo->GetResultValue();
	const tstring sResCmServer(pSotpResponseInfo->m_pResponseList.getParameterValue("cm-server"));
	//const tstring sResCmAppName = pSotpResponseInfo->m_pResponseList.getParameterValue("cm-appname");
	const mycp::bigint sResId = pSotpResponseInfo->m_pResponseList.getParameterValue("r_id",(mycp::bigint)0);

	if (nResultValue == EB_STATE_OK)
	{
		if (sParentResId>0)
		{
			if (!theResourceList.exist(sParentResId))
			{
				//theTempResourceList.insert(sParentResId,pResourceInfo);
				return;
			}
		}

		CEBResourceInfo::pointer pResourceInfo;
		if (!theResourceList.find(sResId,pResourceInfo))
		{
			pResourceInfo = CEBResourceInfo::create();
			theResourceList.insert(sResId,pResourceInfo);
		}
		if (bMove)
		{
			pResourceInfo->m_sParentResId = sNewParentResId;
			if (this->m_callback)
				m_callback->onResourceMove(pResourceInfo.get(),sOldParentResId);
			if (m_pHwnd!=NULL) {
				char lpszBuff[24];
				sprintf(lpszBuff,"%lld",sOldParentResId);
#ifdef _QT_MAKE_
				EB_ResourceInfo * pEvent = new EB_ResourceInfo(pResourceInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_RESOURCE_MOVE);
				pEvent->SetEventString(lpszBuff);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_RESOURCE_MOVE, (WPARAM)(const EB_ResourceInfo*)pResourceInfo.get(), (LPARAM)lpszBuff);
#endif
			}
			return;
		}else
		{
			pResourceInfo->m_sName = sResName;
			pResourceInfo->m_sDescription = sDescription;
			if (!bEdit)
			{
				pResourceInfo->m_sResId = sResId;
				pResourceInfo->m_sParentResId = sParentResId;
				pResourceInfo->m_sName = sResName;
				pResourceInfo->m_nResType = (EB_RESOURCE_TYPE)nResType;
				pResourceInfo->m_nShare = (EB_RESOURCE_SHARE_TYPE)nShare;
				pResourceInfo->m_sDescription = sDescription;
				pResourceInfo->m_nFromId = nFromId;
				pResourceInfo->m_nFromType = nFromType;
				//pResourceInfo->m_sEnterpriseCode = sEntCode;
				//pResourceInfo->m_sGroupCode = sDepCode;
				pResourceInfo->m_sCmServer = sResCmServer;
				//pResourceInfo->m_sCmAppName = sResCmAppName;
				pResourceInfo->m_nCreateUserId = m_pMyAccountInfo->GetUserId();
				time_t tNow = time(0);
				struct tm *nowtime = localtime(&tNow);
				char lpszDateDir[32];
				sprintf(lpszDateDir,"%04d-%02d-%02d %02d:%02d:%02d",nowtime->tm_year+1900,nowtime->tm_mon+1,nowtime->tm_mday,nowtime->tm_hour,nowtime->tm_min,nowtime->tm_sec);
				pResourceInfo->m_sTime = lpszDateDir;
			}
		}

		// 请求上传资源
		const tstring sKey = pResourceInfo->m_sCmServer;
		if (bEdit || sKey.empty())
		{
			if (this->m_callback)
				m_callback->onResourceInfo(pResourceInfo.get());
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
				EB_ResourceInfo * pEvent = new EB_ResourceInfo(pResourceInfo.get());
				pEvent->SetQEventType((QEvent::Type)EB_WM_RESOURCE_INFO);
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::SendMessage(m_pHwnd, EB_WM_RESOURCE_INFO, (WPARAM)(const EB_ResourceInfo*)pResourceInfo.get(), 0);
#endif
			}
		}else
		{
			const tstring sCmServerKey = GetCmServerKey(pResourceInfo->m_sCmServer);
			Cchatroom::pointer pChatRoom;
			if (!theChatRoomList.find(sCmServerKey,pChatRoom))
			{
				pChatRoom = Cchatroom::create();
				pChatRoom->SetSslKey(m_pRsa.GetPublicKey().c_str(),m_pRsa.GetPrivateKey().c_str(),m_pRsa.GetPrivatePwd().c_str());
				pChatRoom->Start(pResourceInfo->m_sCmServer.c_str(), pResourceInfo->m_sCmAppName.c_str(),0,true,1);
				theChatRoomList.insert(sCmServerKey,pChatRoom);
			}
			theResourceChatRoom.insert(sResId,pChatRoom);
			theResourceFilePath.insert(sResId,CFilePathInfo::create(CFilePathInfo::FILE_PAHT_RESOURCE,sFilePath,false));
			pChatRoom->SetCallBack(this);
			pChatRoom->EnterRoom(m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetUserId(),m_pMyAccountInfo->GetACMKey("").c_str(),m_pMyAccountInfo->GetLogonType());
#ifdef _QT_MAKE_
            pChatRoom->SendResource(sResId,sFilePath,true);
#else
			pChatRoom->SendResource(sResId,sFilePath.c_str(),true);
#endif
		}
	}
}
void CUserManagerApp::ProcessSubResourceInfo(mycp::bigint sResId)
{
	std::vector<CEBResourceInfo::pointer> pSubInfos; 
	theTempResourceList.find(sResId, pSubInfos);
	for (size_t i=0; i<pSubInfos.size(); i++)
	{
        /// 找到下一级部门，需要处理
		const CEBResourceInfo::pointer pResourceInfo = pSubInfos[i];
		theResourceList.insert(pResourceInfo->m_sResId,pResourceInfo);
		if (this->m_callback)
			m_callback->onResourceInfo(pResourceInfo.get());
		if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
			EB_ResourceInfo * pEvent = new EB_ResourceInfo(pResourceInfo.get());
			pEvent->SetQEventType((QEvent::Type)EB_WM_RESOURCE_INFO);
            QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
			::SendMessage(m_pHwnd, EB_WM_RESOURCE_INFO, (WPARAM)(const EB_ResourceInfo*)pResourceInfo.get(), 0);
#endif
		}
		if (pResourceInfo->m_nResType == EB_RESOURCE_DIR)
			ProcessSubResourceInfo(pResourceInfo->m_sResId);
	}
	theTempResourceList.remove(sResId);
}
void CUserManagerApp::OnResInfo(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pSotpResponseInfo,const CPOPCUserManager* pUMOwner)
{
	const mycp::bigint nDownloadResourceId = pReqeustInfo.get()==NULL?0:pReqeustInfo->m_pRequestList.getParameterValue("download_resource_id",(mycp::bigint)0);
    const EBFileString sDownloadSaveTo = pReqeustInfo.get()==NULL?"":pReqeustInfo->m_pRequestList.getParameterValue("download_save_to").c_str();

	const int sResCount = pSotpResponseInfo->m_pResponseList.getParameterValue("r-count",(int)-1);
//	const int nResOffset = pSotpResponseInfo->m_pResponseList.getParameterValue("r-offset",(int)-1);
	const mycp::bigint sResId = pSotpResponseInfo->m_pResponseList.getParameterValue("r_id",(mycp::bigint)0);
	const mycp::bigint sParentResId = pSotpResponseInfo->m_pResponseList.getParameterValue("pr_id",(mycp::bigint)0);
	const tstring sResName(pSotpResponseInfo->m_pResponseList.getParameterValue("name"));
	const int nResType = pSotpResponseInfo->m_pResponseList.getParameterValue("type",(int)EB_RESOURCE_UNKNOWN);
	const int nShare = pSotpResponseInfo->m_pResponseList.getParameterValue("share",(int)EB_RESOURCE_SHARE_UNKNOWN);
	const tstring sDescription(pSotpResponseInfo->m_pResponseList.getParameterValue("description"));
	const int nCmServiceId = pSotpResponseInfo->m_pResponseList.getParameterValue("cm-service-id",(int)0);
	const tstring sResCmServer(pSotpResponseInfo->m_pResponseList.getParameterValue("cm-server"));
	//const tstring sResHttpCmServer(pSotpResponseInfo->m_pResponseList.getParameterValue("cm-http-server"));
	//const tstring sResCmAppName = pSotpResponseInfo->m_pResponseList.getParameterValue("cm-appname");
	//const mycp::bigint nShareId = pSotpResponseInfo->m_pResponseList.getParameterValue("share-id",(mycp::bigint)0);
	const mycp::bigint nFromId = pSotpResponseInfo->m_pResponseList.getParameterValue("from-id",(mycp::bigint)0);
	const EB_RESOURCE_FROM_TYPE nFromType = (EB_RESOURCE_FROM_TYPE)pSotpResponseInfo->m_pResponseList.getParameterValue("from-type",(int)EB_RESOURCE_FROM_TYPE_UNKNOWN);
	// ??? 兼容旧版本 1.19
	const mycp::bigint sEntCode = pSotpResponseInfo->m_pResponseList.getParameterValue("ent-code",(mycp::bigint)0);
	const mycp::bigint sDepCode = pSotpResponseInfo->m_pResponseList.getParameterValue("dep-code",(mycp::bigint)0);
	const mycp::bigint nCreateUserId = pSotpResponseInfo->m_pResponseList.getParameterValue("create_uid",(mycp::bigint)0);
	//const tstring sCreateUserName(pSotpResponseInfo->m_pResponseList.getParameterValue("create_username"));
	//const tstring sCreator = pSotpResponseInfo->m_pResponseList.getParameterValue("creator");
	const tstring sTime(pSotpResponseInfo->m_pResponseList.getParameterValue("time"));
	const tstring sDeleteTime(pSotpResponseInfo->m_pResponseList.getParameterValue("delete_time"));
	const int nDownloads = pSotpResponseInfo->m_pResponseList.getParameterValue("dlc",(int)0);
	const mycp::bigint nSize = pSotpResponseInfo->m_pResponseList.getParameterValue("size",(mycp::bigint)0);
	const tstring sOnlineViewUrl(pSotpResponseInfo->m_pResponseList.getParameterValue("online-view-url"));

	if (sResId==0 || sResCount>=0)
	{
		if (nDownloadResourceId>0 && pSotpResponseInfo->GetResultValue()!=EB_STATE_OK)
		{
			if (m_callback)
				m_callback->onStateCode((EB_STATE_CODE)pSotpResponseInfo->GetResultValue(),0);
			if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
                EB_Event * pEvent = new EB_Event((QEvent::Type)EB_WM_STATE_CODE);
//				pEvent->SetQEventType((QEvent::Type)EB_WM_STATE_CODE);
				pEvent->SetEventParameter((long)pSotpResponseInfo->GetResultValue());
                QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
				::PostMessage(m_pHwnd, EB_WM_STATE_CODE, (WPARAM)pSotpResponseInfo->GetResultValue(), 0);
#endif
			}
		}
		return;
	}

	CEBResourceInfo::pointer pResourceInfo;
	if (!theResourceList.find(sResId,pResourceInfo))
	{
		pResourceInfo = CEBResourceInfo::create();
		pResourceInfo->m_sResId = sResId;
		theResourceList.insert(sResId,pResourceInfo);
	}else if ((pResourceInfo->m_nItemData & EB_RESOURCE_DATA_REFRESH)==EB_RESOURCE_DATA_REFRESH)
	{
		// 去掉刷新标识
		pResourceInfo->m_nItemData &= ~EB_RESOURCE_DATA_REFRESH;
		if (m_pProcessCheckResourceRefresh.get()!=NULL)
		{
			m_pProcessCheckResourceRefresh->m_tProcessTime = time(0)+3;	// 0123，等待4秒
		}
	}

	pResourceInfo->m_sParentResId = sParentResId;
	pResourceInfo->m_sName = sResName;
	pResourceInfo->m_nResType = (EB_RESOURCE_TYPE)nResType;
	pResourceInfo->m_nShare = (EB_RESOURCE_SHARE_TYPE)nShare;
	pResourceInfo->m_sDescription = sDescription;
	if (sDepCode>0)	// ??? 兼容旧版本 1.19
	{
		pResourceInfo->m_nFromId = sDepCode;
		pResourceInfo->m_nFromType = EB_RESOURCE_FROM_TYPE_GROUP;
	}else if (sEntCode>0)	// ??? 兼容旧版本 1.19
	{
		pResourceInfo->m_nFromId = sEntCode;
		pResourceInfo->m_nFromType = EB_RESOURCE_FROM_TYPE_ENT;
	}else if (nFromId==0)
	{
		pResourceInfo->m_nFromType = EB_RESOURCE_FROM_TYPE_USER;
	}else
	{
		pResourceInfo->m_nFromId = nFromId;
		pResourceInfo->m_nFromType = nFromType;
	}
	//pResourceInfo->m_sEnterpriseCode = sEntCode;
	//pResourceInfo->m_sGroupCode = sDepCode;
	if (!sResCmServer.empty())
		pResourceInfo->m_sCmServer = sResCmServer;
	//LogMessage("OnResInfo sResId=%lld,sResCmServer=%s\r\n",sResId,sResCmServer.c_str());

	//pResourceInfo->m_sCmHttpServer = sResHttpCmServer;
	//pResourceInfo->m_sCmAppName = sResCmAppName;
	pResourceInfo->m_nCreateUserId = nCreateUserId;
	//pResourceInfo->m_sCreator = sCreator;
	pResourceInfo->m_sTime = sTime;
	pResourceInfo->m_sDeleteTime = sDeleteTime;
	pResourceInfo->m_nDownloads = nDownloads;
	pResourceInfo->m_nSize = nSize;
	pResourceInfo->m_nCmServiceId = nCmServiceId;
	if (!sOnlineViewUrl.empty())
		pResourceInfo->m_sOnlineViewUrl = sOnlineViewUrl;

	if (sParentResId>0)
	{
		if (!theResourceList.exist(sParentResId))
		{
			theTempResourceList.insert(sParentResId,pResourceInfo);
			return;
		}
	}
	if (this->m_callback)
		m_callback->onResourceInfo(pResourceInfo.get());
	if (m_pHwnd!=NULL) {
#ifdef _QT_MAKE_
		EB_ResourceInfo * pEvent = new EB_ResourceInfo(pResourceInfo.get());
		pEvent->SetQEventType((QEvent::Type)EB_WM_RESOURCE_INFO);
        QCoreApplication::postEvent( m_pHwnd, pEvent,thePostEventPriority );
#else
		::SendMessage(m_pHwnd, EB_WM_RESOURCE_INFO, (WPARAM)(const EB_ResourceInfo*)pResourceInfo.get(), 0);
#endif
	}
    /// 处理前面未处理，下一级资源数据
    if (pResourceInfo->m_nResType == EB_RESOURCE_DIR) {
		ProcessSubResourceInfo(sResId);
    }

#ifdef _QT_MAKE_
    if (nDownloadResourceId==sResId && !sDownloadSaveTo.isEmpty() && pSotpResponseInfo->GetResultValue()==EB_STATE_OK) {
        DownloadFileRes(nDownloadResourceId,sDownloadSaveTo);
    }
#else
	if (nDownloadResourceId==sResId && !sDownloadSaveTo.empty() && pSotpResponseInfo->GetResultValue()==EB_STATE_OK)
	{
		DownloadFileRes(nDownloadResourceId,sDownloadSaveTo);
	}
#endif
}

//#include <boost/shared_ptr.hpp>
//class EbcCallInfo
//{
//public:
//    typedef boost::shared_ptr<EbcCallInfo> pointer;
//    static EbcCallInfo::pointer create(void) {
//        return EbcCallInfo::pointer(new EbcCallInfo());
//    }

//    EB_CallInfo m_pCallInfo;
//    EB_AccountInfo m_pFromAccountInfo;
//    bool m_bOffLineUser;
//    time_t m_tLastTime;

//    EbcCallInfo(void)
//    {
//        m_tLastTime = time(0);
//    }
//};
//const EbcCallInfo::pointer EbcCallInfoNull;

} // namespace entboost

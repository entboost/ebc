// ebrd.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "../include/EBRtpManager.h"
#define EBRD_EXPORTS
#include "ebrd.h"
#include "rdpinfo.h"
#include "RDWnd.h"
#include "ScrollStatic.h"
#include "AVCodecProxy.h"
#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace entboost {

#define RDS_APP_NAME EB_APP_NAME_RD_SERVER

class CP2PTaskInfo
{
public:
	typedef boost::shared_ptr<CP2PTaskInfo> pointer;
	static CP2PTaskInfo::pointer create(void)
	{
		return CP2PTaskInfo::pointer(new CP2PTaskInfo());
	}
	mycp::bigint m_nCallId;
	mycp::bigint m_nFromUserId;
	tstring m_sFromLocalIp;
	tstring m_sFromRemoteIp;
	//CCrP2PInfo m_pP2PInfo;
	time_t m_tP2PTry;
	bool m_bP2PResponse;
	CP2PTaskInfo(void)
		: m_nCallId(0), m_nFromUserId(0), m_tP2PTry(0),m_bP2PResponse(false)
	{}
};

//#define USES_FILE_LOG1

//#pragma comment(lib,"Ws2_32.lib")
//#include <Iphlpapi.h>
//#pragma comment(lib, "Iphlpapi.lib")
//using namespace std;
typedef struct tagNetworkCfg 
{
	char szIP[18];
	char szNetmask[18];
	char szGateway[18];
	//char szDns1[18];
	//char szDns2[18];
}NetworkCfg;
bool GetNetworkCfg(std::vector<NetworkCfg>& pOutNetworkCfg,bool bGetEmptyGateway)
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
int GetLocalHostIp(std::vector<std::string> & pOutIps)
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

////CPOPCLogonCenter::pointer theDeletePOPCLogonCenter;		// 无用临时数据
//int ParseString(const char * lpszString, const char * lpszInterval, std::vector<std::string> & pOut)
//{
//	std::string sIn(lpszString);
//	pOut.clear();
//	while (!sIn.empty())
//	{
//		std::string::size_type find = sIn.find(lpszInterval);
//		if (find == std::string::npos)
//		{
//			pOut.push_back(sIn);
//			break;
//		}
//		if (find==0)
//			pOut.push_back("");	// 空
//		else
//			pOut.push_back(sIn.substr(0, find));
//		sIn = sIn.substr(find+strlen(lpszInterval));
//	}
//	return (int)pOut.size();
//}
//bool SplitString(const std::string& sSource, const char* sStart, const char* sEnd, std::string & sOut)
//{
//	sOut.empty();
//	std::string::size_type find1 = sSource.find(sStart);
//	if (find1 == std::string::npos) return false;
//	std::string::size_type find2 = sSource.find(sEnd, find1+strlen(sStart));
//	if (find2 == std::string::npos) return false;
//	sOut = sSource.substr(find1+strlen(sStart),find2-find1-strlen(sStart));
//	return true;
//}
//mycp::bigint SplitBigIntValue(const std::string& sSource, const char* sStart, const char* sEnd, mycp::bigint nDefault)
//{
//	mycp::bigint nResult(nDefault);
//	std::string sOut;
//	if (SplitString(sSource,sStart,sEnd,sOut))
//	{
//		nResult = cgc_atoi64(sOut.c_str());
//	}
//	return nResult;
//}
//int SplitIntValue(const std::string& sSource, const char* sStart, const char* sEnd, int nDefault)
//{
//	int nResult(nDefault);
//	std::string sOut;
//	if (SplitString(sSource,sStart,sEnd,sOut))
//	{
//		nResult = atoi(sOut.c_str());
//	}
//	return nResult;
//}
//
class CRDSinkWnd
{
public:
	typedef boost::shared_ptr<CRDSinkWnd> pointer;
	static CRDSinkWnd::pointer create(mycp::bigint nDestId)
	{
		return CRDSinkWnd::pointer(new CRDSinkWnd(nDestId));
	}
	bool m_bLocalP2POk;
	bool m_bRemoteP2POk;
	CEBRtpManager::pointer m_pRDManager1;	// 内网
	CEBRtpManager::pointer m_pRDManager2;	// 公网
	void SetLocalAddress(const std::string & value) {m_sLocalAddress = value;}
	const tstring & GetLocalAddress(void) const {return m_sLocalAddress;}
	void SetRemoteAddress(const std::string & value) {m_sRemoteAddress = value;}
	const tstring & GetRemoteAddress(void) const {return m_sRemoteAddress;}

	mycp::bigint GetSinkDestId(void) const {return m_nDestId;}
	bool CreateWnd(HWND hWndParent)
	{
		return m_rdWnd.CreateWnd(CWnd::FromHandle(hWndParent));
	}
	bool UpdateScreenInfo(int nWidth, int nHeight, int nBitCount)
	{
		if (!m_videoCodec.isopenav())
		{
#ifdef USES_FFMPEG_0_8
			FVInfo fvinfo(PIX_FMT_BGRA, nBitCount, nWidth, nHeight);		// OK
#else
			FVInfo fvinfo(AV_PIX_FMT_BGRA, nBitCount, nWidth, nHeight);		// OK
#endif
			//FVInfo fvinfo(PIX_FMT_BGR24, nBitCount, nVideoWidth, nVideoHeight);		// OK
			//FVInfo fvinfo(PIX_FMT_BGR24, 24, nVideoWidth, nVideoHeight);			// 不行
			m_videoCodec.setdec_info(fvinfo);
#ifdef USES_FFMPEG_0_8
			if (!m_videoCodec.openav(CODEC_ID_MPEG4))
#else
			if (!m_videoCodec.openav(AV_CODEC_ID_MPEG4))
#endif
			{
				m_videoCodec.closeav();
				return false;
			}
			if (m_rdWnd.m_bitmapManager.IsInvalid())
			{
				m_rdWnd.m_bitmapManager.Create(nWidth, nHeight, nBitCount);
			}
			m_rdWnd.MoveParentFullSize();
		}
		return true;
	}
	void OnRtpFrame(int nWidth, int nHeight, int nBitCount, const CSotpRtpFrame::pointer& pRtpFrame)
	{
		if (!UpdateScreenInfo(nWidth, nHeight, nBitCount))
		{
			return;
		}
		const unsigned int nDecDesktopFrameSize = ((int)nWidth)*((int)nHeight)*(nBitCount/4);
		if (m_pDecDataBuffer==NULL)
		{
			m_nDecBufferSize = nDecDesktopFrameSize;
			m_pDecDataBuffer = new unsigned char[m_nDecBufferSize];
		}else if (m_nDecBufferSize < nDecDesktopFrameSize)
		{
			delete[] m_pDecDataBuffer;
			m_nDecBufferSize = nDecDesktopFrameSize;
			m_pDecDataBuffer = new unsigned char[m_nDecBufferSize];
		}

		const char* pDesktopFrameData = pRtpFrame->m_pPayload+RDPSTRUCT_SIZE;
		const int nDesktopFrameSize = pRtpFrame->m_pRtpHead.m_nTotleLength-RDPSTRUCT_SIZE;
		// test
		int decode_size = 0;
		if (m_videoCodec.decode((const unsigned char*)pDesktopFrameData,nDesktopFrameSize, m_pDecDataBuffer, decode_size, false)!=NULL)
		{
			//m_nErrorCount = 0;
			DrawScreen(m_pDecDataBuffer);
		//}else if ((++m_nErrorCount)>10)
		//{
		//	AfxMessageBox(_T("m_videoCodec.decode error!"));
		//	m_nErrorCount = 0;
		//	m_videoCodec.closeav();
		}
	}
	void DrawScreen(const void* pDibBits)
	{
		m_rdWnd.m_bitmapManager.MakeBitmap(pDibBits);
		m_rdWnd.drawScreen();
	}

	CScrollStatic m_rdWnd;
	//CRDWnd m_rdWnd;
	CRDSinkWnd(mycp::bigint nDestId)
		: m_bLocalP2POk(false), m_bRemoteP2POk(false)
		, m_nDestId(nDestId)
		, m_pDecDataBuffer(NULL), m_nDecBufferSize(0)
		, m_nErrorCount(0)
	{}
	virtual ~CRDSinkWnd(void)
	{
		m_pRDManager1.reset();
		m_pRDManager2.reset();

		m_videoCodec.closeav();
		m_rdWnd.DestroyWindow();

		if (m_pDecDataBuffer!=NULL)
		{
			delete[] m_pDecDataBuffer;
			m_pDecDataBuffer = NULL;
		}
	}
protected:
	mycp::bigint m_nDestId;
	CAVCodecProxy m_videoCodec;

	unsigned char* m_pDecDataBuffer;
	unsigned int m_nDecBufferSize;
	int m_nErrorCount;

	tstring m_sLocalAddress;
	tstring m_sRemoteAddress;

};
const CRDSinkWnd::pointer NullRDSinkWnd;

class CRDRoomWndList
{
public:
	typedef boost::shared_ptr<CRDRoomWndList> pointer;
	static CRDRoomWndList::pointer create(void)
	{
		return CRDRoomWndList::pointer(new CRDRoomWndList());
	}
	CRDSinkWnd::pointer AddSinkWnd(mycp::bigint nDestId, HWND hWndParent)
	{
		CRDSinkWnd::pointer pSinkWnd;
		if (!m_pSinkWndList.find(nDestId,pSinkWnd))
		{
			pSinkWnd = CRDSinkWnd::create(nDestId);
			if (hWndParent!=NULL)
			{
				if (!pSinkWnd->CreateWnd(hWndParent))
					return NullRDSinkWnd;
			}
			m_pSinkWndList.insert(nDestId,pSinkWnd);
		}
		return pSinkWnd;
	}
	void DelSinkWnd(mycp::bigint nDestId)
	{
		m_pSinkWndList.remove(nDestId);
	}
	void DelAllSinkWnd(void)
	{
		m_pSinkWndList.clear();
	}
	CRDSinkWnd::pointer GetSinkWnd(mycp::bigint nDestId) const
	{
		CRDSinkWnd::pointer pSinkWnd;
		m_pSinkWndList.find(nDestId,pSinkWnd);
		return pSinkWnd;
	}
	bool MoveSinkWindow(mycp::bigint nDestId, int x, int y, int width, int height, BOOL bRepaint)
	{
		CRDSinkWnd::pointer pSinkWnd;
		if (!m_pSinkWndList.find(nDestId,pSinkWnd))
		{
			return false;
		}
		pSinkWnd->m_rdWnd.MoveWindow(x,y,width,height,bRepaint);
		return true;
	}
	bool EnableControl(mycp::bigint nDestId, int nMouseFPS)
	{
		CRDSinkWnd::pointer pSinkWnd;
		if (!m_pSinkWndList.find(nDestId,pSinkWnd))
		{
			return false;
		}
		return pSinkWnd->m_rdWnd.EnableControl(nMouseFPS);
	}
	int EnableControl(mycp::bigint nDestId) const
	{
		CRDSinkWnd::pointer pSinkWnd;
		if (!m_pSinkWndList.find(nDestId,pSinkWnd))
		{
			return 0;
		}
		return pSinkWnd->m_rdWnd.EnableControl();
	}
	CLockMap<mycp::bigint,CRDSinkWnd::pointer> m_pSinkWndList;
	CRDRoomWndList(void)
	{}
	virtual ~CRDRoomWndList(void)
	{
	}
protected:
};

class CRDManager : public CEBRtpHangler
	, public CRDEventHandler
	, public CRemoteEventHandler
{
public:
	CEBRtpManager::pointer m_pRDManager;
	CEBRDCallBack* m_pCallback;
	HWND m_pHwnd;

	//void SetDefaultQuant(int v) {m_nDefaultQuant = v;}
	//int GetDefaultQuant(void) const {return m_nDefaultQuant;}
	//void SetDefaultFps(int v) {m_nDefaultFps = v;}
	//int GetDefaultFps(void) const {return m_nDefaultFps;}

	void SetRoomId(mycp::bigint nRoomId, bool bIsGroup) {m_nRoomId = nRoomId; m_bIsGroup = bIsGroup;}
	mycp::bigint GetRoomId(void) const {return m_nRoomId;}
	bool IsGroup(void) const {return m_bIsGroup;}
	void SetParam(mycp::bigint nParam) {m_nParam = nParam;}
	mycp::bigint GetParam(void) const {return m_nParam;}

	bool StartLocalDesktop(int quant, int fps)
	{
		m_nDefaultQuant = quant;
		m_nDefaultFps = fps;
		if (m_pRDManagerP2POk.get()!=NULL)
		{
			fps += 1;
			if (m_nLocalP2PFlag)
				quant -= 3;
			else
				quant -= 1;
		}
		return m_rdLocalDesktopWnd.StartLocalDesktop(quant, fps, this);
	}
	void UnRegister(void)
	{
		const mycp::bigint nRoomId = GetRoomId();
		const mycp::bigint nFromUserId = m_pRDManager->GetSourceId();
		m_pRDManager->SendRDOff(nRoomId, nFromUserId, this->GetParam());
		m_pRDManager->UnRegisterSource(nRoomId);
		m_pRDManager->UnRegisterAllSource();
		m_pRDManagerP2POk.reset();
		m_pP2PTaskInfo.reset();
	}
	void DelAllSink(void)
	{
		m_pRoomWndList.DelAllSinkWnd();
		m_pRDManager->UnRegisterAllSink();
		m_pRDManagerP2POk.reset();
		m_pP2PTaskInfo.reset();
	}
	void DelSink(mycp::bigint nDestId)
	{
		const mycp::bigint nRoomId = GetRoomId();
		m_pRoomWndList.DelSinkWnd(nDestId);
		m_pRDManager->UnRegisterSink(nRoomId, nDestId);
		m_pRDManagerP2POk.reset();
		m_pP2PTaskInfo.reset();
	}
	bool AddSink(mycp::bigint nDestId, HWND hWndParent=NULL)
	{
		const mycp::bigint nRoomId = GetRoomId();
		const bool ret = m_pRDManager->RegisterSink(nRoomId, nDestId)==0?true:false;
		if (ret)
		{
			CRDSinkWnd::pointer pSinkWnd = m_pRoomWndList.AddSinkWnd( nDestId, hWndParent);
			if (pSinkWnd.get()==NULL) return false;
			if (hWndParent!=NULL)
				pSinkWnd->m_rdWnd.SetCallback((CRemoteEventHandler*)this, pSinkWnd.get());
			GetLocalHostInfo();
			if (!IsGroup() && !m_pLocalHostIp.empty())
			{
				if (pSinkWnd->m_pRDManager1.get()==NULL)
				{
					pSinkWnd->m_pRDManager1 = CEBRtpManager::create(this);
					pSinkWnd->m_pRDManager1->Start(m_pRDManager->GetAddress(),RDS_APP_NAME,0,0);
					pSinkWnd->m_pRDManager1->SetParameter(1);	// 这是P2P
				}

				if (!pSinkWnd->m_bLocalP2POk)	// **本地未打通
				{
					CP2PTaskInfo::pointer pP2PTaskInfo = m_pP2PTaskInfo;
					if (pP2PTaskInfo.get()==NULL)
					{
						pP2PTaskInfo = CP2PTaskInfo::create();
						m_pP2PTaskInfo = pP2PTaskInfo;
					}
					pP2PTaskInfo->m_nCallId = this->GetRoomId();
					pP2PTaskInfo->m_nFromUserId = nDestId;
					pP2PTaskInfo->m_tP2PTry = time(0);
					pP2PTaskInfo->m_bP2PResponse = false;
				}
				// **这里不用担心，对方 rd_on太迟，因为双方都会做这个动作；只要一方做请求成功就可以的；
				char lpszLocalIp[64];
				sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pSinkWnd->m_pRDManager1->GetLocalPort());
				pSinkWnd->m_pRDManager1->SetDestAddress(0,m_pRDManager->GetAddress().address());
				pSinkWnd->m_pRDManager1->SendP2PRequest(m_pRDManager->GetSourceId(),this->GetRoomId(),nDestId,lpszLocalIp,0);	// 0 正常P2P；1：离线重试；
			}
		}
		return ret;
	}
	CRDWnd m_rdLocalDesktopWnd;
	CRDRoomWndList m_pRoomWndList;
	CRDManager(void)
		: m_pCallback(NULL), m_pHwnd(NULL)
		, m_nRoomId(0), m_bIsGroup(false), m_nParam(0)
		, m_pSendDataBuffer(NULL), m_nSendBufferSize(0)
		//, m_pDecDataBuffer(NULL), m_nDecBufferSize(0)
#ifdef USES_FILE_LOG1
		, m_flog(NULL)
#endif
		, m_nLocalP2PFlag(false), m_nDefaultQuant(10), m_nDefaultFps(3)

	{
		m_pRDManager = CEBRtpManager::create(this,true);
		memset(&m_pDesktopData,0,RDPSTRUCT_SIZE);
	}
	virtual ~CRDManager(void)
	{
		m_pClearChatRoom.clear();
#ifdef USES_FILE_LOG1
		if (m_flog!=NULL)
		{
			fclose(m_flog);
			m_flog = 0;
		}
#endif
		if (m_pSendDataBuffer!=NULL)
		{
			delete[] m_pSendDataBuffer;
			m_pSendDataBuffer = NULL;
		}
		//if (m_pDecDataBuffer!=NULL)
		//{
		//	delete[] m_pDecDataBuffer;
		//	m_pDecDataBuffer = NULL;
		//}
	}
protected:
	void GetLocalHostInfo(void)
	{
		if (m_pLocalHostIp.empty())
		{
			std::vector<tagNetworkCfg> pNetworkCfg;
			GetNetworkCfg(pNetworkCfg,false);	// **有可能取到二个以上，其中有一个是不对的
			if (pNetworkCfg.size()>1)
			{
				// 做交叉比较，去掉没用的IP地址，取出真实IP地址；
				CLockMap<std::string,bool> pIpList;
				for (size_t i=0;i<pNetworkCfg.size();i++)
					pIpList.insert(pNetworkCfg[i].szIP,true);
				std::vector<std::string> pLocalHostIp;
				GetLocalHostIp(pLocalHostIp);
				for (size_t i=0;i<pLocalHostIp.size();i++)
				{
					const tstring sIp = pLocalHostIp[i];
					if (pIpList.exist(sIp))
					{
						m_pLocalHostIp.push_back(sIp);
						break;
					}
				}
				if (m_pLocalHostIp.empty())
					GetLocalHostIp(m_pLocalHostIp);
			}else if (pNetworkCfg.size()==1)
			{
				m_pLocalHostIp.push_back(pNetworkCfg[0].szIP);
			}else
			{
				GetLocalHostIp(m_pLocalHostIp);
			}
		}
	}
	//CXvidProxy m_xvid;
	//CAVCodecProxy m_videoCodec;

	// CRemoteEventHandler
	virtual void onSendRdpMessage(const rdpStruct& pRdpMessage, void* pUserData)
	{
		//CRDSinkWnd* pSinkWnd = (CRDSinkWnd*)pUserData;
		//if (pSinkWnd==0) return;
		if (m_pRDManager.get()==NULL) return;
		const mycp::uint32 nTimestamp = timeGetTime();
		try
		{
			if (m_pRDManagerP2POk.get()!=NULL)
				m_pRDManagerP2POk->SendRtpData(m_nRoomId,(const unsigned char*)&pRdpMessage,RDPSTRUCT_SIZE,nTimestamp,SOTP_RTP_NAK_DATA_MOUSE,SOTP_RTP_NAK_REQUEST_1);
			else if (m_pP2PTaskInfo.get()==NULL)	// m_pP2PTaskInfo.get()!=NULL：表示正在做P2P尝试；
				m_pRDManager->SendRtpData(m_nRoomId,(const unsigned char*)&pRdpMessage,RDPSTRUCT_SIZE,nTimestamp,SOTP_RTP_NAK_DATA_MOUSE,SOTP_RTP_NAK_REQUEST_1);
			//m_pRDManager->SendRtpData(m_nRoomId,(const unsigned char*)&pRdpMessage,RDPSTRUCT_SIZE,0,SOTP_RTP_NAK_DATA_MOUSE,SOTP_RTP_NAK_NONE);
		}catch(std::exception&)
		{}catch(...)
		{}
	}

	rdpStruct m_pDesktopData;
	//rdpDesktopInfo m_pDesktopInfo;
	// CRDEventHandler
	virtual bool onCanProcess(void) const
	{
		return m_pP2PTaskInfo.get()==NULL?true:false;
	}
	virtual void onLocalDesktopInfo(int nVideoWidth, int nVideoHeight, int nBitCount)
	{
		memset(&m_pDesktopData, 0, RDPSTRUCT_SIZE);
		m_pDesktopData.m_messageEvent = RDP_DESKTOP_DATA;
		m_pDesktopData.u.desktopInfo.m_nWidth = htons(nVideoWidth);
		m_pDesktopData.u.desktopInfo.m_nHeight = htons(nVideoHeight);
		m_pDesktopData.u.desktopInfo.m_nBitCount = (unsigned char)nBitCount;

		//XInfo xinfo(XVID_CSP_RGB, 24, nVideoWidth, nVideoHeight);	// 不行
		////XInfo xinfo(XVID_CSP_BGRA, 24, nVideoWidth, nVideoHeight);	// 不行
		////XInfo xinfo(XVID_CSP_BGRA, nBitCount, nVideoWidth, nVideoHeight);	// 不行
		//m_xvid.setdec_info(xinfo);

		//m_videoCodec.closeav();
		//FVInfo fvinfo(PIX_FMT_BGRA, nBitCount, nVideoWidth, nVideoHeight);		// OK
		////FVInfo fvinfo(PIX_FMT_BGR24, nBitCount, nVideoWidth, nVideoHeight);		// OK
		////FVInfo fvinfo(PIX_FMT_BGR24, 24, nVideoWidth, nVideoHeight);			// 不行
		//m_videoCodec.setdec_info(fvinfo);
		//if (m_videoCodec.openav(CODEC_ID_MPEG4))
		//{
		//	//m_requestDestIp = receiveData->destip();
		//	//m_reqeustDestPort = receiveData->destport();

		//	//DWORD imageSize = m_eventHandler->onRDP_Request(nVideoWidth, nVideoHeight, nBitCount);
		//	//m_bufferManager.GetBuffer(imageSize);
		//	//m_bufferManager.ResetDataSize();
		//}

	}

	////创建一个bmp文件。用于调试
	//static int av_create_bmp(const char* filename, const unsigned char *pRGBBuffer, int width, int height, int bpp)
	//{
	//	BITMAPFILEHEADER bmpheader;
	//	BITMAPINFO bmpinfo;
	//	FILE *fp;

	//	fp = fopen(filename,"wb");
	//	if(!fp)return -1;

	//	bmpheader.bfType = ('M'<<8)|'B';
	//	bmpheader.bfReserved1 = 0;
	//	bmpheader.bfReserved2 = 0;
	//	bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	//	bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp/8;

	//	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//	bmpinfo.bmiHeader.biWidth = width;
	//	bmpinfo.bmiHeader.biHeight = height;
	//	bmpinfo.bmiHeader.biPlanes = 1;
	//	bmpinfo.bmiHeader.biBitCount = bpp;
	//	bmpinfo.bmiHeader.biCompression = BI_RGB;
	//	bmpinfo.bmiHeader.biSizeImage = 0;
	//	bmpinfo.bmiHeader.biXPelsPerMeter = 100;
	//	bmpinfo.bmiHeader.biYPelsPerMeter = 100;
	//	bmpinfo.bmiHeader.biClrUsed = 0;
	//	bmpinfo.bmiHeader.biClrImportant = 0;

	//	fwrite(&bmpheader,sizeof(BITMAPFILEHEADER),1,fp);
	//	fwrite(&bmpinfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
	//	fwrite(pRGBBuffer,width*height*bpp/8,1,fp);
	//	fclose(fp);

	//	return 0;
	//}

	unsigned char* m_pSendDataBuffer;
	unsigned int m_nSendBufferSize;
	virtual void onLocalDesktopData(const unsigned char* pData, unsigned int nSize, bool bKeyFrame, unsigned int nTimestamp)
	{
		if (m_pRDManager.get()==NULL) return;
		if (m_pSendDataBuffer==NULL)
		{
			m_nSendBufferSize = RDPSTRUCT_SIZE+nSize;
			m_pSendDataBuffer = new unsigned char[m_nSendBufferSize];
		}else if (m_nSendBufferSize<RDPSTRUCT_SIZE+nSize)
		{
			delete[] m_pSendDataBuffer;
			m_nSendBufferSize = RDPSTRUCT_SIZE+nSize;
			m_pSendDataBuffer = new unsigned char[m_nSendBufferSize];
		}
		memcpy(m_pSendDataBuffer,&m_pDesktopData,RDPSTRUCT_SIZE);
		memcpy(m_pSendDataBuffer+RDPSTRUCT_SIZE,pData,nSize);
		const unsigned int nSendSize = RDPSTRUCT_SIZE+nSize;
		const mycp::uint8 nDataType = bKeyFrame?SOTP_RTP_NAK_DATA_VIDEO_I:SOTP_RTP_NAK_DATA_VIDEO;
		try
		{
			if (m_pRDManagerP2POk.get()!=NULL)
				m_pRDManagerP2POk->SendRtpData(m_nRoomId,m_pSendDataBuffer,nSendSize,nTimestamp,nDataType,SOTP_RTP_NAK_REQUEST_1);
			else if (m_pP2PTaskInfo.get()==NULL)	// m_pP2PTaskInfo.get()!=NULL：表示正在做P2P尝试；
				m_pRDManager->SendRtpData(m_nRoomId,m_pSendDataBuffer,nSendSize,nTimestamp,nDataType,SOTP_RTP_NAK_REQUEST_1);
		}catch(std::exception&)
		{}catch(...)
		{}

		//m_pRDManager->SendRtpData(m_nRoomId,m_pSendDataBuffer,nSendSize,nTimestamp,SOTP_RTP_NAK_DATA_SCREEN,SOTP_RTP_NAK_REQUEST_2);

		//unsigned char * decode_buffer;
		//int decode_size = 0;
		//if (m_videoCodec.decode((const unsigned char*)pData,nSize, &decode_buffer, decode_size, false))
		//{
		//	av_create_bmp("D:/quant-4.bmp",(unsigned char *)decode_buffer,m_nWidth,m_nHeight,m_nBitCount);
		//	delete[] decode_buffer;
		//}

		//unsigned char * decode_buffer;
		//int decode_size = 0;
		//if (m_xvid.decode(pData, nSize, &decode_buffer, decode_size))
		//{
		//	av_create_bmp("D:/2.bmp",(unsigned char *)decode_buffer,m_nWidth,m_nHeight,24);
		//	delete[] decode_buffer;
		//}
	}
#ifdef USES_FILE_LOG1
	FILE * m_flog;
#endif

	//unsigned char* m_pDecDataBuffer;
	//unsigned int m_nDecBufferSize;
	virtual void OnRtpFrame(mycp::bigint nSrcId, const CSotpRtpFrame::pointer& pRtpFrame, mycp::uint16 nLostCount, mycp::uint32 nUserData, CEBRtpManager* pLCOwner)
	{
		if (pRtpFrame->m_pRtpHead.m_nRoomId!=this->GetRoomId()) return;
		if (pRtpFrame->m_pRtpHead.m_nTotleLength<RDPSTRUCT_SIZE) return;

#ifdef USES_FILE_LOG1
		char lpszBuffer[260];
		sprintf(lpszBuffer,"f:\\room-%lld-kfi30-screen-req2.txt",this->GetRoomId());
		if (m_flog==NULL)
			m_flog = fopen(lpszBuffer,"w+");

		if (m_flog!=NULL)
		{
			sprintf(lpszBuffer,"ts=%d:packet=%d,seq=%d,lostcount=%d\n",pRtpFrame->m_pRtpHead.m_nTimestamp,pRtpFrame->m_nPacketNumber,pRtpFrame->m_pRtpHead.m_nSeq,nLostCount);
			fwrite(lpszBuffer,1,strlen(lpszBuffer),m_flog);
		}
#endif

		rdpStruct rdp;
		memcpy(&rdp, pRtpFrame->m_pPayload, RDPSTRUCT_SIZE);
		switch (rdp.m_messageEvent)
		{
		case RDP_DESKTOP_DATA:
			{
				CRDSinkWnd::pointer pSinkWnd = m_pRoomWndList.GetSinkWnd(nSrcId);
				if (pSinkWnd.get()==NULL) return;

				const unsigned short nWidth = ntohs(rdp.u.desktopInfo.m_nWidth);
				const unsigned short nHeight = ntohs(rdp.u.desktopInfo.m_nHeight);
				const unsigned char nBitCount = rdp.u.desktopInfo.m_nBitCount;
				pSinkWnd->OnRtpFrame(nWidth,nHeight,nBitCount,pRtpFrame);
				break;
				//pSinkWnd->UpdateScreenInfo(nWidth, nHeight, nBitCount);
				//if (!m_videoCodec.isopenav())
				//{
				//	FVInfo fvinfo(PIX_FMT_BGRA, nBitCount, nWidth, nHeight);		// OK
				//	//FVInfo fvinfo(PIX_FMT_BGR24, nBitCount, nVideoWidth, nVideoHeight);		// OK
				//	//FVInfo fvinfo(PIX_FMT_BGR24, 24, nVideoWidth, nVideoHeight);			// 不行
				//	m_videoCodec.setdec_info(fvinfo);
				//	if (!m_videoCodec.openav(CODEC_ID_MPEG4))
				//	{
				//		m_videoCodec.closeav();
				//		return;
				//	}
				//	pSinkWnd->UpdateScreenInfo(nWidth, nHeight, nBitCount);
				//	pSinkWnd->m_rdWnd.MoveParentFullSize();
				//}
				//const unsigned int nDecDesktopFrameSize = ((int)nWidth)*((int)nHeight)*(nBitCount/4);
				//if (m_pDecDataBuffer==NULL)
				//{
				//	m_nDecBufferSize = nDecDesktopFrameSize;
				//	m_pDecDataBuffer = new unsigned char[m_nDecBufferSize];
				//}else if (m_nDecBufferSize < nDecDesktopFrameSize)
				//{
				//	delete[] m_pDecDataBuffer;
				//	m_nDecBufferSize = nDecDesktopFrameSize;
				//	m_pDecDataBuffer = new unsigned char[m_nDecBufferSize];
				//}

				//const char* pDesktopFrameData = pRtpFrame->m_pPayload+RDPSTRUCT_SIZE;
				//const int nDesktopFrameSize = pRtpFrame->m_pRtpHead.m_nTotleLength-RDPSTRUCT_SIZE;
				//// test
				//int decode_size = 0;
				//if (m_videoCodec.decode((const unsigned char*)pDesktopFrameData,nDesktopFrameSize, m_pDecDataBuffer, decode_size, false)!=NULL)
				//{
				//	pSinkWnd->DrawScreen(m_pDecDataBuffer);
				//	//char lpszBuffer[260];
				//	//sprintf(lpszBuffer,"D:\\12-ts-%d-seq-%d-lost-%d.bmp",pRtpFrame->m_pRtpHead.m_nTimestamp,pRtpFrame->m_pRtpHead.m_nSeq,nLostCount);
				//	//av_create_bmp(lpszBuffer,m_pDecDataBuffer,pDesktopInfo.m_nWidth, pDesktopInfo.m_nHeight, pDesktopInfo.m_nBitCount);
				//}

				//EBRD_RtpFrame pEBRtpFrame(nSrcId, pRtpFrame, nLostCount);
				//if (m_pHwnd!=NULL)
				//{
				//	SendMessage(m_pHwnd, EBCR_WM_RTP_FRAME, (WPARAM)&pEBRtpFrame, (LPARAM)nUserData);
				//}
				//if (m_pCallback!=NULL)
				//{
				//	m_pCallback->OnRtpFrame(pEBRtpFrame, nUserData);
				//}
			}break;
		case RDP_UNKNOWN:
			break;
		default:
			{
				m_rdLocalDesktopWnd.AddRdpMessage(nSrcId, rdp);
			}break;
		}
	}
	virtual void OnRDOnResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner)
	{
		const int nResultValue = pResponseInfo->GetResultValue();
		if (nResultValue==EB_STATE_OK)
		{
			m_pRDManager->RegisterSource(this->GetRoomId(),this->GetParam());
		}
	}

	bool DoCheckP2PState(const tstring& sLocalAddress, const tstring& sRemoteAddress,const CRDSinkWnd::pointer& pDestSink)
	{
		if (pDestSink->m_bLocalP2POk || pDestSink->m_bRemoteP2POk)
		{
			if (pDestSink->m_pRDManager1.get()!=NULL)
			{
				pDestSink->m_pRDManager1->SetDestAddress(1,pDestSink->GetLocalAddress());
				pDestSink->m_pRDManager1->SendP2PTest(3);
			}else if (pDestSink->m_pRDManager2.get()!=NULL)
			{
				pDestSink->m_pRDManager2->SetDestAddress(1,pDestSink->GetRemoteAddress());
				pDestSink->m_pRDManager2->SendP2PTest(3);
			}
			return true;
		}
		if (!sLocalAddress.empty())
			pDestSink->SetLocalAddress(sLocalAddress);		// 这个内网地址，可以打通P2P；connect&send也可以打通
		if (!sRemoteAddress.empty())
			pDestSink->SetRemoteAddress(sRemoteAddress);		// 这个外网地址，可以打通P2P；（大部分路由器支持）
		return false;
	}
	CLockList<CEBRtpManager::pointer> m_pClearChatRoom;
	bool DoP2PTask(mycp::bigint sCallId, mycp::bigint sChatId,mycp::bigint nFromUserId,const tstring& sLocalAddress, const tstring& sRemoteAddress,bool& pOutP2POk)
	{
		//const int nP2PData = pP2PInfo.m_nP2PData;
		CRDSinkWnd::pointer pSinkWnd = m_pRoomWndList.GetSinkWnd(nFromUserId);
		if (pSinkWnd.get()==NULL) return false;

		if (DoCheckP2PState(sLocalAddress,sRemoteAddress,pSinkWnd))
		{
			pOutP2POk = true;
			return false;	// 已经打通P2P
		}

		// LOCAL P2P
		if (!pSinkWnd->GetLocalAddress().empty() && pSinkWnd->m_pRDManager1.get()!=NULL)
		{
			CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRDManager1;	// 使用这个，不会导致OnP2POk清空异常
			pRDManager->SetDestAddress(1,pSinkWnd->GetLocalAddress());
			pRDManager->SendP2PTry(sCallId,sChatId,m_pRDManager->GetSourceId(),0,1);	// 1=内网地址
		}

		// REMOTE P2P
		if (!pSinkWnd->GetRemoteAddress().empty() && pSinkWnd->m_pRDManager2.get()!=NULL)
		{
			CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRDManager2;	// 使用这个，不会导致OnP2POk清空异常
			pRDManager->SetDestAddress(1,pSinkWnd->GetRemoteAddress());
			pRDManager->SendP2PTry(sCallId,sChatId,m_pRDManager->GetSourceId(),0,2);	// 2=NAT公网地址
		}
		return true;
	}
	virtual void OnP2POk(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner)
	{
		// 已经打通P2P通道
		const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
		const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
		const mycp::bigint sCallId = pResponseInfo->m_pResponseList.getParameterValue("call-id", (mycp::bigint)0);
		const int nP2PData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
		//CCrP2PInfo pP2PInfo(m_nParam,sCallId,sChatId);
		//pP2PInfo.m_sFromAccount = sFromAccount;
		//pP2PInfo.m_nP2PData = nP2PData;
		//if (m_callback)
		//	m_callback->OnP2POk(pP2PInfo);
		//if (m_pHwnd)
		//	::SendMessage(m_pHwnd, CR_WM_P2P_OK, (WPARAM)&pP2PInfo, 0);

		//// 已经打通P2P通道；
		//const mycp::bigint sCallId = pP2PInfo.GetCallId();
		//const mycp::bigint sChatId = pP2PInfo.GetChatId();
		//const int nP2PData = pP2PInfo.m_nP2PData;	// 1：是内网；2：公网

		if (this->IsGroup()) return;

		CRDSinkWnd::pointer pSinkWnd = m_pRoomWndList.GetSinkWnd(sFromAccount);
		if (pSinkWnd.get()==NULL) return;

		if (nP2PData==1 && pSinkWnd->m_pRDManager1.get()!=NULL && !pSinkWnd->m_bLocalP2POk)
		{
			// 找到对方；
			pSinkWnd->m_bLocalP2POk = true;
			pSinkWnd->m_pRDManager1->SetDestAddress(1,pSinkWnd->GetLocalAddress());
			//pSinkWnd->m_pLocalChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),1);	// 1=NAT内网地址

			if (m_rdLocalDesktopWnd.IsLocalDesktopStart())
			{
				m_rdLocalDesktopWnd.StopLocalDesktop();
				m_rdLocalDesktopWnd.StartLocalDesktop(this->m_nDefaultQuant-3, this->m_nDefaultFps+1, this);
			}
			m_nLocalP2PFlag = true;
			m_pRDManagerP2POk = pSinkWnd->m_pRDManager1;
			m_pRDManagerP2POk->SetTranSpeedLimit(256+128);	// 

			// 清空外网P2P，否则会有问题
			pSinkWnd->m_bRemoteP2POk = false;
			if (pSinkWnd->m_pRDManager2.get()!=NULL)
			{
				m_pClearChatRoom.add(pSinkWnd->m_pRDManager2);
				pSinkWnd->m_pRDManager2.reset();
			}
		}else if (nP2PData==2 && pSinkWnd->m_pRDManager2.get()!=NULL && !pSinkWnd->m_bRemoteP2POk && !pSinkWnd->m_bLocalP2POk)
		{
			// 找到对方；
			pSinkWnd->m_bRemoteP2POk = true;
			pSinkWnd->m_pRDManager2->SetDestAddress(1,pSinkWnd->GetRemoteAddress());
			//pFromAccount->m_pRemoteChatRoom->SendP2PTry(sCallId,sChatId,m_pMyAccountInfo->GetUserId(),2);	// 2=NAT公网地址
			if (m_rdLocalDesktopWnd.IsLocalDesktopStart())
			{
				m_rdLocalDesktopWnd.StopLocalDesktop();
				m_rdLocalDesktopWnd.StartLocalDesktop(this->m_nDefaultQuant-1, this->m_nDefaultFps+1, this);
			}
			m_nLocalP2PFlag = false;
			m_pRDManagerP2POk = pSinkWnd->m_pRDManager2;

			// 清空内网P2P，否则会有问题
			pSinkWnd->m_bLocalP2POk = false;
			if (pSinkWnd->m_pRDManager1.get()!=NULL)
			{
				m_pClearChatRoom.add(pSinkWnd->m_pRDManager1);
				pSinkWnd->m_pRDManager1.reset();
			}
		}
		if (m_pRDManagerP2POk.get()!=NULL)
		{
			m_pRDManagerP2POk->SetSourceId(this->m_pRDManager->GetSourceId());
			m_pRDManagerP2POk->RegisterSource(this->GetRoomId(), this->GetParam());
			m_pRDManagerP2POk->RegisterSink(this->GetRoomId(), sFromAccount);
		}
		//*必须放后面
		m_pP2PTaskInfo.reset();
	}
	virtual void OnFCMP2PRequest(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner)
	{
		const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
		//const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
		const tstring sLocalIp = pResponseInfo->m_pResponseList.getParameterValue("local-ip", "");
		const tstring sRemoteIp = pResponseInfo->m_pResponseList.getParameterValue("remote-ip", "");
		const int nRequestData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
		//CCrP2PInfo pP2PInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId());
		//pP2PInfo.m_sFromAccount = sFromAccount;
		//pP2PInfo.m_sFromLocalAddress = sLocalIp;
		//pP2PInfo.m_sFromRemoteAddress = sRemoteIp;
		//pP2PInfo.m_nP2PData = nRequestData;
		//if (m_callback)
		//	m_callback->OnP2PRequest(pP2PInfo);
		//if (m_pHwnd)
		//	::SendMessage(m_pHwnd, CR_WM_P2P_REQUEST, (WPARAM)&pP2PInfo, 0);

		// 服务端转发P2P请求；
		const mycp::bigint sCallId = this->GetRoomId();
		const mycp::bigint sChatId = this->GetRoomId();
		const int nP2PData = nRequestData;	// nP2PData=1：表示对方离线超时，重试
		if (this->IsGroup()) return;

		CRDSinkWnd::pointer pSinkWnd = m_pRoomWndList.GetSinkWnd(sFromAccount);
		if (pSinkWnd.get()==NULL) return;

		// 找到对方；
		if (nP2PData==1)	// 对方发超时离线，请求重连
		{
			pSinkWnd->m_bLocalP2POk = false;
			pSinkWnd->m_bRemoteP2POk = false;
			//MessageBox(NULL,"nP2PData==1","",MB_OK);
		}
		DoCheckP2PState(sLocalIp, sRemoteIp, pSinkWnd);			// **已经打通也不返回，可以再处理一次；
		//if (DoCheckP2PState(sLocalIp, sRemoteIp, pSinkWnd))	// 已经打通，直接返回
		//{
		//	return;
		//}

		// LOCAL P2P，这个可以打通，connect&send也可以打通
		if (!sLocalIp.empty())
		{
			CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRDManager1;
			if (pRDManager.get()==NULL)
			{
				pRDManager = CEBRtpManager::create(this);
				pRDManager->Start(m_pRDManager->GetAddress(),RDS_APP_NAME,0,0);
				pRDManager->SetParameter(1);	// 这是P2P
				pSinkWnd->m_pRDManager1 = pRDManager;

				GetLocalHostInfo();
				if (!m_pLocalHostIp.empty())
				{
					// *这边也主动请求一次；
					char lpszLocalIp[64];
					sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pRDManager->GetLocalPort());
					pRDManager->SetDestAddress(0,m_pRDManager->GetAddress().address());
					pRDManager->SendP2PRequest(m_pRDManager->GetSourceId(),this->GetRoomId(),sFromAccount,lpszLocalIp,0);
				}
			}
			if (!m_pLocalHostIp.empty())
			{
				CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
				pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", sFromAccount));

				char lpszLocalIp[64];
				sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pRDManager->GetLocalPort());
				pRDManager->SetDestAddress(0,m_pRDManager->GetAddress().address());
				pRDManager->SendP2PResponse(m_pRDManager->GetSourceId(),this->GetRoomId(),sFromAccount,lpszLocalIp,0,pRequestInfo);
			}
		}

		// REMOTE P2P
		{
			CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRDManager2;
			if (pRDManager.get()==NULL)
			{
				pRDManager = CEBRtpManager::create(this);
				pRDManager->Start(m_pRDManager->GetAddress(),RDS_APP_NAME,0,0);
				pRDManager->SetParameter(1);	// 这是P2P
				pSinkWnd->m_pRDManager2 = pRDManager;

				// 这里主动请求一次；
				pRDManager->SetDestAddress(0,m_pRDManager->GetAddress().address());
				pRDManager->SendP2PRequest(m_pRDManager->GetSourceId(),this->GetRoomId(),sFromAccount,"",0);
			}
			// 响应
			pRDManager->SetDestAddress(0,m_pRDManager->GetAddress().address());
				CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
				pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", sFromAccount));
			pRDManager->SendP2PResponse(m_pRDManager->GetSourceId(),this->GetRoomId(),sFromAccount,"",0,pRequestInfo);
		}
	}
	CP2PTaskInfo::pointer m_pP2PTaskInfo;
	virtual void OnFCMP2PResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner)
	{
		const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
		//const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
		const tstring sLocalIp = pResponseInfo->m_pResponseList.getParameterValue("local-ip", "");
		const tstring sRemoteIp = pResponseInfo->m_pResponseList.getParameterValue("remote-ip", "");
		const int nRequestData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
		//CCrP2PInfo pP2PInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId());
		//pP2PInfo.m_sFromAccount = sFromAccount;
		//pP2PInfo.m_sFromLocalAddress = sLocalIp;
		//pP2PInfo.m_sFromRemoteAddress = sRemoteIp;
		//pP2PInfo.m_nP2PData = nRequestData;
		//if (m_callback)
		//	m_callback->OnP2PResponse(pP2PInfo,0);
		//if (m_pHwnd)
		//	::SendMessage(m_pHwnd, CR_WM_P2P_RESPONSE, (WPARAM)&pP2PInfo, 0);

		// nFlag==0服务端转发P2P请求；
		const mycp::bigint sCallId = this->GetRoomId();
		const mycp::bigint sChatId = this->GetRoomId();
		const int nP2PData = nRequestData;
		bool bP2POk = false;
		if (DoP2PTask(sCallId,sChatId,sFromAccount,sLocalIp,sRemoteIp,bP2POk))
		{
			// 未打通P2P，或其他错误，需要保存做超时检查
			CP2PTaskInfo::pointer pP2PTaskInfo = m_pP2PTaskInfo;
			if (pP2PTaskInfo.get()==NULL)
			{
				pP2PTaskInfo = CP2PTaskInfo::create();
				m_pP2PTaskInfo = pP2PTaskInfo;
			}
			pP2PTaskInfo->m_nCallId = sCallId;
			pP2PTaskInfo->m_nFromUserId = sFromAccount;
			pP2PTaskInfo->m_sFromLocalIp = sLocalIp;
			pP2PTaskInfo->m_sFromRemoteIp = sRemoteIp;
			pP2PTaskInfo->m_tP2PTry = time(0);
			pP2PTaskInfo->m_bP2PResponse = true;
		}else
		{
			// 已经打通P2P
			m_pP2PTaskInfo.reset();
		}
	}
	virtual void OnCMP2PResponseRes(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner)
	{
		if (pRequestInfo.get()==NULL) return;
		const mycp::bigint sFromAccount = pRequestInfo->m_pRequestList.getParameterValue("to-uid", (mycp::bigint)0);
		//CCrP2PInfo pP2PInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId());
		//pP2PInfo.m_sFromAccount = sFromAccount;
		//if (m_callback)
		//	m_callback->OnP2PResponse(pP2PInfo,1);
		//if (m_pHwnd)
		//	::SendMessage(m_pHwnd, CR_WM_P2P_RESPONSE, (WPARAM)&pP2PInfo, 1);

		// nFlag==1:响应返回；
		const mycp::bigint sCallId = this->GetRoomId();
		const mycp::bigint sChatId = this->GetRoomId();
		if (this->IsGroup()) return;

		CRDSinkWnd::pointer pSinkWnd = m_pRoomWndList.GetSinkWnd(sFromAccount);
		if (pSinkWnd.get()==NULL) return;

		if (!pSinkWnd->GetLocalAddress().empty())
		{
			CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRDManager1;	// 使用这个，不会导致OnP2POk清空异常
			if (pRDManager.get()!=NULL)
			{
				pRDManager->SetDestAddress(1,pSinkWnd->GetLocalAddress());
				pRDManager->SendP2PTry(sCallId,sChatId,m_pRDManager->GetSourceId(),0,1);	// 1=内网地址
			}
		}
		if (!pSinkWnd->GetRemoteAddress().empty())
		{
			CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRDManager2;	// 使用这个，不会导致OnP2POk清空异常
			if (pRDManager.get()!=NULL)
			{
				pRDManager->SetDestAddress(1,pSinkWnd->GetRemoteAddress());
				pRDManager->SendP2PTry(sCallId,sChatId,m_pRDManager->GetSourceId(),0,2);	// 2=NAT公网地址
			}
		}
	}
	void OnProcessTimer(const CEBRtpManager* pUMOwner)
	{
		const time_t tNow = time(0);
		CP2PTaskInfo::pointer pP2PTaskInfo = m_pP2PTaskInfo;
		if (pP2PTaskInfo.get()!=NULL)
		{
			// 检查P2P穿透任务；
			const mycp::bigint sCallId = pP2PTaskInfo->m_nCallId;
			const mycp::bigint sChatId = pP2PTaskInfo->m_nCallId;
			const mycp::bigint sFromUserId = pP2PTaskInfo->m_nFromUserId;
			bool bP2POk = false;
			if (pP2PTaskInfo->m_bP2PResponse && !DoP2PTask(sCallId,sChatId,sFromUserId,pP2PTaskInfo->m_sFromLocalIp,pP2PTaskInfo->m_sFromRemoteIp,bP2POk))
			{
				// 打通成功，或其他失败
				m_pRDManager->RegisterSink(this->GetRoomId(), sFromUserId);
				m_pP2PTaskInfo.reset();
			}else if (pP2PTaskInfo->m_tP2PTry>0 && (tNow-pP2PTaskInfo->m_tP2PTry)>=10)
			{
				// 超过8秒，当超时处理；

				// 清空P2P连接资源
				CRDSinkWnd::pointer pSinkWnd = m_pRoomWndList.GetSinkWnd(sFromUserId);
				if (pSinkWnd.get()!=NULL)
				{
					//MessageBox(NULL,"OnProcessTimer","",MB_OK);
					pSinkWnd->m_bLocalP2POk = false;
					if (pSinkWnd->m_pRDManager1.get()!=NULL)
					{
						m_pClearChatRoom.add(pSinkWnd->m_pRDManager1);
						pSinkWnd->m_pRDManager1.reset();
					}
					pSinkWnd->m_bRemoteP2POk = false;
					if (pSinkWnd->m_pRDManager2.get()!=NULL)
					{
						m_pClearChatRoom.add(pSinkWnd->m_pRDManager2);
						pSinkWnd->m_pRDManager2.reset();
					}
				}
				m_pRDManager->RegisterSink(this->GetRoomId(), sFromUserId);
				m_pP2PTaskInfo.reset();
			}
		}

		static unsigned int theIndex = 0;
		theIndex++;
		if ((theIndex%5)==4)
		{
			CEBRtpManager::pointer pChatRoom;
			if (m_pClearChatRoom.front(pChatRoom))
			{
				//pChatRoom->Stop();
			}
		}

	}

protected:
	mycp::bigint m_nRoomId;
	bool m_bIsGroup;
	mycp::bigint m_nParam;
	std::vector<std::string> m_pLocalHostIp;
	CEBRtpManager::pointer m_pRDManagerP2POk;
	bool m_nLocalP2PFlag;		// true:内网；false:公网

	int m_nDefaultQuant;
	int m_nDefaultFps;
};

Cebrd::Cebrd(void)
: m_handle(NULL)

{

}

Cebrd::~Cebrd(void)
{
	Stop();
}

int ParseString(const char * lpszString, const char * lpszInterval, std::vector<std::string> & pOut)
{
	const std::string sIn(lpszString);
	const size_t nInLen = sIn.size();
	const size_t nIntervalLen = strlen(lpszInterval);
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

	//std::string sIn(lpszString);
	//pOut.clear();
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
std::string GetHostIp(const char * lpszHostName,const char* lpszDefaultName)
{
	if (lpszHostName==NULL || strlen(lpszHostName)==0)
		return lpszDefaultName;
	try
	{
		struct hostent *host_entry;
		//struct sockaddr_in addr;
		/* 即要解析的域名或主机名 */
		host_entry=gethostbyname(lpszHostName);
		//printf("%s\n", dn_or_ip);
		if(host_entry!=0)
		{
			//printf("解析IP地址: ");
			char lpszIpAddress[50];
			memset(lpszIpAddress, 0, sizeof(lpszIpAddress));
			sprintf(lpszIpAddress, "%d.%d.%d.%d",
				(host_entry->h_addr_list[0][0]&0x00ff),
				(host_entry->h_addr_list[0][1]&0x00ff),
				(host_entry->h_addr_list[0][2]&0x00ff),
				(host_entry->h_addr_list[0][3]&0x00ff));
			return lpszIpAddress;
		}
	}catch(std::exception&)
	{
	}catch(...)
	{}
	return lpszDefaultName;
}
tstring tehAppDataTempPath;
tstring GetAddressList(const tstring& sFullAddress)
{
	std::vector<std::string> pList;
	if (ParseString(sFullAddress.c_str(),":",pList)!=2)
		return "";

	const tstring sAddress = pList[0];
	char sTempIniFile[260];
	sprintf(sTempIniFile,"%s\\temp.ini",tehAppDataTempPath.c_str());
	char lpszIp[101];
	memset(lpszIp,0,101);
	GetPrivateProfileString(_T("address_list"),sAddress.c_str(),"",lpszIp,100,sTempIniFile);
	if (strlen(lpszIp)==0)
	{
		return "";
	}
	tstring result = lpszIp;
	result.append(":");
	result.append(pList[1]);
	return result;
}
void SaveAddressList(const tstring& sFullAddress)
{
	std::vector<std::string> pList;
	if (ParseString(sFullAddress.c_str(),":",pList)==2)
	{
		const tstring sAddress(pList[0]);
		const std::string sIp = GetHostIp(sAddress.c_str(),sAddress.c_str());
		if (sAddress!=sIp)
		{
			char sTempIniFile[260];
			sprintf(sTempIniFile,"%s\\temp.ini",tehAppDataTempPath.c_str());
			WritePrivateProfileString(_T("address_list"),sAddress.c_str(),sIp.c_str(),sTempIniFile);
		}
	}
}

int Cebrd::Start(const char* address, unsigned int bindPort)
{
	int result = 0;
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager == NULL)
	{
		bool bRetry = false;
		pChatManager = new CRDManager();
		//pChatManager->m_chatManager->SetSslKey(theSslPublicKey,theSslPrivateKey,theSslPrivatePwd);
		result = pChatManager->m_pRDManager->Start(CCgcAddress(address, CCgcAddress::ST_UDP), RDS_APP_NAME, bindPort, 0);
		if (result==-1)
		{
			// 有可能域名解析问题，再试一次；
			const tstring sServerAddress = GetAddressList(address);
			if (!sServerAddress.empty())
			{
				delete pChatManager;
				pChatManager = new CRDManager();
				//pChatManager->m_chatManager->SetSslKey(theSslPublicKey,theSslPrivateKey,theSslPrivatePwd);
				result = pChatManager->m_pRDManager->Start(CCgcAddress(address, CCgcAddress::ST_UDP), RDS_APP_NAME, bindPort, 0);
				bRetry = true;
			}
		}
		if (!bRetry)
		{
			SaveAddressList(address);
		}
		m_handle = pChatManager;
	}else
	{
		result = 1;
	}
	return result;
}


bool Cebrd::IsStart(void) const
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_pRDManager->IsStart();
	}
	return false;
}
void Cebrd::SetCallBack(CEBRDCallBack * pCallBack)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_pCallback = pCallBack;
	}
}
void Cebrd::SetMsgHwnd(HWND pHwnd)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_pHwnd = pHwnd;
	}
}
//void Cchatroom::SetParam(unsigned long nParam)
//{
//	CChatManager * pChatManager = (CChatManager*)m_handle;
//	if (pChatManager != NULL)
//	{
//		pChatManager->m_nParam = nParam;
//		//pChatManager->m_sParam = "";
//	}
//}
//
void Cebrd::Stop(void)
{
	StopLocalDesktop();
	DelAllSink();
	UnRegister();
	CRDManager * pChatManager = (CRDManager*)m_handle;
	m_handle = NULL;
	if (pChatManager != NULL)
	{
		pChatManager->m_pRDManager->Stop();
		delete pChatManager;
	}
}

//void Cebrd::SetRDFPS(int fps)
//{
//	CRDManager * pChatManager = (CRDManager*)m_handle;
//	if (pChatManager != NULL)
//	{
//		pChatManager->SetFPS(fps);
//	}
//}
//int Cebrd::GetRDFPS(void) const
//{
//	CRDManager * pChatManager = (CRDManager*)m_handle;
//	if (pChatManager != NULL)
//	{
//		return pChatManager->GetFPS();
//	}
//	return 0;
//}

//void Cebrd::SetSourceId(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam)
//{
//	CRDManager * pChatManager = (CRDManager*)m_handle;
//	if (pChatManager != NULL)
//	{
//		pChatManager->SetRoomId(nRoomId);
//		pChatManager->SetParam(nParam);
//		pChatManager->m_pRDManager->SetSourceId(nSourceId);
//	}
//}
mycp::bigint Cebrd::GetRoomId(void) const
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->GetRoomId();
	}
	return 0;
}
mycp::bigint Cebrd::GetSourceId(void) const
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_pRDManager->GetSourceId();
	}
	return 0;
}

bool Cebrd::Register(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam, bool bIsGroup)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->SetRoomId(nRoomId, bIsGroup);
		pChatManager->SetParam(nParam);
		pChatManager->m_pRDManager->SetSourceId(nSourceId);
		pChatManager->m_pRDManager->RegisterSource(nRoomId, nParam);	// **需要先做一次注册，本地保存数据，否则会出错；
		return pChatManager->m_pRDManager->SendRDOn(nRoomId,nSourceId,nParam,EB_LOGON_TYPE_PC)==0?true:false;
		//return pChatManager->m_pRDManager->RegisterSource(nRoomId, nParam)==0?true:false;
	}
	return false;
}
void Cebrd::UnRegister(void)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->UnRegister();
	}
}

bool Cebrd::AddSink(mycp::bigint nDestId, HWND hWndParent)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->AddSink(nDestId, hWndParent);
		//const mycp::bigint nRoomId = pChatManager->GetRoomId();
		//const bool ret = pChatManager->m_pRDManager->RegisterSink(nRoomId, nDestId)==0?true:false;
		//if (ret)
		//{
		//	return pChatManager->m_pRoomWndList.AddSinkWnd( nDestId, hWndParent);
		//}
		//return ret;
	}
	return false;
}
void Cebrd::DelSink(mycp::bigint nDestId)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->DelSink(nDestId);
	}
}
void Cebrd::DelAllSink(void)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->DelAllSink();
	}
}
bool Cebrd::MoveSinkWindow(mycp::bigint nDestId, int x, int y, int width, int height, BOOL bRepaint)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_pRoomWndList.MoveSinkWindow(nDestId, x, y, width, height, bRepaint);
	}
	return false;
}
bool Cebrd::ControlSinkDesktop(mycp::bigint nDestId, int nMouseFPS)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_pRoomWndList.EnableControl(nDestId, nMouseFPS);
	}
	return false;
}
int Cebrd::ControlSinkDesktop(mycp::bigint nDestId) const
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_pRoomWndList.EnableControl(nDestId);
	}
	return 0;
}
bool Cebrd::StartLocalDesktop(int quant, int fps)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->StartLocalDesktop(quant, fps);
	}
	return false;
}
int Cebrd::GetLocalDesktopFPS(void) const
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_rdLocalDesktopWnd.GetLocalDesktopFPS();
	}
	return 0;
}
void Cebrd::EnableControlLocalDesktop(mycp::bigint nDestId)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		if (nDestId==0)
		{
			nDestId = pChatManager->m_rdLocalDesktopWnd.EnableControl();
			pChatManager->DelSink(nDestId);
			pChatManager->m_rdLocalDesktopWnd.EnableControl(0);
		}else
		{
			pChatManager->AddSink(nDestId,NULL);
			pChatManager->m_rdLocalDesktopWnd.EnableControl(nDestId);
		}
	}
}
mycp::bigint Cebrd::EnableControlLocalDesktop(void) const
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		return pChatManager->m_rdLocalDesktopWnd.EnableControl();
	}
	return 0;
}
void Cebrd::StopLocalDesktop(void)
{
	CRDManager * pChatManager = (CRDManager*)m_handle;
	if (pChatManager != NULL)
	{
		pChatManager->m_rdLocalDesktopWnd.StopLocalDesktop();
	}
}
//
//bool Cebrd::SendData(const unsigned char* pSendData, mycp::uint32 nDataSize, mycp::uint32 nTimestamp, mycp::uint8 nDataType, mycp::uint8 nNAKType)
//{
//	CRDManager * pChatManager = (CRDManager*)m_handle;
//	if (pChatManager != NULL)
//	{
//		const mycp::bigint nRoomId = pChatManager->GetRoomId();
//		return pChatManager->m_pRDManager->SendRtpData(nRoomId, pSendData, nDataSize, nTimestamp, nDataType, nNAKType)==0?true:false;
//	}
//	return false;
//}

} // namespace entboost

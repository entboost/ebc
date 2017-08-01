// SMTP.cpp: implementation of the CSMTP class.
#include "SMTP.h"
#include "Base64.h"
#ifdef WIN32
#pragma warning(disable:4267)
#endif // WIN32

#define USES_OPENSSL
#ifdef USES_OPENSSL
#ifdef WIN32
#pragma comment(lib, "libeay32.lib")  
#pragma comment(lib, "ssleay32.lib") 
#endif // WIN32
#endif // USES_OPENSSL

int ParseString(const char * lpszString, const char * lpszInterval, std::vector<tstring> & pOut)
{
	const tstring sIn(lpszString);
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
}
#ifdef WIN32
inline tstring str_convert(const char * strSource, int sourceCodepage, int targetCodepage)
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

	tstring result(pTargetData);
	//	tstring result(pTargetData, targetLen);
	delete[] pUnicode;
	delete[] pTargetData;
	return   result;
}
inline tstring ACP2UTF8(const char* sString)
{
	return str_convert(sString,CP_ACP,CP_UTF8);
}
inline tstring UTF82ACP(const char* sString)
{
	return str_convert(sString,CP_UTF8,CP_ACP);
}
#endif

//本结构数组代表在每一步期待的正确返回值和如果返回值和该返回值不同时应该显示的出错信息
CSMTP::response_code CSMTP::response_table_smtp[] =
{
	{ 250, _T( "SMTP server error" ) },
	{ 220, _T( "SMTP server not available" ) },
	{ 235, _T( "PASS error." ) },
	{ 334, _T( "AUTH LOGIN error." ) },
	{ 354, _T( "SMTP server not ready for data" ) },
	{ 221, _T( "SMTP server didn't terminate session" ) }
};
CSMTP::response_code CSMTP::response_table_pop3[] =
{
	{ 0, _T( "POP3 error" ) }
};

namespace base64
{
	struct data6  
	{  
		unsigned int d4:6;  
		unsigned int d3:6;  
		unsigned int d2:6;  
		unsigned int d1:6;  
	};  
	char con628(char c6)  
	{  
		char rtn = '\0';  
		if (c6 < 26) rtn = c6 + 65;  
		else if (c6 < 52) rtn = c6 + 71;  
		else if (c6 < 62) rtn = c6 - 4;  
		else if (c6 == 62) rtn = 43;  
		else rtn = 47;  
		return rtn;  
	}
	// base64的实现  
	void base64(char *dbuf, const char *buf128, int len)  
	{  
		struct data6 *ddd = NULL;  
		int i = 0;  
		char buf[256] = {0};  
		char *tmp = NULL;  
		char cc = '\0';  
		memset(buf, 0, 256);  
		strcpy(buf, buf128);  
		for(i = 1; i <= len/3; i++)  
		{  
			tmp = buf+(i-1)*3;  
			cc = tmp[2];  
			tmp[2] = tmp[0];  
			tmp[0] = cc;  
			ddd = (struct data6 *)tmp;  
			dbuf[(i-1)*4+0] = con628((unsigned int)ddd->d1);  
			dbuf[(i-1)*4+1] = con628((unsigned int)ddd->d2);  
			dbuf[(i-1)*4+2] = con628((unsigned int)ddd->d3);  
			dbuf[(i-1)*4+3] = con628((unsigned int)ddd->d4);  
		}  
		if(len%3 == 1)  
		{  
			tmp = buf+(i-1)*3;  
			cc = tmp[2];  
			tmp[2] = tmp[0];  
			tmp[0] = cc;  
			ddd = (struct data6 *)tmp;  
			dbuf[(i-1)*4+0] = con628((unsigned int)ddd->d1);  
			dbuf[(i-1)*4+1] = con628((unsigned int)ddd->d2);  
			dbuf[(i-1)*4+2] = '=';  
			dbuf[(i-1)*4+3] = '=';  
		}  
		if(len%3 == 2)  
		{  
			tmp = buf+(i-1)*3;  
			cc = tmp[2];  
			tmp[2] = tmp[0];  
			tmp[0] = cc;  
			ddd = (struct data6 *)tmp;  
			dbuf[(i-1)*4+0] = con628((unsigned int)ddd->d1);  
			dbuf[(i-1)*4+1] = con628((unsigned int)ddd->d2);  
			dbuf[(i-1)*4+2] = con628((unsigned int)ddd->d3);  
			dbuf[(i-1)*4+3] = '=';  
		}  
		return;  
	}  
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSMTP::CSMTP( const char* szSMTPServerName, size_t nPort, bool bSSL, MAIL_TYPE nMailType)
: m_sSMTPServerHostName(szSMTPServerName)
, m_nPort(nPort), m_bSsl(bSSL), m_nMailType(nMailType)
, m_bDeleteMail(false)
, m_sslctx(NULL), m_bDeleteCtx(false)

{
	m_sError = "OK";
}

CSMTP::~CSMTP()
{
	//printf("**** CSMTP::~CSMTP() Disconnect...\n");
	Disconnect();
	//printf("**** CSMTP::~CSMTP() Disconnect ok\n");
	if (m_sslctx!=0 && m_bDeleteCtx)
	{
		delete m_sslctx;
		m_sslctx = 0;
	}
}

void CSMTP::SetSSLContext(bool bDelete, boost::asio::ssl::context * sslctx)
{
	if (m_sslctx!=0 && m_bDeleteCtx)
	{
		delete m_sslctx;
		m_sslctx = 0;
	}
	m_sslctx = sslctx;
	m_bDeleteCtx = bDelete;
}

//void CSMTP::Test2_SSL_library_init(void)
//{
//	static bool theTestInit = false;
//	// ???
//	printf("**** Test2_SSL_library_init = %d\n",(int)(theTestInit?1:0));
//	if (!theTestInit)
//	{
//		theTestInit = true;
//		SSL_CTX * handle_ = ::SSL_CTX_new(::SSLv2_client_method());	// 返回0
//		if (handle_==NULL)
//		{
//			SSL_library_init();	// *** 初始化SSL环境，解决组件在线更新 context error 异常问题；
//		}else
//		{
//			SSL_CTX_free(handle_);
//		}
//	}
//}

// 定义存放主机名的缓冲区大小
int CSMTP::Connect(const char* sMailAccount, const char* sPasswd, bool* pOutTimeout)
{
	char sHello[128];

	if ( m_wsSMTPServer.get()!=0 )
	{
		return 1;
	}
	if (m_wsSMTPServer.get()==0)
	{
		boost::asio::ssl::context * sslctx = NULL;
		//printf("**** CSMTP::Connect m_bSsl=%d,sMailAccount=%s\n",(int)(m_bSsl?1:0),sMailAccount);
		if (m_bSsl)
		{
			if (m_sslctx==NULL)
			{
				mycp::asio::TcpClient::Test_To_SSL_library_init();
				namespace ssl = boost::asio::ssl;
				//printf("**** CSMTP::Connect 11111111\n");
				m_sslctx = new boost::asio::ssl::context (ssl::context::sslv23_client);	// OK	** 更新组件，这里会有异常
				m_bDeleteCtx = true;
				//printf("**** CSMTP::Connect 2222222222222\n");
				m_sslctx->set_default_verify_paths();
				m_sslctx->set_options(ssl::context::verify_peer);
				m_sslctx->set_verify_mode(ssl::verify_peer);
				//m_sslctx->set_verify_callback(ssl::rfc2818_verification("smtp.163.com"));
			}
			sslctx = m_sslctx;
		}
		char lpszServer[260];
		sprintf(lpszServer,"%s:%d",m_sSMTPServerHostName.c_str(),(int)GetPort());
		//printf("**** CSMTP::Connect CgcTcpClient::create %s\n",lpszServer);
		m_wsSMTPServer = entboost::CgcTcpClient::create(this);
		//printf("**** CSMTP::Connect CgcTcpClient::create %s ok\n",lpszServer);
		if (m_wsSMTPServer->startClient(lpszServer,0,sslctx)!=0)
		{
			//printf("**** CSMTP::Connect startClient error\n");
			ClearTcpClient();
			m_sError = "Start Smtp Client Error!";
			return -1;
		}
		//printf("**** CSMTP::Connect startClient ok\n");
	}
	if (m_nMailType==MAIL_TYPE_SMTP)
	{
		if(!getMailResponseSMTP(CONNECT_SUCCESS))
		{
			// 接收应答信息失败
			m_sError = "Server Not Response!";
			ClearTcpClient();
			return -1;
		}
		char local_host[100];
		gethostname(local_host,sizeof(local_host));

		// 以HELO命令开始发信 
		sprintf(sHello, "HELO %s\r\n",local_host);
		m_pResponseList.clear();
		m_wsSMTPServer->sendData((const unsigned char*)sHello,strlen(sHello));
		if(!getMailResponseSMTP(GENERIC_SUCCESS, 5, pOutTimeout))
		{
			// 获得应答信息失败
			ClearTcpClient();
			return -1;
		}
		// 以HELO命令开始发信 
		sprintf(sHello,"AUTH LOGIN\r\n");
		m_pResponseList.clear();
		m_wsSMTPServer->sendData((const unsigned char*)sHello,strlen(sHello));
		if(!getMailResponseSMTP(AUTH_LOGIN_SUCCESS, 5, pOutTimeout))
		{
			ClearTcpClient();
			return 0;
		}
		if (strlen(sMailAccount)==0 || strlen(sPasswd)==0)
			return 0;

		char buf[64];
		strcpy(buf, sMailAccount);
		char login[128];
		memset(login,0,sizeof(login));
		base64::base64(login, buf, (int)strlen(buf));
		sprintf(buf, "%s\r\n", login);  
		m_pResponseList.clear();
		m_wsSMTPServer->sendData((const unsigned char*)buf,strlen(buf));
		if(!getMailResponseSMTP(AUTH_LOGIN_SUCCESS, 5, pOutTimeout))
		{
			ClearTcpClient();
			return 0;
			//return -1;
		}
		strcpy(buf, sPasswd);
		memset(login,0,sizeof(login));
		base64::base64(login, buf, (int)strlen(buf));
		sprintf(buf, "%s\r\n", login);  
		m_pResponseList.clear();
		m_wsSMTPServer->sendData((const unsigned char*)buf,strlen(buf));
		if(!getMailResponseSMTP(PASS_SUCCESS,5,pOutTimeout))
		{
			ClearTcpClient();
			return 0;
			//return bTimeout?-1:0;
		}
	}else
	{
		const int nTimeoutSecond = 5;
		//const int nTimeoutSecond = m_bSsl?8:5;
		CSmtpResponseInfo::pointer pResponseInfo;
		if(!getMailResponsePOP3(nTimeoutSecond,&pResponseInfo,pOutTimeout))
		{
			// 接收应答信息失败
			m_sError = "Server Not Response!";
			ClearTcpClient();
			return pResponseInfo.get()==NULL?-1:0;
			//return -1;
		}
		if (strlen(sMailAccount)==0 || strlen(sPasswd)==0)
			return 0;

		sprintf(sHello, "USER %s\r\n", sMailAccount);  
		m_pResponseList.clear();
		m_wsSMTPServer->sendData((const unsigned char*)sHello,strlen(sHello));
		if(!getMailResponsePOP3(5,NULL,pOutTimeout))
		{
			// 获得应答信息失败
			ClearTcpClient();
			return 0;
			//return -1;
		}

		sprintf(sHello, "PASS %s\r\n", sPasswd);  
		m_pResponseList.clear();
		m_wsSMTPServer->sendData((const unsigned char*)sHello,strlen(sHello));
		//CSmtpResponseInfo::pointer pResponseInfo;
		//if(!getMailResponsePOP3(5,&pResponseInfo))
		if(!getMailResponsePOP3(5, NULL, pOutTimeout))
		{
			ClearTcpClient();
			return 0;
			//printf("**** PASS response=%d\n",(int)pResponseInfo.get()==NULL?0:1);
			//return pResponseInfo.get()==NULL?-1:0;
		}
	}
	return 1;
}


bool CSMTP::Disconnect(void)
{
	if(m_wsSMTPServer.get()==0)
		return true;

	// 发送QUIT命令断开连接
	tstring strQuit = _T( "QUIT\r\n" );
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)strQuit.c_str(), strQuit.size());

	const int nTimeoutSecond = m_bDeleteMail?5:1;
	bool bRet = false;
	if (m_nMailType==MAIL_TYPE_SMTP)
		bRet = getMailResponseSMTP(QUIT_SUCCESS,nTimeoutSecond);
	else
		bRet = getMailResponsePOP3(nTimeoutSecond);
	// 关闭套接字
	if (m_wsSMTPServer.get()!=0)
	{
#ifdef WIN32
		Sleep(200);
#else
		usleep(200000);
#endif
		ClearTcpClient();
	}

	// 释放资源，并将连接标志置为FALSE
	return bRet;
}
bool CSMTP::IsConnected(void) const
{
	return m_wsSMTPServer.get()==NULL?false:true;
}
void CSMTP::ClearTcpClient(void)
{
	if (m_wsSMTPServer.get()!=0)
	{
		m_wsSMTPServer->stopClient();
		m_wsSMTPServer.reset();
	}
}

//
//bool CSMTP::Login(void)
//{
//
//}

tstring CSMTP::GetLastError(void) const
{
	return m_sError;
}

bool CSMTP::SendMailMessage(CMailMessage * msg)
{
	if (msg == NULL)
	{
		// msg为空是没有意义的
		return false;
	}

	// 如果还没有建立连接
//#ifdef USES_AFX_SOCKET
//	if (m_wsSMTPServer==0)
//#else
	if (m_wsSMTPServer.get()==0)
//#endif
	{
		m_sError = "Disconnect Error!";
		return false;
	}

	if(!FormatMailMessage(msg))
	{
		// 如果调整消息体的格式不成功
		return false;
	}
	return transmit_message(msg);
}
bool CSMTP::SendSTATCommand(void)
{
	if (m_wsSMTPServer.get()==0)
	{
		// 若还未建立连接
		m_sError = "Disconnect Error!";
		return false;
	}
	// 发送STAT命令
	tstring strFrom = "STAT\r\n";
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)strFrom.c_str(),strFrom.size());
	CSmtpResponseInfo::pointer pResponseInfo;
	if (!getMailResponsePOP3(5,&pResponseInfo))
	{
		return false;
	}
	//getMailResponsePOP3();	// +OK
//13 115281
	pResponseInfo->m_sResponseMsg;

	return true;
}
bool CSMTP::SendLISTCommand(int nMailId,std::vector<int>& pOutMailSizeList,bool& pOutFinished)
{
	if (m_wsSMTPServer.get()==0)
	{
		// 若还未建立连接
		m_sError = "Disconnect Error!";
		return false;
	}
	char buf[64];
	if (nMailId<=0)
		sprintf(buf,"LIST\r\n");
	else
		sprintf(buf,"LIST %d\r\n",nMailId);
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)buf,strlen(buf));
	CSmtpResponseInfo::pointer pResponseInfo;
	if (!getMailResponsePOP3(5,&pResponseInfo))
	{
		return false;
	}
//+OK 13 115281
//1 10176
//2 9898
//3 1625
//4 8999
//5 4633
//6 10492
//7 10503
//8 10536
//9 10548
//10 10527
//11 10596
//12 10596
//13 6152
//.

	tstring sReceiveData(pResponseInfo->m_sResponseMsg);
	if (nMailId<=0)
	{
		std::string::size_type nFind1 = pResponseInfo->m_sResponseMsg.find(".\r\n");
		// 未接收完成，分多次发送；类似 QQ 邮箱
		while (nFind1==std::string::npos)
		{
			pResponseInfo.reset();
			getMailResponsePOP3(2,&pResponseInfo);
			if (pResponseInfo.get()==NULL)
			{
				break;
			}
			sReceiveData.append(pResponseInfo->m_sResponseMsg);
			nFind1 = pResponseInfo->m_sResponseMsg.find(".\r\n");
		}
	}
	std::vector<tstring> pList;
	ParseString(sReceiveData.c_str(),"\r\n",pList);
	for (size_t i=0; i<pList.size(); i++)
	{
		const tstring& sMailInfo = pList[i];
		if (nMailId==0 && i==0 && sMailInfo.find("1 ")==std::string::npos)	// ** QQ邮箱，没有返回类型 13 115281 数据头；
			continue;
		else if (sMailInfo==".")
		{
			pOutFinished = true;
			break;
		}

		const std::string::size_type find = sMailInfo.find(' ');
		if (find!=std::string::npos)
		{
			const tstring sMailId(sMailInfo.substr(find+1));
			pOutMailSizeList.push_back(atoi(sMailId.c_str()));
		}		
	}
	//if (nMailId>0 && pOutMailSizeList.size()==1)
	//	pOutFinished = true;
	return true;
}
bool CSMTP::SendUIDLCommand(int nMailId, std::vector<tstring>& pOutMailIdList,bool& pOutFinished)
{
	if (m_wsSMTPServer.get()==0)
	{
		// 若还未建立连接
		m_sError = "Disconnect Error!";
		return false;
	}
	// 发送STAT命令
	char buf[64];
	if (nMailId<=0)
		sprintf(buf,"UIDL\r\n");
	else
		sprintf(buf,"UIDL %d\r\n",nMailId);
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)buf,strlen(buf));
	CSmtpResponseInfo::pointer pResponseInfo;
	if (!getMailResponsePOP3(5,&pResponseInfo))
	{
		return false;
	}
//13 115281
//1 xtbBLxsvylUL29B4BQAAs5
//2 1tbitB8vylSIJokz+gAAs0
//3 1tbiFA0vylD+V--ZiwAAso
//4 xtbBZhk2ylEAArsl2AAAsW
//5 1tbiTxY7ylWBOKg1NgAAsv
//6 1tbiTwtFylWBOU4ROQAAsA
//7 1tbitAVFylSIJ-9P+wAAsO
//8 1tbiFA9GylD+WXciMQAAsE
//9 1tbiTwtGylWBOVMAzgAAs4
//10 xtbBoQ1GylQGy9Z4cgAAsl
//11 1tbiTwtGylWBOVMMkAABsr
//12 1tbiKRdGylEAPqh3TgAAsM
//13 xtbBZgEIElEABEIF8wAAbc
//.

//1 xtbBLxsvylUL29B4BQAAs5

//1 8753
//1 100.1.Sc5Bp.753k.1446608352.eb2email@21cn.com
//.

	tstring sReceiveData(pResponseInfo->m_sResponseMsg);
	if (nMailId<=0)
	{
		std::string::size_type nFind1 = pResponseInfo->m_sResponseMsg.find(".\r\n");
		// 未接收完成，分多次发送；类似 QQ 邮箱
		while (nFind1==std::string::npos)
		{
			pResponseInfo.reset();
			getMailResponsePOP3(2,&pResponseInfo);
			if (pResponseInfo.get()==NULL)
			{
				break;
			}
			sReceiveData.append(pResponseInfo->m_sResponseMsg);
			nFind1 = pResponseInfo->m_sResponseMsg.find(".\r\n");
		}
	}
	std::vector<tstring> pList;
	ParseString(sReceiveData.c_str(),"\r\n",pList);
	for (size_t i=0; i<pList.size(); i++)
	{
		tstring sMailInfo(pList[i]);
		if (nMailId==0 && i==0)
		{
			// ** QQ邮箱，没有返回类型 13 115281 数据头；
			// ** 但QQ邮箱，第一条邮箱，会有二行，第一行是空行
			std::string::size_type find = sMailInfo.find('\n');
			if (find!=std::string::npos)
				sMailInfo = sMailInfo.substr(find+1);

			if (sMailInfo.size()<=3 || sMailInfo.substr(0,2)!="1 ")	// ** QQ邮箱，没有返回类型 13 115281 数据头；
			//if (sMailInfo.find("1 ")==std::string::npos)	
				continue;

			const tstring sMailId(sMailInfo.substr(2));
			if (sMailId.size()<10 && IsFullNumberX(sMailId.c_str(),sMailId.size()))	// **只有一封邮件；
				continue;
		}
		else if (sMailInfo==".")
		{
			pOutFinished = true;
			break;
		}
		const std::string::size_type find = sMailInfo.find(' ');
		if (find!=std::string::npos)
		{
			const tstring sMailId(sMailInfo.substr(find+1));
			pOutMailIdList.push_back(sMailId);
		}		
	}

	return true;
}
inline bool TrimString(tstring& sString)
{
	trim(sString);
	std::string::size_type nFind1 = sString.find('\"');
	if (nFind1!=std::string::npos)
	{
		std::string::size_type nFind2 = sString.find('\"',nFind1+1);
		if (nFind2!=std::string::npos)
		{
			sString = sString.substr(nFind1+1,nFind2-nFind1-1);
			return true;
		}else
		{
			sString = sString.substr(1);
		}
	}
	return false;

	//if (sString.size()>=2 && sString.c_str()[0]=='"')
	//	sString = sString.substr(1,sString.size()-2);
}

const tstring & CSMTP::replace(tstring & strSource, const tstring & strFind, const tstring &strReplace)
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

inline unsigned char fromHex(const unsigned char &x)
{
	return isdigit(x) ? x-'0' : x-'A'+10;
}
//#define USES_QP_TEMP_RESULT
const tstring& QPDecode(tstring& sString)
{
#ifdef USES_QP_TEMP_RESULT
	tstring sResult;
#endif
	//CSMTP::replace(sString,"=\r\n","");
	//CSMTP::replace(sString,"=\n","");
	const size_t nSizeIn = sString.size();
	const char* pString = sString.c_str();
#ifndef USES_QP_TEMP_RESULT
	char * pOut = const_cast<char*>(pString);
	size_t nOutIndex = 0;
#endif
	for (size_t i=0; i<nSizeIn; )
	{
		char nChar = pString[i];
		if (nChar=='=' && (i+2)<=nSizeIn)
		{
			if (pString[i+1]=='\n')
			{
				if (pString[i+2]=='\n')
					i += 3;
				else
					i += 2;
				continue;
			}else if (pString[i+1]=='\r')
			{
				if (pString[i+2]=='\n')
					i += 3;
				else
					i += 2;
				continue;
			}
			nChar = (fromHex(pString[i+1])<<4);
			nChar |= fromHex(pString[i+2]);
			i += 3;
		}else
		{
			i++;
		}
#ifdef USES_QP_TEMP_RESULT
		sResult += nChar;
#else
		pOut[nOutIndex++] = nChar;
#endif
	}
#ifdef USES_QP_TEMP_RESULT
	sString = sResult;
#else
	if (nOutIndex>0 && nOutIndex<nSizeIn)
		pOut[nOutIndex] = '\0';
#endif
	return sString;
}
//int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
//{
//	int nSrc; // 源字符串的计数值
//	int nDst; // 目标解码串的计数值
//	int nByte; // 当前正在处理的组内字节的序号，范围是0-6
//	unsigned char nLeft; // 上一字节残余的数据
//
//	// 计数值初始化
//	nSrc = 0;
//	nDst = 0;
//	// 组内字节序号和残余数据初始化
//	nByte = 0;
//	nLeft = 0;
//
//	// 将源数据每7个字节分为一组，解压缩成8个字节
//	// 循环该处理过程，直至源数据被处理完
//	// 如果分组不到7字节，也能正确处理
//	while(nSrc<nSrcLength)
//	{
//		// 将源字节右边部分与残余数据相加，去掉最高位，得到一个目标解码字节
//		*pDst = ((*pSrc << nByte) | nLeft) & 0x7f;
//
//		// 将该字节剩下的左边部分，作为残余数据保存起来
//		nLeft = *pSrc >> (7-nByte);
//
//		// 修改目标串的指针和计数值
//		pDst++;
//		nDst++;
//
//		// 修改字节计数值
//		nByte++;
//
//		// 到了一组的最后一个字节
//		if(nByte == 7)
//		{
//			// 额外得到一个目标解码字节
//			*pDst = nLeft;
//
//			// 修改目标串的指针和计数值
//			pDst++;
//			nDst++;
//
//			// 组内字节序号和残余数据初始化
//			nByte = 0;
//			nLeft = 0;
//		}
//
//		// 修改源串的指针和计数值
//		pSrc++;
//		nSrc++;
//	}
//
//	// 输出字符串加个结束符
//	*pDst = '\0';
//
//	// 返回目标串长度
//	return nDst;
//}
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
void DecodeBody(CEBMailBaseInfo* pEBMailInfo, tstring& sBody)
{
	if ("quoted-printable"==pEBMailInfo->GetContentTransferEncoding())
	{
		QPDecode(sBody);
		if (pEBMailInfo->m_pAttachment.get()==NULL)
			pEBMailInfo->SetContentData1(sBody);
		else
			pEBMailInfo->m_pAttachment->SetAttachmentData1(sBody.c_str(),sBody.size());
	}else if ("base64"==pEBMailInfo->GetContentTransferEncoding() && sBody.size()>=4)
	{
		const size_t nBodySize = sBody.size();
		//const std::string::size_type find = sBody.find("\r\n\r",max(0,(((int)nBodySize)-10)));	// error
		//if (find!=std::string::npos)
		//{
		//	sBody.insert(find,'\0');
		//	nBodySize = find;
		//}else if (sBody.substr(nBodySize-2,2)=="\r\n")
		//{
		//	nBodySize -= 2;
		//}
		//std::string::size_type find = sBody.find("==",max(0,(((int)nBodySize)-10)));	// OK
		//if (find!=std::string::npos)
		//{
		//	//sBody.insert(find+2,'\0');
		//	nBodySize = find+2;
		//}else
		//{
		//	find = sBody.find("=",max(0,(((int)nBodySize)-20)));	// OK
		//	if (find!=std::string::npos)
		//	{
		//		//sBody.insert(find+1,'\0');
		//		nBodySize = find+1;
		//	}
		//}
		char * pBodyContent = new char[(nBodySize*3)/4+10];
		//memset(pBodyContent,0,(nBodySize*3)/4+10);
		const size_t nBase64DecodeSize = (size_t)Base64Decode((unsigned char*)pBodyContent,sBody.c_str(), nBodySize);
		//pBodyContent[nBase64DecodeSize] = '\0';
		//const size_t nBodyRealSize = nBodySize-((nBodySize+77)/78)*2;	// 78=76+\r\n，总长度，去掉每行二个回车符
		//const size_t nBase64DecodeSize2 = (nBodyRealSize*3)/4;//-((sBody.substr(sBody.size()-2,2)=="\r\n")?2:0);
		if (pEBMailInfo->m_pAttachment.get()==NULL)
		{
			pEBMailInfo->SetContentData3(pBodyContent, nBase64DecodeSize);
			//if (!pEBMailInfo->GetContentTypeCharset().empty() && pEBMailInfo->GetContentTypeCharset().substr(0,2)=="gb")
			//	pEBMailInfo->SetContentData3(pBodyContent, strlen(pBodyContent));
			//else
			//	pEBMailInfo->SetContentData3(pBodyContent, nBase64DecodeSize);
		}else
		{
			//char lpszBuff[260];
			//sprintf(lpszBuff,"f:\\%s",pEBMailInfo->m_pAttachment->GetName().c_str());
			//FILE * f = fopen(lpszBuff,"wb");
			//if (f!=0)
			//{
			//	fwrite(pBodyContent,1,nBase64DecodeSize,f);
			//	fclose(f);
			//}
			pEBMailInfo->m_pAttachment->SetAttachmentData2(pBodyContent,nBase64DecodeSize);
		}
	//}else if ("7bit"==pEBMailInfo->GetContentTransferEncoding())
	//{
	//	char * pBodyContent = new char[(sBody.size()*8)/7+10];
	//	memset(pBodyContent,0,sizeof(pBodyContent));
	//	gsmDecode7bit((const unsigned char*)sBody.c_str(),pBodyContent,sBody.size());
	//	pEBMailInfo->SetContentData(UTF82ACP(pBodyContent));
	//	//pEBMailInfo->SetContentData(pBodyContent);
	//	delete[] pBodyContent;
	}else if (pEBMailInfo->m_pAttachment.get()!=NULL)
	{
		pEBMailInfo->m_pAttachment->SetAttachmentData1(sBody.c_str(),sBody.size());
	}else
	{
		pEBMailInfo->SetContentData1(sBody);
	}
}
bool ParseMailHead(const char* sHeadString, tstring& pOutCharset, tstring& pOutFromName, tstring& pOutFromMail)
{
	std::vector<tstring> pList;
	const int nSize = ParseString(sHeadString,"?",pList);
	if (nSize==5)
	{
		pOutCharset = pList[1];
		std::transform(pOutCharset.begin(), pOutCharset.end(), pOutCharset.begin(), tolower);
		const tstring sEncoding(pList[2]);
		if (sEncoding=="B" || sEncoding=="b")
		{
			const size_t nDataSize = pList[3].size();
			char* lpszFromName = new char[nDataSize+1];
			memset(lpszFromName,0,nDataSize+1);
			const size_t nBase64DecodeSize = (size_t)Base64Decode((unsigned char*)lpszFromName,pList[3].c_str(), nDataSize);
			lpszFromName[nBase64DecodeSize] = '\0';
			pOutFromName = lpszFromName;
			delete[] lpszFromName;
		}else
		{
			// Quoted Printable
			pOutFromName = QPDecode(pList[3]);
		}
		pOutFromMail = pList[4];
		TrimMailName(pOutFromName);
	}else if (nSize==1)
	{
		// filename*=gb2312''2%2D%CD%D0%D3%D7%BB%FA%B9%B9%B1%A8%B1%ED%28%CE%DE2%CB%EA%B6%F9%CD%AF%29.xls
		// David Wang <fj_david@foxmail.com>
		pOutFromMail = pList[0];
		const std::string::size_type find = pOutFromMail.find("''");
		if (find!=std::string::npos)
		{
			pOutCharset = pOutFromMail.substr(0,find);
			pOutFromName = pOutFromMail.substr(find+2);
			pOutFromMail = pOutFromName;
			TrimMailName(pOutFromName);
			return true;
		}
	}else
		return false;
	std::string::size_type nFind1 = pOutFromMail.find('<');
	if (nFind1!=std::string::npos)
	{
		if (pOutFromName.empty() && nFind1>1)
		{
			pOutFromName = pOutFromMail.substr(0,nFind1);
			//trim(pOutFromName);
			TrimMailName(pOutFromName);
		}
		std::string::size_type nFind2 = pOutFromMail.find('>',nFind1+1);
		if (nFind2!=std::string::npos)
		{
			pOutFromMail = pOutFromMail.substr(nFind1+1,nFind2-nFind1-1);
		}
	}
	if (nSize==1 && pOutFromName.empty())
		pOutFromName = pOutFromMail;
	return true;
}

inline int GetStrMonthValue(const tstring& sMonth)
{
	// = "Jan" | "Feb" | "Mar" | "Apr"| "May" | "Jun" | "Jul" | "Aug" | "Sep" | "Oct" | "Nov" | "Dec" 
	if (sMonth=="Jan" || sMonth=="一月" || sMonth=="1月") return 0;
	else if (sMonth=="Feb" || sMonth=="二月" || sMonth=="2月") return 1;
	else if (sMonth=="Mar" || sMonth=="三月" || sMonth=="3月") return 2;
	else if (sMonth=="Apr" || sMonth=="四月" || sMonth=="4月") return 3;
	else if (sMonth=="May" || sMonth=="五月" || sMonth=="5月") return 4;
	else if (sMonth=="Jun" || sMonth=="六月" || sMonth=="6月") return 5;
	else if (sMonth=="Jul" || sMonth=="七月" || sMonth=="7月") return 6;
	else if (sMonth=="Aug" || sMonth=="八月" || sMonth=="8月") return 7;
	else if (sMonth=="Sep" || sMonth=="九月" || sMonth=="9月") return 8;
	else if (sMonth=="Oct" || sMonth=="十月" || sMonth=="10月") return 9;
	else if (sMonth=="Nov" || sMonth=="十一月" || sMonth=="11月") return 10;
	else return 11;
	//else if (sMonth=="Dec") return 11;
	return 0;	// ?
}
#define USES_PARSER_CONTENT_TYPE_2

//#define USES_PARSER_SUBSTR_DEBUG

CEBMailInfo::pointer ParserMailInfo(const tstring& sMailString, bool bMailMessage, bool bParserHeadOnly)
{
	CEBMailInfo::pointer pEBMailInfo = CEBMailInfo::create();
	std::string::size_type nFind1 = 0;
	std::string::size_type nReceiveGetPos = 0;
	int nFindHeadOffset = 1;
	std::string::size_type nReceiveFindHead = sMailString.find("\n");
	if (nReceiveFindHead!=std::string::npos && nReceiveFindHead>0 && sMailString.c_str()[nReceiveFindHead-1]=='\r')
	{
		nFindHeadOffset = 2;
		nReceiveFindHead-=1;
	}
#ifdef USES_PARSER_SUBSTR_DEBUG
	char lpszBuffer[102400];
#ifdef WIN32
	sprintf(lpszBuffer,"f:/email_substr_log.txt");
#else
	sprintf(lpszBuffer,"/eb/ebs/email_substr_log.txt");
#endif
	static FILE * theFile = NULL;
	if (theFile==NULL)
		theFile = fopen(lpszBuffer, "a");
	if (theFile==NULL)
		theFile = fopen(lpszBuffer, "w");

	if (theFile!=NULL)
	{
		sprintf(lpszBuffer,"************************ begin size=%d\n",(int)sMailString.size());
		fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
		fflush(theFile);
	}
	if (sMailString.size()==8476)
	{
		FILE * f = fopen("/eb/ebs/email_8476.txt", "wb");
		if (f != NULL)
		{
			fwrite(sMailString.c_str(),1,sMailString.size(),f);
			fclose(f);
		}
	}

#endif
	//static int theIndex = 0;
	//char lpszBuffer[260];
	//sprintf(lpszBuffer,"/eb/ebs/email_%d.txt",theIndex++);
	//FILE * f = fopen(lpszBuffer, "wb");
	//if (f != NULL)
	//{
	//	fwrite(sMailString.c_str(),1,sMailString.size(),f);
	//	fclose(f);
	//	return NullEBMailInfo;
	//}

	//const size_t nMailStringSize = sMailString.size();
	//std::string::size_type nReceiveFindHead = sMailString.find("\r\n");
	cgcParameter::pointer pLastParameter;
	//while (nReceiveGetPos!=std::string::npos && nReceiveFindHead>nReceiveGetPos)
	while (nReceiveFindHead!=std::string::npos)
	{
		const std::string::size_type nOldGetPos = nReceiveGetPos;
#ifdef USES_PARSER_SUBSTR_DEBUG
	if (theFile!=NULL)
	{
		sprintf(lpszBuffer,"*** nReceiveGetPos=%d,nReceiveFindHead=%d\n",(int)nReceiveGetPos,(int)nReceiveFindHead);
		fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
		fflush(theFile);
	}
#endif
		const tstring sHeadLine(sMailString.substr(nReceiveGetPos,nReceiveFindHead-nReceiveGetPos));
		nReceiveGetPos = nReceiveFindHead+nFindHeadOffset;
		nReceiveFindHead = sHeadLine.find(":");
		if (nReceiveFindHead==std::string::npos)
		{
			if (pLastParameter.get()!=NULL)
			{
				if (sHeadLine.size()>2 && (sHeadLine.substr(0,1)==" " || sHeadLine.substr(0,1)=="\t"))
				{
					pLastParameter->operator +=(CGC_VALUEINFO(sHeadLine));
					if (pLastParameter->getName()=="subject")
					{
						tstring sCharset;
						tstring sName;
						tstring sTemp;
						if (ParseMailHead(sHeadLine.c_str(), sCharset, sName, sTemp))
						{
							if (pEBMailInfo->m_pSubject.GetCharset().empty())
								pEBMailInfo->m_pSubject.SetCharset(sCharset);
							if (sCharset==pEBMailInfo->m_pSubject.GetCharset())
							{
								sTemp = pEBMailInfo->m_pSubject.GetName() + sName;
								pEBMailInfo->m_pSubject.SetName(sTemp);
							}
							//if (sCharset=="utf-8")
							//	sName = UTF82ACP(pEBMailInfo->m_pSubject.GetName().c_str());
						}
					}else if (pLastParameter->getName()=="from")
					{
						//  <digitalmagazines@msg.zinio.com>
						if (pEBMailInfo->m_pFrom.GetEmail().empty())
						{
							tstring sCharset;
							tstring sName;
							tstring sFromMail;
							if (ParseMailHead(sHeadLine.c_str(), sCharset, sName, sFromMail) && sFromMail.find("@")!=std::string::npos)
							{
								pEBMailInfo->m_pFrom.SetEmail(sFromMail);
							}
						}
					}else if (pLastParameter->getName()=="to")
					{
						std::vector<tstring> pList;
						ParseString(sHeadLine.c_str(),",",pList);
						for (size_t i=0;i<pList.size(); i++)
						{
							const tstring& sToValue = pList[i];
							tstring sCharset;
							tstring sName;
							tstring sFromMail;
							if (ParseMailHead(sToValue.c_str(), sCharset, sName, sFromMail) && sFromMail.find("@")!=std::string::npos)
							{
								CEBMailNameInfo::pointer pNameInfo = CEBMailNameInfo::create(sCharset,sName);
								pNameInfo->SetEmail(sFromMail);
								pEBMailInfo->m_pTo.push_back(pNameInfo);
							}
						}
						//tstring sCharset;
						//tstring sName;
						//tstring sFromMail;
						//if (ParseMailHead(sHeadLine.c_str(), sCharset, sName, sFromMail) && sFromMail.find("@")!=std::string::npos)
						//{
						//	if (pEBMailInfo->m_pTo.empty())
						//	{
						//		CEBMailNameInfo::pointer pNameInfo = CEBMailNameInfo::create(sCharset,sName);
						//		pNameInfo->SetEmail(sFromMail);
						//		pEBMailInfo->m_pTo.push_back(pNameInfo);
						//	}else
						//	{
						//		const size_t index = pEBMailInfo->m_pTo.size()-1;
						//		CEBMailNameInfo::pointer pNameInfo = pEBMailInfo->m_pTo[index];
						//		pNameInfo->SetEmail(sFromMail);

						//	}
						//}
					}else if (pLastParameter->getName()=="content-type")
					{
						if (pEBMailInfo->GetContentTypeCharset().empty())
						{
							tstring sHeadValue(sHeadLine);
							std::transform(sHeadValue.begin(), sHeadValue.end(), sHeadValue.begin(), tolower);
							nFind1 = sHeadValue.find("charset=");	// *nFind2
							if (nFind1!=std::string::npos)
							{
								tstring sContentTypeCharset(sHeadValue.substr(nFind1+8));
								TrimString(sContentTypeCharset);
								pEBMailInfo->SetContentTypeCharset(sContentTypeCharset);
							}
						}
					}else if (pLastParameter->getName()=="cc")
					{
						std::vector<tstring> pList;
						ParseString(sHeadLine.c_str(),",",pList);
						for (size_t i=0;i<pList.size(); i++)
						{
							const tstring& sToValue = pList[i];
							tstring sCharset;
							tstring sName;
							tstring sFromMail;
							if (ParseMailHead(sToValue.c_str(), sCharset, sName, sFromMail) && sFromMail.find("@")!=std::string::npos)
							{
								CEBMailNameInfo::pointer pNameInfo = CEBMailNameInfo::create(sCharset,sName);
								pNameInfo->SetEmail(sFromMail);
								pEBMailInfo->m_pCc.push_back(pNameInfo);
							}
						}

						//tstring sCharset;
						//tstring sName;
						//tstring sFromMail;
						//if (ParseMailHead(sHeadLine.c_str(), sCharset, sName, sFromMail) && sFromMail.find("@")!=std::string::npos)
						//{
						//	if (pEBMailInfo->m_pCc.empty())
						//	{
						//		CEBMailNameInfo::pointer pNameInfo = CEBMailNameInfo::create(sCharset,sName);
						//		pNameInfo->SetEmail(sFromMail);
						//		pEBMailInfo->m_pCc.push_back(pNameInfo);
						//	}else
						//	{
						//		const size_t index = pEBMailInfo->m_pCc.size()-1;
						//		CEBMailNameInfo::pointer pNameInfo = pEBMailInfo->m_pCc[index];
						//		pNameInfo->SetEmail(sFromMail);

						//	}
						//}
					}
				}else
				{
					pLastParameter.reset();
					//nReceiveFindHead = sHeadLine.find(":");	// X-CM-TRANSID:C8CowEAZKFPBqbdVqlkhAA--.35S2.B24906
					//if (nReceiveFindHead!=std::string::npos)
					//{
					//}
				}
			}
			if (sMailString.substr(nReceiveGetPos,2)=="\r\n" || sMailString.substr(nReceiveGetPos,1)=="\n")
			//if (sMailString.substr(nReceiveGetPos,2)=="\r\n")
			{
				pEBMailInfo->SetHead(sMailString.substr(0,nReceiveGetPos));
				if (bParserHeadOnly)
					return pEBMailInfo;
				break;
			}
			nFindHeadOffset = 1;
			nReceiveFindHead = sMailString.find("\n",nReceiveGetPos);
			if (nReceiveFindHead!=std::string::npos && nReceiveFindHead>0 && sMailString.c_str()[nReceiveFindHead-1]=='\r')
			{
				nFindHeadOffset = 2;
				nReceiveFindHead-=1;
			}
			//nReceiveFindHead = sMailString.find("\r\n",nReceiveGetPos);
			continue;
			//break;	// return false;
		}
#ifdef USES_PARSER_SUBSTR_DEBUG
		if (theFile!=NULL)
		{
			sprintf(lpszBuffer,"111 nReceiveFindHead=%d\n",(int)nReceiveFindHead);
			fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
			fflush(theFile);
		}
#endif
		tstring sHeadParam(sHeadLine.substr(0,nReceiveFindHead));
		std::transform(sHeadParam.begin(), sHeadParam.end(), sHeadParam.begin(), tolower);
		tstring sHeadValue(sHeadLine.substr(nReceiveFindHead+1));	// 2=": "
		if (!sHeadValue.empty() && sHeadValue.c_str()[0]==' ')
			sHeadValue = sHeadValue.substr(1);

		if (!pEBMailInfo->m_pHeaders.find(sHeadParam,pLastParameter))
		{
			pLastParameter = CGC_PARAMETER(sHeadParam,sHeadValue);
			pEBMailInfo->m_pHeaders.insert(sHeadParam,pLastParameter);
		}else
		{
			pLastParameter->operator +=(CGC_VALUEINFO(sHeadValue));
		}
		if ("content-type"==sHeadParam)
		{
			std::transform(sHeadValue.begin(), sHeadValue.end(), sHeadValue.begin(), tolower);
			if (sHeadValue.find("multipart/")!=std::string::npos)
			{
//Content-Type: multipart/alternative; BOUNDARY="=_Part_299476_137018032.1350739565566"
				nFind1 = sMailString.find("boundary=",nOldGetPos);	// *nFind2
				if (nFind1==std::string::npos)
					nFind1 = sMailString.find("BOUNDARY=",nOldGetPos);	// *nFind2
				if (nFind1==std::string::npos)
				{
					tstring sStringTemp(sMailString);
					std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), tolower);
					nFind1 = sStringTemp.find("boundary=",nOldGetPos);	// *nFind2
					if (nFind1==std::string::npos)
						return NullEBMailInfo;
				}
				//std::string::size_type nFind2 = sMailString.find("\r\n",nFind1+9);
				std::string::size_type nFind2 = sMailString.find("\n",nFind1+9);
				if (nFind2==std::string::npos)
					return NullEBMailInfo;
				if (nFind2>0 && sMailString.c_str()[nFind2-1]=='\r')
				{
					nFind2 -= 1;
					nReceiveGetPos = nFind2+2;
				}else
					nReceiveGetPos = nFind2+1;
#ifdef USES_PARSER_SUBSTR_DEBUG
				if (theFile!=NULL)
				{
					sprintf(lpszBuffer,"222 nFind1=%d,nFind2=%d\n",(int)nFind1,(int)nFind2);
					fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
					fflush(theFile);
				}
#endif
				tstring sBoundaryTemp(sMailString.substr(nFind1+9,nFind2-nFind1-9));
				TrimString(sBoundaryTemp);
				pEBMailInfo->m_pBoundaryList.push_back(CEBMailBoundaryInfo::create(sBoundaryTemp));
				pLastParameter.reset();
			}else
			{
// ???1
//Content-Type: text/html;charset=gbk
//Content-Transfer-Encoding: base64
// ???2
//Content-Type: image/jpeg;
// name="IMG20160829171415.jpg"
//Content-Disposition: attachment;
// ???3
//Content-Type: Text/HTML;
//  charset="gb2312"
//Content-Transfer-Encoding: base64
#ifdef USES_PARSER_CONTENT_TYPE_2
				nFind1 = sHeadValue.find("charset=");	// *nFind2
#else
				nFind1 = sMailString.find("charset=",nOldGetPos);	// *nFind2
				if (nFind1==std::string::npos)
					nFind1 = sMailString.find("CHARSET=",nOldGetPos);	// *nFind2
				if (nFind1==std::string::npos)
				{
					tstring sStringTemp(sMailString);
					std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), tolower);
					nFind1 = sMailString.find("charset=",nOldGetPos);	// *nFind2
				}
#endif
				if (nFind1!=std::string::npos)
				{
					//std::string::size_type nFind2 = sMailString.find("\r\n",nFind1+8);
#ifndef USES_PARSER_CONTENT_TYPE_2
					std::string::size_type nFind2 = sMailString.find("\n",nFind1+8);
					if (nFind2==std::string::npos)
						return NullEBMailInfo;
					if (nFind2>0 && sMailString.c_str()[nFind2-1]=='\r')
					{
						nFind2 -= 1;
						nReceiveGetPos = nFind2+2;
					}else
						nReceiveGetPos = nFind2+1;
#endif

#ifdef USES_PARSER_SUBSTR_DEBUG
					if (theFile!=NULL)
					{
						sprintf(lpszBuffer,"333 nFind1=%d,sHeadValue=(%s)\n",(int)nFind1,sHeadValue.c_str());
						fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
						fflush(theFile);
					}
#endif
#ifdef USES_PARSER_CONTENT_TYPE_2
					tstring sContentTypeCharset(sHeadValue.substr(nFind1+8));
#else
					tstring sContentTypeCharset(sMailString.substr(nFind1+8,nFind2-nFind1-8));
#endif
					TrimString(sContentTypeCharset);
#ifndef USES_PARSER_CONTENT_TYPE_2
					std::transform(sContentTypeCharset.begin(), sContentTypeCharset.end(), sContentTypeCharset.begin(), tolower);
#endif
					pEBMailInfo->SetContentTypeCharset(sContentTypeCharset);
					pLastParameter.reset();
				}
			}
		}else if ("content-transfer-encoding"==sHeadParam)
		{
			std::transform(sHeadValue.begin(), sHeadValue.end(), sHeadValue.begin(), tolower);
			pEBMailInfo->SetContentTransferEncoding(sHeadValue);
		}else if ("subject"==sHeadParam && !sHeadValue.empty())	// 有可能为空
		{
			// =?utf-8?Q?=E3=80=90=E7=99=BE=E5=BA=A6=E4=BC=97=E6=B5=8B=E3=80=91=E4=BA=94=E6=9C=88=E5=A5=BD=E7=A4=BC=E5=A4=A7=E6=94=BE=E9=80=81=EF=BC=8Cmini=E5?==?utf-8?Q?=8D=95=E5=8F=8D=E7=AD=89=E4=BD=A0=E6=8B=BF=EF=BC=81?=
			// =?UTF-8?B?5qyi6L+O5L2/55SoIGVudGJvb3N0LmNvbSDpgq7nrrHns7vnu58=?=
			tstring sCharset;
			tstring sName;
			if (sHeadValue.substr(0,2)!="=?")
			{
				tstring sTemp;
				ParseMailHead(sHeadValue.c_str(), sCharset, sName, sTemp);
			}else
			{
				bool bFlag = false;
				std::vector<tstring> pList;
				if (sHeadValue.find("?==?")!=std::string::npos)
				{
// =?utf-8?Q?=E3=80=90=E7=99=BE=E5=BA=A6=E4=BC=97=E6=B5=8B=E3=80=91=E4=BA=94=E6=9C=88=E5=A5=BD=E7=A4=BC=E5=A4=A7=E6=94=BE=E9=80=81=EF=BC=8Cmini=E5?==?utf-8?Q?=8D=95=E5=8F=8D=E7=AD=89=E4=BD=A0=E6=8B=BF=EF=BC=81?=
					bFlag = true;
					ParseString(sHeadValue.c_str(),"?==?",pList);
				}else if (sHeadValue.find(" ")!=std::string::npos)
				{
					ParseString(sHeadValue.c_str()," ",pList);
				}else
				{
					pList.push_back(sHeadValue);
				}
				for (size_t j=0; j<pList.size(); j++)
				{
					tstring sValueTemp(pList[j]);
					if (bFlag)
					{
						if (j==0)
							sValueTemp.append("?=");
						else if (j+1==pList.size())
							sValueTemp = "=?"+pList[j];
						else
							sValueTemp = "=?"+pList[j]+"?=";
					}

					tstring sNameTemp;
					tstring sTemp;
					if (ParseMailHead(sValueTemp.c_str(), sCharset, sNameTemp, sTemp))
						sName.append(sNameTemp);
					//else
					//	return NullEBMailInfo;
				}
			}
			//if (sCharset=="utf-8")
			//	sName = UTF82ACP(sName.c_str());
			pEBMailInfo->m_pSubject.SetCharset(sCharset);
			pEBMailInfo->m_pSubject.SetName(sName);
		}else if (!sHeadValue.empty() && "from"==sHeadParam)
		{
			// =?UTF-8?B?44CK6K6h566X5py65LiT5Lia5a+86K665LmL?= =?UTF-8?B?5oCd57u05LiO57O757uf44CL6K++56iL5Zui6Zif?= <no-reply@study.163.com>
			CSMTP::replace(sHeadValue,"?==?","?= =?");
			std::vector<tstring> pList;
			if (ParseString(sHeadValue.c_str(),"?= =?",pList)>1)
			{
				tstring sCharset;
				tstring sName;
				for (size_t j=0; j<pList.size(); j++)
				{
					tstring sValueTemp(pList[j]);
					if (j==0)
						sValueTemp.append("?=");
					else if (j+1==pList.size())
						sValueTemp = "=?"+pList[j];
					else
						sValueTemp = "=?"+pList[j]+"?=";

					tstring sNameTemp;
					tstring sFromMail;
					if (ParseMailHead(sValueTemp.c_str(), sCharset, sNameTemp, sFromMail))
					{
						sName.append(sNameTemp);
						if (sFromMail.find("@")!=std::string::npos)
							pEBMailInfo->m_pFrom.SetEmail(sFromMail);
					}
				}
				pEBMailInfo->m_pFrom.SetCharset(sCharset);
				pEBMailInfo->m_pFrom.SetName(sName);
				//sName = UTF82ACP(sName.c_str());
			}else
			{
				tstring sCharset;
				tstring sName;
				tstring sFromMail;
				if (ParseMailHead(sHeadValue.c_str(), sCharset, sName, sFromMail))
				{
					pEBMailInfo->m_pFrom.SetCharset(sCharset);
					pEBMailInfo->m_pFrom.SetName(sName);
					if (sFromMail.find("@")!=std::string::npos)
						pEBMailInfo->m_pFrom.SetEmail(sFromMail);
				}
			}
		}else if (!sHeadValue.empty() && "to"==sHeadParam)
		{
			// To: <treesonhonest@163.com>,<nick@entboost.com>
			std::vector<tstring> pList;
			ParseString(sHeadValue.c_str(),",",pList);
			for (size_t i=0;i<pList.size(); i++)
			{
				const tstring& sToValue = pList[i];
				tstring sCharset;
				tstring sName;
				tstring sFromMail;
				if (ParseMailHead(sToValue.c_str(), sCharset, sName, sFromMail) && sFromMail.find("@")!=std::string::npos)
				{
					CEBMailNameInfo::pointer pNameInfo = CEBMailNameInfo::create(sCharset,sName);
					pNameInfo->SetEmail(sFromMail);
					pEBMailInfo->m_pTo.push_back(pNameInfo);
				}
			}
		}else if (!sHeadValue.empty() && "cc"==sHeadParam)
		{
			// Cc: <lzy@entboost.com>,<jhhdoctor@sina.com>
			std::vector<tstring> pList;
			ParseString(sHeadValue.c_str(),",",pList);
			for (size_t i=0;i<pList.size(); i++)
			{
				const tstring& sToValue = pList[i];
				tstring sCharset;
				tstring sName;
				tstring sFromMail;
				if (ParseMailHead(sToValue.c_str(), sCharset, sName, sFromMail) && sFromMail.find("@")!=std::string::npos)
				{
					CEBMailNameInfo::pointer pNameInfo = CEBMailNameInfo::create(sCharset,sName);
					pNameInfo->SetEmail(sFromMail);
					pEBMailInfo->m_pCc.push_back(pNameInfo);
				}
			}
		}else if (!sHeadValue.empty() && "date"==sHeadParam)
		{
			trim(sHeadValue);
			//sHeadValue = "Sun, 19 May 13 16:41:19 +0800";
			const char pFirstChar = sHeadValue.empty()?'0':sHeadValue.c_str()[0];
			const bool bIsFirstNumber = (pFirstChar<'0' || pFirstChar>'9')?false:true;

			// Sun, 19 May 13 16:41:19 +0800
			// 29 Aug 2013 09:17:02 +0800
			// Thu, 27 Aug 2015 10:22:25 +0800 = OK
			// Tue, 27 Oct 2015 09:59:47 +0000
			// Wed, 9 Sep 2015 07:04:40 -0400 (EDT) = 9-9 19:11
			// = "Jan" | "Feb" | "Mar" | "Apr"| "May" | "Jun" | "Jul" | "Aug" | "Sep" | "Oct" | "Nov" | "Dec" 
			// 周四, 20 八月 2015 14:48:26 中国标准时间
			// 周三, 04 11月 2015 10:43:18 中国标准时间
			char lpszWeek[20];
			int nDay = 0;
			char lpszMonth[20];
			int nYear = 0;
			int nHour = 0;
			int nMinute = 0;
			int nSecond = 0;
			char lpszTimeFlag[20];
			memset(lpszTimeFlag,0,sizeof(lpszTimeFlag));
			if (bIsFirstNumber)
				sscanf(sHeadValue.c_str(),"%d %s %d %d:%d:%d %s",&nDay,lpszMonth,&nYear,&nHour,&nMinute,&nSecond,lpszTimeFlag);
			else
				sscanf(sHeadValue.c_str(),"%s %d %s %d %d:%d:%d %s",lpszWeek,&nDay,lpszMonth,&nYear,&nHour,&nMinute,&nSecond,lpszTimeFlag);
			if (nYear<999)
				nYear+=2000;
			//strcpy(lpszTimeFlag,"-0400 xx");
			const tstring sTimeFlag(lpszTimeFlag);
			const bool bChinaLocalTime = strstr(lpszTimeFlag,"中国标准")!=NULL?true:false;
			const int nTimeFlag = bChinaLocalTime?0:atoi(lpszTimeFlag);
			struct tm pTime;
			memset(&pTime,0,sizeof(pTime));
			pTime.tm_year = nYear>1900?(nYear-1900):nYear;
			pTime.tm_mon = GetStrMonthValue(lpszMonth);
			pTime.tm_mday = nDay;
			pTime.tm_hour = nHour;
			pTime.tm_min = nMinute;
			pTime.tm_sec = nSecond;
			time_t pMailDate = mktime(&pTime);
			if (pMailDate<=0)
				pMailDate = time(0);
			else if (bChinaLocalTime || sTimeFlag=="+0000" || sTimeFlag=="GMT")
				pMailDate += 8*3600;
			else// if (!bChinaLocalTime)
				pMailDate -= ((nTimeFlag/100)-8)*60;
			const struct tm *newtime = localtime(&pMailDate);
			char lpszBuffer[64];
			sprintf(lpszBuffer,"%04d-%02d-%02d %02d:%02d:%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
			pEBMailInfo->SetDate(lpszBuffer);
			pEBMailInfo->SetDateSecond(pMailDate);
		}
#ifdef USES_PARSER_SUBSTR_DEBUG
		if (theFile!=NULL)
		{
			sprintf(lpszBuffer,"444 nReceiveGetPos=%d\n",(int)nReceiveGetPos);
			fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
			fflush(theFile);
		}
#endif
		if (sMailString.substr(nReceiveGetPos,2)=="\r\n" || sMailString.substr(nReceiveGetPos,1)=="\n")
		{
			pEBMailInfo->SetHead(sMailString.substr(0,nReceiveGetPos));
			if (bParserHeadOnly)
				return pEBMailInfo;
			break;
		}
		nFindHeadOffset = 1;
		nReceiveFindHead = sMailString.find("\n",nReceiveGetPos);
		if (nReceiveFindHead!=std::string::npos && nReceiveFindHead>0 && sMailString.c_str()[nReceiveFindHead-1]=='\r')
		{
			nFindHeadOffset = 2;
			nReceiveFindHead-=1;
		}
		//nReceiveFindHead = sMailString.find("\r\n",nReceiveGetPos);
	}
	if (bParserHeadOnly)
		return pEBMailInfo;
	if (pEBMailInfo->m_pBoundaryList.empty())
	{
		nFindHeadOffset = 4;
		nFind1 = sMailString.find("\r\n\r\n", nReceiveGetPos-2);	// ** QQ 邮件，需要先判断 \r\n\r\n，然后再判断 \n\n，否则会取到最后面内容；
		if (nFind1==std::string::npos)
		{
			nFindHeadOffset = 2;
			nFind1 = sMailString.find("\n\n", nReceiveGetPos-2);
		}

		//nFindHeadOffset = 2;
		//nFind1 = sMailString.find("\n\n", nReceiveGetPos-2);
		//if (nFind1==std::string::npos)
		//{
		//	nFindHeadOffset = 4;
		//	nFind1 = sMailString.find("\r\n\r\n", nReceiveGetPos-2);
		//}
		if (nFind1==std::string::npos)
			return bMailMessage?NullEBMailInfo:pEBMailInfo;
#ifdef USES_PARSER_SUBSTR_DEBUG
		if (theFile!=NULL)
		{
			sprintf(lpszBuffer,"555 nFind1=%d,nFindHeadOffset=%d\n",(int)nFind1,(int)nFindHeadOffset);
			fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
			fflush(theFile);
		}
#endif
		tstring sBody(sMailString.substr(nFind1+nFindHeadOffset));
		DecodeBody(pEBMailInfo.get(),sBody);
	}

	for (size_t i=0;i<pEBMailInfo->m_pBoundaryList.size(); i++)
	{
		const CEBMailBoundaryInfo::pointer pMailBoundaryInfo = pEBMailInfo->m_pBoundaryList[i];
		//const CEBMailBoundaryInfo::pointer& pMailBoundaryInfo = pEBMailInfo->m_pBoundaryList[i];	// *** 这里会异常下面有异常
		const tstring sFindBoundary = "\n--" + pMailBoundaryInfo->GetBoundary();	// 1=\r\n 2=\r\n\r\n
		std::string::size_type nFind2 = 0;
		while (true)
		{
			nFind1 = sMailString.find(sFindBoundary, nFind2);
			if (nFind1==std::string::npos)
				break;	// 后面可能还有数据；// return NullEBMailInfo;
			else if (sMailString.substr(nFind1+sFindBoundary.size(),4)=="--\r\n")	// 已经结束，没有数据
				break;	// 后面可能还有boundary 数据需要处理；return pEBMailInfo;
			int nFindBodyOffset = 4;
			std::string::size_type nFindBody = sMailString.find("\r\n\r\n",(std::string::size_type)(nFind1+sFindBoundary.size()+8));	// \r\n\r\n
			if (nFindBody==std::string::npos)
			{
//Content-Type: text/html;charset=gbk
//Content-Transfer-Encoding: base64
//PHRhYmxlIHdpZHRoPSI2MDYiIGFsaWduPSJjZW50ZXIiIGJvcmRlcj0iMCIgY2VsbHNwYWNpbmc9
				std::string::size_type nLastSearchPos = std::string::npos;
				for (int j=0;j<10;j++)
				{
					if (nLastSearchPos==std::string::npos)
						nFindBody = sMailString.find("\r\n",(std::string::size_type)(nFind1+sFindBoundary.size()+8));	// \r\n\r\n
					else
						nFindBody = sMailString.find("\r\n",nLastSearchPos);	// \r\n\r\n
					if (nFindBody==std::string::npos)
						break;

#ifdef USES_PARSER_SUBSTR_DEBUG
					if (theFile!=NULL)
					{
						sprintf(lpszBuffer,"nLastSearchPos=%d,nFind1=%d,nFindBody=%d,sFindBoundary=(%s)\n",(int)nLastSearchPos,(int)nFind1,(int)nFindBody,sFindBoundary.c_str());
						fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
						fflush(theFile);
					}
#endif
					tstring sStringTemp;
					if (nLastSearchPos==std::string::npos)
						sStringTemp = sMailString.substr(nFind1+sFindBoundary.size()+2,nFindBody-nFind1-sFindBoundary.size());
					else
						sStringTemp = sMailString.substr(nLastSearchPos,nFindBody-nLastSearchPos);
					if (sStringTemp.find(":")!=std::string::npos || (sStringTemp.size()>2 && (sStringTemp.substr(0,1)==" " || sStringTemp.substr(0,1)=="\t")))
					{
						nLastSearchPos = nFindBody+2;
						continue;
					}
					if (nLastSearchPos!=std::string::npos)
					{
						nFindBody = nLastSearchPos-2;	// ** -4用于 \r\n\r\n
						nFindBodyOffset = 2;
					}
					break;
				}
			}
			if (nFindBody==std::string::npos)
				break;
			//char lpszBuf[1024];
			//sprintf(lpszBuf,sMailString.substr(nFind1+sFindBoundary.size(),1000).c_str());
			// ** 先取出boundary head
			CEBMailBaseInfo::pointer pBoundaryBaseInfo = CEBMailBaseInfo::create();
			pMailBoundaryInfo->m_pBoundaryList.push_back(pBoundaryBaseInfo);

#ifdef USES_PARSER_SUBSTR_DEBUG
			if (theFile!=NULL)
			{
				sprintf(lpszBuffer,"AAA nFind1=%d,nFindBody=%d,sFindBoundary=(%s)\n",(int)nFind1,(int)nFindBody,sFindBoundary.c_str());
				fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
				fflush(theFile);
			}
#endif
			const tstring sBoundaryHead(sMailString.substr(nFind1+sFindBoundary.size()+2,nFindBody-nFind1-sFindBoundary.size()));	// -2
			nReceiveGetPos = 0;
			std::string::size_type nFindHead = sBoundaryHead.find("\r\n");
			bool bFindBoundary = false;
			bool bFindMessage = false;
			bool bFindMailMessage = false;
			pLastParameter.reset();
			while (nFindHead!=std::string::npos)
			{
				const std::string::size_type nOldGetPos = nReceiveGetPos;
#ifdef USES_PARSER_SUBSTR_DEBUG
				if (theFile!=NULL)
				{
					sprintf(lpszBuffer,"BBB nReceiveGetPos=%d,nFindHead=%d,sBoundaryHead=(%s)\n",(int)nReceiveGetPos,(int)nFindHead,sBoundaryHead.c_str());
					fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
					fflush(theFile);
				}
#endif
				const tstring sHeadLine(sBoundaryHead.substr(nReceiveGetPos,nFindHead-nReceiveGetPos));
				nReceiveGetPos = nFindHead+2;
				nFindHead = sHeadLine.find(":");
				if (nFindHead==std::string::npos)
				{
					if (pLastParameter.get()!=NULL)
					{
						if (sHeadLine.size()>2 && (sHeadLine.substr(0,1)==" " || sHeadLine.substr(0,1)=="\t"))
						{
							pLastParameter->operator +=(CGC_VALUEINFO(sHeadLine));
							if ((pLastParameter->getName()=="content-type" || pLastParameter->getName()=="content-disposition") &&
								pBoundaryBaseInfo->m_pAttachment.get()!=NULL && !pBoundaryBaseInfo->m_pAttachment->GetFullName())	// * fullname 处理
							//if (pLastParameter->getName()=="content-type" && pBoundaryBaseInfo->m_pAttachment.get()!=NULL)
							{
// ** 以下情况，不能处理
//Content-Transfer-Encoding: base64
//Content-Type: image/jpeg;
// name="IMG20160829171415.jpg"
//Content-Disposition: attachment;
// filename="IMG20160829171415.jpg";
// size=5631360;
// id=31;
// uuid="null";
								tstring sCharset;
								tstring sName;
								tstring sTemp;
								if (ParseMailHead(sHeadLine.c_str(), sCharset, sName, sTemp))
								{
									if (!sCharset.empty() && pBoundaryBaseInfo->m_pAttachment->GetCharset().empty())
										pBoundaryBaseInfo->m_pAttachment->SetCharset(sCharset);
									if (pBoundaryBaseInfo->m_pAttachment->GetCharset()==sCharset)
									{
										sTemp = pBoundaryBaseInfo->m_pAttachment->GetName() + sName;
										pBoundaryBaseInfo->m_pAttachment->SetName(sTemp);
									}
								}
							}
						}else
						{
							pLastParameter.reset();
//Content-Type: text/html;charset=gbk
//Content-Transfer-Encoding: base64
//PHRhYmxlIHdpZHRoPSI2MDYiIGFsaWduPSJjZW50ZXIiIGJvcmRlcj0iMCIgY2VsbHNwYWNpbmc9
							//if (nFindBody==std::string::npos)
							//	nFindBody = nReceiveGetPos-6;	// 6=4+2; 4:\r\n\r\n 2:nFindHead+2;
						}
						nFind2 = nFindBody;
						nFindHead = sBoundaryHead.find("\r\n",nReceiveGetPos);
						continue;
					}
					break;	// return false;
				}
#ifdef USES_PARSER_SUBSTR_DEBUG
				if (theFile!=NULL)
				{
					sprintf(lpszBuffer,"CCC nFindHead=%d,sHeadLine=(%s)\n",(int)nFindHead,sHeadLine.c_str());
					fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
					fflush(theFile);
				}
#endif
				tstring sHeadParam(sHeadLine.substr(0,nFindHead));
				std::transform(sHeadParam.begin(), sHeadParam.end(), sHeadParam.begin(), tolower);
				tstring sHeadValue(sHeadLine.substr(nFindHead+1));	// 2=": "
				if (!sHeadValue.empty() && sHeadValue.c_str()[0]==' ')
					sHeadValue = sHeadValue.substr(1);
				pLastParameter = CGC_PARAMETER(sHeadParam,sHeadValue);
				pBoundaryBaseInfo->m_pHeaders.insert(sHeadParam,pLastParameter);

				if ("content-type"==sHeadParam)
				{
//Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet;
//	name="=?gb2312?B?1tC5+s3iu+O9u9LX1tDQxMHEzOy5pL7f0OjH88HQse0ueGxzeA==?="
//Content-Transfer-Encoding: base64
//Content-Disposition: attachment;
//	filename="=?gb2312?B?1tC5+s3iu+O9u9LX1tDQxMHEzOy5pL7f0OjH88HQse0ueGxzeA==?="

//Content-Type: application/vnd.ms-excel;
//	name="=?gb2312?Q?2-=CD=D0=D3=D7=BB=FA=B9=B9=B1=A8=B1=ED=28=CE=DE2?=
// =?gb2312?Q?=CB=EA=B6=F9=CD=AF=29=2Exls?="

//Content-Type: image/png
//Content-Transfer-Encoding: base64
//Content-Disposition: inline
//Content-ID: <jira-generated-image-static-footer-desktop-logo-d9c29bf3-93bb-4a98-9efe-36a9498c5c41>

					std::transform(sHeadValue.begin(), sHeadValue.end(), sHeadValue.begin(), tolower);
					if (sHeadValue.find("application/")!=std::string::npos ||
						sHeadValue.find("image/")!=std::string::npos)
					{
						nFind1 = sBoundaryHead.find("name",nOldGetPos);	// *nFind2
						if (nFind1==std::string::npos)
						{
							tstring sStringTemp(sBoundaryHead);
							std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), tolower);
							nFind1 = sStringTemp.find("name",nOldGetPos);
							if (nFind1==std::string::npos)
							{
								nFindHead = sBoundaryHead.find("\r\n",nReceiveGetPos);
								continue;		// return NullEBMailInfo;
							}
						}
						nFind2 = sBoundaryHead.find("\r\n",nFind1+5);
						if (nFind2==std::string::npos)
							return NullEBMailInfo;
						nReceiveGetPos = nFind2+2;

						// ??? 这里有异常
#ifdef USES_PARSER_SUBSTR_DEBUG
						if (theFile!=NULL)
						{
							sprintf(lpszBuffer,"DDD nFind1=%d,nFind2=%d,sHeadValue=(%s),sBoundaryHead=(%s)\n",(int)nFind1,(int)nFind2,sHeadValue.c_str(),sBoundaryHead.c_str());
							fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
							fflush(theFile);
						}
#endif
						tstring sBoundaryTemp(sBoundaryHead.substr(nFind1+4,nFind2-nFind1-4));
						nFind1 = sBoundaryTemp.find("=");
						if (nFind1==std::string::npos)
						{
							nFindHead = sBoundaryHead.find("\r\n",nReceiveGetPos);
							continue;
						}
						if (pBoundaryBaseInfo->m_pAttachment.get()==NULL || !pBoundaryBaseInfo->m_pAttachment->GetFullName())
						{
							sBoundaryTemp = sBoundaryTemp.substr(nFind1+1);
							const bool bFullName = TrimString(sBoundaryTemp);

							tstring sCharset;
							tstring sName;
							tstring sTemp;
							if (ParseMailHead(sBoundaryTemp.c_str(), sCharset, sName, sTemp))
							{
								//if (sSubjectCharset=="utf-8")
								//	sSubjectName = UTF82ACP(sSubjectName.c_str());
								if (pBoundaryBaseInfo->m_pAttachment.get()==NULL)
								{
									pBoundaryBaseInfo->m_pAttachment = CEBMailAttachmentInfo::create(sCharset,sName);
									pEBMailInfo->m_pAttachmentList.push_back(pBoundaryBaseInfo->m_pAttachment);
								}else
								{
									pBoundaryBaseInfo->m_pAttachment->SetCharset(sCharset);
									pBoundaryBaseInfo->m_pAttachment->SetName(sName);
								}
								if (!sName.empty() && bFullName)
									pBoundaryBaseInfo->m_pAttachment->SetFullName(true);
							}
						}
					}else if (sHeadValue.find("multipart/")!=std::string::npos)
					{
						nFind1 = sBoundaryHead.find("boundary",nOldGetPos);	// *nFind2
						if (nFind1==std::string::npos)
							nFind1 = sBoundaryHead.find("BOUNDARY",nOldGetPos);	// *nFind2
						if (nFind1==std::string::npos)
						{
							tstring sStringTemp(sBoundaryHead);
							std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), tolower);
							nFind1 = sStringTemp.find("boundary",nOldGetPos);	// *nFind2
							if (nFind1==std::string::npos)
								return NullEBMailInfo;
						}
						nFind2 = sBoundaryHead.find("\r\n",nFind1+8);
						if (nFind2==std::string::npos)
							return NullEBMailInfo;
						nReceiveGetPos = nFind2+2;
#ifdef USES_PARSER_SUBSTR_DEBUG
						if (theFile!=NULL)
						{
							sprintf(lpszBuffer,"EEE nFind1=%d,nFind2=%d,sBoundaryHead=(%s)\n",(int)nFind1,(int)nFind2,sBoundaryHead.c_str());
							fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
							fflush(theFile);
						}
#endif
						tstring sBoundaryTemp(sBoundaryHead.substr(nFind1+8,nFind2-nFind1-8));
						nFind1 = sBoundaryTemp.find("=");
						if (nFind1!=std::string::npos)
						{
							sBoundaryTemp = sBoundaryTemp.substr(nFind1+1);
							TrimString(sBoundaryTemp);
							pEBMailInfo->m_pBoundaryList.push_back(CEBMailBoundaryInfo::create(sBoundaryTemp));
							bFindBoundary = true;
						}
					}else if (sHeadValue.find("message/")!=std::string::npos)
					{
						bFindMessage = true;
						if (sHeadValue=="message/rfc822")
							bFindMailMessage = true;
					}else
					{
						// Content-Type: text/plain; charset = "gbk"
						nFind1 = sBoundaryHead.find("charset=",nOldGetPos);
						if (nFind1!=std::string::npos)
						{
							nFind2 = sBoundaryHead.find("\r\n",nFind1+8);
							if (nFind2==std::string::npos)
								return NullEBMailInfo;
							nReceiveGetPos = nFind2+2;
#ifdef USES_PARSER_SUBSTR_DEBUG
						if (theFile!=NULL)
						{
							sprintf(lpszBuffer,"FFF nFind1=%d,nFind2=%d,sBoundaryHead=(%s)\n",(int)nFind1,(int)nFind2,sBoundaryHead.c_str());
							fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
							fflush(theFile);
						}
#endif
							tstring sContentTypeCharset(sBoundaryHead.substr(nFind1+8,nFind2-nFind1-8));
							TrimString(sContentTypeCharset);
							std::transform(sContentTypeCharset.begin(), sContentTypeCharset.end(), sContentTypeCharset.begin(), tolower);
							pBoundaryBaseInfo->SetContentTypeCharset(sContentTypeCharset);
						}else
						{
							nFind1 = sBoundaryHead.find("charset",nOldGetPos);
							if (nFind1==std::string::npos)
							{
								tstring sStringTemp(sBoundaryHead);
								std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), tolower);
								nFind1 = sStringTemp.find("charset",nOldGetPos);
							}
							if (nFind1!=std::string::npos)
							{
								nFind2 = sBoundaryHead.find("\r\n",nFind1+7);
								if (nFind2==std::string::npos)
									return NullEBMailInfo;
								nReceiveGetPos = nFind2+2;
#ifdef USES_PARSER_SUBSTR_DEBUG
								if (theFile!=NULL)
								{
									sprintf(lpszBuffer,"GGG nFind1=%d,nFind2=%d,sBoundaryHead=(%s)\n",(int)nFind1,(int)nFind2,sBoundaryHead.c_str());
									fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
									fflush(theFile);
								}
#endif
								tstring sContentTypeCharset(sBoundaryHead.substr(nFind1+7,nFind2-nFind1-7));
								nFind1 = sContentTypeCharset.find("=");
								if (nFind1!=std::string::npos)
								{
									sContentTypeCharset = sContentTypeCharset.substr(nFind1+1);
									TrimString(sContentTypeCharset);
									std::transform(sContentTypeCharset.begin(), sContentTypeCharset.end(), sContentTypeCharset.begin(), tolower);
									pBoundaryBaseInfo->SetContentTypeCharset(sContentTypeCharset);
								}
							}
						}
					}
				}else if ("content-transfer-encoding"==sHeadParam)
				{
					std::transform(sHeadValue.begin(), sHeadValue.end(), sHeadValue.begin(), tolower);
					pBoundaryBaseInfo->SetContentTransferEncoding(sHeadValue);
				//}else if ("content-description"==sHeadParam)
				//{
				}else if ("content-disposition"==sHeadParam)
				{
					std::transform(sHeadValue.begin(), sHeadValue.end(), sHeadValue.begin(), tolower);
					if (sHeadValue.find("attachment")!=std::string::npos)
					{
//Content-Disposition: attachment;
//	filename="=?gb2312?B?1tC5+s3iu+O9u9LX1tDQxMHEzOy5pL7f0OjH88HQse0ueGxzeA==?="

//Content-Disposition: attachment;
//	filename*=gb2312''2%2D%CD%D0%D3%D7%BB%FA%B9%B9%B1%A8%B1%ED%28%CE%DE2%CB%EA%B6%F9%CD%AF%29.xls
//Content-Type: application/vnd.ms-excel;
//	name="=?gb2312?Q?2-=CD=D0=D3=D7=BB=FA=B9=B9=B1=A8=B1=ED=28=CE=DE2?=
// =?gb2312?Q?=CB=EA=B6=F9=CD=AF=29=2Exls?="
//Content-Transfer-Encoding: base64

//Content-Type: application/vnd.openxmlformats-officedocument.wordprocessingml.document;
//	name="=?gb2312?B?tveyvMbz0rVJTbL6xrfK2sio0K3S6VYzXzIwMTUwOTMwZmouZG9jeA==?="
//Content-Description: =?gb2312?B?tveyvMbz0rVJTbL6xrfK2sio0K3S6VYzXzIwMTUwOTMwZmouZG9jeA==?=
//Content-Disposition: attachment; filename=
//	"=?gb2312?B?tveyvMbz0rVJTbL6xrfK2sio0K3S6VYzXzIwMTUwOTMwZmouZG9jeA==?=";
//	size=27115; creation-date="Mon, 28 Sep 2015 05:16:34 GMT";
//	modification-date="Wed, 30 Sep 2015 15:42:41 GMT"
//Content-Transfer-Encoding: base64

						const int nFileNameHeadLen = 8;
						nFind1 = sBoundaryHead.find("filename",nOldGetPos);	// *nFind2
						if (nFind1==std::string::npos)
						{
							tstring sStringTemp(sBoundaryHead);
							std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), tolower);
							nFind1 = sStringTemp.find("filename",nOldGetPos);	// *nFind2
							if (nFind1==std::string::npos)
								return NullEBMailInfo;
						}
						//nFind1 = sBoundaryHead.find("filename=",nOldGetPos);	// *nFind2
						//int nFileNameHeadLen = 9;
						//if (nFind1==std::string::npos)
						//{
						//	nFind1 = sBoundaryHead.find("filename*=",nOldGetPos);	// *nFind2
						//	if (nFind1==std::string::npos)
						//		return NullEBMailInfo;
						//	nFileNameHeadLen = 10;
						//}
						nFind2 = sBoundaryHead.find("\r\n",nFind1+nFileNameHeadLen);
						if (nFind2==std::string::npos)
							return NullEBMailInfo;
						nReceiveGetPos = nFind2+2;

#ifdef USES_PARSER_SUBSTR_DEBUG
						if (theFile!=NULL)
						{
							sprintf(lpszBuffer,"HHH nFind1=%d,nFind2=%d,nFileNameHeadLen=%d,sBoundaryHead=(%s)\n",(int)nFind1,(int)nFind2,(int)nFileNameHeadLen,sBoundaryHead.c_str());
							fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
							fflush(theFile);
						}
#endif
						tstring sBoundaryTemp(sBoundaryHead.substr(nFind1+nFileNameHeadLen,nFind2-nFind1-nFileNameHeadLen));
						nFind1 = sBoundaryTemp.find("=");
						if (nFind1==std::string::npos)
							return NullEBMailInfo;
						if (pBoundaryBaseInfo->m_pAttachment.get()==NULL || !pBoundaryBaseInfo->m_pAttachment->GetFullName())
						{
							sBoundaryTemp = sBoundaryTemp.substr(nFind1+1);
							const bool bFullName = TrimString(sBoundaryTemp);

							tstring sCharset;
							tstring sName;
							tstring sTemp;
							if (ParseMailHead(sBoundaryTemp.c_str(), sCharset, sName, sTemp))
							{
								if (pBoundaryBaseInfo->m_pAttachment.get()==NULL)
								{
									pBoundaryBaseInfo->m_pAttachment = CEBMailAttachmentInfo::create(sCharset,sName);
									pEBMailInfo->m_pAttachmentList.push_back(pBoundaryBaseInfo->m_pAttachment);
								}else
								{
									pBoundaryBaseInfo->m_pAttachment->SetCharset(sCharset);
									pBoundaryBaseInfo->m_pAttachment->SetName(sName);
								}
								if (!sName.empty() && bFullName)
									pBoundaryBaseInfo->m_pAttachment->SetFullName(true);
							}
						}
					}
				}else if ("content-id"==sHeadParam)
				{
					tstring sContentId(sHeadValue);
					std::string::size_type nFind1 = sContentId.find('<');
					if (nFind1!=std::string::npos)
					{
						std::string::size_type nFind2 = sContentId.find('>',nFind1+1);
						if (nFind2!=std::string::npos)
						{
#ifdef USES_PARSER_SUBSTR_DEBUG
							if (theFile!=NULL)
							{
								sprintf(lpszBuffer,"III nFind1=%d,nFind2=%d,sContentId=(%s)\n",(int)nFind1,(int)nFind2,sContentId.c_str());
								fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
								fflush(theFile);
							}
#endif
							sContentId = sContentId.substr(nFind1+1,nFind2-nFind1-1);
						}else
						{
							sContentId = sContentId.substr(1);
						}
					}else
					{
						trim(sContentId);
					}
					if (pBoundaryBaseInfo->m_pAttachment.get()!=NULL)
						pBoundaryBaseInfo->m_pAttachment.get()->SetContentId(sContentId);
					pBoundaryBaseInfo->SetContentId(sContentId);
					pEBMailInfo->m_pContentData.insert(sContentId,pBoundaryBaseInfo,false);
				}

				nFind2 = nFindBody;
				nFindHead = sBoundaryHead.find("\r\n",nReceiveGetPos);
			}
			if (bFindBoundary)
			{
				continue;
				//break;
			}
			//if (nFindBody==std::string::npos)
			//{
			//	//const std::string::size_type nFindBody2 = sMailString.find("\r\n",(std::string::size_type)(nFind1+sFindBoundary.size()+8));	// \r\n\r\n
			//	//const std::string::size_type nFindBody3 = sMailString.find("\r\n",(std::string::size_type)(nFind2+sFindBoundary.size()+8));	// \r\n\r\n
			//	break;
			//}
			bool bFindBoundaryEnd = true;
			nFind2 = sMailString.find(sFindBoundary,(std::string::size_type)(nFindBody+4));	// 4
			if (nFind2==std::string::npos)
			{
				nFind2 = sMailString.find("\r\n\r\n",(std::string::size_type)(nFindBody+4));	// 4
				if (nFind2==std::string::npos)
				{
					nFind2 = sMailString.size();
					//return NullEBMailInfo;
				}
				bFindBoundaryEnd = false;
			}
#ifdef USES_PARSER_SUBSTR_DEBUG
			if (theFile!=NULL)
			{
				sprintf(lpszBuffer,"JJJ nFindBody=%d,nFind2=%d,nFindBodyOffset=%d\n",(int)nFindBody,(int)nFind2,(int)nFindBodyOffset);
				fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
				fflush(theFile);
			}
#endif
			tstring sBoundaryContent(sMailString.substr(nFindBody+nFindBodyOffset,nFind2-nFindBody-nFindBodyOffset));
			if (bFindMessage)
			{
				pBoundaryBaseInfo->m_pMessageInfo = ParserMailInfo(sBoundaryContent,bFindMailMessage,false);
				if (pBoundaryBaseInfo->m_pMessageInfo.get()!=NULL)
				{
					pBoundaryBaseInfo->m_pMessageInfo->SetMailData(sBoundaryContent);
					pBoundaryBaseInfo->m_pMessageInfo->SetMailSize((int)sBoundaryContent.size());
				}
			}else
			{
				DecodeBody(pBoundaryBaseInfo.get(),sBoundaryContent);
			}
			// ** 判断后面是否还有数据
#ifdef USES_PARSER_SUBSTR_DEBUG
			if (theFile!=NULL)
			{
				sprintf(lpszBuffer,"KKK nFind2=%d,sFindBoundary=(%s)\n",(int)nFind2,sFindBoundary.c_str());
				fwrite(lpszBuffer,1,strlen(lpszBuffer),theFile);
				fflush(theFile);
			}
#endif
			if (!bFindBoundaryEnd || sMailString.substr(nFind2+sFindBoundary.size(),2)=="--")
				break;
		}
	}
	return pEBMailInfo;
}
//#define USES_WRITE_FILE_MAILID

CEBMailInfo::pointer CSMTP::SendRETRCommand(int nMailId, bool& pOutGetDataError)
{
	if (m_wsSMTPServer.get()==0)
	{
		// 若还未建立连接
		pOutGetDataError = true;
		m_sError = "Disconnect Error!";
		return NullEBMailInfo;
	}
	// 发送STAT命令
	char buf[64];
	sprintf(buf,"RETR %d\r\n",nMailId);
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)buf,strlen(buf));
	CSmtpResponseInfo::pointer pResponseInfo;
	if (!getMailResponsePOP3(5,&pResponseInfo))	// +OK 10176 octets
	{
		pOutGetDataError = true;
		m_sError = pResponseInfo.get()==NULL?"":pResponseInfo->m_sResponseMsg;
		//printf("**** getMailResponsePOP3 timeout ...\n");
		return NullEBMailInfo;
	}
//1217 octets
//X-Alimail-AntiSpam:AC=PASS;BC=-1|-1;BR=01201311R381e1;FP=0|-1|-1|-1|0|-1|-1|-1;HT=e02c03299;MF=system@alibaba-inc.com;NM=1;PH=DW;RN=1;SR=0;
//Received: from WS-web (postmaster@entboost.com[183.12.143.147]) by e02c03309.eu6 at Thu, 27 Aug 2015 10:22:25 +0800
//Date: Thu, 27 Aug 2015 10:22:25 +0800
//From:  <system@alibaba-inc.com>
//Return-Path:  <system@alibaba-inc.com>
//To:  <admin@entboost.com>
//Reply-To:  <system@alibaba-inc.com>
//Message-ID: <----EF------w.TkF$6385a833-e618-41f0-93fe-3b2e854a752a@entboost.com>
//Subject: =?UTF-8?B?5qyi6L+O5L2/55SoIGVudGJvb3N0LmNvbSDpgq7nrrHns7vnu58=?=
//X-Mailer: Alimail-Mailagent revision 2704164
//MIME-Version: 1.0
//Return-Path: <system@alibaba-inc.com>
//****//Content-Type: multipart/alternative;
//  boundary="----=ALIBOUNDARY_120042_52be2940_55de7461_518c0"
//
//------=ALIBOUNDARY_120042_52be2940_55de7461_518c0
//Content-Type: text/plain; charset="UTF-8"
//Content-Transfer-Encoding: 8bit
//
///widget/notifyFreenetcnAccount
//------=ALIBOUNDARY_120042_52be2940_55de7461_518c0
//Content-Type: text/html; charset="UTF-8"
//Content-Transfer-Encoding: 8bit
//
//<div class="__aliyun_email_body_block">/widget/notifyFreenetcnAccount</div>
//------=ALIBOUNDARY_120042_52be2940_55de7461_518c0--
//
//.


	int nMailSize = atoi(pResponseInfo->m_sResponseMsg.c_str());
	tstring sReceiveData;
	std::string::size_type nFind1 = pResponseInfo->m_sResponseMsg.find(" octets\r\n");
	if (nFind1==std::string::npos)
	{
		sReceiveData = pResponseInfo->m_sResponseMsg;
		if (!sReceiveData.empty() && sReceiveData.c_str()[0]=='\n')
			sReceiveData = sReceiveData.substr(1);
	}else if (pResponseInfo->m_sResponseMsg.size()>nFind1+9)
	{
		sReceiveData = pResponseInfo->m_sResponseMsg.substr(nFind1+9);
	}
#ifdef USES_WRITE_FILE_MAILID
#ifndef WIN32
	const bool bPrintDebug = (m_sMailId=="1tbiTxIBFFWBWKYgWgAAbY")?true:false;
	if (bPrintDebug)
	{
		char lpszBuffer[260];
		sprintf(lpszBuffer,"/eb/ebs/smtp_mail_%s_data_A_%d.txt",m_sMailId.c_str(),nMailSize);
		FILE * f = fopen(lpszBuffer,"w");
		if (f!=NULL)
		{
			fwrite(sReceiveData.c_str(),1,sReceiveData.size(),f);
			fclose(f);
		}
	}
#endif
#endif

	// 未接收完成，分多次发送；类似 163 网易邮箱
	while (nMailSize==0 || sReceiveData.size()<(size_t)nMailSize)
	//while (nMailSize==0 || sReceiveData.size()<nMailSize)
	{
		pResponseInfo.reset();
		getMailResponsePOP3(10,&pResponseInfo);
		if (pResponseInfo.get()==NULL)
		{
			// 真实邮件大小 24687
			// ? QQ某些邮件，最后没有.\r\n结束；
			const int nDataSize = (int)sReceiveData.size();
			if (nMailSize==0 || nMailSize>nDataSize)
				nMailSize = nDataSize;

			const std::string::size_type nFindStart = nDataSize<10?0:(nDataSize-8);
			const std::string::size_type nFind = sReceiveData.find("\n.\r\n",nFindStart);
			if (nFind!=std::string::npos)	// *
			{
				nMailSize = (int)(nFind+1);//nDataSize-5;
				sReceiveData.replace(nFind+1,1,"\0");
				break;
			}
			if (nMailSize>1024 && sReceiveData.substr(nMailSize-2,2)=="--")
			{
				//printf("**** getMailResponsePOP3 timeout ...\n");
				pOutGetDataError = true;
				break;
			}
			//if (nMailSize>512 && sReceiveData.substr(nMailSize-2,2)=="\r\n")
			//	break;
			pOutGetDataError = true;
			return NullEBMailInfo;
		}
		sReceiveData.append(pResponseInfo->m_sResponseMsg);
		const std::string::size_type nFindStart = sReceiveData.size()<10?0:(sReceiveData.size()-8);
		const std::string::size_type nFind = sReceiveData.find("\n.\r\n",nFindStart);
		if (nFind!=std::string::npos)	// *
		{
			if (nMailSize==0 || nMailSize>(int)sReceiveData.size())
				nMailSize = (int)(nFind+1);//sReceiveData.size()-5;
			sReceiveData.replace(nFind+1,1,"\0");
			break;
		}
	}
#ifdef USES_WRITE_FILE_MAILID
#ifndef WIN32
	if (bPrintDebug)
	{
		char lpszBuffer[260];
		sprintf(lpszBuffer,"/eb/ebs/smtp_mail_%s_data_B_%d.txt",m_sMailId.c_str(),nMailSize);
		FILE * f = fopen(lpszBuffer,"w");
		if (f!=NULL)
		{
			fwrite(sReceiveData.c_str(),1,sReceiveData.size(),f);
			fclose(f);
		}
	}
#endif
#endif

//Received: from =?UTF-8?B?572R5piT5omL5py65Y+356CB6YKu566x?= (unknown [192.168.194.219])
//	by mfast8 (Coremail) with SMTP id t8CowEAp82S7_plVtoD0Jg--.63537S2;
//	Mon, 06 Jul 2015 12:06:19 +0800 (CST)
//****//From: =?UTF-8?B?572R5piT5omL5py65Y+356CB6YKu566x?= <phone@service.netease.com>
//Sender: phone@service.netease.com
//To: 15813311144@163.com
//Message-ID: <157656826.4464719.1436155579914.JavaMail.phone@service.netease.com>
//****//Subject: =?GBK?B?xPq1xM340tfK1rv6usXC69PKz+S8pLvus8m5pg==?=
//MIME-Version: 1.0
//****//Content-Type: text/html; charset=GBK
//****//Content-Transfer-Encoding: quoted-printable
//X-CM-TRANSID:t8CowEAp82S7_plVtoD0Jg--.63537S2
//X-Coremail-Antispam: 1UD129KBjvdXoW7JF4rCw47uw4rtw13AF1UKFg_yoW3GFX_Zr
//	18JFyIqF15A34UAwn5Jr9rWr1aqa95taya9FyxWF4vyFsYv3Wvk3yUu3y5G3y5tw1Ikr4F
//	qr1vkFW09392yjkaLaAFLSUrUUUjbb8apTn2vfkv8UJUUUU8Yxn0WfASr-VFAUDa7-sFnT
//	9fnUUIcSsGvfJTRUUUbsx2ocxC64kIII0Yj41l84x0c7CEw4AK67xGY2AK021ln4kS14v2
//	6Fy26r43JwAS0I0E0xvYzxvE52x082IY62kv0487Mc02F40Eb4A2aVAYj7AlexkEI4k267
//	I2xxyl5I8CrVAvF7xS0VCYjI8vzVWUXwAqx4xG64x03VAFz4kmzcxKxVAvF2CEx7xS67AK
//	xVWUtwAqx4xG64xvF2IEw4CE5I8CrVC2j2Wl5I8CrVC2j2CEjI02ccxYII8I67AEr4CY67
//	k08wAqx4xG6I80eVAI3cIFxVC2b4A2awAqx4xG6I8Ex7xG6cxCeVC2j2CE14v26r43McIj
//	6x8ErcxFaVAv8VWrMcIj6xkF7I0En7xvr7AKxVW8JVWxJwAm72CE4IkC6x0Yz7v_Jr0_Gr
//	1lF7xvr2IY64vIr41lFcxC0VAqx4xG64AKrs4lw4AK0VCY07AIYIkI8VC2zVAC3wAKzVCj
//	r7xvwVAFz4v204v26I0v724l7I0Y6sxI4wCY1x0262kKe7AKxVW3AVW8Zr1lc7CjxVAKzI
//	0EY4vE52x082I5MxkIecxEwVCm-wCY0x0Ix7I2Y4AK64vIr41l42xK82IYc2Ij64vIr41l
//	42xK82IY6x8ErcxFaVAv8VWrMxCjnVAqn7xvrwCFx2IqxVCFs4IE7xkEbVWUJVW8JwCFI7
//	km07C267AKxVWrJr1j6s0q3s1lx2IqxVAqx4xG67AKxVWUGVWUWwC20s026x8GjcxK67AK
//	xVWUJVWUGwC2zVAF1VAY17CE14v26r1Y6r17MIIYrxkI7VAKI48JYxBIdaVFxhVjvjDU0x
//	ZFpf9x0zNeHqUUUUUU=
//X-Originating-IP: [192.168.194.219]
//****//Date: Mon, 6 Jul 2015 12:06:19 +0800 (CST)
//
//****//<table width=3D"606" align=3D"center" border=3D"0" cellspacing=3D"0" cellpa=
//dding=3D"0" background=3D"http://mimg.127.net/xm/mail_res/common/bg.png" st=
//yle=3D"font-size:14px">
//  <tbody>
//    <tr>
//      <td colspan=3D"3" style=3D"line-height:0">
//        <table width=3D"606" height=3D"66" background=3D"http://mimg.127.ne=
//t/xm/mail_res/common/top.png"  cellpadding=3D"0" cellspacing=3D"0" border=
//=3D"0">
//          <tr>
//            <td width=3D"46"> </td>
//            <td height=3D"61" valign=3D"top" style=3D"line-height:0">
//              <table cellpadding=3D"0" cellspacing=3D"0" border=3D"0" heigh=
//t=3D"61">
//                <tr>
//                  <td height=3D"25"> </td>
//                  <td> </td>
//                  <td> </td>
//                  <td> </td>
//                </tr>
//                <tr>
//                  <td valign=3D"top" width=3D"106"><a href=3D"http://mail.1=
//63.com/" target=3D"_blank"><img src=3D"http://mimg.127.net/logo/163logo-s.g=
//if" alt=3D"163=CD=F8=D2=D7=C3=E2=B7=D1=D3=CA" border=3D"0"/></a></td>
//                  <td valign=3D"top" width=3D"106"><a href=3D"http://www.12=
//6.com/" target=3D"_blank"><img src=3D"http://mimg.127.net/logo/126logo-s.gi=
//f" alt=3D"126=CD=F8=D2=D7=C3=E2=B7=D1=D3=CA" border=3D"0"/></a></td>
//                  <td valign=3D"top" width=3D"106"><a href=3D"http://www.ye=
//ah.net/" target=3D"_blank"><img src=3D"http://mimg.127.net/logo/yeahlogo-s.=
//gif" alt=3D"yeah.net=CD=F8=D2=D7=C3=E2=B7=D1=D3=CA" border=3D"0"/></a></td>
//                  <td> </td>
//                </tr>
//              </table>
//            </td>
//            <td width=3D"46"> </td>
//          </tr>
//        </table>
//      </td>
//    </tr>
//
//    <tr>
//      <td colspan=3D"3" align=3D"center"><img src=3D"http://mimg.127.net/xm=
///mail_res/120918_sjyx/h.jpg" alt=3D"" style=3D"display:block;margin:auto"/>=
//</td>
//    </tr>
//
//=09<tr>
//=09=09<td width=3D"46"> </td>
//=09=09<td width=3D"514" style=3D"line-height:1.666">
//=09=09=09<p style=3D"font-size:14px;margin:20px 0 25px;text-align:center"><=
//img src=3D"http://mimg.127.net/xm/mail_res/120918_sjyx/suc.png" alt=3D"" al=
//ign=3D"absmiddle" /> <strong>=C4=FA=B5=C4=CD=F8=D2=D7=CA=D6=BB=FA=BA=C5=C2=
//=EB=D3=CA=CF=E4=BC=A4=BB=EE=B3=C9=B9=A6=A3=A1</strong></p>
//=09=09=09<p style=3D"font-size:14px;margin:20px 0"><strong>=D7=F0=BE=B4=B5=
//=C4=D3=C3=BB=A7=A3=BA</strong></p>
//=09=09=09<p style=3D"font-size:14px;margin:20px 0">=A1=A1=A1=A1=CF=D6=D4=DA=
//=C4=FA=BE=CD=C4=DC=CA=B9=D3=C3 <strong><a>15813311144@163.com</a></strong> =
//=D5=E2=B8=F6=D3=CA=CF=E4=C0=B4=CA=D5=B7=A2=D3=CA=BC=FE=C1=CB=A1=A3=CE=AA=C1=
//=CB=B7=BD=B1=E3=BA=C3=D3=D1=D6=AA=B5=C0=D5=E2=CA=C7=C4=FA=B5=C4=D3=CA=CF=E4=
//=A3=AC=C7=EB=BE=A1=BF=EC=C9=E8=D6=C3=C4=FA=B5=C4=EA=C7=B3=C6=A1=A3</p>
//=09=09=09<p style=3D"font-size:14px;margin:20px 0">=A1=A1=A1=A1<a sys=3D"1"=
// interface=3D"OptionInterface" param=3D"options.SenderModule" method=3D"ent=
//ry" href=3D"http://email.163.com/" target=3D"_blank"><img border=3D"0" src=
//=3D"http://mimg.127.net/xm/mail_res/120918_sjyx/btn1.png" alt=3D"=C2=ED=C9=
//=CF=C9=E8=D6=C3"/></a></p>
//=09=09=09=09=09=09=09=09=09<p style=3D"font-size:14px;margin:20px 0">=A1=A1=
//=A1=A1=CE=AA=C1=CB=B7=BD=B1=E3=C4=FA=CA=B9=D3=C3=D3=CA=CF=E4=A3=AC=BD=A8=D2=
//=E9=C4=FA=B0=B2=D7=B0 <strong>=D3=CA=CF=E4=B4=F3=CA=A6</strong> =A3=AC=B2=
//=BB=BD=F6=C4=DC=CB=E6=CA=B1=CB=E6=B5=D8=CA=D5=B7=A2=D3=CA=BC=FE=A3=AC=BB=B9=
//=D3=D0=D7=EE=BF=EC=B5=C4=D0=C2=D3=CA=BC=FE=C3=E2=B7=D1=CC=E1=D0=D1=B5=C8=B9=
//=A6=C4=DC=C5=B6=A3=A1</p>
//=09=09=09<p style=3D"font-size:14px;margin:20px 0">=A1=A1=A1=A1<a href=3D"h=
//ttp://mail.163.com/client/dl.html?from=3Dmail13" target=3D"_blank"><img bor=
//der=3D"0" src=3D"http://mimg.127.net/xm/mail_res/120918_sjyx/btn_dl.png" al=
//t=3D"=C1=A2=BC=B4=CF=C2=D4=D8"/></a></p>
//=09=09=09<div style=3D"margin:30px 0 0;border-top:1px dotted #838383;height=
//:1px;overflow:hidden"></div>
//=09=09=09<p style=3D"font-size:12px;margin:20px 0 10px">=D7=F7=CE=AA=B6=D4=
//=C4=FA=B5=C4=B4=F0=D0=BB=A3=AC=CE=D2=C3=C7=D4=F9=CB=CD=C4=FA=D2=D4=CF=C2=B7=
//=FE=CE=F1=A3=BA</p>
//=09=09=09<div style=3D"border:1px solid #E3E7EA;background:#FCFDFE">
//=09=09=09=09<table cellpadding=3D"0" cellspacing=3D"0" border=3D"0" style=
//=3D"font-size:12px;color:#999">
//=09=09=09=09=09=09=09=09=09=09<tr>
//=09=09=09=09=09=09<td style=3D"padding:20px 5px 20px 20px">
//=09=09=09=09=09=09=09<img src=3D"http://mimg.127.net/xm/mail_res/120918_sjy=
//x/ico1.png" alt=3D""/>
//=09=09=09=09=09=09</td>
//=09=09=09=09=09=09<td style=3D"padding:20px 20px 20px 10px">
//=09=09=09=09=09=09=09<p style=3D"margin:8px 0"><strong style=3D"color:#207F=
//C6">=CD=F8=C5=CC=C8=DD=C1=BF=BF=C9=C9=FD=BC=B6=B5=BD5G</strong></p>
//=09=09=09=09=09=09=09<p style=3D"margin:8px 0">=CE=C4=BC=FE=BF=C9=D3=C0=BE=
//=C3=B1=A3=B4=E6=A3=AC=C9=CF=B4=AB=CA=B1=B5=A5=B8=F6=CE=C4=BC=FE=D7=EE=B4=F3=
//=CE=AA1G=A1=A3<a href=3D"http://count.mail.163.com/statistics/inf76s.do" ta=
//rget=3D"_blank" sys=3D"1" interface=3D"OptionInterface" param=3D"optionOutL=
//ink.mobile_set"><img align=3D"absmiddle" src=3D"http://mimg.127.net/xm/mail=
//_res/120918_sjyx/btn3.png" alt=3D"=C2=ED=C9=CF=C9=FD=BC=B6"></a></p>
//=09=09=09=09=09=09</td>
//=09=09=09=09=09</tr>
//=09=09=09=09=09=09=09=09=09=09<tr>
//=09=09=09=09=09=09<td style=3D"padding:20px 5px 20px 20px">
//=09=09=09=09=09=09=09<img src=3D"http://mimg.127.net/xm/mail_res/120918_sjy=
//x/ico2.png" alt=3D""/>
//=09=09=09=09=09=09</td>
//=09=09=09=09=09=09<td style=3D"padding:20px 20px 20px 10px">
//=09=09=09=09=09=09=09<p style=3D"margin:8px 0"><strong style=3D"color:#207F=
//C6">=C3=E2=B7=D1=B6=CC=D0=C5=CC=E1=D0=D1</strong></p>
//=09=09=09=09=09=09=09<p style=3D"margin:8px 0"><strong><a>15813311144@163.c=
//om</a></strong>=CA=D5=B5=BD=B5=C4=D3=CA=BC=FE=BD=AB=D3=D0=C3=E2=B7=D1=B6=CC=
//=D0=C5=CC=E1=D0=D1=A1=A3</p>
//=09=09=09=09=09=09</td>
//=09=09=09=09=09</tr>
//=09=09=09=09=09<tr>
//=09=09=09=09=09=09<td style=3D"padding:20px 5px 20px 20px">
//=09=09=09=09=09=09=09<img src=3D"http://mimg.127.net/xm/mail_res/120918_sjy=
//x/ico3.png" alt=3D""/>
//=09=09=09=09=09=09</td>
//=09=09=09=09=09=09<td style=3D"padding:20px 20px 20px 10px">
//=09=09=09=09=09=09=09<p style=3D"margin:8px 0"><strong style=3D"color:#207F=
//C6">=D3=C5=CF=C8=C8=CB=B9=A4=BF=CD=B7=FE</strong></p>
//=09=09=09=09=09=09=09<p style=3D"margin:8px 0">=D3=C3<strong><a>15813311144=
//</a></strong>=B2=A6=B4=F2=CD=F8=D2=D7=D3=CA=CF=E4=BF=CD=B7=FE=B5=E7=BB=B0=
//=A3=AC=C4=FA=B5=C4=C0=B4=B5=E7=BD=AB=B1=BB=D3=C5=CF=C8=BD=D3=CC=FD=A1=A3</p=
//>
//=09=09=09=09=09=09</td>
//=09=09=09=09=09</tr>
//=09=09=09=09=09<tr>
//=09=09=09=09=09=09<td style=3D"padding:20px 5px 20px 20px">
//=09=09=09=09=09=09=09<img src=3D"http://mimg.127.net/xm/mail_res/120918_sjy=
//x/ico4.png" alt=3D""/>
//=09=09=09=09=09=09</td>
//=09=09=09=09=09=09<td style=3D"padding:20px 20px 20px 10px">
//=09=09=09=09=09=09=09<p style=3D"margin:8px 0"><strong style=3D"color:#207F=
//C6">=B6=E0=B1=B6=BB=FD=B7=D6</strong></p>
//=09=09=09=09=09=09=09<p style=3D"margin:8px 0">=CA=B9=D3=C3=CA=D6=BB=FA=BA=
//=C5=C2=EB=D3=CA=CF=E4WEB=B5=C7=C2=BC=BA=CD=CA=D5=B7=A2=D3=CA=BC=FE=BB=F1=B5=
//=C3=B5=C4=BB=FD=B7=D6=CA=C7=B7=C7=CA=D6=BB=FA=BA=C5=C2=EB=D3=CA=CF=E4=B5=C4=
//6=B1=B6=A1=A3</p>
//=09=09=09=09=09=09</td>
//=09=09=09=09=09</tr>
//=09=09=09=09</table>
//=09=09=09</div>
//=09=09=09<p style=3D"font-size:12px;margin:10px 0 20px;color:#999">=C8=E7=
//=B9=FB=C4=FA=D3=D0=C8=CE=BA=CE=CE=CA=CC=E2=BA=CD=BD=A8=D2=E9=A3=AC=C7=EB=CD=
//=A8=B9=FD <a href=3D"http://help.mail.163.com/feedback.do?m=3Dadd&categoryN=
//ame=3D%e6%89%8b%e6%9c%ba%e5%8f%b7%e7%a0%81%e9%82%ae%e7%ae%b1" style=3D"colo=
//r:#039">=B0=EF=D6=FA=D6=D0=D0=C4</a> =C1=AA=CF=B5=CE=D2=C3=C7=A1=A3</p>
//=09=09</td>
//=09=09<td width=3D"46"> </td>
//=09</tr>
//
//    <tr>
//      <td height=3D"10" colspan=3D"3"> </td>
//    </tr>
//    <tr>
//      <td colspan=3D"3">
//        <table width=3D"606" cellpadding=3D"0" cellspacing=3D"0" border=3D"=
//0">
//          <tr>
//            <td width=3D"46"> </td>
//            <td height=3D"24" style=3D"text-align:right;font-size:14px;line=
//-height:20px">=CD=F8=D2=D7=D3=CA=BC=FE=D6=D0=D0=C4</td>
//            <td width=3D"46"> </td>
//          </tr>
//        </table>
//      </td>
//    </tr>
//
//    <tr>
//      <td height=3D"20" colspan=3D"3" background=3D"http://mimg.127.net/xm/=
//mail_res/common/bottom.jpg"> </td>
//    </tr>
//  </tbody>
//</table>

//3333333333333333333333333333333
//From: Postmaster@163.com
//To: 15813311144@163.com
//Subject: =?gb2312?B?z7XNs83L0MU=?=
//MIME-Version: 1.0
//Content-Type: Multipart/report;
//  report-type=delivery-status;
//  boundary="------------Boundary-00=_A4D79LETZLFHNGCAMY50"
//Message-Id: <55B7962A.07A4C6.04348@163smtp14>
//Date: Tue, 28 Jul 2015 22:48:10 +0800 (CST)
//Delivered-To: mail1@entboost.commail1@entboost.com
//X-CM-Original-Message-ID: <55B79628.07A4BF.04348@m12-18.163.com>
//X-Mailer: Coremail MTA server
//X-CM-TRANSID:EsCowEDZokEmlrdVBrENAA--.4526S2.B27461
//
//
//
//--------------Boundary-00=_A4D79LETZLFHNGCAMY50
//MIME-Version: 1.0
//Content-Type: Multipart/Alternative;
//  boundary="------------Boundary-00=_A4D7Z6MTZLFHNGCAMY50"
//
//
//--------------Boundary-00=_A4D7Z6MTZLFHNGCAMY50
//Content-Type: Text/Plain;
//  charset="gb2312"
//Content-Transfer-Encoding: base64
//Content-Description: Notification
//
//safHuKOsxPq1xNPKvP6xu83Lu9jAtMHLoa2hrQ0KICAgICAg1K3Tyrz+0MXPoqO6DQogICAgICAg
//ICAgICDKsSAgvOQgMjAxNS0wNy0yOCAyMjo0ODowOCAgDQogICAgICAgICAgICDW9yAgzOIgxOO6
//w3N1YmplY3QgIA0KICAgICAgICAgICAgytW8/sjLIG1haWwxQGVudGJvb3N0LmNvbSAgDQogICAg
//IA0KICAgICAgzcvQxdSt0vKjug0KICAgICAgICAgICAg1OO44qOs1eK34tPKvP6xu8rVvP7Iy77c
//vvjBy6Osv8nE3MrHttS3vcno1sPBy7raw/u1pbvy1d+5/cLLxve1yM/gudjJ6NbDoaMNCiAgICAg
//ICAgICAgINOizsTLtcP3OlNNVFAgZXJyb3IsIFJDUFQgVE86IDU1MCA1LjEuMSA8bWFpbDFAZW50
//Ym9vc3QuY29tPjogUmVjaXBpZW50IGFkZHJlc3MgcmVqZWN0ZWQ6IFVzZXIgdW5rbm93biANCg0K
//ICAgICAgvajS6b3ivva3vbC4o7oNCiAgICAgICAgICAgINPKsu7QodLXzsLcsMzhyr6jur2o0unE
//+rOiytTTw8bky/u3vcq9warPtcrVvP7Iy7vywarPtbbUt7253MDt1LGw78Tjz+vP67Dst6iwyaGj
//DQoNCi0tLS0tLS0tLS0tLS0tLS0NClRoaXMgbWVzc2FnZSBpcyBnZW5lcmF0ZWQgYnkgQ29yZW1h
//aWwuDQrE+srVtb21xMrHwLTX1CBDb3JlbWFpbCDXqNK108q8/s+1zbO1xNDFvP4uDQoNCg==
//
//--------------Boundary-00=_A4D7Z6MTZLFHNGCAMY50
//Content-Type: Text/HTML;
//  charset="gb2312"
//Content-Transfer-Encoding: base64
//Content-Description: Notification
//
//PCEtLSBzYXZlZCBmcm9tIHVybD0oMDAyMilodHRwOi8vaW50ZXJuZXQuZS1tYWlsIC0tPiANCjxo
//dG1sPiANCjxoZWFkPiANCjxtZXRhIGh0dHAtZXF1aXY9IkNvbnRlbnQtVHlwZSIgY29udGVudD0i
//dGV4dC9odG1sOyBjaGFyc2V0PWdiMjMxMiIgLz4gDQo8bWV0YSBuYW1lPSJLZXl3b3JkcyIgY29u
//dGVudD0iIiAvPiANCjxtZXRhIG5hbWU9IkRlc2NyaXB0aW9uIiBjb250ZW50PSIiIC8+IA0KPHRp
//dGxlPjwvdGl0bGU+IA0KIA0KPHN0eWxlIHR5cGU9InRleHQvY3NzIj4gDQo8IS0tDQogDQpib2R5
//LGRpdixkbCxkdCxkZCxoMSxoMixoMyxoNCxoNSxoNixwcmUsZm9ybSxmaWVsZHNldCxpbnB1dCx0
//ZXh0YXJlYSxwLGJsb2NrcXVvdGUsdGgsdGR7cGFkZGluZzowOyBtYXJnaW46MDsgfQ0KZmllbGRz
//ZXQsaW1ne2JvcmRlcjowOyB9DQp0YWJsZXtib3JkZXItY29sbGFwc2U6Y29sbGFwc2U7IGJvcmRl
//ci1zcGFjaW5nOjA7IH0NCm9sLHVse30NCmFkZHJlc3MsY2FwdGlvbixjaXRlLGNvZGUsZGZuLGVt
//LHN0cm9uZyx0aCx2YXJ7Zm9udC13ZWlnaHQ6bm9ybWFsOyBmb250LXN0eWxlOm5vcm1hbDsgfQ0K
//Y2FwdGlvbix0aHt0ZXh0LWFsaWduOmxlZnQ7IH0NCmgxLGgyLGgzLGg0LGg1LGg2e2ZvbnQtd2Vp
//Z2h0OmJvbGQ7IGZvbnQtc2l6ZToxMDAlOyB9DQpxOmJlZm9yZSxxOmFmdGVye2NvbnRlbnQ6Jyc7
//IH0NCmFiYnIsYWNyb255bXtib3JkZXI6MDsgfQ0KIA0KYTpsaW5rLGE6dmlzaXRlZHt9DQphOmhv
//dmVye30NCiANCi5CZHl7Zm9udC1zaXplOjE0cHg7IGZvbnQtZmFtaWx5OnZlcmRhbmEsQXJpYWws
//SGVsdmV0aWNhLHNhbnMtc2VyaWY7IHBhZGRpbmc6MjBweDt9DQpoMXtmb250LXNpemU6MjRweDsg
//Y29sb3I6I2NkMDAyMTsgcGFkZGluZy1ib3R0b206MzBweDt9DQpwe30NCiANCi5UYl9tV3B7Ym9y
//ZGVyOjFweCBzb2xpZCAjZGRkOyBib3JkZXItcmlnaHQ6bm9uZTsgYm9yZGVyLWJvdHRvbTpub25l
//OyB0YWJsZS1sYXlvdXQ6Zml4ZWQ7fQ0KICAgIC5UYl9tV3AgdGgsLlRiX21XcCB0ZHtib3JkZXIt
//cmlnaHQ6MXB4IHNvbGlkICNkZGQ7IGJvcmRlci1ib3R0b206MXB4IHNvbGlkICNkZGQ7IHBhZGRp
//bmc6OHB4IDRweDt9DQogICAgLlRiX21XcCB0aHtmb250LXNpemU6MTRweDsgdGV4dC1hbGlnbjpy
//aWdodDsgd2lkdGg6MTMwcHg7IGZvbnQtd2VpZ2h0OmJvbGQ7IGJhY2tncm91bmQ6I2Y2ZjZmNjsg
//Y29sb3I6IzY2Njt9DQogICAgLlRiX21XcCB0ZHtmb250LXNpemU6MTRweDsgcGFkZGluZy1sZWZ0
//OjEwcHg7IHdvcmQtYnJlYWs6YnJlYWstYWxsO30NCiANCi5UYl9taVdweyBtYXJnaW4tdG9wOi0y
//cHg7IG1hcmdpbi1sZWZ0Oi0xcHg7IGZsb2F0OmxlZnQ7IHRhYmxlLWxheW91dDpmaXhlZDt9DQog
//ICAgLlRiX21pV3AgdGgsLlRiX21pV3AgdGR7Ym9yZGVyLWxlZnQ6MXB4IHNvbGlkICNlZWU7IGJv
//cmRlci10b3A6MXB4IHNvbGlkICNlZWU7IGJvcmRlci1yaWdodDpub25lOyBib3JkZXItYm90dG9t
//Om5vbmU7IGZvbnQtc2l6ZToxMnB4O2xpbmUtaGVpZ2h0OjE4cHh9DQogICAgLlRiX21pV3AgdGh7
//d2lkdGg6NjhweDsgYmFja2dyb3VuZDojZjhmOGY4O2xpbmUtaGVpZ2h0OjE4cHh9DQogDQoudHJf
//TWl7fQ0KICAgIC50cl9NaSB0aHt9DQogICAgLnRyX01pIHRke30NCiANCi50cl9Sent9DQogICAg
//LnRyX1J6IHRoe30NCiAgICAudHJfUnogdGR7IGJhY2tncm91bmQ6I2ZmZjRmNjt9DQogICAgICAg
//IC50cl9SeiAuaW5mb1R0eyBjb2xvcjojY2QwMDIxOyBmb250LXdlaWdodDpib2xkOyBsaW5lLWhl
//aWdodDoxOHB4O30NCiAgICAgICAgLnRyX1J6IC5pbmZvRGNyeyBwYWRkaW5nLXRvcDo0cHg7IGNv
//bG9yOiM5OTk7IGxpbmUtaGVpZ2h0OjE4cHg7fQ0KIA0KLnRyX1Nye30NCiAgICAudHJfU3IgdGh7
//fQ0KICAgIC50cl9TciB0ZHtiYWNrZ3JvdW5kOiNmNGZmZjQ7fQ0KIA0KLnVsX2xzdFdwe21hcmdp
//bi1sZWZ0Oi0yMHB4O30NCi51bF9sc3R7cGFkZGluZy10b3A6MHB4OyBwYWRkaW5nLWJvdHRvbTow
//cHg7IG1hcmdpbi10b3A6NnB4OyBtYXJnaW4tYm90dG9tOjZweDt9DQoudWxfbHN0IGxpe3BhZGRp
//bmctdG9wOjNweDsgcGFkZGluZy1ib3R0b206M3B4O30NCiANCiANCiANCiANCi0tPg0KPC9zdHls
//ZT4gDQogDQo8L2hlYWQ+IA0KIA0KPGJvZHkgY2xhc3M9IkJkeSI+IA0KIA0KPGgxPrGnx7ijrMT6
//tcTTyrz+sbvNy7vYwLTBy6Gtoa08L2gxPiANCiANCjxkaXYgY2xhc3M9IkNvbiI+IA0KPHRhYmxl
//IHdpZHRoPSIxMDAlIiBib3JkZXI9IjAiIGNlbGxzcGFjaW5nPSIwIiBjZWxscGFkZGluZz0iMCIg
//Y2xhc3M9IlRiX21XcCI+IA0KICAgIA0KICAgIDwhLS0g1K3Tyrz+0MXPoiAtLT4gDQogICAgPHRy
//IGNsYXNzPSJ0cl9NaSI+IA0KICAgICAgICA8dGggbm93cmFwPtSt08q8/tDFz6KjujwvdGg+IA0K
//ICAgICAgICA8dGQgc3R5bGU9InBhZGRpbmc6MHB4OyBmb250LXNpemU6MXB4OyBsaW5lLWhlaWdo
//dDoxcHg7IG92ZXJmbG93OmhpZGRlbjsgdmVydGljYWwtYWxpZ246dG9wOyI+IA0KDQogICAgICAg
//ICAgICA8dGFibGUgd2lkdGg9IjEwMCUiIGJvcmRlcj0iMCIgY2VsbHNwYWNpbmc9IjAiIGNlbGxw
//YWRkaW5nPSIwIiBjbGFzcz0iVGJfbWlXcCI+IA0KICAgICAgICAgICAgICAgIDx0cj4gDQogICAg
//ICAgICAgICAgICAgICAgIDx0aCBub3dyYXA+yrGhobzko7o8L3RoPiANCiAgICAgICAgICAgICAg
//ICAgICAgPHRkIHN0eWxlPSJsaW5lLWhlaWdodDoxIj4yMDE1LTA3LTI4IDIyOjQ4OjA4PC90ZD4g
//DQogICAgICAgICAgICAgICAgPC90cj4gDQqhoSAgICAgICAgICAgICAgICA8dHI+IA0KICAgICAg
//ICAgICAgICAgICAgICA8dGggbm93cmFwPtb3oaHM4qO6PC90aD4gDQogICAgICAgICAgICAgICAg
//ICAgIDx0ZCBzdHlsZT0ibGluZS1oZWlnaHQ6MSI+xOO6w3N1YmplY3Q8L3RkPg0KICAgICAgICAg
//ICAgICAgIDwvdHI+IA0KICAgICAgICAgICAgICAgIDx0cj4gDQogICAgICAgICAgICAgICAgICAg
//IDx0aCBub3dyYXA+ytW8/sjLo7o8L3RoPiANCiAgICAgICAgICAgICAgICAgICAgPHRkIHN0eWxl
//PSJsaW5lLWhlaWdodDoxIj5tYWlsMUBlbnRib29zdC5jb208L3RkPiANCiAgICAgICAgICAgICAg
//ICA8L3RyPiANCiAgICAgICAgICAgICAgICA8dHIgc3R5bGU9ImRpc3BsYXk6bm9uZTsiPjwhLS0g
//IC0tPiANCiAgICAgICAgICAgICAgICAgICAgPHRoIG5vd3JhcD6zraGhy82jujwvdGg+IA0KICAg
//ICAgICAgICAgICAgICAgICA8dGQgc3R5bGU9ImxpbmUtaGVpZ2h0OjEiPnh4eDwvdGQ+IA0KICAg
//ICAgICAgICAgICAgIDwvdHI+IA0KICAgICAgICAgICAgICAgIDx0ciBzdHlsZT0iZGlzcGxheTpu
//b25lOyI+IA0KICAgICAgICAgICAgICAgICAgICA8dGggbm93cmFwPsPcoaHLzaO6PC90aD48IS0t
//ICAtLT4gDQogICAgICAgICAgICAgICAgICAgIDx0ZCBzdHlsZT0ibGluZS1oZWlnaHQ6MSI+eXl5
//PC90ZD4gDQogICAgICAgICAgICAgICAgPC90cj4gDQogICAgICAgICAgICA8L3RhYmxlPiANCiAg
//ICAgICAgICAgIDwhLS0g1K3Tyrz+0MXPosHQse0gRW5kIC0tPiANCiANCiANCiANCiANCiAgICAg
//ICAgPC90ZD4gDQogICAgPC90cj4gDQogDQogICAgPCEtLSDNy9DF1K3S8iAtLT4gDQogICAgPHRy
//IGNsYXNzPSJ0cl9SeiI+IA0KICAgICAgICA8dGggbm93cmFwPs3L0MXUrdLyo7o8L3RoPiANCiAg
//ICAgICAgPHRkPiANCiAgICAgICAgICAgIDwhLS0gd2F5aG9tZSAyMDA5LTMtOSAtLT4gDQogICAg
//ICAgICAgICA8ZGl2IGNsYXNzPSJpbmZvVHQiPtTjuOKjrNXit+LTyrz+sbvK1bz+yMu+3L74wcuj
//rL/JxNzKx7bUt73J6NbDwcu62sP7taW78tXfuf3Cy8b3tcjP4LnYyejWw6GjPC9kaXY+IA0KICAg
//ICAgICAgICAgPGRpdiBjbGFzcz0iaW5mb0RjciI+06LOxMu1w/c6U01UUCZuYnNwO2Vycm9yLCZu
//YnNwO1JDUFQmbmJzcDtUTzombmJzcDs1NTAmbmJzcDs1LjEuMSZuYnNwOyZsdDttYWlsMUBlbnRi
//b29zdC5jb20mZ3Q7OiZuYnNwO1JlY2lwaWVudCZuYnNwO2FkZHJlc3MmbmJzcDtyZWplY3RlZDom
//bmJzcDtVc2VyJm5ic3A7dW5rbm93bjwvZGl2PiANCiAgICAgICAgPC90ZD4gDQogICAgPC90cj4g
//DQogDQogICAgPCEtLSC94r72t72wuCAtLT4gDQogICAgPHRyIGNsYXNzPSJ0cl9TciI+IA0KICAg
//ICAgICA8dGggbm93cmFwPr2o0um94r72t72wuKO6PC90aD4gDQogICAgICAgIDx0ZD4gDQogICAg
//ICAgICAgICA8ZGl2IGNsYXNzPSJ1bF9sc3RXcCI+IA0KICAgICAgICAgICAgPHVsIGNsYXNzPSJ1
//bF9sc3QiPiANCiAgICAgICAgICAgICAgICA8bGk+08qy7tCh0tfOwtywzOHKvqO6vajS6cT6s6LK
//1NPDxuTL+7e9yr3Bqs+1ytW8/sjLu/LBqs+1ttS3vbncwO3UsbDvxOPP68/rsOy3qLDJoaM8L2xp
//PiANCiAgICAgICAgICAgICAgICA8bGk+yOe5+8T609DG5Mv7zcvQxc7KzOKjrLu2063P8s340tfT
//yrz+1tDQxDxhIGhyZWY9Imh0dHA6Ly9mZWVkYmFjay5tYWlsLjEyNi5jb20vYW50aXNwYW0vcmVw
//b3J0LnBocD8mQm91bmNlUmVhc29uPVNNVFAlMjZuYnNwJTNCZXJyb3IlMkMlMjZuYnNwJTNCUkNQ
//VCUyNm5ic3AlM0JUTyUzQSUyNm5ic3AlM0I1NTAlMjZuYnNwJTNCNS4xLjElMjZuYnNwJTNCJTI2
//bHQlM0JtYWlsMSU0MGVudGJvb3N0LmNvbSUyNmd0JTNCJTNBJTI2bmJzcCUzQlJlY2lwaWVudCUy
//Nm5ic3AlM0JhZGRyZXNzJTI2bmJzcCUzQnJlamVjdGVkJTNBJTI2bmJzcCUzQlVzZXIlMjZuYnNw
//JTNCdW5rbm93biZCb3VuY2VkUmNwdD1tYWlsMSU0MGVudGJvb3N0LmNvbSZDbHVzdGVySUQ9Jk9y
//Z1N1YmplY3Q9JUM0JUUzJUJBJUMzc3ViamVjdCZTZW5kRGF0ZT0xNDM4MDk0ODg4JlNlbmRlcj0x
//NTgxMzMxMTE0NCU0MDE2My5jb20mVHJhbnNJRD1Fc0Nvd0VEWm9rRW1scmRWQnJFTkFBLS0uNDUy
//NlMyLkIyNzQ2MSIgdGFyZ2V0PSJfYmxhbmsiPreiy83Ny9DFsai45jwvYT48L2xpPg0KICAgICAg
//ICAgICAgPC91bD4gDQogICAgICAgICAgICA8L2Rpdj4gICAgICAgIA0KICAgICAgICA8L3RkPiAN
//CiAgICA8L3RyPiANCg0KPC90YWJsZT4gDQogDQo8L2Rpdj4gDQogDQoNCjwhLS0gZm9vdGVyIC0t
//Pg0KPHNwYW4+DQo8YnI+LS0tLS0tLS0tLS0tLS0tLTxicj5UaGlzIG1lc3NhZ2UgaXMgZ2VuZXJh
//dGVkIGJ5IENvcmVtYWlsLjxicj7E+srVtb21xMrHwLTX1CBDb3JlbWFpbCDXqNK108q8/s+1zbO1
//xNDFvP4uPGJyPjxicj4NCjwvc3Bhbj4NCjwvYm9keT4gDQo8L2h0bWw+DQo=
//
//--------------Boundary-00=_A4D7Z6MTZLFHNGCAMY50--
//
//--------------Boundary-00=_A4D79LETZLFHNGCAMY50
//Content-Type: Message/delivery-status
//Content-Description: Delivery error report
//
//Final-Recipient: rfc822; mail1@entboost.com
//Action: failed
//Status: 5.0.0
//Diagnostic-Code: SMTP; SMTP error, RCPT TO: 550 5.1.1 <mail1@entboost.com>: Recipient address rejected: User unknown
//--------------Boundary-00=_A4D79LETZLFHNGCAMY50
//Content-Type: Message/Rfc822
//Content-Description: Undelivered Message
//Content-Transfer-Encoding: 8bit
//
//Received: from AY140529180141956327Z (unknown [114.215.181.249])
//	by smtp14 (Coremail) with SMTP id EsCowEDZokEmlrdVBrENAA--.4526S2;
//	Tue, 28 Jul 2015 22:48:08 +0800 (CST)
//From: =?utf-8?B?5p2o5a6P5bGV?= <15813311144@163.com>
//To: <mail1@entboost.com>
//Cc: 
//Date: Tue, 28 Jul 2015 14:48:08 GMT
//Subject: =?utf-8?B?5L2g5aW9c3ViamVjdA==?=
//MIME-Version: 1.0
//Content-Type: multipart/mixed; boundary=EB_MAIL_55B79628_F6D850AC_2EB141F2_
//X-CM-TRANSID:EsCowEDZokEmlrdVBrENAA--.4526S2
//Message-Id:<55B79628.07A4BF.04348@m12-18.163.com>
//X-Coremail-Antispam: 1Uf129KBjDUn29KB7ZKAUJUUUUU529EdanIXcx71UUUUU7v73
//	VFW2AGmfu7bjvjm3AaLaJ3UbIYCTnIWIevJa73UjIFyTuYvjxU6dbbDUUUU
//X-Originating-IP: [114.215.181.249]
//
//This is a multi-part message in MIME format.
//
//--EB_MAIL_55B79628_F6D850AC_2EB141F2_
//Content-Type: text/html; charset="utf-8"
//Content-Transfer-Encoding: 7Bit
//
//浣犲ソbody
//--EB_MAIL_55B79628_F6D850AC_2EB141F2_--
//
//
//--------------Boundary-00=_A4D79LETZLFHNGCAMY50--
//

/// 44444444444444444444
//X-Alimail-AntiSpam:AC=CONTINUE;BC=0.2168667|-1;FP=0|0|0|0|0|-1|-1|-1;HT=e02c03271;MF=fj_david@foxmail.com;NM=1;PH=DM;RN=1;RT=1;SC=105.6;SR=1;
//Received: from smtpbg9.qq.com(mailfrom:fj_david@foxmail.com ip:183.60.61.202)
//          by mx1.aliyun-inc.com(10.147.35.232);
//          Tue, 08 Sep 2015 17:04:47 +0800
//X-QQ-mid: esmtp33t1441703084t174t02670
//Received: from [192.168.10.105] (unknown [112.5.90.103])
//	by esmtp4.qq.com (ESMTP) with SMTP id 0
//	for <yhz@entboost.com>; Tue, 08 Sep 2015 17:04:43 +0800 (CST)
//X-QQ-SSF: 00010000000000F0F2100F00000000Z
//X-QQ-FEAT: 0YGU8PmKZ8ZbGczzkkg/HBmKXiYGxnuM1UWfzcH1F08oWheJ6VGlq/9380HmC
//	weqAF3p/eJi5PGW/1LbYPXO4WoxIfo2kAH3DA7878p8dwCRkvn6ENo3NuweWpwxa6vgcPYi
//	eSacZ3dTTJG4/IW+pdiPs093ezm8BGCdKdp2VmRcrknpbpWZHKbCzpN9gNMKvahy+CvCYgR
//	6f/KbfO6rskbah0Jf2FFsVnylVQhookV11aq6rsyQMSn7xkA4gEho4K6veD5O9BQ=
//X-QQ-GoodBg: 1
//MIME-Version: 1.0
//To: =?utf-8?Q?=E6=9D=A8=E5=AE=8F=E5=B1=95?= <yhz@entboost.com>
//From: David Wang <fj_david@foxmail.com>
//Subject:
// =?utf-8?B?562U5aSNOiDmganluINJTeS6p+WTgeWKn+iDveWPiuebuOWFs+S7i+e7jQ==?=
//Date: Tue, 8 Sep 2015 17:04:40 +0800
//In-Reply-To: <455A8638D6E7425F8910413CB861FCCF@pcPC>
//References: <455A8638D6E7425F8910413CB861FCCF@pcPC>
//Content-Type: multipart/alternative;
//	boundary="_2592BDD4-FEEE-4C94-8A00-BBB2C7775CF3_"
//X-QQ-SENDSIZE: 520
//X-QQ-Bgrelay: 1
//
//--_2592BDD4-FEEE-4C94-8A00-BBB2C7775CF3_
//Content-Transfer-Encoding: base64
//Content-Type: text/plain; charset="utf-8"
//
//5pS25Yiw77yM6LCi6LCi44CCDQoNCi0tLS0t5Y6f5aeL6YKu5Lu2LS0tLS0NCuWPkeS7tuS6ujog
//IuadqOWuj+WxlSIgPHloekBlbnRib29zdC5jb20+DQrlj5HpgIHml7bpl7Q6IOKAjjIwMTUv4oCO
//OS/igI44IDE2OjU5DQrmlLbku7bkuro6ICIxMDQ0MjczNjhAcXEuY29tIiA8MTA0NDI3MzY4QHFx
//LmNvbT4NCuS4u+mimDog5oGp5biDSU3kuqflk4Hlip/og73lj4rnm7jlhbPku4vnu40NCg0K5oKo
//5aW977yaDQogDQrmganluIPnvZHnu5zmmK/kvIHkuJpJTeengeacieS6keaWueahiOS4k+S4muaP
//kOS+m+WVhu+8jOiHtOWKm+S6juW4ruWKqeS8geS4muW/q+mAn+WuieijhemDqOe9sklN6L+Q6JCl
//57O757uf77ybDQogDQrpmYTku7bljIXmi6zvvIzmganluINJTeeahOWfuuacrOWKn+iDveS7i+e7
//je+8jFBD5a6i5oi356uv55WM6Z2i5pON5L2c5LuL57uN5ZKM5oGp5biDSU3nrqHnkIblkI7lj7Dn
//lYzpnaLku4vnu43vvJsNCiANCuaBqeW4g0lNIDIwMOS6uuS7peWGheWFjei0ue+8jOaOiOadg+eJ
//iOacrOi0ueeUqOWmguS4i++8mg0K5YWo5aWX54mI5pys77yaMy445LiH5Y2V5Y+w5pyN5Yqh5Zmo
//57uI6Lqr5o6I5p2D77yM57uI6Lqr5YWN6LS55Y2H57qn77ybDQogDQoNCiANCuaUr+aMgeWkmuen
//jee8lueoi+ivreiogFNES++8m+aWueS+v+mbhuaIkOesrOS4ieaWueW6lOeUqO+8m+W8gOaUvuWu
//ouaIt+err+a6kOegge+8mw0K5ruh6Laz5LyB5Lia5pWw5o2u5a6J5YWo5ZKM56eB5a+G5oCn77yb
//DQogDQrmm7TlpJrkv6Hmga/or7forr/pl67vvJpodHRwOi8vd3d3LmVudGJvb3N0LmNvbQ0KIA0K
//56Wd5aW977yMDQrmnajlro/lsZUgMTU4MTMzMTExNDQNCua3seWcs+W4guaBqeW4g+e9kee7nOen
//keaKgOaciemZkOWFrOWPuA0K5Zyw5Z2A77ya5rex5Zyz5a6d5a6J5Yy657+76Lqr6LevNDfljLrp
//k7bkuLDllYbliqHlpKfljqY1MTLlrqQNCueUteivne+8mjQwMC04NDAtMTE4MA0K572R56uZ77ya
//d3d3LmVudGJvb3N0LmNvbQ0K6YKu566x77yaeWh6QGVudGJvb3N0LmNvbQ==
//
//--_2592BDD4-FEEE-4C94-8A00-BBB2C7775CF3_
//Content-Transfer-Encoding: base64
//Content-Type: text/html; charset="utf-8"
//
//PGh0bWw+PGhlYWQ+PG1ldGEgaHR0cC1lcXVpdj0iQ29udGVudC1UeXBlIiBjb250ZW50PSJ0ZXh0
//L2h0bWw7IGNoYXJzZXQ9dXRmLTgiPjwvaGVhZD4KPGJvZHkgZGlyPSJsdHIiPjxkaXY+PGRpdiBz
//dHlsZT0iZm9udC1mYW1pbHk6IENhbGlicmksc2Fucy1zZXJpZjsgZm9udC1zaXplOiAxMXB0OyI+
//5pS25Yiw77yM6LCi6LCi44CCPC9kaXY+PC9kaXY+PGRpdiBkaXI9Imx0ciI+PGhyPjxzcGFuIHN0
//eWxlPSJmb250LWZhbWlseTogQ2FsaWJyaSxzYW5zLXNlcmlmOyBmb250LXNpemU6IDExcHQ7IGZv
//bnQtd2VpZ2h0OiBib2xkOyI+5Y+R5Lu25Lq6OiA8L3NwYW4+PHNwYW4gc3R5bGU9ImZvbnQtZmFt
//aWx5OiBDYWxpYnJpLHNhbnMtc2VyaWY7IGZvbnQtc2l6ZTogMTFwdDsiPjxhIGhyZWY9Im1haWx0
//bzp5aHpAZW50Ym9vc3QuY29tIj7mnajlro/lsZU8L2E+PC9zcGFuPjxicj48c3BhbiBzdHlsZT0i
//Zm9udC1mYW1pbHk6IENhbGlicmksc2Fucy1zZXJpZjsgZm9udC1zaXplOiAxMXB0OyBmb250LXdl
//aWdodDogYm9sZDsiPuWPkemAgeaXtumXtDogPC9zcGFuPjxzcGFuIHN0eWxlPSJmb250LWZhbWls
//eTogQ2FsaWJyaSxzYW5zLXNlcmlmOyBmb250LXNpemU6IDExcHQ7Ij7igI4yMDE1L+KAjjkv4oCO
//OCAxNjo1OTwvc3Bhbj48YnI+PHNwYW4gc3R5bGU9ImZvbnQtZmFtaWx5OiBDYWxpYnJpLHNhbnMt
//c2VyaWY7IGZvbnQtc2l6ZTogMTFwdDsgZm9udC13ZWlnaHQ6IGJvbGQ7Ij7mlLbku7bkuro6IDwv
//c3Bhbj48c3BhbiBzdHlsZT0iZm9udC1mYW1pbHk6IENhbGlicmksc2Fucy1zZXJpZjsgZm9udC1z
//aXplOiAxMXB0OyI+PGEgaHJlZj0ibWFpbHRvOjEwNDQyNzM2OEBxcS5jb20iPjEwNDQyNzM2OEBx
//cS5jb208L2E+PC9zcGFuPjxicj48c3BhbiBzdHlsZT0iZm9udC1mYW1pbHk6IENhbGlicmksc2Fu
//cy1zZXJpZjsgZm9udC1zaXplOiAxMXB0OyBmb250LXdlaWdodDogYm9sZDsiPuS4u+mimDogPC9z
//cGFuPjxzcGFuIHN0eWxlPSJmb250LWZhbWlseTogQ2FsaWJyaSxzYW5zLXNlcmlmOyBmb250LXNp
//emU6IDExcHQ7Ij7mganluINJTeS6p+WTgeWKn+iDveWPiuebuOWFs+S7i+e7jTwvc3Bhbj48YnI+
//PGJyPjwvZGl2Pgo8ZGl2IGRpcj0ibHRyIj4KPGRpdiBzdHlsZT0nY29sb3I6IHJnYigwLCAwLCAw
//KTsgZm9udC1mYW1pbHk6ICJDYWxpYnJpIjsgZm9udC1zaXplOiAxMnB0Oyc+CjxkaXYgc3R5bGU9
//J2NvbG9yOiByZ2IoMCwgMCwgMCk7IGZvbnQtZmFtaWx5OiAiQ2FsaWJyaSI7IGZvbnQtc2l6ZTog
//c21hbGw7IGZvbnQtc3R5bGU6IG5vcm1hbDsgZm9udC13ZWlnaHQ6IG5vcm1hbDsgdGV4dC1kZWNv
//cmF0aW9uOiBub25lOyBkaXNwbGF5OiBpbmxpbmU7Jz4KPGRpdiBkaXI9Imx0ciI+CjxkaXYgc3R5
//bGU9J2NvbG9yOiByZ2IoMCwgMCwgMCk7IGZvbnQtZmFtaWx5OiAiQ2FsaWJyaSI7IGZvbnQtc2l6
//ZTogMTJwdDsnPgo8ZGl2PuaCqOWlve+8mjwvZGl2Pgo8ZGl2PiZuYnNwOzwvZGl2Pgo8ZGl2PuaB
//qeW4g+e9kee7nOaYr+S8geS4mklN56eB5pyJ5LqR5pa55qGI5LiT5Lia5o+Q5L6b5ZWG77yM6Ie0
//5Yqb5LqO5biu5Yqp5LyB5Lia5b+r6YCf5a6J6KOF6YOo572ySU3ov5DokKXns7vnu5/vvJs8L2Rp
//dj4KPGRpdj4mbmJzcDs8L2Rpdj4KPGRpdj7pmYTku7bljIXmi6zvvIzmganluINJTeeahOWfuuac
//rOWKn+iDveS7i+e7je+8jFBD5a6i5oi356uv55WM6Z2i5pON5L2c5LuL57uN5ZKM5oGp5biDSU3n
//rqHnkIblkI7lj7DnlYzpnaLku4vnu43vvJs8L2Rpdj4KPGRpdj4mbmJzcDs8L2Rpdj4KPGRpdj7m
//ganluINJTSAyMDDkurrku6XlhoXlhY3otLnvvIzmjojmnYPniYjmnKzotLnnlKjlpoLkuIvvvJo8
//L2Rpdj4KPGRpdj7lhajlpZfniYjmnKzvvJozLjjkuIfljZXlj7DmnI3liqHlmajnu4jouqvmjojm
//nYPvvIznu4jouqvlhY3otLnljYfnuqfvvJs8L2Rpdj4KPGRpdj4mbmJzcDs8L2Rpdj4KPGRpdj48
//aW1nIHdpZHRoPSI2NDQiIGhlaWdodD0iMjcxIiB0aXRsZT0iaW1hZ2UiIHN0eWxlPSJib3JkZXI6
//IDBweCBjdXJyZW50Q29sb3I7IGJvcmRlci1pbWFnZTogbm9uZTsgcGFkZGluZy10b3A6IDBweDsg
//cGFkZGluZy1yaWdodDogMHB4OyBwYWRkaW5nLWxlZnQ6IDBweDsgZGlzcGxheTogaW5saW5lOyBi
//YWNrZ3JvdW5kLWltYWdlOiBub25lOyIgYWx0PSJpbWFnZSIgc3JjPSJjaWQ6MzIyRkJDMkRGN0I3
//NDJDNzlFNDc0M0Y1RTZCM0QxMkVAcGNQQyIgYm9yZGVyPSIwIj48L2Rpdj4KPGRpdj4mbmJzcDs8
//L2Rpdj4KPGRpdj7mlK/mjIHlpJrnp43nvJbnqIvor63oqIBTREvvvJvmlrnkvr/pm4bmiJDnrKzk
//uInmlrnlupTnlKjvvJvlvIDmlL7lrqLmiLfnq6/mupDnoIHvvJs8L2Rpdj4KPGRpdj7mu6HotrPk
//vIHkuJrmlbDmja7lronlhajlkoznp4Hlr4bmgKfvvJs8L2Rpdj4KPGRpdj4mbmJzcDs8L2Rpdj4K
//PGRpdj7mm7TlpJrkv6Hmga/or7forr/pl67vvJo8YSB0aXRsZT0iaHR0cDovL3d3dy5lbnRib29z
//dC5jb20iIGhyZWY9Imh0dHA6Ly93d3cuZW50Ym9vc3QuY29tIj5odHRwOi8vd3d3LmVudGJvb3N0
//LmNvbTwvYT48L2Rpdj4KPGRpdj4mbmJzcDs8L2Rpdj4KPGRpdj7npZ3lpb3vvIw8L2Rpdj4KPGRp
//dj7mnajlro/lsZUgMTU4MTMzMTExNDQ8L2Rpdj4KPGRpdiBzdHlsZT0ibGluZS1oZWlnaHQ6IDEu
//NzsiPgo8cCBhbGlnbj0ianVzdGlmeSIgY2xhc3M9Ik1zb05vcm1hbCIgc3R5bGU9Im1hcmdpbjog
//MHB4IDBjbTsgLW1zLXRleHQtanVzdGlmeTogYXV0bzsiPjxzcGFuPjxmb250IGZhY2U9IuWui+S9
//kyI+5rex5Zyz5biC5oGp5biD572R57uc56eR5oqA5pyJ6ZmQ5YWs5Y+4PC9mb250Pjwvc3Bhbj48
//c3BhbiBsYW5nPSJFTi1VUyI+PC9zcGFuPjwvcD48L2Rpdj4KPGRpdiBzdHlsZT0ibGluZS1oZWln
//aHQ6IDEuNzsiPgo8cCBhbGlnbj0ianVzdGlmeSIgY2xhc3M9Ik1zb05vcm1hbCIgc3R5bGU9Im1h
//cmdpbjogMHB4IDBjbTsgLW1zLXRleHQtanVzdGlmeTogYXV0bzsiPjxzcGFuPjxmb250IGZhY2U9
//IuWui+S9kyI+5Zyw5Z2A77ya5rex5Zyz5a6d5a6J5Yy657+76Lqr6LevPHNwYW4gbGFuZz0iRU4t
//VVMiPjQ3PC9zcGFuPuWMuumTtuS4sOWVhuWKoeWkp+WOpjxzcGFuIGxhbmc9IkVOLVVTIj41MTLl
//rqQ8L3NwYW4+PC9mb250Pjwvc3Bhbj48L3A+CjxwIGFsaWduPSJqdXN0aWZ5IiBjbGFzcz0iTXNv
//Tm9ybWFsIiBzdHlsZT0ibWFyZ2luOiAwcHggMGNtOyAtbXMtdGV4dC1qdXN0aWZ5OiBhdXRvOyI+
//PHNwYW4+PGZvbnQgZmFjZT0i5a6L5L2TIj7nlLXor53vvJo8c3BhbiBsYW5nPSJFTi1VUyI+NDAw
//LTg0MC0xMTgwPC9zcGFuPjwvZm9udD48L3NwYW4+PC9wPjwvZGl2Pgo8ZGl2IHN0eWxlPSJsaW5l
//LWhlaWdodDogMS43OyI+CjxwIGFsaWduPSJqdXN0aWZ5IiBjbGFzcz0iTXNvTm9ybWFsIiBzdHls
//ZT0ibWFyZ2luOiAwcHggMGNtOyAtbXMtdGV4dC1qdXN0aWZ5OiBhdXRvOyI+PHNwYW4+PGZvbnQg
//ZmFjZT0i5a6L5L2TIj7nvZHnq5nvvJo8L2ZvbnQ+PC9zcGFuPjxzcGFuIGxhbmc9IkVOLVVTIj48
//YSBocmVmPSJodHRwOi8vd3d3LmVudGJvb3N0LmNvbSI+PHNwYW4+PGZvbnQgY29sb3I9IiMwNTYz
//YzEiPnd3dy5lbnRib29zdC5jb208L2ZvbnQ+PC9zcGFuPjwvYT48L3NwYW4+PC9wPjwvZGl2Pgo8
//ZGl2IHN0eWxlPSJsaW5lLWhlaWdodDogMS43OyI+CjxwIGFsaWduPSJqdXN0aWZ5IiBjbGFzcz0i
//TXNvTm9ybWFsIiBzdHlsZT0ibWFyZ2luOiAwcHggMGNtOyAtbXMtdGV4dC1qdXN0aWZ5OiBhdXRv
//OyI+PHNwYW4+PGZvbnQgZmFjZT0i5a6L5L2TIj7pgq7nrrHvvJo8L2ZvbnQ+PC9zcGFuPjxzcGFu
//IGxhbmc9IkVOLVVTIj48YSBocmVmPSJtYWlsdG86eWh6QGVudGJvb3N0LmNvbSI+PHNwYW4+PGZv
//bnQgY29sb3I9IiMwNTYzYzEiPnloekBlbnRib29zdC5jb208L2ZvbnQ+PC9zcGFuPjwvYT48L3Nw
//YW4+PC9wPjwvZGl2PjwvZGl2PjwvZGl2PjwvZGl2PjwvZGl2PjwvZGl2PgoKPC9ib2R5PjwvaHRt
//bD4=
//
//--_2592BDD4-FEEE-4C94-8A00-BBB2C7775CF3_--
//
//
//
//.
//

//------=_NextPart_55EF9FFD_0AEFEC68_1BD0138D
//Content-Type: image/jpeg;
//	name="359C3A45@F441617A.FD9FEF55.jpg"
//Content-Transfer-Encoding: base64
//Content-ID: <359C3A45@F441617A.FD9FEF55.jpg>
// <img src="cid:359C3A45@F441617A.FD9FEF55.jpg">
// 
//
//if (sReceiveData.find("Received: from mx35.sx64.email-deliver.com (unknown [118.192.64.35])")==std::string::npos &&
//	sReceiveData.find("Received: from ct174.emailenglish.englishtown. (unknown [222.73.107.174])")==std::string::npos)
////if (sReceiveData.find("We hope you enjoyed receiving this message. ")==std::string::npos &&
////	sReceiveData.find("Facebook, Inc., Attention: Department 415, PO Box 10005, Palo Alto, CA 94303")==std::string::npos &&
////	sReceiveData.find("A big welcome to the Livecoding.tv")==std::string::npos &&
////	sReceiveData.find("感谢您一直以来对灵雀云的支持，我们将用更好的服务，与您携手一起共建未来！")==std::string::npos)
//{
//	//int i=0;
//	return NullEBMailInfo;
//}

	//if (sReceiveData.find("njuhyyb@dmas.net")!=std::string::npos)
	//{
	//	int i=0;
	//}

	replace(sReceiveData,"\r\n\n","\r\n\r\n");
	//replace(sReceiveData,"\r\n\n--","\r\n\r\n--");
	CEBMailInfo::pointer pEBMailInfo = ParserMailInfo(sReceiveData,true,false);
	if (pEBMailInfo.get()!=NULL)
	{
		if (pEBMailInfo->GetDateSecond()==0)
		{
			const time_t pMailDate = time(0);
			const struct tm *newtime = localtime(&pMailDate);
			char lpszBuffer[64];
			sprintf(lpszBuffer,"%04d-%02d-%02d %02d:%02d:%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
			pEBMailInfo->SetDate(lpszBuffer);
			pEBMailInfo->SetDateSecond(pMailDate);
		}
		pEBMailInfo->SetMailData(sReceiveData);
		pEBMailInfo->SetMailSize(nMailSize);
//
//		tstring sBody;
//		if (pEBMailInfo->IsHasContentData() && pEBMailInfo->m_pBoundaryList.empty())
//		{
//			sBody = pEBMailInfo->GetContentData();
//			if (!pEBMailInfo->GetContentTypeCharset().empty() && pEBMailInfo->GetContentTypeCharset()!="utf-8")
//			{
//#ifdef WIN32
//				sBody = ACP2UTF8(sBody.c_str());
//#else
//				entboost::XXX2UTF8(pEBMailInfo->GetContentTypeCharset().c_str(),sBody.c_str(),sBody.size(),sBody);
//#endif
//			}
//		}else if (!pEBMailInfo->m_pBoundaryList.empty())
//		{
//			for (size_t i=0;i<pEBMailInfo->m_pBoundaryList.size(); i++)
//			{
//				bool bFirstPlain = false;
//				//bool bFirstHtml = false;
//				CEBMailBoundaryInfo::pointer pMailBoundaryInfo = pEBMailInfo->m_pBoundaryList[i];
//				for (size_t j=0; j<pMailBoundaryInfo->m_pBoundaryList.size(); j++)
//				{
//					CEBMailBaseInfo::pointer pMailBaseInfo = pMailBoundaryInfo->m_pBoundaryList[j];
//					cgcParameter::pointer pContentType;
//					pMailBaseInfo->m_pHeaders.find("content-type",pContentType);
//					const tstring sContentType = pContentType.get()==0?"":pContentType->getStr();
//					const bool bTextContentType = (sContentType.find("text/")!=std::string::npos)?true:false;
//					if (!pMailBaseInfo->IsHasContentData() ||
//						(!pMailBaseInfo->GetContentId().empty() && !bTextContentType))
//					{
//						//if (pMailBaseInfo->IsHasContentData())
//						//{
//						//	char lpszBuf[260];
//						//	sprintf(lpszBuf,"f:\\mail-%d-%d-%d-content-type.txt",nMailId,i,j);
//						//	FILE * f = fopen(lpszBuf,"w");
//						//	if (f!=0)
//						//	{
//						//		fwrite(sContentType.c_str(),1,sContentType.size(),f);
//						//		fclose(f);
//						//	}
//						//}
//						continue;
//					}
//					const bool bPlain = (bTextContentType && sContentType.find("/plain")!=std::string::npos)?true:false;
//					const bool bHtml = (bTextContentType && sContentType.find("/html")!=std::string::npos)?true:false;
//					if (!pMailBaseInfo->GetContentTypeCharset().empty() && pMailBaseInfo->GetContentTypeCharset()!="utf-8")
//					{
//#ifdef WIN32
//						const tstring sBodyTemp(ACP2UTF8(pMailBaseInfo->GetContentData()));
//#else
//						tstring sBodyTemp(pMailBaseInfo->GetContentData());
//						entboost::XXX2UTF8(pMailBaseInfo->GetContentTypeCharset().c_str(),sBodyTemp.c_str(),sBodyTemp.size(),sBodyTemp);
//#endif
//						if (sBody.empty() || (bHtml && bFirstPlain))
//							sBody = sBodyTemp;
//						else
//						{
//							// <!DOCTYPE HTML PUBLIC 
//							// <html>
//							// <html 
//							tstring sBodyHead(sBodyTemp.substr(0,8));
//							std::transform(sBodyHead.begin(), sBodyHead.end(), sBodyHead.begin(), tolower);
//							if (sBodyHead.find("<!doc")!=std::string::npos || sBodyHead.find("<html")!=std::string::npos)
//							{
//								sBody = sBodyTemp;
//							}else
//							{
//								sBody.append(sBodyTemp);
//							}
//						}
//					}else
//					{
//						const tstring sBodyTemp(pMailBaseInfo->GetContentData());
//						if (sBody.empty() || (bHtml && bFirstPlain))
//							sBody = sBodyTemp;
//						else
//						{
//							tstring sBodyHead(sBodyTemp.substr(0,8));
//							std::transform(sBodyHead.begin(), sBodyHead.end(), sBodyHead.begin(), tolower);
//							if (sBodyHead.find("<!doc")!=std::string::npos || sBodyHead.find("<html")!=std::string::npos)
//								sBody = sBodyTemp;
//							else
//								sBody.append(sBodyTemp);
//						}
//					}
//					if (j==0)
//					{
//						bFirstPlain = bPlain;
//						//bFirstHtml = bHtml;
//					}
//				}
//			}
//		}
//		if (sBody.empty())
//			int k=0;
//		char lpszBuf[260];
//		sprintf(lpszBuf,"f:\\mail-%d.txt",nMailId);
//		FILE * f = fopen(lpszBuf,"w");
//		if (f!=0)
//		{
//			fwrite(sReceiveData.c_str(),1,sReceiveData.size(),f);
//			fclose(f);
//		}
//		sprintf(lpszBuf,"f:\\mail-%d.html",nMailId);
//		f = fopen(lpszBuf,"w");
//		if (f!=0)
//		{
//			fwrite(sBody.c_str(),1,sBody.size(),f);
//			fclose(f);
//		}

	//}else
	//{
	//	char lpszBuf[260];
	//	sprintf(lpszBuf,"/eb/ebs/mail-%d.txt",nMailId);
	//	FILE * f = fopen(lpszBuf,"w");
	//	if (f!=0)
	//	{
	//		fwrite(sReceiveData.c_str(),1,sReceiveData.size(),f);
	//		fclose(f);
	//	}
	//	//sprintf(lpszBuf,"f:\\mail-%d.html",nMailId);
	//	//f = fopen(lpszBuf,"w");
	//	//if (f!=0)
	//	//{
	//	//	fwrite(sBody.c_str(),1,sBody.size(),f);
	//	//	fclose(f);
	//	//}

	//	int i=0;
	}


	
	return pEBMailInfo;
//	CEBMailInfo::pointer pEBMailInfo = ParserMailInfo(sReceiveData,true,false);
//	if (pEBMailInfo.get()!=NULL)
//	{
//		tstring sBody;
//		if (pEBMailInfo->IsHasContentData() && pEBMailInfo->m_pBoundaryList.empty())
//		{
//			sBody = pEBMailInfo->GetContentData();
//		}else if (pEBMailInfo->m_pBoundaryList.empty())
//		{
//			for (size_t i=0;i<pEBMailInfo->m_pBoundaryList.size(); i++)
//			{
//				CEBMailBoundaryInfo::pointer pMailBoundaryInfo = pEBMailInfo->m_pBoundaryList[i];
//				for (size_t j=0; j<pMailBoundaryInfo->m_pBoundaryList.size(); j++)
//				{
//					CEBMailBaseInfo::pointer pMailBaseInfo = pMailBoundaryInfo->m_pBoundaryList[j];
//					if (pMailBaseInfo->IsHasContentData())
//					{
//						if (pMailBaseInfo->GetContentTypeCharset()!="utf-8")
//						{
//#ifdef WIN32
//							sBody.append(ACP2UTF8(pMailBaseInfo->GetContentData()));
//#else
//							??? 
//#endif
//						}else
//						{
//							sBody.append(pMailBaseInfo->GetContentData());
//						}
//					}
//
//				}
//			}
//		}
//	}
//	return pEBMailInfo.get()==NULL?false:true;
}

bool CSMTP::SendTOPCommand(int nMailId, int nNumber)
{
	if (m_wsSMTPServer.get()==0)
	{
		// 若还未建立连接
		m_sError = "Disconnect Error!";
		return false;
	}
	// 发送STAT命令
	char buf[64];
	sprintf(buf,"TOP %d %d\r\n",nMailId,nNumber);
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)buf,strlen(buf));
	CSmtpResponseInfo::pointer pResponseInfo;
	if (!getMailResponsePOP3(5,&pResponseInfo))
	{
		return false;
	}

	//pResponseInfo->m_sResponseMsg;
	return true;
}
bool CSMTP::SendDELECommand(int nMailId)
{
	if (m_wsSMTPServer.get()==0)
	{
		// 若还未建立连接
		m_sError = "Disconnect Error!";
		return false;
	}
	// 发送STAT命令
	char buf[64];
	sprintf(buf,"DELE %d\r\n",nMailId);
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)buf,strlen(buf));
	if (!getMailResponsePOP3(5))
	{
		return false;
	}
	m_bDeleteMail = true;
	return true;
}
bool CSMTP::SendRSETCommand(void)
{
	if (m_wsSMTPServer.get()==0)
	{
		// 若还未建立连接
		m_sError = "Disconnect Error!";
		return false;
	}
	// 发送STAT命令
	char buf[64];
	sprintf(buf,"RSET\r\n");
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)buf,strlen(buf));
	if (!getMailResponsePOP3(5))
	{
		return false;
	}

	m_bDeleteMail = false;
	return true;
}
bool CSMTP::SendNOOPCommand(void)
{
	if (m_wsSMTPServer.get()==0)
	{
		// 若还未建立连接
		m_sError = "Disconnect Error!";
		return false;
	}
	// 发送STAT命令
	char buf[64];
	sprintf(buf,"NOOP\r\n");
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)buf,strlen(buf));
	if (!getMailResponsePOP3(5))
	{
		return false;
	}

	return true;
}

bool CSMTP::FormatMailMessage( CMailMessage* msg )
{
	if (msg == NULL)
	{
		// msg为空是没有意义的
		return false;
	}

	if(!msg->GetNumRecipients())
	{
		m_sError = "No Recipients";
		return false;
	}

	// 在CMailMessage中封装了“格式化”消息的功能
	msg->FormatMailMessage();
	return true;
}

void CSMTP::SetServerProperties( const char* szSMTPServerName, size_t nPort)
{
	if( szSMTPServerName == NULL )
		return;
	m_sSMTPServerHostName = szSMTPServerName;
	m_nPort = nPort;
}

tstring CSMTP::cook_body(CMailMessage * msg) const
{
	if (msg==0) return "";

	tstring strTemp = msg->m_sBody;
	tstring strCook = "";
	tstring strBad = "\r\n.\r\n";
	tstring strGood = "\r\n..\r\n";
	int nPos,nStart = 0,nBadLength = (int)strBad.length();

	if (strTemp.substr(0,3) == ".\r\n")
		strTemp = "." + strTemp;
	while ((nPos = (int)strTemp.find(strBad)) >= 0)
	{
		strCook = strTemp.substr(nStart,nPos);
		strCook += strGood;
		const size_t nTempLen = strTemp.length();
		const size_t nRightLen = nTempLen-(nPos + nBadLength);
		strTemp = strCook + strTemp.substr(nTempLen-nRightLen);
		//strTemp = strCook + strTemp.Right(strTemp.GetLength() - (nPos + nBadLength));
	}
	return strTemp;
}

bool CSMTP::transmit_message(CMailMessage * msg)
{
	tstring strFrom,strTo,strTemp,strEmail;
	if (msg == NULL)
	{
		return false;
	}
//#ifdef USES_AFX_SOCKET
//	if (m_wsSMTPServer==0)
//#else
	if (m_wsSMTPServer.get()==0)
//#endif
	{
		// 若还未建立连接
		m_sError = "Disconnect Error!";
		return false;
	}

	// 发送MAIL命令
	tstring sFromName;
	tstring sFromMail;
	CMailMessage::GetMainAddressName(msg->m_sFrom,sFromName,sFromMail);
	strFrom = "MAIL From: <";
	strFrom.append(sFromMail);
	strFrom.append(">\r\n");
//#ifdef USES_AFX_SOCKET
//	m_wsSMTPServer->Send((const unsigned char*)strFrom.c_str(),strFrom.size());
//#else
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)strFrom.c_str(),strFrom.size());
//#endif
	if (!getMailResponseSMTP(GENERIC_SUCCESS))
	{
		return false;
	}

	// 发送RCPT命令
	int nCount = msg->GetNumRecipients();
	int i = 0;
	for(i = 0;i < nCount;i++)
	{
		msg->GetRecipient(strEmail,strTemp,i);
		strTo = "RCPT TO: <";
		strTo.append(strEmail); 
		strTo.append(">\r\n");
//#ifdef USES_AFX_SOCKET
//		m_wsSMTPServer->Send((const unsigned char*)strTo.c_str(),strTo.size());
//#else
	m_pResponseList.clear();
		m_wsSMTPServer->sendData((const unsigned char*)strTo.c_str(),strTo.size());
//#endif
		getMailResponseSMTP(GENERIC_SUCCESS);
	}

	// 处理抄送 
	nCount = msg->GetNumRecipients(CMailMessage::CC);
	for(i = 0;i < nCount;i++)
	{
		msg->GetRecipient(strEmail,strTemp,i,CMailMessage::CC);
		strTo = "RCPT TO: <";
		strTo.append(strEmail);
		strTo.append(">\r\n");
//#ifdef USES_AFX_SOCKET
//		m_wsSMTPServer->Send((const unsigned char*)strTo.c_str(),strTo.size());
//#else
	m_pResponseList.clear();
		m_wsSMTPServer->sendData((const unsigned char*)strTo.c_str(),strTo.size());
//#endif
		getMailResponseSMTP(GENERIC_SUCCESS);
	}

	// 处理暗送
	nCount = msg->GetNumRecipients(CMailMessage::BCC);
	for(i = 0;i < nCount;i++)
	{
		msg->GetRecipient(strEmail,strTemp,i,CMailMessage::BCC);
		strTo = "RCPT TO: <";
		strTo.append(strEmail);
		strTo.append(">\r\n");
//#ifdef USES_AFX_SOCKET
//		m_wsSMTPServer->Send((const unsigned char*)strTo.c_str(),strTo.size());
//#else
	m_pResponseList.clear();
		m_wsSMTPServer->sendData((const unsigned char*)strTo.c_str(),strTo.size());
//#endif
		getMailResponseSMTP(GENERIC_SUCCESS);
	}

	// 发送DATA命令
	strTemp = "DATA\r\n";
//#ifdef USES_AFX_SOCKET
//	m_wsSMTPServer->Send((const unsigned char*)strTemp.c_str(),strTemp.size());
//#else
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)strTemp.c_str(),strTemp.size());
//#endif
	if(!getMailResponseSMTP(DATA_SUCCESS))
	{
		// 发送DATA命令失败
		return false;
	}

	// 发送邮件头
//#ifdef USES_AFX_SOCKET
//	m_wsSMTPServer->Send((const unsigned char*)msg->m_sHeader.c_str(),msg->m_sHeader.size());
//#else
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)msg->m_sHeader.c_str(),msg->m_sHeader.size());
//#endif

	// 发送邮件正文
	strTemp = cook_body(msg);
//#ifdef USES_AFX_SOCKET
//	m_wsSMTPServer->Send((const unsigned char*)strTemp.c_str(),strTemp.size());
//#else
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)strTemp.c_str(),strTemp.size());
//#endif

	// 给邮件正文加上结束标志
	strTemp = "\r\n.\r\n";
//#ifdef USES_AFX_SOCKET
//	m_wsSMTPServer->Send((const unsigned char*)strTemp.c_str(),strTemp.size());
//#else
	m_pResponseList.clear();
	m_wsSMTPServer->sendData((const unsigned char*)strTemp.c_str(),strTemp.size());
//#endif
	if (!getMailResponseSMTP(GENERIC_SUCCESS))
	{
		return FALSE;
	}
	return TRUE;
}



//void CSMTP::Cancel(void)
//{
//	// 取消阻塞的连接
//	// 关闭套接字
////#ifdef USES_AFX_SOCKET
////	m_wsSMTPServer->CancelBlockingCall();
////	delete m_wsSMTPServer;
////	m_wsSMTPServer = 0;
////#else
//	m_wsSMTPServer.reset();
////#endif
//}

void CSMTP::OnReceiveData(const mycp::asio::ReceiveBuffer::pointer& data)
{
	/************
	//6、SMTP常用的响应
	//501——参数格式错误
	//502——命令不可实现
	//503——错误的命令序列
	//504——命令参数不可实现
	//211——系统状态或系统帮助响应
	//214——帮助信息
	//220<domain>——服务器就绪
	//221<domain>——服务关闭
	//421<domain>——服务器未就绪，关闭传输信道
	//250——要求的邮件操作完成
	//251——用户非本地，将转发向<forward-path>
	//450——要求的邮件操作未完成，邮箱不可用
	//550——要求的邮件操作未完成，邮箱不可用
	//451——放弃要求的操作，处理过程中出错
	//551——用户非本地，请尝试<forward-path>
	//452——系统存储不足，要求的操作未执行
	//552——过量的存储分配，要求的操作未执行
	//553——邮箱名不可用，要求的操作未执行
	//354——开始邮件输入，以“.”结束
	//554——操作失败

	// TCP Connect: 220 mail.extmail.org ESMTP Postfix - by extmail.org
	// HELO xxx: 250 mail.extmail.org
	// AUTH LOGIN: 334 VXNlcm5hbWU6		"Username"=
	// USER: 334 UGFzc3dvcmQ6			"Password:"=
	// PASS: 235 2.7.0 Authentication successful
	// MAIL From: 250 2.1.0 Ok
	// .....: 250 2.1.0 Ok
	// DATA: 250 2.1.0 Ok
	// QUIT: 221 2.0.0 Bye
	*/
#ifdef USES_WRITE_FILE_MAILID
#ifndef WIN32
	const bool bPrintDebug = (m_sMailId=="1tbiTxIBFFWBWKYgWgAAbY")?true:false;
	if (data->size()>0 && bPrintDebug)
	{
		char lpszBuffer[260];
		sprintf(lpszBuffer,"/eb/ebs/smtp_mail_%s_rece_data.txt",m_sMailId.c_str());
		FILE * f = fopen(lpszBuffer,"a");
		if (f==NULL)
			f = fopen(lpszBuffer,"w");
		if (f!=NULL)
		{
			fwrite(data->data(),1,data->size(),f);
			fclose(f);
		}
		sprintf(lpszBuffer,"/eb/ebs/smtp_mail_%s_rece_size.txt",m_sMailId.c_str());
		f = fopen(lpszBuffer,"a");
		if (f==NULL)
			f = fopen(lpszBuffer,"w");
		if (f!=NULL)
		{
			sprintf(lpszBuffer,"%d,",data->size());
			fwrite(lpszBuffer,1,strlen(lpszBuffer),f);
			fclose(f);
		}
	}
#endif
#endif

	if (m_nMailType==MAIL_TYPE_SMTP)
	{
		if (data->size()<4)
		{
			return;
		}
		tstring sSmtpResponse((const char*)data->data());
		const int nResponseCode = atoi(sSmtpResponse.substr(0,3).c_str());
		sSmtpResponse = sSmtpResponse.substr(4);
		m_pResponseList.add(CSmtpResponseInfo::create(nResponseCode,sSmtpResponse));
	}else
	{
		if (data->size()==0)
		{
			return;
		}
		// 1=+OK
		// -1=-ERR
		// 0=[DATA]
		tstring sSmtpResponse((const char*)data->data());
		tstring sStateChar(sSmtpResponse.substr(0,3));
		std::transform(sStateChar.begin(), sStateChar.end(), sStateChar.begin(), toupper);
		const int nResponseCode = sStateChar=="+OK"?1:(sStateChar=="-ER"?-1:0);
		if (nResponseCode==1)			// +OK
			sSmtpResponse = sSmtpResponse.substr(4);
		else if (nResponseCode==-1)		// -ERR
			sSmtpResponse = sSmtpResponse.substr(5);
		m_pResponseList.add(CSmtpResponseInfo::create(nResponseCode,sSmtpResponse));
	}
}

bool CSMTP::getMailResponseSMTP(size_t response_expected, int nTimeoutSecond, bool* pOutTimeout)
{
	BOOST_ASSERT( response_expected >= GENERIC_SUCCESS );
	BOOST_ASSERT( response_expected < LAST_RESPONSE );
//#ifdef USES_AFX_SOCKET
//	char lpszBuffer[1024];
//	m_wsSMTPServer->Receive(lpszBuffer,1024);
//	tstring sSmtpResponse(lpszBuffer);
//	const int nResponseCode = atoi(sSmtpResponse.substr(0,3).c_str());
//	sSmtpResponse = sSmtpResponse.substr(4);
//	m_pResponseList.add(CSmtpResponseInfo::create(nResponseCode,sSmtpResponse));
//#endif

	if (nTimeoutSecond<1)
		nTimeoutSecond = 1;
	else
		nTimeoutSecond *= 10;

	CSmtpResponseInfo::pointer pResponseInfo;
	for (int i=0; i<nTimeoutSecond; i++)
	{
		if (m_pResponseList.front(pResponseInfo))
		{
			break;
		}
#ifdef WIN32
		Sleep(100);
#else
		usleep(100000);
#endif
	}
	if (pResponseInfo.get()==NULL)
	{
		m_sError = "Timeout Error!";
		if (pOutTimeout!=0)
			*pOutTimeout = true;
		return false;
	}
	response_code* pResp = &response_table_smtp[response_expected];
	if(pResponseInfo->m_nResponseCode != pResp->nResponse)
	{
		m_sError = pResponseInfo->m_sResponseMsg;
		//m_sError.Format(_T("%d : %s"),pResponseInfo->m_nResponseCode,pResponseInfo->m_sResponseMsg.c_str() );
		return false;
	}
	return true;
}
bool CSMTP::getMailResponsePOP3(int nTimeoutSecond, CSmtpResponseInfo::pointer* pOutResponse, bool* pOutTimeout)
{
	if (nTimeoutSecond<1)
		nTimeoutSecond = 1;
	else
		nTimeoutSecond *= 10;

	int nNullCount = 0;
	CSmtpResponseInfo::pointer pResponseInfo;
	for (int i=0; i<nTimeoutSecond; i++)
	{
		CSmtpResponseInfo::pointer pTempResponseInfo;
		if (m_pResponseList.front(pTempResponseInfo))
		{
			nNullCount = 0;
			if (pResponseInfo.get()==NULL)
				pResponseInfo = pTempResponseInfo;
			else if (!pTempResponseInfo->m_sResponseMsg.empty())
				pResponseInfo->m_sResponseMsg.append(pTempResponseInfo->m_sResponseMsg);
			//break;
			i--;
			continue;
		}else if (pResponseInfo.get()!=NULL)
		{
			if ((nNullCount++)>5)
				break;
		}

		//if (m_pResponseList.front(pResponseInfo))
		//{
		//	break;
		//}
#ifdef WIN32
		Sleep(100);
#else
		usleep(100000);
#endif
	}
	if (pResponseInfo.get()==NULL)
	{
		m_sError = "Timeout Error!";
		if (pOutTimeout!=NULL)
			*pOutTimeout = true;
		return false;
	}
	if (pOutResponse!=NULL)
		*pOutResponse = pResponseInfo;
	if (pResponseInfo->m_nResponseCode<0)
	{
		m_sError = pResponseInfo->m_sResponseMsg;
		return false;
	}
	return true;
}

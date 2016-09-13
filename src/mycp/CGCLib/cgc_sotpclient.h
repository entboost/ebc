// cgc_sotpclient.h file here
#ifndef __cgc_sotpclient_h__
#define __cgc_sotpclient_h__

#ifdef WIN32
#pragma warning(disable:4311)	// for sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PUBLIC_KEY,(unsigned int)m_sSslPublicKey.c_str());
#endif

#include <boost/shared_ptr.hpp>
#include "CGCLib.h"

namespace mycp {

class CCGCSotpClient
	: public CSotpClient
	, public CgcClientHandler
{
public:
	typedef boost::shared_ptr<CCGCSotpClient> pointer;
	static CCGCSotpClient::pointer create(void)
	{
		return CCGCSotpClient::pointer(new CCGCSotpClient());
	}
	CCGCSotpClient(void)
	{
	}
	virtual ~CCGCSotpClient(void)
	{
		StopClient();
		//delallclient();
	}
	//void SetIoService(mycp::asio::IoService::pointer pIoService, bool bExitStop = false) {m_pCgcClient.SetIoService(pIoService, bExitStop);}
	//void ResetIoService(void) {m_pCgcClient.ResetIoService();}
	//mycp::asio::IoService::pointer GetIoService(void) const {return m_pCgcClient.GetIoService();}
private:
	CCgcAddress m_pAddress;
	//CSotpClient m_pCgcClient;
	DoSotpClientHandler::pointer m_pSotpClientHandler;

public:
	void SetSslKey(const tstring& sSslPublicKey,const tstring& sSslPrivateKey,const tstring& sSslPrivatePwd)
	{
		m_sSslPublicKey = sSslPublicKey;
		m_sSslPrivateKey = sSslPrivateKey;
		m_sSslPrivatePwd = sSslPrivatePwd;
		if (IsClientStarted())
		{
			sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PUBLIC_KEY,(unsigned int)m_sSslPublicKey.c_str());
			sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PRIVATE_KEY,(unsigned int)m_sSslPrivateKey.c_str());
			sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PRIVATE_PWD,(unsigned int)m_sSslPrivatePwd.c_str());
		}
	}
	bool StartClient(const CCgcAddress & address, unsigned int bindPort=0, int nThreadStackSize=200)
	{
		if (IsClientStarted()) return true;
		m_pAddress = address;
		m_pSotpClientHandler = startClient(m_pAddress, bindPort, nThreadStackSize);
		//m_pSotpClientHandler = m_pCgcClient.startClient(m_pAddress, bindPort, nThreadStackSize);
		if (IsClientStarted())
		{
			if (!m_sSslPublicKey.empty() && !m_sSslPrivateKey.empty() && !m_sSslPrivatePwd.empty())
			{
				sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PUBLIC_KEY,(unsigned int)m_sSslPublicKey.c_str());
				sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PRIVATE_KEY,(unsigned int)m_sSslPrivateKey.c_str());
				sotp()->doSetConfig(SOTP_CLIENT_CONFIG_PRIVATE_PWD,(unsigned int)m_sSslPrivatePwd.c_str());
			}
			//m_pSotpClientHandler->doSetResponseHandler(&gMyCgcClientHandler);
			//m_pSotpClientHandler->doSetAppName(appname);
			return true;
		}
		return false;
	}
	void StopClient(void)
	{
		if (m_pSotpClientHandler.get() != NULL)
		{
			stopClient(m_pSotpClientHandler);
			//m_pCgcClient.stopClient(m_pSotpClientHandler);
			m_pSotpClientHandler.reset();
		}
	}
	bool IsClientStarted(void) const {return m_pSotpClientHandler.get() != NULL;}
	void SetResponseHandler(CgcClientHandler * pHandler)
	{
		if (IsClientStarted())
		{
			m_pSotpClientHandler->doSetResponseHandler(pHandler);
		}
	}
	void SetAppName(const tstring & sAppName)
	{
		if (IsClientStarted())
		{
			m_pSotpClientHandler->doSetAppName(sAppName);
		}
	}
	const DoSotpClientHandler::pointer& sotp(void) const {return m_pSotpClientHandler;}
	DoSotpClientHandler::pointer newclient(const CCgcAddress & address, unsigned int bindPort=0) {return startClient(address, bindPort);}
	//DoSotpClientHandler::pointer newclient(const CCgcAddress & address, unsigned int bindPort=0) {return m_pCgcClient.startClient(address, bindPort);}
	void delclient(DoSotpClientHandler::pointer sotpClient) {stopClient(sotpClient);}
	//void delclient(DoSotpClientHandler::pointer sotpClient) {m_pCgcClient.stopClient(sotpClient);}
	void delallclient(void) {stopAllClient();}
	//void delallclient(void) {m_pCgcClient.stopAllClient();}
	const CCgcAddress & GetAddress(void) const {return m_pAddress;}

private:
	virtual void OnCgcResponse(const cgcParserSotp & response) {}
	virtual void OnCgcResponse(const unsigned char * recvData, size_t dataSize) {}
	virtual void OnCidTimeout(unsigned long callid, unsigned long sign, bool canResendAgain){}
protected:
	tstring m_sSslPublicKey;
	tstring m_sSslPrivateKey;
	tstring m_sSslPrivatePwd;
};

} // namespace cgc

#endif // __cgc_sotpclient_h__

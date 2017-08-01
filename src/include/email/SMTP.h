// SMTP.h: interface for the CSMTP class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMTP_H__55DE48CB_BEA4_11D1_870E_444553540000__INCLUDED_)
#define AFX_SMTP_H__55DE48CB_BEA4_11D1_870E_444553540000__INCLUDED_

//#define USES_AFX_SOCKET
//#ifdef USES_AFX_SOCKET
//#include "afxsock.h"
//#endif
#include "../CgcTcpClient.h"
#include "MailMessage.h"
#include "ebmailobj.h"

// SMTP服务器标准端口号
#define SMTP_PORT 25
#define SMTP_SSL_PORT 465
// 定义接收缓冲区大小		
#define RESPONSE_BUFFER_SIZE 1024

CEBMailInfo::pointer ParserMailInfo(const tstring& sMailString, bool bMailMessage, bool bParserHeadOnly);

class CSmtpResponseInfo
{
public:
	typedef boost::shared_ptr<CSmtpResponseInfo> pointer;
	static CSmtpResponseInfo::pointer create(int nCode, const tstring& sMsg)
	{
		return CSmtpResponseInfo::pointer(new CSmtpResponseInfo(nCode, sMsg));
	}

	int m_nResponseCode;
	tstring m_sResponseMsg;
	CSmtpResponseInfo(int nCode, const tstring& sMsg)
		: m_nResponseCode(nCode), m_sResponseMsg(sMsg)
	{
	}
	CSmtpResponseInfo(void)
		: m_nResponseCode(0)
	{}
};

class CSMTP  
	: public entboost::TcpClient_Callback	// for ssl tcp
{
public:
	typedef boost::shared_ptr<CSMTP> pointer;
	enum MAIL_TYPE
	{
		MAIL_TYPE_SMTP
		, MAIL_TYPE_POP3
	};
	static CSMTP::pointer create(const char* szSMTPServerName, size_t nPort, bool bSSL, MAIL_TYPE nMailType=MAIL_TYPE_SMTP)
	{
		return CSMTP::pointer(new CSMTP(szSMTPServerName,nPort,bSSL,nMailType));
	}
	CSMTP( const char* szSMTPServerName, size_t nPort, bool bSSL, MAIL_TYPE nMailType=MAIL_TYPE_SMTP);
	virtual ~CSMTP(void);

	static const tstring & replace(tstring & strSource, const tstring & strFind, const tstring &strReplace);
	//static void Test2_SSL_library_init(void);

	// 记录服务器属性的接口
	void SetServerProperties( const char* szSMTPServerName, size_t nPort = SMTP_PORT);
	tstring GetLastError(void) const;
	size_t GetPort(void) const {return m_nPort;}

	// 与服务器的联系函数
	bool Disconnect(void);
	int Connect(const char* sMailAccount, const char* sPasswd, bool* pOutTimeout = NULL);	// 1:ok -1:server error 0:user error
	bool IsConnected(void) const;
	//bool Login(void);
	void SetMailId(const std::string& v) {m_sMailId = v;}
	const std::string& GetMailId(void) const {return m_sMailId;}

	// 解释收到的回应信息
	virtual bool FormatMailMessage( CMailMessage* msg );
	bool SendMailMessage( CMailMessage* msg );
	bool SendSTATCommand(void);
	bool SendLISTCommand(int nMailId,std::vector<int>& pOutMailSizeList,bool& pOutFinished);
	bool SendUIDLCommand(int nMailId, std::vector<tstring>& pOutMailIdList,bool& pOutFinished);
	CEBMailInfo::pointer SendRETRCommand(int nMailId, bool& pOutGetDataError);
	bool SendTOPCommand(int nMailId, int nNumber);
	bool SendDELECommand(int nMailId);
	bool SendRSETCommand(void);
	bool SendNOOPCommand(void);
	tstring GetServerHostName(void) const {return m_sSMTPServerHostName;}
	const tstring& GetErrorString(void) const {return m_sError;}

	//// 连接是阻塞式的，因此要提供取消通信的方法
	//void Cancel(void);
	inline bool IsFullNumber(const char* pString, size_t nLen)
	{
		for (size_t i=0;i<nLen; i++)
		{
			const char pChar = pString[i];
			if (pChar<'0' || pChar>'9')
				return false;
		}
		return true;
	}
	inline bool IsFullNumberX(const char* pString, size_t nLen)
	{
		for (size_t i=0;i<nLen; i++)
		{
			const char pChar = pString[i];
			if (pChar==' ')
				continue;
			if (pChar<'0' || pChar>'9')
				return false;
		}
		return true;
	}
	void SetSSLContext(bool bDelete, boost::asio::ssl::context * sslctx);

private:
	void ClearTcpClient(void);
	// 处理邮件正文
	tstring cook_body( CMailMessage* msg ) const;
	// 处理获得的回应信息
	bool getMailResponseSMTP(size_t response_expected, int nTimeoutSecond = 5, bool* pOutTimeout = NULL);
	bool getMailResponsePOP3(int nTimeoutSecond = 5, CSmtpResponseInfo::pointer* pOutResponse=NULL, bool* pOutTimeout = NULL);

	tstring m_sError;
	tstring m_sSMTPServerHostName;
	size_t m_nPort;
	bool m_bSsl;
	MAIL_TYPE m_nMailType;
	bool m_bDeleteMail;

	// 与SMTP服务器通信的套接字
//#ifdef USES_AFX_SOCKET
//	CSocket* m_wsSMTPServer;
//#else
	boost::asio::ssl::context * m_sslctx;
	bool m_bDeleteCtx;
	entboost::CgcTcpClient::pointer m_wsSMTPServer;
	virtual void OnReceiveData(const mycp::asio::ReceiveBuffer::pointer& data);
//#endif

protected:
	// 发送邮件消息
	virtual bool transmit_message( CMailMessage* msg );

	CLockList<CSmtpResponseInfo::pointer> m_pResponseList;

	struct response_code
	{
		// 如果没有得到回应信息，则给出出错信息
		int nResponse;		
		std::string sMessage;	
	};

	enum eResponse
	{
		GENERIC_SUCCESS = 0,
		CONNECT_SUCCESS,
		PASS_SUCCESS,
		AUTH_LOGIN_SUCCESS,
		DATA_SUCCESS,
		QUIT_SUCCESS,
		// 本例只考虑到上述几种回应消息
		// LAST_RESPONSE是最后一个元素
		LAST_RESPONSE	
	};
	static response_code response_table_smtp[];
	static response_code response_table_pop3[];
	std::string m_sMailId;
};

/*
Delivered-To: akee.yang@gmail.comReceived: by 10.60.174.234 with SMTP id bv10csp324690oec;Fri, 26 Jun 2015 20:56:00 -0700 (PDT)X-Received: by 10.107.128.227 with SMTP id k96mr6151549ioi.7.1435377359944;Fri, 26 Jun 2015 20:55:59 -0700 (PDT)Return-Path: <weeklynewsletter@newsletter.ftchinese.com>Received: from mta1.newsletter.ftchinese.com ([60.28.236.19])by mx.google.com with SMTP id j8si799873igx.48.2015.06.26.20.55.56for <akee.yang@gmail.com>; Fri, 26 Jun 2015 20:55:59 -0700 (PDT)Received-SPF: pass (google.com: domain of weeklynewsletter@newsletter.ftchinese.com designates 60.28.236.19 as permitted sender) client-ip=60.28.236.19;Authentication-Results: mx.google.com;spf=pass (google.com: domain of weeklynewsletter@newsletter.ftchinese.com designates 60.28.236.19 as permitted sender) smtp.mail=weeklynewsletter@newsletter.ftchinese.com;dkim=pass header.i=@newsletter.ftchinese.com;dmarc=pass (p=NONE dis=NONE) header.from=newsletter.ftchinese.comDKIM-Signature: v=1; a=rsa-sha256; q=dns/txt; c=simple/simple; d=newsletter.ftchinese.com; s=default;h=Message-ID:Content-Transfer-Encoding:Content-Type:MIME-Version:Date:To:Subject:From; bh=2097jUEpUbQtDABUw4O9QbE0IysSY0mEeFCqRgNq4q0=;b=njJS8SSuGiWBDgn+/fyY4YZZDGLNWUyo2Pfi+ajC/9OYZ8Phjdz/xkJ97tVB+OhVV+FDry5bNa2tcOscuY1r6CazZSvJuhVLelw8vLF3iRClTlFo7z3OwYkTEQoAEYVFJV3tZxlm0CziPpEY7laK8HPw2qxaJtnYIhoPzg3Q+js=;Received: from mta1.newsletter.ftchinese.com([192.168.1.75])by X1FDWVFAVVxQQFBX(Postfix) with SMTPD id 18955for <akee.yang@gmail.com>; Sat, 27 Jun 2015 11:55:58 +0800From:  =?utf-8?B?RlTkuK3mlofnvZEgLSBGVENoaW5lc2UuY29t?= <weeklynewsletter@newsletter.ftchinese.com>Subject: =?utf-8?B?5LiA5ZGo57K+6YCJ77ya5Y+w5rm+6LWw5YWl4oCc5pyA5ZCO55qE5Lit5Zu95Lq64oCd5pe25Luj?=To: "=?utf-8?B?YWtlZS55YW5nQGdtYWlsLmNvbQ==?=" <akee.yang@gmail.com>Date: Sat, 27 Jun 2015 11:54:58 +0800MIME-Version: 1.0Content-Type: text/html; charset="utf-8" Content-Transfer-Encoding: base64Message-ID: <1435377356059361@3861>邮件首部的扩充=?charset?encoding?encoded-text?text=?utf-8?B?5LiA5ZGo57K+6YCJ77ya5Y+w5rm+6LWw5YWl4oCc5pyA5ZCO55qE5Lit5Zu95Lq64oCd5pe25Luj?=*/

#endif // !defined(AFX_SMTP_H__55DE48CB_BEA4_11D1_870E_444553540000__INCLUDED_)

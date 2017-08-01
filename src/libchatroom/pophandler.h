// pophandler.h file here
#ifndef __pophandler_h__
#define __pophandler_h__

class CPOPHandler
{
public:
	virtual int OnTimeout(CPOPSotpRequestInfo::pointer pRequestInfo) = 0;

	virtual int OnFCMMsg(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnCMMsgResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnFCMExit(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnCMExitResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnCMEnterResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;

	virtual int OnFUMIUHangup(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnUMIUHangup(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnFUMIUAck(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnUMIUAck(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnUMIUUser(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnFUMIUUser(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnUMSMOfflineResponst(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnUMSMOnlineResponst(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;

	virtual int OnLCULQueryResponst(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnLCULLogonResponst(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;

	virtual int OnCSLoadResponst(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
	virtual int OnCSQueryResponst(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo) = 0;
};

#endif // __pophandler_h__

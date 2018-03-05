// rdevent.h file here
#ifndef __rdevent_h__
#define __rdevent_h__

#include "rdpinfo.h"

class CRDEventHandler
{
public:
	virtual bool onCanProcess(void) const = 0;
	virtual void onLocalDesktopInfo(int nVideoWidth, int nVideoHeight, int nBitCount) = 0;
	virtual void onLocalDesktopData(const unsigned char* pData, unsigned int nSize, bool bKeyFrame, unsigned int nTimestamp) = 0;
};

class CRemoteEventHandler
{
public:
	virtual void onSendRdpMessage(const rdpStruct& pRdpMessage, void* pUserData) = 0;
};

#endif // __rdevent_h__

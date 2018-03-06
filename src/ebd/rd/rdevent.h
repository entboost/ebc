// rdevent.h file here
#ifndef __rdevent_h__
#define __rdevent_h__

#include "rdpinfo.h"

class CRDEventHandler
{
public:
	virtual DWORD onRDP_Request(int nVideoWidth, int nVideoHeight, int nBitCount) = 0;
	virtual void onRDP_RequestBusy(void) {}	// ?
	virtual void onRDP_Accept(void) = 0;
	virtual void onRDP_AcceptResponse(void) = 0;
	virtual void onRDP_Close(void) = 0;
	virtual void onRDP_DibBits(const void * pDibBits) = 0;
	virtual void onRDP_MessageEvent(const rdpStruct & rdpMessage) = 0;

};

#endif // __rdevent_h__

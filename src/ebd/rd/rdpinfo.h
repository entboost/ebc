// rdpinfo.h file here
#ifndef __rdpinfo_h__
#define __rdpinfo_h__

#include "string.h"

enum rdpCommandType
{
	RDP_REQUEST	= 0x0
	, RDP_ACCEPT
	, RDP_ACCEPTRES
	//, RDP_REJECT
	, RDP_BUSY
	, RDP_CLOSE

	, RDP_BMPEND	= 0x10
	, RDP_MESSAGE_EVENT	= 0x20
};

enum rdpMessageEvent
{
	RDP_UNKNOWN			= 0x0
	, RDP_MOUSEMOVE
	, RDP_LBUTTONDOWN
	, RDP_LBUTTONUP
	, RDP_RBUTTONDOWN
	, RDP_RBUTTONUP
	, RDP_LBUTTONDBLCLK

	, RDP_KEYDOWN		= 0x100
	, RDP_KEYUP
};

struct rdpSize
{
	int m_width;
	int m_height;
};

struct rdpPoint
{
	int m_x;
	int m_y;
};

struct rdpWindowInfo
{
	rdpSize m_size;
	short m_bitCount;
};

struct rdpKeyInfo
{
	unsigned int m_wParam;
	long m_lParam;
};

struct rdpStruct
{
	rdpCommandType m_commandType;
	rdpMessageEvent m_messageEvent;
	int m_callid;
	union
	{
		rdpWindowInfo windowInfo;
		rdpPoint mousePoint;
		rdpKeyInfo keyInfo;
	}u;
};

#define RDPSTRUCT_SIZE sizeof(rdpStruct)


class CRdpStruct
{

public:
	static rdpStruct GetRdpStruct(rdpCommandType commandType, rdpMessageEvent messageEvent = RDP_UNKNOWN)
	{
		rdpStruct rdp;
		memset(&rdp, 0, RDPSTRUCT_SIZE);
		rdp.m_commandType = commandType;
		rdp.m_messageEvent = messageEvent;
		return rdp;
	}
	
};

#endif // __rdpinfo_h__

// rdpinfo.h file here
#ifndef __rdpinfo_h__
#define __rdpinfo_h__

#include "string.h"

//enum rdpCommandType
//{
//	RDP_REQUEST	= 0x0
//	, RDP_ACCEPT
//	, RDP_ACCEPTRES
//	//, RDP_REJECT
//	, RDP_BUSY
//	, RDP_CLOSE
//
//	, RDP_BMPEND	= 0x10
//	, RDP_MESSAGE_EVENT	= 0x20
//};

enum rdpMessageEvent
{
	RDP_UNKNOWN			= 0x0
	, RDP_MOUSEMOVE
	, RDP_LBUTTONDOWN
	, RDP_LBUTTONUP
	, RDP_RBUTTONDOWN
	, RDP_RBUTTONUP
	, RDP_LBUTTONDBLCLK

	, RDP_KEYDOWN		= 0x10
	, RDP_KEYUP
	//, RDP_SYSKEYDOWN
	//, RDP_SYSKEYUP
	, RDP_DESKTOP_DATA	= 0x20
};

inline UINT ToMouseEvent(unsigned char messageEvent)
{
	switch (messageEvent)
	{
	case RDP_LBUTTONDOWN:
		return MOUSEEVENTF_LEFTDOWN;
	case RDP_LBUTTONUP:
		return MOUSEEVENTF_LEFTUP;
	case RDP_RBUTTONDOWN:
		return MOUSEEVENTF_RIGHTDOWN;
	case RDP_RBUTTONUP:
		return MOUSEEVENTF_RIGHTUP;

	default:
		ASSERT (FALSE);
		return 0;
	}
}
inline rdpMessageEvent WM2RdpMessageEvent(UINT message)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
		return RDP_LBUTTONDOWN;
	case WM_LBUTTONUP:
		return RDP_LBUTTONUP;
	case WM_RBUTTONDOWN:
		return RDP_RBUTTONDOWN;
	case WM_RBUTTONUP:
		return RDP_RBUTTONUP;
	case WM_LBUTTONDBLCLK:
		return RDP_LBUTTONDBLCLK;

	case WM_KEYDOWN:
		return RDP_KEYDOWN;
	case WM_KEYUP:
		return RDP_KEYUP;
	default:
		return RDP_UNKNOWN;
	}
}


#pragma pack(push)
#pragma pack(1)

struct rdpDesktopInfo
{
	unsigned short m_nWidth;
	unsigned short m_nHeight;
	unsigned char m_nBitCount;
};
//#define RDP_DESKTOP_INFO_SIZE 5

struct rdpSize
{
	unsigned short m_width;
	unsigned short m_height;
};

struct rdpPoint
{
	unsigned short m_x;
	unsigned short m_y;
};

//struct rdpWindowInfo
//{
//	rdpSize m_size;
//	unsigned short m_bitCount;
//};

struct rdpKeyInfo
{
	unsigned int m_wParam;
	unsigned int m_lParam;
};

struct rdpStruct
{
	//unsigned char m_commandType;	// rdpCommandType
	unsigned char m_messageEvent;	// rdpMessageEvent
	//int m_callid;
	union
	{
		rdpDesktopInfo desktopInfo;
		//rdpWindowInfo windowInfo;
		rdpPoint mousePoint;
		rdpKeyInfo keyInfo;
	}u;
};

#define RDPSTRUCT_SIZE sizeof(rdpStruct)
#pragma pack(pop)


//class CRdpStruct
//{
//
//public:
//	static rdpStruct GetRdpStruct(rdpCommandType commandType, rdpMessageEvent messageEvent = RDP_UNKNOWN)
//	{
//		rdpStruct rdp;
//		memset(&rdp, 0, RDPSTRUCT_SIZE);
//		//rdp.m_commandType = commandType;
//		rdp.m_messageEvent = messageEvent;
//		return rdp;
//	}
//	
//};

#endif // __rdpinfo_h__

// videoroomdefine.h file here
#ifndef __videoroomdefine_h__
#define __videoroomdefine_h__

namespace vr{
#ifdef WIN32
	typedef __int64				bigint;
#define vr_atoi64(a) _atoi64(a)
#else
	typedef long long			bigint;
#define vr_atoi64(a) atoll(a)
#endif

	typedef enum
	{
		VIDEO_RGB24  = 0
		, VIDEO_RGB32
		, VIDEO_I420
		, VIDEO_H264
		, VIDEO_WMV9
		, VIDEO_RV40
		, VIDEO_XVID
		, VIDEO_YUY2
	}VIDEO_RGB_TYPE;

	//#ifndef VIDEOROOM_EXPORTS
	typedef struct tagVIDEOFRAME
	{
		DWORD type;    //frame type
		WORD  width;
		WORD  height;
		BYTE  *data;
		DWORD len;
		DWORD usedlen;
		DWORD timastamp;
	}VIDEOFRAME;
	//#endif

typedef void (*VideoStreamCallBack) (vr::bigint nId, vr::VIDEOFRAME *pFrame, void *lParam, unsigned long uData);
typedef void (*AudioStreamCallBack) (vr::bigint nId, char *pdata, int length, void* uData);

};

#endif // __videoroomdefine_h__


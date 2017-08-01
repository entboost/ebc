// ebmmhandle.h file here
#ifndef __ebmmhandle_h__
#define __ebmmhandle_h__

#ifdef EBMM_EXPORTS
#define EBMM_API __declspec(dllexport)
#else
#define EBMM_API __declspec(dllimport)
#endif

#include <string>
//#include "ebmmobject.h"

namespace entboost {

#define EBMM_CALLBACK_FIRST_VIDEO_FPS	1
#define EBMM_CALLBACK_VOICE_VOLUME		2
typedef void (*FPMMCallBack) (mycp::bigint nUserId, int nCallBackType, unsigned int lParam, unsigned int wParam, void* pUserData);

//
//#ifdef EB_SERVER
//#define WM_USER 0
//#endif
//#ifndef WM_USER
//#define WM_USER 0
//#endif
//typedef enum EBRD_WINDOW_MESSAGE
//{
//	// const EBRD_RtpFrame* pRtpFrame = (const EBRD_RtpFrame*)wp;
//	// ebrd::uint32 nUserData = (ebrd::uint32)lp;
//	EBCR_WM_RTP_FRAME			= WM_USER+0x2101
//};
//
// If MFC error use EBRD_WINDOW_MESSAGE.
//class CEBMMCallBack
//{
//public:
//	//virtual void OnRtpFrame(const EBRD_RtpFrame& pRtpFrame,mycp::uint32 nUserData) = 0;
//	virtual void OnVoicePos(mycp::bigint nUserId, int nVoicePos, mycp::uint32 nUserData) = 0;
//};

} // namespace entboost

#endif // __ebmmhandle_h__

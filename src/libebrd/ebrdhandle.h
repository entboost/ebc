// ebrdhanle.h file here
#ifndef __ebrdhanle_h__
#define __ebrdhanle_h__

#ifdef EBRD_EXPORTS
#define EBRD_API __declspec(dllexport)
//#define EXPIMP_TEMPLATE
#else
#define EBRD_API __declspec(dllimport)
//#define EXPIMP_TEMPLATE extern
#endif

#include <string>
#include "ebrdobject.h"

namespace entboost {

#ifdef EB_SERVER
#define WM_USER 0
#endif
#ifndef WM_USER
#define WM_USER 0
#endif
typedef enum EBRD_WINDOW_MESSAGE
{
	// const EBRD_RtpFrame* pRtpFrame = (const EBRD_RtpFrame*)wp;
	// ebrd::uint32 nUserData = (ebrd::uint32)lp;
	EBCR_WM_RTP_FRAME			= WM_USER+0x2101
};

// If MFC error use EBRD_WINDOW_MESSAGE.
class CEBRDCallBack
{
public:
	virtual void OnRtpFrame(const EBRD_RtpFrame& pRtpFrame,mycp::uint32 nUserData) = 0;
};

} // namespace entboost

#endif // __ebrdhanle_h__

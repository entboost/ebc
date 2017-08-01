// ebrdobject.h file here
#ifndef __ebrdobject_h__
#define __ebrdobject_h__

//#include "ebstring.h"
#include <vector>
#include <CGCBase/cgcrtpobject.h>
using namespace mycp;

namespace entboost {

class EBRD_RtpFrame
{
public:
	mycp::bigint m_nSourceId;
	CSotpRtpFrame::pointer m_pRtpFrame;
	mycp::uint16 m_nLostCount;

	EBRD_RtpFrame(mycp::bigint nSourceId, const CSotpRtpFrame::pointer& pRtpFrame, mycp::uint16 nLostCount)
		: m_nSourceId(nSourceId)
		, m_pRtpFrame(pRtpFrame)
		, m_nLostCount(nLostCount)
	{}
	EBRD_RtpFrame(void)
		: m_nSourceId(0)
		, m_nLostCount(0)
	{}

};

} // namespace entboost

#endif // __ebrdobject_h__


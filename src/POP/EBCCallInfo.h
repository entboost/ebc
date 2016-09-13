// EBCCallInfo.h file here
#ifndef __EBCCallInfo_h__
#define __EBCCallInfo_h__

//#include "EBAppClient.h"
//#include "chatroom.h"

class CEBCCallInfo
{
public:
	typedef boost::shared_ptr<CEBCCallInfo> pointer;
	static CEBCCallInfo::pointer create(void)
	{
		return CEBCCallInfo::pointer(new CEBCCallInfo());
	}

	EB_CallInfo m_pCallInfo;
	EB_AccountInfo m_pFromAccountInfo;
	bool m_bOffLineUser;
	time_t m_tLastTime;

	CEBCCallInfo(void)
		: m_bOffLineUser(false)
	{
		m_tLastTime = time(0);
	}
	virtual ~CEBCCallInfo(void)
	{}
};
const CEBCCallInfo::pointer NullEBCCallInfo;

#endif // __EBCCallInfo_h__

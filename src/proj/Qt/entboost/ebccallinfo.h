#ifndef EBCCALLINFO_H
#define EBCCALLINFO_H

#include <boost/shared_ptr.hpp>
#include <CGCBase/cgcobject.h>
#include <CGCBase/cgcSmartString.h>
using namespace mycp;
#include "../../../include/eb_object.h"
using namespace entboost;

class EbcCallInfo
{
public:
    typedef boost::shared_ptr<EbcCallInfo> pointer;
    static EbcCallInfo::pointer create(void);

    mycp::bigint callId(void) const {return m_pCallInfo.GetCallId();}
    mycp::bigint groupId(void) const {return m_pCallInfo.m_sGroupCode;}
    bool isGroupCall(void) const {return m_pCallInfo.m_sGroupCode>0?true:false;}
    mycp::bigint fromUserId(void) const {return m_pFromAccountInfo.GetUserId();}
    mycp::bigint fromMemberId(void) const {return m_pFromAccountInfo.m_pFromCardInfo.m_sMemberCode;}
    const tstring &fromUserAccount(void) const {return m_pFromAccountInfo.GetAccount();}
    tstring fromName(void) const;
    EB_CallInfo m_pCallInfo;
    EB_AccountInfo m_pFromAccountInfo;
    bool m_bOffLineUser;
    time_t m_tLastTime;

    EbcCallInfo(void);
};
const EbcCallInfo::pointer EbcCallInfoNull;

#endif // EBCCALLINFO_H

#include "ebccallinfo.h"

EbcCallInfo::EbcCallInfo()
    : m_bOffLineUser(false)
{
    m_tLastTime = time(0);
}
EbcCallInfo::pointer EbcCallInfo::create(void)
{
    return EbcCallInfo::pointer(new EbcCallInfo());
}

cgcSmartString EbcCallInfo::fromName() const
{
//    if (isGroupCall()) {
//        tstring groupName;
//        theApp->m_ebum.EB_GetGroupName(groupId(),groupName);
//        return groupName;
//    }
//    else if ( !m_pFromAccountInfo.m_pFromCardInfo.m_sAccountName.empty() ) {
//        return m_pFromAccountInfo.m_pFromCardInfo.m_sAccountName;
//    }
//    else if ( !m_pFromAccountInfo.GetUserName().empty() ) {
//        return m_pFromAccountInfo.GetUserName();
//    }
//    else {
//        tstring memberName;
//        theApp->m_ebum.EB_GetMemberNameByUserId2( fromUserId(),memberName );
//        return memberName;
//    }

    return "";
}

EbcCallInfo::EbcCallInfo(const EbcCallInfo *other)
    : m_pCallInfo(other->m_pCallInfo)
    , m_pFromAccountInfo(other->m_pFromAccountInfo)
    , m_bOffLineUser(other->m_bOffLineUser)
    , m_tLastTime(other->m_tLastTime)
{

}

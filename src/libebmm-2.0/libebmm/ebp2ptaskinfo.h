#ifndef EBP2PTASKINFO_H
#define EBP2PTASKINFO_H

#include "../include/EBRtpManager.h"
#include <CGCBase/cgcSmartString.h>
#include <CGCBase/cgcobject.h>
#include <boost/shared_ptr.hpp>

using namespace mycp;

namespace entboost {

class EbP2PTaskInfo
{
public:
    EbP2PTaskInfo(void);
    typedef boost::shared_ptr<EbP2PTaskInfo> pointer;
    static EbP2PTaskInfo::pointer create(void);

    mycp::bigint m_nCallId;
    mycp::bigint m_nFromUserId;
    tstring m_sFromLocalIp;
    tstring m_sFromRemoteIp;
    time_t m_tP2PTry;
    bool m_bP2PResponse;
};

}   /// namespace entboost

#endif // EBP2PTASKINFO_H

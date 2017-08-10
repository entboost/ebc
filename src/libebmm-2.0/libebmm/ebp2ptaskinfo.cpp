#include "ebp2ptaskinfo.h"

namespace entboost {

EbP2PTaskInfo::EbP2PTaskInfo()
    : m_nCallId(0), m_nFromUserId(0), m_tP2PTry(0),m_bP2PResponse(false)
{

}

EbP2PTaskInfo::pointer EbP2PTaskInfo::create()
{
    return EbP2PTaskInfo::pointer(new EbP2PTaskInfo());
}

}   /// namespace entboost

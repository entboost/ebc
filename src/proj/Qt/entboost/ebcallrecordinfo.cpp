#include "ebcallrecordinfo.h"

EbCallRecordInfo::EbCallRecordInfo()
    : m_tTime(0)
    , m_listItem(0)
    , m_nFromType(0)
    , m_bRead(true)
    , m_sCallId(0)
    , m_sGroupCode(0)
    , m_sMemberCode(0)
    , m_nFromUserId(0)
    , m_nFromPhone(0)
{

}

EbCallRecordInfo::pointer EbCallRecordInfo::create()
{
    return EbCallRecordInfo::pointer(new EbCallRecordInfo());
}

#ifndef EBCALLRECORDINFO_H
#define EBCALLRECORDINFO_H

#include <ebclientapp.h>
#include <boost/shared_ptr.hpp>

class EbCallRecordInfo
{
public:
    typedef boost::shared_ptr<EbCallRecordInfo> pointer;
    EbCallRecordInfo(void);
    static EbCallRecordInfo::pointer create(void);

    bool isGroupCall(void) const {return m_sGroupCode>0?true:false;}
    eb::bigint groupId(void) const {return m_sGroupCode;}

    eb::bigint m_sCallId;
    time_t m_tTime;
    eb::bigint m_sGroupCode;
    tstring m_sGroupName;
    eb::bigint m_sMemberCode;
    eb::bigint m_nFromUserId;
    eb::bigint m_nFromPhone;
    tstring m_sFromAccount;
    tstring m_sFromName;
    int m_nFromType;
    tstring m_sCompany;
    tstring m_sTitle;
    //tstring m_sPhone;
    tstring m_sTel;
    tstring m_sEmail;
    QListWidgetItem * m_listItem;
    bool m_bRead;
};
const EbCallRecordInfo::pointer EbCallRecordInfoNull;

#endif // EBCALLRECORDINFO_H

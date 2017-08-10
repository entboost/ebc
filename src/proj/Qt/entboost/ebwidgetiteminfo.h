#ifndef EBWIDGETITEMINFO_H
#define EBWIDGETITEMINFO_H

#include <boost/shared_ptr.hpp>
#include <ebclientapp.h>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

class EbWidgetItemInfo
{
public:
    typedef boost::shared_ptr<EbWidgetItemInfo> pointer;
    enum SORT_FLAG {
        SORT_BY_DEFAULT,
        SORT_BY_TIME        /// for MySession
    };

    enum ITEM_TYPE {
        ITEM_TYPE_ENTERPRISE
        , ITEM_TYPE_GROUP
        , ITEM_TYPE_MEMBER
        , ITEM_TYPE_CONTACT
        , ITEM_TYPE_URL
        , ITEM_TYPE_DIR
        , ITEM_TYPE_FILE
        , ITEM_TYPE_NOTE
        , ITEM_TYPE_OTHERRES
        , ITEM_TYPE_SUBINFO
        , ITEM_TYPE_EMAIL
        , ITEM_TYPE_SUBMSG
    };
    enum ITEM_EXT_DATA
    {
        ITEM_EXT_DATA_FORBID_SPEECH = 0x1	/// 禁言
    };

    EbWidgetItemInfo(ITEM_TYPE nItemType, int nIndex);
    EbWidgetItemInfo(ITEM_TYPE nItemType, QListWidgetItem *hItem);
    EbWidgetItemInfo(ITEM_TYPE nItemType, QTreeWidgetItem *hItem);
    EbWidgetItemInfo(const EB_ContactInfo *contactInfo, QListWidgetItem *hItem);
    EbWidgetItemInfo(const EB_ContactInfo *contactInfo, QTreeWidgetItem *hItem);
    EbWidgetItemInfo(const EB_GroupInfo *groupInfo, QTreeWidgetItem *hItem);
    EbWidgetItemInfo(const EB_MemberInfo *memberInfo, QListWidgetItem *hItem);
    EbWidgetItemInfo(const EB_SubscribeFuncInfo *funcInfo, QListWidgetItem *hItem);
    EbWidgetItemInfo(void);
    static EbWidgetItemInfo::pointer create(ITEM_TYPE nItemType, int nIndex);
    static EbWidgetItemInfo::pointer create(ITEM_TYPE nItemType, QListWidgetItem *hItem);
    static EbWidgetItemInfo::pointer create(ITEM_TYPE nItemType, QTreeWidgetItem *hItem);
    static EbWidgetItemInfo::pointer create(const EB_ContactInfo *contactInfo, QListWidgetItem *hItem);
    static EbWidgetItemInfo::pointer create(const EB_ContactInfo *contactInfo, QTreeWidgetItem *hItem);
    static EbWidgetItemInfo::pointer create(const EB_GroupInfo *groupInfo, QTreeWidgetItem *hItem);
    static EbWidgetItemInfo::pointer create(const EB_MemberInfo *memberInfo, QListWidgetItem *hItem);
    static EbWidgetItemInfo::pointer create(const EB_SubscribeFuncInfo *funcInfo, QListWidgetItem *hItem);

    void operator =(const EbWidgetItemInfo *itemInfo);
    bool operator <(const EbWidgetItemInfo *itemInfo);

    static int memberSubType(const EB_MemberInfo *memberInfo);
    static int memberSubType(EB_GROUP_TYPE groupType, eb::bigint groupId, eb::bigint memberUserId);
    void updateMemberInfo(const EB_MemberInfo *memberInfo, bool fromMySession=false);
    void updateContactInfo(const EB_ContactInfo *contactInfo);

    ITEM_TYPE m_nItemType;
    int m_nSubType;
    QTreeWidgetItem * m_hItem;
    QListWidgetItem * m_listItem;
    int m_nIndex;			// for list
    mycp::bigint m_sEnterpriseCode;
    mycp::bigint m_sGroupCode;
    mycp::bigint m_sMemberCode;
    CEBString m_sGroupName;
    CEBString m_sName;	// group_name,emp_name,res_name...
    CEBString m_sAccount;	// time,...
    mycp::bigint m_nUserId;
    mycp::bigint m_nBigId;
    mycp::bigint m_sId;			// resid,...
    mycp::bigint m_sParentId;
    unsigned long m_dwItemData;
    int m_nExtData;
    int m_nCount1;
    int m_nCount2;
    QVariant m_headMd5;
    SORT_FLAG m_sortFlag;
    time_t m_time;
};
const EbWidgetItemInfo::pointer EbWidgetItemInfoNull;

#endif // EBWIDGETITEMINFO_H

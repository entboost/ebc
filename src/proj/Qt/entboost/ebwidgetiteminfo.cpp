#include "ebwidgetiteminfo.h"

EbWidgetItemInfo::EbWidgetItemInfo(EbWidgetItemInfo::ITEM_TYPE nItemType, int nIndex)
    : m_nItemType(nItemType),m_nSubType(0)
    , m_hItem(NULL)
    , m_listItem(0)
    , m_nIndex(nIndex)
    , m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
    , m_nUserId(0),m_nBigId(0)
    , m_sId(0),m_sParentId(0)
    , m_dwItemData(0), m_nExtData(0)
    , m_nCount1(0), m_nCount2(0)
//        , m_pHeadImage(NULL)
    , m_sortFlag(SORT_BY_DEFAULT)
    , m_time(0)
{

}

EbWidgetItemInfo::EbWidgetItemInfo(EbWidgetItemInfo::ITEM_TYPE nItemType, QListWidgetItem *hItem)
    : m_nItemType(nItemType),m_nSubType(0)
    , m_hItem(0)
    , m_listItem(hItem)
    , m_nIndex(0)
    , m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
    , m_nUserId(0),m_nBigId(0)
    , m_sId(0),m_sParentId(0)
    , m_dwItemData(0), m_nExtData(0)
    , m_nCount1(0), m_nCount2(0)
//        , m_pHeadImage(NULL)
    , m_sortFlag(SORT_BY_DEFAULT)
    , m_time(0)
{

}

EbWidgetItemInfo::EbWidgetItemInfo(const EB_ContactInfo *contactInfo, QListWidgetItem *hItem)
    : m_nItemType(EbWidgetItemInfo::ITEM_TYPE_CONTACT),m_nSubType(contactInfo->m_nContactType)
    , m_hItem(0)
    , m_listItem(hItem)
    , m_nIndex(0)
    , m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
    , m_sName(contactInfo->m_sName)
    , m_sAccount(contactInfo->m_sContact)
    , m_nUserId(contactInfo->m_nContactUserId),m_nBigId(0)
    , m_sId(contactInfo->m_nContactId),m_sParentId(0)
    , m_dwItemData(contactInfo->m_nLineState), m_nExtData(0)
    , m_nCount1(0), m_nCount2(0)
    , m_sortFlag(SORT_BY_DEFAULT)
    , m_time(0)
{

}

EbWidgetItemInfo::EbWidgetItemInfo(const EB_ContactInfo *contactInfo, QTreeWidgetItem *hItem)
    : m_nItemType(EbWidgetItemInfo::ITEM_TYPE_CONTACT),m_nSubType(contactInfo->m_nContactType)
    , m_hItem(hItem)
    , m_listItem(0)
    , m_nIndex(0)
    , m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
    , m_sName(contactInfo->m_sName)
    , m_sAccount(contactInfo->m_sContact)
    , m_nUserId(contactInfo->m_nContactUserId),m_nBigId(0)
    , m_sId(contactInfo->m_nContactId),m_sParentId(0)
    , m_dwItemData(contactInfo->m_nLineState), m_nExtData(0)
    , m_nCount1(0), m_nCount2(0)
    , m_sortFlag(SORT_BY_DEFAULT)
    , m_time(0)

{
}

EbWidgetItemInfo::EbWidgetItemInfo(const EB_GroupInfo *groupInfo, QTreeWidgetItem *hItem)
    : m_nItemType(EbWidgetItemInfo::ITEM_TYPE_GROUP),m_nSubType(groupInfo->m_nGroupType)
    , m_hItem(hItem)
    , m_listItem(0)
    , m_nIndex(0)
    , m_sEnterpriseCode(groupInfo->m_sEnterpriseCode),m_sGroupCode(groupInfo->m_sGroupCode),m_sMemberCode(0)
    , m_nUserId(0),m_nBigId(groupInfo->m_nMyEmpId)
    , m_sId(0),m_sParentId(groupInfo->m_sParentCode)
    , m_dwItemData(0), m_nExtData(0)
    , m_nCount1(0), m_nCount2(0)
    , m_sortFlag(SORT_BY_DEFAULT)
    , m_time(0)
{
}

EbWidgetItemInfo::EbWidgetItemInfo(const EB_MemberInfo *memberInfo, QListWidgetItem *hItem)
    : m_nItemType(EbWidgetItemInfo::ITEM_TYPE_MEMBER),m_nSubType(0)
    , m_hItem(0)
    , m_listItem(hItem)
    , m_nIndex(0)
    , m_sEnterpriseCode(0),m_sGroupCode(memberInfo->m_sGroupCode),m_sMemberCode(memberInfo->m_sMemberCode)
    , m_sName(memberInfo->m_sUserName)
    , m_sAccount(memberInfo->m_sMemberAccount)
    , m_nUserId(memberInfo->m_nMemberUserId),m_nBigId(0)
    , m_sId(0),m_sParentId(0)
    , m_dwItemData(0), m_nExtData(0)
    , m_nCount1(0), m_nCount2(0)
    , m_sortFlag(SORT_BY_DEFAULT)
    , m_time(0)
{

}

EbWidgetItemInfo::EbWidgetItemInfo(const EB_SubscribeFuncInfo *funcInfo, QListWidgetItem *hItem)
    : m_nItemType(EbWidgetItemInfo::ITEM_TYPE_SUBINFO),m_nSubType(0)
    , m_hItem(0)
    , m_listItem(hItem)
    , m_nIndex(0)
    , m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
    , m_sGroupName(funcInfo->m_sResFile.toStdString())
    , m_sName(funcInfo->m_sFunctionName)
    , m_sAccount(funcInfo->m_sFunctionName)
    , m_nUserId(0),m_nBigId(0)
    , m_sId(funcInfo->m_nSubscribeId),m_sParentId(0)
    , m_dwItemData(0), m_nExtData(0)
    , m_nCount1(0), m_nCount2(0)
    , m_sortFlag(SORT_BY_DEFAULT)
    , m_time(0)
{

}

EbWidgetItemInfo::EbWidgetItemInfo(EbWidgetItemInfo::ITEM_TYPE nItemType, QTreeWidgetItem *hItem)
    : m_nItemType(nItemType),m_nSubType(0)
    , m_hItem(hItem)
    , m_listItem(0)
    , m_nIndex(0)
    , m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
    , m_nUserId(0),m_nBigId(0)
    , m_sId(0),m_sParentId(0)
    , m_dwItemData(0), m_nExtData(0)
    , m_nCount1(0), m_nCount2(0)
//        , m_pHeadImage(NULL)
    , m_sortFlag(SORT_BY_DEFAULT)
    , m_time(0)
{

}

EbWidgetItemInfo::EbWidgetItemInfo()
    : m_nItemType(ITEM_TYPE_CONTACT),m_nSubType(0)
    , m_hItem(NULL)
    , m_listItem(0)
    , m_nIndex(0)
    , m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
    , m_nUserId(0),m_nBigId(0)
    , m_sId(0),m_sParentId(0)
    , m_dwItemData(0), m_nExtData(0)
    , m_nCount1(0), m_nCount2(0)
//        , m_pHeadImage(NULL)
    , m_sortFlag(SORT_BY_DEFAULT)
    , m_time(0)
{

}

EbWidgetItemInfo::pointer EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE nItemType, int nIndex)
{
    return EbWidgetItemInfo::pointer(new EbWidgetItemInfo(nItemType,nIndex));
}

EbWidgetItemInfo::pointer EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE nItemType, QListWidgetItem *hItem)
{
    return EbWidgetItemInfo::pointer(new EbWidgetItemInfo(nItemType,hItem));
}

EbWidgetItemInfo::pointer EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE nItemType, QTreeWidgetItem *hItem)
{
    return EbWidgetItemInfo::pointer(new EbWidgetItemInfo(nItemType,hItem));
}

EbWidgetItemInfo::pointer EbWidgetItemInfo::create(const EB_ContactInfo *contactInfo, QListWidgetItem *hItem)
{
    return EbWidgetItemInfo::pointer(new EbWidgetItemInfo(contactInfo,hItem));
}

EbWidgetItemInfo::pointer EbWidgetItemInfo::create(const EB_ContactInfo *contactInfo, QTreeWidgetItem *hItem)
{
    return EbWidgetItemInfo::pointer(new EbWidgetItemInfo(contactInfo,hItem));
}

EbWidgetItemInfo::pointer EbWidgetItemInfo::create(const EB_GroupInfo *groupInfo, QTreeWidgetItem *hItem)
{
    return EbWidgetItemInfo::pointer(new EbWidgetItemInfo(groupInfo,hItem));
}

EbWidgetItemInfo::pointer EbWidgetItemInfo::create(const EB_MemberInfo *memberInfo, QListWidgetItem *hItem)
{
    return EbWidgetItemInfo::pointer(new EbWidgetItemInfo(memberInfo,hItem));
}

EbWidgetItemInfo::pointer EbWidgetItemInfo::create(const EB_SubscribeFuncInfo *funcInfo, QListWidgetItem *hItem)
{
    return EbWidgetItemInfo::pointer(new EbWidgetItemInfo(funcInfo,hItem));
}

void EbWidgetItemInfo::operator =(const EbWidgetItemInfo *pItemInfo)
{
    if ( pItemInfo!=0 ) {
        m_nItemType = pItemInfo->m_nItemType;
        m_nSubType = pItemInfo->m_nSubType;
        m_hItem = pItemInfo->m_hItem;
        m_listItem = pItemInfo->m_listItem;
        m_nIndex = pItemInfo->m_nIndex;
        m_sEnterpriseCode = pItemInfo->m_sEnterpriseCode;
        m_sGroupCode = pItemInfo->m_sGroupCode;
        m_sMemberCode = pItemInfo->m_sMemberCode;
        m_sName = pItemInfo->m_sName;
        m_sAccount = pItemInfo->m_sAccount;
        m_dwItemData = pItemInfo->m_dwItemData;
        m_nExtData = pItemInfo->m_nExtData;
        m_nCount1 = pItemInfo->m_nCount1;
        m_nCount2 = pItemInfo->m_nCount2;
//        if (m_pHeadImage!=NULL)
//        {
//            delete m_pHeadImage;
//        }
//        if (pItemInfo->m_pHeadImage!=NULL)
//        {
//#ifdef _QT_MAKE_
//            m_pHeadImage = new QImage(*pItemInfo->m_pHeadImage);
//#else
//            m_pHeadImage = pItemInfo->m_pHeadImage->Clone();
//#endif
//        }else
//        {
//            m_pHeadImage = NULL;
//        }
        m_headMd5 = pItemInfo->m_headMd5;
    }
}

bool EbWidgetItemInfo::operator <(const EbWidgetItemInfo *pItemInfo)
{
    if (pItemInfo==0) {
        return false;
    }
    if (m_sortFlag==SORT_BY_TIME && pItemInfo->m_sortFlag==SORT_BY_TIME) {
        return m_time<pItemInfo->m_time?true:false;
    }
    if (this->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR && pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR) {
//            const bool bAppFrameShowed = theApp.GetWorkFrameShowed();
//#ifdef USES_SUPPORT_UI_STYLE
//            if (bAppFrameShowed || (this->m_nSubType==1 && pItemInfo->m_nSubType==1))
//#else
//            if (bAppFrameShowed)
//#endif
//                return this->m_dwItemData<pItemInfo->m_dwItemData?1:-1;
//            else
//                return this->m_dwItemData>pItemInfo->m_dwItemData?1:-1;
    }
    else if (this->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP && pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP) {
        /// 群组（部门）排序
        if (this->m_nSubType!=pItemInfo->m_nSubType)						/// a 先按类型排序
            return this->m_nSubType<pItemInfo->m_nSubType?true:false;
        else if (this->m_nIndex!=pItemInfo->m_nIndex)					/// b 再按排序比较
            return this->m_nIndex<pItemInfo->m_nIndex?false:true;        /// 排序值高显示在前面（小于对调位置）
        else																/// c 最后按名称gbk排序
            return this->m_sName<pItemInfo->m_sName;
//                return strcmp(this->m_sName.c_str(), pItemInfo->m_sName.c_str());
    }
    else if ((this->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER) ||
        (this->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT && pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)) {
        if (this->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER) {
            /// 员工按照权限类型比较排序
            if (this->m_nIndex!=pItemInfo->m_nIndex) {					/// a 先按排序比较
                return this->m_nIndex<pItemInfo->m_nIndex?false:true;    /// 排序值高显示在前面（小于对调位置）
            }
            else if (this->m_nSubType!=pItemInfo->m_nSubType) {			/// b 再按权限类型排序
                return this->m_nSubType<pItemInfo->m_nSubType?false:true;    /// 权限小排后面，对调位置
            }
        }
        else {
            /// 联系人，按照有没有UID排序；
            if (pItemInfo->m_nUserId>0 && this->m_nUserId==0) return false; 		/// 第2个有UID，对换位置
            else if (this->m_nUserId>0 && pItemInfo->m_nUserId==0) return true;	/// 第2个0，不用对换位置
        }

        /// 员工比较，先比较在线状态
        if ((this->m_dwItemData==EB_LINE_STATE_ONLINE_NEW && pItemInfo->m_dwItemData<EB_LINE_STATE_ONLINE_NEW) ||
            (this->m_dwItemData>=EB_LINE_STATE_BUSY && pItemInfo->m_dwItemData<EB_LINE_STATE_BUSY)) {
            /// a 第一个在线，第二个其他 不用换位置
            /// b 第一个离开/忙，第二个离线 不用换位置
            return true;
        }
        else if ((pItemInfo->m_dwItemData==EB_LINE_STATE_ONLINE_NEW && this->m_dwItemData<EB_LINE_STATE_ONLINE_NEW) ||
            (pItemInfo->m_dwItemData>=EB_LINE_STATE_BUSY && this->m_dwItemData<EB_LINE_STATE_BUSY)) {
            /// a 第2个在线，第1个其他 对调位置
            /// b 第2个离开/忙，第1个离线 对调位置
            return false;
        }
        return this->m_sName<pItemInfo->m_sName;
//            return strcmp(this->m_sName.c_str(), pItemInfo->m_sName.c_str());
    }
    else if (this->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP || this->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR) {
        /// 第一个是部门（目录），排前面（不用调位置）
        return true;
    }
    else if (pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP || pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR) {
        /// 第二个是部门（目录），对调位置
        return false;
    }
    else if (this->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE && pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE) {
        return this->m_nBigId<pItemInfo->m_nBigId?false:true;		// 时间大，排前面（小于对调位置）
    }
    return true;
}

int EbWidgetItemInfo::memberSubType(const EB_MemberInfo *memberInfo)
{
    EB_GROUP_TYPE groupType = EB_GROUP_TYPE_DEPARTMENT;
    theApp->m_ebum.EB_GetGroupType(memberInfo->m_sGroupCode,groupType);
    return memberSubType( groupType, memberInfo->m_sGroupCode, memberInfo->m_nMemberUserId );
}

int EbWidgetItemInfo::memberSubType(EB_GROUP_TYPE groupType, eb::bigint groupId, eb::bigint memberUserId)
{
    int nSubType = 0;
    if (groupType<=EB_GROUP_TYPE_PROJECT && theApp->isEnterpriseCreateUserId(memberUserId))
        nSubType = 11;
    else if (theApp->m_ebum.EB_IsGroupCreator(groupId, memberUserId))
        nSubType = 10;
    else if (theApp->m_ebum.EB_IsGroupManager(groupId, memberUserId))
        nSubType = 10;
    else if (theApp->m_ebum.EB_IsGroupAdminLevel(groupId, memberUserId))
        nSubType = 9;
    else if (theApp->logonUserId()==memberUserId)
        nSubType = 1;
    else
        nSubType = 0;
    return nSubType;
}

void EbWidgetItemInfo::updateMemberInfo(const EB_MemberInfo *memberInfo, bool fromMySession)
{
    const int nSubType = fromMySession?0:memberSubType(memberInfo);
    const QString sImagePath = memberInfo->m_sHeadResourceFile;
    const EB_USER_LINE_STATE pOutLineState = memberInfo->m_nLineState;
    const QString sHeadMd5 = QString::fromStdString(memberInfo->m_sHeadMd5.string());
    /// 在线状态图标
    bool viewGrayImage = false;
    QImage imageLineState;
    switch (pOutLineState)
    {
    case EB_LINE_STATE_UNKNOWN:
    case EB_LINE_STATE_OFFLINE:
        viewGrayImage = true;
        break;
    case EB_LINE_STATE_ONLINE_NEW:
        break;
    case EB_LINE_STATE_BUSY:
        imageLineState = QImage(":/img/btnstatebusy.png");
        break;
    case EB_LINE_STATE_AWAY:
        imageLineState = QImage(":/img/btnstateaway.png");
        break;
    default:
        break;
    }

    /// 禁言图标
    bool updateForbidSpeech = false;
    QImage imageForbid;
    if ((memberInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)==0) {
        updateForbidSpeech = (this->m_nExtData&EbWidgetItemInfo::ITEM_EXT_DATA_FORBID_SPEECH)==0?false:true;
        this->m_nExtData &= ~EbWidgetItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;
    }
    else {
        updateForbidSpeech = (this->m_nExtData&EbWidgetItemInfo::ITEM_EXT_DATA_FORBID_SPEECH)==0?true:false;
        this->m_nExtData |= EbWidgetItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;
        imageForbid = QImage(":/img/imgstateforbid.png");
    }

//    const QVariant variantFileMd5 = this->m_hItem->data( 0,EB_WIDGET_ITEM_USER_ROLE_FILE_MD5 );
    if (!sImagePath.isEmpty() && QFile::exists(sImagePath)) {
        if (updateForbidSpeech || memberInfo->m_nLineState!=(EB_USER_LINE_STATE)this->m_dwItemData ||
                !m_headMd5.isValid() || m_headMd5.toString()!= sHeadMd5) {
            m_headMd5 = QVariant(sHeadMd5);
//            this->m_hItem->setData( 0,EB_WIDGET_ITEM_USER_ROLE_FILE_MD5,QVariant(sHeadMd5) );
            QImage image = QImage(sImagePath).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            if ( !imageForbid.isNull() ) {
                image = libEbc::imageAdd(image,imageForbid,0,image.height()-imageForbid.height()+1);
            }
            if (viewGrayImage) {
                /// 离线状态，显示灰色头像
                if (this->m_hItem!=0)
                    this->m_hItem->setIcon( 0, QIcon( QPixmap::fromImage(libEbc::imageToGray(image))) );
                else if (this->m_listItem!=0)
                    this->m_listItem->setIcon( QIcon( QPixmap::fromImage(libEbc::imageToGray(image))) );
            }
            else {
                if (!imageLineState.isNull()) {
                    image = libEbc::imageAdd(image,imageLineState,image.width()-imageLineState.width()+3,image.height()-imageLineState.height()+3);
                }
                if (this->m_hItem!=0)
                    this->m_hItem->setIcon( 0, QIcon(QPixmap::fromImage(image)) );
                else if (this->m_listItem!=0)
                    this->m_listItem->setIcon( QIcon(QPixmap::fromImage(image)) );
            }
        }
    }
    else {
        if (updateForbidSpeech || memberInfo->m_nLineState!=(EB_USER_LINE_STATE)this->m_dwItemData ||
                !m_headMd5.isValid() || !m_headMd5.toString().isEmpty()) {
            m_headMd5 = QVariant("");
//            this->m_hItem->setData( 0,EB_WIDGET_ITEM_USER_ROLE_FILE_MD5,QVariant(QString()) );
            QImage image = QImage(":/img/defaultmember.png").scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            if ( !imageForbid.isNull() ) {
                image = libEbc::imageAdd(image,imageForbid,0,image.height()-imageForbid.height()+1);
            }
            if (viewGrayImage) {
                /// 离线状态，显示灰色头像
                if (this->m_hItem!=0)
                    this->m_hItem->setIcon( 0, QIcon(QPixmap::fromImage(libEbc::imageToGray(image))) );
                else if (this->m_listItem!=0)
                    this->m_listItem->setIcon( QIcon(QPixmap::fromImage(libEbc::imageToGray(image))) );
            }
            else {
                if (!imageLineState.isNull()) {
                    image = libEbc::imageAdd(image,imageLineState,image.width()-imageLineState.width()+3,image.height()-imageLineState.height()+3);
                }
                if (this->m_hItem!=0)
                    this->m_hItem->setIcon( 0, QIcon(QPixmap::fromImage(image)) );
                else if (this->m_listItem!=0)
                    this->m_listItem->setIcon( QIcon(QPixmap::fromImage(image)) );
            }
        }
    }
    this->m_sAccount = memberInfo->m_sMemberAccount;
    this->m_nUserId = memberInfo->m_nMemberUserId;
    this->m_sName = memberInfo->m_sUserName;
    this->m_dwItemData = memberInfo->m_nLineState;
    if (fromMySession) {
        return;
    }
    this->m_nIndex = memberInfo->m_nDisplayIndex;
    this->m_nSubType = nSubType;
    this->m_sGroupCode = memberInfo->m_sGroupCode;
    this->m_sMemberCode = memberInfo->m_sMemberCode;
    QColor textColor(0,0,0);
    if (this->m_nSubType>1) {
        /// 管理权限
        textColor = QColor(255,0,128);
    }
    else if (this->m_nSubType==1) {
        /// 自己
        textColor = QColor(0,0,255);
    }
//    else {
//        /// 其他
//        this->m_hItem->setTextColor( 0,QColor(0,0,0) );
//    }
    if (this->m_hItem!=0)
        this->m_hItem->setTextColor( 0, textColor );
    else if (this->m_listItem!=0)
        this->m_listItem->setTextColor( textColor );
}

void EbWidgetItemInfo::updateContactInfo(const EB_ContactInfo *contactInfo)
{
    const QString sImagePath = contactInfo->m_sHeadResourceFile;
    const EB_USER_LINE_STATE pOutLineState = contactInfo->m_nLineState;
    const QString sHeadMd5 = QString::fromStdString(contactInfo->m_sHeadMd5.string());
    /// 在线状态图标
    bool viewGrayImage = false;
    QImage imageLineState;
    switch (pOutLineState)
    {
    case EB_LINE_STATE_UNKNOWN:
    case EB_LINE_STATE_OFFLINE:
        viewGrayImage = true;
        break;
    case EB_LINE_STATE_ONLINE_NEW:
        break;
    case EB_LINE_STATE_BUSY:
        imageLineState = QImage(":/img/btnstatebusy.png");
        break;
    case EB_LINE_STATE_AWAY:
        imageLineState = QImage(":/img/btnstateaway.png");
        break;
    default:
        break;
    }

//    const QVariant variantFileMd5 = this->m_hItem->data( 0,EB_WIDGET_ITEM_USER_ROLE_FILE_MD5 );
    if (!sImagePath.isEmpty() && QFile::exists(sImagePath)) {
        if (contactInfo->m_nLineState!=(EB_USER_LINE_STATE)this->m_dwItemData ||
                !m_headMd5.isValid() || m_headMd5.toString()!= sHeadMd5) {
            m_headMd5 = QVariant(sHeadMd5);
            QImage image = QImage(sImagePath).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            if (viewGrayImage) {
                /// 离线状态，显示灰色头像
                if (this->m_hItem!=0)
                    this->m_hItem->setIcon( 0, QIcon( QPixmap::fromImage(libEbc::imageToGray(image))) );
                else if (this->m_listItem!=0)
                    this->m_listItem->setIcon( QIcon( QPixmap::fromImage(libEbc::imageToGray(image))) );
            }
            else {
                if (!imageLineState.isNull()) {
                    image = libEbc::imageAdd(image,imageLineState,image.width()-imageLineState.width()+3,image.height()-imageLineState.height()+3);
                }
                if (this->m_hItem!=0)
                    this->m_hItem->setIcon( 0, QIcon(QPixmap::fromImage(image)) );
                else if (this->m_listItem!=0)
                    this->m_listItem->setIcon( QIcon(QPixmap::fromImage(image)) );
            }
        }
    }
    else {
        if (contactInfo->m_nLineState!=(EB_USER_LINE_STATE)this->m_dwItemData ||
                !m_headMd5.isValid() || !m_headMd5.toString().isEmpty()) {
            m_headMd5 = QVariant("");
            QImage image = QImage(":/img/defaultcontact.png").scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            if (viewGrayImage) {
                /// 离线状态，显示灰色头像
                if (this->m_hItem!=0)
                    this->m_hItem->setIcon( 0, QIcon(QPixmap::fromImage(libEbc::imageToGray(image))) );
                else if (this->m_listItem!=0)
                    this->m_listItem->setIcon( QIcon(QPixmap::fromImage(libEbc::imageToGray(image))) );
            }
            else {
                if (!imageLineState.isNull()) {
                    image = libEbc::imageAdd(image,imageLineState,image.width()-imageLineState.width()+3,image.height()-imageLineState.height()+3);
                }
                if (this->m_hItem!=0)
                    this->m_hItem->setIcon( 0, QIcon(QPixmap::fromImage(image)) );
                else if (this->m_listItem!=0)
                    this->m_listItem->setIcon( QIcon(QPixmap::fromImage(image)) );
            }
        }
    }

    m_nSubType = contactInfo->m_nContactType;
    m_nUserId = contactInfo->m_nContactUserId;
    m_sId = contactInfo->m_nContactId;
    m_sParentId = contactInfo->m_nUGId;
    m_sGroupName = contactInfo->m_sGroupName;
    m_dwItemData = contactInfo->m_nLineState;
    m_sName = contactInfo->m_sName;
    m_sAccount = contactInfo->m_sContact;
}


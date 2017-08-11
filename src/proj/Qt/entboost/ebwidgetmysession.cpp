#include "ebwidgetmysession.h"
#include <eblistwidgetitem.h>
#include <ebmessagebox.h>
#include <ebdialogrequestaddcontact.h>
#include <ebdialogcontactinfo.h>
#include <ebdialogviewecard.h>
#include <ebiconhelper.h>

EbWidgetMySession::EbWidgetMySession(QWidget *parent) : QWidget(parent)
  , m_contextMenu(0)
  , m_bReloadRecordData(false)
{
    m_listWidget = new QListWidget(this);
    m_listWidget->setIconSize(const_tree_icon_size);
    m_listWidget->setSortingEnabled(false);
    m_listWidget->setMouseTracking(true);
    m_listWidget->setWordWrap(true);
    m_listWidget->setObjectName("MessageTipList");
    connect( m_listWidget,SIGNAL(itemEntered(QListWidgetItem*)),this,SLOT(onItemEntered(QListWidgetItem*)) );
    connect( m_listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(onItemDoubleClicked(QListWidgetItem*)) );

    /// “打开会话”按钮
    m_buttonCall = new QPushButton(this);
    m_buttonCall->setParent( m_listWidget );
    m_buttonCall->setVisible(false);
    m_buttonCall->setObjectName("CallButton");
    connect( m_buttonCall, SIGNAL(clicked()),this,SLOT(onClickedButtonCall()) );
    EbIconHelper::Instance()->SetIcon(m_buttonCall,QChar(0xf27a),12 );
    /// 申请加为好友 按钮
    m_buttonAddContact = new QPushButton(this);
    m_buttonAddContact->setParent( m_listWidget );
    m_buttonAddContact->setVisible(false);
    m_buttonAddContact->setObjectName("CallButton");
    connect( m_buttonAddContact, SIGNAL(clicked()),this,SLOT(onClickedButtonAddContact()) );
    EbIconHelper::Instance()->SetIcon(m_buttonAddContact,QChar(0xf067),12 );
    /// “删除聊天记录” 按钮
    m_buttonDelete = new QPushButton(this);
    m_buttonDelete->setParent( m_listWidget );
    m_buttonDelete->setVisible(false);
    m_buttonDelete->setObjectName("CallDelButton");
    connect( m_buttonDelete, SIGNAL(clicked()),this,SLOT(onClickedButtonDelete()) );
    EbIconHelper::Instance()->SetIcon(m_buttonDelete,QChar(0xf014),12 );

    QTimer::singleShot( 100,this,SLOT(onLoadCallRecord()) );

    updateLocaleInfo();
}

EbWidgetMySession::~EbWidgetMySession()
{
}

void EbWidgetMySession::updateLocaleInfo()
{
    m_buttonCall->setToolTip( theLocales.getLocalText("main-frame.button-call.tooltip","Open Chat") );
    m_buttonAddContact->setToolTip( theLocales.getLocalText("main-frame.button-add-contact.tooltip","Add Contact") );
    m_buttonDelete->setToolTip( theLocales.getLocalText("main-frame.button-delete-session.tooltip","Delete Session") );

}

void EbWidgetMySession::timerCheckState()
{
    if ( this->isVisible() &&
         (m_buttonCall->isVisible() || m_buttonAddContact->isVisible() || m_buttonDelete->isVisible()) ) {
        /// 实现定期自动判断当前鼠标位置，并刷新 call 按钮
        const QRect& rect = m_listWidget->geometry();
        const QPoint pointMouseToDialog = this->mapFromGlobal(this->cursor().pos());
        if (!rect.contains(pointMouseToDialog)) {
            m_buttonCall->setVisible(false);
            m_buttonAddContact->setVisible(false);
            m_buttonDelete->setVisible(false);
        }
    }
}

void EbWidgetMySession::insertCallRecord(const EbCallRecordInfo::pointer &pCallRecordInfo, bool bInsertNew,bool sort)
{
    EbCallRecordInfo::pointer pOldCallInfo = callRecordInfo(pCallRecordInfo->m_sGroupCode,
                                                             pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_nFromType);
    if (pOldCallInfo.get()!=0) {
        if (bInsertNew) {
            m_pCallRecordInfo.remove(pOldCallInfo->m_listItem);
            QListWidgetItem * item = m_listWidget->takeItem( m_listWidget->row(pOldCallInfo->m_listItem) );
            if (item!=0) {
                delete item;
            }
        }
        if (pCallRecordInfo->m_sGroupCode>0 || !is_visitor_uid(pCallRecordInfo->m_nFromUserId)) {
            if (bInsertNew) {
                char sql[128];
                sprintf( sql, "DELETE FROM call_record_t WHERE call_id=%lld",pOldCallInfo->m_sCallId);
                theApp->m_sqliteUser->execute(sql);
            }
        }
        if (!bInsertNew) {
            return;
        }
    }

    /// 获取日期字符串
    const QDateTime pNow = QDateTime::currentDateTime();
    const QDate nowDate = pNow.date();
    const QDateTime pTime = QDateTime::fromTime_t(pCallRecordInfo->m_tTime);
    const QDate timeDate = pTime.date();
    QString sCallTime;
    if (nowDate.year()==timeDate.year() && nowDate.month()==timeDate.month() && nowDate.day()==timeDate.day()) {
        /// today
        sCallTime = pTime.toString("HH:mm");
    }
    else if (nowDate.year()==timeDate.year() && nowDate.month()==timeDate.month() && nowDate.day()==timeDate.day()+1)
    {
        /// 昨天 %H:%M
        sCallTime = QString("%1 %2").arg( theLocales.getLocalText("name-text.yesterday","Yesterday")).arg(pTime.toString("HH:mm"));
    }
    else if (nowDate.year()==timeDate.year()) {
        /// 今年
        sCallTime = pTime.toString("MM-dd");
    }
    else {
        sCallTime = pTime.toString("yyyy-MM-dd");
    }
    EbWidgetItemInfo::pointer itemInfo;
    QImage fromImage;
    QString sItemText;
    if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0) {
        /// 验证消息
        itemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_SUBMSG,-1);;
        itemInfo->m_nSubType = pCallRecordInfo->m_nFromType;
        fromImage = QImage(":/img/defaultcontact.png");
        switch (pCallRecordInfo->m_nFromType) {
        case EBC_MSG_TYPE_BC_MSG: {
            /// 广播消息\n%s
            sItemText = theLocales.getLocalText("pop-tip-dialog.type-bc-message", "Broadcast Message");
            sItemText.replace("[MESSAGE_TITLE]", pCallRecordInfo->m_sGroupName.c_str());
            break;
        }
        case EBC_MSG_TYPE_REQUEST_JOIN_2_GROUP: {
            /// 验证消息\n%s申请加入群组%lld
            sItemText = theLocales.getLocalText("pop-tip-dialog.type-request-join-to-group", "Request join to group");
            sItemText.replace("[USER_ACCOUNT]", pCallRecordInfo->m_sFromAccount.c_str());
            sItemText.replace("[GROUP_ID]", QString::number(pCallRecordInfo->m_sGroupCode));
            break;
        }
        case EBC_MSG_TYPE_INVITE_JOIN_2_GROUP: {
            /// 验证消息\n%s邀请加入群组%lld
            sItemText = theLocales.getLocalText("pop-tip-dialog.type-invite-join-to-group", "Invite join to group");
            sItemText.replace("[USER_ACCOUNT]", pCallRecordInfo->m_sFromAccount.c_str());
            sItemText.replace("[GROUP_ID]", QString::number(pCallRecordInfo->m_sGroupCode));
            break;
        }
        case EBC_MSG_TYPE_REQUEST_ADD_CONTACT: {
            /// 验证消息\n%s申请加为好友
            sItemText = theLocales.getLocalText("pop-tip-dialog.type-request-add-contact", "Request add contact");
            sItemText.replace("[USER_ACCOUNT]", pCallRecordInfo->m_sFromAccount.c_str());
            break;
        }
        //case EBC_MSG_TYPE_ACCERPT_ADD_CONTACT:
        //	sItemText.Format(_T("通知消息\n%s成功加为好友"),pCallRecordInfo->m_sFromAccount.c_str());
        //	break;
        case EBC_MSG_TYPE_REJECT_ADD_CONTACT: {
            /// 通知消息\n%s拒绝加为好友
            sItemText = theLocales.getLocalText("pop-tip-dialog.type-reject-add-contact", "Reject add contact");
            sItemText.replace("[USER_ACCOUNT]", pCallRecordInfo->m_sFromAccount.c_str());
            break;
        }
        default: {
            /// 验证消息\n帐号：%s 群组：%lld
            sItemText = theLocales.getLocalText("pop-tip-dialog.type-group-message", "Group message");
            sItemText.replace("[USER_ACCOUNT]", pCallRecordInfo->m_sFromAccount.c_str());
            sItemText.replace("[GROUP_ID]", QString::number(pCallRecordInfo->m_sGroupCode));
            break;
        }
        }
    }
    else if (pCallRecordInfo->isGroupCall()) {
        sItemText = QString("%1 [%2]").arg(pCallRecordInfo->m_sGroupName.c_str()).arg(sCallTime);
        itemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_GROUP,-1);;
        itemInfo->m_sGroupCode = pCallRecordInfo->groupId();

        /// 群组，获取群组图标
        EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
        if (theApp->m_ebum.EB_GetGroupType(pCallRecordInfo->m_sGroupCode,nGroupType)) {
            switch (nGroupType) {
            case EB_GROUP_TYPE_DEPARTMENT:
                fromImage = QImage(":/img/imgdefaultdepartment.png");
                break;
            case EB_GROUP_TYPE_PROJECT:
                fromImage = QImage(":/img/imgdefaultproject.png");
                break;
            case EB_GROUP_TYPE_GROUP:
                fromImage = QImage(":/img/imgdefaultgroup.png");
                break;
            default:
                fromImage = QImage(":/img/imgdefaulttempgroup.png");
                break;
            }
        }

        /// 默认讨论组
        if (fromImage.isNull()) {
            fromImage = QImage(":/img/imgdefaulttempgroup.png");
        }
        if ( theApp->m_ebum.EB_IsGroupForbidSpeech(pCallRecordInfo->groupId()) ) {
            /// 禁言群组，加上禁方图标，显示在右下角
             const QImage imageForbid = QImage(":/img/imgstateforbid.png");
             fromImage = libEbc::imageAdd(fromImage,imageForbid,fromImage.width()-imageForbid.width(),fromImage.height()-imageForbid.height());
        }
//        if (!theApp->m_ebum.EB_IsMyGroup(pCallRecordInfo->groupId())) {
//            /// 不属于本人群组，群组可能已经被删除
//            fromImage = libEbc::imageToGray(fromImage);
//        }
    }
    else {
        sItemText = QString("%1 [%2]").arg(pCallRecordInfo->m_sFromName.c_str()).arg(sCallTime);
        /// 获取用户头像
//        fromImage = theApp->userHeadImage(pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_sMemberCode,pCallRecordInfo->m_sFromAccount);
        if (pCallRecordInfo->m_sMemberCode>0 && theApp->m_ebum.EB_IsExistLocalMemberInfo(pCallRecordInfo->m_sMemberCode))
            itemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_MEMBER,-1);
        else
            itemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_CONTACT,-1);
        itemInfo->m_nUserId = pCallRecordInfo->m_nFromUserId;
        itemInfo->m_sMemberCode = pCallRecordInfo->m_sMemberCode;
        itemInfo->m_sAccount = pCallRecordInfo->m_sFromAccount;
    }
    itemInfo->m_sortFlag = EbWidgetItemInfo::SORT_BY_TIME;
    itemInfo->m_time = pCallRecordInfo->m_tTime;

    EbListWidgetItem * pItem0 = 0;
    if (fromImage.width()>0) {
        const QIcon icon = QIcon( QPixmap::fromImage(fromImage).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
        pItem0 = new EbListWidgetItem( icon,sItemText,m_listWidget );
    }
    else {
        pItem0 = new EbListWidgetItem( sItemText,m_listWidget );
    }
    pCallRecordInfo->m_listItem = pItem0;
    pItem0->setToolTip(sItemText);
    itemInfo->m_listItem = pItem0;
    pItem0->m_itemInfo = itemInfo;
    m_listWidget->insertItem(0,pItem0);
    m_pCallRecordInfo.insert(pItem0,pCallRecordInfo);
    if (itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_MEMBER) {
        EB_MemberInfo memberInfo;
        if (theApp->m_ebum.EB_GetMemberInfoByMemberCode(&memberInfo, pCallRecordInfo->m_sMemberCode) ||
            theApp->m_ebum.EB_GetMemberInfoByUserId2(&memberInfo, pCallRecordInfo->m_nFromUserId)) {
            itemInfo->updateMemberInfo(&memberInfo, true);
        }
        else {
            fromImage = theApp->userHeadImage(pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_sMemberCode,pCallRecordInfo->m_sFromAccount);
            const QIcon icon = QIcon( QPixmap::fromImage(fromImage).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            pItem0->setIcon(icon);
        }
    }
    else if (itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_CONTACT) {
        EB_ContactInfo contactInfo;
        if (theApp->m_ebum.EB_GetContactInfo2(itemInfo->m_nUserId, &contactInfo)) {
            itemInfo->updateContactInfo(&contactInfo);
        }
        else {
            fromImage = theApp->userHeadImage(pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_sMemberCode,pCallRecordInfo->m_sFromAccount);
            const QIcon icon = QIcon( QPixmap::fromImage(fromImage).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            pItem0->setIcon(icon);
        }
    }
    if (sort) {
        m_listWidget->sortItems(Qt::DescendingOrder);
    }
}

EbCallRecordInfo::pointer EbWidgetMySession::callRecordInfo(eb::bigint groupId, eb::bigint userId, int msgType) const
{
    BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_pCallRecordInfo.mutex()));
    CLockMap<const QListWidgetItem*, EbCallRecordInfo::pointer>::const_iterator pIter = m_pCallRecordInfo.begin();
    for (; pIter!=m_pCallRecordInfo.end(); pIter++) {
        const EbCallRecordInfo::pointer& pCallRecordInfo = pIter->second;
        if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0) {
            /// 验证消息
            if (msgType>0)
            ////if (bSysMsg)
            {
                if ((groupId>0 && groupId==pCallRecordInfo->m_sGroupCode) ||
                    (userId>0 && userId==pCallRecordInfo->m_nFromUserId) ||
                    (msgType==EBC_MSG_TYPE_BC_MSG && pCallRecordInfo->m_nFromType==EBC_MSG_TYPE_BC_MSG))	/// 消息广播只显示当前一条
                {
                    return pCallRecordInfo;
                }
            }
            continue;
        }
        if (groupId>0) {
            /// 查询群组
            if (pCallRecordInfo->m_sGroupCode==groupId) {
                return pCallRecordInfo;
            }
        }
        else {
            /// 查询帐号
            if (!pCallRecordInfo->isGroupCall() && pCallRecordInfo->m_nFromUserId==userId)
            {
                return pCallRecordInfo;
            }
        }
    }
    return EbCallRecordInfoNull;
}

void EbWidgetMySession::onMemberInfo(const EB_MemberInfo *memberInfo, bool changeLineState)
{
    if (!changeLineState) {
        return;
    }
    EbCallRecordInfo::pointer recordInfo = callRecordInfo(0, memberInfo->m_nMemberUserId, 0);
    if (recordInfo.get()!=0 && recordInfo->m_listItem!=0) {
        EbListWidgetItem * item = (EbListWidgetItem*)recordInfo->m_listItem;
        item->m_itemInfo->updateMemberInfo(memberInfo);
    }
}

void EbWidgetMySession::onContactStateChanged(const EB_ContactInfo *contactInfo)
{
    EbCallRecordInfo::pointer recordInfo = callRecordInfo(0, contactInfo->m_nContactUserId, 0);
    if (recordInfo.get()!=0 && recordInfo->m_listItem!=0) {
        EbListWidgetItem * item = (EbListWidgetItem*)recordInfo->m_listItem;
        item->m_itemInfo->updateContactInfo(contactInfo);
    }
}

void EbWidgetMySession::onDeleteSession(const EbWidgetItemInfo::pointer &itemInfo)
{
    if (itemInfo.get()==0) {
        return;
    }
    EbCallRecordInfo::pointer pCallRecordInfo;
    if (!m_pCallRecordInfo.find(itemInfo->m_listItem,pCallRecordInfo,true)) {
        return;
    }
    deleteRecord(pCallRecordInfo,true);
    if (m_bReloadRecordData && m_listWidget->count()<20) {
        QTimer::singleShot( 100,this,SLOT(onLoadCallRecord()) );
    }
}

void EbWidgetMySession::onClearSession()
{
    /// 是否确定清空：\r\n所有会话记录和聊天记录吗？
    const QString title = theLocales.getLocalText("message-box.clear-session.title","Clear Session");
    const QString text = theLocales.getLocalText("message-box.clear-session.text","Confirm Clear Session?");
    if (EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )!=QDialog::Accepted) {
        return;
    }

    while (m_listWidget->count()>0) {
        const QListWidgetItem * item = m_listWidget->item(0);
        EbCallRecordInfo::pointer pCallRecordInfo;
        if (!m_pCallRecordInfo.find(item,pCallRecordInfo)) {
            break;
        }
        deleteRecord(pCallRecordInfo,false);
    }
    m_listWidget->clear();
    m_pCallRecordInfo.clear();

    if (m_bReloadRecordData && m_listWidget->count()<20) {
        QTimer::singleShot( 100,this,SLOT(onLoadCallRecord()) );
    }
}

void EbWidgetMySession::onItemEntered(QListWidgetItem *item)
{
    if (!m_listWidget->hasFocus()) {
        m_listWidget->setFocus();
    }
    /// 滚动条能正常显示
    const QRect rectItem = m_listWidget->visualItemRect(item);
    /// EB_VIEW_SELECT_USER 模式，有一条边框
    const int y = rectItem.y();
//    const int y = (m_viewMode==EB_VIEW_SELECT_USER)?(rectItem.y()+1):rectItem.y();
    /// -2（配合下面的 y+1）实现删除按钮显示时，保留ITEM边框，
    const int buttonSize = rectItem.height()-2;
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)item;

    /// 处理显示电子名片 浮动条
    const QPoint pointIconRight = this->mapToGlobal(rectItem.topLeft());
    const QRect rectIconGlobal( pointIconRight.x()-buttonSize,pointIconRight.y(),buttonSize*2,buttonSize );
    theApp->dialgoViewECard(this, rectIconGlobal)->setItemInfo(ebitem->m_itemInfo);

    if (ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_SUBMSG) {
        /// 通知消息
        m_buttonDelete->setVisible(false);
        m_buttonAddContact->setVisible(false);
        m_buttonCall->setVisible(false);
    }
    else if (ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_GROUP) {
        /// 群组会话
        m_buttonDelete->setVisible(false);
        m_buttonAddContact->setVisible(false);
        if (theApp->m_ebum.EB_IsMyGroup(ebitem->m_itemInfo->m_sGroupCode)) {
            m_buttonCall->setGeometry(m_listWidget->width()-buttonSize, y+1, buttonSize, buttonSize);
            m_buttonCall->setProperty("track-item",QVariant((quint64)item));
            m_buttonCall->setVisible(true);
        }
        else {
            /// 群组可能已经被删除，不存在
            m_buttonCall->setVisible(false);
        }
    }
    else {
        /// 用户会话
        m_buttonCall->setGeometry(m_listWidget->width()-buttonSize, y+1, buttonSize, buttonSize);
        m_buttonCall->setProperty("track-item",QVariant((quint64)item));
        m_buttonCall->setVisible(true);
        const bool showAddContactButton = theApp->m_ebum.EB_IsMyContactAccount2(ebitem->m_itemInfo->m_nUserId)?false:true;
        if (showAddContactButton) {
            m_buttonAddContact->setGeometry(m_listWidget->width()-buttonSize*2, y+1, buttonSize, buttonSize);
            m_buttonAddContact->setProperty("track-item",QVariant((quint64)item));
            m_buttonAddContact->setVisible(true);
        }
        if (is_visitor_uid(ebitem->m_itemInfo->m_nUserId)) {
            /// ** 游客增加删除按钮，方便操作
            const int x = m_listWidget->width()-buttonSize*2+(showAddContactButton?buttonSize:0);
            m_buttonDelete->setGeometry( x,y+1,buttonSize,buttonSize );
            m_buttonDelete->setProperty("track-item",QVariant((quint64)item));
            m_buttonDelete->setVisible(true);
        }

    }

}

void EbWidgetMySession::onItemDoubleClicked(QListWidgetItem *item)
{
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)item;
    createMenuData();
    m_contextMenu->onCallItem(ebitem->m_itemInfo);
}

void EbWidgetMySession::onClickedButtonDelete()
{
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)m_buttonDelete->property("track-item").toULongLong();
    if (ebitem==0) {
        return;
    }
    EbCallRecordInfo::pointer pCallRecordInfo;
    if (!m_pCallRecordInfo.find(ebitem,pCallRecordInfo,true)) {
        return;
    }
    m_buttonDelete->hide();
    m_buttonDelete->setProperty("track-item",QVariant((quint64)0));
    deleteRecord(pCallRecordInfo,true);
    if (m_bReloadRecordData && m_listWidget->count()<20) {
        QTimer::singleShot( 100,this,SLOT(onLoadCallRecord()) );
    }
}

void EbWidgetMySession::onClickedButtonAddContact()
{
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)m_buttonAddContact->property("track-item").toULongLong();
    if (ebitem==0) {
        return;
    }
    EbCallRecordInfo::pointer pCallRecordInfo;
    if (!m_pCallRecordInfo.find(ebitem,pCallRecordInfo)) {
        return;
    }
    m_buttonAddContact->hide();
    m_buttonAddContact->setProperty("track-item",QVariant((quint64)0));

    tstring sAddress;
    if (/*pCallRecordInfo->m_nFromType==EB_ACCOUNT_TYPE_VISITOR || */is_visitor_uid(pCallRecordInfo->m_nFromUserId)) {
        sAddress = pCallRecordInfo->m_sFromName;
    }
    else if (theApp->isAuthContact() && (pCallRecordInfo->m_nFromUserId>0 || !pCallRecordInfo->m_sFromAccount.empty())) {
        /// 验证好友方式
        EbDialogRequestAddContact dlg;
        dlg.m_headFilePath = theApp->userHeadFilePath(pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_sMemberCode,pCallRecordInfo->m_sFromAccount);
        dlg.m_fromName = pCallRecordInfo->m_sFromName.c_str();
        dlg.m_fromUserId = pCallRecordInfo->m_nFromUserId;
        dlg.m_fromAccount = pCallRecordInfo->m_sFromAccount.c_str();
        dlg.exec();
        return;
    }

    EB_ContactInfo contactInfo;
    theApp->m_ebum.EB_GetContactInfo2(pCallRecordInfo->m_nFromUserId,&contactInfo);
    contactInfo.m_nContactUserId = pCallRecordInfo->m_nFromUserId;
    contactInfo.m_sContact = pCallRecordInfo->m_sFromAccount;
    contactInfo.m_sAddress = sAddress;

    EbDialogContactInfo dlgContactInfo;
    dlgContactInfo.m_contactInfo = contactInfo;
    dlgContactInfo.m_contactInfo.m_sName = pCallRecordInfo->m_sFromName.c_str();
    if (pCallRecordInfo->m_nFromPhone>0) {
        char sPhoneString[24];
        sprintf(sPhoneString,"%lld",pCallRecordInfo->m_nFromPhone);
        dlgContactInfo.m_contactInfo.m_sPhone = sPhoneString;
    }
    if (dlgContactInfo.exec()==QDialog::Accepted) {
        theApp->m_ebum.EB_EditContact(&dlgContactInfo.m_contactInfo);
    }
}

void EbWidgetMySession::onClickedButtonCall()
{
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)m_buttonCall->property("track-item").toULongLong();
    if (ebitem!=0) {
        createMenuData();
        m_contextMenu->onCallItem(ebitem->m_itemInfo);
    }
    m_buttonCall->hide();
    m_buttonCall->setProperty("track-item",QVariant((quint64)0));
    m_listWidget->setFocus();
}

void EbWidgetMySession::onLoadCallRecord()
{
//    m_listWidget->clear();
    char sql[256];
    sprintf( sql, "SELECT call_id,Datetime(call_time,'localtime'),dep_code,dep_name,emp_code,from_uid,"
             "from_phone,from_account,from_name,from_type,company,title,tel,email "
             "FROM call_record_t ORDER BY call_time desc LIMIT 50");
    int nCookie = 0;
    const int nCount = (int)theApp->m_sqliteUser->select(sql, nCookie);
    m_bReloadRecordData = nCount==50?true:false;
    cgcValueInfo::pointer pRecord = theApp->m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        EbCallRecordInfo::pointer pCallRecordInfo = EbCallRecordInfo::create();
        pCallRecordInfo->m_sCallId = pRecord->getVector()[0]->getBigIntValue();
        const tstring sCallTime = pRecord->getVector()[1]->getStrValue();
        time_t nCallTime = 0;
        libEbc::ChangeTime(sCallTime.c_str(),nCallTime);
        pCallRecordInfo->m_tTime = nCallTime;
        pCallRecordInfo->m_sGroupCode = pRecord->getVector()[2]->getBigIntValue();
        pCallRecordInfo->m_sGroupName = pRecord->getVector()[3]->getStrValue();
        pCallRecordInfo->m_sMemberCode = pRecord->getVector()[4]->getBigIntValue();
        pCallRecordInfo->m_nFromUserId = pRecord->getVector()[5]->getBigIntValue();
        pCallRecordInfo->m_nFromPhone = pRecord->getVector()[6]->getBigIntValue();
        pCallRecordInfo->m_sFromAccount = pRecord->getVector()[7]->getStrValue();
        pCallRecordInfo->m_sFromName = pRecord->getVector()[8]->getStrValue();
        pCallRecordInfo->m_nFromType = pRecord->getVector()[9]->getIntValue();
        pCallRecordInfo->m_sCompany = pRecord->getVector()[10]->getStrValue();
        pCallRecordInfo->m_sTitle = pRecord->getVector()[11]->getStrValue();
        //pCallRecordInfo->m_sPhone = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[12]->v.varcharVal);
        pCallRecordInfo->m_sTel = pRecord->getVector()[12]->getStrValue();
        pCallRecordInfo->m_sEmail = pRecord->getVector()[13]->getStrValue();
        insertCallRecord(pCallRecordInfo,false,(m_listWidget->count()+1)==nCount?true:false);
        pRecord = theApp->m_sqliteUser->next(nCookie);
    }
    theApp->m_sqliteUser->reset(nCookie);
}

void EbWidgetMySession::resizeEvent(QResizeEvent *event)
{
    m_listWidget->setGeometry( 0,0,width(),height() );
    QWidget::resizeEvent(event);
}

void EbWidgetMySession::createMenuData()
{
    if (m_contextMenu==0) {
        m_contextMenu = new EbContextMenu( EbContextMenu::MySession, this );
        connect( (const QObject*)m_contextMenu,SIGNAL(deleteSession(EbWidgetItemInfo::pointer)),this,SLOT(onDeleteSession(EbWidgetItemInfo::pointer)) );
        connect( (const QObject*)m_contextMenu,SIGNAL(clearSession()),this,SLOT(onClearSession()) );
    }
}

void EbWidgetMySession::contextMenuEvent(QContextMenuEvent *e)
{
    createMenuData();
    const EbListWidgetItem* item = (EbListWidgetItem*)m_listWidget->itemAt(e->pos());
    const EbWidgetItemInfo::pointer itemInfo = item==0?EbWidgetItemInfoNull:item->m_itemInfo;
    if (!m_contextMenu->updateMenuItem(itemInfo)) {
        return;
    }
    m_contextMenu->popup(e->globalPos());
}

bool EbWidgetMySession::deleteRecord(const EbCallRecordInfo::pointer &pCallRecordInfo, bool quesiton)
{
    char sql[256];
    sprintf( sql, "DELETE FROM call_record_t WHERE call_id=%lld",pCallRecordInfo->m_sCallId);
    theApp->m_sqliteUser->execute(sql);
    QListWidgetItem * item = m_listWidget->takeItem( m_listWidget->row(pCallRecordInfo->m_listItem) );
    if (item!=0) {
        delete item;
    }
    if ( quesiton ) {

        if (pCallRecordInfo->isGroupCall()) {
            sprintf( sql, "select msg_type FROM msg_record_t WHERE dep_code=%lld LIMIT 1",pCallRecordInfo->m_sGroupCode);
        }
        else {
            sprintf( sql, "select msg_type FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) LIMIT 1",
                pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_nFromUserId);
        }
        if (theApp->m_sqliteUser->select(sql)>0) {
            QString title;
            QString text;
            if ( pCallRecordInfo->isGroupCall() ) {
                title = theLocales.getLocalText("message-box.clear-session-group-chat-record.title","Clear Chat Record");
                text = theLocales.getLocalText("message-box.clear-session-group-chat-record.text","Confirm Clear:<br>[GROUP_NAME] Chat Record?");
                text.replace("[GROUP_NAME]",pCallRecordInfo->m_sGroupName.c_str());
            }
            else {
                title = theLocales.getLocalText("message-box.clear-session-user-chat-record.title","Clear Chat Record");
                text = theLocales.getLocalText("message-box.clear-session-user-chat-record.text","Confirm Clear:<br>[USER_NAME](USER_ID) Chat Record?");
                text.replace("[USER_NAME]",pCallRecordInfo->m_sFromName.c_str());
                text.replace("[USER_ID]",QString::number(pCallRecordInfo->m_nFromUserId));
            }
            if (EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )!=QDialog::Accepted) {
                return false;
            }
        }
    }
    const bool isAccount = pCallRecordInfo->isGroupCall()?false:true;
    const eb::bigint id = pCallRecordInfo->isGroupCall()?pCallRecordInfo->groupId():pCallRecordInfo->m_nFromUserId;
    theApp->deleteDbRecord( id,isAccount);
    return true;
}

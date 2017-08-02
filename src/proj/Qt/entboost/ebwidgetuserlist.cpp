#include "ebwidgetuserlist.h"
#include <QTimer>
#include <eblistwidgetitem.h>
#include <ebiconhelper.h>
#include <ebdialogviewecard.h>

const QSize const_user_image_size(28,28);
EbWidgetUserList::EbWidgetUserList(const EbcCallInfo::pointer &callInfo,QWidget *parent) : QWidget(parent)
  , m_callInfo(callInfo)
  , m_contextMenu(0)
  , m_timerIdCheckState(0)
{
    assert( m_callInfo.get()!=0 );

    m_listWidgetUsers = new QListWidget(this);
    m_listWidgetUsers->setSortingEnabled(false);
    m_listWidgetUsers->setMouseTracking(true);
    m_listWidgetUsers->setObjectName("UserList");
    m_listWidgetUsers->setIconSize(const_user_image_size);
    connect( m_listWidgetUsers,SIGNAL(itemEntered(QListWidgetItem*)),SLOT(onItemEntered(QListWidgetItem*)) );
    connect( m_listWidgetUsers,SIGNAL(itemDoubleClicked(QListWidgetItem*)),SLOT(onItemDoubleClicked(QListWidgetItem*)) );

    /// “打开会话”按钮
    m_pushButtonCall = new QPushButton(this);
    m_pushButtonCall->setParent( m_listWidgetUsers );
    m_pushButtonCall->setVisible(false);
    m_pushButtonCall->setObjectName("CallButton");
    m_pushButtonCall->setToolTip( theLocales.getLocalText("main-frame.button-call.tooltip","open chat") );
    connect( m_pushButtonCall, SIGNAL(clicked()),this,SLOT(onClickedPushButtonCall()) );
    EbIconHelper::Instance()->SetIcon(m_pushButtonCall,QChar(0xf27a),12 );
    /// “修改我的名片”
    m_pushButtonEdit = new QPushButton(this);
    m_pushButtonEdit->setParent( m_listWidgetUsers );
    m_pushButtonEdit->setVisible(false);
    m_pushButtonEdit->setObjectName("CallButton");
    m_pushButtonEdit->setToolTip( theLocales.getLocalText("main-frame.button-edit.tooltip","edit member info") );
    connect( m_pushButtonEdit, SIGNAL(clicked()),this,SLOT(onClickedPushButtonEdit()) );
    EbIconHelper::Instance()->SetIcon(m_pushButtonEdit,QChar(0xf2c3),12 );

    QTimer::singleShot( 1, this, SLOT(onLoadUserList()) );

    m_timerIdCheckState = this->startTimer(500);
}

EbWidgetUserList::~EbWidgetUserList()
{
    this->killTimer(m_timerIdCheckState);
    m_timerIdCheckState = 0;
}

void EbWidgetUserList::onEnterUser(eb::bigint nUserId, bool bSort)
{
    if (!m_pUserItem.exist(nUserId)) {
        EB_MemberInfo pMemberInfo;
        if (theApp->m_ebum.EB_GetMemberInfoByUserId(&pMemberInfo,this->m_callInfo->groupId(),nUserId)) {
            onMemberInfo(&pMemberInfo,bSort);
        }
    }
}

void EbWidgetUserList::onExitUser(eb::bigint userId, bool exitGroup)
{
    if (!exitGroup) {
        if (this->m_callInfo->isGroupCall()) {
            return;
        }
    }
    EbWidgetItemInfo::pointer itemInfo;
    if (m_pUserItem.find(userId,itemInfo,true)) {
        QListWidgetItem * item = m_listWidgetUsers->takeItem( m_listWidgetUsers->row(itemInfo->m_listItem) );
        if (item!=0) {
            delete item;
        }
        updateGroupMemberSize();
    }
}

int EbWidgetUserList::onMemberInfo(const EB_MemberInfo *memberInfo, bool bChangeLineState)
{
    if (memberInfo==0 || memberInfo->m_sGroupCode!=m_callInfo->groupId()) {
        return -1;
    }
    int nResult = 0;
    EbWidgetItemInfo::pointer itemInfo;
    QString sText;
    if (memberInfo->m_sJobTitle.empty()) {
        sText = memberInfo->m_sUserName.c_str();
    }
    else {
        sText = QString("%1-%2").arg(memberInfo->m_sUserName.c_str()).arg(memberInfo->m_sJobTitle.c_str());
    }
    if (!m_pUserItem.find(memberInfo->m_nMemberUserId,itemInfo)) {
        nResult = 1;
//        const tstring userHeadFile = theApp->memberHeadFilePath(memberInfo);
//        const QIcon icon( QPixmap(userHeadFile.c_str()).scaled(const_user_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
//        EbListWidgetItem * item = new EbListWidgetItem( icon, sText, m_listWidgetUsers );
        EbListWidgetItem * item = new EbListWidgetItem( sText, m_listWidgetUsers );
        itemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_MEMBER,item);
        item->m_itemInfo = itemInfo;
        m_listWidgetUsers->addItem(item);
        m_pUserItem.insert(memberInfo->m_nMemberUserId,itemInfo);
    }
    else {
        itemInfo->m_listItem->setText( sText );
    }
    itemInfo->updateMemberInfo(memberInfo);

    if (bChangeLineState) {
        m_listWidgetUsers->sortItems();
    }
    updateGroupMemberSize();
    return nResult;
}

//void EbWidgetUserList::lineStateChange(eb::bigint userId, EB_USER_LINE_STATE lineState)
//{
//    CTreeItemInfo::pointer pTreeItemInfo;
//    if (m_pUserItem.find(nUserId,pTreeItemInfo))
//    {
//        if (pTreeItemInfo->m_dwItemData != bLineState)
//        {
//            pTreeItemInfo->m_dwItemData = bLineState;
//            m_treeUsers.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
//        }
//    }
//}

void EbWidgetUserList::onItemDoubleClicked(QListWidgetItem *item)
{
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)item;
    if (ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_MEMBER) {
        createMenuData();
        m_contextMenu->onCallItem(ebitem->m_itemInfo);
    }
}

void EbWidgetUserList::onItemEntered(QListWidgetItem *item)
{
//    if (!m_listWidgetUsers->hasFocus()) {
//        m_listWidgetUsers->setFocus();
//    }
    /// 滚动条能正常显示
    const QRect rectItem = m_listWidgetUsers->visualItemRect(item);
    const QPoint pointItem = m_listWidgetUsers->mapToParent(rectItem.topRight());
    const int y = pointItem.y()-1;  /// -1 不保留 ITEM 边框
    const int buttonSize = rectItem.height();
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)item;

    /// 处理显示电子名片 浮动条
    const QPoint pointIconRight = this->mapToGlobal(rectItem.topLeft());
    const QRect rectIconGlobal( pointIconRight.x()-buttonSize,pointIconRight.y(),buttonSize*2,buttonSize );
    theApp->dialgoViewECard(this, rectIconGlobal)->setItemInfo(ebitem->m_itemInfo);

    if ( ebitem->m_itemInfo->m_nUserId==theApp->m_ebum.EB_GetLogonUserId() ) {
        m_pushButtonEdit->setGeometry( pointItem.x()-buttonSize,y,buttonSize,buttonSize );
        m_pushButtonEdit->setProperty("track-item",QVariant((quint64)item));
        m_pushButtonEdit->setVisible(true);
        m_pushButtonCall->hide();
    }
    else {
        m_pushButtonCall->setGeometry( pointItem.x()-buttonSize,y+1,buttonSize,buttonSize );
        m_pushButtonCall->setProperty("track-item",QVariant((quint64)item));
        m_pushButtonCall->setVisible(true);
        m_pushButtonEdit->hide();
    }
}

void EbWidgetUserList::onLoadUserList()
{
    m_pUserItem.clear();
    m_listWidgetUsers->clear();

    std::vector<eb::bigint> pCallMemberList;
    theApp->m_ebum.EB_GetCallUserIdList( m_callInfo->callId(), pCallMemberList );
    for (size_t i=0; i<pCallMemberList.size(); i++) {
        const eb::bigint sAccount = pCallMemberList[i];
        const bool bSort = (i+1)==pCallMemberList.size()?true:false;
        onEnterUser(sAccount,bSort);
    }
    updateGroupMemberSize();
}

void EbWidgetUserList::onClickedPushButtonCall()
{
    onCallItem((QListWidgetItem*)m_pushButtonCall->property("track-item").toULongLong());
    m_pushButtonCall->hide();
    m_pushButtonCall->setProperty("track-item",QVariant((quint64)0));
    m_listWidgetUsers->setFocus();
}

void EbWidgetUserList::onClickedPushButtonEdit()
{
    const EbWidgetItemInfo* item = (EbWidgetItemInfo*)m_pushButtonEdit->property("track-item").toULongLong();
    if (item==0) {
        return;
    }
    m_pushButtonEdit->hide();
    m_pushButtonEdit->setProperty("track-item",QVariant((quint64)0));
    m_listWidgetUsers->setFocus();
    onEditItem((QListWidgetItem*)item);
}

void EbWidgetUserList::resizeEvent(QResizeEvent *event)
{
    m_listWidgetUsers->setGeometry( 0,0,width(),height() );

    QWidget::resizeEvent(event);
}

void EbWidgetUserList::createMenuData()
{
    if (m_contextMenu==0) {
        m_contextMenu = new EbContextMenu( EbContextMenu::UserList, this );
    }
}

void EbWidgetUserList::contextMenuEvent(QContextMenuEvent *e)
{
    const EbListWidgetItem* item = (EbListWidgetItem*)m_listWidgetUsers->itemAt(e->pos());
    if (item==0) {
        return;
    }
    const EbWidgetItemInfo::pointer itemInfo = item->m_itemInfo;
    createMenuData();
    if (!m_contextMenu->updateMenuItem(itemInfo)) {
        return;
    }
    m_contextMenu->popup(e->globalPos());
}

void EbWidgetUserList::timerEvent(QTimerEvent *event)
{
    if ( m_timerIdCheckState!=0 && m_timerIdCheckState==event->timerId()
         && (m_pushButtonCall->isVisible() || m_pushButtonEdit->isVisible()) ) {
        /// 实现定期自动判断当前鼠标位置，并刷新 call/edit 按钮
        const QRect& rect = m_listWidgetUsers->geometry();
        const QPoint pointMouseToDialog = this->mapFromGlobal(this->cursor().pos());
        if (!rect.contains(pointMouseToDialog)) {
            m_pushButtonCall->setVisible(false);
            m_pushButtonEdit->setVisible(false);
        }
    }

    QWidget::timerEvent(event);
}

void EbWidgetUserList::onCallItem(QListWidgetItem *item)
{
    if (item==0) return;
    const EbListWidgetItem *ebitem = (EbListWidgetItem*)item;
    createMenuData();
    m_contextMenu->onCallItem(ebitem->m_itemInfo);
}

void EbWidgetUserList::onEditItem(QListWidgetItem *item)
{
    if (item==0) return;
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)item;
    createMenuData();
    m_contextMenu->onEditItem(ebitem->m_itemInfo);
}

void EbWidgetUserList::updateGroupMemberSize()
{
    if ( m_callInfo->isGroupCall() ) {
        int memberSize = 0;
        int onlineSize = 0;
        theApp->m_ebum.EB_GetGroupMemberSize(m_callInfo->groupId(),1,memberSize,onlineSize);
        //const int nMemberSize = theEBAppClient.EB_GetGroupMemberSize(m_pCallInfo.m_sGroupCode,1);
        //const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(m_pCallInfo.m_sGroupCode,1);
        emit memberOnlineSizeChange(memberSize,onlineSize);
//        /// 群成员(%d/%d)
//        /// 群成员(%d)
//        QString text;
//        if (onlineSize>=0)
//            text = QString("%1(%2/%3)").arg(theLocales.getLocalText("user-list.title","User List")).arg(onlineSize).arg(memberSize);
//        else
//            text = QString("%1(%2)").arg(theLocales.getLocalText("user-list.title","User List")).arg(memberSize);
//        setItemText(EbWorkItem::WORK_ITEM_USER_LIST,text);
    }
}

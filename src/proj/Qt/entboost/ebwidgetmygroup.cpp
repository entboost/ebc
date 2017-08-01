#include "ebwidgetmygroup.h"
#include "ebiconhelper.h"
#include "ebtreewidgetitem.h"
#include "ebmessagebox.h"
#include <ebdialogviewecard.h>

EbWidgetMyGroup::EbWidgetMyGroup(EB_VIEW_MODE viewMode, QWidget *parent) : EbWidgetTreeSelectBase(viewMode, parent)
  , m_contextMenu(0)
{
    connect( m_treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onItemClicked(QTreeWidgetItem*,int)) );
    connect( m_treeWidget,SIGNAL(itemEntered(QTreeWidgetItem*,int)),this,SLOT(onItemEntered(QTreeWidgetItem*,int)) );
    connect( m_treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)) );
    //    connect( m_treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)) );

    /// “添加”按钮
    connect( m_pushButtonSelect, SIGNAL(clicked()),this,SLOT(onClickedPushButtonSelect()) );
    /// “打开会话”按钮
    connect( m_pushButtonCall, SIGNAL(clicked()),this,SLOT(onClickedPushButtonCall()) );
    /// “修改我的名片”
    connect( m_pushButtonEdit, SIGNAL(clicked()),this,SLOT(onClickedPushButtonEdit()) );

    updateLocaleInfo();
    /// for test data
    //    QTreeWidgetItem * topItem0 = new QTreeWidgetItem(m_treeWidget, QStringList(QString("Top Item 0")));
    //    QTreeWidgetItem * topItem1 = new QTreeWidgetItem(m_treeWidget, QStringList(QString("Top Item 1")));
    //    QTreeWidgetItem * topItem2 = new QTreeWidgetItem(m_treeWidget, QStringList(QString("Top Item 2")));
    //    m_treeWidget->insertTopLevelItem( 0,topItem0 );
    //    m_treeWidget->insertTopLevelItem( 1,topItem1 );
    //    m_treeWidget->insertTopLevelItem( 2,topItem2 );

    //    const QSize const_tree_icon_size(32,32);
    ////    QIcon icon;
    //    for (int i=0; i<3; i++) {
    //        QTreeWidgetItem * topItem0Sub0 = new QTreeWidgetItem(topItem0, QStringList(QString("Top Item 0 Sub")));
    //        topItem0Sub0->setIcon( 0, QIcon( QPixmap::fromImage(QImage(QStringLiteral(":/res/imgtip"))).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) ) );

    //        topItem0->addChild(topItem0Sub0);
    //        QTreeWidgetItem * topItem1Sub0 = new QTreeWidgetItem(topItem1, QStringList(QString("Top Item 1 Sub")));
    //        topItem1Sub0->setIcon( 0, QIcon( QPixmap::fromImage(QImage(QStringLiteral(":/res/imgerror"))).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) ) );
    //        topItem1->addChild(topItem1Sub0);

    //    }

}

EbWidgetMyGroup::~EbWidgetMyGroup()
{
    //    m_actionList.clear();
}

void EbWidgetMyGroup::setMemberInfo(const EbWidgetItemInfo::pointer& pGroupItemInfo, const EB_MemberInfo* memberInfo, EB_SORT_ITEMS_FLAG nSortItems)
{
    if (pGroupItemInfo.get()==0 || memberInfo==0 || memberInfo->m_sMemberCode==0) {
        return;
    }

    const int nSubType = EbWidgetItemInfo::memberSubType(memberInfo);

    QString sText;
    if (memberInfo->m_sJobTitle.empty())
        sText = memberInfo->m_sUserName.c_str();
    else
        sText = QString("%1 - %2").arg(memberInfo->m_sUserName.c_str()).arg(memberInfo->m_sJobTitle.c_str());
    EbWidgetItemInfo::pointer widgetItemInfo;
    if (!m_pEmpItemInfo.find(memberInfo->m_sMemberCode, widgetItemInfo)) {
        EbTreeWidgetItem * item = new EbTreeWidgetItem(pGroupItemInfo->m_hItem, QStringList(sText));
        widgetItemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_MEMBER,item);
        item->m_itemInfo = widgetItemInfo;
        pGroupItemInfo->m_hItem->addChild(item);
        m_pEmpItemInfo.insert(memberInfo->m_sMemberCode, widgetItemInfo);
        if (nSortItems==EB_AUTO_SORT) {
            nSortItems=EB_FORCE_SORT;
        }
    }
    else {
        widgetItemInfo->m_hItem->setText(0,sText);
        if ( nSortItems==EB_AUTO_SORT &&
             (memberInfo->m_nLineState!=(EB_USER_LINE_STATE)widgetItemInfo->m_dwItemData ||
              widgetItemInfo->m_nIndex!=memberInfo->m_nDisplayIndex ||
              widgetItemInfo->m_sName!=memberInfo->m_sUserName) ||
             widgetItemInfo->m_nSubType!=nSubType) {
            nSortItems=EB_FORCE_SORT;
        }
    }

    widgetItemInfo->updateMemberInfo(memberInfo);

    /// ?? 这里要实现，状况改变
    if (nSortItems==EB_FORCE_SORT) {
        pGroupItemInfo->m_hItem->sortChildren( 0,Qt::AscendingOrder );
    }
}

void EbWidgetMyGroup::onGroupInfo(const EB_GroupInfo * groupInfo)
{
    std::vector<EB_MemberInfo> pOutMemberInfoList;
    if (!theApp->m_ebum.EB_GetGroupMemberInfoList(groupInfo->m_sGroupCode,pOutMemberInfoList)) {
        return;
    }

    QString sText = QString("%1(%2)").arg(groupInfo->m_sGroupName.c_str()).arg(groupInfo->m_nEmpCount);
    bool bSortItems = false;
    EbWidgetItemInfo::pointer groupItemInfo;
    if ( !m_pDepItemInfo.find(groupInfo->m_sGroupCode, groupItemInfo) ) {
        bSortItems = true;
        EbTreeWidgetItem * item = new EbTreeWidgetItem(m_treeWidget, QStringList(sText));
        groupItemInfo = EbWidgetItemInfo::create(groupInfo,item);
        item->m_itemInfo = groupItemInfo;
        m_treeWidget->addTopLevelItem(item);
        m_pDepItemInfo.insert(groupInfo->m_sGroupCode,groupItemInfo);
    }
    else {
        if (groupItemInfo->m_nIndex!=groupInfo->m_nDisplayIndex || groupItemInfo->m_sName != groupInfo->m_sGroupName)
            bSortItems = true;
    }
    groupItemInfo->m_nBigId = groupInfo->m_nMyEmpId;
    groupItemInfo->m_sName = groupInfo->m_sGroupName;
    groupItemInfo->m_nIndex = groupInfo->m_nDisplayIndex;
    /// **需要加载，否则会导致成员显示不完整；
    for (size_t i=0;i<pOutMemberInfoList.size(); i++) {
        const EB_MemberInfo& pMemberInfo = pOutMemberInfoList[i];
        if (pMemberInfo.m_sMemberCode==0) continue;
        setMemberInfo(groupItemInfo, &pMemberInfo,EB_DISABLE_SORT);
    }
    if ( !pOutMemberInfoList.empty() ) {
        groupItemInfo->m_hItem->sortChildren( 0,Qt::AscendingOrder );
    }
    const int nMemberSize = groupInfo->m_nEmpCount;
    const int nOnlineSize = theApp->m_ebum.EB_GetGroupOnlineSize(groupInfo->m_sGroupCode,1);
    if (nOnlineSize>=0)
        sText = QString("%1 [%2/%3]").arg(groupInfo->m_sGroupName.c_str()).arg(nOnlineSize).arg(nMemberSize);
    else
        sText = QString("%1 [%2]").arg(groupInfo->m_sGroupName.c_str()).arg(nMemberSize);
    groupItemInfo->m_hItem->setText(0,sText);
    if (bSortItems) {
        m_treeWidget->sortItems( 0,Qt::AscendingOrder );
    }

//    if (m_treeDepartment.GetSelectedItem()==NULL)
//    {
//        KillTimer(TIMERID_CHECK_SELECT_FIRST);
//        SetTimer(TIMERID_CHECK_SELECT_FIRST,2000,NULL);
//    }

}

void EbWidgetMyGroup::onMemberInfo(const EB_MemberInfo *pMemberInfo,bool bChangeLineState)
{
    EbWidgetItemInfo::pointer pDepItemInfo;
    if (!m_pDepItemInfo.find(pMemberInfo->m_sGroupCode,pDepItemInfo)) {
        return;
    }
    /// 已经加载成员
    if (pDepItemInfo->m_dwItemData==0)
        pDepItemInfo->m_dwItemData = 1;
    setMemberInfo(pDepItemInfo, pMemberInfo);
    if (bChangeLineState) {
        CEBString sGroupName;
        if (theApp->m_ebum.EB_GetGroupName(pMemberInfo->m_sGroupCode,sGroupName)) {
            int nMemberSize = 0;
            int nOnlineSize = 0;
            theApp->m_ebum.EB_GetGroupMemberSize(pMemberInfo->m_sGroupCode,1,nMemberSize,nOnlineSize);
            QString sText;
            if (nOnlineSize>=0)
                sText = QString("%1 [%2/%3]").arg(sGroupName.c_str()).arg(nOnlineSize).arg(nMemberSize);
            else
                sText = QString("%1 [%2]").arg(sGroupName.c_str()).arg(nMemberSize);
            pDepItemInfo->m_hItem->setText(0,sText);
        }
    }
}

void EbWidgetMyGroup::onRemoveGroup(const EB_GroupInfo *pGroupInfo)
{
    //EB_GroupInfo pGroupInfo;
    //if (theApp->m_ebum.EB_GetGroupInfo(sDepCode,&pGroupInfo))
    {
        std::vector<eb::bigint> pOutMemberCodeList;
        if (theApp->m_ebum.EB_GetGroupMemberCodeList(pGroupInfo->m_sGroupCode,pOutMemberCodeList)) {
            for (size_t i=0;i<pOutMemberCodeList.size(); i++) {
                eb::bigint nMemberCode = pOutMemberCodeList[i];
                deleteMemberInfo( pGroupInfo,nMemberCode,true );
            }
        }
    }

    EbWidgetItemInfo::pointer pDepItemInfo;
    if (m_pDepItemInfo.find(pGroupInfo->m_sGroupCode, pDepItemInfo, true)) {
        QTreeWidgetItem * item = m_treeWidget->takeTopLevelItem( m_treeWidget->indexOfTopLevelItem(pDepItemInfo->m_hItem) );
        if (item!=0) {
            delete item;
        }
    }
}

void EbWidgetMyGroup::deleteMemberInfo(const EB_GroupInfo *pGroupInfo, eb::bigint nMemberCode, bool fromDeleteGroup)
{
    const eb::bigint nGroupCode = pGroupInfo->m_sGroupCode;
    EbWidgetItemInfo::pointer pDepItemInfo;
    if (m_pDepItemInfo.find(nGroupCode, pDepItemInfo)) {
        EbWidgetItemInfo::pointer pEmpItemInfo;
        if (m_pEmpItemInfo.find(nMemberCode, pEmpItemInfo, true)) {
            pDepItemInfo->m_hItem->removeChild(pEmpItemInfo->m_hItem);
        }
        if (!fromDeleteGroup) {
            int nMemberSize = 0;
            int nOnlineSize = 0;
            theApp->m_ebum.EB_GetGroupMemberSize(nGroupCode,1,nMemberSize,nOnlineSize);
            const QString sText = QString("%1 [%2/%3]").arg(pGroupInfo->m_sGroupName.c_str()).arg(nOnlineSize).arg(nMemberSize);
            pDepItemInfo->m_hItem->setText(0,sText);
        }
    }
}

void EbWidgetMyGroup::onItemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
    const EbTreeWidgetItem* ebitem = (EbTreeWidgetItem*)item;
    if (ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_MEMBER) {
        createMenuData();
        m_contextMenu->onCallItem(ebitem->m_itemInfo);
    }
}

//void EbWidgetMyGroup::deleteGroupInfo(const EB_GroupInfo *pGroupInfo)
//{
//    {
//        std::vector<eb::bigint> pOutMemberCodeList;
//        if (theApp->m_ebum.EB_GetGroupMemberCodeList(pGroupInfo->m_sGroupCode,pOutMemberCodeList))
//        {
//            for (size_t i=0;i<pOutMemberCodeList.size(); i++)
//            {
//                eb::bigint nMemberCode = pOutMemberCodeList[i];
//                DeleteEmployeeInfo(pGroupInfo,nMemberCode,true);
//            }
//        }
//    }

//    EbWidgetItemInfo::pointer pDepItemInfo;
//    if (m_pDepItemInfo.find(pGroupInfo->m_sGroupCode, pDepItemInfo, true))
//    {
//        m_treeDepartment.DeleteItem(pDepItemInfo->m_hItem);
//    }
//}

void EbWidgetMyGroup::resizeEvent(QResizeEvent * e)
{
    m_treeWidget->setGeometry( 0,0,width(),height() );
    QWidget::resizeEvent(e);
}

void EbWidgetMyGroup::createMenuData()
{
    if (m_contextMenu==0) {
        m_contextMenu = new EbContextMenu( EbContextMenu::MyGroup, this );
    }
}

void EbWidgetMyGroup::contextMenuEvent(QContextMenuEvent *e)
{
    createMenuData();
    const EbTreeWidgetItem* item = (EbTreeWidgetItem*)m_treeWidget->itemAt(e->pos());
    const EbWidgetItemInfo::pointer itemInfo = item==0?EbWidgetItemInfoNull:item->m_itemInfo;
    if (!m_contextMenu->updateMenuItem(itemInfo)) {
        return;
    }
    m_contextMenu->exec( e->globalPos() );
}

void EbWidgetMyGroup::onItemClicked(QTreeWidgetItem *item, int column)
{
    const EbTreeWidgetItem* ebitem = (EbTreeWidgetItem*)item;
    const EbWidgetItemInfo::ITEM_TYPE itemType = ebitem->m_itemInfo->m_nItemType;
    if (itemType==EbWidgetItemInfo::ITEM_TYPE_GROUP) {
        theApp->m_ebum.EB_LoadGroup(ebitem->m_itemInfo->m_sGroupCode,true);
        if (item->isExpanded()) {
            m_treeWidget->collapseItem(item);
        }
        else {
            m_treeWidget->expandItem(item);
        }
    }
}
void EbWidgetMyGroup::onItemEntered(QTreeWidgetItem *item, int /*column*/)
{
    if (!m_treeWidget->hasFocus()) {
        m_treeWidget->setFocus();
    }
    /// 滚动条能正常显示
    const QRect rectItem = m_treeWidget->visualItemRect(item);
    const QPoint pointItem = m_treeWidget->mapToParent(rectItem.topRight());
    /// EB_VIEW_SELECT_USER 模式，有一条边框
    const int y = (m_viewMode==EB_VIEW_SELECT_USER)?(pointItem.y()+1):pointItem.y();
    /// -2（配合下面的 y+1）实现删除按钮显示时，保留ITEM边框，
    const int buttonSize = rectItem.height()-2;
    const EbTreeWidgetItem* ebitem = (EbTreeWidgetItem*)item;

    /// 处理显示电子名片 浮动条
    const QPoint pointIconRight = this->mapToGlobal(rectItem.topLeft());
    const QRect rectIconGlobal( pointIconRight.x()-buttonSize,pointIconRight.y(),buttonSize*2,buttonSize );
    theApp->dialgoViewECard(this, rectIconGlobal)->setItemInfo(ebitem->m_itemInfo);

    if (m_viewMode==EB_VIEW_SELECT_USER) {
        m_pushButtonCall->hide();
        m_pushButtonEdit->hide();
        const bool isSelectedUser = m_selectedUserCallback==0?false:m_selectedUserCallback->isSelectedUser(ebitem->m_itemInfo);
        if ( !isSelectedUser &&
             ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_MEMBER &&
             ebitem->m_itemInfo->m_nUserId!=theApp->m_ebum.EB_GetLogonUserId() ) {
            m_pushButtonSelect->setGeometry( pointItem.x()-buttonSize,y+1,buttonSize,buttonSize );
            m_pushButtonSelect->setProperty("track-item",QVariant((quint64)item));
            m_pushButtonSelect->setVisible(true);
        }
        else {
            m_pushButtonSelect->setVisible(false);
        }
    }
    else {
        m_pushButtonSelect->hide();
        if ( ebitem->m_itemInfo->m_nUserId==theApp->m_ebum.EB_GetLogonUserId() ) {
            m_pushButtonEdit->setGeometry( pointItem.x()-buttonSize,y+1,buttonSize,buttonSize );
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
}

//void EbWidgetMyGroup::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
//{
//    const QRect rectItem = m_treeWidget->visualItemRect(current);
//    const QPoint pointItem = m_treeWidget->mapToParent(rectItem.topRight());
//    const int rowIndexItem = 0;
////    const int rowIndexItem = listWidgetLoginRecords->row(item);
//    const QRect& rect = m_treeWidget->geometry();
//    const int y = rect.top()+rowIndexItem*rectItem.height()+0;  // 0表示 m_treeWidget 没有边框
//    // -2（配合下面的 buttonSize-1和y+1）实现删除按钮显示时，保留ITEM边框，
//    const int buttonSize = rectItem.height()-2;
//    m_pushButtonCall->setGeometry( rect.right()-buttonSize-1,y+1,buttonSize,buttonSize );
//    m_pushButtonCall->setProperty("call-item",QVariant(rowIndexItem));
//    m_pushButtonCall->setVisible(true);
//}

void EbWidgetMyGroup::onClickedPushButtonSelect()
{
    const EbTreeWidgetItem * ebitem = (EbTreeWidgetItem*)m_pushButtonSelect->property("track-item").toULongLong();
    if (ebitem!=0 && ebitem->m_itemInfo.get()!=0) {
        emit selectedItemInfo(ebitem->m_itemInfo);
    }
    m_pushButtonSelect->hide();
    m_pushButtonSelect->setProperty("track-item",QVariant((quint64)0));
    m_treeWidget->setFocus();
}

void EbWidgetMyGroup::onClickedPushButtonCall(void)
{
    const EbTreeWidgetItem * ebitem = (EbTreeWidgetItem*)m_pushButtonCall->property("track-item").toULongLong();
    if (ebitem!=0) {
        createMenuData();
        m_contextMenu->onCallItem(ebitem->m_itemInfo);
    }
    m_pushButtonCall->hide();
    m_pushButtonCall->setProperty("track-item",QVariant((quint64)0));
    m_treeWidget->setFocus();
}

void EbWidgetMyGroup::onClickedPushButtonEdit()
{
    const EbTreeWidgetItem* ebitem = (EbTreeWidgetItem*)m_pushButtonEdit->property("track-item").toULongLong();
    if (ebitem==0) {
        return;
    }
    m_pushButtonEdit->hide();
    m_pushButtonEdit->setProperty("track-item",QVariant((quint64)0));
    m_treeWidget->setFocus();
    createMenuData();
    m_contextMenu->onEditItem(ebitem->m_itemInfo);
}

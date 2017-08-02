#include "ebwidgetmyenterprise.h"
#include "ebiconhelper.h"
#include "ebtreewidgetitem.h"
#include "ebmessagebox.h"
#include "ebdialogviewecard.h"

EbWidgetMyEnterprise::EbWidgetMyEnterprise(EB_VIEW_MODE viewMode, QWidget *parent) : EbWidgetTreeSelectBase(viewMode, parent)
  , m_contextMenu(0)
  , m_timerIdUpdateEnterpriseCount(0)
  , m_timerIdUpdateGroupCount(0)
{
    connect( m_treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onItemClicked(QTreeWidgetItem*,int)) );
    connect( m_treeWidget,SIGNAL(itemEntered(QTreeWidgetItem*,int)),this,SLOT(onItemEntered(QTreeWidgetItem*,int)) );
    connect( m_treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)) );

    /// “添加”按钮
    connect( m_pushButtonSelect, SIGNAL(clicked()),this,SLOT(onClickedPushButtonSelect()) );
    /// “打开会话”按钮
    connect( m_pushButtonCall, SIGNAL(clicked()),this,SLOT(onClickedPushButtonCall()) );
    /// “修改我的名片”
    connect( m_pushButtonEdit, SIGNAL(clicked()),this,SLOT(onClickedPushButtonEdit()) );
}

EbWidgetMyEnterprise::~EbWidgetMyEnterprise()
{
}

void EbWidgetMyEnterprise::onEnterpriseInfo(const EB_EnterpriseInfo *pEnterpriseInfo)
{
//    if ( m_enterpriseInfo.m_sEnterpriseCode!=pEnterpriseInfo->m_sEnterpriseCode ) {
//        m_enterpriseInfo = *pEnterpriseInfo;
//    }
    EbWidgetItemInfo::pointer pEntItemInfo;
    if (m_pEntItemInfo.find(pEnterpriseInfo->m_sEnterpriseCode,pEntItemInfo)) {
//        int nEnterpriseMemberSize = 0;
//        int nEnterpriseOnlineSize = 0;
//        theApp->m_ebum.EB_GetEnterpriseMemberSize(pEnterpriseInfo->m_sEnterpriseCode,nEnterpriseMemberSize,nEnterpriseOnlineSize);
//        const QString sText = QString("%s [%d/%d]").arg(pEnterpriseInfo->m_sEnterpriseName.c_str())
//                .arg(nEnterpriseOnlineSize).arg(nEnterpriseMemberSize);
//        pEntItemInfo->m_hItem->setText(0,sText);
    }
    else {
        int nEnterpriseMemberSize = 0;
        int nEnterpriseOnlineSize = 0;
        theApp->m_ebum.EB_GetEnterpriseMemberSize(pEnterpriseInfo->m_sEnterpriseCode,nEnterpriseMemberSize,nEnterpriseOnlineSize);
        const QString sText = QString("%1 [%2/%3]").arg(pEnterpriseInfo->m_sEnterpriseName.c_str())
                .arg(nEnterpriseOnlineSize).arg(nEnterpriseMemberSize);
        EbTreeWidgetItem * item = new EbTreeWidgetItem(m_treeWidget, QStringList(sText));
        pEntItemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_ENTERPRISE,item);
        pEntItemInfo->m_sEnterpriseCode = pEnterpriseInfo->m_sEnterpriseCode;
        pEntItemInfo->m_sName = pEnterpriseInfo->m_sEnterpriseName;
        item->m_itemInfo = pEntItemInfo;
        m_treeWidget->insertTopLevelItem(0,item);
        m_pEntItemInfo.insert(pEnterpriseInfo->m_sEnterpriseCode,pEntItemInfo);
    }
}

void EbWidgetMyEnterprise::SetMemberInfo(const EbWidgetItemInfo::pointer& pGroupItemInfo, const EB_MemberInfo* memberInfo, EB_SORT_ITEMS_FLAG nSortItems)
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

void EbWidgetMyEnterprise::onGroupInfo(const EB_GroupInfo * groupInfo)
{
    EbWidgetItemInfo::pointer pEntItemInfo;
    if (groupInfo->m_sEnterpriseCode==0 || !m_pEntItemInfo.find(groupInfo->m_sEnterpriseCode,pEntItemInfo))
    {
        return;
    }
    EbWidgetItemInfo::pointer parentItemInfo;
    if (groupInfo->m_sParentCode==0) {
        parentItemInfo = pEntItemInfo;
    }
    else {
        if (!m_pDepItemInfo.find(groupInfo->m_sParentCode,parentItemInfo)) {
            return;
        }
    }

//    std::vector<EB_MemberInfo> pOutMemberInfoList;
//    if (!theApp->m_ebum.EB_GetGroupMemberInfoList(groupInfo->m_sGroupCode,pOutMemberInfoList)) {
//        return;
//    }

    const QString sText = QString("%1(%2)").arg(groupInfo->m_sGroupName.c_str()).arg(groupInfo->m_nEmpCount);
    bool bSortItems = false;
    EbWidgetItemInfo::pointer groupItemInfo;
    if ( !m_pDepItemInfo.find(groupInfo->m_sGroupCode, groupItemInfo) ) {
        bSortItems = true;
        EbTreeWidgetItem * item = new EbTreeWidgetItem(parentItemInfo->m_hItem, QStringList(sText));
        groupItemInfo = EbWidgetItemInfo::create(groupInfo,item);
        groupItemInfo->m_nCount1 = -1;	// for member-size
        groupItemInfo->m_nCount2 = -1;	// for online-size
        item->m_itemInfo = groupItemInfo;
        parentItemInfo->m_hItem->addChild(item);
        m_pDepItemInfo.insert(groupInfo->m_sGroupCode,groupItemInfo);
    }
    else {
        if (groupItemInfo->m_sName != groupInfo->m_sGroupName) {
            groupItemInfo->m_nCount1 = -1;	/// 可以重新统计人数，显示
            groupItemInfo->m_hItem->setText(0,sText);
        }

        if (groupItemInfo->m_nIndex!=groupInfo->m_nDisplayIndex || groupItemInfo->m_sName != groupInfo->m_sGroupName) {
            bSortItems = true;
        }
    }
    groupItemInfo->m_nBigId = groupInfo->m_nMyEmpId;
    groupItemInfo->m_sName = groupInfo->m_sGroupName;
    groupItemInfo->m_nIndex = groupInfo->m_nDisplayIndex;
    if (bSortItems) {
        parentItemInfo->m_hItem->sortChildren( 0,Qt::AscendingOrder );
    }
    setGroupCount(groupItemInfo, groupInfo, false);

//    /// **需要加载，否则会导致成员显示不完整；
//    for (size_t i=0;i<pOutMemberInfoList.size(); i++) {
//        const EB_MemberInfo& pMemberInfo = pOutMemberInfoList[i];
//        if (pMemberInfo.m_sMemberCode==0) continue;
//        SetMemberInfo(groupItemInfo, &pMemberInfo,DISABLE_SORT);
//    }
//    if ( !pOutMemberInfoList.empty() ) {
//        groupItemInfo->m_hItem->sortChildren( 0,Qt::AscendingOrder );
//    }
//    const int nMemberSize = groupInfo->m_nEmpCount;
//    const int nOnlineSize = theApp->m_ebum.EB_GetGroupOnlineSize(groupInfo->m_sGroupCode,1);
//    if (nOnlineSize>=0)
//        sText = QString("%1 [%2/%3]").arg(groupInfo->m_sGroupName.c_str()).arg(nOnlineSize).arg(nMemberSize);
//    else
//        sText = QString("%1 [%2]").arg(groupInfo->m_sGroupName.c_str()).arg(nMemberSize);
//    groupItemInfo->m_hItem->setText(0,sText);
//    if (bSortItems) {
//        m_treeWidget->sortItems( 0,Qt::AscendingOrder );
//    }

}

void EbWidgetMyEnterprise::onMemberInfo(const EB_MemberInfo *pMemberInfo,bool bChangeLineState)
{
    EbWidgetItemInfo::pointer pDepItemInfo;
    if (!m_pDepItemInfo.find(pMemberInfo->m_sGroupCode,pDepItemInfo)) {
        return;
    }
    /// 已经加载成员
    if (pDepItemInfo->m_dwItemData==0) {
        pDepItemInfo->m_dwItemData = 1;
    }

    SetMemberInfo(pDepItemInfo, pMemberInfo);
    if (bChangeLineState) {
        setGroupCount(pDepItemInfo, pMemberInfo->m_sGroupCode,true);
//        CEBString sGroupName;
//        if (theApp->m_ebum.EB_GetGroupName(pMemberInfo->m_sGroupCode,sGroupName)) {
//            int nMemberSize = 0;
//            int nOnlineSize = 0;
//            theApp->m_ebum.EB_GetGroupMemberSize(pMemberInfo->m_sGroupCode,1,nMemberSize,nOnlineSize);
//            QString sText;
//            if (nOnlineSize>=0)
//                sText = QString("%1 [%2/%3]").arg(sGroupName.c_str()).arg(nOnlineSize).arg(nMemberSize);
//            else
//                sText = QString("%1 [%2]").arg(sGroupName.c_str()).arg(nMemberSize);
//            pDepItemInfo->m_hItem->setText(0,sText);
//        }
    }
}

void EbWidgetMyEnterprise::onRemoveGroup(const EB_GroupInfo *groupInfo)
{
    EbWidgetItemInfo::pointer pEntItemInfo;
    if (groupInfo->m_sEnterpriseCode==0 || !m_pEntItemInfo.find(groupInfo->m_sEnterpriseCode,pEntItemInfo))
    {
        return;
    }
    //EB_GroupInfo pGroupInfo;
    //if (theApp->m_ebum.EB_GetGroupInfo(sDepCode,&pGroupInfo))
    {
        std::vector<eb::bigint> pOutMemberCodeList;
        if (theApp->m_ebum.EB_GetGroupMemberCodeList(groupInfo->m_sGroupCode,pOutMemberCodeList)) {
            for (size_t i=0;i<pOutMemberCodeList.size(); i++) {
                eb::bigint nMemberCode = pOutMemberCodeList[i];
                deleteMemberInfo( groupInfo,nMemberCode,true );
            }
        }
    }

    EbWidgetItemInfo::pointer pDepItemInfo;
    if (m_pDepItemInfo.find(groupInfo->m_sGroupCode, pDepItemInfo, true)) {
        /// 找到上一级 ITEM
        EbWidgetItemInfo::pointer parentItemInfo;
        if (groupInfo->m_sParentCode==0) {
            parentItemInfo = pEntItemInfo;
        }
        else {
            m_pDepItemInfo.find(groupInfo->m_sParentCode,parentItemInfo);
        }
        if (parentItemInfo.get()!=0) {
            parentItemInfo->m_hItem->removeChild(pDepItemInfo->m_hItem);
        }
        /// 重新统计企业 在线人数/总人数
        EbWidgetItemInfo::pointer pEntItemInfo;
        if (m_pEntItemInfo.find(pDepItemInfo->m_sEnterpriseCode,pEntItemInfo)) {
            int memberSize = 0;
            int onlineSize = 0;
            theApp->m_ebum.EB_GetEnterpriseMemberSize(pDepItemInfo->m_sEnterpriseCode,memberSize,onlineSize);
            const QString sText = QString("%1 [%2/%3]").arg(pEntItemInfo->m_sName.c_str()).arg(onlineSize).arg(memberSize);
            pEntItemInfo->m_hItem->setText( 0,sText);
        }
    }
}

void EbWidgetMyEnterprise::deleteMemberInfo(const EB_GroupInfo *pGroupInfo, eb::bigint nMemberCode, bool fromDeleteGroup)
{
    const eb::bigint nGroupCode = pGroupInfo->m_sGroupCode;
    EbWidgetItemInfo::pointer pDepItemInfo;
    if (m_pDepItemInfo.find(nGroupCode, pDepItemInfo)) {
        EbWidgetItemInfo::pointer pEmpItemInfo;
        if (m_pEmpItemInfo.find(nMemberCode, pEmpItemInfo, true)) {
            pDepItemInfo->m_hItem->removeChild(pEmpItemInfo->m_hItem);
        }
        if (!fromDeleteGroup) {
            setGroupCount(pDepItemInfo, pGroupInfo, true);
        }
    }
}
void EbWidgetMyEnterprise::onItemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
    const EbTreeWidgetItem* ebitem = (EbTreeWidgetItem*)item;
    if (ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_MEMBER) {
        createMenuData();
        m_contextMenu->onCallItem(ebitem->m_itemInfo);
    }
}

//void EbWidgetMyEnterprise::deleteGroupInfo(const EB_GroupInfo *pGroupInfo)
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

void EbWidgetMyEnterprise::resizeEvent(QResizeEvent * e)
{
    m_treeWidget->setGeometry( 0,0,width(),height() );
    QWidget::resizeEvent(e);
}

void EbWidgetMyEnterprise::createMenuData()
{
    if (m_contextMenu==0) {
        m_contextMenu = new EbContextMenu( EbContextMenu::MyEnterprise, this );
    }
}

void EbWidgetMyEnterprise::contextMenuEvent(QContextMenuEvent *e)
{
    createMenuData();
    const EbTreeWidgetItem* item = (EbTreeWidgetItem*)m_treeWidget->itemAt(e->pos());
    const EbWidgetItemInfo::pointer itemInfo = item==0?EbWidgetItemInfoNull:item->m_itemInfo;
    if (!m_contextMenu->updateMenuItem(itemInfo)) {
        return;
    }
    m_contextMenu->popup(e->globalPos());
}

void EbWidgetMyEnterprise::timerEvent(QTimerEvent *e)
{
    if (m_timerIdUpdateEnterpriseCount!=0 && m_timerIdUpdateEnterpriseCount==e->timerId()) {
        /// 更新企业 在线人数/总人数
        this->killTimer(m_timerIdUpdateEnterpriseCount);
        m_timerIdUpdateEnterpriseCount = 0;
        if (m_pUpdateEntItemInfo.get()!=0) {
            int memberSize = 0;
            int onlineSize = 0;
            theApp->m_ebum.EB_GetEnterpriseMemberSize(m_pUpdateEntItemInfo->m_sEnterpriseCode,memberSize,onlineSize);
            const QString sText = QString("%1 [%2/%3]").arg(m_pUpdateEntItemInfo->m_sName.c_str()).arg(onlineSize).arg(memberSize);
            m_pUpdateEntItemInfo->m_hItem->setText( 0,sText );
            m_pUpdateEntItemInfo.reset();
        }
    }
    else if (m_timerIdUpdateGroupCount!=0 && m_timerIdUpdateGroupCount==e->timerId()) {
        /// 更新部门（群组） 在线人数/总人数
//        this->killTimer(m_timerIdUpdateGroupCount);
//        m_timerIdUpdateGroupCount = 0;

        BoostWriteLock wtlock(m_pUpdateGroupCountList.mutex());
        CLockMap<eb::bigint,EbWidgetItemInfo::pointer>::iterator pIter = m_pUpdateGroupCountList.begin();
        if (pIter!=m_pUpdateGroupCountList.end()) {
            const mycp::bigint nGroupId = pIter->first;
            const EbWidgetItemInfo::pointer groupItemInfo = pIter->second;
            m_pUpdateGroupCountList.erase(pIter);
            wtlock.unlock();
            if (m_pUpdateGroupCountList.empty()) {
                this->killTimer(m_timerIdUpdateGroupCount);
                m_timerIdUpdateGroupCount = 0;
            }
            int memberSize = 0;
            int onlineSize = 0;
            theApp->m_ebum.EB_GetGroupMemberSize(nGroupId,0,memberSize,onlineSize);
            if (memberSize!=groupItemInfo->m_nCount1 || onlineSize!=groupItemInfo->m_nCount2) {
                groupItemInfo->m_nCount1 = memberSize;
                groupItemInfo->m_nCount2 = onlineSize;
                QString sText;
                if (onlineSize>=0 && memberSize>0)
                    sText = QString("%1 [%2/%3]").arg(groupItemInfo->m_sName.c_str()).arg(onlineSize).arg(memberSize);
                else if (memberSize>0)
                    sText = QString("%1 [%2]").arg(groupItemInfo->m_sName.c_str()).arg(memberSize);
                else
                    sText = groupItemInfo->m_sName.c_str();
                groupItemInfo->m_hItem->setText( 0,sText );
                if (groupItemInfo->m_sParentId>0 && theApp->isStatSubGroupMember()) {
                    EbWidgetItemInfo::pointer parentItemInfo;
                    if (m_pDepItemInfo.find(groupItemInfo->m_sParentId,parentItemInfo)) {
                        m_pUpdateGroupCountList.insert(parentItemInfo->m_sGroupCode, parentItemInfo, false);
                        if (m_timerIdUpdateGroupCount!=0) {
                            this->killTimer(m_timerIdUpdateGroupCount);
                        }
                        m_timerIdUpdateGroupCount = this->startTimer(1000);
                        return;
                    }
                }
            }
        }
    }
    QWidget::timerEvent(e);
}

void EbWidgetMyEnterprise::setGroupCount(const EbWidgetItemInfo::pointer &pGroupItem, eb::bigint sGroupCode, bool bFromMemberInfo)
{
    if (pGroupItem.get()==0 || pGroupItem->m_hItem==0 || sGroupCode==0) {
        return;
    }
    EB_GroupInfo pGroupInfo;
    if (theApp->m_ebum.EB_GetGroupInfo(sGroupCode,&pGroupInfo)) {
        setGroupCount(pGroupItem, &pGroupInfo, bFromMemberInfo);
    }
}

void EbWidgetMyEnterprise::setGroupCount(const EbWidgetItemInfo::pointer &pGroupItem, const EB_GroupInfo *pGroupInfo, bool bFromMemberInfo)
{
    if (pGroupItem.get()==0 || pGroupItem->m_hItem==0 || pGroupInfo==0) {
        return;
    }
    EbWidgetItemInfo::pointer pEntItemInfo;
    if (m_pEntItemInfo.find(pGroupInfo->m_sEnterpriseCode,pEntItemInfo)) {
        if (m_timerIdUpdateEnterpriseCount!=0) {
            this->killTimer(m_timerIdUpdateEnterpriseCount);
        }
        m_pUpdateEntItemInfo = pEntItemInfo;
        m_timerIdUpdateEnterpriseCount = this->startTimer(1000);
    }
    if (pGroupItem->m_nCount1==-1 || !theApp->isStatSubGroupMember()) {
        /// 统计当前部门人数
        int memberSize = 0;
        int onlineSize = 0;
        theApp->m_ebum.EB_GetGroupMemberSize(pGroupInfo->m_sGroupCode,1,memberSize,onlineSize);
        if (memberSize==pGroupItem->m_nCount1 && onlineSize==pGroupItem->m_nCount2) {
            return;
        }
        pGroupItem->m_nCount1 = memberSize;
        pGroupItem->m_nCount2 = onlineSize;
        QString sText;
        if (onlineSize>=0 && memberSize>0)
            sText = QString("%1 [%2/%3]").arg(pGroupInfo->m_sGroupName.c_str()).arg(onlineSize).arg(memberSize);
        else if (memberSize>0)
            sText = QString("%1 [%2]").arg(pGroupInfo->m_sGroupName.c_str()).arg(memberSize);
        else
            sText = pGroupInfo->m_sGroupName.c_str();
        pGroupItem->m_hItem->setText( 0,sText );
    }
    if (theApp->isStatSubGroupMember() ) {
        /// 统计包含子部门人数
        m_pUpdateGroupCountList.insert(pGroupItem->m_sGroupCode, pGroupItem, false);
        if (m_timerIdUpdateGroupCount!=0) {
            this->killTimer(m_timerIdUpdateGroupCount);
        }
        m_timerIdUpdateGroupCount = this->startTimer(1500);
    }
}

void EbWidgetMyEnterprise::onItemClicked(QTreeWidgetItem *item, int /*column*/)
{
    const EbTreeWidgetItem* ebitem = (EbTreeWidgetItem*)item;
    const EbWidgetItemInfo::ITEM_TYPE itemType = ebitem->m_itemInfo->m_nItemType;
    if (itemType==EbWidgetItemInfo::ITEM_TYPE_GROUP || itemType==EbWidgetItemInfo::ITEM_TYPE_ENTERPRISE) {
        if (itemType==EbWidgetItemInfo::ITEM_TYPE_GROUP) {
            theApp->m_ebum.EB_LoadGroup(ebitem->m_itemInfo->m_sGroupCode,true);
        }
        if (item->isExpanded()) {
            m_treeWidget->collapseItem(item);
        }
        else {
            m_treeWidget->expandItem(item);
        }
    }
}
void EbWidgetMyEnterprise::onItemEntered(QTreeWidgetItem *item, int /*column*/)
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
        m_pushButtonSelect->setVisible(false);
        if ( ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_ENTERPRISE ) {
            m_pushButtonEdit->setVisible(false);
            m_pushButtonCall->setVisible(false);
        }
        else if ( ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_GROUP ) {
            m_pushButtonEdit->setVisible(false);
            const eb::bigint myMemberId = ebitem->m_itemInfo->m_nBigId;
            if (myMemberId>0) {
                m_pushButtonCall->setGeometry( pointItem.x()-buttonSize,y+1,buttonSize,buttonSize );
                m_pushButtonCall->setProperty("track-item",QVariant((quint64)item));
                m_pushButtonCall->setVisible(true);
            }
            else {
                m_pushButtonCall->setVisible(false);
            }
        }
        else if ( ebitem->m_itemInfo->m_nUserId==theApp->m_ebum.EB_GetLogonUserId() ) {
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

void EbWidgetMyEnterprise::onClickedPushButtonSelect()
{
    const EbTreeWidgetItem * ebitem = (EbTreeWidgetItem*)m_pushButtonSelect->property("track-item").toULongLong();
    if (ebitem!=0 && ebitem->m_itemInfo.get()!=0) {
        emit selectedItemInfo(ebitem->m_itemInfo);
    }
    m_pushButtonSelect->hide();
    m_pushButtonSelect->setProperty("track-item",QVariant((quint64)0));
    m_treeWidget->setFocus();
}

//void EbWidgetMyEnterprise::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
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

void EbWidgetMyEnterprise::onClickedPushButtonCall(void)
{
    const EbTreeWidgetItem * item = (EbTreeWidgetItem*)m_pushButtonCall->property("track-item").toULongLong();
    if (item!=0) {
        createMenuData();
        m_contextMenu->onCallItem(item->m_itemInfo);
    }
    m_pushButtonCall->hide();
    m_pushButtonCall->setProperty("track-item",QVariant((quint64)0));
    m_treeWidget->setFocus();
}

void EbWidgetMyEnterprise::onClickedPushButtonEdit()
{
    const EbTreeWidgetItem* item = (EbTreeWidgetItem*)m_pushButtonEdit->property("track-item").toULongLong();
    if (item==0) {
        return;
    }
    m_pushButtonEdit->hide();
    m_pushButtonEdit->setProperty("track-item",QVariant((quint64)0));
    m_treeWidget->setFocus();
    createMenuData();
    m_contextMenu->onEditItem(item->m_itemInfo);
}

#include "ebframelist.h"
#include <QDateTime>
#include "ebdialogchatbase.h"
//#include "ebclientapp.h"

EbFrameList::EbFrameList(QWidget* parent)
    : m_pParent(parent)
    , m_leftWidth(0)
{
    assert(m_pParent!=0);

}

EbFrameList::~EbFrameList()
{
    /// * 退出清空 LIST，用于 EbFrameItem 构销 DELETE 相应控件
    m_list.clear();
    m_hide.clear();
    m_close.clear();
}

void EbFrameList::timerCheckState()
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& frameInfo = *pIter;
        if (frameInfo->timerCheckState()) {
            return ;
        }
    }
}

EbDialogWorkFrame *EbFrameList::getWorkFrame() const
{
    AUTO_CONST_RLOCK(m_list);
    CLockList<EbFrameItem::pointer>::const_iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& pFrameWndInfo = *pIter;
        if ( pFrameWndInfo->isItemType(EbFrameItem::FRAME_ITEM_WORK_FRAME) ) {
            return pFrameWndInfo->dialogWorkFrame();
        }
    }
    return 0;
}

void EbFrameList::addFrameItem(const EbFrameItem::pointer &frameItem,bool bShow, bool bAutoCall, bool bForceAddToList,int nInsertOffset)
{
    if ( frameItem->isItemType(EbFrameItem::FRAME_ITEM_WORK_FRAME) ) {
        if ( showWorkFrame()!=0 ) {
            return;
        }
    }
    if (!bForceAddToList && !bShow) {
        m_hide.add(frameItem);
        return;
    }
    if (m_list.empty()) {
        bForceAddToList = true;	/// 完善show
        bShow = true;
    }

    int nInsertIndex = -1;
    if (nInsertOffset>=0 && nInsertOffset<(int)m_list.size()) {
        int nCurrentIndex = 0;
        BoostWriteLock wtLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            if ((nCurrentIndex++)==nInsertOffset) {
                m_list.insert(pIter,frameItem);
                nInsertIndex = nInsertOffset;
                break;
            }
        }
    }

    frameItem->buildButton( m_leftWidth,m_pParent );
    if (nInsertIndex>=0) {
        onMove();
    }
    else {
        nInsertIndex = (int)m_list.size();
        m_list.add(frameItem);
    }
    frameItem->onResize( nInsertIndex,m_pParent->geometry(),m_leftWidth );
    if ( frameItem->isItemType(EbFrameItem::FRAME_ITEM_WORK_FRAME) ) {
        if (bShow)
            showByIndex(nInsertIndex);
        else
            frameItem->setChecked(false);
    }
    else {
        EbDialogChatBase::pointer chatBase = frameItem->dialogChatBase();
        if (chatBase.get()!=0) {
            m_pParent->connect( chatBase.get(),SIGNAL(clickedClose()),m_pParent,SLOT(onClickedCloseFromDialogChatBase()) );
            /// 聊天会话是否当前隐藏
            const bool bCallDialogHide = chatBase->isVisible()?false:true;
            if (bShow && (bForceAddToList || (!bAutoCall && !bCallDialogHide && m_pParent->isVisible())))
                showByIndex(nInsertIndex);
            else if (!bShow) {
                frameItem->setChecked(false);
            }
            chatBase->scrollToEnd();
        }
    }
}

EbDialogChatBase::pointer EbFrameList::getDialogChatBase(mycp::bigint nCallId, bool bRemove, bool bAutoCall)
{
//    if (theWantExitApp) return NullDlgDialog;

    if (nCallId==0) {
        return EbDialogChatBaseNull;
    }
    {
        BoostWriteLock wtLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        int currentIndex = 0;
        for (; pIter!=m_list.end(); pIter++) {
            const EbFrameItem::pointer pFrameWndInfo = *pIter;
            currentIndex++;
            if (pFrameWndInfo->callId()!=nCallId) {
                continue;
            }
            if (bRemove) {
                m_list.erase(pIter);
                wtLock.unlock();
                if (m_list.empty()) {
                    this->m_pParent->hide();
                    return pFrameWndInfo->dialogChatBase();
                }
                if ( !this->showByIndex(currentIndex) ) {
                    this->showByIndex(currentIndex-1);
                }
                onMove();
            }
            else {// if (pFrameWndInfo->IsChecked() && m_pParent->IsWindowVisible())
                if (!bAutoCall && pFrameWndInfo->isChecked() && m_pParent->isVisible()) {
                    pFrameWndInfo->clearUnreadMsg();
                }
            }
            return pFrameWndInfo->dialogChatBase();
        }
    }
    {
        BoostWriteLock wtLock(m_hide.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_hide.begin();
        for (; pIter!=m_hide.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            if (pFrameWndInfo->callId()!=nCallId) {
                continue;
            }
            const EbFrameItem::pointer pFrameWndInfoTemp = pFrameWndInfo;
            if (bRemove)
                m_hide.erase(pIter);
            return pFrameWndInfoTemp->dialogChatBase();
        }
    }
    return EbDialogChatBaseNull;
}

bool EbFrameList::addUnreadMsg(eb::bigint nCallId, eb::bigint nMsgId)
{
    bool bResult = false;
    {
        BoostReadLock rdLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            const EbFrameItem::pointer& frameItem = *pIter;
            if (frameItem->callId()==nCallId) {
                frameItem->addUnreadMsg();
                bResult = true;
                goto AddUnreadMsgCount;
            }
        }
    }

    //if (m_pParent->IsWindowVisible())
    {
        BoostWriteLock wtLock(m_hide.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_hide.begin();
        for (; pIter!=m_hide.end(); pIter++) {
            const EbFrameItem::pointer& frameItem = *pIter;
            if (frameItem->callId()==nCallId) {
                const EbFrameItem::pointer frameItemTemp = frameItem;
                m_hide.erase(pIter);
                wtLock.unlock();
                addFrameItem( frameItemTemp,true,false );
                frameItemTemp->addUnreadMsg();
                bResult = true;
                goto AddUnreadMsgCount;
            }
        }
    }
AddUnreadMsgCount:
    if (nMsgId>0) {
        /// *** read_flag=1 已经读；
        /// *** read_flag=2 对方接收回执
        char sql[256];
        sprintf(sql,"UPDATE msg_record_t SET read_flag=read_flag&2 WHERE msg_id=%lld AND (read_flag&1)=1",nMsgId);	// 1->0;3->2
        theApp->m_sqliteUser->execute(sql);
    }
    return bResult;
}

bool EbFrameList::setUnreadMsg(eb::bigint nCallId, size_t nUnreadMsgCount)
{
    {
        BoostReadLock rdLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++)
        {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            if (pFrameWndInfo->callId()==nCallId)
            {
                pFrameWndInfo->setUnreadMsg(nUnreadMsgCount);
                return true;
            }
        }
    }
    return false;
}

bool EbFrameList::existFrameItem(eb::bigint nCallId) const
{
    if (nCallId==0) return false;
    {
        AUTO_CONST_RLOCK(m_list);
        CLockList<EbFrameItem::pointer>::const_iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            if (pFrameWndInfo->callId()==nCallId) {
                return true;
            }
        }
    }
    {
        AUTO_CONST_RLOCK(m_hide);
        CLockList<EbFrameItem::pointer>::const_iterator pIter = m_hide.begin();
        for (; pIter!=m_hide.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            if (pFrameWndInfo->callId()==nCallId) {
                return true;
            }
        }
    }
    return false;
}

void EbFrameList::onUserLineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE nLineState)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& pFrameWndInfo = *pIter;
        pFrameWndInfo->onUserLineStateChange(nGroupCode, nUserId, nLineState);
    }
}

void EbFrameList::onMemberInfo(const EB_MemberInfo *pMemberInfo, bool bChangeLineState)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& pFrameWndInfo = *pIter;
        const EbDialogChatBase::pointer & chatBase = pFrameWndInfo->dialogChatBase();
        if (chatBase.get()!=0) {
            chatBase->onMemberInfo(pMemberInfo,bChangeLineState);
        }
    }
}

void EbFrameList::onGroupInfo(const EB_GroupInfo *pGroupInfo)
{
    {
        BoostReadLock rdLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            pFrameWndInfo->onGroupInfo(pGroupInfo);
        }
    }
    {
        BoostReadLock rdLock(m_hide.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_hide.begin();
        for (; pIter!=m_hide.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            pFrameWndInfo->onGroupInfo(pGroupInfo);
        }
    }
}

void EbFrameList::onRemoveGroup(const EB_GroupInfo* groupInfo)
{
    {
        BoostReadLock rdLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            const EbDialogChatBase::pointer & chatBase = pFrameWndInfo->dialogChatBase();
            if (chatBase.get()!=0) {
                chatBase->onRemoveGroup(groupInfo);
            }
        }
    }
    {
        AUTO_CONST_RLOCK(m_hide);
        CLockList<EbFrameItem::pointer>::const_iterator pIter = m_hide.begin();
        for (; pIter!=m_hide.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            const EbDialogChatBase::pointer & chatBase = pFrameWndInfo->dialogChatBase();
            if (chatBase.get()!=0) {
                chatBase->onRemoveGroup(groupInfo);
            }
        }
    }
}

void EbFrameList::onRemoveMember(const EB_GroupInfo* groupInfo, eb::bigint nMemberId, mycp::bigint memberUserId)
{
    {
        BoostReadLock rdLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            const EbDialogChatBase::pointer & chatBase = pFrameWndInfo->dialogChatBase();
            if (chatBase.get()!=0) {
                chatBase->onRemoveMember( groupInfo,nMemberId,memberUserId );
            }
        }
    }

    {
        AUTO_CONST_RLOCK(m_hide);
        CLockList<EbFrameItem::pointer>::const_iterator pIter = m_hide.begin();
        for (; pIter!=m_hide.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            const EbDialogChatBase::pointer & chatBase = pFrameWndInfo->dialogChatBase();
            if (chatBase.get()!=0) {
                chatBase->onRemoveMember( groupInfo,nMemberId,memberUserId );
            }
        }
    }
}

void EbFrameList::onMemberHeadChange(const EB_MemberInfo *pMemberInfo)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& pFrameWndInfo = *pIter;
        if (pFrameWndInfo->groupId()==0 && pFrameWndInfo->fromUserId()==pMemberInfo->m_nMemberUserId) {
            pFrameWndInfo->onMemberHeadChange(pMemberInfo);
            break;
        }
    }
}

void EbFrameList::onContactHeadChange(const EB_ContactInfo *pContactInfo)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& pFrameWndInfo = *pIter;
        if (pFrameWndInfo->groupId()==0 && pFrameWndInfo->fromUserId()==pContactInfo->m_nContactUserId) {
            pFrameWndInfo->onContactHeadChange(pContactInfo);
            break;
        }
    }
}

void EbFrameList::showByCallId(mycp::bigint nCallId)
{
    if (nCallId==0) return;
    bool bFindInfo = false;
    {
        BoostReadLock rdLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            if (pFrameWndInfo->callId()==nCallId) {
                bFindInfo = true;
                pFrameWndInfo->clearUnreadMsg();
                pFrameWndInfo->setChecked(true);
//                if (m_pCallback!=NULL)
//                    m_pCallback->OnFrameWndShow(pFrameWndInfo,true);
            }
            else {
                pFrameWndInfo->setChecked(false);
//                if (m_pCallback!=NULL)
//                    m_pCallback->OnFrameWndShow(pFrameWndInfo,false);
            }
        }
    }
    if (!bFindInfo) {
        BoostWriteLock wtLock(m_hide.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_hide.begin();
        for (; pIter!=m_hide.end(); pIter++) {
            const EbFrameItem::pointer& pFrameWndInfo = *pIter;
            if (pFrameWndInfo->callId()!=nCallId) {
                continue;
            }
            const EbFrameItem::pointer pFrameWndInfoTemp = pFrameWndInfo;
            m_hide.erase(pIter);
            wtLock.unlock();
            addFrameItem( pFrameWndInfoTemp,true,false,true );
            break;
        }
    }

}

void EbFrameList::onResize(const QRect& rect)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    int index = 0;
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& frameInfo = *pIter;
        frameInfo->onResize(index++, rect, m_leftWidth);
    }

}

void EbFrameList::onMove()
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    int index = 0;
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& frameInfo = *pIter;
        frameInfo->onMove(index++);
    }
}

void EbFrameList::onMoveEvent()
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& frameInfo = *pIter;
        frameInfo->onMoveEvent();
    }
}

void EbFrameList::checkCloseItem()
{
    const qint64 nowTime = QDateTime::currentSecsSinceEpoch();
    BoostWriteLock wtLock(m_hide.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_hide.begin();
    for (; pIter!=m_hide.end(); ) {
        const EbFrameItem::pointer& frameInfo = *pIter;
        const qint64 closeTime = frameInfo->closeTime();
        if (closeTime==0 || (closeTime+300)<nowTime) {
            /// 300=5分钟后，自动关闭隐藏窗口
            m_hide.erase(pIter++);
        }
        else {
            pIter++;
        }
    }
}

bool EbFrameList::showByIndex(int showIndex)
{
    bool result = false;
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    int index = 0;
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& frameInfo = *pIter;
        const bool checked = (showIndex==index)?true:false;
        if ( !result && checked ) {
            /// 找到指定 INDEX
            result = true;
        }
        frameInfo->setChecked(checked);
        index++;
    }
    return result;
}

EbDialogWorkFrame * EbFrameList::showWorkFrame()
{
    EbDialogWorkFrame * result = 0;
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& frameInfo = *pIter;
        if ( result==0 && frameInfo->isItemType(EbFrameItem::FRAME_ITEM_WORK_FRAME)) {
            result = frameInfo->dialogWorkFrame();
            if ( frameInfo->isChecked() ) {
                /// 当前已经 checked ，直接退出
                break;
            }
            frameInfo->setChecked(true);
        }
        else {
            frameInfo->setChecked(false);
        }
    }
    return result;
}

void EbFrameList::clickedLeftButton(const QPushButton *leftButton, const QPoint& pt)
{
    /// *需要判断是否点击了X关闭按钮
    EbFrameItem::pointer frameInfoClose;
    {
        /// 同时判断是否点击了关闭按钮，并切换到不同的ITEM显示
        EbFrameItem::pointer frameInfoPrevChecked;    /// 点击关闭前一个checked ITEM
        EbFrameItem::pointer frameInfoPrevClose;    /// 点击关闭前一个ITEM
        BoostWriteLock wtLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); ) {
            const EbFrameItem::pointer& frameInfo = *pIter;
            if (frameInfoClose.get()!=0 && frameInfoClose->isChecked()) {
                /// *前面处理，关闭了一个显示ITEM，并且没有前面可显示ITEM，设置当前（下一个）ITEM显示，然后退出
                frameInfo->setChecked(true);
                break;
            }
            /// * 检查按钮点击状态：1=点击关闭，2=左边点击，0=没有点击
            const int clickState = frameInfo->checkLeftButtonClickState( leftButton,pt );
            if (clickState==2) {
                frameInfo->setChecked(true);
                frameInfo->clearUnreadMsg(true);
                frameInfoPrevChecked.reset();
            }
            else if (clickState==1) {
                frameInfoClose = frameInfo;
                wtLock.unlock();
                frameInfoClose->requestClose();
//                return;
//                frameInfoClose = frameInfo;
//                m_hide.add(frameInfo);
//                m_list.erase(pIter++);
                if (frameInfoClose->isChecked() && frameInfoPrevClose.get()!=0) {
                    /// *关闭显示ITEM，显示前一个 ITEM，然后退出
                    frameInfoPrevClose->setChecked(true);
                }
                else if(frameInfoClose->isChecked()) {
                    /// *关闭显示ITEM，需要显示下一个ITEM
                }
                else {
                    /// *关闭其他ITEM，直接退出
                    if (frameInfoPrevChecked.get()!=NULL) {
                        /// 重新设置前一个 checked ITEM
                        frameInfoPrevChecked->setChecked(true);
                    }
                }
                return;
            }
            else {
                if (frameInfoPrevChecked.get()==0 && frameInfo->isChecked()) {
                    frameInfoPrevChecked = frameInfo;
                }
                frameInfo->setChecked(false);
            }
            /// *记录前一个ITEM，方便处理
            frameInfoPrevClose = frameInfo;
            pIter++;
        }
    }
    if (frameInfoClose.get()!=0) {
        /// *点击关闭某个 ITEM，上下移动调整按钮位置
        frameInfoClose->setChecked( false,true );
        frameInfoClose->setCloseTime( QDateTime::currentSecsSinceEpoch() );
        onMove();
    }
}

void EbFrameList::showFirst(void)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    int index = 0;
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& frameInfo = *pIter;
        if ( 0==index && frameInfo->isChecked() ) {
            break;
        }
        const bool checked = (0==index)?true:false;
        frameInfo->setChecked(checked);
        index++;
    }
}

void EbFrameList::closeItem(const EbDialogChatBase *chatBase)
{
    if (chatBase==0) {
        return;
    }
    EbFrameItem::pointer frameInfoClose;
    {
        EbFrameItem::pointer frameInfoPrevClose;    /// 点击关闭前一个ITEM

        BoostWriteLock wtLock(m_list.mutex());
        CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); ) {
            const EbFrameItem::pointer& frameInfo = *pIter;
            if (frameInfoClose.get()!=0 && frameInfoClose->isChecked()) {
                /// 前面关闭 ITEM checked 显示中，显示下一个，然后退出
                frameInfo->setChecked(true);
                break;
            }
            if (chatBase==frameInfo->dialogChatBase().get()) {
                /// * 找到要关闭 ITEM
                frameInfoClose = frameInfo;
                m_hide.add(frameInfo);
                m_list.erase(pIter++);
                if (!frameInfoClose->isChecked()) {
                    /// 不是当前显示 checked，直接跳出
                    break;
                }
                else if (frameInfoPrevClose.get()!=0){
                    /// * 当前关闭 ITEM 正在 checked 显示，设置前一个为 checked 显示，然后退出
                    frameInfoPrevClose->setChecked(true);
                    break;
                }
//                else {
//                    /// 当前关闭 ITEM 正在 checked 显示，找下一个 ITEM
//                }
                continue;
            }
            frameInfoPrevClose = frameInfo;
            pIter++;
        }
    }

    if (frameInfoClose.get()!=0) {
        /// *点击关闭某个 ITEM，上下移动调整按钮位置
        frameInfoClose->setChecked( false,true );
        frameInfoClose->setCloseTime( QDateTime::currentSecsSinceEpoch() );
        onMove();
    }
}

void EbFrameList::checkShowHideCloseButton(const QPoint& pt)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbFrameItem::pointer>::iterator pIter = m_list.begin();
    bool bFindVisible = false;
    for (; pIter!=m_list.end(); pIter++) {
        const EbFrameItem::pointer& frameInfo = *pIter;
        if (bFindVisible) {
            /// *显示第一个后，其他设为 hide，提高性能；
            frameInfo->setCloseButtonVisible(false);
        }
        else {
            bFindVisible = frameInfo->checkShowHideCloseButton(pt);
        }
    }
}

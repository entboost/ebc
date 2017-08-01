#include "ebworklist.h"

EbWorkList::EbWorkList(QWidget * parent)
    : m_pParent(parent)
    , m_topHeight(0)
    , m_leftOffset(0)
{
    assert(m_pParent!=0);

}

EbWorkList::~EbWorkList()
{
    m_list.clear();

}

void EbWorkList::setCheckedFocus()
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->setCheckedFocus()) {
            break;
        }
    }
}

EbWidgetUserInfo *EbWorkList::widgetUserInfo() const
{
    AUTO_CONST_RLOCK(m_list);
    CLockList<EbWorkItem::pointer>::const_iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if ( frameInfo->isItemType(EbWorkItem::WORK_ITEM_USER_INFO) ) {
            return frameInfo->widgetUserInfo();
        }
    }
    return 0;
}

EbWidgetUserList *EbWorkList::widgetUserList() const
{
    AUTO_CONST_RLOCK(m_list);
    CLockList<EbWorkItem::pointer>::const_iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if ( frameInfo->isItemType(EbWorkItem::WORK_ITEM_USER_LIST) ) {
            return frameInfo->widgetUserList();
        }
    }
    return 0;
}

EbWidgetChatRecord *EbWorkList::widgetChatRecord() const
{
    AUTO_CONST_RLOCK(m_list);
    CLockList<EbWorkItem::pointer>::const_iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if ( frameInfo->isItemType(EbWorkItem::WORK_ITEM_CHAT_RECORD) ) {
            return frameInfo->widgetChatRecord();
        }
    }
    return 0;
}

EbWidgetFileTranList *EbWorkList::widgetTranFile() const
{
    AUTO_CONST_RLOCK(m_list);
    CLockList<EbWorkItem::pointer>::const_iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if ( frameInfo->isItemType(EbWorkItem::WORK_ITEM_TRAN_FILE) ) {
            return frameInfo->widgetTranFile();
        }
    }
    return 0;
}

void EbWorkList::addWorkItem(bool saveUrl,const EbWorkItem::pointer &workItem, int nInsertOffset)
{
    int nInsertIndex = -1;
    if (nInsertOffset>=0 && nInsertOffset<(int)m_list.size()) {
        int nCurrentIndex = 0;
        BoostWriteLock wtLock(m_list.mutex());
        CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            if ((nCurrentIndex++)==nInsertOffset) {
                m_list.insert(pIter,workItem);
                nInsertIndex = nInsertOffset;
                break;
            }
        }
    }
    int topButtonRightPos = -1;
    int x = onMove();
    if (nInsertIndex>=0) {
        topButtonRightPos = x;
    }
    else {
        nInsertIndex = (int)m_list.size();
        m_list.add(workItem);
    }

    workItem->buildButton( saveUrl,m_topHeight,m_pParent );
    x = workItem->onResize( x,m_pParent->geometry(),m_topHeight,m_leftOffset );
//    const int x = workItem->onResize( nInsertIndex,m_pParent->geometry(),m_topHeight,m_leftOffset );
    showByIndex( nInsertIndex,false );
    if (topButtonRightPos>0)
        onItemSizeChange( workItem,(int)m_list.size(),topButtonRightPos );
    else
        onItemSizeChange( workItem,(int)m_list.size(),x );
}

void EbWorkList::showByIndex(int showIndex, bool bSearchFocus)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    int index = 0;
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        const bool checked = (showIndex==index)?true:false;
        frameInfo->setChecked(checked,false,bSearchFocus);
        index++;
    }
}

void EbWorkList::setItemText(EbWorkItem::WORK_ITEM_TYPE itemType, const QString &text)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->onTitleChanged(itemType,text)) {
            break;
        }
    }
}

void EbWorkList::clickedTopButton(const QPushButton *topButton, const QPoint &pt)
{
    /// *需要判断是否点击了X关闭按钮
    EbWorkItem::pointer frameInfoClose;
//    int leftOffset = - 1;
    {
        /// 同时判断是否点击了关闭按钮，并切换到不同的ITEM显示
        EbWorkItem::pointer frameInfoPrevChecked;    /// 点击关闭前一个checked ITEM
        EbWorkItem::pointer frameInfoPrevClose;    /// 点击关闭前一个ITEM
        BoostWriteLock wtLock(m_list.mutex());
        CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); ) {
            const EbWorkItem::pointer& frameInfo = *pIter;
//            if (leftOffset==-1) {
//                leftOffset = frameInfo->rectButton().left();
//            }
            if (frameInfoClose.get()!=0 && frameInfoClose->isChecked()) {
                /// *前面处理，关闭了一个显示ITEM，并且没有前面可显示ITEM，设置当前（下一个）ITEM显示，然后退出
                frameInfo->setChecked(true);
                break;
            }
            /// * 检查按钮点击状态：1=点击关闭，2=左边点击，0=没有点击
            const int clickState = frameInfo->checkTopButtonClickState( topButton,pt );
            if (clickState==2) {
                frameInfo->setChecked(true);
                frameInfoPrevChecked.reset();
            }
            else if (clickState==1) {
                frameInfoClose = frameInfo;
//                m_hide.add(frameInfo);
                m_list.erase(pIter++);
                if (frameInfoClose->isChecked() && frameInfoPrevClose.get()!=0) {
                    /// *关闭显示ITEM，显示前一个 ITEM，然后退出
                    frameInfoPrevClose->setChecked(true);
                    break;
                }
                else if(frameInfoClose->isChecked()) {
                    /// *关闭显示ITEM，需要显示下一个ITEM
                    continue;
                }
                else {
                    /// *关闭其他ITEM，直接退出
                    if (frameInfoPrevChecked.get()!=NULL) {
                        /// 重新设置前一个 checked ITEM
                        frameInfoPrevChecked->setChecked(true);
                    }
                    break;
                }
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
//        frameInfoClose->setCloseTime( QDateTime::currentSecsSinceEpoch() );
        const int x = onMove();
        onItemSizeChange( frameInfoClose,(int)m_list.size(),x );
    }
}

void EbWorkList::showFirst()
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    int index = 0;
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if ( 0==index && frameInfo->isChecked() ) {
            break;
        }
        const bool checked = (0==index)?true:false;
        frameInfo->setChecked(checked);
        index++;
    }
}

int EbWorkList::indexOf(EbWorkItem::WORK_ITEM_TYPE type, bool sendClose) const
{
    AUTO_CONST_RLOCK(m_list);
    CLockList<EbWorkItem::pointer>::const_iterator pIter = m_list.begin();
    int index = 0;
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->isItemType(type)) {
            if (sendClose) {
                frameInfo->sendClose();
            }
            return index;
        }
        index++;
    }
    return -1;
}

int EbWorkList::indexOf(const EbWidgetWorkView *view) const
{
    AUTO_CONST_RLOCK(m_list);
    CLockList<EbWorkItem::pointer>::const_iterator pIter = m_list.begin();
    int index = 0;
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->widgetWorkView().get()==view) {
            return index;
        }
        index++;
    }
    return -1;
}

void EbWorkList::closeItem(const EbWidgetWorkView * view)
{
    EbWorkItem::pointer frameInfoClose;
    {
        /// 同时判断是否点击了关闭按钮，并切换到不同的ITEM显示
        EbWorkItem::pointer frameInfoPrevChecked;    /// 点击关闭前一个checked ITEM
        EbWorkItem::pointer frameInfoPrevClose;    /// 点击关闭前一个ITEM
        BoostWriteLock wtLock(m_list.mutex());
        CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); ) {
            const EbWorkItem::pointer& frameInfo = *pIter;
            if (frameInfoClose.get()!=0 && frameInfoClose->isChecked()) {
                /// *前面处理，关闭了一个显示ITEM，并且没有前面可显示ITEM，设置当前（下一个）ITEM显示，然后退出
                frameInfo->setChecked(true);
                break;
            }
            if (frameInfo->widgetWorkView().get()==view) {
                frameInfoClose = frameInfo;
                m_list.erase(pIter++);
                if (frameInfoClose->isChecked() && frameInfoPrevClose.get()!=0) {
                    /// *关闭显示ITEM，显示前一个 ITEM，然后退出
                    frameInfoPrevClose->setChecked(true);
                    break;
                }
                else if(frameInfoClose->isChecked()) {
                    /// *关闭显示ITEM，需要显示下一个ITEM
                    continue;
                }
                else {
                    /// *关闭其他ITEM，直接退出
                    if (frameInfoPrevChecked.get()!=NULL) {
                        /// 重新设置前一个 checked ITEM
                        frameInfoPrevChecked->setChecked(true);
                    }
                    break;
                }
            }
            else {
                if (frameInfoPrevChecked.get()==0 && frameInfo->isChecked()) {
                    frameInfoPrevChecked = frameInfo;
                }
            }
            /// *记录前一个ITEM，方便处理
            frameInfoPrevClose = frameInfo;
            pIter++;
         }
    }
    if (frameInfoClose.get()!=0) {
        /// *点击关闭某个 ITEM，上下移动调整按钮位置
        frameInfoClose->setChecked( false,true );
        const int x = onMove();
        onItemSizeChange( frameInfoClose,(int)m_list.size(),x );
    }
}

void EbWorkList::closeItem(int index)
{
    EbWorkItem::pointer frameInfoClose;
    {
        /// 同时判断是否点击了关闭按钮，并切换到不同的ITEM显示
        EbWorkItem::pointer frameInfoPrevChecked;    /// 点击关闭前一个checked ITEM
        EbWorkItem::pointer frameInfoPrevClose;    /// 点击关闭前一个ITEM
        BoostWriteLock wtLock(m_list.mutex());
        CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
        int currentIndex = 0;
        for (; pIter!=m_list.end(); ) {
            const EbWorkItem::pointer& frameInfo = *pIter;
            if (frameInfoClose.get()!=0 && frameInfoClose->isChecked()) {
                /// *前面处理，关闭了一个显示ITEM，并且没有前面可显示ITEM，设置当前（下一个）ITEM显示，然后退出
                frameInfo->setChecked(true);
                break;
            }
            if (index==(currentIndex++)) {
                frameInfoClose = frameInfo;
                m_list.erase(pIter++);
                if (frameInfoClose->isChecked() && frameInfoPrevClose.get()!=0) {
                    /// *关闭显示ITEM，显示前一个 ITEM，然后退出
                    frameInfoPrevClose->setChecked(true);
                    break;
                }
                else if(frameInfoClose->isChecked()) {
                    /// *关闭显示ITEM，需要显示下一个ITEM
                    continue;
                }
                else {
                    /// *关闭其他ITEM，直接退出
                    if (frameInfoPrevChecked.get()!=NULL) {
                        /// 重新设置前一个 checked ITEM
                        frameInfoPrevChecked->setChecked(true);
                    }
                    break;
                }
            }
            else {
                if (frameInfoPrevChecked.get()==0 && frameInfo->isChecked()) {
                    frameInfoPrevChecked = frameInfo;
                }
            }
            /// *记录前一个ITEM，方便处理
            frameInfoPrevClose = frameInfo;
            pIter++;
         }
    }
    if (frameInfoClose.get()!=0) {
        /// *点击关闭某个 ITEM，上下移动调整按钮位置
        frameInfoClose->setChecked( false,true );
        const int x = onMove();
        onItemSizeChange( frameInfoClose,(int)m_list.size(),x );
    }
}

void EbWorkList::clear()
{
    m_list.clear();
}

void EbWorkList::checkShowHideCloseButton(const QPoint &pt)
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    bool bFindVisible = false;
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (bFindVisible) {
            /// *显示第一个后，其他设为 hide，提高性能；
            frameInfo->setCloseButtonVisible(false);
        }
        else {
            bFindVisible = frameInfo->checkShowHideCloseButton(pt);
        }
    }
}

int EbWorkList::onResize(const QRect &rect)
{
    int result = m_leftOffset;
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        result = frameInfo->onResize( result, rect, m_topHeight, m_leftOffset );
    }
    return result;
}

int EbWorkList::onMove(void)
{
    int result = m_leftOffset;
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    int index = 0;
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        result = frameInfo->onMove( result );
    }
    return result;
}

//int EbWorkList::topButtonWidth() const
//{
//    int result = 0;
//    AUTO_CONST_RLOCK(m_list);
//    CLockList<EbWorkItem::pointer>::const_iterator pIter = m_list.begin();
//    for (; pIter!=m_list.end(); pIter++) {
//        const EbWorkItem::pointer& frameInfo = *pIter;
//        result = frameInfo->topGeometry().right();
//    }
//    return result;
//}

void EbWorkList::back()
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->back()) {
            break;
        }
    }
}

void EbWorkList::forward()
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->forward()) {
            break;
        }
    }
}

bool EbWorkList::canGoBack() const
{
    bool bCanGoBack = false;
    AUTO_CONST_RLOCK(m_list);
    CLockList<EbWorkItem::pointer>::const_iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->canGoBack(bCanGoBack)) {
            break;
        }
    }
    return bCanGoBack;
}

bool EbWorkList::canGoForward() const
{
    bool bCanGoForward = false;
    AUTO_CONST_RLOCK(m_list);
    CLockList<EbWorkItem::pointer>::const_iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->canGoForward(bCanGoForward)) {
            break;
        }
    }
    return bCanGoForward;
}

void EbWorkList::refreshOrStop()
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->refreshOrStop()) {
            break;
        }
    }
}

void EbWorkList::saveUrlHistory()
{
    BoostReadLock rdLock(m_list.mutex());
    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
    for (; pIter!=m_list.end(); pIter++) {
        const EbWorkItem::pointer& frameInfo = *pIter;
        if (frameInfo->saveUrlHistory()) {
            break;
        }
    }
}

//void EbWorkList::stop()
//{
//    BoostReadLock rdLock(m_list.mutex());
//    CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
//    for (; pIter!=m_list.end(); pIter++) {
//        const EbWorkItem::pointer& frameInfo = *pIter;
//        if (frameInfo->refreshOrStop()) {
//            break;
//        }
//    }
//}



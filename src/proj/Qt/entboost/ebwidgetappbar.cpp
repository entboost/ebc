#include "ebwidgetappbar.h"

EbWidgetAppBar::EbWidgetAppBar(EB_FUNC_LOCATION funcLocation, QWidget *parent)
    : QWidget(parent)
    , m_funcLocation(funcLocation)
    , m_orientation(Horizontal)
    , m_timerIdLoadFuncInfo(0)
{
    /// for test
//    this->setStyleSheet("background-color: rgb(0,250,250);");
    m_timerIdLoadFuncInfo = this->startTimer(2000);
}

EbWidgetAppBar::~EbWidgetAppBar()
{
    if (m_timerIdLoadFuncInfo!=0) {
        this->killTimer(m_timerIdLoadFuncInfo);
        m_timerIdLoadFuncInfo = 0;
    }
    m_lists.clear();
}

bool EbWidgetAppBar::existSubscribeId(qint64 id) const
{
    for (size_t i=0; i<m_lists.size(); i++) {
        if ( m_lists[i]->subscribeId()==id ) {
            return true;
        }
    }
    return false;
}

EbAppButtonItem::pointer EbWidgetAppBar::subscribeButtonItem(qint64 id) const
{
    for (size_t i=0; i<m_lists.size(); i++) {
        if ( m_lists[i]->subscribeId()==id ) {
            return m_lists[i];
        }
    }
    return EbAppButtonItemNull;
}

void EbWidgetAppBar::setSubscribeMsgCount( qint64 subscribeId, int msgCount )
{
    for (size_t i=0; i<m_lists.size(); i++) {
        if ( m_lists[i]->setMsgCount(subscribeId,msgCount) ) {
            break;
        }
    }
}

void EbWidgetAppBar::addSubscribeMsg(qint64 subscribeId)
{
    for (size_t i=0; i<m_lists.size(); i++) {
        if ( m_lists[i]->addMsgCount(subscribeId) ) {
            break;
        }
    }
}

void EbWidgetAppBar::onClickedAppItem(void)
{
    const QPushButton * button = (QPushButton*)sender();
    const qint64 subId = button->property( "sub-id" ).toLongLong();
    const EbAppButtonItem::pointer existAppButtonItem = subscribeButtonItem( subId );
    if ( existAppButtonItem.get()==0 ) {
        return;
    }
    emit clickedSubApp( existAppButtonItem->subscribeFuncInfo() );
}

void EbWidgetAppBar::onMove()
{
    for (size_t i=0; i<m_lists.size(); i++) {
        const EbAppButtonItem::pointer & buttonItem = m_lists[i];
        const QRect rect = getButtonIndexRect((int)i);
        buttonItem->move( rect.topLeft() );
    }
}

void EbWidgetAppBar::resizeEvent(QResizeEvent *event)
{
    onMove();
    QWidget::resizeEvent(event);
}

void EbWidgetAppBar::timerEvent(QTimerEvent *e)
{
    if ( m_timerIdLoadFuncInfo!=0 && m_timerIdLoadFuncInfo==e->timerId() ) {
        if ( loadFuncInfo() ) {
            this->killTimer(m_timerIdLoadFuncInfo);
            m_timerIdLoadFuncInfo = 0;
        }
    }
    QWidget::timerEvent(e);
}

const int const_button_interval = 2;
const QSize const_app_button_size(24,24);
bool EbWidgetAppBar::loadFuncInfo()
{
    bool hasNewAppButtonItem = false;
    std::vector<EB_SubscribeFuncInfo> m_pSubscribeFuncList;
    theApp->m_ebum.EB_GetSubscribeFuncList(m_funcLocation,m_pSubscribeFuncList);
    for (size_t i=0;i<m_pSubscribeFuncList.size();i++)
    {
        const EB_SubscribeFuncInfo & pSubscribeFuncInfo = m_pSubscribeFuncList[i];
        EbAppButtonItem::pointer existAppButtonItem = subscribeButtonItem( pSubscribeFuncInfo.m_nSubscribeId );
        if ( existAppButtonItem.get()!=0 ) {
            if ( QFile::exists(pSubscribeFuncInfo.m_sResFile) ) {
                existAppButtonItem->updateIcon(pSubscribeFuncInfo.m_sResFile);
            }
            continue;
        }

        hasNewAppButtonItem = true;
        const int index = (int)m_lists.size();
        if ( i>=index ) {
            const QRect rect = getButtonIndexRect(index);
            EbAppButtonItem::pointer buttonItem = EbAppButtonItem::create(rect,pSubscribeFuncInfo,this);
            m_lists.push_back(buttonItem);
        }
        else {
            /// 需要排在前面
            const QRect rect = getButtonIndexRect(i);
            EbAppButtonItem::pointer buttonItem = EbAppButtonItem::create(rect,pSubscribeFuncInfo,this);
            insertIndex( i, buttonItem );
            onMove();
        }
    }
    if ( m_lists.empty() || hasNewAppButtonItem ) {
        /// 空或者，有新　APP　，返回　false，继续读取应用数据
        return false;
    }
    static int theRetryCount = 0;
    if ( (theRetryCount++)<10 ) {
        /// 主要用于重试，或者等待 下载图标
        return false;
    }
    return true;
}

QRect EbWidgetAppBar::getButtonIndexRect(int index) const
{
    QRect rect;
    if (m_orientation==Horizontal) {
        rect.setX( 2 + index*(const_app_button_size.width()+const_button_interval) );
        rect.setY( 2 );
        rect.setSize( const_app_button_size );
    }
    else {
        rect.setX( 2 );
        rect.setY( 2 + index*(const_app_button_size.height()+const_button_interval) );
        rect.setSize( const_app_button_size );
    }
    return rect;
}

void EbWidgetAppBar::insertIndex(int index, const EbAppButtonItem::pointer &buttonItem)
{
    int currentIndex = 0;
    std::vector<EbAppButtonItem::pointer>::iterator iter = m_lists.begin();
    for ( ; iter!=m_lists.end(); iter++ ) {
        if ( (currentIndex++)==index) {
            m_lists.insert( iter, buttonItem );
            return;
        }
    }
    m_lists.push_back( buttonItem );
}

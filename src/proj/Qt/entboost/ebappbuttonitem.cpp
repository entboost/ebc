#include "ebappbuttonitem.h"
#include <ebiconhelper.h>

const QSize const_msg_label_size(16,16);

EbAppButtonItem::EbAppButtonItem(const QRect &rect, const EB_SubscribeFuncInfo &subscribeFuncInfo, QWidget * parent)
    : QObject(parent)
    , m_unreadMsgCount(0)
    , m_subscribeFuncInfo(subscribeFuncInfo)
{
    assert (parent!=0 );
    m_buttonApp = new QPushButton(parent);
    this->connect( m_buttonApp,SIGNAL(clicked()),this,(SLOT(onClickedAppItem())) );
    parent->connect( m_buttonApp,SIGNAL(clicked()),parent,(SLOT(onClickedAppItem())) );
    m_buttonApp->setProperty( "sub-id", QVariant(m_subscribeFuncInfo.m_nSubscribeId) );
    m_buttonApp->setObjectName( "SearchBarButton" );
    m_buttonApp->setToolTip( m_subscribeFuncInfo.m_sFunctionName.c_str() );
    m_buttonApp->setVisible( true );
    m_buttonApp->resize( rect.size() );
    if ( m_subscribeFuncInfo.m_sResFile.isEmpty() || !QFile::exists(m_subscribeFuncInfo.m_sResFile) ) {
        /// 11 是默认字体图标大小
        EbIconHelper::Instance()->SetIcon( subscribeFuncInfo.m_nSubscribeId,m_buttonApp,11);
    }
    else {
        updateIcon( m_subscribeFuncInfo.m_sResFile );
    }

    m_labelMsg = new QLabel(parent);
    m_labelMsg->setObjectName("UnreadMsg");
    m_labelMsg->setAlignment(Qt::AlignCenter);
    m_labelMsg->resize(const_msg_label_size);
    m_labelMsg->setVisible(false);

    move( rect.x(), rect.y() );
}

EbAppButtonItem::pointer EbAppButtonItem::create(const QRect &rect, const EB_SubscribeFuncInfo & subscribeFuncInfo, QWidget * parent)
{
    return EbAppButtonItem::pointer(new EbAppButtonItem(rect,subscribeFuncInfo,parent));
}
void EbAppButtonItem::move(int x, int y)
{
    m_buttonApp->move( x, y );
    const QRect & rect = m_buttonApp->geometry();
    m_labelMsg->move( rect.right()-const_msg_label_size.width()+2, rect.top()-2 );
}

void EbAppButtonItem::move(const QPoint &point)
{
    move( point.x(), point.y() );
}

bool EbAppButtonItem::setMsgCount( qint64 subscribeId, int msgCount )
{
    if (m_subscribeFuncInfo.m_nSubscribeId!=subscribeId) {
        return false;
    }
    m_unreadMsgCount = msgCount;
    if (m_unreadMsgCount<=0)
        m_labelMsg->setText( "" );
    else
        m_labelMsg->setText( QString::number(m_unreadMsgCount) );
    m_labelMsg->setVisible( m_unreadMsgCount>0?true:false );
    return true;
}

bool EbAppButtonItem::addMsgCount(qint64 subscribeId)
{
    return setMsgCount( subscribeId, m_unreadMsgCount+1 );
}

void EbAppButtonItem::updateIcon(const QString &fileName)
{
    if ( m_iconFileName==fileName || !QFile::exists( fileName ) ) {
        return;
    }
    m_iconFileName = fileName;
    m_buttonApp->setIcon( QIcon( m_iconFileName ) );
}

bool EbAppButtonItem::isSysSubscribeId(qint64 subscribeId)
{
    switch ( subscribeId ) {
    case 1002300102:    /// 找群找人
    case 1002300103:    /// 我的消息
    case 1002300104: /// 我的邮件
    case 1002300105: /// [个人]云盘
    case 1002300110:    /// 工作台
    case 1002300111: /// 计划 subid_
    case 1002300112: /// 任务
    case 1002300113: /// 日报
    case 1002300114: /// 报告
    case 1002300115: /// 考勤
        return true;
    default:    /// 其他
        break;
    }
    return false;
}

void EbAppButtonItem::onClickedAppItem()
{
    if ( m_labelMsg!=0 && m_labelMsg->isVisible() ) {
        m_labelMsg->hide();
    }
}

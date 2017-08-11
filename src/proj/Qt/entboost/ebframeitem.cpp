#include "ebframeitem.h"
#include "ebdialogchatbase.h"
#include "ebiconhelper.h"
#include "ebdialogworkframe.h"

const QSize const_image_label_size(32,32);
const QSize const_close_button_size(12,12);
const int const_left_button_height = 36;
const int const_left_button_left = 0;    /// **左边间隔
const int const_left_button_top = 8;    /// **第一个按钮上边间隔
const int const_left_button_right = 2;    /// **右边间隔
#define DEFAULT_MSG_WIDTH1 16
#define DEFAULT_MSG_WIDTH2 28

EbFrameItem::EbFrameItem(const EbDialogChatBase::pointer& dialogChatBase)
    : m_itemType(EbFrameItem::FRAME_ITEM_CALL_DIALOG)
    , m_dialogChatBase(dialogChatBase)
    , m_dialogWorkFrame(0)
    , m_pushButtonLeft(0)
    , m_labelUnreadMsg(0)
    , m_labelImage(0)
    , m_pushButtonClose(0)
    , m_closeTime(0)
    , m_nUnreadMsgCount(0)
{

}

EbFrameItem::EbFrameItem(EbFrameItem::FRAME_ITEM_TYPE itemType)
    : m_itemType(itemType)
    , m_dialogWorkFrame(0)
    , m_pushButtonLeft(0)
    , m_labelUnreadMsg(0)
    , m_labelImage(0)
    , m_pushButtonClose(0)
    , m_closeTime(0)
    , m_nUnreadMsgCount(0)
{

}

EbFrameItem::~EbFrameItem()
{
    /// 可以 delete ，删除会通知 parent
    if (m_pushButtonLeft!=0) {
        delete m_pushButtonLeft;
        m_pushButtonLeft = 0;
    }
    m_dialogChatBase.reset();
}

EbFrameItem::pointer EbFrameItem::create(FRAME_ITEM_TYPE itemType)
{
    return EbFrameItem::pointer( new EbFrameItem(itemType) );
}

EbFrameItem::pointer EbFrameItem::create(const EbDialogChatBase::pointer& dialogChatBase)
{
    return EbFrameItem::pointer( new EbFrameItem(dialogChatBase) );
}

QWidget *EbFrameItem::parent() const
{
    if (m_pushButtonLeft!=0) {
        return m_pushButtonLeft->parentWidget();
    }
    if (m_itemType==EbFrameItem::FRAME_ITEM_CALL_DIALOG && m_dialogChatBase.get()!=0) {
        return m_dialogChatBase->parentWidget();
    }
    return 0;
}

void EbFrameItem::buildButton(int leftWidth, QWidget *parent)
{
    if (m_itemType==EbFrameItem::FRAME_ITEM_WORK_FRAME) {
        /// 工作台
        if (m_dialogWorkFrame==0) {
            m_dialogWorkFrame = new EbDialogWorkFrame(true,true,parent);
            parent->connect( m_dialogWorkFrame,SIGNAL(itemSizeChange(bool,int)),parent,SLOT(onWorkItemSizeChange(bool,int)) );
            this->connect( m_dialogWorkFrame,SIGNAL(itemSizeChange(bool,int)),this,SLOT(onWorkItemSizeChange(bool,int)) );
            m_dialogWorkFrame->setModal(false);
            m_itemText = theLocales.getLocalText("frame-list.work-frame.text","Work Frame");
        }
    }
    else if (m_itemType==EbFrameItem::FRAME_ITEM_CALL_DIALOG && m_dialogChatBase.get()!=0) {
        m_itemText = m_dialogChatBase->fromName();
    }

    /// *左边切换按钮
    const bool showedLeft = leftWidth>42;   /// ?
    if (m_pushButtonLeft==0) {
        m_pushButtonLeft = new QPushButton(parent);
        parent->connect( m_pushButtonLeft,SIGNAL(clicked(bool)),parent,SLOT(onClickedButtonLeft(bool)) );
        parent->connect( this,SIGNAL(checkedFrameitem(const EbFrameItem*)),parent,SLOT(onCheckedFrameitem(const EbFrameItem*)) );

        if (m_itemType==EbFrameItem::FRAME_ITEM_WORK_FRAME) {
            m_pushButtonLeft->setToolTip( theLocales.getLocalText("frame-list.work-frame.tooltip","") );
        }

        m_pushButtonLeft->setObjectName("LeftSwitchButton");
        m_pushButtonLeft->setVisible(true);
        m_pushButtonLeft->setCheckable(true);
        m_pushButtonLeft->raise();
        if (showedLeft) {
//            const int width = m_pushButtonLeft->fontMetrics().width(m_itemText);
//            if (m_pushButtonLeft->geometry().width()-32>width)
//                m_pushButtonLeft->setStyleSheet("text-align : center;");
//            else
//                m_pushButtonLeft->setStyleSheet("text-align : left;");
            m_pushButtonLeft->setText(m_itemText);
        }
    }
    if (m_itemType==EbFrameItem::FRAME_ITEM_CALL_DIALOG && m_dialogChatBase.get()!=0) {
        m_pushButtonLeft->setToolTip( m_dialogChatBase->fullName() );
    }
    if (m_labelUnreadMsg==0) {
        m_labelUnreadMsg = new QLabel(m_pushButtonLeft);
        m_labelUnreadMsg->setVisible(false);
        m_labelUnreadMsg->setAlignment(Qt::AlignCenter);
        m_labelUnreadMsg->setObjectName("UnreadMsg");
        m_labelUnreadMsg->resize(16,16);    /// 显示 99 没有问题
        if (showedLeft)
            m_labelUnreadMsg->move( 36,2 );
        else
            m_labelUnreadMsg->move( 0,0 );
    }
    const int nUnreadMsgCount = m_dialogChatBase.get()==NULL?0:m_dialogChatBase->getUnreadMsgCount();
    setUnreadMsg(nUnreadMsgCount);
    /// * 显示头像
    if (m_labelImage==0) {
//        QPushButton * widgetImage = new QPushButton(m_pushButtonLeft);
//        widgetImage->setStyleSheet("QPushButton{border:2px groove gray;border-radius:10px;padding:2px 4px;}");
//        widgetImage->setVisible(true);
//        widgetImage->resize(const_image_label_size);
//        widgetImage->move( 2,2 );
//////        widgetImage->setIcon( QIcon(QPixmap::fromImage(m_dialogChatBase->fromImage()).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation)) );
//////        widgetImage->setIcon( QIcon(QPixmap::fromImage(QImage(":/"))) );
//        m_labelImage = new QLabel(widgetImage);

        m_labelImage = new QLabel(m_pushButtonLeft);
//        m_labelImage->setObjectName("ChatImage");
        m_labelImage->setStyleSheet("QLabel{background-color:rgb(0,0,0,0);}");
        m_labelImage->setVisible(true);
        m_labelImage->setGeometry( 2,2,const_image_label_size.width(),const_image_label_size.height() );
        if (m_itemType==EbFrameItem::FRAME_ITEM_WORK_FRAME) {
            m_labelImage->setPixmap( QPixmap(":/img/workframe.png").scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
        }
    }
    if (m_itemType==EbFrameItem::FRAME_ITEM_CALL_DIALOG && m_dialogChatBase.get()!=0) {
        if (m_dialogChatBase->isGroupChat() || m_dialogChatBase->isOnlineState()) {
            /// 显示群组/部门默认图片，或显示在线用户头像
            m_labelImage->setPixmap( QPixmap::fromImage(m_dialogChatBase->fromImage()).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
        }
        else {
            /// 显示用户离线头像
            m_labelImage->setPixmap( QPixmap::fromImage(libEbc::imageToGray(m_dialogChatBase->fromImage())).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
        }
    }

    if (m_itemType==EbFrameItem::FRAME_ITEM_CALL_DIALOG) {
        /// *关闭按钮
        if (m_pushButtonClose==0) {
            m_pushButtonClose = new QPushButton(m_pushButtonLeft);
            parent->connect( m_pushButtonClose,SIGNAL(clicked(bool)),parent,SLOT(onClickedButtonLeft(bool)) );

            m_pushButtonClose->setVisible(false);
            m_pushButtonClose->resize( const_close_button_size );
            EbIconHelper::Instance()->SetIcon( m_pushButtonClose,QChar(0xf00d),8 );
            m_pushButtonClose->setObjectName("SysCloseButton");
        }
    }

    /// *返回切换按钮，方便外面 connect();
//    return m_pushButtonLeft;
}

//void EbFrameItem::setItemText(const QString &text)
//{
//    m_itemText = text;
//    if (m_pushButtonLeft!=0) {
//        m_pushButtonLeft->setText(m_itemText);
//    }
//}

//void EbFrameItem::updateWorkFrameSize(bool showLeft,int size)
//{
//    if (m_pushButtonLeft==0 || m_itemType!=EbFrameItem::FRAME_ITEM_WORK_FRAME) {
//        return;
//    }

//    const QString itemText = theLocales.getLocalText("frame-list.work-frame.text","Work Frame");
//    m_pushButtonLeft->setToolTip( theLocales.getLocalText("frame-list.work-frame.tooltip","") );
//    if (size<=0) {
//        m_itemText = itemText;
//    }
//    else {
//        m_itemText = QString("%1 %2").arg(itemText).arg(size);
//    }
//    if (showedLeft) {
//        m_pushButtonLeft->setText(m_itemText);
//    }
////    else {
////        m_pushButtonLeft->setText("");
////    }
//}

int EbFrameItem::checkLeftButtonClickState(const QPushButton *button, const QPoint &pt) const
{
    /// * 检查按钮点击状态：1=点击关闭，2=左边点击，0=没有点击
    if ( m_pushButtonClose!=0 && (button==m_pushButtonClose || m_pushButtonClose->geometry().contains(pt)) ) {
        return 1;
    }
    else if ( m_pushButtonLeft!=0 && (button==m_pushButtonLeft || m_pushButtonLeft->geometry().contains(pt)) ) {
        return 2;
    }
    return 0;
}

bool EbFrameItem::requestClose()
{
    /// 请求退出窗口
    if (m_dialogChatBase.get()!=0) {
        return m_dialogChatBase->onClickedInputClose();
    }
    return true;
}

void EbFrameItem::onResize(int index, const QRect &rect, int leftWidth)
{
    if (m_dialogWorkFrame!=0) {
        m_dialogWorkFrame->setGeometry( leftWidth,0,rect.width()-leftWidth-1,rect.height()-1);
    }
    else if (m_itemType==EbFrameItem::FRAME_ITEM_CALL_DIALOG && m_dialogChatBase.get()!=0) {
        m_dialogChatBase->setGeometry( leftWidth,0,rect.width()-leftWidth,rect.height());
    }
    if (m_pushButtonLeft==0) {
        return;
    }

    const int x = const_left_button_left;
    const int y = const_left_button_top+index*const_left_button_height;
    m_pushButtonLeft->setGeometry( x,y,leftWidth-x-const_left_button_right, const_left_button_height);
    const QRect& rectLeftButton = m_pushButtonLeft->geometry();
    // leftWidth
    if (leftWidth>42) {
        m_pushButtonLeft->setText(m_itemText);
    }
    else {
        m_pushButtonLeft->setText("");
    }
    if (leftWidth>42) {
        /// 显示在图像右边
        m_labelUnreadMsg->move(36,2);
    }
    else {
        /// 显示在左上角
        m_labelUnreadMsg->move(0,0);
    }
    if (m_pushButtonClose!=0) {
        if (leftWidth>42) {
            /// 显示在右边垂直中间位置
            m_pushButtonClose->move( rectLeftButton.width()-const_close_button_size.width()-2,(rectLeftButton.height()-const_close_button_size.height())/2 );
        }
        else {
            /// 显示在右上角
            m_pushButtonClose->move( rectLeftButton.width()-const_close_button_size.width(),0 );
        }
    }

}
void EbFrameItem::onMove(int index)
{
    if (m_pushButtonLeft==0) {
        return;
    }
    const int x = const_left_button_left;
    const int y = const_left_button_top+index*const_left_button_height;
    m_pushButtonLeft->move( x,y );

}

void EbFrameItem::onMoveEvent()
{
    if (m_dialogWorkFrame!=0) {
        m_dialogWorkFrame->onMoveEvent();
    }
}

void EbFrameItem::setChecked(bool checked, bool hideButton)
{
    if (m_pushButtonLeft!=0) {
        m_pushButtonLeft->setChecked(checked);
        if (hideButton) {
            m_pushButtonLeft->setVisible(false);
        }
        else if ( checked && !m_pushButtonLeft->isVisible() ) {
            m_pushButtonLeft->setVisible(true);
        }
    }
    const bool visible = checked;
    if (m_pushButtonClose!=0) {
        m_pushButtonClose->setVisible(visible);
    }
    if (m_dialogWorkFrame!=0) {
        m_dialogWorkFrame->setVisible(visible);
        if (checked) {
            m_dialogWorkFrame->setCheckedFocus();
//            m_dialogWorkFrame->setFocusInput();
        }
    }
    else if (m_itemType==EbFrameItem::FRAME_ITEM_CALL_DIALOG && m_dialogChatBase.get()!=0) {
        m_dialogChatBase->setVisible(visible);
        if (checked) {
            m_dialogChatBase->setFocusInput();
        }
    }

    if ( checked ) {
        emit checkedFrameitem(this);
    }
}

bool EbFrameItem::isChecked() const
{
    if (m_pushButtonLeft!=0) {
        const bool result =  m_pushButtonLeft->isChecked();
        if ( result && !m_pushButtonLeft->isVisible() ) {
            m_pushButtonLeft->setVisible( true );
        }
        return result;
    }
    return false;
}

bool EbFrameItem::checkShowHideCloseButton(const QPoint& pt)
{
    if (m_pushButtonLeft==0 || m_pushButtonClose==0) {
        return false;
    }
    const bool visible = m_pushButtonLeft->geometry().contains(pt);
    m_pushButtonClose->setVisible( visible );
    return visible;
}

void EbFrameItem::setCloseButtonVisible(bool visible)
{
    if (m_pushButtonClose!=0) {
        m_pushButtonClose->setVisible(visible);
    }
}

void EbFrameItem::clearUnreadMsg(bool fromUserClick)
{
    if (m_nUnreadMsgCount>0) {
        if (m_dialogChatBase.get()!=0) {
            if (!m_dialogChatBase->clearUnreadMsg(fromUserClick))
                return;
        }
        m_nUnreadMsgCount = 0;
        if (m_labelUnreadMsg!=0) {
            m_labelUnreadMsg->setText("");
            m_labelUnreadMsg->setToolTip("");
            m_labelUnreadMsg->setVisible(false);
        }
    }
}

void EbFrameItem::addUnreadMsg()
{
    setUnreadMsg(m_nUnreadMsgCount+1);
}

void EbFrameItem::setUnreadMsg(size_t nUnreadMsgCount)
{
    if (m_nUnreadMsgCount==nUnreadMsgCount) {
        return;
    }
    if (nUnreadMsgCount<=0) {
        clearUnreadMsg();
    }
    else {
        m_nUnreadMsgCount = nUnreadMsgCount;
        if (m_labelUnreadMsg!=0) {
            const QString sUnreadMsg = QString::number(m_nUnreadMsgCount);
            m_labelUnreadMsg->setText(sUnreadMsg);
            m_labelUnreadMsg->setToolTip(sUnreadMsg);
            m_labelUnreadMsg->setVisible(true);
        }
    }
}

void EbFrameItem::onUserLineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE bLineState)
{
    if (m_itemType==EbFrameItem::FRAME_ITEM_CALL_DIALOG && m_dialogChatBase.get()!=0) {
        /// 更新会话用户在线状态，或群组成员在线状态
        m_dialogChatBase->onUserLineStateChange(nGroupCode,nUserId,bLineState);
        if (m_dialogChatBase->groupId()==0 && m_dialogChatBase->fromUserId()==nUserId && m_labelImage!=0) {
            /// 更新左边标签
            if (m_dialogChatBase->isOnlineState()) {
                /// 显示群组/部门默认图片，或显示在线用户头像
                m_labelImage->setPixmap( QPixmap::fromImage(m_dialogChatBase->fromImage()).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            }
            else {
                /// 显示用户离线头像
                m_labelImage->setPixmap( QPixmap::fromImage(libEbc::imageToGray(m_dialogChatBase->fromImage())).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            }
        }
    }

}

void EbFrameItem::onMemberHeadChange(const EB_MemberInfo *pMemberInfo)
{
    if (m_labelImage==0 || m_dialogChatBase.get()==0 || m_dialogChatBase->isGroupChat()) {
        return;
    }
    if (fromUserId()==pMemberInfo->m_nMemberUserId) {
        if (m_dialogChatBase->onMemberHeadChange(pMemberInfo)) {
            /// 更新左边标签
            if (m_dialogChatBase->isOnlineState()) {
                /// 显示群组/部门默认图片，或显示在线用户头像
                m_labelImage->setPixmap( QPixmap::fromImage(m_dialogChatBase->fromImage()).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            }
            else {
                /// 显示用户离线头像
                m_labelImage->setPixmap( QPixmap::fromImage(libEbc::imageToGray(m_dialogChatBase->fromImage())).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            }
        }
    }
}

void EbFrameItem::onContactHeadChange(const EB_ContactInfo *pContactInfo)
{
    if (m_labelImage==0 || m_dialogChatBase.get()==0 || m_dialogChatBase->isGroupChat()) {
        return;
    }

    if (fromUserId()==pContactInfo->m_nContactUserId) {
        if (m_dialogChatBase->onContactHeadChange(pContactInfo)) {
            /// 更新左边标签
            if (m_dialogChatBase->isOnlineState()) {
                /// 显示群组/部门默认图片，或显示在线用户头像
                m_labelImage->setPixmap( QPixmap::fromImage(m_dialogChatBase->fromImage()).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            }
            else {
                /// 显示用户离线头像
                m_labelImage->setPixmap( QPixmap::fromImage(libEbc::imageToGray(m_dialogChatBase->fromImage())).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            }
        }
    }
}

void EbFrameItem::onGroupInfo(const EB_GroupInfo *pGroupInfo)
{
//    if (groupId()!=pGroupInfo->m_sGroupCode) {
//        return false;
//    }

    if (m_dialogChatBase.get()!=0) {
        /// Select User 界面需要
        m_dialogChatBase->onGroupInfo(pGroupInfo);

        if (groupId()==pGroupInfo->m_sGroupCode) {
            m_itemText = m_dialogChatBase->fromName();
            if ( m_pushButtonLeft!=0 ) {
                m_pushButtonLeft->setToolTip( m_dialogChatBase->fullName() );
                const bool showedLeft = m_pushButtonLeft->geometry().width()>42;
                if (showedLeft) {
                    m_pushButtonLeft->setText(m_itemText);
                }
            }
            if (m_labelImage!=0) {
                m_labelImage->setPixmap( QPixmap::fromImage(m_dialogChatBase->fromImage()).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            }
        }
    }
//    CheckGroupForbidSpeech();
//    return true;
}

bool EbFrameItem::timerCheckState()
{
    if (m_pushButtonLeft==0 || !m_pushButtonLeft->isChecked()) {
        return false;
    }
    if (m_dialogChatBase.get()!=0) {
        m_dialogChatBase->timerCheckState();
    }

    return true;
}

void EbFrameItem::onWorkItemSizeChange(bool,int size)
{
    if (m_pushButtonLeft==0 || m_itemType!=EbFrameItem::FRAME_ITEM_WORK_FRAME) {
        return;
    }
    const QString itemText = theLocales.getLocalText("frame-list.work-frame.text","Work Frame");
    const QString itemTooltip = theLocales.getLocalText("frame-list.work-frame.tooltip","");
    //    m_pushButtonLeft->setToolTip( theLocales.getLocalText("frame-list.work-frame.tooltip","") );
    if (size<=0) {
        m_itemText = itemText;
        m_pushButtonLeft->setToolTip(itemTooltip);
    }
    else {
        m_itemText = QString("%1 %2").arg(itemText).arg(size);
        m_pushButtonLeft->setToolTip( QString("%1 %2").arg(itemTooltip).arg(size) );
    }
    const bool showedLeft = m_pushButtonLeft->geometry().width()>42;
    if (showedLeft) {
        m_pushButtonLeft->setText(m_itemText);
    }
}


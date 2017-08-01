#include "ebworkitem.h"
#include "ebiconhelper.h"
#include "ebclientapp.h"

const QSize const_image_label_size(16,16);
const QSize const_close_button_size(12,12);
const int const_top_button_left = 0;    /// **左边间隔

//EbWorkItem::EbWorkItem(QObject *parent)
//{

//}

EbWorkItem::EbWorkItem(WORK_ITEM_TYPE itemType)
    : m_itemType(itemType)
    , m_widgetUserInfo(0)
    , m_widgetUserList(0)
    , m_widgetChatRecord(0)
    , m_widgetTranFile(0)
    , m_topButtonWidth(140)
    , m_pushButtonTop(0)
    , m_labelImage(0)
    , m_pushButtonClose(0)
{

}

EbWorkItem::~EbWorkItem()
{
    m_widgetWorkView.reset();
}
EbWorkItem::pointer EbWorkItem::create(WORK_ITEM_TYPE itemType)
{
    return EbWorkItem::pointer( new EbWorkItem(itemType) );
}
void EbWorkItem::setAppUrl(bool saveUrl,const QString &sUrl, const QString &sPostData)
{
    if (sUrl!="about:blank" && sUrl.indexOf("http",Qt::CaseInsensitive)<0) {
        m_appUrl = "http://"+sUrl;
    }
    else {
        m_appUrl = sUrl;
    }
    m_postData = sPostData;

    if (m_widgetWorkView.get()!=0) {
        m_widgetWorkView->setSaveUrl(saveUrl);
        m_widgetWorkView->load(m_appUrl, m_postData);
    }
}

void EbWorkItem::setFunInfo(const EB_SubscribeFuncInfo &subFuncInfo, bool bOpenNewClose)
{
    if (m_pushButtonTop!=0) {
        m_pushButtonTop->setText( subFuncInfo.m_sFunctionName.c_str() );
    }
    m_subFuncInfo = subFuncInfo;
    EbIconHelper::Instance()->SetIcon( m_subFuncInfo.m_nSubscribeId, m_labelImage, 10 );

//    if (m_widgetWorkView.get()!=0) {
//        m_widgetWorkView->m_funcInfo = pFuncInfo;
////        m_pAppWindow->m_bOpenNewClose = bOpenNewClose;
//    }
}

QWidget *EbWorkItem::parent() const
{
    if (m_pushButtonTop!=0) {
        return m_pushButtonTop->parentWidget();
    }
    if (m_widgetWorkView.get()!=0) {
        return m_widgetWorkView->parentWidget();
    }
    return 0;
}

void EbWorkItem::buildButton(bool saveUrl,int topHeight, QWidget *parent)
{
    if ( isItemType(WORK_ITEM_USER_INFO) && m_widgetUserInfo==0 ) {
        m_widgetUserInfo = new EbWidgetUserInfo(m_callInfo,parent);
        m_itemText = theLocales.getLocalText("user-info.title","User Info");
    }
    else if ( isItemType(WORK_ITEM_USER_LIST) && m_widgetUserInfo==0 ) {
        m_widgetUserList = new EbWidgetUserList(m_callInfo,parent);
        m_itemText = theLocales.getLocalText("user-list.title","User List");
    }
    else if ( isItemType(WORK_ITEM_CHAT_RECORD) && m_widgetUserInfo==0 ) {
        m_widgetChatRecord = new EbWidgetChatRecord(m_callInfo,parent);
        parent->connect(m_widgetChatRecord->textBrowser(), SIGNAL(openSubId(eb::bigint)), parent, SLOT(onOpenSubId(eb::bigint)));
        m_itemText = theLocales.getLocalText("chat-record.title","Chat Record");
    }
    else if ( isItemType(WORK_ITEM_TRAN_FILE) && m_widgetTranFile==0 ) {
        m_widgetTranFile = new EbWidgetFileTranList(parent);
        m_itemText = theLocales.getLocalText("tran-file.title","Tran File");
    }
    else if ( isItemType(WORK_ITEM_WEB_BROWSER) && m_widgetWorkView.get()==0 ) {
        m_widgetWorkView = EbWidgetWorkView::create( saveUrl,m_appUrl,m_postData,parent );
        this->connect( m_widgetWorkView.get(),SIGNAL(titleChanged(const EbWidgetWorkView*,QString)),this,SLOT(onTitleChanged(const EbWidgetWorkView*,QString)) );
        this->connect( m_widgetWorkView.get(),SIGNAL(iconChanged(const EbWidgetWorkView*,QIcon)),this,SLOT(onIconChanged(const EbWidgetWorkView*,QIcon)) );
        parent->connect( m_widgetWorkView.get(),SIGNAL(urlChanged(const EbWidgetWorkView*,QUrl)),parent,SLOT(onUrlChanged(const EbWidgetWorkView*,QUrl)) );
        parent->connect( m_widgetWorkView.get(),SIGNAL(loadStateChange(const EbWidgetWorkView*,bool,bool,bool)),parent,SLOT(onLoadStateChange(const EbWidgetWorkView*,bool,bool,bool)) );
        parent->connect( m_widgetWorkView.get(),SIGNAL(openLinkInNewTab(const EbWidgetWorkView*,QUrl)),parent,SLOT(onOpenLinkInNewTab(const EbWidgetWorkView*,QUrl)) );
        parent->connect( m_widgetWorkView.get(),SIGNAL(windowCloseRequested(const EbWidgetWorkView*)),parent,SLOT(onWindowCloseRequested(const EbWidgetWorkView*)) );
//        m_widgetWorkView->SetSaveBrowseTitle(bSaveBrowseTitle);
    }

    if ( m_itemText.isEmpty() || m_itemText=="about:blank" ) {
        m_itemText = theLocales.getLocalText("work-frame.about-blank.text","about:blank");
    }

    /// *上边切换按钮
    if ( m_pushButtonTop==0 ) {
        m_pushButtonTop = new QPushButton(parent);
        parent->connect( m_pushButtonTop,SIGNAL(clicked()),parent,SLOT(onClickedButtonTop()) );
        if ( isItemType(WORK_ITEM_USER_LIST) ) {
            this->connect( m_widgetUserList,SIGNAL(memberOnlineSizeChange(int,int)),this,SLOT(onMemberOnlineSizeChange(int,int)) );
        }
        if ( m_itemType>=WORK_ITEM_WEB_BROWSER )
            m_pushButtonTop->setObjectName("TopSwitchButton");
        else
            m_pushButtonTop->setObjectName("TopSwitchButton2");
        m_pushButtonTop->setVisible(true);
        m_pushButtonTop->setCheckable(true);
//        m_pushButtonTop->raise();
        m_pushButtonTop->setText(m_itemText);
        if ( isItemType(WORK_ITEM_USER_INFO) ) {
            m_pushButtonTop->setToolTip( theLocales.getLocalText("user-info.title","") );
        }
        else if ( isItemType(WORK_ITEM_USER_LIST) ) {
            m_pushButtonTop->setToolTip( theLocales.getLocalText("user-list.title","") );
        }
        else if ( isItemType(WORK_ITEM_CHAT_RECORD) ) {
            m_pushButtonTop->setToolTip( theLocales.getLocalText("chat-record.title","") );
        }
        else if ( isItemType(WORK_ITEM_TRAN_FILE) ) {
            m_pushButtonTop->setToolTip( theLocales.getLocalText("tran-file.title","") );
        }
    }
    /// * 显示头像
    if ( m_labelImage==0 ) {
        m_labelImage = new QLabel(m_pushButtonTop);
        m_labelImage->setStyleSheet("QLabel{background-color:rgb(0,0,0,0);}");
        m_labelImage->setVisible(true);
        m_labelImage->setGeometry( 3,7,const_image_label_size.width(),const_image_label_size.height() );
        if ( m_subFuncInfo.m_nSubscribeId>0 ) {
            EbIconHelper::Instance()->SetIcon( m_subFuncInfo.m_nSubscribeId, m_labelImage, 10 );
        }
    }
    if (m_widgetWorkView.get()!=0) {
//        if (m_dialogChatBase->isGroupChat() || m_dialogChatBase->isOnlineState()) {
//            /// 显示群组/部门默认图片，或显示在线用户头像
//            m_labelImage->setPixmap( QPixmap::fromImage(m_dialogChatBase->formImage()).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
//        }
//        else {
//            /// 显示用户离线头像
//            m_labelImage->setPixmap( QPixmap::fromImage(libEbc::imageToGray(m_dialogChatBase->formImage())).scaled(const_image_label_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
//        }
    }

    /// *关闭按钮
    if (m_itemType>WORK_ITEM_USER_LIST && m_pushButtonClose==0) {
        m_pushButtonClose = new QPushButton(m_pushButtonTop);
        parent->connect( m_pushButtonClose,SIGNAL(clicked()),parent,SLOT(onClickedButtonTop()) );
        m_pushButtonClose->setVisible(false);
        m_pushButtonClose->resize( const_close_button_size );
        EbIconHelper::Instance()->SetIcon( m_pushButtonClose,QChar(0xf00d),8 );
        m_pushButtonClose->setObjectName("SysClose2Button");
    }
}

QRect EbWorkItem::rectButton() const
{
    QRect result;
    if (m_pushButtonTop!=0) {
        result = m_pushButtonTop->geometry();
    }
    return result;
}

int EbWorkItem::checkTopButtonClickState(const QPushButton *button, const QPoint &pt) const
{
    /// * 检查按钮点击状态：1=点击关闭，2=上边点击，0=没有点击
    if (m_pushButtonTop==0) {
        return 0;
    }
    if (m_pushButtonClose!=0 && (button==m_pushButtonClose || m_pushButtonClose->geometry().contains(pt)) ) {
        return 1;
    }
    else if ( button==m_pushButtonTop || m_pushButtonTop->geometry().contains(pt) ) {
        return 2;
    }
    return 0;
}

int EbWorkItem::onResize(int x, const QRect &rect, int topHeight, int leftOffset)
{
    if (m_pushButtonTop==0) {
        return 0;
    }
//    const int x = leftOffset+const_top_button_left+index*m_topButtonWidth;
    int y = topHeight;
    m_pushButtonTop->setGeometry( x,y,m_topButtonWidth,const_top_button_height );
    const QRect& rectLeftButton = m_pushButtonTop->geometry();
    /// 显示在图像右边
//    m_labelUnreadMsg->move(36,2);
    /// 显示在右边垂直中间位置
    if (m_pushButtonClose!=0) {
        m_pushButtonClose->move( rectLeftButton.width()-const_close_button_size.width()-1,(rectLeftButton.height()-const_close_button_size.height())/2 );
    }
    y += const_top_button_height;
    if (m_widgetUserInfo!=0) {
        m_widgetUserInfo->setGeometry( leftOffset,y,rect.width()-leftOffset,rect.height()-y );
    }
    else if (m_widgetUserList!=0) {
        m_widgetUserList->setGeometry( leftOffset,y,rect.width()-leftOffset,rect.height()-y );
    }
    else if (m_widgetChatRecord!=0) {
        m_widgetChatRecord->setGeometry( leftOffset,y,rect.width()-leftOffset,rect.height()-y );
    }
    else if (m_widgetTranFile!=0) {
        m_widgetTranFile->setGeometry( leftOffset,y,rect.width()-leftOffset,rect.height()-y );
    }
    else if (m_widgetWorkView.get()!=0) {
        /// -1 避免看不到右边和下边边框
        m_widgetWorkView->setGeometry( leftOffset,y,rect.width()-leftOffset-1,rect.height()-y-1 );
    }
    return rectLeftButton.right();
}

QRect EbWorkItem::topGeometry() const
{
    if (m_pushButtonTop==0) {
        return QRect();
    }
    return m_pushButtonTop->geometry();
}

int EbWorkItem::onMove(int x)
{
    if (m_pushButtonTop==0) {
        return 0;
    }
//    const int x = leftOffset+const_top_button_left+index*m_topButtonWidth;
    const int y = m_pushButtonTop->geometry().top();
    m_pushButtonTop->move( x,y );
    return m_pushButtonTop->geometry().right();
}

void EbWorkItem::setChecked(bool checked, bool hideButton, bool /*bSearchFocus*/)
{
    if (m_pushButtonTop!=0) {
        m_pushButtonTop->setChecked(checked);
        if (hideButton) {
            m_pushButtonTop->setVisible(false);
        }
    }
    const bool visible = checked;
    if (m_pushButtonClose!=0) {
        m_pushButtonClose->setVisible(visible);
    }

    if (m_widgetUserInfo!=0) {
        m_widgetUserInfo->setVisible(visible);
        if (checked)
            m_widgetUserInfo->setFocus();
    }
    else if (m_widgetUserList!=0) {
        m_widgetUserList->setVisible(visible);
        if (checked)
            m_widgetUserList->setFocus();
    }
    else if (m_widgetChatRecord!=0) {
        m_widgetChatRecord->setVisible(visible);
        if (checked)
            m_widgetChatRecord->setFocus();
    }
    else if (m_widgetTranFile!=0) {
        m_widgetTranFile->setVisible(visible);
        if (checked)
            m_widgetTranFile->setFocus();
    }
    else if (m_widgetWorkView.get()!=0) {
        m_widgetWorkView->setVisible(visible);
        if (checked)
            m_widgetWorkView->setFocusInput();
    }

}

bool EbWorkItem::isChecked() const
{
    if (m_pushButtonTop!=0) {
        return m_pushButtonTop->isChecked();
    }
    return false;
}

bool EbWorkItem::setCheckedFocus()
{
    if ( isChecked() ) {
        if (m_widgetUserInfo!=0) {
            m_widgetUserInfo->setFocus();
        }
        else if (m_widgetUserList!=0) {
            m_widgetUserList->setFocus();
        }
        else if (m_widgetChatRecord!=0) {
            m_widgetChatRecord->setFocus();
        }
        else if (m_widgetTranFile!=0) {
            m_widgetTranFile->setFocus();
        }
        else if (m_widgetWorkView!=0) {
            m_widgetWorkView->setFocusInput();
        }
        return true;
    }
    return false;
}

bool EbWorkItem::checkShowHideCloseButton(const QPoint &pt)
{
    if (m_pushButtonTop==0 || m_pushButtonClose==0) {
        return false;
    }
    const bool visible = m_pushButtonTop->geometry().contains(pt);
    m_pushButtonClose->setVisible( visible );
    return visible;
}

void EbWorkItem::setCloseButtonVisible(bool visible)
{
    if (m_pushButtonClose!=0) {
        m_pushButtonClose->setVisible(visible);
    }
}

void EbWorkItem::sendClose()
{
    if (m_widgetUserInfo!=0) {
        m_widgetUserInfo->close();
    }
    else if (m_widgetUserList!=0) {
        m_widgetUserList->close();
    }
    else if (m_widgetChatRecord!=0) {
        m_widgetChatRecord->close();
    }
    else if (m_widgetTranFile!=0) {
        m_widgetTranFile->close();
    }
    else if (m_widgetWorkView!=0) {
        m_widgetWorkView->close();
    }
}

bool EbWorkItem::onTitleChanged(const EbWidgetWorkView *view, const QString &title)
{
    if (m_pushButtonTop==0) {
        return false;
    }
    if (m_widgetWorkView.get()==view) {
        QString itemText(title);
        if (itemText.isEmpty() || itemText=="about:blank") {
            itemText = theLocales.getLocalText("work-frame.about-blank.text","about:blank");
        }
        m_pushButtonTop->setText(itemText);
        m_pushButtonTop->setToolTip(itemText);
        return true;
    }
    return false;
}

bool EbWorkItem::onIconChanged(const EbWidgetWorkView *view, const QIcon & icon)
{
    if (m_labelImage==0) {
        return false;
    }
    if (m_widgetWorkView.get()==view) {
        m_labelImage->setPixmap( icon.pixmap(const_image_label_size) );
        return true;
    }
    return false;
}

bool EbWorkItem::back()
{
    if (m_widgetWorkView.get()==0 || m_pushButtonTop==0 || !m_pushButtonTop->isChecked()) {
        return false;
    }
    m_widgetWorkView->back();
    return true;
}

bool EbWorkItem::forward()
{
    if (m_widgetWorkView.get()==0 || m_pushButtonTop==0 || !m_pushButtonTop->isChecked()) {
        return false;
    }
    m_widgetWorkView->forward();
    return true;
}

bool EbWorkItem::canGoBack(bool &pOutCanGoBack) const
{
    if (m_widgetWorkView.get()==0 || m_pushButtonTop==0 || !m_pushButtonTop->isChecked()) {
        return false;
    }
    pOutCanGoBack = m_widgetWorkView->canGoBack();
    return true;
}

bool EbWorkItem::canGoForward(bool &pOutCanGoForward) const
{
    if (m_widgetWorkView.get()==0 || m_pushButtonTop==0 || !m_pushButtonTop->isChecked()) {
        return false;
    }
    pOutCanGoForward = m_widgetWorkView->canGoForward();
    return true;
}

bool EbWorkItem::refreshOrStop()
{
    if (m_widgetWorkView.get()==0 || m_pushButtonTop==0 || !m_pushButtonTop->isChecked()) {
        return false;
    }
    m_widgetWorkView->refreshOrStop();
    return true;
}

bool EbWorkItem::saveUrlHistory()
{
    if (m_widgetWorkView.get()==0 || m_pushButtonTop==0 || !m_pushButtonTop->isChecked()) {
        return false;
    }
    m_widgetWorkView->saveUrlHistory();
    return true;
}

void EbWorkItem::onMemberOnlineSizeChange(int memberSize, int onlineSize)
{
    if ( m_pushButtonTop==0 || !isItemType(WORK_ITEM_USER_LIST) ) {
        return;
    }
    /// 群成员(%d/%d)
    /// 群成员(%d)
    QString text;
    if (onlineSize>=0)
        text = QString("%1(%2/%3)").arg(theLocales.getLocalText("user-list.title","User List")).arg(onlineSize).arg(memberSize);
    else
        text = QString("%1(%2)").arg(theLocales.getLocalText("user-list.title","User List")).arg(memberSize);
    m_pushButtonTop->setText(text);
    m_pushButtonTop->setToolTip(text);
}

bool EbWorkItem::onTitleChanged(EbWorkItem::WORK_ITEM_TYPE itemType, const QString &title)
{
    if (m_pushButtonTop==0) {
        return false;
    }
    if (m_itemType==itemType) {
        QString itemText(title);
//        if (itemText.isEmpty() || itemText=="about:blank") {
//            itemText = theLocales.getLocalText("work-frame.about-blank.text","about:blank");
//        }
        m_pushButtonTop->setText(itemText);
        m_pushButtonTop->setToolTip(itemText);
        return true;
    }
    return false;
}

//void EbWorkItem::onItemTitleChanged(EbWidgetWorkView * view, const QString &title)
//{
//    if (m_pushButtonTop!=0) {
//        m_pushButtonTop->setText(title);
//        m_pushButtonTop->setToolTip(title);
//    }
//}

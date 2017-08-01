#include "ebdialogworkframe.h"
#include "ui_ebdialogworkframe.h"
#include "ebclientapp.h"
#include "ebwidgetsearchbar.h"
#include "ebdialogframelist.h"
#include "ebiconhelper.h"
#include "ebwidgetappbar.h"
#include <QTimer>

const int const_frame_tool = 29;

EbDialogWorkFrame::EbDialogWorkFrame(bool showSearchBae, bool showAppBar, QWidget *parent)
    : EbDialogBase(parent)
    , EbWorkList(this)
    , ui(new Ui::EbDialogWorkFrame)
    , m_widgetSearchBar(0), m_showSearchBar(showSearchBae)
    , m_widgetAppBar(0), m_showAppBar(showAppBar)
    , m_timerIdCheckState(0)
    , m_showedLeft(true)
{
    if (m_showSearchBar) {
        m_widgetSearchBar = new EbWidgetSearchBar(this);
        connect( m_widgetSearchBar,SIGNAL(clickedBack()),this,SLOT(onClickedBack()) );
        connect( m_widgetSearchBar,SIGNAL(clickedForward()),this,SLOT(onClickedForward()) );
        connect( m_widgetSearchBar,SIGNAL(clickedRefresh()),this,SLOT(onClickedRefresh()) );
        connect( m_widgetSearchBar,SIGNAL(clickedStop()),this,SLOT(onClickedStop()) );
        connect( m_widgetSearchBar,SIGNAL(searchKeyPressEnter(QString)),this,SLOT(onSearchKeyPressEnter(QString)) );
        connect( m_widgetSearchBar,SIGNAL(saveUrlHistory()),this,SLOT(onSaveUrlHistory()) );
        //    connect( m_widgetSearchBar,SIGNAL(searchKeyPressEsc()),this,SLOT(onSearchKeyPressEsc()) );
        //    connect( m_widgetSearchBar,SIGNAL(searchTextChange(QString)),this,SLOT(onSearchTextChange(QString)) );
        connect( (const QObject*)m_widgetSearchBar->searchResult(),SIGNAL(clickedSearchResultUrl(QString)),this,SLOT(onClickedSearchResultUrl(QString)) );
    }
    if (m_showAppBar) {
        m_widgetAppBar = new EbWidgetAppBar( EB_FUNC_LOCATION_APPFRAME_BTN, this );
        m_widgetAppBar->setOrientation( EbWidgetAppBar::Vertical );
        connect( m_widgetAppBar,SIGNAL(clickedSubApp(EB_SubscribeFuncInfo)),this,SLOT(onClickedSubApp(EB_SubscribeFuncInfo)) );
    }
    ui->setupUi(this);
    updateLocaleInfo();
    this->setObjectName("DialogChatBase");

    /// 去掉鼠标拖动 & 去掉边框
    EbDialogBase::setMouseEasyMove(false);
    EbDialogBase::setDrawBorder(false);
    /// 设置过滤 ESC/Enter 按键
    EbDialogBase::setFilterEscapeKey(true);
    EbDialogBase::setFilterEnterKey(true);
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );

    /// 左下角切换按钮
    if (m_showAppBar) {
        ui->pushButtonShowHideLeft->setObjectName("SwitchButton2");
        ui->pushButtonShowHideLeft->resize(16,16);
        connect( ui->pushButtonShowHideLeft,SIGNAL(clicked()),this,SLOT(onClickedPushButtonShowHideLeft()) );
        updateShowHideIcon();
    }
    else {
        ui->widgetLine3->setVisible(false);
        m_showedLeft = false;
        ui->pushButtonShowHideLeft->setVisible(false);
    }

    if (m_showSearchBar) {
        ui->pushButtonAdd->setObjectName("AddButton");
        ui->pushButtonAdd->resize(22,22);
        EbIconHelper::Instance()->SetIcon(ui->pushButtonAdd,QChar(0xf067),10);
        connect( ui->pushButtonAdd,SIGNAL(clicked()),this,SLOT(onClickedButtonAdd()) );
    }
    else {
        ui->widgetLine1->setVisible(false);
        ui->pushButtonAdd->setVisible(false);
    }

    ui->widgetLine1->setObjectName("LineWidget");
    ui->widgetLine1->lower();
    ui->widgetLine2->setObjectName("LineWidget");
    ui->widgetLine2->lower();
    ui->widgetLine3->setObjectName("LineWidget");
    ui->widgetLine3->lower();
    moveSize();

    m_timerIdCheckState = this->startTimer(200);

}

EbDialogWorkFrame::~EbDialogWorkFrame()
{
    if (m_timerIdCheckState!=0) {
        killTimer(m_timerIdCheckState);
        m_timerIdCheckState = 0;
    }
    delete ui;
}

void EbDialogWorkFrame::updateLocaleInfo()
{
    /// 显示右上角关闭按钮（这几个按钮还是要的，因为透明了也没用，点不到下面按钮）
    if (m_showSearchBar) {
        EbDialogBase::showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close"),"SysClose2Button" );
        EbDialogBase::showPushButtonSysMax( theLocales.getLocalText("base-dialog.maximize-button.tooltip","Maximize"),"SysTrans2Button" );
        EbDialogBase::showPushButtonSysMin( theLocales.getLocalText("base-dialog.minimize-button.tooltip","Minimize"),"SysTrans2Button" );

        ui->pushButtonAdd->setToolTip( theLocales.getLocalText("work-frame.button-add.tooltip","Open New Page") );
    }

    if (m_showAppBar) {
        if (m_showedLeft) {
            ui->pushButtonShowHideLeft->setToolTip( theLocales.getLocalText("work-frame.button-showhide-hide.tooltip","Hide Left"));
        }
        else {
            ui->pushButtonShowHideLeft->setToolTip( theLocales.getLocalText("work-frame.button-showhide-show.tooltip","Show Left"));
        }
    }
}

void EbDialogWorkFrame::setSubUnreadMsg( mycp::bigint subId, size_t unreadMsgCount )
{
    if (m_widgetAppBar!=0) {
        m_widgetAppBar->setSubscribeMsgCount( subId, unreadMsgCount );
    }
}

void EbDialogWorkFrame::addSubUnreadMsg( mycp::bigint subId )
{
    if (m_widgetAppBar!=0) {
        m_widgetAppBar->addSubscribeMsg( subId );
    }
}

void EbDialogWorkFrame::addUrl(bool bSaveBrowseTitle, const QString &sAppUrl, const QString &sPostData, int nInsertOffset)
{
    EB_SubscribeFuncInfo pSubscribeFuncInfo;
    pSubscribeFuncInfo.m_sFunctionName = sAppUrl.toStdString();
    pSubscribeFuncInfo.m_bClosePrompt = false;
    pSubscribeFuncInfo.m_bDisableScrollBar = false;
    pSubscribeFuncInfo.m_bDisableContextMenu = false;
    pSubscribeFuncInfo.m_bWinResizable = true;
    addUrl(bSaveBrowseTitle,sAppUrl,sPostData,pSubscribeFuncInfo,false,nInsertOffset);
}

void EbDialogWorkFrame::addUrl(bool bSaveBrowseTitle, const QString &sAppUrl, const QString &sPostData, const EB_SubscribeFuncInfo &pFuncInfo, bool nOpenNewClose, int nInsertOffset)
{
    const mycp::bigint nSubscribeId = (pFuncInfo.m_nFromSubscribeId>0)?pFuncInfo.m_nFromSubscribeId : pFuncInfo.m_nSubscribeId;
    if (nSubscribeId>0) {
        /// 订购应用，查询是否已经打开，如果是，直接显示；
        BoostReadLock rdlock(m_list.mutex());
        int index = 0;
        CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            const EbWorkItem::pointer pFrameWndInfo = *pIter;
            if (pFrameWndInfo->subscribeId()==nSubscribeId || pFrameWndInfo->fromSubscribeId()==nSubscribeId) {
                rdlock.unlock();
                pFrameWndInfo->setAppUrl(bSaveBrowseTitle,sAppUrl,sPostData);
                showByIndex( index,true );
                return;
            }
            else if (pFrameWndInfo->appUrl()==sAppUrl) {
                rdlock.unlock();
                pFrameWndInfo->refreshOrStop();     /// doRefresh
                showByIndex( index,true );
                return;
            }
            else if (pFrameWndInfo->appUrl()=="about:blank") {
                rdlock.unlock();
                pFrameWndInfo->setFunInfo(pFuncInfo,nOpenNewClose);
                pFrameWndInfo->setAppUrl(bSaveBrowseTitle,sAppUrl,sPostData);
                showByIndex( index,true );
                return;
            }
            index++;
        }
    }
    else {
//    else  if (nInsertOffset==-1) {
        int nCurrentIndex = 0;
        BoostReadLock rdlock(m_list.mutex());
        CLockList<EbWorkItem::pointer>::iterator pIter = m_list.begin();
        for (; pIter!=m_list.end(); pIter++) {
            const EbWorkItem::pointer pFrameWndInfo = *pIter;
            if (pFrameWndInfo->appUrl()=="about:blank" && (nInsertOffset==-1 || nCurrentIndex==nInsertOffset)) {
                rdlock.unlock();
                pFrameWndInfo->setFunInfo(pFuncInfo,nOpenNewClose);
                pFrameWndInfo->setAppUrl(bSaveBrowseTitle,sAppUrl,sPostData);
                if ( !pFrameWndInfo->isChecked() ) {
                    showByIndex( nCurrentIndex,true );
                }
                return;
            }
            nCurrentIndex++;
        }
    }

    EbWorkItem::pointer workItem = EbWorkItem::create(EbWorkItem::WORK_ITEM_WEB_BROWSER);
    workItem->setAppUrl( bSaveBrowseTitle,sAppUrl,sPostData );
    workItem->setFunInfo( pFuncInfo,nOpenNewClose);
    addWorkItem( bSaveBrowseTitle,workItem,nInsertOffset);
}

void EbDialogWorkFrame::onMoveEvent()
{
    if (m_widgetSearchBar!=0) {
        m_widgetSearchBar->onMoveEvent();
    }
}

void EbDialogWorkFrame::onClickedButtonTop()
{
    const QPushButton* leftButton = dynamic_cast<QPushButton*>( sender() );
    const QPoint pt = this->mapFromGlobal( cursor().pos() );
    EbWorkList::clickedTopButton( leftButton,pt );
    /// ???
//    if (m_list.empty()) {
//        this->hide();
    //    }
}

void EbDialogWorkFrame::onUrlChanged(const EbWidgetWorkView *, const QUrl &url)
{
    if (m_widgetSearchBar==0) {
        return;
    }
    const QString sUrl = url.url();
    if (sUrl.isEmpty() || sUrl=="about:blank")
        m_widgetSearchBar->changeUrl("",true,false);
    else
        m_widgetSearchBar->changeUrl(sUrl,false,false);
}

void EbDialogWorkFrame::onLoadStateChange(const EbWidgetWorkView *view, bool loadFinished, bool canGoBack, bool canGoForward)
{
    if (m_widgetSearchBar==0) {
        return;
    }
    m_widgetSearchBar->updateLoadState( loadFinished,canGoBack,canGoForward );
}

void EbDialogWorkFrame::onOpenLinkInNewTab(const EbWidgetWorkView *view, const QUrl &linkUrl)
{
    const QString url = linkUrl.url();
    if ( !url.isEmpty() ) {
        const int index = EbWorkList::indexOf(view);
        const int nInsertOffset = (index>=0)?index+1:-1;
        addUrl(false,url,"",nInsertOffset);
    }
}

void EbDialogWorkFrame::onWindowCloseRequested(const EbWidgetWorkView *view)
{
    /// 必须使用线程关闭
    m_listCloseView.add(view);
    QTimer::singleShot( 1, this, SLOT(onCloseViewList()) );
}

void EbDialogWorkFrame::onClickedBack()
{
    EbWorkList::back();
}

void EbDialogWorkFrame::onClickedForward()
{
    EbWorkList::forward();
}

void EbDialogWorkFrame::onClickedRefresh()
{
    EbWorkList::refreshOrStop();
}

void EbDialogWorkFrame::onClickedStop()
{
    EbWorkList::refreshOrStop();
}

void EbDialogWorkFrame::onSearchKeyPressEnter(const QString &text)
{
    addUrl(true,text,"");
}

void EbDialogWorkFrame::onSaveUrlHistory()
{
    EbWorkList::saveUrlHistory();
}

void EbDialogWorkFrame::onClickedSearchResultUrl(const QString &url)
{
    addUrl(true,url,"");
}

void EbDialogWorkFrame::onClickedPushButtonShowHideLeft()
{
    m_showedLeft = !m_showedLeft;
    updateShowHideIcon();
    if (m_widgetAppBar!=0) {
        m_widgetAppBar->setVisible( m_showedLeft );
    }
    moveSize();
}

void EbDialogWorkFrame::onClickedButtonAdd()
{
    addUrl(false,"about:blank","");
}

void EbDialogWorkFrame::onClickedSubApp(const EB_SubscribeFuncInfo & subFuncInfo)
{
    const QString sFullFuncurl = theApp->subscribeFuncUrl( subFuncInfo.m_nSubscribeId );
    if ( sFullFuncurl.isEmpty() ) {
        return;
    }
    this->addUrl( false, sFullFuncurl, "", subFuncInfo, false );
}

void EbDialogWorkFrame::onCloseViewList()
{
    const EbWidgetWorkView* view = 0;
    while (m_listCloseView.front(view)) {
        EbWorkList::closeItem( view );
    }
}

void EbDialogWorkFrame::updateShowHideIcon()
{
    if (m_showedLeft) {
        EbIconHelper::Instance()->SetIcon(ui->pushButtonShowHideLeft,QChar(0xf060),9);
    }
    else {
        EbIconHelper::Instance()->SetIcon(ui->pushButtonShowHideLeft,QChar(0xf061),9);
    }
    ui->pushButtonShowHideLeft->raise();

    updateLocaleInfo();
}

void EbDialogWorkFrame::moveSize()
{
    int y = 3;
    if (m_widgetSearchBar==0) {
        y = 0;
    }
    else {
        const int const_search_bar_height = 29;
        m_widgetSearchBar->setGeometry( 0,y,width()-getSysButtonWidth()-5,const_search_bar_height ); /// 右边跟右上角系统按钮 5 个像素间隔
        y += const_search_bar_height;
        ui->widgetLine1->setGeometry( 0,y,width(),1 );
        y += 1;
    }
    const int leftOffset = m_showedLeft?const_frame_tool:0;
    ui->widgetLine2->setGeometry( leftOffset,y+const_top_button_height-1,width()-leftOffset,1 );    /// 下面的线，显示在按钮的下边边框位置
    if (m_showAppBar) {
        ui->widgetLine3->setGeometry( leftOffset-1,y,1, height()-y );
    }
    m_topHeight = y;
    m_leftOffset = leftOffset;
//    EbWorkList::onResize( geometry(),y,leftOffset );
    int x  = EbWorkList::onResize( geometry() );
    onItemSizeChange( EbWorkItemNull,-1,x );

    if (m_widgetAppBar!=0) {
        y = ui->widgetLine1->geometry().bottom();
        m_widgetAppBar->setGeometry( 0, y, const_frame_tool, height()-y );
        x = m_showedLeft?10:0;
        ui->pushButtonShowHideLeft->move(x,height()-(16+16));
    }
}

void EbDialogWorkFrame::resizeEvent(QResizeEvent * e)
{
    moveSize();
    EbDialogBase::resizeEvent(e);
}

bool EbDialogWorkFrame::onBeforeClickedPushButtonSysMin()
{
    EbDialogFrameList* parent = (EbDialogFrameList*)this->parentWidget();
    parent->onClickedPushButtonSysMin();
    return false;
}

bool EbDialogWorkFrame::onBeforeClickedPushButtonSysMax()
{
    EbDialogFrameList* parent = (EbDialogFrameList*)this->parentWidget();
    parent->onClickedPushButtonSysMax();
    return false;
}

void EbDialogWorkFrame::reject()
{
    EbDialogFrameList* parent = (EbDialogFrameList*)this->parentWidget();
    parent->reject();
}

void EbDialogWorkFrame::timerEvent(QTimerEvent *event)
{
    if (m_timerIdCheckState!=0 && event->timerId()==m_timerIdCheckState) {
        const QPoint pos = cursor().pos();
        const QPoint pos2 = this->mapFromGlobal(pos);
        EbWorkList::checkShowHideCloseButton(pos2);
//        static int theIndex = 0;
//        if ( ((theIndex++)%5)==4 ) {
//            // 一秒检查一次
//            checkOneSecond();
//        }
    }
    EbDialogBase::timerEvent(event);
}

//void EbDialogWorkFrame::closeEvent(QCloseEvent *event)
//{
//    EbWorkList::clear();
//    EbDialogBase::closeEvent(event);
//}

void EbDialogWorkFrame::onItemSizeChange(const EbWorkItem::pointer & fromWorkItem,int size, int right)
{
    if (right==0) {
        right = m_showedLeft?const_frame_tool:0;
    }
    ui->pushButtonAdd->move( right+2,ui->widgetLine1->geometry().bottom()+5 );
    if ( size>=0 && fromWorkItem.get()!=0 ) {
        const bool fromAboutBlank = fromWorkItem->appUrl().isEmpty() || fromWorkItem->appUrl()=="about:blank";
        if (size==0 && !fromAboutBlank) {
            /// 添加一个空白页
            onClickedButtonAdd();
            return;
        }
        emit itemSizeChange(fromAboutBlank,size);
        if (size==0 && m_widgetSearchBar!=0) {
            m_widgetSearchBar->changeUrl( "",true,false );
        }
    }
    ui->pushButtonShowHideLeft->raise();
}

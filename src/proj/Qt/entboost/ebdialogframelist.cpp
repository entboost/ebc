#include "ebdialogframelist.h"
#include "ui_ebdialogframelist.h"
#include "ebiconhelper.h"
#include "ebdialogchatbase.h"
#include "ebdialogworkframe.h"

EbDialogFrameList::EbDialogFrameList(QWidget *parent)
    : EbDialogBase(parent)
    , EbFrameList(this)
    , ui(new Ui::EbDialogFrameList)
    , m_showedLeft(true)
    , m_timerIdCheckState(0)
    , m_menuContext(0), m_actionShowIconOnly(0)
{
    m_menuContext = new QMenu(this);
    m_actionShowIconOnly = m_menuContext->addAction( theLocales.getLocalText("frame-list.context-menu.show-icon-only.text","Show Icon Only") );
    m_actionShowIconOnly->setCheckable(true);
    m_actionShowIconOnly->setToolTip( theLocales.getLocalText("frame-list.context-menu.show-icon-only.tooltip","") );
    connect( m_actionShowIconOnly, SIGNAL(triggered()), this, SLOT(onClickedPushButtonShowHideLeft()) );
    QAction * action = m_menuContext->addAction( theLocales.getLocalText("frame-list.context-menu.open-work-frame.text","Show Icon Only") );
    action->setToolTip( theLocales.getLocalText("frame-list.context-menu.open-work-frame.tooltip","") );
    connect( action, SIGNAL(triggered()), this, SLOT(onClickedPushButtonShowWorkFrame()) );

//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
    ui->setupUi(this);

    /// 设置过滤 ESC/Enter 按键
    EbDialogBase::setFilterEscapeKey(true);
    EbDialogBase::setFilterEnterKey(true);
    /// 开启双击事件最大化窗口
    EbDialogBase::setDoubleClickMaxEnable(true);

    /// 设置初始大小
    const int const_frame_list_width = theLocales.getLocalInt("frame-list.window-size.width", 1180);
    const int const_frame_list_height = theLocales.getLocalInt("frame-list.window-size.height", 668);
    const QSize const_frame_list_size(const_frame_list_width,const_frame_list_height);
    this->resize(const_frame_list_size);
#ifdef __MACH__
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::WindowMinMaxButtonsHint|Qt::CustomizeWindowHint);
#else
    /// 去掉标题栏
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMinMaxButtonsHint);
#endif
//    // 显示右上角关闭按钮
//    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close"),"SysClose2Button" );
//    this->showPushButtonSysMax( theLocales.getLocalText("base-dialog.maximize-button.tooltip","Maximize"),"SysTrans2Button" );
//    this->showPushButtonSysMin( theLocales.getLocalText("base-dialog.minimize-button.tooltip","Minimize"),"SysTrans2Button" );

    /// 显示左边颜色块
    ui->widgetLeftBackground->setObjectName("LeftBackgroundWidget");
    ui->widgetLeftBackground->lower();
    /// 左下角切换按钮
    ui->pushButtonShowHideLeft->setObjectName("SwitchButton");
    ui->pushButtonShowHideLeft->resize(16,16);
    connect( ui->pushButtonShowHideLeft,SIGNAL(clicked()),this,SLOT(onClickedPushButtonShowHideLeft()) );
    updateShowHideIcon();

    /// for test
//    EbcCallInfo::pointer pCallInfo = EbcCallInfo::create();
//    EbDialogChatBase::pointer dialogChatBase = DialogChatBase::create(pCallInfo,this);
//    dialogChatBase->setVisible(false);
//    dialogChatBase->setModal(false);
//    EbFrameItem::pointer frameItem = EbFrameItem::create(dialogChatBase);
//    addFrameItem( frameItem,true,false );
//    dialogChatBase = DialogChatBase::create(pCallInfo,this);
//    dialogChatBase->setVisible(false);
//    dialogChatBase->setModal(false);
//    frameItem = EbFrameItem::create(dialogChatBase);
//    addFrameItem( frameItem,false,false );
//    dialogChatBase = DialogChatBase::create(pCallInfo,this);
//    dialogChatBase->setVisible(false);
//    dialogChatBase->setModal(false);
//    frameItem = EbFrameItem::create(dialogChatBase);
//    addFrameItem( frameItem,false,false );

    updateSize();

    m_timerIdCheckState = this->startTimer(200);
}

EbDialogFrameList::~EbDialogFrameList()
{
    if (m_timerIdCheckState!=0) {
        killTimer(m_timerIdCheckState);
        m_timerIdCheckState = 0;
    }
    delete ui;
}

void EbDialogFrameList::updateLocaleInfo()
{
    if (m_showedLeft) {
        ui->pushButtonShowHideLeft->setToolTip( theLocales.getLocalText("frame-list.button-showhide-hide.tooltip","Hide Left"));
    }
    else {
        ui->pushButtonShowHideLeft->setToolTip( theLocales.getLocalText("frame-list.button-showhide-show.tooltip","Show Left"));
    }
}

EbDialogWorkFrame *EbDialogFrameList::showWorkFrame()
{
    /// 加到第一个
    EbFrameList::addFrameItem( EbFrameItem::create(EbFrameItem::FRAME_ITEM_WORK_FRAME),true,false,false,0 );
    EbDialogWorkFrame * workFrame = EbFrameList::getWorkFrame();
    assert (workFrame!=0);
    if ( workFrame->isEmpty() ) {
        workFrame->addUrl(false,"about:blank","");
    }
    return workFrame;
}

void EbDialogFrameList::addFrameItem(const EbFrameItem::pointer &frameItem, bool bShow, bool bAutoCall, bool bForceAddToList)
{
    EbFrameList::addFrameItem( frameItem,bShow,bAutoCall,bForceAddToList );
}
void EbDialogFrameList::showFrameItem(mycp::bigint callId)
{
    EbFrameList::showByCallId(callId);
}

void EbDialogFrameList::showFrameList(bool show)
{
    if (show) {
        this->showNormal();
        this->activateWindow();
//        if (!m_bShrinkageWin)
//        {
//            CRect rect;
//            this->GetWindowRect(&rect);
//            if (CheckAutoHide(rect.left, rect.top, true))
//            {
//                m_pDlgShrinkageBar->ShowWindow(SW_SHOW);
//                this->ShowWindow(SW_SHOW);
//            }
//        }
//        KillTimer(TIMERID_CHECK_SHRINKAGEBAR);
//        SetTimer(TIMERID_CHECK_SHRINKAGEBAR,200,NULL);
    }
    else {
//        if (m_bShrinkageWin)
//        {
//            KillTimer(TIMERID_SHRINKAGE_WIN);
//            m_bShrinkageWin = false;
//            m_pDlgShrinkageBar->ShowWindow(SW_HIDE);
//            SetWindowPos(&wndNoTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
//        }
        this->setVisible(false);
//#ifdef USES_FRAMELIST_APPFRAME
//        if (m_pCurrentFrameWndInfo.get()!=NULL &&
//            (m_pList.empty() || (m_pCurrentFrameWndInfo->GetType()==CFrameWndInfo::FRAME_WND_WORK_FRAME && m_pDlgAppFrame->IsEmpty())))
//        {
//            m_pCurrentFrameWndInfo.reset();
//        }
//#endif
    }
}

void EbDialogFrameList::onClickedPushButtonShowWorkFrame()
{
    showWorkFrame();
}

void EbDialogFrameList::onClickedPushButtonShowHideLeft()
{
    m_showedLeft = !m_showedLeft;
    updateShowHideIcon();
    updateSize();
}

void EbDialogFrameList::onClickedButtonLeft(bool /*checked*/)
{
    const QPushButton* leftButton = dynamic_cast<QPushButton*>( sender() );
    const QPoint pt = this->mapFromGlobal( cursor().pos() );
    EbFrameList::clickedLeftButton( leftButton,pt );
    if (m_list.empty()) {
        this->hide();
    }
//    if ( checked ) {
//        ///
//    }
}

void EbDialogFrameList::onClickedCloseFromDialogChatBase()
{
    const EbDialogChatBase* chatBase = dynamic_cast<EbDialogChatBase*>( sender() );
    EbFrameList::closeItem(chatBase);
    if (m_list.empty()) {
        this->hide();
    }
}

void EbDialogFrameList::onCheckedFrameitem(const EbFrameItem * frameItem)
{
    this->setWindowTitle( frameItem->itemText() );
}

void EbDialogFrameList::onWorkItemSizeChange(bool ,int size)
{
    if (size<=0) {
//        if (EbFrameList::size()==1)
        /// 显示下一个，如果没有，显示
        if ( !EbFrameList::showByIndex(1) ) {
            this->hide();
        }
    }
}

void EbDialogFrameList::contextMenuEvent(QContextMenuEvent *e)
{
    m_menuContext->exec( e->globalPos() );
    EbDialogBase::contextMenuEvent(e);
}

void EbDialogFrameList::moveEvent(QMoveEvent *event)
{
//    EbFrameList::onMoveEvent();
    EbDialogBase::moveEvent(event);
}

void EbDialogFrameList::resizeEvent(QResizeEvent *e)
{
    updateSize();

    EbDialogBase::resizeEvent(e);
}

//bool EbDialogFrameList::event(QEvent *e)
//{
//    const QEvent::Type eventType = e->type();
//    switch ((EB_COMMAND_ID)eventType) {
//    case EB_COMMAND_SYS_MIN:
//        EbDialogBase::onClickedPushButtonSysMin();
//        return true;
//    case EB_COMMAND_SYS_MAX:
//        EbDialogBase::onClickedPushButtonSysMax();
//        return true;
//    case EB_COMMAND_SYS_CLOSE:
//        EbDialogBase::onClickedPushButtonSysClose();
//        return true;
//    default:
//        break;
//    }
//    return EbDialogBase::event(e);
//}

void EbDialogFrameList::checkOneSecond()
{
    static int theSecondIndex = 0;
    if ( ((theSecondIndex++)%60)==59 ) {
        /// 一分钟检查一次
        EbFrameList::checkCloseItem();
    }
}

void EbDialogFrameList::timerEvent(QTimerEvent *event)
{
    if (m_timerIdCheckState!=0 && event->timerId()==m_timerIdCheckState) {
        const QPoint pos = cursor().pos();
        const QPoint pos2 = this->mapFromGlobal(pos);
        EbFrameList::checkShowHideCloseButton(pos2);
        static int theIndex = 0;
        if ( ((theIndex++)%5)==4 ) {
            // 一秒检查一次
            checkOneSecond();
        }
    }
    EbDialogBase::timerEvent(event);
}

//void EbDialogFrameList::customEvent(QEvent *e)
//{
//    const QEvent::Type eventType = e->type();
//    switch ((EB_COMMAND_ID)eventType) {
//    case EB_COMMAND_SYS_MIN:
//        EbDialogBase::onClickedPushButtonSysMin();
//        e->ignore();
////        return;
//        break;
//    case EB_COMMAND_SYS_MAX:
//        EbDialogBase::onClickedPushButtonSysMax();
//        break;
//    case EB_COMMAND_SYS_CLOSE:
//        EbDialogBase::onClickedPushButtonSysClose();
//        break;
//    default:
//        break;
//    }
////    e->accept();
//}

void EbDialogFrameList::updateShowHideIcon()
{
    if (m_actionShowIconOnly!=0) {
        const bool checked = !m_showedLeft;
        m_actionShowIconOnly->setChecked(checked);
    }

    if (m_showedLeft) {
        EbIconHelper::Instance()->SetIcon(ui->pushButtonShowHideLeft,QChar(0xf060),9);
    }
    else {
        EbIconHelper::Instance()->SetIcon(ui->pushButtonShowHideLeft,QChar(0xf061),9);
    }
    updateLocaleInfo();
}

void EbDialogFrameList::updateSize()
{
    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
        ui->widgetLeftBackground->setGeometry( 0, 0, m_showedLeft?const_border_left:const_hide_border_chat, height() );
    }
    else {
        ui->widgetLeftBackground->setGeometry( 0, 0, m_showedLeft?const_border_left:const_hide_border_offiice, height() );
    }
    m_leftWidth = ui->widgetLeftBackground->geometry().width();
    ui->pushButtonShowHideLeft->move(8,height()-(8+16));
    EbFrameList::onResize( geometry() );

}

#include "ebdialogmessagetip.h"
#include "ui_ebdialogmessagetip.h"
#include "ebiconhelper.h"
#include "eblistwidgetitem.h"
#include "ebdialogmainframe.h"

const QSize const_icon_size(32,32);
const QSize const_ignore_size(55,19);
EbDialogMessageTip::EbDialogMessageTip(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogMessageTip)
{
    ui->setupUi(this);

#ifdef __MACH__
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
#else
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint );
#endif
    /// 屏蔽 ESC 按键不退出
    this->setFilterEscapeKey(true);
    this->setFilterEnterKey(true);

    updateLocaleInfo();

    /// 消息列表
    ui->listWidgetMessage->setObjectName("MessageTipList");
    ui->listWidgetMessage->setIconSize(const_icon_size);
    ui->listWidgetMessage->setMouseTracking(true);
    connect(ui->listWidgetMessage, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(onItemEnteredMessageTips(QListWidgetItem*)));
    connect(ui->listWidgetMessage, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onItemClickedMessageTips(QListWidgetItem*)));

    /// 忽略按钮 f014/f070
    EbIconHelper::Instance()->SetIcon(ui->pushButtonIgnore,QChar(0xf014),11);
    ui->pushButtonIgnore->setObjectName("IgnoreItem");
    ui->pushButtonIgnore->hide();
    ui->pushButtonIgnore->setParent( ui->listWidgetMessage );
    connect( ui->pushButtonIgnore, SIGNAL(clicked(bool)), this, SLOT(onClickedIgnoreItem(bool)));
    /// 忽略全部/查看全部
    ui->pushButtonIgnoreAll->setObjectName("MessageTipButton");
    ui->pushButtonViewAll->setObjectName("MessageTipButton");
    ui->pushButtonIgnoreAll->resize( const_ignore_size );
    ui->pushButtonViewAll->resize( const_ignore_size );
    connect( ui->pushButtonIgnoreAll, SIGNAL(clicked(bool)), this, SLOT(onClickedIgnoreAll(bool)));
    connect( ui->pushButtonViewAll, SIGNAL(clicked(bool)), this, SLOT(onClickedViewAll(bool)));

}

EbDialogMessageTip::~EbDialogMessageTip()
{
    delete ui;
}

void EbDialogMessageTip::addMsgTip(const QImage &image, eb::bigint nGroupId, eb::bigint nFromUserId, const QString &sMsgTip)
{
    bool bNewMsg = false;
    if (nGroupId>0) {
        EbWidgetItemInfo::pointer pItemInfo;
        if (m_pGroupItemInfo.find(nGroupId,pItemInfo)) {
            pItemInfo->m_listItem->setText(sMsgTip);
            pItemInfo->m_listItem->setToolTip(sMsgTip);
        }
        else {
            const QIcon icon = QIcon( QPixmap::fromImage(image).scaled(const_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            EbListWidgetItem * pItem0 = new EbListWidgetItem( icon, sMsgTip, ui->listWidgetMessage );
            pItem0->setToolTip(sMsgTip);
            ui->listWidgetMessage->insertItem(0,pItem0);
            pItemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_GROUP,0);
            pItem0->m_itemInfo = pItemInfo;
            pItemInfo->m_listItem = pItem0;
            pItemInfo->m_sId = 1;
            pItemInfo->m_nSubType = -2;
            pItemInfo->m_sGroupCode = nGroupId;
            pItemInfo->m_nUserId = nFromUserId;
            m_pGroupItemInfo.insert(nGroupId, pItemInfo);
            bNewMsg = true;
        }
    }
    else {
        EbWidgetItemInfo::pointer pItemInfo;
        if (m_pUserItemInfo.find(nFromUserId,pItemInfo)) {
            pItemInfo->m_listItem->setText(sMsgTip);
            pItemInfo->m_listItem->setToolTip(sMsgTip);
        }
        else {
            const QIcon icon = QIcon( QPixmap::fromImage(image).scaled(const_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            EbListWidgetItem * pItem0 = new EbListWidgetItem( icon, sMsgTip, ui->listWidgetMessage );
            pItem0->setToolTip(sMsgTip);
            ui->listWidgetMessage->insertItem(0,pItem0);
            pItemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_CONTACT,pItem0);
            pItem0->m_itemInfo = pItemInfo;
            pItemInfo->m_nUserId = nFromUserId;
            pItemInfo->m_dwItemData = EB_LINE_STATE_ONLINE_NEW;
            m_pUserItemInfo.insert(nFromUserId, pItemInfo);
            bNewMsg = true;
        }
    }

    if ( bNewMsg ) {
//    if (bNewMsg && this->isVisible()) {
        showMsgTip();
    }
}

void EbDialogMessageTip::addSubMsgTip(eb::bigint nId, eb::bigint nSubId, const QString &sMsgTip, const QString &sParam, eb::bigint nMsgId)
{
    EbWidgetItemInfo::pointer pItemInfo;
    if (m_pIdItemInfo.find(nId,pItemInfo)) {
        if (pItemInfo->m_sParentId>0 && pItemInfo->m_sParentId!=nMsgId)
        {
            const eb::bigint nMsgId = pItemInfo->m_sParentId;
            theApp->m_ebum.EB_AckMsg(nMsgId,1);
        }
        pItemInfo->m_listItem->setText(sMsgTip);
        pItemInfo->m_listItem->setToolTip(sMsgTip);
        pItemInfo->m_nBigId = nSubId;
        pItemInfo->m_sName = sParam.toStdString();
        pItemInfo->m_sParentId = nMsgId;
    }
    else {
        EbListWidgetItem * pItem0 = new EbListWidgetItem(sMsgTip, ui->listWidgetMessage);
        pItem0->setToolTip(sMsgTip);
        ui->listWidgetMessage->insertItem(0,pItem0);
        pItemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_SUBMSG,pItem0);
        pItem0->m_itemInfo = pItemInfo;
        pItemInfo->m_sId = nId;
        pItemInfo->m_nBigId = nSubId;
        pItemInfo->m_sName = sParam.toStdString();
        pItemInfo->m_sParentId = nMsgId;
        m_pIdItemInfo.insert(nId, pItemInfo);

        showMsgTip();
    }

}

void EbDialogMessageTip::addEmailMsgTip(eb::bigint nId, eb::bigint nEmailSubId, const QString &sMsgTip, const QString &sParam)
{
    EbWidgetItemInfo::pointer pItemInfo;
    if (m_pIdItemInfo.find(nId,pItemInfo)) {
        pItemInfo->m_listItem->setText(sMsgTip);
        pItemInfo->m_listItem->setToolTip(sMsgTip);
    }
    else {
        EbListWidgetItem * pItem0 = new EbListWidgetItem(sMsgTip, ui->listWidgetMessage);
        pItem0->setToolTip(sMsgTip);
        ui->listWidgetMessage->insertItem(0,pItem0);
        pItemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_EMAIL,pItem0);
        pItem0->m_itemInfo = pItemInfo;
        pItemInfo->m_sId = nId;
        pItemInfo->m_nBigId = nEmailSubId;
        pItemInfo->m_sName = sParam.toStdString();
        m_pIdItemInfo.insert(nId, pItemInfo);

        showMsgTip();
    }
}

bool EbDialogMessageTip::isEmpty() const
{
    return ui->listWidgetMessage->count()==0?true:false;
}

void EbDialogMessageTip::updateLocaleInfo()
{
    /// 显示标题
    const QString sTitle = theLocales.getLocalText("message-tip-dialog.title","Message Tip");
    this->showTitleLogoText( sTitle,const_common_title_font_size,QChar::Null,12,"MessageTipTitle" );
    this->setWindowTitle(sTitle);

    ui->pushButtonIgnore->setToolTip( theLocales.getLocalText("message-tip-dialog.button-ignore-item.tooltip","Ignore") );
    ui->pushButtonIgnoreAll->setText( theLocales.getLocalText("message-tip-dialog.button-ignore-all.text","Ignore All") );
    ui->pushButtonIgnoreAll->setToolTip( theLocales.getLocalText("message-tip-dialog.button-ignore-all.tooltip","") );
    ui->pushButtonViewAll->setText( theLocales.getLocalText("message-tip-dialog.button-view-all.text","View All") );
    ui->pushButtonViewAll->setToolTip( theLocales.getLocalText("message-tip-dialog.button-view-all.tooltip","") );

}

void EbDialogMessageTip::onItemEnteredMessageTips(QListWidgetItem *item)
{
    ui->listWidgetMessage->setCurrentItem(item);
    const QRect rectItem = ui->listWidgetMessage->visualItemRect(item);
    /// 滚动条能正常显示
    const QPoint pointItem = ui->listWidgetMessage->mapToParent(rectItem.topRight());
    const int buttonSize = rectItem.height()-2;
    ui->pushButtonIgnore->setGeometry( pointItem.x()-buttonSize,rectItem.top()+1,buttonSize,buttonSize );
    ui->pushButtonIgnore->setVisible(true);
}

void EbDialogMessageTip::deleteItem(QListWidgetItem *item)
{
    if (item==NULL) return;
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)item;
    const EbWidgetItemInfo::pointer & pTreeItemInfo = ebitem->m_itemInfo;
    if (pTreeItemInfo->m_nItemType == EbWidgetItemInfo::ITEM_TYPE_GROUP || pTreeItemInfo->m_nItemType == EbWidgetItemInfo::ITEM_TYPE_CONTACT) {
        delMsgTip( pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId );
    }
    else if (pTreeItemInfo->m_nItemType >= EbWidgetItemInfo::ITEM_TYPE_EMAIL) {
        delEmailMsgTip( pTreeItemInfo->m_sId );
    }
}

void EbDialogMessageTip::callItem(QListWidgetItem *item)
{
    if (item==0) return;
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)item;
    const EbWidgetItemInfo::pointer & pTreeItemInfo = ebitem->m_itemInfo;
    if (pTreeItemInfo->m_nItemType == EbWidgetItemInfo::ITEM_TYPE_EMAIL && pTreeItemInfo->m_nBigId>0) {
        const eb::bigint nEmailSubId = pTreeItemInfo->m_nBigId;
        if (m_emailSubscribeFuncInfo.m_nSubscribeId!=nEmailSubId) {
            if (!theApp->m_ebum.EB_GetSubscribeFuncInfo(nEmailSubId,&m_emailSubscribeFuncInfo))
                return;
        }
        if (theApp->mainWnd()->openSubscribeFuncWindow(m_emailSubscribeFuncInfo,"", pTreeItemInfo->m_sName)) {
            delEmailMsgTip(pTreeItemInfo->m_sId);
//            char* lpszSubId = new char[24];
//            sprintf(lpszSubId,"%lld",m_emailSubscribeFuncInfo.m_nSubscribeId);
//            theApp.GetMainWnd()->PostMessage(EB_COMMAND_CLEAR_SUBID_UNREAD_MSG, (WPARAM)lpszSubId,1);
        }
    }
    else if (pTreeItemInfo->m_nItemType == EbWidgetItemInfo::ITEM_TYPE_GROUP || pTreeItemInfo->m_nItemType == EbWidgetItemInfo::ITEM_TYPE_CONTACT) {
        if (pTreeItemInfo->m_sGroupCode>0)
            theApp->m_ebum.EB_CallGroup(pTreeItemInfo->m_sGroupCode);
        else
            theApp->m_ebum.EB_CallUserId(pTreeItemInfo->m_nUserId);
        delMsgTip( pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId );
    }
    else if (pTreeItemInfo->m_nItemType == EbWidgetItemInfo::ITEM_TYPE_SUBMSG) {
        EbWidgetItemInfo::pointer pItemInfo;
        if (m_pIdItemInfo.find(pTreeItemInfo->m_sId,pItemInfo)) {
            if (pItemInfo->m_sParentId>0) {
                const eb::bigint nMsgId = pItemInfo->m_sParentId;
                theApp->m_ebum.EB_AckMsg(nMsgId,1);
                pItemInfo->m_sParentId = 0;
            }
        }

        const eb::bigint nSubId = pTreeItemInfo->m_nBigId;
        if (nSubId==0) {
            delEmailMsgTip( pTreeItemInfo->m_sId );
            return;
        }
        EB_SubscribeFuncInfo pSubscribeFuncInfo;
        if (!theApp->m_ebum.EB_GetSubscribeFuncInfo(nSubId,&pSubscribeFuncInfo)) {
            return;
        }
        if (theApp->mainWnd()->openSubscribeFuncWindow(pSubscribeFuncInfo,"", pTreeItemInfo->m_sName)) {
            delEmailMsgTip(pTreeItemInfo->m_sId);
//            char* lpszSubId = new char[24];
//            sprintf(lpszSubId,"%lld",pSubscribeFuncInfo.m_nSubscribeId);
//            theApp.GetMainWnd()->PostMessage(EB_COMMAND_CLEAR_SUBID_UNREAD_MSG, (WPARAM)lpszSubId,1);
        }
    }
}

void EbDialogMessageTip::onItemClickedMessageTips(QListWidgetItem *item)
{
    callItem(item);
}

void EbDialogMessageTip::onClickedIgnoreItem(bool)
{
    deleteItem(ui->listWidgetMessage->currentItem());
}

void EbDialogMessageTip::onClickedIgnoreAll(bool)
{
    {
        BoostReadLock rdlock(m_pIdItemInfo.mutex());
        CLockMap<eb::bigint,EbWidgetItemInfo::pointer>::iterator pIter = m_pIdItemInfo.begin();
        for (; pIter!=m_pIdItemInfo.end(); pIter++)
        {
            const EbWidgetItemInfo::pointer& pItemInfo = pIter->second;
            if (pItemInfo->m_sParentId>0)
            {
                const eb::bigint nMsgId = pItemInfo->m_sParentId;
                theApp->m_ebum.EB_AckMsg(nMsgId,1);
                pItemInfo->m_sParentId = 0;
            }
        }
    }
    ui->listWidgetMessage->clear();
    m_pIdItemInfo.clear();
    m_pGroupItemInfo.clear();
    m_pUserItemInfo.clear();
    this->hide();
}

void EbDialogMessageTip::onClickedViewAll(bool)
{
    while ( !isEmpty() ) {
        ui->listWidgetMessage->setCurrentRow(0);
        callItem(ui->listWidgetMessage->currentItem());
    }
    onClickedIgnoreAll(false);
}

#define WINDOW_TITLE_HEIGHT			36
void EbDialogMessageTip::resizeEvent(QResizeEvent * e)
{
    int x = 1;
    int y = WINDOW_TITLE_HEIGHT;
    ui->listWidgetMessage->setGeometry( x,y,width()-x*2,height()-y-30);
    x = 8;
    y = height()-25;
    ui->pushButtonIgnoreAll->move( x,y );
    x = width()-const_ignore_size.width()-8;
    ui->pushButtonViewAll->move( x,y );

    EbDialogBase::resizeEvent(e);
}

void EbDialogMessageTip::showMsgTip()
{
    const QRect & rectScreen = theApp->screenRect();
    const int m_nScreenHeight = rectScreen.height();
    const int const_width = 280;
    QRect rect;
    rect.setLeft( rectScreen.width()-const_width-10 );
    rect.setRight( rect.x()+const_width );
#ifdef __MATH__
    rect.setBottom( m_nScreenHeight-85 );
#else
    rect.setBottom( m_nScreenHeight-40 );
#endif
    const int nHeight = WINDOW_TITLE_HEIGHT+MIN(10,ui->listWidgetMessage->count())*40+30;
    rect.setTop( rect.bottom()-nHeight);
    this->setGeometry(rect);
    if (!this->isVisible()) {
        this->show();
//        this->activateWindow();
    }
    else {
        return;
    }
//    if (this->IsIconic())
//        this->ShowWindow(SW_RESTORE);
//    else if (!this->IsWindowVisible())
//        this->ShowWindow(SW_SHOW);
//    else
//        return;
    //    this->SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
}

void EbDialogMessageTip::checkCursorInMsgArea()
{
    QPoint pos = cursor().pos();
    const QRect& rect = this->geometry();
    if (rect.contains(pos)) {
        return;
    }
//    rect.left = m_posTrayIcon.x-20;
//    rect.right = m_posTrayIcon.x+20;
//    rect.top = m_posTrayIcon.y-20;
//    rect.bottom = m_posTrayIcon.y+20;
//    if (rect.PtInRect(pos))
//        return;
    this->hide();
}

void EbDialogMessageTip::delMsgTip(eb::bigint nGroupId, eb::bigint nFromUserId)
{
    try {
        EbWidgetItemInfo::pointer itemInfo;
        if (nGroupId>0) {
            if (!m_pGroupItemInfo.find(nGroupId,itemInfo,true)) {
                return;
            }
        }
        else {
            if (!m_pUserItemInfo.find(nFromUserId,itemInfo,true)) {
                return;
            }
        }
        const int row = ui->listWidgetMessage->row(itemInfo->m_listItem);
        QListWidgetItem * item = ui->listWidgetMessage->takeItem(row);
        if (item!=0)
            delete item;
        if (isEmpty())
            this->hide();
        else
            this->showMsgTip();
    }
    catch(const std::exception&) {
    }
    catch(...) {
    }
}

void EbDialogMessageTip::delEmailMsgTip(eb::bigint nId)
{
    EbWidgetItemInfo::pointer itemInfo;
    if (!m_pIdItemInfo.find(nId,itemInfo,true)) {
        return;
    }
    QListWidgetItem * item = ui->listWidgetMessage->takeItem( ui->listWidgetMessage->row(itemInfo->m_listItem) );
    if (item!=0)
        delete item;
    if (isEmpty())
        this->hide();
    else
        this->showMsgTip();
}

#include "ebwidgetsearchresult.h"
#include "eblabel.h"
#include "ebiconhelper.h"
#include "eblistwidgetitem.h"
#include "ebdialogmainframe.h"

EbWidgetSearchResult::EbWidgetSearchResult(EB_SEARCH_FROM_FLAG fromFlag, QWidget *parent) : QWidget(parent)
  , m_searchFromFlag(fromFlag)
  , m_listWidgetResults(0)
  , m_selectedUserCallback(0)
  , m_itemHeight(0)
{
    ///
    this->setWindowFlags( Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint );
//    this->setStyleSheet("QWidget:{border: none;}");

    m_listWidgetResults = new EbListWidget(this);
    m_listWidgetResults->setCursor( QCursor(Qt::PointingHandCursor) );
    m_listWidgetResults->setObjectName("SelectedUser");
    m_listWidgetResults->setIconSize(const_tree_icon_size);
    connect( m_listWidgetResults,SIGNAL(keyPressEnter(const QListWidgetItem*)),this,SLOT(onKeyPressEnterItem(const QListWidgetItem*)) );
    m_listWidgetResults->setMouseTracking(true);
    connect( m_listWidgetResults,SIGNAL(itemEntered(QListWidgetItem*)),this,SLOT(onItemEntered(QListWidgetItem*)) );
    connect( m_listWidgetResults,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onItemDoubleClicked(QListWidgetItem*)) );
//    connect( m_listWidgetResults,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(onItemDoubleClicked(QListWidgetItem*)) );
//    connect( m_listWidgetResults,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onItemClicked(QListWidgetItem*)) );
    m_menu = new QMenu(this);
    /// 复制
    m_actionCopy = m_menu->addAction( "" );
    m_actionCopy->setVisible(false);
    connect( m_actionCopy, SIGNAL(triggered()), this, SLOT(onTriggeredActionCopy()) );
    /// ----------------------
    m_menu->addSeparator();
    /// 删除
    m_actionDelete = m_menu->addAction( "" );
    m_actionDelete->setVisible(false);
    connect( m_actionDelete, SIGNAL(triggered()), this, SLOT(onTriggeredActionDelete()) );
    /// 打开会话
    m_actionOpenChat = m_menu->addAction( "" );
    m_actionOpenChat->setVisible(false);
    connect( m_actionOpenChat, SIGNAL(triggered()), this, SLOT(onTriggeredActionOpenChat()) );

    /// “添加”按钮
    m_pushButtonSelect = new QPushButton(this);
    m_pushButtonSelect->setParent( m_listWidgetResults );
    m_pushButtonSelect->setVisible(false);
    m_pushButtonSelect->setObjectName("CallButton");
    connect( m_pushButtonSelect, SIGNAL(clicked()),this,SLOT(onClickedButtonSelect()) );
    EbIconHelper::Instance()->SetIcon(m_pushButtonSelect,QChar(0xf067),12 );

    updateLocaleInfo();
}

void EbWidgetSearchResult::search(const char *key)
{
    m_listWidgetResults->clear();
    switch (m_searchFromFlag) {
    case EB_SEARCH_FROM_SEARCH_BAR: {
        const int limit = 20;
        searchApp(key,limit);
        searchUrlRecord(key,limit);
        searchUserInfo(key);
        break;
    }
    case EB_SEARCH_FROM_MAIN_FRAME: {
        const int limit = 20;
        searchApp(key,limit);
        searchUserInfo(key);
        const int count = m_listWidgetResults->count();
        if (count<limit) {
            searchUrlRecord( key,MAX(3,limit-count) );
        }
        break;
    }
    case EB_SEARCH_FROM_SELECT_USER: {
        searchUserInfo(key);
        break;
    }
    default:
        break;
    }
    checkShowHide();
}

void EbWidgetSearchResult::setFocusSelectFirst()
{
    if  ( this->isVisible() && m_listWidgetResults->count()>0 ) {
        m_listWidgetResults->setCurrentRow(0);
        m_listWidgetResults->setFocus();
    }
}

void EbWidgetSearchResult::updateLocaleInfo()
{
    m_actionCopy->setText( theLocales.getLocalText("search-result.context-menu.copy.text","Copy") );
    m_actionCopy->setToolTip( theLocales.getLocalText("context-menu.copy.tooltip","") );
    m_actionDelete->setText( theLocales.getLocalText("search-result.context-menu.delete.text","Delete") );
    m_actionDelete->setToolTip( theLocales.getLocalText("context-menu.delete.tooltip","") );
    m_actionOpenChat->setText( theLocales.getLocalText("search-result.context-menu.open-chat.text","Open chat") );
    m_actionOpenChat->setToolTip( theLocales.getLocalText("context-menu.open-chat.tooltip","") );

}

void EbWidgetSearchResult::onKeyPressEnterItem(const QListWidgetItem *item)
{
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)item;
    switch (ebitem->m_itemInfo->m_nItemType) {
    case EbWidgetItemInfo::ITEM_TYPE_SUBINFO: {
        const eb::bigint subId = ebitem->m_itemInfo->m_sId;
        theApp->mainWnd()->openSubscribeFuncWindow(subId);
        break;
    }
    case EbWidgetItemInfo::ITEM_TYPE_MEMBER:
    case EbWidgetItemInfo::ITEM_TYPE_CONTACT: {
        theApp->m_ebum.EB_CallUserId(ebitem->m_itemInfo->m_nUserId);
        break;
    }
    case EbWidgetItemInfo::ITEM_TYPE_URL: {
        const QString url = ebitem->m_itemInfo->m_sName.c_str();
        if ( !url.isEmpty() ) {
            emit clickedSearchResultUrl(url);
        }
        break;
    }
    default:
        break;
    }
    this->hide();
}

void EbWidgetSearchResult::onItemEntered(QListWidgetItem *item)
{
    if (!m_listWidgetResults->hasFocus()) {
        m_listWidgetResults->setFocus();
    }
    /// 滚动条能正常显示
    const QRect rectItem = m_listWidgetResults->visualItemRect(item);
    if (m_itemHeight!=rectItem.height()) {
        m_itemHeight = rectItem.height();
    }
    const int y = rectItem.y();
    /// -2（配合下面的 y+1）实现删除按钮显示时，保留ITEM边框，
    const int buttonSize = rectItem.height()-2;
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)item;
    if ( ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_MEMBER ||
         ebitem->m_itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_CONTACT ) {

        const bool isSelectedUser = m_selectedUserCallback==0?false:m_selectedUserCallback->isSelectedUser(ebitem->m_itemInfo);
        if (!isSelectedUser) {
            m_pushButtonSelect->setGeometry( rectItem.right()-buttonSize,y+1,buttonSize,buttonSize );
            m_pushButtonSelect->setProperty("track-item",QVariant((quint64)item));
            m_pushButtonSelect->setVisible(true);
        }
        else {
            m_pushButtonSelect->setVisible(false);
        }
    }
    else {
        m_pushButtonSelect->setVisible(false);
    }
}

void EbWidgetSearchResult::onItemDoubleClicked(QListWidgetItem *item)
{
    onKeyPressEnterItem(item);
}

void EbWidgetSearchResult::onClickedItemLabel()
{
    const EbLabel * labelItem = (EbLabel*)sender();
    const QString url = labelItem->property("url").toString();
    if ( !url.isEmpty() ) {
        emit clickedSearchResultUrl(url);
    }
    this->hide();
}

void EbWidgetSearchResult::onTriggeredActionCopy()
{
    /// 复制 URL
    EbListWidgetItem * ebitem = (EbListWidgetItem*)m_listWidgetResults->currentItem();
    if (ebitem==0) {
        return;
    }
    const QString url = ebitem->m_itemInfo->m_sName.c_str();
    QClipboard *board = QApplication::clipboard();
    board->setText( url );
}

void EbWidgetSearchResult::onTriggeredActionDelete()
{
    /// 删除 URL
    EbListWidgetItem * ebitem = (EbListWidgetItem*)m_listWidgetResults->takeItem(m_listWidgetResults->currentRow());
    if (ebitem==0) {
        return;
    }
    const QString url = ebitem->m_itemInfo->m_sName.c_str();
    const QString iconFilePath = theApp->urlIconFilePath( QUrl(url) );
    QFile::remove(iconFilePath);
    const QString sql = QString("DELETE FROM url_record_t WHERE url='%1'").arg(url);
    theApp->m_sqliteUser->execute( sql.toStdString().c_str() );
    m_listWidgetResults->removeItemWidget(ebitem);
    delete ebitem;
    checkShowHide();
}

void EbWidgetSearchResult::onTriggeredActionOpenChat()
{
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)m_listWidgetResults->currentItem();
    if (ebitem==0) {
        return;
    }
    theApp->m_ebum.EB_CallUserId(ebitem->m_itemInfo->m_nUserId);
}

//void EbWidgetSearchResult::onKeyPressEnterItemLabel()
//{
//    onClickedItemLabel();
//}

void EbWidgetSearchResult::onClickedButtonSelect()
{
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)m_pushButtonSelect->property("track-item").toULongLong();
    if (ebitem!=0 && ebitem->m_itemInfo.get()!=0) {
        emit selectedItemInfo(ebitem->m_itemInfo);
    }
    m_pushButtonSelect->hide();
    m_pushButtonSelect->setProperty("track-item",QVariant((quint64)0));
    m_listWidgetResults->setFocus();
}

//void EbWidgetSearchResult::onItemClicked(QListWidgetItem *item)
//{
//    const QString url = item->data(Qt::UserRole).toString();
//    if ( !url.isEmpty() ) {
//        emit clickedSearchResultUrl(url);
//    }
//}

void EbWidgetSearchResult::resizeEvent(QResizeEvent *e)
{
    m_listWidgetResults->setGeometry( 0,0,width(),height() );

    QWidget::resizeEvent(e);
}

void EbWidgetSearchResult::hideAllAciton(void)
{
    m_actionCopy->setVisible(false);
    m_actionDelete->setVisible(false);
    m_actionOpenChat->setVisible(false);

}

void EbWidgetSearchResult::contextMenuEvent(QContextMenuEvent *e)
{
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)m_listWidgetResults->currentItem();
    if (ebitem==0) {
        return;
    }
    hideAllAciton();
    switch (ebitem->m_itemInfo->m_nItemType) {
    case EbWidgetItemInfo::ITEM_TYPE_MEMBER:
    case EbWidgetItemInfo::ITEM_TYPE_CONTACT: {
        m_actionOpenChat->setVisible(true);
        break;
    }
    case EbWidgetItemInfo::ITEM_TYPE_URL: {
        m_actionCopy->setVisible(true);
        m_actionDelete->setVisible(true);
        break;
    }
    default:
        return;
    }
    this->m_menu->exec(e->globalPos());
//    QWidget::contextMenuEvent(e);
}

void EbWidgetSearchResult::checkShowHide()
{
    const int count = m_listWidgetResults->count();
    const int w = width();
    const int h = MIN(count,20)*MAX(m_itemHeight,26)+2;

    resize(w,h);
    if (count==0) {
        this->hide();
    }
    else if ( !this->isVisible() ){
        this->show();
        this->raise();
    }
}

void EbWidgetSearchResult::searchApp(const char *key, int limit)
{
    /// *搜索应用
    const tstring searchKey(key);
    if (searchKey.empty()) {
        return;
    }
    else if (searchKey.c_str()[0]!='/') {
        return;
    }
    std::vector<EB_SubscribeFuncInfo> m_pSubscribeFuncList;	/// for menu

    tstring sAppKey = searchKey.substr(1);
    std::transform(sAppKey.begin(), sAppKey.end(), sAppKey.begin(), tolower);
    m_pSubscribeFuncList.clear();
    theApp->m_ebum.EB_GetSubscribeFuncList((EB_FUNC_LOCATION)0xffffff,m_pSubscribeFuncList);
    int index = 0;
    for (size_t i=0; i<m_pSubscribeFuncList.size(); i++) {
        const EB_SubscribeFuncInfo &funcInfo = m_pSubscribeFuncList[i];
        tstring sFunctionName(funcInfo.m_sFunctionName);
        tstring sDescription(funcInfo.m_sDescription);
        std::transform(sFunctionName.begin(), sFunctionName.end(), sFunctionName.begin(), tolower);
        std::transform(sDescription.begin(), sDescription.end(), sDescription.begin(), tolower);

        if (sFunctionName.find(sAppKey)==std::string::npos &&
            sDescription.find(sAppKey)==std::string::npos) {
            continue;
        }

        const QString label = QString("%1\n%2").arg(funcInfo.m_sFunctionName.c_str()).arg(funcInfo.m_sDescription.c_str());
        const QImage image = theApp->funcImage(&funcInfo);
        const QIcon icon( QPixmap::fromImage(image).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
        EbListWidgetItem * pItem0 = new EbListWidgetItem( icon,label, m_listWidgetResults);
        pItem0->m_itemInfo = EbWidgetItemInfo::create(&funcInfo,pItem0);
        m_listWidgetResults->addItem(pItem0);
        if (m_itemHeight==0) {
            m_itemHeight = m_listWidgetResults->visualItemRect(pItem0).height();
        }
        if ((++index)>=limit) {
            break;
        }

//        CTreeItemInfo::pointer pContactGroupInfo;
//        if (!m_pSubFuncItem.find(0,pContactGroupInfo))
//        {
//            HTREEITEM hEmpItem = m_treeSearch.InsertItem(theAppGroupResultName.c_str());
//            pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
//            pContactGroupInfo->m_dwItemData = theApp.GetWorkFrameShowed()?3:0;	// 保证永远排前面
//            m_pSubFuncItem.insert(0,pContactGroupInfo);
//            m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
//            m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
//        }

//        if (!m_pSubFuncItem.exist(pFuncInfo.m_nSubscribeId))
//        {
//            const std::string sTitle(pFuncInfo.m_sFunctionName);
//            CString sSearchName;
//            sSearchName.Format(_T("%s\nCLR=%d;%s"),sTitle.c_str(),(int)RGB(0,128,255),pFuncInfo.m_sDescription.c_str());
//            HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
//            CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_SUBINFO,hContactItem);
//            pContactItemInfo->m_sName = sTitle;
//            pContactItemInfo->m_sId = pFuncInfo.m_nSubscribeId;
//            pContactItemInfo->m_sGroupName = pFuncInfo.m_sResFile;
//            pContactItemInfo->m_sAccount = sFunctionName;
//            m_pSubFuncItem.insert(pContactItemInfo->m_sId,pContactItemInfo);
//            m_treeSearch.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
//            ShowSearchResult();
//            const int nGroupResultSize = m_pSubFuncItem.size()-1;
//            if (nGroupResultSize==1)
//            {
//                if (this->GetWorkFrameShowed() && m_treeSearch.GetSelectedItem()==NULL)
//                    m_treeSearch.Select(hContactItem,TVGN_CARET);
//                m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
//            }
//            sSearchName.Format(_T("%s%d"),theAppGroupResultName.c_str(),nGroupResultSize);
//            m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
//            if (nGroupResultSize>=15) break;
//        }
    }
}

void EbWidgetSearchResult::searchUrlRecord(const char *key, int limit)
{
    tstring searchKey(key);
    if (searchKey=="*") {
        searchKey.clear();
    }
    else if (searchKey.empty()) {
        return;
    }
    else {
        CSqliteCdbc::escape_string_in(searchKey);
    }

    char sql[2048];
    sprintf(sql, "select url,title,strftime('%%Y-%%m-%%d %%H:%%M',last_time,'localtime') "
                 "FROM url_record_t WHERE url LIKE '%%%s%%' OR title LIKE '%%%s%%' ORDER BY last_time DESC LIMIT %d",
            searchKey.c_str(),searchKey.c_str(),limit);
    int nCookie = 0;
    theApp->m_sqliteUser->select(sql, nCookie);
    cgcValueInfo::pointer pRecord = theApp->m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        tstring url(pRecord->getVector()[0]->getStrValue());
        tstring title(pRecord->getVector()[1]->getStrValue());
        const tstring lastTime(pRecord->getVector()[2]->getStrValue());
        pRecord = theApp->m_sqliteUser->next(nCookie);

//        CSqliteCdbc::escape_string_out(url);
//        CSqliteCdbc::escape_string_out(title);
        /// rgb(0,162,232)=00a2e8, rgb(128,128,128)=808080
        const QString label = QString("<font color=#00a2e8>&nbsp;%1</font> - <font color=#808080>%2</font>").arg(url.c_str()).arg(title.c_str());
        QString iconFilePath = theApp->urlIconFilePath( QUrl(url.c_str()) );
        if (!QFile::exists(iconFilePath)) {
            iconFilePath = ":/img/defaultapp.png";
        }
        const QIcon icon(iconFilePath);
        EbListWidgetItem * pItem0 = new EbListWidgetItem( icon,"", m_listWidgetResults );
        pItem0->m_itemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_URL,pItem0);
        pItem0->m_itemInfo->m_sName = url;
        m_listWidgetResults->addItem(pItem0);
        EbLabel * labelItem = new EbLabel(m_listWidgetResults);
        labelItem->setText(label);
        labelItem->setToolTip( lastTime.c_str() );
        labelItem->setProperty("url",QVariant(url.c_str()));
        connect( labelItem,SIGNAL(clicked()),this,SLOT(onClickedItemLabel()) );
//        connect( labelItem,SIGNAL(keyPressEnter()),this,SLOT(onKeyPressEnterItemLabel()) );
        m_listWidgetResults->setItemWidget( pItem0,labelItem );
        if ( m_listWidgetResults->count()==1 ) {
            m_listWidgetResults->setCurrentRow(0);
            emit searchFirst( QString(url.c_str()) );
        }
        if (m_itemHeight==0) {
            m_itemHeight = m_listWidgetResults->visualItemRect(pItem0).height();
        }
    }
    theApp->m_sqliteUser->reset(nCookie);
}

void EbWidgetSearchResult::searchUserInfo(const char *key)
{
    tstring searchKey(key);
    if (searchKey=="*") {
        searchKey.clear();
    }
    else if (searchKey.empty()) {
        return;
    }
    theApp->m_ebum.EB_SearchUserInfo((CEBSearchCallback*)this,searchKey.c_str(),0);
}

void EbWidgetSearchResult::onMemberInfo(const EB_GroupInfo *pGroupInfo, const EB_MemberInfo *pMemberInfo, unsigned long dwParam)
{
    const QString label = QString("%1 - %2\n%3(%4)").arg(pMemberInfo->m_sUserName.c_str()).arg(pGroupInfo->m_sGroupName.c_str())
            .arg(pMemberInfo->m_sMemberAccount.c_str()).arg(pMemberInfo->m_nMemberUserId);
    const QImage image = theApp->memberHeadImage(pMemberInfo);
    const QIcon icon( QPixmap::fromImage(image).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    EbListWidgetItem * pItem0 = new EbListWidgetItem( icon,label, m_listWidgetResults);
    pItem0->m_itemInfo = EbWidgetItemInfo::create(pMemberInfo,pItem0);
    m_listWidgetResults->addItem(pItem0);
    if (m_itemHeight==0) {
        m_itemHeight = m_listWidgetResults->visualItemRect(pItem0).height();
    }
}

void EbWidgetSearchResult::onContactInfo(const EB_ContactInfo *pContactInfo, unsigned long dwParam)
{
    const QString label = QString("%1 - %2\n%3(%4)").arg(pContactInfo->m_sName.c_str()).arg(pContactInfo->m_sGroupName.c_str())
            .arg(pContactInfo->m_sContact.c_str()).arg(pContactInfo->m_nContactUserId);
    const QImage image = theApp->contactHeadImage(pContactInfo);
    const QIcon icon( QPixmap::fromImage(image).scaled(const_tree_icon_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    EbListWidgetItem * pItem0 = new EbListWidgetItem( icon,label, m_listWidgetResults);
    pItem0->m_itemInfo = EbWidgetItemInfo::create(pContactInfo,pItem0);
    m_listWidgetResults->addItem(pItem0);
    if (m_itemHeight==0) {
        m_itemHeight = m_listWidgetResults->visualItemRect(pItem0).height();
    }

}


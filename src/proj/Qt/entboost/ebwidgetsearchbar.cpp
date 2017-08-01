#include "ebwidgetsearchbar.h"
#include "ebwidgetsearchresult.h"
#include "ebiconhelper.h"
#include "ebclientapp.h"
#include "eblineedit.h"
#include <QWebEngineProfile>
#include <ebmessagebox.h>

const QSize const_button_size(22,22);
EbWidgetSearchBar::EbWidgetSearchBar(QWidget *parent) : QWidget(parent)
  , m_canSearch(true)
  , m_canUpdateSearchFirst(false)
{
    m_buttonBack = new QPushButton(this);
    m_buttonBack->resize(const_button_size);
    m_buttonBack->setObjectName("SearchBarButton");
    EbIconHelper::Instance()->SetIcon(m_buttonBack,QChar(0xf060),10);
    connect( m_buttonBack,SIGNAL(clicked()),this,SLOT(onClickedButtonBack()) );
    m_buttonForward = new QPushButton(this);
    m_buttonForward->resize(const_button_size);
    m_buttonForward->setObjectName("SearchBarButton");
    EbIconHelper::Instance()->SetIcon(m_buttonForward,QChar(0xf061),10);
    connect( m_buttonForward,SIGNAL(clicked()),this,SLOT(onClickedButtonForward()) );
    m_buttonRefresh = new QPushButton(this);
    m_buttonRefresh->resize(const_button_size);
    m_buttonRefresh->setObjectName("SearchBarButton");
    EbIconHelper::Instance()->SetIcon(m_buttonRefresh,QChar(0xf01e),10);
    connect( m_buttonRefresh,SIGNAL(clicked()),this,SLOT(onClickedButtonRefresh()) );
    m_buttonStop = new QPushButton(this);
    m_buttonStop->hide();
    m_buttonStop->resize(const_button_size);
    m_buttonStop->setObjectName("SearchBarCloseButton");
    EbIconHelper::Instance()->SetIcon(m_buttonStop,QChar(0xf057),10);
    connect( m_buttonStop,SIGNAL(clicked()),this,SLOT(onClickedButtonStop()) );
    m_lineEditSearch = new EbLineEdit(this);
    m_lineEditSearch->setMouseFocusInSelectAll(true);
    m_lineEditSearch->installEventFilter(this);
    connect( m_lineEditSearch,SIGNAL(keyPressEnter(QString)),this,SLOT(onSearchEditKeyPressEnter(QString)) );
    connect( m_lineEditSearch,SIGNAL(keyPressEsc()),this,SLOT(onSearchEditKeyPressEsc()) );
    connect( m_lineEditSearch,SIGNAL(textChanged(QString)),this,SLOT(onSearchEditTextChange(QString)) );
    connect( m_lineEditSearch,SIGNAL(keyPressDown()),this,SLOT(onSearchEditKeyPressDown()) );
    m_buttonOptions = new QPushButton(this);
    m_buttonOptions->resize(const_button_size);
    m_buttonOptions->setObjectName("SearchBarButton");
    EbIconHelper::Instance()->SetIcon(m_buttonOptions,QChar(0xf0c9),10);
    connect( m_buttonOptions,SIGNAL(clicked()),this,SLOT(onClickedButtonOptions()) );
    m_menuOptions = new QMenu(this);
    m_actionSaveUrlHistory = m_menuOptions->addAction( "Save Url History");
    m_actionSaveUrlHistory->setData( QVariant((int)EB_COMMAND_SAVE_HISTORY) );
    connect( m_actionSaveUrlHistory, SIGNAL(triggered()), this, SLOT(onClickedMenuOptions()) );
    m_actionClearAllHistory = m_menuOptions->addAction( "Clear All History");
    m_actionClearAllHistory->setData( QVariant((int)EB_COMMAND_CLEAR_HISTORY) );
    connect( m_actionClearAllHistory, SIGNAL(triggered()), this, SLOT(onClickedMenuOptions()) );
    m_menuOptions->addSeparator();
    m_actionClearChromeCache = m_menuOptions->addAction( "Clear Chrome Cache");
    m_actionClearChromeCache->setData( QVariant((int)EB_COMMAND_CLEAR_CHROME_TEMPFILE) );
    connect( m_actionClearChromeCache, SIGNAL(triggered()), this, SLOT(onClickedMenuOptions()) );

    m_widgetSearchResult = new EbWidgetSearchResult(EB_SEARCH_FROM_SEARCH_BAR,parent);
    m_widgetSearchResult->setVisible(false);
    connect( m_widgetSearchResult,SIGNAL(searchFirst(QString)),this,SLOT(onSearchFirst(QString)) );
    connect( m_widgetSearchResult,SIGNAL(clickedSearchResultUrl(QString)),this,SLOT(onClickedSearchResultUrl(QString)) );
    connect( m_widgetSearchResult->listResults(),SIGNAL(keyPressFirstItemUp()),this,SLOT(onListResultsKeyPressFirstItemUp()) );
    connect( m_widgetSearchResult->listResults(),SIGNAL(keyPressEsc()),this,SLOT(onListResultsKeyPressEsc()) );

    updateLocaleInfo();
}

EbWidgetSearchBar::~EbWidgetSearchBar()
{
    delete m_widgetSearchResult;
}

void EbWidgetSearchBar::updateLocaleInfo()
{
    m_buttonBack->setToolTip( theLocales.getLocalText("search-bar.button-back.tooltip","Go Back") );
    m_buttonForward->setToolTip( theLocales.getLocalText("search-bar.button-forward.tooltip","Go Forward") );
    m_buttonRefresh->setToolTip( theLocales.getLocalText("search-bar.button-refresh.tooltip","Refresh") );
    m_buttonStop->setToolTip( theLocales.getLocalText("search-bar.button-stop.tooltip","Stop") );
    m_buttonOptions->setToolTip( theLocales.getLocalText("search-bar.button-options.tooltip","Options") );

    m_actionSaveUrlHistory->setText( theLocales.getLocalText("search-bar.options-menu.save-url-history.text","Save Url History") );
    m_actionSaveUrlHistory->setToolTip( theLocales.getLocalText("search-bar.options-menu.save-url-history.tooltip","") );
    m_actionClearAllHistory->setText( theLocales.getLocalText("search-bar.options-menu.clear-all-history.text","Clear All History") );
    m_actionClearAllHistory->setToolTip( theLocales.getLocalText("search-bar.options-menu.clear-all-history.tooltip","") );
    m_actionClearChromeCache->setText( theLocales.getLocalText("search-bar.options-menu.clear-chrome-cache.text","Clear Chrome Cache") );
    m_actionClearChromeCache->setToolTip( theLocales.getLocalText("search-bar.options-menu.clear-chrome-cache.tooltip","") );

}

void EbWidgetSearchBar::changeUrl(const QString &url, bool searchFucus, bool searchSelectAll)
{
    m_canSearch = false;
    m_lineEditSearch->setText(url);
    if (searchFucus) {
        m_lineEditSearch->setFocus();
    }
    if (searchSelectAll) {
        m_lineEditSearch->selectAll();
    }
    m_canSearch = true;
}

void EbWidgetSearchBar::updateLoadState(bool loadFinished, bool canGoBack, bool canGoForward)
{
    m_buttonBack->setEnabled(canGoBack);
    m_buttonForward->setEnabled(canGoForward);
    m_buttonRefresh->setVisible(loadFinished);
    m_buttonStop->setVisible(!loadFinished);
}

void EbWidgetSearchBar::onMoveEvent()
{
//    if (m_widgetSearchResult!=0) {
//        const int x = m_lineEditSearch->geometry().left();
//        const int y = m_lineEditSearch->geometry().bottom()+1;
//        const QPoint pos = this->mapToGlobal(QPoint(x,y));
//        const int searchResultHeight = m_widgetSearchResult->height();
//        m_widgetSearchResult->setGeometry( pos.x(),pos.y(),m_lineEditSearch->geometry().width(),searchResultHeight );
//    }

}

//const QRect &EbWidgetSearchBar::searchEditRect()
//{
//    return m_lineEditSearch->geometry();
//}

void EbWidgetSearchBar::onClickedButtonBack()
{
    emit clickedBack();
}

void EbWidgetSearchBar::onClickedButtonForward()
{
    emit clickedForward();
}

void EbWidgetSearchBar::onClickedButtonRefresh()
{
    emit clickedRefresh();
}

void EbWidgetSearchBar::onClickedButtonStop()
{
    emit clickedStop();
}

void EbWidgetSearchBar::onClickedButtonOptions()
{
    const bool enable = theApp->m_sqliteUser->select("SELECT url FROM url_record_t LIMIT 1;")==1?true:false;
    m_actionClearAllHistory->setEnabled(enable);

    const QPoint pos(0,m_buttonOptions->geometry().height());
    m_menuOptions->exec(m_buttonOptions->mapToGlobal(pos));
}

void EbWidgetSearchBar::onClickedMenuOptions()
{
    const QAction * action = (QAction*)sender();
    const EB_COMMAND_ID commandId = (EB_COMMAND_ID)action->data().toInt();
    switch (commandId)
    {
    case EB_COMMAND_SAVE_HISTORY:
        emit saveUrlHistory();
        break;
    case EB_COMMAND_CLEAR_HISTORY: {
        /// 清除本地浏览记录
        const QString title = theLocales.getLocalText("message-box.clear-all-history.title","Clear All History");
        const QString text = theLocales.getLocalText("message-box.clear-all-history.text","Confirm clear all history?");
        const int ret = EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION );
        if (ret==QDialog::Accepted) {
            theApp->m_sqliteUser->execute("DELETE FROM url_record_t;");
        }
        break;
    }
    case EB_COMMAND_CLEAR_CHROME_TEMPFILE: {
        /// 清除 chrome 缓存文件
        const QString title = theLocales.getLocalText("message-box.clear-chrome-cache.title","Clear Chrome Cache");
        QString text = theLocales.getLocalText("message-box.clear-chrome-cache.text","Confirm clear chrome <br>cache files?");
        const int ret = EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION );
        if (ret==QDialog::Accepted) {
            QWebEngineProfile::defaultProfile()->clearHttpCache();
            text = theLocales.getLocalText("message-show.clear-chrome-cache-ok","Clear Ok");
            EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_INFORMATION,default_information_auto_close );
        }
        break;
    }
    default:
        break;
    }
    this->m_lineEditSearch->setFocus();

}

void EbWidgetSearchBar::onSearchEditTextChange(const QString &text)
{
    if (m_canSearch) {
        m_widgetSearchResult->search(text.toStdString().c_str());
    }
}

void EbWidgetSearchBar::onSearchEditKeyPressEsc()
{
    if (m_widgetSearchResult->isVisible()) {
        m_widgetSearchResult->setVisible(false);
    }
}

void EbWidgetSearchBar::onSearchEditKeyPressEnter(const QString &text)
{
    if ( !text.isEmpty() ) {
        emit searchKeyPressEnter(text);
    }
    if (m_widgetSearchResult->isVisible()) {
        m_widgetSearchResult->hide();
    }
}

void EbWidgetSearchBar::onSearchEditKeyPressDown()
{
    m_widgetSearchResult->setFocusSelectFirst();
}

void EbWidgetSearchBar::onSearchFirst(const QString &url)
{
    if ( !m_canUpdateSearchFirst ) {
        return;
    }
    const QString text = m_lineEditSearch->text();
    if (text.isEmpty()) {
        return;
    }
    const int index = url.indexOf(text);
    if (index<0) {
        return;
    }
    /// 取后面文本用于显示
    const QString newText = url.mid(index);
    m_lineEditSearch->setText(newText);
    /// 选择后面未输入内容
    m_lineEditSearch->setSelection(text.length(),newText.length());
}

void EbWidgetSearchBar::onClickedSearchResultUrl(const QString &url)
{
    this->m_lineEditSearch->setText(url);
    this->m_lineEditSearch->selectAll();
}

void EbWidgetSearchBar::onListResultsKeyPressFirstItemUp()
{
    this->m_lineEditSearch->setFocus();
}

void EbWidgetSearchBar::onListResultsKeyPressEsc()
{
    this->m_lineEditSearch->setFocus();
    m_widgetSearchResult->hide();
}

void EbWidgetSearchBar::resizeEvent(QResizeEvent *event)
{
    int x = 1;
    int y = 3;
    m_buttonBack->move( x,y );
    x += const_button_size.width();
    m_buttonForward->move( x,y );
    x += const_button_size.width();
    m_buttonRefresh->move( x,y );
    m_buttonStop->move( x,y );
    x += const_button_size.width();
    const int const_edit_width = width()-x-const_button_size.width()-1;
    m_lineEditSearch->setGeometry( x,y,const_edit_width,22 );
    x = width()-const_button_size.width()-1;
    m_buttonOptions->move( x,y );

    x = m_lineEditSearch->geometry().left();
    y = m_lineEditSearch->geometry().bottom()+1;
    QPoint pos = this->mapToParent(QPoint(x,y));
    const int searchResultHeight = m_widgetSearchResult->height();
    m_widgetSearchResult->setGeometry( pos.x(),pos.y(),m_lineEditSearch->width(),searchResultHeight );
//    onMoveEvent();
}

//bool EbWidgetSearchBar::event(QEvent *e)
//{
//    if (e->type()== QEvent::Move) {
//        int i=0;
//    }
//    return QWidget::event(e);
//}

bool EbWidgetSearchBar::eventFilter(QObject *watched, QEvent *e)
{
    if ( watched==m_lineEditSearch && e->type()==QEvent::KeyPress ) {
        const QKeyEvent * event = (QKeyEvent*)e;
        switch (event->key()) {
        case Qt::Key_Backspace:
        case Qt::Key_Delete:
            m_canUpdateSearchFirst = false;
            break;
        default:
            m_canUpdateSearchFirst = true;
            break;
        }
    }
    return QWidget::eventFilter(watched,e);
}

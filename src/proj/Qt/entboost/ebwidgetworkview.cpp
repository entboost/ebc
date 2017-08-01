#include "ebwidgetworkview.h"
#include "ebwebengineview.h"
#include "ebwebenginepage.h"
#include <QAction>
#include <QWebEngineProfile>
#include <ebclientapp.h>
#include <ebframefindtext.h>
#include <ebdialogmainframe.h>

//template<typename Arg, typename R, typename C>
//struct InvokeWrapper {
//    R *receiver;
//    void (C::*memberFun)(Arg);
//    void operator()(Arg result) {
//        (receiver->*memberFun)(result);
//    }
//};

//template<typename Arg, typename R, typename C>
//InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
//{
//    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
//    return wrapper;
//}

EbWidgetWorkView::EbWidgetWorkView(bool saveUrl,const QUrl & url, const QString &postData, QWidget *parent) : QWidget(parent)
  , m_saveUrl(saveUrl), m_savedUrl(false)
  , m_url(url)
  , m_postData(postData)
  , m_openLinkInNewTabAddUrlEnable(false)
  , m_zoomFactor(1.0)
  , m_timerIdCheckZoom(0)
  , m_loadState(LOAD_UNKNOWN)
  , m_timerIdHideStatus(0)
{
    m_webEngineView = new EbWebEngineView(this);
    m_webEngineView->load(m_url);
    m_webEngineView->show();
    init();
}

EbWidgetWorkView::EbWidgetWorkView(const QString &html, QWidget *parent) : QWidget(parent)
  , m_saveUrl(false), m_savedUrl(false)
  , m_html(html)
  , m_openLinkInNewTabAddUrlEnable(false)
  , m_zoomFactor(1.0)
  , m_timerIdCheckZoom(0)
  , m_loadState(LOAD_UNKNOWN)
  , m_timerIdHideStatus(0)
{
    m_webEngineView = new EbWebEngineView(this);
//    m_webEngineView->setStyleSheet("background-color:rgb(0,0,0,0);");
    if (!m_html.isEmpty()) {
        m_webEngineView->setHtml(m_html);
    }
    m_webEngineView->show();
    init();
}

EbWidgetWorkView::~EbWidgetWorkView()
{
    if ( this->isLoading() )
        m_webEngineView->stop();
    delete m_webEngineView; /// ?
}

EbWidgetWorkView::pointer EbWidgetWorkView::create(bool saveUrl, const QUrl &url, const QString &postData, QWidget *parent)
{
    return EbWidgetWorkView::pointer(new EbWidgetWorkView(saveUrl, url, postData, parent));
}

EbWidgetWorkView::pointer EbWidgetWorkView::create(const QString &html, QWidget *parent)
{
    return EbWidgetWorkView::pointer(new EbWidgetWorkView(html, parent));
}

void EbWidgetWorkView::refreshOrStop()
{
    if ( this->isLoading() ) {
        m_webEngineView->stop();
    }
    else if ( isLoadFinished() ) {
        m_webEngineView->reload();
    }
}

void EbWidgetWorkView::saveUrlHistory()
{
    if ( !m_savedUrl ) {
        tstring url( m_webEngineView->url().url().toStdString() );
        tstring title( m_webEngineView->title().toStdString() );
        if( url.empty() || url=="about:blank" || title.empty()) {
            return;
        }
        m_savedUrl = true;
        CSqliteCdbc::escape_string_in(url);
        CSqliteCdbc::escape_string_in(title);
        char sql[3048];
        sprintf(sql, "UPDATE url_record_t SET title='%s',last_time=datetime('now') WHERE url='%s'",title.c_str(),url.c_str());
        if (theApp->m_sqliteUser->execute(sql)<1) {
            sprintf(sql, "INSERT INTO url_record_t(url,title) VALUES('%s','%s')",url.c_str(),title.c_str());
            theApp->m_sqliteUser->execute(sql);
        }
    }
}

bool EbWidgetWorkView::isLoading() const
{
    return LOAD_STARTED==m_loadState?true:false;
}

bool EbWidgetWorkView::isLoadFinished() const
{
    return LOAD_FINISHED==m_loadState?true:false;
}

void EbWidgetWorkView::setFocusInput()
{
    m_webEngineView->setFocus();
    /// ?
    if ( this->isVisible() ) {
        emit urlChanged( this,m_webEngineView->url() );
    }
}

void EbWidgetWorkView::setSaveUrl(bool v)
{
    m_saveUrl = v;
//    m_savedUrl = false;
}

void EbWidgetWorkView::onFindAction(const QString &selectedText)
{
    m_frameFindText->showFocusInput(selectedText);
}

void EbWidgetWorkView::onLoadStarted()
{
    m_loadState = LOAD_STARTED;
    if ( this->isVisible() ) {
        emit loadStateChange(this,false,m_webEngineView->history()->canGoBack(),m_webEngineView->history()->canGoForward());
    }
}

void EbWidgetWorkView::setHtml(const QString &html)
{
    m_html = html;
    m_webEngineView->setHtml(m_html);
}

void EbWidgetWorkView::load(const QUrl &url, const QString &postData)
{
    m_url = url;
    m_postData = postData;
    m_webEngineView->load(m_url);
}

void EbWidgetWorkView::back()
{
    m_webEngineView->back();
}

void EbWidgetWorkView::forward()
{
    m_webEngineView->forward();
}

bool EbWidgetWorkView::canGoBack() const
{
    return m_webEngineView->history()->canGoBack();
}

bool EbWidgetWorkView::canGoForward() const
{
    return m_webEngineView->history()->canGoForward();
}

void EbWidgetWorkView::onLoadFinished(bool)
{
    m_loadState = LOAD_FINISHED;
    if ( m_saveUrl ) {
        /// 本地保存浏览记录
        saveUrlHistory();
    }

    if ( this->isVisible() ) {
        emit loadStateChange(this,true,m_webEngineView->history()->canGoBack(),m_webEngineView->history()->canGoForward());
    }
}

void EbWidgetWorkView::onTitleChanged(const QString &title)
{
    emit titleChanged(this,title);
}

void EbWidgetWorkView::onUrlChanged(const QUrl &url)
{
    if ( this->isVisible() ) {
        emit urlChanged(this,url);
    }
}

void EbWidgetWorkView::onIconChanged(const QIcon &icon)
{
    if ( !icon.isNull() ) {
        const QString iconFilePath = theApp->urlIconFilePath(m_url);
        if (!QFile::exists(iconFilePath)) {
            icon.pixmap(32,32).save(iconFilePath);
        }
    }
    emit iconChanged(this,icon);
}

void EbWidgetWorkView::onLinkHovered(const QString &url)
{
    if (m_labelStatus==0) {
        return;
    }
    if (m_timerIdHideStatus!=0) {
        this->killTimer(m_timerIdHideStatus);
        m_timerIdHideStatus = 0;
    }
    if (url.isEmpty()) {
        m_labelStatus->hide();
    }
    else {
        QRect rect = m_labelStatus->geometry();
        rect.setWidth( m_labelStatus->fontMetrics().width(url)+10 );
        m_labelStatus->setText(url);
        m_labelStatus->setGeometry(rect);
        m_labelStatus->show();
        /// 显示 6 秒后，自动隐藏
        m_timerIdHideStatus = this->startTimer(6*1000);
    }
}

void EbWidgetWorkView::onWindowCloseRequested()
{
    emit windowCloseRequested(this);
}

void EbWidgetWorkView::onOpenLink(const EbWebEngineView *, QWebEnginePage::WebWindowType type, const QUrl &linkUrl)
{
    switch (type)
    {
    case QWebEnginePage::WebBrowserTab: {
        if (m_openLinkInNewTabAddUrlEnable) {
            const QString url = linkUrl.url();
            if ( !url.isEmpty() ) {
                theApp->mainWnd()->openUrl(false,url);
            }
        }
        emit openLinkInNewTab(this,linkUrl);
        break;
    }
    default:
        QDesktopServices::openUrl( linkUrl );
        break;
    }

}

//void EbWidgetWorkView::handleFindTextResult(bool found)
//{
//    int i=0;
//}

void EbWidgetWorkView::onFindText(bool back, const QString &text)
{
    QWebEnginePage::FindFlags options = QWebEnginePage::FindCaseSensitively;
    if (back) {
        options |= QWebEnginePage::FindBackward;
    }
    m_webEngineView->findText(text,options);
//    m_webEngineView->findText(text,options,invoke(this, &EbWidgetWorkView::handleFindTextResult));
}

void EbWidgetWorkView::onExitFindText()
{
    m_webEngineView->findText("");
}

bool EbWidgetWorkView::eventFilter(QObject *obj, QEvent *ev)
{
    if ( obj==this->m_webEngineView && ev->type()==QEvent::KeyPress ) {
         const QKeyEvent *event = (QKeyEvent*)ev;
         if (event->key()==Qt::Key_F && (event->modifiers() & Qt::ControlModifier)) {
             int i=0;
         }
    }

    return QWidget::eventFilter(obj,ev);
}

void EbWidgetWorkView::keyPressEvent(QKeyEvent *e)
{
    if (e->key()==Qt::Key_F && (e->modifiers()&Qt::ControlModifier)!=0 ) {
        m_frameFindText->show();
        m_frameFindText->raise();
    }
    QWidget::keyPressEvent(e);
}

void EbWidgetWorkView::resizeEvent(QResizeEvent *event)
{
    m_webEngineView->setGeometry( 0,0,width(),height() );
    const int w = m_labelStatus->width();
    const int const_status_height = 22;
    m_labelStatus->setGeometry( 0,height()-const_status_height,w,const_status_height );

    const int x = MAX(0,width()-DEFAULT_PANEL_FIND_WIDTH-DEFAULT_PANEL_FIND_RIGHT);
    m_frameFindText->setGeometry( x,-1,DEFAULT_PANEL_FIND_WIDTH,DEFAULT_PANEL_FIND_HEIGHT );

    QWidget::resizeEvent(event);
}

void EbWidgetWorkView::timerEvent(QTimerEvent *e)
{
    if (m_timerIdHideStatus!=0 && m_timerIdHideStatus==e->timerId()) {
        this->killTimer(m_timerIdHideStatus);
        m_timerIdHideStatus = 0;
        m_labelStatus->hide();
    }
    else if (m_timerIdCheckZoom!=0 && m_timerIdCheckZoom==e->timerId()) {
        this->killTimer(m_timerIdCheckZoom);
        m_timerIdCheckZoom = 0;
        if (m_webEngineView->zoomFactor()!=m_zoomFactor) {
            m_webEngineView->setZoomFactor(m_zoomFactor);
            showZoomFactor();
        }
    }
    QWidget::timerEvent(e);
}

void EbWidgetWorkView::showZoomFactor()
{
    const qreal zoomFactor = m_zoomFactor*100.0+0.9;    /// +0.9，避免出现 99.9999 显示 99%问题
    const QString text = QString("%1: %2%").arg(theLocales.getLocalText("web-engine-view.zoom","Zoom")).arg((int)zoomFactor);
    onLinkHovered(text);
}

void EbWidgetWorkView::init()
{
    m_webEngineView->installEventFilter(this);
    connect( m_webEngineView,SIGNAL(findAction(QString)),this,SLOT(onFindAction(QString)) );
    connect( m_webEngineView,SIGNAL(loadStarted()),this,SLOT(onLoadStarted()) );
    connect( m_webEngineView,SIGNAL(loadFinished(bool)),this,SLOT(onLoadFinished(bool)) );
    connect( m_webEngineView,SIGNAL(titleChanged(QString)),this,SLOT(onTitleChanged(QString)) );
    connect( m_webEngineView,SIGNAL(urlChanged(QUrl)),this,SLOT(onUrlChanged(QUrl)) );
    connect( m_webEngineView,SIGNAL(iconChanged(QIcon)),this,SLOT(onIconChanged(QIcon)) );
    connect( m_webEngineView,SIGNAL(openLink(const EbWebEngineView*,QWebEnginePage::WebWindowType,QUrl)),this,SLOT(onOpenLink(const EbWebEngineView*,QWebEnginePage::WebWindowType,QUrl)) );
    connect( (QWebEnginePage*)m_webEngineView->page(),SIGNAL(windowCloseRequested()),this,SLOT(onWindowCloseRequested()) );
    connect( (QWebEnginePage*)m_webEngineView->page(),SIGNAL(linkHovered(QString)),this,SLOT(onLinkHovered(QString)) );
//    m_webEngineUrlRequestInterceptor = new EbWebEngineUrlRequestInterceptor(this);
//    m_webEngineView->page()->profile()->setRequestInterceptor(m_webEngineUrlRequestInterceptor);
    m_labelStatus = new QLabel(this);
    m_labelStatus->setVisible(false);
    m_labelStatus->setStyleSheet("QLabel{background-color:rgb(224,224,224);color:rgb(96,96,96);padding-left:2;padding-right:2;}");
//    m_labelStatus->setObjectName("LabelStatus");
    m_frameFindText = new EbFrameFindText(this);
    m_frameFindText->setVisible(false);
    connect( m_frameFindText,SIGNAL(findText(bool,QString)),this, SLOT(onFindText(bool,QString)) );
    connect( m_frameFindText,SIGNAL(exitFindText()),this,SLOT(onExitFindText()) );
}

#ifndef QT_NO_WHEELEVENT
/// 0.25 to 5.0. The default factor is 1.0.
#define EB_DEFAULT_MIN_ZOOM_FACTOR			0.3 // 0.25
#define EB_DEFAULT_MAX_ZOOM_FACTOR			5.0
#define EB_DEFAULT_ZOOM_OFFSET1             0.1
#define EB_DEFAULT_ZOOM_OFFSET2             0.25
void EbWidgetWorkView::wheelEvent(QWheelEvent *event)
{
    if ( (event->modifiers()&Qt::ControlModifier)!=0 ) {
        const QString url = m_webEngineView->url().url();
        if ( url.isEmpty() || url=="about:blank" ) {
            return;
        }
        const int delta = event->delta();
        if (delta>=(int)QWheelEvent::DefaultDeltasPerStep) {
            /// 向上滚动
            m_zoomFactor = MIN(EB_DEFAULT_MAX_ZOOM_FACTOR,m_zoomFactor+(m_zoomFactor<1.5?EB_DEFAULT_ZOOM_OFFSET1:EB_DEFAULT_ZOOM_OFFSET2));
        }
        else if (delta<=(int)(QWheelEvent::DefaultDeltasPerStep*-1)) {
            /// 向下滚动
            m_zoomFactor = MAX(EB_DEFAULT_MIN_ZOOM_FACTOR,m_zoomFactor-(m_zoomFactor<=1.5?EB_DEFAULT_ZOOM_OFFSET1:EB_DEFAULT_ZOOM_OFFSET2));
        }
        if(m_timerIdCheckZoom!=0) {
            this->killTimer(m_timerIdCheckZoom);
        }
        m_timerIdCheckZoom = this->startTimer(500);
        return;
    }
    QWidget::wheelEvent(event);
}
#endif

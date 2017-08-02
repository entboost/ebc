#include "ebwebengineview.h"
#include <QAction>
#include <QContextMenuEvent>
#include <QWebEngineContextMenuData>
#include <QWebEngineHistory>
#include <QWebEngineProfile>
#include <ebclientapp.h>
#include <ebwebenginepage.h>

const QWebEnginePage::WebAction EB_WEB_ACTION_PRINT = (QWebEnginePage::WebAction)(QWebEnginePage::WebActionCount+10);
const QWebEnginePage::WebAction EB_WEB_ACTION_FIND = (QWebEnginePage::WebAction)(QWebEnginePage::WebActionCount+11);
//const QWebEnginePage::WebAction EB_WEB_ACTION_PRINT_TO_PDF = QWebEnginePage::WebActionCount+11;

EbWebEngineView::EbWebEngineView(QWidget* parent)
    : QWebEngineView(parent)
    , m_menu(0)
{
    EbWebEnginePage * pPage = new EbWebEnginePage(this);
    this->setPage( pPage );
    connect( (QWebEnginePage*)pPage,SIGNAL(linkHovered(QString)),this,SLOT(onLinkHovered(QString)) );

    /// 没用
//    QAction * action = this->pageAction( QWebEnginePage::Back );
//    action->setText("返回1");

    /// JS 调用C++
//    QWebChannel *channel = new QWebChannel(this);
//    channel->registerObject("bridge", (QObject*)bridge::instance());
    //    m_view->page()->setWebChannel(channel);
}

EbWebEngineView::~EbWebEngineView()
{
    m_actions.clear();
}

EbWebEnginePage *EbWebEngineView::page() const
{
    return (EbWebEnginePage*)QWebEngineView::page();
}

QWebEngineView *EbWebEngineView::createWindow(QWebEnginePage::WebWindowType type)
{
    if ( !m_linkHoveredUrl.isEmpty() ) {
        emit openLink( this,type,QUrl(m_linkHoveredUrl) );
//        emit openLinkInNewTab( this, QUrl(m_linkHoveredUrl) );
    }

//    const QWebEngineContextMenuData & pContextMenuData = page()->contextMenuData();
//    if ( pContextMenuData.isValid() ) {
//        /// linkUrl
//        //    const QString sLinkText = pContextMenuData.linkText();
//        const QUrl linkUrl = pContextMenuData.linkUrl();
//        if ( linkUrl.isValid() ) {
//        }
//    }
//    QWebEngineView * result = QWebEngineView::createWindow(type);
//    const QString url = this->url().url();
//    const QString requestedUrl = page()->requestedUrl().url();
    return 0x0;//new QWebEngineView(this->parentWidget());
}

void EbWebEngineView::contextMenuEvent(QContextMenuEvent * e)
{
    const QString title = this->title();
    if ( title.isEmpty() || title=="about:blank") {
        return;
    }
    createMenuData();
    hideAllAction();

    const QWebEngineContextMenuData & pContextMenuData = page()->contextMenuData();
    if ( pContextMenuData.isValid() ) {
        /// selectedText
        const QString sSelectedText = pContextMenuData.selectedText();
        if ( !sSelectedText.isEmpty() ) {
            setActionVisible( QWebEnginePage::Copy,true );
        }
        /// linkUrl
        //    const QString sLinkText = pContextMenuData.linkText();
        const QUrl linkUrl = pContextMenuData.linkUrl();
        if ( linkUrl.isValid() ) {
            setActionVisible( QWebEnginePage::OpenLinkInNewTab,true );
            setActionVisible( QWebEnginePage::OpenLinkInNewWindow,true );
            setActionVisible( QWebEnginePage::DownloadLinkToDisk,true );
            setActionVisible( QWebEnginePage::CopyLinkToClipboard,true );
        }
        /// mediaUrl
        const QUrl mediaUrl = pContextMenuData.mediaUrl();
        if ( mediaUrl.isValid() ) {
            switch ( pContextMenuData.mediaType() )
            {
            case QWebEngineContextMenuData::MediaTypeImage: {
                setActionVisible( QWebEnginePage::DownloadImageToDisk,true );
                setActionVisible( QWebEnginePage::CopyImageToClipboard,true );
                setActionVisible( QWebEnginePage::CopyImageUrlToClipboard,true );
                break;
            }
            case QWebEngineContextMenuData::MediaTypeVideo:
            case QWebEngineContextMenuData::MediaTypeAudio:
            case QWebEngineContextMenuData::MediaTypeCanvas:
            case QWebEngineContextMenuData::MediaTypeFile:
            case QWebEngineContextMenuData::MediaTypePlugin:
                break;
            default:
                break;
            }
        }
    }
    /// Back & Forward
    setActionVisible( QWebEnginePage::Back,true,history()->canGoBack() );
    setActionVisible( QWebEnginePage::Forward,true,history()->canGoForward() );
    /// SavePage & print
    setActionVisible( QWebEnginePage::SavePage,true );
    setActionVisible( EB_WEB_ACTION_PRINT,true );
    /// ViewSource & InspectElement
    setActionVisible( EB_WEB_ACTION_FIND,true );
    setActionVisible( QWebEnginePage::ViewSource,true );
    setActionVisible( QWebEnginePage::InspectElement,true );

    m_menu->popup(e->globalPos());
    //    QWebEngineView::contextMenuEvent(e);
}

void EbWebEngineView::onTriggeredActionOpenLinkInNewTab()
{
    const QWebEngineContextMenuData & pContextMenuData = page()->contextMenuData();
    const QUrl linkUrl = pContextMenuData.linkUrl();
//        const QString scheme = linkUrl.scheme();  /// http
    if ( linkUrl.isValid() ) {
        emit openLink(this,QWebEnginePage::WebBrowserTab,linkUrl);
    }
}

void EbWebEngineView::onTriggeredActionOpenLinkInNewWindow()
{
    const QWebEngineContextMenuData & pContextMenuData = page()->contextMenuData();
    const QUrl linkUrl = pContextMenuData.linkUrl();
    if ( linkUrl.isValid() ) {
        QDesktopServices::openUrl( linkUrl );
    }
}

void EbWebEngineView::onTriggeredActionFind()
{
    emit findAction( this->selectedText() );
}

void EbWebEngineView::onTriggeredActionPrint()
{

}

void EbWebEngineView::onTriggeredActionInspectElement()
{

}

void EbWebEngineView::onTriggeredActionPageTriggerAction()
{
//    const QWebEngineContextMenuData & pContextMenuData = page()->contextMenuData();
    const QAction * action = (QAction*)sender();
    const QWebEnginePage::WebAction webAction = (QWebEnginePage::WebAction)action->data().toInt();
    this->page()->triggerAction( webAction );
}

void EbWebEngineView::onLinkHovered(const QString &url)
{
    m_linkHoveredUrl = url;
}

//bool EbWebEngineView::event(QEvent *e)
//{
//    if ( e->type()==QEvent::KeyPress ) {
//         const QKeyEvent *event = (QKeyEvent*)e;
//         if ( event->key()==Qt::Key_F && (event->modifiers()&Qt::ControlModifier)!=0 ) {
//             int i=0;
//         }
//    }

//    return QWebEngineView::event(e);
//}

//void EbWebEngineView::keyPressEvent(QKeyEvent *e)
//{
//    if (e->key()==Qt::Key_F && QApplication::keyboardModifiers ()==Qt::ControlModifier ) {
//        int i=0;
//    }
//    QWebEngineView::keyPressEvent(e);
//}

void EbWebEngineView::createMenuData()
{
    if (m_menu==0) {
        m_menu = new QMenu(this);

//        QAction * action = m_menu->addAction("打开链接");
//        action->setData(QVariant( (int)webAction) );
//        m_actions.insert( QWebEnginePage::OpenLinkInThisWindow,action,false );

        /////////// linkUrl
        /// 新标签页打开链接
        QAction * action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.open-link-in-new-tab.text","Open Link In New Tab") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionOpenLinkInNewTab()) );
        m_actions.insert( QWebEnginePage::OpenLinkInNewTab,action,false );
        /// 新窗口打开链接
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.open-link-in-new-window.text","Open Link In New Window") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionOpenLinkInNewWindow()) );
        m_actions.insert( QWebEnginePage::OpenLinkInNewWindow,action,false );
        ///----------------------------------
        m_menu->addSeparator();
        /// 链接另存为
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.download-link.text","Download Link") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        QWebEnginePage::WebAction webAction = QWebEnginePage::DownloadLinkToDisk;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( QWebEnginePage::DownloadLinkToDisk,action,false );
        /// 复制链接地址
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.copy-link.text","Copy Link") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::CopyLinkToClipboard;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        ///----------------------------------
        m_menu->addSeparator();
        /// 图片另存为
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.download-image.text","Download Image") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::DownloadImageToDisk;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        /// 复制图片
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.copy-image.text","Copy Image") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::CopyImageToClipboard;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        /// 复制图片地址
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.copy-image-link.text","Copy Image Link") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::CopyImageUrlToClipboard;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );

        ///----------------------------------
        m_menu->addSeparator();
        /// Copy 复制
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.copy.text","Copy") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::Copy;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        /// Find 查找
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.find.text","Find") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionFind()) );
        m_actions.insert( EB_WEB_ACTION_FIND,action,false );
        ///----------------------------------
        m_menu->addSeparator();
        /// Back 返回
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.back.text","Back") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::Back;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        /// Forward 前进
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.forward.text","Forward") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::Forward;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        /// Stop 停止加载
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.stop.text","Stop") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::Stop;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        /// Reload 重新加载
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.reload.text","Reload") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::Reload;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        ///----------------------------------
        m_menu->addSeparator();
        /// 另存为
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.saveas.text","Save As") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::SavePage;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        /// 打印
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.print.text","Print") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPrint()) );
        m_actions.insert( EB_WEB_ACTION_PRINT,action,false );
        ///----------------------------------
        m_menu->addSeparator();
        /// 查看源代码
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.view-source.text","View Source") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionPageTriggerAction()) );
        webAction = QWebEnginePage::ViewSource;
        action->setData(QVariant( (int)webAction) );
        m_actions.insert( webAction,action,false );
        /// 审查元素
        action = m_menu->addAction( theLocales.getLocalText("web-engine-view.context-menu.inspect-element.text","Inspect Element") );
        connect( action,SIGNAL(triggered()),this,SLOT(onTriggeredActionInspectElement()) );
        m_actions.insert( QWebEnginePage::InspectElement,action,false );

    }

}

void EbWebEngineView::hideAllAction()
{
    BoostReadLock rdLock(m_actions.mutex());
    CLockMap<QWebEnginePage::WebAction,QAction*>::iterator iter = m_actions.begin();
    for ( ; iter!=m_actions.end(); iter++ ) {
        QAction * action = iter->second;
        if ( action->isSeparator() ) {
            continue;
        }
        action->setVisible(false);
    }

}

void EbWebEngineView::setActionVisible(QWebEnginePage::WebAction webAction, bool visible, bool enable)
{
    QAction * action = 0;
    if ( m_actions.find(webAction,action) ) {
        action->setVisible(visible);
        action->setEnabled(enable);
    }
}

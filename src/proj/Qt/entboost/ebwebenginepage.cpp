#include "ebwebenginepage.h"
#include <QWebEngineProfile>

EbWebEnginePage::EbWebEnginePage(QObject *parent)
    : QWebEnginePage(parent)
{
}

//QStringList EbWebEnginePage::chooseFiles(QWebEnginePage::FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes)
//{
//    /// 另存为... 没有反映
//    return QStringList("abc.htm");
//}

//bool EbWebEnginePage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
//{
//    return true;
//}

//bool EbWebEnginePage::event(QEvent * e)
//{
//    if ( e->type()==QEvent::KeyPress ) {
//        const QKeyEvent *event = (QKeyEvent*)e;
//        if ( event->key()==Qt::Key_F && (event->modifiers()&Qt::ControlModifier)!=0 ) {
//            int i=0;
//        }
//    }

//    return QWebEnginePage::event(e);
//}

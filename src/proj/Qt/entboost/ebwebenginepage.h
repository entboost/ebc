#ifndef EBWEBENGINEPAGE_H
#define EBWEBENGINEPAGE_H

#include <QWebEnginePage>
#include <QEvent>
#include <QKeyEvent>

class EbWebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    EbWebEnginePage(QObject *parent = 0);

protected:
//    virtual QStringList chooseFiles(FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes);
//    virtual bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
//    virtual bool event(QEvent *);
};

#endif // EBWEBENGINEPAGE_H

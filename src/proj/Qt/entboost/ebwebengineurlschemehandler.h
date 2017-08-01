#ifndef EBWEBENGINEURLSCHEMEHANDLER_H
#define EBWEBENGINEURLSCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>
#include <ebdefines.h>
//#include <QSoundEffect>

class EbWebEngineUrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT
public:
    static EbWebEngineUrlSchemeHandler* instance(void);
    EbWebEngineUrlSchemeHandler(QObject *parent=0);

    void requestUrl(const QUrl &url, QWebEngineUrlRequestJob *job=0);
    bool openSubId(const QString &info, QWebEngineUrlRequestJob *job=0);
protected:
    virtual void requestStarted(QWebEngineUrlRequestJob *job);
private:
    static EbWebEngineUrlSchemeHandler *_instance;
//    QSoundEffect m_sounrEffect;
};

#endif // EBWEBENGINEURLSCHEMEHANDLER_H

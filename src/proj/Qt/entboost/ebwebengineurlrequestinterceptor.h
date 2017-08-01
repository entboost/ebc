#ifndef EBWEBENGINEURLREQUESTINTERCEPTOR_H
#define EBWEBENGINEURLREQUESTINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>

class EbWebEngineUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT
public:
    EbWebEngineUrlRequestInterceptor(QObject * p=0);

protected:
    virtual void interceptRequest(QWebEngineUrlRequestInfo &info);
};

#endif // EBWEBENGINEURLREQUESTINTERCEPTOR_H

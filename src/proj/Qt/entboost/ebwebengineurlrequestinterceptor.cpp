#include "ebwebengineurlrequestinterceptor.h"

EbWebEngineUrlRequestInterceptor::EbWebEngineUrlRequestInterceptor(QObject *p)
    : QWebEngineUrlRequestInterceptor(p)
{
}

void EbWebEngineUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QWebEngineUrlRequestInfo::ResourceType resourceType = info.resourceType();
    const QWebEngineUrlRequestInfo::NavigationType navigationType = info.navigationType();
    const QUrl requestUrl = info.requestUrl();
    const QString url = requestUrl.url();
    const QUrl firstPartyUrl = info.firstPartyUrl();

    int i=0;
}

#ifndef EBWEBENGINEDOWNLOADREQUESTED_H
#define EBWEBENGINEDOWNLOADREQUESTED_H

#include <QObject>
#include <stl/lockmap.h>
#include <ebwebenginedownloaditem.h>

class EbWebEngineDownloadRequested : public QObject
{
    Q_OBJECT
public:
    explicit EbWebEngineDownloadRequested(QObject *parent = nullptr);
    virtual ~EbWebEngineDownloadRequested(void);

signals:

public slots:
    void onDownloadRequested(QWebEngineDownloadItem *item);
    void onDownloadFinished(const EbWebEngineDownloadItem *item);
//    void onDownLoadStateChanged(QWebEngineDownloadItem::DownloadState state);
//    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
private:
    CLockMap<quint32,EbWebEngineDownloadItem::pointer> m_downloads;
};

#endif // EBWEBENGINEDOWNLOADREQUESTED_H

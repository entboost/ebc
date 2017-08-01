#ifndef EBWEBENGINEDOWNLOADITEM_H
#define EBWEBENGINEDOWNLOADITEM_H

#include <boost/shared_ptr.hpp>
#include <QWebEngineDownloadItem>
#include <QUrl>

class EbWebEngineDownloadItem : public QObject
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<EbWebEngineDownloadItem> pointer;
    EbWebEngineDownloadItem(QWebEngineDownloadItem *download, QObject *parent);
    static EbWebEngineDownloadItem::pointer create(QWebEngineDownloadItem *download, QObject *parent=0);

    qint64 msgId(void) const {return m_msgId;}
    quint32 downloadId(void) const {return m_downloadId;}
//    void setState(QWebEngineDownloadItem::DownloadState state) {m_state=state;}
    QWebEngineDownloadItem::DownloadState state(void) const {return m_state;}
//    void setTotalBytes(qint64 v) {m_totalBytes=v;}
    qint64 totalBytes(void) const {return m_totalBytes;}
//    void setReceivedBytes(qint64 v) {m_receivedBytes=v;}
    qint64 receivedBytes(void) const {return m_receivedBytes;}
    const QUrl &url(void) const {return m_url;}
    const QString &mimeType(void) const {return m_mimeType;}
//    void setPath(const QString &v) {m_path=v;}
    const QString &path(void) const {return m_path;}
    bool isFinished(void) const {return m_state==QWebEngineDownloadItem::DownloadCompleted?true:false;}
signals:
//    void downloadFinished(const EbWebEngineDownloadItem*);
////    void downLoadStateChanged(const EbWebEngineDownloadItem*);
//    void downloadProgress(const EbWebEngineDownloadItem*);
public slots:
    void onDownloadFinished(void);
    void onDownLoadStateChanged(QWebEngineDownloadItem::DownloadState state);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
private:
    QWebEngineDownloadItem * m_downloadItem;
    qint64 m_msgId;
    quint32 m_downloadId;
    QWebEngineDownloadItem::DownloadState m_state;
    qint64 m_totalBytes;
    qint64 m_receivedBytes;
    QUrl m_url;
    QString m_mimeType;
    QString m_path;
//    bool m_sendReceiving;
    time_t m_tCreateTime;
};

#endif // EBWEBENGINEDOWNLOADITEM_H

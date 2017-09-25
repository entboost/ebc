#ifndef EBHTTPFILEDOWNLOAD_H
#define EBHTTPFILEDOWNLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class EbHttpFileDownload : public QObject
{
    Q_OBJECT
public:
    EbHttpFileDownload(QObject *parent = 0);
    virtual ~EbHttpFileDownload(void);

signals:
    void downloadHttpFileOk(const QString &saveToFilePath, const QString &lastModified);
    void downloadPercent(qint64 writtenBytes, qint64 contentLength);
    void downloadHttpFileError(const QString &saveToFilePath, QNetworkReply::NetworkError errorCode);
public slots:
    void downloadHttpFile( const QString &url, const QString &saveToFilePath, const QString &oldLastModifited, int waitSeconds=10 );
    void doDownloadHttpFile(void);
    const QString& getSaveToFilePath(void) const {return m_saveToFilePath;}
    bool getDownloadFinished(void) const {return m_downloadFinished;}
    QNetworkReply::NetworkError getLastErrorCode(void) const {return m_lastErrorCode;}
    const QString& getLastModified(void) const {return m_lastModified;}

    void replyReadyRead(void);
    void replyBytesWritten(qint64 bytes);
    void replyFinished(void);
    void replyError(QNetworkReply::NetworkError code);
private:
    QNetworkAccessManager * m_networkManager;
    QNetworkReply * m_reply;
    QFile * m_file;

    QString m_downloadUrl;
    QString m_oldLastModifited;
    QString m_saveToFilePath;
    bool m_downloadFinished;
    QNetworkReply::NetworkError m_lastErrorCode;
    qint64 m_contentLength;
    QString m_lastModified;
};

#endif // EBHTTPFILEDOWNLOAD_H

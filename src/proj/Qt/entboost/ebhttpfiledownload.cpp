#include "ebhttpfiledownload.h"
#include <QFileInfo>
#include <QThread>
//#include <QTimer>
#include <QCoreApplication>

EbHttpFileDownload::EbHttpFileDownload(QObject *parent) : QObject(parent)
  , m_networkManager(NULL)
  , m_reply(NULL)
  , m_file(NULL)

  , m_downloadFinished(false)
  , m_contentLength(0)
  , m_lastErrorCode(QNetworkReply::NoError)

{
    m_networkManager = new QNetworkAccessManager(this);

}
EbHttpFileDownload::~EbHttpFileDownload(void)
{
    if (m_file!=NULL) {
        m_file->close();
        delete m_file;
        m_file = NULL;
    }
}

void EbHttpFileDownload::doDownloadHttpFile(void)
{
    QUrl pUrl = QUrl(m_downloadUrl);

    // 这里可用从url中获取文件名，但不是对所有的url都有效;
//      QString fileName = m_url.fileName();

    // 将当前文件名设置为临时文件名，下载完成时修改回来;
//    m_fileName = fileName + DOWNLOAD_FILE_SUFFIX;

    // 如果当前下载的字节数为0那么说明未下载过或者重新下载
    // 则需要检测本地是否存在之前下载的临时文件，如果有则删除
//    if (m_bytesCurrentReceived <= 0)
//    {
//        removeFile(m_fileName);
//    }

    QNetworkRequest request;
    request.setUrl(pUrl);

    // 如果支持断点续传，则设置请求头信息
//    if (m_isSupportBreakPoint)
//    {
//        QString strRange = QString("bytes=%1-").arg(m_bytesCurrentReceived);
//        request.setRawHeader("Range", strRange.toLatin1());
//    }

    // 请求下载;
    m_reply = m_networkManager->get(request);
//    connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(replyReadyRead()));
    connect(m_reply, SIGNAL(bytesWritten(qint64)), this, SLOT(replyBytesWritten(qint64)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));

}

void EbHttpFileDownload::downloadHttpFile( const QString& url, const QString& saveToFilePath, const QString& oldLastModifited, int nWaitSeconds )
{
    m_downloadFinished = false;
    m_oldLastModifited = oldLastModifited;
    m_downloadUrl = url;
    m_saveToFilePath = saveToFilePath;
    doDownloadHttpFile();
//    QTimer::singleShot( 10, this, SLOT(doDownloadHttpFile()) );

    for (int i=0; i<nWaitSeconds*200; i++) {
        QCoreApplication::processEvents();
        QThread::msleep (5);
        if (m_downloadFinished) {
            break;
        }
    }
}

void EbHttpFileDownload::replyReadyRead(void)
{
    if (m_reply==NULL) {
        return;
    }
    if (m_file==NULL) {
        m_contentLength = m_reply->attribute(QNetworkRequest::OriginalContentLengthAttribute).toLongLong();
        m_lastModified = m_reply->header(QNetworkRequest::LastModifiedHeader).toString();
        if (!m_oldLastModifited.isEmpty() && m_oldLastModifited==m_lastModified) {
            m_lastErrorCode = QNetworkReply::OperationCanceledError;
            m_downloadFinished = true;
            m_reply->abort();
            return;
        }

        QFile::remove(m_saveToFilePath);
        m_file = new QFile(m_saveToFilePath);
        m_file->open(QIODevice::WriteOnly);   /// 只写方式打开文件
    }
    m_file->write(m_reply->readAll());
}

void EbHttpFileDownload::replyBytesWritten(qint64 bytes)
{
    ///
    const qint64 size = m_reply->size();
    const qint64 contentLength = m_contentLength; /// downloadPercent;
    int i=0;
}

void EbHttpFileDownload::replyFinished(void)
{
    //获取响应的信息，状态码为200表示正常
//    QVariant status_code = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (m_file!=NULL) {
        m_file->close();
        delete m_file;
        m_file = NULL;
    }
    if (m_reply==NULL) {
        return;
    }

    QVariant lastModified = m_reply->header(QNetworkRequest::LastModifiedHeader);
    m_lastModified = lastModified.toString();
    m_lastErrorCode = m_reply->error();
    if (m_lastErrorCode == QNetworkReply::NoError) {
        // 无错误返回
        emit downloadHttpFileOk( m_saveToFilePath, m_lastModified );
    }
    else {
        //处理错误
        emit downloadHttpFileError( m_saveToFilePath, m_lastErrorCode );
    }
    m_downloadFinished = true;

    m_reply->deleteLater();//要删除reply，但是不能在repyfinished里直接delete，要调用deletelater;
    m_reply = NULL;
}

void EbHttpFileDownload::replyError(QNetworkReply::NetworkError errorCode)
{
    if (m_reply==NULL) {
        return;
    }
    m_lastErrorCode = errorCode;
    const QString strError = m_reply->errorString();
    qDebug() << "__________" + strError;

    if (m_file!=NULL) {
        m_file->close();
        delete m_file;
        m_file = NULL;
    }
    emit downloadHttpFileError( m_saveToFilePath, m_reply->error() );
    m_downloadFinished = true;
}

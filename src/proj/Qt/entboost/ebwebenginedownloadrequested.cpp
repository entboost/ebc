#include "ebwebenginedownloadrequested.h"
#include <QWebEngineProfile>
#include <QFileDialog>
#include <ebclientapp.h>

EbWebEngineDownloadRequested::EbWebEngineDownloadRequested(QObject *parent) : QObject(parent)
{
    connect(QWebEngineProfile::defaultProfile(), SIGNAL(downloadRequested(QWebEngineDownloadItem*)), this, SLOT(onDownloadRequested(QWebEngineDownloadItem*)));
}

EbWebEngineDownloadRequested::~EbWebEngineDownloadRequested()
{
    m_downloads.clear();
}

void EbWebEngineDownloadRequested::onDownloadRequested(QWebEngineDownloadItem *downloadItem)
{
    /// memeType	"application/x-mimearchive"	QString
//    const QString memeType = download->mimeType();
//    const QWebEngineDownloadItem::DownloadType downloadType = download->type();
//    if (m_downloads.exist(download->id())) {
//        download->accept();
//        return;
//    }
    const QString path = downloadItem->path();
    const QString fileExt = libEbc::fileExt(path);
//    const QString caption = theLocales.getLocalText("change-head-dialog.saveas-file-dialog.caption","Save As");
//    const QString filterImageFile = theLocales.getLocalText("change-head-dialog.open-file-dialog.filter","Image Files");
    /// Optios=0 会提示文件存在 ConfirmOverwrite
    const QString newPath = QFileDialog::getSaveFileName(0, QString(), path, QString(), 0, 0);
    if (newPath.isEmpty()) {
        downloadItem->cancel();
    }
    else {
        if (!fileExt.isEmpty() && newPath.lastIndexOf(fileExt)<0) {
            /// 加上扩展名
            downloadItem->setPath(newPath+fileExt);
        }
        else {
            downloadItem->setPath(newPath);
        }
        EbWebEngineDownloadItem::pointer pDownloadItem;
        if ( !m_downloads.find(downloadItem->id(),pDownloadItem) ) {
            pDownloadItem = EbWebEngineDownloadItem::create(downloadItem);
            m_downloads.insert( downloadItem->id(),pDownloadItem );
        }
        /// 下载状态
        connect( downloadItem,SIGNAL(finished()),pDownloadItem.get(),SLOT(onDownloadFinished()) );
        connect( downloadItem,SIGNAL(stateChanged(QWebEngineDownloadItem::DownloadState)),pDownloadItem.get(),SLOT(onDownLoadStateChanged(QWebEngineDownloadItem::DownloadState)) );
        connect( downloadItem,SIGNAL(downloadProgress(qint64,qint64)),pDownloadItem.get(),SLOT(onDownloadProgress(qint64,qint64)) );
        /// 下载完成通知
        connect( pDownloadItem.get(),SIGNAL(downloadFinished(const EbWebEngineDownloadItem*)),this,SLOT(onDownloadFinished(const EbWebEngineDownloadItem*)) );
//        QFile::remove(newPath);
        downloadItem->accept();
    }
}

void EbWebEngineDownloadRequested::onDownloadFinished(const EbWebEngineDownloadItem *downloadItem)
{
    m_downloads.remove(downloadItem->downloadId());
}

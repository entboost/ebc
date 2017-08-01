#include "ebwebenginedownloaditem.h"
#include "ebclientapp.h"
#include <ebdialogmainframe.h>

inline qint64 getNextBigFileId(void)
{
    static unsigned int theIndex = 0;
    return (time(0)%89999999)*10000000+((++theIndex)%10000)*10000+(rand()%10000);
}
EbWebEngineDownloadItem::EbWebEngineDownloadItem(QWebEngineDownloadItem *download, QObject * parent)
    : QObject(parent)
    , m_downloadItem(download)
    , m_msgId(0)
    , m_downloadId(0)
    , m_state(QWebEngineDownloadItem::DownloadRequested)
    , m_totalBytes(0)
    , m_receivedBytes(0)
//    , m_sendReceiving(false)
    , m_tCreateTime(0)
{
    m_msgId = getNextBigFileId();
    m_downloadId = download->id();
    m_state = download->state();
    m_totalBytes = download->totalBytes();
    m_receivedBytes = download->receivedBytes();
    m_url = download->url();
    m_mimeType = download->mimeType();
    m_path = download->path();
}
EbWebEngineDownloadItem::pointer EbWebEngineDownloadItem::create(QWebEngineDownloadItem * download,QObject * parent)
{
    return EbWebEngineDownloadItem::pointer( new EbWebEngineDownloadItem(download,parent) );
}

void EbWebEngineDownloadItem::onDownloadFinished()
{
    if ( m_state!=QWebEngineDownloadItem::DownloadCompleted ) {
        m_state = QWebEngineDownloadItem::DownloadCompleted;
    }
    CCrFileInfo * fileInfo = new CCrFileInfo();
    fileInfo->SetQEventType((QEvent::Type)CR_WM_RECEIVED_FILE);
    fileInfo->SetEventParameter(10);
    fileInfo->m_sResId = (cr::bigint)m_msgId;
    fileInfo->m_nMsgId = fileInfo->m_sResId;
    fileInfo->m_nFileSize = (cr::bigint)m_totalBytes;
    fileInfo->m_sSendFrom = fileInfo->m_sResId;
    fileInfo->m_sFileName = m_path;
    QCoreApplication::postEvent(theApp->mainWnd(),fileInfo);
    theApp->m_pCancelFileList.remove((eb::bigint)m_msgId);
//    emit downloadFinished(this);
}

void EbWebEngineDownloadItem::onDownLoadStateChanged(QWebEngineDownloadItem::DownloadState state)
{
    m_state = state;
    switch (m_state) {
    case QWebEngineDownloadItem::DownloadCancelled:
    case QWebEngineDownloadItem::DownloadInterrupted: {
        /// 取消
        CCrFileInfo * fileInfo = new CCrFileInfo();
        fileInfo->SetQEventType((QEvent::Type)CR_WM_CANCEL_FILE);
        fileInfo->SetEventParameter(10);
        fileInfo->m_sResId = (cr::bigint)m_msgId;
        fileInfo->m_nMsgId = fileInfo->m_sResId;
        fileInfo->m_sSendFrom = fileInfo->m_sResId;
        QCoreApplication::postEvent(theApp->mainWnd(),fileInfo);
        theApp->m_pCancelFileList.remove((eb::bigint)m_msgId);
        break;
    }
    default:
        break;
    }

}

void EbWebEngineDownloadItem::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal>0 && m_totalBytes!=bytesTotal) {
        m_totalBytes = bytesTotal;
    }
    m_receivedBytes = bytesReceived;
    if ( m_tCreateTime==0 ) {
        m_tCreateTime = time(0);
        CCrFileInfo * fileInfo = new CCrFileInfo();
        fileInfo->SetQEventType((QEvent::Type)CR_WM_RECEIVING_FILE);
        fileInfo->SetEventParameter(10);
        fileInfo->m_sResId = (cr::bigint)m_msgId;
        fileInfo->m_nMsgId = fileInfo->m_sResId;
        fileInfo->m_sSendFrom = fileInfo->m_sResId;
        fileInfo->m_nFileSize = (cr::bigint)m_totalBytes;
        fileInfo->m_sFileName = m_path;
        QCoreApplication::postEvent(theApp->mainWnd(),fileInfo);
    }

    CChatRoomFilePercent * filePercent = new CChatRoomFilePercent();
    filePercent->SetQEventType((QEvent::Type)CR_WM_FILE_PERCENT);
    filePercent->SetEventParameter(10);
    filePercent->m_sResId = (cr::bigint)m_msgId;
    filePercent->m_nMsgId = filePercent->m_sResId;
    if (m_totalBytes>0) {
        filePercent->m_percent = MAX(0.0,(m_receivedBytes*100.0)/(double)m_totalBytes);
    }
    filePercent->m_nTranSeconds = time(0)-m_tCreateTime;
    filePercent->m_nCurSpeed = m_receivedBytes/(MAX(1.0,filePercent->m_nTranSeconds));

//    if (fPercent<=0.0 && pDownloadInfo->m_nReceivedBytes>100)
//        pChatRoomFilePercent->m_percent = (double)pDownloadInfo->m_nReceivedBytes;
//    else
//        pChatRoomFilePercent->m_percent = fPercent;
//    pChatRoomFilePercent->m_nCurSpeed = nCurrentSpeed;
//    pChatRoomFilePercent->m_nTranSeconds = nTranSeconds;
    QCoreApplication::postEvent(theApp->mainWnd(),filePercent);
    if ( theApp->m_pCancelFileList.exist(m_msgId) ) {
        m_downloadItem->cancel();
    }
//    emit downloadProgress(this);
}

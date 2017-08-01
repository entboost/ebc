#include "ebwebengineurlschemehandler.h"
#include <QWebEngineUrlRequestJob>
#include <QHostAddress>
#include <ebclientapp.h>
#include <QSound>
#include <QFileDialog>
#include <ebdialogmainframe.h>

EbWebEngineUrlSchemeHandler * EbWebEngineUrlSchemeHandler::_instance = 0;

EbWebEngineUrlSchemeHandler *EbWebEngineUrlSchemeHandler::instance(void)
{
    static QMutex mutex;
    if ( _instance==0 ) {
        QMutexLocker locker(&mutex);
        if ( _instance==0 ) {
            _instance = new EbWebEngineUrlSchemeHandler;
        }
    }
    return _instance;
}

EbWebEngineUrlSchemeHandler::EbWebEngineUrlSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
{

}

void EbWebEngineUrlSchemeHandler::requestUrl(const QUrl &pUrl, QWebEngineUrlRequestJob *job)
{
    const QString scheme = pUrl.scheme();
    /// 80->"0.0.0.80"
    /// 898542->"0.13.181.238"
    const QString host = pUrl.host();
    if (scheme==theDownloadResource) {
        QString resourceInfo = pUrl.url().mid(scheme.length()+3);
        if (resourceInfo.left(1)=="/") {
            /// 新版本 xxx:///
            resourceInfo = resourceInfo.mid(1);
        }
        const int nFind1 = resourceInfo.indexOf(',');
        if (nFind1<=0) return;
//        const int nType = resourceInfo.left(nFind1).toInt();
        const int nFind2 = resourceInfo.indexOf(',',nFind1+1);
        eb::bigint resourceId = 0;
        tstring defaultFileName;
        if (nFind2>0)
        {
            const QString temp = resourceInfo.mid(nFind1+1,nFind2-nFind1-1);
            resourceId = resourceInfo.mid(nFind1+1,nFind2-nFind1-1).toLongLong();
            defaultFileName = libEbc::URLDecode(resourceInfo.mid(nFind2+1).toStdString().c_str(),true);
//            sDefaultFileName = libEbc::UTF82ACP(sFileName.c_str()).c_str();
        }
        else {
            resourceId = resourceInfo.mid(nFind1+1).toLongLong();
            defaultFileName = QString("%1").arg(resourceId).toStdString();
        }
        const QString newPath = QFileDialog::getSaveFileName(0, QString(), defaultFileName.c_str() );
        if (!newPath.isEmpty()) {
            theApp->m_ebum.EB_DownloadFileRes(resourceId, newPath.toStdString().c_str());
        }
    }
    else if (scheme==theReqAddContact) {
        QString requestInfo = pUrl.url().mid(scheme.length()+3);
        if (requestInfo.left(1)=="/") {
            /// 新版本 xxx:///
            requestInfo = requestInfo.mid(1);
        }
        const int nFind1 = requestInfo.indexOf(',');
        if (nFind1<=0) return;
        const qint64 nContactUserId = requestInfo.left(nFind1).toLongLong();
        if (nContactUserId==0) {
            return;
        }
        /// 最长取100个字符
        const QString sDescription = requestInfo.mid(nFind1+1).left(100);
        theApp->m_ebum.EB_ReqAddContact(nContactUserId,sDescription.toStdString().c_str());
    }
    else if ( scheme==theCallAccount ) {
        const std::string callAccount( host.toStdString() );
        std::vector<tstring> list;
        if ( libEbc::ParseString( callAccount.c_str(), ".", list) == 4) {
            const unsigned int userId = QHostAddress(host).toIPv4Address();
            theApp->m_ebum.EB_CallUserId( (mycp::bigint)userId );
        }
        else {
            theApp->m_ebum.EB_CallAccount( callAccount.c_str() );
        }
    }
    else if ( scheme==theCallGroup ) {
        const std::string sGroupId( host.toStdString() );
        std::vector<tstring> list;
        if ( libEbc::ParseString( sGroupId.c_str(), ".", list) == 4) {
            const unsigned int nGroupId = QHostAddress(host).toIPv4Address();
            theApp->m_ebum.EB_CallGroup( (mycp::bigint)nGroupId );
        }
        else {
            theApp->m_ebum.EB_CallGroup( eb_atoi64(sGroupId.c_str()) );
        }
    }
    else if (scheme==theOpenSubId) {
        QString info = pUrl.url().mid(scheme.length()+3);
        if (info.left(1)=="/") {
            /// 新版本 xxx:///
            info = info.mid(1);
        }
        openSubId(info, job);
    }
    else if ( scheme==theOpenFile ) {
        /// 打开文件
        QString fileName = pUrl.url().mid(scheme.length()+3);
#ifdef WIN32
        if (fileName.left(1)=="/") {
#else
        if (fileName.left(2)=="//") {

#endif
            fileName = fileName.mid(1);
        }
        QDesktopServices::openUrl( QUrl::fromLocalFile(fileName) );
    }
    else if ( scheme==thePlayVoice ) {
        /// 播放语音消息
        const QString fileName = pUrl.url().mid(scheme.length()+4);
        QSound::play( fileName );
    }
    else if (scheme==theClose) {

    }
    else if (scheme==theExitApp) {
        theApp->mainWnd()->onTriggeredActionExitApp();
    }
    else if (scheme==theLogout) {
        theApp->mainWnd()->onTriggeredActionLogout();
    }
    else if (scheme==theShowMainFrame) {
        theApp->mainWnd()->showNormal();
    }
    else if (scheme==theHideMainFrame) {
        theApp->mainWnd()->onClickedPushButtonSysMin();
    }
    else if (scheme==theOpenFileManager) {
        theApp->mainWnd()->onClickedPushButtonFileManager();
    }
    else {
        /// ??? 是否使用工作台打开？
        QDesktopServices::openUrl( pUrl );
//        const QString sUrl( pUrl.url() );
//        if (scheme=="http" || scheme=="https") {
//            if (!sUrl.isEmpty()) {
//                /// ??? 是否使用工作台打开？
//                QDesktopServices::openUrl( url );
//            }
//            return;
//        }
    }
}

bool EbWebEngineUrlSchemeHandler::openSubId(const QString &info, QWebEngineUrlRequestJob *job)
{
    const eb::bigint nSubId = eb_atoi64(info.toStdString().c_str());
    if (nSubId==0) {
        return false;
    }
    bool bOpenNewWindows = true;
    QString sParameters;
    int pos = info.indexOf(",", 1);
    if( pos > 1 ) {
        sParameters = info.mid(pos+1);
        const QString sOpenFlag = sParameters.isEmpty()?"":sParameters.left(1);
        if (sOpenFlag=="0") {
            bOpenNewWindows = false;
            sParameters = sParameters.mid(11);
        }
        else if (sOpenFlag=="1") {
            bOpenNewWindows = true;
            sParameters = sParameters.mid(1);
        }
        if (!sParameters.isEmpty() && sParameters.left(1)==",") {
            sParameters = sParameters.mid(1);
        }
    }

    if (!bOpenNewWindows && job!=0) {
        const QString sUrl = theApp->subscribeFuncUrl(nSubId, sParameters.toStdString());
        if (!sUrl.isEmpty()) {
            job->redirect(QUrl(sUrl));
        }
        return true;
    }

    return theApp->mainWnd()->openSubscribeFuncWindow(nSubId, "", sParameters.toStdString());
//    if (theApp->mainWnd()->openSubscribeFuncWindow(nSubId, "", sParameters.toStdString())) {
//        if (m_bOpenNewClose)
//            this->PostMessage(WM_CLOSE, 0, 0);
//    }
//    return true;
}

void EbWebEngineUrlSchemeHandler::requestStarted( QWebEngineUrlRequestJob *requestJob )
{
    requestUrl(requestJob->requestUrl());
}

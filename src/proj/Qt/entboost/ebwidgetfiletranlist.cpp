#include "ebwidgetfiletranlist.h"
#include <QTimer>
#include "ebclientapp.h"
#include "ebmessagebox.h"

const int const_TranFile_Height		= 60;

EbWidgetFileTranList::EbWidgetFileTranList(QWidget *parent) : QWidget(parent)
//  , m_callInfo(callInfo)
{
//    assert( m_callInfo.get()!=0 );

}

void EbWidgetFileTranList::onExitChat(bool /*bHangup*/)
{
    while (!m_tranFileList.empty())
    {
        BoostWriteLock wtlock(m_tranFileList.mutex());
        CLockMap<eb::bigint, EbWidgetFileTranItem::pointer>::const_iterator pIter = m_tranFileList.begin();
        if (pIter!=m_tranFileList.end()) {
            const EbWidgetFileTranItem::pointer fileTranItem = pIter->second;
            m_tranFileList.erase(pIter);
            wtlock.unlock();	/// 解锁，预防死锁
            fileTranItem->onButtonCancel();
        }
    }
}

void EbWidgetFileTranList::onSendingFile(const CCrFileInfo *fileInfo)
{
    if (fileInfo->GetStateCode()==EB_STATE_FILE_ALREADY_EXIST) {
        EbWidgetFileTranItem::pointer pDlgTranFile;
        if (fileInfo->GetParam()>0 && m_tranFileList.find(fileInfo->GetParam(),pDlgTranFile, true)) {
            /// *不需要处理下面代码
//            m_pDelTranFile.add(pDlgTranFile);
//            SetTimer(TIMER_ID_CLOSE_TRAN_FILE,10,NULL);
        }
        return;
    }
    EbWidgetFileTranItem::pointer tranFile;
    if (fileInfo->GetParam()>0 && m_tranFileList.find(fileInfo->GetParam(),tranFile,true)) {
        /// ** 找到前面等待预处理发送文件，更新界面即可
        tranFile->updateFileInfo(fileInfo);
    }
    else {
        const int index = m_tranFileList.size();
        tranFile = EbWidgetFileTranItem::create(true,fileInfo,this);
        tranFile->setVisible(true);
        tranFile->setGeometry(0, index*const_TranFile_Height, width(), const_TranFile_Height);
    }
    /// 需要放在这里
    m_tranFileList.insert(fileInfo->m_nMsgId, tranFile);
    moveSize();
}

void EbWidgetFileTranList::onReceivingFile(const CCrFileInfo *fileInfo)
{
    if (m_tranFileList.exist(fileInfo->m_nMsgId)) {
        return;
    }
    m_pAddTranFile.add(*fileInfo);
    QTimer::singleShot( 1,this,SLOT(onAddFileTranItem()) );
}

void EbWidgetFileTranList::onFilePercent(const CChatRoomFilePercent *pChatRoomFilePercent)
{
    EbWidgetFileTranItem::pointer tranFile;
    if (m_tranFileList.find(pChatRoomFilePercent->m_nMsgId, tranFile)) {
        tranFile->setFilePercent(pChatRoomFilePercent);
    }
}

void EbWidgetFileTranList::deleteTranFile(eb::bigint msgId)
{
    EbWidgetFileTranItem::pointer pDlgTranFile;
    if (m_tranFileList.find(msgId, pDlgTranFile, true)) {
        /// 不需要处理下面代码
//        m_pDelTranFile.add(pDlgTranFile);
//        SetTimer(TIMER_ID_CLOSE_TRAN_FILE,10,NULL);
        moveSize();
    }
}

bool EbWidgetFileTranList::isEmpty(void) const
{
    return m_tranFileList.empty();
}

bool EbWidgetFileTranList::requestClose(void) const
{
    if (isEmpty()) {
        return true;
    }
    const QString title = theLocales.getLocalText("message-box.tran-file-exit-chat.title","Exit Chat");
    const QString text = theLocales.getLocalText("message-box.tran-file-exit-chat.text","Confirm exit chat?");
    if ( EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )!=QDialog::Accepted) {
        return false;
    }
    return true;
}

void EbWidgetFileTranList::onAddFileTranItem()
{
    CCrFileInfo fileInfo;
    while (m_pAddTranFile.front(fileInfo) && !m_tranFileList.exist(fileInfo.m_nMsgId)) {
        const int index = m_tranFileList.size();
        EbWidgetFileTranItem::pointer tranFile = EbWidgetFileTranItem::create(false,&fileInfo,this);
        tranFile->setVisible(true);
        tranFile->setGeometry(0, index*const_TranFile_Height, width(), const_TranFile_Height);
        m_tranFileList.insert(fileInfo.m_nMsgId, tranFile);
    }
    moveSize();
}

void EbWidgetFileTranList::moveSize()
{
    int index = 0;
    BoostReadLock rdlock(m_tranFileList.mutex());
    CLockMap<eb::bigint, EbWidgetFileTranItem::pointer>::const_iterator pIter = m_tranFileList.begin();
    for (; pIter!=m_tranFileList.end(); pIter++) {
        const EbWidgetFileTranItem::pointer& tranFile = pIter->second;
        tranFile->setGeometry(0, index*const_TranFile_Height, width(), const_TranFile_Height);
        index++;
    }
}

void EbWidgetFileTranList::resizeEvent(QResizeEvent *event)
{
    moveSize();
    QWidget::resizeEvent(event);
}

void EbWidgetFileTranList::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
}

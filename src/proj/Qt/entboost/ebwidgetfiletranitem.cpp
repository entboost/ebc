#include "ebwidgetfiletranitem.h"
#include <QFileDialog>
#include <ebmessagebox.h>

EbWidgetFileTranItem::EbWidgetFileTranItem(bool isSending, const CCrFileInfo *fileInfo, QWidget *parent) : QWidget(parent)
  , m_isSending(isSending)
  , m_fileInfo(*fileInfo)
{
    m_labelFileName = new QLabel(this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0,100);
    /// for test
//    m_progressBar->setValue(10);
//    m_progressBar->setFormat("%p% 2.1MB/S");

    m_buttonSave2Cloud = new QPushButton(this);
    connect( m_buttonSave2Cloud,SIGNAL(clicked()),this,SLOT(onButtonSave2Cloud()) );
    m_buttonSave2Cloud->setVisible(false);
    m_buttonSave2Cloud->setObjectName("RequestAddContact");
    m_buttonSaveas = new QPushButton(this);
    connect( m_buttonSaveas,SIGNAL(clicked()),this,SLOT(onButtonSaveas()) );
    m_buttonSaveas->setVisible(false);
    m_buttonSaveas->setObjectName("RequestAddContact");
    m_buttonSave = new QPushButton(this);
    connect( m_buttonSave,SIGNAL(clicked()),this,SLOT(onButtonSave()) );
    m_buttonSave->setVisible(false);
    m_buttonSave->setObjectName("RequestAddContact");
    m_buttonReject = new QPushButton(this);
    connect( m_buttonReject,SIGNAL(clicked()),this,SLOT(onButtonReject()) );
    m_buttonReject->setVisible(false);
    m_buttonReject->setObjectName("RequestAddContact");
    m_buttonOffSend = new QPushButton(this);
    connect( m_buttonOffSend,SIGNAL(clicked()),this,SLOT(onButtonOffSend()) );
    m_buttonOffSend->setVisible(false);
    m_buttonOffSend->setObjectName("RequestAddContact");
    m_buttonCancel = new QPushButton(this);
    connect( m_buttonCancel,SIGNAL(clicked()),this,SLOT(onButtonCancel()) );
    m_buttonCancel->setVisible(false);
    m_buttonCancel->setObjectName("RequestAddContact");

    if (!m_isSending && m_fileInfo.m_bOffFile) {
        m_buttonSave2Cloud->setVisible(true);
    }
    else {
        m_buttonSave2Cloud->setVisible(false);
    }

    const tstring sFileName = libEbc::GetFileName(m_fileInfo.m_sFileName.toStdString());
    char fileText[260];
    if (m_fileInfo.m_nFileSize >= const_gb_size)
        sprintf( fileText, "%s(%.02fGB)",sFileName.c_str(),(double)m_fileInfo.m_nFileSize/const_gb_size);
    else if (m_fileInfo.m_nFileSize >= const_mb_size)
        sprintf( fileText, "%s(%.02fMB)",sFileName.c_str(),(double)m_fileInfo.m_nFileSize/const_mb_size);
    else if (m_fileInfo.m_nFileSize >= const_kb_size)
        sprintf( fileText, "%s(%.02fKB)",sFileName.c_str(),(double)m_fileInfo.m_nFileSize/const_kb_size);
    else if (m_fileInfo.m_nFileSize>0)
        sprintf( fileText, "%s(%lldByte)",sFileName.c_str(),m_fileInfo.m_nFileSize);
    else
        strcpy( fileText, sFileName.c_str());
    m_labelFileName->setText(fileText);
    m_labelFileName->setToolTip(fileText);
    //if (m_sResourceId>0)
    //{
    //	this->GetDlgItem(IDC_BUTTON_SAVEAS)->ShowWindow(SW_HIDE);
    //	this->GetDlgItem(IDC_BUTTON_SAVE)->ShowWindow(SW_HIDE);
    //	this->GetDlgItem(IDC_BUTTON_REJECT)->ShowWindow(SW_HIDE);
    //	this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
    //	this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_SHOW);
    //}else
    {
        if (m_isSending) {
//            this->GetDlgItem(IDC_BUTTON_SAVEAS)->ShowWindow(SW_HIDE);
//            this->GetDlgItem(IDC_BUTTON_SAVE)->ShowWindow(SW_HIDE);
//            this->GetDlgItem(IDC_BUTTON_REJECT)->ShowWindow(SW_HIDE);
            if (m_fileInfo.GetStateCode()==EB_STATE_WAITING_PROCESS) {
//                this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
//                this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_HIDE);
            }
            else {
                if (m_fileInfo.m_sResId>0 || m_fileInfo.m_bOffFile) {
//                    this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
                }
                else {
                    m_buttonOffSend->setVisible(true);
                }
                m_buttonCancel->setVisible(true);
            }
        }
//        else if (m_fileInfo.m_sResId>0 && m_fileInfo.m_sResId==m_fileInfo.m_nMsgId) {
        else if (m_fileInfo.m_sResId>0) {
//            this->GetDlgItem(IDC_BUTTON_SAVEAS)->ShowWindow(SW_HIDE);
//            this->GetDlgItem(IDC_BUTTON_SAVE)->ShowWindow(SW_HIDE);
//            this->GetDlgItem(IDC_BUTTON_REJECT)->ShowWindow(SW_HIDE);
//            this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
            m_buttonCancel->setVisible(true);
        }
        else {
            m_buttonSaveas->setVisible(true);
            m_buttonSave->setVisible(true);
            m_buttonReject->setVisible(true);
//            this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
//            this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_HIDE);
        }
    }

    updateLocaleInfo();
}

EbWidgetFileTranItem::pointer EbWidgetFileTranItem::create(bool isSending, const CCrFileInfo *fileInfo, QWidget *parent)
{
    return EbWidgetFileTranItem::pointer( new EbWidgetFileTranItem(isSending,fileInfo,parent) );
}

void EbWidgetFileTranItem::updateFileInfo(const CCrFileInfo *pCrFileInfo)
{
    m_fileInfo = pCrFileInfo;
    /// 更新 界面
    if (m_isSending) {
        if (m_fileInfo.GetStateCode()==EB_STATE_WAITING_PROCESS) {
//            this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
//            this->GetDlgItem(IDC_BUTTON_CANCEL)->ShowWindow(SW_HIDE);
        }
        else {
            if (m_fileInfo.m_sResId>0 || m_fileInfo.m_bOffFile) {
//                this->GetDlgItem(IDC_BUTTON_OFFFILE)->ShowWindow(SW_HIDE);
            }
            else {
                m_buttonOffSend->setVisible(true);
            }
            m_buttonCancel->setVisible(true);
        }
    }
}

void EbWidgetFileTranItem::setFilePercent(const CChatRoomFilePercent *filePercent)
{
    if ( m_buttonOffSend->isEnabled() ) {
        m_buttonOffSend->setEnabled(false);
    }
    char status[128];
    if (filePercent->m_percent>100) {
        /// ** 这是传输字节
        const double fReceivedBytes = filePercent->m_percent;
        if (fReceivedBytes >= const_gb_size)
            sprintf( status, "%.02fGB",(double)fReceivedBytes/const_gb_size);
        else if (fReceivedBytes >= const_mb_size)
            sprintf( status, "%.02fMB",(double)fReceivedBytes/const_mb_size);
        else if (fReceivedBytes >= const_kb_size)
            sprintf( status, "%.02fKB",(double)fReceivedBytes/const_kb_size);
        else
            sprintf( status, "%dByte",(int)fReceivedBytes);
    }
    else if (filePercent->m_nCurSpeed>1024000.0) {
        sprintf( status, "%.2fMB/S", filePercent->m_nCurSpeed/(1024.0*1024.0));
//        sprintf( status, "%.2f%% %.2fMB/S", pChatRoomFilePercent->m_percent,pChatRoomFilePercent->m_nCurSpeed/(1024.0*1024.0));
    }
    else {
        sprintf( status, "%.2fKB/S", filePercent->m_nCurSpeed/1024.0);
//        sprintf( status, "%.2f%% %.2fKB/S", pChatRoomFilePercent->m_percent,pChatRoomFilePercent->m_nCurSpeed/1024.0);
    }
    /// ==0.0，没有进度，表示未知文件大小
    if (filePercent->m_percent==0.0) {
        const int value = m_progressBar->value();
        m_progressBar->setValue( MIN(100,value+(100-value)/30) );
    }
    else {
        m_progressBar->setValue( (int)(filePercent->m_percent) );
    }
    const QString format = filePercent->m_percent==0.0?QString("%1").arg(status): QString("%p% %1").arg(status);
    m_progressBar->setFormat( format );
}

void EbWidgetFileTranItem::updateLocaleInfo()
{
    m_buttonSave2Cloud->setText( theLocales.getLocalText("tran-file.button-save-cloud.text","Save Cloud") );
    m_buttonSave2Cloud->setToolTip( theLocales.getLocalText("tran-file.button-save-cloud.tooltip","") );
    m_buttonSaveas->setText( theLocales.getLocalText("tran-file.button-save-as.text","Save As") );
    m_buttonSaveas->setToolTip( theLocales.getLocalText("tran-file.button-save-as.tooltip","") );
    m_buttonSave->setText( theLocales.getLocalText("tran-file.button-save.text","Save") );
    m_buttonSave->setToolTip( theLocales.getLocalText("tran-file.button-save.tooltip","") );
    m_buttonReject->setText( theLocales.getLocalText("tran-file.button-reject.text","Reject") );
    m_buttonReject->setToolTip( theLocales.getLocalText("tran-file.button-reject.tooltip","") );
    m_buttonOffSend->setText( theLocales.getLocalText("tran-file.button-off-send.text","Off Send") );
    m_buttonOffSend->setToolTip( theLocales.getLocalText("tran-file.button-off-send.tooltip","") );
    if (m_fileInfo.m_sResId>0) {
        m_buttonCancel->setText( theLocales.getLocalText("tran-file.button-cancel-down.text","Cancel") );
        m_buttonCancel->setToolTip( theLocales.getLocalText("tran-file.button-cancel-down.tooltip","") );
    }
    else {
        m_buttonCancel->setText( theLocales.getLocalText("tran-file.button-cancel-send.text","Cancel") );
        m_buttonCancel->setToolTip( theLocales.getLocalText("tran-file.button-cancel-send.tooltip","") );
    }

}

void EbWidgetFileTranItem::onButtonSave2Cloud()
{
    theApp->m_ebum.EB_Save2CloudDrive(m_fileInfo.GetCallId(),m_fileInfo.m_nMsgId);
}

void EbWidgetFileTranItem::onButtonSaveas()
{
//    tstring sFileName;
//    tstring sFileExt;
//    libEbc::GetFileExt(m_fileInfo.m_sFileName,sFileName,sFileExt);

    const QString selectedFilter = theLocales.getLocalText("tran-file.button-save-as.file-dialog.filter","All Files") + " (*)";
    const QString newPath = QFileDialog::getSaveFileName(this, theLocales.getLocalText("tran-file.button-save-as.file-dialog.caption","Save As"),
                                                         m_fileInfo.m_sFileName, selectedFilter );
    if (newPath.isEmpty()) {
        return;
    }
    if ( QFile::exists(newPath) ) {
        /// * 判断文件是否打开
        QFile file(newPath);
        if ( !file.open(QFile::Append) ) {
            /// 目标文件被占用，不能接收文件：\r\n请关闭文件后重试！
            const QString text = theLocales.getLocalText("message-show.file-exclusive-error","File Exclusive Error");
            EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING, default_warning_auto_close );
            return;
        }
        file.close();
    }
    theApp->m_ebum.EB_AcceptFile( m_fileInfo.GetCallId(),m_fileInfo.m_nMsgId, newPath.toStdString().c_str() );
    /// ? P2P等开始传统文件再hide
    m_buttonSave2Cloud->setVisible(false);
    m_buttonSaveas->setEnabled(false);
    m_buttonSave->setEnabled(false);
    m_buttonReject->setText( theLocales.getLocalText("tran-file.button-reject-cancel.text","Cancel") );
    m_buttonReject->setToolTip( theLocales.getLocalText("tran-file.button-reject-cancel.tooltip","") );
    m_buttonReject->setVisible(true);
}

void EbWidgetFileTranItem::onButtonSave()
{
    const tstring sFileName = m_fileInfo.m_sFileName.toStdString();
    tstring sFileSimpleName;
    tstring sFileExt;
    libEbc::GetFileExt(sFileName, sFileSimpleName, sFileExt);
    //size_t nFileSize = m_fileInfo.m_nFileSize;
    QString sSaveTo = QString("%1/%2").arg(theApp->userFilePath()).arg(sFileName.c_str());
    int index = 0;
    while (true) {
        if ( !QFile::exists(sSaveTo) ) {
            break;
        }
        if (sFileExt.empty())
            sSaveTo = QString("%1/%2(%3)").arg(theApp->userFilePath()).arg(sFileName.c_str()).arg(++index);
        else
            sSaveTo = QString("%1/%2(%3).%4").arg(theApp->userFilePath()).arg(sFileSimpleName.c_str()).arg(++index).arg(sFileExt.c_str());
    }
    theApp->m_ebum.EB_AcceptFile( m_fileInfo.GetCallId(),m_fileInfo.m_nMsgId, sSaveTo.toStdString().c_str() );
    m_buttonSave2Cloud->setEnabled(false);
    m_buttonSaveas->setEnabled(false);
    m_buttonSave->setEnabled(false);
    m_buttonReject->setText( theLocales.getLocalText("tran-file.button-reject-cancel.text","Cancel") );
    m_buttonReject->setToolTip( theLocales.getLocalText("tran-file.button-reject-cancel.tooltip","") );
    m_buttonReject->setVisible(true);
}

void EbWidgetFileTranItem::onButtonReject()
{
    theApp->m_ebum.EB_CancelFile(m_fileInfo.GetCallId(),m_fileInfo.m_nMsgId);
}

void EbWidgetFileTranItem::onButtonOffSend()
{
    const eb::bigint callId(m_fileInfo.GetCallId());
    theApp->m_ebum.EB_SendFile(callId,m_fileInfo.m_sFileName,0,false,true);	/// 必须先发送，后面取消
    theApp->m_ebum.EB_CancelFile(m_fileInfo.GetCallId(),m_fileInfo.m_nMsgId);
}

void EbWidgetFileTranItem::onButtonCancel()
{
    if (m_fileInfo.m_sResId>0 && m_fileInfo.m_sResId==m_fileInfo.m_nMsgId) {
        theApp->m_pCancelFileList.insert(m_fileInfo.m_nMsgId,true,false);
        return;
    }

    if (m_fileInfo.m_sResId>0)
        theApp->m_ebum.EB_CancelFileRes(m_fileInfo.m_sResId,m_fileInfo.m_nMsgId);
    else
        theApp->m_ebum.EB_CancelFile(m_fileInfo.GetCallId(),m_fileInfo.m_nMsgId);
}

void EbWidgetFileTranItem::resizeEvent(QResizeEvent *e)
{
    m_labelFileName->setGeometry(2, 2, width()-2*2, 20);
    m_progressBar->setGeometry(2, 22, width()-4, 16);
//    this->GetDlgItem(IDC_STATIC_SHEECHSTATUS)->setGeometry(2, 40, width()-4, 14);
    const int const_Button_Top		= 42;
    const int const_Button_Width1	= 35;
    const int const_Button_Width2	= 55;
    const int const_Button_Height	= 17;
    const int const_Button_Interval = 2;	// 按钮间隔
    int x = width()-const_Button_Width1-3;			// 3是最右边间隔

    if (m_fileInfo.m_sResId>0 || m_isSending) {
        m_buttonCancel->setGeometry(x, const_Button_Top, const_Button_Width1,const_Button_Height);
        if (m_fileInfo.m_sResId==0) {
            x -= (const_Button_Width2+const_Button_Interval);
            m_buttonOffSend->setGeometry(x, const_Button_Top,const_Button_Width2,const_Button_Height);
        }
    }
    else {
        m_buttonSaveas->setGeometry(x, const_Button_Top,const_Button_Width1,const_Button_Height);
        x -= (const_Button_Width1+const_Button_Interval);
        m_buttonSave->setGeometry(x, const_Button_Top,const_Button_Width1,const_Button_Height);
        x -= (const_Button_Width1+const_Button_Interval);
        m_buttonReject->setGeometry(x, const_Button_Top,const_Button_Width1,const_Button_Height);
    }
    if (!m_isSending && m_fileInfo.m_bOffFile) {
        x -= (const_Button_Width2+const_Button_Interval);
        m_buttonSave2Cloud->setGeometry(x, const_Button_Top,const_Button_Width2,const_Button_Height);
    }
    QWidget::resizeEvent(e);
}

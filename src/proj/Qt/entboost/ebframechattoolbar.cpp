#include "ebframechattoolbar.h"
#include <ebmessagebox.h>
#include <QFileDialog>

EbFrameChatToolBar::EbFrameChatToolBar(const EbcCallInfo::pointer& pCallInfo,QWidget *parent) : QFrame(parent)
  , m_type(EbFrameChatToolBar::Unknown)
  , m_callInfo(pCallInfo)
  , m_showButtonCount(0)
  , m_msgId(0)
  , m_msgFromUserId(0), m_msgReadFlag(0)
  , m_timerIdCheckMousePos(0)

{
    assert( m_callInfo.get()!=0 );
    this->setObjectName("FindTextFrame");
    this->resize( 2, 30 );

    /// 最多 7 个按钮，足够使用
    for ( int i=0; i<7; i++ ) {
        QPushButton * button = new QPushButton(this);
        button->setObjectName("SearchBarButton");
        button->setVisible(false);
        m_buttonList.push_back(button);
    }
    connect( m_buttonList[0],SIGNAL(clicked(bool)),this,SLOT(onClickedButton0(bool)) );
    connect( m_buttonList[1],SIGNAL(clicked(bool)),this,SLOT(onClickedButton1(bool)) );
    connect( m_buttonList[2],SIGNAL(clicked(bool)),this,SLOT(onClickedButton2(bool)) );
    connect( m_buttonList[3],SIGNAL(clicked(bool)),this,SLOT(onClickedButton3(bool)) );
    connect( m_buttonList[4],SIGNAL(clicked(bool)),this,SLOT(onClickedButton4(bool)) );
    connect( m_buttonList[5],SIGNAL(clicked(bool)),this,SLOT(onClickedButton5(bool)) );
    connect( m_buttonList[6],SIGNAL(clicked(bool)),this,SLOT(onClickedButton6(bool)) );

}

EbFrameChatToolBar::~EbFrameChatToolBar()
{
    m_buttonList.clear();
}

void EbFrameChatToolBar::setCallInfo(const EbcCallInfo::pointer &pCallInfo)
{
    m_callInfo = pCallInfo;
    assert( m_callInfo.get()!=0 );
}

void EbFrameChatToolBar::updateMsgType( const QPoint &pos, qint64 msgId, EbFrameChatToolBar::Type type,
                                        const QVariant &parameter1, const QVariant &parameter2 )
{
    if ( m_msgId==msgId && m_type==type && this->isVisible() ) {
        return;
    }
    m_msgId = msgId;
    m_type = type;
    m_parameter1 = parameter1;
    m_parameter2 = parameter2;

    if (m_msgId>0)
    {
        char sSql[256];
        sprintf( sSql, "select from_uid,read_flag,msg_type FROM msg_record_t WHERE msg_id=%lld LIMIT 1", m_msgId);
        int nCookie = 0;
        theApp->m_sqliteUser->select(sSql, nCookie);
        cgcValueInfo::pointer pRecord = theApp->m_sqliteUser->last(nCookie);
        if (pRecord.get()!=NULL)
        {
            m_msgFromUserId = pRecord->getVector()[0]->getBigIntValue();
            m_msgReadFlag = pRecord->getVector()[1]->getIntValue();
            m_msgType = pRecord->getVector()[2]->getIntValue();
            theApp->m_sqliteUser->reset(nCookie);
        }
    }
    switch ( m_type ) {
    case EbFrameChatToolBar::SelectedText:
        m_showButtonCount = 2;
        break;
    case EbFrameChatToolBar::PointImage:
        m_showButtonCount = 4;
        break;
    case EbFrameChatToolBar::PointFile:
        m_showButtonCount = 4;
        break;
    case EbFrameChatToolBar::PointUserHead:
        m_showButtonCount = 1;
        if ( canCollectMessage() ) {
            m_showButtonCount++;
            if ( canWithdrawMessage() ) {
                m_showButtonCount++;
            }
        }
        break;
    default:
        return;
    }
    updateLocaleInfo();

    showHideButton();
    moveSize();
    this->move( MAX(5,pos.x()-width()/2), pos.y()+10 );
    if ( !this->isVisible() ) {
        this->setVisible( true );
        this->raise();
    }

    if ( m_timerIdCheckMousePos!=0 ) {
        this->killTimer( m_timerIdCheckMousePos );
    }
    m_timerIdCheckMousePos = this->startTimer( 2000 );
}

void EbFrameChatToolBar::updateLocaleInfo()
{
    switch ( m_type ) {
    case EbFrameChatToolBar::SelectedText: {
        QPushButton * button = m_buttonList[0];
        button->setText( theLocales.getLocalText("chat-toolbar.selected-text.copy.text","Copy") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.selected-text.copy.tooltip","") );
        button = m_buttonList[1];
        button->setText( theLocales.getLocalText("chat-toolbar.selected-text.saveas.text","Save As...") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.selected-text.saveas.tooltip","") );
        break;
    }
    case EbFrameChatToolBar::PointImage: {
        QPushButton * button = m_buttonList[0];
        button->setText( theLocales.getLocalText("chat-toolbar.point-image.open.text","Open") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-image.open.tooltip","") );
        button = m_buttonList[1];
        button->setText( theLocales.getLocalText("chat-toolbar.point-image.open-dir.text","Open Directory") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-image.open-dir.tooltip","") );
        button = m_buttonList[2];
        button->setText( theLocales.getLocalText("chat-toolbar.point-image.copy.text","Copy") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-image.copy.tooltip","") );
        button = m_buttonList[3];
        button->setText( theLocales.getLocalText("chat-toolbar.point-image.saveas.text","Save As...") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-image.saveas.tooltip","") );
        break;
    }
    case EbFrameChatToolBar::PointFile: {
        QPushButton * button = m_buttonList[0];
        button->setText( theLocales.getLocalText("chat-toolbar.point-file.open.text","Open") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-file.open.tooltip","") );
        button = m_buttonList[1];
        button->setText( theLocales.getLocalText("chat-toolbar.point-file.open-dir.text","Open Directory") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-file.open-dir.tooltip","") );
        button = m_buttonList[2];
        button->setText( theLocales.getLocalText("chat-toolbar.point-file.copy.text","Copy") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-file.copy.tooltip","") );
        button = m_buttonList[3];
        button->setText( theLocales.getLocalText("chat-toolbar.point-file.saveas.text","Save As...") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-file.saveas.tooltip","") );
        break;
    }
    case EbFrameChatToolBar::PointUserHead: {
        QPushButton * button = m_buttonList[0];
        button->setText( theLocales.getLocalText("chat-toolbar.point-user-head.delete.text","Delete") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-user-head.delete.tooltip","") );
        button = m_buttonList[1];
        button->setText( theLocales.getLocalText("chat-toolbar.point-user-head.collect.text","Collect") );
        button->setToolTip( theLocales.getLocalText("chat-toolbar.point-user-head.collect.tooltip","") );
        if ( canWithdrawMessage() ) {
            button = m_buttonList[2];
            button->setText( theLocales.getLocalText("chat-toolbar.point-user-head.withdraw.text","Withdraw") );
            button->setToolTip( theLocales.getLocalText("chat-toolbar.point-user-head.withdraw.tooltip","") );
        }
        break;
    }
    default:
        return;
    }
}

void EbFrameChatToolBar::onClickedButton0(bool /*checked*/)
{
    switch ( m_type ) {
    case EbFrameChatToolBar::SelectedText: {
        /// 复制文本
        const QString selectedText = m_parameter1.toString();
        if ( selectedText.isEmpty() ) {
            break;
        }
        QClipboard *board = QApplication::clipboard();
        board->setText( selectedText );
        break;
    }
    case EbFrameChatToolBar::PointImage:
    case EbFrameChatToolBar::PointFile: {
        /// 打开图片
        /// 打开文件
        const QString fileName = m_parameter1.toString();
        if ( !QFile::exists( fileName ) ) {
            EbMessageBox::doShow( 0,"", QChar::Null, theLocales.getLocalText("chat-msg-text.file-not-exist","File Not Exist"),
                                  EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
            break;
        }
        QDesktopServices::openUrl( QUrl::fromLocalFile(fileName) );
        break;
    }
    case EbFrameChatToolBar::PointUserHead: {
        /// 删除本地聊天消息
        deleteMessage();
        break;
    }
    default:
        return;
    }

    this->hide();

}

void EbFrameChatToolBar::onClickedButton1(bool /*checked*/)
{
    switch ( m_type ) {
    case EbFrameChatToolBar::SelectedText: {
        /// 选择文本另存到文件
        const QString selectedText = m_parameter1.toString();
        if ( selectedText.isEmpty() ) {
            break;
        }
        char szFileName[260];
        const time_t tNow = time(0);
        const struct tm *newtime = localtime(&tNow);
        sprintf( szFileName,"EB%s%04d%02d%02d%02d%02d%02d", theLocales.getLocalStdString( "name-text.text", "Text" ).c_str(),
                newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec );
        const QString selectedFilter = theLocales.getLocalText("chat-toolbar.selected-text.saveas.file-dialog.filter","Text Files") + " (*.txt)";
        const QString newPath = QFileDialog::getSaveFileName(this, theLocales.getLocalText("chat-toolbar.selected-text.saveas.file-dialog.caption","Save As"),
                                                             szFileName, selectedFilter );
        if (newPath.isEmpty()) {
            break;
        }
        else {
            QFile file(newPath);
            if ( file.open(QFile::WriteOnly) ) {
                /// 中文能正确保存，和显示
                file.write( selectedText.toUtf8() );
                file.close();
                break;
            }
        }
        break;
    }
    case EbFrameChatToolBar::PointImage:
    case EbFrameChatToolBar::PointFile: {
        /// 打开图片目录
        /// 打开文件目录
        QString fileName = m_parameter1.toString();
        const QFileInfo fileInfo(fileName);
        const QString filePath( fileInfo.absolutePath() );
        if ( !fileInfo.exists() ) {
            QDesktopServices::openUrl( QUrl::fromLocalFile(filePath) );
        }
        else {
#ifdef WIN32
            fileName.replace("/","\\");
            const QString param = "/select, \""+fileName+"\"";
            QProcess::startDetached( "explorer "+param );
#else
            QDesktopServices::openUrl( QUrl::fromLocalFile(filePath) );
//            QDesktopServices::openUrl( QUrl(filePath, QUrl::TolerantMode) );
#endif
        }
        break;
    }
    case EbFrameChatToolBar::PointUserHead: {
        /// 收藏消息
        collectMessage();
        break;
    }
    default:
        return;
    }
    this->hide();

}

void EbFrameChatToolBar::deleteMessage()
{
    if (m_msgId==0) {
        return;
    }
    const QString title = theLocales.getLocalText("message-box.delete-chat-msg.title","Delete Chat Msg");
    const QString text = theLocales.getLocalText("message-box.delete-chat-msg.text","Confirm Delete Chat Msg?");
    const int ret = EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION );
    if (ret==QDialog::Accepted) {
        emit deleteMessage( m_msgId );
        theApp->deleteDbRecord( m_msgId );
    }
}

void EbFrameChatToolBar::withdrawMessage()
{
    if ( canWithdrawMessage() ) {
        theApp->m_ebum.EB_RequestWithdrawMsg( m_callInfo->callId(), m_msgId );
    }
}

bool EbFrameChatToolBar::canWithdrawMessage() const
{
    if (m_msgId==0) {
        return false;
    }
    if (m_msgFromUserId==theApp->logonUserId() && m_msgType!=(int)MRT_RESOURCE && (m_msgReadFlag&EBC_READ_FLAG_WITHDRAW)==0 && m_callInfo->callId()>0 )
        return true;
    else
        return false;
}

void EbFrameChatToolBar::collectMessage()
{
    if ( canCollectMessage() ) {
        /// 个人收藏
        theApp->m_ebum.EB_RequestCollectMsg(m_callInfo->callId(), m_msgId, false);
    }
}

bool EbFrameChatToolBar::canCollectMessage() const
{
    if (m_msgId==0) {
        return false;
    }
    if (m_callInfo->callId()>0 && isCanCollectRecordType((EB_MSG_RECORD_TYPE)m_msgType) && (m_msgReadFlag&EBC_READ_FLAG_WITHDRAW)==0)
        return true;
    else
        return false;
}

void EbFrameChatToolBar::onClickedButton2(bool /*checked*/)
{
    switch ( m_type ) {
    case EbFrameChatToolBar::SelectedText: {
        /// ---
        break;
    }
    case EbFrameChatToolBar::PointImage: {
        /// 复制图片
        if ( m_parameter2.isNull() || !m_parameter2.isValid()) {
            break;
        }
        QClipboard *board = QApplication::clipboard();
        QMimeData * mimeData = new QMimeData();
        mimeData->setImageData( m_parameter2 );
        board->setMimeData( mimeData );
        break;
    }
    case EbFrameChatToolBar::PointFile: {
        /// 复制文件
        const QString filePath = m_parameter1.toString();
        if ( !QFile::exists(filePath) ) {
            EbMessageBox::doShow( 0,"", QChar::Null, theLocales.getLocalText("chat-msg-text.file-not-exist","File Not Exist"),
                                  EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
            break;
        }

        QClipboard *board = QApplication::clipboard();
        /// 目前只实现复制文件名称
        board->setText(filePath);
//        const QString oldText = board->text();
//        const QMimeData * oldMimeData = board->mimeData();
//        if (oldMimeData!=0) {
//            const QString oldMimeDataText = oldMimeData->text();
//            const QByteArray byteArray = oldMimeData->data();
//            const QList<QUrl> urls = oldMimeData->urls();
//            int i=0;
//        }

//        QMimeData * mimeData = new QMimeData();

        /// Copy file (gnome)
//        QByteArray gnomeFormat = QByteArray("copy\n");
        const QString text = filePath;
//        gnomeFormat.append(QUrl::fromLocalFile(filePath).toEncoded()).append("\n");
//        QList<QUrl> dataUrls;
//        dataUrls << QUrl::fromLocalFile(filePath);

        //setText  need to be called, or you can't clip the context to clipboard.
//        mimeData->setText(text);
//        mimeData->setUrls(dataUrls);
//        gnomeFormat.remove(gnomeFormat.length() - 1, 1);
//        mimeData->setData("x-special/gnome-copied-files", gnomeFormat);
        /// Set the mimedata
//        board->setMimeData(mimeData);
        break;
    }
    case EbFrameChatToolBar::PointUserHead: {
        /// 撤回消息
        withdrawMessage();
        break;
    }
    default:
        return;
    }
    this->hide();

}

void EbFrameChatToolBar::onClickedButton3(bool /*checked*/)
{
    switch ( m_type ) {
    case EbFrameChatToolBar::SelectedText: {
        /// ---
        break;
    }
    case EbFrameChatToolBar::PointImage: {
        /// 图片另存
        if ( m_parameter2.isNull() || !m_parameter2.isValid()) {
            break;
        }
        char szFileName[260];
        const time_t tNow = time(0);
        const struct tm *newtime = localtime(&tNow);
        sprintf( szFileName,"EB%s%04d%02d%02d%02d%02d%02d", theLocales.getLocalStdString( "name-text.image", "Image" ).c_str(),
                newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec );
        const QString selectedFilter = theLocales.getLocalText("chat-toolbar.point-image.saveas.file-dialog.filter","Image Files") +
                " (*.png)";
        const QString newPath = QFileDialog::getSaveFileName(this, theLocales.getLocalText("chat-toolbar.point-image.saveas.file-dialog.caption","Save As"),
                                                             szFileName, selectedFilter );
        if (newPath.isEmpty()) {
            break;
        }
        else {
            /// 保存到文件
            const QVariant::Type imageResourceType = m_parameter2.type();
            switch (imageResourceType) {
            case QVariant::Pixmap: {
                const QPixmap pixmap = m_parameter2.value<QPixmap>();
                pixmap.save(newPath);
                break;
            }
            case QVariant::Image: {
                const QImage image = m_parameter2.value<QImage>();
                image.save(newPath);
                break;
            }
//            case QVariant::Icon: {
//                const QIcon image = imageResource.value<QIcon>();
//                rect.setWidth( image.width() );
//                rect.setHeight( image.height() );
//                break;
//            }
            case QVariant::Bitmap: {
                const QBitmap bitmap = m_parameter2.value<QBitmap>();
                bitmap.save(newPath);
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case EbFrameChatToolBar::PointFile: {
        /// 文件另存
        const QString filePath = m_parameter1.toString();
        if ( !QFile::exists(filePath) ) {
            EbMessageBox::doShow( 0,"", QChar::Null, theLocales.getLocalText("chat-msg-text.file-not-exist","File Not Exist"),
                                  EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
            break;
        }
        tstring fileName;
        tstring fileExt;
        libEbc::GetFileExt( filePath.toStdString().c_str(),fileName,fileExt );
        QString selectedFilter = theLocales.getLocalText("chat-toolbar.point-file.saveas.file-dialog.filter","All Files");
        if ( fileExt.empty() ) {
            selectedFilter += " (*)";
        }
        else {
            selectedFilter += " (*.";
            selectedFilter += fileExt.c_str();
            selectedFilter += ")";
        }
        /// 新文件
        QString newFilePath;
        while (true) {
            static int theIndex = 1;
            if (fileExt.empty())
                newFilePath = QString("%1(%2)").arg(fileName.c_str()).arg(theIndex++);
            else
                newFilePath = QString("%1(%2).%3").arg(fileName.c_str()).arg(theIndex++).arg(fileExt.c_str());
            if ( !QFile::exists(newFilePath) ) {
                break;
            }
        }
        const QString newPath = QFileDialog::getSaveFileName(this, theLocales.getLocalText("chat-toolbar.point-file.saveas.file-dialog.caption","Save As"),
                                                             newFilePath, selectedFilter );
        if ( newPath.isEmpty() || newPath==filePath ) {
            break;
        }
        else {
            /// 复制文件(另存)
            QFile::remove(newPath);
            if ( QFile::copy( filePath, newPath ) ) {
                EbMessageBox::doShow( 0,"", QChar::Null, theLocales.getLocalText("chat-msg-text.file-saveas-ok","File Saveas Ok"),
                                      EbMessageBox::IMAGE_INFORMATION,default_warning_auto_close );
            }
        }
        break;
    }
    case EbFrameChatToolBar::PointUserHead: {
        break;
    }
    default:
        return;
    }
    this->hide();

}

void EbFrameChatToolBar::onClickedButton4(bool /*checked*/)
{
    switch ( m_type ) {
    case EbFrameChatToolBar::SelectedText: {
        /// ---
        break;
    }
    case EbFrameChatToolBar::PointImage: {
        /// ---
        break;
    }
    case EbFrameChatToolBar::PointFile: {
        break;
    }
    case EbFrameChatToolBar::PointUserHead: {
        break;
    }
    default:
        return;
    }
    this->hide();

}

void EbFrameChatToolBar::onClickedButton5(bool /*checked*/)
{
    switch ( m_type ) {
    case EbFrameChatToolBar::SelectedText: {
        ///---
        break;
    }
    case EbFrameChatToolBar::PointImage: {
        /// ---
        break;
    }
    case EbFrameChatToolBar::PointFile: {
        break;
    }
    case EbFrameChatToolBar::PointUserHead: {
        break;
    }
    default:
        return;
    }
    this->hide();

}

void EbFrameChatToolBar::onClickedButton6(bool /*checked*/)
{
    switch ( m_type ) {
    case EbFrameChatToolBar::SelectedText: {
        ///---
        break;
    }
    case EbFrameChatToolBar::PointImage: {
        /// ---
        break;
    }
    case EbFrameChatToolBar::PointFile: {
        break;
    }
    case EbFrameChatToolBar::PointUserHead: {
        break;
    }
    default:
        return;
    }
}

void EbFrameChatToolBar::moveSize()
{
    int x = 6;
    for ( int i=0; i<m_showButtonCount; i++ ) {
        QPushButton * button = m_buttonList[i];
//        if ( !button->isVisible() ) {
//            /// 隐藏按钮，退出，不需要处理后面
//            break;
//        }
        const QString text = button->text();
        const int width = button->fontMetrics().width(text) + 6;
        button->setGeometry( x, 4, width, 22 );
        x += (width+6);
    }

    const int newWidth = x + 2;
    QRect rect = this->geometry();
    if ( rect.width()!=newWidth ) {
        rect.setWidth( newWidth );
        this->setGeometry( rect );
    }
}

void EbFrameChatToolBar::resizeEvent(QResizeEvent *e)
{
    moveSize();
    QFrame::resizeEvent(e);
}

void EbFrameChatToolBar::timerEvent(QTimerEvent *e)
{
    if ( m_timerIdCheckMousePos!=0 && m_timerIdCheckMousePos==e->timerId() ) {
        const QRect & rect = this->geometry();
        const QPoint pos = cursor().pos();
        if ( !this->isVisible() || !rect.contains( this->parentWidget()->mapFromGlobal(pos)) ) {
            this->killTimer( m_timerIdCheckMousePos );
            m_timerIdCheckMousePos = 0;
            this->hide();
        }

    }

    QFrame::timerEvent(e);
}

void EbFrameChatToolBar::hideAllButton()
{
    for ( int i=0; i<(int)m_buttonList.size(); i++ ) {
        QPushButton * button = m_buttonList[i];
        button->hide();
    }
}

void EbFrameChatToolBar::showHideButton()
{
    for ( int i=0; i<(int)m_buttonList.size(); i++ ) {
        QPushButton * button = m_buttonList[i];
        button->setVisible( (i<m_showButtonCount)?true:false );
    }
}

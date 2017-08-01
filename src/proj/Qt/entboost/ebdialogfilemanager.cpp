#include "ebdialogfilemanager.h"
#include "ui_ebdialogfilemanager.h"
#include <ebiconhelper.h>
#include <eblistwidgetitem.h>
#include <ebmessagebox.h>
#include <QTimer>

EbDialogFileManager::EbDialogFileManager(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogFileManager)
{
    ui->setupUi(this);
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint );
    this->showTitleBackground( theLocales.titleBackgroundHeight() );

    ui->pushButtonTraningFile->setCheckable(true);
    ui->pushButtonTraningFile->setObjectName("MsgCheckButton");
    connect( ui->pushButtonTraningFile,SIGNAL(clicked()),this,SLOT(onClickedButtonTraningFile()) );
    ui->pushButtonTranedFile->setCheckable(true);
    ui->pushButtonTranedFile->setObjectName("MsgCheckButton");
    connect( ui->pushButtonTranedFile,SIGNAL(clicked()),this,SLOT(onClickedButtonTranedFile()) );
    ui->lineEditSearch->setMaxLength(64);
    connect( ui->lineEditSearch,SIGNAL(textChanged(QString)),this,SLOT(onTextChangedSearch(QString)) );
    ui->pushButtonRefresh->setObjectName("BlackMainColorButton");
    connect( ui->pushButtonRefresh,SIGNAL(clicked()),this,SLOT(onClickedButtonRefresh()) );

    m_tranFiles = new EbWidgetFileTranList(this);
    m_tranFiles->setVisible(false);

    ui->listWidgetTranedFiles->setMouseTracking(true);
    ui->listWidgetTranedFiles->setObjectName("MessageTipList");
    connect( ui->listWidgetTranedFiles,SIGNAL(itemEntered(QListWidgetItem*)),this,SLOT(onItemEntered(QListWidgetItem*)) );
    connect( ui->listWidgetTranedFiles,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(onItemDoubleClicked(QListWidgetItem*)) );

    ui->pushButtonOpenFile->setParent(ui->listWidgetTranedFiles);
    EbIconHelper::Instance()->SetIcon(ui->pushButtonOpenFile,QChar(0xf016),12 );
    ui->pushButtonOpenFile->setObjectName("CallButton");
    ui->pushButtonOpenFile->setVisible(false);
    connect( ui->pushButtonOpenFile,SIGNAL(clicked()),this,SLOT(onClickedButtonOpenFile()) );
    ui->pushButtonOpenDir->setParent(ui->listWidgetTranedFiles);
    EbIconHelper::Instance()->SetIcon(ui->pushButtonOpenDir,QChar(0xf07c),12 );
    ui->pushButtonOpenDir->setObjectName("CallButton");
    ui->pushButtonOpenDir->setVisible(false);
    connect( ui->pushButtonOpenDir,SIGNAL(clicked()),this,SLOT(onClickedButtonOpenDir()) );
    ui->pushButtonDeleteFile->setParent(ui->listWidgetTranedFiles);
    EbIconHelper::Instance()->SetIcon(ui->pushButtonDeleteFile,QChar(0xf014),12 );
    ui->pushButtonDeleteFile->setObjectName("CallDelButton");
    ui->pushButtonDeleteFile->setVisible(false);
    connect( ui->pushButtonDeleteFile,SIGNAL(clicked()),this,SLOT(onClickedButtonDeleteFile()) );

    updateLocaleInfo();
    onClickedButtonTranedFile();
    onClickedButtonRefresh();
}

EbDialogFileManager::~EbDialogFileManager()
{
    delete ui;
}

void EbDialogFileManager::updateLocaleInfo()
{
    const QString sDialogTitle = theLocales.getLocalText("file-manager.title","File Manager");
    this->setWindowTitle( sDialogTitle );
    this->showTitleLogoText( sDialogTitle,const_common_title_font_size );

    /// 显示右上角关闭按钮
    EbDialogBase::showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    EbDialogBase::showPushButtonSysMax( theLocales.getLocalText("base-dialog.maximize-button.tooltip","Maximize") );
    EbDialogBase::showPushButtonSysMin( theLocales.getLocalText("base-dialog.minimize-button.tooltip","Minimize") );

    ui->pushButtonTraningFile->setText( theLocales.getLocalText("file-manager.button-traning-file.text","Tran File") );
    ui->pushButtonTraningFile->setToolTip( theLocales.getLocalText("file-manager.button-traning-file.tooltip","") );
    ui->pushButtonTranedFile->setText( theLocales.getLocalText("file-manager.button-traned-file.text","Received File") );
    ui->pushButtonTranedFile->setToolTip( theLocales.getLocalText("file-manager.button-traned-file.tooltip","") );
    ui->lineEditSearch->setPlaceholderText( theLocales.getLocalText("file-manager.edit-search.bg-text","Search File") );
    ui->pushButtonRefresh->setText( theLocales.getLocalText("file-manager.button-refresh.text","Refresh") );
    ui->pushButtonRefresh->setToolTip( theLocales.getLocalText("file-manager.button-refresh.tooltip","") );
    ui->pushButtonDeleteFile->setToolTip( theLocales.getLocalText("file-manager.button-delete-file.tooltip","Delete File") );
    ui->pushButtonOpenFile->setToolTip( theLocales.getLocalText("file-manager.button-open-file.tooltip","Open File") );
    ui->pushButtonOpenDir->setToolTip( theLocales.getLocalText("file-manager.button-open-dir.tooltip","Open Dir") );
}

void EbDialogFileManager::timerCheckState()
{
    if ( ui->listWidgetTranedFiles->isVisible() &&
         (ui->pushButtonOpenFile->isVisible() || ui->pushButtonOpenDir->isVisible() || ui->pushButtonDeleteFile->isVisible()) ) {
        /// 实现定期自动判断当前鼠标位置，并刷新 call/edit 按钮
        const QRect& rect = ui->listWidgetTranedFiles->geometry();
        const QPoint pointMouseToDialog = this->mapFromGlobal(this->cursor().pos());
        if (!rect.contains(pointMouseToDialog)) {
            ui->pushButtonOpenFile->setVisible(false);
            ui->pushButtonOpenDir->setVisible(false);
            ui->pushButtonDeleteFile->setVisible(false);
        }
    }
}

void EbDialogFileManager::onSendingFile(const CCrFileInfo *fileInfo)
{
    m_tranFiles->onSendingFile(fileInfo);
}

void EbDialogFileManager::onReceivingFile(const CCrFileInfo *fileInfo)
{
    m_tranFiles->onReceivingFile(fileInfo);
}

void EbDialogFileManager::onReceivedFile(const CCrFileInfo *fileInfo)
{
    deleteTranFile(fileInfo->m_nMsgId);
}

void EbDialogFileManager::onFilePercent(const CChatRoomFilePercent *filePercent)
{
    m_tranFiles->onFilePercent(filePercent);
}

void EbDialogFileManager::deleteTranFile(eb::bigint msgId)
{
    m_tranFiles->deleteTranFile(msgId);
}

bool EbDialogFileManager::isEmpty() const
{
    return m_tranFiles->isEmpty();
}

void EbDialogFileManager::updateWidgetInfo(bool traningFile)
{
    ui->pushButtonTraningFile->setChecked(traningFile);
    ui->pushButtonTranedFile->setChecked(!traningFile);
    ui->lineEditSearch->setVisible(!traningFile);
    ui->pushButtonRefresh->setVisible(!traningFile);

    m_tranFiles->setVisible(traningFile);
    ui->listWidgetTranedFiles->setVisible(!traningFile);

}

void EbDialogFileManager::onClickedButtonTraningFile()
{
    updateWidgetInfo(true);
}

void EbDialogFileManager::onClickedButtonTranedFile()
{
    updateWidgetInfo(false);
}

void EbDialogFileManager::onClickedButtonRefresh()
{
    QTimer::singleShot( 100,this,SLOT(onRefreshTranedFile()) );

}

void EbDialogFileManager::onRefreshTranedFile()
{
    m_pItemItemInfo.clear();
    ui->listWidgetTranedFiles->clear();
    const std::string sSearchString = ui->lineEditSearch->text().toStdString();
    char sSql[1024];
    if ( sSearchString.empty() ) {
        sprintf( sSql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_text FROM msg_record_t "
                       "WHERE msg_type=%d AND from_uid<>%lld ORDER BY msg_time desc",
                 (int)MRT_FILE,theApp->logonUserId() );  /// 不查找本端发送文件
    }
    else if (libEbc::IsFullNumber(sSearchString.c_str(),sSearchString.size())) {
        const eb::bigint nUserId = eb_atoi64(sSearchString.c_str());
        sprintf( sSql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_text FROM msg_record_t "
                       "WHERE msg_type=%d AND from_uid<>%lld AND (from_uid=%lld OR msg_text LIKE '%%%s%%' OR from_name LIKE '%%%s%%') ORDER BY msg_time desc",
                 (int)MRT_FILE,theApp->logonUserId(),nUserId,sSearchString.c_str(),sSearchString.c_str());	/// 不查找本端发送文件
    }
    else {
        tstring sSearchStringTemp(sSearchString);
        CSqliteCdbc::escape_string_in(sSearchStringTemp);
        sprintf( sSql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_text FROM msg_record_t "
                       "WHERE msg_type=%d AND from_uid<>%lld AND (msg_text LIKE '%%%s%%' OR from_name LIKE '%%%s%%') ORDER BY msg_time desc",
                 (int)MRT_FILE,theApp->logonUserId(),sSearchStringTemp.c_str(),sSearchStringTemp.c_str());	/// 不查找本端发送文件
    }

    int nCookie = 0;
    theApp->m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = theApp->m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        const tstring sMsgTime(pRecord->getVector()[0]->getStrValue());
        time_t nMsgTime = 0;
        libEbc::ChangeTime(sMsgTime.c_str(),nMsgTime);
        const eb::bigint sMsgId = pRecord->getVector()[1]->getBigIntValue();
//        const tstring soffTime(pRecord->getVector()[2]->getStrValue());
        const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
        const tstring sFromName(pRecord->getVector()[4]->getStrValue());
        //const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
        //const tstring sToName(libEbc::UTF82ACP(pRecord->getVector()[6]->getStrValue().c_str()));
//        const int nPrivate = pRecord->getVector()[7]->getIntValue();
        const tstring sMsgText(pRecord->getVector()[8]->getStrValue());
        pRecord = theApp->m_sqliteUser->next(nCookie);

        const tstring sFileName(libEbc::GetFileName(sMsgText));
        const QDateTime pTime = QDateTime::fromTime_t(nMsgTime);
        QString sItemName;
        int nSubType = 0;
        if ( QFile::exists(sMsgText.c_str()) ) {
            if (sFromName.empty())
                sItemName = QString(" %1\n %2").arg(sFileName.c_str()).arg(pTime.toString("yyyy-MM-dd HH:mm:ss"));
            else
                sItemName = QString(" %1\n %2 - %3").arg(sFileName.c_str()).arg(pTime.toString("yyyy-MM-dd HH:mm:ss")).arg(sFromName.c_str());
        }
        else {
            nSubType = -1;
            if (sFromName.empty()) {
                sItemName = QString(" %1(%2)\n %3").arg(sFileName.c_str()).arg(theLocales.getLocalText("chat-msg-text.file-not-exist","File not exist"))
                        .arg(pTime.toString("yyyy-MM-dd HH:mm:ss"));
            }
            else {
                sItemName = QString(" %1(%2)\n %3 - %4").arg(sFileName.c_str()).arg(theLocales.getLocalText("chat-msg-text.file-not-exist","File not exist"))
                        .arg(pTime.toString("yyyy-MM-dd HH:mm:ss")).arg(sFromName.c_str());
            }
        }

        EbWidgetItemInfo::pointer itemInfo;
        if (!m_pItemItemInfo.find(sMsgId,itemInfo)) {
            QFileInfo fileInfo(sMsgText.c_str());
            QFileIconProvider icon_provider;
            QIcon icon = fileInfo.exists()?icon_provider.icon(fileInfo):icon_provider.icon(QFileIconProvider::File);
            EbListWidgetItem * item = new EbListWidgetItem( icon, sItemName, ui->listWidgetTranedFiles );
            item->setToolTip(sItemName);
            ui->listWidgetTranedFiles->addItem(item);
            itemInfo = EbWidgetItemInfo::create(EbWidgetItemInfo::ITEM_TYPE_FILE,item);
            item->m_itemInfo = itemInfo;

            itemInfo->m_sId = sMsgId;
            itemInfo->m_nUserId  = sFromAccount;
            itemInfo->m_nBigId  = nMsgTime;
            m_pItemItemInfo.insert(sMsgId,itemInfo);
        }
        else {
            itemInfo->m_listItem->setText(sItemName);
            itemInfo->m_listItem->setToolTip(sItemName);
        }
        itemInfo->m_nSubType = nSubType;
        itemInfo->m_sName = sMsgText;	/// 文件路径
    }
    theApp->m_sqliteUser->reset(nCookie);
}

void EbDialogFileManager::onTextChangedSearch(const QString & text)
{
    static QString theSearchString;
    if (text==theSearchString) {
        return;
    }
    theSearchString = text;
    if (this->isVisible()) {
        /// 避免关闭窗口后，重新打开；
        onClickedButtonRefresh();
    }
}

void EbDialogFileManager::onItemDoubleClicked(QListWidgetItem *item)
{

}

void EbDialogFileManager::onItemEntered(QListWidgetItem *item)
{
    if (!ui->listWidgetTranedFiles->hasFocus()) {
        ui->listWidgetTranedFiles->setFocus();
    }
    /// 滚动条能正常显示
    const QRect rectItem = ui->listWidgetTranedFiles->visualItemRect(item);
//    const QPoint pointItem = ui->listWidgetTranedFiles->mapToParent(rectItem.topRight());
    const int y = rectItem.y();
    /// -2（配合下面的 y+1）实现删除按钮显示时，保留ITEM边框，
    const int buttonSize = rectItem.height()-2;
    ui->pushButtonDeleteFile->setGeometry( rectItem.right()-buttonSize,y+1,buttonSize,buttonSize );
    ui->pushButtonDeleteFile->setProperty("track-item",QVariant((quint64)item));
    ui->pushButtonDeleteFile->setVisible(true);
    ui->pushButtonOpenDir->setGeometry( rectItem.right()-buttonSize*2,y+1,buttonSize,buttonSize );
    ui->pushButtonOpenDir->setProperty("track-item",QVariant((quint64)item));
    ui->pushButtonOpenDir->setVisible(true);
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)item;
    const QString filePath = ebitem->m_itemInfo->m_sName.c_str();
    if ( QFile::exists(filePath) ) {
        ui->pushButtonOpenFile->setGeometry( rectItem.right()-buttonSize*3,y+1,buttonSize,buttonSize );
        ui->pushButtonOpenFile->setProperty("track-item",QVariant((quint64)item));
        ui->pushButtonOpenFile->setVisible(true);
    }
    else {
        ui->pushButtonOpenFile->setVisible(false);
    }

}

void EbDialogFileManager::onClickedButtonOpenFile()
{
    /// 打开文件
    bool ok = false;
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)ui->pushButtonOpenDir->property("track-item").toULongLong(&ok);
    if ( !ok || ebitem==0 ) {
        return;
    }
    ui->pushButtonOpenDir->setProperty("track-item",QVariant((qint64)0));

    const QString filePath(ebitem->m_itemInfo->m_sName.c_str());
    if ( QFileInfo::exists(filePath) ) {
        QDesktopServices::openUrl( QUrl::fromLocalFile(filePath) );
    }
}

void EbDialogFileManager::onClickedButtonOpenDir()
{
    /// 打开目录
    bool ok = false;
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)ui->pushButtonOpenDir->property("track-item").toULongLong(&ok);
    if ( !ok || ebitem==0 ) {
        return;
    }
    ui->pushButtonOpenDir->setProperty("track-item",QVariant((qint64)0));

    QString filePath(ebitem->m_itemInfo->m_sName.c_str());
    const QFileInfo fileInfo(filePath);
    if ( !fileInfo.exists() ) {
        QDesktopServices::openUrl( QUrl(filePath, QUrl::TolerantMode) );
    }
    else {
#ifdef WIN32
        filePath.replace("/","\\");
        const QString param = "/select, \""+filePath+"\"";
        QProcess::startDetached("explorer "+param);
#else
        QDesktopServices::openUrl( QUrl(filePath, QUrl::TolerantMode) );
#endif
    }
}

void EbDialogFileManager::onClickedButtonDeleteFile()
{
    /// 删除消息（包括文件）
    bool ok = false;
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)ui->pushButtonDeleteFile->property("track-item").toULongLong(&ok);
    if ( !ok || ebitem==0 ) {
        return;
    }
    ui->pushButtonDeleteFile->setProperty("track-item",QVariant((qint64)0));

    const QString filePath = ebitem->m_itemInfo->m_sName.c_str();
    if ( QFile::exists(filePath) ) {
        QString title;
        QString text;
        if (ebitem->m_itemInfo->m_nUserId==ebitem->m_itemInfo->m_sId) {
            /// 确定删除文件：\r\n%s 和下载记录吗？
            title = theLocales.getLocalText("message-box.delete-download-file.title","Delete File");
            text = theLocales.getLocalText("message-box.delete-download-file.text","Confirm delete:<br>[FILE_NAME] download file?");
        }
        else {
            /// 确定删除文件：\r\n%s 和聊天记录吗？
            title = theLocales.getLocalText("message-box.delete-chat-file.title","Delete File");
            text = theLocales.getLocalText("message-box.delete-chat-file.text","Confirm delete:<br>[FILE_NAME] chat file?");
        }
        if (EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )!=QDialog::Accepted) {
            return;
        }
        QFile::remove(filePath);
    }

    const eb::bigint msgId = ebitem->m_itemInfo->m_sId;
    m_pItemItemInfo.remove(msgId);
    QListWidgetItem * item = ui->listWidgetTranedFiles->takeItem( ui->listWidgetTranedFiles->row(ebitem) );
    if (item!=0) {
        delete item;
    }
    char sql[128];
    sprintf( sql, "DELETE FROM msg_record_t WHERE msg_id=%lld",msgId);
    theApp->m_sqliteUser->execute(sql);
}

void EbDialogFileManager::resizeEvent(QResizeEvent *e)
{
    const int const_left_intever	= 1;
    const int const_tab_width1 = 60;
    const int const_tab_width2 = 36;
    const int const_tab_height = 22;

    int x = 10;//const_left_intever+5;
    int y = 50;	// 42
    ui->pushButtonTraningFile->setGeometry(x, y,const_tab_width1,const_tab_height);
    x += const_tab_width1+5;
    ui->pushButtonTranedFile->setGeometry(x, y,const_tab_width1,const_tab_height);
    x += const_tab_width1+5;
    ui->lineEditSearch->setGeometry(x,y,120,22);
    x = width()-const_tab_width2-const_left_intever-5;
    ui->pushButtonRefresh->setGeometry(x,y,const_tab_width2,const_tab_height);
    y = 80;
    //y += 30;
    QRect rect(const_left_intever,y,width()-const_left_intever,height()-20);
    this->m_tranFiles->setGeometry(rect);
    ui->listWidgetTranedFiles->setGeometry(rect);

    EbDialogBase::resizeEvent(e);
}


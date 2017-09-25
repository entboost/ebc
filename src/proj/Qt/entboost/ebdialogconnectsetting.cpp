#include "ebdialogconnectsetting.h"
#include "ui_ebdialogconnectsetting.h"
#include "ebclientapp.h"

EbDialogConnectSetting::EbDialogConnectSetting(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogConnectSetting)
{
    ui->setupUi(this);
    /// 设置初始大小
    this->resize(400,160);
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );

    const QString sDialogTitle = theLocales.getLocalText("connect-setting-dialog.title","Connect Setting");
    this->setWindowTitle( sDialogTitle );
    this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf0ec),const_common_title_image_font_size );
//    this->showTitleLogoText( sDialogTitle,12,QChar(0xf013),10 );
//    this->showTitleLogoText( sDialogTitle,12,QChar(0xf069),10 );
    this->showTitleBackground( theLocales.titleBackgroundHeight() );
    /// 显示右上角关闭按钮
    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    /// 显示“确定/取消”按钮
    this->showPushButtonStdOkCancel( theLocales.getLocalText("base-dialog.ok-button.text","Ok"), "OkButton",
                                     theLocales.getLocalText("base-dialog.cancel-button.text","Cancel"), "CancelButton" );

    const int y = 61;
    ui->labelServerAddress->resize(80, 21);
    ui->labelServerAddress->move(35, y);
    ui->labelServerAddress->setText( theLocales.getLocalText("connect-setting-dialog.label-server-address.text","Server Address:") );
    ui->lineEditServerAddress->resize(215, 21);
    ui->lineEditServerAddress->move(123, y);
    /// 格式：IP:PORT（域名:PORT）；例如：192.168.10.100:18012
    ui->lineEditServerAddress->setToolTip( theLocales.getLocalText("connect-setting-dialog.edit-server-address.tooltip","") );
    /// 留空默认连接公有云环境！
    ui->lineEditServerAddress->setPlaceholderText( theLocales.getLocalText("connect-setting-dialog.edit-server-address.bg-text","") );

    //
    const QString ebcIniFileServer = theApp->ebcIniFileServer();
    if (!ebcIniFileServer.isEmpty()) {
        m_oldServer = ebcIniFileServer;
        ui->lineEditServerAddress->setText(m_oldServer);
    }
//    const QString& sEbcIniFileSetting = theApp->getEbcIniFile();
//    if (QFile::exists(sEbcIniFileSetting)) {
//        char lpszBuffer[512];
//        memset(lpszBuffer,0,sizeof(lpszBuffer));
//        GetPrivateProfileStringABoost("system","server","",lpszBuffer,sizeof(lpszBuffer),sEbcIniFileSetting.toLocal8Bit().constData());
//        m_oldServer = QString::fromUtf8(lpszBuffer);
//        ui->lineEditServerAddress->setText(m_oldServer);
//    }
}

EbDialogConnectSetting::~EbDialogConnectSetting()
{
    delete ui;
}

void EbDialogConnectSetting::accept(void)
{
    m_newServer = ui->lineEditServerAddress->text();
    if (m_oldServer==m_newServer) {
        ui->lineEditServerAddress->setFocus();
        return;
    }
    theApp->setEbcIniFileServer(m_newServer);
//    const QString& sEbcIniFileSetting = theApp->getEbcIniFile();
//    WritePrivateProfileStringABoost("system", "server", m_newServer.toStdString().c_str(), sEbcIniFileSetting.toLocal8Bit().constData());
    EbDialogBase::accept();
}

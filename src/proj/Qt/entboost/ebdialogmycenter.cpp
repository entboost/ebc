#include "ebdialogmycenter.h"
#include "ui_ebdialogmycenter.h"
#include "ebwidgetaccountinfo.h"
#include "ebwidgetmysetting.h"
#include "ebwidgetmodifypassword.h"
#include "ebiconhelper.h"

EbDialogMyCenter::EbDialogMyCenter(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogMyCenter)
  , m_labelAccountInfoIcon(0)
  , m_labelModifyPasswordIcon(0)
  , m_widgetAccountInfo(0)
  , m_widgetMySetting(0)
  , m_widgetModifyPassword(0)
{
    ui->setupUi(this);
    /// 设置初始大小
    this->resize(588,452);
#ifdef __MACH__
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::WindowMinimizeButtonHint|Qt::CustomizeWindowHint);
#else
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );
#endif

    const int const_title_height = 80;
    this->showTitleBackground( const_title_height );
    updateLocaleInfo();

    /// 帐号信息
    m_labelAccountInfoIcon = new QLabel(ui->pushButtonAccountInfo);
    m_labelAccountInfoIcon->setObjectName("IconLabel");
    EbIconHelper::Instance()->SetIcon(m_labelAccountInfoIcon,QChar(0xf2c3),10);
    m_labelAccountInfoIcon->setVisible(true);
    m_labelAccountInfoIcon->setGeometry( 7,13,16,16 );
    const QSize const_check_button_size(86,39+3);   /// 49+3 (3主要用于下面圆角不显示)
    ui->pushButtonAccountInfo->setCheckable(true);
    ui->pushButtonAccountInfo->setObjectName("TitleCheckButton");
    int x = 14;
    int y = 41;
    ui->pushButtonAccountInfo->setGeometry( x,y,const_check_button_size.width(),const_check_button_size.height() );
    connect( ui->pushButtonAccountInfo,SIGNAL(clicked()),this,SLOT(onClickedPushButtonAccountInfo()) );
    /// 个人设置
    ui->pushButtonMySetting->hide();
//    ui->pushButtonMySetting->setCheckable(true);
//    ui->pushButtonMySetting->setObjectName("TitleCheckButton");
//    x += const_check_button_size.width();
//    ui->pushButtonMySetting->setGeometry( x,y,const_check_button_size.width(),const_check_button_size.height() );
//    connect( ui->pushButtonMySetting,SIGNAL(clicked()),this,SLOT(onClickedPushButtonMySetting()) );
    /// 修改密码
    m_labelModifyPasswordIcon = new QLabel(ui->pushButtonModifyPassword);
    m_labelModifyPasswordIcon->setObjectName("IconLabel");
    EbIconHelper::Instance()->SetIcon(m_labelModifyPasswordIcon,QChar(0xf023),10);
    m_labelModifyPasswordIcon->setVisible(true);
    m_labelModifyPasswordIcon->setGeometry( 7,13,16,16 );
    ui->pushButtonModifyPassword->setCheckable(true);
    ui->pushButtonModifyPassword->setObjectName("TitleCheckButton");
    if ( theApp->isDisableModifyPasswd() ) {
        /// 禁止客户端修改密码
        ui->pushButtonModifyPassword->hide();
        m_labelModifyPasswordIcon->hide();
    }
    else {
        /// 支持客户端修改密码
        x += const_check_button_size.width();
        ui->pushButtonModifyPassword->setGeometry( x,y,const_check_button_size.width(),const_check_button_size.height() );
        connect( ui->pushButtonModifyPassword,SIGNAL(clicked()),this,SLOT(onClickedPushButtonModifyPassword()) );
    }

    /// 默认名片
    ui->pushButtonDefaultMember->setObjectName("RequestAddContact");
    ui->pushButtonDefaultMember->setGeometry(20, height()-42, 60, 22);
    connect( ui->pushButtonDefaultMember, SIGNAL(clicked()), this, SLOT(onClickedPushButtonDefaultMember()) );
    eb::bigint nMyDefaultmemberCode = 0;
    if (!theApp->m_ebum.EB_GetMyDefaultMemberCode(nMyDefaultmemberCode) || nMyDefaultmemberCode==0) {
        ui->pushButtonDefaultMember->setVisible(false);
    }

    /// 默认选择第一个
    onClickedPushButtonAccountInfo();

}

EbDialogMyCenter::~EbDialogMyCenter()
{
    delete ui;
}

void EbDialogMyCenter::updateLocaleInfo()
{
    const QString sDialogTitle = theLocales.getLocalText("my-center-dialog.title","My Center");
    this->setWindowTitle( sDialogTitle );
    this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf007),const_common_title_image_font_size );
    /// 显示右上角关闭按钮
    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    /// 显示 “保存/关闭”按钮
    this->showPushButtonStdOkCancel( theLocales.getLocalText("base-dialog.save-button.text","Save"), "OkButton",
                                     theLocales.getLocalText("base-dialog.close-button.text","Close"), "CancelButton" );
    /// 帐号信息
    ui->pushButtonAccountInfo->setText(  theLocales.getLocalText("my-center-dialog.button-account-info.text","Account Info") );
    ui->pushButtonAccountInfo->setToolTip( theLocales.getLocalText("my-center-dialog.button-account-info.tooltip","") );
    /// 个人设置
    ui->pushButtonMySetting->setText(  theLocales.getLocalText("my-center-dialog.button-my-setting.text","My Setting") );
    ui->pushButtonMySetting->setToolTip( theLocales.getLocalText("my-center-dialog.button-my-setting.tooltip","") );
    /// 修改密码
    ui->pushButtonModifyPassword->setText(  theLocales.getLocalText("my-center-dialog.button-modify-password.text","Modify Password") );
    ui->pushButtonModifyPassword->setToolTip( theLocales.getLocalText("my-center-dialog.button-modify-password.tooltip","") );

    /// 默认名片
    ui->pushButtonDefaultMember->setText(  theLocales.getLocalText("my-center-dialog.button-default-member.text","Defalt Member") );
    ui->pushButtonDefaultMember->setToolTip( theLocales.getLocalText("my-center-dialog.button-default-member.tooltip","") );

}

void EbDialogMyCenter::onAreaInfo(const EB_AreaInfo *pAreaInfo, int nParameter)
{
    if (m_widgetAccountInfo!=0)
        m_widgetAccountInfo->onAreaInfo(pAreaInfo,nParameter);
}

void EbDialogMyCenter::onClickedPushButtonAccountInfo()
{
    if (m_widgetAccountInfo==0) {
        m_widgetAccountInfo = new EbWidgetAccountInfo(this);
        const int x = 28;
        const int y = ui->pushButtonAccountInfo->geometry().bottom();
        m_widgetAccountInfo->setGeometry( x,y,width()-x*2,height()-y-50 );  /// 50最下面间隔
    }
    m_widgetAccountInfo->setFocus();
    udpateClickedPushButton( sender() );
}

void EbDialogMyCenter::onClickedPushButtonMySetting()
{
//    udpateClickedPushButton( sender() );
//    if (m_widgetMySetting==0) {
//        m_widgetMySetting = new EbWidgetMySetting(this);
//        const int x = 28;
//        const int y = ui->pushButtonAccountInfo->geometry().bottom();
//        m_widgetMySetting->setGeometry( x,y,width()-x*2,height()-y-50 );  /// 50最下面间隔
//    }
//    m_widgetMySetting->setFocus();
}

void EbDialogMyCenter::onClickedPushButtonModifyPassword()
{
    if (m_widgetModifyPassword==0) {
        m_widgetModifyPassword = new EbWidgetModifyPassword(this);
        const int x = 28;
        const int y = ui->pushButtonAccountInfo->geometry().bottom();
        m_widgetModifyPassword->setGeometry( x,y,width()-x*2,height()-y-50 );  /// 50最下面间隔
    }
    m_widgetModifyPassword->setFocus();
    udpateClickedPushButton( sender() );
}

void EbDialogMyCenter::udpateClickedPushButton(const QObject *sender)
{
    ui->pushButtonAccountInfo->setChecked( (sender==0||sender==ui->pushButtonAccountInfo)?true:false );
    if (m_widgetAccountInfo!=0)
        m_widgetAccountInfo->setVisible( (sender==0||sender==ui->pushButtonAccountInfo)?true:false );
    ui->pushButtonMySetting->setChecked( sender==ui->pushButtonMySetting?true:false );
    if (m_widgetMySetting!=0)
        m_widgetMySetting->setVisible( sender==ui->pushButtonMySetting?true:false );
    ui->pushButtonModifyPassword->setChecked( sender==ui->pushButtonModifyPassword?true:false );
    if (m_widgetModifyPassword!=0)
        m_widgetModifyPassword->setVisible( sender==ui->pushButtonModifyPassword?true:false );
}

void EbDialogMyCenter::onClickedPushButtonDefaultMember()
{
    EB_MemberInfo pMemberInfo;
    if (theApp->m_ebum.EB_GetMyDefaultMemberInfo(&pMemberInfo)) {
        theApp->editMemberInfo(&pMemberInfo);
    }
}

void EbDialogMyCenter::accept()
{
    if ( ui->pushButtonAccountInfo->isChecked() ) {
        /// 保存帐号信息
        m_widgetAccountInfo->save();
    }
//    else if ( ui->pushButtonMySetting->isChecked() && m_widgetMySetting!=0 ) {
//        /// 修改个人设置
//        m_pDlgEditAccSetting->Save();
//    }
    else if ( ui->pushButtonModifyPassword->isChecked() && m_widgetModifyPassword!=0 ) {
        /// 修改密码
        m_widgetModifyPassword->save();
    }
}

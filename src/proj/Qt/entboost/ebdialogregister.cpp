#include "ebdialogregister.h"
#include "ui_ebdialogregister.h"
#include "ebclientapp.h"
#include "ebmessagebox.h"

EbDialogRegister::EbDialogRegister(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogRegister)

  , m_nRegisterUserId(0)
{
    ui->setupUi(this);

    this->resize(480,340);
    // 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );

    const QString sDialogTitle = theLocales.getLocalText("register-dialog.title","Register Account");
    this->setWindowTitle( sDialogTitle );
    this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf234),const_common_title_image_font_size );
    this->showTitleBackground( theLocales.titleBackgroundHeight() );
    // 显示右上角关闭按钮
    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    // 显示 “注册/取消” 按钮
    this->showPushButtonStdOkCancel( theLocales.getLocalText("base-dialog.register-button.text","Register"), "OkButton",
                                     theLocales.getLocalText("base-dialog.cancel-button.text","Cancel"), "CancelButton" );

    ui->labelAccount->setText( theLocales.getLocalText("register-dialog.label-account.text","Account:") );
    ui->labelPassword->setText( theLocales.getLocalText("register-dialog.label-password.text","Password:") );
    ui->labelPwdConfirm->setText( theLocales.getLocalText("register-dialog.label-confirm.text","Confirm:") );
    ui->labelUserName->setText( theLocales.getLocalText("register-dialog.label-user-name.text","User Name:") );
    ui->labelEntName->setText( theLocales.getLocalText("register-dialog.label-ent-name.text","Ent Name:") );
    const int const_x1	= 52;
    const int const_x2	= 140;
    const int const_static_width	= 80;
    const int const_edit_width		= 245;
    const int const_height_interval	= 43;
    int y = 61;
//    ui->lineEditAccount->
    ui->labelAccount->setGeometry( const_x1,y,const_static_width,const_common_edit_height );
    ui->lineEditAccount->setGeometry( const_x2,y,const_edit_width,const_common_edit_height );
    y += const_height_interval;
    ui->labelPassword->setGeometry( const_x1,y,const_static_width,const_common_edit_height );
    ui->lineEditPassword->setGeometry( const_x2,y,const_edit_width,const_common_edit_height );
    y += const_height_interval;
    ui->labelPwdConfirm->setGeometry( const_x1,y,const_static_width,const_common_edit_height );
    ui->lineEditPwdConfirm->setGeometry( const_x2,y,const_edit_width,const_common_edit_height );
    y += const_height_interval;
    ui->labelUserName->setGeometry( const_x1,y,const_static_width,const_common_edit_height );
    ui->lineEditUserName->setGeometry( const_x2,y,const_edit_width,const_common_edit_height );
    y += const_height_interval;
    ui->labelEntName->setGeometry( const_x1,y,const_static_width,const_common_edit_height );
    ui->lineEditEntName->setGeometry( const_x2,y,const_edit_width,const_common_edit_height );

}

EbDialogRegister::~EbDialogRegister()
{
    m_pEBAppClient.EB_UnInit();
    delete ui;
}

void EbDialogRegister::accept(void)
{
    m_sAccount = ui->lineEditAccount->text();
    if (m_sAccount.isEmpty()) {
        ui->lineEditAccount->setFocus();
        /// 帐号不能为空：<br>请重新输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.account-empty-error","Account Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    m_sPassword = ui->lineEditPassword->text();
    if (m_sPassword.isEmpty()) {
        ui->lineEditPassword->setFocus();
        /// 密码不能为空：<br>请输入密码！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.password-empty","Password Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    m_sConfirmPwd = ui->lineEditPwdConfirm->text();
    if (m_sConfirmPwd.isEmpty()) {
        ui->lineEditPwdConfirm->setFocus();
        /// 确认密码不能为空：<br>请输入确认密码！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.confirm-pwd-empty","Confirm Password Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    if (m_sPassword != m_sConfirmPwd) {
        ui->lineEditPassword->setFocus();
        ui->lineEditPassword->selectAll();
        /// 密码不一致：<br>请重新输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.password-not-equal","Password Not Equal" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }

    const std::string sAddress = theApp->m_setting.GetServerAddress();
    m_pEBAppClient.EB_Init(sAddress.c_str());
    m_pEBAppClient.EB_SetMsgReceiver(this);
    if (!m_pEBAppClient.EB_IsInited()) {
        /// 本地服务启动失败：<br>请重试！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.ebum-start-error","EBUM Start Error" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }

    EbDialogBase::setButtonOkEnabled(false);

    m_sUserName = ui->lineEditUserName->text();
    m_sEnterpriseName = ui->lineEditEntName->text();
    const eb::bigint sAppId = theApp->m_ebum.EB_GetDevAppId();
    const tstring sAppOnlineKey = theApp->m_ebum.EB_GetDevAppOnlineKey();
    m_pEBAppClient.EB_SetDevAppOnlineKey(sAppId,sAppOnlineKey.c_str());
    m_pEBAppClient.EB_Register(m_sAccount.toStdString().c_str(),m_sPassword.toStdString().c_str(),
                               m_sUserName.toStdString().c_str(),m_sEnterpriseName.toStdString().c_str());
    return;

//    if (m_oldServer==m_newServer) {
//        ui->lineEditServerAddress->setFocus();
//        return false;
//    }
//    const QString& sEbcIniFileSetting = theApp->getEbcIniFile();
//    WritePrivateProfileStringABoost("system","server",m_newServer.toStdString().c_str(),sEbcIniFileSetting.toStdString().c_str());
//    return true;
    EbDialogBase::accept();
}

void EbDialogRegister::customEvent(QEvent *e)
{
    const QEvent::Type eventType = e->type();
    switch ((int)eventType) {
    case EB_WM_REGISTER_RESPONSE:
        onRegisterResponse(e);
        break;
    default:
        break;
    }

}

void EbDialogRegister::onRegisterResponse(QEvent *e)
{
    EbDialogBase::setButtonOkEnabled(true);
    ui->lineEditAccount->setFocus();
//    ui->lineEditAccount->selectAll();
    const EB_Event * pEvent = (EB_Event*)e;
    const EB_STATE_CODE nStateCode = (EB_STATE_CODE)pEvent->GetEventParameter();
    QString sMessageText;
    switch(nStateCode) {
    case EB_STATE_NOT_AUTH_ERROR: {
        const std::string sAccount = m_sAccount.toStdString();
        if (libEbc::IsFullNumber(sAccount.c_str(),sAccount.size()) )
            /// 不支持全数字帐号注册：<br>请更换帐号后重试！
            sMessageText = theLocales.getLocalText( "on-register-response.number-not-auth.text","EB_STATE_NOT_AUTH_ERROR" );
        else
            sMessageText = theLocales.getLocalText( "on-register-response.not-auth-error.text","EB_STATE_NOT_AUTH_ERROR" );
        break;
    }
    case EB_STATE_CONTENT_BIG_LENGTH:
        // 字段内容超过最大长度：<br>请重新输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "on-register-response.content-big-length.text","EB_STATE_CONTENT_BIG_LENGTH" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        break;
    case EB_STATE_OK: {
        m_nRegisterUserId = pEvent->GetEventBigParameter();
        if (theApp->sendRegMail()) {
            // 注册成功：<br>请及时查阅邮件，完成帐号验证！
            sMessageText = theLocales.getLocalText( "on-register-response.ok-send-mail.text","EB_STATE_OK" );
        }
        else {
            sMessageText = theLocales.getLocalText( "on-register-response.ok-login.text","EB_STATE_OK" );
//            if (m_sAccount.isEmpty()) {
//                sText = QString("注册成功：\r\n请使用 %1 登录！").arg(m_nRegisterUserId);
//            }
//            else {
//                sText = QString("注册成功：\r\n请使用 %1 或 %2 登录！").arg(m_sAccount).arg(m_nRegisterUserId);
//            }
        }
        sMessageText.replace( "[ACCOUNT]", m_sAccount );
        char lpszUserId[24];
        sprintf(lpszUserId,"%lld",m_nRegisterUserId);
        sMessageText.replace( "[USERID]", lpszUserId );
        EbMessageBox::doExec( NULL, this->windowTitle(), QChar::Null, sMessageText, EbMessageBox::IMAGE_INFORMATION,0,QMessageBox::Ok );
        EbDialogBase::accept();
        return;
    }
    case EB_STATE_ACCOUNT_ALREADY_EXIST:
        // 帐号已经存在：<br>请更换帐号后重试！
        sMessageText = theLocales.getLocalText( "on-register-response.account-already-exist.text","EB_STATE_ACCOUNT_ALREADY_EXIST" );
        break;
    case EB_STATE_ENTERPRISE_ALREADY_EXIST:
        // 公司名称已经被注册：<br>请确认后重试！
        sMessageText = theLocales.getLocalText( "on-register-response.enterprise-already-exist.text","EB_STATE_ENTERPRISE_ALREADY_EXIST" );
        break;
    case EB_STATE_APPID_KEY_ERROR:
        // APPID KEY错误：<br>请重试或联系公司客服！
        sMessageText = theLocales.getLocalText( "on-register-response.appid-key-error.text","EB_STATE_APPID_KEY_ERROR" );
        break;
    case EB_STATE_DISABLE_REGISTER_USER:
        // 禁止用户注册功能：<br>请联系公司客服！
        sMessageText = theLocales.getLocalText( "on-register-response.disable-register-account.text","EB_STATE_DISABLE_REGISTER_USER" );
        break;
    case EB_STATE_DISABLE_REGISTER_ENT:
        sMessageText = theLocales.getLocalText( "on-register-response.disable-register-ent.text","EB_STATE_DISABLE_REGISTER_ENT" );
        break;
    case EB_STATE_ERROR:
        // 注册失败：<br>请重试！
        sMessageText = theLocales.getLocalText( "on-register-response.error.text","EB_STATE_ERROR" );
        break;
    default:
        // 注册失败，请重试或联系公司客服：<br>错误代码：[STATE_CODE]
        sMessageText = theLocales.getLocalText( "on-register-response.other-error.text","Other Error" );
        break;
    }
    sMessageText.replace( "[STATE_CODE]",QString::number((int)nStateCode));
    EbMessageBox::doShow( NULL, "", QChar::Null, sMessageText, EbMessageBox::IMAGE_WARNING,default_warning_auto_close );

}

#include "ebwidgetmodifypassword.h"
#include "ebmessagebox.h"

EbWidgetModifyPassword::EbWidgetModifyPassword(QWidget *parent) : QWidget(parent)
{
    const int const_y	= 18;
    const int const_x1	= 45-28;	// 28是父窗口左边间隔
    //const int const_x2	= 133-28;
    const int const_static_width	= 80;
    const int const_edit_width		= 315;
    const int const_edit_height		= 21;
    const int const_height_interval	= 33;
    int nX = const_x1;
    int nY = const_y;
    m_labelCurrentPassword = new QLabel(this);
    m_labelCurrentPassword->show();
    m_labelCurrentPassword->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditCurrentPassword = new QLineEdit(this);
    m_lineEditCurrentPassword->show();
    m_lineEditCurrentPassword->setFocus();
    m_lineEditCurrentPassword->setEchoMode(QLineEdit::Password);
    m_lineEditCurrentPassword->setGeometry(nX,nY,const_edit_width,const_edit_height);
    nX = const_x1;
    nY += const_height_interval;
    m_labelNewPassword = new QLabel(this);
    m_labelNewPassword->show();
    m_labelNewPassword->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditNewPassword = new QLineEdit(this);
    m_lineEditNewPassword->show();
    m_lineEditNewPassword->setEchoMode(QLineEdit::Password);
    m_lineEditNewPassword->setGeometry(nX,nY,const_edit_width,const_edit_height);
    nX = const_x1;
    nY += const_height_interval;
    m_labelConfirmPassword = new QLabel(this);
    m_labelConfirmPassword->show();
    m_labelConfirmPassword->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditConfirmPassword = new QLineEdit(this);
    m_lineEditConfirmPassword->show();
    m_lineEditConfirmPassword->setEchoMode(QLineEdit::Password);
    m_lineEditConfirmPassword->setGeometry(nX,nY,const_edit_width,const_edit_height);

    updateLocaleInfo();
}

void EbWidgetModifyPassword::updateLocaleInfo()
{
    m_labelCurrentPassword->setText( theLocales.getLocalText("my-center-dialog.modify-password-widget.label-current-password.text","Current Password:") );
    m_labelNewPassword->setText( theLocales.getLocalText("my-center-dialog.modify-password-widget.label-new-password.text","New Password:") );
    m_labelConfirmPassword->setText( theLocales.getLocalText("my-center-dialog.modify-password-widget.label-confirm-password.text","Confirm Password:") );

}

void EbWidgetModifyPassword::setFocus()
{
    QWidget::setFocus();
    m_lineEditCurrentPassword->setFocus();
}

void EbWidgetModifyPassword::save()
{
    if ( theApp->isDisableModifyPasswd() ) {
        return;
    }
    const QString sCurrentPwd = m_lineEditCurrentPassword->text();
    if (sCurrentPwd.isEmpty()) {
        m_lineEditCurrentPassword->setFocus();
        /// 当前密码不能为空：\r\n请输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.current-pwd-empty","Current Password Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    const QString sNewPwd = m_lineEditNewPassword->text();
    const QString sConfirm = m_lineEditConfirmPassword->text();
    if (sNewPwd.isEmpty()) {
        m_lineEditNewPassword->setFocus();
        /// 新密码不能为空：\r\n请输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.new-pwd-empty","New Password Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    else if (sConfirm.isEmpty()) {
        m_lineEditConfirmPassword->setFocus();
        /// 确认密码不能为空：\r\n请输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.confirm-pwd-empty","Confirm Password Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    else if (sNewPwd!=sConfirm) {
        m_lineEditNewPassword->setFocus();
        m_lineEditNewPassword->selectAll();
        /// 新密码不一致：\r\n请重新输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.password-not-equal","Password Not Equal" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    else if (sCurrentPwd==sNewPwd) {
        /// 新密码跟当前密码一样：\r\n请重新输入！
        m_lineEditNewPassword->setFocus();
        m_lineEditNewPassword->selectAll();
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.password-equal-warning","Password Equal Warning" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    theApp->m_ebum.EB_SetPassword( sCurrentPwd.toStdString().c_str(),sNewPwd.toStdString().c_str() );
    /// *** 不保存用户数据，保存OAuthKey实现自动登录
    /// *** 兼容旧服务端版本 1.24.2 以前版本； v1.25.0.544, 2017-04-11
    /// *** v1.25.0 以上版本，客户端不保存密码，只保存本地硬件唯一标识，实现自动登录，提高安全性
    if (theApp->ebServerVersion()==0) {
        const QString sSql = QString("update user_login_record_t set password='%1' where (account='%2' OR user_id=%3) AND password='%4'")
                .arg(sNewPwd).arg(theApp->logonUserAccount().c_str()).arg(theApp->logonUserId()).arg(sCurrentPwd);
        theApp->m_sqliteEbc->execute(sSql.toStdString().c_str());
    }
    m_lineEditCurrentPassword->setText("");
}

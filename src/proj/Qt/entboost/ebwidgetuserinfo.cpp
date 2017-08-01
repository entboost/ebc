#include "ebwidgetuserinfo.h"
#include <ebdialogrequestaddcontact.h>

const QSize const_user_image_size(80,80);
EbWidgetUserInfo::EbWidgetUserInfo(const EbcCallInfo::pointer &callInfo, QWidget *parent) : QWidget(parent)
  , m_callInfo(callInfo)
{
    assert (m_callInfo.get()!=0);
    m_labelImage = new QLabel(this);
    m_labelImage->resize(const_user_image_size);
    const QString imageFile = theApp->userHeadFilePath(
                m_callInfo->fromUserId(), m_callInfo->fromMemberId(),
                m_callInfo->fromUserAccount() );
    m_labelImage->setPixmap( QPixmap(imageFile).scaled(const_user_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    m_labelUserName = new QLabel(this);
    m_lineEditUserName = new QLineEdit(this);
    m_lineEditUserName->setObjectName("UserInfoEdit");
    m_lineEditUserName->setReadOnly(true);
    m_lineEditUserName->setFrame(false);
    m_labelAccount = new QLabel(this);
    m_lineEditAccount = new QLineEdit(this);
    m_lineEditAccount->setObjectName("UserInfoEdit");
    m_lineEditAccount->setReadOnly(true);
    m_lineEditAccount->setFrame(false);
    m_labelTel = new QLabel(this);
    m_lineEditTel = new QLineEdit(this);
    m_lineEditTel->setObjectName("UserInfoEdit");
    m_lineEditTel->setReadOnly(true);
    m_lineEditTel->setFrame(false);
    m_labelPhone = new QLabel(this);
    m_lineEditPhone = new QLineEdit(this);
    m_lineEditPhone->setObjectName("UserInfoEdit");
    m_lineEditPhone->setReadOnly(true);
    m_lineEditPhone->setFrame(false);
    m_labelEmail = new QLabel(this);
    m_lineEditEmail = new QLineEdit(this);
    m_lineEditEmail->setObjectName("UserInfoEdit");
    m_lineEditEmail->setReadOnly(true);
    m_lineEditEmail->setFrame(false);
    m_labelJobTitle = new QLabel(this);
    m_lineEditJobTitle = new QLineEdit(this);
    m_lineEditJobTitle->setObjectName("UserInfoEdit");
    m_lineEditJobTitle->setReadOnly(true);
    m_lineEditJobTitle->setFrame(false);
    m_labelGroupName = new QLabel(this);
    m_lineEditGroupName = new QLineEdit(this);
    m_lineEditGroupName->setObjectName("UserInfoEdit");
    m_lineEditGroupName->setReadOnly(true);
    m_lineEditGroupName->setFrame(false);
    m_labelEnterprise = new QLabel(this);
    m_lineEditEnterprise = new QLineEdit(this);
    m_lineEditEnterprise->setObjectName("UserInfoEdit");
    m_lineEditEnterprise->setReadOnly(true);
    m_lineEditEnterprise->setFrame(false);
    m_labelAddress = new QLabel(this);
    m_lineEditAddress = new QLineEdit(this);
    m_lineEditAddress->setObjectName("UserInfoEdit");
    m_lineEditAddress->setReadOnly(true);
    m_lineEditAddress->setFrame(false);
    m_lineEditAddress->setAlignment( Qt::AlignLeft|Qt::AlignTop );
    m_buttonReqeustAddContact = new QPushButton(this);
    m_buttonReqeustAddContact->setObjectName("RequestAddContact");

    this->m_lineEditUserName->setText(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sName.c_str());
    this->m_lineEditAccount->setText(m_callInfo->fromUserAccount().c_str());
    this->m_lineEditTel->setText(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sTel.c_str());
    this->m_lineEditPhone->setText(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sPhone.c_str());
    this->m_lineEditEmail->setText(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sEmail.c_str());
    this->m_lineEditJobTitle->setText(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sTitle.c_str());
    this->m_lineEditGroupName->setText(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sGroupName.c_str());
    this->m_lineEditEnterprise->setText(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sEnterprise.c_str());
    this->m_lineEditAddress->setText(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sAddress.c_str());
    if (m_callInfo->fromUserId()==0 || theApp->m_ebum.EB_IsMyContactAccount2(m_callInfo->fromUserId()))
        m_buttonReqeustAddContact->setVisible(false);
    else {
        m_buttonReqeustAddContact->setVisible(true);
        connect( m_buttonReqeustAddContact,SIGNAL(clicked()),this,SLOT(onClickedButtonAddContact()) );
    }

    updateLocaleInfo();
}

void EbWidgetUserInfo::updateLocaleInfo()
{
    m_labelUserName->setText( theLocales.getLocalText("user-info.label-user-name.text","User Name:") );
    m_labelAccount->setText( theLocales.getLocalText("user-info.label-account.text","Account:") );
    m_labelTel->setText( theLocales.getLocalText("user-info.label-tel.text","Tel:") );
    m_labelPhone->setText( theLocales.getLocalText("user-info.label-phone.text","Phone:") );
    m_labelEmail->setText( theLocales.getLocalText("user-info.label-email.text","Email:") );
    m_labelJobTitle->setText( theLocales.getLocalText("user-info.label-job-title.text","Job Title:") );
    m_labelGroupName->setText( theLocales.getLocalText("user-info.label-group-name.text","Group Name:") );
    m_labelEnterprise->setText( theLocales.getLocalText("user-info.label-enterprise.text","Enterprise:") );
    m_labelAddress->setText( theLocales.getLocalText("user-info.label-address.text","Address:") );

    m_buttonReqeustAddContact->setText( theLocales.getLocalText("user-info.button-add-contact.text","Add Contact") );
//    if (theApp.GetAuthContact() && !is_visitor_uid(m_callInfo->m_pFromAccountInfo.GetUserId()))
//        m_btnRequestAddContact.SetToolTipText(_T("点击申请添加好友"));
//    else
//        m_btnRequestAddContact.SetToolTipText(_T("添加到我的联系人"));
    m_buttonReqeustAddContact->setToolTip( theLocales.getLocalText("user-info.button-add-contact.tooltip","Request Add Contact") );
}

void EbWidgetUserInfo::onClickedButtonAddContact()
{
    EbDialogRequestAddContact dlg;
    dlg.m_headFilePath = theApp->userHeadFilePath( m_callInfo->fromUserId(),m_callInfo->fromMemberId(),m_callInfo->fromUserAccount() );
    dlg.m_fromName = m_callInfo->fromName().c_str();
    dlg.m_fromUserId = m_callInfo->fromUserId();
    dlg.m_fromAccount = m_callInfo->fromUserAccount().c_str();
    dlg.exec();

}

void EbWidgetUserInfo::resizeEvent(QResizeEvent *event)
{
    const int const_sta_left = 6;
    const int const_sta_top = 18;
    const int const_sta_width = 40;
    const int const_sta_height = 18;
    const int const_text_width = width()-const_sta_width-const_sta_left;
    const int const_line_intever = 5;					/// 行间隔
    /// 用户头像
    this->m_labelImage->move( 42,const_sta_top );
    /// 姓名
    int x = const_sta_left;
    int y = this->m_labelImage->geometry().bottom()+16;
//    int y = const_sta_top;
    this->m_labelUserName->setGeometry(x, y, const_sta_width, const_sta_height);
    x += (const_sta_width);
    this->m_lineEditUserName->setGeometry(x, y, const_text_width, const_sta_height);
    /// 帐号
    x = const_sta_left;
    y += (const_sta_height+const_line_intever);
    this->m_labelAccount->setGeometry(x, y, const_sta_width, const_sta_height);
    x += (const_sta_width);
    this->m_lineEditAccount->setGeometry(x, y, const_text_width, const_sta_height);
    y += 8;	/// 上下分隔
    /// 电话
    x = const_sta_left;
    y += (const_sta_height+const_line_intever);
    this->m_labelTel->setGeometry(x, y, const_sta_width, const_sta_height);
    x += (const_sta_width);
    this->m_lineEditTel->setGeometry(x, y, const_text_width, const_sta_height);
    /// 手机
    x = const_sta_left;
    y += (const_sta_height+const_line_intever);
    this->m_labelPhone->setGeometry(x, y, const_sta_width, const_sta_height);
    x += (const_sta_width);
    this->m_lineEditPhone->setGeometry(x, y, const_text_width, const_sta_height);
    /// 邮箱
    x = const_sta_left;
    y += (const_sta_height+const_line_intever);
    this->m_labelEmail->setGeometry(x, y, const_sta_width, const_sta_height);
    x += (const_sta_width);
    this->m_lineEditEmail->setGeometry(x, y, const_text_width, const_sta_height);
    y += 8;	/// 上下分隔
    /// 职务
    x = const_sta_left;
    y += (const_sta_height+const_line_intever);
    this->m_labelJobTitle->setGeometry(x, y, const_sta_width, const_sta_height);
    x += (const_sta_width);
    this->m_lineEditJobTitle->setGeometry(x, y, const_text_width, const_sta_height);
    /// 部门
    x = const_sta_left;
    y += (const_sta_height+const_line_intever);
    this->m_labelGroupName->setGeometry(x, y, const_sta_width, const_sta_height);
    x += (const_sta_width);
    this->m_lineEditGroupName->setGeometry(x, y, const_text_width, const_sta_height);
    /// 公司
    x = const_sta_left;
    y += (const_sta_height+const_line_intever);
    this->m_labelEnterprise->setGeometry(x, y, const_sta_width, const_sta_height);
    x += (const_sta_width);
    this->m_lineEditEnterprise->setGeometry(x, y, const_text_width, const_sta_height);
    y += 8;	/// 上下分隔
    /// 地址
    x = const_sta_left;
    y += (const_sta_height+const_line_intever);
    this->m_labelAddress->setGeometry(x, y, const_sta_width, const_sta_height);
    x += (const_sta_width);
    this->m_lineEditAddress->setGeometry(x, y, const_text_width, const_sta_height*3);
    //x = const_sta_left;
    y += (const_sta_height*2+const_line_intever);
    m_buttonReqeustAddContact->setGeometry( x,y,55,23 );
    QWidget::resizeEvent(event);
}

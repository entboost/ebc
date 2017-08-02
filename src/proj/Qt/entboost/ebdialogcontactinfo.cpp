#include "ebdialogcontactinfo.h"
#include "ui_ebdialogcontactinfo.h"
#include <QTimer>
#include <ebmessagebox.h>
#include <ebcontextmenu.h>

EbDialogContactInfo::EbDialogContactInfo(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogContactInfo)
{
    ui->setupUi(this);
    this->resize(612,410);
#ifdef __MACH__
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::CustomizeWindowHint);
#else
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );
#endif
    this->showTitleBackground( theLocales.titleBackgroundHeight() );

    ui->pushButtonAddContact->setObjectName("RequestAddContact");
    if (theApp->isAuthContact()) {
        ui->lineEditContact->setReadOnly(true);
    }
    QTimer::singleShot( 100,this,SLOT(onLoadContactGroup()) );

    const int const_y	= 60;
    const int const_x1	= 34;
    const int const_x2	= 314;
    const int const_x_interval	= 70;
    const int const_static_width	= 70;
    const int const_edit_width1		= 175;
    const int const_edit_width2		= 455;
    const int const_edit_height		= 21;
    const int const_y_interval	= 35;
    int nX = const_x1;
    int nY = const_y;
    ui->labelContact->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditContact->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    nX = const_x2;
    ui->labelGroupName->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->comboBoxGroupName->setGeometry(nX,nY,const_edit_width1,const_edit_height+1);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelUserName->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditUserName->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    nX = const_x2;
    ui->labelPhone->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditPhone->setGeometry(nX,nY,const_edit_width1,const_edit_height);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelEnterprise->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditEnterprise->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    nX = const_x2;
    ui->labelJobTitle->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditJobTitle->setGeometry(nX,nY,const_edit_width1,const_edit_height);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelUrl->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditUrl->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    nX = const_x2;
    ui->labelTel->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditTel->setGeometry(nX,nY,const_edit_width1,const_edit_height);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelEmail->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditEmail->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    nX = const_x2;
    ui->labelFax->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditFax->setGeometry(nX,nY,const_edit_width1,const_edit_height);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelAddress->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditAddress->setGeometry(nX,nY,const_edit_width2,const_edit_height);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelDescription->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->plainTextEditDescription->setGeometry(nX,nY,const_edit_width2,70);

    nY += 85;
    ui->pushButtonAddContact->setGeometry(nX,nY,60,22);

}

EbDialogContactInfo::~EbDialogContactInfo()
{
    delete ui;
}

void EbDialogContactInfo::updateLocaleInfo()
{
    /// 显示右上角关闭按钮
    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    /// 显示 保存/关闭” 按钮
    this->showPushButtonStdOkCancel( theLocales.getLocalText("base-dialog.save-button.text","Save"), "OkButton",
                                     theLocales.getLocalText("base-dialog.close-button.text","Close"), "CancelButton" );
    ui->labelContact->setText( theLocales.getLocalText("contact-info-dialog.label-contact.text","Contact:") );
    ui->lineEditContact->setPlaceholderText( theLocales.getLocalText("contact-info-dialog.edit-contact.bg-text","") );
    ui->lineEditContact->setToolTip( theLocales.getLocalText("contact-info-dialog.edit-contact.tooltip","") );
    ui->labelGroupName->setText( theLocales.getLocalText("contact-info-dialog.label-group-name.text","Group:") );
    ui->labelUserName->setText( theLocales.getLocalText("contact-info-dialog.label-user-name.text","Name:") );
    ui->lineEditUserName->setPlaceholderText( theLocales.getLocalText("contact-info-dialog.edit--user-name.bg-text","") );
    ui->lineEditUserName->setToolTip( theLocales.getLocalText("contact-info-dialog.edit-user-name.tooltip","") );
    ui->labelPhone->setText( theLocales.getLocalText("contact-info-dialog.label-phone.text","Phone:") );
    ui->labelEnterprise->setText( theLocales.getLocalText("contact-info-dialog.label-enterprise.text","Enterprise:") );
    ui->labelJobTitle->setText( theLocales.getLocalText("contact-info-dialog.label-job-title.text","Job Title:") );
    ui->labelUrl->setText( theLocales.getLocalText("contact-info-dialog.label-url.text","Url:") );
    ui->labelTel->setText( theLocales.getLocalText("contact-info-dialog.label-tel.text","Tel:") );
    ui->labelEmail->setText( theLocales.getLocalText("contact-info-dialog.label-email.text","Email:") );
    ui->labelFax->setText( theLocales.getLocalText("contact-info-dialog.label-fax.text","Fax:") );
    ui->labelAddress->setText( theLocales.getLocalText("contact-info-dialog.label-address.text","Address:") );
    ui->labelDescription->setText( theLocales.getLocalText("contact-info-dialog.label-description.text","Description:") );
    ui->pushButtonAddContact->setText( theLocales.getLocalText("contact-info-dialog.button-add-contact.text","Add Contact") );
    ui->pushButtonAddContact->setToolTip( theLocales.getLocalText("contact-info-dialog.button-add-contact.tooltip","") );
}

int EbDialogContactInfo::exec()
{
    QString sDialogTitle = theLocales.getLocalText("contact-info-dialog.title","Contact Info");
//    sDialogTitle.replace( "[GROUP_TYPE_MEMBER]", theLocales.getGroupTypeName((int)m_groupType)->member().c_str() );
    this->setWindowTitle( sDialogTitle );
    if (m_contactInfo.m_nContactId>0) {
        /// 修改用户图标
        this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf007),const_common_title_image_font_size );
    }
    else {
        /// 新建用户图标
        this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf234),const_common_title_image_font_size );
    }
    updateLocaleInfo();

    if (theApp->isAuthContact() && m_contactInfo.m_nContactUserId>0 &&
            (m_contactInfo.m_nContactType&EB_CONTACT_TYPE_AUTH)==0) {
        ui->pushButtonAddContact->setVisible(true);
    }
    else {
        ui->pushButtonAddContact->setVisible(false);
    }

    ui->lineEditContact->setText( m_contactInfo.m_sContact.c_str() );
    if (!m_contactInfo.m_sContact.empty() && m_contactInfo.m_nContactUserId>0) {
        ui->lineEditContact->setToolTip( QString("%1(%2)").arg(m_contactInfo.m_sContact.c_str()).arg(m_contactInfo.m_nContactUserId) );
    }
    ui->lineEditUserName->setText( m_contactInfo.m_sName.c_str() );
    ui->lineEditPhone->setText( m_contactInfo.m_sPhone.c_str() );
    ui->lineEditEnterprise->setText( m_contactInfo.m_sCompany.c_str() );
    ui->lineEditJobTitle->setText( m_contactInfo.m_sJobTitle.c_str() );
    ui->lineEditUrl->setText( m_contactInfo.m_sUrl.c_str() );
    ui->lineEditTel->setText( m_contactInfo.m_sTel.c_str() );
    ui->lineEditEmail->setText( m_contactInfo.m_sEmail.c_str() );
    ui->lineEditFax->setText( m_contactInfo.m_sFax.c_str() );
    ui->lineEditAddress->setText( m_contactInfo.m_sAddress.c_str() );
    ui->plainTextEditDescription->setPlaceholderText( m_contactInfo.m_sDescription.c_str() );

    return EbDialogBase::exec();

}

void EbDialogContactInfo::accept()
{
    const QString userName = ui->lineEditUserName->text();
    if ( userName.isEmpty() ) {
        ui->lineEditUserName->setFocus();
        /// 联系人姓名不能为空：<br>请重新输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.contact-name-empty-error","Contact Name Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    m_contactInfo.m_sContact = ui->lineEditContact->text().toStdString();
    m_contactInfo.m_sName = userName.toStdString();
    m_contactInfo.m_nUGId = ui->comboBoxGroupName->currentData().toLongLong();
//    m_contactInfo.m_sGroupName = ui->comboBoxGroupName->currentText().toStdString();
    m_contactInfo.m_sPhone = ui->lineEditPhone->text().toStdString();
    m_contactInfo.m_sCompany = ui->lineEditEnterprise->text().toStdString();
    m_contactInfo.m_sJobTitle = ui->lineEditJobTitle->text().toStdString();
    m_contactInfo.m_sUrl = ui->lineEditUrl->text().toStdString();
    m_contactInfo.m_sTel = ui->lineEditTel->text().toStdString();
    m_contactInfo.m_sEmail = ui->lineEditEmail->text().toStdString();
    m_contactInfo.m_sFax = ui->lineEditFax->text().toStdString();
    m_contactInfo.m_sAddress = ui->lineEditAddress->text().toStdString();
    m_contactInfo.m_sDescription = ui->plainTextEditDescription->toPlainText().toStdString();

    EbDialogBase::accept();
}

void EbDialogContactInfo::onLoadContactGroup()
{
    if (!m_contactInfo.m_sContact.empty() && m_contactInfo.m_sName.empty()) {
        ui->lineEditUserName->setFocus();
    }
    ui->comboBoxGroupName->clear();
    const QString const_default_group_name = theLocales.getLocalText("name-text.default-group-name","Default Group");
    ui->comboBoxGroupName->addItem( const_default_group_name, QVariant(const_default_group_ugid) );
    std::vector<EB_UGInfo> pUGInfoList;
    theApp->m_ebum.EB_GetUGInfoList(pUGInfoList);
    for (size_t i=0; i<pUGInfoList.size(); i++) {
        const EB_UGInfo& pUGInfo = pUGInfoList[i];
        ui->comboBoxGroupName->addItem( pUGInfo.m_sGroupName.c_str(), QVariant(pUGInfo.m_nUGId) );
        if (m_contactInfo.m_nUGId>0 && m_contactInfo.m_nUGId==pUGInfo.m_nUGId) {
            ui->comboBoxGroupName->setCurrentIndex((int)i);
        }
    }
    if ( !m_contactInfo.m_sGroupName.empty() && ui->comboBoxGroupName->currentIndex()<0) {
        ui->comboBoxGroupName->setCurrentText( m_contactInfo.m_sGroupName.c_str() );
    }
}

#include "ebdialogrequestaddcontact.h"
#include "ui_ebdialogrequestaddcontact.h"

EbDialogRequestAddContact::EbDialogRequestAddContact(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogRequestAddContact)
{
    ui->setupUi(this);
    this->resize(380,220);

    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );
    this->showTitleBackground( theLocales.titleBackgroundHeight() );

    QString sDialogTitle = theLocales.getLocalText("request-add-contact-dialog.title","Request Add Contact");
    this->setWindowTitle( sDialogTitle );
    this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf234),const_common_title_image_font_size );

    const int const_image_size = 80;
    int x = 25;
    int y = 55;
    ui->labelHeadImage->setGeometry( x,y,const_image_size,const_image_size );
    ui->plainTextEditDescription->setGeometry( x+const_image_size+10,y,230,const_image_size);
    y += (const_image_size+5);
    ui->labelUserInfo->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    ui->labelUserInfo->setGeometry( x,y,const_image_size+40,height()-y);
}

EbDialogRequestAddContact::~EbDialogRequestAddContact()
{
    delete ui;
}

void EbDialogRequestAddContact::updateLocaleInfo()
{
    /// 显示右上角关闭按钮
    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    /// 显示 保存/关闭” 按钮
    this->showPushButtonStdOkCancel( theLocales.getLocalText("base-dialog.request-contact.text","Request"), "OkButton",
                                     theLocales.getLocalText("base-dialog.close-button.text","Close"), "CancelButton" );
    this->setButtonStdOkTooltip( theLocales.getLocalText("base-dialog.request-contact.tooltip","") );

    ui->plainTextEditDescription->setPlaceholderText( theLocales.getLocalText("request-add-contact-dialog.edit-description.bg-text","") );
    ui->plainTextEditDescription->setToolTip( theLocales.getLocalText("request-add-contact-dialog.edit-description.tooltip","") );
}

int EbDialogRequestAddContact::exec()
{
    updateLocaleInfo();
    ui->labelHeadImage->setPixmap( QPixmap(m_headFilePath) );
    const QString text = QString("%1<br>%2<br>%3").arg(m_fromName).arg(m_fromUserId).arg(m_fromAccount);
    ui->labelUserInfo->setText(text);
    ui->labelUserInfo->setToolTip(text);
    ui->plainTextEditDescription->setFocus();

    return EbDialogBase::exec();
}

void EbDialogRequestAddContact::accept()
{
    EB_ContactInfo pContactInfo;
    pContactInfo.m_nContactUserId = m_fromUserId;
    pContactInfo.m_sContact = m_fromAccount.toStdString();
    pContactInfo.m_sDescription = ui->plainTextEditDescription->toPlainText().toStdString();
    theApp->m_ebum.EB_EditContact(&pContactInfo);
    EbDialogBase::accept();
}

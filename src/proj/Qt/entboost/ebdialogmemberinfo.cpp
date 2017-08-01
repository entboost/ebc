#include "ebdialogmemberinfo.h"
#include "ui_ebdialogmemberinfo.h"
#include "ebmessagebox.h"
#include "ebdialogchangehead.h"

const QSize const_user_head_size(80,80);
EbDialogMemberInfo::EbDialogMemberInfo(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogMemberInfo)
  , m_labelUserHead(0)
  , m_groupType(EB_GROUP_TYPE_DEPARTMENT)
  , m_newEemployee(false)
  , m_dialogChangeHead(0)

{
    ui->setupUi(this);
    this->resize(612,455);
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );
    this->showTitleBackground( theLocales.titleBackgroundHeight() );

    /// 用户头像
    m_labelUserHead = new EbLabel(this);
    m_labelUserHead->setGeometry( 25,55,const_user_head_size.width(),const_user_head_size.height() );
    connect( m_labelUserHead,SIGNAL(clicked()),this,SLOT(onClickedLabelUserHead()) );

    const int const_y1	= 60+5;
    const int const_y2	= 148;
    const int const_x1	= 34;
    const int const_x2	= 314;
    const int const_x_interval	= 70;
    const int const_static_width	= 70;
    const int const_edit_width1		= 175;
    const int const_edit_width2		= 455;
    const int const_edit_height		= 21;
    const int const_y_interval	= 35;
    int nX = 123;
    int nY = const_y1;
    ui->labelAccount->setAlignment( Qt::AlignRight );
    ui->labelAccount->setGeometry(nX,nY,const_static_width-20,const_edit_height);
    nX += (const_x_interval-10);
    ui->lineEditAccount->setGeometry( nX,nY,const_edit_width1,const_edit_height );
    nX = 123;
    nY += const_y_interval;
    ui->labelGroupName->setAlignment( Qt::AlignRight );
    ui->labelGroupName->setGeometry(nX,nY,const_static_width-20,const_edit_height);
    nX += (const_x_interval-10);
    ui->lineEditGroupName->setGeometry( nX,nY,const_edit_width1,const_edit_height );
    ui->lineEditGroupName->setReadOnly(true);
//    ui->pushButtonUserHead->setGeometry( 123,const_y1+(const_edit_height+2)*2+5,60,22 );
//    if (theApp.GetSendRegMail() && m_newEemployee)
//    {
//        Gdiplus::SolidBrush brushTip(Gdiplus::Color(255,0,128));
//        graphics.DrawString(L"添加成功，请接收验证邮件，验证恩布帐号！",-1,&fontEbTitle,PointF(310,63),&brushTip);
//    }
    nX = const_x1;
    nY = const_y2;
    ui->labelUserName->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditUserName->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    nX = const_x2;
    ui->labelJobTitle->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditJobTitle->setGeometry(nX,nY,const_edit_width1,const_edit_height+1);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelGender->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->comboBoxGender->setGeometry(nX,nY,const_edit_width1,const_edit_height+1);
    nX = const_x2;
    ui->labelJobPosition->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditJobPosition->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    /// 限制只能输入数字
    ui->lineEditJobPosition->setValidator(new QIntValidator());

    nX = const_x1;
    nY += const_y_interval;
    ui->labelBirthday->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->dateEditBirthday->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    nX = const_x2;
    ui->labelTel->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditTel->setGeometry(nX,nY,const_edit_width1,const_edit_height);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelPhone->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditPhone->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    nX = const_x2;
    ui->labelFax->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditFax->setGeometry(nX,nY,const_edit_width1,const_edit_height);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelEmail->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditEmail->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    nX = const_x2;
    ui->labelDisplayIndex->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditDisplayIndex->setGeometry(nX,nY,const_edit_width1,const_edit_height);
    /// 限制只能输入数字
    ui->lineEditDisplayIndex->setValidator(new QIntValidator());

    nX = const_x1;
    nY += const_y_interval;
    ui->labelAddress->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditAddress->setGeometry(nX,nY,const_edit_width2,const_edit_height);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelDescription->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->plainTextEditDescription->setGeometry(nX,nY,const_edit_width2,43);	// 53

    /// 限制只能输入数字
//    QRegExp regx("[1-9][0-9]+$");
//    QValidator *validator = new QRegExpValidator(regx, ui->lineEdit );
//    lineEdit->setValidator(validator);
//    lineEdit的第一个数是1-9的，第二个数和之后的是0-9的

    ui->pushButtonDefaultMember->setObjectName("RequestAddContact");
    connect( ui->pushButtonDefaultMember, SIGNAL(clicked()), this, SLOT(onClickedButtonDefaultMember()) );
}

EbDialogMemberInfo::~EbDialogMemberInfo()
{
    if (m_dialogChangeHead!=0) {
        delete m_dialogChangeHead;
        m_dialogChangeHead = 0;
    }
    delete ui;
}

void EbDialogMemberInfo::updateLocaleInfo()
{
    /// 显示右上角关闭按钮
    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    /// 显示 保存/关闭” 按钮
    this->showPushButtonStdOkCancel( theLocales.getLocalText("base-dialog.save-button.text","Save"), "OkButton",
                                     theLocales.getLocalText("base-dialog.close-button.text","Close"), "CancelButton" );
    if (m_newEemployee)
        m_labelUserHead->setToolTip( "" );
    else
        m_labelUserHead->setToolTip( theLocales.getLocalText("member-info-dialog.label-user-head.tooltip","") );
    ui->labelAccount->setText( theLocales.getLocalText("member-info-dialog.label-account.text","Account:") );
    ui->lineEditAccount->setPlaceholderText( theLocales.getLocalText("member-info-dialog.edit-account.bg-text","") );
    ui->lineEditAccount->setToolTip( theLocales.getLocalText("member-info-dialog.edit-account.tooltip","") );

    ui->labelUserName->setText( theLocales.getLocalText("member-info-dialog.label-user-name.text","Name:") );
    ui->lineEditUserName->setPlaceholderText( theLocales.getLocalText("member-info-dialog.edit--user-name.bg-text","") );
    ui->lineEditUserName->setToolTip( theLocales.getLocalText("member-info-dialog.edit-user-name.tooltip","") );
    ui->labelJobTitle->setText( theLocales.getLocalText("member-info-dialog.label-job-title.text","Job Title:") );
    ui->labelGender->setText( theLocales.getLocalText("member-info-dialog.label-gender.text","Gender:") );
    ui->comboBoxGender->clear();
    ui->comboBoxGender->addItem("");
    ui->comboBoxGender->addItem( theLocales.getLocalText("gender.1.name","Male"),QVariant((int)EB_GENDER_MALE) );
    ui->comboBoxGender->addItem( theLocales.getLocalText("gender.2.name","Female"),QVariant((int)EB_GENDER_FEMALE) );
    ui->labelJobPosition->setText( theLocales.getLocalText("member-info-dialog.label-job-position.text","Job Position:") );
    ui->labelBirthday->setText( theLocales.getLocalText("member-info-dialog.label-birthday.text","Birthday:") );
    ui->dateEditBirthday->setDisplayFormat( theLocales.getLocalText("member-info-dialog.date-birthday.format","yyyy-MM-dd") );
    ui->labelTel->setText( theLocales.getLocalText("member-info-dialog.label-tel.text","Tel:") );
    ui->labelPhone->setText( theLocales.getLocalText("member-info-dialog.label-phone.text","Phone:") );
    ui->labelFax->setText( theLocales.getLocalText("member-info-dialog.label-fax.text","Fax:") );
    ui->labelEmail->setText( theLocales.getLocalText("member-info-dialog.label-email.text","Email:") );
    ui->labelDisplayIndex->setText( theLocales.getLocalText("member-info-dialog.label-display-index.text","Display Index:") );
    ui->labelAddress->setText( theLocales.getLocalText("member-info-dialog.label-address.text","Address:") );
    ui->labelDescription->setText( theLocales.getLocalText("member-info-dialog.label-description.text","Description:") );

    ui->pushButtonDefaultMember->setToolTip( theLocales.getLocalText("member-info-dialog.button-default-member.tooltip","") );
    /// 设为默认名片
    ui->pushButtonDefaultMember->setText( theLocales.getLocalText("member-info-dialog.button-default-member.text-0","Set Default Member") );
    if (m_memberInfo.m_sMemberCode>0) {
        eb::bigint defaultMemberId = 0;
        theApp->m_ebum.EB_GetMyDefaultMemberCode(defaultMemberId);
        if (m_memberInfo.m_sMemberCode==defaultMemberId) {
            /// 当前默认名片
            ui->pushButtonDefaultMember->setText( theLocales.getLocalText("member-info-dialog.button-default-member.text-1","Current Default Member") );
        }
    }
    else {
        ui->pushButtonDefaultMember->setVisible(false);
    }

}

//void EbDialogMemberInfo::open()
//{

//    EbDialogBase::open();
//}

int EbDialogMemberInfo::exec()
{
    updateUserHeadImage();

    QString sDialogTitle = theLocales.getLocalText("member-info-dialog.title","Member Info");
    sDialogTitle.replace( "[GROUP_TYPE_MEMBER]", theLocales.getGroupTypeName((int)m_groupType)->member().c_str() );
    this->setWindowTitle( sDialogTitle );
    if ( !m_memberInfo.m_sMemberAccount.empty() ) {
        m_newEemployee = false;
        /// 修改用户图标
        this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf007),const_common_title_image_font_size );

        m_labelUserHead->setCursor( QCursor(Qt::PointingHandCursor) );
        ui->lineEditAccount->setReadOnly(true);

        theApp->m_ebum.EB_GetGroupType(m_memberInfo.m_sGroupCode,m_groupType);
        if (!theApp->m_ebum.EB_CanEditMyBaseMemberInfo(m_memberInfo.m_sGroupCode)) {
            /// 限制不能修改自己成员名片
            ui->lineEditUserName->setReadOnly(true);
            ui->comboBoxGender->setEnabled(false);
            ui->dateEditBirthday->setReadOnly(true);
            ui->lineEditPhone->setReadOnly(true);
            ui->lineEditEmail->setReadOnly(true);
            ui->lineEditJobTitle->setReadOnly(true);
            ui->lineEditJobPosition->setReadOnly(true);
            ui->lineEditTel->setReadOnly(true);
            ui->lineEditFax->setReadOnly(true);
            ui->lineEditDisplayIndex->setReadOnly(true);
            ui->lineEditAddress->setReadOnly(true);
        }
        else if (!theApp->m_ebum.EB_IsGroupAdminLevel(m_memberInfo.m_sGroupCode)) {
            /// 非管理员权限，不能修改显示排序
            ui->lineEditDisplayIndex->setReadOnly(true);
        }
//        if ( m_memberInfo.m_nMemberUserId==theApp->logonUserId() ) {	/// 自己才能修改
//            CString sText;
//            sText.Format(_T("更换当前%s[%s]电子名片显示头像"),GetGroupTypeText(m_groupType,false),m_sGroupName);
//            m_btnChangeHead.SetToolTipText(sText);
//        }
//        else {
//            this->GetDlgItem(IDC_BUTTON_CHANGEHEAD)->ShowWindow(SW_HIDE);
//            this->GetDlgItem(IDC_BUTTON_DEFAULT_EMP)->ShowWindow(SW_HIDE);
//        }
    }
    else {
        m_newEemployee = true;
        /// 新建用户图标
        this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf234),const_common_title_image_font_size );

        m_labelUserHead->setCursor( QCursor(Qt::ArrowCursor) );

//        this->GetDlgItem(IDC_BUTTON_CHANGEHEAD)->EnableWindow(FALSE);
//        this->GetDlgItem(IDC_BUTTON_DEFAULT_EMP)->EnableWindow(FALSE);
        ui->lineEditAccount->setReadOnly(false);
    }

    QString groupNameText = theLocales.getLocalText("member-info-dialog.label-group-name.text","Group Name:");
    groupNameText.replace( "[GROUP_TYPE_NAME]", theLocales.getGroupTypeName((int)m_groupType)->name().c_str() );
    ui->labelGroupName->setText( groupNameText );

    ui->lineEditAccount->setText(m_memberInfo.m_sMemberAccount.c_str());
    ui->lineEditGroupName->setText(m_groupName);
    ui->lineEditUserName->setText(m_memberInfo.m_sUserName.c_str());
    ui->lineEditJobTitle->setText(m_memberInfo.m_sJobTitle.c_str());
    ui->comboBoxGender->setCurrentIndex((int)m_memberInfo.m_nGender);
    ui->lineEditJobPosition->setText(QString::number(m_memberInfo.m_nJobPosition).toStdString().c_str());
    const int birthdayYear = m_memberInfo.m_nBirthday/10000;
    const int birthdayMonth = (m_memberInfo.m_nBirthday%10000)/100;
    const int birthdayDay = m_memberInfo.m_nBirthday%100;
    ui->dateEditBirthday->setDate( QDate(birthdayYear,birthdayMonth,birthdayDay) );
    ui->lineEditTel->setText(m_memberInfo.m_sWorkPhone.c_str());
    ui->lineEditPhone->setText(m_memberInfo.m_sCellPhone.c_str());
    ui->lineEditFax->setText(m_memberInfo.m_sFax.c_str());
    ui->lineEditEmail->setText(m_memberInfo.m_sEmail.c_str());
    ui->lineEditDisplayIndex->setText(QString::number(m_memberInfo.m_nDisplayIndex));
    ui->lineEditAddress->setText(m_memberInfo.m_sAddress.c_str());
    ui->plainTextEditDescription->setPlainText(m_memberInfo.m_sDescription.c_str());

    updateLocaleInfo();
    return EbDialogBase::exec();
}

void EbDialogMemberInfo::accept()
{
    QString sMemberAccount = ui->lineEditAccount->text();
    if (sMemberAccount.isEmpty()) {
        ui->lineEditAccount->setFocus();
        /// 帐号不能为空：<br>请重新输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.account-empty-error","Account Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
//    else if (m_newEemployee && theApp.GetAccountPrefix().IsEmpty()) {
//        const tstring sAccount(m_sMemberAccount);
//        //eb::bigint nPhone = 0;
//        if (sAccount.find("@") != std::string::npos || !libEbc::IsFullNumber(sAccount.c_str(),sAccount.size()))
//        {
//            // xx@xx邮箱格式
//        }else if (sAccount.size()==11 && sAccount.substr(0,1)=="1")
//        {
//            //nPhone = eb_atoi64(sAccount.c_str());
//        }else
//        {
//            //// 错误格式；
//            //this->GetDlgItem(IDC_EDIT_ACCOUNT)->SetFocus();
//            //CDlgMessageBox::EbMessageBox(this,"","帐号格式错误：\r\n请重新输入正确邮箱地址或手机号码！",CDlgMessageBox::IMAGE_WARNING,5);
//            //return;
//        }
//    }

    QString sUserName = ui->lineEditUserName->text();
    if (sUserName.isEmpty()) {
        ui->lineEditUserName->setFocus();
        /// 用户名称不能为空：<br>请重新输入！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.user-name-empty-error","User Name Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }
    if (m_newEemployee) {
        if (theApp->m_ebum.EB_IsExistMemberByAccount(m_memberInfo.m_sGroupCode, sMemberAccount.toStdString().c_str())) {
            ui->lineEditAccount->setFocus();
            /// 姓名不能为空：<br>请重新输入！
            QString text = theLocales.getLocalText( "message-show.account-exist-error","Account Exist Error" );
            text.replace("[USER_ACCOUNT]",sMemberAccount);
            EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
            return;
        }
    }
    m_memberInfo.m_sMemberAccount = sMemberAccount.toStdString();
    m_memberInfo.m_sUserName = sUserName.toStdString();
    m_memberInfo.m_sJobTitle = ui->lineEditJobTitle->text().toStdString();
    m_memberInfo.m_nGender = (EB_GENDER_TYPE)ui->comboBoxGender->currentIndex();
    m_memberInfo.m_nJobPosition = ui->lineEditJobPosition->text().toInt();
    /// 计算生日整数
    const QDate date = ui->dateEditBirthday->date();
    m_memberInfo.m_nBirthday = date.year()*10000+date.month()*100+date.day();
    m_memberInfo.m_sWorkPhone = ui->lineEditTel->text().toStdString();
    m_memberInfo.m_sCellPhone = ui->lineEditPhone->text().toStdString();
    m_memberInfo.m_sFax = ui->lineEditFax->text().toStdString();
    m_memberInfo.m_sEmail = ui->lineEditEmail->text().toStdString();
    m_memberInfo.m_nDisplayIndex = ui->lineEditDisplayIndex->text().toInt();
    m_memberInfo.m_sAddress = ui->lineEditAddress->text().toStdString();
    m_memberInfo.m_sDescription = ui->plainTextEditDescription->toPlainText().toStdString();

    EbDialogBase::accept();
}

void EbDialogMemberInfo::resizeEvent(QResizeEvent *e)
{
    ui->pushButtonDefaultMember->setGeometry(20, height()-42, 80, 22);
    EbDialogBase::resizeEvent(e);
}

void EbDialogMemberInfo::onClickedButtonDefaultMember()
{
    if (m_memberInfo.m_sMemberCode==0) {
        ui->pushButtonDefaultMember->setVisible(false);
        return;
    }
    eb::bigint defaultMemberId = 0;
    theApp->m_ebum.EB_GetMyDefaultMemberCode(defaultMemberId);
    if (m_memberInfo.m_sMemberCode==defaultMemberId) {
        /// 当前默认名片
        const QString text = theLocales.getLocalText("member-info-dialog.button-default-member.text-1","Current Default Member");
        ui->pushButtonDefaultMember->setText(text);
        EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_INFORMATION, default_warning_auto_close );
        return;
    }
    theApp->m_ebum.EB_SetMyDefaultMemberCode(m_memberInfo.m_sMemberCode);
}

void EbDialogMemberInfo::onSelectedHeadImage(qint64 resourceId, const QString &resourceFile)
{
    if ( m_memberInfo.m_sGroupCode==0 || !QFileInfo::exists(resourceFile) ) {
        return;
    }
    int ret = 0;
    if (resourceId==0) {
        /// 自定义头像
        ret = theApp->m_ebum.EB_SetMyGroupHeadFile(m_memberInfo.m_sGroupCode,resourceFile);
    }
    else {
        /// 默认头像
        ret = theApp->m_ebum.EB_SetMyGroupHeadRes(m_memberInfo.m_sGroupCode,resourceId);
    }
    if (ret == 0) {
        m_dialogChangeHead->setHeadResorceFile(resourceFile);
        m_dialogChangeHead->hide();
        //m_pMemberInfo.m_sHeadResourceId = pSelectedImageInfo.m_sResId;
        m_memberInfo.m_sHeadResourceFile = resourceFile;
        mycp::bigint m_nOldFileSize = 0;
        m_oldFileMd5.clear();
        libEbc::GetFileMd5(resourceFile,m_nOldFileSize,m_oldFileMd5);
        updateUserHeadImage();
    }
}

void EbDialogMemberInfo::updateUserHeadImage()
{
    const QString sHeadResourceFile = m_memberInfo.m_sHeadResourceFile;
    QImage userHead;
    if (!sHeadResourceFile.isEmpty() && QFileInfo::exists(sHeadResourceFile)) {
        if (m_oldFileMd5.empty()) {
            mycp::bigint m_nOldFileSize = 0;
            libEbc::GetFileMd5(sHeadResourceFile,m_nOldFileSize,m_oldFileMd5);
        }
        userHead = QImage(sHeadResourceFile);
    }
    if (userHead.isNull()) {
        userHead = QImage(":/img/defaultmember.png");
    }
    m_labelUserHead->setPixmap( QPixmap::fromImage(userHead).scaled(const_user_head_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
}

void EbDialogMemberInfo::onClickedLabelUserHead()
{

    if (m_dialogChangeHead==0) {
        m_dialogChangeHead = new EbDialogChangeHead;
//        m_dialogChangeHead->m_sGroupCode = m_memberInfo.m_sGroupCode;
        m_dialogChangeHead->setHeadResorceFile(m_memberInfo.m_sHeadResourceFile);
        connect( m_dialogChangeHead,SIGNAL(onSelectedImage(qint64,QString)),this,SLOT(onSelectedHeadImage(qint64,QString)) );
    }
    m_dialogChangeHead->exec();
}

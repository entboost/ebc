#include "ebdialoggroupinfo.h"
#include "ui_ebdialoggroupinfo.h"
#include "ebmessagebox.h"

EbDialogGroupInfo::EbDialogGroupInfo(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogGroupInfo)
  , m_labelManagerUserName(0)
{
    ui->setupUi(this);
    m_labelManagerUserName = new EbLabel(this);
    connect( m_labelManagerUserName,SIGNAL(clicked()),this,SLOT(onClickedLabelManagerUserName()) );
    this->resize(612,340);
    /// 去掉标题栏
#ifdef __MACH__
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::CustomizeWindowHint);
#else
    this->setWindowFlags( Qt::FramelessWindowHint );
#endif
    this->showTitleBackground( theLocales.titleBackgroundHeight() );
}

EbDialogGroupInfo::~EbDialogGroupInfo()
{
    delete ui;
}

void EbDialogGroupInfo::updateLocaleInfo()
{
    /// 显示右上角关闭按钮
    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    /// 显示 保存/关闭” 按钮
    this->showPushButtonStdOkCancel( theLocales.getLocalText("base-dialog.save-button.text","Save"), "OkButton",
                                     theLocales.getLocalText("base-dialog.close-button.text","Close"), "CancelButton" );

    QString sDialogTitle = theLocales.getLocalText("group-info-dialog.title","Group Info");
    sDialogTitle.replace( "[GROUP_TYPE_NAME]", theLocales.getGroupTypeName((int)m_groupInfo.m_nGroupType)->name().c_str() );
    this->setWindowTitle( sDialogTitle );
    this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf0c0),const_common_title_image_font_size );
    m_labelManagerUserName->setCursor( QCursor(Qt::PointingHandCursor) );

//    this->GetDlgItem(IDC_BUTTON_MANAGER_NAME)->SetWindowText(_T("设置"));

    QString groupNameText = theLocales.getLocalText("group-info-dialog.label-group-name.text","Group Name:");
    groupNameText.replace( "[GROUP_TYPE_NAME]", theLocales.getGroupTypeName((int)m_groupInfo.m_nGroupType)->name().c_str() );
    ui->labelGroupName->setText( groupNameText );

    QString groupNameBgText = theLocales.getLocalText("group-info-dialog.edit-group-name.bg-text","");
    if ( !groupNameBgText.isEmpty() ) {
        groupNameBgText.replace( "[GROUP_TYPE_NAME]", theLocales.getGroupTypeName((int)m_groupInfo.m_nGroupType)->name().c_str() );
        ui->lineEditGroupName->setPlaceholderText( groupNameBgText );
    }
    QString managerText = theLocales.getLocalText("group-info-dialog.label-manager.text","Manager:");
    managerText.replace( "[GROUP_TYPE_MANAGER]", theLocales.getGroupTypeName((int)m_groupInfo.m_nGroupType)->manager().c_str() );
    ui->labelManager->setText( managerText );

    if (m_groupInfo.m_nGroupType==EB_GROUP_TYPE_GROUP) {
        ui->labelManager->hide();
        m_labelManagerUserName->hide();
    }
    else {
        QString managerUserNameTooltip = theLocales.getLocalText("group-info-dialog.label-manager-user-name.text","Setting Manager");
        managerUserNameTooltip.replace( "[GROUP_TYPE_NAME]", theLocales.getGroupTypeName((int)m_groupInfo.m_nGroupType)->name().c_str() );
        m_labelManagerUserName->setToolTip( managerUserNameTooltip );
    }

    ui->labelTel->setText( theLocales.getLocalText("group-info-dialog.label-tel.text","Tel:") );
    ui->labelFax->setText( theLocales.getLocalText("group-info-dialog.label-fax.text","Fax:") );
    ui->labelEmail->setText( theLocales.getLocalText("group-info-dialog.label-email.text","Email:") );
    ui->labelUrl->setText( theLocales.getLocalText("group-info-dialog.label-url.text","Url:") );
    ui->labelAddress->setText( theLocales.getLocalText("group-info-dialog.label-address.text","Address:") );
    ui->labelDisplayIndex->setText( theLocales.getLocalText("group-info-dialog.label-display-index.text","Display Index:") );
    ui->labelDescription->setText( theLocales.getLocalText("group-info-dialog.label-description.text","Description:") );

}

int EbDialogGroupInfo::exec()
{
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
    ui->labelGroupName->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    if (m_groupInfo.m_nGroupType != EB_GROUP_TYPE_GROUP) {
    //if (m_nGroupType == EB_GROUP_TYPE_DEPARTMENT || m_nGroupType == EB_GROUP_TYPE_PROJECT) {
        ui->lineEditGroupName->setGeometry(nX,nY,const_edit_width1,const_edit_height);
        nX = const_x2;
        ui->labelManager->setGeometry(nX,nY,const_static_width,const_edit_height);
        nX += const_x_interval;
        m_labelManagerUserName->setGeometry(nX,nY,const_edit_width1,const_edit_height+1);
//        nX += (const_edit_width1+2);
//        m_btnManager.setGeometry(nX,nY,30,const_edit_height);
    }
    else {
        ui->labelManager->setVisible(false);
        m_labelManagerUserName->setVisible(false);
        ui->lineEditGroupName->setGeometry(nX,nY,const_edit_width2,const_edit_height);
    }
    /// 限制只能输入数字
    ui->lineEditDisplayIndex->setValidator(new QIntValidator());

    nX = const_x1;
    nY += const_y_interval;
    ui->labelTel->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditTel->setGeometry(nX,nY,const_edit_width1,const_edit_height+1);
    nX = const_x2;
    ui->labelFax->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditFax->setGeometry(nX,nY,const_edit_width1,const_edit_height+1);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelEmail->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditEmail->setGeometry(nX,nY,const_edit_width1,const_edit_height+1);
    nX = const_x2;
    ui->labelUrl->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->lineEditUrl->setGeometry(nX,nY,const_edit_width1,const_edit_height+1);

    nX = const_x1;
    nY += const_y_interval;
    ui->labelAddress->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    if (m_groupInfo.m_nGroupType!=EB_GROUP_TYPE_GROUP) {
    //if (m_nGroupType == EB_GROUP_TYPE_DEPARTMENT || m_nGroupType == EB_GROUP_TYPE_PROJECT) {
        ui->lineEditAddress->setGeometry(nX,nY,const_edit_width1,const_edit_height);
        nX = const_x2;
        ui->labelDisplayIndex->setGeometry(nX,nY,const_static_width,const_edit_height);
        nX += const_x_interval;
        ui->lineEditDisplayIndex->setGeometry(nX,nY,const_edit_width1,const_edit_height+1);
    }
    else {
        ui->lineEditAddress->setGeometry(nX,nY,const_edit_width2,const_edit_height);
        ui->labelDisplayIndex->setVisible(false);
        ui->lineEditDisplayIndex->setVisible(false);
    }

    nX = const_x1;
    nY += const_y_interval;
    ui->labelDescription->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_x_interval;
    ui->plainTextEditDescription->setGeometry(nX,nY,const_edit_width2,70);

    /// 设置值
    ui->lineEditGroupName->setText( m_groupInfo.m_sGroupName.c_str() );
    if (m_groupInfo.m_nManagerUserId==0) {
        m_labelManagerUserName->setText("");
    }
    else if (m_groupInfo.m_sGroupCode>0) {
        EB_MemberInfo pMemberInfo;
        if (theApp->m_ebum.EB_GetMemberInfoByUserId(&pMemberInfo,m_groupInfo.m_sGroupCode,m_groupInfo.m_nManagerUserId)) {
//            m_sManagerAccount = pMemberInfo.m_sMemberAccount;
            //m_nManagerEmpId = pMemberInfo.m_sMemberCode;
            m_labelManagerUserName->setText( pMemberInfo.m_sUserName.c_str() );
        }
        else if (theApp->m_ebum.EB_GetMemberInfoByUserId2(&pMemberInfo,m_groupInfo.m_nManagerUserId)) {
//            m_sManagerAccount = pMemberInfo.m_sMemberAccount;
            //m_nManagerEmpId = pMemberInfo.m_sMemberCode;
            m_labelManagerUserName->setText( pMemberInfo.m_sUserName.c_str() );
        }
        else {
            m_labelManagerUserName->setText("");
        }
    }else
    {
        EB_MemberInfo pMemberInfo;
        if (theApp->m_ebum.EB_GetMemberInfoByUserId2(&pMemberInfo,m_groupInfo.m_nManagerUserId)) {
//            m_sManagerAccount = pMemberInfo.m_sMemberAccount;
            //m_nManagerEmpId = pMemberInfo.m_sMemberCode;
            m_labelManagerUserName->setText( pMemberInfo.m_sUserName.c_str() );
        }
        else {
            m_labelManagerUserName->setText("");
        }
    }
    ui->lineEditTel->setText( m_groupInfo.m_sPhone.c_str() );
    ui->lineEditFax->setText( m_groupInfo.m_sFax.c_str() );
    ui->lineEditEmail->setText( m_groupInfo.m_sEmail.c_str() );
    ui->lineEditUrl->setText( m_groupInfo.m_sUrl.c_str() );
    ui->lineEditAddress->setText( m_groupInfo.m_sAddress.c_str() );
    ui->lineEditDisplayIndex->setText( QString::number(m_groupInfo.m_nDisplayIndex) );
    ui->plainTextEditDescription->setPlainText( m_groupInfo.m_sDescription.c_str() );

    updateLocaleInfo();
    return EbDialogBase::exec();
}

void EbDialogGroupInfo::accept()
{
    const QString groupName = ui->lineEditGroupName->text();
    if ( groupName.isEmpty() ) {
        ui->lineEditGroupName->setFocus();
        /// %s名称不能为空：\r\n请输入！
        QString text = theLocales.getLocalText( "message-show.group-empty-error","Group Empty" );
        text.replace( "[GROUP_TYPE_NAME]", theLocales.getGroupTypeName((int)m_groupInfo.m_nGroupType)->name().c_str() );
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              text,
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }

    //m_groupInfo->m_sParentCode = sParentCode;
    m_groupInfo.m_sGroupName = groupName.toStdString();
    m_groupInfo.m_sPhone = ui->lineEditTel->text().toStdString();
    m_groupInfo.m_sFax = ui->lineEditFax->text().toStdString();
    m_groupInfo.m_sEmail = ui->lineEditEmail->text().toStdString();
    m_groupInfo.m_sUrl = ui->lineEditUrl->text().toStdString();
    m_groupInfo.m_sAddress = ui->lineEditAddress->text().toStdString();
    m_groupInfo.m_sDescription = ui->plainTextEditDescription->toPlainText().toStdString();
    m_groupInfo.m_nCreateUserId = theApp->logonUserId();
//    m_groupInfo.m_nManagerUserId = pDlgGroupInfo.m_nManagerUserId;
    m_groupInfo.m_nDisplayIndex = ui->lineEditDisplayIndex->text().toInt();

    EbDialogBase::accept();
}

void EbDialogGroupInfo::onClickedLabelManagerUserName()
{
//    if (m_pDlgSelectUser.GetSafeHwnd()==NULL)
//    {
//        m_pDlgSelectUser.SetSingleSelect(true);
//        m_pDlgSelectUser.Create(CDlgSelectUser::IDD,this);
//    }else if (m_pDlgSelectUser.IsWindowVisible())
//    {
//        return;
//    }
//    m_pDlgSelectUser.SetSeledtedGroupId(m_nGroupId);
//#ifdef USES_SELECTED_ITEM_UID
//    m_pDlgSelectUser.m_pSelectedUserTreeItem.insert(m_sManagerAccount,m_nManagerUserId);
//#else
//    int m_nManagerEmpId = 0;
//    m_pDlgSelectUser.m_pSelectedTreeItem.insert(m_sManagerAccount,m_nManagerEmpId);
//#endif
//    m_pDlgSelectUser.ShowWindow(SW_SHOW);

//    const INT_PTR nResponse = m_pDlgSelectUser.RunModalLoop();
//    m_pDlgSelectUser.ShowWindow(SW_HIDE);
//    if (nResponse == IDOK)
//    {
//        m_nManagerUserId = 0;
//        //m_nManagerEmpId = 0;
//#ifdef USES_SELECTED_ITEM_UID
//        BoostReadLock rdlock(m_pDlgSelectUser.m_pSelectedUserTreeItem.mutex());
//        CLockMap<tstring,eb::bigint>::const_iterator pIter = m_pDlgSelectUser.m_pSelectedUserTreeItem.begin();
//        for (; pIter!=m_pDlgSelectUser.m_pSelectedUserTreeItem.end(); pIter++)
//#else
//        BoostReadLock rdlock(m_pDlgSelectUser.m_pSelectedTreeItem.mutex());
//        CLockMap<tstring,eb::bigint>::const_iterator pIter = m_pDlgSelectUser.m_pSelectedTreeItem.begin();
//        for (; pIter!=m_pDlgSelectUser.m_pSelectedTreeItem.end(); pIter++)
//#endif
//        {
//            m_sManagerAccount = pIter->first;
//            //m_nManagerEmpId = pIter->second;

//            if (m_nGroupId>0)
//            {
//                EB_MemberInfo pMemberInfo;
//                if (theEBAppClient.EB_GetMemberInfoByAccount(&pMemberInfo,m_nGroupId,m_sManagerAccount.c_str()))
//                {
//                    m_nManagerUserId = pMemberInfo.m_nMemberUserId;
//                    m_staManagerName.SetWindowText(pMemberInfo.m_sUserName.c_str());
//                    //m_nManagerEmpId = pMemberInfo.m_sMemberCode;
//                }else if (theEBAppClient.EB_GetMemberInfoByAccount2(&pMemberInfo,m_sManagerAccount.c_str()))
//                {
//                    m_nManagerUserId = pMemberInfo.m_nMemberUserId;
//                    m_staManagerName.SetWindowText(pMemberInfo.m_sUserName.c_str());
//                    //m_nManagerEmpId = pMemberInfo.m_sMemberCode;
//                }
//            }else
//            {
//                EB_MemberInfo pMemberInfo;
//                if (theEBAppClient.EB_GetMemberInfoByAccount2(&pMemberInfo,m_sManagerAccount.c_str()))
//                {
//                    m_nManagerUserId = pMemberInfo.m_nMemberUserId;
//                    m_staManagerName.SetWindowText(pMemberInfo.m_sUserName.c_str());
//                    //m_nManagerEmpId = pMemberInfo.m_sMemberCode;
//                }
//            }
//            break;
//        }
//        if (m_nManagerUserId>0)
//        {
//            theApp.InvalidateParentRect(&m_staManagerName);
//        }else
//        {
//            m_staManagerName.SetWindowText("");
//        }
//    }

}

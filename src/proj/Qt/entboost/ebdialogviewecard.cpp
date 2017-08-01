#include "ebdialogviewecard.h"
#include "ui_ebdialogviewecard.h"
#include <ebdialogrequestaddcontact.h>

const QSize const_dialog_view_ecard_size(380,220);
const QSize const_image_size(60,60);
const QSize const_button_size(55,19);
EbDialogViewECard::EbDialogViewECard(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogViewECard)
  , m_widgetValid(0)
  , m_timerIdCheck2Show(0)
//  , m_checkShowCount(0)
  , m_timerIdCheck2Hide(0)
  , m_viewType(VIEW_UNKNOWE)
{
    ui->setupUi(this);
    resize(const_dialog_view_ecard_size);
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );
    /// 设置位置，显示在上面
    this->showTitleBackground(60);
    /// 屏蔽 ESC 按键不退出
    this->setFilterEscapeKey(true);

    ui->labelImage->resize(const_image_size);
    /// 163,73,164=#a349a4
    ui->labelType->setStyleSheet("background-color:rgb(163,73,164); color: rgb(255,255,255);");
    ui->labelType->setAlignment( Qt::AlignCenter );
    ui->labelDetail->setAlignment( Qt::AlignLeft|Qt::AlignTop );

    ui->pushButtonSendMsg->setObjectName("RequestAddContact");
    ui->pushButtonSendMsg->resize(const_button_size);
    connect( ui->pushButtonSendMsg,SIGNAL(clicked()),this,SLOT(onClieckedButtonSendMsg()) );
    ui->pushButtonAddContact->setObjectName("RequestAddContact");
    ui->pushButtonAddContact->resize(const_button_size);
    connect( ui->pushButtonAddContact,SIGNAL(clicked()),this,SLOT(onClieckedButtonAddContact()) );

    updateLocaleInfo();
}

EbDialogViewECard::~EbDialogViewECard()
{
    delete ui;
}

void EbDialogViewECard::updateLocaleInfo()
{
    ui->pushButtonSendMsg->setText( theLocales.getLocalText("base-dialog.send-msg-button.text","Send Msg") );
    ui->pushButtonSendMsg->setToolTip( theLocales.getLocalText("base-dialog.send-msg-button.tooltip","") );
    ui->pushButtonAddContact->setText( theLocales.getLocalText("base-dialog.add-contact-button.text","Add Contact") );
    ui->pushButtonAddContact->setToolTip( theLocales.getLocalText("base-dialog.add-contact-button.tooltip","") );
}

bool EbDialogViewECard::setItemInfo(const EbWidgetItemInfo::pointer &itemInfo)
{
    if (itemInfo.get()==0) {
        return false;
    }
    if ( itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_ENTERPRISE ) {
        if ( theApp->m_ebum.EB_GetEnterpriseInfo(&m_enterpriseInfo,itemInfo->m_sEnterpriseCode) ) {
            m_viewType = VIEW_ENTERPRISE;
            updateEnterpriseInfo();
            return true;
        }
    }
    else if ( itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_GROUP ) {
        setGroupInfo( itemInfo->m_sGroupCode );
        return true;
    }
    else if ( itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_MEMBER ) {
        setMemberInfo( itemInfo->m_sGroupCode,itemInfo->m_nUserId );
        return true;
    }
    else if ( itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_CONTACT ) {
        setContactInfo1( itemInfo->m_sId );
        return true;
    }

    hideReset();
    return false;
}

void EbDialogViewECard::setEnterpriseInfo(const EB_EnterpriseInfo *pEnterpriseInfo)
{
    m_viewType = VIEW_ENTERPRISE;
    m_enterpriseInfo = *pEnterpriseInfo;
    updateEnterpriseInfo();
}

void EbDialogViewECard::setGroupInfo(const EB_GroupInfo *pGroupInfo, const EB_EnterpriseInfo *pEnterpriseInfo)
{
    m_viewType = VIEW_GROUP;
    if (pEnterpriseInfo!=0 && pEnterpriseInfo->m_sEnterpriseCode==pGroupInfo->m_sEnterpriseCode)
        m_enterpriseInfo = *pEnterpriseInfo;
    else
        m_enterpriseInfo = EB_EnterpriseInfo();
    m_groupInfo = *pGroupInfo;
    updateGroupInfo();
}

void EbDialogViewECard::setGroupInfo(eb::bigint groupId)
{
    if ( theApp->m_ebum.EB_GetGroupInfo(groupId,&m_groupInfo) ) {
        if ( m_groupInfo.m_sEnterpriseCode==0 ||
             !theApp->m_ebum.EB_GetEnterpriseInfo(&m_enterpriseInfo,m_groupInfo.m_sEnterpriseCode) ) {
            m_enterpriseInfo = EB_EnterpriseInfo();
        }
        m_viewType = VIEW_GROUP;
        updateGroupInfo();
    }
    else {
        hideReset();
    }

}

void EbDialogViewECard::setMemberInfo(const EB_MemberInfo *pMemberInfo, const EB_GroupInfo *pGroupInfo)
{
    m_viewType = VIEW_MEMBER;
    m_memberInfo = *pMemberInfo;
    m_groupInfo = *pGroupInfo;
    updateMemberInfo();
}

void EbDialogViewECard::setMemberInfo(eb::bigint groupId, eb::bigint memberUserId)
{
    if ( theApp->m_ebum.EB_GetGroupInfo(groupId,&m_groupInfo) &&
         theApp->m_ebum.EB_GetMemberInfoByUserId(&m_memberInfo,groupId,memberUserId) ) {
        m_viewType = VIEW_MEMBER;
        updateMemberInfo();
    }
    else {
        hideReset();
    }

}

void EbDialogViewECard::setContactInfo(const EB_ContactInfo *pContactInfo)
{
    m_viewType = VIEW_CONTACT;
    m_contactInfo = *pContactInfo;
    updateContactInfo();
}

void EbDialogViewECard::setContactInfo1(eb::bigint contactId)
{
    if ( theApp->m_ebum.EB_GetContactInfo1(contactId,&m_contactInfo) ) {
        m_viewType = VIEW_CONTACT;
        updateContactInfo();
    }
    else {
        hideReset();
    }
}

void EbDialogViewECard::setContactInfo2(eb::bigint userId)
{
    if ( theApp->m_ebum.EB_GetContactInfo2(userId,&m_contactInfo) ) {
        m_viewType = VIEW_CONTACT;
        updateContactInfo();
    }
    else {
        hideReset();
    }
}

void EbDialogViewECard::setMouseEnter(const QWidget * widgetValid, const QRect &rectValid, bool showImmediate)
{
    m_widgetValid = widgetValid;
    m_rectValid = rectValid;
    if (m_timerIdCheck2Hide!=0) {
        this->killTimer(m_timerIdCheck2Hide);
        m_timerIdCheck2Hide = 0;
    }
    if (m_timerIdCheck2Show!=0) {
        this->killTimer(m_timerIdCheck2Show);
    }
//    m_checkShowCount = 0;
    if (showImmediate)
        m_timerIdCheck2Show = this->startTimer(1);
    else
        m_timerIdCheck2Show = this->startTimer(1500);
}

void EbDialogViewECard::hideReset()
{
    if (m_timerIdCheck2Hide!=0) {
        this->killTimer(m_timerIdCheck2Hide);
        m_timerIdCheck2Hide = 0;
    }
//    m_checkShowCount = 0;
    if (m_timerIdCheck2Show!=0) {
        this->killTimer(m_timerIdCheck2Show);
        m_timerIdCheck2Show = 0;
    }
    m_viewType = VIEW_UNKNOWE;
    this->hide();
}

void EbDialogViewECard::onClieckedButtonAddContact()
{
    switch (m_viewType) {
    case VIEW_ENTERPRISE: {
        break;
    }
    case VIEW_GROUP: {
        break;
    }
    case VIEW_MEMBER: {
        /// 验证好友方式
        EbDialogRequestAddContact dlg;
        dlg.m_headFilePath = theApp->memberHeadFilePath(&m_memberInfo);
        dlg.m_fromName = m_memberInfo.m_sUserName.c_str();
        dlg.m_fromUserId = m_memberInfo.m_nMemberUserId;
        dlg.m_fromAccount = m_memberInfo.m_sMemberAccount.c_str();
        dlg.exec();
        break;
    }
    case VIEW_CONTACT: {
        /// 验证好友方式
        EbDialogRequestAddContact dlg;
        dlg.m_headFilePath = theApp->contactHeadFilePath(&m_contactInfo);
        dlg.m_fromName = m_contactInfo.m_sName.c_str();
        dlg.m_fromUserId = m_contactInfo.m_nContactUserId;
        dlg.m_fromAccount = m_contactInfo.m_sContact.c_str();
        dlg.exec();
        break;
    }
    default:
        break;
    }
    hideReset();
}

void EbDialogViewECard::onClieckedButtonSendMsg()
{
    switch (m_viewType) {
    case VIEW_ENTERPRISE: {
        break;
    }
    case VIEW_GROUP: {
        theApp->m_ebum.EB_CallGroup(m_groupInfo.m_sGroupCode);
        break;
    }
    case VIEW_MEMBER: {
        theApp->m_ebum.EB_CallUserId(m_memberInfo.m_nMemberUserId);
        break;
    }
    case VIEW_CONTACT: {
        if (m_contactInfo.m_nContactUserId>0)
            theApp->m_ebum.EB_CallUserId(m_contactInfo.m_nContactUserId);
        else
            theApp->m_ebum.EB_CallAccount(m_contactInfo.m_sContact.c_str());
        break;
    }
    default:
        break;
    }
    hideReset();
}

void EbDialogViewECard::resizeEvent(QResizeEvent *e)
{
    ui->labelImage->move( 8,8 );
    ui->labelType->setGeometry( width()-55,1,54,22 );

    int x = 75;
    int y = 6;  /// 12
    ui->labelTitle->setGeometry( x,y,width()-x,44 );
    y = this->titleBackgroundHeight()+1;
    ui->labelDescription->setGeometry( x,y,width()-x,44 );
    x = 40;
    y += (44+1);
    ui->labelDetail->setGeometry( x,y,width()-x,height()-y );

//    x = width()-65;
//    y = height()-25;
//    const int const_btn_width = 55;
//    const int const_btn_height = 19;
//    ui->pushButtonSendMsg->setGeometry( x,y,const_btn_width,const_btn_height );
//    x -= const_btn_width;
//    ui->pushButtonAddContact->setGeometry( x,y,const_btn_width,const_btn_height );

    EbDialogBase::resizeEvent(e);
}

void EbDialogViewECard::timerEvent(QTimerEvent *event)
{
    if ( m_timerIdCheck2Show!=0 && m_timerIdCheck2Show==event->timerId() ) {
        const QPoint pos = cursor().pos();
        const QRect & rectClient = this->geometry();
        if (m_widgetValid!=0) {
            const QPoint posWidgetValid = m_widgetValid->mapToGlobal(QPoint(0,0));
            const QRect rectWidgetValid(posWidgetValid, m_widgetValid->size());
            if (!rectWidgetValid.contains(pos)) {
                /// 鼠标不在当前有效窗口，不需要继续显示
                this->killTimer(m_timerIdCheck2Show);
                m_timerIdCheck2Show = 0;
                this->hide();
                return;
            }
        }

        if (this->isVisible() || rectClient.contains(pos) || this->m_rectValid.contains(pos) ) {
            this->killTimer(m_timerIdCheck2Show);
            m_timerIdCheck2Show = 0;
            this->show();
            this->activateWindow();
            if (m_timerIdCheck2Hide!=0) {
                this->killTimer(m_timerIdCheck2Hide);
            }
            m_timerIdCheck2Hide = this->startTimer(2000);
        }
        else if (m_widgetValid!=0 && !m_widgetValid->isVisible()) {
            /// 窗口已经隐藏，不需要继续显示
            this->killTimer(m_timerIdCheck2Show);
            m_timerIdCheck2Show = 0;
            return;
        }
//        else if ((++m_checkShowCount)>=3) {
//            /// 3*1.5=4.5秒左右
//            this->killTimer(m_timerIdCheck2Show);
//            m_timerIdCheck2Show = 0;
//        }
    }
    else if ( m_timerIdCheck2Hide!=0 && m_timerIdCheck2Hide==event->timerId() ) {
        const QRect& rect = this->geometry();
        if (!rect.contains(this->cursor().pos())) {
            this->killTimer(m_timerIdCheck2Hide);
            m_timerIdCheck2Hide = 0;
            this->setVisible(false);
        }
    }

    EbDialogBase::timerEvent(event);
}

void EbDialogViewECard::updateEnterpriseInfo()
{
    ui->labelType->setVisible(false);
    ui->labelImage->setPixmap( QPixmap(":/img/imgdefaultorg.png").scaled(const_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    QString text = QString("<font color=#ffffff ><b>%1<b></font><br>").arg(m_enterpriseInfo.m_sEnterpriseName.c_str());
    ui->labelTitle->setText(text);
    ui->labelTitle->setVisible(true);
    text = QString("%1").arg(m_enterpriseInfo.m_sDescription.c_str());
    ui->labelDescription->setText(text);
    ui->labelDescription->setVisible(true);
    text = QString("%1 : %2<br>%3 : %4<br>%5 : %6<br>%7 : %8<br>%9 : %10<br>")
            .arg(theLocales.getLocalText("name-text.tel","tel")).arg(m_enterpriseInfo.m_sPhone.c_str())
            .arg(theLocales.getLocalText("name-text.fax","fax")).arg(m_enterpriseInfo.m_sFax.c_str())
            .arg(theLocales.getLocalText("name-text.email","email")).arg(m_enterpriseInfo.m_sEmail.c_str())
            .arg(theLocales.getLocalText("name-text.url","url")).arg(m_enterpriseInfo.m_sUrl.c_str())
            .arg(theLocales.getLocalText("name-text.address","address")).arg(m_enterpriseInfo.m_sAddress.c_str());
    ui->labelDetail->setText(text);
    ui->labelDetail->setVisible(true);

    ui->pushButtonAddContact->setVisible(false);
    ui->pushButtonSendMsg->setVisible(false);
}

void EbDialogViewECard::updateGroupInfo()
{
    const QString name = theLocales.getGroupTypeName((int)m_groupInfo.m_nGroupType)->name().c_str();
    ui->labelType->setText(name);
    ui->labelType->setVisible(true);
    const QImage image = theApp->groupHeadImage( m_groupInfo.m_sGroupCode, m_groupInfo.m_nGroupType );
    ui->labelImage->setPixmap( QPixmap::fromImage(image).scaled(const_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    QString text;
    if (m_groupInfo.m_nGroupType==EB_GROUP_TYPE_TEMP) {
        text = QString("<font color=#ffffff ><b>%1<b><br>%2</font>")
                .arg(m_groupInfo.m_sGroupName.c_str())
                .arg(m_groupInfo.m_sDescription.c_str());
    }
    else {
        text = QString("<font color=#ffffff ><b>%1<b> (%2)<br>%3</font>")
                .arg(m_groupInfo.m_sGroupName.c_str())
                .arg(m_groupInfo.m_sGroupCode)
                .arg(m_groupInfo.m_sDescription.c_str());
    }
    ui->labelTitle->setText(text);
    ui->labelTitle->setVisible(true);
    if (m_groupInfo.m_sEnterpriseCode>0) {
        text = QString("%1<br>")
                .arg(m_enterpriseInfo.m_sEnterpriseName.c_str());
        ui->labelDescription->setText(text);
        ui->labelDescription->setVisible(true);
    }
    else {
        ui->labelDescription->setVisible(false);
    }
    text = QString("%1 : %2<br>%3 : %4<br>%5 : %6<br>%7 : %8<br>%9 : %10<br>")
            .arg(theLocales.getLocalText("name-text.tel","tel")).arg(m_groupInfo.m_sPhone.c_str())
            .arg(theLocales.getLocalText("name-text.fax","fax")).arg(m_groupInfo.m_sFax.c_str())
            .arg(theLocales.getLocalText("name-text.email","email")).arg(m_groupInfo.m_sEmail.c_str())
            .arg(theLocales.getLocalText("name-text.url","url")).arg(m_groupInfo.m_sUrl.c_str())
            .arg(theLocales.getLocalText("name-text.address","address")).arg(m_groupInfo.m_sAddress.c_str());
    ui->labelDetail->setText(text);
    ui->labelDetail->setVisible(true);

    int x = width()-65;
    int y = height()-25;
    ui->pushButtonAddContact->setVisible(false);
    if (m_groupInfo.m_nMyEmpId>0) {
        ui->pushButtonSendMsg->move(x,y);
        ui->pushButtonSendMsg->setVisible(true);
    }
    else {
        ui->pushButtonSendMsg->setVisible(false);
    }
}

void EbDialogViewECard::updateMemberInfo()
{
    if (m_groupInfo.m_nManagerUserId==m_memberInfo.m_nMemberUserId) {
        const QString manager = theLocales.getGroupTypeName((int)m_groupInfo.m_nGroupType)->manager().c_str();
        ui->labelType->setText(manager);
        ui->labelType->setVisible(true);
    }
    else {
        ui->labelType->setVisible(false);
    }
    const QString imageFilePath = theApp->memberHeadFilePath(&m_memberInfo);
    ui->labelImage->setPixmap( QPixmap(imageFilePath).scaled(const_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    QString text = QString("<font color=#ffffff ><b>%1<b> (%2)</font>")
            .arg(m_memberInfo.m_sUserName.c_str())
            .arg(m_memberInfo.m_nMemberUserId);
    ui->labelTitle->setText(text);
    ui->labelTitle->setVisible(true);
    if (m_groupInfo.m_sEnterpriseCode>0) {
        tstring enterpriseName;
        theApp->m_ebum.EB_GetEnterpriseName(enterpriseName,m_groupInfo.m_sEnterpriseCode);
        text = QString("%1 %2<br>%3")
                .arg(m_groupInfo.m_sGroupName.c_str())
                .arg(m_memberInfo.m_sJobTitle.c_str())
                .arg(enterpriseName.c_str());
    }
    else {
        text = QString("%1<br>%2")
                .arg(m_groupInfo.m_sGroupName.c_str())
                .arg(m_memberInfo.m_sJobTitle.c_str());
    }
    ui->labelDescription->setText(text);
    ui->labelDescription->setVisible(true);
    text = QString("%1 : %2<br>%3 : %4<br>%5 : %6<br>%7 : %8<br>%9 : %10<br>")
            .arg(theLocales.getLocalText("name-text.mobile","mobile")).arg(m_memberInfo.m_sCellPhone.c_str())
            .arg(theLocales.getLocalText("name-text.tel","tel")).arg(m_memberInfo.m_sWorkPhone.c_str())
            .arg(theLocales.getLocalText("name-text.fax","fax")).arg(m_memberInfo.m_sFax.c_str())
            .arg(theLocales.getLocalText("name-text.email","email")).arg(m_memberInfo.m_sEmail.c_str())
            .arg(theLocales.getLocalText("name-text.address","address")).arg(m_memberInfo.m_sAddress.c_str());
    ui->labelDetail->setText(text);
    ui->labelDetail->setVisible(true);

    int x = width()-65;
    int y = height()-25;
    if ( m_memberInfo.m_nMemberUserId!=theApp->logonUserId() &&
         !theApp->m_ebum.EB_IsMyContactAccount2(m_memberInfo.m_nMemberUserId) ) {
        /// 非好友，申请“加为好友”
        ui->pushButtonAddContact->move(x,y);
        ui->pushButtonAddContact->setVisible(true);
        x -= const_button_size.width();
    }
    else{
        /// 已经是我的好友
        ui->pushButtonAddContact->setVisible(false);
    }
    if ( m_memberInfo.m_nMemberUserId!=theApp->logonUserId() ) {
        ui->pushButtonSendMsg->move(x,y);
        ui->pushButtonSendMsg->setVisible(true);
    }
    else {
        ui->pushButtonSendMsg->setVisible(false);
    }
}

void EbDialogViewECard::updateContactInfo()
{
    if ((m_contactInfo.m_nContactType&EB_CONTACT_TYPE_MAIL)==EB_CONTACT_TYPE_MAIL) {
        /// 邮件联系人
        ui->labelType->setText( theLocales.getLocalText("name-text.mail-type-contact","Mail Contact") );
        ui->labelType->setVisible(true);
    }
    else {
        ui->labelType->setVisible(false);
    }
    const QString imageFilePath = theApp->contactHeadFilePath(&m_contactInfo);
    ui->labelImage->setPixmap( QPixmap(imageFilePath).scaled(const_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    QString text;
    if (m_contactInfo.m_nContactUserId>0) {
        text = QString("<font color=#ffffff ><b>%1<b> (%2)</font>")
                .arg(m_contactInfo.m_sName.c_str())
                .arg(m_contactInfo.m_nContactUserId);
    }
    else {
        text = QString("<font color=#ffffff ><b>%1<b> (%2)</font>")
                .arg(m_contactInfo.m_sName.c_str())
                .arg(m_contactInfo.m_sContact.c_str());
    }
    ui->labelTitle->setText(text);
    ui->labelTitle->setVisible(true);
    text = QString("%1<br>%2")
            .arg(m_contactInfo.m_sGroupName.c_str())
            .arg(m_contactInfo.m_sJobTitle.c_str());
    ui->labelDescription->setText(text);
    ui->labelDescription->setVisible(true);
    text = QString("%1 : %2<br>%3 : %4<br>%5 : %6<br>%7 : %8<br>%9 : %10<br>")
            .arg(theLocales.getLocalText("name-text.mobile","mobile")).arg(m_contactInfo.m_sPhone.c_str())
            .arg(theLocales.getLocalText("name-text.tel","tel")).arg(m_contactInfo.m_sTel.c_str())
            .arg(theLocales.getLocalText("name-text.fax","fax")).arg(m_contactInfo.m_sFax.c_str())
            .arg(theLocales.getLocalText("name-text.email","email")).arg(m_contactInfo.m_sEmail.c_str())
            .arg(theLocales.getLocalText("name-text.address","address")).arg(m_contactInfo.m_sAddress.c_str());
    ui->labelDetail->setText(text);
    ui->labelDetail->setVisible(true);

    int x = width()-65;
    int y = height()-25;
    if ( m_contactInfo.m_nContactUserId!=theApp->logonUserId() &&
         (m_contactInfo.m_nContactUserId==0 ||
          (theApp->isAuthContact() && m_contactInfo.m_nContactUserId>0
           && (m_contactInfo.m_nContactType&EB_CONTACT_TYPE_AUTH)==0)) ) {
        /// 非好友，申请“加为好友”
        ui->pushButtonAddContact->move(x,y);
        ui->pushButtonAddContact->setVisible(true);
        x -= const_button_size.width();
    }
    else{
        /// 已经是我的好友，或其他
        ui->pushButtonAddContact->setVisible(false);
    }
    if ( m_contactInfo.m_nContactUserId>0 && m_contactInfo.m_nContactUserId!=theApp->logonUserId() ) {
        ui->pushButtonSendMsg->move(x,y);
        ui->pushButtonSendMsg->setVisible(true);
    }
    else {
        ui->pushButtonSendMsg->setVisible(false);
    }
}

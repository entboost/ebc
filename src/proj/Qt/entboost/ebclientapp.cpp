#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include "ebclientapp.h"
#include "../../../include/colorconver.h"
#include "../../../include/ebc_public.h"
#include "../include/boost_ini.h"
#include "ebhttpfiledownload.h"
#include "ebmessagebox.h"
#include "ebdialogemotionselect.h"
#include "ebdialogmemberinfo.h"
#include "ebdialoggroupinfo.h"
#include "ebdialogviewecard.h"
#include "ebdialogcontactinfo.h"
#include "ebwebenginedownloadrequested.h"

EbClientApp::pointer theApp;
//EbcLocales theLocales;
//CEBParseSetting theSetting;
//CEBAppClient this->m_ebum;

EbClientApp::EbClientApp(QObject *parent)
    : EbClientAppBase(parent)
    , m_nSelectCallId(0)
    , m_nSelectGroupId(0)
//    , m_hotColor(m_mainColor), m_preColor(m_mainColor)

    , m_nDefaultBrowserType(EB_BROWSER_TYPE_CEF)
    , m_nDefaultUIStyleType(EB_UI_STYLE_TYPE_OFFICE)    //EB_UI_STYLE_TYPE_CHAT;
    , m_mainWnd(0)
    , m_receiver(0)
    , m_dialogEmotionSelect(0)
    , m_dialogViewECard(0)
    , m_webWngineDownloadRequested(0)

{
    // ? for test
    m_nDefaultUIStyleType = EB_UI_STYLE_TYPE_CHAT;

    QDesktopWidget* desktopWidget = QApplication::desktop();
    /// 获取可用桌面大小
    m_deskRect = desktopWidget->availableGeometry();
    /// 获取设备屏幕大小（得到应用程序矩形）
    m_screenRect = desktopWidget->screenGeometry();
    /// 获取系统设置的屏幕个数（屏幕拷贝方式该值为1）
//    g_nScreenCount = desktopWidget->screenCount();

}
EbClientApp::~EbClientApp(void)
{
    exitApp( false );
    if (m_dialogEmotionSelect!=0) {
        delete m_dialogEmotionSelect;
        m_dialogEmotionSelect = 0;
    }
    if (m_dialogViewECard!=0) {
        delete m_dialogViewECard;
        m_dialogViewECard = 0;
    }
    if (m_webWngineDownloadRequested!=0) {
        delete m_webWngineDownloadRequested;
        m_webWngineDownloadRequested = 0;
    }
}

EbClientApp::pointer EbClientApp::create(QObject *parent)
{
    return EbClientApp::pointer( new EbClientApp(parent) );
}

void EbClientApp::clearSubscribeSelectInfo()
{
    m_nSelectCallId = 0;
    m_nSelectGroupId = 0;
}

//void EbClientApp::triggeredApps(eb::bigint subId)
//{

//}


bool EbClientApp::setDevAppId(QObject* receiver)
{
    m_receiver = receiver;
    const CEBString sAddress = this->m_setting.GetServerAddress();
    //entboost::GetAccountAddress(lpszAccount, sAddress);
    //this->m_ebum.Stop();
    this->m_ebum.EB_Init(sAddress.c_str());
    //this->m_ebum.EB_SetMsgHwnd(this->GetSafeHwnd());
    if (!this->m_ebum.EB_IsInited()) {
        return false;
    }
    this->m_ebum.EB_SetMsgReceiver(this);
    if (this->m_ebum.EB_SetDevAppId(278573612908LL,"ec1b9c69094db40d9ada80d657e08cc6",true)!=0) {
        return false;
    }
    return true;
}

bool EbClientApp::setDefaultUIStyleType(EB_UI_STYLE_TYPE newValue)
{
    if (m_nDefaultUIStyleType != newValue) {
        m_nDefaultUIStyleType = newValue;
        WritePrivateProfileIntABoost("default", "uistyle-type", (int)m_nDefaultUIStyleType,
                                     m_userSettingIniFile.toLocal8Bit().constData());
        return true;
    }
    return false;
}

void EbClientApp::setSendKeyType(EB_SEND_KEY_TYPE v)
{
    if (m_sendKeyType != v) {
        m_sendKeyType = v;
        WritePrivateProfileIntABoost("default", "send-type", (int)m_sendKeyType,
                                     m_userSettingIniFile.toLocal8Bit().constData());
    }
}

EbDialogEmotionSelect* EbClientApp::showDialogEmotionSelect(const QPoint& pt,QObject* receiver)
{
    if (m_dialogEmotionSelect==0) {
        m_dialogEmotionSelect = new EbDialogEmotionSelect;
        m_dialogEmotionSelect->setVisible(false);
        m_dialogEmotionSelect->setModal(false);
        m_dialogEmotionSelect->setWindowModality(Qt::WindowModal);
    }
    if (receiver==0) {
        return m_dialogEmotionSelect;
    }
    m_dialogEmotionSelect->disconnect();
    receiver->connect( m_dialogEmotionSelect,SIGNAL(selectedResource(QIcon,mycp::bigint,QString,QString)),receiver,SLOT(onSelectedResourceEmotion(QIcon,mycp::bigint,QString,QString)) );
    const QRect & rect = m_dialogEmotionSelect->geometry();
    m_dialogEmotionSelect->move(pt.x()-rect.width()/3,pt.y()-rect.height());
    m_dialogEmotionSelect->setFocus();
    m_dialogEmotionSelect->show();
    return m_dialogEmotionSelect;
}

EbDialogViewECard *EbClientApp::dialgoViewECard(const QWidget * wdigetValid, const QRect &rectValid, bool showImmediate)
{
    if (m_dialogViewECard==0) {
        m_dialogViewECard = new EbDialogViewECard;
        m_dialogViewECard->setVisible(false);
        m_dialogViewECard->setModal(false);
        m_dialogViewECard->setWindowModality(Qt::WindowModal);
    }
    /// 默认显示在左边
    int x = rectValid.x()-m_dialogViewECard->geometry().width()-10;
    if (x<0) {
        /// 显示在右边
        x = rectValid.right()+10;
    }
    int y = MAX(0,rectValid.y()-10);    /// -10 往上移一点
    if (y+m_dialogViewECard->geometry().height()>this->screenRect().height()) {
        y = this->screenRect().height()-m_dialogViewECard->geometry().height();
    }
    m_dialogViewECard->move(x,y);
    m_dialogViewECard->setMouseEnter(wdigetValid, rectValid, showImmediate);
    return m_dialogViewECard;
}

void EbClientApp::hideViewECard(const QWidget *hideViewECard)
{
    if (m_dialogViewECard!=0) {
        m_dialogViewECard->hideReset(hideViewECard);
    }
}

void EbClientApp::editContactInfo(mycp::bigint contactId, QWidget *parent)
{
    EB_ContactInfo pContactInfo;
    if (!this->m_ebum.EB_GetContactInfo1(contactId,&pContactInfo)) {
        return;
    }
    EbDialogContactInfo dlgContactInfo(parent);
    dlgContactInfo.m_contactInfo = pContactInfo;
    if ( dlgContactInfo.exec()==QDialog::Accepted ) {
        theApp->m_ebum.EB_EditContact(&dlgContactInfo.m_contactInfo);
    }
}

void EbClientApp::editGroupInfo(mycp::bigint groupId, QWidget *parent)
{
    EB_GroupInfo pGroupInfo;
    if ( !m_ebum.EB_GetGroupInfo(groupId,&pGroupInfo) ) {
        return;
    }
    else if (!m_ebum.EB_CanEditGroupInfo(pGroupInfo.m_sEnterpriseCode,pGroupInfo.m_sGroupCode))
    //}else if (pGroupInfo.m_nCreateUserId != m_ebum.EB_GetLogonUserId())
    {
        /// 不是部门创建者，不能编辑
        return;
    }

    EbDialogGroupInfo pDlgGroupInfo(parent);
    pDlgGroupInfo.m_groupInfo = pGroupInfo;
    if (pDlgGroupInfo.exec()==QDialog::Accepted) {
        EB_GroupInfo pPopDepartment(&pDlgGroupInfo.m_groupInfo);
        m_ebum.EB_EditGroup(&pPopDepartment);
    }
}

void EbClientApp::newGroupInfo(EB_GROUP_TYPE groupType, eb::bigint enterpriseId, eb::bigint parentGroupId, QWidget *parent)
{
    EbDialogGroupInfo pDlgGroupInfo(parent);
    pDlgGroupInfo.m_groupInfo.m_nGroupType = groupType;
    pDlgGroupInfo.m_groupInfo.m_sEnterpriseCode = enterpriseId;
    pDlgGroupInfo.m_groupInfo.m_sParentCode = parentGroupId;
    if (pDlgGroupInfo.exec()==QDialog::Accepted) {
        pDlgGroupInfo.m_groupInfo.m_nCreateUserId = theApp->logonUserId();
        m_ebum.EB_EditGroup(&pDlgGroupInfo.m_groupInfo);
    }
}

void EbClientApp::newMemberInfo(EB_GROUP_TYPE groupType, eb::bigint groupId, const QString &groupName, QWidget *parent)
{
    EbDialogMemberInfo dlg(parent);
    dlg.m_memberInfo.m_sGroupCode = groupId;
    dlg.m_groupType = groupType;
    dlg.m_groupName = groupName;
    if (dlg.exec()==QDialog::Accepted) {
        m_ebum.EB_EditMember(&dlg.m_memberInfo);
    }
}

void EbClientApp::editMemberInfo(const EB_MemberInfo *pMemberInfo, QWidget *parent)
{
    if (pMemberInfo==NULL) return;
    tstring groupName;
    if (!this->m_ebum.EB_GetGroupName(pMemberInfo->m_sGroupCode,groupName)) return;
    //EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
    //m_ebum.EB_GetGroupType(pMemberInfo->m_sGroupCode,nGroupType);

    EbDialogMemberInfo dlg(parent);
    //pDlgMemberInfo.m_nGroupType = nGroupType;
    dlg.m_memberInfo = pMemberInfo;
    dlg.m_groupName = groupName.c_str();
    if (dlg.exec()==QDialog::Accepted) {
        m_ebum.EB_EditMember(&dlg.m_memberInfo);
    }
}

void EbClientApp::deleteDbRecord(eb::bigint sId, bool bIsAccount)
{
    /// 先删除图片，语音，文件...
    if ( this->m_sqliteUser.get()==0 ) {
        return;
    }
    char sSql[256];
    if (bIsAccount) {
        sprintf(sSql,"select msg_name,msg_text FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) AND msg_type>=%d",sId,sId,(int)MRT_JPG);
    }
    else {
        sprintf(sSql,"select msg_name,msg_text FROM msg_record_t WHERE dep_code=%lld AND msg_type>=%d",sId,(int)MRT_JPG);
    }
    const QString sUserImagePath(userImagePath());
    const QString sUserFilePath(userFilePath());
    int nCookie = 0;
    m_sqliteUser->select( sSql, nCookie );
    cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        const QString sMsgName( pRecord->getVector()[0]->getStrValue().c_str() );
        const QString sMsgText( pRecord->getVector()[1]->getStrValue().c_str() );
        pRecord = m_sqliteUser->next(nCookie);
        /// 判断是临时图片，语音，文件目录
        if ( !sMsgName.isEmpty() && sMsgName.indexOf( sUserImagePath,Qt::CaseInsensitive )==0) {
            /// 删除临时目录图片
            QFile::remove(sMsgName);
        }
        if ( !sMsgText.isEmpty() && sMsgText.indexOf( sUserFilePath,Qt::CaseInsensitive )==0) {
            /// 删除临时目录图片
            QFile::remove(sMsgText);
        }
    }
    m_sqliteUser->reset(nCookie);
    /// 删除数据
    if (bIsAccount) {
        sprintf(sSql,"DELETE FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld)",sId,sId);
    }
    else {
        sprintf(sSql, "DELETE FROM msg_record_t WHERE dep_code=%lld",sId);
    }
    m_sqliteUser->execute(sSql);
}

void EbClientApp::deleteDbRecord(eb::bigint sMsgId)
{
    /// 先删除图片，语音，文件...
    if ( this->m_sqliteUser.get()==0 ) {
        return;
    }
    char sSql[256];
    sprintf( sSql, "select msg_name,msg_text FROM msg_record_t WHERE msg_id=%lld AND msg_type>=%d", sMsgId, (int)MRT_JPG);
    const QString sUserImagePath(userImagePath());
    const QString sUserFilePath(userFilePath());
    int nCookie = 0;
    m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
    while (pRecord.get()!=NULL)
    {
        const QString sMsgName( pRecord->getVector()[0]->getStrValue().c_str() );
        const QString sMsgText( pRecord->getVector()[1]->getStrValue().c_str() );
        pRecord = m_sqliteUser->next(nCookie);
        /// 判断是临时图片，语音，文件目录
        if ( !sMsgName.isEmpty() && sMsgName.indexOf( sUserImagePath,Qt::CaseInsensitive )==0) {
            /// 删除临时目录图片
            QFile::remove(sMsgName);
        }
        if ( !sMsgText.isEmpty() && sMsgText.indexOf( sUserFilePath,Qt::CaseInsensitive )==0) {
            /// 删除临时目录图片
            QFile::remove(sMsgText);
        }
    }
    m_sqliteUser->reset(nCookie);
    /// 删除数据
    sprintf( sSql, "DELETE FROM msg_record_t WHERE msg_id=%lld", sMsgId );
    m_sqliteUser->execute(sSql);
}

void EbClientApp::updateMsgReceiptData(eb::bigint nMsgId, eb::bigint nFromUserId, int nAckType)
{
    /// nAckType=0 对方收到消息
    /// nAckType=4 请求撤回消息
    /// nAckType=6 请求“个人收藏”消息
    /// nAckType=7 请求“群收藏”消息
    if (this->m_sqliteUser.get()==0) {
        return;
    }
    char sSql[256];
    if (nAckType==6) {
        /// 个人收藏
        return;
    }
    else if (nAckType==7) {
        /// 群收藏
        return;
    }
    else if (nAckType==4) {
        sprintf( sSql, "UPDATE msg_record_t SET msg_name='',msg_text='',read_flag=read_flag|%d WHERE msg_id=%lld AND from_uid=%lld AND (read_flag&%d)=0",EBC_READ_FLAG_WITHDRAW,nMsgId,nFromUserId,EBC_READ_FLAG_WITHDRAW);
    }
    else if (nAckType==0) {
        /// ?
        sprintf(sSql, "UPDATE msg_record_t SET read_flag=read_flag|%d WHERE msg_id=%lld AND dep_code=0 AND to_uid=%lld AND (read_flag&%d)=0",EBC_READ_FLAG_RECEIPT,nMsgId,nFromUserId,EBC_READ_FLAG_RECEIPT);
    }
    m_sqliteUser->execute(sSql);
}

QImage EbClientApp::userHeadImage(eb::bigint userId, eb::bigint memberId, const cgcSmartString &account) const
{
    EB_MemberInfo pMemberInfo;
    bool findMemberInfo = false;
    if (memberId>0)
        findMemberInfo = this->m_ebum.EB_GetMemberInfoByMemberCode(&pMemberInfo,memberId);
    if (!findMemberInfo && userId>0)
        findMemberInfo = this->m_ebum.EB_GetMemberInfoByUserId2(&pMemberInfo,userId);
    if (!findMemberInfo && !account.empty())
        findMemberInfo = this->m_ebum.EB_GetMemberInfoByAccount2(&pMemberInfo,account.c_str());
    if (findMemberInfo) {
        return memberHeadImage(&pMemberInfo);
    }
    EB_ContactInfo contactInfo;
    if (userId>0 && m_ebum.EB_GetContactInfo2(userId, &contactInfo)) {
        if ( QFileInfo::exists(contactInfo.m_sHeadResourceFile) ) {
            return contactHeadImage(&contactInfo);
        }
    }
    const QString imagePath = memberId>0?":/img/defaultmember.png":":/img/defaultcontact.png";
    return fromHeadImage(imagePath,EB_LINE_STATE_UNKNOWN);
}

QString EbClientApp::userHeadFilePath(mycp::bigint nUserId, eb::bigint memberId, const cgcSmartString &sAccount) const
{
    EB_MemberInfo pMemberInfo;
    bool findMemberInfo = false;
    if (memberId>0)
        findMemberInfo = this->m_ebum.EB_GetMemberInfoByMemberCode(&pMemberInfo,memberId);
    if (!findMemberInfo && nUserId>0)
        findMemberInfo = this->m_ebum.EB_GetMemberInfoByUserId2(&pMemberInfo,nUserId);
    if (!findMemberInfo && !sAccount.empty())
        findMemberInfo = this->m_ebum.EB_GetMemberInfoByAccount2(&pMemberInfo,sAccount.c_str());
    if (findMemberInfo) {
        return memberHeadFilePath(&pMemberInfo);
    }
    EB_ContactInfo contactInfo;
    if (nUserId>0 && m_ebum.EB_GetContactInfo2(nUserId, &contactInfo)) {
        if ( QFileInfo::exists(contactInfo.m_sHeadResourceFile) ) {
            return contactInfo.m_sHeadResourceFile;
        }
    }
    return memberId>0?":/img/defaultmember.png":":/img/defaultcontact.png";
}

QImage EbClientApp::funcImage(const EB_SubscribeFuncInfo *funcInfo) const
{
    QString imagePath;
    if ( funcInfo!=0 && QFileInfo::exists(funcInfo->m_sResFile) ) {
        imagePath = funcInfo->m_sResFile;
    }
    if (imagePath.isEmpty()) {
        imagePath = ":/img/defaultapp.png";
    }
    return QImage(imagePath);
}


QImage EbClientApp::memberHeadImage(const EB_MemberInfo *memberInfo) const
{
    const EB_USER_LINE_STATE lineState = memberInfo==0?EB_LINE_STATE_UNKNOWN:memberInfo->m_nLineState;
    return fromHeadImage(memberHeadFilePath(memberInfo),lineState);
//    tstring imagePath;
//    if ( memberInfo!=0 && QFileInfo::exists(memberInfo->m_sHeadResourceFile.c_str()) ) {
//        imagePath = memberInfo->m_sHeadResourceFile;
//    }
//    if (imagePath.empty()) {
//        imagePath = ":/img/defaultmember.png";
//    }
//    return fromHeadImage(imagePath,lineState);
}

QImage EbClientApp::contactHeadImage(const EB_ContactInfo *contactInfo) const
{
    const EB_USER_LINE_STATE lineState = contactInfo==0?EB_LINE_STATE_UNKNOWN:contactInfo->m_nLineState;
    return fromHeadImage(contactHeadFilePath(contactInfo),lineState);
}

QString EbClientApp::contactHeadFilePath(const EB_ContactInfo *contactInfo) const
{
    if ( contactInfo!=0 && QFileInfo::exists(contactInfo->m_sHeadResourceFile) ) {
        return contactInfo->m_sHeadResourceFile;
    }
    return ":/img/defaultcontact.png";
}

QImage EbClientApp::fromHeadImage(const QString &imagePath, EB_USER_LINE_STATE lineState) const
{
    QImage image(imagePath);
    /// 在线状态图标
    bool viewGrayImage = false;
    QImage imageLineState;
    switch (lineState)
    {
    case EB_LINE_STATE_UNKNOWN:
    case EB_LINE_STATE_OFFLINE:
        viewGrayImage = true;
        break;
    case EB_LINE_STATE_ONLINE_NEW:
        break;
    case EB_LINE_STATE_BUSY:
        imageLineState = QImage(":/img/btnstatebusy.png");
        break;
    case EB_LINE_STATE_AWAY:
        imageLineState = QImage(":/img/btnstateaway.png");
        break;
    default:
        break;
    }
    if (viewGrayImage) {
        /// 离线状态，显示灰色头像
        image = libEbc::imageToGray(image);
    }
    else {
        if (!imageLineState.isNull()) {
            image = libEbc::imageAdd(image,imageLineState,image.width()-imageLineState.width()+3,image.height()-imageLineState.height()+3);
        }
    }
    return image;
}

QString EbClientApp::memberHeadFilePath(const EB_MemberInfo *memberInfo) const
{
    if ( memberInfo!=0 && QFileInfo::exists(memberInfo->m_sHeadResourceFile) ) {
        return memberInfo->m_sHeadResourceFile;
    }
    return ":/img/defaultmember.png";
}

QImage EbClientApp::groupHeadImage(eb::bigint groupId, EB_GROUP_TYPE groupType) const
{
    QImage groupImage;
    switch (groupType) {
    case EB_GROUP_TYPE_DEPARTMENT:
        groupImage = QImage(":/img/imgdefaultdepartment.png");
        break;
    case EB_GROUP_TYPE_PROJECT:
        groupImage = QImage(":/img/imgdefaultproject.png");
        break;
    case EB_GROUP_TYPE_GROUP:
        groupImage = QImage(":/img/imgdefaultgroup.png");
        break;
//    case EB_GROUP_TYPE_TEMP:
    default:
        groupImage = QImage(":/img/imgdefaulttempgroup.png");
        break;
    }
    if ( m_ebum.EB_IsGroupForbidSpeech(groupId) ) {
        /// 禁言群组，加上禁方图标，显示在右下角
         const QImage imageForbid = QImage(":/img/imgstateforbid.png");
         groupImage = libEbc::imageAdd(groupImage,imageForbid,groupImage.width()-imageForbid.width(),groupImage.height()-imageForbid.height());
    }
    return groupImage;
}

//cgcSmartString EbClientApp::groupHeadFilePath(EB_GROUP_TYPE groupType) const
//{
//    switch (groupType) {
//    case EB_GROUP_TYPE_DEPARTMENT:
//        return ":/img/imgdefaultdepartment.png";
//    case EB_GROUP_TYPE_PROJECT:
//        return ":/img/imgdefaultproject.png";
//    case EB_GROUP_TYPE_GROUP:
//        return ":/img/imgdefaultgroup.png";
//    default:
//        return ":/img/imgdefaulttempgroup.png";
//    }
//    return ":/img/imgdefaulttempgroup.png";
//}

void EbClientApp::customEvent(QEvent *e)
{
    const QEvent::Type eventType = e->type();
    switch ((int)eventType) {
    case EB_WM_APPID_SUCCESS:
        onAppIdSuccess(e);
        break;
    case EB_WM_APPID_ERROR:
        onAppIdError(e);
        break;
    default:
        break;
    }
}


void EbClientApp::onAppIdSuccess(QEvent * e)
{
    m_isAppIdResponseOk = true;

    unsigned long pAccountPrefix = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,&pAccountPrefix);
    if (pAccountPrefix!=0 && strlen((const char*)pAccountPrefix)>0)
    {
        m_sAccountPrefix = (const char*)pAccountPrefix;
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX,pAccountPrefix);
    }

    unsigned long nLicenseUser = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LICENSE_USER,&nLicenseUser);
    m_bLicenseUser = nLicenseUser==0?false:true;
    if (m_bLicenseUser) {
        unsigned long pProductName = 0;
        this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,&pProductName);
        if (pProductName!=0 && strlen((const char*)pProductName)>0) {
            const std::string productName((const char*)pProductName);
            this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_PRODUCT_NAME,pProductName);
            m_sProductName = QString::fromStdString(productName);
            char sql[256];
            sprintf(sql, "UPDATE sys_value_t SET value1='%s' WHERE name='product-name'",productName.c_str());
            m_sqliteEbc->execute(sql);
//            if (m_receiver!=NULL)
//                QApplication::postEvent(m_receiver, new QEvent((QEvent::Type)EB_COMMAND_UPDATE_PRODUCT_NAME));
        }
        else {
            /// 没有配置，使用默认
            m_sqliteEbc->execute("UPDATE sys_value_t SET value1='' WHERE name='product-name'");
            this->m_sProductName = QString::fromStdString( this->m_setting.GetEnterprise() );
        }
    }
    unsigned long nSendRegMail = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SEND_REG_MAIL,&nSendRegMail);
    m_bSendRegMail = nSendRegMail==1?true:false;
    unsigned long nSaveConversatios = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_SAVE_CONVERSATIONS,&nSaveConversatios);
    m_nSaveConversations = nSaveConversatios;
    unsigned long nAuthContact = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTH_CONTACT,&nAuthContact);
    m_bAuthContact = nAuthContact==1?true:false;

    unsigned long pDeployId = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,&pDeployId);
    if (pDeployId!=0 && strlen((const char*)pDeployId)>0)
    {
        m_nDeployId = eb_atoi64((const char*)pDeployId);
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_DEPLOY_ID,pDeployId);
    }
    unsigned long nLicenseType = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_LICENSE_TYPE,&nLicenseType);
    m_nLicenstType = nLicenseType;
    unsigned long nEBServerVersion = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_EB_SERVER_VERSION,&nEBServerVersion);
    m_nEBServerVersion = nEBServerVersion;
    //unsigned long pGroupMsgSubId = 0;
    //this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,&pGroupMsgSubId);
    //if (pGroupMsgSubId!=NULL && strlen((const char*)pGroupMsgSubId)>0)
    //{
    //	m_nGroupMsgSubId = eb_atoi64((const char*)pGroupMsgSubId);
    //	this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID,pGroupMsgSubId);
    //}
    unsigned long pAutoOpenSubId = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID,&pAutoOpenSubId);
    if (pAutoOpenSubId!=0 && strlen((const char*)pAutoOpenSubId)>0)
    {
        m_nAutoOpenSubId = eb_atoi64((const char*)pAutoOpenSubId);
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID,pAutoOpenSubId);
    }
    unsigned long nAutoHideMainFrame = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_AUTOHIDE_MAINFRAME,&nAutoHideMainFrame);
    m_bAutoHideMainFrame = nAutoHideMainFrame==1?true:false;
    unsigned long nHideMainFrame = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_HIDE_MAINFRAME,&nHideMainFrame);
    m_bHideMainFrame = nHideMainFrame==1?true:false;
    unsigned long nDisableUserCloudDrive = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_USER_CLOUD_DRIVE,&nDisableUserCloudDrive);
    m_bDisableUserCloudDrive = nDisableUserCloudDrive==1?true:false;
    unsigned long nDisableGroupShareCloud = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_GROUP_SHARED_CLOUD,&nDisableGroupShareCloud);
    m_bDisableGroupShareCloud = nDisableGroupShareCloud==1?true:false;
    unsigned long nDisableModifyPasswd = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_MODIFY_PASSWD,&nDisableModifyPasswd);
    m_bDisableModifyPasswd = nDisableModifyPasswd==1?true:false;
    unsigned long nDisableVideo = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_VIDEO,&nDisableVideo);
    m_bDisableVideo = nDisableVideo==1?true:false;
    unsigned long nDisableRD = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_REMOTE_DESKTOP,&nDisableRD);
    m_bDisableRemoteDesktop = nDisableRD==1?true:false;
    unsigned long nDisableAccountEdit = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_ACCOUNT_EDIT,&nDisableAccountEdit);
    m_bDisableAccountEdit = nDisableAccountEdit==1?true:false;
    unsigned long nDefaultBrowserType = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEFAULT_BROWSER_TYPE,&nDefaultBrowserType);
    m_nDefaultBrowserType = nDefaultBrowserType==1?EB_BROWSER_TYPE_IE:EB_BROWSER_TYPE_CEF;
    unsigned long nDisableMsgReceipt = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DISABLE_MSG_RECEIPT,&nDisableMsgReceipt);
    m_bDisableMsgReceipt = nDisableMsgReceipt==1?true:false;
    unsigned long nStatSubGroupMember = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_STAT_SUB_GROUP_MEMBER,&nStatSubGroupMember);
    m_bStatSubGroupMember = nStatSubGroupMember==1?true:false;
    unsigned long pDefaultUrl = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_DEFAULT_URL,&pDefaultUrl);
    if (pDefaultUrl!=0 && strlen((const char*)pDefaultUrl)>0)
    {
        m_sDefaultUrl = (const char*)pDefaultUrl;
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_DEFAULT_URL,pDefaultUrl);
    }

    unsigned long nOpenRegisiter = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_REGISTER,&nOpenRegisiter);
    m_nOpenRegister = nOpenRegisiter;
    unsigned long nOpenVisitor = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_OPEN_VISITOR,&nOpenVisitor);
    m_bOpenVisitor = nOpenVisitor==0?false:true;
    unsigned long pForgetPwdUrl = 0;
    this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL,&pForgetPwdUrl);
    if (pForgetPwdUrl!=0 && strlen((const char*)pForgetPwdUrl)>0)
    {
        m_sForgetPwdUrl = (const char*)pForgetPwdUrl;
        this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_FORGET_PWD_URL,pForgetPwdUrl);
    }

    const QString sEntLogoImagePath = m_appImgPath + "/entlogo";			/// 企业定制LOGO
    const QString sEntLogoImagePathTemp = m_appImgPath + "/entlogotemp";	/// 先保存到临时中间文件
    if (m_bLicenseUser) {
        /// 企业LOGO
        unsigned long pEntLogoUrl = 0;
        this->m_ebum.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL,&pEntLogoUrl);
        const std::string sImageTempIniPath = m_appImgPath.toLocal8Bit().toStdString() + "/temp.ini";
        if (pEntLogoUrl!=0 && strlen((const char*)pEntLogoUrl)>0) {
            /// http://test-um.entboost.com/images/entlogo.png
            const std::string sEntLogoUrl((const char*)pEntLogoUrl);
            this->m_ebum.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_LOGO_URL,pEntLogoUrl);
            char lpszEntLogoLastModified[64];
            memset(lpszEntLogoLastModified,0,64);
            GetPrivateProfileStringABoost( "entlogo", "last_modified", "",lpszEntLogoLastModified,64,sImageTempIniPath.c_str());
            if (!QFile::exists(sEntLogoImagePath)) {
                memset(lpszEntLogoLastModified,0,64);
            }
            const QString sOldLastModified(lpszEntLogoLastModified);
            m_httpFileDownload.downloadHttpFile( QString::fromStdString(sEntLogoUrl), sEntLogoImagePathTemp, sOldLastModified );
            if (m_httpFileDownload.getDownloadFinished() && m_httpFileDownload.getLastErrorCode()==QNetworkReply::NoError) {
                if (QFile::exists(sEntLogoImagePathTemp)) {
                    QFile::remove(sEntLogoImagePath);
                    QFile::rename(sEntLogoImagePathTemp, sEntLogoImagePath);
                    WritePrivateProfileStringABoost("entlogo", "last_modified",
                                                    m_httpFileDownload.getLastModified().toStdString().c_str(),
                                                    sImageTempIniPath.c_str());
                }
//                if (m_receiver!=NULL)
//                    QApplication::postEvent(m_receiver, new QEvent((QEvent::Type)EB_COMMAND_UPDATE_ENT_LOGO));
            }
            else if ( m_httpFileDownload.getDownloadFinished() && m_httpFileDownload.getLastModified()==sOldLastModified) {
                /// 下载文件没有改变
            }
            else if (QFile::exists(sEntLogoImagePath)) {
                QFile::remove(sEntLogoImagePath);
                QFile::remove(sEntLogoImagePathTemp);
//                if (m_receiver!=NULL)
//                    QApplication::postEvent(m_receiver, new QEvent((QEvent::Type)EB_COMMAND_UPDATE_ENT_LOGO));
            }
        }
        else if (QFile::exists(sEntLogoImagePath)) {
            // 没有设置企业LOGO，但有企业LOGO，应该是其他系统，删除并通知事件更新界面
            QFile::remove(sEntLogoImagePath);
            QFile::remove(sEntLogoImagePathTemp);
//            if (m_receiver!=NULL)
//                QApplication::postEvent(m_receiver, new QEvent((QEvent::Type)EB_COMMAND_UPDATE_ENT_LOGO));
        }
    }
    else {
        const std::string sEnterprise( this->m_setting.GetEnterprise() );
        if ( sEnterprise.find("恩布")==std::string::npos ) {
            m_sProductName = QString::fromUtf8("恩布互联");
        }
        else {
            m_sProductName = sEnterprise.c_str();
        }

        if (QFile::exists(sEntLogoImagePath)) {
            QFile::remove(sEntLogoImagePath);   // ?
        }
    }

    if (m_receiver!=NULL) {
        const EB_Event * pEvent = (EB_Event*)e;
        QApplication::postEvent( m_receiver, new EB_Event(*pEvent) );
    }
}
void EbClientApp::onAppIdError(QEvent *e)
{
    m_isAppIdResponseOk = false;
    if (m_receiver!=NULL) {
        const EB_Event * pEvent = (EB_Event*)e;
        QApplication::postEvent( m_receiver, new EB_Event(*pEvent) );
    }
}

void EbClientApp::loadUserSetting()
{
    m_sendKeyType = (EB_SEND_KEY_TYPE)GetPrivateProfileIntABoost("default","send-type",(int)EB_SEND_KEY_ENTER,
                                                                 m_userSettingIniFile.toLocal8Bit().constData());

}

//bool EbClientApp::startEBUMClient(void)
//{

//    const CEBString sAddress = this->m_setting.GetServerAddress();
//    //entboost::GetAccountAddress(lpszAccount, sAddress);
//    //this->m_ebum.Stop();
//    this->m_ebum.EB_Init(sAddress.c_str());
//    //this->m_ebum.EB_SetMsgHwnd(this->GetSafeHwnd());
//    return this->m_ebum.EB_IsInited();
//}

bool EbClientApp::initApp(void)
{
    if (!EbClientAppBase::initApp()) {
        return false;
    }
    if (m_webWngineDownloadRequested==0) {
        m_webWngineDownloadRequested = new EbWebEngineDownloadRequested(this);
    }
    return true;
}

//bool EbClientApp::initApp(void)
//{
//    this->m_setting.load(this->getSettingFile().toLocal8Bit().constData());
//    /// default this->setMainColor(0, 162, 232);
//    this->setMainColor( this->m_setting.GetDefaultColor(),false );
//    this->m_sProductName = QString::fromStdString( this->m_setting.GetEnterprise() );

//    if (m_sqliteEbc.get()==NULL) {
//        m_sqliteEbc = CSqliteCdbc::create();
//        const QString sSqliteEbcFile = m_appBodbPath + "/ebcdatas";
//        if (!QFile::exists(sSqliteEbcFile)) {
//            QString title = theLocales.getLocalText("message-box.ebc-file-error.title","");
//            if (title.isEmpty())
//                title = m_sProductName;
//            const QString text = theLocales.getLocalText("message-box.ebc-file-error.text","安装目录损坏：<br>请重新安装，或联系公司客服！");
//            EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_WARNING,0,QMessageBox::Ok );
//            return false;
//        }
//        if (!m_sqliteEbc->open(sSqliteEbcFile.toStdString().c_str())) {
//            m_sqliteEbc.reset();
//            /// 当前安装目录文件损坏：<br>请重新安装或联系公司客服！
//            QString title = theLocales.getLocalText("message-box.ebc-file-error.title","");
//            if (title.isEmpty())
//                title = m_sProductName;
//            const QString text = theLocales.getLocalText("message-box.ebc-file-error.text","安装目录损坏：<br>请重新安装，或联系公司客服！");
//            EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_WARNING,0,QMessageBox::Ok );
//            return false;
//        }
//    }
//    /// 获取本地上次产品名称
//    int nCookie = 0;
//    m_sqliteEbc->select("SELECT value1 FROM sys_value_t WHERE name='product-name'", nCookie);
//    cgcValueInfo::pointer pRecord = m_sqliteEbc->first(nCookie);
//    if (pRecord.get()!=0) {
//        const tstring productName(pRecord->getVector()[0]->getStr());
//        m_sqliteEbc->reset(nCookie);
//        if ( !productName.empty() )
//            this->m_sProductName = QString::fromStdString( productName.string() );
//    }
//    else {
//        m_sqliteEbc->execute("INSERT INTO sys_value_t(name,value1,value2) VALUES('product-name','',0)");
//    }
//    /// 获取本地设置色调
//    m_sqliteEbc->select("SELECT value2 FROM sys_value_t WHERE name='main-color'", nCookie);
//    pRecord = m_sqliteEbc->first(nCookie);
//    if (pRecord.get()!=0) {
//        const unsigned int nMainColor = (unsigned int)pRecord->getVector()[0]->getBigIntValue();
//        m_sqliteEbc->reset(nCookie);
//        this->setMainColor(GetRValue(nMainColor),GetGValue(nMainColor),GetBValue(nMainColor),false);
//    }
//    else {
//        char sql[256];
//        const QColor color(this->m_setting.GetDefaultColor());
//        sprintf(sql, "INSERT INTO sys_value_t(name,value1,value2) VALUES('main-color','',%lld)",(mycp::bigint)RGB(color.red(),color.green(),color.blue()));
//        m_sqliteEbc->execute(sql);
//    }
//    m_appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
//    checkCreateDir(m_appDataLocation);
//    m_appDataTempLocation = m_appDataLocation + "/temp";
//    checkCreateDir(m_appDataTempLocation);
//    m_appDataCefCacheTemp = m_appDataLocation + "/cef_cache_temp";
//    checkCreateDir(m_appDataCefCacheTemp);
//    m_appDataImageTempLocation = m_appDataLocation + "/image_temp";
//    checkCreateDir(m_appDataImageTempLocation);

//    if (m_webWngineDownloadRequested==0) {
//        m_webWngineDownloadRequested = new EbWebEngineDownloadRequested(this);
//    }
//    return true;
//}
//void EbClientApp::exitApp(bool bResetEbumOnly)
//{
//    if (!bResetEbumOnly) {
//        this->m_ebum.EB_SetMsgReceiver(0);
//    }
//    this->m_ebum.EB_UnInit();
//    if (!bResetEbumOnly) {
//        m_sqliteEbc.reset();
//        m_sqliteUser.reset();
//    }
//    m_isAppIdResponseOk = false;
//}

bool EbClientApp::onLogonSuccess(void)
{
    /// 检查本地默认浏览器
    int nCookie = 0;
    m_sqliteEbc->select("SELECT value2 FROM sys_value_t WHERE name='default-browser-type'", nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteEbc->first(nCookie);
    if (pRecord.get()!=NULL) {
        this->setDefaultBrowserType((EB_BROWSER_TYPE)pRecord->getVector()[0]->getIntValue());
        m_sqliteEbc->reset(nCookie);
//        if (m_bIeException && this->GetDefaultBrowserType())
//            SetDefaultBrowserType(EB_BROWSER_TYPE_CEF);
    }
    else {
        char sql[256];
        sprintf(sql, "INSERT INTO sys_value_t(name,value2) VALUES('default-browser-type',%d)",(int)this->defaultBrowserType());
        m_sqliteEbc->execute(sql);
    }
    const mycp::tstring sLogonAccount = this->m_ebum.EB_GetLogonAccount();
    m_userMainPath = qApp->applicationDirPath() + "/users";
    checkCreateDir(m_userMainPath);
//    return m_userMainPath + _T("\\setting.ini");
    if (this->m_ebum.EB_IsLogonVisitor()) {
        m_userMainPath += "/visitor";
    }
    else {
        m_userMainPath = m_userMainPath + "/" + sLogonAccount.c_str();
    }
    if (!QFile::exists(m_userMainPath)) {
        QDir dir(m_userMainPath);
        dir.mkdir(m_userMainPath);
    }
    m_userSettingIniFile = m_userMainPath + "/" + sLogonAccount.c_str() + "/setting.ini";
    m_userImagePath = m_userMainPath + "/image";
    if (!QFile::exists(m_userImagePath)) {
        QDir dir(m_userImagePath);
        dir.mkdir(m_userImagePath);
    }
    m_userFilePath = m_userMainPath + "/file";
    if (!QFile::exists(m_userFilePath)) {
        QDir dir(m_userFilePath);
        dir.mkdir(m_userFilePath);
    }
    loadUserSetting();

    if (this->m_sqliteUser.get()==NULL) {
        QString sBoPath = QString("%1/%2").arg(m_userMainPath).arg(this->deployId());
        if (!QFile::exists(sBoPath)) {
            QDir dir(sBoPath);
            dir.mkdir(sBoPath);
        }
        sBoPath += "/bodb";
        const QString sDefaultUserBoFile = m_appBodbPath + "/userdatas";
        const QString sUserBoFile = sBoPath + "/userdatas";
        if (!QFile::exists(sBoPath)) {
            QDir dir(sBoPath);
            dir.mkdir(sBoPath);
            QFile::remove(sUserBoFile);
            QFile::copy(sDefaultUserBoFile,sUserBoFile);
        }
        else if (libEbc::GetFileSize(sUserBoFile)<=0) {
            QFile::remove(sUserBoFile);
            QFile::copy(sDefaultUserBoFile,sUserBoFile);
        }
        m_sqliteUser = CSqliteCdbc::create();
        if (!m_sqliteUser->open(sUserBoFile.toStdString().c_str())) {
            m_sqliteUser.reset();
            QString title = theLocales.getLocalText("message-box.ebc-file-error.title","");
            if (title.isEmpty())
                title = m_sProductName;
            const QString text = theLocales.getLocalText("message-box.ebc-file-error.text","安装目录损坏：<br>请重新安装，或联系公司客服！");
            EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_WARNING,0,QMessageBox::Ok );
            return false;
        }
    }

    if (this->isHideMainFrame()) {
        m_nDefaultUIStyleType = EB_UI_STYLE_TYPE_CHAT;	// *
        //m_nDefaultUIStyleType = EB_UI_STYLE_TYPE_OFFICE;	// *
    }
    else {
        /// 检查默认界面类型
        const std::string sUserSettingIniFile = m_userSettingIniFile.toLocal8Bit().toStdString();
        const int nDefaultUIStyleType = GetPrivateProfileIntABoost("default","uistyle-type",2,sUserSettingIniFile.c_str());
        if (nDefaultUIStyleType>=2) {
            /// first time
            m_nDefaultUIStyleType = (EB_UI_STYLE_TYPE)this->m_setting.GetDefaultUIStyleType();
            if (m_nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
                if (GetPrivateProfileIntABoost("default","main-w",UISTYLE_CHAT_DEFAULT_DLG_WIDTH,sUserSettingIniFile.c_str())>UISTYLE_CHAT_DEFAULT_DLG_WIDTH) {
                    WritePrivateProfileIntABoost("default","main-w",UISTYLE_CHAT_DEFAULT_DLG_WIDTH,sUserSettingIniFile.c_str());
                }
                if (GetPrivateProfileIntABoost("default","main-h",UISTYLE_CHAT_DEFAULT_DLG_HEIGHT,sUserSettingIniFile.c_str())>UISTYLE_CHAT_DEFAULT_DLG_HEIGHT) {
                    WritePrivateProfileIntABoost("default","main-h",UISTYLE_CHAT_DEFAULT_DLG_HEIGHT,sUserSettingIniFile.c_str());
                }
            }
        }
        else {
            m_nDefaultUIStyleType = (EB_UI_STYLE_TYPE)nDefaultUIStyleType;
        }
    }
    // ??? for test
    m_nDefaultUIStyleType = EB_UI_STYLE_TYPE_CHAT;

    return true;
}

//bool EbClientApp::setLocaleLanguage(const QString &file)
//{
//    const QByteArray byteArray = file.toLocal8Bit();
//    char sql[256];
//    sprintf(sql, "UPDATE sys_value_t SET value1='%s' WHERE name='locale-file' AND value1<>'%s'",
//            byteArray.constData(), byteArray.constData());
//    return m_sqliteEbc->execute(sql)==1?true:false;
//}

//QString EbClientApp::localeLanguage(const QString &defaultFile)
//{
//    QString result(defaultFile);
//    if (m_sqliteEbc.get()==0) {
//        return result;
//    }
//    int nCookie = 0;
//    m_sqliteEbc->select("SELECT value1 FROM sys_value_t WHERE name='locale-file'", nCookie);
//    cgcValueInfo::pointer pRecord = m_sqliteEbc->first(nCookie);
//    if (pRecord.get()!=0) {
//        result = QString::fromStdString(pRecord->getVector()[0]->getStrValue().string());
//        m_sqliteEbc->reset(nCookie);
//    }
//    else {
//        char sql[256];
//        sprintf(sql, "INSERT INTO sys_value_t(name,value1) VALUES('locale-file','%s')", defaultFile.toStdString().c_str());
//        m_sqliteEbc->execute(sql);
//    }
//    return result;
//}

void EbClientApp::setLocalVideoIndex(int index)
{
    char sql[256];
    sprintf(sql, "UPDATE sys_value_t SET value2=%d WHERE name='local-video-index' AND value2<>%d", index, index);
    m_sqliteEbc->execute(sql);
}

int EbClientApp::localVideoIndex(int defaultIndex) const
{
    int index = defaultIndex;
    int nCookie = 0;
    m_sqliteEbc->select("SELECT value2 FROM sys_value_t WHERE name='local-video-index'", nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteEbc->first(nCookie);
    if (pRecord.get()!=0) {
        index = pRecord->getVector()[0]->getIntValue();
        m_sqliteEbc->reset(nCookie);
    }
    else {
        char sql[256];
        sprintf(sql, "INSERT INTO sys_value_t(name,value1,value2) VALUES('local-video-index','',%d)", index);
        m_sqliteEbc->execute(sql);
    }
    return index;
}

//void EbClientApp::setMainColor(unsigned char r, unsigned char g, unsigned char b, bool bUpdateDatabase)
//{
//    m_mainColor.setRed(r);
//    m_mainColor.setGreen(g);
//    m_mainColor.setBlue(b);
//    updateColor(bUpdateDatabase);
//}
//void EbClientApp::setMainColor(const QColor& pColor, bool bUpdateDatabase)
//{
//    m_mainColor = pColor;
//    updateColor(bUpdateDatabase);
//}

void EbClientApp::updateStyleSheet()
{
    qApp->setStyleSheet(m_styleSheet);
}

QString EbClientApp::urlIconFilePath(const QUrl &url)
{
    const QString host = url.host();
    return QString("%1/%2.png").arg(m_appDataImageTempLocation).arg(host);
}

QString EbClientApp::subscribeFuncUrl(mycp::bigint subId, const std::string &sParameters)
{
    mycp::bigint m_nSelectUserId = 0;
    tstring m_sSelectAccount;

    const tstring sFuncUrl = m_ebum.EB_GetSubscribeFuncUrl(
                subId,m_nSelectCallId,m_nSelectUserId,m_sSelectAccount,m_nSelectGroupId );
    if ( sFuncUrl.empty() ) {
        return "";
    }

    char buffer[1024];
    sprintf( buffer, "%s&color=%02x%02x%02x", sFuncUrl.c_str(), m_mainColor.red(), m_mainColor.green(), m_mainColor.blue() );
    QString sFullFuncurl(buffer);
    if ( !sParameters.empty() ) {
        sFullFuncurl.append( "&" );
        sFullFuncurl.append( sParameters.c_str() );
    }
    return sFullFuncurl;
}

QString EbClientApp::lineStateText(EB_USER_LINE_STATE lineState) const
{
    switch (lineState) {
    case EB_LINE_STATE_BUSY:
        return theLocales.getLocalText("line-state.busy.text","Busy");
    case EB_LINE_STATE_AWAY:
        return theLocales.getLocalText("line-state.away.text","Away");
    case EB_LINE_STATE_ONLINE_NEW:
        return theLocales.getLocalText("line-state.online.text","Online");
    default:
        return theLocales.getLocalText("line-state.offline.text","Offline");
    }
}
QString EbClientApp::lineStateText(void) const
{
    return lineStateText( m_ebum.EB_GetLineState() );
}

//void EbClientApp::updateColor(bool bUpdateDatabase)
//{
//    const int r = m_mainColor.red();
//    const int g = m_mainColor.green();
//    const int b = m_mainColor.blue();
//    if (bUpdateDatabase) {
//        char sql[256];
//        sprintf(sql, "UPDATE sys_value_t SET value2=%lld WHERE name='main-color'",(mycp::bigint)RGB(r,g,b));
//        m_sqliteEbc->execute(sql);
//    }
////    float h,s,l;
////    ::RGBtoHSL(r, g, b,&h,&s,&l);
////    // 按钮 hot,pre 二个状态
////    const unsigned int m_hotColorRgb = ::HSLtoRGB(h, s, std::min(theDefaultMaxBtnHot,(l+theDefaultBtnHot)));
////    const unsigned int m_preColorRgb = ::HSLtoRGB(h, s, std::min(theDefaultMaxBtnPre,(l+theDefaultBtnPre)));
////    m_hotColor.setRed(GetRValue(m_hotColorRgb));
////    m_hotColor.setGreen(GetGValue(m_hotColorRgb));
////    m_hotColor.setBlue(GetBValue(m_hotColorRgb));
////    m_preColor.setRed(GetRValue(m_preColorRgb));
////    m_preColor.setGreen(GetGValue(m_preColorRgb));
////    m_preColor.setBlue(GetBValue(m_preColorRgb));

//    const QString sQssFile = qApp->applicationDirPath()+"/entboost.qss";
//    QFile qssFile(sQssFile);
//    if (qssFile.open(QFile::ReadOnly)) {
//        m_styleSheet = QString::fromUtf8(qssFile.readAll());   // ** ok
//        qssFile.close();
//        char lpszBuffer[64];
//        sprintf( lpszBuffer,"%d,%d,%d",r,g,b );
//        m_styleSheet.replace( "[EB_MAIN_RGB]", lpszBuffer );
////        sprintf( lpszBuffer,"%d,%d,%d",m_hotColor.red(),m_hotColor.green(),m_hotColor.blue() );
////        qss.replace( "[EB_HOT_RGB]", lpszBuffer );
////        sprintf( lpszBuffer,"%d,%d,%d",m_preColor.red(),m_preColor.green(),m_preColor.blue() );
////        qss.replace( "[EB_PRE_RGB]", lpszBuffer );
//        qApp->setStyleSheet(m_styleSheet);
//    }
//}

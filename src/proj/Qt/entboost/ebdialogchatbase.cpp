#include "ebdialogchatbase.h"
#include "ui_ebdialogchatbase.h"
#include "ebdialogframelist.h"
#include "ebiconhelper.h"
//#include "ebwidgetchatmessage.h"
#include "ebdialogselectuser.h"
#include "ebwidgetchatinput.h"
#include "EbTextBrowser.h"
#include "ebwidgetchatright.h"
#include "ebtextblockuserdata.h"
#include "ebmessagebox.h"
#include <QFileDialog>

EbDialogChatBase::EbDialogChatBase(const EbcCallInfo::pointer& pCallInfo,QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogChatBase)
  , m_splitterMain(0), m_splitterInput(0)
  , m_dialogSelectUser(0)
  , m_widgetChatRight(0)
  , m_callInfo(pCallInfo)
  , m_bOwnerCall(false)
  , m_nFromLineState(EB_LINE_STATE_UNKNOWN)
  , m_nGroupType(EB_GROUP_TYPE_DEPARTMENT)
  , m_menuChatApps(0)
{
    assert(m_callInfo.get()!=0);
    ui->setupUi(this);
    updateFromInfo();
    this->setObjectName("DialogChatBase");

    /// 去掉鼠标拖动 & 去掉边框
    EbDialogBase::setMouseEasyMove(false);
    EbDialogBase::setDrawBorder(false);
    /// 设置过滤 ESC/Enter 按键
    EbDialogBase::setFilterEscapeKey(true);
    EbDialogBase::setFilterEnterKey(true);

//    // 设置初始大小
//    const int const_frame_list_width = theLocales.getLocalInt("frame-list.window-size.width", 1180);
//    const int const_frame_list_height = theLocales.getLocalInt("frame-list.window-size.height", 668);
//    const QSize const_frame_list_size(const_frame_list_width,const_frame_list_height);
//    this->resize(const_frame_list_size);
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );

    /// 显示 名称和描述；
    ui->labelName->setObjectName("DialogChatName");
    int x = 5;
    int y = 5;
    ui->labelName->setGeometry( x,y,width()-80,22 );
    ui->labelDescription->setObjectName("DialogChatDescription");
    y += 22;
    ui->labelDescription->setGeometry( x,y,width()-x-1,22);
    /// 显示 “邀请好友/发送文件/...” 按钮
    EbIconHelper::Instance()->SetIcon(ui->pushButtonAddUser, QChar(0xf234), 14);
    EbIconHelper::Instance()->SetIcon(ui->pushButtonSendFile, QChar(0xf07c), 14);
    EbIconHelper::Instance()->SetIcon(ui->pushButtonVideoChat, QChar(0xf03d), 14);
    EbIconHelper::Instance()->SetIcon(ui->pushButtonGroupShare, QChar(0xf0c2), 14);
    EbIconHelper::Instance()->SetIcon(ui->pushButtonChatApps, QChar(0xf009), 14);
    EbIconHelper::Instance()->SetIcon(ui->pushButtonExitChat, QChar(0xf08b), 14);   // f05c
    ui->pushButtonAddUser->setObjectName("DialogChatButton");
    ui->pushButtonSendFile->setObjectName("DialogChatButton");
    ui->pushButtonVideoChat->setObjectName("DialogChatButton");
    ui->pushButtonGroupShare->setObjectName("DialogChatButton");
    ui->pushButtonChatApps->setObjectName("DialogChatButton");
    ui->pushButtonExitChat->setObjectName("DialogChatButton");
    x = 5;
    y += 22;
    const QSize const_chat_base_button_size(36,26);
    bool showAddUserButton = this->isGroupChat()?false:true;
    if (this->isGroupChat()) {
        switch (m_nGroupType) {
        case EB_GROUP_TYPE_GROUP:
        case EB_GROUP_TYPE_TEMP:
            showAddUserButton = true;
            break;
        default:
            break;
        }
    }
    if (showAddUserButton) {
        ui->pushButtonAddUser->setGeometry( x,y,const_chat_base_button_size.width(),const_chat_base_button_size.height() );
        connect( ui->pushButtonAddUser,SIGNAL(clicked()),this,SLOT(onClickedButtonAddUser()) );
        x += const_chat_base_button_size.width();
    }
    else {
        ui->pushButtonAddUser->setVisible(false);
    }
    ui->pushButtonSendFile->setGeometry( x,y,const_chat_base_button_size.width(),const_chat_base_button_size.height() );
    connect( ui->pushButtonSendFile,SIGNAL(clicked()),this,SLOT(onClickedButtonSendFile()) );
#ifdef _EB_USES_VIDEO_ROOM
    x += const_chat_base_button_size.width();
    ui->pushButtonVideoChat->setGeometry( x,y,const_chat_base_button_size.width(),const_chat_base_button_size.height() );
    connect( ui->pushButtonVideoChat,SIGNAL(clicked()),this,SLOT(onClickedButtonVideoChat()) );
#else
    ui->pushButtonVideoChat->hide();
#endif

#ifdef _EB_USES_REMOTE_DESKTOP
    ///
#else
    ui->pushButtonGroupShare->hide();
#endif
    x += const_chat_base_button_size.width();
    ui->pushButtonChatApps->setGeometry( x,y,const_chat_base_button_size.width(),const_chat_base_button_size.height() );
    connect( ui->pushButtonChatApps,SIGNAL(clicked()),this,SLOT(onClickedButtonChatApps()) );
    switch (m_nGroupType)
    {
    case EB_GROUP_TYPE_GROUP:
    case EB_GROUP_TYPE_TEMP: {
        x += const_chat_base_button_size.width();
        ui->pushButtonExitChat->setGeometry( x,y,const_chat_base_button_size.width(),const_chat_base_button_size.height() );
        connect( ui->pushButtonExitChat,SIGNAL(clicked()),this,SLOT(onClickedButtonExitChat()) );
        break;
    }
    default:
        ui->pushButtonExitChat->setVisible(false);
        break;
    }

    /// 上面边框线
    ui->widgetLine->setObjectName("LineWidget");

    /// 聊天记录
    m_textBrowserMessage = EbTextBrowser::create(m_callInfo,this);
    m_textBrowserMessage->loadHistoryMsg(20);
    connect( m_textBrowserMessage,SIGNAL(openSubId(eb::bigint)),this,SLOT(onOpenSubId(eb::bigint)) );
    m_widgetChatInput = new EbWidgetChatInput(m_callInfo,this);
    connect( m_widgetChatInput,SIGNAL(clickedClose()),this,SLOT(onClickedInputClose()) );
    connect( m_widgetChatInput,SIGNAL(clickedMsgRecord()),this,SLOT(onClickedInputMsgRecord()) );
    m_splitterInput = new QSplitter(Qt::Vertical, this);
    m_splitterInput->setHandleWidth(1);
    m_splitterInput->addWidget(m_textBrowserMessage);
    m_splitterInput->addWidget(m_widgetChatInput);
    m_splitterInput->setStretchFactor(0,4);
    m_splitterInput->setStretchFactor(1,6);
    /// 聊天右边界面
    m_widgetChatRight = new EbWidgetChatRight(m_callInfo,this);
    m_widgetChatRight->setModal(false);
    m_splitterMain = new QSplitter(Qt::Horizontal, this);
    m_splitterMain->setHandleWidth(1);
    m_splitterMain->addWidget(m_splitterInput);
    m_splitterMain->addWidget(m_widgetChatRight);
    m_splitterMain->setStretchFactor(0,8);
    m_splitterMain->setStretchFactor(1,2);

    updateLocaleInfo();
}

EbDialogChatBase::~EbDialogChatBase()
{
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onExitChat(false);
    }
//    if (m_dialogSelectUser!=0) {
//        delete m_dialogSelectUser;
//        m_dialogSelectUser = 0;
//    }
    delete ui;
}

EbDialogChatBase::pointer EbDialogChatBase::create(const EbcCallInfo::pointer& pCallInfo,QWidget *parent)
{
    return EbDialogChatBase::pointer(new EbDialogChatBase(pCallInfo,parent));
}

void EbDialogChatBase::updateLocaleInfo()
{
    /// 显示右上角关闭按钮（这几个按钮还是要的，因为透明了也没用，点不到下面按钮）
    EbDialogBase::showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close"),"SysClose2Button" );
    EbDialogBase::showPushButtonSysMax( theLocales.getLocalText("base-dialog.maximize-button.tooltip","Maximize"),"SysTrans2Button" );
    EbDialogBase::showPushButtonSysMin( theLocales.getLocalText("base-dialog.minimize-button.tooltip","Minimize"),"SysTrans2Button" );

    ui->pushButtonAddUser->setToolTip( theLocales.getLocalText("chat-base-dialog.button-add-user.tooltip","add user") );
    ui->pushButtonSendFile->setToolTip( theLocales.getLocalText("chat-base-dialog.button-send-file.tooltip","send user") );
    if ( m_callInfo->isGroupCall() ) {
        ui->pushButtonVideoChat->setToolTip( theLocales.getLocalText("chat-base-dialog.button-group-video.tooltip","group video") );
    }
    else {
        ui->pushButtonVideoChat->setToolTip( theLocales.getLocalText("chat-base-dialog.button-video-chat.tooltip","video chat") );
    }
    ui->pushButtonGroupShare->setToolTip( theLocales.getLocalText("chat-base-dialog.button-group-share.tooltip","group share") );
    ui->pushButtonChatApps->setToolTip( theLocales.getLocalText("chat-base-dialog.button-chat-apps.tooltip","chat apps") );
    ui->pushButtonExitChat->setToolTip( theLocales.getLocalText("chat-base-dialog.button-exit-chat.tooltip","exit chat") );

}

void EbDialogChatBase::timerCheckState()
{
    if (m_dialogSelectUser!=0) {
        m_dialogSelectUser->timerCheckState();
    }
}

void EbDialogChatBase::setFocusInput()
{
    if (m_widgetChatInput!=0) {
        m_widgetChatInput->setFocusInput();
    }
}

void EbDialogChatBase::scrollToEnd()
{
    QScrollBar *scrollbar = m_textBrowserMessage->verticalScrollBar();
    scrollbar->setSliderPosition(scrollbar->maximum());
}

void EbDialogChatBase::setCallInfo(const EbcCallInfo::pointer &pCallInfo)
{
    m_callInfo = pCallInfo;
    assert(m_callInfo.get()!=0);
    if ( m_textBrowserMessage!=0 ) {
        m_textBrowserMessage->setCallInfo(m_callInfo);
    }
    if (m_widgetChatInput!=0) {
        m_widgetChatInput->setCallInfo(m_callInfo);
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->setCallInfo(m_callInfo);
    }
}

bool EbDialogChatBase::clearUnreadMsg(bool fromUserClick)
{
    if (m_callInfo.get()==0) {
        return false;
    }
    char sql[256];
    if (m_callInfo->isGroupCall()) {
        if (fromUserClick)
            theApp->m_pAutoCallGroupIdList.remove(m_callInfo->m_pCallInfo.m_sGroupCode);
        else if (theApp->m_pAutoCallGroupIdList.exist(m_callInfo->m_pCallInfo.m_sGroupCode)) return false;
        sprintf(sql,"UPDATE msg_record_t SET read_flag=read_flag|1 WHERE dep_code=%lld AND (read_flag&1)=0",m_callInfo->m_pCallInfo.m_sGroupCode);
    }
    else {
        if (fromUserClick)
            theApp->m_pAutoCallFromUserIdList.remove(m_callInfo->m_pFromAccountInfo.GetUserId());
        else if (theApp->m_pAutoCallFromUserIdList.exist(m_callInfo->m_pFromAccountInfo.GetUserId())) return false;
        sprintf(sql,"UPDATE msg_record_t SET read_flag=read_flag|1 WHERE from_uid=%lld AND dep_code=0 AND (read_flag&1)=0",m_callInfo->m_pFromAccountInfo.GetUserId());
    }
    theApp->m_sqliteUser->execute(sql);
    return true;
}

int EbDialogChatBase::getUnreadMsgCount() const
{
    // for test
    if (theApp->m_sqliteUser.get()==0) return 0;
    if (m_callInfo.get()==NULL) return 0;
    char sSql[256];
    if (m_callInfo->m_pCallInfo.m_sGroupCode>0) {
        sprintf(sSql,"SELECT count(read_flag) FROM msg_record_t WHERE dep_code=%lld AND (read_flag&1)=0",m_callInfo->m_pCallInfo.m_sGroupCode);
    }
    else {
        sprintf(sSql,"SELECT count(read_flag) FROM msg_record_t WHERE from_uid=%lld AND dep_code=0 AND (read_flag&1)=0",m_callInfo->m_pFromAccountInfo.GetUserId());
    }
    int nResult = 0;
    int nCookie = 0;
    theApp->m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = theApp->m_sqliteUser->first(nCookie);
    if (pRecord.get()!=NULL) {
        nResult = pRecord->getVector()[0]->getIntValue();
        theApp->m_sqliteUser->reset(nCookie);
    }
    return nResult;
}

void EbDialogChatBase::onUserExitRoom(eb::bigint nUserId, bool bExitDep)
{
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onUserExit(nUserId,bExitDep);
    }
    if ( this->isGroupChat() ) {
        ///
    }
    else {
        if (theApp->m_ebum.EB_IsLogonVisitor() ||
            m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_VISITOR) {	// 游客不能被动邀请
            /// 不是公司内部员工，需要重新呼叫才能通讯
            /// "对方已经退出本次会话！"
            m_textBrowserMessage->addLineString(0,theLocales.getLocalText("chat-msg-text.on-user-exit","User Exit") );
        }
        if (m_widgetChatInput!=0) {
            m_widgetChatInput->onUserExit(nUserId);
        }
        // ?
        //size_t nUserSize = m_callInfo->m_pChatRoom->GetUserSize();
        //if (nUserSize <= 1)
        //{
        //	m_btnSendFile.EnableWindow(FALSE);
        //}
    }
}

void EbDialogChatBase::onAlertingCall()
{
    /// "对方已经退出本次会话，正在重新邀请，请稍候...！"
    m_textBrowserMessage->addLineString(0,theLocales.getLocalText("chat-msg-text.alerting-call","Alerting Call") );
}

void EbDialogChatBase::onSendRich(const CCrRichInfo *pCrMsgInfo, EB_STATE_CODE nState)
{
    processMsg(false,pCrMsgInfo,nState);
}

void EbDialogChatBase::onReceiveRich(const CCrRichInfo *pCrMsgInfo, QString *sOutFirstMsg1, QString *sOutFirstMsg2)
{
    if (m_widgetChatInput!=0) {
        m_widgetChatInput->onReceiveRich();
    }
    processMsg(true,pCrMsgInfo,EB_STATE_OK,sOutFirstMsg1,sOutFirstMsg2);
//    if (this->isVisible()) {
//            ::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
    //    }
}

void EbDialogChatBase::onUserLineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE bLineState)
{
    if (groupId()==0 && fromUserId()==nUserId) {
        m_nFromLineState = bLineState;
    }
    else if (nGroupCode>0 && groupId()==nGroupCode) {
        /// 更新群组成员，放在 onMemberInfo 处理
//        if (m_widgetChatRight!=0) {
//            m_widgetChatRight->lineStateChange(nUserId, bLineState);
//        }
    }
}

void EbDialogChatBase::onMemberInfo(const EB_MemberInfo *memberInfo, bool bChangeLineState)
{
    if (m_dialogSelectUser!=0) {
        m_dialogSelectUser->onMemberInfo(memberInfo,bChangeLineState);
    }

    if ( m_widgetChatInput!=0 &&
         memberInfo->m_sGroupCode==this->m_callInfo->groupId() &&
         memberInfo->m_nMemberUserId==theApp->logonUserId() ) {

        QString sForbidMessage;
        m_widgetChatInput->checkMyForbidSpeechState(true,true,&sForbidMessage);
        if ( !sForbidMessage.isEmpty() ) {
            m_textBrowserMessage->addLineString(0,sForbidMessage);
        }
    }

    if ( m_widgetChatRight!=0) {
        m_widgetChatRight->onMemberInfo(memberInfo,bChangeLineState);
    }
}

bool EbDialogChatBase::onMemberHeadChange(const EB_MemberInfo *pMemberInfo)
{
    if (this->isGroupChat()) return false;
    if (m_callInfo->m_pFromAccountInfo.GetUserId()!=pMemberInfo->m_nMemberUserId) return false;
    if (m_imageMd5==pMemberInfo->m_sHeadMd5) return false;

    if ( QFileInfo::exists(pMemberInfo->m_sHeadResourceFile) ) {
        m_imageMd5 = pMemberInfo->m_sHeadMd5;
        m_fromImage = QImage( pMemberInfo->m_sHeadResourceFile );
        return true;
    }
    return false;
}

bool EbDialogChatBase::onContactHeadChange(const EB_ContactInfo *pContactInfo)
{
    if (this->isGroupChat()) return false;
    if (m_callInfo->m_pFromAccountInfo.GetUserId()!=pContactInfo->m_nContactUserId) return false;
    if (m_imageMd5==pContactInfo->m_sHeadMd5) return false;

    if ( QFileInfo::exists(pContactInfo->m_sHeadResourceFile) ) {
        m_imageMd5 = pContactInfo->m_sHeadMd5;
        m_fromImage = QImage( pContactInfo->m_sHeadResourceFile );
        return true;
    }
    return false;
}

void EbDialogChatBase::onGroupInfo(const EB_GroupInfo *groupInfo)
{
    if (m_dialogSelectUser!=0) {
        m_dialogSelectUser->onGroupInfo(groupInfo);
    }
    if (m_callInfo->groupId()!=groupInfo->m_sGroupCode) {
        return;
    }

    if (m_widgetChatInput!=0) {
        QString sForbidMessage;
        m_widgetChatInput->checkMyForbidSpeechState(true,false,&sForbidMessage);
        if ( !sForbidMessage.isEmpty() ) {
            m_textBrowserMessage->addLineString(0,sForbidMessage);
        }
//        m_widgetChatInput->onGroupInfo(groupInfo);
    }

    updateFromInfo();
//    m_sFromName = pGroupInfo->m_sGroupName.c_str();
//    m_sFromDescription = pGroupInfo->m_sDescription.c_str();
//    m_nGroupType = pGroupInfo->m_nGroupType; // ?
//    if (m_sFromName.isEmpty()) {
//        char lpszBuffer[24];
//        sprintf( lpszBuffer,"%lld",m_callInfo->groupId() );
//        m_sFromName = lpszBuffer;
//    }
//    ui->labelName->setText(m_sFromName);
//    ui->labelDescription->setText(m_sFromDescription);
}

void EbDialogChatBase::onRemoveGroup(const EB_GroupInfo* groupInfo)
{
    if (m_dialogSelectUser!=0) {
        m_dialogSelectUser->onRemoveGroup(groupInfo);
    }
}

void EbDialogChatBase::onRemoveMember(const EB_GroupInfo* groupInfo, mycp::bigint nMemberId, mycp::bigint memberUserId)
{
    if (m_dialogSelectUser!=0) {
        m_dialogSelectUser->deleteMemberInfo(groupInfo,nMemberId,false);
    }
    if ( this->groupId()==groupInfo->m_sGroupCode ) {
        onUserExitRoom( memberUserId,true );
    }
}

void EbDialogChatBase::onMsgReceipt(const CCrRichInfo *pCrMsgInfo, int nAckType)
{
    if (m_textBrowserMessage!=0) {
        const eb::bigint nFromUserId = pCrMsgInfo->m_sSendFrom;
        const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();
        m_textBrowserMessage->updateMsgReceiptData(nMsgId, nFromUserId, nAckType, pCrMsgInfo->GetStateCode());
    }

    if ( m_widgetChatRight!=0 ) {
        m_widgetChatRight->onMsgReceipt(pCrMsgInfo, nAckType);
    }
}

void EbDialogChatBase::onSendingFile(const CCrFileInfo *pCrFileInfo)
{
    if ( pCrFileInfo->GetParam()==0 && pCrFileInfo->m_nMsgId>0 && pCrFileInfo->GetStateCode()==EB_STATE_OK ) {
        if ( theApp->isSaveConversationLocal()  && !theApp->isLogonVisitor() ) {
            //if (pCrFileInfo->m_sReceiveAccount==theApp->m_ebum.EB_GetLogonUserId())
            {
                tstring sInFromName;
                if (m_callInfo->m_pCallInfo.m_sGroupCode==0)
                    sInFromName = theApp->m_ebum.EB_GetUserName();
                else
                    theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),theApp->logonUserId(),sInFromName);
                theApp->m_sqliteUser->escape_string_in(sInFromName);
                tstring sInFileName(pCrFileInfo->m_sFileName.toStdString());
                theApp->m_sqliteUser->escape_string_in(sInFileName);
                const eb::bigint sSaveDbToAccount = m_callInfo->groupId()==0?m_callInfo->fromUserId():pCrFileInfo->m_sSendTo;
                char sSql[1024];
                sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_text) "
                               "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s')",
                    pCrFileInfo->m_nMsgId,m_callInfo->groupId(),pCrFileInfo->m_sSendFrom,sInFromName.c_str(),
                    sSaveDbToAccount,pCrFileInfo->m_bPrivate?1:0,MRT_FILE,sInFileName.c_str() );
                theApp->m_sqliteUser->execute(sSql);
            }
        }

        const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName.toStdString());
        /// 发送文件：%s
        const QString sWindowText = QString("%1: %2").arg(theLocales.getLocalText("chat-msg-text.send-file","Send File")).arg(sFileName.c_str());
        m_textBrowserMessage->addLineString(pCrFileInfo->m_nMsgId,sWindowText);
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onSendingFile(pCrFileInfo);
    }
}

bool EbDialogChatBase::onSentFile(const CCrFileInfo *fileInfo)
{
    const EB_STATE_CODE nState = fileInfo->GetStateCode();
    bool ret = false;
    if (fileInfo->GetParam()==0) {
        //    if (pCrFileInfo->GetParam()==0 && m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
        if ( theApp->isSaveConversationLocal() && !theApp->isLogonVisitor() ) {
            //if (pCrFileInfo->m_sReceiveAccount==theApp->m_ebum.EB_GetLogonUserId())
            {
                char sSql[1024];
                if (nState==EB_STATE_FILE_ALREADY_EXIST) {
                    /// ** 保存本地数据库
                    if ( m_callInfo->isGroupCall() ) {
                        char sMsgText[64];
                        sprintf( sMsgText, "%lld,%lld",fileInfo->m_sResId,fileInfo->m_nFileSize);
                        tstring sInMemberName;
                        theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),fileInfo->m_sSendFrom,sInMemberName);
                        theApp->m_sqliteUser->escape_string_in(sInMemberName);
                        const tstring sFileName = libEbc::GetFileName(fileInfo->m_sFileName.toStdString());
                        tstring sInFileName(sFileName);
                        theApp->m_sqliteUser->escape_string_in(sInFileName);
                        sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_name,msg_text,read_flag) "
                                       "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s','%s',%d)",
                                 fileInfo->m_nMsgId,m_callInfo->groupId(),fileInfo->m_sSendFrom,sInMemberName.c_str(),
                                 fileInfo->m_sSendTo,fileInfo->m_bPrivate?1:0,MRT_RESOURCE,sInFileName.c_str(),sMsgText,(int)EBC_READ_FLAG_SENT);
                    }
                    else {
                        tstring sInFromName(theApp->m_ebum.EB_GetUserName());
                        theApp->m_sqliteUser->escape_string_in(sInFromName);
                        tstring sInFileName(fileInfo->m_sFileName.toStdString());
                        theApp->m_sqliteUser->escape_string_in(sInFileName);
                        const eb::bigint sSaveDbToAccount = m_callInfo->fromUserId();
                        //const eb::bigint sSaveDbToAccount = m_callInfo->m_pCallInfo.m_sGroupCode==0?m_callInfo->m_pFromAccountInfo.GetUserId():pCrFileInfo->m_sSendTo;
                        sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_text) "
                                       "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s')",
                                 fileInfo->m_nMsgId,m_callInfo->groupId(),fileInfo->m_sSendFrom,sInFromName.c_str(),
                                 sSaveDbToAccount,fileInfo->m_bPrivate?1:0,MRT_FILE,sInFileName.c_str());
                    }
                }
                else {
                    int nReadFlag = EBC_READ_FLAG_SENT;
                    if (!fileInfo->m_bOffFile) {
                        nReadFlag |= EBC_READ_FLAG_RECEIPT;
                    }
                    sprintf( sSql, "UPDATE msg_record_t SET read_flag=read_flag|%d WHERE msg_id=%lld AND (read_flag&%d)=0",nReadFlag,fileInfo->m_nMsgId,nReadFlag);
                }
                theApp->m_sqliteUser->execute(sSql);
            }
        }
        ret = processFile(false, fileInfo);
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->deleteTranFile(fileInfo->m_nMsgId);
    }
//    if (this->GetParent()->IsWindowVisible())
//        ::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
    return ret;
}

void EbDialogChatBase::onCancelFile(const CCrFileInfo *fileInfo, bool bChangeP2PSending)
{
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->deleteTranFile(fileInfo->m_nMsgId);
    }
    if (fileInfo->GetParam()==0) {
//    if (m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd() && pCrFileInfo->GetParam()==0)//m_callInfo->m_pCallInfo.m_sGroupCode==0)
        if (!bChangeP2PSending) {
            char sSql[128];
            sprintf( sSql, "DELETE FROM msg_record_t WHERE msg_id=%lld",fileInfo->m_nMsgId);
            theApp->m_sqliteUser->execute(sSql);
        }
        const tstring sFileName = libEbc::GetFileName(fileInfo->m_sFileName.toStdString());
        QString sWindowText;
        if (bChangeP2PSending) {
            /// 对方在线，使用P2P点对点发送:
            sWindowText = QString("%1: %2").arg(theLocales.getLocalText("chat-msg-text.change-p2p-sending","Change P2P Sending")).arg(sFileName.c_str());
        }
        else if ( fileInfo->m_sSendFrom!=theApp->logonUserId() || fileInfo->m_sReceiveAccount==0
                 || fileInfo->m_sReceiveAccount==theApp->logonUserId() ) {
            /// 取消文件:
            sWindowText = QString("%1: %2").arg(theLocales.getLocalText("chat-msg-text.cancel-file","Cancel File")).arg(sFileName.c_str());
        }
        else if (m_callInfo->groupId()==0) {
            /// 对方拒绝接收文件：
            sWindowText = QString("%1: %2").arg(theLocales.getLocalText("chat-msg-text.dest-reject-file","Dest Reject File")).arg(sFileName.c_str());
        }
        else {
            /// %s 拒绝接收文件：%s
            CEBString sMemberName;
            theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),fileInfo->m_sReceiveAccount,sMemberName);
            sWindowText = QString("%1: %2 %3").arg(sMemberName.c_str()).arg(theLocales.getLocalText("chat-msg-text.reject-file","Reject File")).arg(sFileName.c_str());
        }
        m_textBrowserMessage->addLineString(fileInfo->m_nMsgId,sWindowText);
    }
}

void EbDialogChatBase::onReceivingFile(const CCrFileInfo *fileInfo, QString *sOutFirstMsg)
{
    const bool bOffLineUser = m_callInfo->m_bOffLineUser;
    const eb::bigint sSendFrom = fileInfo->m_sSendFrom;
    const tstring sFileName = libEbc::GetFileName(fileInfo->m_sFileName.toStdString());
    QString sWindowText;
    if (this->m_callInfo->isGroupCall() && fileInfo->m_sResId>0 &&
            (fileInfo->m_sSendFrom>0 || fileInfo->m_sSendTo>0)) {
        /// fileInfo->m_sSendFrom==0 && fileInfo->m_sSendTo==0 是下载资源文件，不处理
        m_textBrowserMessage->onReceivingFile(m_callInfo->m_bOffLineUser, fileInfo);
//        m_textBrowserMessage->addFileMsg( true,fileInfo );

        CEBString sMemberName;
        theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),sSendFrom,sMemberName);
        /// %1 上传群共享文件：%s // send-group-file
        sWindowText = QString("%1 %2 :\n%3").arg(sMemberName.c_str())
                .arg(theLocales.getLocalText("chat-msg-text.send-group-file","Send Group File")).arg(sFileName.c_str());

        /// ** 保存本地数据库
        if ( theApp->isSaveConversationLocal() && !theApp->isLogonVisitor() ) {
            char sMsgText[64];
            sprintf( sMsgText, "%lld,%lld",fileInfo->m_sResId,fileInfo->m_nFileSize);
            tstring sInMemberName(sMemberName);
            theApp->m_sqliteUser->escape_string_in(sInMemberName);
            tstring sInFileName(sFileName);
            theApp->m_sqliteUser->escape_string_in(sInFileName);
            char sSql[1024];
            sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_name,msg_text) "
                           "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s','%s')",
                fileInfo->m_nMsgId,m_callInfo->groupId(),fileInfo->m_sSendFrom,sInMemberName.c_str(),
                fileInfo->m_sSendTo,fileInfo->m_bPrivate?1:0,MRT_RESOURCE,sInFileName.c_str(),sMsgText);
            theApp->m_sqliteUser->execute(sSql);
        }
    }
    else if (bOffLineUser || fileInfo->m_bOffFile) {
        //if (m_pCallInfo->m_bOffLineCall)
        //if (m_bReceiveOffLineMsg)
        /// 对方发送离线文件：%s
        sWindowText = QString("%1: %2").arg(theLocales.getLocalText("chat-msg-text.dest-send-off-file","Dest Send Off File")).arg(sFileName.c_str());
        m_textBrowserMessage->addLineString(fileInfo->m_nMsgId,sWindowText);
    }
    else if (!m_callInfo->isGroupCall() && (fileInfo->m_sSendFrom>0 || fileInfo->m_sSendTo>0)) {
        /// 对方发送文件：%s
        sWindowText = QString("%1: %2").arg(theLocales.getLocalText("chat-msg-text.dest-send-file","Dest Send File")).arg(sFileName.c_str());
        m_textBrowserMessage->addLineString(fileInfo->m_nMsgId,sWindowText);
    }
    if (sOutFirstMsg!=0) {
        *sOutFirstMsg = sWindowText;
//        if ( m_callInfo->isGroupCall() )
//            sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-group://%lld\">%s</a>"),libEbc::ACP2UTF8(sWindowText).c_str(),m_pCallInfo.m_sGroupCode,libEbc::ACP2UTF8("接收文件").c_str());
//        else
//            sOutFirstMsg->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-account://%lld\">%s</a>"),libEbc::ACP2UTF8(sWindowText).c_str(),m_pCallInfo.GetFromUserId(),libEbc::ACP2UTF8("接收文件").c_str());
    }

    if ( m_widgetChatRight!=0 ) {
        if ( m_callInfo->isGroupCall() && fileInfo->m_sResId>0 &&
                fileInfo->m_sSendFrom>0 && fileInfo->m_sSendFrom!=theApp->logonUserId() ) {
            /// ** 对方发送群共享文件，在线消息不处理
            return;
        }
        m_widgetChatRight->onReceivingFile(fileInfo);
//        if (this->IsWindowVisible())
//            ::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
    }

}

void EbDialogChatBase::onReceivedFile(const CCrFileInfo *fileInfo)
{
    m_textBrowserMessage->addFileMsg( true,fileInfo );
    if ( fileInfo->GetParam()==0 && theApp->isSaveConversationLocal() && !theApp->isLogonVisitor() ) {
        const eb::bigint sSaveDbToAccount = fileInfo->m_sSendTo;
        tstring fromName;
        if ( !m_callInfo->isGroupCall() ) {
            fromName = m_sFromName.toStdString();
        }
        else {
            theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),fileInfo->m_sSendFrom,fromName);
        }
        theApp->m_sqliteUser->escape_string_in(fromName);
        tstring sInFileName(fileInfo->m_sFileName.toStdString());
        theApp->m_sqliteUser->escape_string_in(sInFileName);
        char sSql[1024];
        sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_text) "
                       "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s')",
                    fileInfo->m_nMsgId,m_callInfo->groupId(),fileInfo->m_sSendFrom,fromName.c_str(),
                    sSaveDbToAccount,fileInfo->m_bPrivate?1:0,MRT_FILE,sInFileName.c_str());
        theApp->m_sqliteUser->execute(sSql);
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onReceivedFile(fileInfo);
    }
//    ::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}

void EbDialogChatBase::onFilePercent(const CChatRoomFilePercent *pChatRoomFilePercent)
{
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onFilePercent(pChatRoomFilePercent);
    }
}

void EbDialogChatBase::onSave2Cloud(const CCrFileInfo *fileInfo)
{
    const EB_STATE_CODE nState = (EB_STATE_CODE)fileInfo->GetStateCode();
    const tstring sFileName = libEbc::GetFileName(fileInfo->m_sFileName.toStdString());
    QString sWindowText;
    if (nState==EB_STATE_OK) {
        /// 成功存入个人云盘！
        sWindowText = QString("%1: %2").arg(sFileName.c_str()).arg(theLocales.getLocalText("chat-msg-text.save2-cloud-ok","Save To Cloud Ok"));
    }
    else {
        /// 《%s》存入个人云盘失败（%d）！
        sWindowText = QString("%1: %2").arg(sFileName.c_str()).arg(theLocales.getLocalText("chat-msg-text.save2-cloud-error","Save To Cloud Error"));
        sWindowText.replace( "[STATE_CODE]", QString::number((int)nState) );
    }
    m_textBrowserMessage->addLineString(fileInfo->m_nMsgId, sWindowText);
    if (m_widgetChatRight!=0) {
        if (nState==0)
            m_widgetChatRight->deleteTranFile(fileInfo->m_nMsgId);
    }
}

void EbDialogChatBase::onVRequestResponse(const EB_VideoInfo *pVideoInfo, int nStateValue)
{
    if (m_callInfo->isGroupCall()) {
        /// 打开本地视频加入视频会议...
        m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.group-video-reqeust-ok","Join videos..."));
    }
    else {
        /// 邀请对方视频通话...
        m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.video-reqeust-ok","Open video..."));
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onVRequestResponse(pVideoInfo, nStateValue);
    }
}

void EbDialogChatBase::onVAckResponse(const EB_VideoInfo *pVideoInfo, int nStateValue)
{
    if (!m_callInfo->isGroupCall()) {
        /// 接受对方视频通话，视频通话中...
        if (nStateValue==EB_STATE_OK)
            m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.video-ack-ok","Video chat..."));
        else {
//            return;
        }
    }
    else if (nStateValue==EB_STATE_OK) {
        bool pVideoProcessing = false;
        bool pFileProcessing = false;
        bool pDesktopProcessing = false;
        if (m_widgetChatRight!=0) {
            m_widgetChatRight->getProcessing(pVideoProcessing, pFileProcessing, pDesktopProcessing);
        }
        if (!pVideoProcessing) {
            /// 成功申请视频会议，可以“打开视频”加入视频会议，然后“退出会议”！
            m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.group-video-ack-ok","Group video chat..."));
        }
    }
    else {
        /// 申请视频会议失败！
        m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.group-video-ack-error","Group video error"));
        return;
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onVAckResponse(pVideoInfo, nStateValue);
    }
}

void EbDialogChatBase::onVideoRequest(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    if (m_callInfo->isGroupCall()) {
        /// [MEMBER_NAME] 打开本地视频加入视频会议...
        tstring sMemberName;
        theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(), pUserVideoInfo->m_sUserAccount, sMemberName);
        QString text = theLocales.getLocalText("chat-msg-text.member-video-reqeust", "Member open video...");
        text.replace("[MEMBER_NAME]", sMemberName.c_str());
        m_textBrowserMessage->addLineString(0, text);
    }
    else {
        /// 对方邀请视频通话...
        m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.dest-video-reqeust","Dest request video..."));
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onVideoRequest(pVideoInfo, pUserVideoInfo);
    }
    /// ??
    this->activateWindow();
//    if (this->IsWindowVisible())
//        ::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}

void EbDialogChatBase::onVideoAccept(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    if (m_callInfo->isGroupCall()) {
        /// [MEMBER_NAME] 打开我的视频会议视频...
        tstring sMemberName;
        theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(), pUserVideoInfo->m_sUserAccount, sMemberName);
        QString text = theLocales.getLocalText("chat-msg-text.member-video-accept", "Member open accept video...");
        text.replace("[MEMBER_NAME]", sMemberName.c_str());
        m_textBrowserMessage->addLineString(0, text);
    }
    else {
        /// 对方接受视频通话，视频通话中...
        m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.dest-video-accept","Dest accept video..."));
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onVideoAccept(pVideoInfo, pUserVideoInfo);
    }
    /// ??
    this->activateWindow();
//    if (this->IsWindowVisible())
//        ::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}

void EbDialogChatBase::onVideoCancel(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    if (m_callInfo->isGroupCall()) {
        /// [MEMBER_NAME] 退出视频会议
        tstring sMemberName;
        theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(), pUserVideoInfo->m_sUserAccount, sMemberName);
        QString text = theLocales.getLocalText("chat-msg-text.member-video-exit", "Member exit video");
        text.replace("[MEMBER_NAME]", sMemberName.c_str());
        m_textBrowserMessage->addLineString(0, text);
    }
    else {
        /// 对方拒绝视频通话
        m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.dest-video-reject","Dest reject video"));
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onVideoCancel(pVideoInfo, pUserVideoInfo);
    }
    /// ??
    this->activateWindow();
//    if (this->IsWindowVisible())
//        ::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}

void EbDialogChatBase::onVideoTimeout(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    if (!m_callInfo->isGroupCall()) {
        /// 请求视频通话超时
        m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.video-request-timeout","Video request timeout"));
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onVideoCancel(pVideoInfo, pUserVideoInfo);
    }
    /// ??
    this->activateWindow();
//    if (this->IsWindowVisible())
//        ::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}

void EbDialogChatBase::onVideoEnd(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    if (!m_callInfo->isGroupCall()) {
        /// 视频通话结束
        m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.video-end","Video chat end"));
    }
    else if (theApp->logonUserId()==pUserVideoInfo->m_sUserAccount) {
        /// 关闭本地视频会议视频
        m_textBrowserMessage->addLineString(0, theLocales.getLocalText("chat-msg-text.local-video-close","Local video close"));
    }
    else {
        /// [MEMBER_NAME] 关闭视频会议视频
        tstring sMemberName;
        theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(), pUserVideoInfo->m_sUserAccount, sMemberName);
        QString text = theLocales.getLocalText("chat-msg-text.member-video-close", "Member video close");
        text.replace("[MEMBER_NAME]", sMemberName.c_str());
        m_textBrowserMessage->addLineString(0, text);
    }
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onVideoEnd(pVideoInfo, pUserVideoInfo);
    }
    /// ??
    this->activateWindow();
//    if (this->IsWindowVisible())
//        ::FlashWindow(this->GetParent()->GetSafeHwnd(), TRUE);
}

void EbDialogChatBase::onClickedInputClose()
{
    if (!requestClose(false)) {
        /// 检查不能退出会话，直接返回
        return;
    }
    emit clickedClose();
}

void EbDialogChatBase::onClickedInputMsgRecord()
{
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->showMsgRecord();
    }
}

void EbDialogChatBase::onClickedButtonAddUser()
{
    std::vector<tstring> pExistUserList;
    if ( m_callInfo->isGroupCall() ) {
        if ( m_nGroupType==EB_GROUP_TYPE_DEPARTMENT || m_nGroupType==EB_GROUP_TYPE_PROJECT ) {
            /// 部门及项目组，不能随意添加成员
            return;
        }
        if ( m_nGroupType!=EB_GROUP_TYPE_TEMP && !theApp->m_ebum.EB_IsGroupAdminLevel(m_callInfo->groupId()) ) {
            /// 没有管理员权限：\t\n不能邀请好友！
            const QString text = theLocales.getLocalText("message-show.add-user-not-auth","Add user not auth");
            EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING, default_warning_auto_close );
            return;
        }
        theApp->m_ebum.EB_GetGroupMemberAccountList(m_callInfo->groupId(),pExistUserList);
    }
    else {
        theApp->m_ebum.EB_GetCallAccountList(m_callInfo->callId(),pExistUserList);
    }

    createDialogSelectUser();
    m_dialogSelectUser->setSingleSelect(false);
    m_dialogSelectUser->setSelectedGroupId(-1);
    for (size_t i=0; i<pExistUserList.size(); i++) {
        m_dialogSelectUser->m_existedUser.insert(pExistUserList[i],true,false);
    }

    if (m_dialogSelectUser->exec()==QDialog::Accepted) {
        AUTO_RLOCK(m_dialogSelectUser->m_selectedList);
        CLockMap<tstring,EbWidgetItemInfo::pointer>::iterator iter = m_dialogSelectUser->m_selectedList.begin();
        for (; iter!=m_dialogSelectUser->m_selectedList.end(); iter++) {
            const EbWidgetItemInfo::pointer & itemInfo = iter->second;
            const tstring& sSelAccount = itemInfo->m_sAccount;
            const eb::bigint nToUserId = itemInfo->m_nUserId;
            /// 判断是否已经存在该会话
            if (m_dialogSelectUser->m_existedUser.exist(sSelAccount)) {
                continue;
            }
//            bool bExistAccount = false;
//            for (size_t i=0; i<pExistUserList.size(); i++) {
//                if (pExistUserList[i] == sSelAccount)
//                {
//                    bExistAccount = true;
//                    break;
//                }
//            }
//            if (bExistAccount) {
//                continue;
//            }
            if (!m_callInfo->isGroupCall()) {
                theApp->m_ebum.EB_Call2Group( m_callInfo->callId(), sSelAccount.c_str());
            }
            else if (nToUserId>0) {
                theApp->m_ebum.EB_CallUserId( nToUserId, m_callInfo->callId() );
            }
            else {
                theApp->m_ebum.EB_CallAccount( sSelAccount.c_str(), m_callInfo->callId() );
            }
        }
    }
    m_dialogSelectUser->clearSelected();

//    const QRect & rect = m_dialogSelectUser->geometry();
//    m_dialogSelectUser->move(pt.x()-rect.width()/3,pt.y()-rect.height());
//    m_dialogSelectUser->setFocus();
//    m_dialogSelectUser->show();

}

void EbDialogChatBase::onClickedButtonSendFile()
{
    if (m_callInfo->isGroupCall() && theApp->isDisableGroupShareCloud()) {
        /// 禁用群共享：\r\n请使用一对一聊天窗口发送文件！
        const QString text = theLocales.getLocalText("message-show.disable-group-share-tip","Disable group share error");
        EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING, default_warning_auto_close );
        return;
    }
    else if (m_widgetChatInput->isForbidSpeech()) {
        /// 禁言限制中：\r\n不能发送群文件！
        const QString text = theLocales.getLocalText("message-show.forbid-send-file-tip","Forbid speech error");
        EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING, default_warning_auto_close );
        return;
    }

    const QStringList paths = QFileDialog::getOpenFileNames(this);
//    const QStringList paths = QFileDialog::getOpenFileNames(this, QString(), QString(), QString(), 0, QFileDialog::DontResolveSymlinks);
    for (int i=0; i<paths.size(); i++) {
        const QString &filePath = paths.at(i);
        m_widgetChatInput->sendFile(filePath, false);
    }
}

void EbDialogChatBase::onClickedButtonVideoChat()
{
#ifdef _EB_USES_VIDEO_ROOM
    if (m_callInfo->isGroupCall()) {
        theApp->m_ebum.EB_VideoAck(m_callInfo->callId(), true, 0);
    }
    else {
        theApp->m_ebum.EB_VideoRequest(m_callInfo->callId(), EB_VIDEO_BOTH);
    }
#endif  /// _EB_USES_VIDEO_ROOM
}

void EbDialogChatBase::onTriggeredActionSendECard()
{
    /// 发送名片
    if (m_widgetChatInput->isForbidSpeech()) {
        /// 禁言限制中：\r\n不能发送名片！
        const QString text = theLocales.getLocalText("message-show.forbid-send-ecard-tip","Forbid speech error");
        EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING, default_warning_auto_close );
        return;
    }

    createDialogSelectUser();
    m_dialogSelectUser->setSingleSelect(true);
    m_dialogSelectUser->setSelectedGroupId(-1);
    m_dialogSelectUser->setVisible(true);
    if (m_dialogSelectUser->exec()==QDialog::Accepted) {
        tstring selectedAccount;
        EbWidgetItemInfo::pointer selectedItem;
        if (m_dialogSelectUser->m_selectedList.get_begin(&selectedAccount,&selectedItem,true)) {
            const eb::bigint selectedUserId =selectedItem->m_nUserId;
            if (selectedUserId>0)
                theApp->m_ebum.EB_SendUserCard( m_callInfo->callId(), selectedUserId );
            else
                theApp->m_ebum.EB_SendUserCard( m_callInfo->callId(), selectedAccount.c_str() );
        }
    }
    m_dialogSelectUser->clearSelected();

}

void EbDialogChatBase::onTriggeredActionChatApps()
{
    bool ok = false;
    const eb::bigint subId = ((QAction*)sender())->data().toLongLong(&ok);
    if (ok && subId>0) {
        m_widgetChatRight->onOpenSubId(subId);
    }
}

void EbDialogChatBase::onClickedButtonChatApps()
{
    if (m_menuChatApps==0) {
        m_menuChatApps = new QMenu(this);
        /// 发送名片
        const QString text = theLocales.getLocalText("context-menu.send-ecard.text","Send ECard");
        const QImage image = theApp->funcImage(0);
        const QIcon icon( QPixmap::fromImage(image).scaled(const_default_menu_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
        QAction * action = m_menuChatApps->addAction( icon,text );
        action->setToolTip( theLocales.getLocalText("context-menu.send-ecard.tooltip","") );
        connect( action, SIGNAL(triggered()), this, SLOT(onTriggeredActionSendECard()) );
    }
    else {
        QList<QAction*> actions = m_menuChatApps->actions();
        for ( int i=1;i<actions.size(); i++ ) {
            m_menuChatApps->removeAction( actions.at(i) );
        }
    }

    /// 应用功能菜单
    const EB_FUNC_LOCATION nFuncLocation = m_callInfo->isGroupCall()?EB_FUNC_LOCATION_GROUP_CHAT_BTN1:EB_FUNC_LOCATION_USER_CHAT_BTN1;
    theApp->clearSubscribeSelectInfo();
    std::vector<EB_SubscribeFuncInfo> funcList;
    theApp->m_ebum.EB_GetSubscribeFuncList(nFuncLocation, funcList );
    if ( !funcList.empty() ) {
        for (size_t i=0;i<funcList.size();i++) {
            const EB_SubscribeFuncInfo & funcInfo = funcList[i];
            const QImage image = theApp->funcImage(&funcInfo);
            const QIcon icon( QPixmap::fromImage(image).scaled(const_default_menu_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
            QAction * action = m_menuChatApps->addAction( icon,funcInfo.m_sFunctionName.c_str() );
            action->setToolTip( funcInfo.m_sFunctionName.c_str() );
            action->setData( QVariant(funcInfo.m_nSubscribeId) );
            connect( action, SIGNAL(triggered()), this, SLOT(onTriggeredActionChatApps()) );
        }
    }
    else {
        QAction * action = m_menuChatApps->addAction( theLocales.getLocalText("name-text.no-apps","No Apps") );
        action->setEnabled(false);
    }

    m_menuChatApps->popup(cursor().pos());
}

void EbDialogChatBase::exitChat(bool hangup)
{
    if (!requestClose(true)) {
        /// 检查不能退出会话，直接返回
        return;
    }

    if (hangup) {
        /// 需要挂断会话
        if ( !m_callInfo->isGroupCall() ) {
            theApp->m_ebum.EB_CallHangup(m_callInfo->callId());
            emit clickedClose();
        }
        else {
            EB_GroupInfo pGroupInfo;
            if (!theApp->m_ebum.EB_GetGroupInfo(m_callInfo->groupId(),&pGroupInfo)) {
                emit clickedClose();
            }
            else if ( pGroupInfo.m_nGroupType == EB_GROUP_TYPE_DEPARTMENT ||
                      pGroupInfo.m_nGroupType == EB_GROUP_TYPE_PROJECT ) {
                /// 部门，项目组
                emit clickedClose();
            }
            else if (pGroupInfo.m_nCreateUserId == theApp->logonUserId()) {
                /// 部门创建者，不能退出
                /// sText.Format(_T("群主退出，将会解散当前群组\r\n确定解散：\r\n%s 吗？"),pGroupInfo.m_sGroupName.c_str());
                /// sText.Format(_T("讨论组长退出，将会解散当前讨论组\r\n确定解散：\r\n%s 吗？"),pGroupInfo.m_sGroupName.c_str());
                QString title = theLocales.getLocalText("message-box.creator-exit-group.title","Exit Group");
                title.replace( "[GROUP_TYPE_NAME]",theLocales.getGroupTypeName(pGroupInfo.m_nGroupType)->name().c_str() );
                QString text = theLocales.getLocalText("message-box.creator-exit-group.text","Confirm exit group?");
                text.replace( "[GROUP_TYPE_MANAGER]",theLocales.getGroupTypeName(pGroupInfo.m_nGroupType)->manager().c_str() );
                text.replace( "[GROUP_TYPE_NAME]",theLocales.getGroupTypeName(pGroupInfo.m_nGroupType)->name().c_str() );
                text.replace( "[GROUP_NAME]",pGroupInfo.m_sGroupName.c_str() );
                if ( EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )!=QDialog::Accepted) {
                    this->m_widgetChatInput->setFocusInput();
                    return;
                }
                theApp->m_ebum.EB_DeleteGroup(m_callInfo->groupId());
                emit clickedClose();
            }
            else {
                /// 确定退出：\r\n%s 吗？
                QString title = theLocales.getLocalText("message-box.member-exit-group.title","Exit Group");
                title.replace( "[GROUP_TYPE_NAME]",theLocales.getGroupTypeName(pGroupInfo.m_nGroupType)->name().c_str() );
                QString text = theLocales.getLocalText("message-box.member-exit-group.text","Confirm exit group?");
                text.replace( "[GROUP_NAME]",pGroupInfo.m_sGroupName.c_str() );
                if ( EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )!=QDialog::Accepted) {
                    this->m_widgetChatInput->setFocusInput();
                    return;
                }
                theApp->m_ebum.EB_ExitGroup(m_callInfo->groupId());
                emit clickedClose();
            }
        }
    }
    else {
        /// 不需要挂断，直接退出即可
        emit clickedClose();
    }
}

void EbDialogChatBase::onClickedButtonExitChat()
{
    exitChat(true);
}

void EbDialogChatBase::onOpenSubId(eb::bigint subId)
{
    if (m_widgetChatRight!=0) {
        m_widgetChatRight->onOpenSubId(subId);
    }
}

bool EbDialogChatBase::onBeforeClickedPushButtonSysMin()
{
    EbDialogFrameList* parent = (EbDialogFrameList*)this->parentWidget();
    parent->onClickedPushButtonSysMin();
//    QApplication::postEvent(this->parent(), new QEvent((QEvent::Type)EB_COMMAND_SYS_MIN));
    return false;
}

bool EbDialogChatBase::onBeforeClickedPushButtonSysMax()
{
    EbDialogFrameList* parent = (EbDialogFrameList*)this->parentWidget();
    parent->onClickedPushButtonSysMax();
//    QApplication::postEvent(this->parent(), new QEvent((QEvent::Type)EB_COMMAND_SYS_MAX));
    return false;
}

bool EbDialogChatBase::requestClose(bool checkOnly)
{
    if (m_widgetChatRight!=0) {
        bool pVideoProcessing = false;
        bool pFileProcessing = false;
        bool pDesktopProcessing = false;
        m_widgetChatRight->getProcessing(pVideoProcessing,pFileProcessing,pDesktopProcessing);

        if (pVideoProcessing) {
            const QString title = theLocales.getLocalText("message-box.video-chat-exit-chat.title","Exit Chat");
            const QString text = theLocales.getLocalText("message-box.video-chat-exit-chat.text","Confirm exit chat?");
            if ( EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )!=QDialog::Accepted) {
                this->m_widgetChatInput->setFocusInput();
                return false;
            }
        }
        else if (pFileProcessing) {
            const QString title = theLocales.getLocalText("message-box.tran-file-exit-chat.title","Exit Chat");
            const QString text = theLocales.getLocalText("message-box.tran-file-exit-chat.text","Confirm exit chat?");
            if ( EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )!=QDialog::Accepted) {
                this->m_widgetChatInput->setFocusInput();
                return false;
            }
        }
//        if (pDesktopProcessing) {
//            if (CDlgMessageBox::EbDoModal(this,"退出会话",_T("正在远程桌面中：\t\n确定退出吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
//            {
//                return;
//            }
//        }
        m_widgetChatRight->onExitChat(false);
        if (checkOnly) {
            return true;
        }
    }
    theApp->m_ebum.EB_CallExit( this->m_callInfo->callId() );
    theApp->m_pCallList.remove( this->m_callInfo->callId() );
    return true;
}

void EbDialogChatBase::reject()
{
    if (!requestClose(false)) {
        /// 检查不能退出会话，直接返回
        return;
    }

    EbDialogFrameList* parent = (EbDialogFrameList*)this->parentWidget();
    parent->reject();//onClickedPushButtonSysClose();
//    QApplication::postEvent(this->parent(), new QEvent((QEvent::Type)EB_COMMAND_SYS_CLOSE));
//    return false;
}

void EbDialogChatBase::resizeEvent(QResizeEvent * e)
{
    updateSize();
    EbDialogFrameList* parent = (EbDialogFrameList*)this->parentWidget();
    if (parent->isMaximizedEb()) {
        /// 当前窗口最大化，显示还原按钮
        EbDialogBase::setMaxRestoreIcon(false);
    }
    else {
        /// 当前窗口还原，显示最大化按钮
        EbDialogBase::setMaxRestoreIcon(true);
    }
    EbDialogBase::resizeEvent(e);
}

void EbDialogChatBase::keyPressEvent(QKeyEvent * e)
{
    if (e->key()==Qt::Key_Escape) {
        onClickedInputClose();
        return;
    }
    EbDialogBase::keyPressEvent(e);
}

void EbDialogChatBase::updateSize()
{
//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
//        ui->widgetLeftBackground->setGeometry( 0, 0, m_showedLeft?const_border_left:const_hide_border_chat, height() );
//    }
//    else {
//        ui->widgetLeftBackground->setGeometry( 0, 0, m_showedLeft?const_border_left:const_hide_border_offiice, height() );
//    }
//    ui->pushButtonShowHideLeft->move(8,height()-(8+16));
    int y = 78;
    ui->widgetLine->setGeometry( 0,y,width(),1 );
    y += 1;
    if (m_splitterMain!=NULL) {
        m_splitterMain->setGeometry( 0,y,width(),height()-y );
    }

}

const int const_max_length = 80;

void EbDialogChatBase::processMsg(bool bReceive,const CCrRichInfo* pCrMsgInfo,EB_STATE_CODE nState,QString* sOutFirstMsg1,QString* sOutFirstMsg2)
{
    if (nState==EB_STATE_MAX_CAPACITY_ERROR) {
        /// 超过最大消息长度！
        m_textBrowserMessage->addLineString(0,theLocales.getLocalText("chat-msg-text.max-capacity-error","Max Capacity Error") );
        return;
    }
    else if (nState==EB_STATE_GROUP_FORBID_SPEECH) {
        /// 群禁言中！
        m_textBrowserMessage->addLineString(0,theLocales.getLocalText("chat-msg-text.group-forbid-speech","Group Forbid Speech") );
        return;
    }
    else if (nState==EB_STATE_FORBID_SPEECH) {
        /// 你被禁言中！
        m_textBrowserMessage->addLineString(0,theLocales.getLocalText("chat-msg-text.forbid-speech","Forbid Speech") );
        return;
    }
    m_textBrowserMessage->addRichMsg( true, bReceive, pCrMsgInfo, nState, sOutFirstMsg1, sOutFirstMsg2 );

//    QTextCursor cursor = this->m_textBrowserMessage->textCursor();
//    const bool hIsScrollEnd = (!bReceive || cursor.atEnd())?true:false;
//    if (hIsScrollEnd)
//        cursor.movePosition(QTextCursor::End);
    QScrollBar *scrollbar = m_textBrowserMessage->verticalScrollBar();
    scrollbar->setSliderPosition(scrollbar->maximum());

    if (nState==EB_STATE_EXCESS_QUOTA_ERROR) {
        /// 发送不成功，对方离线消息空间已满！
        m_textBrowserMessage->addLineString(0,theLocales.getLocalText("chat-msg-text.excess-quota-error","Excess Quota Error") );
    }
    else if (EB_STATE_NOT_MEMBER_ERROR==nState && m_callInfo->m_pCallInfo.m_sGroupCode>0) {
        /// 没有其他群组成员，不能发送聊天信息！
        m_textBrowserMessage->addLineString(0,theLocales.getLocalText("chat-msg-text.not-member-error","Not Member Error") );
    }
    else if (nState!=EB_STATE_OK && nState!=EB_STATE_FORWARD_MSG) {
        /// 发送失败，请重试！错误代码: %1
        QString sText = theLocales.getLocalText("chat-msg-text.send-rich-error","Send Rich Error");
        sText.replace("[STATE_CODE]",QString::number((int)nState));
        m_textBrowserMessage->addLineString(0,sText);
    }

//    if (sOutFirstMsg1!=NULL)
//    {
//        const CString sTemp(*sOutFirstMsg1);
//        if (bIsDepDialog)
//            sOutFirstMsg1->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-group://%lld\">%s</a>"),sTemp,m_callInfo->m_pCallInfo.m_sGroupCode,libEbc::ACP2UTF8("查看消息").c_str());
//        else
//            sOutFirstMsg1->Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-account://%lld\">%s</a>"),sTemp,m_callInfo->m_pCallInfo.GetFromUserId(),libEbc::ACP2UTF8("查看消息").c_str());
    //    }
}

bool EbDialogChatBase::processFile(bool bReceive, const CCrFileInfo *fileInfo)
{
    m_textBrowserMessage->addFileMsg( bReceive,fileInfo );
    return true;
}

void EbDialogChatBase::updateFromInfo()
{
//    CEBString sDescription;
    if (m_nFromLineState==EB_LINE_STATE_UNKNOWN) {
        m_nFromLineState = EB_LINE_STATE_ONLINE_NEW;
    }
    if ( isGroupChat() ) {
        /// 群组聊天
        EB_GroupInfo pGroupInfo;
        if (theApp->m_ebum.EB_GetGroupInfo(m_callInfo->groupId(),&pGroupInfo)) {
            m_sFromName = pGroupInfo.m_sGroupName.c_str();
            m_sFromDescription = pGroupInfo.m_sDescription.c_str();
            m_nGroupType = pGroupInfo.m_nGroupType;
            if (m_nGroupType==EB_GROUP_TYPE_TEMP)
                m_sFullName = m_sFromName;
            else
                m_sFullName = QString("%1(%2)").arg(m_sFromName).arg(pGroupInfo.m_sGroupCode);
        }
        if ( m_sFromName.isEmpty() ) {
            char lpszBuffer[24];
            sprintf(lpszBuffer,"%lld",m_callInfo->groupId());
            m_sFromName = lpszBuffer;
            m_sFullName = m_sFromName;
        }

        m_fromImage = theApp->groupHeadImage( m_callInfo->groupId(),m_nGroupType );
        switch (m_nGroupType)
        {
        case EB_GROUP_TYPE_DEPARTMENT:
        case EB_GROUP_TYPE_PROJECT: {
            /// 部门及项目组，不能随意添加成员，和不能随意退出
            ui->pushButtonAddUser->setVisible(false);
            ui->pushButtonExitChat->setVisible(false);
            break;
        }
        default:
            break;
        }
    }
    else {
        /// 单聊会话
        if (m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_VISITOR) {
            /// 游客会话
            m_fromImage = QImage(":/img/defaultvisitor.png");
            const QString visitorText = theLocales.getLocalText("chat-msg-text.visitor","Visitor");
            m_sFromName = QString("%1%2").arg(visitorText).arg(m_callInfo->m_pFromAccountInfo.GetAccount().c_str());
            m_sFullName = m_sFromName;
            m_sShortName = visitorText;
        }
        else {
            /// 普通用户聊天
            EB_MemberInfo pMemberInfo;
            if (theApp->m_ebum.EB_GetMemberInfoByMemberCode(&pMemberInfo,m_callInfo->fromMemberId()) ||
                theApp->m_ebum.EB_GetMemberInfoByUserId2(&pMemberInfo,m_callInfo->fromUserId())) {
                m_sFromName = pMemberInfo.m_sUserName.c_str();
                m_sFullName = QString("%1(%2)").arg(m_sFromName).arg(pMemberInfo.m_nMemberUserId);
                m_nFromLineState = pMemberInfo.m_nLineState;
                m_sFromDescription = pMemberInfo.m_sDescription.c_str();
                if ( QFileInfo::exists(pMemberInfo.m_sHeadResourceFile) ) {
                    m_imageMd5 = pMemberInfo.m_sHeadMd5;
                    m_fromImage = QImage(pMemberInfo.m_sHeadResourceFile);
                 }
            }
            else if (m_callInfo->m_bOffLineUser) {
                m_nFromLineState = EB_LINE_STATE_OFFLINE;
            }
            if (m_sFromName.isEmpty()) {
                if (!m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sName.empty()) {
                    m_sFromName = m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sName.c_str();
                }
                else {
                    m_sFromName = m_callInfo->m_pFromAccountInfo.GetUserName().c_str();
                }
                m_sFullName = QString("%1(%2)").arg(m_sFromName).arg(m_callInfo->fromUserId());
            }
        }
    }

    if (m_fromImage.isNull()) {
        if (QFileInfo::exists(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sHeadResourceFile)) {
            m_imageMd5 = m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sHeadMd5;
            m_fromImage = QImage(m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sHeadResourceFile);
        }
        else {
            m_fromImage = QImage(":/img/defaultmember.png");
        }
    }

    ui->labelName->setText(m_sFullName);
    ui->labelDescription->setText(m_sFromDescription);
}

void EbDialogChatBase::createDialogSelectUser()
{
    if (m_dialogSelectUser==0) {
        m_dialogSelectUser = new EbDialogSelectUser;
        m_dialogSelectUser->setModal(true);
//        m_dialogSelectUser->setWindowModality(Qt::WindowModal);
    }
}

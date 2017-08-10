#include "ebwidgetchatright.h"
#include <ebdialogmainframe.h>

const int const_top_button_width = 85;
EbWidgetChatRight::EbWidgetChatRight(const EbcCallInfo::pointer& pCallInfo,QWidget *parent)
    : EbDialogWorkFrame(false,false,parent)
    , m_callInfo(pCallInfo)
//    , m_workFrame(0)
{
    assert(m_callInfo.get()!=0);

    /// for test
//    m_workFrame = new EbDialogWorkFrame(this);
//    m_workFrame->setModal(false);
//    this->addUrl(false,"www.entboost.com");

    if (m_callInfo->groupId()==0) {
        ///
        EbWorkItem::pointer workItem = EbWorkItem::create(EbWorkItem::WORK_ITEM_USER_INFO);
        workItem->setCallInfo( m_callInfo );
        workItem->setTopButtonWidth( const_top_button_width );
        EbDialogWorkFrame::addWorkItem(false,workItem,0);
    }
    else {
        ///
        EbWorkItem::pointer workItem = EbWorkItem::create(EbWorkItem::WORK_ITEM_USER_LIST);
        workItem->setCallInfo( m_callInfo );
        workItem->setTopButtonWidth( const_top_button_width );
        EbDialogWorkFrame::addWorkItem(false,workItem,0);
    }
}

EbWidgetChatRight::~EbWidgetChatRight()
{
    onExitChat(false);
}

void EbWidgetChatRight::onExitChat(bool bHangup)
{
    EbWidgetVideoFrame *videoFrame = widgetVideoFrame();
    if ( videoFrame!=0 ) {
        videoFrame->onExitChat(bHangup);
    }
    EbWidgetFileTranList *tranFile = widgetTranFile();
    if ( tranFile!=0 ) {
        tranFile->onExitChat(bHangup);
    }
//    if (m_pPanRemoteDesktop!=NULL)
//        m_pPanRemoteDesktop->ExitChat(bHangup);
}

void EbWidgetChatRight::onUserExit(eb::bigint nUserId, bool exitGroup)
{
    EbWidgetUserList *widgetUserList = EbDialogWorkFrame::widgetUserList();
    if ( widgetUserList!=0 ) {
        widgetUserList->onExitUser(nUserId, exitGroup);
    }
}

//void EbWidgetChatRight::lineStateChange(eb::bigint userId, EB_USER_LINE_STATE lineState)
//{
//    EbWidgetUserList *widgetUserList = EbDialogWorkFrame::widgetUserList();
//    if ( widgetUserList!=0 ) {
//        widgetUserList->lineStateChange(userId, lineState);
//    }
//}

void EbWidgetChatRight::setCallInfo(const EbcCallInfo::pointer &pCallInfo)
{
    m_callInfo = pCallInfo;
    assert(m_callInfo.get()!=0);
}

void EbWidgetChatRight::onMsgReceipt(const CCrRichInfo *pCrMsgInfo, int nAckType)
{
//    if (m_pMsgRecord!=NULL)
//		m_pMsgRecord->OnMsgReceipt(pCrMsgInfo, nAckType);

}

void EbWidgetChatRight::onSendingFile(const CCrFileInfo *fileInfo)
{
    EbWidgetFileTranList *tranFile = this->openTranFile();
    tranFile->onSendingFile(fileInfo);
    if (tranFile->isEmpty()) {
        EbDialogWorkFrame::closeItem( indexOf(EbWorkItem::WORK_ITEM_TRAN_FILE) );
    }
}

void EbWidgetChatRight::onReceivingFile(const CCrFileInfo *fileInfo)
{
    EbWidgetFileTranList *tranFile = this->openTranFile();
    tranFile->onReceivingFile(fileInfo);
}

void EbWidgetChatRight::onReceivedFile(const CCrFileInfo *fileInfo)
{
    deleteTranFile(fileInfo->m_nMsgId);
}

void EbWidgetChatRight::onFilePercent(const CChatRoomFilePercent *pChatRoomFilePercent)
{
    EbWidgetFileTranList *tranFile = widgetTranFile();
    if ( tranFile!=0 ) {
        tranFile->onFilePercent(pChatRoomFilePercent);
    }
}

void EbWidgetChatRight::deleteTranFile(eb::bigint msgId)
{
    EbWidgetFileTranList *tranFile = widgetTranFile();
    if (tranFile!=0) {
        tranFile->deleteTranFile(msgId);
        if (tranFile->isEmpty()) {
            EbDialogWorkFrame::closeItem( indexOf(EbWorkItem::WORK_ITEM_TRAN_FILE) );
        }
    }
}

void EbWidgetChatRight::onMemberInfo(const EB_MemberInfo *memberInfo, bool bChangeLineState)
{
    EbWidgetUserList *userList = widgetUserList();
    if (userList!=0) {
        userList->onMemberInfo(memberInfo,bChangeLineState);
    }
}

void EbWidgetChatRight::getProcessing(bool &pVideoProcessing, bool &pFileProcessing, bool &/*pDesktopProcessing*/) const
{
    EbWidgetVideoFrame *videoFrame = widgetVideoFrame();
    pVideoProcessing = (videoFrame!=0 && !videoFrame->isEmpty())?true:false;
    const EbWidgetFileTranList *tranFile = widgetTranFile();
    pFileProcessing = (tranFile!=0 && !tranFile->isEmpty())?true:false;

//    if (m_pPanRemoteDesktop!=NULL)
//        pDesktopProcessing = m_pPanRemoteDesktop->GetInDesktop();
//    else
//        pDesktopProcessing = false;
}

void EbWidgetChatRight::showMsgRecord()
{
    EbWidgetChatRecord *chatRecord = widgetChatRecord();
    if (chatRecord==0) {
        EbWorkItem::pointer workItem = EbWorkItem::create(EbWorkItem::WORK_ITEM_CHAT_RECORD);
        workItem->setCallInfo( m_callInfo );
        workItem->setTopButtonWidth( const_top_button_width );
        EbDialogWorkFrame::addWorkItem(false,workItem);
    }
    else {
        EbDialogWorkFrame::closeItem( indexOf(EbWorkItem::WORK_ITEM_CHAT_RECORD) );
    }

}

void EbWidgetChatRight::onVRequestResponse(const EB_VideoInfo *pVideoInfo, int nStateValue)
{
    EbWidgetVideoFrame *videoFrame = this->openVideoFrame();
    videoFrame->onVRequestResponse(pVideoInfo, nStateValue);
}

void EbWidgetChatRight::onVAckResponse(const EB_VideoInfo *pVideoInfo, int nStateValue)
{
    EbWidgetVideoFrame *videoFrame = this->openVideoFrame();
    videoFrame->onVAckResponse(pVideoInfo, nStateValue);
}

void EbWidgetChatRight::onVideoRequest(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    EbWidgetVideoFrame *videoFrame = this->openVideoFrame();
    videoFrame->onVideoRequest(pVideoInfo, pUserVideoInfo);
//    if (m_callInfo->isGroupCall())
//        SetTimer(TIMERID_CHECK_ADJUST_WIDTH,500,NULL);
}

void EbWidgetChatRight::onVideoAccept(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    EbWidgetVideoFrame *videoFrame = this->openVideoFrame();
    videoFrame->onVideoAccept(pVideoInfo, pUserVideoInfo);
}

void EbWidgetChatRight::onVideoCancel(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    EbWidgetVideoFrame *videoFrame = this->widgetVideoFrame();
    if (videoFrame!=0) {
        videoFrame->onVideoCancel(pVideoInfo, pUserVideoInfo);
        if (!m_callInfo->isGroupCall()) {
            /// 一对一视频，关闭TAB
            EbDialogWorkFrame::closeItem( indexOf(EbWorkItem::WORK_ITEM_VIDEO_FRAME) );
        }
    }
//    if (m_pCallInfo.m_sGroupCode>0)
//        SetTimer(TIMERID_CHECK_ADJUST_WIDTH,200,NULL);
}

void EbWidgetChatRight::onVideoEnd(const EB_VideoInfo *pVideoInfo, const EB_UserVideoInfo *pUserVideoInfo)
{
    EbWidgetVideoFrame *videoFrame = this->widgetVideoFrame();
    if (videoFrame!=0) {
        videoFrame->onVideoEnd(pVideoInfo, pUserVideoInfo);
        if (!m_callInfo->isGroupCall()) {
            /// 一对一视频，关闭TAB
            EbDialogWorkFrame::closeItem( indexOf(EbWorkItem::WORK_ITEM_VIDEO_FRAME) );
        }
    }
//    if (m_pCallInfo.m_sGroupCode>0)
//        SetTimer(TIMERID_CHECK_ADJUST_WIDTH,200,NULL);
}

void EbWidgetChatRight::onOpenSubId(eb::bigint subId)
{
    EB_SubscribeFuncInfo funcInfo;
    if (!theApp->m_ebum.EB_GetSubscribeFuncInfo(subId, &funcInfo)) {
        return;
    }
    const eb::bigint m_nSelectCallId = m_callInfo->callId();
    const eb::bigint m_nSelectUserId = 0;
    const tstring m_sSelectAccount;
    const eb::bigint m_nSelectGroupId = m_callInfo->groupId();
    if (funcInfo.m_nFunctionMode == EB_FUNC_MODE_WINDOW ||
        funcInfo.m_nFunctionMode == EB_FUNC_MODE_MAINFRAME) {
        /// 窗口模式；
        const CEBString funcUrl = theApp->m_ebum.EB_GetSubscribeFuncUrl(funcInfo.m_nSubscribeId,
                                                                         m_nSelectCallId,m_nSelectUserId,m_sSelectAccount,m_nSelectGroupId);
        if (!funcUrl.empty()) {
            /// 250,250,25=fafafa
            char fullFuncUrl[260];
            sprintf( fullFuncUrl, "%s&color=fafafa",funcUrl.c_str());
            addUrl( false,fullFuncUrl,"", funcInfo );
        }
    }
    else {
        theApp->m_nSelectCallId = m_callInfo->callId();
        theApp->m_nSelectGroupId = m_callInfo->groupId();
        theApp->mainWnd()->openSubscribeFuncWindow(funcInfo);
    }
}

void EbWidgetChatRight::resizeEvent(QResizeEvent *e)
{
//    const int nwidth = width();
    EbDialogWorkFrame::resizeEvent(e);
}

EbWidgetVideoFrame *EbWidgetChatRight::openVideoFrame()
{
    EbWidgetVideoFrame *videoFrame = widgetVideoFrame();
    if (videoFrame==0) {
        EbWorkItem::pointer workItem = EbWorkItem::create(EbWorkItem::WORK_ITEM_VIDEO_FRAME);
        workItem->setCallInfo( m_callInfo );
        workItem->setTopButtonWidth( const_top_button_width );
        EbDialogWorkFrame::addWorkItem(false, workItem);
        videoFrame = workItem->widgetVideoFrame();
    }
    return videoFrame;
}

EbWidgetFileTranList *EbWidgetChatRight::openTranFile()
{
    EbWidgetFileTranList *tranFile = widgetTranFile();
    if (tranFile==0) {
        EbWorkItem::pointer workItem = EbWorkItem::create(EbWorkItem::WORK_ITEM_TRAN_FILE);
        workItem->setCallInfo( m_callInfo );
        workItem->setTopButtonWidth( const_top_button_width );
        EbDialogWorkFrame::addWorkItem(false, workItem);
        tranFile = workItem->widgetTranFile();
    }
    return tranFile;
}

//void EbWidgetChatRight::updateGroupUsers()
//{
//    if ( m_callInfo->isGroupCall() ) {
//        int memberSize = 0;
//        int onlineSize = 0;
//        theApp->m_ebum.EB_GetGroupMemberSize(m_callInfo->groupId(),1,memberSize,onlineSize);
//        //const int nMemberSize = theEBAppClient.EB_GetGroupMemberSize(m_pCallInfo.m_sGroupCode,1);
//        //const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(m_pCallInfo.m_sGroupCode,1);
//        /// 群成员(%d/%d)
//        /// 群成员(%d)
//        QString text;
//        if (onlineSize>=0)
//            text = QString("%1(%2/%3)").arg(theLocales.getLocalText("user-list.title","User List")).arg(onlineSize).arg(memberSize);
//        else
//            text = QString("%1(%2)").arg(theLocales.getLocalText("user-list.title","User List")).arg(memberSize);
//        setItemText(EbWorkItem::WORK_ITEM_USER_LIST,text);
//    }
//}

#include "ebtextbrowser.h"
#include <ebwebengineurlschemehandler.h>
#include <ebtextblockuserdata.h>
#include <ebframechattoolbar.h>
#include <QTextFragment>
#include <QBitmap>
#include <QFileIconProvider>
#include <ebdialogviewecard.h>
#include <ebmessagebox.h>

const int const_max_length = 80;

EbTextBrowser::EbTextBrowser(const EbcCallInfo::pointer& pCallInfo,QWidget* parent)
    : QTextBrowser(parent)
    , m_callInfo(pCallInfo)
    , m_timerIdFindTextBlockFromPosition(0)
{
//    str.replace("&","&amp;");
//    str.replace(">","&gt;");
//    str.replace("<","&lt;");
//    str.replace("\"","&quot;");
//    str.replace("\'","&#39;");
//    str.replace(" ","&nbsp;");
//    str.replace("\n","<br>");
//    str.replace("\r","<br>");

    assert( m_callInfo.get()!=0 );
    m_chatToolBar = new EbFrameChatToolBar( m_callInfo, parent );
    m_chatToolBar->setVisible( false );
    m_chatToolBar->raise();
    connect( m_chatToolBar,SIGNAL(deleteMessage(qint64)),this,SLOT(onDeleteMessage(qint64)) );

    connect( this,SIGNAL(anchorClicked(QUrl)),this,SLOT(onAnchorClicked(QUrl)) );
//    connect( this,SIGNAL(copyAvailable(bool)),this,SLOT(onCopyAvailable(bool)) );

//    loadHistoryMsg();
}

EbTextBrowser::~EbTextBrowser()
{
//    if ( m_chatToolBar!=0 ) {
//        delete m_chatToolBar;
//        m_chatToolBar = 0;
//    }

}

EbTextBrowser *EbTextBrowser::create(const EbcCallInfo::pointer &callInfo, QWidget *parent)
{
    EbTextBrowser *textBrowser = new EbTextBrowser(callInfo, parent);
    textBrowser->setStyleSheet("border: none;");
    textBrowser->setReadOnly(true);
    /// ** 屏蔽打开链接（打开会刷新整个界面白屏），由程序内部处理
    textBrowser->setOpenLinks(false);
    return textBrowser;
}

void EbTextBrowser::setCallInfo(const EbcCallInfo::pointer &pCallInfo)
{
    m_callInfo = pCallInfo;
    assert(m_callInfo.get()!=0);
    m_chatToolBar->setCallInfo(m_callInfo);
}

void EbTextBrowser::insertUrl(const QString &url)
{
    /// 后面增加一个空格，避免后面字符同样是链接点击问题
    if (url.left(4).toLower()=="http") {
        insertHtml(QString("<a href=\"%1\">%2</a> ").arg(url).arg(url));
    }
    else {
        insertHtml(QString("<a href=\"http://%1\">%2</a> ").arg(url).arg(url));
    }
}

void EbTextBrowser::insertPlainTextEb(const QString &text,const QColor & color)
{
    QString sString1(text);
    int nfind1 = sString1.indexOf("http://",Qt::CaseInsensitive);
    if (nfind1 < 0)
        nfind1 = sString1.indexOf("https://",Qt::CaseInsensitive);
    if (nfind1 < 0)
        nfind1 = sString1.indexOf("www.",Qt::CaseInsensitive);
    if (nfind1>=0) {
        if (nfind1 > 0) {
            setTextColor(color);
            QTextBrowser::insertPlainText( sString1.left(nfind1) );
            sString1 = sString1.mid( nfind1 );
        }
        nfind1 = -1;
        //nfind1 = sString1.FindOneOf(L" \t\r\n;,；，。");
        //bool bFindBigChar = false;
        //if (nfind1 < 0)
        {
            /// 判断中文和 HTML 链接结束符
            const int nCount = sString1.count();
            for(int i=0; i<nCount ; i++)
            {
                QChar cha = sString1.at(i);
                ushort uni = cha.unicode();
                if(uni>=0x4E00 && uni<=0x9FA5) {
                    /// 这个字符是中文
                    nfind1 = i;
                    break;
                }
                else if (cha==160 || cha=='\t' || cha=='\r' || cha=='\n' || cha==';' || cha==',' || cha==' ') {
                    /// HTML 链接结束符
                    /// 160=' '
                    nfind1 = i;
                    break;
                }
            }
        }

        if (nfind1 > 0) {
            insertUrl( sString1.left(nfind1) );
            sString1 = sString1.mid(nfind1);
            insertPlainTextEb(sString1,color);
            return;
        }
        else {
            insertUrl(sString1);
            return;
        }
    }
    else {
        setTextColor(color);
        QTextBrowser::insertPlainText(text);
    }
}

void EbTextBrowser::addRichMsg(bool saveHistory, bool bReceive, const CCrRichInfo *pCrMsgInfo, EB_STATE_CODE nState, QString *sOutFirstMsg1, QString *sOutFirstMsg2)
{
    const EB_ChatRoomRichMsg * pRichMsg = pCrMsgInfo->m_pRichMsg;
    const mycp::bigint msgId = pRichMsg->GetMsgId();
//    const int nSubType = pRichMsg->GetSubType();

//    m_bLastReceiveMsg = bReceive;
    //if (m_pPanelStatus!=NULL && m_pPanelStatus->IsWindowVisible())
    //{
    //	CRect rectPanelStatus;
    //	m_pPanelStatus->GetClientRect(&rectPanelStatus);
    //	if (rectPanelStatus.left==0 && bReceive)
    //	{
    //		//CRect rect;
    //		//m_wndSplitter1.GetClientRect(&rect);
    //		m_pPanelStatus->MoveWindow(100,rectPanelStatus.top,rectPanelStatus.Width(),rectPanelStatus.Height());
    //		//m_pPanelStatus->MoveWindow(rect.right-rectPanelStatus.Width(),rectPanelStatus.top,rectPanelStatus.Width(),rectPanelStatus.Height());
    //		//m_pPanelStatus->ShowWindow(SW_HIDE);
    //	}
    //}
//    const eb::bigint sSender = pCrMsgInfo->m_sSendFrom;
    CEBString sFromName;
    CEBString sToName;
    getFromToName( bReceive,pCrMsgInfo->m_sSendFrom,pCrMsgInfo->m_sSendTo,sFromName,sToName );
//    if (bReceive) {
//        if (m_callInfo->m_pCallInfo.m_sGroupCode==0) {
//            if (m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_VISITOR) {
//                const std::string visitorText = theLocales.getLocalStdString("chat-msg-text.visitor","Visitor");
//                sFromName = visitorText + m_callInfo->m_pFromAccountInfo.GetAccount();
//            }
//            else
//                sFromName = m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sName;
//            sToName = theApp->m_ebum.EB_GetUserName();
////            if (m_pPanelStatus!=NULL && m_pPanelStatus->IsWindowVisible())
////            {
////                KillTimer(TIMERID_HIDE_NOTIFY1);
////                SetTimer(TIMERID_HIDE_NOTIFY1,1000,NULL);
////            }
//        }
//        else {
//            theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->m_pCallInfo.m_sGroupCode,sSender,sFromName);
//            if (pCrMsgInfo->m_sSendTo>0)
//                theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendTo,sToName);
//        }
//    }
//    else {
//        if (m_callInfo->m_pCallInfo.m_sGroupCode==0)
//            sFromName = theApp->m_ebum.EB_GetUserName();
//        else {
//            theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->m_pCallInfo.m_sGroupCode,theApp->m_ebum.EB_GetLogonUserId(),sFromName);
//        }
//        if (pCrMsgInfo->m_sSendTo>0)
//            theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendTo,sToName);
//    }

    std::string sDBMsgTimeField;
    char sDBMsgTimeValue[64];
    memset(sDBMsgTimeValue,0,sizeof(sDBMsgTimeValue));
    time_t tMsgTime = time(0);
    if (!pRichMsg->m_sMsgTime.empty()) {
        libEbc::ChangeTime(pRichMsg->m_sMsgTime.c_str(),tMsgTime);
        sDBMsgTimeField = "msg_time,";
        sprintf(sDBMsgTimeValue, "Datetime('%s','utc'),", pRichMsg->m_sMsgTime.c_str());
    }
    QString sWindowText;
    const bool writeLeft = bReceive;
    writeTitle( writeLeft,msgId,pCrMsgInfo->m_bPrivate,pCrMsgInfo->m_sSendFrom,sFromName,pCrMsgInfo->m_sSendTo,sToName,tMsgTime,0,&sWindowText );
    addChatMsgBlock( msgId,bReceive );

    const int nReadFlag = 1;
    //const int nReadFlag = this->GetParent()->IsWindowVisible()?1:0;
    //if (bReceive && nReadFlag==0)
    //	m_pMrFrameInterface->WritePic(1,2,4,7,7,theDefaultBtnCloseColor,EB_MR_CTRL_TYPE_LCLICK_CB,EB_MR_CTRL_DATA_TYPE_READ_FLAG);

    if (sOutFirstMsg1!=0)
        *sOutFirstMsg1 = sWindowText + "<br>";
    if (sOutFirstMsg2!=0)
        *sOutFirstMsg2 = sWindowText + "\n";

    eb::bigint sSaveDbToAccount = pCrMsgInfo->m_sSendTo;
    if (m_callInfo->m_pCallInfo.m_sGroupCode==0)
        sSaveDbToAccount = m_callInfo->m_pFromAccountInfo.GetUserId();
    char sSql[theMaxDatabaseTextSize+2048];
    //int ret = 0;
//    m_pMrFrameInterface->AddLine(pRichMsg->GetMsgId());
//    m_pMrFrameInterface->SetAlignmentFormat(bReceive?0:2);
//    m_pMrFrameInterface->SetFrameArc(theArcOffset,thePoloygonWidth,thePoloygonHeight);
//    m_pMrFrameInterface->SetFrameBorderColor(RGB(128,128,128));
//    m_pMrFrameInterface->SetFrameBackGroundColor(bReceive?theDefaultChatBackGroundColor2:theDefaultChatBackGroundColor1);
//    if (!bReceive && m_callInfo->m_pCallInfo.m_sGroupCode==0)
//        m_pMrFrameInterface->SetReceiptFlag(EBC_CONTRON_RECEIPT_FLAG_SHOW);

    tstring sInFromName(sFromName);
    theApp->m_sqliteUser->escape_string_in(sInFromName);
    tstring sInToName(sToName);
    theApp->m_sqliteUser->escape_string_in(sInToName);

    int nOutMsgLength = 0;
    const std::vector<EB_ChatRoomMsgItem*>& pRichMsgList = pRichMsg->GetList();
    for (size_t i=0; i<pRichMsgList.size(); i++) {
        const EB_ChatRoomMsgItem* pMsgItem = pRichMsgList[i];
        if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_TEXT) {
            if (sOutFirstMsg1!=0 && nOutMsgLength<const_max_length) {
                const QString sTemp(pMsgItem->GetText().substr(0,const_max_length).c_str());
                nOutMsgLength += sTemp.length();
                *sOutFirstMsg1 += sTemp;
            }
            if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
                const QString sTemp(pMsgItem->GetText().substr(0,const_max_length).c_str());
                *sOutFirstMsg2 += sTemp;
            }
            this->insertPlainTextEb( QString::fromStdString(pMsgItem->GetText().string()),QColor(64,64,64) );

            if (saveHistory && theApp->isSaveConversationLocal() && !theApp->m_ebum.EB_IsLogonVisitor())
            {
                tstring sText(pMsgItem->GetText());
                CSqliteCdbc::escape_string_in(sText);
                while (!sText.empty()) {
                    const tstring sSaveText = sText.size()>theMaxDatabaseTextSize?sText.substr(0,theMaxDatabaseTextSize):sText;
                    sprintf(sSql,"INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_text,read_flag) "
                                 "VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s',%d)",
                            sDBMsgTimeField.c_str(),sDBMsgTimeValue,pRichMsg->GetMsgId(),m_callInfo->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendFrom,sInFromName.c_str(),
                        sSaveDbToAccount,sInToName.c_str(),(int)(pCrMsgInfo->m_bPrivate?1:0),(int)MRT_TEXT,sSaveText.c_str(),nReadFlag);
                    theApp->m_sqliteUser->execute(sSql);
                    sText = sText.substr(sSaveText.size());
                }
            }
        }
        else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_RESOURCE) {
            const tstring sResourceInfo(pMsgItem->GetText());
            std::vector<tstring> pList;
            if (libEbc::ParseString(sResourceInfo.c_str(),";",pList)>=3) {
                const tstring sResourceId(pList[0]);
                const tstring sDescription = pList.size()>3?pList[3]:"";
                QString sImageFileName = QString("%1/%2").arg(theApp->m_ebum.EB_GetResourcePath()).arg(sResourceId.c_str());
                if (sOutFirstMsg1!=0 && nOutMsgLength<const_max_length) {
                    const QString sTemp = QString("<img src=\"%1\" alt=\"%2\"/>").arg(sImageFileName).arg(sDescription.c_str());
                    nOutMsgLength += 30;
                    *sOutFirstMsg1 += sTemp;
                }
                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
                    const QString sTemp = QString("[%1]").arg(sDescription.c_str());
                    *sOutFirstMsg2 += sTemp;
                }
                /// 这种头像复制后，粘贴有问题，不能正常显示
//                const QImage image(sImageFileName);
//                const QString resourceCmInfo = QString::fromStdString(sResourceInfo.string());
//                this->document()->addResource(QTextDocument::ImageResource, QUrl(resourceCmInfo), image);
//                this->textCursor().insertImage(resourceCmInfo);
                /// 使用这种复制后，可以粘贴显示，并发送（以文件图片发送，不是以资源发送，后期再优化成资源发送）
                QString sTemp = QString("<img src=\"%1\" alt=\"%2\"/>").arg(sImageFileName).arg(sDescription.c_str());
                this->insertHtml(sTemp);

                if (saveHistory && theApp->isSaveConversationLocal() && !theApp->m_ebum.EB_IsLogonVisitor()) {
                    sprintf(sSql,"INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag) "
                                 "VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s','%s',%d)",
                            sDBMsgTimeField.c_str(),sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_callInfo->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendFrom,sInFromName.c_str(),
                        sSaveDbToAccount,sInToName.c_str(),(int)(pCrMsgInfo->m_bPrivate?1:0),(int)MRT_JPG,sImageFileName.toStdString().c_str(),sDescription.c_str(),nReadFlag);
                    theApp->m_sqliteUser->execute(sSql);
                }
            }
        }
        else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT) {
            QString sObjectFileName;
            tstring sObjectSaveData;	// ** to save msg_text
            const EB_RICH_SUB_TYPE nSubType = (EB_RICH_SUB_TYPE)pCrMsgInfo->m_pRichMsg->GetSubType();
            bool bIsFile = true;
            EB_MSG_RECORD_TYPE nRecordType = MRT_JPG;
            if (bReceive || nState==EB_STATE_FORWARD_MSG) {
                static unsigned int static_index = 0;
                static_index++;
                if (nSubType == EB_RICH_SUB_TYPE_JPG) {
                    nRecordType = MRT_JPG;
                    sObjectFileName = QString("%1/%2%3%4.jpg").arg(theApp->userImagePath()).arg(time(0)).arg(rand()%0xff).arg(static_index%0xff);
                }
                else if (nSubType==EB_RICH_SUB_TYPE_AUDIO) {	/// wav格式
                    nRecordType = MRT_WAV;
                    sObjectFileName = QString("%1/%2%3%4.wav").arg(theApp->userImagePath()).arg(time(0)).arg(rand()%0xff).arg(static_index%0xff);
                }
                else if (nSubType==EB_RICH_SUB_TYPE_MAP_POS) {	/// 地图位置
                    bIsFile = false;
                    nRecordType = MRT_MAP_POS;
                    //sObjectFileName = pMsgItem->GetData();
                }
                else if (nSubType==EB_RICH_SUB_TYPE_CARD_INFO) {	/// 名片数据
                    bIsFile = false;
                    nRecordType = MRT_CARD_INFO;
                    //sObjectFileName = pMsgItem->GetData();
                    sObjectSaveData = pMsgItem->GetData();
                }
                else if (nSubType==EB_RICH_SUB_TYPE_USER_DATA) {	/// 用户自定义数据
                    nRecordType = MRT_USER_DATA;
                    sObjectFileName = QString("%1/%2%3%4").arg(theApp->userImagePath()).arg(time(0)).arg(rand()%0xff).arg(static_index%0xff);
                }
                else {
                    /// 普通文件；
                    nRecordType = MRT_FILE;
                    sObjectFileName = QString("%1/%2%3%4").arg(theApp->userImagePath()).arg(time(0)).arg(rand()%0xff).arg(static_index%0xff);
                }
                if (bIsFile) {
                //if (nRecordType != MRT_MAP_POS) {
                    const char * lpObjectData = pMsgItem->GetData();
                    const unsigned long dwDataSize = pMsgItem->GetSize();
                    FILE * hFile = fopen(sObjectFileName.toLocal8Bit().constData(),"wb");
                    if (hFile!=0) {
                        fwrite(lpObjectData,dwDataSize,1,hFile);
                        fclose(hFile);
                    }
                }
            }
            else {
                sObjectFileName = pMsgItem->GetText().c_str();
                if (nSubType == EB_RICH_SUB_TYPE_AUDIO)
                    nRecordType = MRT_WAV;
                else if (nSubType == EB_RICH_SUB_TYPE_MAP_POS)		/// ??
                    nRecordType = MRT_MAP_POS;
                else if (nSubType == EB_RICH_SUB_TYPE_CARD_INFO) {	/// 名片信息
                    nRecordType = MRT_CARD_INFO;
                    sObjectSaveData = pMsgItem->GetData();
                }
                else if (nSubType == EB_RICH_SUB_TYPE_USER_DATA) 	/// ??
                    nRecordType = MRT_USER_DATA;
            }

            if (nSubType == EB_RICH_SUB_TYPE_JPG) {
                QString sTemp = QString("<img src=\"%1\" />").arg(sObjectFileName);
                this->insertHtml(sTemp);
                if (sOutFirstMsg1!=0 && nOutMsgLength<const_max_length) {
                    nOutMsgLength += 30;
                    *sOutFirstMsg1 += QString("<img src=\"%1\" width=\"80\" height=\"80\" />").arg(sObjectFileName);
                }
                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
                    /// [图片]
                    *sOutFirstMsg2 += "[";
                    *sOutFirstMsg2 += theLocales.getLocalText("chat-msg-text.image","Image");
                    *sOutFirstMsg2 += "]";
                }
            }
            else if (nSubType==EB_RICH_SUB_TYPE_AUDIO) {
                /// wav格式
                //WriteFileHICON(sObjectFileName);
                QString sText;
                writeVoiceMessage( sObjectFileName, &sText );
                if (sOutFirstMsg1!=0 && nOutMsgLength<const_max_length) {
                    nOutMsgLength += 12;
                    *sOutFirstMsg1 +=  sText;
                }
                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
                    /// [语音]
                    *sOutFirstMsg2 += "[";
                    *sOutFirstMsg2 += theLocales.getLocalText("chat-msg-text.voice","Voice");
                    *sOutFirstMsg2 += "]";
                }
            }
            else if (nSubType==EB_RICH_SUB_TYPE_MAP_POS) {
                /// 地图位置 ???
                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
                    /// [地图位置]
                    *sOutFirstMsg2 += "[";
                    *sOutFirstMsg2 += theLocales.getLocalText("chat-msg-text.map-pos","Map Pos");
                    *sOutFirstMsg2 += "]";
                }
            }
            else if (nSubType==EB_RICH_SUB_TYPE_CARD_INFO) {
                /// 名片信息
                /// *** 这里要解析并显示名片信息
                QString sCardMessage;
                if (writeCardDataMessage( bReceive,msgId, sObjectSaveData, &sCardMessage ) ) {
                    if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
                        /// %s[个人名片]
                        *sOutFirstMsg2 += sCardMessage;
                    }
                }
                else if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
                    /// [名片信息]
                    *sOutFirstMsg2 += "[";
                    *sOutFirstMsg2 += theLocales.getLocalText("chat-msg-text.user-ecard","User Ecard");
                    *sOutFirstMsg2 += "]";
                }
            }
            else if (nSubType==EB_RICH_SUB_TYPE_USER_DATA) {
                /// 用户自定义数据 ???
                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
                    /// [用户自定义数据]
                    *sOutFirstMsg2 += "[";
                    *sOutFirstMsg2 += theLocales.getLocalText("chat-msg-text.user-data","User Data");
                    *sOutFirstMsg2 += "]";
                }
            }
            else {
                QString sFileName;
                writeFileMessage(msgId, 0, sObjectFileName, 0, false, &sFileName);
                if (sOutFirstMsg1!=0 && nOutMsgLength<const_max_length) {
                    /// 《file-name》文件
                    const QString sTemp = QString("《%1》%2").arg(sFileName).arg(theLocales.getLocalText("chat-msg-text.file","File"));
                    nOutMsgLength += sTemp.length();
                    *sOutFirstMsg1 += sTemp;
                }
            }
            if (saveHistory && theApp->isSaveConversationLocal() && !theApp->m_ebum.EB_IsLogonVisitor()) {
                CSqliteCdbc::escape_string_in(sObjectSaveData);
                sprintf(sSql,"INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag) "
                             "VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s','%s',%d)",
                        sDBMsgTimeField.c_str(),sDBMsgTimeValue,pRichMsg->GetMsgId(),this->m_callInfo->m_pCallInfo.m_sGroupCode,pCrMsgInfo->m_sSendFrom,sInFromName.c_str(),
                        sSaveDbToAccount,sInToName.c_str(),(int)(pCrMsgInfo->m_bPrivate?1:0),nRecordType,sObjectFileName.toStdString().c_str(),sObjectSaveData.c_str(),nReadFlag);
                theApp->m_sqliteUser->execute(sSql);
            }
        }
    }
}

void EbTextBrowser::onReceivingFile(bool offLineUser, const CCrFileInfo *fileInfo)
{
    const eb::bigint sendFromUserId = fileInfo->m_sSendFrom;
    if (m_callInfo->isGroupCall() && fileInfo->m_sResId>0) {
        /// %s 上传群共享文件：%s
        const mycp::bigint msgId = fileInfo->m_nMsgId;
        time_t tMsgTime = time(0);
        if (!fileInfo->m_sFileTime.empty()) {
            libEbc::ChangeTime(fileInfo->m_sFileTime.c_str(),tMsgTime);
        }
        CEBString fromUserName;
        CEBString toUserName;
        getFromToName( true,fileInfo->m_sSendFrom,0,fromUserName,toUserName );
        writeTitle( true,msgId,false,fileInfo->m_sSendFrom,fromUserName,0,"",tMsgTime,0 );
        this->addChatMsgBlock( msgId,true );
        writeFileMessage(fileInfo->m_nMsgId, fileInfo->m_sResId, fileInfo->m_sFileName, fileInfo->m_nFileSize);
    }
}

void EbTextBrowser::addFileMsg(bool bReceive, const CCrFileInfo *fileInfo)
{
//    const EB_STATE_CODE nState = fileInfo->GetStateCode();
    if (fileInfo->m_sSendFrom==0 && fileInfo->m_sSendTo==0) {
        /// 应该是 onReceivingFile 下载资源文件，不需要提示
        return;
    }
    const mycp::bigint msgId = fileInfo->m_nMsgId;
    time_t tMsgTime = time(0);
    if (!fileInfo->m_sFileTime.empty()) {
        libEbc::ChangeTime(fileInfo->m_sFileTime.c_str(),tMsgTime);
    }
    CEBString fromUserName;
    CEBString toUserName;
    getFromToName( bReceive,fileInfo->m_sSendFrom,fileInfo->m_sSendTo,fromUserName,toUserName );
    if (bReceive && m_callInfo->isGroupCall() && fileInfo->m_sResId>0) {
        /// 成功下载
        writeTitle( false,msgId,false,fileInfo->m_sSendTo,toUserName,fileInfo->m_sSendFrom,fromUserName,tMsgTime,EBC_READ_FLAG_RECEIPT );
        this->addChatMsgBlock( msgId,false );
    }
    else if (bReceive && this->m_callInfo->isGroupCall() && fileInfo->m_sResId>0) {
        writeTitle(bReceive, msgId, false, fileInfo->m_sSendFrom, fromUserName, fileInfo->m_sSendTo, toUserName, tMsgTime, EBC_READ_FLAG_RECEIPT);
        this->addChatMsgBlock(msgId, bReceive);
    }
    else if ((!bReceive && fileInfo->m_sSendFrom!=theApp->logonUserId())) {
        writeTitle( true,msgId,false,fileInfo->m_sSendFrom,fromUserName,fileInfo->m_sSendTo,toUserName,tMsgTime,0 );
        this->addChatMsgBlock( msgId,true );
    }
    else {
        writeTitle( false,msgId,false,fileInfo->m_sSendTo,toUserName,fileInfo->m_sSendFrom,fromUserName,tMsgTime,EBC_READ_FLAG_RECEIPT );
        this->addChatMsgBlock( msgId,false );
    }

    QString sWindowText;
    bool bUpdateMsgReceiptData = false;
    bool bUploadGroupFile = false;
    bool showFileNameOnly = true;
    if (bReceive && m_callInfo->isGroupCall() && fileInfo->m_sResId>0) {
        /// 成功下载
        sWindowText = theLocales.getLocalText("chat-msg-text.down-file-ok","Download File Ok");
    }
    else if (fileInfo->m_sSendFrom!=theApp->logonUserId() && this->m_callInfo->isGroupCall() && fileInfo->m_sResId>0) {
        /// fileInfo->m_sSendFrom==theApp->logonUserId() 是
        showFileNameOnly = false;
//        CEBString memberName;
//        theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),fileInfo->m_sSendFrom,memberName);
//        sWindowText = memberName.c_str();
    }
    else if (bReceive) {
        if (fileInfo->m_sResId>0)   /// 成功下载文件
            sWindowText = theLocales.getLocalText("chat-msg-text.down-file-ok","Download File Ok");
        else    /// 成功接收文件
            sWindowText = theLocales.getLocalText("chat-msg-text.receive-file-ok","Receive File Ok");
    }
    else if (fileInfo->m_sReceiveAccount==theApp->logonUserId()) {
        if (fileInfo->m_bOffFile) {
            /// 成功发送离线文件
            sWindowText = theLocales.getLocalText("chat-msg-text.send-off-file-ok","Send Off File Ok");
        }
        else {
            if (m_pPrevReceivedFileMsgIdList.remove(fileInfo->m_nMsgId)) {
                return;	/// *前面已经提示“对方已经接收文件”，直接返回不再打印提示
//                return false;	/// *前面已经提示“对方已经接收文件”，直接返回不再打印提示
            }
            if ( m_callInfo->isGroupCall() ) {
                bUploadGroupFile = true;
                /// 上传群共享文件
                sWindowText = theLocales.getLocalText("chat-msg-text.send-group-file","Send Group File");
            }
            else {
                /// 成功发送文件
                sWindowText = theLocales.getLocalText("chat-msg-text.send-file-ok","Send File Ok");
            }
        }
    }
    else {
        if ( m_callInfo->isGroupCall() ) {
            /// %lld已经接收
            sWindowText = QString("%1 %2").arg(fileInfo->m_sReceiveAccount).arg(theLocales.getLocalText("chat-msg-text.user-receive-file","Receive File"));
        }
        else {
            /// 对方已经接收文件
            sWindowText = theLocales.getLocalText("chat-msg-text.dest-receive-file","Dest Receive File");
            m_pPrevReceivedFileMsgIdList.insert(fileInfo->m_nMsgId,true,false);
            bUpdateMsgReceiptData = true;
        }
    }
    if ( !sWindowText.isEmpty() ) {
        /// 上传群共享文件，在 writeFileMessage() 里面会打开日志
        const QString html = QString("<font color=#808080>%1</font>").arg(sWindowText);
        this->insertHtml(html);
    }

    const int nWavTimeLength = libEbc::GetWaveTimeLength(fileInfo->m_sFileName);
    if (nWavTimeLength > 0) {
        /// 是WAV语音文件
        this->writeVoiceMessage(fileInfo->m_sFileName);
    }
    else {
        writeFileMessage(msgId, fileInfo->m_sResId, fileInfo->m_sFileName, fileInfo->m_nFileSize, showFileNameOnly);
    }

    if (bUpdateMsgReceiptData) {
        this->updateMsgReceiptData(fileInfo->m_nMsgId, m_callInfo->fromUserId(), 0, fileInfo->GetStateCode());
    }

//    const bool hIsScrollEnd = (!bReceive || m_pMrFrameInterface->IsScrollEnd()==VARIANT_TRUE)?true:false;
//    m_pMrFrameInterface->UpdateSize(VARIANT_TRUE);
//    if (hIsScrollEnd)
//        m_pMrFrameInterface->ScrollToPos(-1);
//    return true;
}

void EbTextBrowser::writeMsgDate(time_t tMsgTime)
{
    const QDateTime pMsgDateTime = QDateTime::fromSecsSinceEpoch(tMsgTime);
    const QDate dateMsg = pMsgDateTime.date();
    const QDate dateLastMsg = m_tLastMsgDayTime.date();
    if ( this->document()->isEmpty() ||
         dateMsg.year() != dateLastMsg.year()  ||
         dateMsg.month() != dateLastMsg.month() ||
         dateMsg.day() != dateLastMsg.day() ) {
        m_tLastMsgDayTime = pMsgDateTime;
        int nDayOfWeek = dateMsg.dayOfWeek();
        if (nDayOfWeek>=7) {
            nDayOfWeek = 0; // 0=星期天
        }
        const EbDayOfWeekInfo::pointer & pDayOfWeekInfo = theLocales.getDayOfWeekInfo(nDayOfWeek);
        char sText[256];
        sprintf(sText,"--------  %04d-%02d-%02d %s  --------",
                dateMsg.year(),dateMsg.month(),dateMsg.day(),
                pDayOfWeekInfo->shortName().c_str());
        addLineString( 0,sText );
    }
}

void EbTextBrowser::moveTextBrowserToStart()
{
    const QTextBlock firstBlock = this->document()->firstBlock();
    QTextCursor firstBlackCursor(firstBlock);
    firstBlackCursor.movePosition(QTextCursor::Start);
    this->setTextCursor(firstBlackCursor);
}

void EbTextBrowser::moveTextBrowserToEnd()
{
    const QTextBlock lastBlack = this->document()->lastBlock();
    QTextCursor lastBlackCursor(lastBlack);
    lastBlackCursor.movePosition(QTextCursor::End);
    this->setTextCursor(lastBlackCursor);
}

void EbTextBrowser::addLineString(eb::bigint msgId, const QString &sText)
{
    moveTextBrowserToEnd();
    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(Qt::AlignCenter);
//    blockFmt.setTopMargin(2.0);
//    blockFmt.setLeftMargin(10.0);
//    blockFmt.setRightMargin(10.0);
    this->textCursor().insertBlock(blockFmt);
    updateBlockMsgId(msgId);
    const QString sTipText = QString("<font size=\"3\" color=#c0c0c0>%1</font>").arg(sText);
    this->insertHtml(sTipText);
}

void EbTextBrowser::updateMsgText(eb::bigint msgId, const QString &newText, int blockFrom, int blockTo)
{
    QTextDocument *document = this->document();
    if (document->isEmpty()) {
        return;
    }

    int findMsgBlock = -1;
    QTextBlock currentBlock = document->begin();
    while( true) {
        const EbTextBlockUserData * userData = (EbTextBlockUserData*)currentBlock.userData();
        const qint64 id = userData!=0?userData->msgId():0;
        if ( userData!=0 && userData->msgId()==msgId ) {
            if (findMsgBlock==-1) {
                findMsgBlock = 0;
            }
            else {
                findMsgBlock++;
            }
        }
        else if (findMsgBlock>=0) {
            /// 已经处理完成
            break;
        }
        if ( findMsgBlock>=0 ) {
            /// 当前 block msgid 是要处理的 block
            if (findMsgBlock==blockFrom) {
                /// udpate block text
                QTextCursor cursor(currentBlock);
                cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
                this->setTextCursor(cursor);
                this->insertHtml(newText);
            }
            else if ( findMsgBlock>blockFrom && (blockTo==-1 || findMsgBlock<=blockTo) ) {
                /// delete block
                QTextBlock deleteBlock = currentBlock;
                currentBlock = currentBlock.next();
                QTextCursor cursor(deleteBlock);
                if ( !cursor.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor) ) {
                    cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
                }
                cursor.removeSelectedText();
                if ( !currentBlock.isValid() ) {
                    break;
                }
                continue;
            }
        }
        currentBlock = currentBlock.next();
        if ( !currentBlock.isValid() ) {
            break;
        }
    }
}

void EbTextBrowser::setMsgReceiptFlag(eb::bigint msgId, int /*receiptFlag*/)
{
    QTextDocument *document = this->document();
    if (document->isEmpty()) {
        return;
    }

    int findMsgBlock = -1;
    const int blockFrom = 0;
    QTextBlock currentBlock = document->begin();
    while( true) {
        const EbTextBlockUserData * userData = (EbTextBlockUserData*)currentBlock.userData();
        const qint64 id = userData!=0?userData->msgId():0;
        if ( userData!=0 && userData->msgId()==msgId ) {
            if (findMsgBlock==-1) {
                findMsgBlock = 0;
            }
            else {
                findMsgBlock++;
            }
        }
        else if (findMsgBlock>=0) {
            /// 已经处理完成
            break;
        }
        if ( findMsgBlock>=0 ) {
            /// 当前 block msgid 是要处理的 block
            if (findMsgBlock==blockFrom) {
                /// udpate block text color
                QTextBlock::iterator it = currentBlock.begin();
                if ( !it.atEnd() ) {
                    ///
                    /// my title: RGB(0, 110, 254)=#006efe

                    QTextFragment currentFragment = it.fragment();
                    const int length = currentFragment.length();
//                    const QString text = currentFragment.text();
                    QTextCharFormat charFormat = currentFragment.charFormat();
                    charFormat.setForeground( QBrush(QColor(0,110,254)) );
                    QTextCursor cursor(currentBlock);
                    cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,length);
//                    cursor.movePosition(QTextCursor::NextCell,QTextCursor::KeepAnchor);
                    cursor.mergeCharFormat(charFormat);
                }
                return;
            }
        }
        currentBlock = currentBlock.next();
        if ( !currentBlock.isValid() ) {
            break;
        }
    }
}

void EbTextBrowser::onAnchorClicked(const QUrl &url)
{
    const QString scheme = url.scheme();
    if (scheme==theViewEcard) {
        const QString ecardData = url.url().mid( scheme.length()+4 );
        tstring sCardInfo;
        int nCardType = 0;
        theApp->m_ebum.EB_ParseCardInfo(ecardData.toStdString(),nCardType,sCardInfo);
        EB_ECardInfo pUserECard;
        if (nCardType==1 && theApp->m_ebum.EB_GetUserECardByFromInfo(sCardInfo,&pUserECard)) {
            const QPoint pos = cursor().pos();
            const QRect rectValid( pos.x()-12,pos.y()-12,24,24 );
            EB_MemberInfo pMemberInfo;
            EB_GroupInfo pGroupInfo;
            if ( pUserECard.m_sMemberCode>0 &&
                 theApp->m_ebum.EB_GetMemberInfoByMemberCode(&pMemberInfo,&pGroupInfo,pUserECard.m_sMemberCode) ) {
                theApp->dialgoViewECard(this,rectValid,true)->setMemberInfo(&pMemberInfo,&pGroupInfo);
            }
            else if ( pUserECard.m_nMemberUserId>0 &&
                      theApp->m_ebum.EB_GetMemberInfoByUserId2(&pMemberInfo,&pGroupInfo,pUserECard.m_nMemberUserId) ) {
                theApp->dialgoViewECard(this,rectValid,true)->setMemberInfo(&pMemberInfo,&pGroupInfo);
            }
            else {
                EB_ContactInfo pContactInfo;
                pContactInfo.m_nContactUserId = pUserECard.m_nMemberUserId;
                pContactInfo.m_sName = pUserECard.m_sAccountName;
                pContactInfo.m_sCompany = pUserECard.m_sEnterprise;
                pContactInfo.m_sGroupName = pUserECard.m_sGroupName;
                pContactInfo.m_sJobTitle = pUserECard.m_sTitle;
                pContactInfo.m_sPhone = pUserECard.m_sPhone;
                pContactInfo.m_sTel = pUserECard.m_sTel;
                pContactInfo.m_sEmail = pUserECard.m_sEmail;
                pContactInfo.m_sAddress = pUserECard.m_sAddress;
                theApp->dialgoViewECard(this,rectValid,true)->setContactInfo(&pContactInfo);
            }
        }
    }
    else if (scheme==theOpenSubId) {
        QString info = url.url().mid(scheme.length()+3);
        if (info.left(1)=="/") {
            /// 新版本 xxx:///
            info = info.mid(1);
        }
        /// 1002300105=群共享应用
        if (info.left(10).toLongLong()==constGroupShareSubId) {
            emit openSubId(constGroupShareSubId);
        }
        else {
            EbWebEngineUrlSchemeHandler::instance()->requestUrl(url);
        }
    }
    else {
        EbWebEngineUrlSchemeHandler::instance()->requestUrl(url);
    }
}

void EbTextBrowser::onDeleteMessage(qint64 msgId)
{
    QTextDocument *document = this->document();
    if (document->isEmpty()) {
        return;
    }

    QTextBlock currentBlock = document->begin();
    while( true) {
        const EbTextBlockUserData * userData = (EbTextBlockUserData*)currentBlock.userData();
        if ( userData!=0 && userData->msgId()==msgId ) {
            QTextBlock deleteBlock = currentBlock;
            currentBlock = currentBlock.next();
            QTextCursor cursor(deleteBlock);
            if ( !cursor.movePosition(QTextCursor::NextBlock,QTextCursor::KeepAnchor) ) {
                cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
            }
            cursor.removeSelectedText();
        }
        else {
            currentBlock = currentBlock.next();
        }
        if ( !currentBlock.isValid() ) {
            break;
        }
    }
}

//void EbTextBrowser::onCopyAvailable(bool b)
//{
//    int i=0;
//}

void EbTextBrowser::findBlockFromPosition(const QPoint & pos)
{
    if ( m_chatToolBar->isVisible() ) {
        return;
    }

    QTextDocument *document = this->document();
    if (document->isEmpty()) {
        return;
    }
    {
        /// 判断是否移动有选择 text
        QTextCursor cursor = this->textCursor();
        if ( cursor.hasSelection() ) {
//            QTextCursor cursorPos = this->cursorForPosition(pos);
////            cursorPos.selectedText()
//            this->setTextCursor(cursorPos);
//            if (this->textCursor().hasSelection()) {
////            if (cursorPos.hasSelection()) {
//                int i=0;
//            }
////            QString anchorAtString = this->anchorAt(pos);
//            const int cursorPos2 = cursorPos.position();
////            QTextBlock::contains()

            const QRect rect = cursorRect();
            const int clientWidth = width();
            const QString selectedText = cursor.selectedText();
            const int textWidth = this->fontMetrics().width(selectedText);

            /// 第一行选择文本区域
            const QRect rectLine1( MAX(0,rect.left()-textWidth),rect.top(),MIN(textWidth,rect.x()),rect.height()+1 );     /// 光标在后面，往前移动
            const QRect rectLine2( rect.left(),rect.top(),MIN(textWidth,clientWidth-rect.x()),rect.height()+1 );          /// 光标在前面，往后移动
            if ( rectLine1.contains(pos) || rectLine2.contains(pos) ) {
                EbTextBlockUserData * userData = (EbTextBlockUserData*)cursor.block().userData();
                if ( userData!=0 ) {
                    const QPoint pos2 = m_chatToolBar->parentWidget()->mapFromGlobal( this->mapToGlobal(pos) );
                    m_chatToolBar->updateMsgType( pos2, userData->msgId(), EbFrameChatToolBar::SelectedText, QVariant(selectedText) );
                }
                return;
            }
            if (textWidth>=clientWidth || (rect.x()+textWidth)>=clientWidth) {
                /// 计算多行选择文本区域
                /// * 先处理光标在后面，住前移动，需要从下往上计算 RECT 区域
                QRect rectSelectedText(rectLine1);
                int countWidth = rect.x();
                while ( countWidth<textWidth ) {
                    const int countAdd = MIN(textWidth-countWidth,clientWidth);
                    countWidth += countAdd;
                    if (countAdd>=clientWidth) {
                        rectSelectedText.setX( 0 );
                    }
                    else {
                        rectSelectedText.setX( clientWidth);
//                        rectSelectedText.setX( clientWidth-countAdd );    /// 使用这个，可能会有偏差
                    }
                    rectSelectedText.setRight( clientWidth );
                    rectSelectedText.setBottom( rectSelectedText.top() );
                    rectSelectedText.setTop( rectSelectedText.bottom()-rect.height() );
                    if ( rectSelectedText.contains(pos) ) {
                        EbTextBlockUserData * userData = (EbTextBlockUserData*)cursor.block().userData();
                        if ( userData!=0 ) {
                            const QPoint pos2 = m_chatToolBar->parentWidget()->mapFromGlobal( this->mapToGlobal(pos) );
                            m_chatToolBar->updateMsgType( pos2, userData->msgId(), EbFrameChatToolBar::SelectedText, QVariant(selectedText) );
                        }
                        return;
                    }
                }
                /// * 再处理光标在前面，住后移动，需要从上往下计算 RECT 区域
                rectSelectedText = QRect(rectLine2);
                countWidth = clientWidth-rect.x();
                while ( countWidth<textWidth ) {
                    const int countAdd = MIN(textWidth-countWidth,clientWidth);
                    countWidth += countAdd;
                    rectSelectedText.setX( 0 );
                    if (countAdd>=clientWidth) {
                        rectSelectedText.setRight( clientWidth );
                    }
                    else {
                        rectSelectedText.setRight( clientWidth );
//                        rectSelectedText.setRight( countAdd );    /// 使用这个，本端向下右对齐，会有偏差，选择不正确
                    }
                    rectSelectedText.setTop( rectSelectedText.bottom() );
                    rectSelectedText.setBottom( rectSelectedText.bottom()+rect.height() );
                    if ( rectSelectedText.contains(pos) ) {
                        EbTextBlockUserData * userData = (EbTextBlockUserData*)cursor.block().userData();
                        if ( userData!=0 ) {
                            const QPoint pos2 = m_chatToolBar->parentWidget()->mapFromGlobal( this->mapToGlobal(pos) );
                            m_chatToolBar->updateMsgType( pos2, userData->msgId(), EbFrameChatToolBar::SelectedText, QVariant(selectedText) );
                        }
                        return;
                    }
                }
            }

//            QRect rect1(rect);  /// 选择光标在后面，Left 往前移
//            rect1.setX( rect.x()-textWidth );
//            QRect rect2(rect);  /// 选择光标在前面，Right 往后移
//            rect2.setRight( rect.right()+textWidth );
//            if ( rect1.contains(pos) || rect2.contains(pos) ) {
//                EbTextBlockUserData * userData = (EbTextBlockUserData*)cursor.block().userData();
//                if ( userData!=0 ) {
//                    const QPoint pos2 = m_chatToolBar->parentWidget()->mapFromGlobal( this->mapToGlobal(pos) );
//                    m_chatToolBar->updateMsgType( pos2, userData->msgId(), EbFrameChatToolBar::SelectedText, QVariant(selectedText) );
//                }
//                return;
//            }
        }
    }

    QTextBlock currentBlock = document->begin();
    QTextBlock::iterator it;
    while( true) {
        const QTextCursor cursor(currentBlock);
        QRect rect = cursorRect(cursor);
        if ( rect.top()>height() ) {
            /// 当前 block 在显示区域下方，直接退出
            break;
        }
        else if (rect.bottom()<0) {
            ///
            QTextBlock nextBlock = currentBlock.next();
            if ( nextBlock.isValid() ) {
                ///
                const QTextCursor nextBlockCursor(nextBlock);
                const QRect rectNextBlock = cursorRect(nextBlockCursor);
                if (rectNextBlock.top()<=0 ) {
                    /// 当前 block 在显示区域的上方，处理下一个
                    currentBlock = nextBlock;
                    continue;
                }
            }
        }

        /// 判断鼠标指向用户 title 头像
        const EbTextBlockUserData * userData = (EbTextBlockUserData*)currentBlock.userData();
        if ( userData!=0 && userData->isTitleMessage() ) {
            const QString titleText = currentBlock.text();
            const int textWidth = this->fontMetrics().width(titleText);
            rect.setWidth(textWidth);
            if ( rect.contains(pos) ) {
                const QPoint pos2 = m_chatToolBar->parentWidget()->mapFromGlobal( this->mapToGlobal(pos) );
                m_chatToolBar->updateMsgType( pos2, userData->msgId(), EbFrameChatToolBar::PointUserHead, QVariant(userData->userId()) );
                return;
            }
        }

//        if ( cursor.hasSelection() ) {
//            const QString selectedText = cursor.selectedText();
//            const int textWidth = this->fontMetrics().width(selectedText);
//            rect.setWidth(textWidth);
//            if ( rect.contains(pos) ) {
//                return currentBlock;
//            }
//        }

//        const QString blockText = currentBlock.text();
//        const int textWidth = this->fontMetrics().width(blockText);

//        QTextLayout * layout = currentBlock.layout();
//        const QRectF rectf = layout->boundingRect();
//        const int position = currentBlock.position();
//        const int length = currentBlock.length();
//        rect.setWidth(textWidth);
//        const QPoint posRect = rect.topLeft();
//        if ( rect.contains(pos) ) {
//            return currentBlock;
//        }

        /// 在修改chatformat时会改变当前Block的fragment
        for ( it=currentBlock.begin(); !(it.atEnd()); it++) {

            QTextFragment currentFragment = it.fragment();
//            const int position = currentFragment.position();
//            const int length = currentFragment.length();

            QTextImageFormat newImageFormat = currentFragment.charFormat().toImageFormat();
            if (newImageFormat.isValid()) {

                const QString sResourceCmInfo = newImageFormat.name();
                const QVariant imageResource = document->resource( QTextDocument::ImageResource, sResourceCmInfo );
                if ( imageResource.isValid() ) {

                    const QVariant::Type imageResourceType = imageResource.type();
                    switch (imageResourceType) {
                    case QVariant::Pixmap: {
                        const QPixmap pixmap = imageResource.value<QPixmap>();
                        rect.setWidth( pixmap.width() );
                        rect.setHeight( pixmap.height() );
                        break;
                    }
                    case QVariant::Image: {
                        const QImage image = imageResource.value<QImage>();
                        rect.setWidth( image.width() );
                        rect.setHeight( image.height() );
                        break;
                    }
//                    case QVariant::Icon: {
//                        const QIcon image = imageResource.value<QIcon>();
//                        rect.setWidth( image.width() );
//                        rect.setHeight( image.height() );
//                        break;
//                    }
                    case QVariant::Bitmap: {
                        const QBitmap bitmap = imageResource.value<QBitmap>();
                        rect.setWidth( bitmap.width() );
                        rect.setHeight( bitmap.height() );
                        break;
                    }
                    default:
                        break;
                    }
                    if ( rect.contains(pos) ) {
                        /// 鼠标定位到图片上
                        if ( userData!=0 ) {
                            const QPoint pos2 = m_chatToolBar->parentWidget()->mapFromGlobal( this->mapToGlobal(pos) );
                            m_chatToolBar->updateMsgType( pos2, userData->msgId(), EbFrameChatToolBar::PointImage, QVariant(sResourceCmInfo), imageResource  );
                        }
                        return;
                    }

                }


            }
            else {
                /// 普通文本
                if ( userData!=0 && userData->isFileMessage() && !userData->filePath().isEmpty() ) {
                    const QString fileName = currentFragment.text();
                    const int textWidth = this->fontMetrics().width(fileName);
                    rect.setWidth(textWidth);
                    if ( rect.contains(pos) ) {
                        const QPoint pos2 = m_chatToolBar->parentWidget()->mapFromGlobal( this->mapToGlobal(pos) );
                        m_chatToolBar->updateMsgType( pos2, userData->msgId(), EbFrameChatToolBar::PointFile, QVariant(userData->filePath()) );
                        return;
                    }
                }
            }


//            const QTextCursor cursor(it);
//            const QRect rect = cursorRect(cursor);
//            if ( rect.contains(pos) ) {
//                const QString s = it.text();
//                return it;
//            }
        }

        currentBlock = currentBlock.next();
        if ( !currentBlock.isValid() ) {
            break;
        }
    }
}


void EbTextBrowser::mouseMoveEvent(QMouseEvent *e)
{
    if (m_timerIdFindTextBlockFromPosition!=0) {
        this->killTimer(m_timerIdFindTextBlockFromPosition);
    }
    m_timerIdFindTextBlockFromPosition = this->startTimer(600);
    QTextBrowser::mouseMoveEvent(e);
}

void EbTextBrowser::timerEvent(QTimerEvent *e)
{
    if ( m_timerIdFindTextBlockFromPosition!=0 && m_timerIdFindTextBlockFromPosition==e->timerId() ) {
        this->killTimer(m_timerIdFindTextBlockFromPosition);
        m_timerIdFindTextBlockFromPosition = 0;
        const QPoint pos = cursor().pos();
        findBlockFromPosition( this->mapFromGlobal(pos) );
    }

    QTextBrowser::timerEvent(e);
}

//void EbTextBrowser::drawBlock()
//{
//    QTextDocument *document = this->document();
//    if (document->isEmpty()) {
//        return;
//    }

////    QPainter painter(this);
////    /// 绘制边框
////    painter.setPen(QColor(128,128,128));

//    QTextBlock currentBlock = document->begin();
//    while( true) {
//        EbTextBlockUserData * userData = (EbTextBlockUserData*)currentBlock.userData();
//        if ( userData!=0 && userData->isChatMessage() ) {
//            const QRectF rectf = currentBlock.layout()->boundingRect();
////            QTextCursor cursor(currentBlock);
////            QRect rect = this->cursorRect(cursor);
////            rect.setWidth( (int)rectf.width() );
////            rect.setHeight( (int)rectf.height() );

//            QTextCursor cursor(currentBlock);
//            const QRect rect1 = this->cursorRect(cursor);
//            cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::MoveAnchor);
//            const QRect rect2 = this->cursorRect(cursor);
//            QRect rect( rect1.topLeft(), rect2.bottomRight() );
////            rect.setWidth( (int)rectf.width() );
////            rect.setHeight( (int)rectf.height() );
//            rect.setWidth( (int)200 );
//            rect.setHeight( (int)20 );

////            if (rect.bottom()>0 ) {
//            userData->createWidget(this);
////                userData->createWidget(this->parentWidget());
////                const QPoint posWidget = this->mapToParent(rect.topLeft());
////                rect.setTopLeft(posWidget);
////                rect.setWidth( (int)rectf.width() );
////                rect.setHeight( (int)rectf.height() );
//                userData->setGeometry(rect);

////                painter.drawLine(rect.left(), rect.top(), rect.width() - 1, 0);
////                painter.drawLine(rect.width() - 1, rect.top(), rect.width() - 1, rect.height() - 1);
////                painter.drawLine(rect.width() - 1, rect.height() - 1,rect.left(), rect.height() - 1);
////                painter.drawLine(rect.left(), rect.height() - 1, rect.left(), rect.top());
////                currentBlock.layout()->draw(&painter,QPointF(10.0,10.0));
////            }
//        }

//        currentBlock = currentBlock.next();
//        if ( !currentBlock.isValid() ) {
//            break;
//        }
//    }
//}

//void EbTextBrowser::paintEvent(QPaintEvent *e)
//{
//    drawBlock();
//    QTextBrowser::paintEvent(e);
//}

EbTextBlockUserData *EbTextBrowser::updateBlockMsgId(eb::bigint msgId)
{
    EbTextBlockUserData * userData = (EbTextBlockUserData*)this->textCursor().block().userData();
    if (userData==0) {
        userData = new EbTextBlockUserData(msgId);
        this->textCursor().block().setUserData( userData );
    }
    else {
        userData->setMsgId(msgId);
    }
    return userData;
}

void EbTextBrowser::loadHistoryMsg(int loadLastCount)
{
    char sql[512];
    if (m_callInfo->isGroupCall()) {
        sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t "
                      "WHERE dep_code=%lld ORDER BY msg_time DESC LIMIT %d",m_callInfo->groupId(),loadLastCount);
    }
    else if (m_callInfo->fromUserId()>0) {
        sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t "
                      "WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) ORDER BY msg_time DESC LIMIT %d",
                 m_callInfo->fromUserId(),m_callInfo->fromUserId(),loadLastCount);
    }
    else {
        return;
    }
    loadMsgRecord(sql,true);
}

void EbTextBrowser::loadMsgRecord(const char *sql, bool desc)
{
    this->clear();

    //    m_nLastMsgId = 0;
    //    m_nFromUid = 0;
    //    m_nToUid = 0;
    //CString sMsgTime;
    //time_t nLocalMsgTime = 0;
    tstring sToWriteString;
    bool bReceive = false;
    int nCookie = 0;
    mycp::bigint nLastWithdrawMsgId = 0;
    const mycp::bigint nRet = theApp->m_sqliteUser->select(sql, nCookie);
    cgcValueInfo::pointer pRecord = desc?theApp->m_sqliteUser->last(nCookie):theApp->m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        const tstring sMsgTime(pRecord->getVector()[0]->getStrValue());
        const eb::bigint msgId = pRecord->getVector()[1]->getBigIntValue();
//        const tstring soffTime(pRecord->getVector()[2]->getStrValue());
        const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
        const tstring sFromName(pRecord->getVector()[4]->getStrValue());
        const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
        const tstring sToName(pRecord->getVector()[6]->getStrValue());
        const int nPrivate = pRecord->getVector()[7]->getIntValue();
        const int nMsgType = pRecord->getVector()[8]->getIntValue();
        const tstring sMsgName(pRecord->getVector()[9]->getStrValue() );
        const tstring sMsgText(pRecord->getVector()[10]->getStrValue() );
        //theApp.m_pBoUsers->escape_string_out(sMsgName);
        //theApp.m_pBoUsers->escape_string_out(sMsgText);
        const unsigned int nMsgSize = sMsgText.size();
        const int nReadFlag = pRecord->getVector()[11]->getIntValue();
        pRecord = desc?theApp->m_sqliteUser->previous(nCookie):theApp->m_sqliteUser->next(nCookie);

        /// 前面未显示，并且当前文本不是最长文本，显示前面内容。
        if (!sToWriteString.empty() && nMsgSize!=theMaxDatabaseTextSize) {
            this->insertPlainTextEb( QString::fromStdString(sToWriteString.string()),QColor(64,64,64) );
            sToWriteString.clear();
        }
        bReceive = sFromAccount!=theApp->logonUserId()?true:false;
        //// 兼容旧版本
        //if (sFromName.empty())
        //	sFromName = sFromAccount;
        //if (sToName.empty())
        //	sToName = sToAccount;
        time_t nMsgTime = 0;
        libEbc::ChangeTime(sMsgTime.c_str(),nMsgTime);
        const bool writeLeft = bReceive;
        writeTitle( writeLeft,msgId,nPrivate==1,sFromAccount,sFromName,sToAccount,sToName,nMsgTime,nReadFlag );
        addChatMsgBlock( msgId,bReceive );
        if ((nReadFlag&EBC_READ_FLAG_WITHDRAW)==EBC_READ_FLAG_WITHDRAW) {
            if (nLastWithdrawMsgId!=msgId) {
                nLastWithdrawMsgId = msgId;
                /// [撤回一条消息]
                const QString text = theLocales.getLocalText("withdraw-msg-response.state-ok-update","");
                const QString newText = QString("<font color=#808080>%1</font>").arg(text);
                this->insertHtml( newText );
            }
            continue;
        }
        nLastWithdrawMsgId = 0;
//        QString sWindowText;
        switch (nMsgType) {
        case MRT_UNKNOWN: {
            break;
        }
        case MRT_TEXT: {
            /// 先临时保存，后面显示；处理分段保存长文本；
            if (sToWriteString.empty())
                sToWriteString = sMsgText;
            else
                sToWriteString = sMsgText+sToWriteString;
            break;
        }
        case MRT_JPG: {
            /// 图片
            QString sTemp = QString("<img src=\"%1\" alt=\"%2\"/>").arg(sMsgName.c_str()).arg(sMsgText.c_str());
            this->insertHtml(sTemp);
            break;
        }
        case MRT_MAP_POS:
            /// 地图位置
            break;
        case MRT_CARD_INFO:
            /// 名片信息
            writeCardDataMessage( bReceive,msgId, sMsgText.c_str() );
            break;
        case MRT_USER_DATA:
            /// 用户自定义数据
            break;
        case MRT_WAV:
            /// 语音消息
            writeVoiceMessage(sMsgName.c_str());
            break;
        case MRT_RESOURCE: {
            /// 资源文件
            const std::string::size_type nFind = sMsgText.find(",");
            if (nFind == std::string::npos) {
                break;
            }
            const QString sFileName = sMsgName.c_str();
            const mycp::bigint nResourceId = cgc_atoi64(sMsgText.substr(0,nFind).c_str());
            const mycp::bigint nFileSize = cgc_atoi64(sMsgText.substr(nFind+1).c_str());
            writeFileMessage(msgId, nResourceId, sFileName, nFileSize);
            break;
        }
        case MRT_FILE: {
            /// 文件
            const QString sFileName = sMsgText.c_str();
            writeFileMessage(msgId, 0, sFileName, 0);
            break;
        }
        default:
            break;
        }
    }
    theApp->m_sqliteUser->reset(nCookie);

    if (!sToWriteString.empty()) {
        this->insertPlainTextEb( QString::fromStdString(sToWriteString.string()),QColor(64,64,64) );
        sToWriteString.clear();
    }

    if (nRet > 0) {
        /// -------- 以上是历史消息  --------
        addLineString( 0,theLocales.getLocalText( "chat-msg-text.history-msg-separate","History Message" ) );
        moveTextBrowserToEnd();
    }

}

void EbTextBrowser::updateMsgReceiptData(eb::bigint msgId, eb::bigint fromUserId, int ackType, EB_STATE_CODE state)
{
    /// nAckType=0 收到消息回执
    /// nAckType=4 请求撤回消息
    /// nAckType=6 请求“个人收藏”消息
    /// nAckType=7 请求“群收藏”消息
    /// *** read_flag=1 已经读；
    /// *** read_flag=2 对方接收回执
    if (state==EB_STATE_OK)
        theApp->updateMsgReceiptData(msgId, fromUserId, ackType);

    if (ackType==6 ||	/// 个人收藏
            ackType==7) {			/// 群收藏
        QString text;
        EbMessageBox::IMAGE_TYPE imageType = EbMessageBox::IMAGE_WARNING;
        if (state==EB_STATE_OK) {   /// 消息收藏成功！
            imageType = EbMessageBox::IMAGE_INFORMATION;
            text = theLocales.getLocalText("message-show.collect-msg-ok","");
        }
        else if (state==EB_STATE_NOT_AUTH_ERROR)   /// 没有操作权限：\r\n消息收藏失败！
            text = theLocales.getLocalText("message-show.collect-msg-not-auth","");
        else    /// 消息不存在或者已经被删除：\r\n消息收藏失败！
            text = theLocales.getLocalText("message-show.collect-msg-error","");
        if ( !text.isEmpty() ) {
            text.replace( "[STATE_CODE]", QString::number((int)state) );
            EbMessageBox::doShow( NULL, "", QChar::Null, text, imageType, default_warning_auto_close );
        }
    }
    else if (ackType==4) {
        /// 请求撤回消息
        if (state==EB_STATE_OK) {
            /// 1: 表示只更新第1行，第0行是标题，不更新；
            /// [撤回一条消息]
            const QString text = theLocales.getLocalText("withdraw-msg-response.state-ok-update","");
            const QString newText = QString("<font color=#808080>%1</font>").arg(text);
            this->updateMsgText(msgId,newText,1);
//            m_pMrFrameInterface->SetMsgReceiptFlag(nMsgId, EBC_CONTRON_RECEIPT_FLAG_HIDE);
        }
        if ( fromUserId==theApp->logonUserId() ) {
            if (state!=EB_STATE_OK) {
                /// 发送时间超过2分钟的消息：<br>不能撤回！
                QString text = theLocales.getLocalText("withdraw-msg-response.other-error.text","");
                if ( !text.isEmpty() ) {
                    text.replace( "[STATE_CODE]", QString::number((int)state) );
                    EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING, default_warning_auto_close );
                }
            }
            else {
                /// 请求撤回了一条消息！
                this->addLineString(0, theLocales.getLocalText("withdraw-msg-response.state-ok","") );
            }
        }
        else if (m_callInfo->groupId()==0) {
            /// 对方撤回了一条消息！
            this->addLineString(0, theLocales.getLocalText("withdraw-msg-response.msg-receipt-dest","") );
        }
        else {
            /// %s 撤回了一条消息！
            tstring sMemberUserName;
            theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),fromUserId,sMemberUserName);
            QString text = theLocales.getLocalText("withdraw-msg-response.msg-receipt-group","");
            text.replace( "[USER_NAME]",sMemberUserName.c_str() );
            this->addLineString(0,text);
        }
    }
    else if (ackType==0) {	/// ?
        const long nReceiptFlag = EBC_CONTRON_RECEIPT_FLAG_TRUE|EBC_CONTRON_RECEIPT_FLAG_SHOW;
        this->setMsgReceiptFlag(msgId, nReceiptFlag);
    }
}

void EbTextBrowser::getFromToName(bool bReceive,eb::bigint fromUserId, eb::bigint toUserId,
                                  cgcSmartString &pOutFromName, cgcSmartString &pOutToUserName)
{
    if (bReceive) {
        if (m_callInfo->m_pCallInfo.m_sGroupCode==0) {
            if (m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_VISITOR) {
                const std::string visitorText = theLocales.getLocalStdString("chat-msg-text.visitor","Visitor");
                pOutFromName = visitorText + m_callInfo->m_pFromAccountInfo.GetAccount();
            }
            else
                pOutFromName = m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_sName;
            pOutToUserName = theApp->m_ebum.EB_GetUserName();
//            if (m_pPanelStatus!=NULL && m_pPanelStatus->IsWindowVisible())
//            {
//                KillTimer(TIMERID_HIDE_NOTIFY1);
//                SetTimer(TIMERID_HIDE_NOTIFY1,1000,NULL);
//            }
        }
        else {
            theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),fromUserId,pOutFromName);
            if (toUserId>0) {
                theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),toUserId,pOutToUserName);
            }
        }
    }
    else {
        if (m_callInfo->isGroupCall()) {
            theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),theApp->logonUserId(),pOutFromName);
        }
        else {
            pOutFromName = theApp->m_ebum.EB_GetUserName();
        }
        if (toUserId>0) {
            theApp->m_ebum.EB_GetMemberNameByUserId(m_callInfo->groupId(),toUserId,pOutToUserName);
        }
    }

}

void EbTextBrowser::writeTitle(bool writeLeft,eb::bigint msgId, bool bPrivate, eb::bigint nFromUid, const cgcSmartString &sFromName,
                               eb::bigint nToUid, const cgcSmartString &/*sToName*/, time_t tMsgTime, int nReadFlag,
                               QString * pOutWindowText)
{
    writeMsgDate(tMsgTime);
    const bool bReceive = nFromUid!=theApp->logonUserId()?true:false;
    QString sPrivateText;
    QString sToText;
    if (m_callInfo->isGroupCall() && nToUid>0) {
        if (bReceive) {
            //if (pCrMsgInfo->m_sSendTo>0)
            {
                if (bPrivate)
                    sPrivateText = "[私聊]";
//                if (pCrMsgInfo->m_sSendTo != theApp->m_ebum.EB_GetLogonUserId())
//                    sToText = QString("对%1说").arg(sToName.c_str());
//                else
//                    sToText = "对你说";
            }
        }
        else {
            //if (m_callInfo->m_pCallInfo.m_sGroupCode>0 && pCrMsgInfo->m_sSendTo>0)
            {
//                sToText = QString("对%1说").arg(sToName.c_str());
//                if (pCrMsgInfo->m_bPrivate)
//                    sPrivateText = "[私聊]";
            }
        }
    }


    //const COLORREF crTextColor = bReceive?RGB(0, 110, 254):RGB(66, 180, 117);
    QString sWindowText;
    sWindowText = QString("%1%2%3").arg(sPrivateText).arg(sFromName.c_str()).arg(sToText);
    if (pOutWindowText!=0) {
        *pOutWindowText = sWindowText;
    }
    /// user title: RGB(66, 180, 117)=#42b475
    /// my title: RGB(0, 110, 254)=#006efe
    /// 发送/发送中: RGB(237, 28, 36)=#ed1c24
    const QString crTextColor = writeLeft?"#42b475":( (!m_callInfo->isGroupCall() && (nReadFlag&EBC_READ_FLAG_RECEIPT)==0)?"#ed1c24":"#006efe" );

    {
        /// 光标移到最后面
        moveTextBrowserToEnd();
        /// 标题 block
        QTextBlockFormat blockFmt;
//        blockFmt.setProperty( QTextFormat::UserProperty, QVariant(msgId) );
        blockFmt.setAlignment(writeLeft?Qt::AlignLeft:Qt::AlignRight);
        blockFmt.setTopMargin(5.0);
        blockFmt.setLeftMargin(5.0);
        blockFmt.setRightMargin(5.0);
        blockFmt.setBottomMargin(5.0);
//        blockFmt.setBackground(QBrush(QColor(200,200,200)));  /// 可以刷新背景
        this->textCursor().insertBlock(blockFmt);
        EbTextBlockUserData * userData = updateBlockMsgId(msgId);
        userData->setUserId(nFromUid);
        userData->setFlags( EbTextBlockUserData::TitleMessage );
        if ( bReceive ) {
            userData->setFlags( userData->flags()|EbTextBlockUserData::ReceiveMessage );
        }

    }

//    const QString sTitle = QString().arg(QChar(0xf00c));
//    this->insertHtml( "<font color=#c0c0c0>[未读] </font>" );
//    /// 蓝色 0,128,255=#0080ff
//    /// 红色 255,128,64=#ff8040
//    this->insertHtml("<font color=#0080ff>&nbsp;</font>");
////    this->insertHtml("<font color=#ff8040>&nbsp;</font>");
//    QTextCursor cursor = this->textCursor();
//    QTextBlockFormat blockFmt = cursor.blockFormat();
//    QTextListFormat listFmt;
//    if (cursor.currentList()) {
//        listFmt = cursor.currentList()->format();
//    } else {
//        listFmt.setIndent(blockFmt.indent() + 1);
//        blockFmt.setIndent(0);
//        cursor.setBlockFormat(blockFmt);
//    }
////    listFmt.setForeground( QBrush( QColor(0,128,255) ));
//    listFmt.setStyle(QTextListFormat::ListUpperRoman);  // I可以接受
////    listFmt.setNumberPrefix("I");
//    listFmt.setNumberSuffix("I");
//    cursor.createList(listFmt);

    const QDateTime pMsgDateTime = QDateTime::fromSecsSinceEpoch(tMsgTime);
    const QString sMsgHourSecond = pMsgDateTime.toString("HH:mm");
    if ( bReceive && m_callInfo->isGroupCall() ) {
        const QString sTitle = QString("%1<a href=\"ebim-call-account://%2\"><font color=%3>%4</font></a>%5 "
                                       "<font color=#c0c0c0>%6</font>")
                .arg(sPrivateText).arg(nFromUid).arg(crTextColor).arg(sFromName.c_str())
                .arg(sToText).arg(sMsgHourSecond);
        this->insertHtml(sTitle);
    }
    else {
        const QString sTitle = QString("%1<font color=%2>%3</font>%4 <font color=#c0c0c0>%5</font>")
                .arg(sPrivateText).arg(crTextColor).arg(sFromName.c_str())
                .arg(sToText).arg(sMsgHourSecond);
        this->insertHtml(sTitle);
    }

//    const long nReceiptFlag = (nReadFlag&EBC_READ_FLAG_RECEIPT)==EBC_READ_FLAG_RECEIPT?(EBC_CONTRON_RECEIPT_FLAG_SHOW|EBC_CONTRON_RECEIPT_FLAG_TRUE):EBC_CONTRON_RECEIPT_FLAG_SHOW;

}

void EbTextBrowser::writeFileMessage(eb::bigint msgId, eb::bigint resourceId, const QString &filePath,
                                     eb::bigint fileSizeOrg, bool showNameOnly, QString *pOutMsgText)
{
    moveTextBrowserToEnd();
    bool isImage = false;
    const eb::bigint fileSize = fileSizeOrg>0?fileSizeOrg:(eb::bigint)libEbc::GetFileSize(filePath);
    if (fileSize>0) {
        const QImage image(filePath);
        isImage = image.isNull()?false:true;
    }
    if (isImage) {
        QString sTemp = QString("<img src=\"%1\" alt=\"%2\"/><br>").arg(filePath).arg(filePath);
        this->insertHtml(sTemp);
    }
    else if (resourceId==0){
        QFileIconProvider icon_provider;
        QIcon icon = icon_provider.icon( QFileInfo(filePath) );
        this->document()->addResource(QTextDocument::ImageResource, QUrl(filePath), icon.pixmap(24,24));
        this->textCursor().insertImage(filePath);
//        QString sTemp = QString("<img src=\"%1\" alt=\"%2\"/><br>").arg(filePath).arg(filePath);
//        this->insertHtml(sTemp);
    }
    const tstring fileName = libEbc::GetFileName(filePath.toStdString());
    if (pOutMsgText!=0) {
        *pOutMsgText = fileName.c_str();
    }

    /// "上传群共享文件："
    char textTemp[128];
    memset(textTemp, 0, sizeof(textTemp));
    if (showNameOnly) {
        strcpy(textTemp, " : ");
    }
    else if (m_callInfo->isGroupCall() && resourceId>0) {
        sprintf( textTemp, "%s: ", theLocales.getLocalStdString("chat-msg-text.send-group-file","Send Group File").c_str() );
    }
    EbTextBlockUserData * userData = updateBlockMsgId(msgId);
    userData->setFlags( userData->flags()|EbTextBlockUserData::FileMessage );
    userData->setFilePath(filePath);
    char fileText[260];

    if (resourceId==0 && fileSize == -1)    /// 文件不存在
        sprintf(fileText,"%s%s (%s)",textTemp,fileName.c_str(),theLocales.getLocalStdString("chat-msg-text.file-not-exist","File not exist").c_str());
    else if (fileSize >= const_gb_size)
        sprintf(fileText,"%s%s (%.02fGB)",textTemp,fileName.c_str(),(double)fileSize/const_gb_size);
    else if (fileSize >= const_mb_size)
        sprintf(fileText,"%s%s (%.02fMB)",textTemp,fileName.c_str(),(double)fileSize/const_mb_size);
    else if (fileSize >= const_kb_size)
        sprintf(fileText,"%s%s (%.02fKB)",textTemp,fileName.c_str(),(double)fileSize/const_kb_size);
    else if (fileSize>=0)
        sprintf(fileText,"%s%s (%lldByte)",textTemp,fileName.c_str(),fileSize);
    else if (resourceId>0)
        sprintf(fileText,"%s%s",textTemp,fileName.c_str());
    if (resourceId>0) {
        this->insertPlainText(fileText);
    }
    else {
        const QString text = QString(" <a href=\"%1:///%2\">%3</a> ").arg(theOpenFile).arg(filePath).arg(fileText);
        this->insertHtml(text);
    }

    if (resourceId>0) {
        /// 下载
        const QString text = QString(" <a href=\"%1:///0,%2,%3\">%4</a> ")
                .arg(theDownloadResource).arg(resourceId).arg(fileName.c_str())
                .arg(theLocales.getLocalText("chat-msg-text.download","Download"));
        this->insertHtml(text);
        if (m_callInfo->isGroupCall() && !theApp->isDisableGroupShareCloud()) {
            /// 群共享
            const QString text = QString(" <a href=\"%1:///%2\">%3</a>")
                    .arg(theOpenSubId).arg(constGroupShareSubId)
                    .arg(theLocales.getLocalText("chat-msg-text.group-share","Group share"));
            this->insertHtml(text);
        }
    }

}

void EbTextBrowser::writeVoiceMessage(const QString &voiceFile,QString *pOutMsgText)
{
    const int nWavTimeLength = libEbc::GetWaveTimeLength(voiceFile);
    QString sText;
    if (nWavTimeLength>=0) {
        /// 语音消息
        sText = QString("%1 %2\"").arg(theLocales.getLocalText( "chat-msg-text.voice-msg","Voice Message" )).arg(nWavTimeLength);
    }
    else if (nWavTimeLength==-1) {
        /// 语音消息不存在
        sText = theLocales.getLocalText( "chat-msg-text.voice-msg-not-exist","Voice Message Not Exist" );
    }
    else {
        /// 语音消息格式错误
        sText = theLocales.getLocalText( "chat-msg-text.voice-msg-format-error","Voice Message Format Error" );
    }
    if (pOutMsgText!=0) {
        *pOutMsgText = sText;
    }
    const QString text = QString("<a href=\"%1:///%2\">%3</a>").arg(thePlayVoice).arg(voiceFile).arg(sText);
    this->insertHtml(text);
}

bool EbTextBrowser::writeCardDataMessage( bool bReceive,mycp::bigint msgId, const char *cardData, QString *pOutMsgText)
{
    int nCardType = 0;
    tstring sCardData;
    theApp->m_ebum.EB_ParseCardInfo(cardData,nCardType,sCardData);
    EB_ECardInfo pUserECard;
    if (nCardType==1 && theApp->m_ebum.EB_GetUserECardByFromInfo(sCardData,&pUserECard)) {
        /// ** 用户名片（个人名片）
//        m_cardInfoList.insert( msgId, sCardData.string(), false );
        const QString sUserEcardText = theLocales.getLocalText("chat-msg-text.user-ecard","User Ecard");
        const QString sUserEcardTip = QString("%1[%2]").arg(pUserECard.m_sAccountName.c_str()).arg(sUserEcardText);
        if (pOutMsgText!=0) {
            *pOutMsgText = sUserEcardTip;
        }
        /// 圆角矩形
//        {
//           width: 140px;
//           height: 100px;
//           background: #088cb7;
//           position: relative;
//           -moz-border-radius: 12px;
//           -webkit-border-radius: 12px;
//           border-radius: 12px;
//       }
        /// 圆角: border-radius:3px 4px 5px 6px
//        const QString line = "<div style=\"width: 140px;height: 100px;background: #088cb7;position: relative;border-radius: 12px;\"></div>";
//        -moz-border-radius: 12px;
//        -webkit-border-radius: 12px;
        /// 选择框，三角形
//width:0px;
//height:0px;
//border-width:5px;
//border-style:solid;
//border-color:rgb([EB_MAIN_RGB]) rgb(255,255,255) rgb(255,255,255) rgb(255,255,255);
        /// 不行
//        const QString line = "<div style=\"width:100px;height:10px;border-width:50px;border-style:solid;border-color:rgb(0,0,0); \">abc</div>";
//        const QString line = "<div style=\"margin:0;padding:0; width:100px;height:10px;border-top:2px solid rgb(0,0,0);border-style: inset;\">&nbsp;</div>";
//        const QString line = "<span style=\"display:inline-block;margin:0;padding:0; width:75px;height:1px;background-color:#000;overflow:hidden;\"></span>";
        /// NOSHADE
//        const QString line = "<p align=\"left\"><hr width=50 SIZE=5 /></p>"; /// 可以显示，但不能靠左显示
//        const QString line = "<div align=\"left\" text-align=\"left\" ><hr style=\"float:left;\" width=50 SIZE=5 />abc</div>"; /// 可以显示，但不能靠左显示
                //        const QString line = "<div style=\"width:80px;height:1px;\">"
                //                "<hr style=\"margin:0px;height:1px;border:0px;background-color:#D5D5D5;color:#D5D5D5;\"/></div>";
        /// <HR align=left width=50 SIZE=2>
//        const QString line = "<div style=\"margin:0;padding:0; width:80px;height:1px;background-color:#EFEFEF;overflow:hidden;margin-top: 15px;\"></div>";
        const QString sFilePath = theApp->userHeadFilePath(pUserECard.m_nMemberUserId,pUserECard.m_sMemberCode,"");
        const QString align = bReceive?"left":"right";
        const QString html = QString("<p align=\"%1\"><table width=\"200\" border=\"0\" style=\"margin-left:10px; margin-right:10px;\">"
                             "<tr>"
                             "<td width=\"36\"><a href=\"%2:///%3\" ><img src=\"%4\" width=\"48\" height=\"48\" /></a></td>"
                             "<td valign=\"middle\"><p align=\"left\"><span style=\" font-weight:600;\">%5</span></p></td>"
                             "</tr>"
                             "<tr>"
                             "<td colspan=\"2\"><p>--------------------</p></td>"
                             "</tr>"
                             "<tr>"
                             "<td colspan=\"2\"><p align=\"left\"><font color=#808080>%6</font></p></td>"
                             "</tr>"
                             "</table></p>")
                .arg(align)
                .arg(theViewEcard).arg(cardData)
                .arg(sFilePath).arg(pUserECard.m_sAccountName.c_str())
                .arg(sUserEcardText);
        this->insertHtml(html);
//        updateBlockMsgId(msgId);
//        <p align="justify" style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:16pt; font-weight:600;"><span style=" font-size:11pt; font-weight:400;">QTextEdit can arrange and format tables, supporting features such as row and column spans, text formatting within cells, and size constraints for columns. </span></p>
        /// padding-top:10px; padding-bottom:10px;
        /// 可以显示
//        const QString html2 = QString("<table border=\"1\" height=\"50\" style=\"margin-top:12px;background-color:#ffffff;\" >"
//                                      "<tr>"
//                                      "<td><p style=\" margin-left:5px; margin-right:5px;background-color:#ffffff;\">123</p></td>"
//                                      "</tr>"
//                                      "</table>");
////        const QString html2 = QString("<td><p align=\"justify\" style=\" border-style:solid;padding-top:10px; padding-bottom:10px;margin-top:12px; margin-bottom:12px; margin-left:5px; margin-right:5px;background-color:#ffffff;\">123</p></td>");
//        this->insertHtml(html2);
        return true;
    }
    return false;
}

void EbTextBrowser::addChatMsgBlock(eb::bigint msgId, bool alignLeft)
{
    moveTextBrowserToEnd();
    /// 内容 black
    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(alignLeft?Qt::AlignLeft:Qt::AlignRight);
    blockFmt.setTopMargin(2.0);
    blockFmt.setLeftMargin(15.0);
    blockFmt.setRightMargin(15.0);
    //        blockFmt.setForeground(QBrush(QColor(64,64,64)));
    //        blockFmt.setBackground(QBrush(QColor(200,200,200)));  // 可以刷新背景
    this->textCursor().insertBlock(blockFmt);
    EbTextBlockUserData * userData = updateBlockMsgId(msgId);
//    userData->setUserId(nFromUid);
    userData->setFlags( EbTextBlockUserData::ChatMessage );
    if ( alignLeft ) {
        userData->setFlags( userData->flags()|EbTextBlockUserData::ReceiveMessage );
    }
    //        this->insertHtml( "<font color=#c0c0c0>[成功] </font>" );
}


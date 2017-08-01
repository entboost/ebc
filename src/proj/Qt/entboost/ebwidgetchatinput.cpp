#include "ebwidgetchatinput.h"
#include "ebtextedit.h"
#include "ebiconhelper.h"

const QSize const_button_emotion_size(24,20);

EbWidgetChatInput::EbWidgetChatInput(const EbcCallInfo::pointer& pCallInfo,QWidget *parent)
    : QWidget(parent)
    , m_textEditInput(NULL)
    , m_pushButtonEmotion(NULL), m_pushButtonMsgRecord(NULL)
    , m_pushButtonClose(NULL), m_pushButtonSend(NULL), m_pushButtonSendSwitch(0)
    , m_menuSendSwitch(0)
    , m_actionSwitchSend0(0)
    , m_actionSwitchSend1(0)
    , m_callInfo(pCallInfo)
    , m_timerIdCheckForbid(0)
{
    assert(m_callInfo.get()!=0);
    m_textEditInput = new EbTextEdit(this);
    m_textEditInput->setStyleSheet("border: none;");
    connect( m_textEditInput,SIGNAL(dropSendFile(QString)),this,SLOT(onDropSendFileTextEdit(QString)) );
    connect( m_textEditInput,SIGNAL(requestSendText()),this,SLOT(onClickedPushButtonSend()) );
    m_labelInputForbidState = new QLabel(this);
    m_labelInputForbidState->setVisible(false);
    m_labelInputForbidState->setAlignment( Qt::AlignLeft|Qt::AlignTop );
    m_labelInputForbidState->setIndent(2);

    /// for test
//    m_menuTextEdit = m_textEditInput->createStandardContextMenu();
//    m_menuTextEdit->addSeparator();
//    const QString selectText = theLocales.getLocalText("color-skin.select-color.text","选择色调");
//    m_menuTextEdit->addAction( QIcon(QStringLiteral(":/res/color_select.bmp")), selectText );

    /// 表情 & 消息记录
    m_pushButtonEmotion = new QPushButton(this);
    m_pushButtonMsgRecord = new QPushButton(this);
    m_pushButtonEmotion->resize(const_button_emotion_size);
    m_pushButtonMsgRecord->resize(const_button_emotion_size);
    EbIconHelper::Instance()->SetIcon(m_pushButtonEmotion,QChar(0xf118),12);
    EbIconHelper::Instance()->SetIcon(m_pushButtonMsgRecord,QChar(0xf298),12);
    m_pushButtonEmotion->setObjectName("DialogChatButton");
    m_pushButtonMsgRecord->setObjectName("DialogChatButton");
    connect( m_pushButtonEmotion,SIGNAL(clicked()),this,SLOT(onClickedPushButtonEmotion()) );
    connect( m_pushButtonMsgRecord,SIGNAL(clicked()),this,SLOT(onClickedPushButtonMsgRecord()) );

    /// 关闭/发送按钮
    m_pushButtonClose = new QPushButton(this);
    m_pushButtonClose->setObjectName("SendButton");
    connect( m_pushButtonClose,SIGNAL(clicked()),this,SLOT(onClickedPushButtonClose()) );
    m_pushButtonSend = new QPushButton(this);
    m_pushButtonSend->setAutoDefault(true);
    m_pushButtonSend->setObjectName("SendButton");
    connect( m_pushButtonSend,SIGNAL(clicked()),this,SLOT(onClickedPushButtonSend()) );
    m_pushButtonSendSwitch = new QPushButton(this);
    m_pushButtonSendSwitch->raise();
    EbIconHelper::Instance()->SetIcon(m_pushButtonSendSwitch,QChar(0xf0d7),12);
    m_pushButtonSendSwitch->setObjectName("SendButton");
    connect( m_pushButtonSendSwitch,SIGNAL(clicked()),this,SLOT(onClickedPushButtonSendSwitch()) );
    if (theApp->sendKeyType()==EB_SEND_KEY_ENTER)
        onTriggeredSendSwitch0();
    else
        onTriggeredSendSwitch1();

    updateLocaleInfo();
    checkMyForbidSpeechState(false,false);
}

EbWidgetChatInput::~EbWidgetChatInput()
{
    if (m_timerIdCheckForbid!=0) {
        this->killTimer(m_timerIdCheckForbid);
        m_timerIdCheckForbid = 0;
    }
}

void EbWidgetChatInput::updateLocaleInfo()
{
    m_pushButtonEmotion->setToolTip( theLocales.getLocalText("chat-input-widget.button-emotion.tooltip", "emotion") );
    m_pushButtonMsgRecord->setToolTip( theLocales.getLocalText("chat-input-widget.button-msg-record.tooltip", "msg record") );
    m_pushButtonClose->setText( theLocales.getLocalText("chat-input-widget.button-close.text", "close") );
    m_pushButtonClose->setToolTip( theLocales.getLocalText("chat-input-widget.button-close.tooltip", "close") );
    m_pushButtonSend->setText( theLocales.getLocalText("chat-input-widget.button-send.text", "Send") );

    if (m_actionSwitchSend0!=0) {
        m_actionSwitchSend0->setText( theLocales.getLocalText("chat-input-widget.button-send.switch-menu.1","Enter Send Msg") );
    }
    if (m_actionSwitchSend1!=0) {
        m_actionSwitchSend1->setText( theLocales.getLocalText("chat-input-widget.button-send.switch-menu.2","Ctrl+Enter Send Msg") );
    }

}

void EbWidgetChatInput::setFocusInput()
{
    if (m_textEditInput!=0) {
        m_textEditInput->setFocus();
    }
}

void EbWidgetChatInput::setCallInfo(const EbcCallInfo::pointer &pCallInfo)
{
    m_callInfo = pCallInfo;
    assert(m_callInfo.get()!=0);
}

void EbWidgetChatInput::onUserExit(eb::bigint nFromAccount)
{
    if (m_pushButtonSend!=0) {
        m_pushButtonSend->setEnabled(false);
    }
}

void EbWidgetChatInput::onReceiveRich()
{
    if (m_pushButtonSend!=0 && !m_pushButtonSend->isEnabled()) {
        if (theApp->m_ebum.EB_IsLogonVisitor() ||
                m_callInfo->m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_VISITOR) {	// 游客不能被动邀请
            m_pushButtonSend->setEnabled(true);
        }
    }
}

void EbWidgetChatInput::sendFile(const QString &filePath, bool checkImage)
{
    if (isForbidSpeech()) {
        /// ** 禁言限制中
        return;
    }

    if (checkImage) {
        const QImage image(filePath);
        if (image.width()>0 && image.height()>0) {
            m_textEditInput->document()->addResource(QTextDocument::ImageResource, QUrl(filePath), image);
            m_textEditInput->textCursor().insertImage(filePath);
            return;
        }
    }

    if (m_callInfo->isGroupCall()) {
        theApp->m_ebum.EB_AddTempGroupFileRes(filePath.toStdString().c_str(), "", m_callInfo->groupId());
    }
    else {
        eb::bigint nToUserId = 0;
//        //CString sToAccount;
//        const int nCurSel = m_comboSendTo.GetCurSel();
//        if (nCurSel > 0)	// >=
//        {
//            const CTreeItemInfo * pTreeItemInfo = (CTreeItemInfo*)m_comboSendTo.GetItemData(nCurSel);
//            if (pTreeItemInfo==NULL) return;
//            //sToAccount = pTreeItemInfo->m_sAccount.c_str();
//            nToUserId = pTreeItemInfo->m_nUserId;
//            UpdateData();	// update m_bPrivate
//        }
        theApp->m_ebum.EB_SendFile(m_callInfo->callId(), filePath, nToUserId, false);
    }
}

void EbWidgetChatInput::onDropSendFileTextEdit(const QString &filePath)
{
    sendFile(filePath, true);
    /// 支持中文路径，文件名
//    const QByteArray filePathByteArray = filePath.toLocal8Bit();
//    theApp->m_ebum.EB_SendFile( m_callInfo->callId(), filePathByteArray.constData() );
//    const std::string fileName = filePath.toStdString();
//    const bool fileExist1 = QFileInfo::exists(filePath);
//    const bool fileExist2 = QFileInfo::exists( QString::fromStdString(fileName) );
//    FILE * file = fopen( fileName1.constData(), "r");
//    if (file!=0) {
//        fclose(file);
//    }
//    theApp->m_ebum.EB_SendFile( m_callInfo->callId(), filePath );
}

void EbWidgetChatInput::onClickedPushButtonEmotion()
{
    const QPoint pt = cursor().pos();
    theApp->showDialogEmotionSelect( pt,this );
}

void EbWidgetChatInput::onClickedPushButtonMsgRecord()
{
    emit clickedMsgRecord();
}

void EbWidgetChatInput::onClickedPushButtonClose()
{
    emit clickedClose();
}

void EbWidgetChatInput::onClickedPushButtonSend()
{
    EB_ChatRoomRichMsg pRichMsg;
    if ( !m_textEditInput->formatInputMsg(&pRichMsg)) {
        return;
    }
    const std::vector<EB_ChatRoomMsgItem*>& pRichMsgList = pRichMsg.GetList();
    if (pRichMsgList.empty()) {
        this->m_textEditInput->setFocus();
        return;
    }
//    const int nCurSel = m_comboSendTo.GetCurSel();
//    eb::bigint nToUserId = 0;
//    CString sToAccount;
//    const bool bIsDepDialog = m_pCallInfo.m_sGroupCode>0;
//    if (nCurSel > 0)
//    {
//        CTreeItemInfo * pTreeItemInfo = (CTreeItemInfo*)m_comboSendTo.GetItemData(nCurSel);
//        if (pTreeItemInfo==NULL) return;
//        sToAccount = pTreeItemInfo->m_sAccount.c_str();
//        nToUserId = pTreeItemInfo->m_nUserId;
//        UpdateData();
//    }

    eb::bigint nToUserId = 0;
    const bool bPrivate = false;
    if ( theApp->m_ebum.EB_SendRich(m_callInfo->m_pCallInfo.GetCallId(),&pRichMsg,nToUserId,bPrivate)==0 ) {
        m_textEditInput->clear();
    }
    m_textEditInput->setFocus();
}

void EbWidgetChatInput::onClickedPushButtonSendSwitch()
{
    if (m_menuSendSwitch==0) {
        m_menuSendSwitch = new QMenu(this);
        m_actionSwitchSend0 = m_menuSendSwitch->addAction( theLocales.getLocalText("chat-input-widget.button-send.switch-menu.1","Enter Send Msg") );
        m_actionSwitchSend0->setCheckable(true);
        m_actionSwitchSend0->setChecked(true);
        connect( m_actionSwitchSend0, SIGNAL(triggered()), this, SLOT(onTriggeredSendSwitch0()) );
        m_actionSwitchSend1 = m_menuSendSwitch->addAction( theLocales.getLocalText("chat-input-widget.button-send.switch-menu.2","Ctrl+Enter Send Msg") );
        m_actionSwitchSend1->setCheckable(true);
        connect( m_actionSwitchSend1, SIGNAL(triggered()), this, SLOT(onTriggeredSendSwitch1()) );
    }
    m_menuSendSwitch->exec( cursor().pos() );
}

void EbWidgetChatInput::onTriggeredSendSwitch0()
{
    theApp->setSendKeyType(EB_SEND_KEY_ENTER);
    if (m_actionSwitchSend0!=0) {
        m_actionSwitchSend0->setChecked(true);
    }
    if (m_actionSwitchSend1!=0) {
        m_actionSwitchSend1->setChecked(false);
    }
    m_pushButtonSend->setToolTip( theLocales.getLocalText("chat-input-widget.button-send.tooltip.1","") );
    m_textEditInput->setFocus();
}

void EbWidgetChatInput::onTriggeredSendSwitch1()
{
    theApp->setSendKeyType(EB_SEND_KEY_CTRL_ENTER);
    if (m_actionSwitchSend0!=0) {
        m_actionSwitchSend0->setChecked(false);
    }
    if (m_actionSwitchSend1!=0) {
        m_actionSwitchSend1->setChecked(true);
    }
    m_pushButtonSend->setToolTip( theLocales.getLocalText("chat-input-widget.button-send.tooltip.2","") );
    m_textEditInput->setFocus();
}

void EbWidgetChatInput::onSelectedResourceEmotion(const QIcon &icon,mycp::bigint /*resourceId*/,
                                                  const QString& resourceCmInfo,const QString & resourceFilePath)
{
    QImage image;
    if (!resourceFilePath.isEmpty() && QFileInfo::exists(resourceFilePath)) {
        ///
        image = QImage(resourceFilePath);
    }
    if (image.isNull()) {
        image = icon.pixmap(24,24).toImage();
    }
    /// 下面的addResource代码非常重要, 内存增加很少
    m_textEditInput->document()->addResource(QTextDocument::ImageResource, QUrl(resourceCmInfo), image);
    m_textEditInput->textCursor().insertImage(resourceCmInfo);
}

const int const_button_send_switch_width = 16;
void EbWidgetChatInput::resizeEvent(QResizeEvent * e)
{
    int x = 0;
    int y = 0;
    const QSize const_send_button_size(68,26);
    const int const_bottom_height = const_send_button_size.height()+10; /// 下边“发送/关闭”工具条高度
    m_textEditInput->setGeometry( x,y,width(),height()-const_bottom_height );
    m_labelInputForbidState->setGeometry(m_textEditInput->geometry());
    x = 6;
    y = height()-const_bottom_height+1;
    m_pushButtonEmotion->move(x,y);
    x += const_button_emotion_size.width();
    m_pushButtonMsgRecord->move(x,y);
    x = width()-const_button_send_switch_width-8;
    m_pushButtonSendSwitch->setGeometry( x,y,const_button_send_switch_width,const_send_button_size.height() );
    x -= (const_send_button_size.width()-const_button_send_switch_width/2-2);
    m_pushButtonSend->setGeometry( x,y,const_send_button_size.width()-const_button_send_switch_width/2,const_send_button_size.height());
    x -= (const_send_button_size.width()+8);
    m_pushButtonClose->setGeometry( x,y,const_send_button_size.width(),const_send_button_size.height());

    QWidget::resizeEvent(e);
}

void EbWidgetChatInput::contextMenuEvent(QContextMenuEvent *e)
{
//    QPoint pos; //获取按键菜单的坐标
//    // int x = pos.x();
//    int y = pos.y();
//    // pos.setX(x + this->geometry().width()/2);
//    pos.setY(y+m_pushButtonSend->geometry().height());
    //    m_menuTextEdit->exec(m_pushButtonSend->mapToGlobal(pos));
}

void EbWidgetChatInput::timerEvent(QTimerEvent *e)
{
    if ( m_timerIdCheckForbid!=0 && m_timerIdCheckForbid==e->timerId() ) {
        this->killTimer(m_timerIdCheckForbid);
        m_timerIdCheckForbid = 0;
        checkMyForbidSpeechState(false,false);
    }
    QWidget::timerEvent(e);
}

void EbWidgetChatInput::checkMyForbidSpeechState(bool bNewMessage, bool bFromMemberInfo, QString *pOutForbidMessage)
{
    if ( !m_callInfo->isGroupCall() ) {
        return;
    }
    /// *** 群组聊天，检查当前用户是否禁言状态
    int nForbidMinutes = 0;
    if (theApp->m_ebum.EB_IsMemberForbidSpeech(m_callInfo->groupId(),theApp->logonUserId(),nForbidMinutes))
    {
        /// ** 检查到我的帐号，当前群组部门禁言
        QString sStringText;
        if (nForbidMinutes==0) {
            /// _T("你被永久禁言：");
            sStringText = theLocales.getLocalText("chat-msg-text.your-forbid-speech-forever","Your Forbid Speech Forever");
        }
        else {
            if (nForbidMinutes<60) {
                /// 你被禁言 %d 分钟：
                sStringText = QString("%1 %2 %3 :")
                        .arg(theLocales.getLocalText("chat-msg-text.your-forbid-speech","Your Forbid Speech"))
                        .arg(nForbidMinutes)
                        .arg(theLocales.getLocalText("name-text.minute","minute"));
            }
            else if (nForbidMinutes<1440) {
                /// 1440分钟=24小时=1天
                /// a 剩余分钟数
                /// 你被禁言 %d 小时：
                const int nRemainMinutes = nForbidMinutes%60;
                if (nRemainMinutes==0) {
                    sStringText = QString("%1 %2 %3 :")
                            .arg(theLocales.getLocalText("chat-msg-text.your-forbid-speech","Your Forbid Speech"))
                            .arg((int)nForbidMinutes/60)
                            .arg(theLocales.getLocalText("name-text.hour","hour"));
                }
                else {
                    /// 你被禁言 %d 小时 %d 分钟：
                    sStringText = QString("%1 %2 %3 %4 %5 :")
                            .arg(theLocales.getLocalText("chat-msg-text.your-forbid-speech","Your Forbid Speech"))
                            .arg((int)nForbidMinutes/60)
                            .arg(theLocales.getLocalText("name-text.hour","hour"))
                            .arg((int)nRemainMinutes)
                            .arg(theLocales.getLocalText("name-text.minute","minute"));
                }
            }
            else {
                const int nDays = nForbidMinutes/1440;
                const int nHours = (nForbidMinutes%1440)/60;
                const int nMinutes = nForbidMinutes%60;
                sStringText = QString("%1 %2 %3")
                .arg(theLocales.getLocalText("chat-msg-text.your-forbid-speech","Your Forbid Speech"))
                .arg(nDays)
                .arg(theLocales.getLocalText("name-text.day","day"));
                if (nHours>0) {
                    sStringText += QString(" %1 %2")
                            .arg(nHours)
                            .arg(theLocales.getLocalText("name-text.hour","hour"));
                }
                if (nMinutes>0) {
                    sStringText += QString(" %1 %2")
                            .arg(nMinutes)
                            .arg(theLocales.getLocalText("name-text.minute","minute"));
                }
                sStringText += " :";
            }
        }
        if (bNewMessage && !m_labelInputForbidState->isVisible() && pOutForbidMessage!=0) {
            *pOutForbidMessage = sStringText;
        }
        m_labelInputForbidState->setText(QString("<font color=#606060>%1</font> ").arg(sStringText));
        m_labelInputForbidState->setVisible(true);
        m_labelInputForbidState->raise();
        m_textEditInput->setEnabled(false);
        m_pushButtonEmotion->setEnabled(false);
        m_pushButtonSend->setEnabled(false);
        if (m_timerIdCheckForbid==0) {
            m_timerIdCheckForbid = this->startTimer(60*1000);
        }
    }
    else if (theApp->m_ebum.EB_IsGroupForbidSpeech(m_callInfo->groupId())) {

        if ( theApp->m_ebum.EB_IsGroupAdminLevel(m_callInfo->groupId()) ||
             theApp->m_ebum.EB_IsGroupManager(m_callInfo->groupId()) ) {
            m_labelInputForbidState->setVisible(false);
            m_textEditInput->setEnabled(true);
            m_pushButtonEmotion->setEnabled(true);
            m_pushButtonSend->setEnabled(true);
        }
        else {
            /// 群禁言中，群管理员可以发言：
            const QString sStringText = theLocales.getLocalText("chat-msg-text.group-forbid-speech-tip","Group Forbid Speech");
            if ( bNewMessage && !m_labelInputForbidState->isVisible() && pOutForbidMessage!=0 ) {
                *pOutForbidMessage = sStringText;
            }
            m_labelInputForbidState->setText(QString("<font color=#606060>%1</font> ").arg(sStringText));
            m_labelInputForbidState->setVisible(true);
            m_labelInputForbidState->raise();
            m_textEditInput->setEnabled(false);
            m_pushButtonEmotion->setEnabled(false);
            m_pushButtonSend->setEnabled(false);
            if (m_timerIdCheckForbid==0) {
                m_timerIdCheckForbid = this->startTimer(60*1000);
            }
        }
        return;
    }
    else if ( m_labelInputForbidState->isVisible() ) {
        if (pOutForbidMessage!=0) {
            if (bFromMemberInfo) /// 你已被解除禁言：
                *pOutForbidMessage = theLocales.getLocalText("chat-msg-text.your-delete-forbid-speech","Your Delete Forbid Speech");
            else    /// 已经解除群禁言：
                *pOutForbidMessage = theLocales.getLocalText("chat-msg-text.group-delete-forbid-speech","Group Delete Forbid Speech");
        }
        m_labelInputForbidState->setVisible(false);
        m_textEditInput->setEnabled(true);
        m_pushButtonEmotion->setEnabled(true);
        m_pushButtonSend->setEnabled(true);
        this->m_textEditInput->setFocus();
    }
}

bool EbWidgetChatInput::isForbidSpeech() const
{
    return (m_callInfo->isGroupCall() && m_labelInputForbidState!=0 && m_labelInputForbidState->isVisible())?true:false;
}



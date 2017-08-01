#ifndef EBWIDGETCHATINPUT_H
#define EBWIDGETCHATINPUT_H

#include "ebclientapp.h"
#include <QWidget>
#include <QPushButton>
#include <QLabel>
class EbTextEdit;

class EbWidgetChatInput : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetChatInput(const EbcCallInfo::pointer &callInfo, QWidget *parent = 0);
    virtual ~EbWidgetChatInput(void);

    void updateLocaleInfo(void);
    void setFocusInput(void);
    void setCallInfo(const EbcCallInfo::pointer &callInfo);    /// 主要用于更新 CALLID
    void onUserExit(eb::bigint fromUserId);
    void onReceiveRich(void);
    void checkMyForbidSpeechState(bool newMessage, bool fromMemberInfo, QString *pOutForbidMessage=0);
    bool isForbidSpeech(void) const;
signals:
    void clickedClose(void);
    void clickedMsgRecord(void);
public slots:
    void sendFile(const QString &filePath, bool checkImage);
    void onDropSendFileTextEdit(const QString &filePath);
    void onClickedPushButtonEmotion(void);
    void onClickedPushButtonMsgRecord(void);
    void onClickedPushButtonClose(void);
    void onClickedPushButtonSend(void);
    void onClickedPushButtonSendSwitch(void);
    void onTriggeredSendSwitch0(void);
    void onTriggeredSendSwitch1(void);

    void onSelectedResourceEmotion(const QIcon& icon,mycp::bigint resourceId,const QString& resourceCmInfo,const QString & resourceFilePath);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void timerEvent(QTimerEvent *e);
private:
    EbTextEdit* m_textEditInput;
    QLabel * m_labelInputForbidState;
    QPushButton* m_pushButtonEmotion;
    QPushButton* m_pushButtonMsgRecord;
    QPushButton* m_pushButtonClose;
    QPushButton* m_pushButtonSend;
    QPushButton* m_pushButtonSendSwitch;
    QMenu * m_menuSendSwitch;
    QAction * m_actionSwitchSend0;
    QAction * m_actionSwitchSend1;
    QMenu * m_menuTextEdit;
    EbcCallInfo::pointer m_callInfo;
    int m_timerIdCheckForbid;
};

#endif // EBWIDGETCHATINPUT_H

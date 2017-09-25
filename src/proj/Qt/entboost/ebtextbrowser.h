#ifndef EBTEXTBROWSER_H
#define EBTEXTBROWSER_H

#include <ebclientapp.h>
#include <QTextBrowser>
#include <QMouseEvent>

class EbFrameChatToolBar;
class EbTextBlockUserData;

class EbTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit EbTextBrowser(const EbcCallInfo::pointer &callInfo, QWidget *parent = Q_NULLPTR);
    virtual ~EbTextBrowser(void);
    static EbTextBrowser * create(const EbcCallInfo::pointer &callInfo, QWidget *parent = 0);

    void setCallInfo(const EbcCallInfo::pointer &callInfo);    /// 主要用于更新 CALLID
    void insertUrl(const QString &url);
    void insertPlainTextEb(const QString &text,const QColor &color);
    void addRichMsg(bool saveHistory, bool receive, const CCrRichInfo *crMsgInfo,EB_STATE_CODE state=EB_STATE_OK, QString* sOutFirstMsg1=0,QString* sOutFirstMsg2=0);
    void onReceivingFile(bool offLineUser, const CCrFileInfo * fileInfo);
    void addFileMsg(bool receive, const CCrFileInfo *fileInfo);
    void writeMsgDate(time_t msgTime);
    void moveTextBrowserToStart(void);
    void moveTextBrowserToEnd(void);
    void addLineString(eb::bigint msgId, const QString &text);
    void updateMsgText(eb::bigint msgId, const QString &newText, int blockFrom, int blockTo=-1);
    void setMsgReceiptFlag(eb::bigint msgId, int receiptFlag);
    void loadHistoryMsg(int loadLastCount);
    void loadMsgRecord(const char *sql, bool desc);
    void updateMsgReceiptData(eb::bigint msgId, eb::bigint fromUserId, int ackType, EB_STATE_CODE state);
signals:
    void openSubId(eb::bigint subId);
public slots:
    void onAnchorClicked(const QUrl &url);
    void onDeleteMessage(qint64 msgId);
//    void onCopyAvailable(bool b);
protected:
    void findBlockFromPosition(const QPoint &pos);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void timerEvent(QTimerEvent *e);
//    void drawBlock(void);
//    virtual void paintEvent(QPaintEvent *e);

    EbTextBlockUserData * updateBlockMsgId(eb::bigint msgId);
    void getFromToName(bool receive, eb::bigint fromUserId, eb::bigint toUserId, tstring &outFromUserName, tstring &outToUserName);
    bool writeTitle(bool writeLeft, eb::bigint msgId, bool aprivate, eb::bigint fromUserId, const tstring &fromName,
                    eb::bigint toUserId, const tstring &toName, time_t msgTime, int nReadFlag, QString *pOutWindowText=0);
    bool insertImage(const QString &filePath, const QString &alt, QString *pOutText=0);
    void writeFileMessage(bool receive, eb::bigint msgId, eb::bigint resourceId, const QString &filePath, eb::bigint fileSize, bool showNameOnly=false, QString *pOutMsgText=0);
    void writeVoiceMessage(const QString &voiceFile, QString *pOutMsgText=0);
    bool writeCardDataMessage( bool receive, eb::bigint msgId, const char *cardData, QString *pOutMsgText=0);
    void addChatMsgBlock(eb::bigint msgId, bool alignLeft);
private:
    EbcCallInfo::pointer m_callInfo;
    int m_timerIdFindTextBlockFromPosition;
    EbFrameChatToolBar * m_chatToolBar;
    QDateTime m_tLastMsgDayTime;
    mycp::bigint m_nLastMsgId;
    mycp::bigint m_nFromUid;
    mycp::bigint m_nToUid;
//    CLockMap<eb::bigint,std::string> m_cardInfoList;
    CLockMap<eb::bigint,bool> m_pPrevReceivedFileMsgIdList;   /// msgid->
};

#endif // EBTEXTBROWSER_H

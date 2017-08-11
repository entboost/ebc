#ifndef EBFRAMEITEM_H
#define EBFRAMEITEM_H

#include <QPushButton>
#include <QLabel>
#include <boost/shared_ptr.hpp>
//#include <stl/locklist.h>
#include "ebdialogchatbase.h"

class EbDialogWorkFrame;

class EbFrameItem : public QObject
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<EbFrameItem> pointer;
    enum FRAME_ITEM_TYPE {
        FRAME_ITEM_WORK_FRAME           /// 工作台
        , FRAME_ITEM_MAIN_FRAME         /// 联系人（主界面）
        , FRAME_ITEM_CALL_DIALOG        /// 聊天会话
        , FRAME_ITEM_UNKNOWN
    };
    EbFrameItem(const EbDialogChatBase::pointer &dialogChatBase);
    EbFrameItem(FRAME_ITEM_TYPE itemType);
    virtual ~EbFrameItem(void);
    static EbFrameItem::pointer create(FRAME_ITEM_TYPE itemType);
    static EbFrameItem::pointer create(const EbDialogChatBase::pointer &dialogChatBase);

    FRAME_ITEM_TYPE itemType(void) const {return m_itemType;}
    bool isItemType(FRAME_ITEM_TYPE itemType) const {return m_itemType==itemType?true:false;}
    QWidget* parent(void) const;

    void buildButton(int leftWidth, QWidget *parent = 0);
//    QPushButton* buttonClose(void) const {return m_pushButtonClose;}
//    void setItemText(const QString & text);
    const QString &itemText(void) const {return m_itemText;}
    EbDialogWorkFrame *dialogWorkFrame(void) const {return m_dialogWorkFrame;}
//    void updateWorkFrameSize(bool showLeft,int size);
    const EbDialogChatBase::pointer &dialogChatBase(void) const {return m_dialogChatBase;}
    mycp::bigint callId(void) const {return (m_dialogChatBase.get()==0)?0:m_dialogChatBase->callId();}
    mycp::bigint groupId(void) const {return (m_dialogChatBase.get()==0)?0:m_dialogChatBase->groupId();}
    mycp::bigint fromUserId(void) const {return (m_dialogChatBase.get()==0)?0:m_dialogChatBase->fromUserId();}
    /// * 检查按钮点击状态：1=点击关闭，2=上边点击，0=没有点击
    int checkLeftButtonClickState(const QPushButton *button, const QPoint &pt) const;
    bool requestClose(void);
//    QPushButton* buttonLeft(void) const {return m_pushButtonLeft;}

    void onResize(int index, const QRect& rect, int leftWidth);
    /// * 用于关闭某个ITEM，上下移动按钮位置
    void onMove(int index);
    void onMoveEvent(void);
    void setChecked(bool checked, bool hideButton=false);
    bool isChecked(void) const;
    bool checkShowHideCloseButton(const QPoint &pt);
    void setCloseButtonVisible(bool visible);
    void setCloseTime(qint64 t) {m_closeTime=t;}
    qint64 closeTime(void) const {return m_closeTime;}
    void clearUnreadMsg(bool fromUserClick=false);
    void addUnreadMsg(void);
    void setUnreadMsg(size_t unreadMsgCount);

    void onUserLineStateChange(eb::bigint groupId, eb::bigint userId, EB_USER_LINE_STATE lineState);
    void onMemberHeadChange(const EB_MemberInfo * memberInfo);
    void onContactHeadChange(const EB_ContactInfo* contactInfo);
    void onGroupInfo(const EB_GroupInfo* groupInfo);
    bool timerCheckState(void);
signals:
    void checkedFrameitem(const EbFrameItem *frameItem);
public slots:
    void onWorkItemSizeChange(bool fromAboutBlank, int size);
private:
    FRAME_ITEM_TYPE m_itemType;
    QString m_itemText;
    EbDialogChatBase::pointer m_dialogChatBase;
    EbDialogWorkFrame * m_dialogWorkFrame;
    QPushButton * m_pushButtonLeft;
    QLabel * m_labelUnreadMsg;
    QLabel * m_labelImage;
    QPushButton * m_pushButtonClose;
    qint64 m_closeTime;
    size_t m_nUnreadMsgCount;
};
#endif // EBFRAMEITEM_H

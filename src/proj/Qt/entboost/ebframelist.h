#ifndef EBFRAMELIST_H
#define EBFRAMELIST_H

#include "ebframeitem.h"

class EbFrameList
{
public:
    EbFrameList(QWidget* parent);
    virtual ~EbFrameList(void);

    void timerCheckState(void);
    EbDialogWorkFrame * getWorkFrame(void) const;
    EbDialogChatBase::pointer getDialogChatBase(eb::bigint callId, bool remove, bool autoCall);
    bool addUnreadMsg(eb::bigint callId, eb::bigint msgId);
    bool setUnreadMsg(eb::bigint callId, size_t unreadMsgCount);
//    void setUnreadMsg(CFrameWndInfo::FRAME_WND_TYPE nType, size_t nUnreadMsgCount);	// for FRAME_WND_WORK_FRAME
    bool existFrameItem(eb::bigint nCallId) const;

    void onUserLineStateChange(eb::bigint groupCode, eb::bigint userId, EB_USER_LINE_STATE lineState);
    void onMemberInfo(const EB_MemberInfo *memberInfo, bool changeLineState);
    void onGroupInfo(const EB_GroupInfo *groupInfo);
    void onRemoveGroup(const EB_GroupInfo *groupInfo);
    void onRemoveMember(const EB_GroupInfo *groupInfo, eb::bigint memberId, eb::bigint memberUserId);
    void onMemberHeadChange(const EB_MemberInfo *memberInfo);
    void onContactHeadChange(const EB_ContactInfo *cContactInfo);
protected:
    void addFrameItem(const EbFrameItem::pointer &frameItem, bool show, bool autoCall,
                      bool forceAddToList=false, int insertOffset=-1);
    void showByCallId(eb::bigint callId);
    bool showByIndex(int index);
    EbDialogWorkFrame* showWorkFrame(void);
    void clickedLeftButton(const QPushButton *leftButton, const QPoint &pt);
    void showFirst(void);
    void closeItem(const EbDialogChatBase *chatBase);

    void checkShowHideCloseButton(const QPoint &pt);    /// 定期检查显示或隐藏关闭按钮
    void onResize(const QRect& rect);
    /// * 用于关闭某个ITEM，上下移动按钮位置
    void onMove(void);
    /// 主要用于移某些 parent=0 窗口
    void onMoveEvent(void);
    size_t size(void) const {return m_list.size();}

    /// 定期检查并关闭 ITEM对应窗口
    void checkCloseItem(void);
protected:
    QWidget* m_pParent;
    int m_leftWidth;
    CLockList<EbFrameItem::pointer> m_list;
    CLockList<EbFrameItem::pointer> m_hide;
    CLockList<EbFrameItem::pointer> m_close;
};

#endif // EBFRAMELIST_H

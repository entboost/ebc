#ifndef EBWORKLIST_H
#define EBWORKLIST_H

#include <ebworkitem.h>
#include <stl/locklist.h>

class EbWorkList
{
public:
    EbWorkList(QWidget *parent);
    virtual ~EbWorkList(void);

    bool isEmpty(void) const {return m_list.empty();}
    void setCheckedFocus(void);

    EbWidgetUserInfo *widgetUserInfo(void) const;
    EbWidgetUserList *widgetUserList(void) const;
    EbWidgetVideoFrame *widgetVideoFrame(void) const;
    EbWidgetChatRecord *widgetChatRecord(void) const;
    EbWidgetFileTranList *widgetTranFile(void) const;
protected:
    void addWorkItem(bool saveUrl, const EbWorkItem::pointer& workItem, int insertOffset=-1);
    void showByIndex(int index, bool bSearchFocus);
    void setItemText(EbWorkItem::WORK_ITEM_TYPE itemType, const QString &text);
    void clickedTopButton(const QPushButton *topButton, const QPoint &pos);
    void showFirst(void);
    int indexOf(const EbWidgetWorkView *view) const;
    int indexOf(EbWorkItem::WORK_ITEM_TYPE type, bool sendClose=false) const;
    void closeItem(const EbWidgetWorkView *view);
    void closeItem(int index);
//    void closeItem(EbWorkItem::WORK_ITEM_TYPE type);
    void clear(void);
//    void closeItem(const DialogChatBase* chatBase);

    void checkShowHideCloseButton(const QPoint& pt);    /// 定期检查显示或隐藏关闭按钮
    int onResize(const QRect& rect);
    /// *用于关闭某个ITEM，左右移动按钮位置
    int onMove(void);
//    int topButtonWidth(void) const;

    void back(void);
    void forward(void);
    bool canGoBack() const;
    bool canGoForward(void) const;
    void refreshOrStop(void);
    void saveUrlHistory(void);
//    void stop(void);
protected:
    virtual void onItemSizeChange(const EbWorkItem::pointer& /*fromWorkItem*/,int /*size*/,int /*right*/) {}
protected:
    QWidget* m_pParent;
    int m_topHeight;
    int m_leftOffset;
    CLockList<EbWorkItem::pointer> m_list;
};

#endif // EBWORKLIST_H

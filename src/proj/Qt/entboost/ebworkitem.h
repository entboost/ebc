#ifndef EBWORKITEM_H
#define EBWORKITEM_H

//#include <QObject>
#include "ebclientapp.h"
#include <QPushButton>
#include <QLabel>
#include <ebwidgetworkview.h>
#include <ebwidgetuserinfo.h>
#include <ebwidgetuserlist.h>
#include <ebwidgetchatrecord.h>
#include <ebwidgetvideoframe.h>
#include <ebwidgetfiletranlist.h>

//const int const_top_button_width = 140;
const int const_top_button_height = 30;

class EbWorkItem : public QObject
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<EbWorkItem> pointer;
    enum WORK_ITEM_TYPE {
        WORK_ITEM_USER_INFO,             /// 用户名片
        WORK_ITEM_USER_LIST,             /// 群组成员
        WORK_ITEM_TRAN_FILE,             /// 文件传输
        WORK_ITEM_CHAT_RECORD,             /// 聊天记录
        WORK_ITEM_VIDEO_FRAME,             /// 视频面板
        WORK_ITEM_WEB_BROWSER             /// 浏览器
    };
//    explicit EbWorkItem(QObject *parent=0);
    explicit EbWorkItem(WORK_ITEM_TYPE itemType);
    virtual ~EbWorkItem(void);
    static EbWorkItem::pointer create(WORK_ITEM_TYPE itemType);

    void setTopButtonWidth(int v) {m_topButtonWidth=v;}
    void setCallInfo(const EbcCallInfo::pointer &v) {m_callInfo=v;}
    WORK_ITEM_TYPE itemType(void) const {return m_itemType;}
    bool isItemType(WORK_ITEM_TYPE itemType) const {return m_itemType==itemType?true:false;}
    mycp::bigint subscribeId(void) const {return m_widgetWorkView==0?0:m_widgetWorkView->m_funcInfo.m_nSubscribeId;}
    mycp::bigint fromSubscribeId(void) const {return m_widgetWorkView==0?0:m_widgetWorkView->m_funcInfo.m_nFromSubscribeId;}
    void setAppUrl(bool saveUrl, const QString &url, const QString &postData);
    void setFunInfo(const EB_SubscribeFuncInfo& funcInfo, bool openNewClose);
    const QString& appUrl(void) const {return m_appUrl;}

    QWidget* parent(void) const;
    void buildButton(bool saveUrl,int topHeight, QWidget *parent = 0);
    QPushButton* buttonClose(void) const {return m_pushButtonClose;}
    EbWidgetWorkView::pointer widgetWorkView(void) const {return m_widgetWorkView;}
    EbWidgetUserInfo *widgetUserInfo(void) const {return m_widgetUserInfo;}
    EbWidgetUserList *widgetUserList(void) const {return m_widgetUserList;}
    EbWidgetVideoFrame *widgetVideoFrame(void) const {return m_widgetVideoFrame;}
    EbWidgetChatRecord *widgetChatRecord(void) const {return m_widgetChatRecord;}
    EbWidgetFileTranList *widgetTranFile(void) const {return m_widgetTranFile;}

    QRect rectButton(void) const;
    /// *检查按钮点击状态：1=点击关闭，2=左边点击，0=没有点击
    int checkTopButtonClickState(const QPushButton* button, const QPoint& pt) const;

    int onResize(int x, const QRect& rect, int topHeight,int leftOffset);
    /// *用于关闭某个ITEM，左右移动按钮位置
    int onMove(int x);
    QRect topGeometry(void) const;
    void setChecked(bool checked, bool hideButton=false, bool searchFocus=false);
    bool isChecked(void) const;
    bool setCheckedFocus(void);
    bool checkShowHideCloseButton(const QPoint& pt);
    void setCloseButtonVisible(bool visible);
    void sendClose(void);

    bool back(void);
    bool forward(void);
    bool canGoBack(bool &outCanGoBack) const;
    bool canGoForward(bool &outCanGoForward) const;
    bool refreshOrStop(void);
    bool saveUrlHistory(void);
signals:

public slots:
    void onMemberOnlineSizeChange(int memberSize, int onlineSize);
    bool onTitleChanged(WORK_ITEM_TYPE itemType, const QString &title);
    bool onTitleChanged(const EbWidgetWorkView *view, const QString &title);
    bool onIconChanged(const EbWidgetWorkView *view, const QIcon &icon);
private:
    WORK_ITEM_TYPE m_itemType;
    QString m_itemText;
    QString m_appUrl;
    QString m_postData;
    EbWidgetWorkView::pointer m_widgetWorkView;
    EbcCallInfo::pointer m_callInfo;
    EbWidgetUserInfo *m_widgetUserInfo;
    EbWidgetUserList *m_widgetUserList;
    EbWidgetVideoFrame *m_widgetVideoFrame;
    EbWidgetChatRecord *m_widgetChatRecord;
    EbWidgetFileTranList *m_widgetTranFile;
    int m_topButtonWidth;
    QPushButton *m_pushButtonTop;
//    QLabel *m_labelUnreadMsg;
    QLabel *m_labelImage;
    EB_SubscribeFuncInfo m_subFuncInfo;
    QPushButton *m_pushButtonClose;
};
const EbWorkItem::pointer EbWorkItemNull;

#endif // EBWORKITEM_H

#ifndef EBDIALOGWORKFRAME_H
#define EBDIALOGWORKFRAME_H

#include "ebclientapp.h"
#include <ebdialogbase.h>
#include <ebworklist.h>

namespace Ui {
class EbDialogWorkFrame;
}

class EbWidgetSearchBar;
class EbWidgetAppBar;

/// 工作台界面
/// 上面集成一个搜索地址栏，下面集成浏览器
class EbDialogWorkFrame : public EbDialogBase
        , public EbWorkList
{
    Q_OBJECT
public:
    explicit EbDialogWorkFrame(bool showSearchBae, bool showAppBar, QWidget *parent = 0);
    ~EbDialogWorkFrame(void);

    void updateLocaleInfo(void);
    void setSubUnreadMsg(mycp::bigint subId, size_t unreadMsgCount);
    void addSubUnreadMsg(mycp::bigint subId);
//    bool addSubscribeFuncInfo( const EB_SubscribeFuncInfo & pSubscribeFuncInfo);
//    bool IsSubscribeFuncInfoEmpty(void) const {return m_pMainFuncButtonList.empty();}
    void addUrl(bool saveBrowseTitle, const QString &appUrl, const QString &postData="", int insertOffset=-1);
    void addUrl(bool saveBrowseTitle, const QString &appUrl, const QString &postData, const EB_SubscribeFuncInfo &funcInfo,
                bool openNewClose=false, int insertOffset=-1);
    void onMoveEvent(void);
signals:
    void itemSizeChange(bool fromAboutBlank, int size);
public slots:
    void onClickedButtonTop(void);
    void onUrlChanged(const EbWidgetWorkView *view, const QUrl &url);
    void onLoadStateChange(const EbWidgetWorkView *view, bool loadFinished, bool canGoBack, bool canGoForward);
    void onOpenLinkInNewTab(const EbWidgetWorkView *view, const QUrl &linkUrl);
    void onWindowCloseRequested(const EbWidgetWorkView *view);

    void onClickedBack(void);
    void onClickedForward(void);
    void onClickedRefresh(void);
    void onClickedStop(void);
    void onSearchKeyPressEnter(const QString &text);
    void onSaveUrlHistory(void);
    void onClickedSearchResultUrl(const QString &url);

    void onClickedPushButtonShowHideLeft(void);
    void onClickedButtonAdd(void);
    void onClickedSubApp(const EB_SubscribeFuncInfo &funcInfo);

    void onCloseViewList(void);
protected:
    void updateShowHideIcon(void);
    void moveSize(void);
    virtual void resizeEvent(QResizeEvent *e);
    virtual bool onBeforeClickedPushButtonSysMin(void);
    virtual bool onBeforeClickedPushButtonSysMax(void);
    virtual void reject(void);
    virtual void timerEvent(QTimerEvent *e);
//    virtual void closeEvent(QCloseEvent *e);

    virtual void onItemSizeChange(const EbWorkItem::pointer &fromWorkItem, int size, int right);
private:
    Ui::EbDialogWorkFrame *ui;
    EbWidgetSearchBar * m_widgetSearchBar;
    bool m_showSearchBar;
    EbWidgetAppBar * m_widgetAppBar;
    bool m_showAppBar;
    int m_timerIdCheckState;
    bool m_showedLeft;
    CLockList<const EbWidgetWorkView*> m_listCloseView;
};

#endif // EBDIALOGWORKFRAME_H

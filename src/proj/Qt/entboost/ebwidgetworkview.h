#ifndef EBWIDGETWORKVIEW_H
#define EBWIDGETWORKVIEW_H

#include "ebclientapp.h"
#include <QWidget>
#include <QLabel>
#include <QWebEngineView>
#include <QWebEngineHistory>
//#include <ebwebengineurlrequestinterceptor.h>
#include <boost/shared_ptr.hpp>

class EbWebEngineView;
class EbFrameFindText;

class EbWidgetWorkView : public QWidget
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<EbWidgetWorkView> pointer;
    enum LOAD_STATE {
        LOAD_UNKNOWN
        , LOAD_STARTED
        , LOAD_FINISHED
    };
    explicit EbWidgetWorkView(bool saveUrl, const QUrl &url, const QString &postData, QWidget *parent = 0);
    explicit EbWidgetWorkView(const QString &html, QWidget *parent = 0);
    virtual ~EbWidgetWorkView(void);
    static EbWidgetWorkView::pointer create(bool saveUrl, const QUrl &url, const QString &postData, QWidget *parent = 0);
    static EbWidgetWorkView::pointer create(const QString &html, QWidget *parent = 0);

    EB_SubscribeFuncInfo m_funcInfo;
    void setHtml(const QString &html);
    void load(const QUrl &url, const QString &postData);
    void setOpenLinkInNewTabAddUrlEnable(bool enable) {m_openLinkInNewTabAddUrlEnable=enable;}
    bool openLinkInNewTabAddUrlEnable(void) const {return m_openLinkInNewTabAddUrlEnable;}
    void back(void);
    void forward(void);
    bool canGoBack(void) const;
    bool canGoForward(void) const;
    void refreshOrStop(void);
    void saveUrlHistory(void);
    bool isLoading(void) const;
    bool isLoadFinished(void) const;

    void setFocusInput(void);
    void setSaveUrl(bool v);
    bool saveUrl(void) const {return m_saveUrl;}
signals:
    void titleChanged(const EbWidgetWorkView *view, const QString &title);
    void urlChanged(const EbWidgetWorkView *view, const QUrl &url);
    void iconChanged(const EbWidgetWorkView *view, const QIcon &url);
    void loadStateChange(const EbWidgetWorkView *view, bool loadFinished, bool canGoBack, bool canGoForward);
    void openLinkInNewTab(const EbWidgetWorkView *view, const QUrl &linkUrl);
    void windowCloseRequested(const EbWidgetWorkView *view);
public slots:
    void onFindAction(const QString &selectedText);
    void onLoadStarted(void);
//    void onLoadProgress(int progress);
    void onLoadFinished(bool);
    void onTitleChanged(const QString &title);
//    void onSelectionChanged(void);
    void onUrlChanged(const QUrl &url);
//    void onIconUrlChanged(const QUrl &url);
    void onIconChanged(const QIcon &icon);
    void onLinkHovered(const QString &url);
//    void onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus,
//                             int exitCode);

    void onWindowCloseRequested(void);
    void onOpenLink(const EbWebEngineView *view, QWebEnginePage::WebWindowType type, const QUrl &linkUrl);
    /// find text
    void onFindText(bool back, const QString &text);
    void onExitFindText(void);

    /// 可以 callback
//    void handleFindTextResult(bool found);
protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void timerEvent(QTimerEvent *e);
#ifndef QT_NO_WHEELEVENT
    virtual void wheelEvent(QWheelEvent *e);
#endif
    void showZoomFactor(void);
    void init(void);
private:
    bool m_saveUrl;
    bool m_savedUrl;
    QUrl m_url;
    QString m_html;
    QString m_postData;
    EbWebEngineView * m_webEngineView;
    bool m_openLinkInNewTabAddUrlEnable;
    qreal m_zoomFactor;
    int m_timerIdCheckZoom;
    LOAD_STATE m_loadState;
    QLabel * m_labelStatus;
    int m_timerIdHideStatus;
    EbFrameFindText * m_frameFindText;
//    EbWebEngineUrlRequestInterceptor * m_webEngineUrlRequestInterceptor;
};

#endif // EBWIDGETWORKVIEW_H

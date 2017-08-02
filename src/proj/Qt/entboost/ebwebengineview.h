#ifndef EBWEBENGINEVIEW_H
#define EBWEBENGINEVIEW_H

#include <QWebEngineView>
#include <QMenu>
#include <stl/lockmap.h>

class EbWebEnginePage;

class EbWebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit EbWebEngineView(QWidget* parent=0);
    virtual ~EbWebEngineView(void);

    EbWebEnginePage* page(void) const;
signals:
    void openLink(const EbWebEngineView * view, QWebEnginePage::WebWindowType type, const QUrl &linkUrl);
    void findAction(const QString &selectedText);
public slots:
    void onTriggeredActionOpenLinkInNewTab(void);
    void onTriggeredActionOpenLinkInNewWindow(void);
    void onTriggeredActionFind(void);
    void onTriggeredActionPrint(void);
    void onTriggeredActionInspectElement(void);
    void onTriggeredActionPageTriggerAction(void);

    void onLinkHovered(const QString &url);
protected:
    virtual QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);
    virtual void contextMenuEvent(QContextMenuEvent *e);
//    virtual void keyPressEvent(QKeyEvent *e);
//    virtual bool event(QEvent *e);
private:
    void createMenuData(void);
    void hideAllAction(void);
    void setActionVisible(QWebEnginePage::WebAction webAction, bool visible, bool enable=true);
private:
    QMenu * m_menu;
    QString m_linkHoveredUrl;
    CLockMap<QWebEnginePage::WebAction,QAction*> m_actions;
};

#endif // EBWEBENGINEVIEW_H

#ifndef EBWIDGETSEARCHBAR_H
#define EBWIDGETSEARCHBAR_H

#include <QWidget>
#include <QPushButton>
#include <QMoveEvent>

class EbWidgetSearchResult;
class EbLineEdit;

class EbWidgetSearchBar : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetSearchBar(QWidget *parent = nullptr);
    virtual ~EbWidgetSearchBar(void);

    void updateLocaleInfo(void);
    void changeUrl(const QString &url, bool searchFucus, bool searchSelectAll);
    void updateLoadState(bool loadFinished, bool canGoBack, bool canGoForward);
    void onMoveEvent(void);
//    const QRect &searchEditRect(void);
    const EbWidgetSearchResult * searchResult(void) const {return m_widgetSearchResult;}
signals:
    void clickedBack(void);
    void clickedForward(void);
    void clickedRefresh(void);
    void clickedStop(void);
    void searchKeyPressEnter(const QString &text);
    void saveUrlHistory(void);
public slots:
    void onClickedButtonBack(void);
    void onClickedButtonForward(void);
    void onClickedButtonRefresh(void);
    void onClickedButtonStop(void);
    void onClickedButtonOptions(void);
    void onClickedMenuOptions(void);

    void onSearchEditTextChange(const QString &text);
    void onSearchEditKeyPressEsc(void);
    void onSearchEditKeyPressEnter(const QString &text);
    void onSearchEditKeyPressDown(void);

    void onSearchFirst(const QString &url);
    void onClickedSearchResultUrl(const QString &url);
    void onListResultsKeyPressFirstItemUp(void);
    void onListResultsKeyPressEsc(void);
protected:
//    void moveSize(void);
    virtual void resizeEvent(QResizeEvent *e);
    virtual bool eventFilter(QObject *watched, QEvent *e);
//    virtual bool event(QEvent *e);
private:
    QPushButton * m_buttonBack;
    QPushButton * m_buttonForward;
    QPushButton * m_buttonRefresh;
    QPushButton * m_buttonStop;
    EbLineEdit * m_lineEditSearch;
    QPushButton * m_buttonOptions;
    QMenu * m_menuOptions;
    QAction * m_actionSaveUrlHistory;
    QAction * m_actionClearAllHistory;
    QAction * m_actionClearChromeCache;
    EbWidgetSearchResult * m_widgetSearchResult;
    bool m_canSearch;
    bool m_canUpdateSearchFirst;
};

#endif // EBWIDGETSEARCHBAR_H

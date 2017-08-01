#ifndef EBWIDGETSEARCHRESULT_H
#define EBWIDGETSEARCHRESULT_H

#include <ebclientapp.h>
#include <QWidget>
#include <QPushButton>
#include <QResizeEvent>
#include <QMenu>
#include <QContextMenuEvent>
#include <ebwidgetiteminfo.h>
#include <eblistwidget.h>
#include <ebcallback.h>
#include <ebdefines.h>

class EbWidgetSearchResult : public QWidget
        , public CEBSearchCallback
{
    Q_OBJECT
public:
    explicit EbWidgetSearchResult(EB_SEARCH_FROM_FLAG fromFlag, QWidget *parent = nullptr);

    void setSelectedUserCallback(EbSelectedUserCallback *cb) {m_selectedUserCallback=cb;}
    void search(const char *key);
    void setFocusSelectFirst(void);
    const EbListWidget * listResults(void) const {return m_listWidgetResults;}
    void updateLocaleInfo(void);
signals:
    void clickedSearchResultUrl(const QString &url);
    void searchFirst(const QString &url);
    void selectedItemInfo(const EbWidgetItemInfo::pointer &itemInfo);
public slots:
//    void onItemClicked(QListWidgetItem *item);
    void onKeyPressEnterItem(const QListWidgetItem *item);
    void onItemEntered(QListWidgetItem *item);
    void onItemDoubleClicked(QListWidgetItem *item);
    void onClickedItemLabel(void);
//    void onKeyPressEnterItemLabel(void);
    void onTriggeredActionCopy(void);
    void onTriggeredActionDelete(void);
    void onTriggeredActionOpenChat(void);
    void onClickedButtonSelect(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    void hideAllAciton(void);
private:
    inline void checkShowHide(void);
    void searchApp(const char *key, int limit);
    void searchUrlRecord(const char *key, int limit);
    void searchUserInfo(const char *key);
    /// CEBSearchCallback
    virtual void onGroupInfo(const EB_GroupInfo */*groupInfo*/, const EB_EnterpriseInfo * /*enterpriseInfo*/, unsigned long /*dwParam*/) {}
    virtual void onMemberInfo(const EB_GroupInfo *groupInfo, const EB_MemberInfo *memberInfo, unsigned long dwParam);
    virtual void onContactInfo(const EB_ContactInfo *contactInfo, unsigned long dwParam);
private:
    EB_SEARCH_FROM_FLAG m_searchFromFlag;
    EbSelectedUserCallback * m_selectedUserCallback;
    EbListWidget * m_listWidgetResults;
    int m_itemHeight;
    QMenu * m_menu;
    QAction * m_actionCopy;
    QAction * m_actionDelete;
    QAction * m_actionOpenChat;
    QPushButton * m_pushButtonSelect;  /// for EB_VIEW_SELECT_USER
};

#endif // EBWIDGETSEARCHRESULT_H

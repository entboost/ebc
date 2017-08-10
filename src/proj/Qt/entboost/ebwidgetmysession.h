#ifndef EBWIDGETMYSESSION_H
#define EBWIDGETMYSESSION_H

#include <ebclientapp.h>
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <ebcontextmenu.h>
#include <ebcallrecordinfo.h>

class EbWidgetMySession : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetMySession(QWidget *parent = nullptr);
    virtual ~EbWidgetMySession(void);

    void updateLocaleInfo(void);
    void timerCheckState(void);
    void insertCallRecord(const EbCallRecordInfo::pointer &callRecordInfo, bool insertNew, bool sort=true);
    EbCallRecordInfo::pointer callRecordInfo(eb::bigint groupId, eb::bigint userId, int msgType=0) const;
    void onMemberInfo(const EB_MemberInfo *memberInfo, bool changeLineState);
    void onContactStateChanged(const EB_ContactInfo *contactInfo);
signals:

public slots:
    void onDeleteSession(const EbWidgetItemInfo::pointer &itemInfo);
    void onClearSession(void);
    void onItemEntered(QListWidgetItem *item);
    void onItemDoubleClicked(QListWidgetItem *item);
    void onClickedButtonDelete(void);
    void onClickedButtonAddContact(void);
    void onClickedButtonCall(void);
    void onLoadCallRecord(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    void createMenuData(void);
    bool deleteRecord(const EbCallRecordInfo::pointer &callRecordInfo, bool quesiton);
private:
    QListWidget * m_listWidget;
    QPushButton * m_buttonAddContact;
    QPushButton * m_buttonCall;
    QPushButton * m_buttonDelete;
    EbContextMenu * m_contextMenu;
    CLockMap<const QListWidgetItem*, EbCallRecordInfo::pointer> m_pCallRecordInfo;
    bool m_bReloadRecordData;
};

#endif // EBWIDGETMYSESSION_H

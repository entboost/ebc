#ifndef EBWIDGETMYCONTACT_H
#define EBWIDGETMYCONTACT_H

#include <ebclientapp.h>
#include <ebwidgettreeselectbase.h>
#include <QTreeWidget>
#include <QPushButton>
#include <ebcontextmenu.h>
#include <ebcallback.h>
#include <ebdefines.h>

class EbWidgetMyContact : public EbWidgetTreeSelectBase
{
    Q_OBJECT
public:
    explicit EbWidgetMyContact(EB_VIEW_MODE viewMode, QWidget *parent = nullptr);
    virtual ~EbWidgetMyContact(void);

    const EbWidgetItemInfo::pointer onUGInfo(const EB_UGInfo *ugInfo);
    void onUGDelete(const EB_UGInfo *ugInfo);
    void onContactInfo(const EB_ContactInfo *contactInfo);
    void onContactDelete(const EB_ContactInfo *contactInfo);
    void onContactHeadChange(const EB_ContactInfo *contactInfo);
signals:

public slots:
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onItemClicked(QTreeWidgetItem *item, int column);
    void onItemEntered(QTreeWidgetItem *item, int column);
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onClickedPushButtonSelect(void);
    void onClickedPushButtonCall(void);
    void onClickedPushButtonEdit(void);
    void onAddContactGroup(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    void createMenuData(void);
    void deleteGroupItem(QTreeWidgetItem *item);
private:
    CLockMap<eb::bigint,EbWidgetItemInfo::pointer> m_pGroupItemInfo;	/// ug_id->
    CLockMap<eb::bigint,EbWidgetItemInfo::pointer> m_pContactItemInfo;	/// contact_id->
    EbContextMenu * m_contextMenu;
    time_t m_timeNewGroup;
    bool m_inGroupEdit;
};

#endif // EBWIDGETMYCONTACT_H

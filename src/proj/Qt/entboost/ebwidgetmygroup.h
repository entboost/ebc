#ifndef EBWIDGETMYGROUP_H
#define EBWIDGETMYGROUP_H

#include "ebclientapp.h"
#include <ebwidgettreeselectbase.h>
#include <QTreeWidget>
#include <QPushButton>
#include <QMenu>
#include <ebdefines.h>
#include <ebcallback.h>
#include <ebwidgetiteminfo.h>
#include <ebcontextmenu.h>

class EbWidgetMyGroup : public EbWidgetTreeSelectBase
{
    Q_OBJECT
public:
    explicit EbWidgetMyGroup(EB_VIEW_MODE viewMode, QWidget *parent = 0);
    virtual ~EbWidgetMyGroup(void);

    void setMemberInfo(const EbWidgetItemInfo::pointer &groupItemInfo, const EB_MemberInfo *memberInfo, EB_SORT_ITEMS_FLAG sortItems=EB_AUTO_SORT);
    void onGroupInfo(const EB_GroupInfo *groupInfo);
    void onMemberInfo(const EB_MemberInfo *memberInfo, bool changeLineState);
    void onRemoveGroup(const EB_GroupInfo *groupInfo);
    void deleteMemberInfo(const EB_GroupInfo *groupInfo, eb::bigint memberCode, bool fromDeleteGroup);
//    void deleteGroupInfo(const EB_GroupInfo *groupInfo);
signals:

public slots:
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onItemClicked(QTreeWidgetItem *item, int column);
    void onItemEntered(QTreeWidgetItem *item, int column);
//    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void onClickedPushButtonSelect(void);
    void onClickedPushButtonCall(void);
    void onClickedPushButtonEdit(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    void createMenuData(void);
private:
    CLockMap<eb::bigint,EbWidgetItemInfo::pointer> m_pDepItemInfo;	/// dep_code->
    CLockMap<eb::bigint,EbWidgetItemInfo::pointer> m_pEmpItemInfo;	/// emp_code->
    EbContextMenu * m_contextMenu;
};

#endif // EBWIDGETMYGROUP_H

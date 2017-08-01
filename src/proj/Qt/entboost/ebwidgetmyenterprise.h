#ifndef EBWIDGETMYENTERPRISE_H
#define EBWIDGETMYENTERPRISE_H

#include "ebclientapp.h"
#include <ebwidgettreeselectbase.h>
#include <QTreeWidget>
#include <QPushButton>
#include <QMenu>
#include <ebcallback.h>
#include <ebwidgetiteminfo.h>
#include <ebcontextmenu.h>
#include <ebdefines.h>

class EbWidgetMyEnterprise : public EbWidgetTreeSelectBase
{
    Q_OBJECT
public:
    explicit EbWidgetMyEnterprise(EB_VIEW_MODE viewMode, QWidget *parent = nullptr);
    virtual ~EbWidgetMyEnterprise(void);

    void onEnterpriseInfo(const EB_EnterpriseInfo *enterpriseInfo);
    void SetMemberInfo(const EbWidgetItemInfo::pointer &groupItemInfo, const EB_MemberInfo *memberInfo, EB_SORT_ITEMS_FLAG sortItems=EB_AUTO_SORT);
    void onGroupInfo(const EB_GroupInfo *groupInfo);
    void onMemberInfo(const EB_MemberInfo *memberInfo, bool changeLineState);
    void onRemoveGroup(const EB_GroupInfo *groupInfo);
    void deleteMemberInfo(const EB_GroupInfo *gGroupInfo, eb::bigint memberId, bool fromDeleteGroup);
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
    virtual void timerEvent(QTimerEvent *e);

    void createMenuData(void);
    void setGroupCount(const EbWidgetItemInfo::pointer &groupItem, eb::bigint groupId, bool fromMemberInfo);
    void setGroupCount(const EbWidgetItemInfo::pointer &groupItem, const EB_GroupInfo *gGroupInfo, bool fromMemberInfo);
private:
    CLockMap<eb::bigint,EbWidgetItemInfo::pointer> m_pEntItemInfo;	/// ent_code->
    CLockMap<eb::bigint,EbWidgetItemInfo::pointer> m_pDepItemInfo;	/// dep_code->
    CLockMap<eb::bigint,EbWidgetItemInfo::pointer> m_pEmpItemInfo;	/// emp_code->
    EbContextMenu * m_contextMenu;
    int m_timerIdUpdateEnterpriseCount;
    EbWidgetItemInfo::pointer m_pUpdateEntItemInfo;
    int m_timerIdUpdateGroupCount;
    CLockMap<eb::bigint,EbWidgetItemInfo::pointer> m_pUpdateGroupCountList;	// group_id->
//    EB_EnterpriseInfo m_enterpriseInfo;
};

#endif // EBWIDGETMYENTERPRISE_H

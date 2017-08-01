#ifndef EBCONTEXTMENU_H
#define EBCONTEXTMENU_H

#include <QMenu>
#include <ebwidgetiteminfo.h>
#include <stl/lockmap.h>

const eb::bigint const_default_group_ugid = 0;

class EbContextMenu : QObject
{
    Q_OBJECT
public:
    enum Type {
        MyGroup,
        MyContact,
        MySession,
        MyEnterprise,
        UserList
    };
    EbContextMenu(Type type, QWidget * parent=0);
    virtual ~EbContextMenu(void);

    QAction * exec(const QPoint & pos);
    QAction * setMenuActionVisible(int commandId, bool visible);
    bool updateMenuItem(const EbWidgetItemInfo::pointer &itemInfo);

    void onCallItem(const EbWidgetItemInfo::pointer &itemInfo);
    void onEditItem(const EbWidgetItemInfo::pointer &itemInfo);
signals:
    void addContactGroup(void);
    void deleteSession(const EbWidgetItemInfo::pointer &itemInfo);
    void clearSession(void);
public slots:
    void onTriggeredActionDeleteSession(void);
    void onTriggeredActionClearSession(void);

    void onTriggeredActionAddContact(void);
    void onTriggeredActionEditContact(void);
    void onTriggeredActionDeleteContact(void);
    void onTriggeredActionAddContactGroup(void);
    void onTriggeredActionRenameContactGroup(void);
    void onTriggeredActionDeleteContactGroup(void);

    void onTriggeredActionSystemManager(void);
    void onTriggeredActionNewSubDepartment(void);
    void onTriggeredActionNewSubProject(void);
    void onTriggeredActionNewMember(void);

    void onTriggeredActionNewGroup(void);
    void onTriggeredActionEditGroup(void);
    void onTriggeredActionDeleteGroup(void);
    void onTriggeredActionExitGroup(void);
    void onTriggeredActionGroupAddForbidSpeech(void);
    void onTriggeredActionGroupDelForbidSpeech(void);
    void onTriggeredActionEditMember(void);
    void onTriggeredActionDeleteMember(void);
    void onTriggeredActionMemberAddAdminLevel(void);
    void onTriggeredActionMemberDelAdminLevel(void);
    void onTriggeredActionMemberAddForbidSpeech(void);
    void onTriggeredActionMemberDelForbidSpeech(void);
    void onTriggeredActionCallUser(void);
    void onTriggeredActionViewMsgRecord(void);
    void onTriggeredActionViewGroupShare(void);

    void onTriggeredActionContactMoveTo(void);
private:
    void hideAllMenuAction(void);
    void checkGroupAction(eb::bigint groupId, EB_GROUP_TYPE groupType);
private:
    Type m_type;
    EbWidgetItemInfo::pointer m_itemInfo;
    CLockMap<int,QAction*> m_actionList;
    QAction *m_adminSeparatorAction;
    QAction *m_memberSeparatorAction;
    QAction *m_memberForbidSpeechAction;
    QMenu *m_menuContext;
    QAction *m_actionMoveTo;
    QMenu *m_menuMoveTo;
};

#endif // EBCONTEXTMENU_H

import QtQuick 2.0
import QtQuick.Controls 2.2

EbPage {
    id: rootPage

    signal pressedContactInfo(var contactInfo);
    signal requestDeleteContact(var contactInfo);
    signal pressedGroupInfo(var groupInfo);
    signal pressedLoadSubGroupInfo(var parentGroupInfo);
    signal requestAddContact();
    signal requestNewGroup(var groupType, var parentGroupId);
    signal requestNewContactGroup();
    signal requestRenameUGinfo(var ugId, var groupName);
    signal requestSearch();

    function initData() {
        myGroupListItem.clearData();
        myContactPage.clearData();
        myEnterpriseListItem.clearData();
        theApp.loadOrgData();
        theApp.loadContactData();
    }

    EbMenu {
        id: theTitleMenu
        EbMenuItem {
            id: addContactMenuItem
//            label: "添加联系人"
            onTriggered: { rootPage.requestAddContact(); }
        }
        EbMenuSeparator {
        }
        EbMenuItem {
            id: addGroup0
//            label: "新建部门"
            onTriggered: {
                rootPage.requestNewGroup(0, 0);
            }
        }
        EbMenuItem {
            id: addGroup1
//            label: "新建项目组"
            onTriggered: {
                rootPage.requestNewGroup(1, 0);
            }
        }
        EbMenuItem {
            id: addGroup2
//            label: "新建个人群组"
            onTriggered: {
                rootPage.requestNewGroup(2, 0);
            }
        }
        EbMenuItem {
            id: addContactUserGroup
//            label: "新建个人分组"
            onTriggered: {
                rootPage.requestNewContactGroup();
            }
        }
    }

    header: Item {
        id: headerItem;
        height: titleBar.height+42
        property int currentIndex: -1
        EbTitleBar {
            id: titleBar;
            width: parent.width
            iconVisible: false
            rightButton1Visible: true
            rightButton1Label: "\uf002"
            onPressedRightButton1: { requestSearch(); }
            rightButton0Label: "\uf067"
            rightButton0Visible: true
            onPressedRightButton0: {
                theTitleMenu.x = parent.width-theTitleMenu.width-10;
                theTitleMenu.y = titleBar.height;
                theTitleMenu.open();
                theTitleMenu.parent = rootPage;
            }
        }

        Rectangle {
            y: titleBar.height
            width: parent.width
            height: parent.height-titleBar.height-1
            Row {
                anchors.fill: parent
                EbIconButton {
                    id: tabButton0
                    width: parent.width/3;
                    icon: "\uf0c0"
//                    label: qsTr("部门群组")
                    onPressed: {
                        currentIndexChanged(0);
                    }
                }

                EbIconButton {
                    id: tabButton1
                    width: tabButton0.width
                    icon: "\uf007"
//                    label: qsTr("我的好友")
                    onPressed: {
                        currentIndexChanged(1);
                    }
                }
                EbIconButton {
                    id: tabButton2
                    width: tabButton0.width
                    icon: "\uf19c"
//                    label: qsTr("公司架构")
                    onPressed: {
                        currentIndexChanged(2);
                    }
                }

            }

            /// line
            Rectangle {
                y: parent.height-1
                width: parent.width
                height: 1
                color: "#ddd"
            }
        }
    }

    SwipeView {
        id: contactSwipeView
        anchors.fill: parent
//        y: headerItem.height
//        height: parent.height - headerItem.height
        currentIndex: headerItem.currentIndex

        EbPage {
            footerLineHeight: 0
            /// 解决显示主界面，页面显示
            visible: contactSwipeView.currentIndex==0?true:false
            EbGroupListItem {
                id: myGroupListItem
                anchors.fill: parent
                listViewType: 1
                onPressedGroupInfo: { rootPage.pressedGroupInfo(groupInfo); }
                onPressedLoadSubGroupInfo: { rootPage.pressedLoadSubGroupInfo(parentGroupInfo); }
                onRequestNewGroup: { rootPage.requestNewGroup(groupType, parentGroupId); }
            }
        }
        EbMyContactPage {
            id: myContactPage
            footerLineHeight: 0
            /// 解决显示主界面，页面显示
            visible: contactSwipeView.currentIndex==1?true:false
//            Label {
//                text: qsTr("contact 222")
//                anchors.centerIn: parent
//            }
            onRequestRenameUGinfo: { rootPage.requestRenameUGinfo(ugId, groupName); }
            onPressedContactInfo: { rootPage.pressedContactInfo(contactInfo); }
            onRequestDeleteContact: {rootPage.requestDeleteContact(contactInfo); }
        }
        EbPage {
            footerLineHeight: 0
            EbGroupListItem {
                id: myEnterpriseListItem
                anchors.fill: parent
                listViewType: 2
                onPressedGroupInfo: { rootPage.pressedGroupInfo(groupInfo); }
                onPressedLoadSubGroupInfo: { rootPage.pressedLoadSubGroupInfo(parentGroupInfo); }
                onRequestNewGroup: { rootPage.requestNewGroup(groupType, parentGroupId); }
            }
        }
        onCurrentIndexChanged: {
//            console.log("onCurrentIndexChanged: " + currentIndex);
            rootPage.currentIndexChanged(currentIndex);
        }
    }

    function updateLocaleInfo() {
        addContactMenuItem.label = theLocales.getLocalText("context-menu.add-contact.text", "New Contact");
        var label = theLocales.getLocalText("context-menu.new-group.text", "New Group");
        var groupTypeInfo = theLocales.getGroupTypeInfo(0);
        addGroup0.label = label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        groupTypeInfo = theLocales.getGroupTypeInfo(1);
        addGroup1.label = label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        groupTypeInfo = theLocales.getGroupTypeInfo(2);
        addGroup2.label = label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        addContactUserGroup.label = theLocales.getLocalText("context-menu.add-contact-group.text", "Add contact group");;
        tabButton0.label = theLocales.getLocalText("main-frame.mid-button-my-group.text", "My Group");
        tabButton1.label = theLocales.getLocalText("main-frame.mid-button-my-contact.text", "My Contact");
        tabButton2.label = theLocales.getLocalText("main-frame.mid-button-my-ent.text", "My Enterprise");
    }

    Component.onCompleted: {
        updateLocaleInfo();
        currentIndexChanged(0);
    }

    function currentIndexChanged(index) {
        if (index>=0 && headerItem.currentIndex!==index) {
            headerItem.currentIndex = index;
            tabButton0.checkedStateChanged(index===0?true:false);
            tabButton1.checkedStateChanged(index===1?true:false);
            tabButton2.checkedStateChanged(index===2?true:false);

            /// 公司组织架构才显示“新建部门”和“新建项目组”
            var canNewGroupInfo = theApp.canNewGroupInfo(-1);
            addGroup0.visible = (index===2 && canNewGroupInfo);
            addGroup1.visible = (index===2 && canNewGroupInfo);
//            addGroup1.visible = (index===2);
            /// 部门群组，显示“新建个人群组”
            /// 如果没有权限，创建部门，index===2 也显示新建个人群组，避免该菜单下没有内容
            addGroup2.visible = (index===0 || (index===2 && !canNewGroupInfo));
            /// 我的好友，显示添加分组
            addContactUserGroup.visible = index===1;
            if (index===0) {
                titleBar.title = theLocales.getLocalText("main-frame.mid-button-my-group.tooltip", "My Group");
            }
            else if (index===1) {
                titleBar.title = theLocales.getLocalText("main-frame.mid-button-my-contact.tooltip", "My Contact");
            }
            else if (index===2) {
                titleBar.title = theApp.enterpriseName(0);
//                titleBar.title = theLocales.getLocalText("main-frame.mid-button-my-ent.tooltip", "My Enterprise");
            }
        }
    }

}

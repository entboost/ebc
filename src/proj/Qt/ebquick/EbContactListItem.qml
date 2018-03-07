import QtQuick 2.0

Rectangle {
    id: rootItem
//    anchors.fill: parent
    property var contactUGInfo: null

    signal requestDeleteContact(var contactInfo);
    signal pressedContactInfo(var contactInfo);
    signal listCountChanged();

    function visibleHeight() {
        if (theModel.count==0) {
            theApp.loadUGContact(rootItem.contactUGInfo.ugId.toString());
        }
        return theModel.count*listItem.height;
    }

    EbMenu {
        id: theContactMenu
        EbMenuItem {
            id: deleteContactMenuItem
//            label: "删除联系人"
            onTriggered: {
                var obj = theModel.get(listView.currentIndex);
                if (obj==null) {
                    return;
                }
                requestDeleteContact(obj.contactInfo);
            }
        }
    }

    ListModel {
        id: theModel
//        ListElement { name: "同学"; file: "zh-CN.json" }
    }
    Component {
        id: myDelegate

        Rectangle {
            id: listItem
            color: "#fff"
            x: 1
            width: parent.width-x
            height: 46

            EbImage {
                id: imageIcon
                width: 38
                height: width
                y: (parent.height-height)/2
                x: y
//                width: parent.height
//                height: parent.height-1
                fillMode: Image.PreserveAspectFit
                source: theApp.contactHeadImage(contactInfo)
                grayImage: !contactInfo.isOnlineState
            }
            Text {
                id: textAccount
                x: imageIcon.x + imageIcon.width + 4
                width: parent.width-x///-rightIconText.width
                height: parent.height-1
                verticalAlignment: Text.AlignVCenter
                text: contactInfo.name
            }
//            Text {
//                id: rightIconText
//                z: 1
//                height: parent.height
//                width: 36;
//                x: parent.width-width
//                horizontalAlignment: Text.AlignHCenter
//                verticalAlignment: Text.AlignVCenter
//                font.family: "FontAwesome"
//                text: "\uf107"
//                color: "#999"
//                font.pointSize: 14
//                visible: true

//                property bool pressedReleased: false
//                Timer {
//                    id: rightIconDelayPressTimer
//                    interval: 200
//                    onTriggered: {
//                        if (rightIconText.pressedReleased) {
//                            if (groupListItem.height==0) {
//                                groupListItem.height = groupListItem.visibleHeight();
//                            }
//                            else {
//                                groupListItem.height = 0;
//                            }
//                            columnItem.height = listItem.height + groupListItem.height;
//                        }
//                    }
//                }
//                MouseArea {
//                    anchors.fill: parent
//                    onPressed: {
//                        rightIconText.pressedReleased = false;
//                        rightIconDelayPressTimer.start();
//                    }
//                    onReleased: {rightIconText.pressedReleased = true; }
//                }
//            }
            Rectangle {
                /// line
                x: 0
                y: parent.height-1
                width: parent.width
                height: 1
                color: "#eee"
            }

            EbTimerPress {
                id: delayPressTimer
                onPressed: { rootItem.pressedContactInfo(contactInfo); }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
//                width: parent.width-rightIconText.width
//                height: parent.height
                onPressed: {
                    listItem.color = "#eee";
                    delayPressTimer.initStart();
                }
                onReleased: {
                    listItem.color = "#fff";
                    delayPressTimer.pressedReleased = true;
                }
                onExited: {
                    listItem.color = "#fff";
                }
                onPressAndHold: {
                    mouse.accepted = true;
                    listView.currentIndex = index;
                    /// 计算弹出菜单位置
                    var obj = mouseArea.mapToGlobal(mouse.x, mouse.y);
                    obj = rootItem.mapFromGlobal(obj.x, obj.y);
                    theContactMenu.x = obj.x-theContactMenu.width+5;
                    theContactMenu.y = obj.y-30;
                    theContactMenu.open();
                }

            }
        }

    }
    ListView {
        id: listView
        anchors.fill: parent
        model: theModel
        delegate: myDelegate
    }
    Timer {
        id: delaySendCountChanged
        interval: 1000
        onTriggered: { rootItem.listCountChanged(); }
    }

    function delContactInfoModelData(contactId) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.contactInfo==null) {
                return;
            }

            if (obj.contactInfo.contactIdcontactId) {
                theModel.remove(i);
                delaySendCountChanged.stop();
                delaySendCountChanged.start();
                return;
            }
        }
    }
    function setContactInfoModelData(contactInfo) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.contactInfo==null) {
                return;
            }

            if (obj.contactInfo.contactId==contactInfo.contactId) {
                /// 找到现存部门，直接更新（后期再显示重新排序问题）
                theModel.set(i, { contactInfo: contactInfo } );
                return;
            }
            else if (theApp.compareContactInfo(obj.contactInfo, contactInfo)) {
//            else if (contactInfo.groupName<obj.contactInfo.groupName) {
                /// 查找后面删除存在数据
                /// 名称小排前面，按UTF-8编码排
                theModel.insert(i, { contactInfo: contactInfo });
                var findRemove = false;
                for (var j=i+1 ; j<theModel.count; j++) {
                    obj = theModel.get(j);
                    if (obj.contactInfo==null) {
                        break;
                    }
                    if (obj.contactInfo.contactId==contactInfo.contactId) {
                        theModel.remove(j);
                        findRemove = true;
                        break;
                    }
                }
                if (!findRemove) {
                    delaySendCountChanged.stop();
                    delaySendCountChanged.start();
                }
                return;
            }
        }
        /// 前面找不到，直接添加
        theModel.append( { contactInfo: contactInfo } );
        delaySendCountChanged.stop();
        delaySendCountChanged.start();
    }

    Connections {
        target: theApp
        onUgInfoDeleted: {
            if (rootItem.contactUGInfo.ugId==0) {
                /// 默认分组，加载多一次数据
                theApp.loadUGContact(0);
            }
        }
        onReceivedContactInfo: {
            if (contactInfo.ugId!=rootItem.contactUGInfo.ugId) {
                /// 删除可能移到其他分组好友
                delContactInfoModelData(contactInfo.contactId);
                return;
            }
            setContactInfoModelData(contactInfo);
        }
        onContactLineStateChanged: {
            if (contactInfo.ugId!=rootItem.contactUGInfo.ugId) {
                return;
            }
            setContactInfoModelData(contactInfo);
        }
        onContactDeleted: {
            if (contactInfo.ugId!=rootItem.contactUGInfo.ugId) {
                return;
            }
            delContactInfoModelData(contactInfo.contactId);
        }
    }

    function updateLocaleInfo() {
        deleteContactMenuItem.label = theLocales.getLocalText("context-menu.delete-contact.text", "Delete contact");
    }

    Component.onCompleted: {
        updateLocaleInfo();
    }
}

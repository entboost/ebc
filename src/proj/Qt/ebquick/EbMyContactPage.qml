import QtQuick 2.7
import QtQuick.Controls 2.2
import Entboost.UGInfo 1.0

EbPage {
    id: rootPage
    footerLineHeight: 0

    signal pressedContactInfo(var contactInfo);
    signal requestDeleteContact(var contactInfo);
    signal requestRenameUGinfo(var ugId, var groupName);

    function clearData() {
        theModel.clear();
    }

    EbMenu {
        id: theContactMenu
        EbMenuItem {
            id: renameUGInfo
//            label: "修改分组"
            onTriggered: {
                var obj = theModel.get(listView.currentIndex);
                if (obj!=null) {
                    requestRenameUGinfo(obj.ugInfo.ugId,
                                        obj.ugInfo.groupName);
                }
            }
        }
        EbMenuSeparator {
            id: removeMemberSeparator
        }
        EbMenuItem {
            id: deleteUGInfo
//            label: "删除该组"
            onTriggered: {
                var obj = theModel.get(listView.currentIndex);
                if (obj==null) {
                    return;
                }

                var title = theLocales.getLocalText("message-box.delete-contact-group.title","移除成员");
                if (title.length==0) {
                    title = theApp.productName;
                }
                var hasUgContact = theApp.hasUGContact(obj.ugInfo.ugId.toString());
                var text;
                if (hasUgContact)
                    text = theLocales.getLocalText("message-box.delete-contact-group.text","Confirm delete contact group?");
                else
                    text = theLocales.getLocalText("message-box.delete-contact-empty-group.text","Confirm delete const group?");
                text = text.replace( "[GROUP_NAME]", obj.ugInfo.groupName);
                theMessageBoxLoadItem.messageBoxType = 1;
                theMessageBoxLoadItem.visible = true;
                theMessageBoxLoader.setSource("EbMessageDialog.qml", {"title": title, "text": text});
                theMessageBoxLoader.item.open();
            }
        }
    }

    Item {
        id: theMessageBoxLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: theMessageBoxLoader
            anchors.fill: parent
        }
        /// 1=删除分组
        property int messageBoxType: 0
        Connections {
            target: theMessageBoxLoader.item
            onAccepted: {
//                tipText.text = "";
                theMessageBoxLoader.source = "";
                theMessageBoxLoadItem.visible = false;
                if (theMessageBoxLoadItem.messageBoxType==1) {
                    var obj = theModel.get(listView .currentIndex);
                    if (obj==null) {
                        return;
                    }
                    theApp.deleteUGInfo(obj.ugInfo.ugId.toString());
                }
            }
            onRejected: {
                theMessageBoxLoader.source = "";
                theMessageBoxLoadItem.visible = false;
            }
        }
    }


    ListModel {
        id: theModel
//        ListElement { name: "同学"; file: "zh-CN.json" }
    }
    Component {
        id: myDelegate

        Column {
            id: columnItem
            width: parent.width
            height: listItem.height

            Rectangle {
                id: listItem
                color: "#fff"
                width: parent.width
                height: 46

                Text {
                    id: iconText
                    height: parent.height-1
                    width: height;
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.family: "FontAwesome"
                    text: theContactListItem.visible?"\uf107":"\uf105"
                    color: "#999"
                    font.pointSize: 14
                    visible: true
//                    visible: theApp.hasUGContact(ugId.toString())?true:false
                }
                Text {
                    id: groupNameText
                    x: iconText.width
                    width: parent.width-x
                    height: parent.height-1
                    verticalAlignment: Text.AlignVCenter
                    text: ugInfo.groupFullName
//                    text: theApp.ugFullName(ugInfo.ugId.toString(), groupName)
                }
//                Text {
//                    id: rightIconText
//                    z: 1
//                    height: parent.height
//                    width: 36;
//                    x: parent.width-width
//                    horizontalAlignment: Text.AlignHCenter
//                    verticalAlignment: Text.AlignVCenter
//                    font.family: "FontAwesome"
//                    text: "\uf107"
//                    color: "#999"
//                    font.pointSize: 14
//                    visible: true

//                    property bool pressedReleased: false
//                    Timer {
//                        id: rightIconDelayPressTimer
//                        interval: 200
//                        onTriggered: {
//                            if (rightIconText.pressedReleased) {
//                                if (theContactListItem.visible) {
//                                    theContactListItem.visible = false;
//                                    columnItem.height = listItem.height;
//                                }
//                                else {
//                                    theContactListItem.height = theContactListItem.visibleHeight();
//                                    theContactListItem.visible = true;
//                                    columnItem.height = listItem.height + theContactListItem.height;
//                                }

//                            }
//                        }
//                    }
//                    MouseArea {
//                        anchors.fill: parent
//                        onPressed: {
//                            rightIconText.pressedReleased = false;
//                            rightIconDelayPressTimer.start();
//                        }
//                        onReleased: {rightIconText.pressedReleased = true; }
//                    }
//                }
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
                    onPressed: {
                        if (theContactListItem.visible) {
                            theContactListItem.visible = false;
                            theContactListItem.height = 0;
                            columnItem.height = listItem.height;
                        }
                        else {
                            theContactListItem.height = theContactListItem.visibleHeight();
                            theContactListItem.visible = true;
                            columnItem.height = listItem.height + theContactListItem.height;
                        }
                    }
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
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
                        if (ugInfo.ugId==0) {
                            /// 0 默认分组，不处理
                            return;
                        }
                        mouse.accepted = true;
                        listView.currentIndex = index;
                        /// 计算弹出菜单位置
                        var obj = mouseArea.mapToGlobal(mouse.x, mouse.y);
                        obj = rootPage.mapFromGlobal(obj.x, obj.y);
                        theContactMenu.x = obj.x-theContactMenu.width+5;
                        theContactMenu.y = obj.y-30;
                        theContactMenu.open();
                    }

                }
            }

            EbContactListItem {
                id: theContactListItem
                width: parent.width
                height: 20
                contactUGInfo: ugInfo
                visible: false
                onListCountChanged: {
//                    console.log("*************** onListCountChanged........");
//                    groupNameText.text = ugInfo.groupFullName;
                    groupNameText.text = theApp.ugFullName(
                                theContactListItem.contactUGInfo.ugId.toString(),
                                theContactListItem.contactUGInfo.groupName);
                    if (theContactListItem.visible) {
                        theContactListItem.height = theContactListItem.visibleHeight();
                        columnItem.height = listItem.height + theContactListItem.height;
                    }
                }
                onPressedContactInfo: { rootPage.pressedContactInfo(contactInfo); }
                onRequestDeleteContact: { rootPage.requestDeleteContact(contactInfo); }
            }

        }

    }
    ListView {
        id: listView
        anchors.fill: parent
        model: theModel
        delegate: myDelegate
    }

    function delUGInfoModelData(ugInfo) {
        /// 第 0 个是默认分组
        for (var i=1; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.ugInfo.ugId==ugInfo.ugId) {
                theModel.remove(i);
                return;
            }
        }
    }
    function getUGInfoModelData(ugId) {
        /// 第 0 个是默认分组，也需要查找
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.ugInfo.ugId==ugId) {
                return obj.ugInfo;
            }
        }
        return null;
    }
    function setUGInfoModelData(ugInfo) {
        /// 第 0 个是默认分组
        for (var i=1; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.ugInfo.ugId==ugInfo.ugId) {
                /// 找到现存部门，直接更新（后期再显示重新排序问题）
                theModel.set(i, { ugInfo: ugInfo } );
                return;
            }
            else if (ugInfo.groupName<obj.ugInfo.groupName) {
                /// 名称小排前面，按UTF-8编码排
                theModel.insert(i, { ugInfo: ugInfo } );
                /// 查找后面删除存在数据
                for (var j=i+1; j<theModel.count; j++) {
                    obj = theModel.get(j);
                    if (obj.ugInfo.ugId==ugInfo.ugId) {
                        theModel.remove(j);
                        break;
                    }
                }
                return;
            }
        }
        /// 前面找不到，直接添加
        theModel.append( { ugInfo: ugInfo } );
    }

    Connections {
        target: theApp
        onContactLineStateChanged: {
            var ugInfo = getUGInfoModelData(contactInfo.ugId);
            if (ugInfo!=null) {
                ugInfo.groupFullName = theApp.ugFullName(
                            ugInfo.ugId.toString(), ugInfo.groupName);
            }
        }

        onReceivedContactInfo: {
            if (theModel.count==0) {
                var defaultInfo = theApp.buildUGInfo(0, theLocales.getLocalText("name-text.default-group-name", "Default group name"));
                theModel.append( { ugInfo: defaultInfo } );
            }
            var ugInfo = getUGInfoModelData(contactInfo.ugId);
            if (ugInfo==null) {
                ugInfo = theApp.buildUGInfo(
                            contactInfo.ugId.toString(),
                            contactInfo.groupName);
                if (ugInfo.groupName.length==0) {
                    ugInfo.groupName = theLocales.getLocalText("name-text.default-group-name", "Default group name");
                }
                setUGInfoModelData(ugInfo);
            }
        }
        onReceivedUGInfo: {
            if (theModel.count==0) {
                var defaultInfo = theApp.buildUGInfo(0, theLocales.getLocalText("name-text.default-group-name", "Default group name"));
                theModel.append( { ugInfo: defaultInfo } );
            }
            setUGInfoModelData(ugInfo);
        }
        onUgInfoDeleted: {
            delUGInfoModelData(ugInfo);
        }
    }

    function updateLocaleInfo() {
        renameUGInfo.label = theLocales.getLocalText("context-menu.rename-contact-group.text", "Rename contact group");
        deleteUGInfo.label = theLocales.getLocalText("context-menu.delete-contact-group.text", "Delete contact group");
    }
    Component.onCompleted: {
        updateLocaleInfo();
//        var defaultInfo = theApp.buildUGInfo(0, theLocales.getLocalText("name-text.default-group-name", "Default group name"));
//        theModel.append( { ugInfo: defaultInfo } );
//        theModel.append( { ugId: 0, groupName: "默认分组" } );
    }

}

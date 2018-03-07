import QtQuick 2.0
import QtQuick.Controls 1.4
import Entboost.KeyFilter 1.0

/// 上面一个标题栏
/// 中间一个显示用户好友分组列表
/// 和下面一个确定按钮
Rectangle {
    id: rootItem
    anchors.fill: parent
    z: 100

    signal accepted(var ugId, var groupName);
    signal returnBack();

    property var contactInfo: null
    function init(contactInfo) {
        rootItem.contactInfo = contactInfo;
        if (theModel.count==0) {
            theApp.loadAllUGInfo(0);
        }
        titleBar.title = contactInfo.name + ": " +
                theLocales.getLocalText("context-menu.move-contact-to.text", "Move contact to");
    }

    EbTitleBar {
        id: titleBar;
        width: parent.width
        onReturnBack: rootItem.returnBack()
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
                height: 42

                Text {
                    id: iconText
                    height: parent.height-1
                    width: 32
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.family: "FontAwesome"
                    text: "\uf00c"
                    color: "#999"
                    visible: rootItem.contactInfo.ugId==ugId?true:false
                }

                Text {
                    id: groupNameText
                    x: iconText.width+2
                    width: parent.width-x
                    height: parent.height-1
                    verticalAlignment: Text.AlignVCenter
                    text: groupName
                }
                Rectangle {
                    /// line
                    x: 0
                    y: parent.height-1
                    width: parent.width
                    height: 1
                    color: "#eee"
                }

//                property bool pressedReleased: false
//                Timer {
//                    id: delayPressTimer
//                    interval: 150
//                    onTriggered: {
//                        if (listItem.pressedReleased) {
//                            if (ugId!=rootItem.contactInfo.ugId) {
//                                var contactInfo = rootItem.contactInfo;
//                                contactInfo.ugId = ugId;
//                                contactInfo.groupName = groupName;
//                                theApp.editContactInfo(contactInfo);
////                                rootItem.accepted(ugId, groupName);
//                            }
//                        }
//                    }
//                }
                EbTimerPress {
                    id: delayPressTimer
                    onPressed: {
                        if (ugId!=rootItem.contactInfo.ugId) {
                            theApp.moveContactToByContactId(
                                        contactInfo.contactId.toString(),
                                        ugId.toString());
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

                }
            }


        }

    }
    ListView {
        id: listView
        y: titleBar.height
        width: parent.width
        height: parent.height-y
//        anchors.fill: parent
        model: theModel
        delegate: myDelegate
    }

    function delUGInfoModelData(ugInfo) {
        /// 第 0 个是默认分组
        for (var i=1; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.ugId==ugInfo.ugId) {
                theModel.remove(i);
                return;
            }
        }
    }
    function setUGInfoModelData(ugInfo) {
        /// 第 0 个是默认分组
        for (var i=1; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.ugId==ugInfo.ugId) {
                /// 找到现存部门，直接更新（后期再显示重新排序问题）
                theModel.set(i, { ugId: ugInfo.ugId, groupName: ugInfo.groupName } );
                return;
            }
            else if (ugInfo.groupName<obj.groupName) {
                /// 名称小排前面，按UTF-8编码排
                theModel.insert(i, { ugId: ugInfo.ugId, groupName: ugInfo.groupName } );
                /// 查找后面删除存在数据
                for (var j=i+1; j<theModel.count; j++) {
                    obj = theModel.get(j);
                    if (obj.ugId==ugInfo.ugId) {
                        theModel.remove(j);
                        break;
                    }
                }
                return;
            }
        }
        /// 前面找不到，直接添加
        theModel.append( { ugId: ugInfo.ugId, groupName: ugInfo.groupName } );
    }
    Connections {
        target: theApp
        onLoadUGInfoResult: {
//            console.log("********** onLoadUGInfoResult, groupName=" + ugInfo.groupName)
            if (theModel.count==0) {
                theModel.append( { ugId: "0", groupName: theLocales.getLocalText("name-text.default-group-name", "Default group name") } );
            }
            setUGInfoModelData(ugInfo);
        }
        onUgInfoDeleted: {
            delUGInfoModelData(ugInfo);
        }
        onReceivedContactInfo: {
            if (contactInfo.contactId==rootItem.contactInfo.contactId) {
                rootItem.accepted(contactInfo.ugId, contactInfo.groupName);
                return;
            }
        }
    }

    /// 延时返回退出
    Timer {
        id: timerReturnBack
        interval: 100
        onTriggered: {
            rootItem.returnBack();
        }
    }
    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            timerReturnBack.start();
        }
    }
    Component.onCompleted: {
        var docRoot = rootItem.parent;
        while (docRoot.parent) {
            docRoot = docRoot.parent;
        }
        theKeyFilter.setFilter(docRoot, 1);
    }
}

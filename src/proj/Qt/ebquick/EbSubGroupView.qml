import QtQuick 2.0
import Entboost.KeyFilter 1.0

/// 用于显示下级部门列表
Rectangle {
    id: rootItem
    anchors.fill: parent
    z: 100

    signal returnBack();
    signal sendMemberMessage(var memberInfo);
    signal sendGroupMessage(var groupInfo);

    property var parentGroupInfo: null
    property bool inited: false
    function setGroupInfo(parentGroupInfo) {
        rootItem.parentGroupInfo = parentGroupInfo;
        var count = theGroupListItem.loadSubGroupInfo(parentGroupInfo);
        titleBar.title = parentGroupInfo.groupName+"-"+
                theLocales.getLocalText("group-info-dialog.label-sub-group.label", "Sub Group")+
                "("+count+")";
        rootItem.inited = true;
    }
    /// 请求隐藏子窗口
    function requestReturnBack() {
        if (inputViewLoadItem.visible) {
            inputViewLoader.source = "";
            inputViewLoadItem.visible = false;
            return false;
        }
        if (theViewLoadItem.visible) {
            if (!theViewLoader.item.requestReturnBack()) {
                return false;
            }
            theViewLoadItem.visible = false;
            theViewLoader.source = "";
            return false;
        }
        return true;
    }

    Item {
        id: theViewLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: theViewLoader
            anchors.fill: parent
        }
        Connections {
            target: theViewLoader.item
            onReturnBack: {
                if (requestReturnBack()) {
                    rootItem.returnBack();
                }
            }
            onSendMemberMessage: {
                requestReturnBack();
                rootItem.sendMemberMessage(memberInfo);
            }
            onSendGroupMessage: {
                requestReturnBack();
                rootItem.sendGroupMessage(groupInfo);
            }
        }
    }
    Item {
        id: inputViewLoadItem
        anchors.fill: parent
        visible: false
        z: 100

        property var parentGroupId: null
        property int groupType: 0
        /// 1=新建部门/项目组/个人群组
        property int inputType: 0
        Loader {
            id: inputViewLoader
            anchors.fill: parent
        }
        Connections {
            target: inputViewLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onAccepted: {
                requestReturnBack();
//                console.log("onAccepted text=" + text);
                if (inputViewLoadItem.inputType===1) {
                    /// 新建部门/项目组/个人群组
                    theApp.newGroupInfo(inputViewLoadItem.groupType,
                                        text,
                                        0,
                                        inputViewLoadItem.parentGroupId.toString());
                }
            }
        }
    }

    EbTitleBar {
        id: titleBar;
        width: parent.width
        onReturnBack: {
            while (!requestReturnBack()) {
            }
            rootItem.returnBack();
        }
    }

    EbGroupListItem {
        id: theGroupListItem
        y: titleBar.height
        width: parent.width
        height: parent.height-1
//        anchors.fill: parent
        listViewType: 3
        onRequestNewGroup: {
            if (groupType<=1) {
                /// 0=新建部门
                /// 1=新建项目组
                /// 打开新建提示对话框
                var acceptButtonLabel = theLocales.getLocalText("base-dialog.new-button.text", "New");
                var groupTypeInfo = theLocales.getGroupTypeInfo(groupType);
                var title = acceptButtonLabel+groupTypeInfo.groupTypeName;
                var placeholderText = theLocales.getLocalText("group-info-dialog.edit-group-name.bg-text","");
                if (placeholderText.length>0) {
                    placeholderText = placeholderText.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
                }
                inputViewLoader.setSource("EbInputView.qml",
                                          {"title": title,
                                              "placeholderText": placeholderText,
                                              "acceptButtonLabel": acceptButtonLabel});
                inputViewLoadItem.inputType = 1;
                inputViewLoadItem.groupType = groupType;
                inputViewLoadItem.parentGroupId = parentGroupId;
                inputViewLoadItem.visible = true;
            }
        }

        onPressedGroupInfo: {
            theViewLoader.source = "EbGroupInfoView.qml";
            theViewLoader.item.setGroupInfo(groupInfo);
            theViewLoadItem.visible = true;
        }
        onPressedLoadSubGroupInfo: {
            theViewLoader.source = "EbSubGroupView.qml";
            theViewLoader.item.setGroupInfo(parentGroupInfo);
            theViewLoadItem.visible = true;
        }
        onGroupCoundChanged: {
            if (!rootItem.inited) {
                return;
            }
            titleBar.title = rootItem.parentGroupInfo.groupName+"-"+
                    theLocales.getLocalText("group-info-dialog.label-sub-group.label", "Sub Group")+
                    "("+count+")";

        }
    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            if (requestReturnBack()) {
                rootItem.returnBack();
            }
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

import QtQuick 2.0

Item {
    id: rootItem

    signal returnBack();
    signal pressedGroupInfo(var groupInfo);
    signal pressedLoadSubGroupInfo(var parentGroupInfo);
    signal requestNewGroup(var groupType, var parentGroupId);
    signal groupCoundChanged(var count);

    /// 显示类型
    /// 1=我的部门/群组
    /// 2=公司组织架构
    /// 3=子部门
    property int listViewType: 0

    function clearData() {
        theModel.clear();
    }

    property var parentGroupInfo: null
    function loadSubGroupInfo(parentGroupInfo) {
        theModel.clear();
        rootItem.listViewType = 3;
        rootItem.parentGroupInfo = parentGroupInfo;
        var count = theApp.loadSubGroupInfo(parentGroupInfo.groupId.toString());
        return count;
    }
    function visibleHeight() {
        return theModel.count*listItem.height;
    }

    /// 请求隐藏子窗口
    function requestReturnBack() {
//        if (theViewLoadItem.visible) {
//            if (!theViewLoader.item.requestReturnBack()) {
//                return false;
//            }
//            theViewLoadItem.visible = false;
//            theViewLoader.source = "";
//        }
        return true;
    }

//    Item {
//        id: groupInfoViewLoadItem
//        anchors.fill: parent
//        visible: false
//        z: 100
//        Loader {
//            id: groupInfoViewLoader
//            anchors.fill: parent
//        }
//        Connections {
//            target: groupInfoViewLoader.item
//            onReturnBack: {
//                listView.visible = true;
//                groupInfoViewLoadItem.visible = false;
//            }
//        }
//    }

//    Item {
//        id: theViewLoadItem
//        anchors.fill: parent
//        visible: false
//        z: 101
//        Loader {
//            id: theViewLoader
//            anchors.fill: parent
//        }
////        property int viewType: 0
//        Connections {
//            target: theViewLoader.item
//            onReturnBack: {
//                requestReturnBack();
//            }
//        }
//    }

    EbMenu {
        id: theGroupMenu
        EbMenuItem {
            id: newSubGroup0
//            label: "新建下级部门"
            onTriggered: {
                var obj = theModel.get(listView.currentIndex);
                if (obj==null) {
                    return;
                }
                rootItem.requestNewGroup(0, obj.groupInfo.groupId);
            }
        }
        EbMenuItem {
            id: newSubGroup1
//            label: "新建下级项目组"
            onTriggered: {
                var obj = theModel.get(listView.currentIndex);
                if (obj==null) {
                    return;
                }
                rootItem.requestNewGroup(1, obj.groupInfo.groupId);
            }
        }
    }
    ListModel {
        id: theModel
//        ListElement { name: "中文简体"; file: "zh-CN.json" }
    }
    Component {
        id: myDelegate

        Rectangle {
            id: listItem
            color: "#fff"
            x: 1
            width: parent.width
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
                source: groupInfo.groupImagePath
            }
            Text {
                id: textAccount
                x: imageIcon.x + imageIcon.width + 4
                width: parent.width-x-rightIconText.width
                height: parent.height-1
                verticalAlignment: Text.AlignVCenter
                text: groupInfo.groupFullName
//                text: theApp.groupFullName(groupInfo.groupId.toString(),
//                                           groupInfo.groupName,
//                                           groupInfo.memberCount)
//                text: groupInfo.groupName+" [" + groupInfo.memberCount + "]"
            }
            Text {
                id: rightIconText
                z: 1
                height: parent.height
                /// 暂时不显示
                width: 0
//                width: theApp.hasSubGroup(groupInfo.groupId.toString())?36:0
                x: parent.width-width
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.family: "FontAwesome"
                text: "\uf107"
                color: "#999"
                font.pointSize: 14
                visible: width>0?true:false

                EbTimerPress {
                    id: rightIconDelayPressTimer
                    onPressed: { rootItem.pressedLoadSubGroupInfo(groupInfo); }
                }
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        rightIconDelayPressTimer.initStart();
                    }
                    onReleased: {rightIconDelayPressTimer.pressedReleased = true; }
                }
            }
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
                onPressed: { rootItem.pressedGroupInfo(groupInfo); }
            }
            MouseArea {
                id: mouseArea
                width: parent.width-rightIconText.width
                height: parent.height
//                anchors.fill: parent
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
                    if (groupInfo.groupType!=0) {
                        /// 非部门类型，不创建下级
                        return;
                    }
                    else if (!theApp.canEditGroupInfo(groupInfo.groupId.toString())) {
                        /// 没有权限创建下级部门，下级项目组
                        return;
                    }
                    /// 部门类型，可以创建下级
                    mouse.accepted = true;
                    listView.currentIndex = index;
                    /// 计算弹出菜单位置
                    var obj = mouseArea.mapToGlobal(mouse.x, mouse.y);
                    obj = rootItem.mapFromGlobal(obj.x, obj.y);
                    theGroupMenu.x = obj.x-theGroupMenu.width+5;
                    theGroupMenu.y = obj.y-30;
                    theGroupMenu.open();
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
    function delGroupInfoFromModel(groupId) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.groupInfo==null) {
                return;
            }
            if (obj.groupInfo.groupId==groupId) {
                theModel.remove(i);
                rootItem.groupCoundChanged(theModel.count);
                break;
            }
        }
    }
    function getGroupInfoFromModel(groupId) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.groupInfo==null) {
                return null;
            }
            if (obj.groupInfo.groupId==groupId) {
                return obj.groupInfo;
            }
        }
        return null;
    }

    Timer {
        id: delaySendCountChanged
        interval: 1000
        onTriggered: { rootItem.groupCoundChanged(theModel.count); }
    }

    function setGroupInfoToModel(groupInfo) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.groupInfo==null) {
                return;
            }

            if (obj.groupInfo.groupId==groupInfo.groupId) {
                /// 找到现存部门，直接更新（后期再显示重新排序问题）
                theModel.set(i, { groupInfo: groupInfo } );
                return;
            }
            else if (groupInfo.displayIndex>obj.groupInfo.displayIndex ||
                     groupInfo.groupType<obj.groupInfo.groupType ||
                     (groupInfo.groupType==obj.groupInfo.groupType &&
                      groupInfo.groupName<obj.groupInfo.groupName)) {
                /// 1=部门类型排前面
                /// 2=相同群组类型，名称小排前面，按UTF-8编码排
                theModel.insert(i, { groupInfo: groupInfo });
                /// 查找后面删除存在数据
                var findRemove = false;
                for (var j=i+1; j<theModel.count; j++) {
                    obj = theModel.get(j);
                    if (obj.groupInfo==null) {
                        break;
                    }
                    if (obj.groupInfo.groupId==groupInfo.groupId) {
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
        theModel.append( { groupInfo: groupInfo } );
        delaySendCountChanged.stop();
        delaySendCountChanged.start();
    }

    Connections {
        target: theApp
        onMemberLineStateChanged: {
            var groupInfo = getGroupInfoFromModel(memberInfo.groupId);
            if (groupInfo!=null) {
                groupInfo.groupFullName = theApp.groupFullName(groupInfo.groupId.toString(),
                                                               groupInfo.groupName,
                                                               groupInfo.memberCount);
            }

        }

        onDeletedGroupId: {
            delGroupInfoFromModel(groupId);
        }
        onReceivedGroupInfo: {
            if (listViewType==1 && groupInfo.myMemberId==0) {
                /// 我的部门，其他部门不显示
                return;
            }
            else if (listViewType==2 &&
                     (groupInfo.groupType>=2 || groupInfo.parentId!=0)) {
                /// 公司部门，个人群组不显示
                /// 公司部门，这里只显示根部门，子部分在其他页面显示
                return;
            }
            else if (listViewType==3) {
                /// 下级部门不在这里处理，在 onReceivedSubGroupInfo 处理
                return;
            }

            setGroupInfoToModel(groupInfo);
        }
        onReceivedSubGroupInfo: {
            if (parentGroupInfo==null ||
                    parentGroupInfo.groupId!=parentGroupId ||
                    listViewType!=3) {
                return
            }
            setGroupInfoToModel(subGroupInfo);
        }

    }
    function updateLocaleInfo() {
        var newSubGroupLabel = theLocales.getLocalText("context-menu.new-sub-group.text", "New sub group");
        var groupType0Info = theLocales.getGroupTypeInfo(0);
        var groupType1Info = theLocales.getGroupTypeInfo(1);
        newSubGroup0.label = newSubGroupLabel.replace("[GROUP_TYPE_NAME]", groupType0Info.groupTypeName);
        newSubGroup1.label = newSubGroupLabel.replace("[GROUP_TYPE_NAME]", groupType1Info.groupTypeName);
    }

    Component.onCompleted: {
        updateLocaleInfo();
    }

}

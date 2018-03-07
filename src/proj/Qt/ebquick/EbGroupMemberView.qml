import QtQuick 2.0
import QtQuick.Controls 2.2
import Entboost.MemberInfo 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent

    signal returnBack();
    signal sendMemberMessage(var memberInfo);

    function requestReturnBack() {
        if (theMessageBoxLoadItem.visible) {
            theMessageBoxLoadItem.visible = false;
            theMessageBoxLoader.source = ""
            return false;
        }
        if (theViewLoadItem.visible) {
            theViewLoadItem.visible = false;
            theViewLoader.source = ""
            return false;
        }
        return true;
    }

    property bool waittingGroupEditResponse: false
    property var groupInfo: null
    property var groupTypeInfo: null
    property bool inited: false
    property bool addMemberButtonViewed: false
    function setGroupInfo(groupInfo) {
        theModel.clear();
        tipText.text = "";
        rootItem.groupInfo = groupInfo;
        theApp.loadGroupMemberInfo(groupInfo.groupId.toString());
        groupTypeInfo = theLocales.getGroupTypeInfo(groupInfo.groupType);
        setGridViewHeight();

        var label = theLocales.getLocalText("group-info-dialog.set-manager.label","Set Manager");
        setGroupManager.label = label.replace("[GROUP_TYPE_MANAGER]", groupTypeInfo.groupTypeManager);
        var temp;
        var canExitGroup = theApp.canExitGroupInfo(groupInfo.groupId.toString(), groupInfo.groupType);
        exitGroupButton.visible = canExitGroup;
        exitGroupLine.visible = canExitGroup;
        if (canExitGroup) {
            temp = theLocales.getLocalText("context-menu.exit-group.text", "Exit Group");
            exitGroupButton.label = temp.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        }
        var canDeleteGroup = theApp.canDeleteGroupInfo(groupInfo.groupId.toString());
        deleteGroupButton.visible = canDeleteGroup;
        deleteGroupLine.visible = canDeleteGroup;
        if (canDeleteGroup) {
            temp = theLocales.getLocalText("context-menu.delete-group.text", "Delete Group");
            deleteGroupButton.label = temp.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        }
        if (theApp.isGroupAdminLevel(groupInfo.groupId.toString())) {
            var isGroupForbidSpeech = theApp.isGroupForbidSpeech(groupInfo.groupId.toString());
            addForbidSpeechGroupButton.visible = !isGroupForbidSpeech;
            deleteForbidSpeechGroupButton.visible = isGroupForbidSpeech;
        }
        else {
            addForbidSpeechGroupButton.visible = false;
            deleteForbidSpeechGroupButton.visible = false;
        }
        if (groupInfo.enterpriseId>0 && theApp.canNewMemberInfo(groupInfo.groupId)) {
            theModel.append( { addType: 1, memberInfo: null } );
            addMemberButtonViewed = true;
        }
        else {
            addMemberButtonViewed = false;
        }

        rootItem.inited = true;
        memberCountChanged();
    }
    /// 判断员工ID是否存在
    function exitMemberId(memberId) {
        var result = false;
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.memberInfo==null) {
                break;
            }
            if (obj.memberInfo.memberId==memberId) {
                result = true;
                break;
            }
        }
        return result;
    }


    Item {
        /// 部门员工
        id: theViewLoadItem
        anchors.fill: parent
        visible: false
        z: 101
        Loader {
            id: theViewLoader
            anchors.fill: parent
        }
        Connections {
            target: theViewLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onNewMemberResponse: {
                requestReturnBack();
                if (stateCode==0) {
                    /// 下面有问题，直接重新刷新一次
                    theModel.clear();
                    setGroupInfo(rootItem.groupInfo);
                }
//                if (stateCode==0 && !exitMemberId(memberInfo.memberId)) {
//                    theModel.insert(theModel.count-1, {addType: 0, memberInfo: memberInfo } );
//                }
            }
            onSendMemberMessage: {
                requestReturnBack();
                rootItem.sendMemberMessage(memberInfo);
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
        /// 1=移除成员
        /// 2=退出群组
        /// 3=解散群组
        property int messageBoxType: 0
        Connections {
            target: theMessageBoxLoader.item
            onAccepted: {
                tipText.text = "";
                //                waittingGroupEditResponse = true;
                theMessageBoxLoader.source = "";
                theMessageBoxLoadItem.visible = false;
                if (theMessageBoxLoadItem.messageBoxType==1) {
                    var obj = theModel.get(theGridView.currentIndex);
                    if (obj==null) {
                        return;
                    }
                    theApp.deleteMemberInfo(obj.memberInfo.groupId.toString(),
                                            obj.memberInfo.memberUserId.toString());
                }
                else if (theMessageBoxLoadItem.messageBoxType==2) {
//                    console.log("************ exitGroupInfo1 groupId=" + rootItem.groupInfo.groupId);
//                    console.log("************ exitGroupInfo2 groupId=" + rootItem.groupInfo.groupId.toString());
                    theApp.exitGroupInfo(rootItem.groupInfo.groupId.toString());
                }
                else if (theMessageBoxLoadItem.messageBoxType==3) {
                    theApp.deleteGroupInfo(rootItem.groupInfo.groupId.toString());
                }
            }
            onRejected: {
                theMessageBoxLoader.source = "";
                theMessageBoxLoadItem.visible = false;
            }
        }
    }

    EbMenu {
        id: theMemberMenu
        EbMenuItem {
            id: setGroupManager
//            label: "设为部门经理"
            onTriggered: {
                var obj = theModel.get(theGridView.currentIndex);
                if (obj!=null) {
                    waittingGroupEditResponse = true;
                    var groupInfo = rootItem.groupInfo;
                    groupInfo.managerUserId = obj.memberInfo.memberUserId;
                    theApp.editGroupInfo(groupInfo);
                }
            }
        }
        EbMenuItem {
            id: addAdminLevel
//            label: "加为管理员"
            onTriggered: {
                var obj = theModel.get(theGridView.currentIndex);
                if (obj!=null) {
                    theApp.addMemberAdminLevel(obj.memberInfo.groupId.toString(), obj.memberInfo.memberUserId.toString());
                }
            }
        }
        EbMenuItem {
            id: delAdminLevel
//            label: "取消管理员资格"
            onTriggered: {
                var obj = theModel.get(theGridView.currentIndex);
                if (obj!=null) {
                    theApp.deleteMemberAdminLevel(obj.memberInfo.groupId.toString(), obj.memberInfo.memberUserId.toString());
                }
            }
        }
//        EbMenuItem {
//            id: editMemberInfo
////            label: "修改成员名称"
//            onTriggered: {
//            }
//        }
        MenuSeparator {
            id: removeMemberSeparator
            padding: 0
            topPadding: 3
            bottomPadding: 3
            contentItem: Rectangle {
                implicitWidth: parent.implicitWidth
                implicitHeight: 1
                color: "#1E000000"
            }
        }
        EbMenuItem {
            id: removeMemberInfo
//            label: "移除成员"
            onTriggered: {
                var obj = theModel.get(theGridView.currentIndex);
                if (obj==null) {
                    return;
                }

                var title = theLocales.getLocalText("message-box.delete-member.title","移除成员");
                if (title.length==0) {
                    title = theApp.productName;
                }
                var text = theLocales.getLocalText("message-box.delete-member.text","Confirm Delete Member?");
                text = text.replace( "[USER_ACCOUNT]", obj.memberInfo.memberAccount);
                text = text.replace( "[USER_ID]", obj.memberInfo.memberUserId);
                theMessageBoxLoadItem.messageBoxType = 1;
                theMessageBoxLoadItem.visible = true;
                theMessageBoxLoader.setSource("EbMessageDialog.qml", {"title": title, "text": text});
                theMessageBoxLoader.item.open();
            }
        }
    }

    EbTitleBar {
        id: titleBar;
        width: parent.width
//        title: "详细资料"
        rightButton0Label: "\uf021"
        rightButton0Visible: true;
        onReturnBack: {
            if (requestReturnBack()) {
//                titleBar.title = rootItem.groupInfo.groupName;
                rootItem.returnBack();
            }
        }
        onPressedRightButton0: {
            theModel.clear();
            setGroupInfo(rootItem.groupInfo);
        }
    }

    function memberCountChanged() {
        if (!rootItem.inited) {
            return;
        }
        var count = theGridView.count;
        if (count>0 && addMemberButtonViewed) {
            count = count-1;
        }
        titleBar.title = rootItem.groupInfo.groupName+"-"+rootItem.groupTypeInfo.groupTypeMember+"("+count+")";
    }

    function deleteMemberInfo(groupInfo, memberInfo) {
        if (memberInfo.groupId!=rootItem.groupInfo.groupId) {
            return;
        }
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.memberInfo==null) {
                break;
            }
            if (obj.memberInfo.memberId==memberInfo.memberId) {
                theModel.remove(i);
                memberCountChanged();
                break;
            }
        }
    }
    function setGridViewHeight() {
        var lineCount = (theGridView.count+4)/5;
        if (lineCount>5) {
            lineCount = 5;
        }
        theGridView.height = lineCount*theGridView.cellHeight+6;
    }

    function setMemberInfo(memberInfo) {
        var memberId = memberInfo.memberId;
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.memberInfo===null ||
                    obj.memberInfo===undefined) {
                break;
            }
            if (obj.memberInfo.memberId==memberId) {
                theModel.set(i, { addType: 0, memberInfo: memberInfo });
                return;
            }
        }
        if (addMemberButtonViewed && theModel.count>0)
            theModel.insert(theModel.count-1, { addType: 0, memberInfo: memberInfo } );
        else {
            theModel.append( { addType: 0, memberInfo: memberInfo } );
            setGridViewHeight();
        }
        memberCountChanged();
    }

    Connections {
        target: theApp
        onDeletedGroupId: {
            if (groupId==rootItem.groupInfo.groupId) {
                rootItem.returnBack();
            }
        }

        onMemberEditResponse: {
            if (memberInfo.groupId!=rootItem.groupInfo.groupId) {
                return;
            }
            if (stateCode!=0) {
                tipText.text = stateString;
                return;
            }
            rootItem.setMemberInfo(memberInfo);
//            for (var i=0; i<theModel.count; i++) {
//                var obj = theModel.get(i);
//                if (obj.memberInfo==null) {
//                    break;
//                }
//                if (obj.memberInfo.memberId==memberInfo.memberId) {
//                    theModel.set(i, { addType: 0, memberInfo: memberInfo });
//                    break;
//                }
//            }
        }
        onReceivedMemberInfo: {
            if (memberInfo.groupId!=rootItem.groupInfo.groupId) {
                return;
            }
            rootItem.setMemberInfo(memberInfo);
        }

        onMemberRemoveGroup: {
            deleteMemberInfo(groupInfo, memberInfo);
        }

        onMemberExitGroup: {
            deleteMemberInfo(groupInfo, memberInfo);
        }

        onLoadedGroupMemberInfo: {
            theModel.append( { addType: 0, memberInfo: memberInfo } );
            memberCountChanged();
        }
        onGroupEditResponse: {
            if (groupInfo.groupId!=rootItem.groupInfo.groupId) {
                return;
            }
            if (!waittingGroupEditResponse) {
                return;
            }
            waittingGroupEditResponse = false;
            if (stateCode==0) {
                if (theApp.isGroupAdminLevel(groupInfo.groupId.toString())) {
                    var isGroupForbidSpeech = theApp.isGroupForbidSpeech(groupInfo.groupId.toString());
                    addForbidSpeechGroupButton.visible = !isGroupForbidSpeech;
                    deleteForbidSpeechGroupButton.visible = isGroupForbidSpeech;
                }
                /// 重设为部门，重新加载一次成员
                rootItem.setGroupInfo(groupInfo);
            }
            else {
                tipText.text = stateString;
            }
        }
    }

    ListModel {
        id: theModel
//        ListElement {
//            image: "assets:/res/imgdefaultgroup.png"
//            name: "11111"
//        }
    }

    Component {
        id: theDelegate
        Rectangle {
            id: listItem
            width: theGridView.cellWidth;
            height: theGridView.cellHeight
            color: "transparent"
            Item {
                width: parent.width-20
                height: width
                x: (parent.width-width)/2
                y: (parent.height-height)/2
                Column {
                    anchors.fill: parent
                    EbImage {
                        id: image
                        width: parent.width*0.70
                        height: addType==1?1:width
                        source: addType==1?"":theApp.memberHeadImage(memberInfo);
                        fillMode: Image.PreserveAspectFit
                        anchors.horizontalCenter: parent.horizontalCenter
                        grayImage: (memberInfo==null || !memberInfo.isOnlineState)?true:false;
                    }
                    Text {
                        font.family: addType==1?"FontAwesome":""
                        text: addType==1?"\uf067":memberInfo.userName;
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: addType==1?"#666":(memberInfo.subType>0?"red":"#000")
                        height: addType==1?(parent.height-2):(parent.height-image.height)
                        verticalAlignment: Text.AlignVCenter
                        font.pointSize: addType==1?18:13
                    }
                }
            }

            EbTimerPress {
                id: delayPressTimer
                onPressed: {
                    if (addType==0) {
                        theViewLoader.source = "EbMemberInfoView.qml";
                        theViewLoader.item.setMemberInfo(rootItem.groupInfo, memberInfo);
                        theViewLoadItem.visible = true;
                    }
                    else {
                        /// 新建员工
                        theViewLoader.source = "EbMemberInfoView.qml";
                        theViewLoader.item.newMemberInfo(rootItem.groupInfo);
                        theViewLoadItem.visible = true;
                    }
                }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onPressed: {
                    parent.color = "#eee";
                    delayPressTimer.initStart();
                }
                onReleased: {
                    parent.color = "transparent";
                    delayPressTimer.pressedReleased = true;
                }
                onExited: {
                    parent.color = "transparent";
                }
                onPressAndHold: {
                    mouse.accepted = true;
                    var hasAnyMenuItemVisible = false;
                    theGridView.currentIndex = index;
                    if (memberInfo.memberUserId!=rootItem.groupInfo.managerUserId &&
                            theApp.canEditGroupInfo(memberInfo.groupId.toString())) {
                        setGroupManager.visible = true;
                    }
                    else {
                        setGroupManager.visible = false;
                    }
                    if (theApp.canEditMemberAdminLevel(memberInfo.groupId.toString(), memberInfo.memberUserId.toString())) {
                        var isGroupAdminLevel = theApp.isGroupAdminLevel(memberInfo.groupId.toString(), memberInfo.memberUserId.toString());
                        addAdminLevel.visible = !isGroupAdminLevel;
                        delAdminLevel.visible = isGroupAdminLevel;
                        hasAnyMenuItemVisible = true;
                    }
                    else {
                        addAdminLevel.visible = false;
                        delAdminLevel.visible = false;
                    }
                    var canEditMemberInfo = theApp.canEditMemberInfo(memberInfo.groupId.toString(),
                                                                     memberInfo.memberUserId.toString());
                    /// 该功能未实现
//                    editMemberInfo.visible = false;///canEditMemberInfo;
                    removeMemberSeparator.visible = canEditMemberInfo;
                    removeMemberInfo.visible = canEditMemberInfo;
                    if (canEditMemberInfo) {
                        hasAnyMenuItemVisible = true;
                    }
                    if (!hasAnyMenuItemVisible) {
                        /// 没有任何权限，不做下面处理
                        return;
                    }

                    /// 计算弹出菜单位置
                    var obj = mouseArea.mapToGlobal(mouse.x, mouse.y);
                    obj = rootItem.mapFromGlobal(obj.x, obj.y);
                    theMemberMenu.x = obj.x-theMemberMenu.width+5;
                    theMemberMenu.y = obj.y-30;
                    theMemberMenu.open();
                }
            }
        }
    }

    Column {
        y: titleBar.height
        width: parent.width
        height: parent.height-y

        GridView {
            id: theGridView
            width: parent.width
            height: 80
//            height: parent.height-y
            //        anchors.fill: parent
            leftMargin: 10
            topMargin: 10
            rightMargin: 10
            bottomMargin: 10
            cellWidth: 70;
            cellHeight: 70
            model: theModel
            delegate: theDelegate
            //        highlight: Rectangle {
            //            color: "lightsteelblue";
            //            radius: 5
            //        }
            focus: true
        }
        Rectangle {
            width: parent.width
            height: 10
            color: "#eee"
        }

        Item {
            width: parent.width
            height: 10
        }
        EbButton {
            /// 群禁言
            id: addForbidSpeechGroupButton
            x: 20
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            textColor: "#fff"
            visible: false
            buttonColor: theApp.mainColor
            buttonHoverColor: theApp.mainHoverColor
            buttonPressedColor: theApp.mainPressedColor
            onClickedButton: {
                tipText.text = "";
                waittingGroupEditResponse = true;
                theApp.setGroupForbidSpeech(rootItem.groupInfo.groupId.toString(), true);
            }
        }
        EbButton {
            /// 解除禁言
            id: deleteForbidSpeechGroupButton
            x: 20
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            textColor: "#fff"
            visible: false
            buttonColor: theApp.mainColor
            buttonHoverColor: theApp.mainHoverColor
            buttonPressedColor: theApp.mainPressedColor
            onClickedButton: {
                tipText.text = "";
                waittingGroupEditResponse = true;
                theApp.setGroupForbidSpeech(rootItem.groupInfo.groupId.toString(), false);
            }
        }
        Item {
            width: parent.width
            height: 10
        }

        EbButton {
            /// 退出群组
            id: exitGroupButton
            x: 20
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            textColor: "#fff"
            visible: false
            buttonColor: "#ed1c24"  /// 237, 28, 36
            buttonHoverColor: "#f14e56" /// 241, 78, 86
            buttonPressedColor: "#c5101a"   /// 197, 16, 26
            onClickedButton: {
                var title = theLocales.getLocalText("message-box.exit-group.title", "Exit Group");
                if (title.length==0) {
                    title = theApp.productName;
                }
                else {
                    title = title.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
                }
                var text = theLocales.getLocalText("message-box.exit-group.text", "Confirm Exit Group?");
                text = text.replace( "[GROUP_NAME]", rootItem.groupInfo.groupName);
                text = text.replace( "[GROUP_ID]", rootItem.groupInfo.groupId);
                theMessageBoxLoadItem.messageBoxType = 2;
                theMessageBoxLoadItem.visible = true;
                theMessageBoxLoader.setSource("EbMessageDialog.qml", {"title": title, "text": text});
                theMessageBoxLoader.item.open();
//                exitGroupLoadItem.visible = true;
//                exitGroupLoader.setSource("EbMessageDialog.qml", {"title": title, "text": text});
//                exitGroupLoader.item.open();
            }
        }
        Item {
            id: exitGroupLine
            width: parent.width
            height: 10
        }

        EbButton {
            /// 解散群组
            id: deleteGroupButton
            x: 20
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            textColor: "#fff"
            visible: false
            buttonColor: "#ed1c24"  /// 237, 28, 36
            buttonHoverColor: "#f14e56" /// 241, 78, 86
            buttonPressedColor: "#c5101a"   /// 197, 16, 26
            onClickedButton: {
                var title = theLocales.getLocalText("message-box.delete-group.title", "Delete Group");
                if (title.length==0) {
                    title = theApp.productName;
                }
                else {
                    title = title.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
                }
                var text = theLocales.getLocalText("message-box.delete-group.text", "Confirm Delete Group?");
                text = text.replace( "[GROUP_NAME]", rootItem.groupInfo.groupName);
                text = text.replace( "[GROUP_ID]", rootItem.groupInfo.groupId);
                theMessageBoxLoadItem.messageBoxType = 3;
                theMessageBoxLoadItem.visible = true;
                theMessageBoxLoader.setSource("EbMessageDialog.qml", {"title": title, "text": text});
                theMessageBoxLoader.item.open();
//                deleteGroupLoadItem.visible = true;
//                deleteGroupLoader.setSource("EbMessageDialog.qml", {"title": title, "text": text});
//                deleteGroupLoader.item.open();
            }
        }
        Item {
            id: deleteGroupLine
            width: parent.width
            height: 10
        }
        Text {
            id: tipText
            x: deleteGroupButton.x
            width: deleteGroupButton.width
            height: 68
            wrapMode: Text.Wrap
            lineHeight: 1.2
            color: "red"
        }

    }

    function updateLocaleInfo() {
        addAdminLevel.label = theLocales.getLocalText("context-menu.member-add-admin.text","Add Admin");
        delAdminLevel.label = theLocales.getLocalText("context-menu.member-del-admin.text","Delete Admin");
//        editMemberInfo.label = theLocales.getLocalText("context-menu.edit-user-member.text","Edit User Member");
        removeMemberInfo.label = theLocales.getLocalText("context-menu.del-member.text","Delete Member");
        addForbidSpeechGroupButton.label = theLocales.getLocalText("context-menu.group-add-forbid-speech.text", "Group Add Forbid Speech");
        deleteForbidSpeechGroupButton.label = theLocales.getLocalText("context-menu.group-del-forbid-speech.text", "Group Delete Forbid Speech");
    }

    Component.onCompleted: {
        updateLocaleInfo();
    }

}

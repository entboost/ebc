import QtQuick 2.0
import Entboost.KeyFilter 1.0
import Entboost.GroupTypeName 1.0
import Entboost.MemberInfo 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent
    z: 100

    signal returnBack();
    signal sendMemberMessage(var memberInfo);
    signal sendGroupMessage(var groupInfo);

    /// 用于父窗口请求返回子窗口，
    /// 如果有子窗口显示，隐藏后返回 false ，父窗口会根据返回 true 不处理
    /// 如果没有返回  true，父窗口会隐藏当前窗口
    function requestReturnBack() {
        if (theViewLoadItem.visible) {
            if (!theViewLoader.item.requestReturnBack()) {
                return false;
            }
            theViewLoadItem.visible = false;
            theViewLoader.source = "";
            return false;
        }

//        if (groupMemberViewLoadItem.visible) {
//            if (!groupMemberViewLoader.item.requestReturnBack()) {
//                return false;
//            }
//            groupMemberViewLoadItem.visible = false;
//            groupMemberViewLoader.source = ""
//            return false;
//        }
//        if (parentGroupViewLoadItem.visible) {
//            if (!parentGroupViewLoader.item.requestReturnBack()) {
//                return false;
//            }
//            parentGroupViewLoadItem.visible = false;
//            parentGroupViewLoader.source = "";
//            return false;
//        }
//        if (subGroupViewLoadItem.visible) {
//            if (!subGroupViewLoader.item.requestReturnBack()) {
//                return false;
//            }
//            subGroupViewLoadItem.visible = false;
//            subGroupViewLoader.source = "";
//            return false;
//        }
//        if (editMode) {
//            editMode = false;
//            updateEditModeControl();
//            return false;
//        }
        return true;
    }

    property bool waittingGroupEditResponse: false
    property var groupInfo: null
    function setGroupInfo(groupInfo) {
        rootItem.groupInfo = groupInfo;
        theApp.requestLoadGroupMember(groupInfo.groupId);
        titleBar.title = groupInfo.groupName+"-"+theLocales.getLocalText("name-text.detail", "Detail");
        titleBar.rightButton0Visible = theApp.canEditGroupInfo(groupInfo.groupId.toString());
        groupImageNameInfo.source = groupInfo.groupImagePath;
        groupImageNameInfo.name = groupInfo.groupName;
        var groupTypeInfo = theLocales.getGroupTypeInfo(groupInfo.groupType);
        var label = theLocales.getLocalText("group-info-dialog.label-group-name.label", "Group Name");
        groupNameInput.label = label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        label = theLocales.getLocalText("group-info-dialog.edit-group-name.bg-text", "");
        groupNameInput.placeholderText = label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        if (groupInfo.groupType<=1) {
            /// 0=部门
            /// 1=项目
            /// 这2个才需要
            managerText.visible = true;
            managerLine.visible = true;
            managerText.label = groupTypeInfo.groupTypeManager;
            if (groupInfo.managerUserId>0) {
                managerText.text = theApp.userName(groupInfo.groupId.toString(),
                                                   groupInfo.managerUserId.toString());
                managerText.visibleRightIcon = true;
                managerText.pressEnable = true;
            }
            else {
                managerText.visibleRightIcon = false;
                managerText.pressEnable = false;
            }
        }
        else {
            managerText.visible = false;
            managerLine.visible = false;
        }

        memberText.label = groupTypeInfo.groupTypeMember;
        memberText.text = groupInfo.memberCount;
        groupImageNameInfo.info = "ID: " + groupInfo.groupId;
        telText.text = groupInfo.phone;
        faxText.text = groupInfo.fax;
        emailText.text = groupInfo.email;
        urlText.text = groupInfo.url;
        addressText.text = groupInfo.address;
        descriptionText.text = groupInfo.description;
        if (groupInfo.parentId>0) {
            parentGroupText.visible = true;
            parentGroupLine.visible = true;
            parentGroupText.text = theApp.groupName(groupInfo.parentId.toString());
        }
        else {
            parentGroupText.visible = false;
            parentGroupLine.visible = false;
        }
        var subGroupCount = theApp.subGroupCount(groupInfo.groupId.toString());
        if (subGroupCount>0) {
            subGroupText.visible = true;
            subGroupLine.visible = true;
            subGroupText.text = subGroupCount;
        }
        else {
            subGroupText.visible = false;
            subGroupLine.visible = false;
        }

        if (theApp.isEnterpriseGroupType(groupInfo.groupType)) {
            enterpriseText.visible = true;
            enterpriseLine.visible = true;
            enterpriseText.text = theApp.enterpriseName(groupInfo.enterpriseId.toString());
        }
        else {
            enterpriseText.visible = false;
            enterpriseLine.visible = false;
        }
        sendChatMessageButton.visible = groupInfo.myMemberId>0?true:false;
    }

    EbTitleBar {
        id: titleBar;
        width: parent.width
        rightButton0Label: "\uf044"
//        rightButton0Visible: true
//        title: "详细资料"
        onReturnBack: {
            while (!requestReturnBack()) {
            }
            rootItem.returnBack();
        }
        onPressedRightButton0: {
            editMode = !editMode;
            updateEditModeControl();
        }
    }

    Item {
        id: theViewLoadItem
        anchors.fill: parent
        visible: false
        z: 101
        Loader {
            id: theViewLoader
            anchors.fill: parent
        }
//        property int viewType: 0
        Connections {
            target: theViewLoader.item
            onReturnBack: {
                requestReturnBack();
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
//    Item {
//        /// 部门员工（成员）
//        id: groupMemberViewLoadItem
//        anchors.fill: parent
//        visible: false
//        z: 101
//        Loader {
//            id: groupMemberViewLoader
//            anchors.fill: parent
//        }
//        Connections {
//            target: groupMemberViewLoader.item
//            onReturnBack: {
//                requestReturnBack();
//            }
//        }
//    }
//    Item {
//        /// 上级部门
//        id: parentGroupViewLoadItem
//        anchors.fill: parent
//        visible: false
//        z: 101
//        Loader {
//            id: parentGroupViewLoader
//            anchors.fill: parent
//        }
//        Connections {
//            target: parentGroupViewLoader.item
//            onReturnBack: {
//                requestReturnBack();
//            }
//        }
//    }
//    Item {
//        /// 下级部门
//        id: subGroupViewLoadItem
//        anchors.fill: parent
//        visible: false
//        z: 101
//        Loader {
//            id: subGroupViewLoader
//            anchors.fill: parent
//        }
//        Connections {
//            target: subGroupViewLoader.item
//            onReturnBack: {
//                requestReturnBack();
//            }
//        }
//    }
    Connections {
        target: theApp
        onGroupEditResponse: {
            if (groupInfo.groupId!=rootItem.groupInfo.groupId) {
                return;
            }
            if (stateCode==0) {
                /// 重设为部门，重新加载一次成员
                rootItem.setGroupInfo(groupInfo);
            }
        }
        onReceivedMemberInfo: {
            if (memberInfo.memberUserId==rootItem.groupInfo.managerUserId) {
                managerText.text = memberInfo.userName;
            }
        }
    }

    property bool editMode: false
    function updateEditModeControl() {
        tipText.text = "";
        groupNameInput.visible = editMode
        groupImageNameInfo.visible = !editMode
        telInput.visible = editMode;
        telText.visible = !editMode;
//        if (editMode) {
//            telInput.focusInput = true;
//        }
        faxInput.visible = editMode;
        faxText.visible = !editMode;
        emailInput.visible = editMode;
        emailText.visible = !editMode;
        urlInput.visible = editMode;
        urlText.visible = !editMode;
        addressInput.visible = editMode;
        addressText.visible = !editMode;
        descriptionInput.visible = editMode;
        descriptionText.visible = !editMode;
        saveButton.visible = editMode;
        if (rootItem.groupInfo.myMemberId>0) {
            sendChatMessageButton.visible = !editMode;
        }
    }

    Column {
//        anchors.fill: parent
        y: titleBar.height
        width: parent.width
        height: parent.height-y

        EbInputLine {
            /// 电话
            id: groupNameInput
            colorLabel: "#222"
            visible: false
            text: groupImageNameInfo.name
        }
        EbImageNameInfo {
            id: groupImageNameInfo
//            source: "qrc:/res/entlogo.png"
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }

        EbTextLine {
            /// 部门成员
            id: memberText
            visibleRightIcon: true
            pressEnable: true
            onPressed: {
                theViewLoader.source = "EbGroupMemberView.qml";
                theViewLoader.item.setGroupInfo(groupInfo);
                theViewLoadItem.visible = true;
            }
        }
        Rectangle {
            id: managerLine
            width: parent.width
            height: 1
            visible: false
            color: "#eee"
        }
        EbTextLine {
            /// 部门经理
            id: managerText
//            visibleRightIcon: rootItem.groupInfo.managerUserId>0?true:false;
//            pressEnable: rootItem.groupInfo.managerUserId>0?true:false;
            visible: false
            onPressed: {
                var memberInfo = theApp.memberInfo(rootItem.groupInfo.groupId.toString(),
                                                   rootItem.groupInfo.managerUserId.toString());
                if (memberInfo!=null) {
                    theViewLoader.source = "EbMemberInfoView.qml";
                    theViewLoader.item.setMemberInfo(groupInfo, memberInfo);
                    theViewLoadItem.visible = true;
                }
            }
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }

        EbInputLine {
            /// 电话
            id: telInput
            colorLabel: "#222"
            visible: false
            text: telText.text
        }
        EbTextLine {
            /// 电话
            id: telText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            /// 传真
            id: faxInput
            colorLabel: "#222"
            visible: false
            text: faxText.text
        }
        EbTextLine {
            /// 传真
            id: faxText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            /// 邮箱
            id: emailInput
            colorLabel: "#222"
            visible: false
            text: emailText.text
        }
        EbTextLine {
            /// 邮箱
            id: emailText
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }
        EbInputLine {
            /// 网页
            id: urlInput
            colorLabel: "#222"
            visible: false
            text: urlText.text
        }
        EbTextLine {
            /// 网页
            id: urlText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            /// 地址
            id: addressInput
            colorLabel: "#222"
            visible: false
            text: addressText.text
        }
        EbTextLine {
            /// 地址
            id: addressText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            /// 备注
            id: descriptionInput
            colorLabel: "#222"
            visible: false
            text: descriptionText.text
        }
        EbTextLine {
            /// 备注
            id: descriptionText
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }
        EbTextLine {
            /// 上级
            id: parentGroupText
            visibleRightIcon: true
            pressEnable: true
            onPressed: {
                var parentGroupInfo = theApp.groupQuickInfo(groupInfo.parentId.toString(), true);
                if (parentGroupInfo==null || parentGroupInfo.groupId==0) {
                    parentGroupText.visible = false;
                    parentGroupLine.visible = false;
                }
                else {
                    theViewLoader.source = "EbGroupInfoView.qml";
                    theViewLoader.item.setGroupInfo(parentGroupInfo);
                    theViewLoadItem.visible = true;
                }
            }
        }
        Rectangle {
            id: parentGroupLine
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            /// 下级
            id: subGroupText
            visibleRightIcon: true
            pressEnable: true
            onPressed: {
                theViewLoader.source = "EbSubGroupView.qml";
                theViewLoader.item.setGroupInfo(rootItem.groupInfo);
                theViewLoadItem.visible = true;
            }
        }
        Rectangle {
            id: subGroupLine
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            /// 公司
            id: enterpriseText
        }
        Rectangle {
            id: enterpriseLine
            width: parent.width
            height: 8
            color: "#eee"
        }

        Item {
            width: parent.width
            height: 8
        }

        EbButton {
            id: saveButton
            x: 20
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            visible: false
            textColor: "#fff"
            buttonColor: theApp.mainColor
            buttonHoverColor: theApp.mainHoverColor
            buttonPressedColor: theApp.mainPressedColor
            onClickedButton: {
                waittingGroupEditResponse = true;
                if (groupNameInput.text.length==0) {
                    groupNameInput.focusInput = true;
                    return;
                }
                var newGroupInfo = rootItem.groupInfo;
                newGroupInfo.groupName = groupNameInput.text;
                newGroupInfo.phone = telInput.text;
                newGroupInfo.fax = faxInput.text;
                newGroupInfo.email = emailInput.text;
                newGroupInfo.url = urlInput.text;
                newGroupInfo.address = addressInput.text;
                newGroupInfo.description = descriptionInput.text;
                theApp.editGroupInfo(newGroupInfo);
            }
        }
        EbButton {
            id: sendChatMessageButton
            x: 20
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            textColor: "#fff"
            buttonColor: theApp.mainColor
            buttonHoverColor: theApp.mainHoverColor
            buttonPressedColor: theApp.mainPressedColor
            onClickedButton: {
                rootItem.sendGroupMessage(rootItem.groupInfo);
            }
        }
        Item {
            width: parent.width
            height: 8
        }
        Text {
            id: tipText
            x: sendChatMessageButton.x
            width: sendChatMessageButton.width
            height: 68
            wrapMode: Text.Wrap
            lineHeight: 1.2
            color: "red"
        }

    }

    Connections {
        target: theApp
//        onDeletedMyGroupInfo: {
//            if (groupId==rootItem.groupInfo.groupId) {
//                while (!requestReturnBack()) {
//                }
//                rootItem.returnBack();
//            }
//        }
        onDeletedGroupInfo: {
//            console.log("onDeletedGroupInfo..., groupid=" + groupInfo.groupId);
            if (groupInfo.groupId==rootItem.groupInfo.groupId) {
                while (!requestReturnBack()) {
                }
                rootItem.returnBack();
            }
        }

        onGroupEditResponse: {
            if (!waittingGroupEditResponse) {
                return;
            }
            waittingGroupEditResponse = false;
            if (groupInfo.groupId==rootItem.groupInfo.groupId) {
                tipText.text = stateString;
                if (stateCode==0) {
                    setGroupInfo(groupInfo);
                    if (editMode) {
                        editMode = false;
                        updateEditModeControl();
                    }
                }
            }
        }
    }

    function updateLocaleInfo() {
        telInput.label = theLocales.getLocalText("group-info-dialog.label-tel.label", "Tel");
        telText.label = theLocales.getLocalText("group-info-dialog.label-tel.label", "Tel");
        faxInput.label = theLocales.getLocalText("group-info-dialog.label-fax.label", "Fax");
        faxText.label = theLocales.getLocalText("group-info-dialog.label-fax.label", "Fax");
        emailInput.label = theLocales.getLocalText("group-info-dialog.label-email.label", "Email");
        emailText.label = theLocales.getLocalText("group-info-dialog.label-email.label", "Email");
        urlInput.label = theLocales.getLocalText("group-info-dialog.label-url.label", "Url");
        urlText.label = theLocales.getLocalText("group-info-dialog.label-url.label", "Url");
        addressInput.label = theLocales.getLocalText("group-info-dialog.label-address.label", "Address");
        addressText.label = theLocales.getLocalText("group-info-dialog.label-address.label", "Address");
        descriptionInput.label = theLocales.getLocalText("group-info-dialog.label-description.label", "Description");
        descriptionText.label = theLocales.getLocalText("group-info-dialog.label-description.label", "Description");
        parentGroupText.label = theLocales.getLocalText("group-info-dialog.label-parent-group.label", "Parent Group");
        subGroupText.label = theLocales.getLocalText("group-info-dialog.label-sub-group.label", "Sub Group");
        enterpriseText.label = theLocales.getLocalText("group-info-dialog.label-enterprise.label", "Enterprise");
        saveButton.label = theLocales.getLocalText("base-dialog.save-button.text", "Save");
        sendChatMessageButton.label = theLocales.getLocalText("base-dialog.send-msg-button.text", "Send Message");
    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            if (editMode) {
                editMode = false;
                updateEditModeControl();
                return;
            }
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
        updateLocaleInfo();
    }

}

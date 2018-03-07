import QtQuick 2.0
import QtQuick.Controls 1.4
import Entboost.KeyFilter 1.0
import Entboost.GroupTypeName 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent
    z: 100

    signal returnBack();
    signal sendMemberMessage(var memberInfo);
    signal newMemberResponse(var memberInfo, var stateCode);

    /// 用于父窗口请求返回子窗口，
    /// 如果有子窗口显示，隐藏后返回 false ，父窗口会根据返回 true 不处理
    /// 如果没有返回  true，父窗口会隐藏当前窗口
    function requestReturnBack() {
        if (theViewLoadItem.visible) {
            theViewLoader.source = "";
            theViewLoadItem.visible = false;
            return false;
        }
        return true;
    }

    property bool waittingGroupEditResponse: false
    property var groupInfo: null
    property var memberInfo: null
    function newMemberInfo(groupInfo) {
        rootItem.groupInfo = groupInfo;
        var newText = theLocales.getLocalText("base-dialog.new-button.text", "New");
        var groupTypeInfo = theLocales.getGroupTypeInfo(groupInfo.groupType);
        titleBar.title = newText+groupTypeInfo.groupTypeMember;
        titleBar.rightButton0Visible = false;
        var label = theLocales.getLocalText("member-info-dialog.label-group-name.label", "Group Name");
        groupText.label = label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        groupText.text = theApp.groupName(groupInfo.groupId.toString());
        if (groupInfo.enterpriseId>0 && theApp.isEnterpriseGroupType(groupInfo.groupType)) {
            enterpriseText.visible = true;
            enterpriseLine.visible = true;
            enterpriseText.text = theApp.enterpriseName(groupInfo.enterpriseId.toString());
        }
        else {
            enterpriseText.visible = false;
            enterpriseLine.visible = false;
        }
        newMode = true;
        editMode = true;
        updateEditModeControl();
        /// 新建员工，签名可以不要（必须放在最后面）
        descriptionInput.visible = false;
        descriptionText.visible = false;
        descriptionLine.visible = false;
    }

    function setMemberInfo(groupInfo, memberInfo) {
        rootItem.groupInfo = groupInfo;
        rootItem.memberInfo = memberInfo;
        titleBar.title = memberInfo.userName+"-"+theLocales.getLocalText("name-text.detail", "Detail");
        titleBar.rightButton0Visible = theApp.canEditMemberInfo(memberInfo.groupId.toString(),
                                                                memberInfo.memberUserId.toString());
        memberImageNameInfo.source = theApp.memberHeadImage(memberInfo);
        memberAccountInput.text = memberInfo.memberAccount
        memberUserNameInput.text = memberInfo.userName;
        memberImageNameInfo.name = memberInfo.userName;
        var groupTypeInfo = theLocales.getGroupTypeInfo(groupInfo.groupType);
        memberImageNameInfo.info = memberInfo.memberAccount + "(" + memberInfo.memberUserId + ")";
        genderInput.gender = memberInfo.gender;
        if (memberInfo.gender>0) {
            genderText.text = genderInput.genderText;
        }
        theViewLoadItem.setBirthday(memberInfo.birthday);
        if (memberInfo.birthday>0) {
            birthdayText.text = theViewLoadItem.dateString;
        }
        addressText.text = memberInfo.address;
        descriptionText.text = memberInfo.description;
        phoneText.text = memberInfo.cellPhone;
        telText.text = memberInfo.workPhone;
        faxText.text = memberInfo.fax;
        emailText.text = memberInfo.email;
        jobTitleText.text = memberInfo.jobTitle;
        jobPositionText.text = memberInfo.jobPosition;
        var label = theLocales.getLocalText("member-info-dialog.label-group-name.label", "Group Name");
        groupText.label = label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        groupText.text = theApp.groupName(groupInfo.groupId.toString());
        if (groupInfo.enterpriseId>0 && theApp.isEnterpriseGroupType(groupInfo.groupType)) {
            enterpriseText.visible = true;
            enterpriseLine.visible = true;
            enterpriseText.text = theApp.enterpriseName(groupInfo.enterpriseId.toString());
        }
        else {
            enterpriseText.visible = false;
            enterpriseLine.visible = false;
        }
        sendChatMessageButton.visible = memberInfo.memberUserId!=theApp.logonUserId?true:false;

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
        /// 生日
        property var birthday: 0
        property string dateFormat: "yyyy-MM-dd"
        property string dateString: ""
        function setBirthday(birthday) {
            theViewLoadItem.birthday = birthday;
            if (birthday>0) {
                /// format: yyyyMMdd
                var birthdayYear = (birthday/10000);
                var birthdayMonth = (birthday%10000)/100;
                var birthdayDay = birthday%100;
                var birthdayDate = new Date(birthdayYear, birthdayMonth-1);
                /// ** 必须使用 setDate ，避免日期显示错误问题；
                birthdayDate.setDate(birthdayDay);
                theViewLoadItem.dateString = Qt.formatDate(birthdayDate, theViewLoadItem.dateFormat);
            }
        }

        /// 1=地区
        /// 2=生日
        property int viewType: 0
        Connections {
            target: theViewLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onAccepted: {
                if (theViewLoadItem.viewType==2) {
                    theViewLoadItem.setBirthday(theViewLoader.item.birthday);
                    birthdayText.text = theViewLoadItem.dateString;
                }
                requestReturnBack();
            }
        }
    }

    EbTitleBar {
        id: titleBar;
        width: parent.width
        rightButton0Label: "\uf044"
//        title: "详细资料"
        onReturnBack: {
            if (requestReturnBack()) {
                rootItem.returnBack();
            }
        }
        onPressedRightButton0: {
            editMode = !editMode;
            updateEditModeControl();
        }
    }

    property bool newMode: false
    property bool editMode: false
    function updateEditModeControl() {
        tipText.text = "";
        memberUserNameInput.visible = editMode;
        memberAccountInput.visible = editMode;
        memberAccountUserNameLine.visible = editMode;
        memberImageNameInfo.visible = !editMode
        phoneInput.visible = editMode;
        phoneText.visible = !editMode;
        telInput.visible = editMode;
        telText.visible = !editMode;
//        if (editMode) {
//            telInput.focusInput = true;
//        }
        faxInput.visible = editMode;
        faxText.visible = !editMode;
        emailInput.visible = editMode;
        emailText.visible = !editMode;
        genderInput.visible = editMode;
        genderText.visible = !editMode;
        birthdayText.colorLabel = editMode?"#222":"#666"
        birthdayText.pressEnable = editMode;
        birthdayText.visibleRightIcon = editMode;
        addressInput.visible = editMode;
        addressText.visible = !editMode;
        descriptionInput.visible = editMode;
        descriptionText.visible = !editMode;
        jobPositionInput.visible = editMode;
        jobPositionText.visible = !editMode;
        jobTitleInput.visible = editMode;
        jobTitleText.visible = !editMode;
        saveButton.visible = editMode;
        sendChatMessageButton.visible = (!editMode && memberInfo.memberUserId!=theApp.logonUserId)?true:false;
    }

    Column {
//        anchors.fill: parent
        y: titleBar.height
        width: parent.width
        height: parent.height-y

//        EbButton {
//            id: sendChatMessageButton2
//            x: 20
//            width: parent.width-x*2
//            height: 36
//            radius: 3
//            border.width: 0
//            textColor: "#fff"
//            buttonColor: theApp.mainColor
//            buttonHoverColor: theApp.mainHoverColor
//            buttonPressedColor: theApp.mainPressedColor
//            onClickedButton: {
//                rootItem.sendMemberMessage(rootItem.memberInfo);
//            }
//        }
        EbInputLine {
            /// 用户帐号
            id: memberAccountInput
            colorLabel: "#222"
            visible: false
            enabled: newMode
        }
        Rectangle {
            id: memberAccountUserNameLine
            width: parent.width
            height: 1
            color: "#eee"
            visible: false
        }
        EbInputLine {
            /// 用户名称
            id: memberUserNameInput
            colorLabel: "#222"
            visible: false
        }

        EbImageNameInfo {
            id: memberImageNameInfo
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }

        // 姓名 (性别,用一个小图标代替)
        /// 签名（个性签名）,性别,生日,地址，
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
            id: descriptionLine
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbGenderSelect {
            /// 性别
            id: genderInput
            colorLabel: "#222"
            visible: false
        }
        EbTextLine {
            /// 性别
            id: genderText
            text: genderInput.genderText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
//        EbDateSelect {
//            /// 生日
//            id: birthdayInput
//            colorLabel: "#222"
//            visible: false
//        }
        EbTextLine {
            /// 生日
            id: birthdayText
//            text: birthdayInput.dateString
            onPressed: {
                theViewLoadItem.viewType = 2;
                var birthday = rootItem.memberInfo==null?0:rootItem.memberInfo.birthday;
                theViewLoader.setSource("EbDateSelectView.qml",
                                        { "birthday": birthday });
                theViewLoadItem.visible = true;
            }
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
            height: 8
            color: "#eee"
        }
        /// 手机,电话,传真,邮箱,
        EbInputLine {
            /// 手机
            id: phoneInput
            colorLabel: "#222"
            visible: false
            text: phoneText.text
        }
        EbTextLine {
            /// 手机
            id: phoneText
        }
        Rectangle {
            width: parent.width
            height: 1
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
        /// 职位,岗位级别,部门,公司
        EbInputLine {
            /// 职位
            id: jobTitleInput
            colorLabel: "#222"
            visible: false
            text: jobTitleText.text
        }
        EbTextLine {
            /// 职位
            id: jobTitleText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            /// 岗位级别
            id: jobPositionInput
            colorLabel: "#222"
            visible: false
            text: jobPositionText.text
            validator: IntValidator {bottom: 0; top: 65535;}
        }
        EbTextLine {
            /// 岗位级别
            id: jobPositionText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            /// 部门
            id: groupText
        }
        Rectangle {
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
        /// ~显示排序
        /// 是否默认名片（设为默认名片）
        /// ???

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
                if (newMode && memberAccountInput.text.length===0) {
                    memberAccountInput.focusInput = true;
                    return;
                }
                var userName = memberUserNameInput.text;
                if (userName.length===0) {
                    memberUserNameInput.focusInput = true;
                    return;
                }
                waittingGroupEditResponse = true;
                var newMemberInfo = newMode?theApp.buildNewMemberInfo(
                                                 rootItem.groupInfo.groupId.toString(),
                                                 memberAccountInput.text, userName)
                                           : rootItem.memberInfo;
                if (newMode) {
                    newMemberInfo.memberAccount = memberAccountInput.text;
                }
                newMemberInfo.userName = userName;
                newMemberInfo.cellPhone = phoneInput.text;
                newMemberInfo.workPhone = telInput.text;
                newMemberInfo.fax = faxInput.text;
                newMemberInfo.email = emailInput.text;
                newMemberInfo.gender = genderInput.gender;
                newMemberInfo.birthday = theViewLoadItem.birthday;
                newMemberInfo.address = addressInput.text;
                newMemberInfo.description = descriptionInput.text;
                newMemberInfo.jobPosition = parseInt(jobPositionInput.text);
                newMemberInfo.jobTitle = jobTitleInput.text;
                theApp.editMemberInfo(newMemberInfo);
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
                rootItem.sendMemberMessage(rootItem.memberInfo);
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
        onMemberEditResponse: {
            if (!newMode && memberInfo.memberId!=rootItem.memberInfo.memberId) {
                return;
            }
            if (stateCode!=0) {
                if (waittingGroupEditResponse) {
                    tipText.text = stateString;
                }
            }
            else {
                setMemberInfo(rootItem.groupInfo, memberInfo);
            }
            waittingGroupEditResponse = false;
            if (editMode) {
                editMode = false;
                updateEditModeControl();
            }
            if (newMode) {
                newMode = false;
                rootItem.newMemberResponse(memberInfo, stateCode);
            }
        }

//        onDeletedGroupInfo: {
////            console.log("onDeletedGroupInfo..., groupid=" + groupInfo.groupId);
//            if (groupInfo.groupId==rootItem.groupInfo.groupId) {
//                while (!requestReturnBack()) {
//                }
//                rootItem.returnBack();
//            }
//        }

//        onGroupEditResponse: {
//            if (!waittingGroupEditResponse) {
//                return;
//            }
//            waittingGroupEditResponse = false;
//            if (groupInfo.groupId==rootItem.groupInfo.groupId) {
//                tipText.text = stateString;
//                if (stateCode==0) {
//                    setGroupInfo(groupInfo);
//                    if (editMode) {
//                        editMode = false;
//                        updateEditModeControl();
//                    }
//                }
//            }
//        }
    }

    function updateLocaleInfo() {
        memberAccountInput.label = theLocales.getLocalText("member-info-dialog.label-account.label", "Account");
        memberAccountInput.placeholderText = theLocales.getLocalText("member-info-dialog.edit-account.bg-text", "");
        memberUserNameInput.label = theLocales.getLocalText("member-info-dialog.label-user-name.label", "User Name");
        memberUserNameInput.placeholderText = theLocales.getLocalText("member-info-dialog.edit-user-name.bg-text", "");
        genderInput.label = theLocales.getLocalText("member-info-dialog.label-gender.label", "Gender");
        genderText.label = theLocales.getLocalText("member-info-dialog.label-gender.label", "Gender");
        theViewLoadItem.dateFormat = theLocales.getLocalText("member-info-dialog.date-birthday.format", "yyyy-MM-dd");
        birthdayText.label = theLocales.getLocalText("member-info-dialog.label-birthday.label", "Birthday");
        addressInput.label = theLocales.getLocalText("member-info-dialog.label-address.label", "Address");
        addressText.label = theLocales.getLocalText("member-info-dialog.label-address.label", "Address");
        descriptionInput.label = theLocales.getLocalText("member-info-dialog.label-description.label", "Description");
        descriptionText.label = theLocales.getLocalText("member-info-dialog.label-description.label", "Description");
        phoneInput.label = theLocales.getLocalText("member-info-dialog.label-phone.label", "Phone");
        phoneText.label = theLocales.getLocalText("member-info-dialog.label-phone.label", "Phone");
        telInput.label = theLocales.getLocalText("member-info-dialog.label-tel.label", "Tel");
        telText.label = theLocales.getLocalText("member-info-dialog.label-tel.label", "Tel");
        faxInput.label = theLocales.getLocalText("member-info-dialog.label-fax.label", "Fax");
        faxText.label = theLocales.getLocalText("member-info-dialog.label-fax.label", "Fax");
        emailInput.label = theLocales.getLocalText("member-info-dialog.label-email.label", "Email");
        emailText.label = theLocales.getLocalText("member-info-dialog.label-email.label", "Email");
        jobPositionInput.label = theLocales.getLocalText("member-info-dialog.label-job-position.label", "Job Position");
        jobPositionText.label = theLocales.getLocalText("member-info-dialog.label-job-position.label", "Job Position");
        jobTitleInput.label = theLocales.getLocalText("member-info-dialog.label-job-title.label", "Job Title");
        jobTitleText.label = theLocales.getLocalText("member-info-dialog.label-job-title.label", "Job Title");
//        groupText.label = theLocales.getLocalText("member-info-dialog.label-parent-group.label", "Parent Group");
        enterpriseText.label = theLocales.getLocalText("member-info-dialog.label-enterprise.label", "Enterprise");
        saveButton.label = theLocales.getLocalText("base-dialog.save-button.text", "Save");
        sendChatMessageButton.label = theLocales.getLocalText("base-dialog.send-msg-button.text", "Send Message");
    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            if (requestReturnBack()) {
                if (!newMode && editMode) {
                    editMode = false;
                    updateEditModeControl();
                    return;
                }
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

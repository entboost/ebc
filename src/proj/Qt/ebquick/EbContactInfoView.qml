import QtQuick 2.0
import QtQuick.Controls 1.4
import Entboost.KeyFilter 1.0
import Entboost.GroupTypeName 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent
    z: 100

    signal returnBack();
    signal sendContactMessage(var contactInfo);
    signal newContactResponse(var contactInfo);

    /// 用于父窗口请求返回子窗口，
    /// 如果有子窗口显示，隐藏后返回 false ，父窗口会根据返回 true 不处理
    /// 如果没有返回  true，父窗口会隐藏当前窗口
    function requestReturnBack() {
        if (theViewLoadItem.visible) {
//            if (!theViewLoader.item.requestReturnBack()) {
//                return false;
//            }
            theViewLoadItem.visible = false;
            theViewLoader.source = "";
            return false;
        }
        return true;
    }

    property bool waittingGroupEditResponse: false
    property var contactInfo: null
//    function newMemberInfo(groupInfo) {
//        rootItem.groupInfo = groupInfo;
//        var newText = theLocales.getLocalText("base-dialog.new-button.text", "New");
//        var groupTypeInfo = theLocales.getGroupTypeInfo(groupInfo.groupType);
//        titleBar.title = newText+groupTypeInfo.groupTypeMember;
//        titleBar.rightButton0Visible = false;
//        var label = theLocales.getLocalText("contact-info-dialog.label-group-name.label", "Group Name");
//        groupText.label = label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
//        groupText.text = theApp.groupName(groupInfo.groupId.toString());
//        if (groupInfo.enterpriseId>0 && theApp.isEnterpriseGroupType(groupInfo.groupType)) {
//            enterpriseText.visible = true;
//            enterpriseLine.visible = true;
//            enterpriseText.text = theApp.enterpriseName(groupInfo.enterpriseId.toString());
//        }
//        else {
//            enterpriseText.visible = false;
//            enterpriseLine.visible = false;
//        }
//        newMode = true;
//        editMode = true;
//        updateEditModeControl();
//        /// 新建员工，签名可以不要（必须放在最后面）
//        descriptionInput.visible = false;
//        descriptionText.visible = false;
//        descriptionLine.visible = false;
//    }

    function setContactInfo(contactInfo) {
        rootItem.contactInfo = contactInfo;
        titleBar.title = contactInfo.name+"-"+theLocales.getLocalText("name-text.detail", "Detail");
        titleBar.rightButton0Visible = true;
        contactImageNameInfo.source = theApp.contactHeadImage(contactInfo);
        contactImageNameInfo.name = contactInfo.name;
        if (contactInfo.contactUserId>0) {
            contactImageNameInfo.info = contactInfo.contact + "(" + contactInfo.contactUserId + ")";
        }
        else {
            contactImageNameInfo.info = contactInfo.contact;
        }
        contactAccountInput.text = contactInfo.contact;
        groupNameText.text = contactInfo.groupName;
        if (contactInfo.ugId==0) {
            groupNameText.text = theLocales.getLocalText("name-text.default-group-name", "Default group name");
        }
        descriptionText.text = contactInfo.description;
        phoneText.text = contactInfo.phone;
        telText.text = contactInfo.tel;
        faxText.text = contactInfo.fax;
        emailText.text = contactInfo.email;
        jobTitleText.text = contactInfo.jobTitle;
        enterpriseText.text = contactInfo.company
        urlText.text = contactInfo.url;
        addressText.text = contactInfo.address;
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
        Connections {
            target: theViewLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onAccepted: {
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
        contactAccountInput.visible = editMode;
        contactAccountNameLine.visible = editMode;
        contactNameInput.visible = editMode;
        contactImageNameInfo.visible = !editMode;
        descriptionInput.visible = editMode;
        descriptionText.visible = !editMode;
        phoneInput.visible = editMode;
        phoneText.visible = !editMode;
        telInput.visible = editMode;
        telText.visible = !editMode;
        faxInput.visible = editMode;
        faxText.visible = !editMode;
        emailInput.visible = editMode;
        emailText.visible = !editMode;
        jobTitleInput.visible = editMode;
        jobTitleText.visible = !editMode;
        enterpriseInput.visible = editMode;
        enterpriseText.visible = !editMode;
        urlInput.visible = editMode;
        urlText.visible = !editMode;
        addressInput.visible = editMode;
        addressText.visible = !editMode;
        saveButton.visible = editMode;
        sendChatMessageButton.visible = !editMode;
    }

    Column {
//        anchors.fill: parent
        y: titleBar.height
        width: parent.width
        height: parent.height-y

        /// 联系人(帐号，ID）
        EbInputLine {
            /// 帐号
            id: contactAccountInput
            colorLabel: "#222"
            enabled: false
            visible: false
        }
        Rectangle {
            id: contactAccountNameLine
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            /// 姓名
            id: contactNameInput
            colorLabel: "#222"
            visible: false
            text: contactImageNameInfo.name
        }
        EbImageNameInfo {
            id: contactImageNameInfo
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }
        /// 分组 >，备注
        EbTextLine {
            /// 分组
            id: groupNameText
            pressEnable: true
            visibleRightIcon: true
            onPressed: {
                theViewLoader.source = "EbContactGroupSelect.qml";
                theViewLoader.item.init(rootItem.contactInfo);
                theViewLoadItem.visible = true;
            }
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
            label: descriptionText.label
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

        /// 手机，电话，传真，邮箱
        EbInputLine {
            /// 手机
            id: phoneInput
            colorLabel: "#222"
            visible: false
            text: phoneText.text
            label: phoneText.label
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
            label: telText.label
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
            label: faxText.label
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
            label: emailText.label
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
        /// 职位，公司，网址，地址
        EbInputLine {
            /// 职位
            id: jobTitleInput
            colorLabel: "#222"
            visible: false
            text: jobTitleText.text
            label: jobTitleText.label
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
            /// 公司
            id: enterpriseInput
            colorLabel: "#222"
            visible: false
            text: enterpriseText.text
            label: enterpriseText.label
        }
        EbTextLine {
            /// 公司
            id: enterpriseText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            /// 网址
            id: urlInput
            colorLabel: "#222"
            visible: false
            text: urlText.text
            label: urlText.label
        }
        EbTextLine {
            /// 网址
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
            label: addressText.label
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
//                if (newMode && memberAccountInput.text.length==0) {
//                    memberAccountInput.focusInput = true;
//                    return;
//                }
                var userName = contactNameInput.text;
                if (userName.length==0) {
                    contactNameInput.focusInput = true;
                    return;
                }
                waittingGroupEditResponse = true;
                var newContactInfo = rootItem.contactInfo;
//                var newMemberInfo = newMode?theApp.buildNewMemberInfo(
//                                                 rootItem.groupInfo.groupId.toString(),
//                                                 memberAccountInput.text, userName)
//                                           : rootItem.memberInfo;
//                if (newMode) {
//                    newMemberInfo.memberAccount = memberAccountInput.text;
//                }
                newContactInfo.name = userName;
                newContactInfo.description = descriptionInput.text;
                newContactInfo.phone = phoneInput.text;
                newContactInfo.tel = telInput.text;
                newContactInfo.fax = faxInput.text;
                newContactInfo.email = emailInput.text;
                newContactInfo.jobTitle = jobTitleInput.text;
                newContactInfo.company = enterpriseInput.text;
                newContactInfo.address = addressInput.text;
                newContactInfo.url = urlInput.text;
                theApp.editContactInfo(newContactInfo);
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
                rootItem.sendContactMessage(rootItem.contactInfo);
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
        onReceivedContactInfo: {
            if (!newMode && contactInfo.contactId!=rootItem.contactInfo.contactId) {
                return;
            }
            setContactInfo(contactInfo);
            waittingGroupEditResponse = false;
            if (editMode) {
                editMode = false;
                updateEditModeControl();
            }
            if (newMode) {
                newMode = false;
                rootItem.newContactResponse(contactInfo);
            }
        }

    }

    function updateLocaleInfo() {
        contactAccountInput.label = theLocales.getLocalText("contact-info-dialog.label-contact.label", "Contact");
        contactNameInput.label = theLocales.getLocalText("contact-info-dialog.label-user-name.label", "User Name");
        groupNameText.label = theLocales.getLocalText("contact-info-dialog.label-group-name.label", "Group Name");
        descriptionText.label = theLocales.getLocalText("contact-info-dialog.label-description.label", "Description");
        phoneText.label = theLocales.getLocalText("contact-info-dialog.label-phone.label", "Phone");
        telText.label = theLocales.getLocalText("contact-info-dialog.label-tel.label", "Tel");
        faxText.label = theLocales.getLocalText("contact-info-dialog.label-fax.label", "Fax");
        emailText.label = theLocales.getLocalText("contact-info-dialog.label-email.label", "Email");
        jobTitleText.label = theLocales.getLocalText("contact-info-dialog.label-job-title.label", "Job Title");
        enterpriseText.label = theLocales.getLocalText("contact-info-dialog.label-enterprise.label", "Enterprise");
        urlText.label = theLocales.getLocalText("contact-info-dialog.label-url.label", "Homepage");
        addressText.label = theLocales.getLocalText("contact-info-dialog.label-address.label", "Address");
        saveButton.label = theLocales.getLocalText("base-dialog.save-button.text", "Save");
        sendChatMessageButton.label = theLocales.getLocalText("base-dialog.send-msg-button.text", "Send Message");
    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            if (!newMode && editMode) {
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

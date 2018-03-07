import QtQuick 2.0
import QtQuick.Controls 1.4
import Entboost.KeyFilter 1.0
import Entboost.GroupTypeName 1.0
//import Entboost.AccountInfo 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent
    z: 100

    signal returnBack();
//    signal sendMessage(var groupInfo);
//    signal newMemberResponse(var memberInfo, var stateCode);

    /// 用于父窗口请求返回子窗口，
    /// 如果有子窗口显示，隐藏后返回 false ，父窗口会根据返回 true 不处理
    /// 如果没有返回  true，父窗口会隐藏当前窗口
    function requestReturnBack() {
        if (theViewLoadItem.visible) {
            if (!theViewLoader.item.requestReturnBack()) {
                return false;
            }
            theViewLoader.source = "";
            theViewLoadItem.visible = false;
            return false;
        }

        return true;
    }

    property bool waittingGroupEditResponse: false
    property var accountInfo: null
    function setAccountInfo(accountInfo) {
        rootItem.accountInfo = accountInfo;
        titleBar.title = accountInfo.userName+"-"+theLocales.getLocalText("name-text.detail", "Detail");
        titleBar.rightButton0Visible = theApp.logonUserId==accountInfo.userId?true:false;
        if (accountInfo.userId==theApp.logonUserId)
            accountImageNameInfo.source = theApp.myHeadImage;
        else
            accountImageNameInfo.source = theApp.accountHeadImage(accountInfo);
        memberAccountInput.text = accountInfo.account
        accountUserNameInput.text = accountInfo.userName;
        accountImageNameInfo.name = accountInfo.userName;
        accountImageNameInfo.info = accountInfo.account + "(" + accountInfo.userId + ")";
        accountImageNameInfo.pressEnable = theApp.logonUserId==accountInfo.userId?true:false;
        phoneText.text = accountInfo.mobile;
        telText.text = accountInfo.tel;
        emailText.text = accountInfo.email;
        urlText.text = accountInfo.url;
        genderInput.gender = accountInfo.gender;
        if (accountInfo.gender>0) {
            genderText.text = genderInput.genderText;
        }
        theViewLoadItem.setBirthday(accountInfo.birthday);
        if (accountInfo.birthday>0) {
            birthdayText.text = theViewLoadItem.dateString;
        }
//        birthdayInput.setBirthday(accountInfo.birthday);
//        if (accountInfo.birthday>0) {
//            birthdayText.text = birthdayInput.dateString;
//        }
        theViewLoadItem.area1Code = accountInfo.area1;
        theViewLoadItem.area2Code = accountInfo.area2;
        theViewLoadItem.area3Code = accountInfo.area3;
        theViewLoadItem.area4Code = accountInfo.area4;
        areaText.text = accountInfo.area1String + " " +
                accountInfo.area2String + " " +
                accountInfo.area3String + " " +
                accountInfo.area4String;
        addressText.text = accountInfo.address;
        zipcodeText.text = accountInfo.zipCode;
        descriptionText.text = accountInfo.description;
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
        /// 地区
        property var area1Code: 0
        property var area2Code: 0
        property var area3Code: 0
        property var area4Code: 0
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
                if (theViewLoadItem.viewType==1) {
                    theViewLoadItem.area1Code = theViewLoader.item.area1Code;
                    theViewLoadItem.area2Code = theViewLoader.item.area2Code;
                    theViewLoadItem.area3Code = theViewLoader.item.area3Code;
                    theViewLoadItem.area4Code = theViewLoader.item.area4Code;
                    areaText.text = theViewLoader.item.area1String + " " +
                            theViewLoader.item.area2String + " " +
                            theViewLoader.item.area3String + " " +
                            theViewLoader.item.area4String;
                }
                else if (theViewLoadItem.viewType==2) {
                    theViewLoadItem.setBirthday(theViewLoader.item.birthday);
                    birthdayText.text = theViewLoadItem.dateString;
                }
                theViewLoadItem.viewType = 0;
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
        accountUserNameInput.visible = editMode;
        memberAccountInput.visible = editMode;
        memberAccountUserNameLine.visible = editMode;
        accountImageNameInfo.visible = !editMode
        phoneInput.visible = editMode;
        phoneText.visible = !editMode;
        telInput.visible = editMode;
        telText.visible = !editMode;
        emailInput.visible = editMode;
        emailText.visible = !editMode;
        urlInput.visible = editMode;
        urlText.visible = !editMode;
        genderInput.visible = editMode;
        genderText.visible = !editMode;
        birthdayText.colorLabel = editMode?"#222":"#666"
        birthdayText.pressEnable = editMode;
        birthdayText.visibleRightIcon = editMode;
        areaText.colorLabel = editMode?"#222":"#666"
        areaText.pressEnable = editMode;
        areaText.visibleRightIcon = editMode;
        addressInput.visible = editMode;
        addressText.visible = !editMode;
        zipcodeInput.visible = editMode;
        zipcodeText.visible = !editMode;
        descriptionInput.visible = editMode;
        descriptionText.visible = !editMode;
        saveButton.visible = editMode;
//        sendChatMessageButton.visible = !editMode;
    }

    Column {
//        anchors.fill: parent
        y: titleBar.height
        width: parent.width
        height: parent.height-y

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
            id: accountUserNameInput
            colorLabel: "#222"
            visible: false
        }
        EbImageNameInfo {
            id: accountImageNameInfo
            onPressed: {
                theViewLoader.source = "EbChangeHeadImageView.qml";
                theViewLoader.item.setAccountInfo(rootItem.accountInfo);
                theViewLoadItem.visible = true;
            }

        }
        Rectangle {
            width: parent.width
            height: 10
            color: "#eee"
        }
        /// 手机,电话,邮箱,主页
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
            height: 1
            color: "#eee"
        }
        EbInputLine {
            /// 主页
            id: urlInput
            colorLabel: "#222"
            visible: false
            text: urlText.text
            label: urlText.label
        }
        EbTextLine {
            /// 主页
            id: urlText
        }
        Rectangle {
            width: parent.width
            height: 10
            color: "#eee"
        }
        /// 性别,生日
        EbGenderSelect {
            /// 性别
            id: genderInput
            colorLabel: "#222"
            visible: false
            label: genderText.label
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
//            label: birthdayText.label
//        }
        EbTextLine {
            /// 生日
            id: birthdayText
//            text: birthdayInput.dateString
            onPressed: {
                theViewLoadItem.viewType = 2;
                theViewLoader.setSource("EbDateSelectView.qml",
                                        {"birthday": rootItem.accountInfo.birthday });
                theViewLoadItem.visible = true;
            }
        }
        Rectangle {
            width: parent.width
            height: 10
            color: "#eee"
        }

        /// 地区,地址,邮编,备注
        EbTextLine {
            /// 地区
            id: areaText
            onPressed: {
                theViewLoadItem.viewType = 1;
                theViewLoader.setSource("EbAreaSelectView.qml",
                                        {"area1Code": rootItem.accountInfo.area1,
                                            "area2Code": rootItem.accountInfo.area2,
                                            "area3Code": rootItem.accountInfo.area3,
                                            "area4Code": rootItem.accountInfo.area4
                                        } );
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
            label: addressText.label
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
            /// 邮编
            id: zipcodeInput
            colorLabel: "#222"
            visible: false
            text: zipcodeText.text
            label: zipcodeText.label
        }
        EbTextLine {
            /// 邮编
            id: zipcodeText
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
            id: descriptionLine
            width: parent.width
            height: 10
            color: "#eee"
        }

        Item {
            width: parent.width
            height: 10
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
                var userName = accountUserNameInput.text;
                if (userName.length==0) {
                    accountUserNameInput.focusInput = true;
                    return;
                }
                waittingGroupEditResponse = true;
                var newAccountInfo = rootItem.accountInfo;
                newAccountInfo.userName = userName;
                newAccountInfo.mobile = phoneInput.text;
                newAccountInfo.tel = telInput.text;
                newAccountInfo.email = emailInput.text;
                newAccountInfo.url = urlInput.text;
                newAccountInfo.gender = genderInput.gender;
                newAccountInfo.birthday = theViewLoadItem.birthday;
                newAccountInfo.area1 = theViewLoadItem.area1Code;
                newAccountInfo.area2 = theViewLoadItem.area2Code;
                newAccountInfo.area3 = theViewLoadItem.area3Code;
                newAccountInfo.area4 = theViewLoadItem.area4Code;
                newAccountInfo.address = addressInput.text;
                newAccountInfo.zipCode = zipcodeInput.text;
                newAccountInfo.description = descriptionInput.text;
                if (newAccountInfo.userId==theApp.logonUserId) {
                    theApp.setMyAccountInfo(newAccountInfo);
                }
                else {
                    /// ???
                }
            }
        }
//        EbButton {
//            id: sendChatMessageButton
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
//                rootItem.sendMessage(rootItem.groupInfo);
//            }
//        }
//        Item {
//            width: parent.width
//            height: 10
//        }
        Text {
            id: tipText
            x: saveButton.x
            width: saveButton.width
            height: 68
            wrapMode: Text.Wrap
            lineHeight: 1.2
            color: "red"
        }

    }

    Connections {
        target: theApp
        onMyHeadImageChanged: {
            if (rootItem.accountInfo.userId==theApp.logonUserId) {
                accountImageNameInfo.source = "";
                accountImageNameInfo.source = imageUrl;
            }
        }
        onSetAccountInfoResponse: {
            if (stateCode!=0) {
                if (waittingGroupEditResponse) {
                    tipText.text = stateString;
                }
            }
            else {
                setAccountInfo(theApp.myAccountInfo());
            }
            waittingGroupEditResponse = false;
            if (editMode) {
                editMode = false;
                updateEditModeControl();
            }
        }
    }

    function updateLocaleInfo() {
        memberAccountInput.label = theLocales.getLocalText("account-info-widget.label-account.label", "Account");
        memberAccountInput.placeholderText = theLocales.getLocalText("account-info-widget.edit-account.bg-text", "");
        accountUserNameInput.label = theLocales.getLocalText("account-info-widget.label-name.label", "Name");
//        accountUserNameInput.placeholderText = theLocales.getLocalText("account-info-widget.edit-name.bg-text", "");
        phoneText.label = theLocales.getLocalText("account-info-widget.label-phone.label", "Phone");
        telText.label = theLocales.getLocalText("account-info-widget.label-tel.label", "Tel");
        emailText.label = theLocales.getLocalText("account-info-widget.label-email.label", "Email");
        urlText.label = theLocales.getLocalText("account-info-widget.label-url.label", "Homepage");
        genderText.label = theLocales.getLocalText("account-info-widget.label-gender.label", "Gender");
        theViewLoadItem.dateFormat = theLocales.getLocalText("account-info-widget.date-birthday.format", "yyyy-MM-dd");
        birthdayText.label = theLocales.getLocalText("account-info-widget.label-birthday.label", "Birthday");
        areaText.label = theLocales.getLocalText("account-info-widget.label-area.label", "Area");
        addressText.label = theLocales.getLocalText("account-info-widget.label-address.label", "Address");
        zipcodeText.label = theLocales.getLocalText("account-info-widget.label-zipcode.label", "Zipcode");
        descriptionText.label = theLocales.getLocalText("account-info-widget.label-description.label", "Description");
        saveButton.label = theLocales.getLocalText("base-dialog.save-button.text", "Save");
//        sendChatMessageButton.label = theLocales.getLocalText("base-dialog.send-msg-button.text", "Send Message");
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

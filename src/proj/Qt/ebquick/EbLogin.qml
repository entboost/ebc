import QtQuick 2.0
import QtQuick.Controls 2.0
//import QtQuick.Controls.Styles 1.4

//import "storage.js" as Storag

Item {
    id: rootItem

    property int titleHeight: 191
    property color mainColor: "#00a2e8"
    property string savePassword: ""
    property bool logoutMode: false
    property alias tip: tipText.text

    signal returnBack()
    signal loginSuccess()

    function requestReturnBack() {
        if (registerLoadItem.visible) {
            registerLoadItem.visible = false;
            registerLoader.source = "";
            if (!accountTextField.focus && !passwordTextField.focus) {
                accountTextField.focus = true;
            }
            return false;
        }
        if (connectSettingLoadItem.visible) {
            connectSettingLoadItem.visible = false;
            connectSettingLoader.source = "";
            if (!accountTextField.focus && !passwordTextField.focus) {
                accountTextField.focus = true;
            }
            return false;
        }
        return true;
    }

    Item {
        id: registerLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: registerLoader
            anchors.fill: parent
        }
        Connections {
            target: registerLoader.item
            onReturnBack: {
                theBackAnimation.popObject();
                registerLoadItem.visible = false;
                registerLoader.source = "";
                if (!accountTextField.focus && !passwordTextField.focus) {
                    accountTextField.focus = true;
                }
            }
            onRegisterOk: {
                theBackAnimation.popObject();
                registerLoadItem.visible = false;
                registerLoader.source = "";
                accountTextField.text = account;
//                accountTextField.text = userId;
                passwordTextField.focus = true;
            }
        }
    }
    Item {
        id: connectSettingLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: connectSettingLoader
            anchors.fill: parent
        }
        Connections {
            target: connectSettingLoader.item
            onReturnBack: {
                theBackAnimation.popObject();
                connectSettingLoadItem.visible = false;
                connectSettingLoader.source = "";
                if (!accountTextField.focus && !passwordTextField.focus) {
                    accountTextField.focus = true;
                }
            }
            onServerChanged: {
                theBackAnimation.popObject();
                connectSettingLoadItem.visible = false;
                connectSettingLoader.source = "";
                if (!accountTextField.focus && !passwordTextField.focus) {
                    accountTextField.focus = true;
                }
//                var newServer = theApp.connectSettingServer;
                tipText.text = "";
                theApp.exitApp(true);
                theApp.initApp();
                theApp.setDevAppId();
            }
        }
    }

    function updateEntLogo(filePath) {
        if (filePath.length>0 && theApp.fileExist(filePath)) {
            logoImage.source = "file:///"+filePath;
//            logoImage.source = "file://"+filePath;
        }
        else {
            logoImage.source = "qrc:/res/entlogo.png";
        }
    }


    /// 产品 LOGO 背景
    Rectangle {
        id: titleBg
        color: mainColor
        x:0
        y:0
        width: parent.width
        height: titleHeight

        Image {
            id: logoImage;
            anchors.fill: parent;
            width: 120
            height: width
            x: (parent.width-width)/2
            y: 50
            fillMode: Image.PreserveAspectFit
        }
    }
    ///  左上角产品名称
    Label {
        id: productNameText
        x: 5
        y: 5
        z: 20
        visible: true
        text: theApp.productName
        width: 120
        height: 32
        font.pointSize: 15
//        font.bold: true
        color: "#fff"
    }

    function onReturnBackSetting() {
        /// 退出设置菜单，重新设置 account focus true
        accountTextField.focus = true;
    }
    function checkHideLoginSetting(id) {
        if (ebLoginSetting!=null && ebLoginSetting.visible) {
            ebLoginSetting.visible = false;
        }
    }
    property var ebLoginSetting: null
    function createMenuSetting(){
        if (ebLoginSetting==null){
            ebLoginSetting = Qt.createComponent("EbLoginSetting.qml").createObject(rootItem);
            ebLoginSetting.width = 72;
            ebLoginSetting.height = 64;
            ebLoginSetting.y = 32;
            ebLoginSetting.returnBack.connect(rootItem.onReturnBackSetting);
        }
        else {
            ebLoginSetting.visible = !ebLoginSetting.visible;
        }
        ebLoginSetting.x = parent.width-ebLoginSetting.width;
        if (ebLoginSetting.visible) {
            ebLoginSetting.focus = true;
        }
        else {
            accountTextField.focus = true;
        }
    }

    function showLoginRecord(){
        loginRecordItem.visible = !loginRecordItem.visible;
        if (loginRecordItem.visible) {
//            loginRecordItem.focus = true;
            ebLoginRecord.focus = true;
            ebLoginRecord.initData();
        }
    }

    /// 右上角设置按钮
    EbButton {
        visible: false  /// 暂时不支持
        width: 48
        height: 32
        x: parent.width-width
        y: 0
        fontFamily: "FontAwesome"
        label: "\uf013"
        textColor: "#fff"
        buttonColor: mainColor
        borderColor: "transparent"
        borderPressedColor: "#fff"
        onClickedButton: {
            createMenuSetting();
        }
    }
    Item {
        id: loginRecordItem
        x: 0
        y: rootItem.mapFromItem(accountTextField, 0, accountTextField.height).y;
        z: 2
        width: parent.width
        height: 36
        visible: false
        Row {
            anchors.centerIn: parent
            height: parent.height

            EbLoginRecord {
                id: ebLoginRecord
                width: accountTextField.width
                height: parent.height
                onFirstLoginInfo: {
                    if (accountInput.length>0)
                        accountTextField.text = accountInput;
                    else if (accountReal.length>0)
                        accountTextField.text = accountReal;
                    else
                        accountTextField.text = userId;
                    if (savePassword.length>0) {
                        passwordTextField.text = "********abc";
                    }
                    else {
                        passwordTextField.text = savePassword;
                    }
                    passwordTextField.focus = true;
                    rootItem.savePassword = savePassword;
                }

                onSelectedLoginInfo: {
                    loginRecordItem.visible = false;
                    if (accountInput.length>0)
                        accountTextField.text = accountInput;
                    else if (accountReal.length>0)
                        accountTextField.text = accountReal;
                    else
                        accountTextField.text = userId;
                    if (savePassword.length>0) {
                        passwordTextField.text = "********abc";
                    }
                    else {
                        passwordTextField.text = savePassword;
                    }
                    passwordTextField.focus = true;
                    rootItem.savePassword = savePassword;
                }
                onReturnBack: {
                    loginRecordItem.visible = false;
                    if (!accountTextField.focus && !passwordTextField.focus) {
                        accountTextField.focus = true;
                    }
                }
            }
        }
    }

    function inputWidth() {
        var width = rootItem.width-30*2
        if (width>260)
            width=260;
        return width;
    }

//    property int iconWidth: 20
    property int rowHeight: 32
    Item {
        id: itemAccount
        x: 0
        y: titleBg.height+15
        width: parent.width
        height: rowHeight
        Row {
            anchors.centerIn: parent
            height: parent.height
            EbIconTextField {
                id: accountTextField
                width: inputWidth()
                height: parent.height
                iconText: "\uf007"
                rightPadding: parent.height
//                verticalAlignment: TextField.AlignVCenter
                onFocusChanged: {
                    if (accountTextField.focus) {
                        if (loginRecordItem.visible) {
                            loginRecordItem.visible = false;
                        }
                        checkHideLoginSetting();
                    }
                }

                EbButton {
                    border.width: 0
                    width: parent.height+2
                    height: parent.height+2
                    x: parent.width-width
                    y: loginRecordItem.visible?4:-4
                    z: 1
                    fontFamily: "FontAwesome"
                    label: loginRecordItem.visible?"\uf0de":"\uf0dd"
                    textColor: "#808080"
                    buttonColor: "transparent"
                    borderColor: "transparent"
                    fontPointSize: 14
                    onClickedButton: { showLoginRecord(); }
                }
            }
        }
    }
    Item {
        x: 0
//        y: rootItem.mapFromItem(accountTextField, 0, accountTextField.height).y+10;
        y: titleHeight+20+rowHeight+10
        width: parent.width
        height: rowHeight
        Row {
            height: parent.height
            anchors.centerIn: parent

            EbIconTextField {
                id: passwordTextField
                width: accountTextField.width
                height: parent.height
                iconText: "\uf023"
                rightClearButtonEnable: true
                echoMode: TextInput.Password
                onFocusChanged: {
                    if (passwordTextField.focus) {
                        checkHideLoginSetting();
                    }
                }

            }
        }
    }

    Item {
        x: 0
        y: titleHeight+20+(rowHeight+10)*2+5
        width: parent.width
        height: rowHeight+6
        Row {
            height: parent.height
            anchors.centerIn: parent
            EbButton {
                id: loginButton
                z: 10
                enabled: false
                width: accountTextField.width
                height: parent.height
                radius: 3
                border.width: 0
                fontBold: true
                textColor: "#fff"
                buttonColor: mainColor
                buttonHoverColor: theApp.mainHoverColor
                buttonPressedColor: theApp.mainPressedColor
                onClickedButton: {
                    /// for test
//                    theApp.openFile("abc");
//                    theApp.getManufacturerName();
                    checkHideLoginSetting();
                    if (accountTextField.text.length===0) {
                        tipText.text = theLocales.getLocalText("message-show.account-empty-error", "Account empty error!");
                        accountTextField.focus = true;
                        return;
                    }
                    else if (passwordTextField.text.length===0) {
                        tipText.text = theLocales.getLocalText("message-show.password-empty", "Password empty error!");
                        passwordTextField.focus = true;
                        return;
                    }
                    tipText.text = "";
                    if (passwordTextField.text=="********abc" ||
                            passwordTextField.text==rootItem.savePassword) {
                        theApp.login(accountTextField.text, "", rootItem.savePassword);
                    }
                    else {
                        theApp.login(accountTextField.text, passwordTextField.text, "");
                    }
                }
            }
        }
    }
    Item {
        x: 0
        y: titleHeight+20+(rowHeight+10)*2+loginButton.height+10
        width: parent.width
        height: 64
        Row {
            height: parent.height
            anchors.centerIn: parent
            Text {
                id: tipText
                width: accountTextField.width
                height: parent.height
                wrapMode: Text.Wrap
                lineHeight: 1.2
                color: "red"
//                font.bold: true
            }
        }
    }

    /// 底部四个按钮
    property int buttonWidth: 68
    property int buttonHeight: 26
    Item {
        x: 0
        y: parent.height-buttonHeight-12
//        y: parent.height-buttonHeight-productVersionText.height
        width: parent.width
        height: 32
        Row {
            anchors.centerIn: parent
            spacing: 8
            EbButton {
                id: visitorButton
                visible: false
                width: buttonWidth
                height: buttonHeight
                border.width: 0
                textColor: mainColor
                buttonColor: "transparent"
                buttonPressedColor: "#eee"
                borderColor: "transparent"
                borderPressedColor: "#eee"
                onClickedButton: {
                    checkHideLoginSetting();
                    theApp.loginVisitor();
                }
            }



//            Component {
//                id: rect
//                Rectangle {
//                    width: 50
//                    height: 50
//                    color: "red"
//                }
//            }
            EbButton {
                id: registerButton
                visible: false
                width: buttonWidth
                height: buttonHeight
                border.width: 0
                textColor: mainColor
                buttonColor: "transparent"
                buttonPressedColor: "#eee"
                borderColor: "transparent"
                borderPressedColor: "#eee"
                onClickedButton: {
                    checkHideLoginSetting();
                    theBackAnimation.addObject();
                    registerLoader.source = "EbRegister.qml";
                    registerLoadItem.visible = true;
                    registerLoadItem.focus = true;
//                    registerLoader.setSource("EbRegister.qml",
//                                             {"width": 200, "height": 200});
                }
            }
            EbButton {
                id: forgetPasswordButton
                visible: false
                width: buttonWidth
                height: buttonHeight
                border.width: 0
                textColor: mainColor
                buttonColor: "transparent"
                buttonPressedColor: "#eee"
                borderColor: "transparent"
                borderPressedColor: "#eee"
                onClickedButton: {
                    Qt.openUrlExternally(theApp.forgetPwdUrl);
                }
            }
            EbButton {
                id: connectSettingButton
                width: buttonWidth
                height: buttonHeight
                z: 1
                border.width: 0
                textColor: mainColor
                buttonColor: "transparent"
                buttonPressedColor: "#eee"
                borderColor: "transparent"
                borderPressedColor: "#eee"
                onClickedButton: {
                    checkHideLoginSetting();
                    theBackAnimation.addObject();
                    connectSettingLoader.source = "EbConnectSetting.qml";
                    connectSettingLoadItem.visible = true;
                    connectSettingLoadItem.focus = true;
                    connectSettingLoader.item.initData(theApp.connectSettingServer);
                }
            }

        }
    }
    /// 底部右下角产品版本号
    Text {
        id: productVersionText
        x: parent.width-width
        y: parent.height-height
        width: 56
        height: 16
        text: theApp.productVersion
        font.pointSize: 10
        color: "#ccc"
    }

//    Item {
//        id: productNameVersion
//        x: 0
//        y: parent.height-height-3
//        width: parent.width
//        height: 20
//        Row {
//            anchors.centerIn: parent
//            height: parent.height
//            spacing: 2

//            Text {
//                id: productVersionText
//                width: 80
//                text: theApp.productVersion
//                height: parent.height
//                visible: true
//                font.pointSize: 10
//                color: "#bbb"
//            }
//        }
//    }

    function updateLocaleInfo() {
        accountTextField.placeholderText = theLocales.getLocalText("login-dialog.edit-account.bg-text", "");
        passwordTextField.placeholderText = theLocales.getLocalText("login-dialog.edit-password.bg-text", "");
        loginButton.label = theLocales.getLocalText("login-dialog.button-logon.text", "Login");
        visitorButton.label = theLocales.getLocalText("login-dialog.button-visitor-logon.text", "Visitor");
        registerButton.label = theLocales.getLocalText("login-dialog.button-register.text", "Register");
        forgetPasswordButton.label = theLocales.getLocalText("login-dialog.button-forget-pwd.text", "Forget Password");
        connectSettingButton.label = theLocales.getLocalText("login-dialog.button-connect-setting.text", "Setting");
    }

    Connections {
        target: theApp
        onReceivedGroupInfo: {
//            console.log("onGroupInfo: enterprise-id=" + groupInfo.enterpriseId);
//            console.log("onGroupInfo: group-name=" + groupInfo.groupName);
            tipText.text = groupInfo.groupName;
        }

        onAppidSuccess: {
            if (!logoutMode) {
                tipText.text = "";
            }
            loginButton.enabled = true;
            productNameText.text = theApp.productName
            productVersionText.text = theApp.productVersion
            /// 手机端暂时不支持游客登录
//            visitorButton.visible = theApp.isOpenVisitor;
            registerButton.visible = theApp.isOpenRegister;
            forgetPasswordButton.visible = theApp.forgetPwdUrl.length>0?true:false;
            if (theApp.isLicenseUser)
                updateEntLogo(theApp.appImgPath + "/entlogo");    /// 企业定制LOGO，空或不存在使用系统默认
            else
                updateEntLogo("");
            if (!rootItem.logoutMode && accountTextField.text.length>0 &&
                    rootItem.savePassword.length>0) {
                /// 自动登录
                loginButton.enabled = false;
                theApp.login(accountTextField.text, "", rootItem.savePassword);
            }
        }
        onAppidError: {
            updateEntLogo("");
            tipText.text = errorString;
        }

        onLoginSuccess: {
//            console.log("EbQuickAccountInfo: account=" + accountInfo.account);
            theBackAnimation.removeFilter(rootFilterObject, 1);
            rootItem.loginSuccess();
        }
        onLoginError: {
            loginButton.enabled = true;
            accountTextField.focus = true;
            tipText.text = errorString;
        }
    }

    EbBackAnimation {
        id: theBackAnimation
        z: 10
        interval: 3000
        display: theLocales.getLocalText("message-show.press-again-exit", "Press Again Exit");
        anchors.centerIn: parent
        onReturnBack: {
            rootItem.returnBack();
        }
        onPopObjectBack: {
            if (requestReturnBack()) {
                theBackAnimation.clearObject();
            }
        }
    }

    property var rootFilterObject: null
    Component.onCompleted: {
        var docRoot = rootItem.parent;
        while (docRoot.parent) {
            docRoot = docRoot.parent;
        }
        rootFilterObject = docRoot;
        theBackAnimation.setFilter(rootFilterObject, 1);
        /// 程序启动，先默认显示定制图片
        /// 企业定制LOGO，空或不存在使用系统默认
        updateEntLogo(theApp.appImgPath + "/entlogo");
        updateLocaleInfo();
        theApp.setDevAppId();
    }

}

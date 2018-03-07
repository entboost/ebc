import QtQuick 2.0
import Entboost.KeyFilter 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent

    property string registerUserId: ""
    property string registerAccount: ""

    signal returnBack();
    signal registerOk(string userId, string account);

    Column {
        anchors.fill: parent

        EbTitleBar {
            id: titleBar;
            width: parent.width
            onReturnBack: rootItem.returnBack()
        }
        EbInputLine {
            id: accountInput
            focus: true
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            id: passwordInput
            passwordMode: true
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            id: passwordConfirm
            passwordMode: true
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            id: userNameInput
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbInputLine {
            id: enterpriseNameInput
            visible: theApp.isOpenRegisterEnterprise
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
            visible: theApp.isOpenRegisterEnterprise
        }

        Item {
            width: parent.width
            height: 20
        }
        EbButton {
            id: registerButton
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
                var account = accountInput.text;
                var password = passwordInput.text;
                var confirm = passwordConfirm.text;
                if (account.length===0) {
                    tipText.text = theLocales.getLocalText("message-show.account-empty-error", "Account Empty");
                    accountInput.focus = true;
                }
                else if (password.length===0) {
                    tipText.text = theLocales.getLocalText("message-show.password-empty", "Password Empty");
                    passwordInput.focus = true;
                }
                else if (confirm.length===0) {
                    tipText.text = theLocales.getLocalText("message-show.confirm-pwd-empty", "Confirm Password Empty");
                    passwordConfirm.focus = true;
                }
                else if (password!==confirm) {
                    tipText.text = theLocales.getLocalText("message-show.password-not-equal", "Password Not Equal");
                    passwordConfirm.focus = true;
                    passwordConfirm.selectAll();
                }
                else {
                    tipText.text = "";
                    registerButton.enabled = false;
                    theApp.registerAccount(account, password, userNameInput.text, enterpriseNameInput.text);
                }

            }
        }

        Item {
            id: okLine
            width: parent.width
            height: 10
            visible: false
        }
        EbButton {
            id: okButton
            x: 20
            width: parent.width-x*2
            height: 36
            visible: false
            radius: 3
            border.width: 1
            textColor: theApp.mainColor
            textPressedColor: theApp.mainPressedColor
            textHoverColor: theApp.mainHoverColor
            borderColor: theApp.mainColor
            borderPressedColor: theApp.mainPressedColor
            borderHoverColor: theApp.mainHoverColor
            buttonColor: "#fff"
            onClickedButton: { rootItem.registerOk(registerUserId, registerAccount); }
        }

        Item {
            width: parent.width
            height: 10
        }
        Text {
            id: tipText
            x: registerButton.x
            width: registerButton.width
            height: 68
            wrapMode: Text.Wrap
            lineHeight: 1.2
            color: "red"
        }
    }

    Connections {
        target: theApp
        onRegisterSuccess: {
            registerUserId = userId;
            registerAccount = account;
            tipText.text = tip;
            registerButton.enabled = true;
            resetData(true);
        }
        onRegisterError: {
            registerUserId = "";
            registerAccount = "";
            tipText.text = errorString;
            registerButton.enabled = true;
            resetData(fale);
        }
    }

    function resetData(okVisible) {
        accountInput.text = "";
        passwordInput.text = "";
        passwordConfirm.text = "";
        userNameInput.text = "";
        enterpriseNameInput.text = "";
        okLine.visible = okVisible;
        okButton.visible = okVisible;
    }

    function updateLocaleInfo() {
        titleBar.title = theLocales.getLocalText("register-dialog.title", "Register New User");
        accountInput.label = theLocales.getLocalText("register-dialog.label-account.text", "Account:");
        accountInput.placeholderText = theLocales.getLocalText("register-dialog.edit-account.bg-text", "");
        passwordInput.label = theLocales.getLocalText("register-dialog.label-password.text", "Password:");
        passwordConfirm.label = theLocales.getLocalText("register-dialog.label-confirm.text", "Confirm Password:");
        userNameInput.label = theLocales.getLocalText("register-dialog.label-user-name.text", "User Name:");
        enterpriseNameInput.label = theLocales.getLocalText("register-dialog.label-ent-name.text", "Enterprise Name:");
        registerButton.label = theLocales.getLocalText("base-dialog.register-button.text", "Register");
        okButton.label = theLocales.getLocalText("base-dialog.ok-button.text", "Ok");
    }

    /// 延时返回退出
    Timer {
        id: timerReturnBack
        interval: 300
        triggeredOnStart: false
        onTriggered: {
            resetData(false);
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
        updateLocaleInfo();
        theKeyFilter.setFilter(rootItem, 3);
    }
}

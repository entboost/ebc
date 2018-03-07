import QtQuick 2.0
import Entboost.KeyFilter 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent

    property string registerUserId: ""
    property string registerAccount: ""

    signal returnBack();
    signal requestLogout(string tip);
//    signal registerOk(string userId, string account);

    Column {
        anchors.fill: parent

        EbTitleBar {
            id: titleBar;
            width: parent.width
            onReturnBack: rootItem.returnBack()
        }

//        Rectangle {
//            width: parent.width
//            height: 10
//            color: "#eee"
//        }
//        Item {
//            width: parent.width
//            height: 10
//        }
        EbIconTextField {
            id: currentPasswordTextField
            x: 30
            width: parent.width-x*2
            height: 36
//            placeholderText: "请输入当前密码"
            iconText: "\uf023"
            rightClearButtonEnable: true
            echoMode: TextInput.Password
            background: Rectangle {
                border.width: 0
            }
            onTextChanged: { okButton.enabled = checkOkButtonEnable();}
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbIconTextField {
            id: newPasswordTextField
            x: 30
            width: parent.width-x*2
            height: 36
//            placeholderText: "请输入新密码"
            iconText: "\uf023"
            rightClearButtonEnable: true
            echoMode: TextInput.Password
            background: Rectangle {
                border.width: 0
            }
            onTextChanged: { okButton.enabled = checkOkButtonEnable();}
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbIconTextField {
            id: confirmPasswordTextField
            x: 30
            width: parent.width-x*2
            height: 36
//            placeholderText: "请输入确认密码"
            iconText: "\uf023"
            rightClearButtonEnable: true
            echoMode: TextInput.Password
            background: Rectangle {
                border.width: 0
            }
            onTextChanged: { okButton.enabled = checkOkButtonEnable();}
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }

        Item {
            width: parent.width
            height: 20
        }
        EbButton {
            id: okButton
            x: currentPasswordTextField.x
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            textColor: "#fff"
            buttonColor: theApp.mainColor
            buttonHoverColor: theApp.mainHoverColor
            buttonPressedColor: theApp.mainPressedColor
            enabled: false
//            enabled: rootItem.checkOkButtonEnable()
            onClickedButton: {
                var currentPassword = currentPasswordTextField.text;
                var newPassword = newPasswordTextField.text;
                var confirmPassword = confirmPasswordTextField.text;
                if (currentPassword.length===0) {
                    tipText.text = theLocales.getLocalText( "message-show.current-pwd-empty","Current Password Empty" )
                    currentPasswordTextField.focus = true;
                }
                else if (newPassword.length===0) {
                    tipText.text = theLocales.getLocalText("message-show.password-empty", "Password Empty");
                    newPasswordTextField.focus = true;
                }
                else if (confirmPassword.length===0) {
                    tipText.text = theLocales.getLocalText("message-show.confirm-pwd-empty", "Confirm Password Empty");
                    confirmPasswordTextField.focus = true;
                }
                else if (newPassword!==confirmPassword) {
                    tipText.text = theLocales.getLocalText("message-show.password-not-equal", "Password Not Equal");
                    confirmPasswordTextField.focus = true;
                    confirmPasswordTextField.selectAll();
                }
                else {
                    tipText.text = "";
                    okButton.enabled = false;
                    theApp.setPassword(currentPassword, newPassword);
                }

            }
        }
        Item {
            width: parent.width
            height: 10
        }
        Text {
            id: tipText
            x: okButton.x
            width: okButton.width
            height: 68
            wrapMode: Text.Wrap
            lineHeight: 1.2
            color: "red"
        }
    }
    function checkOkButtonEnable() {
        var currentPassword = currentPasswordTextField.text;
        var newPassword = newPasswordTextField.text;
        var confirmPassword = confirmPasswordTextField.text;
        if (currentPassword.length===0) {
            return false;
        }
        else if (newPassword.length===0) {
            return false;
        }
        else if (confirmPassword.length===0) {
            return false;
        }
        else if (newPassword!==confirmPassword) {
            return false;
        }
        else {
            return true;
        }
    }

    Connections {
        target: theApp
        onSetPasswordResponse: {
            tipText.text = stateString;
            okButton.enabled = true;
            if (stateCode==0) {
                currentPasswordTextField.text = "";
                newPasswordTextField.text = "";
                confirmPasswordTextField.text = "";
                rootItem.requestLogout(stateString);
            }
            else {
                currentPasswordTextField.focus = true;
            }
        }
    }

    function updateLocaleInfo() {
        titleBar.title = theLocales.getLocalText("my-center-dialog.button-modify-password.text", "Modify password");
        currentPasswordTextField.placeholderText = theLocales.getLocalText("my-center-dialog.modify-password-widget.label-current-password.bg-text", "Current password");
        newPasswordTextField.placeholderText = theLocales.getLocalText("my-center-dialog.modify-password-widget.label-new-password.bg-text", "New password");
        confirmPasswordTextField.placeholderText = theLocales.getLocalText("my-center-dialog.modify-password-widget.label-confirm-password.bg-text", "Confirm password");
        okButton.label = theLocales.getLocalText("base-dialog.ok-button.text", "Ok");
    }

    /// 延时返回退出
    Timer {
        id: timerReturnBack
        interval: 200
        triggeredOnStart: false
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
        updateLocaleInfo();
    }
}

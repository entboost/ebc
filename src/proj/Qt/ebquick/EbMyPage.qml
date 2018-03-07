import QtQuick 2.0
import Entboost.SubscribeFuncInfo 1.0

EbPage {
    id: rootItem

    signal requestLogout();
    signal requestAddContact();
    signal pressedAccountInfo(var accountInfo);
    signal pressedFuncInfo(var funcInfo);
    signal requestModifyPassword();
    signal requestMessageNotifySetting();
    signal requestSearch();

    property var myMessageFuncInfo: null
    property var myCollectFuncInfo: null
    property var myAccountInfo: null
    function updateMyData() {
        myAccountInfo = theApp.myAccountInfo();
        myAccountImageNameInfo.name = myAccountInfo.userName;
        myAccountImageNameInfo.info = myAccountInfo.account + "(" + myAccountInfo.userId + ")";

        checkFuncInfoTimer.stop()
        rootItem.myMessageFuncInfo = null;
        rootItem.myCollectFuncInfo = null;
        checkFuncInfoTimer.start();
    }

    EbMenu {
        id: theTitleMenu
        EbMenuItem {
            id: addContactMenuItem
//            label: "添加联系人"
            onTriggered: { rootItem.requestAddContact(); }
        }
    }
    header: Item {
        id: headerItem;
        height: titleBar.height
        EbTitleBar {
            id: titleBar;
            width: parent.width
            iconVisible: false
//            title: "个人中心"
            rightButton1Visible: true
            rightButton1Label: "\uf002"
            onPressedRightButton1: { requestSearch(); }
            rightButton0Visible: true
            rightButton0Label: "\uf067"
            onPressedRightButton0: {
                theTitleMenu.x = parent.width-theTitleMenu.width-10;
                theTitleMenu.y = titleBar.height;
                theTitleMenu.open();
                theTitleMenu.parent = rootItem;
            }
        }
    }

    Column {
        y: titleBar.height
        anchors.fill: parent

//        Rectangle {
//            width: parent.width
//            height: 8
//            color: "#eee"
//        }
        EbImageNameInfo {
            id: myAccountImageNameInfo
            source: theApp.myHeadImage;
            pressEnable: true
            onPressed: {
                rootItem.pressedAccountInfo(rootItem.myAccountInfo);
            }
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }
        EbTextLine {
            id: myMessageText
//            label: "我的消息"
            visible: false
            visibleLeftIcon: true
            leftIconText: "\uf27b"
            pressEnable: true
            onPressed: {
                if (rootItem.myMessageFuncInfo!=null) {
                    rootItem.pressedFuncInfo(rootItem.myMessageFuncInfo);
                }
            }
        }
        Rectangle {
            id: myMessageLine
            visible: false
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            id: myCollectText
//            label: "个人收藏"
            visible: false
            visibleLeftIcon: true
            leftIconText: "\uf1b2"
            pressEnable: true
            onPressed: {
                if (rootItem.myCollectFuncInfo!=null) {
                    rootItem.pressedFuncInfo(rootItem.myCollectFuncInfo);
                }
            }
        }
        Rectangle {
            id: myCollectLine
            visible: false
            width: parent.width
            height: 8
            color: "#eee"
        }
        EbTextLine {
            id: messageNotifyTextLine
//            label: "通知提醒"
            visibleLeftIcon: true
            leftIconText: "\uf0f3"
//            leftIconText: "\uf013"
            pressEnable: true
            onPressed: {
                rootItem.requestMessageNotifySetting();
            }
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            id: modifyPasswordTextLine
//            label: "修改密码"
            visible: !theApp.isDisableModifyPasswd()
            visibleLeftIcon: true
            leftIconText: "\uf13e"
            pressEnable: true
            onPressed: { rootItem.requestModifyPassword(); }
        }
//        Rectangle {
//            width: parent.width
//            height: 1
//            color: "#eee"
//        }
//        EbTextLine {
//            id: checkVersionTextLine
//            label: "检查版本"
//            visibleLeftIcon: true
//            leftIconText: "\uf13e"
//            pressEnable: true
//            onPressed: { theApp.checkVersion(); }
//        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }
        EbTextLine {
            id: logoutTextLine
//            label: "更换帐号"
            visibleLeftIcon: true
            leftIconText: "\uf090"
            pressEnable: true
            onPressed: { requestLogout(); }
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            id: quitText
//            label: "退出"
            visibleLeftIcon: true
            leftIconText: "\uf08b"
            pressEnable: true
            onPressed: { Qt.quit(); }
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }

    }

    Timer {
        id: checkFuncInfoTimer
        interval: 2000
        repeat: true
        property int repeatCount: 0
        onTriggered: {
            /// 我的消息
            if (rootItem.myMessageFuncInfo==null) {
                rootItem.myMessageFuncInfo = theApp.subscribeFuncInfo(1002300103);
                if (rootItem.myMessageFuncInfo==null) {
                    myMessageText.visible = false;
                    myMessageLine.visible = false;
                }
                else {
                    myMessageText.label = rootItem.myMessageFuncInfo.functionName;
                    myMessageText.visible = true;
                    myMessageLine.visible = true;
                }
            }
            /// 个人收藏
            if (rootItem.myCollectFuncInfo==null) {
                rootItem.myCollectFuncInfo = theApp.subscribeFuncInfo(1002300106);
                if (rootItem.myCollectFuncInfo==null) {
                    myCollectText.visible = false;
                    myCollectLine.visible = false;
                }
                else {
                    myCollectText.label = rootItem.myCollectFuncInfo.functionName;
                    myCollectText.visible = true;
                    myCollectLine.visible = true;
                }
            }

            if (rootItem.myMessageFuncInfo!=null &&
                    rootItem.myCollectFuncInfo!=null) {
                checkFuncInfoTimer.stop();
            }
            else {
                checkFuncInfoTimer.repeatCount = checkFuncInfoTimer.repeatCount+1;
                if (checkFuncInfoTimer.repeatCount>10) {
                    checkFuncInfoTimer.stop();
                }

            }

        }

    }

    function updateLocaleInfo() {
        titleBar.title = theLocales.getLocalText("my-center-dialog.title", "My center");
        addContactMenuItem.label = theLocales.getLocalText("context-menu.add-contact.text", "New Contact");
        messageNotifyTextLine.label = theLocales.getLocalText("my-center-dialog.button-message-notify.text", "Message notify");
        modifyPasswordTextLine.label = theLocales.getLocalText("my-center-dialog.button-modify-password.text", "Modify password");
        logoutTextLine.label = theLocales.getLocalText("context-menu.logout.text", "Logout");
        quitText.label = theLocales.getLocalText("context-menu.quit.text", "Quit");
//        accountInput.placeholderText = theLocales.getLocalText("register-dialog.edit-account.bg-text", "");
//        registerButton.label = theLocales.getLocalText("base-dialog.register-button.text", "Register");
    }

    Connections {
        target: theApp
        onMyHeadImageChanged: {
            myAccountImageNameInfo.source = "";
            myAccountImageNameInfo.source = imageUrl;
        }
        onSetAccountInfoResponse: {
            if (stateCode==0) {
                rootItem.myAccountInfo = theApp.myAccountInfo();
                myAccountImageNameInfo.name = rootItem.myAccountInfo.userName;
            }
        }

    }

    Component.onCompleted: {
        updateLocaleInfo();
    }

}

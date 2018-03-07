import QtQuick 2.0
import QtQuick.Controls 2.0
import Entboost.KeyFilter 1.0

Rectangle {
    id: rootItem
    signal returnBack()
    signal serverChanged(string newServer)

    anchors.fill: parent
    width: parent.width
    height: parent.height-32;

    EbTitleBar {
        id: titleBar;
        width: parent.width
        onReturnBack: rootItem.returnBack()
    }

    property string currentServer: ""
    TextField {
        id: serverTextField
        x: 10
        y: titleBar.height+8
        width: parent.width-10*2;
        height: 28
        focus: true
        text: currentServer
    }
    Text {
        id: tipText
        x: 12
        y: titleBar.height+serverTextField.height+15
        width: serverTextField.width
        height: 42
        color: "red"
        wrapMode: Text.Wrap
    }
    EbButton {
        id: saveButton
        x: 10
        y: titleBar.height+serverTextField.height+tipText.height+24
        width: serverTextField.width
        height: 32
        radius: 3
        border.width: 0
        textColor: "#fff"
        buttonColor: theApp.mainColor
        buttonHoverColor: theApp.mainHoverColor
        buttonPressedColor: theApp.mainPressedColor
        onClickedButton: {
            var newServer = serverTextField.text;
            if (currentServer!=newServer || !theApp.isAppIdResponseOk) {
                theApp.connectSettingServer = newServer;
                currentServer = newServer;
                rootItem.serverChanged(newServer);
            }
            else {
                rootItem.returnBack();
            }
        }
    }

    function initData(server) {
        currentServer = server;
        serverTextField.text = currentServer;
        serverTextField.focus = true;
    }

    function updateLocaleInfo() {
        titleBar.title = theLocales.getLocalText("connect-setting-dialog.label-server-address.text", "Server Address:");
        serverTextField.placeholderText = theLocales.getLocalText("connect-setting-dialog.edit-server-address.bg-text", "");
        tipText.text = theLocales.getLocalText("connect-setting-dialog.edit-server-address.tooltip", "");
        saveButton.label = theLocales.getLocalText("base-dialog.save-button.text", "Save");
    }

    onFocusChanged: {
        if (rootItem.focus) {
            serverTextField.focus = true;
        }
    }
    /// 延时返回退出
    Timer {
        id: timerReturnBack
        interval: 300
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
        updateLocaleInfo();
        theKeyFilter.setFilter(rootItem, 3);
    }

//    MouseArea {
//        anchors.fill: parent
//    }
    Keys.onBackPressed: {
        event.accepted = true;
//        rootItem.visible = false;
        rootItem.returnBack();
    }
}

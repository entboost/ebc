import QtQuick 2.5
import QtQuick.Controls 2.2
//import QtQuick.Dialogs 1.2
import Entboost.KeyFilter 1.0

Rectangle {
    id: rootItem
    border.width: 1
    border.color: "#ccc"
    property int itemHeight: 36

    signal returnBack();
    signal firstLoginInfo(string accountInput, string accountReal, string userId, string savePassword);
    signal selectedLoginInfo(string accountInput, string accountReal, string userId, string savePassword);
//    signal exitMessageBox();
    function updateLocaleInfo() {
        deleteLoginIntoMenuItem.label = theLocales.getLocalText("login-dialog.button-delete-account.tooltip", "Delete Account");
    }
    /// 根据列表数量，自动调整高度
    function rebuildSize() {
        var countHeight = listView.count;
        if (countHeight>6)
            countHeight = 6;
        else if(countHeight==0)
            countHeight = 1;
        rootItem.height = countHeight*itemHeight;
    }
    function initData() {
        if (listView.count==0) {
            theApp.loadLoginData();
            rebuildSize();
        }
    }

    property var deleteMessageBox: null
    function onAcceptedMessageBox() {
        rootItem.focus = true;
        var object = theModel.get(listView.currentIndex);
        if (object!=null) {
            var error = theApp.deleteLoginData(object.accountInput, object.accountReal, object.userId);
            if (error.length===0) {
                theModel.remove(listView.currentIndex, 1);
                rebuildSize();
            }
        }
    }
    function onRejectedMessageBox() {
        rootItem.focus = true;
    }

    EbMenu {
        id: loginRecordMenu
        EbMenuItem {
            id: deleteLoginIntoMenuItem
            onTriggered: {
                var object = theModel.get(listView.currentIndex);
//                console.log("object= " + object);
                if (object!=null) {
//                    console.log("object.accountInput= " + object.accountInput);
//                    console.log("object.accountReal= " + object.accountReal);
//                    console.log("object.userId= " + object.userId.toString());

                    if (deleteMessageBox==null) {
                        deleteMessageBox = Qt.createComponent("EbMessageDialog.qml").createObject(rootItem);
                        deleteMessageBox.accepted.connect(rootItem.onAcceptedMessageBox);
                        deleteMessageBox.rejected.connect(rootItem.onRejectedMessageBox);
                    }

                    var title = theLocales.getLocalText("message-box.delete-logon-account.title", "Delete Login Account");
                    if (title.length===0) {
                        title = theApp.productName;
                    }
                    deleteMessageBox.title = title;
                    var text = theLocales.getLocalText("message-box.delete-logon-account.text", "Confirm Delete Local <br>Account and Chat Messages?");
                    text = text.replace("[USER_ACCOUNT]", object.accountReal);
                    if (object.userId>0) {
                        text = text.replace("[USER_ID]", object.userId.toString());
                    }
                    deleteMessageBox.text = text;
                    deleteMessageBox.open();
                }
            }
        }
    }

    ListModel {
        id: theModel
//        ListElement { name: "中文简体"; file: "zh-CN.json" }
    }
    Component {
        id: myDelegate

        Rectangle {
            id: listItem
            color: "#fff"
            x: 1
            width: parent.width-2
            height: itemHeight

            Image {
                id: imageIcon
                width: parent.height
                height: parent.height-1
                fillMode: Image.PreserveAspectFit
                source: "qrc:/res/entlogo.png"
            }
            Text {
                id: textAccount
                x: imageIcon.width
                width: parent.width-x
                height: parent.height-1
                verticalAlignment: Text.AlignVCenter
                text: {
                    if (accountInput.length>0)
                        accountInput+"(" + userId.toString() + ")"
                    else
                        accountReal+"(" + userId.toString() + ")"
                }
            }
            Rectangle {
                /// line
                x: 0
                y: parent.height-1
                width: parent.width
                height: 1
                color: rootItem.border.color
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                /// for test
                onPressed: {
                    listItem.color = "#eee";
                }
                onExited: {
                    listItem.color = "#fff";
                }

                onClicked: {
                    listView.currentIndex = index;
//                    console.log("clicked index:" + index);
                    rootItem.selectedLoginInfo(
                                accountInput,
                                accountReal,
                                userId.toString(),
                                savePassword);
//                    loginRecordMenu.open();
                }
                onPressAndHold: {
                    mouse.accepted = true;
//                    rootItem.focus = true;
                    listView.currentIndex = index;
                    var obj = mouseArea.mapToGlobal(mouse.x, mouse.y);
                    obj = rootItem.mapFromGlobal(obj.x, obj.y);
//                    loginRecordMenu.x = mouse.x-loginRecordMenu.width+5;
//                    loginRecordMenu.y = mouse.y+index*itemHeight-20;
                    loginRecordMenu.x = obj.x-loginRecordMenu.width+5;
                    loginRecordMenu.y = obj.y-20;
                    loginRecordMenu.open();
                }

            }
        }

    }
    ListView {
        id: listView
        anchors.fill: parent
        x: 1
        y: 1
        width: parent.width-2
        height: parent.height-2
//        highlightFollowsCurrentItem: true
        model: theModel
        delegate: myDelegate
    }
    Connections {
        target: theApp
        onLoginRecordInfo: {
            theModel.append( { accountInput: accountInput,
                                accountReal: accountReal,
                                userId: userId,
                                savePassword: savePassword } );
            if (theModel.count==1) {
                rootItem.firstLoginInfo(
                            accountInput,
                            accountReal,
                            userId.toString(),
                            savePassword);
            }
        }
    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: { rootItem.returnBack(); }
    }
    Component.onCompleted: {
        updateLocaleInfo();
        theKeyFilter.setFilter(rootItem, 3);
        initData();
    }
}

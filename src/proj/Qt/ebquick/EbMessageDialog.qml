import QtQuick 2.0
//import QtQuick.Dialogs 1.2
import Entboost.KeyFilter 1.0

Rectangle {
    id: messageBoxBackground
    property alias title: titleText.text
    property alias titleColor: titleText.color
    property alias text: contentText.text
    property alias textColor: contentText.color
//    property int fontPixelSize : 0.03 * parent.height


    signal accepted();
    signal rejected();
//    signal itemClicked(int index)

    function open() {
        /// 必须设为 true 才能处理 Back 返回退出按钮
        messageBoxBackground.focus = true;
        messageBoxBackground.state = "visible";
//        Qt.inputMethod.hide();
    }
    visible: false
    anchors.fill: parent
    color: "#80000000"
    /// 点击对话框区域外，隐藏对话框，其父窗口最后变为最外层窗口，在Component.onCompleted事件中设置
    MouseArea {
        anchors.fill: parent
        onClicked: {
            //menuRoot.state = "";//非模态时可以这样用
        }
    }
    /// 对话框
    Rectangle {
        id: messageBox
        color: "white"
        radius: 3
        width: 0.8 * parent.width
        height: 0.3 * parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        Column {
            anchors.fill: parent
            Rectangle {
                /// title
                x: 10
                radius: messageBoxBackground.radius
                width: parent.width-10-3
                height: 0.25 * parent.height
                color: "#fff"
                Text {
                    id: titleText
                    anchors.fill: parent
                    horizontalAlignment:Text.AlignLeft
                    verticalAlignment:Text.AlignVCenter
                    font.pointSize: 14
//                    font.pixelSize: messageBoxBackground.fontPixelSize
                    text: "title"
                    color: theApp.mainColor
                }
            }
            Rectangle {
                /// title line
                width: parent.width
                height: 1
                color: messageBoxBackground.titleColor
            }
            Text {
                /// content
                id: contentText
                x: 6
                width: parent.width-12
                height: 0.5 * parent.height
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
//                font.pixelSize: messageBoxBackground.fontPixelSize-1
                wrapMode: Text.Wrap
                text: "Are you ready?"
                color: "#000"
                lineHeight: 1.3
            }
            Rectangle {
                /// line
                width: parent.width
                height: 1
                color: "#eee"
            }
            Row {
                width: parent.width-2
                height: 0.25 * parent.height-3

                EbButton {
                    id: cancelButton
                    width: (parent.width-1)/2
                    height: parent.height
                    label: theLocales.getLocalText("base-dialog.cancel-button.text", "Cancel");
                    textColor: "#222"
                    buttonPressedColor: "#eee"
//                    textHoverColor: "#fff"
//                    buttonHoverColor: theApp.mainColor
//                    fontPixelSize: messageBoxBackground.fontPixelSize
                    onClickedButton: {
                        messageBoxBackground.state = "";
                        messageBoxBackground.rejected();
                    }
                }
                Rectangle {
                    /// line
                    width: 1
                    height: parent.height
                    color: "#eee"
                }
                EbButton {
                    id: okButton
                    width: cancelButton.width
                    height: parent.height
                    label: theLocales.getLocalText("base-dialog.ok-button.text", "Ok");
                    textColor: theApp.mainColor
//                    textHoverColor: "#fff"
//                    buttonHoverColor: theApp.mainColor
                    buttonPressedColor: "#eee"
//                    fontPixelSize: messageBoxBackground.fontPixelSize
                    onClickedButton: {
                        messageBoxBackground.state = "";
                        messageBoxBackground.accepted();
                    }
                }
            }

//            Rectangle {
//                /// button
//                width: parent.width
//                height:0.25 * parent.height
////                color: "#3399db"
//                Text {
//                    anchors.fill: parent
//                    horizontalAlignment:Text.AlignHCenter
//                    verticalAlignment:Text.AlignVCenter
//                    font.pixelSize: fontSize
//                    text: "确定"
//                }
//                MouseArea {
//                    anchors.fill: parent//
//                    onClicked: {
//                        console.log("onClicked 2")
//                        messageBoxBackground.state = "";
//                    }
//                }
//            }
        }
    }
    states: State {
        name: "visible"
        PropertyChanges {target: messageBoxBackground; visible: true}
    }
    transitions: Transition {
        NumberAnimation {
            target:  messageBoxBackground
            properties: "visible"
            duration: 250
        }
    }

    EbKeyFilter {
        id: aKeyFilter
        onKeyBackPressed: {
            messageBoxBackground.state = "";
            messageBoxBackground.rejected();
        }
    }
    Component.onCompleted: {
        aKeyFilter.setFilter(messageBoxBackground, 1);
//        aKeyFilter.setFilter(messageBoxBackground.parent);
//        if (messageBoxBackground.parent.parent!=0) {
//            aKeyFilter.setFilter(messageBoxBackground.parent.parent);
//        }
        var docRoot = messageBoxBackground.parent;
        while (docRoot.parent) {
            docRoot = docRoot.parent;
        }
        messageBoxBackground.parent = docRoot;
    }
}

import QtQuick 2.0
import QtQuick.Controls 2.0
import Entboost.KeyFilter 1.0

/// 上面一个标题栏
/// 中间一个靠左输入框
/// 和下面一个确定按钮
Rectangle {
    id: rootItem
    anchors.fill: parent
    z: 100

    property alias title: titleBar.title
    property alias placeholderText: inputTextField.placeholderText
    property alias text: inputTextField.text
    property alias acceptButtonLabel: acceptButton.label

    signal accepted(var text);
    signal returnBack();

    Column {
        anchors.fill: parent

        EbTitleBar {
            id: titleBar;
            width: parent.width
            onReturnBack: rootItem.returnBack()
        }

        Item {
            width: parent.width
            height: 2
        }
        TextField {
            id: inputTextField
            width: parent.width
            height: 36
            background: Rectangle {
                border.width: 0
            }
            focus: true
            leftPadding: 20
            rightPadding: 20
            horizontalAlignment: TextField.AlignLeft
            verticalAlignment: TextField.AlignVCenter
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
            id: acceptButton
            x: inputTextField.leftPadding
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            enabled: inputTextField.text.length>0?true:false
            textColor: "#fff"
            buttonColor: theApp.mainColor
            buttonHoverColor: theApp.mainHoverColor
            buttonPressedColor: theApp.mainPressedColor
            onClickedButton: {
                if (inputTextField.text.length==0) {
                    inputTextField.focus = true;
                    return;
                }
                acceptButton.enabled = false;
                rootItem.accepted(inputTextField.text);
            }
        }

    }

    /// 延时返回退出
    Timer {
        id: timerReturnBack
        interval: 100
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
    }
}

import QtQuick 2.0
import QtQuick.Controls 2.0

/// 左右结构，左边提示输入，右边单行输入框
Item {
    id: rootItem
    property alias label: labelText.text
    property alias colorLabel: labelText.color
    property alias text: inputTextField.text
    property alias focusInput: inputTextField.focus
    property alias placeholderText: inputTextField.placeholderText
    property alias selectedText: inputTextField.selectedText
    property alias readOnly: inputTextField.readOnly
    property alias validator: inputTextField.validator
    property bool passwordMode: false

    function selectAll() {inputTextField.selectAll();}

    width: parent.width
    height: 32
    Text {
        id: labelText
        x: 8
        width: parent.width*0.28-x
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        color: "#666"
    }
    TextField {
        id: inputTextField
        x: labelText.width
        width: parent.width-labelText.width
        height: parent.height
        focus: true
        background: Rectangle {
            border.width: 0
        }
        rightPadding: 10
        horizontalAlignment: TextField.AlignRight
        verticalAlignment: TextField.AlignVCenter
    }

//    onFocusChanged: {
//        if (rootItem.focus) {
//            inputTextField.focus = true;
//        }
//    }
    Component.onCompleted: {
        if (passwordMode) {
            inputTextField.echoMode = TextInput.Password;
        }
    }


}

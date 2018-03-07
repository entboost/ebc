import QtQuick 2.0
import QtQuick.Controls 2.0

TextField {
    id: rootItem
    property alias iconText: iconText.text
    property alias iconFontPointSize: iconText.font.pointSize
    property alias iconColor: iconText.color
    property bool rightClearButtonEnable: false
    property int iconWidth: 20

    leftPadding: iconWidth+8
    rightPadding: rightClearButton.visible?rootItem.height:2
    verticalAlignment: TextField.AlignVCenter

    Text {
        id: iconText
        width: iconWidth
        height: parent.height
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.family: "FontAwesome"
//        text: "\uf007"
        font.pointSize: 14
        color: "#808080"
    }

    EbButton {
        id: rightClearButton
        border.width: 0
        width: parent.height+2
        height: parent.height+2
        x: parent.width-width
        y: -2
        z: 1
        fontFamily: "FontAwesome"
        label: "\uf00d"
        textColor: "#808080"
        buttonColor: "transparent"
        borderColor: "transparent"
        fontPointSize: 12
        visible: (rootItem.rightClearButtonEnable && rootItem.text.length>0)?true:false
        onClickedButton: {
            if (rootItem.rightClearButtonEnable) {
                rootItem.clear();
                rootItem.focus = true;
            }
        }
    }
}

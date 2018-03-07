import QtQuick 2.0

///  全屏显示一个图像
/// 下面加二个按钮，一个关闭按钮，一个确定按钮
Rectangle {
    id: rootItem
    z: 10
    anchors.fill: parent
    property alias source: image.source
    property bool showOnly: false
    color: "#333"

    signal returnBack();
    signal accepted();

    function requestReturnBack() {
        return true;
    }

    EbButton {
        id: returnButton
        z: 1
        width: 42
        height: width
        border.width: 0
        fontFamily: "FontAwesome"
        label: "\uf060";
        fontPointSize: 15
        textColor: "#666"
        textPressedColor: theApp.mainColor
        buttonColor: "transparent"
        buttonPressedColor: buttonColor
        onClickedButton: { rootItem.returnBack(); }
    }
    Image {
        id: image
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
//                    anchors.horizontalCenter: parent.horizontalCenter
    }

    /// f00c=check
    /// f00d=close
    EbButton {
        id: closeButton
        z: 1
        visible: showOnly?false:true
        width: 64
        height: width
        x: width
        y: parent.height-height-30
        radius: width/2
        border.width: 1
        fontFamily: "FontAwesome"
        label: "\uf00d";
        fontPointSize: 18
        textColor: "#666"
        textPressedColor: theApp.mainColor
        buttonColor: "transparent"
        buttonPressedColor: buttonColor
        borderColor: textColor
        borderPressedColor: theApp.mainColor
        onClickedButton: { rootItem.returnBack(); }
    }
    EbButton {
        id: acceptButton
        z: 1
        visible: showOnly?false:true
        width: closeButton.width
        height: width
        x: parent.width-width*2
        y: closeButton.y
        radius: width/2
        border.width: 1
        fontFamily: "FontAwesome"
        label: "\uf00c";
        fontPointSize: 18
        textColor: theApp.mainColor
        disabledColor: "#999"
        textPressedColor: textColor
        buttonColor: "transparent"
        buttonPressedColor: buttonColor
        borderColor: textColor
        borderPressedColor: theApp.mainColor
        onClickedButton: {
            acceptButton.enabled = false;
            rootItem.accepted();
        }
    }

}

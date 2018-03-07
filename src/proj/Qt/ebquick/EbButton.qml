import QtQuick 2.0

Rectangle {
    id: rootItem
    border.width: 1
    property color buttonColor: "#fff"
    property color buttonHoverColor: buttonColor
    property color buttonPressedColor: buttonColor
    property color borderColor: buttonColor
    property color borderHoverColor: "#fff"
    property color borderPressedColor: borderHoverColor
    property color textColor: "#000"
    property color textHoverColor: textColor
    property color textPressedColor: textColor
    property color disabledColor: "#aaa"
    property alias label: buttonLabel.text
    property alias fontFamily: buttonLabel.font.family
    property alias fontPointSize: buttonLabel.font.pointSize
    property alias fontPixelSize: buttonLabel.font.pixelSize
    property alias fontBold: buttonLabel.font.bold

    signal clickedButton()

    Text {
        id: buttonLabel
        anchors.centerIn: parent
        color: textColor
    }
//    Component.onCompleted: {
//        if (fontFamily.length>0) {
//            buttonLabel.font.family = fontFamily;
//        }
//        if (fontPointSize>0) {
//            buttonLabel.font.pointSize = fontPointSize;
//        }
//        if (fontPixelSize>0) {
//            buttonLabel.font.pixelSize = fontPixelSize;
//        }
//    }

    EbTimerPress {
        id: delayPressTimer
        onPressed: { rootItem.clickedButton(); }
    }
    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            buttonLabel.color = textPressedColor;
            parent.color = buttonPressedColor;
            parent.border.color = borderPressedColor;
            delayPressTimer.initStart();
        }
        onEntered: {
            buttonLabel.color = textHoverColor;
            parent.color = buttonHoverColor;
            parent.border.color = borderHoverColor;
        }
        onReleased: {
            delayPressTimer.pressedReleased = true;
            buttonLabel.color = textColor;
            parent.color = rootItem.enabled?buttonColor:disabledColor;
            parent.border.color = rootItem.enabled?borderColor:disabledColor;
        }
        onExited: {
            buttonLabel.color = textColor;
            parent.color = rootItem.enabled?buttonColor:disabledColor;
            parent.border.color = rootItem.enabled?borderColor:disabledColor;
        }
    }
//    onEnabledChanged: {
//        parent.color = rootItem.enabled?buttonColor:disabledColor;
//        parent.border.color = rootItem.enabled?borderColor:disabledColor;
////        if (!rootItem.enabled) {
////            rootItem.color = disabledColor;
////            rootItem.border.color = disabledColor;
////        }
////        else {
////            rootItem.color = buttonColor;
////            rootItem.border.color = borderColor;
////        }
//    }

    /// 实现点击背景颜色切换
    color: rootItem.enabled?buttonColor:disabledColor;
    border.color: rootItem.enabled?borderColor:disabledColor;
//     color: buttonMouseArea.pressed ? Qt.darker(buttonColor, 1.5) : buttonColor
//     Behavior on color { ColorAnimation{ duration: 55} }

     /// 实现点击，边框放大，还原
//     scale: buttonMouseArea.pressed ? 1.1 : 1.00
//     Behavior on scale { NumberAnimation { duration: 55} }

//     /// 使用渐变 0.0 至 1.0 之间
//     gradient: Gradient {
//         GradientStop { position: 0.0; color: "#8C8F8C" }
//         GradientStop { position: 0.17; color: "#6A6D6A" }
//         GradientStop { position: 0.98;color: "#3F3F3F" }
//         GradientStop { position: 1.0; color: "#0e1B20" }
//     }
}

import QtQuick 2.0

Rectangle {
    id: rootRect
    color: theApp.mainColor
    height: 48
    z: 1
    property alias iconVisible: returnBackButton.visible
    property alias iconLabel: returnBackButton.label
    property alias title: titleText.text
    /// 位置排列：button1 -> button0
    property alias rightButton1Visible: rightButton1.visible
    property alias rightButton1Label: rightButton1.label
    property alias rightButton0Visible: rightButton0.visible
    property alias rightButton0Label: rightButton0.label
    property bool pressTitleEnable: false

    signal returnBack()
    signal pressedTitle()
    signal pressedRightButton1()
    signal pressedRightButton0()

    EbButton {
        id: returnBackButton
        width: parent.height
        height: parent.height
        color: "transparent"
        fontFamily: "FontAwesome"
        label: "\uf060"
        textColor: "#fff"
        buttonColor: color
        borderColor: color
        fontPointSize: 16
        onClickedButton: returnBack()
    }
    Text {
        id: titleText
        x: returnBackButton.visible?(returnBackButton.width+2):8
        width: parent.width-x-height*2-2;
//        width: 120
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 17
        color: "#fff"

        EbTimerPress {
            id: delayPressTimer
            onPressed: { rootRect.pressedTitle(); }
        }
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onPressed: {
                if (pressTitleEnable) {
                    rootRect.color = theApp.mainPressedColor;
                    delayPressTimer.initStart();
                }
            }
            onReleased: {
                rootRect.color = theApp.mainColor;
                delayPressTimer.pressedReleased = true;
            }
            onExited: {
                rootRect.color = theApp.mainColor;
            }

        }
    }
    EbButton {
        id: rightButton1
        x: rightButton0.visible?(parent.width-width*2):(parent.width-width)
        width: parent.height
        height: parent.height
        color: "transparent"
        fontFamily: "FontAwesome"
//        label: "\uf060"
        textColor: "#fff"
        buttonColor: theApp.mainColor
        buttonPressedColor: theApp.mainPressedColor
        border.width: 0
//        borderColor: color
        fontPointSize: 16
        visible: false
        onClickedButton: pressedRightButton1()
    }
    EbButton {
        id: rightButton0
        x: parent.width-width
        width: parent.height
        height: parent.height
        color: "transparent"
        fontFamily: "FontAwesome"
//        label: "\uf060"
        textColor: "#fff"
        buttonColor: theApp.mainColor
        buttonPressedColor: theApp.mainPressedColor
        border.width: 0
//        borderColor: color
        fontPointSize: 16
        visible: false
        onClickedButton: pressedRightButton0()
    }

}

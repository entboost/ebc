import QtQuick 2.0

/// 左右结构，左边提示，右边单行内容
Rectangle {
    id: rootItem
    property alias label: labelText.text
    property alias colorLabel: labelText.color
    property alias text: contentText.text
    property alias visibleLeftIcon: leftIconText.visible
    property alias leftIconText: leftIconText.text
    property alias visibleRightIcon: rightIconText.visible
    property bool pressEnable: false

    signal pressed();

    width: parent.width
    height: 32
    Text {
        id: leftIconText
        height: parent.height
        width: visible?height:0
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "FontAwesome"
        text: "\uf105"
        color: "#999"
        font.pointSize: 14
        visible: false
    }
    Text {
        id: labelText
        x: leftIconText.visible?(leftIconText.width+4):8
        width: parent.width*0.28-x
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        color: "#666"
    }

    EbTimerPress {
        id: delayPressTimer
        onPressed: { rootItem.pressed(); }
    }
    Text {
        id: contentText
        x: labelText.width
        width: rightIconText.visible?(parent.width-x-2-rightIconText.width):(parent.width-x-10)
        height: parent.height
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
    }
    Text {
        id: rightIconText
        height: parent.height
        width: 24
        x: parent.width-width
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "FontAwesome"
        text: "\uf105"
        color: "#999"
        font.pointSize: 14
        visible: false
    }

    MouseArea {
        anchors.fill: parent
//        hoverEnabled: true
        onPressed: {
            if (pressEnable) {
                rootItem.color = "#ddd";
                delayPressTimer.initStart();
            }
        }
        onReleased: {
            delayPressTimer.pressedReleased = true;
            if (pressEnable) {
                rootItem.color = "#fff";
            }
        }
        onExited: {
            if (pressEnable) {
                rootItem.color = "#fff";
            }
        }
    }


}

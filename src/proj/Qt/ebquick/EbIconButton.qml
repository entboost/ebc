import QtQuick 2.0

Item {
    id: rootItem
    height: parent.height
    property alias label: labelText.text
    property alias icon: iconText.text

//    signal clicked();
    signal pressed();

    function checkedStateChanged(checked) {
        iconText.color = checked?theApp.mainColor:"#999";
        labelText.color = checked?theApp.mainColor:"#999";
    }

    Text {
        id: iconText
        width: parent.width
        height: parent.height*0.52
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignBottom
        font.family: "FontAwesome"
        color: "#999"
        font.pointSize: 15
    }
    Text {
        x: 2
        id: labelText
        y: iconText.height
        width: parent.width-x
        height: parent.height-y
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignTop
        color: iconText.color
        font.pointSize: 12
    }
    EbTimerPress {
        id: delayPressTimer
        onPressed: { rootItem.pressed(); }
    }
    MouseArea {
        anchors.fill: parent
        onPressed: { delayPressTimer.initStart(); }
        onReleased: { delayPressTimer.pressedReleased = true; }
    }
}

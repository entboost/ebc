import QtQuick 2.0
import QtQuick.Controls 2.2

MenuItem {
    id: rootItem
    property int normalHeight: 36
    height: normalHeight

    property alias label: labelText.text

    Rectangle {
        anchors.fill: parent
        color: theApp.mainPressedColor
        Text {
            id: labelText
            x: 12
            width: parent.width-x;
            height: parent.height
//            anchors.fill: parent
            color: "#fff"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        EbTimerPress {
            id: delayPressTimer
            onPressed: { rootItem.triggered(); }
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                parent.color = theApp.mainHoverColor;
                delayPressTimer.initStart();
            }
            onReleased: { delayPressTimer.pressedReleased = true; }
            onExited: { parent.color = theApp.mainPressedColor; }
        }
    }
    onVisibleChanged: {
        rootItem.height = visible?normalHeight:0;
    }

}

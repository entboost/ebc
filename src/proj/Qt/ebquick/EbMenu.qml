import QtQuick 2.0
import QtQuick.Controls 2.2

Menu {
    id: myGroupMenu

    background: Rectangle {
        implicitWidth: 160
        implicitHeight: 36+2
        color: theApp.mainPressedColor
        border.width: 0
        border.color: "#ccc"
    }

}

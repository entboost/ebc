import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

RadioButton {
    id: rootItem;
    width: 12
    height: parent.height
    property string label: ""

    style: RadioButtonStyle {
        indicator: Rectangle {
            implicitWidth: 16
            implicitHeight: 16
            radius: 9
            border.color: control.activeFocus ? "darkblue" : "gray"
            border.width: 1
            Rectangle {
                anchors.fill: parent
                visible: control.checked
                color: "#555"
                radius: 9
                anchors.margins: 4
            }

            Label {
                id: labelText
                z: 1
                x: implicitWidth+8
                y: -3
                height: implicitHeight+4
                horizontalAlignment: Label.AlignLeft
                verticalAlignment: Label.AlignVCenter
                text: rootItem.label
                MouseArea {
                    anchors.fill: parent
                    onPressed: { rootItem.checked = true; }
                }
            }
        }

    }
}

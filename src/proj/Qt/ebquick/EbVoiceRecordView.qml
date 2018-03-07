import QtQuick 2.0

Item {
    id: rootItem
    anchors.fill: parent

//    signal returnBack();
//    signal accepted();

    Rectangle {
        id: mainRect
        width: 280
        height: width
        x: (parent.width-width)/2
        y: (parent.height-height)/2
        radius: 8
        color: "#999"

        Text {
            id: recorderIconText
            width: 200
            height: width
            x: (parent.width-width)/2
            y: (parent.height-height)/2
            color: "#fff"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: "FontAwesome"
            text: "\uf130"
            font.pointSize: 40
        }
        Text {
            id: recorderTipText
            y: recorderIconText.y + recorderIconText.height + 10
            width: parent.width
            height: 22
            color: "#fff"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: theLocales.getLocalText("message-show.finger-up-cancel", "Finger up, cancel send")
//            text: "手指上滑 取消发送"
        }
    }

}

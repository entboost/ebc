import QtQuick 2.0

/// 用于显示左边头像，右边上下结构，上边显示一个名称，下面显示一段文字或其他
Rectangle {
    id: rootItem
    width: parent.width
    height: 60
    property alias source: imageIcon.source
    property alias icon: iconText.text
    property alias name: nameText.text
    property alias info: infoText.text
    property bool textIconMode: false
    property bool pressEnable: false

    signal pressed();

    EbImage {
        id: imageIcon
        width: parent.height-8
        height: width
        y: (parent.height-height)/2
        x: y
        visible: !iconText.visible
        fillMode: Image.PreserveAspectFit
//        source: "qrc:/res/entlogo.png"
    }
    Text {
        id: iconText
        anchors.fill: imageIcon
        visible: textIconMode
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "FontAwesome"
        color: "#999"
        font.pointSize: 16
    }

    Column {
        x: imageIcon.x + imageIcon.width + 8
        width: parent.width-x
        height: parent.height
        Text {
            id: nameText
            height: rootItem.info.length==0?(parent.height):(parent.height/2)
            verticalAlignment: rootItem.info.length==0?Text.AlignVCenter:Text.AlignBottom
//            height: parent.height*0.5
//            verticalAlignment: Text.AlignBottom
            lineHeight: rootItem.info.length==0?1.1:1.0
        }
        Text {
            id: infoText
            y: parent.height/2
            height: rootItem.info.length==0?0:(parent.height-y)
            visible: rootItem.info.length==0?false:true
            verticalAlignment: Text.AlignTop
//            y: nameText.height
//            height: parent.height-y
//            verticalAlignment: Text.AlignTop
            color: "#999"
            font.pointSize: 12
        }
    }

    EbTimerPress {
        id: delayPressTimer
        onPressed: { rootItem.pressed(); }
    }
    MouseArea {
        anchors.fill: parent
        onPressed: {
            if (pressEnable) {
                rootItem.color = "#ddd";
                delayPressTimer.initStart();
            }
        }
//        onCanceled: {
//            if (pressEnable) {
//                rootItem.color = "#fff";
//            }
//        }
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

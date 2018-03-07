import QtQuick 2.0
import QtQuick.Controls 2.2

Page {
    property alias footerLineHeight: footerLine.height

//    header: EbTitleBar {
//        id: titleBar;
//        width: parent.width
//        iconVisible: false
//        title: "聊天1(8)"
////        onReturnBack: {
////            if (requestHideWindow()) {
////                titleBar.iconVisible = false;
////                titleBar.title = "聊天(8)";
////            }
////        }
//    }


    footer: Rectangle {
        id: footerLine
        width: parent.width
        height: 1
        y: parent.height-height
        color: "#ddd"
    }
}

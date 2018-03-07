import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

/// 左右结构，左边提示“性别”，右边显示二个单选组件
Rectangle {
    id: rootItem
    property alias label: labelText.text
    property alias colorLabel: labelText.color
//    property alias visibleRightIcon: rightIconText.visible
//    property bool pressEnable: false
    property int gender: 0
    property string genderText: ""

//    signal pressed();

    width: parent.width
    height: 32
    Text {
        id: labelText
        x: 8
        width: parent.width*0.28-x
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        color: "#666"
    }

    ExclusiveGroup {
        id: gender;
    }

    EbRadioButton {
        id: maleRadioButton;
        x: femaleRadioButton.x - 60
        width: 12
        height: parent.height
        label: theLocales.getLocalText("gender.1.short", "M");
        exclusiveGroup: gender;
        checked:  rootItem.gender==1?true:false;
        onClicked: {
            rootItem.gender = 1;
        }
        onCheckedChanged: {
            if (checked) {
                rootItem.genderText = label;
            }
        }
    }
    EbRadioButton {
        id: femaleRadioButton;
        x: parent.width-60-10
        width: 12
        height: parent.height
        label: theLocales.getLocalText("gender.2.short", "F");
        exclusiveGroup: gender;
        checked:  rootItem.gender==2?true:false;
        onClicked: {
            rootItem.gender = 2;
//            rootItem.genderText = text;
        }
        onCheckedChanged: {
            if (checked) {
                rootItem.genderText = label;
            }
        }
    }

//    Text {
//        id: rightIconText
//        height: parent.height
//        width: 24
//        x: parent.width-width
//        horizontalAlignment: Text.AlignHCenter
//        verticalAlignment: Text.AlignVCenter
//        font.family: "FontAwesome"
//        text: "\uf105"
//        color: "#999"
//        font.pointSize: 14
//        visible: false
//    }

//    MouseArea {
//        anchors.fill: parent
////        hoverEnabled: true
//        onPressed: {
//            if (pressEnable) {
//                rootItem.color = "#ddd";
//                /// 使用延时返回，显示效果好一些
//                timerSendPressed.start();
//            }
//        }
//        onExited: {
//            if (pressEnable) {
//                rootItem.color = "#fff";
//            }
//        }
//    }
}

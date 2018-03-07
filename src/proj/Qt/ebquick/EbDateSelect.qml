import QtQuick 2.0
import QtQml 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

/// 左右结构，左边提示 “label”，右边显示一个日期单选
Rectangle {
    id: rootItem
    z: 100
    property alias label: labelText.text
    property alias colorLabel: labelText.color
    property alias dateString: dateText.text
//    property alias visibleRightIcon: rightIconText.visible
//    property bool pressEnable: false
    property string dateFormat: "yyyy-MM-dd"

//    signal pressed();

    property int birthday: 0
    function setBirthday(birthday) {
        rootItem.birthday = birthday;
        if (rootItem.birthday>0) {
            /// format: yyyyMMdd
            var birthdayYear = (rootItem.birthday/10000);
            var birthdayMonth = (rootItem.birthday%10000)/100;
            var birthdayDay = rootItem.birthday%100;
//            console.log("************* setBirthday, birthdayDay=" + birthdayDay);
//            console.log("************* setBirthday, dateFormat=" + dateFormat);
            var birthdayDate = new Date(birthdayYear, birthdayMonth-1);
            /// ** 必须使用 setDate ，避免日期显示错误问题；
            birthdayDate.setDate(birthdayDay);
            calendar.selectedDate = birthdayDate;
            dateText.text = Qt.formatDate(birthdayDate, rootItem.dateFormat);
        }
    }

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
    Item {
        x: labelText.width;
        width: parent.width-x
        height: parent.height
        Text {
            id: dateText
            width: parent.width-dateButton.width
            height: parent.height
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }
        EbButton {
            id: dateButton
            x: parent.width-width
            width: parent.height
            height: parent.height
            color: "transparent"
            fontFamily: "FontAwesome"
            label: calendar.visible?"\uf106":"\uf107"
            textColor: "#666"
            onClickedButton: {
                calendar.visible = !calendar.visible;
                var visible = calendar.visible;
                dateButton.label = visible?"\uf106":"\uf107";
            }
        }
    }
    Calendar {
        id: calendar
        y: parent.height
        z: 1
        width: parent.width
        height: 320
//        height: parent.parent.height-y;
        visible: false
        selectedDate: "2000-01-02"
        activeFocusOnTab: true
        onReleased: {
//            console.log("*************** onReleased:" + date);
            dateText.text = Qt.formatDate(date, dateFormat);
            rootItem.birthday = Qt.formatDate(date, "yyyyMMdd");
//            console.log("*************** birthday=" + rootItem.birthday);
        }

        style: CalendarStyle {
            gridVisible: false
            dayDelegate: Rectangle {
                gradient: Gradient {
                    GradientStop {
                        position: 0.00
                        color: styleData.selected ? "#111" : (styleData.visibleMonth && styleData.valid ? "#444" : "#666");
                    }
                    GradientStop {
                        position: 1.00
                        color: styleData.selected ? "#444" : (styleData.visibleMonth && styleData.valid ? "#111" : "#666");
                    }
                    GradientStop {
                        position: 1.00
                        color: styleData.selected ? "#777" : (styleData.visibleMonth && styleData.valid ? "#111" : "#666");
                    }
                }

                Label {
                    text: styleData.date.getDate()
                    anchors.centerIn: parent
                    color: styleData.valid ? "white" : "grey"
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#555"
                    anchors.bottom: parent.bottom
                }

                Rectangle {
                    width: 1
                    height: parent.height
                    color: "#555"
                    anchors.right: parent.right
                }
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

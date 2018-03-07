import QtQuick 2.0

Item {
    id: rootItem;
    anchors.fill: parent

    property int birthday: 0
    property alias yearUnitStr: yearList.strUnitString;
    property alias monthUnitStr: monthList.strUnitString;
    property alias dayUnitStr: dayList.strUnitString;

    /// SIGNAL
    signal returnBack();
    signal accepted();

    function requestReturnBack() {
        return true;
    }

//    function setBirthday(birthday) {
//        rootItem.birthday = birthday;
//        if (rootItem.birthday>0) {
//            /// format: yyyyMMdd
//            yearList.number = (rootItem.birthday/10000);
//            monthList.number = (rootItem.birthday%10000)/100;
//            dayList.number = rootItem.birthday%100;
//        }
//    }

    /// 上方透明
    Rectangle {
        id: topRect;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            bottom: mainRect.top;
        }
        opacity: 0.6;
        color:"#989898";

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                rootItem.returnBack();
//                rootItem.visible = false;
            }
        }
    }

    Rectangle {
        id: mainRect;
        color: "#FEFEFE";
        anchors.centerIn: parent;
        width: parent.width-20
        height: {
            if (parent.height>240)
                return 240;
            else
                return parent.height-40
        }
        Column {
            anchors.fill: parent

            Row {
                width: parent.width
                height: parent.height - buttonRow.height-1
//                height: parent.height/2

                /// 年
                EbWheelBase {
                    id: yearList;
                    width: parent.width/3
                    height: parent.height
//                    onValueChanged: {
//                        rootItem.sigYearValue(value);
//                    }
                    onCurrentIndexChanged: {
                        rootItem.birthday = yearList.number*10000+monthList.number*100+dayList.number;
                        var days = getDaysInOneMonth(yearList.number, monthList.number);
                        dayList.setFromToNumber(1, days);
                    }
                }

                /// 月
                EbWheelBase {
                    id: monthList;
                    width: yearList.width
                    height: parent.height
                    onCurrentIndexChanged: {
                        rootItem.birthday = yearList.number*10000+monthList.number*100+dayList.number;
                        var days = getDaysInOneMonth(yearList.number, monthList.number);
//                        console.log("**************** year=" + yearList.number +
//                                    ",month=" + monthList.number +
//                                    ",day=" + dayList.number +
//                                    ",montn-days=" + days);
                        dayList.setFromToNumber(1, days);
                    }

                }

                /// 日
                EbWheelBase {
                    id: dayList;
                    width: yearList.width
                    height: parent.height
                    onCurrentIndexChanged: {
                        rootItem.birthday = yearList.number*10000+monthList.number*100+dayList.number;
                    }
                }
            }

            Rectangle {
                /// line
                width: parent.width
                height: 1
                color: "#eee"
            }
            Row {
                id: buttonRow
                width: parent.width-2
                height: 0.2 * parent.height-3

                EbButton {
                    id: cancelButton
                    width: (parent.width-1)/2
                    height: parent.height
                    label: theLocales.getLocalText("base-dialog.cancel-button.text", "Cancel");
                    textColor: "#222"
                    buttonPressedColor: "#eee"
                    onClickedButton: { rootItem.returnBack(); }
                }
                Rectangle {
                    /// line
                    width: 1
                    height: parent.height
                    color: "#eee"
                }
                EbButton {
                    id: okButton
                    width: cancelButton.width
                    height: parent.height
                    label: theLocales.getLocalText("base-dialog.ok-button.text", "Ok");
                    textColor: theApp.mainColor
                    buttonPressedColor: "#eee"
                    onClickedButton: { rootItem.accepted(); }
                }
            }
        }
    }

    /// 左边透明
    Rectangle {
        id :leftRect;
        anchors {
            left: parent.left;
            right: mainRect.left;
            top: topRect.bottom;
            bottom: bottomRect.top;
        }
        color:"#989898";
        opacity: 0.6;

        MouseArea{
            anchors.fill: parent;
            onClicked: {
                rootItem.returnBack();
            }
        }
    }
    function getDaysInOneMonth(year, month){
        month = parseInt(month, 10);
        var d = new Date(year, month, 0);
        return d.getDate();
    }

    /// 右边透明
    Rectangle {
        id :rightRect;
        anchors {
            left: mainRect.right;
            right: parent.right;
            top: topRect.bottom;
            bottom: bottomRect.top;
        }
        color:"#989898";
        opacity: 0.6;
        MouseArea{
            anchors.fill: parent;
            onClicked: {
                rootItem.returnBack();
            }
        }
    }

    /// 下方透明
    Rectangle {
        id: bottomRect;
        anchors {
            left: parent.left;
            top: mainRect.bottom;
            bottom: parent.bottom;
        }
        width: parent.width;
        opacity: 0.6;
        color:"#989898";

        MouseArea{
            anchors.fill: parent;
            onClicked: {
                rootItem.returnBack();
            }
        }
    }

    function updateLocaleInfo() {
        yearList.strUnitString = theLocales.getLocalText("name-text.year", "Year");
        monthList.strUnitString = theLocales.getLocalText("name-text.month", "Month");
        dayList.strUnitString = theLocales.getLocalText("name-text.date", "Date");
    }

    Component.onCompleted: {
        updateLocaleInfo();
        /// for test
//        setBirthday(19781125);
        if (rootItem.birthday>0) {
            /// format: yyyyMMdd
            yearList.number = (rootItem.birthday/10000);
            monthList.number = (rootItem.birthday%10000)/100;
            dayList.number = rootItem.birthday%100;
        }
        else {
            yearList.number = 1990;
        }

        yearList.setFromToNumber(1949, 2058);
        monthList.setFromToNumber(1, 12);
        dayList.setFromToNumber(1, 31);
    }
}

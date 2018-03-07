import QtQuick 2.0

Item {
    id: rootItem;
    anchors.fill: parent
    property alias area1Code: area1List.number
    property alias area1String: area1List.text
    property alias area2Code: area2List.number
    property alias area2String: area2List.text
    property alias area3Code: area3List.number
    property alias area3String: area3List.text
    property alias area4Code: area4List.number
    property alias area4String: area4List.text

    /// SIGNAL
    signal returnBack();
    signal accepted();

    function requestReturnBack() {
        return true;
    }

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

                /// 地区1
                EbWheelBase {
                    id: area1List;
                    width: parent.width/4
                    height: parent.height
                    textMode: true
                    onCurrentIndexChanged: {
//                        console.log("*********** area1code=", + area1Code +
//                                    ",area1Name=" + area1List.text);
                        area2List.clear();
                        if (area1Code>0) {
                            theApp.loadAreaInfo(area1Code, 2);
                        }
                        if (area2List.count==0) {
                            area3List.clear();
                            area4List.clear();
                        }
                    }
                }

                /// 地区2
                EbWheelBase {
                    id: area2List;
                    width: area1List.width
                    height: parent.height
                    textMode: true
                    onCurrentIndexChanged: {
                        area3List.clear();
                        if (area2Code>0) {
                            theApp.loadAreaInfo(area2Code, 3);
                        }
                        if (area3List.count==0) {
                            area4List.clear();
                        }
                    }

                }

                /// 地区3
                EbWheelBase {
                    id: area3List;
                    width: area1List.width
                    height: parent.height
                    textMode: true
                    onCurrentIndexChanged: {
                        area4List.clear();
                        if (area3Code>0) {
                            theApp.loadAreaInfo(area3Code, 4);
                        }
                    }

                }

                /// 地区4
                EbWheelBase {
                    id: area4List;
                    width: area1List.width
                    height: parent.height
                    textMode: true
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
                    onClickedButton: {
                        area1List.buildNumber();
                        area2List.buildNumber();
                        area3List.buildNumber();
                        area4List.buildNumber();
                        rootItem.accepted();
                    }
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
    Connections {
        target: theApp
        onLoadAreaInfoResponse: {
            if (parameter==1) {
                area1List.addNumberText(areaInfo.areaCode, areaInfo.areaName);
            }
            else if (parameter==2) {
                area2List.addNumberText(areaInfo.areaCode, areaInfo.areaName);
            }
            else if (parameter==3) {
                area3List.addNumberText(areaInfo.areaCode, areaInfo.areaName);
            }
            else if (parameter==4) {
                area4List.addNumberText(areaInfo.areaCode, areaInfo.areaName);
            }
        }
    }
    property bool inited: false
    Component.onCompleted: {
        if (area1Code==0) {
            /// 156 默认 中国
            area1Code = 156;
        }
        theApp.loadAreaInfo(0, 1);
        rootItem.inited = true;
    }
}

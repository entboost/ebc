import QtQuick 2.0

Rectangle {
    id: rootItem
    property int number: 0
    property string text: ""
    property alias value: list.currentIndex;
    property alias count: theModel.count;
    property string strUnitString: "";
    property bool textMode: false

    signal currentIndexChanged(var index);
    function setFromToNumber(from, to) {
        theModel.clear();
        var findCurrent = false;
        for (var i=from; i<=to; i++) {
            theModel.append( {number: i, textString: "" } )
            if (number===i) {
                findCurrent = true;
                rootItem.findedNumber = true;
                list.currentIndex = theModel.count-1;
            }
        }
        if (!findCurrent) {
            list.currentIndex = 0;
        }
    }
    function addNumberText(number, text) {
        theModel.append( {number: number, textString: text } );
        if (rootItem.number==number) {
            rootItem.findedNumber = true;
            list.currentIndex = theModel.count-1;
        }
    }

    property bool findedNumber: false
    function clear() {
        findedNumber = false;
        theModel.clear();
        list.currentIndex = 0;
//        if (list.currentIndex!=-1) {
//            list.currentIndex = -1;
//            rootItem.currentIndexChanged(list.currentIndex);
//        }
    }
    function buildNumber() {
        /// 主要处理，最后列，自动显示，但没有选择，不会更新 number 数据
        if (!rootItem.findedNumber) {
            var obj = theModel.get(list.currentIndex);
            if (obj==null) {
                rootItem.number = 0;
                rootItem.text = "";
            }
            else {
                rootItem.number = obj.number;
                rootItem.text = obj.textString;
            }
        }
    }

    ListModel {
        id: theModel
    }

    border.width: 0
    Rectangle{
        id: wheelList;
        anchors.fill: parent
        border.width: 0

        ListView {
            id: list;
            width: parent.width * 0.9;
            height: parent.height;
            model: theModel
            highlightRangeMode: ListView.StrictlyEnforceRange;
            preferredHighlightBegin: height / 3;
            preferredHighlightEnd: height / 3;
            clip: true;
            delegate: Rectangle {
                id: modelRect;
//                color: "#FEFEFE";
                width: parent.width;
                height: ListView.isCurrentItem?38:32
                Text {
                    id: modelText;
                    anchors.centerIn: parent;
                    color: modelRect.ListView.isCurrentItem ? theApp.mainColor : "#ddd";
                    font.pointSize: modelRect.ListView.isCurrentItem ? 15 : 13;
                    text: {
                        if (rootItem.textMode)
                            return textString;
                        else
                            return number + strUnitString;
                    }
                }
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        list.currentIndex = index;
                    }
                }
            }
            onCurrentIndexChanged: {
                var obj = theModel.get(list.currentIndex);
                if (obj==null) {
                    return;
                }
                rootItem.findedNumber = true;
                rootItem.number = obj.number;
                rootItem.text = obj.textString;
                rootItem.currentIndexChanged(list.currentIndex);
            }
        }
    }
}

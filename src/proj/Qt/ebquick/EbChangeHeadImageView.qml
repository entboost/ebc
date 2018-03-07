import QtQuick 2.0
import QtQuick.Controls 2.2
//import Entboost.EmotionInfo 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent

    signal returnBack();
    signal accepted();

    function requestReturnBack() {
        if (theViewLoadItem.visible) {
            if (!theViewLoader.item.requestReturnBack()) {
                return false;
            }
            theViewLoadItem.visible = false;
            theViewLoader.source = ""
            return false;
        }
        return true;
    }

    property bool waittingGroupEditResponse: false
    property var accountInfo: null
    property var defaultMemberInfo: null
    function setAccountInfo(accountInfo) {
        theModel.clear();
        tipText.text = "";
        rootItem.accountInfo = accountInfo;
        rootItem.defaultMemberInfo = theApp.defaultMemberInfo(accountInfo.userId.toString());
        theApp.loadHeadEmotionInfo(0);
        var lineCount = (theGridView.count+4)/5;
        if (lineCount>5) {
            lineCount = 5;
        }
        theGridView.height = lineCount*theGridView.cellHeight+10;
    }

    Item {
        id: theViewLoadItem
        anchors.fill: parent
        visible: false
        z: 101
        Loader {
            id: theViewLoader
            anchors.fill: parent
        }
        /// 1=相册图片/自拍头像
        property int viewType: 0
        Connections {
            target: theViewLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onAccepted: {
                if (theViewLoadItem.viewType==1) {
                    rootItem.waittingGroupEditResponse = true;
                    var imageFile = theViewLoader.item.source;
                    theApp.setMyGroupHeadImageFile(
                                rootItem.defaultMemberInfo.groupId.toString(),
                                imageFile);
                }
                theViewLoadItem.viewType = 0;
                requestReturnBack();
            }
        }
    }

    EbTitleBar {
        id: titleBar;
        width: parent.width
//        title: "选择头像"
        onReturnBack: {
            if (requestReturnBack()) {
                rootItem.returnBack();
            }
        }
    }

    function setEmotionInfo(emotionInfo) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.emotionInfo==null) {
                break;
            }
            if (obj.emotionInfo.resourceId==emotionInfo.resourceId) {
                theModel.set(i, { emotionInfo: emotionInfo });
                return;
            }
        }
        theModel.append( { emotionInfo: emotionInfo } );
    }

    Connections {
        target: theApp
        onOpenGalleryResponse: {
            theViewLoader.setSource("EbHeadImageView.qml",
                                    {"source": uriFile} );
            theViewLoadItem.viewType = 1;
            theViewLoadItem.visible = true;
        }

        onLoadHeadEmotionInfoResponse: {
            setEmotionInfo(emotionInfo);
        }
        onMyHeadImageChanged: {
            if (rootItem.waittingGroupEditResponse &&
                    rootItem.accountInfo.userId==theApp.logonUserId) {
                /// 设置头像成功，退出所有窗口
                while (!requestReturnBack()) {
                }
                rootItem.returnBack();
            }
        }

        onSetAccountInfoResponse: {
            if (!waittingGroupEditResponse) {
                return;
            }
            waittingGroupEditResponse = false;
            if (stateCode==0) {
                /// 成功不会跑这里，成功跑 onMyHeadImageChanged
            }
            else {
                tipText.text = stateString;
            }
        }
    }

    ListModel {
        id: theModel
//        ListElement {
//            image: "assets:/res/imgdefaultgroup.png"
//            name: "11111"
//        }
    }

    Component {
        id: theDelegate
        Rectangle {
            id: listItem
            width: theGridView.cellWidth;
            height: theGridView.cellHeight
            color: "transparent"
            Column {
                anchors.fill: parent
                EbImage {
                    id: image
                    width: parent.width
                    height: width
                    source: emotionInfo.resourceImageFile
                    fillMode: Image.PreserveAspectFit
//                    anchors.horizontalCenter: parent.horizontalCenter
                }
//                Text {
//                    font.family: addType==1?"FontAwesome":""
//                    text: addType==1?"\uf067":memberInfo.userName;
//                    anchors.horizontalCenter: parent.horizontalCenter
//                    color: addType==1?"#666":(memberInfo.subType>0?"red":"#000")
//                    height: addType==1?(parent.height-2):(parent.height-image.height)
//                    verticalAlignment: Text.AlignVCenter
//                    font.pointSize: addType==1?18:13
//                }
            }

            EbTimerPress {
                id: delayPressTimer
                onPressed: {
                    /// 设置头像
                    rootItem.waittingGroupEditResponse = true;
                    theApp.setMyGroupHeadImageResourceId(
                                rootItem.defaultMemberInfo.groupId.toString(),
                                emotionInfo.resourceId.toString());
                }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onPressed: {
                    parent.color = "#eee";
                    delayPressTimer.initStart()();
                }
                onReleased: {
                    parent.color = "transparent";
                    delayPressTimer.pressedReleased = true;
                }
                onExited: {
                    parent.color = "transparent";
                }
            }
        }
    }

    Column {
        y: titleBar.height
        width: parent.width
        height: parent.height-y

        GridView {
            id: theGridView
            width: parent.width
            height: 80
//            height: parent.height-y
            //        anchors.fill: parent
            leftMargin: 10
            topMargin: 10
            rightMargin: 10
            bottomMargin: 10
            cellWidth: 70;
            cellHeight: 70
            model: theModel
            delegate: theDelegate
            //        highlight: Rectangle {
            //            color: "lightsteelblue";
            //            radius: 5
            //        }
            focus: true
        }
        Rectangle {
            width: parent.width
            height: 10
            color: "#eee"
        }

        Item {
            width: parent.width
            height: 10
        }

        EbButton {
            /// 自拍头像
            id: openVideoButton
            x: 20
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            textColor: "#fff"
            buttonColor: theApp.mainColor
            buttonHoverColor: theApp.mainHoverColor
            buttonPressedColor: theApp.mainPressedColor
            onClickedButton: {
                theViewLoader.source = "EbMyVideoHead.qml";
                theViewLoadItem.viewType = 1;
                theViewLoadItem.visible = true;
            }
        }
        Item {
            width: parent.width
            height: 10
        }
        EbButton {
            /// 本地相册
            id: openFileButton
            x: openVideoButton.x
            width: openVideoButton.width
            height: openVideoButton.height
            radius: openVideoButton.radius
            border.width: 0
            textColor: "#fff"
            buttonColor: theApp.mainColor
            buttonHoverColor: theApp.mainHoverColor
            buttonPressedColor: theApp.mainPressedColor
            onClickedButton: {
                theApp.openGallery(0);
            }
        }
        Item {
            width: parent.width
            height: 10
        }
        Text {
            id: tipText
            x: openVideoButton.x
            width: openVideoButton.width
            height: 68
            wrapMode: Text.Wrap
            lineHeight: 1.2
            color: "red"
        }

    }

    function updateLocaleInfo() {
        titleBar.title = theLocales.getLocalText("change-head-dialog.title","Change head");
        openVideoButton.label = theLocales.getLocalText("change-head-dialog.button-open-video.text","Open video");
        openFileButton.label = theLocales.getLocalText("change-head-dialog.button-open-file.text","Open file");
    }

    Component.onCompleted: {
        updateLocaleInfo();
    }

}

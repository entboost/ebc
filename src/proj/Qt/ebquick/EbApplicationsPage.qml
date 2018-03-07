import QtQuick 2.0
//import QtWebEngine 1.5
//import QtWebKit 3.0
import "js/ebbase.js" as Ebbasejs

EbPage {
    id: rootPage

    signal requestAddContact();
    signal pressedFuncInfo(var funcInfo);
    signal requestSearch();

    function loadApplicationData() {
        theModel.clear();
        rootPage.loadEmptyCont = 0;
        loadFuncInfoTimer.stop();
        loadFuncInfoTimer.start();
    }

    EbMenu {
        id: theTitleMenu
        EbMenuItem {
            id: addContactMenuItem
//            label: "添加联系人"
            onTriggered: {
                rootPage.requestAddContact();
            }
        }
    }
    header: Item {
        id: headerItem;
        height: titleBar.height
        EbTitleBar {
            id: titleBar;
            width: parent.width
            iconVisible: false
//            title: "集成应用"
            rightButton1Visible: true
            rightButton1Label: "\uf002"
            onPressedRightButton1: { requestSearch(); }
            rightButton0Visible: true
            rightButton0Label: "\uf067"
            onPressedRightButton0: {
                theTitleMenu.x = parent.width-theTitleMenu.width-10;
                theTitleMenu.y = titleBar.height;
                theTitleMenu.open();
                theTitleMenu.parent = rootPage;
            }
        }
    }

    ListModel {
        id: theModel
//        ListElement { name: "同学"; file: "zh-CN.json" }
    }
    Component {
        id: theDelegate

        Rectangle {
            id: listItem
            color: "#fff"
            x: 1
            width: parent.width-x
            height: 42

//            EbImageNameInfo {
//                id: theImageNameInfo
//                width: parent.width
//                height: parent.height-1
//                pressEnable: true
//                textIconMode: funcInfo.iconImageFile.length>0?false:true
//                icon: Ebbasejs.funcIconText(funcInfo)
//                source: funcInfo.iconImageFile
//                name: funcInfo.functionName
//                info: "ID: " + funcInfo.subscribeId
//                onPressed: { rootPage.pressedFuncInfo(funcInfo); }
//            }

            Text {
                id: iconText
                width: height
                height: parent.height-1
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.family: "FontAwesome"
                text: Ebbasejs.funcIconText(funcInfo)
                color: "#999"
                font.pointSize: 16
                visible: funcInfo.iconImageFile.length>0?false:true
            }
            EbImage {
                id: imageIcon
                width: parent.height
                height: parent.height-1
                fillMode: Image.PreserveAspectFit
                source: funcInfo.iconImageFile
                visible: funcInfo.iconImageFile.length>0?true:false
            }
            Text {
                x: imageIcon.width+4
                width: parent.width-x
                height: parent.height-1
                verticalAlignment: Text.AlignVCenter
                text: funcInfo.functionName
            }

            Rectangle {
                /// line
                x: 0
                y: parent.height-1
                width: parent.width
                height: 1
                color: "#eee"
            }

            EbTimerPress {
                id: delayPressTimer
                onPressed: { rootPage.pressedFuncInfo(funcInfo); }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
//                width: parent.width-rightIconText.width
//                height: parent.height
                onPressed: {
                    listItem.color = "#eee";
                    delayPressTimer.initStart();
                }
                onReleased: {
                    listItem.color = "#fff";
                    delayPressTimer.pressedReleased = true;
                }
                onExited: {
                    listItem.color = "#fff";
                }

            }
        }

    }
    ListView {
        id: listView
        anchors.fill: parent
        model: theModel
        delegate: theDelegate
    }

    function setFuncInfoModelData(funcInfo) {
        if (funcInfo.subscribeId==1002300103 ||
                funcInfo.subscribeId==1002300105 ||
                funcInfo.subscribeId==1002300106) {
            /// 1002300103=我的消息，在“个人中心”显示
            /// 1002300105=群共享，不在这里显示
            /// 1002300106=个人收藏，在“个人中心”显示
            return;
        }

        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.funcInfo==null) {
                return;
            }

            if (obj.funcInfo.subscribeId==funcInfo.subscribeId) {
                /// 找到现存部门，直接更新（后期再显示重新排序问题）
                theModel.set(i, { funcInfo: funcInfo } );
                return;
            }
        }
        rootPage.loadEmptyCont = 0;
        /// 前面找不到，直接添加
        theModel.append( { funcInfo: funcInfo } );
    }

    property int loadEmptyCont: 0
    Timer {
        id: loadFuncInfoTimer
        interval: 3000
        repeat: true
        onTriggered: {
            /// 0x4=手机应用面板应用
            rootPage.loadEmptyCont = rootPage.loadEmptyCont+1;
            if (theModel.count==0 || rootPage.loadEmptyCont<8) {
                theApp.loadSubscribeFuncInfo(0x4);
            }
            else {
                loadFuncInfoTimer.stop();
            }
        }
    }

    Connections {
        target: theApp
        onLoadSubscribeFuncInfoResult: {
            setFuncInfoModelData(funcInfo);
        }
    }
    function updateLocaleInfo() {
        titleBar.title = theLocales.getLocalText("main-frame.mid-button-my-app.tooltip", "Application");
        addContactMenuItem.label = theLocales.getLocalText("context-menu.add-contact.text", "New Contact");
    }

    Component.onCompleted: {
        updateLocaleInfo();
    }



//    WebEngineView {
//        anchors.fill: parent
//        url: "http://www.baidu.com"

//    }

//    Text {
//        text: qsTr("application page")
//        anchors.centerIn: parent
//    }
}

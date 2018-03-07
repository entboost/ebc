import QtQuick 2.0
import QtQuick.Controls 2.0
import Entboost.KeyFilter 1.0

Item {
    id: itemRoot
    property string display: "Press Again Exit"
    property int interval: 3000

    signal popObjectBack(int objectCount);
    signal returnBack();

//    function setRootFilter(object) {
//        return theKeyFilter.setRootFilter(object);
//    }
//    function removeRootFilter(object) {
//        theKeyFilter.removeRootFilter(object);
//    }
    function setFilter(object, deep) {
        theKeyFilter.setFilter(object, deep);
    }
    function removeFilter(object, deep) {
        theKeyFilter.removeFilter(object, deep);
    }
    property int objectCount: 0
    function clearObject() { objectCount=0; }
    function addObject() {
        return ++objectCount;
    }
    function popObject() {
        if (objectCount>0) {
            return --objectCount;
        }
        else {
            return 0;
        }
    }

//    property var object1: null
//    property var object2: null
//    property var object3: null
//    function addObject(obj) {
//        if (object1==null) {
//            object1 = obj;
//        }
//        else if (object1===obj) {
//            // ?
//        }
//        else if (object2==null) {
//            object2 = obj;
//        }
//        else if (object2===obj) {
//            // ?
//        }
//        else if (object3==null) {
//            object3 = obj;
//        }
//    }
//    function popObject() {
//        var result = null;
//        if (object3!=null) {
//            result = object3;
//            object3 = null;
//        }
//        else if (object2!=null) {
//            result = object2;
//            object2 = null;
//        }
//        else if (object1!=null) {
//            result = object1;
//            object1 = null;
//        }
//        return result;
//    }

    EbKeyFilter {
        id: theKeyFilter
        //        onKeyBackPressed: {
        //            itemRoot.returnBack();
        //        }
    }
    property bool backPressed: false;
    /// 链接过滤器信号
    Connections {
        target: theKeyFilter;
        onKeyBackPressed: {
            processBackPressed();
        }
    }
    function processBackPressed() {
//        var popObject = null;
//        if (object3!=null) {
//            popObject = object3;
//            object3 = null;
//        }
//        else if (object2!=null) {
//            popObject = object2;
//            object2 = null;
//        }
//        else if (object1!=null) {
//            popObject = object1;
//            object1 = null;
//        }

        if (objectCount>0) {
            objectCount = objectCount-1;
            rootItem.popObjectBack(objectCount);
        }
        else {
            /// 第一次接受到back
            if (!backPressed) {
                /// 启动一个定时器，在定时器结束后没收再次收到back信号就将back标记初始化
                timer.start();
                backPressed = true;
                /// 启动提示动画
                backAnimation.start();
            }
            else if (backPressed) {
                /// 在一定时间连续收到back信号
                itemRoot.returnBack();
            }
        }
    }

    Keys.onBackPressed: {
        event.accepted = true;
        processBackPressed();
    }

    /// 定时器
    Timer {
        id: timer
        interval: itemRoot.interval
        triggeredOnStart: false
        onTriggered: {
            backPressed = false;
            timer.stop();
        }
    }
    Rectangle {
        id: rectExit
        radius: 20
        z: 10
        width: 130
        height: 30
        color: "black"
        opacity: 0
        anchors.centerIn: parent
        Text {
            anchors.centerIn: parent
            text: display
            color: "white"
            //            font.pixelSize: 15
            z: 10
        }
    }
    /// 退出提示的动画
    SequentialAnimation {
        id: backAnimation
        NumberAnimation {
            target: rectExit
            property: "opacity"
            duration: 1000
            to: 100
            easing.type: Easing.InCubic
        }
        NumberAnimation {
            target: rectExit
            property: "opacity"
            duration: 1000
            to: 100
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: rectExit
            property: "opacity"
            duration: 1000
            to: 0
            easing.type: Easing.InOutQuad
        }
    }
    //    Component.onCompleted: {
    //        theKeyFilter.setFilter(itemRoot);
    //    }
}

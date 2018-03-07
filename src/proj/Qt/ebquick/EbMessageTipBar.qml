import QtQuick 2.0
import "js/ebbase.js" as Ebbasejs

Rectangle {
    id: rootRect
    x: 10
    width: parent.width-x*2
    height: 52
    y: 2
    radius: 5
    border.width: 1
    border.color: "#ddd"
    visible: false

    signal pressedFuncInfo(var funcInfo, string parameter);

    EbImageNameInfo {
        id: messageTipInfo
        y: 2
        x: 5
        width: parent.width-x*2
        height: 48
//        source: "assets:/res/imgdefaultdepartment.png"
//        name: "abcd"
//        info: "abcdasdfasdf"
    }
    Timer {
        id: timeToCloseTimer
        interval: 10000
        onTriggered: {
            rootRect.visible = false;
        }
    }

    /// 1=广播消息
    /// 2=订购应用通知，如邮件通知，协同办公通知=
    property int msgType: 0
    property var msgId: 0
    property var msgSubId: 0
    property string msgParameter: ""
    EbTimerPress {
        id: delayPressTimer
        onPressed: {
            rootRect.visible = false;
            if (rootRect.msgId>0) {
                /// *点击响应消息，设为已读
                theApp.ackMessage(rootRect.msgId.toString(), 1);
                rootRect.msgId = 0;
            }

            switch (msgType) {
            case 1:
            case 2:
                pressedFuncInfo( theApp.subscribeFuncInfo(msgSubId), msgParameter );
                break;
            default:
                break;
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        onPressed: {
            rootRect.color = "#ddd";
            delayPressTimer.initStart();
        }
        onReleased: {
            delayPressTimer.pressedReleased = true;
            rootRect.color = "#fff";
        }
        onExited: {
            rootRect.color = "#fff";
        }
    }

    Connections {
        target: theApp
        onReceivedSubMsg: {
            timeToCloseTimer.stop();
            messageTipInfo.textIconMode = true;
            messageTipInfo.icon = Ebbasejs.funcIconTextBySubscribeId(msgSubId);
            messageTipInfo.name = msgTitle;
            messageTipInfo.info = msgContent;
            timeToCloseTimer.start();
            rootRect.visible = true;

            rootRect.msgType = 2;
            rootRect.msgId = msgId;
            rootRect.msgSubId = msgSubId;
            rootRect.msgParameter = parameter;
        }

        onReceivedBcMsg: {
            timeToCloseTimer.stop();
            messageTipInfo.textIconMode = true;
            messageTipInfo.icon = Ebbasejs.funcIconTextBySubscribeId(msgSubId);
            messageTipInfo.name = msgTitle;
            messageTipInfo.info = msgContent;
            timeToCloseTimer.start();
            rootRect.visible = true;

            rootRect.msgType = 1;
            rootRect.msgId = msgId;
            rootRect.msgSubId = msgSubId;
            rootRect.msgParameter = parameter;

        }

    }
}

import QtQuick 2.0
import QtQuick.Controls 1.4
import Entboost.KeyFilter 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent
    z: 100

    signal returnBack();

    /// 用于父窗口请求返回子窗口，
    /// 如果有子窗口显示，隐藏后返回 false ，父窗口会根据返回 true 不处理
    /// 如果没有，返回 true，父窗口会隐藏当前窗口
    function requestReturnBack() {
        return true;
    }

    EbTitleBar {
        id: titleBar;
        width: parent.width
//        title: "消息通知提醒设置"
        onReturnBack: {
            if (requestReturnBack()) {
                rootItem.returnBack();
            }
        }
    }

    Column {
        y: titleBar.height
        width: parent.width
        height: parent.height-y

//        Rectangle {
//            width: parent.width
//            height: 10
//            color: "#eee"
//        }
        EbSwitchCheckedSelect {
            id: newMessageNotify
            height: 42
//            label: "接收新消息通知"
            checked: theApp.newMessageNotify(true)
//            checkedLabel: "开"
//            uncheckLabel: "关"
            onSwitchCheckedChanged: {
                if (!rootItem.inited) {
                    return;
                }
                theApp.setNewMessageNotify(checked);
            }
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbSwitchCheckedSelect {
            id: detailMessageNotify
            height: 42
//            label: "通知显示消息详情"
            checked: theApp.detailMessageNotify(true)
            checkedLabel: newMessageNotify.checkedLabel
            uncheckLabel: newMessageNotify.uncheckLabel
            onSwitchCheckedChanged: {
                if (!rootItem.inited) {
                    return;
                }
                theApp.setDetailMessageNotify(checked);
            }
        }
        Rectangle {
            width: parent.width
            height: 10
            color: "#eee"
        }
        EbSwitchCheckedSelect {
            id: voiceNotify
            height: 42
//            label: "声音"
            checked: theApp.voiceNotify(true)
            checkedLabel: newMessageNotify.checkedLabel
            uncheckLabel: newMessageNotify.uncheckLabel
            onSwitchCheckedChanged: {
                if (!rootItem.inited) {
                    return;
                }
                theApp.setVoiceNotify(checked);
                if (checked) {
                    theApp.playTone();
                }
            }
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbSwitchCheckedSelect {
            id: vibrateNotify
            height: 42
//            label: "振动"
            checked: theApp.vibrateNotify(false)
            checkedLabel: newMessageNotify.checkedLabel
            uncheckLabel: newMessageNotify.uncheckLabel
            onSwitchCheckedChanged: {
                if (!rootItem.inited) {
                    return;
                }
                theApp.setVibrateNotify(checked);
                if (checked) {
                    theApp.vibrate(500);
                }
            }
        }
        Rectangle {
            width: parent.width
            height: 10
            color: "#eee"
        }

    }

    function updateLocaleInfo() {
        titleBar.title = theLocales.getLocalText("message-notify-setting.title", "Message notify setting");
        newMessageNotify.checkedLabel = theLocales.getLocalText("message-notify-setting.checked", "Open");
        newMessageNotify.uncheckLabel = theLocales.getLocalText("message-notify-setting.uncheck", "Close");
        newMessageNotify.label = theLocales.getLocalText("message-notify-setting.new-message-notify", "New message notify");
        detailMessageNotify.label = theLocales.getLocalText("message-notify-setting.detail-message-notify", "Detail message notify");
        voiceNotify.label = theLocales.getLocalText("message-notify-setting.voice", "Voice");
        vibrateNotify.label = theLocales.getLocalText("message-notify-setting.vibrate", "Vibrate");
    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            if (requestReturnBack()) {
                rootItem.returnBack();
            }
        }
    }
    property bool inited: false
    Component.onCompleted: {
        var docRoot = rootItem.parent;
        while (docRoot.parent) {
            docRoot = docRoot.parent;
        }
        theKeyFilter.setFilter(docRoot, 1);
        updateLocaleInfo();
        rootItem.inited = true;
    }

}

import QtQuick 2.0
//import QtQuick.Window 2.2
//import QtWebEngine 1.5
import QtWebView 1.1
import Entboost.KeyFilter 1.0

Rectangle {
    id: rootItem
    visible: true
    z: 100
    anchors.fill: parent

    signal returnBack();

    function loadFuncInfo(funcInfo, groupId, parameter) {
        theWebBrowserView.stop();
        var url = theApp.subscribeFuncUrl(funcInfo.subscribeId.toString(),
                                          groupId.toString(),
                                          parameter);
//        console.log("************* loadFuncInfo, url=" + url);
        theWebBrowserView.url = url;
        titleBar.title = funcInfo.functionName;
//        theWebBrowserView.url = "http://www.entboost.com"
//        theWebBrowserView.url = "http://www.baidu.com"
    }
    function loadUrl(url, title) {
        theWebBrowserView.stop();
        theWebBrowserView.url = url;
        titleBar.title = title;
    }

    EbTitleBar {
        id: titleBar;
        width: parent.width
        title: "EbWebBrowser"
        iconLabel: "\uf00d"
        rightButton0Label: "\uf142"
        rightButton0Visible: true
        onPressedRightButton0: {
            browserBar.visible = !browserBar.visible;
            updateWebBrowserSize();
        }
        onReturnBack: {
            theWebBrowserView.stop();
            rootItem.returnBack();
        }
    }

    WebView {
//    WebEngineView {
        id: theWebBrowserView
        y: titleBar.height
        width: parent.width
//        height: parent.height-y
        height: browserBar.visible?(parent.height-y-browserBar.height):(parent.height-y)

//        onLoadingChanged: {
//            console.log("************* onLoadingChanged, url=" + loadRequest.url);
//        }
        onUrlChanged: {
//            console.log("************* onUrlChanged, url=" + url);
            /// ebim-call-account://0.0.0.65
            var ret = theApp.requestUrl(url);
//            refreshButton.enabled = (url.length>0 && url!="about:blank")?true:false;
        }
//        property bool resetSize: false
//        onLoadingChanged: {
//            if (!resetSize && loadRequest.status==WebView.LoadSucceededStatus) {
//                resetSize = true;
////                theWebBrowserView.height = parent.height-y;
//                theWebBrowserView.height = browserBar.visible?
//                            (parent.height-y-browserBar.height):(parent.height-y);
//            }
//        }
    }

    Rectangle {
        id: browserBar
        z: 10
        height: 42
        width: parent.width
        y: parent.height-height
        /// 必须初始设为 true
        visible: true
        Rectangle {
            /// line
            width: parent.width
            height: 1
            color: "#ddd"
        }
        EbButton {
            id: backButton
            y: 1
            border.width: 0
            height: parent.height-y
            width: height+20
            color: "transparent"
            fontFamily: "FontAwesome"
            label: "\uf104"
            textColor: enabled?"#333":"#999"
            buttonColor: "transparent"
            buttonPressedColor: "#eee"
            textPressedColor: theApp.mainColor
            borderColor: "transparent"
            fontPointSize: 16
            enabled: theWebBrowserView.canGoBack
            onClickedButton: theWebBrowserView.goBack();
        }
        EbButton {
            id: forwardButton
            x: backButton.width
            y: 1
            border.width: 0
            height: backButton.height
            width: backButton.width
            color: "transparent"
            fontFamily: "FontAwesome"
            label: "\uf105"
            textColor: enabled?"#333":"#999"
            textPressedColor: theApp.mainColor
            buttonColor: "transparent"
            buttonPressedColor: "#eee"
            borderColor: "transparent"
            fontPointSize: 16
            enabled: theWebBrowserView.canGoForward
            onClickedButton: theWebBrowserView.goForward();
        }
        EbButton {
            id: refreshButton
            x: backButton.width*2
            y: 1
            border.width: 0
            height: backButton.height
            width: backButton.width
            color: "transparent"
            fontFamily: "FontAwesome"
            label: "\uf021"
            textColor: enabled?"#333":"#999"
            textPressedColor: theApp.mainColor
            buttonColor: "transparent"
            buttonPressedColor: "#eee"
            borderColor: "transparent"
            fontPointSize: 14
            enabled: true
//            enabled: (theWebBrowserView.url.length>0 && theWebBrowserView.url!="about:blank")?true:false
            onClickedButton: theWebBrowserView.reload();
        }

    }

    function requestReturnBack() {
        if (theWebBrowserView.canGoBack) {
            theWebBrowserView.goBack();
            return false;
        }
        theWebBrowserView.url = "about:blank";
        return true;
    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            if (requestReturnBack()) {
                rootItem.returnBack();
            }
        }
    }
    function updateWebBrowserSize() {
        theWebBrowserView.height = browserBar.visible?
                    (rootItem.height-titleBar.height-browserBar.height):
                    (rootItem.height-titleBar.height);
    }
    Connections {
        target: theApp
        onRequestUrlRedirect: { theWebBrowserView.url = url; }
//        onRequestUrlCallUserId: { rootItem.returnBack(); }
//        onRequestUrlCallUserAccount: { rootItem.returnBack(); }
//        onRequestUrlCallGroup: { rootItem.returnBack(); }
//        onRequestUrlOpenSubId: { rootItem.returnBack(); }
    }
    onHeightChanged: {
        /// 解决屏幕转向时，浏览器高度能自动适应
        updateWebBrowserSize();
    }

    Component.onCompleted: {
        var docRoot = rootItem.parent;
        while (docRoot.parent) {
            docRoot = docRoot.parent;
        }
        theKeyFilter.setFilter(theWebBrowserView, 1);
//        updateLocaleInfo();
    }
}

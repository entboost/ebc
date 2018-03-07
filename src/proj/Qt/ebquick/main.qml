import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: mainApplicationWindow
    visible: true
    width: 480
    height: 640
    title: theApp.productName

//    onActiveChanged: {
//        console.log("********* onActiveChanged, active=" + active);
////        console.log("********* onActiveChanged, Qt.application.active=" + Qt.application.active);
//    }

    EbMessageTipBar {
        id: theMessageTipBar
        z: 1000
        onPressedFuncInfo: { doPressedFuncInfo(funcInfo, parameter); }
    }

    Item {
        id: loginLoadItem
        anchors.fill: parent
        visible: true
        z: 100
        Loader {
            id: loginLoader
            anchors.fill: parent
            source: "EbLogin.qml"
            focus: true
        }
        Connections {
            target: loginLoader.item
            onReturnBack: {
                Qt.quit();
            }
            onLoginSuccess: {
//                titleBar.visible = true;
                swipeView.visible = true;
                footerTabBar.visible = true;
                loginLoadItem.visible = false;
                /// 卸载登录界面
                loginLoader.source = "";
                /// 自动切换到最一页
                swipeView.setCurrentIndex(0);
                /// 登录成功，请求加载所有企业结构，好友信息数据
                contactPage.initData();
                /// 加载集成应用列表
                applicationsPage.loadApplicationData();
                sessionsPage.loadSessionData();
                myPage.updateMyData();
                /// 登录成功才安装按钮过滤器，避免影响登录界面处理
                theBackAnimation.setFilter(mainApplicationWindow, 1);
            }
        }
    }

    Item {
        id: webBrowserLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: webBrowserLoader
            anchors.fill: parent
            /// ***初始化必须填写，否则会打不开浏览器
            source: "EbWebBrowser.qml"
            focus: true
        }
        Connections {
            target: webBrowserLoader.item
            onReturnBack: {
                while (!requestReturnBack()) {
                }
            }
        }
    }
    Item {
        id: inputViewLoadItem
        anchors.fill: parent
        visible: false
        z: 100

        property var parentGroupId: null
        property int groupType: 0
        /// 1=新建部门/项目组/个人群组
        /// 2=添加分组
        /// 3=修改分组
        property var contactUGId: null
        property int inputType: 0
        Loader {
            id: inputViewLoader
            anchors.fill: parent
        }
        Connections {
            target: inputViewLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onAccepted: {
                requestReturnBack();
//                console.log("onAccepted text=" + text);
                if (inputViewLoadItem.inputType===3) {
                    /// 修改分组
                    theApp.editUGInfo(inputViewLoadItem.contactUGId.toString(), text);
                }
                else if (inputViewLoadItem.inputType===2) {
                    /// 添加分组
                    theApp.editUGInfo(0, text);
                }
                else if (inputViewLoadItem.inputType===1) {
                    /// 新建部门/项目组/个人群组
                    theApp.newGroupInfo(inputViewLoadItem.groupType,
                                        text,
                                        0,
                                        inputViewLoadItem.parentGroupId.toString());
                }
            }
        }
    }

    function doPressedFuncInfo(funcInfo, parameter) {
//        typedef enum EB_FUNC_MODE {
//            EB_FUNC_MODE_BROWSER			// 浏览器模式
//            , EB_FUNC_MODE_MAINFRAME                    // 主面板（默认）
//            , EB_FUNC_MODE_MODAL			// 对话框模式（模式）
//            , EB_FUNC_MODE_PROGRAM			// 打开应用程序
//            , EB_FUNC_MODE_SERVER			// 服务模式（HTTP POST）
//            , EB_FUNC_MODE_WINDOW			// 窗口模式（无模式）
//        }EB_FUNC_MODE;
        var functionMode = funcInfo.functionMode;
        /// 1002300104 = 我的邮件（使用默认打开）
        if (1002300104!=funcInfo.subscribeId && functionMode===0) {
            /// 浏览器模式，
            Qt.openUrlExternally(theApp.subscribeFuncUrl(funcInfo, 0, parameter));
            return;
        }
        /// 其他全部用面板模式
        footerTabBar.visible = false;
        theBackAnimation.removeFilter(mainApplicationWindow, 1);
        webBrowserLoader.source = "EbWebBrowser.qml";
        webBrowserLoader.item.loadFuncInfo(funcInfo, 0, parameter);
        webBrowserLoadItem.visible = true;
    }

    function checkAnyLoadItemVisible() {
        if (theLoadItem.visible) {
            return true;
        }
        if (groupInfoViewLoadItem.visible) {
            return true;
        }
        if (inputViewLoadItem.visible) {
            return true;
        }
        if (subGroupInfoViewLoadItem.visible) {
            return true;
        }

        return false;
    }

    /// 请求隐藏子窗口
    function requestReturnBack() {
        var ret = true;
        if (theMessageBoxLoadItem.visible) {
            theMessageBoxLoader.source = "";
            theMessageBoxLoadItem.visible = false;
            ret = false;
        }
        if (chatSessionViewLoadItem.visible) {
            if (!chatSessionViewLoader.item.requestReturnBack()) {
                return false;
            }
            chatSessionViewLoader.source = "";
            chatSessionViewLoadItem.visible = false;
            ret = false;
        }

        if (theLoadItem.visible) {
//            if (!theLoader.item.requestReturnBack()) {
//                return false;
//            }
            theLoadItem.visible = false;
            theLoader.source = "";
            ret = false;
        }
        if (webBrowserLoadItem.visible) {
            if (!webBrowserLoader.item.requestReturnBack()) {
                return false;
            }
            webBrowserLoadItem.visible = false;
            webBrowserLoader.source = "";
            ret = false;
        }
        ///
        if (groupInfoViewLoadItem.visible) {
//        if (groupInfoViewLoadItem.visible || groupInfoViewLoader.source!="") {
            if (!groupInfoViewLoader.item.requestReturnBack()) {
                return false;
            }
            groupInfoViewLoadItem.visible = false;
            groupInfoViewLoader.source = "";
            ret = false;
        }
        if (inputViewLoadItem.visible) {
            inputViewLoadItem.visible = false;
            inputViewLoader.source = "";
            ret = false;
        }
        if (subGroupInfoViewLoadItem.visible) {
            subGroupInfoViewLoadItem.visible = false;
            subGroupInfoViewLoader.source = "";
            ret = false;
        }

        /// 重新安装事件过滤器，保证能处理 BACK 按键
        swipeView.visible = true;
        footerTabBar.visible = true;
        theBackAnimation.setFilter(mainApplicationWindow, 1);
        return ret;
    }
//    function requestHideWindow() {
//        var ret = false;
//        if (groupInfoViewLoadItem.visible) {
//            if (groupInfoViewLoader.item.requestReturnBack()) {
//                groupInfoViewLoadItem.visible = false;
//                groupInfoViewLoader.source = "";
//                ret = true;
//            }
//        }
//        if (ret) {

//            footerTabBar.visible = true;
//            theBackAnimation.setFilter(mainApplicationWindow, 1);
//        }
//        return ret;
//    }

    Item {
        id: theLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: theLoader
            anchors.fill: parent
        }
        Connections {
            target: theLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onRequestLogout: {
                while (!requestReturnBack()) {
                }
                doRequestLogout(tip, true);
            }

            onSendContactMessage: {
                requestReturnBack();
                sendUserMessage(contactInfo.contactUserId);
            }
            onPressedGroupInfo: {
                requestReturnBack();
                doPressedGroupInfo(groupInfo);
            }
            onPressedMemberInfo: {
                requestReturnBack();
                doPressedMemberInfo(groupInfo, memberInfo);
            }
            onPressedContactInfo: {
                requestReturnBack();
                doPressedContactInfo(contactInfo);
            }
            onPressedFuncInfo: {
                requestReturnBack();
                doPressedFuncInfo(funcInfo, "");
            }
        }
    }
    Item {
        id: groupInfoViewLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: groupInfoViewLoader
            anchors.fill: parent
        }
        Connections {
            target: groupInfoViewLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onSendMemberMessage: {
                requestReturnBack();
                sendUserMessage(memberInfo.memberUserId);
//                rootItem.sendMemberMessage(memberInfo);
            }
            onSendGroupMessage: {
                requestReturnBack();
                sendGroupMessage(groupInfo.groupId);
            }

        }
    }
    Item {
        id: subGroupInfoViewLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: subGroupInfoViewLoader
            anchors.fill: parent
        }
        Connections {
            target: subGroupInfoViewLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onSendGroupMessage: {
                requestReturnBack();
                sendGroupMessage(groupInfo.groupId);
            }
        }
    }
    Item {
        id: chatSessionViewLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: chatSessionViewLoader
            anchors.fill: parent
        }
        Connections {
            target: chatSessionViewLoader.item
            onReturnBack: {
                requestReturnBack();
            }
            onSendUserMessage: {
                requestReturnBack();
                sendUserMessage(userId);
            }
            onSendContactMessage: {
                requestReturnBack();
                sendUserMessage(contactInfo.contactUserId);
            }
            onSendMemberMessage: {
                requestReturnBack();
                sendUserMessage(memberInfo.memberUserId);
            }

        }
    }
    Item {
        id: theMessageBoxLoadItem
        anchors.fill: parent
        visible: false
        z: 100
        Loader {
            id: theMessageBoxLoader
            anchors.fill: parent
        }
        /// 1=删除聊天
        /// 2=删除联系人
        property int messageBoxType: 0
        property var sessionInfo: null
        property var contactInfo: null
        Connections {
            target: theMessageBoxLoader.item
            onAccepted: {
                requestReturnBack();
                switch (theMessageBoxLoadItem.messageBoxType)
                {
                case 1:
                    /// 删除聊天
                    if (theMessageBoxLoadItem.sessionInfo.isMsgSession) {
                        /// 通知消息，不能删除 groupId/fromUserId 数据
                        theApp.deleteChat(theMessageBoxLoadItem.sessionInfo.callId.toString(), 0, 0);
                    }
                    else {
                        theApp.deleteChat(theMessageBoxLoadItem.sessionInfo.callId.toString(),
                                          theMessageBoxLoadItem.sessionInfo.groupId.toString(),
                                          theMessageBoxLoadItem.sessionInfo.fromUserId.toString());
                    }
                    sessionsPage.deleteCurrentIndex();
                    break;
                case 2:
                    /// 删除联系人
                    theApp.deleteContactInfo(theMessageBoxLoadItem.contactInfo.contactId.toString());
                    break;
                }
            }
            onRejected: {
                requestReturnBack();
            }
        }
    }

    function sendGroupMessage(groupId) {
        footerTabBar.visible = false;
        theBackAnimation.removeFilter(mainApplicationWindow, 1);
        chatSessionViewLoader.setSource("EbChatSessionView.qml",
                                        { "groupId": groupId } );
        chatSessionViewLoadItem.visible = true;
    }
    function sendUserMessage(userId) {
        if (userId==theApp.logonUserId) {
            /// 不可以给自己发消息
            return;
        }
        footerTabBar.visible = false;
        theBackAnimation.removeFilter(mainApplicationWindow, 1);
        chatSessionViewLoader.setSource("EbChatSessionView.qml",
                                      { "fromUserId": userId } );
        chatSessionViewLoadItem.visible = true;
    }
    function sendAccountMessage(account) {
        if (account==theApp.logonAccount) {
            /// 不可以给自己发消息
            return;
        }
        footerTabBar.visible = false;
        theBackAnimation.removeFilter(mainApplicationWindow, 1);
        chatSessionViewLoader.setSource("EbChatSessionView.qml",
                                        { "fromAccount": account } );
        chatSessionViewLoadItem.visible = true;
    }
    function doPressedGroupInfo(groupInfo) {
        if (checkAnyLoadItemVisible()) {
            /// 当前 有显示窗口，不处理
            return;
        }
        footerTabBar.visible = false;
        theBackAnimation.removeFilter(mainApplicationWindow, 1);
        groupInfoViewLoader.source = "EbGroupInfoView.qml";
        groupInfoViewLoader.item.setGroupInfo(groupInfo);
        groupInfoViewLoadItem.visible = true;
    }
    function doPressedMemberInfo(groupInfo, memberInfo) {
        if (checkAnyLoadItemVisible()) {
            /// 当前 有显示窗口，不处理
            return;
        }
//        theViewLoader.source = "EbMemberInfoView.qml";
//        theViewLoader.item.setMemberInfo(groupInfo, memberInfo);
//        theViewLoadItem.visible = true;
        footerTabBar.visible = false;
        theBackAnimation.removeFilter(mainApplicationWindow, 1);
        groupInfoViewLoader.source = "EbMemberInfoView.qml";
        groupInfoViewLoader.item.setMemberInfo(groupInfo, memberInfo);
        groupInfoViewLoadItem.visible = true;
    }
    function doPressedContactInfo(contactInfo) {
        if (checkAnyLoadItemVisible()) {
            /// 当前 有显示窗口，不处理
            return;
        }
        footerTabBar.visible = false;
        theBackAnimation.removeFilter(mainApplicationWindow, 1);
        theLoader.source = "EbContactInfoView.qml";
        theLoader.item.setContactInfo(contactInfo);
        theLoadItem.visible = true;
    }
    function doRequestSearch() {
        if (checkAnyLoadItemVisible()) {
            /// 当前 有显示窗口，不处理
            return;
        }
        footerTabBar.visible = false;
        theBackAnimation.removeFilter(mainApplicationWindow, 1);
        theLoader.source = "EbSearchView.qml";
        theLoadItem.visible = true;
    }
    function doRequestOpenSubId(subId, parameter) {
        var funcInfo = theApp.subscribeFuncInfo(subId);
        if (funcInfo==null) {
            return;
        }
        doPressedFuncInfo(funcInfo, parameter);
    }

    function doRequestAddContact() {
        /// 添加联系人
        if (checkAnyLoadItemVisible()) {
            /// 当前 有显示窗口，不处理
            return;
        }
//        if (theApp->isAuthContact()) {
//            /// 添加好友需要验证
//            if (theApp->findAppSubId()==0) {
//                /// 暂时未开放添加好友验证功能：\r\n请联系管理员！
//                EbMessageBox::doShow( NULL, "", QChar::Null,
//                                      theLocales.getLocalText( "message-show.not-support-auth-contact","Not supprt auth contact" ),
//                                      EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
//            }
//            else {
//                /// 找群找人
//                theApp->mainWnd()->openSubscribeFuncWindow(theApp->findAppSubId());
//            }
//        }
        /// 打开“找群找人”应用
        doRequestOpenSubId(1002300102, "");
    }

    function doRequestLogout(tip, acceptPush) {
        /// 注销（更换帐号）
        swipeView.visible = false;
        footerTabBar.visible = false;
        unreadText.text = "";
        theBackAnimation.removeFilter(mainApplicationWindow, 1);
        theApp.logout(true, acceptPush);
        theMessageTipBar.visible = false;
        loginLoader.setSource("EbLogin.qml",
                              { logoutMode: true, "tip": tip } );
        loginLoadItem.visible = true;
    }

    SwipeView {
        id: swipeView
        visible: false
        anchors.fill: parent
        currentIndex: footerTabBar.currentIndex

        EbSessionsPage {
            id: sessionsPage
            onPressedFuncInfo: {
                doPressedFuncInfo(funcInfo, parameter);
            }
            onRequestAddContact: { doRequestAddContact(); }
            onRequestSearch: { doRequestSearch(); }
            onUnreadCout: {
                if (count==0) {
                    unreadText.text = "";
                }
                else {
                    unreadText.text = count;
                }
            }

            onRequestDeleteChat: {
                if (checkAnyLoadItemVisible()) {
                    /// 当前 有显示窗口，不处理
                    return;
                }
                var title;
                var text;
                if (sessionInfo.isMsgSession) {
                    title = theLocales.getLocalText("message-box.delete-chat-msg.title","Clear Chat Record");
                    text = theLocales.getLocalText("message-box.delete-chat-msg.text","Confirm Clear:<br>[GROUP_NAME] Chat Record?");
                }
                else if (sessionInfo.isGroupCall ) {
                    title = theLocales.getLocalText("message-box.delete-session-group-chat.title","Clear Chat Record");
                    text = theLocales.getLocalText("message-box.delete-session-group-chat.text","Confirm Clear:<br>[GROUP_NAME] Chat Record?");
                    text = text.replace("[GROUP_NAME]", sessionInfo.groupName);
                }
                else {
                    title = theLocales.getLocalText("message-box.delete-session-user-chat.title","Clear Chat Record");
                    text = theLocales.getLocalText("message-box.delete-session-user-chat.text","Confirm Clear:<br>[USER_NAME](USER_ID) Chat Record?");
                    text = text.replace("[USER_NAME]", sessionInfo.fromName);
                    text = text.replace("[USER_ID]", sessionInfo.fromUserId);
                }
                theBackAnimation.removeFilter(mainApplicationWindow, 1);
                theMessageBoxLoadItem.messageBoxType = 1;
                theMessageBoxLoadItem.sessionInfo = sessionInfo;
                theMessageBoxLoadItem.visible = true;
                theMessageBoxLoader.setSource("EbMessageDialog.qml", {"title": title, "text": text});
                theMessageBoxLoader.item.open();
            }

            onRequestOpenGroupSession: {
                if (checkAnyLoadItemVisible()) {
                    /// 当前 有显示窗口，不处理
                    return;
                }
                sendGroupMessage(groupId);
            }
            onRequestOpenUserSession: {
                if (checkAnyLoadItemVisible()) {
                    /// 当前 有显示窗口，不处理
                    return;
                }
                sendUserMessage(userId);
            }
        }
        EbContactPage {
            id: contactPage
            onRequestSearch: { doRequestSearch(); }
            onPressedContactInfo: {
                /// 查看联系人资料
                doPressedContactInfo(contactInfo);
            }
            onRequestDeleteContact: {
                /// 删除联系人资料
                var title;
                var text;
                if (theApp.isAuthContact && contactInfo.contactUserId>0) {
                    /// 确定删除联系人：\r\n%s(%s) 同时从对方联系人列表中消失吗？
                    title = theLocales.getLocalText("message-box.delete-auth-contact.title","Delete Contact");
                    text = theLocales.getLocalText("message-box.delete-auth-contact.text","Confirm delete contact?");
                }
                else {
                    /// 确定删除联系人：\r\n%s(%s) 吗？
                    title = theLocales.getLocalText("message-box.delete-contact.title","Delete Contact");
                    text = theLocales.getLocalText("message-box.delete-contact.text","Confirm delete contact?");
                }
                if (title.length==0) {
                    title = theApp.productName;
                }
                text = text.replace("[USER_NAME]", contactInfo.name);
                text = text.replace("[USER_ACCOUNT]", contactInfo.contact);
                theBackAnimation.removeFilter(mainApplicationWindow, 1);
                theMessageBoxLoadItem.messageBoxType = 2;
                theMessageBoxLoadItem.contactInfo = contactInfo;
                theMessageBoxLoadItem.visible = true;
                theMessageBoxLoader.setSource("EbMessageDialog.qml", {"title": title, "text": text});
                theMessageBoxLoader.item.open();

//                /// MessageBox
//                if (EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )==QDialog::Accepted) {
//                    if (theApp->isAuthContact())
//                        theApp->m_ebum.EB_DeleteContact(m_itemInfo->m_sId,true);
//                    else
//                        theApp->m_ebum.EB_DeleteContact(m_itemInfo->m_sId,false);
//                }
            }

            onRequestNewContactGroup: {
                /// 添加分组
                footerTabBar.visible = false;
                theBackAnimation.removeFilter(mainApplicationWindow, 1);
                /// 打开新建提示对话框
                var acceptButtonLabel = theLocales.getLocalText("base-dialog.save-button.text", "Save");
                var title = theLocales.getLocalText("context-menu.add-contact-group.text", "Add contact group")
                inputViewLoader.setSource("EbInputView.qml",
                                          {"title": title,
                                              "acceptButtonLabel": acceptButtonLabel});
                inputViewLoadItem.inputType = 2;
                inputViewLoadItem.visible = true;
            }
            onRequestRenameUGinfo: {
                /// 修改分组
                footerTabBar.visible = false;
                theBackAnimation.removeFilter(mainApplicationWindow, 1);
                /// 打开新建提示对话框
                var acceptButtonLabel = theLocales.getLocalText("base-dialog.save-button.text", "Save");
                var title = theLocales.getLocalText("context-menu.add-contact-group.text", "Add contact group")
                inputViewLoader.setSource("EbInputView.qml",
                                          {"title": title,
                                              "text": groupName,
                                              "acceptButtonLabel": acceptButtonLabel});
                inputViewLoadItem.inputType = 3;
                inputViewLoadItem.contactUGId = ugId;
                inputViewLoadItem.visible = true;
            }

            onRequestAddContact: { doRequestAddContact(); }

            onRequestNewGroup: {
                if (groupType<=2) {
                    /// 0=新建部门
                    /// 1=新建项目组
                    /// 2=新建个人群组
                    footerTabBar.visible = false;
                    theBackAnimation.removeFilter(mainApplicationWindow, 1);
                    /// 打开新建提示对话框
                    var acceptButtonLabel = theLocales.getLocalText("base-dialog.new-button.text", "New");
                    var groupTypeInfo = theLocales.getGroupTypeInfo(groupType);
                    var title = acceptButtonLabel+groupTypeInfo.groupTypeName;
                    var placeholderText = theLocales.getLocalText("group-info-dialog.edit-group-name.bg-text","");
                    if (placeholderText.length>0) {
                        placeholderText = placeholderText.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
                    }
                    inputViewLoader.setSource("EbInputView.qml",
                                              {"title": title,
                                                  "placeholderText": placeholderText,
                                                  "acceptButtonLabel": acceptButtonLabel});
                    inputViewLoadItem.inputType = 1;
                    inputViewLoadItem.groupType = groupType;
                    inputViewLoadItem.parentGroupId = parentGroupId;
                    inputViewLoadItem.visible = true;
                }
            }

            onPressedGroupInfo: {
                doPressedGroupInfo(groupInfo);
            }
            onPressedLoadSubGroupInfo: {
                if (checkAnyLoadItemVisible()) {
                    /// 当前 有显示窗口，不处理
                    return;
                }
                footerTabBar.visible = false;
                theBackAnimation.removeFilter(mainApplicationWindow, 1);
                subGroupInfoViewLoader.source = "EbSubGroupView.qml";
                subGroupInfoViewLoader.item.setGroupInfo(parentGroupInfo);
                subGroupInfoViewLoadItem.visible = true;
            }
        }

        EbApplicationsPage {
            id: applicationsPage
            onRequestAddContact: { doRequestAddContact(); }
            onRequestSearch: { doRequestSearch(); }
            onPressedFuncInfo: { doPressedFuncInfo(funcInfo); }
        }
        EbMyPage {
            id: myPage

            onRequestLogout: { doRequestLogout("", false); }
            onRequestAddContact: { doRequestAddContact(); }
            onRequestSearch: { doRequestSearch(); }
            onPressedAccountInfo: {
                if (checkAnyLoadItemVisible()) {
                    /// 当前 有显示窗口，不处理
                    return;
                }
                swipeView.visible = false;
                footerTabBar.visible = false;
                theBackAnimation.removeFilter(mainApplicationWindow, 1);
                theLoader.source = "EbAccountInfoView.qml";
                theLoader.item.setAccountInfo(accountInfo);
                theLoadItem.visible = true;
            }


            onRequestMessageNotifySetting: {
                if (checkAnyLoadItemVisible()) {
                    /// 当前 有显示窗口，不处理
                    return;
                }
                swipeView.visible = false;
                footerTabBar.visible = false;
                theBackAnimation.removeFilter(mainApplicationWindow, 1);
                theLoader.source = "EbMessageNotifySetting.qml";
                theLoadItem.visible = true;
            }

            onRequestModifyPassword: {
                if (checkAnyLoadItemVisible()) {
                    /// 当前 有显示窗口，不处理
                    return;
                }

                footerTabBar.visible = false;
                theBackAnimation.removeFilter(mainApplicationWindow, 1);
                theLoader.source = "EbModifyPassword.qml";
                theLoadItem.visible = true;
            }

            onPressedFuncInfo: {
                doPressedFuncInfo(funcInfo);
            }
        }
    }

    footer: TabBar {
        id: footerTabBar
        height: 42
        visible: false
        currentIndex: swipeView.currentIndex

        EbTabButton {
            id: tabButton0
            icon: "\uf086"
//            label: qsTr("聊天")

            Rectangle {
                id: unreadRect
                z: 1
                visible: unreadText.text.length>0?true:false
                width: 18
                height: width
                color: "red"
                radius: width/2
                x: parent.width/2+10    /// 中间往右10位置
                y: 1
                Text {
                    id: unreadText
                    anchors.centerIn: parent
                    color: "#fff"
//                    text: "10"
                    font.pointSize: 12
                }
            }
        }
        EbTabButton {
            id: tabButton1
            icon: "\uf007"
//            label: qsTr("联系人")
        }
        EbTabButton {
            id: tabButton2
            icon: "\uf009"
//            label: qsTr("应用")
        }

        EbTabButton {
            id: tabButton3
            icon: "\uf013"
//            label: qsTr("我")
        }
        onCurrentIndexChanged: {
            tabButton0.checkedStateChanged(currentIndex==0?true:false);
            tabButton1.checkedStateChanged(currentIndex==1?true:false);
            tabButton2.checkedStateChanged(currentIndex==2?true:false);
            tabButton3.checkedStateChanged(currentIndex==3?true:false);
        }
        Component.onCompleted: {
            tabButton0.checkedStateChanged(true);
        }
    }

    EbBackAnimation {
        id: theBackAnimation
        z: 10
        interval: 3000
        display: theLocales.getLocalText("message-show.press-again-exit", "Press Again Exit");

        anchors.centerIn: parent
        onReturnBack: {
//            mainApplicationWindow.visibility = Windows.Minimized;
//            Qt.application.active = false;
//            Qt.application.state = Qt.ApplicationHidden;
            /// 没有效果，重进时还会导致主界面黑色
//            mainApplicationWindow.visible = false
            Qt.quit()
        }

        onPopObjectBack: {
            if (requestReturnBack()) {
                theBackAnimation.clearObject();
            }
            else {
                theBackAnimation.addObject();
            }

//            requestHideWindow();
        }
    }
//    property var rootFilterObject: null
    function updateLocaleInfo() {
        tabButton0.label = theLocales.getLocalText("main-frame.main-phone.chat", "Chat");
        tabButton1.label = theLocales.getLocalText("main-frame.main-phone.contact", "Contact");
        tabButton2.label = theLocales.getLocalText("main-frame.main-phone.application", "Application");
        tabButton3.label = theLocales.getLocalText("main-frame.main-phone.me", "Me");
    }
    Connections {
        target: theApp
        onLogonAnother: {
            while (!requestReturnBack()) {
            }

            /// 0=已经在其他地方登录，退出当前连接；
            /// 1=修改密码，退出当前连接；
            if (type==0)
                doRequestLogout(theLocales.getLocalText("message-show.logon-another", "Logon another"), true);
            else if (type==1)
                doRequestLogout(theLocales.getLocalText("on-editinfo-response.modify-password-ok.text", "Modify password ok"), true);
            else
                doRequestLogout("", true);
        }
        onRequestUrlCallUserId: {
            requestReturnBack();
            sendUserMessage(userId);
        }
        onRequestUrlCallUserAccount: {
            requestReturnBack();
            sendAccountMessage(account);
        }
        onRequestUrlCallGroup: {
            requestReturnBack();
            sendGroupMessage(groupId);
        }
        onRequestUrlOpenSubId: {
            requestReturnBack();
            doRequestOpenSubId(subId, parameter);
        }
    }
    Component.onCompleted: {
        updateLocaleInfo();
    }

}

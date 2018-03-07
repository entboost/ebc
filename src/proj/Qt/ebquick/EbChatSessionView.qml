import QtQuick 2.7
import QtQuick.Controls 2.2
import Entboost.KeyFilter 1.0
import Entboost.RichInfo 1.0
import Entboost.ECardInfo 1.0

Rectangle {
    id: rootRect
    color: "#fafafa"

    property var groupId: 0
    property var fromUserId: 0
    property string fromAccount: ""
    property string fromName: ""

    signal returnBack();
    signal sendContactMessage(var contactInfo);
    signal sendMemberMessage(var memberInfo);
    signal sendUserMessage(var userId);
    signal sendGroupMessage(var groupInfo);

    property string userHeadSource: ""  /// 用于单聊
    property var connectedCallInfo: null
    function requestReturnBack() {
        if (webBrowserLoadItem.visible) {
            webBrowserLoader.source = ""
            webBrowserLoadItem.visible = false;
            return false;
        }
        if (theMessageBoxLoadItem.visible) {
            theMessageBoxLoader.source = ""
            theMessageBoxLoadItem.visible = false;
            return false;
        }
        if (theViewLoadItem.visible) {
//            if (!theViewLoader.item.requestReturnBack()) {
//                return false;
//            }
            theViewLoader.source = "";
            theViewLoadItem.visible = false;
            return false;
        }
        if (inputRect.showedEmotion) {
            inputRect.showedEmotion = false;
            return false;
        }
        if (inputRect.showedPlus) {
            inputRect.showedPlus = false;
            return false;
        }

        theRichModel.clear();
        theApp.currentChatGroupId = "0";
        theApp.currentChatUserId = "0";
        return true;
    }

//    function setGroupChat(groupId) {
//        theRichModel.clear();
//        rootPage.loadEmptyCount = 0;
//        loadFuncInfoTimer.stop();
//        loadFuncInfoTimer.start();
//    }

    function isCurrentChatInfo(groupId, fromUserId) {
        if (rootRect.groupId>0 && rootRect.groupId!=groupId) {
            /// 其他群组
            return false;
        }
        else if (rootRect.groupId==0 && groupId>0) {
            /// 其他群组
            return false;
        }
        else if (fromUserId!=rootRect.fromUserId && fromUserId!=theApp.logonUserId) {
            /// 其他单聊窗口
            return false;
        }

        return true;

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
        /// ~2=文件
        /// 10=邀请好友
        /// 11=发送名片
        /// 12=转发消息
        property int viewType: 0
        Connections {
            target: theViewLoader.item
            onSendUserMessage: {
                requestReturnBack();
                if (rootRect.groupId==0 && userId==rootRect.fromUserId) {
                    /// 当前会话不处理
                    return;
                }
                rootRect.sendUserMessage(userId);
            }

            onSelectedMemberInfo: {
                switch (theViewLoadItem.viewType)
                {
                case 10:
                    /// 邀请好友
                    if (!rootRect.connectedCallInfo.isGroupCall) {
                        theApp.call2Group(rootRect.connectedCallInfo.callId.toString(),
                                          memberInfo.memberAccount);
                    }
                    else {
                        theApp.callUserId(memberInfo.memberUserId.toString(),
                                          rootRect.connectedCallInfo.callId.toString());
                    }
                    break;
                case 11:
                    /// 发送名片
                    theApp.sendUserAccountEChard(rootRect.connectedCallInfo.callId.toString(),
                                                 memberInfo.memberUserId.toString());
                    break;
                }
            }

            onSelectedContactInfo: {
                switch (theViewLoadItem.viewType)
                {
                case 10:
                    /// 邀请好友
                    if (!rootRect.connectedCallInfo.isGroupCall) {
                        theApp.call2Group(rootRect.connectedCallInfo.callId.toString(),
                                          contactInfo.contact);
                    }
                    else if (contactInfo.contactUserId>0){
                        theApp.callUserId(contactInfo.contactUserId.toString(),
                                          rootRect.connectedCallInfo.callId.toString());
                    }
                    else {
                        theApp.callAccount(contactInfo.contact,
                                           rootRect.connectedCallInfo.callId.toString());
                    }
                    break;
                case 11:
                    /// 发送名片
                    if (contactInfo.contactUserId>0){
                        theApp.sendUserIdEChard(rootRect.connectedCallInfo.callId.toString(),
                                                contactInfo.contactUserId.toString());
                    }
                    else {
                        theApp.sendUserAccountEChard(rootRect.connectedCallInfo.callId.toString(),
                                                     contactInfo.contact);
                    }
                    break;
                }
            }
            onPressedMemberInfo: {
                requestReturnBack();
                switch (theViewLoadItem.viewType)
                {
                case 12:
                    /// 转发消息
                    if (rootRect.groupId==0 && memberInfo.memberUserId==rootRect.fromUserId) {
                        /// 当前会话不处理
                        break;
                    }
                    var obj = theRichModel.get(theRichView.currentIndex);
                    if (obj==null) {
                        break;
                    }
                    theApp.forwardMessageToUserId(memberInfo.memberUserId.toString(),
                                                  obj.richInfo.msgId.toString());
                    break;
                }
            }
            onPressedContactInfo: {
                requestReturnBack();
                switch (theViewLoadItem.viewType)
                {
                case 12:
                    /// 转发消息
                    if (rootRect.groupId==0 && contactInfo.contactUserId==rootRect.fromUserId) {
                        /// 当前会话不处理
                        break;
                    }
                    var obj = theRichModel.get(theRichView.currentIndex);
                    if (obj==null) {
                        break;
                    }
                    if (contactInfo.contactUserId>0){
                        theApp.forwardMessageToUserId(contactInfo.contactUserId.toString(),
                                                      obj.richInfo.msgId.toString());
                    }
                    else {
                        theApp.forwardMessageToAccount(contactInfo.contact,
                                                       obj.richInfo.msgId.toString());
                    }

                    break;
                }
            }

            onPressedGroupInfo: {
                requestReturnBack();
                switch (theViewLoadItem.viewType)
                {
                case 12:
                    /// 转发消息
                    if (rootRect.groupId>0 && groupInfo.groupId==rootRect.groupId) {
                        /// 当前会话不处理
                        break;
                    }
                    else if (groupInfo.myMemberId==0) {
                        break;
                    }
                    var obj = theRichModel.get(theRichView.currentIndex);
                    if (obj==null) {
                        break;
                    }
                    theApp.forwardMessageToGroupId(groupInfo.groupId.toString(),
                                                   obj.richInfo.msgId.toString());
                    break;
                }
            }

            onReturnBack: {
                requestReturnBack();
            }
            onSendContactMessage: {
                requestReturnBack();
                if (rootRect.groupId==0 &&
                        rootRect.fromUserId==contactInfo.contactUserId) {
                    /// 一对一聊天，并且相同UID,不需要重新处理
                    return;
                }
                rootRect.sendContactMessage(contactInfo);
            }
            onSendGroupMessage: {
                requestReturnBack();
                if (rootRect.groupId>0 &&
                        rootRect.groupId==groupInfo.groupId) {
                    /// 本群聊ID,不需要重新处理
                    return;
                }
                rootRect.sendGroupMessage(groupInfo);
            }

            onSendMemberMessage: {
                requestReturnBack();
                if (rootRect.groupId==0 &&
                        rootRect.fromUserId==memberInfo.memberUserId) {
                    /// 一对一聊天，并且相同UID,不需要重新处理
                    return;
                }
                rootRect.sendMemberMessage(memberInfo);
            }
            onAccepted: {
                var imageFile;
                if (theViewLoadItem.viewType==1) {
                    imageFile = theViewLoader.item.source;
                    theApp.sendImage(rootRect.connectedCallInfo.callId.toString(), imageFile);
                }
//                else if (theViewLoadItem.viewType==2) {
//                    imageFile = theViewLoader.item.source;
//                    theApp.sendFile(rootRect.connectedCallInfo.callId.toString(), imageFile);
//                }
                theViewLoadItem.viewType = 0;
                requestReturnBack();
            }
//            onSendGroupMessage: {
//                requestReturnBack();
////                rootItem.sendGroupMessage(groupInfo);
//            }
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
        /// 1=删除消息
        property int messageBoxType: 0
        Connections {
            target: theMessageBoxLoader.item
            onAccepted: {
//                tipText.text = "";
                //                waittingGroupEditResponse = true;
                theMessageBoxLoader.source = "";
                theMessageBoxLoadItem.visible = false;
                if (theMessageBoxLoadItem.messageBoxType==1) {
                    var obj = theRichModel.get(theRichView.currentIndex);
                    if (obj==null) {
                        return;
                    }
                    /// 删除本地消息
                    theApp.deleteDbRecord(obj.richInfo.msgId.toString());
                    theRichModel.remove(theRichView.currentIndex);
                }
            }
            onRejected: {
                theMessageBoxLoader.source = "";
                theMessageBoxLoadItem.visible = false;
            }
        }
    }

    function hideAllMenuItem() {
        copyMessageMenuItem.visible = false;
        forwardMessageMenuItem.visible = false;
        deleteMessageMenuItem.visible = false;
        collectMessageMenuItem.visible = false;
        withdrawMessageMenuItem.visible = false;
    }

    EbMenu {
        id: theChatMenu
        EbMenuItem {
            id: copyMessageMenuItem
            label: theLocales.getLocalText("chat-toolbar.selected-text.copy.text","Copy")
//            label: "复制"
            onTriggered: {
                var obj = theRichModel.get(theRichView.currentIndex);
                if (obj==null) {
                    return;
                }
                theApp.copyMessage(obj.richInfo.msgId);
            }
        }
        EbMenuItem {
            id: forwardMessageMenuItem
            label: theLocales.getLocalText("chat-toolbar.selected-text.forward.text","Forward")
//            label: "转发"
            onTriggered: {
                theViewLoader.setSource("EbSearchView.qml",
                                        { "searchMode": 3,
                                            "searchFromUserId": rootRect.fromUserId } );
                theViewLoadItem.viewType = 12;
                theViewLoadItem.visible = true;
            }
        }
        EbMenuItem {
            id: deleteMessageMenuItem
            label: theLocales.getLocalText("chat-toolbar.point-user-head.delete.text","Delete")
//            label: "删除消息"
            onTriggered: {
                var obj = theRichModel.get(theRichView.currentIndex);
                if (obj==null) {
                    return;
                }
                if (obj.richType==2 || obj.richInfo.msgId==0) {
                    theRichModel.remove(theRichView.currentIndex);
                    return;
                }
                var title = theLocales.getLocalText("message-box.delete-chat-msg.title","Delete Chat Msg");
                if (title.length==0) {
                    title = theApp.productName;
                }
                var text = theLocales.getLocalText("message-box.delete-chat-msg.text","Confirm Delete Chat Msg?");
                theMessageBoxLoadItem.messageBoxType = 1;
                theMessageBoxLoadItem.visible = true;
                theMessageBoxLoader.setSource("EbMessageDialog.qml", {"title": title, "text": text});
                theMessageBoxLoader.item.open();
            }
        }
        EbMenuItem {
            id: collectMessageMenuItem
            label: theLocales.getLocalText("chat-toolbar.point-user-head.collect.text","Collect")
//            label: "收藏消息"
            onTriggered: {
                var obj = theRichModel.get(theRichView.currentIndex);
                if (obj==null) {
                    return;
                }
                if (obj.richInfo.msgId==0) {
                    return;
                }
                theApp.collectMessage(rootRect.connectedCallInfo.callId.toString(),
                                      obj.richInfo.msgId.toString());
            }
        }
        EbMenuItem {
            id: withdrawMessageMenuItem
            label: theLocales.getLocalText("chat-toolbar.point-user-head.withdraw.text","Withdraw")
//            label: "撤回消息"
            onTriggered: {
                var obj = theRichModel.get(theRichView.currentIndex);
                if (obj==null) {
                    return;
                }
                if (obj.richInfo.msgId==0) {
                    return;
                }
                theApp.withdrawMessage(rootRect.connectedCallInfo.callId.toString(),
                                       obj.richInfo.msgId.toString());
            }
        }

//        MenuSeparator {
//            id: removeMemberSeparator
//            padding: 0
//            topPadding: 3
//            bottomPadding: 3
//            contentItem: Rectangle {
//                implicitWidth: parent.implicitWidth
//                implicitHeight: 1
//                color: "#1E000000"
//            }
//        }
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
                webBrowserLoader.source = "";
                webBrowserLoadItem.visible = false;
            }
        }
    }

    function openUserInfoView(userId) {
        var groupInfo = null;
        var memberInfo = null;
        if (rootRect.groupId>0) {
            groupInfo = theApp.groupQuickInfo(rootRect.groupId.toString(), false);
            memberInfo = theApp.memberInfo(rootRect.groupId.toString(),
                                               userId.toString());
            theViewLoader.source = "EbMemberInfoView.qml";
            theViewLoader.item.setMemberInfo(groupInfo, memberInfo);
            theViewLoadItem.visible = true;
        }
        else {
            var contactInfo = theApp.getContactInfoByUserId(userId.toString());
            if (contactInfo!=null) {
                /// 优先查看联系人资料
                theViewLoader.source = "EbContactInfoView.qml";
                theViewLoader.item.setContactInfo(contactInfo);
                theViewLoadItem.visible = true;
                return;
            }
            if (rootRect.connectedCallInfo.fromMemberId>0) {
                memberInfo = theApp.memberInfoByMemberId(rootRect.connectedCallInfo.fromMemberId.toString());
            }
            if (memberInfo==null) {
                memberInfo = theApp.defaultMemberInfo(userId.toString());
            }
            if (memberInfo!=null) {
                groupInfo = theApp.groupQuickInfo(memberInfo.groupId.toString(), false);
                theViewLoader.source = "EbMemberInfoView.qml";
                theViewLoader.item.setMemberInfo(groupInfo, memberInfo);
                theViewLoadItem.visible = true;
            }

        }
    }

    EbMenu {
        id: theTitleMenu
        EbMenuItem {
            id: addUserMenuItem
//            label: "邀请好友"
            label: {
//                return theLocales.getLocalText("chat-base-dialog.button-add-user.label", "Add user");
                if (rootRect.groupId>0) {
                    return theLocales.getLocalText("context-menu.add-member.text", "Add Member");
                }
                else {
                    var label = theLocales.getLocalText("context-menu.new-group.text", "New Group");
                    /// 9=临时讨论组
                    var groupTypeInfo = theLocales.getGroupTypeInfo(9);
                    return label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
                }
            }
            onTriggered: {
                theViewLoader.setSource("EbSearchView.qml",
                                        { "searchMode": 1,
                                        "searchFromGroupId": rootRect.groupId,
                                        "searchFromUserId": rootRect.fromUserId } );
                theViewLoadItem.viewType = 10;
                theViewLoadItem.visible = true;
            }
        }
        EbMenuSeparator {
            visible: addUserMenuItem.visible
        }
        EbMenuItem {
            id: groupMemberMenuItem
            visible: rootRect.groupId>0?true:false
//            label: "群组成员"
            onTriggered: {
                var groupInfo = theApp.groupQuickInfo(
                            rootRect.groupId.toString(),
                            false);
                if (groupInfo==null) {
                    return;
                }
                theViewLoader.source = "EbGroupMemberView.qml";
                theViewLoader.item.setGroupInfo(groupInfo);
                theViewLoadItem.visible = true;
            }
        }
        EbMenuSeparator {
            visible: groupMemberMenuItem.visible
        }
        EbMenuItem {
            id: groupShareMenuItem
            visible: (rootRect.groupId>0 && theApp.isExistSubscribeFunc(1002300105))?true:false
            label: "群共享"
            onTriggered: {
                var funcInfo = theApp.subscribeFuncInfo(1002300105);
                if (funcInfo==null) {
                    return;
                }
                theViewLoader.source = "EbWebBrowser.qml";
                theViewLoader.item.loadFuncInfo(funcInfo, rootRect.groupId, "");
                theViewLoadItem.visible = true;
            }
        }
        EbMenuSeparator {
            visible: groupShareMenuItem.visible
        }
        EbMenuItem {
            id: chatRecordMenuItem
            label: theLocales.getLocalText("chat-input-widget.button-msg-record.label","Chat record")
//            label: "消息记录"
            onTriggered: {
                var url = theApp.conversationsUrl(rootRect.groupId.toString(),
                                                  rootRect.fromUserId.toString());
                if (url.length==0) {
                    return;
                }
                webBrowserLoader.source = "EbWebBrowser.qml";
                webBrowserLoader.item.loadUrl(url, chatRecordMenuItem.label);
                webBrowserLoadItem.visible = true;
            }
        }

    }
    EbTitleBar {
        id: titleBar;
        width: parent.width
        iconVisible: true
        pressTitleEnable: true
//        title: "聊天会话"
        rightButton0Label: "\uf067";
        rightButton0Visible: true
        onReturnBack: {
            while (!requestReturnBack()) {
            }
            rootRect.returnBack();
        }
        onPressedTitle: {
            if (rootRect.groupId>0) {
                var groupInfo = theApp.groupQuickInfo(rootRect.groupId.toString(), true);
                if (groupInfo==null) {
                    return;
                }
                theViewLoader.source = "EbGroupInfoView.qml";
                theViewLoader.item.setGroupInfo(groupInfo);
                theViewLoadItem.visible = true;
            }
            else if (rootRect.fromUserId>0){
                openUserInfoView(rootRect.fromUserId);

            }
        }

        onPressedRightButton0: {
            theTitleMenu.x = parent.width-theTitleMenu.width-10;
            theTitleMenu.y = titleBar.height;
            theTitleMenu.open();
            theTitleMenu.parent = rootRect;
//            var groupInfo = theApp.groupQuickInfo(rootRect.groupId.toString(), false);
//            if (groupInfo==null) {
//                return;
//            }
//            theViewLoader.source = "EbGroupMemberView.qml";
//            theViewLoader.item.setGroupInfo(groupInfo);
//            theViewLoadItem.visible = true;
        }

    }

    ListModel {
        id: theRichModel
//        ListElement { name: "同学"; file: "zh-CN.json" }
    }
    Component {
        id: theRichDelegate

        Rectangle {
            id: listItem
            color: rootRect.color
            width: parent.width
            height: {
                if (richType==1)
                    return chatRichRect.height+userNameTimeText.height+10;
                else
                    return chatRichRect.height;
            }
//            height: 42
            Rectangle {
                id: tipRect
                visible: richType==2?true:false
                color: "#ddd"
                radius: 4
                x: (parent.width-width)/2
                width: tipText.contentWidth+12
                height: parent.height
                Text {
                    id: tipText
                    anchors.fill: parent
                    text: richType==2?messageTip:""
                    color: "#fff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 11
                }
            }

            EbImage {
                id: userHeadImage
                visible: richType==1?true:false
                x: {
                    if (richType==1 && richInfo.isReceive) {
                        return 5;
                    }
                    else {
                        return parent.width-width-5;
                    }
                }
                y: 6
                width: 30
                height: width
                fillMode: Image.PreserveAspectFit
                source: {
                    if (richType!=1) {
                        return "";
                    }

                    if (!richInfo.isReceive) {
                        return theApp.myHeadImage;
                    }
//                    else if (!richInfo.isGroupCall) {
//                        if (userHeadSource.length==0) {
//                            userHeadSource = theApp.userHeadImage(richInfo.sendFromUserId.toString());
//                        }
//                        return userHeadSource;
//                    }
                    else {
                        return theApp.userHeadImage(richInfo.sendFromUserId.toString());
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        if (richType!=1 || !richInfo.isReceive) {
                            return;
                        }
                        /// 打开用户资料
                        openUserInfoView(richInfo.sendFromUserId);
                    }

                }
            }

            Text {
                id: userNameTimeText
                visible: richType==1?true:false
                x: {
                    if (richType==1 && richInfo.isReceive) {
                        return userHeadImage.x + userHeadImage.width + 6;
                    }
                    else {
                        return 5;
                    }
                }
                y: userHeadImage.y
                width: parent.width-userHeadImage.width-x-12
//                visible: richInfo.isReceive && richInfo.isGroupCall
                height: (visible && text.length>0)?20:0
                text: {
                    if (richType!=1) {
                        return "";
                    }

                    if (richInfo.isGroupCall) {
                        if (richInfo.isReceive) {
                            richInfo.sendFromName + " " + richInfo.timeString
                        }
                        else {
                            return richInfo.timeString;
                        }
                    }
                    else {
                        return richInfo.timeString;
                    }
                }
                color: "#666"
                font.pointSize: 11
                horizontalAlignment: (richType==1 && richInfo.isReceive)?Text.AlignLeft:Text.AlignRight
            }

            Rectangle {
                id: chatRichRect
                visible: richType==1?true:false
                border.width: 1
                border.color: "#ddd"
                radius: 4
                /// 183,253,159=#b7fd9f
                /// 142,252,105=#8efc69 (深一点)
                color: (richType==1 && richInfo.isReceive)?"#fff":"#b7fd9f"
                x: {
                    if (richType==1 && richInfo.isReceive) {
                        return userHeadImage.x + userHeadImage.width + 6;
                    }
                    else {
                        return userHeadImage.x - width - 6;
                    }
                }
                y: userNameTimeText.height + 6
                width: chatRichText.contentWidth+chatRichText.x*2
                height: {
                    if (richType==2) {
                        return chatRichText.contentHeight +
                                chatRichText.y*2;
                    }
                    else {
                        return chatRichText.contentHeight+
                                chatRichText.y*2+
                                richLineRect.height+
                                fileProgressBarText.height+
                                chatFileDesc.height +
                                chatButtonRow.height;
                    }
                }

                /// 文本
                Text {
                    id: chatRichText
                    x: 8
                    y: 6
                    width: listItem.width-x*2-userHeadImage.width*2-6*2;
//                    height: 680
//                    horizontalAlignment: Text.AlignHCenter
//                    verticalAlignment: Text.AlignVCenter
                    text: richType==1?richInfo.richText:""
                    wrapMode: Text.Wrap
                    lineHeight: 1.2
                    onWidthChanged: {
                        if (richType==1) {
                            theApp.notifyMaxWidth(width);
                        }
                    }
                }
                Rectangle {
                    id: richLineRect
                    color: "#ddd"
                    y: chatRichText.y + chatRichText.contentHeight+2
                    width: parent.width
                    visible: (chatButtonRow.visible || (richType==1 && richInfo.richDesc.length>0))?true:false
//                    visible: (richInfo.msgType==1)?true:false
//                    visible: richInfo.progressValue>=0?true:false
                    height: visible?1:0
                }
                ProgressBar {
                    id: fileProgressBarText
                    visible: (richType==1 && richInfo.progressValue>0 && richInfo.progressValue<100)?true:false
                    y: richLineRect.y + richLineRect.height
                    width: parent.width
                    height: visible?22:0
                    from: 0
                    to: 100
                    value: (richType==1)?richInfo.progressValue:0
                }
                /// 描述（文件状态等）
                Text {
                    id: chatFileDesc
                    x: chatRichText.x
                    y: fileProgressBarText.y + fileProgressBarText.height
                    width: chatRichText.width
                    height: (richType==1 && richInfo.richDesc.length>0)?22:0
                    text: richType==1?richInfo.richDesc:""
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.Wrap
                    color: "#666"
                    font.pointSize: 12
//                    onWidthChanged: {
//                        theApp.notifyMaxWidth(width);
//                    }
                }
                Row {
                    id: chatButtonRow
                    y: chatFileDesc.y + chatFileDesc.height
                    width: chatRichText.contentWidth
                    visible: (richType==1 && richInfo.progressValue>=0 &&
                              richInfo.progressValue<100)?true:false
                    height: visible?28:0
                    topPadding: 3
                    leftPadding: 10
                    spacing: 20

                    EbButton {
                        id: cancelFileButton
                        width: 42
                        height: parent.height-parent.topPadding
                        visible: (richType==1 && !richInfo.isReceive &&
                                  richInfo.progressValue>=0 &&
                                  richInfo.progressValue<100)?true:false
                        label: theLocales.getLocalText("tran-file.button-cancel-send.text", "Cancel")
//                        label: "取消"
                        radius: 3
                        border.width: 0
                        textColor: "#fff"
                        buttonColor: theApp.mainColor
                        buttonHoverColor: theApp.mainHoverColor
                        buttonPressedColor: theApp.mainPressedColor
                        onClickedButton: {
                            theApp.cancelFile(rootRect.connectedCallInfo.callId.toString(),
                                              richInfo.msgId.toString());
                        }
                    }

                    EbButton {
                        id: rejectFileButton
                        width: 42
                        height: parent.height-parent.topPadding
                        visible: (richType==1 && richInfo.isReceive &&
                                  richInfo.progressValue>=0 &&
                                  richInfo.progressValue<100)?true:false
                        label: theLocales.getLocalText("tran-file.button-reject.text", "Reject")
//                        label: "拒绝"
                        radius: 3
                        border.width: 0
                        textColor: "#fff"
                        buttonColor: theApp.mainColor
                        buttonHoverColor: theApp.mainHoverColor
                        buttonPressedColor: theApp.mainPressedColor
                        onClickedButton: {
                            theApp.cancelFile(rootRect.connectedCallInfo.callId.toString(),
                                              richInfo.msgId.toString());
                        }
                    }
                    EbButton {
                        id: saveFileButton
                        visible: (richType==1 && richInfo.isReceive &&
                                  richInfo.progressValue>=0 &&
                                  richInfo.progressValue<100)?true:false
                        width: 40
                        height: parent.height-parent.topPadding
                        label: theLocales.getLocalText("tran-file.button-save.text", "Save")
//                        label: "保存"
                        radius: 3
                        border.width: 0
                        textColor: "#fff"
                        buttonColor: theApp.mainColor
                        buttonHoverColor: theApp.mainHoverColor
                        buttonPressedColor: theApp.mainPressedColor
                        onClickedButton: {
                            theApp.saveFile(rootRect.connectedCallInfo.callId.toString(),
                                            richInfo.msgId.toString(),
                                            richInfo.msgName);
                        }
                    }
                }
            }
            Text {
                id: checkText
                visible: (richType==1 && rootRect.groupId==0 && !richInfo.isReceive)?true:false
                x: chatRichRect.x - width
                y : chatRichRect.y + chatRichRect.height - height
                width: 20
                height: visible?width:0
                font.family: "FontAwesome"
                text: "\uf00c"
                /// 34,177,76=#22b14c
                color: (richType==1 && !richInfo.isGroupCall && richInfo.isRead)?"#22b14c":"red"
                font.pointSize: 12
            }

            EbTimerPress {
                id: delayPressTimer
                onPressed: {
                    /// msgType=1 file
                    /// msgType=2 image?
                    /// msgType=3 wav-voice?
                    /// msgType=4 个人名片
                    if (richInfo.msgType==4) {
                        var userECard = theApp.parserECardInfo(richInfo.msgName);
                        if (userECard!=null) {
                            theViewLoader.source = "EbUserECardInfoView.qml";
                            theViewLoader.item.setUserECardInfo(userECard);
                            theViewLoadItem.visible = true;
                        }

                    }
                    else if (richInfo.msgType>0 &&
                            richInfo.progressValue==-1) {
                        /// 默认程序，打开文件
//                        var url = "file://" + richInfo.msgName;
                        var url = "file:///" + richInfo.msgName;
                        if (richInfo.msgType==2 || theApp.isImageFile(url)) {
                            theViewLoader.setSource("EbHeadImageView.qml",
                                                    { "source": url,
                                                        "showOnly": true } );
                            theViewLoadItem.visible = true;
                        }
                        else if (richInfo.msgType==3) {
                            theApp.playWavFile(richInfo.msgName);
                        }
                        else {
                            Qt.openUrlExternally(url);
//                            Qt.resolvedUrl()
//                            theApp.openFile(url);
//                            Qt.openUrlExternally(encodeURIComponent(url));    /// ~
//                            theApp.openFile(richInfo.msgName);
//                            Qt.openUrlExternally(richInfo.msgName);
//                            Qt.openUrlExternally(theApp.getOpenFileUrl(richInfo.msgName));
                        }
                    }
                }
            }
            MouseArea {
                id: mouseArea
                x: richType==2?tipRect.x:chatRichRect.x
                width: richType==2?tipRect.width:chatRichRect.width
                height: {
                    if (richType==2)
                        return tipRect.height;
                    else if (richInfo.msgType==1 && chatButtonRow.visible)
                        return chatRichText.y + chatRichText.contentHeight;
                    else
                        return chatRichRect.y + chatRichRect.height;
//                    richType==2?tipRect.height:(chatRichText.y + chatRichText.contentHeight)
                }
//                height: richType==2?tipRect.height:(chatRichRect.y + chatRichRect.height)
//                height: chatButtonRow.y
//                anchors.fill: parent
                onPressed: {
                    if (richType==2)
                        tipRect.color = "#ccc";
                    else
                        chatRichRect.color = richInfo.isReceive?"#eee":"#8efc69";
                    delayPressTimer.initStart();
                }
                onReleased: {
                    if (richType==2)
                        tipRect.color = "#ddd";
                    else
                        chatRichRect.color = richInfo.isReceive?"#fff":"#b7fd9f"
                    delayPressTimer.pressedReleased = true;
                }
                onExited: {
                    if (richType==2)
                        tipRect.color = "#ddd";
                    else
                        chatRichRect.color = richInfo.isReceive?"#fff":"#b7fd9f"
                }
                onPressAndHold: {
                    mouse.accepted = true;
//                    console.log("*************** onPressAndHold, msgId=" + richInfo.msgId);
                    /// richInfo.msgId==0 也可以删除
                    var obj;
                    if (richType==2) {
                        theRichView.currentIndex = index;
                        hideAllMenuItem();
                        deleteMessageMenuItem.visible = true;
                        /// 计算弹出菜单位置
                        obj = mouseArea.mapToGlobal(mouse.x, mouse.y);
                        obj = rootRect.mapFromGlobal(obj.x, obj.y);
                        theChatMenu.x = obj.x-theChatMenu.width+5;
                        theChatMenu.y = obj.y-30;
                        theChatMenu.open();
                    }
                    else if (richInfo.msgId>0) {
                        theRichView.currentIndex = index;
                        hideAllMenuItem();
                        copyMessageMenuItem.visible = theApp.isCanCopyMessage(richInfo.msgId.toString());
                        forwardMessageMenuItem.visible = theApp.isCanForwardMessage(richInfo.msgId.toString());
                        deleteMessageMenuItem.visible = true;
                        collectMessageMenuItem.visible = theApp.isCanCollectMessage(richInfo.msgId.toString());
                        withdrawMessageMenuItem.visible = theApp.isCanWithdrawMessage(richInfo.msgId.toString());
                        /// 计算弹出菜单位置
                        obj = mouseArea.mapToGlobal(mouse.x, mouse.y);
                        obj = rootRect.mapFromGlobal(obj.x, obj.y);
                        theChatMenu.x = obj.x-theChatMenu.width+5;
                        theChatMenu.y = obj.y-30;
                        theChatMenu.open();
                    }
                }

            }
        }

    }
    ListView {
        id: theRichView
        y: titleBar.height
        width: parent.width
        height: inputRect.y-y;
//        height: parent.height-y-inputRect.height
        model: theRichModel
        delegate: theRichDelegate
    }

    Rectangle {
        id: inputRect
        y: parent.height-height-faceRect.height-plusRect.height
        width: parent.width
        height: 56
        /// true = 文件
        /// false = 语音
        property bool inputTextMode: true
        property bool showedEmotion: false
        property bool showedPlus: false
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbButton {
            id: textVoiceSwitchButton
            width: 36
            height: width
            x: (parent.height-1-height)/2
            y: x
            radius: width/2
            border.width: 1
            fontFamily: "FontAwesome"
            label: inputRect.inputTextMode?"\uf130":"\uf11c";
            fontPointSize: 14
            textColor: "#666"
            textPressedColor: theApp.mainColor
            textHoverColor: theApp.mainColor
            buttonColor: "transparent"
            buttonPressedColor: buttonColor
            borderColor: textColor
            borderPressedColor: theApp.mainColor
            borderHoverColor: theApp.mainColor
            onClickedButton: {
                inputRect.inputTextMode = !inputRect.inputTextMode
                if (!inputRect.inputTextMode) {
                    inputRect.showedEmotion = false;
                    inputRect.showedPlus = false;
                    inputTextArea.focus = false;
                }

//                if (theApp.isVoiceRecordStarted()) {
//                    theApp.voiceRecordStop();
//                }
//                else {
//                    theApp.voiceRecordStart();
//                }
            }
        }

//        ScrollView {
//            x: textVoiceSwitchButton.x + textVoiceSwitchButton.width + 2
//            width: faceButton.x - x
//            y: 8
//            height: parent.height - y*2

            EbButton {
                id: voiceRecorderButton
                visible: !inputRect.inputTextMode
                y: inputTextArea.y
                x: inputTextArea.x
                width: inputTextArea.width
                height: inputTextArea.height
                radius: 4
//                label: "按住说话"
                textColor: "#666"
//                textPressedColor: theApp.mainColor
//                textHoverColor: theApp.mainColor
                buttonColor: "transparent"
                buttonPressedColor: "#ddd"
                borderColor: "#aaa"
                borderPressedColor: "transparent"
                borderHoverColor: textColor
                EbSwipeArea {
                    id: mouse
                    anchors.fill: parent
                    threshold: 3    /// 2
                    onPressed: {
                        voiceRecorderButton.color = "#ddd";
                        if (theApp.voiceRecordStart()) {
                            theViewLoader.source = "EbVoiceRecordView.qml";
                            theViewLoadItem.visible = true;
                        }
                    }

                    onCanceled: {
                        voiceRecorderButton.color = "transparent";
                        theViewLoader.source = "";
                        theViewLoadItem.visible = false;
                        /// 发送语音
                        theApp.voiceRecordStop(rootRect.connectedCallInfo.callId.toString());
                    }

                    onSwipe: {
//                        console.log("********* onSwipe, direction=" + direction);
                        voiceRecorderButton.color = "transparent";
                        theViewLoader.source = "";
                        theViewLoadItem.visible = false;
                        switch (direction) {
                        case "up":
                            /// 取消发送
                            theApp.voiceRecordStop(0);
                            break
                        default:
                            /// 发送语音
                            theApp.voiceRecordStop(rootRect.connectedCallInfo.callId.toString());
                            break;
                        }
                    }
                }
            }

            TextArea {
                id: inputTextArea
                visible: inputRect.inputTextMode
//                anchors.fill: parent
                x: textVoiceSwitchButton.x + textVoiceSwitchButton.width + 4
                width: faceButton.x - x - 2
                y: 8
                height: parent.height - y*2
                selectByMouse: true
//                textFormat: TextEdit.AutoText
                textFormat: TextEdit.RichText
                wrapMode: TextEdit.Wrap
                focus: true
                leftPadding: 6
                rightPadding: 3
                bottomPadding: 2
                verticalAlignment: TextEdit.AlignVCenter
                Rectangle {
                    y: parent.height-1
                    x: 6
                    width: parent.width - x*2
                    height: 1
                    color: theApp.mainColor
                }
            }
//        }
        EbButton {
            id: faceButton
            width: textVoiceSwitchButton.width
            height: width
            x: plusButton.x - width - 6
            y: textVoiceSwitchButton.y
            radius: width/2
            border.width: 1
            fontFamily: "FontAwesome"
            label: inputRect.showedEmotion?"\uf11c":"\uf118";
            fontPointSize: textVoiceSwitchButton.fontPointSize
            textColor: "#666"
            textPressedColor: theApp.mainColor
            textHoverColor: theApp.mainColor
            buttonColor: "transparent"
            buttonPressedColor: buttonColor
            borderColor: textColor
            borderPressedColor: theApp.mainColor
            borderHoverColor: theApp.mainColor
            onClickedButton: {
                inputRect.showedEmotion = !inputRect.showedEmotion
                if (inputRect.showedEmotion) {
                    inputRect.showedPlus = false;
                    inputRect.inputTextMode = true;
                    inputTextArea.focus = false;
//                    Qt.inputMethod.hide();
                }
            }
        }
        EbButton {
            id: plusButton
            visible: inputTextArea.length==0?true:false
            width: textVoiceSwitchButton.width
            height: width
            x: parent.width-textVoiceSwitchButton.x-width-6
            y: textVoiceSwitchButton.y
            radius: width/2
            border.width: 1
            fontFamily: "FontAwesome"
            label: inputRect.showedPlus?"\uf068":"\uf067";
            fontPointSize: textVoiceSwitchButton.fontPointSize
            textColor: "#666"
            textPressedColor: theApp.mainColor
            textHoverColor: theApp.mainColor
            buttonColor: "transparent"
            buttonPressedColor: buttonColor
            borderColor: textColor
            borderPressedColor: theApp.mainColor
            borderHoverColor: theApp.mainColor
            onClickedButton: {
                inputRect.showedPlus = !inputRect.showedPlus
                if (inputRect.showedPlus) {
                    inputRect.showedEmotion = false;
                }
            }
        }
        EbButton {
            id: sendButton
            visible: inputTextArea.length>0?true:false
            width: 42
            height: 28
            x: plusButton.x
            y: (parent.height-height)/2
            radius: 2
            border.width: 0
//            label: "发送";
            textColor: "#fff"
            buttonColor: theApp.mainColor
            buttonPressedColor: theApp.mainPressedColor
            onClickedButton: {
                if (inputTextArea.length==0) {
                    inputTextArea.focus = true;
                    return;
                }
                else if (rootRect.connectedCallInfo==null) {
                    rootRect.connectedCallInfo = theApp.findCallInfo(rootRect.groupId.toString(),
                                                                     rootRect.fromUserId.toString(),
                                                                     rootRect.fromAccount);
                    if (rootRect.connectedCallInfo==null) {
                        doCallGroupUser();
                        return;
                    }
                }
                theApp.sendRich(rootRect.connectedCallInfo.callId.toString(),
                                inputTextArea.textDocument);
//                theApp.sendText(rootRect.connectedCallInfo.callId, inputTextArea.text);
                inputTextArea.clear();
                inputTextArea.focus = true;
            }
        }

    }
    Rectangle {
        id: faceRect
        y: parent.height-height
        width: parent.width
        height: visible?280:0
        visible: inputRect.showedEmotion
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }

        ListModel {
            id: theFaceModel
        }

        Component {
            id: theFaceDelegate
            Rectangle {
                id: listItem
                width: theGridView.cellWidth;
                height: theGridView.cellHeight
                color: "transparent"
                EbImage {
                    id: faceImage
                    y: (parent.height-height)/2
                    x: y
                    visible: deleteFace!=1?true:false
                    width: 24
                    height: width
                    source: deleteFace!=1?emotionInfo.resourceImageFile:""
                    fillMode: Image.PreserveAspectFit
                }
                Text {
                    anchors.fill: parent
//                    x: faceImage.x
//                    y: faceImage.y
//                    width: faceImage.width
//                    height: faceImage.height
                    visible: deleteFace==1?true:false
                    font.family: deleteFace==1?"FontAwesome":""
                    /// "\uf060"
                    text: deleteFace==1?"\uf177":"";
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "#666"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
//                    font.pointSize: addType==1?18:13
                }

//                property bool pressedReleased: false
//                Timer {
//                    id: delayPressTimer
//                    interval: 150
//                    onTriggered: {
//                        if (listItem.pressedReleased) {
//                            if (deleteFace==1) {
//                                /// 刪除前一个
//                                if (inputTextArea.cursorPosition>0) {
//                                    inputTextArea.remove(inputTextArea.cursorPosition-1, inputTextArea.cursorPosition)
//                                }
//                            }
//                            else {
//                                /// 插入表情
//                                var image = "<img src=\"" + emotionInfo.resourceImageFile +
//                                        "\" width=24 height=24></img>";
//                                inputTextArea.insert(inputTextArea.cursorPosition, image);
////                                theApp.insertEmotion(inputTextArea.textDocument, emotionInfo);
////                                var image = "<img src=\":/abc\" width=24 height=24></img>";
////                                var image = "<img src=\"image://abc\" width=24 height=24></img>";
////                                var image = "<img src=\"resource://abc\" width=24 height=24></img>";
////                                inputTextArea.insert(inputTextArea.cursorPosition, image);
//                            }
//                        }
//                    }
//                }
                EbTimerPress {
                    id: delayPressTimer
                    onPressed: {
                        if (deleteFace==1) {
                            /// 刪除前一个
                            if (inputTextArea.cursorPosition>0) {
                                inputTextArea.remove(inputTextArea.cursorPosition-1, inputTextArea.cursorPosition)
                            }
                        }
                        else {
                            /// 插入表情
                            var image = "<img src=\"" + emotionInfo.resourceImageFile +
                                    "\" width=24 height=24></img>";
                            inputTextArea.insert(inputTextArea.cursorPosition, image);
//                                theApp.insertEmotion(inputTextArea.textDocument, emotionInfo);
//                                var image = "<img src=\":/abc\" width=24 height=24></img>";
//                                var image = "<img src=\"image://abc\" width=24 height=24></img>";
//                                var image = "<img src=\"resource://abc\" width=24 height=24></img>";
//                                inputTextArea.insert(inputTextArea.cursorPosition, image);
                        }
                    }
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    onPressed: {
                        parent.color = "#eee";
                        delayPressTimer.initStart();
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

        GridView {
            id: theGridView
            anchors.fill: parent
//            leftMargin: 10
            topMargin: 10
            rightMargin: 10
            bottomMargin: 10
            cellWidth: 40
            cellHeight: 40
            model: theFaceModel
            flow: GridView.FlowTopToBottom
            delegate: theFaceDelegate
            //        highlight: Rectangle {
            //            color: "lightsteelblue";
            //            radius: 5
            //        }
            focus: true
        }

    }
    Rectangle {
        id: plusRect
        y: parent.height-height
        width: parent.width
        height: visible?100:0
        visible: inputRect.showedPlus
        color: "#fafafa"
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }

        ListModel {
            id: thePlusModel
        }

        Component {
            id: thePlusDelegate
            Rectangle {
                id: listItem
                width: thePlusGridView.cellWidth;
                height: thePlusGridView.cellHeight
                color: "transparent"
                Rectangle {
                    id: plusItemRect
                    radius: 6
                    width: 48
                    height: 48
                    x: (parent.width-width)/2
                    y: 15
                    border.width: 1
                    border.color: "#eee"
                    Text {
                        id: plusIconText
                        anchors.fill: parent
                        font.family: "FontAwesome"
                        text: plusIcon
                        color: "#666"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pointSize: 22
                    }
                }
                Text {
                    id: plusText
                    y: plusItemRect.y + plusItemRect.height + 6
                    height: parent.height-y
                    width: parent.width
                    text: plusLabel
                    color: "#666"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignTop
                    font.pointSize: 11
                }

                EbTimerPress {
                    id: delayPressTimer
                    onPressed: {
                        switch (plusType)
                        {
                        case 1:
                            /// 拍摄
                            theViewLoader.source = "EbMyVideoHead.qml";
                            theViewLoadItem.viewType = 1;
                            theViewLoadItem.visible = true;
                            break;
                        case 2:
                            /// 相册
                            theApp.openGallery(1);
                            /// for test
//                                var imageFile = "file:///D:/test.png";
//                                theApp.sendImage(rootRect.connectedCallInfo.callId.toString(), imageFile);
                            break;
                        case 3:
                            /// 文件
                            theApp.openGallery(2);
                            /// for test
//                                var imageFile = "file:///E:/ActivePerl-5.22.0.2200-MSWin32-x64-299195.msi";
//                                var imageFile = "file:///D:/ebtw.zip";
//                                var imageFile = "file:///D:/test.png";
//                                theApp.sendFile(
//                                    rootRect.connectedCallInfo.callId.toString(),
//                                    imageFile);
                            break;
                        case 4:
                            /// 名片
                            theViewLoader.setSource("EbSearchView.qml",
                                                    { "searchMode": 2,
                                                        "searchFromUserId": rootRect.fromUserId } );
                            theViewLoadItem.viewType = 11;
                            theViewLoadItem.visible = true;
                            break;
                        default:
                            break;
                        }
                    }
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    onPressed: {
                        parent.color = "#eee";
                        delayPressTimer.initStart();
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

        GridView {
            id: thePlusGridView
            anchors.fill: parent
//            topMargin: 10
//            rightMargin: 10
//            bottomMargin: 10
            cellWidth: 80
            cellHeight: 80
            model: thePlusModel
            delegate: thePlusDelegate
        }

    }

    function findMsgRichInfoFromModelData(msgId) {
        for (var i=0; i<theRichModel.count; i++) {
            var obj = theRichModel.get(i);
            /// **必须放在最前面
            if (obj.richType!=1) {
                continue;
            }
            if (obj.richInfo==null) {
                return null;
            }

            if (obj.richInfo.msgId==msgId) {
                return obj.richInfo;
            }
        }
        return null;
    }
    function existMsgRichInfoFromModelData(richInfo, updateMsgId) {
        for (var i=0; i<theRichModel.count; i++) {
            var obj = theRichModel.get(i);
            /// **必须放在最前面
            if (obj.richType!=1) {
                continue;
            }
            if (obj.richInfo==null) {
                return false;
            }

            if (obj.richInfo.msgId==richInfo.msgId ||
                    (updateMsgId>0 && updateMsgId==obj.richInfo.msgId)) {
                obj.richInfo.msgId = richInfo.msgId;
                obj.richInfo.richText = richInfo.richText;
                return true;
            }
        }
        return false;
    }
    function setReceiptDataModelData(msgId, fromUserId, ackType) {
        /// nAckType=0 对方收到消息
        /// nAckType=4 请求撤回消息
        /// nAckType=6 请求“个人收藏”消息
        /// nAckType=7 请求“群收藏”消息
        var richInfo = findMsgRichInfoFromModelData(msgId);
        if (richInfo==null) {
            return;
        }
        if (ackType==0) {
            richInfo.isRead = true;
        }
        else if (ackType==4) {
            /// [撤回一条消息]
            richInfo.richText = theLocales.getLocalText("withdraw-msg-response.state-ok-update","");
        }

//        for (var i=0; i<theRichModel.count; i++) {
//            var obj = theRichModel.get(i);
//            /// **必须放在最前面
//            if (obj.richType!=1) {
//                continue;
//            }
//            if (obj.richInfo==null) {
//                return;
//            }

//            if (obj.richInfo.msgId==msgId) {
//                if (ackType==0) {
//                    obj.richInfo.isRead = true
//                }
//                else if (ackType==1) {
//                    /// ???
//                }

//                return;
//            }
//        }
    }

    function setEmotionInfoModelData(emotionInfo) {
        for (var i=0; i<theFaceModel.count; i++) {
            var obj = theFaceModel.get(i);
            if (obj.emotionInfo==null) {
                return;
            }

            if (obj.emotionInfo.resourceId==emotionInfo.resourceId) {
                return;
            }
        }
        loadMyEmotionTimer.loadEmptyCount = 0;
        /// 前面找不到，直接添加
        theFaceModel.append( { deleteFace: 0, emotionInfo: emotionInfo } );
    }


    Connections {
        target: theApp
        onDeletedGroupId: {
            if (groupId==rootRect.groupId) {
                while (!requestReturnBack()) {
                }
                rootRect.returnBack();
            }
        }

        /// 以下三段代码，完全一样，计算群组名称 (%d)
        onMemberRemoveGroup: {
            if (rootRect.groupId==0 || memberInfo.groupId!=rootRect.groupId) {
                return;
            }
            rootRect.fromName = theApp.groupFullName(rootRect.groupId);
            titleBar.title = rootRect.fromName;
        }
        onMemberExitGroup: {
            if (rootRect.groupId==0 || memberInfo.groupId!=rootRect.groupId) {
                return;
            }
            rootRect.fromName = theApp.groupFullName(rootRect.groupId);
            titleBar.title = rootRect.fromName;
        }
        onReceivedMemberInfo: {
            if (rootRect.groupId==0 || memberInfo.groupId!=rootRect.groupId) {
                return;
            }
            rootRect.fromName = theApp.groupFullName(rootRect.groupId);
            titleBar.title = rootRect.fromName;
        }

        onOpenGalleryResponse: {
//            theRichModel.append( { richType: 2, messageTip: uriFile, richInfo: null } );
            if (parameter==1) {
                theViewLoader.setSource("EbHeadImageView.qml",
                                        {"source": uriFile} );
                theViewLoadItem.viewType = parameter;
//                theViewLoadItem.viewType = 1;
                theViewLoadItem.visible = true;
            }
            else {
                /// 直接发送文件
                if (rootRect.groupId==0)
                    theApp.sendFile(rootRect.connectedCallInfo.callId.toString(), uriFile);
                else
                    theApp.sendGroupTempFile(rootRect.groupId.toString(), uriFile);
            }
        }
        onReceivedMsgReceiptData: {
            if (callId!=rootRect.connectedCallInfo.callId) {
                return;
            }
            var messageTip = "";
            if (stateCode==0) {
                setReceiptDataModelData(msgId, fromUserId, ackType);
            }
            else {
            }
            if (ackType==6 ||	/// 个人收藏
                    ackType==7) {			/// 群收藏
                if (stateCode==0) {
                    /// 消息收藏成功！
                    messageTip = theLocales.getLocalText("message-show.collect-msg-ok","");
                }
                else if (stateCode==2) {
                    /// 没有操作权限：\r\n消息收藏失败！
                    messageTip = theLocales.getLocalText("message-show.collect-msg-not-auth","");
                }
                else {
                    /// 消息不存在或者已经被删除：\r\n消息收藏失败！
                    messageTip = theLocales.getLocalText("message-show.collect-msg-error","");
                }
                if (messageTip.length>0) {
                    messageTip = messageTip.replace("[STATE_CODE]", stateCode);
                    theRichModel.append( { richType: 2, messageTip: messageTip, richInfo: null } );
                    theRichView.currentIndex = theRichModel.count-1;
                }
            }
            else if (ackType==4) {
                /// 请求撤回消息
                if (fromUserId==theApp.logonUserId) {
                    if (stateCode!=0) {
                        /// 发送时间超过2分钟的消息：<br>不能撤回！
                        messageTip = theLocales.getLocalText("withdraw-msg-response.other-error","");
                        if (messageTip.length>0 ) {
                            messageTip = messageTip.replace("[STATE_CODE]", stateCode);
                            theRichModel.append( { richType: 2, messageTip: messageTip, richInfo: null } );
                            theRichView.currentIndex = theRichModel.count-1;
                        }
                    }
                    else {
                        /// 请求撤回了一条消息！
                        messageTip = theLocales.getLocalText("withdraw-msg-response.state-ok", "");
                        if (messageTip.length>0) {
                            theRichModel.append( { richType: 2, messageTip: messageTip, richInfo: null } );
//                            theRichView.currentIndex = theRichModel.coun-1;
                        }
                    }
                }
                else if (rootRect.groupId==0) {
                    /// 对方撤回了一条消息！
                    messageTip = theLocales.getLocalText("withdraw-msg-response.msg-receipt-dest","");
                    if (messageTip.length>0) {
                        theRichModel.append( { richType: 2, messageTip: messageTip, richInfo: null } );
                    }
                }
                else {
                    /// %s 撤回了一条消息！
                    var memberUserName = theApp.userName(rootRect.groupId.toString(),
                                                          fromUserId.toString());
                    messageTip = theLocales.getLocalText("withdraw-msg-response.msg-receipt-group","");
                    if (messageTip.length>0) {
                        messageTip = messageTip.replace("[USER_NAME]", memberUserName);
                        theRichModel.append( { richType: 2, messageTip: messageTip, richInfo: null } );
                    }
                }
            }

        }
        onDisplayMessageTip: {
            if (!isCurrentChatInfo(groupId, fromUserId)) {
                return;
            }
            theRichModel.append( { richType: 2, messageTip: messageTip, richInfo: null } );
        }
        onFilePercent: {
            if (!isCurrentChatInfo(groupId, fromUserId)) {
                return;
            }
            var richInfo = findMsgRichInfoFromModelData(msgId);
            if (richInfo==null) {
                return;
            }
            richInfo.progressValue = percent;
            richInfo.richDesc = richDesc;
        }
        onFileReceived: {
            if (!isCurrentChatInfo(groupId, fromUserId)) {
                return;
            }
            var richInfo = findMsgRichInfoFromModelData(msgId);
            if (richInfo==null) {
                return;
            }
            richInfo.progressValue = -1;
            /// 成功接收文件
            richInfo.msgName = filePath;
            richInfo.richText = richText;
            richInfo.richDesc = theLocales.getLocalText("chat-msg-text.receive-file-ok", "Receive file ok");
        }

        onFileSent: {
            if (!isCurrentChatInfo(groupId, fromUserId)) {
                return;
            }
            var richInfo = findMsgRichInfoFromModelData(msgId);
            if (richInfo==null) {
                return;
            }
            richInfo.progressValue = -1;
            richInfo.isRead = true;
            /// 成功发送文件
            richInfo.richDesc = theLocales.getLocalText("chat-msg-text.send-file-ok", "Send file ok");
        }
        onFileCancel: {
            if (!isCurrentChatInfo(groupId, fromUserId)) {
                return;
            }
            var richInfo = findMsgRichInfoFromModelData(msgId);
            if (richInfo==null) {
                return;
            }
            richInfo.progressValue = -1;
            if (fromUserId==0 || fromUserId==theApp.logonUserId) {
                /// 取消发送文件
                richInfo.richDesc = theLocales.getLocalText("chat-msg-text.cancel-file", "Canceel file");
            }
            else {
                /// 对方拒绝接收文件
                richInfo.richDesc = theLocales.getLocalText("chat-msg-text.dest-reject-file", "Dest jrect file");
            }
        }
        onReceivedRichInfo: {
//            if (richInfo.callId!=rootRect.connectedCallInfo.callId) {
//                return;
//            }
            if ( (richInfo.isGroupCall && richInfo.groupId==rootRect.groupId) ||
                    (rootRect.groupId==0 && !richInfo.isGroupCall &&
                     (richInfo.sendFromUserId==rootRect.fromUserId ||
                      richInfo.sendFromUserId==theApp.logonUserId) )) {
                if (!rootRect.inited || !existMsgRichInfoFromModelData(richInfo, updateMsgId)) {
                    theRichModel.append( { richType: 1, messageTip: "", richInfo: richInfo } );
                    if (rootRect.inited) {
                        /// 移到最后面
                        theRichView.currentIndex = theRichModel.count-1;
                    }
                }
            }
        }

        onLoadMyEmotionInfoResponse: {
//            console.log("************* onLoadMyEmotionInfoResponse, " + emotionInfo.description);
            setEmotionInfoModelData(emotionInfo);
        }

        onReceivedCallConnected: {
            if (rootRect.groupId==0 && callInfo.groupId==0 &&
                    rootRect.fromUserId==0 &&
                    rootRect.fromAccount.length==0 &&
                    rootRect.fromAccount==callInfo.fromAccount) {
                rootRect.fromUserId = callInfo.fromUserId;
                theApp.currentChatUserId = rootRect.fromUserId;
                /// 加载历史聊天记录
                theApp.loadRichInfo(rootRect.groupId.toString(),
                                    rootRect.fromUserId.toString());
                if (theRichModel.count>0) {
                    theRichView.currentIndex = theRichModel.count-1;
                }
                if (rootRect.fromName.length==0) {
                    theApp.callUserId(rootRect.fromUserId.toString(), 0);
                    rootRect.fromName = theApp.userNameByUserId(rootRect.fromUserId);
                    titleBar.title = rootRect.fromName;
                }
            }
//            console.log("************* onReceivedCallConnected, callid=" + callInfo.callId);
            if ((callInfo.isGroupCall && callInfo.groupId==rootRect.groupId) ||
                    (!callInfo.isGroupCall && rootRect.groupId==0 &&
                     callInfo.fromUserId==rootRect.fromUserId)) {
                rootRect.connectedCallInfo = callInfo;
                if (rootRect.groupId==0 && rootRect.fromName==rootRect.fromUserId) {
                    /// 获取不到用户相关信息，使用 call-info 内容显示；
//                    console.log("************* onReceivedCallConnected, rootRect.fromUserId=" + rootRect.fromUserId);
//                    console.log("************* onReceivedCallConnected, rootRect.fromName=" + rootRect.fromName);
                    rootRect.fromName = theApp.userNameByUserId(rootRect.fromUserId, callInfo.callId);
                    titleBar.title = rootRect.fromName;
                }
//                console.log("************* callConnected = true, callId=" + rootRect.connectedCallInfo.callId);
            }
        }
    }
    function updateLocaleInfo() {
//        titleBar.title = theLocales.getLocalText("main-frame.mid-button-my-app.tooltip", "Applications");
        sendButton.label = theLocales.getLocalText("chat-input-widget.button-send.label", "Send");
        voiceRecorderButton.label = theLocales.getLocalText("chat-input-widget.press-speech.label", "Press speech");
    }

    Timer {
        id: loadMyEmotionTimer
        interval: 3000
        repeat: true
        triggeredOnStart: true
        property int loadEmptyCount: 0
        onTriggered: {
            loadMyEmotionTimer.loadEmptyCount = loadMyEmotionTimer.loadEmptyCount+1;
            theApp.loadMyEmotionInfo(0);
            if (loadMyEmotionTimer.loadEmptyCount>3) {
                theFaceModel.append( { deleteFace: 1, emotionInfo: null } );
                stop();
            }
        }
    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            if (requestReturnBack()) {
                rootRect.returnBack();
            }
        }
    }

    function doCallGroupUser() {
        if (rootRect.groupId>0) {
            theApp.callGroupId(rootRect.groupId.toString());
        }
        else if (rootRect.fromUserId>0){
            theApp.callUserId(rootRect.fromUserId.toString(), 0);
        }
        else if (rootRect.fromAccount.length>0){
            theApp.callAccount(rootRect.fromAccount, 0);
        }
    }

    property bool inited: false
    Component.onCompleted: {
        /// ** 初始化
        theApp.notifyMaxWidth(parent.width/2);
        var docRoot = rootRect.parent;
        while (docRoot.parent) {
            docRoot = docRoot.parent;
        }
        theKeyFilter.setFilter(docRoot, 1);
        updateLocaleInfo();
        /// groupType=9（临时讨论组）
        addUserMenuItem.visible = (rootRect.groupId==0 ||
                                   theApp.groupType(rootRect.groupId)==9 ||
                                   theApp.isGroupAdminLevel(rootRect.groupId))?true:false;
        loadMyEmotionTimer.start();
        /// * 必须放在 loadRichInfo 前面
        theApp.currentChatGroupId = rootRect.groupId;
        theApp.currentChatUserId = rootRect.fromUserId;
        /// 加载历史聊天记录
        theApp.loadRichInfo(rootRect.groupId.toString(),
                            rootRect.fromUserId.toString());
        if (theRichModel.count>0) {
            theRichView.currentIndex = theRichModel.count-1;
        }
        /// 清空未读消息
        theApp.clearUnreadCount(rootRect.groupId.toString(), rootRect.fromUserId.toString());

        if (rootRect.fromName.length==0) {
            if (rootRect.groupId>0) {
                rootRect.fromName = theApp.groupFullName(rootRect.groupId.toString());
//                rootRect.fromName = theApp.groupName(rootRect.groupId.toString());
                var groupType = theApp.groupType(rootRect.groupId.toString());
                var groupTypeInfo = theLocales.getGroupTypeInfo(groupType);
                groupMemberMenuItem.label = groupTypeInfo.groupTypeMember;
            }
            else if (rootRect.fromUserId>0){
                rootRect.fromName = theApp.userNameByUserId(rootRect.fromUserId);
            }
            else if (rootRect.fromAccount.length>0){
                rootRect.fromName = theApp.userNameByAccount(rootRect.fromAccount);
            }
        }
        titleBar.title = rootRect.fromName;
        rootRect.inited = true;
        doCallGroupUser();
        /// 拍摄
        var label = theLocales.getLocalText("name-text.shoot","Shoot");
        thePlusModel.append( { plusType: 1, plusIcon: "\uf030", plusLabel: label } );
        /// 相册
        label = theLocales.getLocalText("name-text.album","Album");
        thePlusModel.append( { plusType: 2, plusIcon: "\uf1c5", plusLabel: label } );
        /// ** 暂时不支持发送群组文件
        if (rootRect.groupId==0) {
            /// \uf15b
            /// 文件
            label = theLocales.getLocalText("name-text.file","File");
            thePlusModel.append( { plusType: 3, plusIcon: "\uf016", plusLabel: label } );
        }
        label = theLocales.getLocalText("name-text.ecard","ECard");
        thePlusModel.append( { plusType: 4, plusIcon: "\uf007", plusLabel: label } );
    }

}

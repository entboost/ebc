import QtQuick 2.0
import "js/ebbase.js" as Ebbasejs

EbPage {
    id: rootPage

    signal requestAddContact();
    signal requestSearch();
    signal requestDeleteChat(var sessionInfo);
    signal pressedFuncInfo(var funcInfo, string parameter);
    signal requestOpenGroupSession(var groupId);
    signal requestOpenUserSession(var userId);
    signal unreadCout(var count);

    function loadSessionData() {
        /// 加载
        theModel.clear();
        loadSessionDataTimer.start();
//        theApp.loadSessionInfo(50, 0);
    }
    function deleteCurrentIndex() {
        var obj = theModel.get(listView.currentIndex);
        if (obj==null) {
            return;
        }
        var unreadCount = obj.sessionInfo.unreadCount;
        theModel.remove(listView.currentIndex);
        if (unreadCount>0) {
            rootPage.unreadCout(getUnreadCount());
        }
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
//            title: "最近会话"
            rightButton1Visible: true
            rightButton1Label: "\uf002"
            onPressedRightButton1: {
                requestSearch();
            }
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

//    Item {
//        id: theLoadItem
//        anchors.fill: parent
//        visible: false
//        z: 100
//        Loader {
//            id: theLoader
//            anchors.fill: parent
//        }
//        Connections {
//            target: theLoader.item
//            onReturnBack: {
//                theLoader.source = "";
//                theLoader.visible = false;
//            }
//        }
//    }

    EbMenu {
        id: theMenu
        EbMenuItem {
            id: deleteChatMenuItem
//            label: "删除聊天"
            onTriggered: {
                var obj = theModel.get(listView.currentIndex);
                if (obj==null) {
                    return;
                }
                rootPage.requestDeleteChat(obj.sessionInfo);
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
            height: 52

            Text {
                id: iconText
                anchors.fill: fromHeadImage
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.family: "FontAwesome"
                color: "#999"
                font.pointSize: 16
                visible: sessionInfo.isMsgSession?true:false
                text: {
                    return Ebbasejs.funcIconTextBySubscribeId(1002300103);
//                    switch (sessionInfo.fromType) {
//                    case 10:
//                    case 1:
//                    case 2:
//                    case 3:
//                    case 5:
//                        /// 10=广播消息\n%s
//                        /// 1=验证消息\n%s申请加入群组%lld
//                        /// 2=验证消息\n%s邀请加入群组%lld
//                        /// 3=验证消息\n%s申请加为好友
//                        /// 5=通知消息\n%s拒绝加为好友
//                    //case EBC_MSG_TYPE_ACCERPT_ADD_CONTACT:
//                    //	sItemText.Format(_T("通知消息\n%s成功加为好友"),pCallRecordInfo->m_sFromAccount.c_str());
//                    //	break;
//                    default:
//                        /// 验证消息\n帐号：%s 群组：%lld
//                        sItemText = theLocales.getLocalText("pop-tip-dialog.type-group-message", "Group message");
//                        sItemText = sItemText.replace("[USER_ACCOUNT]", sessionInfo.fromAccount);
//                        sItemText = sItemText.replace("[GROUP_ID]", sessionInfo.groupId);
//                        break;
//                    }
                }
            }
            EbImage {
                id: fromHeadImage
                width: 42
                height: width
                y: (parent.height-height)/2;
                x: y
//                width: parent.height
//                height: parent.height-1
                fillMode: Image.PreserveAspectFit
                visible: !iconText.visible
                source: {
                    if (sessionInfo.fromHeadImage.length>0) {
                        return sessionInfo.fromHeadImage;
                    }
                    if (sessionInfo.isMsgSession) {
                        /// 消息通知
                        return "";
                    }
                    else if (sessionInfo.isGroupCall) {
                        /// 群组聊天
                        return theApp.groupImage(sessionInfo.groupId.toString());
                    }
                    else {
                        /// 用户会话
                        return theApp.userHeadImage(sessionInfo.fromUserId.toString());
                    }
                }
            }
            Rectangle {
                id: unreadRect
                z: 1
                visible: ((sessionInfo.isMsgSession && !sessionInfo.isRead) ||
                         sessionInfo.unreadCount>0)?true:false
                width: sessionInfo.isMsgSession?12:18
                height: width
                color: "red"
                radius: width/2
                x: fromHeadImage.x + fromHeadImage.width - width
                y: 2
                Text {
                    anchors.centerIn: parent
                    color: "#fff"
                    text: sessionInfo.isMsgSession?"":sessionInfo.unreadCount
                    font.pointSize: 12
                }
            }

            Text {
                id: nameText
                x: fromHeadImage.x + fromHeadImage.width+5
                width: parent.width-x-timeText.width
                height: sessionInfo.isMsgSession?(parent.height-1):(parent.height/2)
                verticalAlignment: sessionInfo.isMsgSession?Text.AlignVCenter:Text.AlignBottom
                lineHeight: sessionInfo.isMsgSession?1.2:1.1
                text: {
                    if (sessionInfo.isMsgSession) {
                        var sItemText;
                        switch (sessionInfo.fromType) {
                        case 10:
                            /// 广播消息\n%s
                            sItemText = theLocales.getLocalText("pop-tip-dialog.type-bc-message", "Broadcast Message");
                            sItemText = sItemText.replace("[MESSAGE_TITLE]", sessionInfo.groupName);
                            break;
                        case 1:
                            /// 验证消息\n%s申请加入群组%lld
                            sItemText = theLocales.getLocalText("pop-tip-dialog.type-request-join-to-group", "Request join to group");
                            sItemText = sItemText.replace("[USER_ACCOUNT]", sessionInfo.fromAccount);
                            sItemText = sItemText.replace("[GROUP_ID]", sessionInfo.groupId);
                            break;
                        case 2:
                            /// 验证消息\n%s邀请加入群组%lld
                            sItemText = theLocales.getLocalText("pop-tip-dialog.type-invite-join-to-group", "Invite join to group");
                            sItemText = sItemText.replace("[USER_ACCOUNT]", sessionInfo.fromAccount);
                            sItemText = sItemText.replace("[GROUP_ID]", sessionInfo.groupId);
                            break;
                        case 3:
                            /// 验证消息\n%s申请加为好友
                            sItemText = theLocales.getLocalText("pop-tip-dialog.type-request-add-contact", "Request add contact");
                            sItemText = sItemText.replace("[USER_ACCOUNT]", sessionInfo.fromAccount);
                            break;
                        //case EBC_MSG_TYPE_ACCERPT_ADD_CONTACT:
                        //	sItemText.Format(_T("通知消息\n%s成功加为好友"),pCallRecordInfo->m_sFromAccount.c_str());
                        //	break;
                        case 5:
                            /// 通知消息\n%s拒绝加为好友
                            sItemText = theLocales.getLocalText("pop-tip-dialog.type-reject-add-contact", "Reject add contact");
                            sItemText = sItemText.replace("[USER_ACCOUNT]", sessionInfo.fromAccount);
                            break;
                        default:
                            /// 验证消息\n帐号：%s 群组：%lld
                            sItemText = theLocales.getLocalText("pop-tip-dialog.type-group-message", "Group message");
                            sItemText = sItemText.replace("[USER_ACCOUNT]", sessionInfo.fromAccount);
                            sItemText = sItemText.replace("[GROUP_ID]", sessionInfo.groupId);
                            break;
                        }
                        return sItemText;
                    }
                    else if (sessionInfo.isGroupCall)
                        return sessionInfo.groupFullName;
                    else
                        return sessionInfo.fromName;
                }
            }
            Text {
                id: infoText
                x: nameText.x
                y: nameText.height
                width: nameText.width
                height: sessionInfo.isMsgSession?0:(parent.height/2-1)
                verticalAlignment: Text.AlignTop
                text: sessionInfo.lastInfo
                color: "#666"
                font.pointSize: 12
            }
            Text {
                id: timeText
                x: parent.width-width-8    /// 8 是右边间隔
                y: 5
                width: 80
                height: parent.height-y-1
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignTop
                font.pointSize: 12
                text: sessionInfo.timeString
                color: "#666"
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
                onPressed: {
                    if (sessionInfo.isMsgSession) {
                        /// 消息通知，打开消息
                        var funcInfo = theApp.subscribeFuncInfo(1002300103);
                        if (funcInfo==null) {
                            return;
                        }
                        if (!sessionInfo.isRead) {
                            sessionInfo.isRead = true;
                            theApp.setSessionIsRead(sessionInfo.callId.toString());
                        }

                        switch (sessionInfo.fromType) {
                        case 10:
                            pressedFuncInfo(funcInfo, "tab_type=bc_msg");
                            break;
//                        case 1:
//                        case 2:
//                        case 3:
//                        case 5:
//                            /// 10=广播消息\n%s
//                            /// 1=验证消息\n%s申请加入群组%lld
//                            /// 2=验证消息\n%s邀请加入群组%lld
//                            /// 3=验证消息\n%s申请加为好友
//                            /// 5=通知消息\n%s拒绝加为好友
//                            //case EBC_MSG_TYPE_ACCERPT_ADD_CONTACT:
//                            /// other=验证消息\n帐号：%s 群组：%lld
                        default:
                            pressedFuncInfo(funcInfo, "tab_type=sys_msg");
                            break;
                        }
                    }
                    else if (sessionInfo.isGroupCall) {
                        /// 群组聊天
                        rootPage.requestOpenGroupSession(sessionInfo.groupId);
                    }
                    else {
                        /// 用户会话
                        rootPage.requestOpenUserSession(sessionInfo.fromUserId);
                    }
                }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
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
                onPressAndHold: {
                    mouse.accepted = true;
                    listView.currentIndex = index;
                    /// 计算弹出菜单位置
                    var obj = mouseArea.mapToGlobal(mouse.x, mouse.y);
                    obj = listView.mapFromGlobal(obj.x, obj.y);
                    theMenu.x = obj.x-theMenu.width+5;
                    theMenu.y = obj.y-30;
                    theMenu.open();
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

    function checkSameSessionInfo(sessionInfo, isMsgSession, groupId, userId, msgType) {
        if (sessionInfo.isMsgSession) {
            /// 验证消息
            if (isMsgSession && sessionInfo.fromType==msgType) {
                return true;
            }
        }
        else if (!isMsgSession && groupId>0) {
            /// 查询群组
            if (sessionInfo.groupId==groupId) {
                return true;
            }
        }
        else if (!isMsgSession) {
            /// 查询帐号
            if (!sessionInfo.isGroupCall && sessionInfo.fromUserId==userId)
            {
                return true;
            }
        }
        return false;
    }

    function findSessionInfo(isMsgSession, groupId, fromUserId, msgType) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.sessionInfo==null) {
                return null;
            }

            if (obj.sessionInfo.isMsgSession) {
                /// 验证消息
                if (isMsgSession && msgType>0) {
                    if ((groupId>0 && groupId==obj.sessionInfo.groupId) ||
                        (fromUserId>0 && fromUserId==obj.sessionInfo.fromUserId) ||
                        (msgType==10 && obj.sessionInfo.fromType==10)) {
                        /// 10=EBC_MSG_TYPE_BC_MSG
                        /// 消息广播只显示当前一条
                        return obj.sessionInfo;
                    }
                }
                continue;
            }
            else if (!isMsgSession && groupId>0 && obj.sessionInfo.groupId==groupId) {
                /// 查询群组
                return obj.sessionInfo;
            }
            else if (!isMsgSession && groupId==0 && obj.sessionInfo.groupId==0 &&
                     (fromUserId==obj.sessionInfo.fromUserId ||
                      fromUserId==theApp.logonUserId)) {
                return obj.sessionInfo;
            }
        }
        return null;
    }
    function deleteSessionInfo(groupId, fromUserId, msgType) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.sessionInfo==null) {
                return null;
            }

            if (obj.sessionInfo.isMsgSession &&
                    obj.sessionInfo.fromType>0) {
//                /// 验证消息
//                if (msgType>0) {
//                    if ((groupId>0 && groupId==obj.sessionInfo.groupId) ||
//                            (fromUserId>0 && fromUserId==obj.sessionInfo.fromUserId) ||
//                            (msgType==10 && obj.sessionInfo.fromType==10)) {
//                        /// 10=EBC_MSG_TYPE_BC_MSG
//                        /// 消息广播只显示当前一条
//                        return obj.sessionInfo;
//                    }
//                }
                continue;
            }
            else if (groupId>0 && obj.sessionInfo.groupId==groupId) {
                theModel.remove(i);
                return true;
            }
            else if (groupId==0 && obj.sessionInfo.groupId==0 &&
                     (fromUserId==obj.sessionInfo.fromUserId ||
                      fromUserId==theApp.logonUserId)) {
                theModel.remove(i);
                return true;
            }
        }
        return false;
    }

    function setSessionInfoModelData(sessionInfo) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.sessionInfo==null) {
                return;
            }

            if (obj.sessionInfo.time<sessionInfo.time) {
                /// 时间较新，插入到前面
                theModel.insert(i, { sessionInfo: sessionInfo } );
                /// 查找后面存在记录，删除
//                var findRemove = false;
                for (var j=i+1 ; j<theModel.count; j++) {
                    obj = theModel.get(j);
                    if (obj.sessionInfo==null) {
                        break;
                    }
                    if (checkSameSessionInfo(
                                obj.sessionInfo,
                                sessionInfo.isMsgSession,
                                sessionInfo.groupId,
                                sessionInfo.fromUserId,
                                sessionInfo.fromType)) {
                        theModel.remove(j);
//                        findRemove = true;
                        break;
                    }
                }
                return;
            }
            else if (checkSameSessionInfo(
                         obj.sessionInfo,
                         sessionInfo.isMsgSession,
                         sessionInfo.groupId,
                         sessionInfo.fromUserId,
                         sessionInfo.fromType)) {

//                console.log("************, setSessionInfoModelData, checkSameSessionInfo ok, i=" + i);
//                console.log("************, setSessionInfoModelData, checkSameSessionInfo ok, fromName=" + sessionInfo.fromName);

                /// 更新记录
                if (obj.sessionInfo.isMsgSession &&
                        !obj.sessionInfo.isRead &&
                        sessionInfo.isRead) {
                    /// 广播消息，验证消息，未读
                    sessionInfo.isRead = false;
                }
                theModel.set(i, { sessionInfo: sessionInfo } );
                return;
            }

        }
        /// 前面找不到，直接添加
        theModel.append( { sessionInfo: sessionInfo } );
    }
    function getUnreadCount() {
        var count = 0;
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.sessionInfo==null) {
                break;
            }
            if (obj.sessionInfo.isMsgSession && !obj.sessionInfo.isRead) {
                count = count + 1;
            }
            else if (obj.sessionInfo.unreadCount>0){
                count = count + obj.sessionInfo.unreadCount;
            }
        }
        return count;
    }
//    function findSessionInfo(groupId, fromUserId) {
//        for (var i=0; i<theModel.count; i++) {
//            var obj = theModel.get(i);
//            if (obj.sessionInfo==null) {
//                return null;
//            }

//            if (groupId>0 && groupId==obj.sessionInfo.groupId) {
//                return obj.sessionInfo;
//            }
//            else if (groupId==0 &&
//                     (fromUserId==obj.sessionInfo.fromUserId ||
//                      fromUserId==theApp.logonUserId)) {
//                return obj.sessionInfo;
//            }
//        }
//        return null;
//    }

    Timer {
        id: loadSessionDataTimer
        interval: 600
        onTriggered: { theApp.loadSessionInfo(50, 0); }
    }

    Connections {
        target: theApp
        onDeletedGroupId: {
            deleteSessionInfo(groupId, 0, 0);
        }

        onReceivedGroupInfo: {
            var sessionInfo = findSessionInfo(false, groupInfo.groupId, 0, 0);
            if (sessionInfo==null) {
                return;
            }
            if (groupInfo.memberCount>0) {
                var groupFullName = groupInfo.groupName +
                        " <font color=\"#999\">(" + groupInfo.memberCount + ")</font>";
                sessionInfo.groupFullName = groupFullName;
            }
            else {
                sessionInfo.groupFullName = groupInfo.groupName;
            }
        }

        onReceivedSessionLastInfo: {
            var sessionInfo = findSessionInfo(false, groupId, fromUserId, 0);
            if (sessionInfo!=null) {
                sessionInfo.lastInfo = lastInfo;
            }
        }

        onClearSessionUnreadCount: {
            var sessionInfo = findSessionInfo(false, groupId, fromUserId, 0);
            if (sessionInfo==null) {
                return;
            }
            if (sessionInfo.unreadCount>0 ||
                    (sessionInfo.isMsgSession && !sessionInfo.isRead)) {
                sessionInfo.unreadCount = 0;
                sessionInfo.isRead = true;
                rootPage.unreadCout(getUnreadCount());
            }
        }

        onReceivedRichInfo: {
            if (!richInfo.isReceive) {
                /// 不是消息，不处理
                return;
            }
            var groupId = richInfo.groupId;
            var fromUserId = richInfo.sendFromUserId;
            if (groupId>0 && groupId==theApp.currentChatGroupId) {
                return;
            }
            else if (groupId==0 && fromUserId==theApp.currentChatUserId) {
                return;
            }

            var sessionInfo = findSessionInfo(false, groupId, fromUserId, 0);
            if (sessionInfo==null) {
                return;
            }
            sessionInfo.unreadCount = sessionInfo.unreadCount+1;
            rootPage.unreadCout(getUnreadCount());
        }

        onLoadSessionInfoResponse: {
            setSessionInfoModelData(sessionInfo);
            if (sessionInfo.unreadCount>0 ||
                    (sessionInfo.isMsgSession && !sessionInfo.isRead)) {
                rootPage.unreadCout(getUnreadCount());
            }
        }
        onReceivedUserHeadImage: {
            if (userId==theApp.logonUserId) {
                return;
            }
            var sessionInfo = findSessionInfo(false, 0, userId, 0);
            if (sessionInfo!=null) {
                sessionInfo.fromHeadImage = headImage;
            }
        }
    }

    function updateLocaleInfo() {
        titleBar.title = theLocales.getLocalText("main-frame.mid-button-my-session.tooltip", "Session");
        addContactMenuItem.label = theLocales.getLocalText("context-menu.add-contact.text", "New Contact");
        deleteChatMenuItem.label = theLocales.getLocalText("main-frame.delete-chat.label", "Delete chat");
    }

    Component.onCompleted: {
        updateLocaleInfo();
    }

}

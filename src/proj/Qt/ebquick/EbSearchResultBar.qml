import QtQuick 2.0
import "js/ebbase.js" as Ebbasejs

Rectangle {
    id: rootRect
    /// 1=联系人
    /// 2=部门员工
    /// 3=群组成员
    /// 4=群聊（部门/群组）
    /// 5=集成应用
    /// 10=邀请好友
    /// 11=发送名片
    property int type: 0
    /// mode=1, 邀请好友
//    property int mode: 0
    property string key: ""
    property var searchFromGroupId: null
    property var searchFromUserId: null
    property int itemHeight: 48
    width: parent.width
    height: theModel.count==0?0:(theListView.y+theListView.height+bottomLineRect.height)
    visible: theModel.count>0?true:false
//    color: "#fff"

    signal pressedGroupInfo(var groupInfo);
    signal pressedMemberInfo(var groupInfo, var memberInfo);
    signal pressedContactInfo(var contactInfo);
    signal pressedFuncInfo(var funcInfo);
    signal selectedMemberInfo(var groupInfo, var memberInfo);
    signal selectedContactInfo(var contactInfo);
    signal checkedCountChanged(var count);

    function clearData() {
        theModel.clear();

    }
    function checkSelectedData() {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (!obj.checked) {
                /// 未选择
                continue;
            }
            if (obj.contactInfo!=null) {
                rootRect.selectedContactInfo(obj.contactInfo);
            }
            else if (obj.groupInfo!=null && obj.memberInfo!=null) {
                rootRect.selectedMemberInfo(obj.groupInfo, obj.memberInfo);
            }
        }
    }
    function sendCheckedCount(sendChanged) {
        var count = 0;
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (!obj.checked) {
                /// 未选择
                continue;
            }
            if (obj.contactInfo!=null) {
                count++;
            }
            else if (obj.groupInfo!=null && obj.memberInfo!=null) {
                count++;
            }
        }
        if (sendChanged) {
            rootRect.checkedCountChanged(count);
        }
    }
    function resetChecked(checkedIndex, sendChanged) {
        var count = 0;
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.contactInfo!=null) {
                obj.checked = checkedIndex==i?true:false;
                if (obj.checked) {
                    count = 1;
                }
            }
            else if (obj.groupInfo!=null && obj.memberInfo!=null) {
                obj.checked = checkedIndex==i?true:false;
                if (obj.checked) {
                    count = 1;
                }
            }
        }

        if (sendChanged) {
            rootRect.checkedCountChanged(count);
        }
    }

    Text {
        id: titleText
        x: 10
        height: 38
        width: parent.width-x
        verticalAlignment: Text.AlignVCenter
        color: "#666"
        text: {
            switch (rootRect.type)
            {
            case 1:
                /// 联系人
                return theLocales.getLocalText("main-frame.main-phone.contact", "Contact");
            case 2:
                /// 部门员工
                var groupType0Info = theLocales.getGroupTypeInfo(0);
                return groupType0Info.groupTypeMember;
            case 3:
                /// 群组成员
                var groupType2Info = theLocales.getGroupTypeInfo(2);
                return groupType2Info.groupTypeMember;
            case 4:
                /// 群聊
                return theLocales.getLocalText("name-text.group-chat", "Group chat");
            case 5:
                /// 集成应用
                return theLocales.getLocalText("main-frame.mid-button-my-app.tooltip", "Application");
            case 10:
                /// 选择用户
                return theLocales.getLocalText("select-user-dialog.title", "Select User");
                /// 邀请好友
//                return theLocales.getLocalText("chat-base-dialog.button-add-user.label", "Add user");
            case 11:
                /// 发送名片
                return theLocales.getLocalText("context-menu.send-ecard.text", "Send ECard");
            default:
                /// 其他搜索
                return "";
            }
        }
    }
    Rectangle {
        id: titleLineRect
        y: titleText.height
        width: parent.width
        height: 1
        color: "#ddd"
    }

    ListModel {
        id: theModel
    }
    Component {
        id: theDelegate

        Rectangle {
            id: theListItem
            width: parent.width
            height: rootRect.itemHeight

            EbImageNameInfo {
                id: theImageNameInfo
                width: parent.width
                height: rootRect.itemHeight-1
                pressEnable: true
                textIconMode: (rootRect.type==5 && funcInfo.iconImageFile.length==0)?true:false
                icon: rootRect.type==5?Ebbasejs.funcIconText(funcInfo):"";
                source: {
                    if (rootRect.type==1 ||
                            (dataType==1 && rootRect.type>=10 && contactInfo!=null)) {
                        return theApp.contactHeadImage(contactInfo);
                    }
                    else if (rootRect.type==2 || rootRect.type==3 ||
                             (dataType==2 && rootRect.type>=10 && memberInfo!=null)) {
                        return theApp.memberHeadImage(memberInfo);
                    }
                    else if (rootRect.type==4) {
                        return groupInfo.groupImagePath;
                    }
                    else if (rootRect.type==5) {
                        return funcInfo.iconImageFile;
                    }
                    return "";
                }
                name: {
                    var name;
                    if (rootRect.type==1 ||
                            (dataType==1 && rootRect.type>=10 && contactInfo!=null)) {
                        name = contactInfo.name;
                    }
                    else if (rootRect.type==2 || rootRect.type==3 ||
                             (dataType==2 && rootRect.type>=10 && memberInfo!=null)) {
                        name = memberInfo.userName;
                    }
                    else if (rootRect.type==4) {
                        name = groupInfo.groupFullName;
                    }
                    else if (rootRect.type==5) {
                        name = funcInfo.functionName;
                    }
                    else {
                        return "";
                    }

                    if (rootRect.key.length>0) {
                        /// 255.127.39=#ff7f27
                        var replaceText = "<font color=\"#ff7f27\">"+rootRect.key+"</font>";
                        name = name.replace(rootRect.key, replaceText);
                    }
                    return name;
                }
                info: {
                    var info;
                    if (rootRect.type==1 ||
                            (dataType==1 && rootRect.type>=10 && contactInfo!=null)) {
                        info = contactInfo.contact+"("+contactInfo.contactUserId+")";
                    }
                    else if (rootRect.type==2 || rootRect.type==3 ||
                             (dataType==2 && rootRect.type>=10 && memberInfo!=null)) {
                        info = memberInfo.memberAccount+"("+memberInfo.memberUserId+")";
                    }
                    else if (rootRect.type==4) {
                        info = "ID: " + groupInfo.groupId;
                    }
                    else if (rootRect.type==5) {
                        info = funcInfo.description;
                        if (info.length==0) {
                            info = "ID: " + funcInfo.subscribeId;
                        }
                    }
                    if (rootRect.key.length>0) {
                        /// 255.127.39=#ff7f27
                        var replaceText = "<font color=\"#ff7f27\">"+rootRect.key+"</font>";
                        info = info.replace(rootRect.key, replaceText);
                    }
                    return info;
                }
                Rectangle {
                    id: checkRect
                    visible: rootRect.type>=10?true:false
                    width: 20
                    height: width
                    x: parent.width-width-20
                    y: (parent.height-height)/2
                    radius: width/2
                    border.width: 1
                    border.color: "#ddd"
                    color: checked?"#22b14c":parent.color
                    Text {
                        /// type=10, 邀请好友，使用 checked ，多选
                        /// type=11，发送名片，单选
                        visible: (rootRect.type==10&&checked)?true:false
                        anchors.fill: parent
                        font.family: "FontAwesome"
                        text: "\uf00c"
                        color: "#fff"
                        font.pointSize: 9
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                onPressed: {
                    switch (rootRect.type)
                    {
                    case 1:
                        rootRect.pressedContactInfo(contactInfo);
                        break;
                    case 2:
                    case 3:
                        rootRect.pressedMemberInfo(groupInfo, memberInfo);
                        break;
                    case 4:
                        rootRect.pressedGroupInfo(groupInfo);
                        break;
                    case 5:
                        rootRect.pressedFuncInfo(funcInfo);
                        break;
                    case 10:
                        checked = !checked;
                        sendCheckedCount(true);
                        break;
                    case 11:
                        resetChecked(index, true);
                        break;
                    default:
                        break;
                    }
                }
            }

            Rectangle {
                /// line
                x: 0
                y: parent.height-1
                width: parent.width
                height: 1
                color: "#eee"
            }
        }

    }

    ListView {
        id: theListView
        y: titleText.height+titleLineRect.height
        width: parent.width
        height: rootRect.type>=10?(parent.height-y):(theModel.count*rootRect.itemHeight)
//        height: parent.height-y
//        anchors.fill: parent
        model: theModel
        delegate: theDelegate
    }

    Rectangle {
        /// line
        id: bottomLineRect;
        y: theListView.y + theListView.height
        width: parent.width
        height: 10
        color: "#eee"
    }

    function findContactInfoFromModelData(contactId) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.contactInfo==null) {
                break;
            }
            if (obj.contactInfo.contactId==contactId) {
                return true;
            }
        }
        return false;
    }
    function findMemberInfoFromModelData(userId) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.memberInfo==null) {
                break;
            }
            if (obj.memberInfo.memberUserId==userId) {
                return true;
            }
        }
        return false;
    }
    function findGroupInfoFromModelData(groupId) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.groupInfo.groupId==groupId) {
                return true;
            }
        }
        return false;
    }
    function findFuncInfoFromModelData(subscribeId) {
        for (var i=0; i<theModel.count; i++) {
            var obj = theModel.get(i);
            if (obj.funcInfo.subscribeId==subscribeId) {
                return true;
            }
        }
        return false;
    }

    Connections {
        target: theApp
        onSearchResultContactInfo: {
            if (rootRect.searchFromUserId!=null &&
                    contactInfo.contactUserId>0 &&
                    rootRect.searchFromUserId==contactInfo.contactUserId) {
                return;
            }
            if (rootRect.searchFromGroupId!=null && contactInfo.contactUserId>0 &&
                    theApp.isGroupMember(rootRect.searchFromGroupId, contactInfo.contactUserId)) {
                /// 存在
                return;
            }

            if (rootRect.type==1 ||
                    rootRect.type==10 ||
                    rootRect.type==11) {
                if (findContactInfoFromModelData(contactInfo.contactId)) {
                    return;
                }
                theModel.append( { dataType:1, groupInfo: null, memberInfo: null, contactInfo: contactInfo, funcInfo: null, checked: false } );
            }
        }
        onSearchResultMemberInfo: {
            if (memberInfo.memberUserId==theApp.logonUserId ||
                    (rootRect.searchFromUserId!=null &&
                     rootRect.searchFromUserId==memberInfo.memberUserId)) {
                return;
            }
            if (rootRect.searchFromUserId!=null &&
                    rootRect.searchFromUserId==memberInfo.memberUserId) {
                return;
            }
            if (rootRect.searchFromGroupId!=null &&
                    theApp.isGroupMember(rootRect.searchFromGroupId, memberInfo.memberUserId)) {
                /// 存在
                return;
            }
            if (rootRect.type==10 || rootRect.type==11 ||
                    (rootRect.type==2 && groupInfo.groupType<=1) ||
                    (rootRect.type==3 && groupInfo.groupType>=2)) {

                if (findMemberInfoFromModelData(memberInfo.memberUserId)) {
                    return;
                }
                theModel.append( { dataType:2, groupInfo: groupInfo, memberInfo: memberInfo, contactInfo: null, funcInfo: null, checked: false } );
            }
        }
        onSearchResultGroupInfo: {
            if (rootRect.type!=4) {
                return;
            }
            if (findGroupInfoFromModelData(groupInfo.groupId)) {
                return;
            }
            theModel.append( { dataType:3, groupInfo: groupInfo, memberInfo: null, contactInfo: null, funcInfo: null, checked: false } );
        }
        onSearchResultFuncInfo: {
            if (rootRect.type!=5) {
                return;
            }
            if (findFuncInfoFromModelData(funcInfo.subscribeId)) {
                return;
            }
            theModel.append( { dataType:4, groupInfo: null, memberInfo: null, contactInfo: null, funcInfo: funcInfo, checked: false } );
        }

    }
}

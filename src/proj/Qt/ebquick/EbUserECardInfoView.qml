import QtQuick 2.0
import QtQuick.Controls 1.4
import Entboost.KeyFilter 1.0
import Entboost.ECardInfo 1.0

Rectangle {
    id: rootItem
    anchors.fill: parent
    z: 100

    signal returnBack();
    signal sendUserMessage(var userId);

    function requestReturnBack() {
        return true;
    }

    property var userECardInfo: null
    function setUserECardInfo(userECardInfo) {
        rootItem.userECardInfo = userECardInfo;
        titleBar.title = userECardInfo.accountName+"-"+theLocales.getLocalText("chat-msg-text.user-ecard", "User ECard");
        userECardImageNameInfo.source = theApp.userHeadImage2(userECardInfo.memberUserId, userECardInfo.headResourceFile);
        userECardImageNameInfo.name = userECardInfo.accountName;
        var groupType = theApp.groupType(userECardInfo.groupId);
        var groupTypeInfo = theLocales.getGroupTypeInfo(groupType);
        userECardImageNameInfo.info = "UID: " + userECardInfo.memberUserId;
        phoneText.text = userECardInfo.phone;
        telText.text = userECardInfo.tel;
        emailText.text = userECardInfo.email;
        addressText.text = userECardInfo.address;
        nameText.text = userECardInfo.name;
        titleText.text = userECardInfo.title;
        var label = theLocales.getLocalText("member-info-dialog.label-group-name.label", "Group Name");
        groupNameText.label = label.replace("[GROUP_TYPE_NAME]", groupTypeInfo.groupTypeName);
        groupNameText.text = userECardInfo.groupName;
        enterpriseText.text = userECardInfo.enterprise;
        sendChatMessageButton.visible = (userECardInfo.memberUserId!=theApp.logonUserId)?true:false;
    }

    EbTitleBar {
        id: titleBar;
        width: parent.width
        rightButton0Label: "\uf044"
//        title: "个人名片"
        onReturnBack: {
            if (requestReturnBack()) {
                rootItem.returnBack();
            }
        }
    }

    Column {
//        anchors.fill: parent
        y: titleBar.height
        width: parent.width
        height: parent.height-y

        EbImageNameInfo {
            id: userECardImageNameInfo
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }
        /// phone,tel,email,address
        EbTextLine {
            /// 手机
            id: phoneText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            /// 电话
            id: telText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            /// 邮箱
            id: emailText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            /// 地址
            id: addressText
        }
        Rectangle {
            width: parent.width
            height: 8
            color: "#eee"
        }

        /// name,title,groupName,groupId,enterprise,
        EbTextLine {
            /// 名称
            id: nameText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            /// 职位
            id: titleText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            /// 部门
            id: groupNameText
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#eee"
        }
        EbTextLine {
            /// 公司
            id: enterpriseText
        }
        Rectangle {
            id: enterpriseLine
            width: parent.width
            height: 8
            color: "#eee"
        }

        Item {
            width: parent.width
            height: 8
        }

        EbButton {
            id: sendChatMessageButton
            x: 20
            width: parent.width-x*2
            height: 36
            radius: 3
            border.width: 0
            textColor: "#fff"
            buttonColor: theApp.mainColor
            buttonHoverColor: theApp.mainHoverColor
            buttonPressedColor: theApp.mainPressedColor
            onClickedButton: { rootItem.sendUserMessage(rootItem.userECardInfo.memberUserId); }
        }
//        Item {
//            width: parent.width
//            height: 8
//        }
//        Text {
//            id: tipText
//            x: sendChatMessageButton.x
//            width: sendChatMessageButton.width
//            height: 68
//            wrapMode: Text.Wrap
//            lineHeight: 1.2
//            color: "red"
//        }

    }

    function updateLocaleInfo() {
        phoneText.label = theLocales.getLocalText("member-info-dialog.label-phone.label", "Phone");
        telText.label = theLocales.getLocalText("member-info-dialog.label-tel.label", "Tel");
        emailText.label = theLocales.getLocalText("member-info-dialog.label-email.label", "Email");
        addressText.label = theLocales.getLocalText("member-info-dialog.label-address.label", "Address");
        nameText.label = theLocales.getLocalText("member-info-dialog.label-user-name.label", "Name");
        titleText.label = theLocales.getLocalText("member-info-dialog.label-job-title.label", "Job Title");
//        groupNameText.label = theLocales.getLocalText("member-info-dialog.label-parent-group.label", "Parent Group");
        enterpriseText.label = theLocales.getLocalText("member-info-dialog.label-enterprise.label", "Enterprise");
        sendChatMessageButton.label = theLocales.getLocalText("base-dialog.send-msg-button.text", "Send Message");
    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            if (requestReturnBack()) {
                rootItem.returnBack();
            }
        }
    }
    Component.onCompleted: {
        var docRoot = rootItem.parent;
        while (docRoot.parent) {
            docRoot = docRoot.parent;
        }
        theKeyFilter.setFilter(docRoot, 1);
        updateLocaleInfo();
    }

}

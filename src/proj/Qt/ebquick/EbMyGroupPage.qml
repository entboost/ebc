import QtQuick 2.0
//import QtQuick.Controls 2.2

EbPage {
    id: rootPage
    footerLineHeight: 0

    signal pressedGroupInfo(var groupInfo);
    signal requestNewGroup(var groupType, var parentGroupId);

    EbGroupListItem {
        anchors.fill: parent
        listViewType: 1
        onPressedGroupInfo: { rootPage.pressedGroupInfo(groupInfo); }
        onRequestNewGroup: { rootPage.requestNewGroup(groupType, parentGroupId); }
    }

}

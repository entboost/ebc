import QtQuick 2.7
import QtQuick.Controls 2.2
import Entboost.KeyFilter 1.0

Rectangle {
    id: rootRect
    anchors.fill: parent
    /// searchMode=1 邀请好友
    /// searchMode=2 发送名片
    property int searchMode: 0
    property var searchFromGroupId: null
    property var searchFromUserId: null

    signal returnBack();
    signal pressedGroupInfo(var groupInfo);
    signal pressedMemberInfo(var groupInfo, var memberInfo);
    signal pressedContactInfo(var contactInfo);
    signal pressedFuncInfo(var funcInfo);
    signal selectedMemberInfo(var groupInfo, var memberInfo);
    signal selectedContactInfo(var contactInfo);

    function requestReturnBack() {
        theModel.clear();
        return true;
    }

    function checkSelectedData() {
        addUserSearchResultBar.checkSelectedData()
        rootRect.returnBack();
    }

    property int searchParameter: 1
    Rectangle {
        id: headerRect
        width: parent.width
        height: 48
        color: theApp.mainColor
        EbButton {
            id: returnBackButton
            width: parent.height
            height: parent.height
            color: "transparent"
            fontFamily: "FontAwesome"
            label: "\uf060"
            textColor: "#fff"
            buttonColor: theApp.mainColor
            borderColor: theApp.mainColor
            fontPointSize: 16
            onClickedButton: returnBack()
        }

        TextInput {
            id: searchTextInput
            color: "#fff"
            x: returnBackButton.width + 8
            y: 6;
            height: 32
            width: okSelectButton.visible?(parent.width - x - 4 - okSelectButton.width):(parent.width - x - 8)
            leftPadding: searchIconText.width+2
            rightPadding: rightClearButton.width+2
            verticalAlignment: TextInput.AlignVCenter

            Timer {
                id: searchTimer
                interval: 100
                onTriggered: {
                    theApp.search(searchTextInput.text, rootRect.searchMode, searchParameter);
                }
            }

            onTextChanged: {
                theModel.clear();
                if (rootRect.searchMode==1 ||
                        rootRect.searchMode==2) {
                    addUserSearchResultBar.clearData();
                    addUserSearchResultBar.key = text;
                    /// ** 必须使用定期器搜索，否则会有问题
                    searchTimer.stop();
                    searchTimer.start();
                }
                else if (text.length>0 || rootRect.searchMode==3) {
                    theModel.append( { resultType: 1, searchKey: text } );
                    theModel.append( { resultType: 2, searchKey: text } );
                    theModel.append( { resultType: 3, searchKey: text } );
                    if (rootRect.searchMode==0 ||
                            rootRect.searchMode==3) {
                        theModel.append( { resultType: 4, searchKey: text } );
                    }
                    if (rootRect.searchMode==0) {
                        theModel.append( { resultType: 5, searchKey: text } );
                    }
                    /// ** 必须使用定期器搜索，否则会有问题
                    searchTimer.stop();
                    searchTimer.start();
                }
//                rightClearButton.visible = (rootRect.searchMode==0 && text.length>0)?true:false;
            }

            Text {
                id: searchIconText
                width: parent.height
                height: width
                font.family: "FontAwesome"
                font.pointSize: 15
                text: "\uf002"
                color: "#fff"
                verticalAlignment: Text.AlignVCenter
            }

            EbButton {
                id: rightClearButton
                border.width: 0
                width: parent.height
                height: width
                x: parent.width-width
                fontFamily: "FontAwesome"
                label: "\uf00d"
                textColor: "#ddd"
                buttonColor: "transparent"
                borderColor: "transparent"
                fontPointSize: 15
//                visible: false
                visible: (searchTextInput.text.length>0)?true:false
//                visible: (rootRect.searchMode==0 && searchTextInput.text.length>0)?true:false
                onClickedButton: { searchTextInput.clear(); }
            }
        }

        Rectangle {
            id: searchLine
            color: "#fff"
            x: searchTextInput.x
            y: searchTextInput.y + searchTextInput.height
            width: searchTextInput.width
            height: 1
        }

        EbButton {
            id: okSelectButton
            height: parent.height
            width: height+10
            x: parent.width-width
            label: theLocales.getLocalText("base-dialog.ok-button.text", "Ok");
            textColor: "#fff"
            buttonColor: theApp.mainColor
            buttonPressedColor: theApp.mainPressedColor
            border.width: 0
            fontPointSize: 15
            visible: false
//            visible: (rootRect.searchMode==1)?true:false
            onClickedButton: { checkSelectedData(); }
        }
    }

    EbSearchResultBar {
        id: addUserSearchResultBar
        y: headerRect.height
        width: parent.width
        height: parent.height-y
        type: {
            switch(rootRect.searchMode) {
            case 1:
                return 10;
            case 2:
                return 11;
//            case 3:
//                return 12;
            }
            return 0;
        }
        searchFromGroupId: rootRect.searchFromGroupId
        searchFromUserId: rootRect.searchFromUserId
//        key: searchKey
//        mode: rootRect.searchMode
        visible: (rootRect.searchMode==1 || rootRect.searchMode==2)?true:false
//        onPressedGroupInfo: { rootRect.pressedGroupInfo(groupInfo); }
//        onPressedMemberInfo: { rootRect.pressedMemberInfo(groupInfo, memberInfo); }
//        onPressedContactInfo: { rootRect.pressedContactInfo(contactInfo); }
//        onPressedFuncInfo: { rootRect.pressedFuncInfo(funcInfo); }

        onSelectedMemberInfo: { rootRect.selectedMemberInfo(groupInfo, memberInfo); }
        onSelectedContactInfo: { rootRect.selectedContactInfo(contactInfo); }
        onCheckedCountChanged: { okSelectButton.visible = ((rootRect.searchMode>=1) && count>0)?true:false; }
    }

    ListModel {
        id: theModel
    }
    Component {
        id: theDelegate

        EbSearchResultBar {
            id: theListItem
            type: resultType
            key: searchKey
            searchFromGroupId: rootRect.searchFromGroupId
            searchFromUserId: rootRect.searchFromUserId
//            mode: rootRect.searchMode
            onPressedGroupInfo: { rootRect.pressedGroupInfo(groupInfo); }
            onPressedMemberInfo: { rootRect.pressedMemberInfo(groupInfo, memberInfo); }
            onPressedContactInfo: { rootRect.pressedContactInfo(contactInfo); }
            onPressedFuncInfo: { rootRect.pressedFuncInfo(funcInfo); }
        }
    }

    ListView {
        id: theListView
        y: headerRect.height
        width: parent.width
        height: parent.height-y
//        anchors.fill: parent
        model: theModel
        delegate: theDelegate
        visible: (rootRect.searchMode==0 || rootRect.searchMode==3)?true:false
    }

//    function findResultInfoFromModelData(type) {
//        for (var i=0; i<theModel.count; i++) {
//            var obj = theModel.get(i);
//            if (obj.type===type) {
//                return obj;
//            }
//        }
//        return null;
//    }

//    Connections {
//        target: theApp
//        onSearchResultMemberInfo: {
//            var obj = findResultInfoFromModelData(1);
//            if (obj==null) {
//                theModel.append( { type:1 } );
//                obj = findResultInfoFromModelData(1);
//            }
//            console.log("********** onSearchResultMemberInfo, obj=" + obj);
//            if (obj!=null) {
//                obj.theListItem.addData(memberInfo);
////                obj.addData(memberInfo);
//            }
//        }

//        onSearchResultContactInfo: {
//            var obj = findResultInfoFromModelData(2);

//        }
//    }

    EbKeyFilter {
        id: theKeyFilter
        onKeyBackPressed: {
            if (requestReturnBack()) {
                rootRect.returnBack();
            }
        }
    }
    ///
    function updateLocaleInfo() {
//        searchTextInput.p
    }

    Component.onCompleted: {
        var docRoot = rootRect.parent;
        while (docRoot.parent) {
            docRoot = docRoot.parent;
        }
        theKeyFilter.setFilter(docRoot, 1);
        updateLocaleInfo();
        if (rootRect.searchMode==1 ||
                rootRect.searchMode==2) {
            /// ** 必须使用定期器搜索，否则会有问题
            searchTimer.stop();
            searchTimer.start();
        }
        else if (rootRect.searchMode==3) {
            theModel.append( { resultType: 1, searchKey: "" } );
            theModel.append( { resultType: 2, searchKey: "" } );
            theModel.append( { resultType: 3, searchKey: "" } );
            theModel.append( { resultType: 4, searchKey: "" } );
            searchTimer.stop();
            searchTimer.start();
        }
    }
}

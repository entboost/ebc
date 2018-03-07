import QtQuick 2.0
import QtQuick.Controls 2.0

Item {
    id: rootItem
    signal returnBack()

    ListModel {
        id: myModel
//        ListElement { name: "中文简体"; file: "zh-CN.json" }
//        ListElement { name: "中文繁体"; file: "zh-TW.json" }
//        ListElement { name: "English"; file: "en-US.json" }
    }
    Component {
        id: myDelegate
        EbButton {
            width: parent.width
            height: 32
            label: name
            textColor: "#fff"
            buttonColor: theApp.mainColor
            borderColor: "transparent"
            borderPressedColor: "#fff"
        }
    }
    ListView {
        width: parent.width
        height: parent.height
        anchors.fill: parent
        model: myModel
        delegate: myDelegate
    }
    Connections {
        target: theLocales;
        onLocaleInfo: {
            /// (language, name, file)
            myModel.append( { name: name, file: file } );
//            console.log("onLocaleInfo"+language);
        }
    }
    Component.onCompleted: {
        theLocales.loadLocaleList();
    }

    MouseArea {
        anchors.fill: parent
    }
    Keys.onBackPressed: {
        event.accepted = true;
        rootItem.visible = false;
        returnBack();
    }

}

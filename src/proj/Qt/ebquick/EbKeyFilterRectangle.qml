import QtQuick 2.0
import Entboost.KeyFilter 1.0

Rectangle {
    id: rootItem

    signal returnBack();

    EbKeyFilter {
        id: aKeyFilter
        onKeyBackPressed: {
            rootItem.returnBack();
        }
    }

//    property var rootFilterObject: null
    Component.onCompleted: {
        aKeyFilter.setFilter(rootItem, 3);
//        var docRoot = rootItem.parent;
//        while (docRoot.parent) {
//            docRoot = docRoot.parent;
//        }
//        rootFilterObject = docRoot;
//        aKeyFilter.setFilter(rootFilterObject, 1);
    }
}

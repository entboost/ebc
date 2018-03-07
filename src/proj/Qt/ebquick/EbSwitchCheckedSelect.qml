import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

/// 左右结构
/// 左边 label 提示
/// 右边二个 radio button选择
Rectangle {
    id: rootItem
    property alias label: labelText.text
    property alias colorLabel: labelText.color
    property alias checkedLabel: checkedRadioButton.label
    property alias uncheckLabel: uncheckRadioButton.label
    property alias checked: checkedRadioButton.checked

    signal switchCheckedChanged(var checked);
//    signal pressed();

    width: parent.width
    height: 32
    Text {
        id: labelText
        x: 8
        width: parent.width*0.28-x
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        color: "#666"
    }

    ExclusiveGroup {
        id: switchGroup;
    }

    EbRadioButton {
        id: checkedRadioButton;
        x: uncheckRadioButton.x - 60
        width: 12
        height: parent.height
        exclusiveGroup: switchGroup;
        onCheckedChanged: {
            rootItem.switchCheckedChanged(checked);
        }
    }
    EbRadioButton {
        id: uncheckRadioButton;
        x: parent.width-60-10
        width: 12
        height: parent.height
        checked: !checkedRadioButton.checked
        exclusiveGroup: switchGroup;
    }

}

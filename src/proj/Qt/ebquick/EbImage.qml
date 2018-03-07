import QtQuick 2.7
import QtGraphicalEffects 1.0

///  显示圆角图像
Rectangle {
    id: img
    property alias source: _image.source
    property alias fillMode: _image.fillMode
    property alias sourceSize: _image.sourceSize
    property alias grayImage: grayItem.visible

    height: parent.height
    width: height
    radius: width/2
    color: "#fff"

    Image {
        id: _image
        smooth: true
        visible: false
        anchors.fill: parent
        /// *必须设为 false ，用户头像修改能实时更新
        cache: false
//        antialiasing: true
    }
    Rectangle {
        id: _mask
        color: "#fff"
        anchors.fill: parent
        radius: width/2
        visible: false
        antialiasing: true
        smooth: true
    }
    OpacityMask {
        id: mask_image
        anchors.fill: _image
        source: _image
        maskSource: _mask
        visible: true
        antialiasing: true
    }
    /// 灰色效果
    Colorize {
        id: grayItem
        anchors.fill: mask_image
        source: mask_image
        hue: 0.0
        saturation: 0.0
        lightness: 0.0
        visible: false
    }
}

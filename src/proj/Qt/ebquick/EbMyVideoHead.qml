import QtQuick 2.0
import QtMultimedia 5.8
//import Entboost.KeyFilter 1.0

Rectangle {
    id: rootItem
    z: 1
    anchors.fill: parent
    property alias source: photoPreview.source
    color: "#333"

    signal returnBack();
    signal accepted();

    function requestReturnBack() {
        if (!captureButton.visible) {
            /// 切换回浏览摄像头状态
            captureButton.visible = true;
            return false;
        }
        return true;
    }

//    EbTitleBar {
//        id: titleBar;
//        z: 1
//        width: parent.width
//        title: "头像"
//        onReturnBack: {
//            rootItem.returnBack();
//        }
//    }
    EbButton {
        id: returnButton
        z: 1
        width: 42
        height: width
        border.width: 0
        fontFamily: "FontAwesome"
        label: "\uf060";
        fontPointSize: 15
        textColor: "#666"
        textPressedColor: theApp.mainColor
        buttonColor: "transparent"
        buttonPressedColor: buttonColor
        onClickedButton: {
            /// 切换回浏览状态，可以直接退出
            captureButton.visible = true;
            rootItem.returnBack();
        }
    }

    Camera {
        id: camera
        position: Camera.FrontFace
        imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceFlash
        exposure {
            exposureCompensation: -1.0
            exposureMode: Camera.ExposurePortrait
        }
        captureMode: Camera.CaptureStillImage
        flash.mode: Camera.FlashRedEyeReduction

        property bool savedTest: false
        imageCapture {
//            resolution {
//            }

//            onImageCaptured: {
//                console.log("*************** onImageCaptured... preview=" + preview);
//                photoPreview.source = preview  /// Show the preview in an Image
//            }
            onImageSaved: {
//                console.log("********** onImageSaved saved path: " + path);
                photoPreview.source = "file:///" + path;
//                photoPreview.source = "file://" + path;
            }
        }
    }
    VideoOutput {
        id: videoOutput
        source: camera
//        y: titleBar.height
//        width: parent.width
//        height: parent.height-y
        anchors.fill: parent
        rotation: 90    /// 安卓手机，转到正确角度
//        orientation: 0
//        orientation: -90
        focus : visible /// to receive focus and capture key events when visible
    }

    Image {
        id: photoPreview
        visible: !captureButton.visible
        anchors.fill: parent
//        y: titleBar.height
//        width: parent.width
//        height: parent.height-y
        fillMode: Image.PreserveAspectFit
    }

    /// f00c=check
    /// f00d=close
    /// f030=camera
    EbButton {
        id: captureButton
        z: 1
        width: 64
        height: width
        x: (parent.width-width)/2
        y: parent.height-height-30
        radius: width/2
        border.width: 1
        fontFamily: "FontAwesome"
        label: "\uf030";
        fontPointSize: 18
        textColor: "#666"
        textPressedColor: theApp.mainColor
        buttonColor: "transparent"
        buttonPressedColor: buttonColor
        borderColor: textColor
        borderPressedColor: borderColor
        onClickedButton: {
            captureButton.visible = false;
            photoPreview.source = "";
            camera.imageCapture.captureToLocation(theApp.userImagePath);
        }
    }
    EbButton {
        id: changeFaceButton
        z: 1
        visible: captureButton.visible
        width: 36
        height: width
        x: captureButton.x + captureButton.width + width
        y: captureButton.y + (captureButton.height-height)/2
        radius: width/2
        border.width: 1
        fontFamily: "FontAwesome"
        label: "\uf021";
        fontPointSize: 15
        textColor: "#666"
        textPressedColor: theApp.mainColor
        buttonColor: "transparent"
        buttonPressedColor: buttonColor
        borderColor: textColor
        borderPressedColor: theApp.mainColor
        onClickedButton: {
            if (camera.position==Camera.FrontFace)
                camera.position=Camera.BackFace;
            else
                camera.position=Camera.FrontFace;
        }
    }
    EbButton {
        id: closeButton
        z: 1
        visible: !captureButton.visible
        width: 64
        height: width
        x: width
        y: captureButton.y
        radius: width/2
        border.width: 1
        fontFamily: "FontAwesome"
        label: "\uf00d";
        fontPointSize: 18
        textColor: "#666"
        textPressedColor: theApp.mainColor
        buttonColor: "transparent"
        buttonPressedColor: buttonColor
        borderColor: textColor
        borderPressedColor: theApp.mainColor
        onClickedButton: {
            captureButton.visible = true;
        }
    }
    EbButton {
        id: acceptButton
        z: 1
        visible: !captureButton.visible
        width: closeButton.width
        height: width
        x: parent.width-width*2
        y: captureButton.y
        radius: width/2
        border.width: 1
        fontFamily: "FontAwesome"
        label: "\uf00c";
        fontPointSize: 18
        textColor: theApp.mainColor
        disabledColor: "#999"
        textPressedColor: textColor
        buttonColor: "transparent"
        buttonPressedColor: buttonColor
        borderColor: "#666"
        borderPressedColor: theApp.mainColor
        onClickedButton: {
            acceptButton.enabled = false;
            captureButton.visible = true;
            rootItem.accepted();
        }
    }
}

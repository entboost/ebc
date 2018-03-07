import QtQuick 2.0

Timer {
    id: delayPressTimer
    interval: 50
    repeat: true

    signal pressed();

    property bool pressedReleased: false
    property int count: 0
    function initStart() {
        count = 0;
        pressedReleased = false;
        start();
    }

    onTriggered: {
        if (!repeat || pressedReleased) {
            stop();
            delayPressTimer.pressed();
        }
        else if ((count++) > 6) {
            stop();
        }
    }
}

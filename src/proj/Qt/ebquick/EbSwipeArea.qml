import QtQuick 2.0

MouseArea {
    property point origin
    property bool ready: false
    property int threshold: units.gu(20)
    signal move(int x, int y)
    signal swipe(string direction)

    onPressed: {
        drag.axis = Drag.XAndYAxis
        origin = Qt.point(mouse.x, mouse.y)
    }

    onPositionChanged: {
        switch (drag.axis) {
        case Drag.XAndYAxis:
            /// 优化判断，上下或者左右
            var yoffset = Math.abs(mouse.y - origin.y);
            var xoffset = Math.abs(mouse.x - origin.x);
            if (yoffset>=xoffset && yoffset>threshold) {
                drag.axis = Drag.YAxis
            }
            else if (xoffset > threshold ) {
                drag.axis = Drag.XAxis
            }
            else {
                drag.axis = Drag.XAndYAxis
            }

//            if (Math.abs(mouse.y - origin.y) > threshold) {
//                drag.axis = Drag.YAxis
//            }
//            else if (Math.abs(mouse.x - origin.x) > threshold ) {
//                drag.axis = Drag.XAxis
//            }
            break
        case Drag.XAxis:
            move(mouse.x - origin.x, 0)
            break
        case Drag.YAxis:
            move(0, mouse.y - origin.y)
            break
        }
    }

    onReleased: {
        switch (drag.axis) {
        case Drag.XAndYAxis:
            canceled(mouse)
            break
        case Drag.XAxis:
            swipe(mouse.x - origin.x < 0 ? "left" : "right")
            break
        case Drag.YAxis:
            swipe(mouse.y - origin.y < 0 ? "up" : "down")
            break
        }
    }
}

import QtQuick 2.0
import "."

Point {
    id: insertPoint
    property color pointColor : "black"
    z: 20

    Rectangle {
        id: point
        width: Settings.pointSize
        height: width
        radius: width / 2.0
        color: pointColor
        x: start.x - radius
        y: start.y - radius
    }
}


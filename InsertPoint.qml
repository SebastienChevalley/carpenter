import QtQuick 2.0
import "."

Point {
    id: insertPoint
    property color pointColor : "black"
    property bool selected: false;
    z: 20

    property MouseArea mouseArea : mouseArea


    Rectangle {
        id: point
        width: parent.selected ? Settings.pointSize * 1.8 : Settings.pointSize
        height: width
        radius: width / 2.0
        color: parent.selected ? "blue" : pointColor
        x: start.x - radius
        y: start.y - radius

        MouseArea {
            id: mouseArea;
            anchors.fill: parent
            enabled: false
        }
    }
}


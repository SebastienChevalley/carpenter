import QtQuick 2.0
import QtQuick.Controls 1.4
import "." // to import Settings

Point {
    id: insertPoint
    property color pointColor : "black"
    property bool selected: false;
    z: 20

    property MouseArea mouseArea : mouseArea

    Rectangle {
        id: point
        width: Settings.pointSize
        height: width
        radius: width / 2.0
        color: parent.selected ? Settings.insertPointSelected : pointColor
        x: start.x - radius
        y: start.y - radius


        // Want to display the identifier on object ?
        Label {
            text: identifier
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 16
            color: "white"
            visible: Settings.displayIdentifier
        }


        MouseArea {
            id: mouseArea;
            anchors.fill: parent
            enabled: false
        }
    }
}


import QtQuick 2.0
import "."

Line {


    property vector2d horizontal: Qt.vector2d(1,0)

    property IntermediatePoint intermediatePoint
    property bool selected: false;


    function thePointer() {
        return pointer;
    }

    function updatePointer() {
        pointer = computeLine();
        updateIntermediatePoint();
    }

    property color fill: "grey"

    function getLine() {
        return line;
    }

    function updateIntermediatePoint() {
        if(intermediatePoint !== null) {
            intermediatePoint.start = start.plus(pointer.times(0.5))
        }
    }

    property MouseArea mouseArea : mouseArea

    Rectangle {
        id: line
        // radius
        width: parent.pointer.length() + height / 2.0 + (Settings.constructionLineWidth/2.0);
        color: parent.selected ? "blue" : fill
        height: Settings.constructionLineWidth
        opacity: 0.5
        radius: Settings.constructionLineWidth / 2.0
        z: 10;

        x: parent.start.x - height*0.5
        y: parent.start.y - height*0.5

        transform: Rotation {
            // assume unit vector for horizontal
            angle: (thePointer().y < 0 ? -1 : 1) * Math.acos(horizontal.dotProduct(thePointer()) / (thePointer().length())) * 180 / Math.PI
            origin.x: Settings.constructionLineWidth / 2.0
            origin.y: Settings.constructionLineWidth / 2.0
        }

        MouseArea {
            id: mouseArea;
            anchors.fill: parent
            enabled: false
        }
    }
}


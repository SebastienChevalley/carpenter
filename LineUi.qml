import QtQuick 2.0
import "."

Line {

    property vector2d pointer: updatePointer();
    property vector2d horizontal: Qt.vector2d(1,0)

    function updatePointer() {
        pointer = computeLine();
    }

    function thePointer() {
        return pointer;
    }

    onStartChanged: {
        updatePointer()
    }
    onEndChanged: {
        updatePointer()
    }

    property color fill: "grey"

    function getLine() {
        return line;
    }

    Rectangle {
        id: line
        // radius
        width: parent.pointer.length() + height / 2.0 + (Settings.constructionLineWidth/2.0);
        color: fill
        height: Settings.constructionLineWidth
        opacity: 0.5
        radius: Settings.constructionLineWidth / 2.0

        x: parent.start.x - height*0.5
        y: parent.start.y - height*0.5

        transform: Rotation {
            // assume unit vector for horizontal
            angle: (thePointer().y < 0 ? -1 : 1) * Math.acos(horizontal.dotProduct(thePointer()) / (thePointer().length())) * 180 / Math.PI
            origin.x: Settings.constructionLineWidth / 2.0
            origin.y: Settings.constructionLineWidth / 2.0
        }
    }
}


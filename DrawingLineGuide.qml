import QtQuick 2.0
import "."

Item {

    property vector2d start
    property vector2d to;
    property vector2d horizontal: Qt.vector2d(1, 0);
    property vector2d pointer: computePointer();

    property var points: [];

    property bool isInsertable: false;

    function setOrigin(vector) {
        start = vector;
        pointer = computePointer();
        isInsertable = computeIsInsertable();
        return this;
    }

    function setDestination(v) {
        to = v;
        pointer = computePointer();
        isInsertable = computeIsInsertable();
        return this;
    }

    // TODO memoize this ?
    function computePointer() {
        return to.minus(start);
    }

    function thePointer() {
        if(pointer === null) {
            pointer = computePointer();
        }

        return pointer;
    }

    function computeIsInsertable() {
            // minimum line length
        return line.width > Settings.minimalPointDistance
            // minimum distance between points
            // TODO: remove after the distance between lines will be removed
            // points.every(function(x) { return x.distanceTo(to) >= Settings.minimalPointDistance });
            // minimum distance between lines
    }

    /*Rectangle {
        id: circle
        color: "red"
        width: 50
        height: width
        radius: width / 2.0
        x: parent.start.x - radius
        y: parent.start.y - radius
    }*/

    Rectangle {

        id: line
        // radius
        width: parent.pointer.length() + height / 2.0 + (Settings.constructionLineWidth/2.0);
        color: isInsertable ? "green" : "red"
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


import QtQuick 2.0
import "."

LineUi {
    fill: "blue"

    property bool isInsertable: computeIsInsertable()

    onStartChanged: {
        isInsertable = computeIsInsertable()
    }
    onEndChanged: {
        isInsertable = computeIsInsertable()
    }


    function computeIsInsertable() {
            // minimum line length
        return getLine().width > Settings.minimalPointDistance
            // minimum distance between points
            // TODO: remove after the distance between lines will be removed
            // points.every(function(x) { return x.distanceTo(to) >= Settings.minimalPointDistance });
            // minimum distance between lines
    }
}


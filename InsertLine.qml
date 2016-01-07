import QtQuick 2.0
import "." // to import Settings

LineUi {
    z: 10

    property bool isInsertable: computeIsInsertable()

    onStartChanged: {
        isInsertable = computeIsInsertable()
    }
    onEndChanged: {
        isInsertable = computeIsInsertable()
    }

    fill: isInsertable ? Settings.insertLineIsInsertable : Settings.insertLineIsNotInsertable

    function computeIsInsertable() {
        return getLine().width > Settings.minimalPointDistance
    }
}


import QtQuick 2.0
import "." // to import Settings

InsertPoint {
    pointColor: Settings.intermediatePointColor
    property Line line;
    property Line lineUi;
    property real identifier: -1
    property bool stop: false


    visible: !(line.start.fuzzyEquals(line.end));
}


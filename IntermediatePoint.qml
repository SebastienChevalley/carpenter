import QtQuick 2.0

InsertPoint {
    pointColor: "red"
    property Line line;
    property Line lineUi;

    Component.onCompleted: {
       lineUi.endPoint.startChanged.connect(updatePoint)
       lineUi.startPoint.startChanged.connect(updatePoint)
    }

    function updatePoint() {
        start = lineUi.computeIntermediatePoint()
    }

}


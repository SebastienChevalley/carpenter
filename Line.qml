import QtQuick 2.0

Item {
    property Point startPoint: Point {
        start: Qt.vector2d(10,10)
    }

    property Point endPoint: Point {
        start: Qt.vector2d(100,100)
    }

    property vector2d start: startPoint.start
    property vector2d end: endPoint.start


    /*endPoint.onStartChanged: {
        end = endPoint.start
    }

    startPoint.onStartChanged: {
        start = startPoint.start
    }*/

    // compare the two side of the line
    function equal(that) {
        return [[start, end], [end, start]]
            .map(function(line) { return line[0] === that.start && line[1] === that.end })
            .filter(function(x) { return x === true })
            .length > 0
    }

    function isContainsIn(lines) {
        return lines.map(equal).filter(function(x) { return x === true }).length > 0
    }

    function computeLine() {
        return end.minus(start)
    }

    function computeIntermediatePoint() {
        return start.plus(computeLine().times(0.5));
    }

    // to store in ui
    function key() {
        return '(' + start.toString() + ',' + end.toString() + ')'
    }
}


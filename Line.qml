import QtQuick 2.0

Item {
    property vector2d start
    property vector2d end

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
}


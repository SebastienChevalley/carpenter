import QtQuick 2.0

Item {
    property vector2d start;

    // todo put in a model class
    function distanceTo(point) {
        return point.minus(start).length();
    }

    function setStart(newStart) {
        //this.destroy();
        start = newStart;
        return this;
    }

    function isContainsIn(points) {
        return points.filter(function(point) { return start.x === point.start.x && start.y === point.start.y }).length > 0;
    }
}


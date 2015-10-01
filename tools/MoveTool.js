Qt.include("Tool.js")
Qt.include("qrc:/lib/lib/lodash.js");

var _ = lodash(this);

function MoveTool(context) {
    Tool.call(this, context);


    console.log('Build a MoveTool');


    this.intermediatePointUi = Qt.createComponent("qrc:/IntermediatePoint.qml");

    this.fakePoints = [];
    this.movingPoint = null;

    this.createIntermediatePoint = function(line, lineUi) {
        var point = this.intermediatePointUi.createObject(this.mouseArea, { 'line': line, 'lineUi': lineUi, 'start': line.computeIntermediatePoint() });
        return point;
    };

    /**
      * @param position from which point do the computation, usually current mouse position
      * @return null if no nearest point found or the nearest point
      */
    this.computeNearestPoint = function(position) {
        var nearestPoints = this.sketch.nearestPoints(position, this.fakePoints);

        if(nearestPoints.length === 0) {
            return null;
        }
        else {
            return nearestPoints[0];
        }
    };

    this.fakePointMapper = function(line) {
        return this.createIntermediatePoint(line, this.mouseArea.lines[line.key()]);
    }
}

MoveTool.prototype = Object.create(Tool.prototype);
MoveTool.prototype.constructor = MoveTool;

MoveTool.prototype.onPressed = function() {
    var mousePosition = this.mouseArea.getMousePosition();
    var nearestPoint = this.computeNearestPoint(mousePosition);
    var nearestPointVector = nearestPoint === null ? mousePosition : nearestPoint.start;

    this.movingPoint = this.mouseArea.createPointUi(nearestPointVector)

    /*
     * if point is not on the sketch, then it's a fake one,
     * then it's time to insert it on the sketch, remove
     * the line on it and insert two new lines
     */
    if(!this.sketch.pointExists(this.movingPoint.start)) {
        var correspondingFakePointIndex = _.findIndex(this.fakePoints, function(point) {
            return this.sketch.comparePoint(point.start, this.movingPoint.start)
        }, this);

        if(correspondingFakePointIndex === -1) {
            console.error("fake point doesn't exist in point collection")
        }
        else {
            var correspondingFakePoint = this.fakePoints[correspondingFakePointIndex];
            this.sketch.insertIntermediatePoint(correspondingFakePoint.lineUi);

            this.fakePoints = this.fakePoints.filter(function(x) { return x !== correspondingFakePoint });
            console.assert(this.fakePoints.length === countBefore - 1)

            var newFakePoints = this.sketch.linesRelatedToPosition(correspondingFakePoint.start).map(this.fakePointMapper, this);

            this.fakePoints = [].concat(this.fakePoints, newFakePoints)

            correspondingFakePoint.destroy()
        }
    }

}

MoveTool.prototype.onPositionChanged = function() {
    var newPosition = this.mouseArea.getMousePosition()

    // it should send store update
    this.sketch.movePoint(this.movingPoint, newPosition)
    this.movingPoint.setStart(newPosition)
}

MoveTool.prototype.onReleased = function() {
    this.movingPoint.destroy();
    this.movingPoint = null;
}

MoveTool.prototype.onEnterTool = function() {
    // insert fakes points
    // we need to use lines on the UI not in the model (to enable auto update)
    // not : this.sketch.getLines().map...
    this.fakePoints = this.sketch.getLines().map(this.fakePointMapper, this);

    // reset the handle point
    this.movingPoint = null;
}

MoveTool.prototype.onLeaveTool = function() {
    if(this.movingPoint !== null) {
        this.movingPoint.destroy();
        this.movingPoint = null;
    }

    this.fakePoints.forEach(function(point) { point.destroy(); });
    this.fakePoints = [];
}

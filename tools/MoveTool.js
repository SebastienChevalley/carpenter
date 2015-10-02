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
        var point = this.sketch.components.intermediatePoint.createObject(this.mouseArea.parent, { 'line': line, 'lineUi': lineUi, 'start': line.computeIntermediatePoint() });
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
        var lineUi = this.mouseArea.lines[line.identifier]
        var point = this.createIntermediatePoint(line, lineUi);

        // link point inthe UI
        lineUi.intermediatePoint = point

        return point;
    }
}

MoveTool.prototype = Object.create(Tool.prototype);
MoveTool.prototype.constructor = MoveTool;

MoveTool.prototype.onPressed = function() {
    var mousePosition = this.mouseArea.getMousePosition();
    var nearestPoint = this.computeNearestPoint(mousePosition);
    var isNearestPoint = nearestPoint !== null;
    var nearestPointVector = !isNearestPoint ? mousePosition : nearestPoint.start;

    /*
     * if point is not on the sketch, then it's a fake one,
     * then it's time to insert it on the sketch, remove
     * the line on it and insert two new lines
     */
    if(isNearestPoint) {
        this.movingPoint = this.mouseArea.createPointUi(nearestPointVector)
        this.movingPoint.identifier = isNearestPoint ? nearestPoint.identifier : -1;

        var correspondingFakePoint = _.find(this.fakePoints, function(point) {
            return this.sketch.comparePoint(point.start, this.movingPoint.start)
        }, this);

        if(correspondingFakePoint === undefined) {
            console.error("fake point doesn't exist in point collection")
        }
        else {
            /*
             * Convert the fake to true one and remove the line between the points
             *  and add two new lines
             */
            this.sketch.insertIntermediatePoint(correspondingFakePoint.lineUi);

            this.movingPoint.identifier = this.sketch.pointByVector(correspondingFakePoint.start).identifier

            /*
             *  Remove the old fake point and add the two new created to the collection
             */
            this.fakePoints = this.fakePoints.filter(function(x) { return x !== correspondingFakePoint });

            var newFakePoints = this.sketch.linesRelatedToPosition(correspondingFakePoint.start).map(this.fakePointMapper, this);
            this.fakePoints = [].concat(this.fakePoints, newFakePoints)

            correspondingFakePoint.destroy()
        }
    }



}

MoveTool.prototype.onPositionChanged = function() {
    if(this.movingPoint !== null) {
        /*
         * Try to find some point to stick on, if exists,
         * show to the user it can be merged
         */
        var newPosition = this.mouseArea.getMousePosition()
        var nearestPoint = this.computeNearestPoint(newPosition)
        var isPointToStick = nearestPoint !== null;
        //newPosition = isPointToStick ? nearestPoint.start : newPosition;

        this.sketch.movePoint(this.movingPoint.identifier, newPosition)
        this.movingPoint.setStart(newPosition)
    }
}

MoveTool.prototype.onReleased = function() {
    if(this.movingPoint !== null) {

        this.movingPoint.destroy();
        this.movingPoint = null;
    }
}

MoveTool.prototype.onEnterTool = function() {
    // insert fakes points
    // we need to use lines on the UI not in the model (to enable auto update)
    this.fakePoints = this.sketch.getLines().map(this.fakePointMapper, this);

    // reset the handle point
    this.movingPoint = null;
}

MoveTool.prototype.onLeaveTool = function() {
    if(this.movingPoint !== null)
        this.movingPoint.destroy();

    this.fakePoints.forEach(function(point) { point.destroy(); });
    this.fakePoints = [];
}

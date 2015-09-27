Qt.include("Tool.js")

function MoveTool(context) {
    Tool.call(this, context);

    console.log('Build a MoveTool');

    this.fakePoints = [];
    this.movingPoint = null;
}

MoveTool.prototype = Object.create(Tool.prototype);
MoveTool.prototype.constructor = MoveTool;

MoveTool.prototype.onPressed = function() {
    var nearests = this.nearestPoints(this.getMousePosition());

    if(nearests.length > 0) {
        // take the nearest.
        this.movingPoint = nearests[0];
    }
}


MoveTool.prototype.onPositionChanged = function() {
    var lines = this.sketch.lines;
    var parent = this.mouseArea.parent;
    var newPoint = this.getMousePosition();
    var oldPoint = Qt.vector2d(this.movingPoint.start.x, this.movingPoint.start.y);

    function updateLines(lines, point, newPoint) {
        return function (direction) {
            if(direction !== 'end' && direction !== 'start') {
                throw new Error("direction can only be start or end");
            }

            var found = lines.filter(function(line) { return line[direction].fuzzyEquals(point) })
            console.log(found);
            found.forEach(function(line) {
                line[direction] = newPoint
            });
        }
    }

    console.log('oldPoint', oldPoint);

    if(this.movingPoint !== null) {
        if(this.fakePoints.indexOf(this.movingPoint) !== -1) {
            var movingPoint = this.movingPoint;

            // get start and stop from current line
            var start = movingPoint.line.start;
            var end = movingPoint.line.end;
            var point = movingPoint.start;

            // remove the line between start and stop
            this.sketch.lines = lines.filter(function(line) { return line !== movingPoint.line })
            // remove it from sketch
            movingPoint.line.destroy();

            // add two lines [start, inter], [inter, stop]
            var newLines = [
                this.components.lineUi.createObject(parent, { 'start': start, 'end' : point }),
                this.components.lineUi.createObject(parent, { 'start': point, 'end' : end })
            ]

            newLines.forEach(function(line) {
                this.sketch.lines.push(line);
                this.createIntermediatePointForLine(line);
            }, this);

            // remove 'inter' from fakePoints
            var filterPoint = function(point) { return point !== movingPoint };
            this.fakePoints = this.fakePoints.filter(filterPoint);
            this.sketch.points = this.sketch.points(filterPoint);

            // insert a new point
            movingPoint.destroy();
            this.movingPoint = this.components.insertPoint.createObject(parent, { 'start' : point });
        }

        console.log('oldPoint', oldPoint);
        console.log('--------------------')

        // update connected line
        var lineChanger = updateLines(this.sketch.lines, oldPoint, newPoint);

        ['end', 'start'].forEach(function(direction) { lineChanger(direction) })

        this.movingPoint.setStart(newPoint);

        console.log('points : ', this.sketch.points);
        console.log('lines :  ', this.sketch.lines);
    }
}


MoveTool.prototype.onReleased = function() {
    this.movingPoint = null;
}

MoveTool.prototype.createIntermediatePointForLine = function(line) {
    var parent = this.mouseArea.parent;
    var intermediatePoint = this.components.intermediatePoint;

    var insertedPoint = intermediatePoint.createObject(parent, { 'start': line.computeIntermediatePoint(), 'line': line });
    line.intermediatePoint = insertedPoint;
    this.sketch.points.push(insertedPoint);
    this.fakePoints.push(insertedPoint);
}


MoveTool.prototype.onEnterTool = function() {
    console.log('points on enter before : ', this.sketch.points);

    var lines = this.sketch.lines;

    lines.forEach(this.createIntermediatePointForLine, this)

    console.log('points on enter after : ', this.sketch.points);
}



MoveTool.prototype.onLeaveTool = function() {
    console.log('points on leave before : ', this.sketch.points);

    var points = this.sketch.points;
    var fakePoints = this.fakePoints;

    this.sketch.points = points.filter(function(point) { return fakePoints.indexOf(point) === -1 }, this)
    this.fakePoints.forEach(function(pointToDelete) { pointToDelete.destroy() })
    this.fakePoints = [];

    console.log('points on leave after : ', this.sketch.points);
}


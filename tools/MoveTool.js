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

function updateLines(lines, point, newPoint) {
    return function (direction) {
        if(direction !== 'end' && direction !== 'start') {
            throw new Error("direction can only be start or end");
        }

        lines
            .filter(function(line) { return line[direction].fuzzyEquals(point) })
            .forEach(function(line) {
                line[direction] = newPoint
            });
    }
}

MoveTool.prototype.onPositionChanged = function() {
    if(this.movingPoint !== null) {
        var lines = this.sketch.lines;
        var parent = this.mouseArea.parent;
        var newPoint = this.getMousePosition();
        var oldPoint = Qt.vector2d(this.movingPoint.start.x, this.movingPoint.start.y);

        if(this.fakePoints.indexOf(this.movingPoint) !== -1) {
            var movingPoint = this.movingPoint;

            // get start and stop from current line
            var start = movingPoint.line.start;
            var end = movingPoint.line.end;
            var point = movingPoint.start;

            // add two lines [start, inter], [inter, stop]
            [ [start, point], [point, end] ].forEach(function(component) {
                var line = this.components.lineUi.createObject(parent, {  'start': component[0], 'end' : component[1]  })
                this.sketch.lines.push(line);
                this.createIntermediatePointForLine(line);
            }, this);

            // removal
            this.sketch.lines = lines.filter(function(line) { return line !== movingPoint.line })
            movingPoint.line.destroy();

            var filterPoint = function(point) { return point !== movingPoint };
            this.fakePoints = this.fakePoints.filter(filterPoint);
            this.sketch.points = this.sketch.points.filter(filterPoint);

            // insert a new point after removed actual one
            movingPoint.destroy();
            this.movingPoint = this.components.insertPoint.createObject(parent, { 'start' : point });
            this.sketch.points.push(this.movingPoint)
        }

        // update connected line
        var lineChanger = updateLines(this.sketch.lines, oldPoint, newPoint);
        ['end', 'start'].forEach(function(direction) { lineChanger(direction) })

        this.movingPoint.setStart(newPoint);
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


Qt.include("Tool.js")

function MoveTool(context) {
    Tool.call(this, context);

    console.log('Build a MoveTool');
}

MoveTool.prototype = Object.create(Tool.prototype);
MoveTool.prototype.constructor = MoveTool;

MoveTool.prototype.onPressed = function() {
    var nereast = nereastPoints(getMousePosition());

    if(nereast.length > 0) {
        // take the nearest.
        movingPoint = nearest[0];
    }
}


MoveTool.prototype.onPositionChanged = function() {
    if(movingPoint !== null) {

    }
}

MoveTool.prototype.onReleased = function() {
    movingPoint = null;
}

MoveTool.prototype.onEnterTool = function() {
    var intermediatePoint = this.components.intermediatePoint;
    var parent = this.mouseArea.parent;
    var lines = this.sketch.lines;

    lines.forEach(function(line) {
        lines.push(intermediatePoint.createObject(parent, { 'start': line.intermediatePoint() }))
    })
}
MoveTool.prototype.onLeaveTool = function() { console.log('leave move tool') }



var honk = new MoveTool();

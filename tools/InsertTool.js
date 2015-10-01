Qt.include("Tool.js")

function InsertTool(context) {
    //console.log('Build an InsertTool');
    console.log('context2', context);

    Tool.call(this, context);

    /**
      * UI Component involved in this tool
      */
    this.startingPoint = null;
    this.endingPoint = null;
    this.insertLine = null;

    /**
      * UI components class
      */

    /**
      * Return a vector2d to indicate the starting point of the new line
      */
    this.computeNearestPoint = function(position) {
        var nearestPoints = this.sketch.nearestPoints(position)

        return nearestPoints.length === 0 ? position : nearestPoints[0].start;
    }

    /**
      * Create a new "InsertLine" from starting to ending point
      */
    this.createInsertLine = function() {
        var newLine = this.sketch.components.insertLine.createObject(
                 this.mouseArea,
                 { 'startPoint': this.startingPoint, 'endPoint' : this.endingPoint }
        );

        return newLine
    }
}

InsertTool.prototype = Object.create(Tool.prototype);
InsertTool.prototype.constructor = InsertTool;

InsertTool.prototype.onPressed = function() {
    var mousePosition = this.mouseArea.getMousePosition();

    this.startingPoint = this.mouseArea.createPointUi(this.computeNearestPoint(mousePosition));
    this.endingPoint = this.mouseArea.createPointUi(mousePosition);
    this.insertLine = this.createInsertLine();

    //this.endingPoint.visible = this.insertLine.visible;
    this.endingPoint.visible = true;

}

InsertTool.prototype.onPositionChanged = function() {
    this.endingPoint.setStart(this.computeNearestPoint(this.mouseArea.getMousePosition()));
}

InsertTool.prototype.onReleased = function() {

    this.insertLine.destroy();
    this.sketch.addLineAndPoints(this.startingPoint.start, this.endingPoint.start);

    this.startingPoint.destroy();
    this.endingPoint.destroy();
}

InsertTool.prototype.onEnterTool = function() {
   console.log('on enter insert', this.sketch.points);
}
InsertTool.prototype.onLeaveTool = function() {
}

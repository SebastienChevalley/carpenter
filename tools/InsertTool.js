Qt.include("Tool.js")
Qt.include("qrc:/lib/lib/lodash.js");

var _ = lodash(this);

function InsertTool(context) {
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
    this.computeNearestPoint = function(position, distance) {
        // we omit the starting point if it exists to compute nearest points
        var collection = this.sketch.getPoints().filter(function(point) {
            var start = this.startingPoint;

            return  start.start === undefined ? true : !(start.start.fuzzyEquals(point.start))
        }, this);

        var nearestPoints = this.sketch.nearestPoints(position, collection)

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

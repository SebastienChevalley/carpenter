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
    this.insertLineComponent = Qt.createComponent('qrc:/Line.qml');

    /**
      * Return a vector2d to indicate the starting point of the new line
      */
    this.computeStartingPoint = function(position) {
        var nearestPoints = this.sketch.nearestPoints(position)

        return nearestPoints.length === 0 ? position : nearestPoints[0];
    }

    /**
      * Create a new "InsertLine" from starting to ending point
      */
    this.createInsertLine = function() {
        this.insertLine = this.insertLineComponent.createObject(
                 this.mouseArea.parent,
                 { 'start': this.startingPoint, 'end' : this.endingPoint }
        );
    }
}

InsertTool.prototype = Object.create(Tool.prototype);
InsertTool.prototype.constructor = InsertTool;

InsertTool.prototype.onPressed = function() {
    var parent = this.mouseArea.parent;


    console.log(Qt.vector2d(1,2));

    var mousePosition = this.getMousePosition();
    var properties = { start: mousePosition, end: mousePosition };


    var distanceToMousePosition = this.nearestPoints(mousePosition);

    var insertedNew = distanceToMousePosition.length === 0;
    this.startingPoint = insertedNew ? this.components.insertPoint.createObject(parent, properties) : distanceToMousePosition[0];
    var startingPoint = this.startingPoint;

    var guideProperties = properties;
    // FIXME sharing of points is bad
    guideProperties.start = this.startingPoint.start;
    //guideProperties.points = points;


    // creating the ending point, only visible when insertable
    this.endingPoint = this.components.insertPoint.createObject(parent, properties);
    var endingPoint = this.endingPoint;

    this.insertLine = this.components.insertLine.createObject(parent, properties)
    var insertLine = this.insertLine;

    // visibility need guide properties /2.0
    endingPoint.visible = insertLine.isInsertable;
    startingPoint.visible =  true;


    // keep starting point because we're sure it will be on the schema
    if(insertedNew && !startingPoint.isContainsIn(this.sketch.points)) {
        console.log('will be inserted');
        this.sketch.points.push(startingPoint);
        //this.sketch.addPoint(startingPoint);
    }
}

/*InsertTool.prototype.onPressedNew = function() {
    this.startingPoint = this.computeStartingPoint;
    this.endingPoint = this.sketch.addPoint(mousePosition);
    this.insertLine = this.createInsertLine();
    this.endingPoint.visible = this.insertLine.visible;
}*/

InsertTool.prototype.onPositionChanged = function() {
    if(!this.mouseArea.containsMouse) {
        return;
    }

    var points = this.sketch.points;
    var endingPoint = this.endingPoint;
    var insertLine = this.insertLine;

    var mousePosition = this.getMousePosition();
    var nearest = this.nearestPoints(mousePosition);
    var isExistingPoints = points.indexOf(endingPoint) !== -1;
    var insertPoint = this.components.insertPoint;
    var parent = this.mouseArea.parent;

    this.endingPoint = (function() {
        // there's a point to magnet
        if(nearest.length > 0) {
            // if the selected point doesn't exist and is not stored in database, remove it from ui
            if(!isExistingPoints && !endingPoint.isContainsIn(points))
                endingPoint.destroy();

            // and use the point to magnet
            return nearest[0]
        }
        else {
            // if was existing before, creating a new component
            if(isExistingPoints) {
                return insertPoint.createObject(parent, { 'start' : mousePosition, 'visible': false });
            }
            // else, just update the existing point
            else
                return endingPoint.setStart(mousePosition);
        }
    })();

    insertLine.end = endingPoint.start;
    this.mouseArea.cursorShape = insertLine.isInsertable ? Qt.DragCopyCursor : Qt.ForbiddenCursor;
    endingPoint.visible = isExistingPoints ? true : insertLine.isInsertable;
}

/*InsertTool.protototype.onPositionChangedNew = function() {
    if(!this.mouseArea.containsMouse) {
        return;
    }

    this.endingPoint = (function() {
        var nearestPoints = this.sketch.nearestPoints(position)
        var existPointToStickWith = nearestPoints.length !== 0
        var nearestPoint = existPointToStickWith ? nearestPoints[0] : null;
        var currentEndingPointIsVirtual = !this.sketch.existPoints(this.endingPoint);

        if(existPointToStickWith) {
            if(currentEndingPointIsVirtual) {
                this.endingPoint.destroy();
            }

            return nearestPoint;
        }
        else {
            var mousePosition = this.mousePosition();

            if(!currentEndingPointIsVirtual) {
                return this.sketch.components.insertPoint.createObject(this.mouseArea.parent, { 'start': mousePosition, 'visible' : false });
            }
            else {
                return this.endingPoint.setStart(mousePosition);
            }
        }
    })();

    this.insertLine.end = endingPoint.start;
    this.endingPoint.visible = this.sketch.existPoints(this.endingPoint) ? true : this.insertLine.isInsertable;
}*/

InsertTool.prototype.onReleased = function() {
    var startingPoint = this.startingPoint;
    var endingPoint = this.endingPoint;
    var insertLine = this.insertLine;
    var lineUiComponent = this.components.lineUi;
    var points = this.sketch.points;
    var lines = this.sketch.lines;

    // only keep the ending point if it was insertable at the time of release
    if(insertLine.isInsertable) {
        if(!endingPoint.isContainsIn(points)) {
            points.push(endingPoint);
        }

        if(!insertLine.isContainsIn(lines)) {
            lines.push(lineUiComponent.createObject(this.mouseArea.parent, { 'start': startingPoint.start, 'end': endingPoint.start }));
        }
    }

    insertLine.destroy();
    this.mouseArea.cursorShape = Qt.ArrowCursor;

}

InsertTool.prototype.onEnterTool = function() {
   console.log('on enter insert', this.sketch.points);
}
InsertTool.prototype.onLeaveTool = function() {
}

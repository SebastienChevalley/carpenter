Qt.include("Tool.js")

function InsertTool(context) {
    //console.log('Build an InsertTool');
    console.log('context2', context);

    Tool.call(this, context);

    this.startingPoint = null;
    this.endingPoint = null;
}

InsertTool.prototype = Object.create(Tool.prototype);
InsertTool.prototype.constructor = InsertTool;

InsertTool.prototype.onPressed = function() {
    var parent = this.mouseArea.parent;

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
    }
}

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
        if(nearest.length > 0) {
            if(!isExistingPoints && !endingPoint.isContainsIn(points))
                endingPoint.destroy();

            return nearest[0]
        }
        else {
            if(isExistingPoints) {
                return insertPoint.createObject(parent, { 'start' : mousePosition, 'visible': false });
            }
            else
                return endingPoint.setStart(mousePosition);
        }
    })();

    insertLine.end = endingPoint.start;
    this.mouseArea.cursorShape = insertLine.isInsertable ? Qt.DragCopyCursor : Qt.ForbiddenCursor;
    endingPoint.visible = isExistingPoints ? true : insertLine.isInsertable;
}

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

InsertTool.prototype.onEnterTool = function() { console.log('enter insert tool') }
InsertTool.prototype.onLeaveTool = function() { console.log('leave insert tool') }

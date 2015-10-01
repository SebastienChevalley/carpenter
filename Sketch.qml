import QtQuick 2.0
import "."
import "qrc:/lib/lib/lodash.js" as Lodash

/**
  * Contains the document on the interface
  */
Item {
    id: sketch;


    property var _;
    property var history: [];

    property var store: { 'points': [], 'lines': [] };


    property Item components : Item {
        id: components

        property var point : Qt.createComponent("Point.qml");
        property var insertPoint : Qt.createComponent("InsertPoint.qml");
        property var intermediatePoint : Qt.createComponent("IntermediatePoint.qml");
        property var line: Qt.createComponent("Line.qml");
        property var lineUi : Qt.createComponent("LineUi.qml");
        property var insertLine : Qt.createComponent("InsertLine.qml");
    }

    Component.onCompleted: {

        this._ = Lodash.lodash(this);
        console.log(this._)
        console.log(_.assign)
    }

    /*
     * Events
     */
    signal addPoint(vector2d position)
    signal pointInserted(vector2d point)

    onAddPoint: {
        updateStore(addPointReducer(store, position))
    }

    function addPointReducer(store, position) {
        if(pointExists(position)) {
            console.error("Point already in sketch",  position);
            return store;
        }
        else {
            pointInserted(position);
            return _.assign({}, store, { 'points': [].concat(store.points, [createPoint(position)]) });
        }
    }

    signal removePoint(vector2d position)
    signal pointRemoved(vector2d point)

    onRemovePoint: function(point) {
        updateStore(removePointReducer(store, point))
    }

    function removePointReducer(store, pointToRemove) {
        if(!pointExists(point)) {
            console.error("Point doesn't exist in sketch")
            return store;
        }
        else {
            pointRemoved(pointToRemove)
            return _.assign({}, store, { 'points': store.points.filter(function(point) { return !comparePoint(point, pointToRemove) }) })
        }
    }

    signal movePoint(Point pointToMove, vector2d to)
    signal pointMoved(vector2d point, vector2d to)

    onMovePoint: {
        updateStore(movePointReducer(store, pointToMove, to))
    }

    function movePointReducer(store, pointToMove, to) {
        if(!pointExists(pointToMove.start)) {
            console.error("Point doesn't exist in sketch")
            return store;
        }
        else {
            var oldPoint = pointToMove.start;
            var indexOfPoint = this.indexOfPoint(store, oldPoint);

            if(indexOfPoint === -1) {
                console.error("Point not in the setch")
                return store;
            }
            else {
                var newPoint = createPoint(to)
                var newPoints = [].concat(store.points.slice(0, indexOfPoint), [newPoint], store.points.slice(indexOfPoint + 1, store.points.length))
                pointMoved(oldPoint, to)

                // we should update the line too
                var newStore = updatePointInLines(store, oldPoint, newPoint)

                return _.assign({}, newStore, { 'points': newPoints });
            }
        }
    }

    // Line related
    signal addLine(vector2d start, vector2d end)
    signal addLineAndPoints(vector2d start, vector2d end)
    signal lineAdded(Line line)

    onAddLine: {
        updateStore(addLineReducer(store, start, end))
    }

    onAddLineAndPoints: {
        updateStore(addLineAndPointsReducer(store, start, end))
    }

    function addLineReducer(store, start, end) {
        var indexOfStart = indexOfPoint(store, start);
        var indexOfEnd = indexOfPoint(store, end);

        if(indexOfStart === -1) {
            console.error("starting point doesn't exist in sketch")
            return store;
        }
        else if(indexOfEnd === -1) {
            console.error("ending point doesn't exist in sketch")
            return store;
        }
        else if(lineExists(start, end)) {
            console.error("line already exists in sketch")
            return store;
        }
        else {
            var newLine = createLine(store.points[indexOfStart], store.points[indexOfEnd])
            lineAdded(newLine)
            return _.assign({}, store, { 'lines': [].concat(store.lines, [newLine]) })
        }
    }

    function addLineAndPointsReducer(store, start, end) {
        var newStore = addPointReducer(store, start);
        newStore = addPointReducer(newStore, end);

        return addLineReducer(newStore, start, end);
    }

    signal removeLine(Line line)
    signal lineRemoved(Line line)

    function removeLineReducer(store, line) {

        if(!lineExists(line.start, line.end)) {
            console.error("line doesn't exist in sketch")
            return store;
        }
        else {
            lineRemoved(line);
            return _.assign({}, store, { 'lines': store.lines.filter(function(line2) { return !compareLine(line2, line) }) })
        }
    }

    signal insertIntermediatePoint(Line line)

    onInsertIntermediatePoint: {
        // compute intermediatePoint
        var intermediatePoint = line.computeIntermediatePoint();
        var start = line.start;
        var end = line.end;

        // remove the line
        var newStore = removeLineReducer(store, line)
        // add intermediate point
        newStore = addPointReducer(newStore, intermediatePoint)
        // add lines
        newStore = addLineReducer(newStore, start, intermediatePoint);
        newStore = addLineReducer(newStore, intermediatePoint, end);

        updateStore(newStore);
    }

    /*
     * Comparator
     */
    function comparePoint(a, b) {
        return a.fuzzyEquals(b)
    }

    function compareLine(a, b) {
        return compareLineToPoints(a, b.start, b.end)
    }

    function compareLineToPoints(line, start, end) {
        return (comparePoint(line.start, start) && comparePoint(line.end, end))
            || (comparePoint(line.end, start) && comparePoint(line.start, end))
    }

    /**
      * @var position: vector2d
      */
    function nearestPoints(position, additionalPointsToConsider) {
        if(additionalPointsToConsider === null || additionalPointsToConsider === undefined) {
            additionalPointsToConsider = [];
        }

        return [].concat(additionalPointsToConsider, store.points)
            .map(function(x) { return { 'point' : x, 'distance' : x.distanceTo(position) } })
            .filter(function(x) { return x.distance < Settings.minimalPointDistance })
            .sort(function(a, b) {
                if(a.distance > b.distance) return 1;
                else if(a.distance < b.distance) return -1;
                else return 0;
            })
            .map(function(x) { return x.point });
    }

    function linesRelatedToPosition(vector) {
        return store.lines.filter(function(line) { return comparePoint(line.start, vector) || comparePoint(line.end, vector) })
    }

    /**
      * @param start vector2d
      * @param end vector2d
      */
    function lineExists(start, end, providedStore) {
        if(providedStore === undefined || providedStore === null) {
            providedStore = store;
        }

        return store.lines.filter(function(line) {
            return compareLineToPoints(line, start, end)
        }).length === 1
    }

    /**
       @param searchPoint vector2d
      */
    function pointExists(searchPoint) {
        var result = store.points.filter(function(point) {
            return comparePoint(searchPoint, point.start);
        }).length === 1;

        return result;
    }

    /**
      * Find the index of a point in the points store collection
      * @param store
      * @param searchPoint vector2d
      */
    function indexOfPoint(store, searchPoint) {
        store = store === undefined ? sketch.store : store;
        return _.findIndex(store.points, function(point) { return comparePoint(searchPoint, point.start); })
    }

    /**
      * @param oldPoint Point
      * @param newPoint Point
      */
    function updatePointInLines(store, oldPoint, newPoint) {
        var updatedLines = store.lines.map(function(line) {
           if(comparePoint(line.startPoint.start, oldPoint)) {
                return createLine(newPoint, line.endPoint)
           }
           else if(comparePoint(line.endPoint.start, oldPoint)) {
                return createLine(line.startPoint, newPoint)
           }
           else {
               // keep the line
               return line;
           }
        });

        return _.assign({}, store, { 'lines': updatedLines })
    }

    // Qt component creation
    function createPoint(start) {
        var newPoint = components.point.createObject(parent, { 'start': start })
        return newPoint;
    }

    function createLine(startPoint, endPoint) {
        var newPoint = components.line.createObject(parent, { 'startPoint': startPoint, 'endPoint': endPoint })
        return newPoint;
    }

    // Store functions
    function updateStore(newStore) {
        history.push(store);
        store = newStore;
    }

    function getLines() {
        return [].concat(store.lines);
    }

    function getPoints() {
        [].concat(store.points);
    }
}


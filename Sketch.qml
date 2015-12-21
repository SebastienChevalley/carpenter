import QtQuick 2.0
import SketchConstraintsSolver 1.0

import "."
import "qrc:/lib/lib/lodash.js" as Lodash

/**
  * Contains the document on the interface
  */
Item {
    id: sketch;

    property real identifier: 0;

    property var _;
    property var history: [];

    property var store: {
        'points': [],
        'lines': [],
        'constraints': [],
        'scale': {
            'set': false,
            'mmPerPixel': 0.0
        }
    };


    property Item components : Item {
        id: components

        property var point : Qt.createComponent("Point.qml");
        property var insertPoint : Qt.createComponent("InsertPoint.qml");
        property var intermediatePoint : Qt.createComponent("IntermediatePoint.qml");
        property var line: Qt.createComponent("Line.qml");
        property var lineUi : Qt.createComponent("LineUi.qml");
        property var insertLine : Qt.createComponent("InsertLine.qml");
    }

    property SketchConstraintsSolver constraintsSolver: SketchConstraintsSolver { }

    Component.onCompleted: {

        this._ = Lodash.lodash(this);
        console.log(this._)
        console.log(_.assign)

        constraintsSolver.setSketch(this);
    }

    /*
     * Events
     */
    signal addPoint(vector2d position)
    signal pointInserted(vector2d point, real identifier)

    onAddPoint: {
        updateStore(addPointReducer(store, position))
    }

    function addPointReducer(store, position) {
        if(pointExists(position)) {
            console.error("Point already in sketch",  position);
            return store;
        }
        else {
            var newPoint = createPoint(position);
            pointInserted(position, newPoint.identifier);
            return _.assign({}, store, { 'points': [].concat(store.points, [newPoint]) });
        }
    }

    signal removePoint(real identifier)
    signal pointRemoved(real identifier)

    onRemovePoint: {
        updateStore(removePointReducer(store, identifier))
    }

    function removePointReducer(store, id) {
        if(!pointExistsById(id)) {
            console.error("Point doesn't exist in sketch")
            return store;
        }
        else {
            pointRemoved(id)
            return _.assign({}, store, { 'points': store.points.filter(function(point) { return point.identifier !== id }) })
        }
    }

    signal movePoint(real identifier, vector2d to)
    signal movePoints(var pointsToMove)
    signal pointMoved(real identifier, vector2d to)

    onMovePoint: {
        updateStore(movePointReducer(store, identifier, to))
    }

    onMovePoints: {
        console.log("constrained", store.lines.filter(function(x) { return x.verticallyConstrained || x.horizontallyConstrained; }))

        // TODO : merge points if they fall in same place (degenerate situation)

        var newStore = store;
        for (var identifier in pointsToMove) {
            newStore = movePointReducer(newStore, parseInt(identifier, 10), pointsToMove[identifier]);
        }

        updateStore(newStore);
    }

    function movePointReducer(store, id, to) {
        if(!pointExistsById(id, store)) {
            console.error("Point doesn't exist in sketch")
            return store;
        }
        else {
            var oldPoint = _.find(store.points, function(point) { return point.identifier === id });
            var indexOfPoint = this.indexOfPoint(store, id);

            if(indexOfPoint === -1) {
                console.error("Point not in the setch")
                return store;
            }
            else {
                var newPoint = createPoint(to, id)
                var newPoints = [].concat(store.points.slice(0, indexOfPoint), [newPoint], store.points.slice(indexOfPoint + 1, store.points.length))
                pointMoved(id, to)

                // we should update the line too
                var newStore = updatePointInLines(store, id, newPoint)

                return _.assign({}, newStore, { 'points': newPoints });
            }
        }
    }

    // Line related
    signal addLine(real idStart, real idEnd)
    signal addLineAndPoints(vector2d start, vector2d end)
    signal lineAdded(Line line)

    onAddLine: {
        updateStore(addLineReducer(store, idStart, idEnd))
    }

    onAddLineAndPoints: {
        updateStore(addLineAndPointsReducer(store, start, end))
    }

    function addLineReducer(store, idStart, idEnd) {
        var indexOfStart = indexOfPoint(store, idStart);
        var indexOfEnd = indexOfPoint(store, idEnd);

        if(indexOfStart === -1) {
            console.error("starting point doesn't exist in sketch")
            return store;
        }
        else if(indexOfEnd === -1) {
            console.error("ending point doesn't exist in sketch")
            return store;
        }
        else {
            var startPoint = store.points[indexOfStart]
            var endPoint = store.points[indexOfEnd]

            if(lineExistsByVector(startPoint.start, endPoint.start, store)) {
                console.error("line already exists in sketch")
                return store;
            }
            else {
                var newLine = createLine(startPoint, endPoint)
                lineAdded(newLine)
                return _.assign({}, store, { 'lines': [].concat(store.lines, [newLine]) })
            }
        }
    }

    function addLineAndPointsReducer(store, start, end) {
        var newStore = addPointReducer(store, start);

        if(start.fuzzyEquals(end)) {
            console.error("addLineAndPointsReducer")
        }
        else {
            newStore = addPointReducer(newStore, end);
            newStore = addLineReducer(newStore, pointByVector(start, newStore).identifier, pointByVector(end, newStore).identifier);
        }

        return newStore;
    }

    signal removeLine(real identifier)
    signal lineRemoved(real identifier)

    function removeLineReducer(store, identifier) {
        if(!lineExistsById(identifier, store)) {
            console.error("line doesn't exist in sketch")
            return store;
        }
        else {
            lineRemoved(identifier);
            return _.assign({}, store, { 'lines': store.lines.filter(function(line) { return line.identifier !== identifier }) })
        }
    }

    signal insertIntermediatePoint(Line line)

    onInsertIntermediatePoint: {
        // compute intermediatePoint
        var intermediatePoint = line.computeIntermediatePoint();
        var startId = line.startPoint.identifier;
        var endId = line.endPoint.identifier;

        // remove the line
        var newStore = removeLineReducer(store, line.identifier)
        // add intermediate point
        newStore = addPointReducer(newStore, intermediatePoint)

        var intermediatePointId = pointByVector(intermediatePoint, newStore).identifier

        newStore = addLineReducer(newStore, startId, intermediatePointId);
        newStore = addLineReducer(newStore, intermediatePointId, endId);

        updateStore(newStore);
    }

    signal mergeTwoPoint(Point thisOne, Point withThis)

    onMergeTwoPoint: {
        console.log("ON MERGE TWO POINT")
        // get each lines concerned with this point
        var lines = linesRelatedToId(thisOne.identifier)
        var newStore = store

        if(lines.length === 0) {
            //console.error("no related lines with this point")
        }
        else {
            // make a couple with each endPoint and the newPoint
            var couples = lines
                .map(function(line) {
                    if(line.startPoint.identifier === thisOne.identifier) {
                        return [withThis, line.endPoint];
                    }
                    else {
                        //console.assert(comparePoint(line.end, thisOne.start), "MERGE TWO POINT : line related to point but doesn't match with extremities")
                        return [line.startPoint, withThis];
                    }
                })
                .filter(function(couple) { return couple[0].identifier !== couple[1].identifier && couple[0].start !== couple[1].start })
                .filter(function(couple) { return !lineExists(couple[0].identifier, couple[1].identifier) }, this)

            console.log("LINES TO REMOVE", lines)

            // remove the lines
            newStore = lines.reduce(function(store, line) { return removeLineReducer(store, line.identifier) }, store)

            // insert new lines
            newStore = couples.reduce(function(store, couple) { return addLineReducer(store, couple[0].identifier, couple[1].identifier) }, newStore)
        }

        // we always want to remove
        newStore = removePointReducer(newStore, thisOne.identifier)
        console.assert(newStore.points.length === store.points.length - 1, "MERGE TWO POINT :merged point wasn't removed correctly")

        updateStore(newStore)
    }

    signal setInitialScale(real line, real mmLength)

    onSetInitialScale: {
        console.log("received setInitialScale(", line, mmLength, ")")
        updateStore(_.assign({}, store, { 'scale' : { 'set': true, 'mmPerPixel': mmLength / line } }))
        console.log("store.scale", JSON.stringify(store.scale))
    }

    function isScaleSet() {
        return store.scale.set === true;
    }

    signal verticallyConstrainLine(real identifier, bool constrain)

    onVerticallyConstrainLine: {
        var index = indexOfLine(store, identifier);
        store.lines[index].verticallyConstrained = constrain;
    }

    signal horizontallyConstrainLine(real identifier, bool constrain)

    onHorizontallyConstrainLine: {
        var index = indexOfLine(store, identifier);
        store.lines[index].horizontallyConstrained = constrain;
    }

    signal setDesiredDistance(real identifier, real desiredLength)

    onSetDesiredDistance: {
        var index = indexOfLine(store, identifier);
        store.lines[index].distanceFixed = true;
        store.lines[index].desiredDistance = desiredLength;
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

    function compareLineToIdentifier(line, idStart, idEnd) {
        var lineStart = line.startPoint.identifier
        var lineEnd = line.endPoint.identifier
        return (lineStart === idStart && lineEnd    === idEnd)
           || (lineEnd   === idStart && lineStart === idStart)
    }

    /**
      * @var position: vector2d
      */
    function nearestPoints(position, collection) {
        if(collection === null || collection === undefined) {
            collection = store.points;
        }

        //console.log("search on", collection, "near", position)

        return collection
            .map(function(x) { return { 'point' : x, 'distance' : x.distanceTo(position) } })
            .filter(function(x) { return x.distance < Settings.minimalPointDistance })
            .sort(function(a, b) {
                if(a.distance > b.distance) return 1;
                else if(a.distance < b.distance) return -1;
                else return 0;
            })
            .map(function(x) { return x.point });
    }

    function linesRelatedToPosition(vector, store) {
        store = store === undefined ? this.store : store;
        return store.lines.filter(function(line) { return comparePoint(line.start, vector) || comparePoint(line.end, vector) })
    }

    function linesRelatedToId(id, store) {
        store = store === undefined ? this.store : store;
        return store.lines.filter(function(line) { return line.startPoint.identifier === id || line.endPoint.identifier === id })
    }

    /**
      * @param start vector2d
      * @param end vector2d
      */
    function lineExists(idStart, idEnd, providedStore) {
        if(providedStore === undefined || providedStore === null) {
            providedStore = store;
        }

        return providedStore.lines.filter(function(line) {
            return compareLineToIdentifier(line, idStart, idEnd)
        }).length === 1
    }

    function lineExistsByVector(start, end, providedStore) {
        if(providedStore === undefined || providedStore === null) {
            providedStore = store;
        }

        return providedStore.lines.filter(function(line) {
            return compareLineToPoints(line, start, end)
        }).length === 1
    }

    function lineExistsById(id, providedStore) {
        if(providedStore === undefined || providedStore === null) {
            providedStore = store;
        }

        return providedStore.lines.filter(function(line) { return line.identifier === id }).length === 1
    }

    /**
       @param searchPoint vector2d
      */
    function pointExists(searchPoint) {
        var result = store.points.filter(function(point) {
            return comparePoint(searchPoint, point.start);
        }).length > 0;

        return result;
    }

    function pointExistsById(id, store) {
        store = store === undefined ? this.store : store;
        return store.points.filter(function(point) {
            return point.identifier === id
        }).length > 0;
    }

    function pointById(id, store) {
        store = store === undefined ? this.store : store;
        return _.find(store.points, function(point) { return point.identifier === id });
    }

    function pointByVector(vector, store) {
        store = store === undefined ? this.store : store;
        return _.find(store.points, function(point) { return comparePoint(point.start, vector) });
    }

    /**
      * Find the index of a point in the points store collection
      * @param store
      * @param searchPoint vector2d
      */
    function indexOfPoint(store, id) {
        store = store === undefined ? this.store : store;
        return _.findIndex(store.points, function(point) { return point.identifier === id; })
    }

    function indexOfLine(store, id) {
        store = store === undefined ? this.store : store;
        return _.findIndex(store.lines, function(line) { return line.identifier === id; })
    }

    signal lineUpdated(real identifier, Line newLine);

    /**
      * @param oldPoint Point
      * @param newPoint Point
      */
    function updatePointInLines(store, id, newPoint) {
        var updatedLines = store.lines.map(function(line) {
           if(line.startPoint.identifier === id) {
                var newLine = createLine(newPoint, line.endPoint, line.identifier, store)
                lineUpdated(line.identifier, newLine)
                return newLine
           }
           else if(line.endPoint.identifier === id) {
               var newLine = createLine(line.startPoint, newPoint, line.identifier, store)
               lineUpdated(line.identifier, newLine)
               return newLine;
           }
           else {
               // keep the line
               return line;
           }
        });

        return _.assign({}, store, { 'lines': updatedLines })
    }

    // Qt component creation
    function createPoint(start, id) {
        if(id === undefined) {
            id = identifier++;
            console.log("NEW IDENTIFIER : Point", id)
        }

        var newPoint = components.point.createObject(parent, {
                                                         'start': start,
                                                         'identifier': id })
        return newPoint;
    }

    function createLine(startPoint, endPoint, id, providedStore) {
        var newLine = false;
        if(id === undefined) {
            newLine = true;
            id = identifier++;
            console.log("NEW IDENTIFIER : Line", id)
        }
        else {
            var providedStore = providedStore || store
            var oldLine = providedStore.lines[indexOfLine(providedStore, id)]
        }

        var newLine = components.line.createObject(parent, {
            'startPoint': startPoint,
            'endPoint': endPoint,
            'identifier': id,
            'verticallyConstrained': newLine ? false : oldLine.verticallyConstrained,
            'horizontallyConstrained': newLine ? false: oldLine.horizontallyConstrained
        })
        return newLine;
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
        return [].concat(store.points);
    }
}


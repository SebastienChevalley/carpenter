import QtQuick 2.0
import "."

/**
  * Contains the document on the interface
  */
Item {
    /**
      * @var Point[]
      */
    property var points : [];

    /**
      * @var Line[]
      */
    property var lines : [];


    Item {
        id: components

        property var insertPoint : Qt.createComponent("InsertPoint.qml");
        property var intermediatePoint : Qt.createComponent("IntermediatePoint.qml");
        property var lineUi : Qt.createComponent("LineUi.qml");
        property var insertLine : Qt.createComponent("InsertLine.qml");
    }

    /**
      Add a point to the schema
      * @param point: vector2d
      */
    function addPoint(point) {
        var component = components.insertPoint.createObject(parent, { 'start': point });
        points.push(component);

        return component;
    }

    /**
      * @var position: vector2d
      */
    function nearestPoints(position) {
        return points
            .map(function(x) { return { 'point' : x, 'distance' : x.distanceTo(position) } })
            .filter(function(x) { return x.distance < Settings.minimalPointDistance })
            .sort(function(a, b) {
                if(a.distance > b.distance) return 1;
                else if(a.distance < b.distance) return -1;
                else return 0;
            })
            .map(function(x) { return x.point });
    }

    /**
      * @var point: vector2d
      */
    function pointExists(searchPoint) {
        return points.filter(function(point) { return point.start.x === searchPoint.x && point.start.y === searchPoint.y })
                     .length === 1;
    }
}


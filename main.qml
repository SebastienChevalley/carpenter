import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import "."

Window {
    visible: true
    width: 600
    height: 600


    MainForm {
        id: mainForm
        anchors.fill: parent

        property var points : [];

        states: [
            State {
                name: "InsertTool"
                PropertyChanges {
                    target: mouseArea
                    onPressed: { mouseArea.insertTools.onPressed() }
                    onPositionChanged: { mouseArea.insertTools.onPositionChanged() }
                    onReleased: { mouseArea.insertTools.onReleased() }
                }
            },
            State {
                name: "MoveTool"
                PropertyChanges {
                    target: mouseArea
                    onPressed: { mouseArea.insertTools.onPressed() }
                    onPositionChanged: { mouseArea.insertTools.onPositionChanged() }
                    onReleased: { mouseArea.insertTools.onReleased() }
                }
            }
        ]
        state: "InsertTool"

        onStateChanged: {
            if(state === "MoveTool") {
                // generate intermediate points;
                mouseArea.lines.forEach(function(line) { mouseArea.lines.push(line.intermediatePoint()) });
            }
            else {
                // remove intermediate points
                mouseArea.lines = mouseArea.lines.filter(function(x) { return !(x instanceof IntermediatePoint) })
            }
        }

        FontLoader { source: "fonts/Material-Design-Iconic-Font.ttf" }
        FontLoader { source: "fonts/FontAwesome.otf" }

        MenuItem{ id: menuItems }

        function changeTool(newTool) {
            state = newTool
        }

        ToolsMenu {
            anchors.right: mainLayout.right
            id: menu

        }

        ColumnLayout {
            id: mainLayout
            anchors.fill: parent
            spacing: 0

            // todo : put in menubar after
            TopBar {
                id: menuBar
            }

            Rectangle {
                id: sketchArea
                Layout.fillWidth: true
                Layout.fillHeight: true
                MouseArea {
                    id: mouseArea

                    property InsertPoint startingPoint
                    property InsertPoint endingPoint
                    property InsertPoint movingPoint

                    property InsertLine insertLine
                    property var points: []
                    property var lines: []

                    property var insertPoint : Qt.createComponent("InsertPoint.qml");
                    property var lineUiComponent : Qt.createComponent("LineUi.qml");
                    property var insertLineComponent : Qt.createComponent("InsertLine.qml");

                    function getMousePosition() {
                        return Qt.vector2d(mouseX, mouseY);
                    }

                    function nereastPoints(mousePosition) {
                        return points
                            .map(function(x) { return { 'point' : x, 'distance' : x.distanceTo(mousePosition) } })
                            .filter(function(x) { return x.distance < Settings.minimalPointDistance })
                            .sort(function(a, b) {
                                if(a.distance > b.distance) return 1;
                                else if(a.distance < b.distance) return -1;
                                else return 0;
                            })
                            .map(function(x) { return x.point });
                    }

                    property var insertTools : {
                        'onPressed': function() {
                            var drawingLineGuide = Qt.createComponent("DrawingLineGuide.qml");

                            var mousePosition = getMousePosition();
                            var properties = { start: mousePosition, end: mousePosition };


                            var distanceToMousePosition = nereastPoints(mousePosition);

                            var insertedNew = distanceToMousePosition.length === 0;
                            startingPoint = insertedNew ? insertPoint.createObject(parent, properties) : distanceToMousePosition[0];

                            var guideProperties = properties;
                            // FIXME sharing of points is bad
                            guideProperties.start = startingPoint.start;
                            //guideProperties.points = points;


                            // creating the ending point, only visible when insertable
                            endingPoint = insertPoint.createObject(parent, properties);

                            insertLine = insertLineComponent.createObject(parent, properties)

                            // visibility need guide properties /2.0
                            endingPoint.visible = insertLine.isInsertable;
                            startingPoint.visible =  true;


                            // keep starting point because we're sure it will be on the schema
                            if(insertedNew && !startingPoint.isContainsIn(points)) {
                                console.log('will be inserted');
                                points.push(startingPoint);
                            }
                        },
                        'onPositionChanged': function() {
                            if(!containsMouse) {
                                return;
                            }

                            var mousePosition = getMousePosition();
                            var nearest = nereastPoints(mousePosition);
                            var isExistingPoints = points.indexOf(endingPoint) !== -1;

                            endingPoint = (function() {
                                if(nearest.length > 0) {
                                    if(!isExistingPoints && !endingPoint.isContainsIn(points))
                                        endingPoint.destroy();

                                    return nearest[0]
                                }
                                else {
                                    if(isExistingPoints)
                                        return insertPoint.createObject(parent, { 'start' : mousePosition, 'visible': false });
                                    else
                                        return endingPoint.setStart(mousePosition);
                                }
                            })();

                            insertLine.end = endingPoint.start;
                            cursorShape = insertLine.isInsertable ? Qt.DragCopyCursor : Qt.ForbiddenCursor;
                            endingPoint.visible = isExistingPoints ? true : insertLine.isInsertable;
                        },
                        'onReleased': function() {
                            // only keep the ending point if it was insertable at the time of release
                            if(insertLine.isInsertable) {
                                if(!endingPoint.isContainsIn(points)) {
                                    points.push(endingPoint);
                                }


                                if(!insertLine.isContainsIn(lines)) {
                                    lines.push(lineUiComponent.createObject(parent, { 'start': startingPoint.start, 'end': endingPoint.start }));
                                }
                            }

                            insertLine.destroy();
                            cursorShape = Qt.ArrowCursor;

                        }
                    }

                    property var moveTool : {
                        'onPressed': function() {

                            var nereast = nereastPoints(getMousePosition());

                            if(nereast.length > 0) {
                                // take the nearest.
                                movingPoint = nearest[0];
                            }
                        },
                        'onPositionChanged': function() {
                            if(movingPoint !== null) {

                            }
                        },
                        'onReleased': function() {
                            movingPoint = null;
                        }
                    }


                    anchors.fill: parent
                }
            }
        }

    }
}


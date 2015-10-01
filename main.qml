import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import "."
import "qrc:/tools/tools/InsertTool.js" as InsertTool
import "qrc:/tools/tools/MoveTool.js" as MoveTool

import "qrc:/lib/lib/lodash.js" as Lodash


Window {
    visible: true
    width: 600
    height: 600


    MainForm {
        id: mainForm
        anchors.fill: parent

        states: [
            State {
                name: "InsertTool"
                PropertyChanges {
                    target: mouseArea
                    onPressed: { console.log('pressed', mouseArea.insertTool.onPressed); mouseArea.insertTool.onPressed() }
                    onPositionChanged: { mouseArea.insertTool.onPositionChanged() }
                    onReleased: { mouseArea.insertTool.onReleased() }
                }
            },
            State {
                name: "MoveTool"
                PropertyChanges {
                    target: mouseArea
                    onPressed: { mouseArea.moveTool.onPressed() }
                    onPositionChanged: { mouseArea.moveTool.onPositionChanged() }
                    onReleased: { mouseArea.moveTool.onReleased() }
                }
            }
        ]
        state: "InsertTool"
        property string previousTool: ""
        property string currentTool: state;

        property var _;

        Component.onCompleted: {

            this._ = Lodash.lodash(this);
        }

        onCurrentToolChanged: {
            state = currentTool;
        }

        FontLoader { source: "fonts/Material-Design-Iconic-Font.ttf" }
        FontLoader { source: "fonts/FontAwesome.otf" }

        MenuItem{ id: menuItems }

        function toolItem(name) {
            return name.charAt(0).toLowerCase() + name.slice(1);
        }

        function changeTool(newTool) {
            if(newTool !== currentTool) {
                previousTool = currentTool;
                mouseArea[toolItem(previousTool)].onLeaveTool();
                mouseArea[toolItem(newTool)].onEnterTool();

                currentTool = newTool
            }
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

                    property var points: Object.create(Object.prototype)
                    property var lines: Object.create(Object.prototype)

                    property var sketch : Sketch {
                        id:sketch

                        onPointInserted: {
                            mouseArea.points[point.toString()] = mouseArea.createPointUi(point);
                        }
                        onPointRemoved: {
                            mouseArea.points[point.toString()].destroy();
                        }
                        onPointMoved: {
                            var pointUi = mouseArea.points[point.toString()]
                            mouseArea.points = _.omit(mouseArea.points, point.toString());
                            mouseArea.points[to.toString()] = pointUi;
                            pointUi.setStart(to)

                            //var relatedLines = _.values(mouseArea.lines).filter(function(x) { return x.startPoint === pointUi || x.endPoint === pointUi })
                            //console.log("relatedLines", relatedLines)

                            mouseArea.lines = _.mapKeys(mouseArea.lines, function(v, k) { return v.key() });
                        }

                        onLineAdded: {
                            mouseArea.lines[line.key()] = mouseArea.createLineUi(line)
                        }
                        onLineRemoved: {
                            var key = line.key();
                            mouseArea.lines[key].destroy()
                            mouseArea.lines = _.omit(mouseArea.lines, key)
                        }
                    }


                    property var insertPoint : Qt.createComponent("InsertPoint.qml");
                    property var intermediatePoint : Qt.createComponent("IntermediatePoint.qml");
                    property var lineUiComponent : Qt.createComponent("LineUi.qml");
                    property var insertLineComponent : Qt.createComponent("InsertLine.qml");

                    function createPointUi(point) {
                        // todo move ui components outside of sketch class
                        var newPoint = sketch.components.insertPoint.createObject(parent, { 'start': point })
                        return newPoint
                    }

                    function createLineUi(line) {
                        // todo move ui components outside of sketch class
                        var startPoint =  mouseArea.points[line.startPoint.start.toString()]
                        var endPoint =  mouseArea.points[line.endPoint.start.toString()]

                        var properties = { 'startPoint':startPoint, 'endPoint': endPoint }
                        var newLine = sketch.components.lineUi.createObject(parent, properties)
                        return newLine
                    }

                    function getMousePosition() {
                        return Qt.vector2d(mouseX, mouseY);
                    }

                    property var insertTool: new InsertTool.InsertTool(mouseArea);
                    property var moveTool: new MoveTool.MoveTool(mouseArea);

                    anchors.fill: parent
                }
            }
        }

    }
}


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
                            mouseArea.points = Lodash.omit(mouseArea.points, point.toString());
                            mouseArea.points[to.toString()] = pointUi.start.setStart(to);
                        }

                        onLineAdded: {

                            mouseArea.lines[line.key()] = mouseArea.createLineUi(line)
                        }
                        onLineRemoved: {
                            mouseArea.lines[line.key()].destroy()
                            mouseArea.lines = Lodash.omit(mouseArea.lines, line.key())
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

                        var properties = { 'startPoint': line.startPoint, 'endPoint': line.endPoint }
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


import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtMultimedia 5.5
import QtGraphicalEffects 1.0

import "." // to import Settings
import "qrc:/tools/tools/SelectTool.js" as SelectTool
import "qrc:/tools/tools/InsertTool.js" as InsertTool
import "qrc:/tools/tools/MoveTool.js" as MoveTool
import "qrc:/tools/tools/DeleteTool.js" as DeleteTool

import "qrc:/lib/lib/lodash.js" as Lodash
import SketchConverter 1.0
import SketchConstraintsSolver 1.0
import SketchLolExporter 1.0

Window {
    visible: true
    width: Settings.appWidth
    height: Settings.appHeight

    MainForm {
        id: mainForm
        anchors.fill: parent

        property var captureImage : Qt.createComponent("CaptureImage.qml");
        property var captureImagePanel : null;
        property Sketch sketch: sketch;

        states: [
            State {
                name: "SelectTool"
                PropertyChanges {
                    target: mouseArea
                    onPressed: { mouseArea.selectTool.onPressed() }
                    onPositionChanged: { mouseArea.selectTool.onPositionChanged() }
                    onReleased: { mouseArea.selectTool.onReleased() }
                }
            },
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
            },
            State {
                name: "DeleteTool"
                PropertyChanges {
                    target: mouseArea
                    onPressed: { mouseArea.deleteTool.onPressed() }
                    onPositionChanged: { mouseArea.deleteTool.onPositionChanged() }
                    onReleased: { mouseArea.deleteTool.onReleased() }
                }
            }
        ]
        state: Settings.defaultTool
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

        function toolItem(name) {
            return name.charAt(0).toLowerCase() + name.slice(1);
        }

        function changeTool(newTool, name) {
            if(newTool !== currentTool) {
                previousTool = currentTool;
                mouseArea[toolItem(previousTool)].onLeaveTool();
                mouseArea[toolItem(newTool)].onEnterTool();

                // Could be used if you want a message on every tool changes
                // message.displayInfoMessage("Selected tool: " + name);

                currentTool = newTool
            }
        }

        function displayCameraPanel() {
            if(captureImagePanel == null) {
                captureImagePanel = captureImage.createObject(mainForm, {});
            }
            else {
                captureImagePanel.visible = true;
            }
            mouseArea.enabled = false;
        }

        function hideCameraPanel() {
            mouseArea.enabled = true
            captureImagePanel.visible = false
        }

        MenuItem { id: menuItems }
        MessageBox { id: message }

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

                ToolsMenu { id: menu }

                Label {
                    text: "First, you should set the scale by selecting an item, and defines its length"
                    z:100
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 10
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    id: helpTip
                }

                Ruler { id: ruler }

                PointContextMenu { id: pointContextMenu }
                LineContextMenu { id: lineContextMenu }

                Image {
                    id: backgroundImage
                    anchors.fill: parent
                    fillMode: Settings.backgroundFillMode
                    opacity: 0.5
                    source: sketch.isBackgroundSet() ? sketch.getBackground() : ""
                }

                MouseArea {
                    id: mouseArea

                    property var points: Object.create(Object.prototype)
                    property var lines: Object.create(Object.prototype)

                    property var sketch : Sketch {
                        id:sketch

                        onPointInserted: {
                            mouseArea.points[identifier] = mouseArea.createPointUi(point, identifier);
                        }
                        onPointRemoved: {
                            console.log("pointRemoved(id: ", identifier, ")")
                            mouseArea.points[identifier].destroy();
                            mouseArea.points = _.omit(mouseArea.points, identifier)
                        }
                        onPointMoved: {
                            var pointUi = mouseArea.points[identifier]
                            pointUi.setStart(to)
                        }
                        onLineUpdated: {
                           var line = mouseArea.lines[identifier]

                            if(line.intermediatePoint) {
                                line.intermediatePoint.line = newLine
                            }
                        }

                        onLineAdded: {
                            mouseArea.lines[line.identifier] = mouseArea.createLineUi(line)
                        }
                        onLineRemoved: {
                            console.log("lineRemoved(id: ", identifier, ")")
                            mouseArea.lines[identifier].destroy()
                            mouseArea.lines = _.omit(mouseArea.lines, identifier)
                        }

                        onHorizontallyConstrainLine: {
                            mouseArea.lines[identifier].horizontallyConstrained = constrain;
                        }
                        onVerticallyConstrainLine: {
                            mouseArea.lines[identifier].verticallyConstrained = constrain;
                        }

                        onSetInitialScale: {
                            helpTip.text = "Then you can set the distance for each line"
                            lineContextMenu.widthEdit.placeholderText = "Line length"
                        }

                        onSetDesiredDistance: {
                            mouseArea.lines[identifier].distanceFixed = true;
                            mouseArea.lines[identifier].desiredDistance = desiredLength;
                        }

                        onPointReactionUpdated: {
                            mouseArea.points[identifier][constraint] = value
                        }
                    }

                    onClicked: {
                        message.hideMessage()
                    }

                    property var insertPoint : Qt.createComponent("InsertPoint.qml");
                    property var intermediatePoint : Qt.createComponent("IntermediatePoint.qml");
                    property var lineUiComponent : Qt.createComponent("LineUi.qml");
                    property var insertLineComponent : Qt.createComponent("InsertLine.qml");

                    property SketchConverter converter: SketchConverter { }
                    property SketchConstraintsSolver constraintsSolver: SketchConstraintsSolver { sketch: sketch }
                    property SketchLolExporter lolExporter: SketchLolExporter { sketch: sketch }

                    function createPointUi(point, identifier) {
                        // todo move ui components outside of sketch class
                        var newPoint = sketch.components.insertPoint.createObject(parent, { 'start': point, 'identifier': identifier })
                        return newPoint
                    }

                    function createLineUi(line) {
                        // todo move ui components outside of sketch class
                        var startPoint =  mouseArea.points[line.startPoint.identifier]
                        var endPoint =  mouseArea.points[line.endPoint.identifier]

                        var properties = { 'startPoint':startPoint, 'endPoint': endPoint, 'identifier': line.identifier }
                        var newLine = sketch.components.lineUi.createObject(parent, properties)
                        return newLine
                    }

                    function getMousePosition() {
                        return Qt.vector2d(mouseX, mouseY);
                    }

                    property var selectTool: new SelectTool.SelectTool(mouseArea);
                    property var insertTool: new InsertTool.InsertTool(mouseArea);
                    property var moveTool: new MoveTool.MoveTool(mouseArea);
                    property var deleteTool: new DeleteTool.DeleteTool(mouseArea);

                    property PointContextMenu pointContextMenu: pointContextMenu;
                    property LineContextMenu lineContextMenu: lineContextMenu;

                    property ToolsMenu toolsMenu: menu;

                    anchors.fill: parent
                }
            }

        }

    }
}


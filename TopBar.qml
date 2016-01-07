import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtMultimedia 5.5
import QtQuick.Controls.Styles 1.4
import "." // to import Settings

Rectangle {
    id: menuBar
    Layout.fillWidth: true
    color: "transparent"
    height:60
    z: 1000

    RowLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true

        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.fill: parent

        Label {
            Layout.fillWidth: true
            Layout.minimumWidth: 100
            text: "Sketch"
            color: Settings.topBarLabelColor
            font.pointSize: 18
        }

        Button {
            text: "Set background"
            style: RoundedButton {
                icon: "\uf030"
            }

            onClicked: {
                console.log(QtMultimedia.availableCameras);
                //mainForm.displayCameraPanel()
            }
        }

        Button {
            text:"Apply constraints"
            style: RoundedButton {
                icon: "\uf021"
            }

            onClicked: {
                var solveResult = mouseArea.constraintsSolver.solve()
                console.log("solveResult: ", solveResult);
                if(solveResult === true) {
                    mouseArea.constraintsSolver.applyOnSketch();
                    message.displaySuccessMessage("Constraints application succeed")
                }
                else {
                    message.displayErrorMessage("Constraints application failed : " + solveResult)
                }
            }
        }

        Button {
            text:"Export"
            style: RoundedButton {
                icon: "\uf1b2"
            }

            onClicked: {
                var lolExportResult = mouseArea.lolExporter.exportToFile("model");
                console.log("lolExportResult", lolExportResult);

                var exportResult = mouseArea.converter.exportToFile(sketch, "./output.dae");
                console.log("exportResult: ", exportResult);

                if(exportResult === true) {
                    message.displaySuccessMessage("3D export succeed")
                }
                else {
                    message.displayErrorMessage("3D export failed : " + exportResult);
                }
            }
        }
    }
}




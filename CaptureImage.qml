import QtQuick 2.0
import QtMultimedia 5.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

import "." // to import Settings

Rectangle {
    anchors.fill: parent
    z:900
    color: Settings.captureImagePanelBackground
    id: capturePanel

    property bool previewPaneVisible: false;

    onVisibleChanged: {
        if(visible) {
            camera.start()
            previewPaneVisible = false
        }
    }

    Rectangle {
        anchors.centerIn: parent
        color: "#20FFFFFF"
        border.color: "#80FFFFFF"
        radius:5;
        z:100
        width: childrenRect.width + 20;
        height: childrenRect.height + 20;
        visible: !previewPaneVisible

        Label {
            text: "Touch the screen to take a picture"
            color: "#ffffff"
        }
    }

    Camera {
        id: camera

        imageCapture {
            resolution: Qt.size(1920,1080)
            onImageCaptured: {
                photoPreview.source = preview
                previewPaneVisible = true
            }
            onImageSaved: {
                console.log("preview", camera.imageCapture.capturedImagePath)

            }
        }
    }

    VideoOutput {
        id: videoOutput
        source: camera
        anchors.fill: parent
        fillMode: Settings.backgroundFillMode

        MouseArea {
            anchors.fill: parent
            onPressed: {
                console.log("resolution", camera.imageCapture.resolution)
                camera.imageCapture.captureToLocation(appPath)
            }
        }
    }

    Rectangle {
        id: previewPane
        visible: previewPaneVisible
        anchors.fill: parent

        Image {
            id: photoPreview
            fillMode: Settings.backgroundFillMode
            anchors.fill: parent
        }

        Button {
            text: "Retake"
            anchors.leftMargin: 20
            anchors.bottomMargin: 20
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            onClicked: {
                camera.start()
                previewPaneVisible = false
            }
        }
        Button {
            text: "Cancel"
            anchors.bottomMargin: 20
            anchors.bottom: parent.bottom;
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                mainForm.hideCameraPanel();
            }
        }

        Button {
            text: "Use it"
            anchors.bottomMargin: 20
            anchors.rightMargin: 20
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            onClicked: {
                previewPaneVisible = false
                mainForm.hideCameraPanel()
                mainForm.sketch.setBackground("file:///" + camera.imageCapture.capturedImagePath)

            }
        }
    }
}


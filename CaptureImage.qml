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

    onVisibleChanged: {
        if(visible) {
            camera.start()
            previewPane.visible = false
        }
    }

    Camera {
        id: camera

        imageCapture {
            onImageCaptured: {
                photoPreview.source = preview
                previewPane.visible = true
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
                camera.imageCapture.captureToLocation(Settings.backgroundImagePath)
            }
        }
    }

    Rectangle {
        id: previewPane
        visible: false
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
                previewPane.visible = false
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
                previewPane.visible = false
                mainForm.hideCameraPanel()
                mainForm.sketch.setBackground(photoPreview.source)

            }
        }
    }
}


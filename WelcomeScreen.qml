import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtMultimedia 5.5
import QtGraphicalEffects 1.0
import Qt.labs.folderlistmodel 2.1


Rectangle {
    id: welcomeScreen
    anchors.fill: parent
    color: "white"
    z: 1500

    SplitView {
        z: 100
        anchors.fill: parent

        Rectangle {
            id: newSketchButton
            color: "#CCCCCC"


            width: welcomeScreen.width / 2
            height: childrenRect.height

            Label {
                text: "Create a new sketch"
                //font.family: "Avenir LT Std 65 Medium"
                font.pointSize: 25
                anchors.centerIn: parent

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        welcomeScreen.visible = false
                    }
                }
            }
        }

        ColumnLayout {
            id: fileColumn
            Layout.fillHeight: true


            width: newSketchButton.width
            Rectangle {
                color: "transparent"
                height: childrenRect.height + 40
                Layout.fillWidth: true
                Label {
                    y:20
                    x: 20
                    Layout.fillWidth: true
                    text: "Browse models"
                    //font.family: "Avenir LT Std 65 Medium"
                    font.pointSize: 16
                }
            }
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                ListView {
                    id: fileList
                    property var colors: ["#f2fbff", "transparent"]
                    width: fileColumn.width


                    model: FolderListModel {
                        nameFilters: ["*.final.obj"]
                        showDirs: false
                        showOnlyReadable: true
                        folder: appPath

                    }
                    delegate: Column {
                        Rectangle {
                            color: fileList.colors[index % 2]
                            width: fileColumn.width
                            height: 100
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                x: 20
                                text:fileName
                                horizontalAlignment: Qt.AlignLeft
                                font.pointSize: 14
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    viewer3d.mesh.source = fileURL
                                    viewer3d.visible = true
                                    welcomeScreen.visible = false
                                }
                            }
                        }


                    }
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        z: 88
    }
}


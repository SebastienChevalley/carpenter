import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Rectangle {
    id: menuBar
    Layout.fillWidth: true
    color: "#333333"
    height:60
    z: 1000

    RowLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true
        anchors.fill: parent

        Label {
            Layout.fillWidth: true
            text: "Plan"
            color: "white"
            font.pointSize: 16
        }
        Rectangle {
            width: menuBar.height
            height: width
            color: "transparent"

            Label {
                width: parent.width;
                height: parent.height
                font.family: "FontAwesome"
                text: "\uf013"
                color: "white"
                font.pointSize: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    menu.toggleState()
                }
            }
        }
    }
}




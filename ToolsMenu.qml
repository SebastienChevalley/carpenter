import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import '.'


Rectangle {

    id: menu
    width: 320

    z: 100
    height: menuItems.count * itemHeight
    color: "#555555"

    property real itemHeight: 64

    state: "hidden"

    states: [
        State {
            name: "hidden"
            PropertyChanges { target: menu; y: - menuBar.height - menu.height }
        },
        State {
            name: "visible"
            PropertyChanges { target: menu; y: menuBar.height }
        }
    ]

    function toggleState() {
        console.log("change state")
        if(menu.state === "hidden") {
            menu.state = "visible"
        }
        else {
            menu.state = "hidden"
        }
    }

    transitions: Transition {
        PropertyAnimation { properties: "y"; easing.type: Easing.InOutQuad }
    }

    ListView {
        width: menu.width
        height: menu.height
        model: menuItems
        delegate: RowLayout {
            anchors.margins: 10
            height: menu.itemHeight
            spacing: 5

            function isToolSelected(tool) {
                return mainForm.state === tool ? Settings.selectedToolColor : Settings.toolColor;
            }

            property var labelColor : isToolSelected(tool)
            property var labelFontSize : 14

            Label {
                text: icon
                font.family: fontName
                height:menu.itemHeight
                width: height
                font.pointSize: labelFontSize
                color: labelColor
            }
            Label {
                Layout.fillWidth: true
                text: name
                font.pointSize: labelFontSize
                color: labelColor
            }

            MouseArea {
                anchors.fill: parent
                onClicked: mainForm.changeTool(tool)
            }
        }
    }
}




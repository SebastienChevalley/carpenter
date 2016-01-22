import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import "." // to import Settings


Rectangle {

    id: menu
    width: childrenRect.width
    z: 100
    height: menuItems.count * itemHeight


    property real itemHeight: 72

    anchors.left: sketchArea.left
    anchors.leftMargin: 10
    anchors.verticalCenter: sketchArea.verticalCenter

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
        height: menu.height
        width:itemHeight

        model: menuItems
        delegate: Rectangle {
            width: childrenRect.width
            height: childrenRect.height
            color: isToolSelected(tool) ? Settings.paletteHighlight : Settings.palette;

            function isToolSelected(tool) {
                return mainForm.state === tool;
            }

            property color labelColor : isToolSelected(tool) ? Settings.selectedToolColor : Settings.toolColor
            property int labelFontSize : 24

            Label {
                text: icon
                font.family: fontName
                font.pointSize: labelFontSize
                color: labelColor
                height: menu.itemHeight
                width: menu.width
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
                anchors.fill: parent
                onClicked: mainForm.changeTool(tool, name)
            }
        }
    }
}




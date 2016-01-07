import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

import "." // to import Settings

Rectangle {
    id: pointContextMenu
    width: childrenRect.width + 10
    height: childrenRect.height
    z: Settings.contextMenuZ
    radius: Settings.contextMenuRadius
    color: Settings.contextMenuColor
    visible: false

    property Item cx: cx
    property Item cy: cy
    property Item cz: cz

    property Item mx: mx
    property Item my: my
    property Item mz: mz

    RowLayout {
        x: 5
        spacing: 0

        Button {
            id: cx
            text: "x"
            style: TogglableButton { icon: "\uf023" }
            onClicked: {
                checked = !checked
                sketch.setPointReaction("cx", checked, mouseArea.selectTool.selectedItem.identifier);
            }
        }
        Button {
            id: cy
            text: "y"
            style: TogglableButton { icon: "\uf023" }
            onClicked: {
                checked = !checked
                sketch.setPointReaction("cy", checked, mouseArea.selectTool.selectedItem.identifier);
            }
        }
        Button {
            id: cz
            text: "z"
            style: TogglableButton { icon: "\uf023" }
            onClicked: {
                checked = !checked
                sketch.setPointReaction("cz", checked, mouseArea.selectTool.selectedItem.identifier);
            }
        }

        Button {
            id: mx
            text: "x"
            style: TogglableButton { icon: "\uf01e" }
            onClicked: {
                checked = !checked
                sketch.setPointReaction("mx", checked, mouseArea.selectTool.selectedItem.identifier);
            }
        }
        Button {
            id: my
            text: "y"
            style: TogglableButton { icon: "\uf01e" }
            onClicked: {
                checked = !checked
                sketch.setPointReaction("my", checked, mouseArea.selectTool.selectedItem.identifier);
            }
        }
        Button {
            id: mz
            text: "z"
            style: TogglableButton { icon: "\uf01e" }
            onClicked: {
                checked = !checked
                sketch.setPointReaction("mz", checked, mouseArea.selectTool.selectedItem.identifier);
            }
        }
    }
}


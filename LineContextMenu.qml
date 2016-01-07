import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

import "." // to import Settings

Rectangle {
    color: Settings.contextMenuColor
    width: childrenRect.width + 10
    height: childrenRect.height
    z: Settings.contextMenuZ
    radius: Settings.contextMenuRadius
    visible: false


    property TextField widthEdit: widthEditField
    property Button verticalConstraint: verticalConstraint
    property Button horizontalConstraint: horizontalConstraint

    RowLayout {
        x: 5
        id: rowLayout1
        spacing: 0

        Item {
            width: childrenRect.width + 10
            height: childrenRect.height + 20
            Label {
                x: 5
                y: 10
                text: "\uf07e"
                font.family: "FontAwesome"
                color: "white"
            }

            TextField {
                x: 5
                y: 10
                id: widthEditField
                width: 75
                enabled: false
                validator: RegExpValidator {
                    regExp: /^([0-9]*)\.([0-9]*)|([0-9]+)$/
                }
                placeholderText: "Initial scale"
            }
        }

        Item {
            width: childrenRect.width + 5
            height: childrenRect.height + 20
            Label {
                y: 10
                x: 5
                text: "mm"
                color: "white"

                MouseArea {
                    anchors.fill: parent
                }
            }
        }
        Item {
            width: childrenRect.width + 20
            height: childrenRect.height + 20
            Label {
                y: 10
                x: 10
                text: "constraints"
                color: "white"

                MouseArea {
                    anchors.fill: parent
                }
            }
        }

        Button {
            text: ""
            id: verticalConstraint
            style: TogglableButton {
                icon: " | "
                iconFont: ""
            }
            onClicked: {
                checked = !checked
            }
        }

        Button {
            text: ""
            id: horizontalConstraint
            style: TogglableButton {
                icon: " – "
                iconFont: ""
            }
            onClicked: {
                checked = !checked
            }
        }

    }
}


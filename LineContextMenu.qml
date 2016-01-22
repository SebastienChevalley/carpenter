import QtQuick 2.5
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
                font.pointSize: 14
            }

            TextField {
                x: 5
                y: 10
                id: widthEditField
                width: 200
                enabled: false
                validator: RegExpValidator {
                    regExp: /^([0-9]*)\.([0-9]*)|([0-9]+)$/
                }
                placeholderText: "Initial scale"
                font.pointSize: 14

                signal gotFocus(var field);
                signal lostFocus(var field);

                property bool wasFocus: false;

                Component.onCompleted: {
                    displayKeyboard.registerTextField(widthEditField)
                }

                onFocusChanged: {
                    if(focus && !wasFocus) {
                        gotFocus(widthEditField)
                    }
                    else {
                        lostFocus(widthEditField)
                    }
                }
                onVisibleChanged: {
                    if(!visible) {
                        focus = false;
                    }
                }
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
                font.pointSize: 11
                MouseArea {
                    anchors.fill: parent
                }
            }
        }
        Item {
            width: childrenRect.width + 50
            height: childrenRect.height + 20
            Label {
                y: 10
                x: 40
                text: "constraints"
                color: "white"
                font.pointSize: 16

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


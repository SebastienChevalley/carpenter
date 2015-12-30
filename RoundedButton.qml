import QtQuick 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2



ButtonStyle {
    property string icon: ""
    id: style
    background: Rectangle {
        border.color: "#cccccc"
        border.width: 1
        radius: 5
        id: buttonBackground
        color: control.pressed ? "#22000000" : "transparent"
    }
    label: RowLayout {
        Rectangle {
            width: childrenRect.width + 5
            height: childrenRect.height + 10
            color: "transparent"
            Label {
                y: 5
                x: 5
                font.family: "FontAwesome"
                font.pointSize: 16
                text: icon
                color: "#222222"
            }
        }

        Rectangle {
            width: childrenRect.width + 5
            height: childrenRect.height + 10
            color: "transparent"
            Label {
                y: 5
                color: "#222222"
                text: control.text
            }
        }
    }

}


import QtQuick 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import "." // to import Settings


ButtonStyle {
    property string icon: ""
    property string iconFont: "FontAwesome"
    id: style
    background: Rectangle {
        id: buttonBackground
        color: control.checked ? "#60FFFFFF" : "transparent"
    }
    label: RowLayout {
        spacing: 0

        Rectangle {
            width: childrenRect.width + 5
            height: childrenRect.height + 20
            color: "transparent"
            Label {
                y: 10
                x: 5
                font.family: iconFont
                font.pointSize: 16
                text: icon
                color: "#ffffff"
            }
        }

        Rectangle {
            width: childrenRect.width + 5
            height: childrenRect.height + 20
            color: "transparent"
            Label {
                y: 10
                color: "#ffffff"
                text: control.text
            }
        }
    }

}


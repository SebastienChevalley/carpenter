pragma Singleton
import QtQuick 2.0

QtObject {
    readonly property real pointSize: 20 // diameter

    readonly property real minimalPointDistance: 100
    readonly property real constructionLineWidth: 50 // diameter

    readonly property string selectedToolColor: "#444444"
    readonly property string toolColor: "black"

    readonly property color palette: "#40000000"
    readonly property color paletteHighlight: "#20000000"

}

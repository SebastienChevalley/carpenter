pragma Singleton
import QtQuick 2.0

QtObject {
    readonly property real pointSize: 20 // diameter

    readonly property real minimalPointDistance: 100
    readonly property real constructionLineWidth: 50 // diameter

    readonly property string selectedToolColor: "blue"
    readonly property string toolColor: "white"

}

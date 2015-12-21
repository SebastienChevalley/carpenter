import QtQuick 2.0

ListModel {
    ListElement {
        name: "Select"
        icon: "\uf245"
        fontName: "FontAwesome"
        tool: "SelectTool"
    }
    ListElement {
        name: "Insert line"
        icon: "\uf040"
        fontName: "FontAwesome"
        tool: "InsertTool"
    }
    ListElement {
        name: "Move points"
        icon: "\uf047"
        fontName: "FontAwesome"
        tool: "MoveTool"
    }
    ListElement {
        name: "Delete"
        icon: "\uf00d"
        fontName: "FontAwesome"
        tool: "DeleteTool"
    }
}


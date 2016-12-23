import QtQuick 2.7
import QtQuick.Controls 1.4

import Colors 1.0

Column {
    height: childrenRect.height
    Rectangle {
        color: Colors.base02
        id: exprRect

        TextEdit {
            color: Colors.base1
            selectionColor: Colors.base00
            id: exprText
            selectByMouse: true
            anchors.left: parent.left
            anchors.right: parent.right
            padding: 5

        }
        height: childrenRect.height
        width: parent.width
    }

    property alias expr: exprText.text
}

import QtQuick 2.7
import QtQuick.Controls 1.4

import Colors 1.0

Column {
    Rectangle {
        color: Colors.base02
        id: exprRect

        TextEdit {
            color: Colors.base0
            id: exprText
            text: 'Hello, world'
            selectByMouse: true
            anchors.left: parent.left
            anchors.right: parent.right
            padding: 5

        }
        height: childrenRect.height
        width: parent.width
    }
    Text {
        id: resultText
        text: 'Result:'
        color: Colors.base0
    }

    property alias expr: exprText.text
    property alias result: resultText.text
}

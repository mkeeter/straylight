import QtQuick 2.7

import Colors 1.0

Rectangle {
    color: Colors.base02
    height: childrenRect.height

    TextInput {
        anchors.left: parent.left
        anchors.right: parent.right
        color: Colors.base1
        selectionColor: Colors.base00
        selectByMouse: true
        id: txt
        padding: 3

        onActiveFocusChanged: {
            if (!activeFocus)
                parent.lostFocus()
        }
    }

    signal setFocus
    signal lostFocus

    onSetFocus: { txt.forceActiveFocus() }

    property alias text: txt.text
}

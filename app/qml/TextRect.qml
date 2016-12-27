import QtQuick 2.7

import Style 1.0

Column {
    TextInput {
        anchors.left: parent.left
        anchors.right: parent.right

        selectByMouse: true
        id: txt
        padding: 3

        font.family: fixedWidth.name
        color: Style.textDarkPrimary
        // TODO selectionColor: Colors.base00

        onActiveFocusChanged: {
            if (!activeFocus)
                parent.lostFocus()
        }
    }

    Rectangle {
        height: 2
        width: txt.width
        color: Style.accent
    }

    signal setFocus
    signal lostFocus

    onSetFocus: { txt.forceActiveFocus() }

    property alias text: txt.text
}

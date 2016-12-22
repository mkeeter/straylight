import QtQuick 2.7

import Colors 1.0

Rectangle {
    color: Colors.base02
    height: childrenRect.height

    TextInput {
        anchors.left: parent.left
        anchors.right: parent.right
        color: Colors.base0
        selectByMouse: true
        id: txt
        padding: 3
    }

    property alias text: txt.text
}

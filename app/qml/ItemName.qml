import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Colors 1.0
import Bridge 1.0
import Awesome 4.7

Item {
    height: childrenRect.height + 5

    Text {
        anchors.left: parent.left
        id: nameText
        color: Colors.base1
        padding: 5
    }

    Row {
        id: editButtons

        anchors.right: parent.right
        anchors.verticalCenter: nameText.verticalCenter
        spacing: 4
        rightPadding: 5

        IconButton {
            id: editName
            anchors.verticalCenter: parent.verticalCenter
            text: Awesome.fa_pencil
            onClicked: {
                editRow.editing = true
                newName.enable(nameText.text)
            }
            enabled: editRow.height == 0
        }

        IconButton {
            id: deleteCell
            anchors.verticalCenter: parent.verticalCenter
            text: Awesome.fa_trash
        }
    }

    RowLayout
    {
        id: editRow
        property bool editing

        anchors.top: nameText.bottom
        width: parent.width
        height: editing ? newName.height : 0
        clip: true

        Behavior on height {
            NumberAnimation {
                duration: 50
            }
        }

        Text {
            text: "Rename to"
            padding: 3
            color: Colors.base1
        }

        TextValidator {
            id: newName
            Layout.fillWidth: true
            validate: function(name) {
                return Bridge.checkName(name)
            }
            onAccepted: function(t) {
                console.log("Renaming to " + t)
                parent.editing = false
            }
            onCancelled: { parent.editing = false }
        }
    }

    property alias text: nameText.text
}

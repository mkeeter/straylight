import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Colors 1.0
import Bridge 1.0
import Awesome 4.7

Item {
    height: childrenRect.height + 5

    id: base
    signal eraseMe

    property int itemIndex

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
            anchors.verticalCenter: parent.verticalCenter
            text: Awesome.fa_trash
            onClicked: { base.eraseMe() }
        }
    }

    GridLayout
    {
        id: editRow
        property bool editing

        anchors.top: nameText.bottom
        width: parent.width
        height: editing ? (nameError.visible ? (newName.height + nameError.height + 5)
                                             : newName.height) : 0
        clip: true

        Behavior on height {
            NumberAnimation {
                duration: 50
            }
        }

        Text {
            Layout.row: 0
            Layout.column: 0
            text: "Rename to"
            padding: 3
            color: Colors.base1
        }

        TextValidator {
            Layout.row: 0
            Layout.column: 1
            Layout.fillWidth: true

            id: newName
            validate: function(name) {
                var e = Bridge.checkRename(itemIndex, name)
                nameError.text = e
                return e == ""
            }
            onAccepted: function(t) {
                parent.editing = false
                Bridge.renameItem(itemIndex, name)
            }
            onCancelled: { parent.editing = false }
        }

        Text {
            Layout.row: 1
            Layout.column: 0
            Layout.columnSpan: 2

            id: nameError
            visible: text.length != 0
            color: Colors.base1
        }
    }


    property alias text: nameText.text
}

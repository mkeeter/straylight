import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import NameValidator 1.0
import Colors 1.0
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
                newName.text = nameText.text
                newName.setFocus()
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
        height: editing ? nameText.height : 0
        clip: true

        Behavior on height {
            NumberAnimation {
                duration: 50
            }
        }

        NameValidator {
            id: validator
        }

        Text {
            text: "Rename to"
            padding: 3
            color: Colors.base1
        }

        TextRect {
            id: newName
            Layout.fillWidth: true
            onLostFocus: {
                parent.editing = false
            }
            onAccepted: {
                if (acceptRename.enabled)
                {
                    console.log("Renaming to " + text)
                    parent.editing = false
                }
            }
        }

        IconButton {
            id: acceptRename
            text: Awesome.fa_check
            enabled: validator.checkName(newName.text)
        }
        IconButton {
            rightPadding: 3
            id: cancelRename
            text: Awesome.fa_times
            onClicked: {
                editRow.editing = false
            }
        }
    }

    property alias text: nameText.text
}
/*
Column {
    NameValidator {
        id: validator
    }
    Rectangle {
        color: Colors.base02
        id: nameRect

        TextInput {
            color: Colors.base0
            id: textItem
            text: 'Hello, world'
            selectByMouse: true
            anchors.left: parent.left
            anchors.right: parent.right
            padding: 5

            property string oldName
            onActiveFocusChanged: {
                if (focus) oldName = text
                else if (!validator.checkName(text)) {
                    text = oldName
                    errorFlash.start()
                }
            }

            Keys.onReturnPressed: {
                focus = false
            }

            SequentialAnimation {
                id: errorFlash
                ColorAnimation {
                    target: nameRect
                    property: 'color'
                    to: Colors.red
                    duration: 100
                }
                ColorAnimation {
                    target: nameRect
                    property: 'color'
                    to: nameRect.color
                    duration: 100
                }
            }
        }
        height: childrenRect.height
        width: parent.width
    }

    Text {
        color: Colors.red
        height: (textItem.cursorVisible && !validator.checkName(textItem.text)) ? contentHeight + padding : 0

        Behavior on height {
            NumberAnimation {
                duration: 50
            }
        }
        topPadding: 2
        bottomPadding: 2
        text: 'Error'
        clip: true
    }
    property alias text: textItem.text
}
*/

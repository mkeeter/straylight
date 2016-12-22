import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import NameValidator 1.0
import Colors 1.0

Item
{
    id: creator

    property real preferredHeight

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: creator
                preferredHeight: 0
            }
        },
        State {
            name: "visible"
            PropertyChanges {
                target: creator
                preferredHeight: grid.height
            }
        },
        State {
            name: "sheet"
            extend: "visible"
            PropertyChanges {
                target: sheetLabel
                text: "Creating new sheet:"
            }
        },
        State {
            name: "cell"
            extend: "visible"
            PropertyChanges {
                target: sheetLabel
                text: "Creating new cell:"
            }
        },
        State {
            name: "instance"
            extend: "visible"
            PropertyChanges {
                target: sheetLabel
                text: "Creating new instance of " + sheetName + ":"
            }
        }
    ]

    transitions: Transition {
        NumberAnimation {
            properties: "preferredHeight"
            duration: 5000
        }
    }

    state: "hidden"

    property string sheetName

    function activateCell() {
        state = "cell"
        console.log("Cell")
        sheetNamer.enable("c")
    }

    function activateSheet() {
        state = "sheet"
        console.log("sheet")
        sheetNamer.enable("s")
    }

    function activateInstance(n) {
        sheetName = n
        state = "instance"
        console.log("instance")
        sheetNamer.enable("i")
    }

GridLayout
{
    id: grid
    width: parent.width

    rows: 2
    columns: 2

    Text {
        Layout.row: 0
        Layout.columnSpan: 2
        padding: 5

        id: sheetLabel
        color: Colors.base1
    }

    Text {
        Layout.row: 1
        Layout.column: 0

        text: "Name:"
        color: Colors.base1
        leftPadding: 5
        Layout.fillHeight: true
    }

    TextValidator {
        id: sheetNamer
        Layout.row: 1
        Layout.column: 1
        Layout.fillWidth: true
        Layout.fillHeight: true
        validate: function(name) {
            return validator.checkName(name)
        }
        onAccepted: function(t) {
            console.log("Creating new sheet with name " + t)
            creator.state = "hidden"
        }
        onCancelled: {
            creator.state = "hidden"
            console.log("cancelled")
        }
    }

    NameValidator {
        id: validator
    }
}

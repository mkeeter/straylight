import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Colors 1.0
import Bridge 1.0

GridLayout
{
    id: grid
    width: parent.width

    rows: 2
    columns: 2

    /*
     *  As this item is stored in a Layout, we offer height hints here
     */
    property real preferredHeight: 0
    Behavior on preferredHeight {
        NumberAnimation {
            duration: 50
        }
    }

    function activateCell() {
        console.log("Cell")
        preferredHeight = childrenRect.height
        label.text = "Creating new cell:"
        namer.enable("c")
    }

    function activateSheet() {
        console.log("sheet")
        preferredHeight = childrenRect.height
        label.text = "Creating new sheet:"
        namer.enable("s")
    }

    function activateInstance(n) {
        preferredHeight = childrenRect.height
        console.log("instance")
        label.text = "Creating new instance of " + n + ":"
        namer.enable("i")
    }

    Text {
        Layout.row: 0
        Layout.columnSpan: 2
        padding: 5

        id: label
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
        id: namer
        Layout.row: 1
        Layout.column: 1
        Layout.fillWidth: true
        Layout.fillHeight: true
        validate: function(name) {
            return Bridge.checkName(name)
        }
        onAccepted: function(t) {
            preferredHeight = 0
            console.log("Creating new sheet with name " + t)
        }
        onCancelled: {
            preferredHeight = 0
            console.log("cancelled")
        }
    }
}

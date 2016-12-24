import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Colors 1.0
import Bridge 1.0

ColumnLayout
{
    id: grid

    property int sheetIndex

    function activateCell() {
        label.text = "Creating new cell:"
        console.log("Cell")

        nameInput.mode = "cell"
        nameInput.enable("i")
    }

    function activateSheet() {
        label.text = "Creating new sheet:"
        console.log("sheet")

        nameInput.mode = "sheet"
        nameInput.enable("i")
    }

    function activateInstance(n) {
        label.text = "Creating new instance of " + n + ":"
        console.log("instance")

        nameInput.mode = "instance"
        nameInput.sheetIndex = n
        nameInput.enable("i")
    }

    Text {
        Layout.row: 0
        font.bold: true

        id: label
        color: Colors.base1
    }

    ValidatedInput {
        id: nameInput

        Layout.row: 1
        property string mode: ""
        property int sheetIndex

        label: "Name:"

        getError: function(name) {
            if (mode == "cell") {
                return "bad cell"
            } else if (mode == "sheet") {
                return "bad sheet"
            } else if (mode == "instance") {
                return "bad instance"
            }
        }

        onAccepted: function(t) {
            if (mode == "cell") {
                console.log("Making cell " + t)
            } else if (mode == "sheet") {
                console.log("Making sheet " + t)
            } else if (mode == "instance") {
                console.log("Making instance " + t)
            }
        }
    }

    property alias active: nameInput.active
}

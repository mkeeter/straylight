import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Colors 1.0
import Bridge 1.0

ColumnLayout
{
    id: grid

    property int sheetIndex

    // for when we're creating an instance
    property int targetSheetIndex

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
        targetSheetIndex = n
        nameInput.enable("i")
    }

    Text {
        id: label
        Layout.alignment: Qt.AlignTop
        color: Colors.base1
        font.bold: true
    }

    ValidatedInput {
        id: nameInput
        Layout.alignment: Qt.AlignTop

        label: "Name:"
        property string mode: ""

        getError: function(name) {
            if (mode == "cell") {
                return Bridge.checkName(sheetIndex, name)
            } else if (mode == "instance") {
                return Bridge.checkName(sheetIndex, name)
            } else if (mode == "sheet") {
                return Bridge.checkSheetName(sheetIndex, name)
            }
        }

        onAccepted: function(t) {
            if (mode == "cell") {
                Bridge.insertCell(sheetIndex, t)
            } else if (mode == "sheet") {
                console.log("Making sheet " + t)
            } else if (mode == "instance") {
                console.log("Making instance " + t)
            }
        }
    }

    // Dummy item to pad any extra height
    // TODO: this shouldn't be necessary with Layout.alignment specified
    Item {
        Layout.fillHeight: true
    }

    property alias active: nameInput.active
}

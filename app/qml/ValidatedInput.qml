import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Awesome 4.7
import Colors 1.0

GridLayout {
    function enable(t) {
        visible = true
        editor.text = t
        editor.setFocus()
    }

    property var getError
    property string error
    property bool valid: error.length == 0

    signal accepted(string t)
    signal cancelled

    visible: false

    onCancelled: { visible = false }
    onAccepted:  { visible = false }

    Text {
        Layout.row: 0
        Layout.column: 0

        id: textLabel
        color: Colors.base1
    }

    TextRect {
        id: editor

        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true

        Keys.onReturnPressed: {
            if (valid)
                accepted(text)
            else
                event.accepted = false
        }

        onLostFocus: {
            cancelled()
        }

        onTextChanged: {
            error = getError(text)
        }
    }

    IconButton {
        Layout.row: 0
        Layout.column: 2

        id: accept
        text: Awesome.fa_check
        enabled: valid
        onClicked: { parent.accepted(editor.text) }
    }

    IconButton {
        Layout.row: 0
        Layout.column: 3

        rightPadding: 3
        id: cancel
        text: Awesome.fa_times
        onClicked: {
            cancelled()
        }
    }

    Row {
        visible: !valid

        Layout.row: 1
        Layout.column: 0
        Layout.columnSpan: 4

        Text {
            visible: !valid
            text: Awesome.fa_exclamation_triangle
            color: Colors.base0
            font.family: fontAwesome.name
        }

        Text {
            text: error
            color: Colors.base1
            leftPadding: 5
        }
    }

    property alias label: textLabel.text
}

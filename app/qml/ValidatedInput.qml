import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Awesome 4.7
import Colors 1.0

ColumnLayout {
    function enable(t) {
        active = true
        editor.text = t
        editor.setFocus()
    }

    property var getError
    property string error
    property bool valid: error.length == 0
    property bool active: false

    signal accepted(string t)
    signal cancelled

    onCancelled: { active = false }
    onAccepted:  { active = false }

    RowLayout {
        Layout.alignment: Qt.AlignTop

        Text {
            id: textLabel
            color: Colors.base1
        }

        TextRect {
            id: editor
            Layout.fillWidth: true

            Keys.onReturnPressed: {
                if (valid)
                    accepted(text)
                else
                    event.accepted = false
            }
            Keys.onEscapePressed: {
                cancelled()
            }

            onLostFocus: {
                cancelled()
            }

            onTextChanged: {
                error = getError(text)
            }
        }

        IconButton {
            id: accept
            text: Awesome.fa_check
            enabled: valid
            onClicked: { accepted(editor.text) }
        }

        IconButton {
            rightPadding: 3
            id: cancel
            text: Awesome.fa_times
            onClicked: {
                cancelled()
            }
        }
    }

    Row {
        visible: !valid

        Text {
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

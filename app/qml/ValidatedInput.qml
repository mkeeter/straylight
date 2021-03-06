import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Awesome 4.7
import Style 1.0

ColumnLayout {
    function enable(t) {
        active = true
        editor.text = t
        editor.setFocus()
        editor.selectAll()
        error = getError(t)
    }

    property var getError
    property string error
    property bool valid: error.length == 0
    property bool active: false

    signal accepted(string t)
    signal cancelled

    onCancelled: { active = false }
    onAccepted:  { active = false }

    // When active becomes false, steal focus so that undo actions
    // are propagated up to the top of the UI.
    onActiveChanged: {
        if (!active && editor.activeFocus) {
            forceActiveFocus()
        }
    }

    RowLayout {
        Layout.alignment: Qt.AlignTop
        id: editRow

        Text {
            id: textLabel
            color: Style.textDarkSecondary
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
            id: warnIcon
            text: Awesome.fa_exclamation_triangle
            color: errorMessage.color
            font.family: fontAwesome.name
        }

        Text {
            id: errorMessage
            width: editRow.width - warnIcon.width
            text: error
            color: Style.textDarkSecondary
            leftPadding: 5
            wrapMode: Text.Wrap
        }
    }

    property alias label: textLabel.text
}

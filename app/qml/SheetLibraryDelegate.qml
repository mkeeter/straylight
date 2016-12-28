import QtQuick 2.7
import QtQuick.Layouts 1.3

import Style 1.0
import Awesome 4.7
import Bridge 1.0

GridLayout {
    Text {
        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true

        id: sheetName
        text: name

        color: Style.textDarkPrimary
        topPadding: 3
        leftPadding: 5
        bottomPadding: topPadding
    }

    Row {
        Layout.row: 0
        Layout.column: 1

        spacing: 4

        IconButton {
            text: Awesome.fa_lock
            enabled: false
            visible: !editable
        }

        IconButton {
            text: Awesome.fa_plus_square_o
            toolTip: "Insert instance"
            enabled: insertable
        }
        IconButton {
            text: Awesome.fa_pencil
            enabled: !renamer.active
            toolTip: "Rename"
            onClicked: {
                renamer.enable(name)
            }
            visible: editable
        }
        IconButton {
            text: Awesome.fa_trash
            toolTip: "Delete"
            visible: editable
        }
    }

    ValidatedInput {
        Layout.row: 1
        Layout.column: 0
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Layout.preferredHeight: active ? implicitHeight : 0
        Behavior on Layout.preferredHeight {
            NumberAnimation { duration: 50 }
        }
        clip: true

        id: renamer
        width: parent.width
        label: "Rename to"
        getError: function(name) {
            return Bridge.checkSheetRename(itemIndex, name)
        }
        onAccepted: function(t) {
            Bridge.renameSheet(itemIndex, t)
        }
    }

    // Divider between sheets
    Rectangle {
        Layout.row: 2
        Layout.column: 0
        Layout.columnSpan: 2

        Layout.fillWidth: true
        height: 2
        color: last ? 'transparent' : Style.dividerDark
    }
}

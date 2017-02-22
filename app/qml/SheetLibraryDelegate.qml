import QtQuick 2.7
import QtQuick.Layouts 1.3

import Style 1.0
import Awesome 4.7
import Bridge 1.0

GridLayout {
    objectName: "SheetLibraryDelegate"

    Text {
        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true

        id: sheetName
        text: name

        color: insertable ? Style.textDarkPrimary : Style.textDarkSecondary
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
            onClicked: { sheetInstanceModel.insertInstance(sheetIndex) }
            visible: insertable
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
            onClicked: { sheetInstanceModel.eraseSheet(sheetIndex) }
        }
    }

    ValidatedInput {
        id: renamer

        Layout.row: 1
        Layout.column: 0
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Layout.preferredHeight: active ? implicitHeight : 0
        Behavior on Layout.preferredHeight {
            NumberAnimation { duration: 50 }
        }
        clip: true

        width: parent.width
        label: "Rename to"
        getError: function(name) {
            return sheetInstanceModel.checkSheetRename(sheetIndex, name)
        }
        onAccepted: function(t) {
            sheetInstanceModel.renameSheet(sheetIndex, t)
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

    function renameMe() { renamer.enable(name) }
}

import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Style 1.0
import Awesome 4.7

GridLayout {
    id: base

    signal eraseMe
    signal openSheet
    signal closeSheet
    signal focusCell

    function openRename() {
        renamer.enable(name)
    }

    Text {
        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true

        text: name
        font.family: fixedWidth.name
        font.pointSize: 18
        color: Style.textDarkPrimary
    }

    Row {
        Layout.row: 0
        Layout.column: 1

        Text {
            text: type == 'cell' ? '' : sheet
            font.pointSize: 14
            color: Style.textDarkSecondary
            rightPadding: 8
        }

        Row {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            IconButton {
                text: isInstanceOpen ? Awesome.fa_outdent
                                     : Awesome.fa_indent
                visible: type == 'instance'
                toolTip: isInstanceOpen ? "Close" : "Edit"
                onClicked: { if (isInstanceOpen) base.closeSheet()
                             else                base.openSheet() }
            }
            IconButton {
                text: Awesome.fa_pencil
                enabled: !renamer.active
                onClicked: {
                    renamer.enable(name)
                }
                toolTip: "Rename"
            }
            IconButton {
                text: Awesome.fa_trash
                onClicked: { base.eraseMe() }
                toolTip: "Delete"
            }
        }
    }

    ValidatedInput {
        Layout.row: 1
        Layout.column: 0
        Layout.columnSpan: 2
        Layout.preferredHeight: active ? implicitHeight : 0
        Layout.fillWidth: true
        Behavior on Layout.preferredHeight {
            NumberAnimation { duration: 50 }
        }
        clip: true

        id: renamer
        label: "Rename to"
        getError: function(name) {
            return sheetInstanceModel().checkItemRename(uniqueIndex, name)
        }
        onAccepted: function(t) {
            Bridge.renameItem(uniqueIndex, t)
        }
        onActiveChanged: { focusCell() }
    }
}

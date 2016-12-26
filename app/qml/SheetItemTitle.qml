import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Colors 1.0
import Bridge 1.0
import Awesome 4.7

GridLayout {
    id: base

    signal eraseMe
    signal openSheet
    signal closeSheet

    Text {
        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true

        text: type == 'cell' ? name : name + " [" + sheet + "]"
        font.family: fixedWidth.name
        font.pointSize: 18
        color: Colors.base1
    }

    Row {
        Layout.row: 0
        Layout.column: 1
        spacing: 4

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

        IconButton {
            text: isInstanceOpen ? Awesome.fa_angle_double_left
                                 : Awesome.fa_angle_double_right
            visible: type == 'instance'
            toolTip: isInstanceOpen ? "Close" : "Edit"
            onClicked: { if (isInstanceOpen) base.closeSheet()
                         else                base.openSheet() }
        }
    }

    ValidatedInput {
        Layout.row: 1
        Layout.column: 0
        Layout.columnSpan: 2
        Layout.preferredHeight: active ? implicitHeight : 0
        Behavior on Layout.preferredHeight {
            NumberAnimation { duration: 50 }
        }
        clip: true

        id: renamer
        width: parent.width
        label: "Rename to"
        getError: function(name) {
            return Bridge.checkRename(itemIndex, name)
        }
        onAccepted: function(t) {
            Bridge.renameItem(itemIndex, t)
        }
    }
}

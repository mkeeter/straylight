import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Colors 1.0
import Bridge 1.0
import Awesome 4.7

GridLayout {
    id: base

    signal eraseMe

    Text {
        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true

        text: type == 'cell' ? name : name + " [" + sheet + "]"
        font.family: sansSerif.name
        font.pointSize: 18
        color: Colors.base1
    }

    IconButton {
        Layout.row: 0
        Layout.column: 1

        id: editName
        text: Awesome.fa_pencil
        onClicked: {
            renamer.enable(name)
        }
        enabled: !renamer.active
    }

    IconButton {
        Layout.row: 0
        Layout.column: 2

        text: Awesome.fa_trash
        onClicked: { base.eraseMe() }
    }

    ValidatedInput {
        Layout.row: 1
        Layout.column: 0
        Layout.columnSpan: 3
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

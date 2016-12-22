import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Awesome 4.7

RowLayout {

    function enable(t) {
        txt.text = t
        txt.setFocus()
    }

    property var validate
    signal accepted(string t)
    signal cancelled

    TextRect {
        id: txt
        Layout.fillWidth: true

        Keys.onReturnPressed: {
            if (accept.enabled)
                parent.accepted(text)
            else
                event.accepted = false
        }

        onLostFocus: {
            parent.cancelled()
        }
    }

    IconButton {
        id: accept
        text: Awesome.fa_check
        enabled: validate(txt.text)
    }

    IconButton {
        rightPadding: 3
        id: cancel
        text: Awesome.fa_times
        onClicked: {
            parent.cancelled()
        }
    }
}

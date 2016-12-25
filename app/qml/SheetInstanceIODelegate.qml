import QtQuick 2.7
import QtQuick.Layouts 1.0

import Colors 1.0
import Bridge 1.0

Item {
    id: sheetInstanceIODelegate

    function bestDelegate(t) {
        console.log("Getting best delegate for " + t)
        if (t == "input")
            return inputDelegate
        else
            return outputDelegate
    }

    Component {
        id: inputDelegate
        Rectangle { width: 100; height: 50; color: 'red' }
    }

    Component {
        id: outputDelegate
        Rectangle { width: 100; height: 50; color: 'blue' }
    }

    Loader {
        width: parent.width
        sourceComponent: bestDelegate(type)
    }
}

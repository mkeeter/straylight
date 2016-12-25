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
        Rectangle { width: 100; height: 50; color: 'red'
            Component.onCompleted: { console.log("Made input delegate") }
            Component.onDestruction: { console.log("Destroyed output delegate") }
        }
    }

    Component {
        id: outputDelegate
        Rectangle { width: 100; height: 50; color: 'blue'
            Component.onCompleted: { console.log("Made input delegate") }
            Component.onDestruction: { console.log("Destroyed input delegate") }
        }
    }

    Loader {
        width: parent.width
        height: 100
        sourceComponent: bestDelegate(type)
    }
}

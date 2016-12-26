import QtQuick 2.7
import QtQuick.Controls 1.4

import Bridge 1.0
import Colors 1.0

SplitView {
    id: sstack
    width: 0

    property var env: []

    // We have to manually track items because SplitView doesn't have
    // a way to look up the item at a particular index.
    property var items: []

    function openTo(e) {
        var new_width = width
        while (env.length > e.length ||
               env[env.length - 1] != e[env.length - 1])
        {
            new_width -= items[items.length - 1].width + 2
            removeItem(items[items.length - 1])

            env.pop()
            items.pop()
        }
        while (env.length < e.length)
        {
            env.push(e[env.length])
            items.push(sheetViewComponent.createObject(sheetStack,
                {sheetEnv: env.slice(),
                 sheetIndex: Bridge.sheetOf(env[env.length - 1])}))
            addItem(items[items.length - 1])
            new_width += 250
        }

        width = new_width
        envChanged()
        Bridge.sync()
    }

    property var widthAnim: NumberAnimation {
        target: sstack
        property: 'width'
        duration: 50
    }

    handleDelegate: Component {
        Rectangle {
            width: 2
            color: Colors.base00
        }
    }
}

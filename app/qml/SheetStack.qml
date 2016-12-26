import QtQuick 2.7
import QtQuick.Controls 1.4

import Bridge 1.0
import Colors 1.0

SplitView {
    id: sstack
    width: 0

    property var env: []
    function openTo(e) {
        while (env.length > e.length ||
               env[env.length - 1] != e[env.length - 1])
        {
            env.pop()
            removeItemAt(env.length)
        }
        while (env.length < e.length)
        {
            env.push(e[env.length])
            addItem(sheetViewComponent.createObject(sheetStack,
                {sheetEnv: env.slice(),
                 sheetIndex: Bridge.sheetOf(env[env.length - 1])}))
            widthAnim.to = width + 200
            widthAnim.start()
        }
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

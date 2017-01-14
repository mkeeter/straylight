import QtQuick 2.7
import QtQuick.Controls 1.4

import Bridge 1.0
import Style 1.0

SplitView {
    id: sstack
    width: 0

    // This is the component that's initialized in our stack of sheets
    Component {
        id: sheetViewComponent
        SheetView { }
    }

    property var env: []

    // We have to manually track items because SplitView doesn't have
    // a way to look up the item at a particular index.
    property var items: []
    property var toErase: []
    property var toOpen: []

    function closeTo(e) {
        var new_width = width
        while (env.length > e.length ||
               env[env.length - 1] != e[env.length - 1])
        {
            new_width -= items[items.length - 1].width

            env.pop()
            toErase.push(items.pop())
        }
        envChanged()

        widthAnim.to = new_width
        widthAnim.start()
        // widthAnim will clean out toErase when it's done
    }

    function openTo(e) {
        var prefix = []
        for (var i in env) {
            if (i < e.length && env[i] == e[i]) {
                prefix.push(env[i])
            } else {
                break
            }
        }
        if (prefix.length < env.length) {
            toOpen = e // widthAnim will re-open when it's done
            closeTo(prefix)
            return
        }

        var new_width = width
        while (env.length < e.length)
        {
            env.push(e[env.length])
            items.push(sheetViewComponent.createObject(sheetStack,
                {sheetEnv: env.slice(),
                 sheetIndex: Bridge.sheetOf(env[env.length - 1])}))
            addItem(items[items.length - 1])
            new_width += 250
        }
        envChanged()
        Bridge.sync()

        widthAnim.to = new_width
        widthAnim.start()
    }

    property var widthAnim: NumberAnimation {
        target: sstack
        property: 'width'
        duration: 100
        onStopped: {
            for (var i in toErase) {
                removeItem(toErase[i])
                toErase[i].destroy()
            }
            toErase = []

            if (toOpen.length) {
                var t = toOpen.slice()
                toOpen = []
                openTo(t)
            }
        }
    }

    // The handle between items is a fake drop-shadow
    handleDelegate: Component {
        Item {
            width: 0
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: 3
                height: 5
                width: parent.height
                rotation: -90
                gradient: Gradient {
                    GradientStop { position: 0.0; color: '#40000000'}
                    GradientStop { position: 1.0; color: '#00000000'}
                }
            }
        }
    }
}

import QtQuick 2.7
import QtQuick.Controls 1.4

import Bridge 1.0
import Style 1.0
import Graph 1.0

SplitView {
    id: sstack
    width: 0

    // Global variable to keep track of the selected item
    property var selectedItem: undefined

    // Called when a file is loaded (and used to trigger a layout fix)
    function fixLayout() { layoutTimer.restart() }
    signal _fixLayout

    signal animDone

    Component.onCompleted: {
        Graph.instanceErased.connect(onInstanceErased)
    }

    function onInstanceErased(instance) {
        var prefix = []
        for (var i in env) {
            if (env[i] == instance) {
                break;
            } else {
                prefix.push(env[i]);
            }
        }
        if (prefix.length < env.length)
        {
            closeTo(prefix)
        }
    }

    Timer {
        id: layoutTimer
        interval: 10
        onTriggered: {
            sstack._fixLayout()
        }
    }

    // This is the component that's initialized in our stack of sheets
    Component {
        id: sheetViewComponent
        SheetView {
            Component.onCompleted: {
                _fixLayout.connect(fixLayout)
            }
        }
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
                {sheetEnv: env.slice()}))
            addItem(items[items.length - 1])
            new_width += 250
        }
        envChanged()

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
            else
            {
                animDone()
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

import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import FbItem 1.0
import Colors 1.0
import Bridge 1.0

import "vendor/underscore.js" as Underscore

ApplicationWindow {
    visible: true
    title: "Straylight"

    width: 640
    height: 480

    style: ApplicationWindowStyle {
        background: Rectangle {
            color: Colors.base03
        }
    }
    FontLoader {
        id: fontAwesome
        source: "qrc:/fonts/fontawesome-webfont.ttf"
    }
    FontLoader {
        id: sansSerif
        source: "qrc:/fonts/Inconsolata-Regular.ttf"
    }

    property var _: Underscore.init()

    //menu containing two menu items
    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
                shortcut: StandardKey.Open
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
                shortcut: StandardKey.Quit
            }
        }
    }

    Component {
        id: sheetViewComponent
        SheetView { }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        SplitView {
            id: sheetStack

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
                        {sheetEnv: env.slice() }))
                    width += 200
                }
                Bridge.sync()
            }
            anchors.bottom: parent.bottom
            anchors.top: parent.top

            handleDelegate: Component {
                Rectangle {
                    width: 2
                    color: Colors.base00
                }
            }
        }

        FbItem {
            id: viewport
            Layout.minimumWidth: 100
            Layout.fillWidth: true
        }

        Component.onCompleted: {
            sheetStack.openTo([0])
        }
    }
}

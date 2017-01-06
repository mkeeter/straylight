import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Canvas 1.0
import Style 1.0
import Material 1.0
import Bridge 1.0

import "/js/vendor/underscore.js" as Underscore

ApplicationWindow {
    visible: true
    title: "Straylight"

    width: 640
    height: 480

    style: ApplicationWindowStyle {
        background: Rectangle {
            color: Material.white
        }
    }
    FontLoader {
        id: fontAwesome
        source: "qrc:/fonts/fontawesome-webfont.ttf"
    }
    FontLoader {
        id: fixedWidth
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

        SheetStack {
            id: sheetStack

            anchors.bottom: parent.bottom
            anchors.top: parent.top
        }

        // Rendering window (handled in C++)
        Canvas {
            id: viewport
            Layout.minimumWidth: 100
            Layout.fillWidth: true

            function emitSize() {
                Bridge.canvasResized(width, height)
            }
            Component.onCompleted: {
                widthChanged.connect(emitSize)
                heightChanged.connect(emitSize)
            }
        }

        Component.onCompleted: {
            sheetStack.openTo([0])
        }
    }
}

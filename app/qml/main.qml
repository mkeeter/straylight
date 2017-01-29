import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import Canvas 1.0
import Style 1.0
import Material 1.0
import Bridge 1.0

import "/js/vendor/underscore.js" as Underscore

ApplicationWindow {
    visible: true
    title: "Straylight [" + (file ? Bridge.filePath(file) : "unsaved") + "]"

    width: 640
    height: 480

    style: ApplicationWindowStyle {
        background: Rectangle {
            color: Material.white
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //  Global properties for fonts and underscore
    FontLoader {
        id: fontAwesome
        source: "qrc:/fonts/fontawesome-webfont.ttf"
    }
    FontLoader {
        id: fixedWidth
        source: "qrc:/fonts/Inconsolata-Regular.ttf"
    }
    property var _: Underscore.init()

    ////////////////////////////////////////////////////////////////////////////
    //  Menu bar, with all the functions you'd expect
    property var file: false

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: "&New"
                onTriggered: {
                    Bridge.clearFile()
                    file = false
                }
                shortcut: StandardKey.New
            }
            MenuItem {
                text: "&Save"
                onTriggered: {
                    if (file) {
                        Bridge.saveFile(file)
                    } else {
                        saveDialog.visible = true
                    }
                }
                shortcut: StandardKey.Save
            }
            MenuItem {
                text: "&Save As"
                onTriggered: saveDialog.visible = true
                shortcut: StandardKey.SaveAs
            }
            MenuItem {
                text: "&Open"
                onTriggered: loadDialog.visible = true
                shortcut: StandardKey.Open
            }
            MenuItem {
                text: "Exit"
                onTriggered: Qt.quit();
                shortcut: StandardKey.Quit
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //  All of the dialogs, with visible = false until shown
    FileDialog {
        id: saveDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            file = saveDialog.fileUrl
            Bridge.saveFile(file)
            // TODO: error handling here
        }
        selectExisting: false
        visible: false
    }

    FileDialog {
        id: loadDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            Bridge.loadFile(loadDialog.fileUrl)
            // TODO: error handling here
        }
        visible: false
    }

    ////////////////////////////////////////////////////////////////////////////
    //  This splitview contains our sheets on the left and the OpenGL canvas
    //  on the right.  The OpenGL canvas is a minimal object that forwards
    //  everything through the Bridge
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

            MouseArea {
                property real prevX: 0
                property real prevY: 0

                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                hoverEnabled: true
                onPositionChanged: function(event) {
                    if (event.buttons & Qt.LeftButton) {
                        viewport.rotateIncremental(event.x - prevX, event.y - prevY)
                    }
                    else if (event.buttons & Qt.RightButton) {
                        viewport.panIncremental(event.x - prevX, event.y - prevY)
                    }
                    prevX = event.x
                    prevY = event.y
                }
                onPressed: function(event) {
                    prevX = event.x
                    prevY = event.y
                }
                onWheel: function(event) {
                    viewport.zoomIncremental(event.angleDelta.y, event.x, event.y)
                }
            }
        }

        Component.onCompleted: {
            sheetStack.openTo([1])
        }
    }
}

import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import FbItem 1.0
import Colors 1.0

ApplicationWindow {
    visible: true
    title: "Beep boop"

    width: 640
    height: 480

    style: ApplicationWindowStyle {
        background: Rectangle {
            color: Colors.base03
        }
    }

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

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        SheetView {
            id: root

            libraryModel: ListModel {
                ListElement {
                    name: "New cell..."
                    sheetIndex: -1
                }
                ListElement {
                    name: "Circle"
                    sheetIndex: 1
                }
                ListElement {
                    name: "Sphere"
                    sheetIndex: 4
                }
                ListElement {
                    name: "Cube"
                    sheetIndex: 5
                }
                ListElement {
                    name: "beep"
                    sheetIndex: 5
                }
                ListElement {
                    name: "boop"
                    sheetIndex: 5
                }
                ListElement {
                    name: "wat"
                    sheetIndex: 5
                }
                ListElement {
                    name: "New sheet..."
                    sheetIndex: -1
                }
            }

            sheetModel: ListModel {
                ListElement {
                    type: "cell"
                    index: 1
                    name: "x"
                    expr: "(+ 1 2)"
                    value: "3"
                    valid: true
                }
                ListElement {
                    type: "cell"
                    index: 0
                    name: "y"
                    expr: "(+ (x) 2)"
                    value: "5"
                    valid: true
                }
            }
        }

        FbItem {
            id: viewport
            Layout.minimumWidth: 100
            Layout.fillWidth: true
        }
    }
}

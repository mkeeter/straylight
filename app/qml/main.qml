import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import FbItem 1.0

ApplicationWindow {
    visible: true
    title: "Beep boop"

    width: 640
    height: 480

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
            sheetIndex: [0]

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

import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Colors 1.0

ColumnLayout {
    Text {
        id: header
        text: "<b>Library:<b>"
        color: Colors.base0
        padding: 5
    }

    ScrollView {
        Layout.fillWidth: true
        Layout.fillHeight: true

        id: libraryView

        property ListModel libraryModel: ListModel {
            ListElement {
                name: "New cell..."
                sheetIndex: -1
            }
            ListElement {
                name: "New sheet..."
                sheetIndex: -2
            }
        }

        style: ScrollViewStyle {
            transientScrollBars: true
        }

        ListView {
            id: sheetList
            model: libraryModel
            anchors.fill: parent

            delegate: Rectangle {
                height: itemName.height
                anchors.left: parent.left
                anchors.right: parent.right

                color: hover.containsMouse ? Colors.base01 :
                       (index % 2 == 0 ? Colors.base02 : Colors.base03)
                Text {
                    id: itemName
                    text: name
                    color: Colors.base1
                    topPadding: 3
                    leftPadding: 5
                    bottomPadding: topPadding
                }

                MouseArea {
                    id: hover
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (sheetIndex == -1)
                            creator.activateCell()
                        else if (sheetIndex == -2)
                            creator.activateSheet()
                        else
                            creator.activateInstance(sheetIndex)
                    }
                }
            }
        }
    }

    // Divider
    Rectangle {
        Layout.fillWidth: true
        height: 2
        visible: creator.active
        color: Colors.base00
    }

    SheetCreator {
        id: creator
        Layout.fillWidth: true

        Layout.preferredHeight: active ? implicitHeight : 0
        Behavior on Layout.preferredHeight {
            NumberAnimation { duration: 50 }
        }
    }

    property alias libraryModel: libraryView.libraryModel
    property alias sheetIndex: creator.sheetIndex
}

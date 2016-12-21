import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Colors 1.0

Column {
    id: root
    width: parent.width

    Text {
        id: header
        width: root.width
        text: "<b>Library:<b>"
        color: Colors.base0
        padding: 5
    }

    ScrollView {
        id: libraryView

        property ListModel libraryModel
        /*
        style: ScrollViewStyle {
            transientScrollBars: true
        }
        */
        clip: true
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

        Column {
            Rectangle {
                id: newCell
                width: root.width
                height: childrenRect.height
                color: Colors.base02
                Text {
                    text: "New cell..."
                    color: Colors.base0
                    padding: 5
                }
            }

            ListView {
                id: sheetList
                model: libraryModel
                height: childrenRect.height
                delegate: Rectangle {
                    width: root.width
                    height: childrenRect.height
                    color: index % 2 == 0 ? Colors.base02 : Colors.base03
                    Text {
                        text: name
                        color: Colors.base0
                        topPadding: 3
                        leftPadding: 5
                        bottomPadding: topPadding
                    }
                }
            }

            Rectangle {
                width: root.width
                height: childrenRect.height
                color: libraryModel.count % 2 == 0 ? Colors.base03 : Colors.base02
                Text {
                    id: newSheet
                    text: "New sheet..."
                    color: Colors.base0
                    padding: 5
                }
            }
        }
    }

    property alias libraryModel: libraryView.libraryModel
}

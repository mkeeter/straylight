import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Colors 1.0

Column {
    height: childrenRect.height
    width: parent.width

    Text {
        id: header
        width: parent.width
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
        width: parent.width
        clip: true

        Column {
            height: childrenRect.height
            width: parent.width

            Text {
                id: newCell
                width: parent.width
                text: "New cell..."
                color: Colors.base0
                padding: 5
            }

            ListView {
                id: sheetList
                anchors.top: newCell.bottom
                model: libraryModel
                height: childrenRect.height
                width: parent.width
                delegate: Rectangle {
                    width: parent.width
                    height: childrenRect.height
                    color: index % 2 == 0 ? Colors.base03 : Colors.base02
                    Text {
                        text: name
                        color: Colors.base0
                        topPadding: 3
                        leftPadding: 5
                        bottomPadding: topPadding
                    }
                }
            }

            Text {
                id: newSheet
                anchors.top: sheetList.bottom
                width: parent.width
                text: "New sheet..."
                color: Colors.base0
                padding: 5
            }
        }
    }

    property alias libraryModel: libraryView.libraryModel
}

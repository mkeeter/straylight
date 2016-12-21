import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Colors 1.0

Item {
    id: root
    width: parent.width
    Layout.minimumHeight: 100

    Text {
        id: header
        width: root.width
        text: "<b>Library:<b>"
        color: Colors.base0
        padding: 5
    }

    ScrollView {
        id: libraryView
        anchors.top: header.bottom
        anchors.bottom: root.bottom

        property ListModel libraryModel
        style: ScrollViewStyle {
            transientScrollBars: true
        }

        ListView {
            id: sheetList
            model: libraryModel
            delegate: Rectangle {
                width: root.width
                height: childrenRect.height
                color: ((index % 2) == 0) ? Colors.base02 : Colors.base03
                Text {
                    text: name
                    color: Colors.base0
                    topPadding: 3
                    leftPadding: 5
                    bottomPadding: topPadding
                }
            }
        }
    }

    property alias libraryModel: libraryView.libraryModel
}

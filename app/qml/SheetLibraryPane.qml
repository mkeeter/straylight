import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Style 1.0

ColumnLayout {
    // Library title
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height
        color: Style.primary
        Text {
            id: header
            text: "<b>Library</b>"
            color: Style.textLightPrimary
            padding: 5
        }
    }

    property ListModel libraryModel: ListModel { }

    ScrollView {
        Layout.fillWidth: true
        Layout.fillHeight: true

        id: libraryView

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

                Text {
                    id: itemName
                    text: name
                    color: Style.textDarkSecondary
                    topPadding: 3
                    leftPadding: 5
                    bottomPadding: topPadding
                }

                MouseArea {
                    id: hover
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        console.log("Tried to do something")
                    }
                }
            }
        }
    }
}

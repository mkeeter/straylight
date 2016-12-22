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
        width: parent.width

        id: libraryView
        Layout.fillHeight: true

        property ListModel libraryModel
        style: ScrollViewStyle {
            transientScrollBars: true
        }

        ListView {
            id: sheetList
            model: libraryModel
            width: parent.width

            delegate: Rectangle {
                height: childrenRect.height
                width: parent.width

                color: hover.containsMouse ? Colors.base01 : (index % 2 == 0 ? Colors.base02 : Colors.base03)
                Text {
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

    Rectangle {
        Layout.fillWidth: true
        height: 2
        color: Colors.base00
        visible: creator.preferredHeight > 0
    }

    SheetCreator {
        id: creator
        Layout.fillWidth: true
        Layout.preferredHeight: preferredHeight
    }

    property alias libraryModel: libraryView.libraryModel
}

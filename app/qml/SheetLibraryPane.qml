import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Style 1.0
import "/js/model_util.js" as ModelUtil

ColumnLayout {
    property ListModel libraryModel: ListModel { }

    // Used when deserializing from bridge
    property int itemIndex: 0

    function push() {
        itemIndex = 0;
    }

    function pop() {
        ModelUtil.pop(libraryModel, itemIndex)
    }

    function sheet(sheet_index, sheet_name, editable, insertable)
    {
        var found = ModelUtil.findItem('sheet', sheet_index,
                                       itemIndex, libraryModel)

        libraryModel.setProperty(itemIndex, "name", sheet_name)
        libraryModel.setProperty(itemIndex, "editable", editable)
        libraryModel.setProperty(itemIndex, "insertable", insertable)

        itemIndex++
    }

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
                height: sheetName.height
                anchors.left: parent.left
                anchors.right: parent.right

                Text {
                    id: sheetName
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

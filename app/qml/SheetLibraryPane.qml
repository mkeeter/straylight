import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Style 1.0
import Awesome 4.7
import "/js/model_util.js" as ModelUtil

ColumnLayout {
    id: lib
    property ListModel libraryModel: ListModel { }

    signal addInstance(int targetSheetIndex)

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
        id: titleRect

        Layout.fillWidth: true
        height: childrenRect.height

        color: Style.primary
        Text {
            id: header
            text: "<b>Library</b>"
            color: Style.textLightPrimary
            padding: 5
        }

        IconButton {
            anchors.right: titleRect.right
            anchors.rightMargin: 10
            anchors.verticalCenter: header.verticalCenter

            text: Awesome.fa_plus
            mode: "light"
            toolTip: "Add sheet"
            onClicked: { console.log("Adding sheet") }
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

            delegate: SheetLibraryDelegate {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.leftMargin: 10

                Component.onCompleted: {
                    addInstance.connect(lib.addInstance)
                }
            }

        }
    }
}

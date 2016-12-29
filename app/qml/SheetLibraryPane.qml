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
    signal eraseSheet(int targetSheetIndex)
    signal addSheet()
    signal closePane()

    function renameLast() {
        // XXX This is slightly evil, as we're poking members of the ListView
        // directly, but should be safe enough...
        var children = sheetList.contentItem.children
        var y = 0
        var lastIndex = 0
        for(var i in children) {
            if (children[i].objectName === "SheetLibraryDelegate") {
                y += children[i].height
                lastIndex = i
            }
        }
        y -= libraryView.height
        scrollTo.to = Math.max(y, 0)
        scrollTo.start()
        children[lastIndex].renameMe()
    }

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

        Row {
            anchors.right: titleRect.right
            anchors.rightMargin: 10
            anchors.verticalCenter: header.verticalCenter
            spacing: 5

            IconButton {
                text: Awesome.fa_plus_square
                mode: "light"
                toolTip: "Add sheet"
                onClicked: { addSheet() }
            }
            IconButton {
                text: Awesome.fa_arrow_circle_down
                mode: "light"
                toolTip: "Hide library"
                onClicked: { closePane() }
            }
        }
    }

    Text {
        visible: libraryModel.count == 0
        text: "No sheets in library"
        color: Style.textDarkHint
        padding: 10
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
                    eraseSheet.connect(lib.eraseSheet)
                }
            }

            /* TODO: enable these once I figure out layout glitches
            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 100 }
            }
            move: Transition {
                NumberAnimation { properties: "x,y"; duration: 100 }
            }
            remove: Transition {
                NumberAnimation { property: "opacity"; to: 0; duration: 100 }
            }
            add: Transition {
                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 100 }
            }
            */

            NumberAnimation {
                id: scrollTo
                target: libraryView.flickableItem
                property: 'contentY'
                duration: 100
            }
        }
    }
}

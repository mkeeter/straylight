import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Style 1.0
import Bridge 1.0
import Graph 1.0
import Awesome 4.7

SplitView {
    id: sview
    orientation: Qt.Vertical

    property var sheetEnv

    // Used in drawing title
    property string instanceName: ""
    property string sheetName: ""

    property var sheetInstanceModel: Graph.modelOf(sheetEnv)

    // Forwards a call to fix item layout
    function fixLayout() { items.fixLayout() }

    Component.onCompleted: {
        items.itemsModel = sheetInstanceModel.itemsModel()
        lib.libraryModel = sheetInstanceModel.libraryModel()
        lib.slideOpen()
    }

    // XXX This is an inelegant strategy to call renameLast
    // after a short wait, which gives the engine time to construct
    // the Delegate and animate its appearance
    Timer {
        id: renameLastItemTimer
        interval: 100
        onTriggered: { items.renameLast() }
    }
    Timer {
        id: renameLastSheetTimer
        interval: 100
        onTriggered: { lib.renameLast() }
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true

        ColumnLayout {
            anchors.fill: parent

            SheetTitle {
                Layout.fillWidth: true
                onInsertCell: {
                    //var instance = sheetEnv[sheetEnv.length - 1]
                    //var sheet = Bridge.sheetOf(instance)
                    //var name = Bridge.nextItemName(sheet)
                    sheetInstanceModel.insertCell()
                    renameLastItemTimer.restart()
                }
            }
            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Text {
                    opacity: items.itemsModel.count == 0
                    Behavior on opacity { OpacityAnimator { duration: 100 }}
                    text: "Nothing here yet..."
                    color: Style.textDarkHint
                    padding: 10
                }
                SheetItemsPane {
                    anchors.fill: parent
                    id: items
                    width: parent.width
                }
            }
        }

        IconButton {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 10
            text: Awesome.fa_arrow_circle_up
            visible: !lib.visible
            onClicked: { lib.slideOpen() }
            toolTip: "Show library"
        }
    }

    SheetLibraryPane {
        id: lib
        Layout.fillWidth: true

        function slideOpen() {
            anim.from = 0
            anim.to = Math.max(200, items.height / 3)
            anim.start()
            visible = true
        }

        function slideClose() {
            anim.from = height
            anim.to = 0
            anim.start()
        }

        property var anim: NumberAnimation {
            target: lib
            properties: 'Layout.minimumHeight, Layout.maximumHeight'
            duration: 100
            onStopped: {
                lib.Layout.minimumHeight = -1
                lib.Layout.maximumHeight = -1
                lib.visible = (to > 0)
            }
        }
        visible: false

        onAddInstance: {
            sheetInstanceModel.insertInstance(targetSheetIndex)
            renameLastItemTimer.restart()
        }

        onAddSheet: {
            sheetInstanceModel.insertSheet()
            renameLastSheetTimer.restart()
        }

        onEraseSheet: {
            sheetInstanceModel.eraseSheet(targetSheetIndex)
        }

        onClosePane: { slideClose() }
    }

    handleDelegate: Component {
        id: splitViewHandle
        Rectangle {
            height: 3
            color: Style.accent
        }
    }
}

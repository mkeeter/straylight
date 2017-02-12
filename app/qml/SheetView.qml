import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Style 1.0
import Bridge 1.0
import Awesome 4.7

SplitView {
    orientation: Qt.Vertical

    property var sheetEnv
    // TODO property alias sheetIndex: lib.sheetIndex

    // Used in drawing title
    property string instanceName: ""
    property string sheetName: ""

    // Keep track of bridge deserialization protocol
    property var bridgeEnv: []
    property bool visited: false
    function inBridgeEnv() {
        return _.isEqual(sheetEnv, bridgeEnv)
    }

    // Forwards a call to fix item layout
    function fixLayout() { items.fixLayout() }

    Component.onCompleted: {
        Bridge.push.connect(push)
        Bridge.pop.connect(pop)
        lib.slideOpen()
    }

    Component.onDestruction: {
        Bridge.push.disconnect(push)
        Bridge.pop.disconnect(pop)
    }

    function connectBridge() {
        Bridge.instance.connect(instance)
        Bridge.input.connect(input)
        Bridge.output.connect(output)
        Bridge.cell.connect(cell)
        Bridge.sheet.connect(sheet)
    }

    function disconnectBridge() {
        Bridge.instance.disconnect(instance)
        Bridge.input.disconnect(input)
        Bridge.output.disconnect(output)
        Bridge.cell.disconnect(cell)
        Bridge.sheet.disconnect(sheet)
    }

    function push(instance_index, instance_name, sheet_name) {
        // Reset the visited flag on our first push
        if (bridgeEnv.length == 0) {
            visited = false
        }

        bridgeEnv.push(instance_index)
        if (inBridgeEnv()) {
            visited = true
            instanceName = instance_name
            sheetName = sheet_name

            items.push()
            lib.push()
            Bridge.ping()

            connectBridge()
        } else {
            disconnectBridge()
        }
    }

    function pop() {
        if (inBridgeEnv()) {
            items.pop()
            lib.pop()
        }
        bridgeEnv.pop()
        if (inBridgeEnv()) {
            items.cleanPreviousInstance()
            connectBridge()
        } else {
            disconnectBridge()
        }

        // If we're done and didn't visit this sheet, then
        // request that we open to the next-highest sheet
        if (bridgeEnv.length == 0 && !visited)
        {
            Bridge.push.disconnect(push)
            Bridge.pop.disconnect(pop)

            var env_ = sheetEnv.slice()
            env_.pop()

            // TODO: we should open to the longest prefix visited here
            sheetStack.closeTo(env_)
        }
    }

    function instance(instance_index, name, sheet) {
        items.instance(instance_index, name, sheet)
    }

    function input(cell_index, name, expr, valid, value) {
        items.input(cell_index, name, expr, valid, value)
    }

    function output(cell_index, name, valid, value) {
        items.output(cell_index, name, valid, value)
    }

    function cell(cell_index, name, expr, type, valid, value, ptr) {
        items.cell(cell_index, name, expr, type, valid, value)
    }

    function sheet(sheet_index, name, editable, insertable) {
        lib.sheet(sheet_index, name, editable, insertable)
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
                    var instance = sheetEnv[sheetEnv.length - 1]
                    var sheet = Bridge.sheetOf(instance)
                    var name = Bridge.nextItemName(sheet)
                    Bridge.insertCell(sheet, name)
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
            var instance = sheetEnv[sheetEnv.length - 1]
            var sheet = Bridge.sheetOf(instance)
            var name = Bridge.nextItemName(sheet)
            Bridge.insertInstance(sheet, name, targetSheetIndex)
            renameLastItemTimer.restart()
        }

        onAddSheet: {
            var instance = sheetEnv[sheetEnv.length - 1]
            var sheet = Bridge.sheetOf(instance)
            var name = Bridge.nextSheetName(sheet)
            Bridge.insertSheet(sheet, name)
            renameLastSheetTimer.restart()
        }

        onEraseSheet: {
            Bridge.eraseSheet(targetSheetIndex)
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

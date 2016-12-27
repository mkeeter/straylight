import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Style 1.0
import Bridge 1.0

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
    property int bridgeInstance
    function inBridgeEnv() {
        return _.isEqual(sheetEnv, bridgeEnv)
    }

    Component.onCompleted: {
        Bridge.push.connect(push)
        Bridge.pop.connect(pop)
        Bridge.instance.connect(instance)
        Bridge.input.connect(input)
        Bridge.output.connect(output)
        Bridge.cell.connect(cell)
    }

    function push(instance_name, sheet_name) {
        // Reset the visited flag on our first push
        if (bridgeEnv.length == 0) {
            visited = false
        }

        bridgeEnv.push(bridgeInstance)
        if (inBridgeEnv()) {
            visited = true
            items.push()
            instanceName = instance_name
            sheetName = sheet_name
        }
    }

    function pop() {
        if (inBridgeEnv()) {
            items.pop()
        }
        bridgeEnv.pop()
        if (inBridgeEnv()) {
            items.cleanPreviousInstance()
        }

        // If we're done and didn't visit this sheet, then
        // request that we open to the next-highest sheet
        if (bridgeEnv.length == 0 && !visited)
        {
            // TODO: we should open to the longest prefix visited here
            Bridge.push.disconnect(push)
            Bridge.pop.disconnect(pop)
            Bridge.instance.disconnect(instance)
            Bridge.input.disconnect(input)
            Bridge.output.disconnect(output)
            Bridge.cell.disconnect(cell)

            var env_ = sheetEnv.slice()
            env_.pop()

            sheetStack.openTo(env_)
        }
    }

    function instance(instance_index, name, sheet) {
        bridgeInstance = instance_index
        if (inBridgeEnv()) {
            items.instance(instance_index, name, sheet)
        }
    }

    function input(cell_index, name, expr, valid, value) {
        if (inBridgeEnv()) {
            items.input(cell_index, name, expr, valid, value)
        }
    }

    function output(cell_index, name, valid, value) {
        if (inBridgeEnv()) {
            items.output(cell_index, name, valid, value)
        }
    }

    function cell(cell_index, name, expr, type, valid, value) {
        if (inBridgeEnv()) {
            items.cell(cell_index, name, expr, type, valid, value)
        }
    }

    ColumnLayout {
        SheetTitle {
            Layout.fillWidth: true
        }
        SheetItemsPane {
            id: items
            width: parent.width
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Layout.fillHeight: true
    }

    SheetLibraryPane {
        id: lib
        width: parent.width
    }

    handleDelegate: Component {
        id: splitViewHandle
        Rectangle {
            height: 3
            color: Style.accent
        }
    }
}

import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Colors 1.0
import Bridge 1.0

SplitView {
    orientation: Qt.Vertical

    property var sheetEnv

    // Keep track of bridge deserialization protocol
    property var bridgeEnv: []
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

    function push() {
        bridgeEnv.push(bridgeInstance)
        if (inBridgeEnv()) {
            items.push()
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
        SheetTitle { }
        SheetItemsPane {
            id: items
            width: parent.width
            Layout.fillHeight: true
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
            height: 1
            color: Colors.base01
        }
    }

    property alias sheetIndex: lib.sheetIndex
}

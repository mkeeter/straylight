import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Bridge 1.0

ScrollView {
    property ListModel itemsModel: ListModel { }

    property var sheetEnv

    // Used when deserializing from bridge
    property int itemIndex: 0
    property int ioIndex: 0

    // Keep track of bridge deserialization protocol
    property var bridgeEnv: []
    property int bridgeInstance
    function inBridgeEnv() {
        return _.isEqual(sheetEnv, bridgeEnv)
    }

    Component.onCompleted: {
        Bridge.push.connect(push)
        Bridge.pop.connect(pop)

        // Theoretically, we could connect these only when the
        // bridge environment matches the sheet's environment
        Bridge.cell.connect(cell)
        Bridge.input.connect(input)
        Bridge.output.connect(output)
        Bridge.instance.connect(instance)
    }

    function push() {
        bridgeEnv.push(bridgeInstance)
        if (inBridgeEnv()) {
            itemIndex = 0;
        }
    }

    function pop() {
        if (inBridgeEnv()) {
            while (itemIndex < itemsModel.count) {
                itemsModel.remove(itemIndex)
                itemIndex++
            }
        }
        bridgeEnv.pop()
    }

    // Search forward through the list of items to find our cell,
    // moving it to the current index if it's in the wrong place
    function findItem(item_type, item_index, startIndex, model) {
        var searchIndex = startIndex
        while (searchIndex < model.count)
        {
            var item = model.get(searchIndex)
            if (item.type == item_type &&
                item.itemIndex == item_index)
            {
                if (searchIndex != itemIndex)
                {
                    model.move(searchIndex, startIndex, 1)
                }
                return true
            }
            searchIndex++;
        }
        return false
    }

    function instance(instance_index, name) {
        bridgeInstance = instance_index
        if (inBridgeEnv()) {
            var found = findItem('instance', instance_index,
                                 itemIndex, itemsModel)

            // If we reached the end of the list and didn't find anything, then
            // insert a brand new instance at our current index
            if (!found)
            {
                itemsModel.insert(itemIndex,
                    {type: 'instance', itemIndex: instance_index,
                    ioCells: []})
            }
            itemsModel.setProperty(itemIndex, "name", name)

            itemIndex++
            ioIndex = 0
        }
    }

    function input(cell_index, name, expr, valid, value) {
        if (inBridgeEnv()) {
            var model = itemsModel.get(itemIndex - 1).ioCells
            var found = findItem('input', cell_index, ioIndex, model);

            if (!found)
            {
                model.insert(ioIndex,
                    {type: 'input', itemIndex: cell_index})
            }

             model.setProperty(ioIndex, "name", name)
             model.setProperty(ioIndex, "valid", valid)
             model.setProperty(ioIndex, "expr", expr)
             model.setProperty(ioIndex, "value", value)

             ioIndex++
        }
    }

    function output(cell_index, name, valid, value) {
        if (inBridgeEnv()) {
            var model = itemsModel.get(itemIndex - 1).ioCells
            var found = findItem('output', cell_index, ioIndex, model);

            console.log("output happening " + found)
            if (!found)
            {
                model.insert(ioIndex,
                    {type: 'output', itemIndex: cell_index})
            }

             model.setProperty(ioIndex, "name", name)
             model.setProperty(ioIndex, "valid", valid)
             model.setProperty(ioIndex, "value", value)

             ioIndex++
        }
    }

    function cell(cell_index, name, expr, type, valid, value) {
        if (inBridgeEnv()) {
            var found = findItem('cell', cell_index,
                                 itemIndex, itemsModel)

            // If we reached the end of the list and didn't find anything, then
            // insert a brand new cell at our current index
            if (!found)
            {
                itemsModel.insert(itemIndex,
                    {type: 'cell', itemIndex: cell_index})
            }

            // Then update all of the relevant properties
            itemsModel.setProperty(itemIndex, "name", name)
            itemsModel.setProperty(itemIndex, "valid", valid)
            itemsModel.setProperty(itemIndex, "expr", expr)
            itemsModel.setProperty(itemIndex, "value", value)

            // TODO: care about type here
            itemIndex++
        }
    }

    ListView {
        anchors.fill: parent
        model: itemsModel
        spacing: 5
        delegate: SheetItemDelegate {
            width: parent.width
        }

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
    }
    style: ScrollViewStyle {
        transientScrollBars: true
    }
}

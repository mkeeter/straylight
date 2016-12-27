import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Bridge 1.0

ScrollView {
    property ListModel itemsModel: ListModel { }

    // Used when deserializing from bridge
    property int itemIndex: 0
    property int ioIndex: 0

    function renameLast() {
        // XXX This is slightly evil, as we're poking members of the ListView
        // directly, but should be safe enough...
        var children = listView.contentItem.children
        var y = 0
        for(var i in children) {
            y += children[i].height
        }
        y -= items.height
        scrollTo.to = Math.max(y, 0)
        scrollTo.start()
        children[i].renameMe()
    }

    function push() {
        itemIndex = 0;
    }

    function setLast(model) {
        for (var i=0; i < model.count - 1; ++i) {
            model.setProperty(i, 'last', false)
        }
        if (i < model.count) {
            model.setProperty(i, 'last', true)
        }
    }

    function pop() {
        while (itemIndex < itemsModel.count) {
            itemsModel.remove(itemIndex++)
        }
        setLast(itemsModel)
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

    function instance(instance_index, name, sheet) {
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
        itemsModel.setProperty(itemIndex, "sheet", sheet)

        itemIndex++
        ioIndex = 0
    }

    function cleanPreviousInstance() {
        if (itemIndex > 0 && itemIndex <= itemsModel.count &&
            itemsModel.get(itemIndex - 1).type == 'instance')
        {
            var model = itemsModel.get(itemIndex - 1).ioCells
            while (ioIndex < model.count) {
                model.remove(ioIndex++)
            }
            setLast(model)
        }
    }

    function input(cell_index, name, expr, valid, value) {
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

    function output(cell_index, name, valid, value) {
        var model = itemsModel.get(itemIndex - 1).ioCells
        var found = findItem('output', cell_index, ioIndex, model);

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

    function cell(cell_index, name, expr, type, valid, value) {
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
        itemsModel.setProperty(itemIndex, "ioType",
            ["unknown", "basic", "input", "output"][type])

        itemIndex++
    }

    ListView {
        id: listView

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

        NumberAnimation {
            id: scrollTo
            target: items.flickableItem
            property: 'contentY'
        }
    }
    style: ScrollViewStyle {
        transientScrollBars: true
    }
}

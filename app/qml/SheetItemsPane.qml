import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Bridge 1.0
import "/js/model_util.js" as ModelUtil

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
        var lastIndex = 0
        for(var i in children) {
            if (children[i].objectName === "SheetItemDelegate") {
                y += children[i].height
                lastIndex = i
            }
        }
        y -= items.height
        scrollTo.to = Math.max(y, 0)
        scrollTo.start()
        children[lastIndex].renameMe()
    }

    function push() {
        itemIndex = 0;
    }

    function pop() {
        ModelUtil.pop(itemsModel, itemIndex)
    }

    function instance(instance_index, name, sheet) {
        ModelUtil.findItem('instance', instance_index, itemIndex, itemsModel,
            {'ioCells': new Array()})

        itemsModel.setProperty(itemIndex, "name", name)
        itemsModel.setProperty(itemIndex, "sheet", sheet)

        itemIndex++
        ioIndex = 0
    }

    function cleanPreviousInstance() {
        if (itemIndex > 0 && itemIndex <= itemsModel.count &&
            itemsModel.get(itemIndex - 1).type == 'instance')
        {
            ModelUtil.pop(itemsModel.get(itemIndex - 1).ioCells, ioIndex)
        }
    }

    function input(cell_index, name, expr, valid, value) {
        var model = itemsModel.get(itemIndex - 1).ioCells
        ModelUtil.findItem('input', cell_index, ioIndex, model)

        model.setProperty(ioIndex, "name", name)
        model.setProperty(ioIndex, "valid", valid)
        model.setProperty(ioIndex, "expr", expr)
        model.setProperty(ioIndex, "value", value)

        ioIndex++
    }

    function output(cell_index, name, valid, value) {
        var model = itemsModel.get(itemIndex - 1).ioCells
        ModelUtil.findItem('output', cell_index, ioIndex, model)

        model.setProperty(ioIndex, "name", name)
        model.setProperty(ioIndex, "valid", valid)
        model.setProperty(ioIndex, "value", value)

        ioIndex++
    }

    function cell(cell_index, name, expr, type, valid, value) {
        ModelUtil.findItem('cell', cell_index, itemIndex, itemsModel)

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
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: 15
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
            duration: 100
        }
    }
    style: ScrollViewStyle {
        transientScrollBars: true
    }
}

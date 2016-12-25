import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Bridge 1.0

ScrollView {
    property ListModel itemsModel: ListModel { }

    property var sheetEnv
    property bool listening: false
    property int currentIndex: 0

    // Keep track of bridge deserialization protocol
    property var bridgeEnv: []
    property int bridgeInstance

    Component.onCompleted: {
        Bridge.push.connect(push)
        Bridge.pop.connect(pop)
        Bridge.cell.connect(cell)
        Bridge.instance.connect(instance)
        Bridge.sync()
    }

    function push() {
        bridgeEnv.push(bridgeInstance)
        console.log(bridgeEnv)
        console.log(sheetEnv)
        if (_.isEqual(sheetEnv, bridgeEnv)) {
            console.log("It's a match")
            listening = true;
            currentIndex = 0;
        }
    }

    function pop() {
        bridgeEnv.pop()

        if (listening) {
            while (currentIndex < itemsModel.count) {
                itemsModel.remove(currentIndex)
                currentIndex++
            }
            listening = false
        }
    }

    function instance(instance_index, name) {
        bridgeInstance = instance_index
        if (!listening) {
            return;
        }
        // TODO: draw instance here
    }

    function cell(cell_index, name, expr, type, valid, value) {
        console.log("Got cell")
        if (!listening) {
            return
        }

        console.log(cell_index)
        console.log(name)

        // Search forward through the list of items to find our cell,
        // moving it to the current index if it's in the wrong place
        var searchIndex = currentIndex
        var found = false
        while (!found && searchIndex < itemsModel.count)
        {
            var item = itemsModel.get(searchIndex)
            if (item.type == 'cell' && item.cellIndex == cell_index)
            {
                if (searchIndex != currentIndex)
                {
                    itemsModel.move(searchIndex, currentIndex, 1)
                }
                found = true
            }
            searchIndex++;
        }

        // If we reached the end of the list and didn't find anything, then
        // insert a brand new cell at our current index
        if (!found)
        {
            itemsModel.insert(currentIndex,
                {type: 'cell', cellIndex: cell_index})
        }

        // Then update all of the relevant properties
        itemsModel.setProperty(currentIndex, "name", name)
        itemsModel.setProperty(currentIndex, "valid", valid)
        itemsModel.setProperty(currentIndex, "expr", expr)
        itemsModel.setProperty(currentIndex, "value", value)

        // TODO: care about type here
        currentIndex++
    }

    ListView {
        anchors.fill: parent
        model: itemsModel
        spacing: 5
        delegate: SheetItemDelegate {}

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

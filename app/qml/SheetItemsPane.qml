import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Bridge 1.0

ScrollView {
    property ListModel itemsModel: ListModel { }

    property int sheetIndex
    property bool listening: false
    property int currentIndex: 0

    Component.onCompleted: {
        Bridge.beginSheet.connect(beginSheet)
        Bridge.endSheet.connect(endSheet)
        Bridge.cell.connect(cell)
        Bridge.sync()
    }

    function beginSheet(i) {
        if (i == sheetIndex) {
            listening = true;
            currentIndex = 0;
        }
    }

    function endSheet(i) {
        if (listening) {
            while (currentIndex < itemsModel.count) {
                itemsModel.remove(currentIndex)
                currentIndex++
            }
            listening = false
        }
    }

    function cell(cell_index, name, expr, type, valid, value) {
        if (!listening) {
            return
        }

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
                    console.log("Moving " + searchIndex + " to " + currentIndex)
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
    }
    style: ScrollViewStyle {
        transientScrollBars: true
    }
}

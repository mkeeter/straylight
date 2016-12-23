import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Bridge 1.0

ScrollView {
    property ListModel itemsModel: ListModel { }

    property int sheetIndex
    property var viewEnv
    property bool listening: false
    property int currentIndex: 0

    Component.onCompleted: {
        Bridge.beginSheet.connect(beginSheet)
        Bridge.cell.connect(cell)
        Bridge.sync()
    }

    function beginSheet(i) {
        if (i == sheetIndex)
        {
            listening = true;
            currentIndex = 0;
        }
    }

    function cell(cell_index, name, expr, type, valid, value)
    {
        if (!listening)
            return

        var i = currentIndex < itemsModel.count
                    ? itemsModel.get(currentIndex)
                    : {type: 'none', cellIndex: -1}
        if (i.type != 'cell' || i.cellIndex != cell_index)
        {
            if (i.type != 'none')
                itemsModel.remove(currentIndex)
            itemsModel.insert(currentIndex,
                {type: 'cell', cellIndex: cell_index})
        }
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
        spacing: 0
        delegate: SheetItemDelegate {}
    }
    style: ScrollViewStyle {
        transientScrollBars: true
    }
}

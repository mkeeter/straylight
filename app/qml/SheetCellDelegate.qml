import QtQuick 2.7
import QtQuick.Layouts 1.0

import Bridge 1.0

Column {
    spacing: 5

    SheetItemTitle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5

        onEraseMe: {
            sheetModel.eraseCell(uniqueIndex)
        }

        Component.onCompleted: {
            renameMe.connect(openRename)
            focusCell.connect(cellBody.setFocus)
        }
    }

    SheetCellBody {
        id: cellBody

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
    }

    // Dummy item that forwards changes into cell body
    Item {
        property string text: expr
        onTextChanged: { cellBody.setExpr(text) }
    }
}

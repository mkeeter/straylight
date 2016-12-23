import QtQuick 2.7
import QtQuick.Layouts 1.0

import Colors 1.0
import Awesome 4.7
import Bridge 1.0

Column {
    id: sheetItemDelegate
    width: parent.width
    spacing: 8

    function bestDelegate(t) {
        if (t == "cell")
            return cellDelegate;
        else
            return instanceDelegate;
    }

    Component {
        id: cellDelegate

        Column {
            spacing: 5

            SheetItemTitle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5

                text: name
                itemIndex: cellIndex

                onEraseMe: {
                    Bridge.eraseCell(cellIndex)
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
    }

    Loader {
        id: itemDisplay
        width: parent.width
        sourceComponent: bestDelegate(type)
    }

    Rectangle {
        width: parent.width
        height: 2
        color: Colors.base0
    }
}



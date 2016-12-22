import QtQuick 2.7
import QtQuick.Layouts 1.0

import Colors 1.0
import Awesome 4.7

Item {
    id: sheetItemDelegate
    width: parent.width
    height: childrenRect.height

    function bestDelegate(t) {
        if (t == "cell")
            return cellDelegate;
        else
            return instanceDelegate;
    }

    Component {
        id: cellDelegate

        Column {
            width: parent.width
            height: childrenRect.height

            ItemName {
                width: parent.width
                text: name
            }

            CellExpr {
                width: parent.width
                expr: expr
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
        anchors.top: itemDisplay.bottom
    }
}



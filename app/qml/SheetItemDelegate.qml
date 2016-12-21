import QtQuick 2.7
import QtQuick.Layouts 1.0

import Colors 1.0

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
        GridLayout {
            anchors.right: parent.right
            columns: 3

            Text {
                color: Colors.base0
                Layout.column: 0
                Layout.row: 0
                text: '<b>Name: </b>'
            }
            ItemName {
                Layout.column: 1
                Layout.row: 0
                Layout.fillWidth: true

                text: name
            }
            Rectangle {
                Layout.column: 2
                Layout.row: 0

                color:'black';width:10;height:10
            }

            Text {
                color: Colors.base0
                Layout.column: 0
                Layout.row: 1
                text: '<b>Expr: </b>'
            }
            TextEdit {
                Layout.column: 1
                Layout.row: 1
                Layout.fillWidth: true

                text: expr
                selectByMouse: true
            }
            Rectangle {
                Layout.column: 2
                Layout.row: 1
                color:'black'
                width:10
                height:10
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
        color: 'black'
        anchors.top: itemDisplay.bottom
        anchors.topMargin: 8
    }
}



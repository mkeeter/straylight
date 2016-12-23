import QtQuick 2.7
import QtQuick.Layouts 1.0

import Colors 1.0
import Awesome 4.7
import Bridge 1.0

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
            spacing: 5

            ItemTitle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5

                text: name
                itemIndex: cellIndex

                onEraseMe: {
                    Bridge.eraseCell(cellIndex)
                }
            }

            CellExpr {
                id: cell
                width: parent.width
                onExprChanged: {
                    Bridge.setExpr(cellIndex, expr)
                }
            }

            Row {
                Text {
                    visible: !valid
                    text: Awesome.fa_exclamation_triangle
                    color: Colors.base0
                    font.family: fontAwesome.name
                    padding: 5
                    leftPadding: 15
                    topPadding: 8
                }

                Text {
                    id: resultText
                    text: value

                    font.family: sansSerif.name
                    color: Colors.base0
                    padding: 5
                }
            }

            /*
             *  Dummy text field to forward changes into cell
             */
            Text {
                visible: false
                text: expr
                onTextChanged: {
                    if (cell.expr != text)
                        cell.expr = text
                }
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



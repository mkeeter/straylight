import QtQuick 2.7
import QtQuick.Layouts 1.0

import Colors 1.0
import Awesome 4.7
import Bridge 1.0

Column {
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

                onEraseMe: {
                    Bridge.eraseCell(itemIndex)
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

    Component {
        id: instanceDelegate

        Column {
            spacing: 5

            SheetItemTitle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5

                text: name

                onEraseMe: {
                    Bridge.eraseInstance(itemIndex)
                }
            }

            ListView {
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height

                model: ioCells

                delegate: SheetInstanceIODelegate {
                    width: parent.width
                }

                Component.onCompleted: {
                    console.log("COmpleted view")
                    for (var i=0; i < model.count; ++i)
                        console.log("   " + i + ": " + model.get(i).itemIndex + " " + model.get(i).type)
                    }
            }
        }
    }

    Loader {
        width: parent.width
        sourceComponent: bestDelegate(type)
    }

    Rectangle {
        width: parent.width
        height: 2
        color: Colors.base02
    }
}



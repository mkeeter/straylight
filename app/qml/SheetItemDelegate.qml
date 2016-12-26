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

            // Store the item index under a different name so that we can
            // refer to it in IO cells (which have their own itemIndex)
            property int instanceIndex: itemIndex
            property real labelPadding: 50

            SheetItemTitle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5

                onEraseMe: {
                    Bridge.eraseInstance(itemIndex)
                }

                onOpenSheet: {
                    var env = sheetEnv.slice()
                    env.push(itemIndex)
                    sheetStack.openTo(env)
                }
            }

            ListView {
                id: ioView

                interactive: false
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.right: parent.right
                height: childrenRect.height

                model: ioCells

                delegate: SheetInstanceIODelegate {
                    width: ioView.width
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



import QtQuick 2.7
import QtQuick.Layouts 1.0

import Style 1.0
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
                onCloseSheet: {
                    var env = sheetEnv.slice()
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

    Row {
        width: parent.width

        Rectangle {
            id: instancePadding
            // TODO color: isInstanceOpen ? Colors.base00 : 'transparent'
            width: 10
            height: del.height
            Behavior on color {
                ColorAnimation { duration: 100 }
            }
        }

        Loader {
            id: del
            width: parent.width - instancePadding.width - scrollPadding.width
            sourceComponent: bestDelegate(type)
        }

        Rectangle {
            id: scrollPadding
            color: 'transparent'
            width: 10
            height: del.height
        }
    }
    property bool isInstanceOpen: sheetStack.env.indexOf(itemIndex) != -1

    Rectangle {
        visible: !last
        width: parent.width
        height: 2
        color: Style.dividerDark
    }
}



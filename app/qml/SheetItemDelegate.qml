import QtQuick 2.7
import QtQuick.Layouts 1.0

import Style 1.0
import Awesome 4.7
import Bridge 1.0

Column {
    objectName: "SheetItemDelegate"
    spacing: 8

    function bestDelegate(t) {
        if (t == "cell")
            return cellDelegate;
        else
            return instanceDelegate;
    }

    signal renameMe

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

                Component.onCompleted: {
                    renameMe.connect(openRename)
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

            // Here, we have a single canonical padding value
            // (which is updated when io labels change)
            property real labelPadding: 0
            property var labelWidths: new Object()

            function setLabelWidth(i, w) {
                if (w == 0) {
                    delete labelWidths[i]
                } else {
                    labelWidths[i] = w
                }
                labelPadding = _.max(labelWidths)
            }

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
                    sheetStack.closeTo(env)
                }

                Component.onCompleted: {
                    renameMe.connect(openRename)
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
                    Component.onCompleted: {
                        var cb = function() { setLabelWidth(index, labelWidth) }
                        labelWidthChanged.connect(cb)
                        cb()
                    }
                }
            }
        }
    }

    Row {
        width: parent.width

        Rectangle {
            id: instancePadding
            color: Style.primary
            width: 10
            height: del.height
            opacity: isInstanceOpen
            Behavior on opacity { OpacityAnimator { duration: 100 }}
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
        width: parent.width
        height: 2
        color: last ? 'transparent' : Style.dividerDark
    }
}



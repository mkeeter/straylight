import QtQuick 2.7
import QtQuick.Layouts 1.0

import Style 1.0
import Awesome 4.7
import Bridge 1.0

Column {
    objectName: "SheetItemDelegate"
    spacing: 8

    function bestDelegate(t) {
        if (t == "cell") {
            return cellDelegate;
        } else {
            return instanceDelegate;
        }
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
                    Bridge.eraseCell(uniqueIndex)
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
    }

    Component {
        id: instanceDelegate

        Column {
            spacing: 5

            // Store the item index under a different name so that we can
            // refer to it in IO cells (which have their own uniqueIndex)
            property int instanceIndex: uniqueIndex

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
                    Bridge.eraseInstance(uniqueIndex)
                }

                onOpenSheet: {
                    var env = sheetEnv.slice()
                    env.push(uniqueIndex)
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

    Item {
        width: parent.width
        height: fullDelegate.height

        Row {
            width: parent.width
            id: fullDelegate
            Rectangle {
                id: instancePadding
                color: Style.primary
                width: 10
                height: itemDelegate.height
                opacity: isInstanceOpen || sstack.selectedItem == itemDelegate
                Behavior on opacity { OpacityAnimator { duration: 100 }}
            }

            Loader {
                id: itemDelegate
                width: parent.width - instancePadding.width
                sourceComponent: bestDelegate(type)
            }
        }
        MouseArea {
            anchors.fill: fullDelegate
            onPressed: { sstack.selectedItem = itemDelegate }
            z: -100
        }
    }
    property bool isInstanceOpen: sheetStack.env.indexOf(uniqueIndex) != -1

    Rectangle {
        width: parent.width
        height: 2
        color: last ? 'transparent' : Style.dividerDark
    }
}



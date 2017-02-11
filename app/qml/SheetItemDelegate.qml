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
        SheetCellDelegate { }
    }

    Component {
        id: instanceDelegate
        SheetInstanceDelegate { }
    }

    Item {
        width: parent.width
        height: fullDelegate.height

        Row {
            width: parent.width
            id: fullDelegate

            // This rectangle is colorful when an instance is open
            Rectangle {
                id: instancePadding
                color: Style.primary
                width: 10
                height: itemDelegate.height
                opacity: isInstanceOpen
                Behavior on opacity { OpacityAnimator { duration: 100 }}
            }

            // Here, we load the correct item delegate
            Loader {
                id: itemDelegate
                width: parent.width - instancePadding.width
                sourceComponent: bestDelegate(type)
            }
        }
        MouseArea {
            anchors.fill: fullDelegate
            onPressed: {
                // TODO: use currentItem / currentIndex of the ListView
                if (sstack.selectedItem == itemDelegate) {
                    sstack.selectedItem = undefined
                } else {
                    sstack.selectedItem = itemDelegate
                    selectRect.focus = true
                }
            }
            z: -100
        }

        // This rectangle handles selection and the actions that can be taken
        // when an item is selected (mostly deletion at the moment)
        Rectangle {
            id: selectRect
            anchors.left: fullDelegate.left
            anchors.verticalCenter: fullDelegate.verticalCenter

            width: fullDelegate.width + 15
            height: fullDelegate.height + 10

            color: Style.itemSelect
            opacity: sstack.selectedItem == itemDelegate
            Behavior on opacity { OpacityAnimator { duration: 50 }}
            Shortcut {
                sequence: StandardKey.Delete
                onActivated: {
                    if (type == 'cell') {
                        Bridge.eraseCell(uniqueIndex)
                    } else if (type == 'instance') {
                        Bridge.eraseInstance(uniqueIndex)
                    } else {
                        console.log("Unknown type " + type)
                    }
                }
            }
            z: -1
        }
    }
    property bool isInstanceOpen: sheetStack.env.indexOf(uniqueIndex) != -1

    Rectangle {
        width: parent.width
        height: 2
        color: last ? 'transparent' : Style.dividerDark
    }
}



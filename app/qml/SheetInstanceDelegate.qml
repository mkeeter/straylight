import QtQuick 2.7
import QtQuick.Layouts 1.0

import Bridge 1.0

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
            sheetInstanceModel().eraseInstance(uniqueIndex)
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


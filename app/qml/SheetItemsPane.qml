import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Bridge 1.0

ScrollView {
    property alias itemsModel: listView.model

    function renameLast() {
        // XXX This is slightly evil, as we're poking members of the ListView
        // directly, but should be safe enough...
        var children = listView.contentItem.children
        var y = 0
        var lastIndex = 0
        for(var i in children) {
            if (children[i].objectName === "SheetItemDelegate") {
                y += children[i].height + listView.spacing
                lastIndex = i
            }
        }
        y -= items.height
        scrollTo.to = Math.max(y, 0)
        scrollTo.start()
        children[lastIndex].renameMe()
    }

    // Here, we correct for an bug in QML's ListView interactions with
    // wrapping text.  A delegate with wrapping text is constructed and
    // laid out, but then text wrapping is applied, so later delegates
    // end up in the wrong y position.
    function fixLayout() {
        var children = listView.contentItem.children
        var y = 0
        for(var i in children) {
            if (children[i].objectName === "SheetItemDelegate") {
                if (y != children[i].y) {
                    children[i].y = y
                }
                y += children[i].height + listView.spacing
            }
        }
    }

    ListView {
        id: listView

        anchors.fill: parent
        spacing: 5

        delegate: SheetItemDelegate {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: 15
        }

        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 100 }
        }
        move: Transition {
            NumberAnimation { properties: "x,y"; duration: 100 }
        }
        remove: Transition {
            NumberAnimation { property: "opacity"; to: 0; duration: 100 }
        }
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 100 }
        }

        NumberAnimation {
            id: scrollTo
            target: items.flickableItem
            property: 'contentY'
            duration: 100
        }
    }
    style: ScrollViewStyle {
        transientScrollBars: true
    }
}

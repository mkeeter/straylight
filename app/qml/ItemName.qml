import QtQuick 2.7
import QtQuick.Controls 1.4

import NameValidator 1.0

Column {
    property int index
    property var indexPath

    NameValidator {
        id: validator
    }
    Rectangle {
        color: '#c0c0c0'
        id: nameRect

        TextInput {
            id: textItem
            text: 'Hello, world'
            selectByMouse: true
            anchors.left: parent.left
            anchors.right: parent.right

            property string oldName
            onActiveFocusChanged: {
                if (focus) oldName = text
                else if (!validator.checkName(text)) {
                    text = oldName
                    errorFlash.start()
                }
            }

            Keys.onReturnPressed: {
                focus = false
            }

            SequentialAnimation {
                id: errorFlash
                ColorAnimation {
                    target: nameRect
                    property: 'color'
                    to: 'red'
                    duration: 100
                }
                ColorAnimation {
                    target: nameRect
                    property: 'color'
                    to: nameRect.color
                    duration: 100
                }
            }
        }
        height: childrenRect.height
        width: parent.width
    }

    Rectangle {
        color: '#d0a0a0'
        height: childrenRect.height
        width: parent.width
        Text {
            height: (textItem.cursorVisible && !validator.checkName(textItem.text)) ? contentHeight + padding : 0

            Behavior on height {
                NumberAnimation {
                    duration: 50
                }
            }
            text: 'Error'
            clip: true
        }
        visible: height > 0
    }
    property alias text: textItem.text
}

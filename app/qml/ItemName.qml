import QtQuick 2.7
import QtQuick.Controls 1.4

import NameValidator 1.0
import Colors 1.0

Column {
    NameValidator {
        id: validator
    }
    Rectangle {
        color: Colors.base02
        id: nameRect

        TextInput {
            color: Colors.base0
            id: textItem
            text: 'Hello, world'
            selectByMouse: true
            anchors.left: parent.left
            anchors.right: parent.right
            padding: 5

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
                    to: Colors.red
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
        color: 'transparent'
        height: childrenRect.height
        width: parent.width
        Text {
            color: Colors.red
            height: (textItem.cursorVisible && !validator.checkName(textItem.text)) ? contentHeight + padding : 0

            Behavior on height {
                NumberAnimation {
                    duration: 50
                }
            }
            topPadding: 2
            bottomPadding: 2
            text: 'Error'
            clip: true
        }
        visible: height > 0
    }
    property alias text: textItem.text
}

import QtQuick 2.7
import QtQuick.Layouts 1.0

import Colors 1.0
import Awesome 4.7

Column {
    spacing: 8

    function bestDelegate(t) {
        if (t == "input")
            return inputDelegate
        else
            return outputDelegate
    }

    Component {
        id: outputDelegate

        Row {
            anchors.left: parent.left
            anchors.right: parent.right

            Text {
                id: resultText
                wrapMode: Text.Wrap
                text: value
                width: parent.width - statusIcon.width

                font.family: sansSerif.name
                color: Colors.base0
            }

            Text {
                id: statusIcon
                visible: !valid
                text: Awesome.fa_exclamation_triangle

                color: Colors.base0
                font.family: fontAwesome.name
                font.pointSize: 14

                leftPadding: 10
                rightPadding: 10
                anchors.verticalCenter: resultText.verticalCenter
            }
        }
    }

    Component {
        id: inputDelegate

        Column {
            SheetInstanceInput {
                width: parent.width
                id: cellBody
            }

            // Dummy item that forwards changes into cell body
            Item {
                property string text: expr
                onTextChanged: { cellBody.setExpr(text) }
            }
        }
    }

    Row {
        width: parent.width

        Text {
            id: nameLabel
            text: name + ":"
            width: labelPadding
            color: Colors.base0
            font.bold: true
            font.family: sansSerif.name
            rightPadding: 10
        }

        Loader {
            width: parent.width - nameLabel.width
            sourceComponent: bestDelegate(type)
        }
    }

    Rectangle {
        width: parent.width
        color: Colors.base02
        height: 2
    }

    Rectangle {
        height: 4
        width: parent.width
        color: 'transparent'
    }
}

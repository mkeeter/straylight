import QtQuick 2.7
import QtQuick.Controls 1.4

import Style 1.0
import Bridge 1.0
import Awesome 4.7

Column {
    spacing: 5

    Rectangle {
        color: 'transparent' // TODO
        id: exprRect

        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right

        TextEdit {
            color: Style.textDarkPrimary
            font.family: fixedWidth.name
            // TODO selectionColor: Colors.base00
            selectByMouse: true

            id: exprText

            anchors.left: parent.left
            anchors.right: parent.right
            padding: 5

            onTextChanged: {
                Bridge.setInput(instanceIndex, itemIndex, text)
            }
        }
    }

    Row {
        anchors.left: parent.left
        anchors.right: parent.right

        Text {
            id: statusIcon
            text: valid ? Awesome.fa_long_arrow_right
                        : Awesome.fa_exclamation_triangle

            color: resultText.color
            font.family: fontAwesome.name
            font.pointSize: 14

            rightPadding: 10
            anchors.verticalCenter: resultText.verticalCenter
        }

        Text {
            id: resultText
            wrapMode: Text.Wrap
            text: value
            width: parent.width - statusIcon.width

            font.family: fixedWidth.name
            color: Style.textDarkSecondary
        }
    }

    function setExpr(e)  {
        if (exprText.text != e)
            exprText.text = e
    }

}


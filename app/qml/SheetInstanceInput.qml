import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

import Style 1.0
import Bridge 1.0
import Awesome 4.7

GridLayout {
    Rectangle {
        Layout.row: 0
        Layout.column: 0
        Layout.fillHeight: true
        width: 2
        color: valid ? Style.textEditValid : Style.textEditInvalid
        opacity: exprText.lineCount > 1
        Behavior on opacity { OpacityAnimator { duration: 100 }}
    }

    TextEdit {
        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true
        Layout.fillHeight: true

        color: Style.textDarkPrimary
        font.family: fixedWidth.name
        selectionColor: Style.textSelect
        selectByMouse: true

        id: exprText

        onTextChanged: {
            Bridge.setInput(instanceIndex, itemIndex, text)
        }
    }

    Rectangle {
        Layout.row: 1
        Layout.column: 1
        Layout.fillWidth: true
        height: 2
        color: valid ? Style.textEditValid : Style.textEditInvalid
        opacity: exprText.lineCount <= 1
        Behavior on opacity { OpacityAnimator { duration: 100 }}
    }

    Row {
        Layout.row: 2
        Layout.column: 1
        Layout.fillWidth: true

        visible: !(resultText.text.trim() === exprText.text.trim())

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


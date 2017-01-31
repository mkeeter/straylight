import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

import Style 1.0
import Bridge 1.0
import Awesome 4.7

GridLayout {

    property color indicatorColor: valid ? Style.textEditValid : Style.textEditInvalid

    Rectangle {
        Layout.row: 0
        Layout.column: 0
        Layout.fillHeight: true
        width: 2
        color: indicatorColor
        opacity: exprEdit.lineCount > 1
        Behavior on opacity { OpacityAnimator { duration: 100 }}
    }

    ExpressionEdit {
        id: exprEdit

        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true
        Layout.fillHeight: true

        function setExpr(text) {
            Bridge.setInput(instanceIndex, uniqueIndex, text)
        }

        function getText(focus) {
            return (activeFocus || expr.length) ? expr : 'Expression'
        }
    }

    Rectangle {
        Layout.row: 1
        Layout.column: 1
        Layout.fillWidth: true
        height: 2
        color: indicatorColor
        opacity: exprEdit.lineCount <= 1
        Behavior on opacity { OpacityAnimator { duration: 100 }}
    }

    Row {
        Layout.row: 2
        Layout.column: 1
        Layout.fillWidth: true

        visible: !(resultText.text.trim() === exprEdit.expr.trim())

        Text {
            id: statusIcon
            text: valid ? Awesome.fa_long_arrow_right
                        : Awesome.fa_exclamation_triangle

            color: indicatorColor
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
        if (exprEdit.expr != e) {
            exprEdit.expr = e
        }
    }

}


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
        opacity: exprText.lineCount > 1
        Behavior on opacity { OpacityAnimator { duration: 100 }}
    }

    Flickable {
        id: flick

        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true
        Layout.preferredHeight: exprText.height
        clip: true

        function scrollTo(r) {
            if (contentX >= r.x) {
                contentX = r.x;
            } else if (contentX + width <= r.x + r.width) {
                contentX = r.x + r.width - width;
            }
        }

        TextEdit {
            color: expr.length ? Style.textDarkPrimary : Style.textDarkHint
            font.family: fixedWidth.name
            selectionColor: Style.textSelect
            selectByMouse: true

            id: exprText
            property string expr

            function syncText() {
                var c = cursorPosition
                text = activeFocus ? expr :
                    (ioType == 'input' ? '(input ...)' :
                        (expr.length ? expr : 'Expression'))
                cursorPosition = Math.min(text.length, c)
            }

            Component.onCompleted: {
                syncText()
                Bridge.installHighlighter(textDocument)
            }
            onActiveFocusChanged: syncText()
            onExprChanged: syncText()

            onTextChanged: {
                if (activeFocus) {
                    Bridge.setExpr(uniqueIndex, text)
                }
            }

            onCursorRectangleChanged: flick.scrollTo(cursorRectangle)
            onCursorPositionChanged: {
                var pos = Bridge.matchedParen(textDocument, cursorPosition)
                //var rect = positionToRectangle(pos)
                console.log(pos)
            }
        }
    }

    Rectangle {
        Layout.row: 1
        Layout.column: 1
        Layout.fillWidth: true
        height: 2
        color: indicatorColor
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
        if (exprText.expr != e) {
            exprText.expr = e
        }
    }
}

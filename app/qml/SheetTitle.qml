import QtQuick 2.7
import QtGraphicalEffects 1.0

import Style 1.0
import Awesome 4.7

Item {
    height: childrenRect.height

    signal insertCell

    Rectangle {
        height: sheetTitleText.height
        anchors.left: parent.left
        anchors.right: parent.right

        color: Style.sheetTitle
        id: header

        Text {
            id: sheetTitleText

            text: sheetName ? instanceName : "Root"
            color: Style.textLightPrimary
            font.family: fixedWidth.name
            font.pointSize: 18
            leftPadding: 5
            topPadding: 3
            bottomPadding: 3
        }

        Row {
            anchors.right: header.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter

            Text {
                text: sheetName
                font.pointSize: 14
                color: Style.textLightSecondary
                rightPadding: 14
            }

            Row {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 5
                IconButton {
                    text: Awesome.fa_plus
                    mode: "light"
                    toolTip: "Add cell"
                    onClicked: { insertCell() }
                }
                IconButton {
                    mode: "light"
                    visible: sheetEnv.length > 1
                    text: Awesome.fa_window_close
                    onClicked: {
                        var env = sheetEnv.slice()
                        env.pop()
                        sheetStack.closeTo(env)
                    }
                    toolTip: "Collapse"
                }
            }
        }
    }

    DropShadow {
        anchors.fill: header
        horizontalOffset: 0
        verticalOffset: 3
        radius: 8.0
        color: "#80000000"
        source: header
        cached: true
    }
}

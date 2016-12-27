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

            text: sheetName ? (instanceName + " [" + sheetName + "]") : "Root"
            color: Style.textLightPrimary
            font.family: fixedWidth.name
            font.pointSize: 18
            leftPadding: 5
            topPadding: 3
            bottomPadding: 3
        }

        Row {
            anchors.right: header.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 10
            spacing: 5

            IconButton {
                text: Awesome.fa_plus_square
                mode: "light"
                toolTip: "Add cell"
                onClicked: { insertCell() }
            }
            IconButton {
                text: Awesome.fa_arrow_circle_up
                mode: "light"
                toolTip: "Show library"
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

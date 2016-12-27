import QtQuick 2.7
import QtGraphicalEffects 1.0

import Style 1.0

Item {
    height: childrenRect.height

    Rectangle {
        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right

        color: Style.sheetTitle
        id: header

        Text {
            text: sheetName ? (instanceName + " [" + sheetName + "]") : "Root"
            color: Style.textLightPrimary
            font.family: fixedWidth.name
            font.pointSize: 18
            leftPadding: 5
            topPadding: 3
            bottomPadding: 3
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

import QtQuick 2.7

import Style 1.0

Rectangle {
    height: childrenRect.height + 10
    color: Style.sheetTitle

    Text {
        text: sheetName ? (instanceName + " [" + sheetName + "]") : "Root"
        color: Style.textLightPrimary
        font.family: fixedWidth.name
        font.pointSize: 18
        leftPadding: 3
        topPadding: 3
    }
}

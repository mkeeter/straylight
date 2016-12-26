import QtQuick 2.7

import Colors 1.0

Text {
    text: sheetName ? (instanceName + " [" + sheetName + "]") : "Root"
    color: Colors.base1
    font.family: fixedWidth.name
    font.pointSize: 18
    leftPadding: 3
    topPadding: 3
}

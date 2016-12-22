import QtQuick 2.7

import Colors 1.0
import Awesome 4.7

Text {
    color: enabled ? (ma.containsMouse ? Colors.base2 : Colors.base1)
                   : Colors.base01
    text: Awesome.fa_question_circle
    padding: 0
    font.family: fontAwesome.name
    MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            if (parent.enabled) {
                parent.clicked()
            }
        }
    }
    property bool enabled: true

    signal clicked
}

import QtQuick 2.7
import QtQuick.Controls 2.0

import Style 1.0
import Awesome 4.7

Text {
    property string mode: "dark"
    color: enabled
        ? (ma.containsMouse
            ? (mode == "dark" ? Style.textDarkPrimary : Style.textLightPrimary)
            : (mode == "dark" ? Style.textDarkSecondary : Style.textLightSecondary))
        : mode == "dark" ? Style.textDarkHint : Style.textLightHint

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
    ToolTip {
        id: tt
        text: ""
        visible: ma.containsMouse && text
        delay: 1000
        timeout: 5000

        /*
        contentItem: Text {
            text: tt.text
            color: Colors.base01
        }
        */

       /*
        background: Rectangle {
            color: Colors.base2
        }
        */
    }
    property bool enabled: true
    property alias toolTip: tt.text

    signal clicked
}

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

        Row {
            spacing: 10
            Text {
                id: sheetTitleText

                text: sheetName ? instanceName : "Root"
                color: Style.textLightPrimary
                font.family: fixedWidth.name
                font.pointSize: 18
                leftPadding: 10
                topPadding: 3
                bottomPadding: 3
            }

            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                mode: "light"
                visible: sheetEnv.length > 1
                text: Awesome.fa_outdent
                onClicked: {
                    var env = sheetEnv.slice()
                    env.pop()
                    sheetStack.closeTo(env)
                }
                toolTip: "Collapse"
            }
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

            IconButton {
                anchors.verticalCenter: parent.verticalCenter
                text: Awesome.fa_plus
                mode: "light"
                toolTip: "Add cell"
                onClicked: { insertCell() }
            }
        }
    }

    Rectangle {
        anchors.top: header.bottom
        anchors.left: header.left
        anchors.right: header.right
        height: 10

        gradient: Gradient {
            GradientStop { position: 0.0; color: '#40000000'}
            GradientStop { position: 1.0; color: '#00000000'}
        }
    }
}

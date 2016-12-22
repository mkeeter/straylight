import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Bridge 1.0

ScrollView {
    property ListModel itemsModel

    Component.onCompleted: {
        Bridge.beginSheet.connect(beepboop)
    }

    function beepboop() { console.log("beep") }

    ListView {
        anchors.fill: parent
        model: itemsModel
        spacing: 0
        delegate: SheetItemDelegate {}
    }
    style: ScrollViewStyle {
        transientScrollBars: true
    }
}

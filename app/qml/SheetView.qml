import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

ScrollView {
    id: sheetView

    property ListModel sheetModel
    property var sheetIndex

    ListView {
        anchors.fill: parent
        model: sheetModel
        spacing: 8
        delegate: SheetItemDelegate {}
    }
    style: ScrollViewStyle {
        transientScrollBars: true
    }

}

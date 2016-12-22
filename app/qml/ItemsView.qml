import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

ScrollView {
    property ListModel itemsModel

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

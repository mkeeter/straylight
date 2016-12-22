import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

SplitView {
    orientation: Qt.Vertical

    ItemsView {
        id: itemsView
        Layout.fillHeight: true
    }

    LibraryView {
        id: libraryView
        width: parent.width
        Layout.minimumHeight: 100
    }

    property alias sheetIndex: itemsView.sheetIndex
    property alias viewEnv: itemsView.viewEnv

    property alias libraryModel: libraryView.libraryModel
}

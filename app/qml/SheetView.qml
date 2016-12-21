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
        Layout.minimumHeight: 100
    }

    property alias sheetModel: itemsView.itemsModel
    property alias libraryModel: libraryView.libraryModel
}

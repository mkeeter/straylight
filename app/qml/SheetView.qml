import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

SplitView {
    orientation: Qt.Vertical

    property int sheetIndex

    ItemsView {
        id: itemsView
        sheetIndex: sheetIndex
        Layout.fillHeight: true
    }

    LibraryView {
        id: libraryView
        width: parent.width
        Layout.minimumHeight: 100

        sheetIndex: sheetIndex
    }

    property alias viewEnv: itemsView.viewEnv

    property alias libraryModel: libraryView.libraryModel
}

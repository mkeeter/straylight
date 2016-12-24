import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

SplitView {
    orientation: Qt.Vertical

    property int sheetIndex
    property var sheetEnv

    SheetItemsPane {
        id: items
        Layout.fillHeight: true

        sheetIndex: sheetIndex
    }

    SheetLibraryPane {
        id: lib
        width: parent.width

        sheetIndex: sheetIndex
    }

    property alias libraryModel: lib.libraryModel
}

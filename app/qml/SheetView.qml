import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

SplitView {
    orientation: Qt.Vertical

    SheetItemsPane {
        id: items
        Layout.fillHeight: true
    }

    SheetLibraryPane {
        id: lib
        width: parent.width
    }

    property alias sheetEnv: items.sheetEnv
    property alias sheetIndex: lib.sheetIndex
}

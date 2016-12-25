import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Colors 1.0

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

    handleDelegate: splitViewHandle

    Component {
        id: splitViewHandle
        Rectangle {
            height: 3
            color: Colors.base00
        }
    }

    property alias sheetEnv: items.sheetEnv
    property alias sheetIndex: lib.sheetIndex
}

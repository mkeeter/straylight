import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import Colors 1.0

SplitView {
    orientation: Qt.Vertical

    ColumnLayout {
        SheetTitle { }
        SheetItemsPane {
            id: items
            width: parent.width
            Layout.fillHeight: true
        }
        Layout.fillHeight: true
    }

    SheetLibraryPane {
        id: lib
        width: parent.width
    }

    handleDelegate: Component {
        id: splitViewHandle
        Rectangle {
            height: 1
            color: Colors.base01
        }
    }

    property alias sheetEnv: items.sheetEnv
    property alias sheetIndex: lib.sheetIndex
}

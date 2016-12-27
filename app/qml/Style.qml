pragma Singleton
import QtQuick 2.0

import Material 1.0

Item {
readonly property color textDarkPrimary: '#de000000'
readonly property color textDarkSecondary: '#8a000000'
readonly property color textDarkHint: '#61000000'
readonly property color dividerDark: '#1e000000'

readonly property color textLightPrimary: '#ffffffff'
readonly property color textLightSecondary: '#b3ffffff'
readonly property color textLightHint: '#80ffffff'
readonly property color dividerLight: '#1effffff'

readonly property color primary: Material.blue_grey_500

readonly property color sheetTitle: primary
readonly property color textEditValid: Material.green_500
readonly property color textEditInvalid: Material.red_500
}

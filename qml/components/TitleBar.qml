import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

// Custom title bar for frameless window mode (future use)
Rectangle {
    id: root
    height: 32
    color: "#1a1a1a"

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 8
        spacing: 4

        Label {
            text: player.windowTitle
            font.pixelSize: 12
            elide: Text.ElideRight
            Layout.fillWidth: true
            opacity: 0.8
        }
    }
}

import QtQuick
import QtQuick.Controls.Material
import IINAWindows 1.0

Item {
    id: root

    // mpv rendering surface
    MpvObject {
        id: mpvObject
        anchors.fill: parent
    }

    // Idle state overlay (when no file is loaded)
    Rectangle {
        anchors.fill: parent
        color: "#1a1a1a"
        visible: mpv.idle && !mpv.playing

        Column {
            anchors.centerIn: parent
            spacing: 16

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "IINA"
                font.pixelSize: 48
                font.bold: true
                color: "#e0e0e0"
                opacity: 0.6
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Drop files here or use File > Open")
                font.pixelSize: 14
                color: "#808080"
            }
        }
    }
}

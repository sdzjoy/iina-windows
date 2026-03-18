import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Rectangle {
    id: controlBar
    height: 72
    color: "#cc1a1a1a"

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 2

        SeekBar {
            id: seekBar
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 4

            // Play/Pause
            ToolButton {
                icon.source: mpv.paused || !mpv.playing
                    ? "qrc:/resources/icons/play.svg"
                    : "qrc:/resources/icons/pause.svg"
                icon.width: 24
                icon.height: 24
                onClicked: mpv.togglePause()
                ToolTip.text: mpv.paused ? qsTr("Play") : qsTr("Pause")
                ToolTip.visible: hovered
            }

            // Previous
            ToolButton {
                icon.source: "qrc:/resources/icons/prev.svg"
                icon.width: 18
                icon.height: 18
                onClicked: mpv.playlistPrev()
                enabled: mpv.playlistCount > 1
                ToolTip.text: qsTr("Previous")
                ToolTip.visible: hovered
            }

            // Next
            ToolButton {
                icon.source: "qrc:/resources/icons/next.svg"
                icon.width: 18
                icon.height: 18
                onClicked: mpv.playlistNext()
                enabled: mpv.playlistCount > 1
                ToolTip.text: qsTr("Next")
                ToolTip.visible: hovered
            }

            // Time display
            Label {
                text: player.positionText + " / " + player.durationText
                font.pixelSize: 12
                font.family: "Consolas, monospace"
                color: "#cccccc"
                Layout.leftMargin: 8
            }

            // Chapter indicator (shown when chapters exist)
            Label {
                visible: chapterModel.count > 0
                text: qsTr("Ch.") + " " + (chapterModel.currentIndex + 1) + "/" + chapterModel.count
                font.pixelSize: 10
                color: "#888888"
                Layout.leftMargin: 4

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: player.toggleChapters()
                }
            }

            Item { Layout.fillWidth: true }

            // Speed indicator
            Label {
                visible: Math.abs(mpv.speed - 1.0) > 0.01
                text: mpv.speed.toFixed(2) + "x"
                font.pixelSize: 11
                color: Material.accent
            }

            VolumeControl {
                id: volumeControl
            }

            // Subtitle button
            ToolButton {
                icon.source: "qrc:/resources/icons/subtitle.svg"
                icon.width: 20
                icon.height: 20
                onClicked: subtitleMenu.open()
                ToolTip.text: qsTr("Subtitle")
                ToolTip.visible: hovered

                SubtitleMenu {
                    id: subtitleMenu
                    y: -implicitHeight
                }
            }

            // Screenshot button
            ToolButton {
                icon.source: "qrc:/resources/icons/screenshot.svg"
                icon.width: 18
                icon.height: 18
                onClicked: mpv.screenshot()
                ToolTip.text: qsTr("Screenshot (Ctrl+S)")
                ToolTip.visible: hovered
            }

            // Chapters button
            ToolButton {
                icon.source: "qrc:/resources/icons/chapters.svg"
                icon.width: 18
                icon.height: 18
                visible: chapterModel.count > 0
                highlighted: player.chaptersVisible
                onClicked: player.toggleChapters()
                ToolTip.text: qsTr("Chapters")
                ToolTip.visible: hovered
            }

            // Playlist button
            ToolButton {
                icon.source: "qrc:/resources/icons/playlist.svg"
                icon.width: 20
                icon.height: 20
                onClicked: player.togglePlaylist()
                highlighted: player.playlistVisible
                ToolTip.text: qsTr("Playlist")
                ToolTip.visible: hovered
            }

            // Fullscreen button
            ToolButton {
                icon.source: root.parent && root.parent.parent
                    ? (root.parent.parent.isFullscreen
                        ? "qrc:/resources/icons/fullscreen-exit.svg"
                        : "qrc:/resources/icons/fullscreen.svg")
                    : "qrc:/resources/icons/fullscreen.svg"
                icon.width: 20
                icon.height: 20
                onClicked: player.toggleFullscreen()
                ToolTip.text: qsTr("Fullscreen (F)")
                ToolTip.visible: hovered
            }
        }
    }
}

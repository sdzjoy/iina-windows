import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.platform as Platform
import IINAWindows 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 960
    height: 600
    minimumWidth: 480
    minimumHeight: 320
    title: player.windowTitle
    color: "#1a1a1a"

    Material.theme: Material.Dark
    Material.accent: Material.Teal
    Material.primary: "#1a1a1a"
    Material.background: "#1a1a1a"

    property bool isFullscreen: false
    visibility: isFullscreen ? Window.FullScreen : Window.Windowed

    Connections {
        target: player
        function onRequestFullscreen(fs) {
            root.isFullscreen = fs
        }
    }

    // ─── Keyboard Shortcuts (driven by ShortcutManager) ───
    Shortcut { sequence: shortcuts.keyForAction("play_pause"); onActivated: mpv.togglePause() }
    Shortcut { sequence: shortcuts.keyForAction("seek_forward_5"); onActivated: mpv.seek(5) }
    Shortcut { sequence: shortcuts.keyForAction("seek_backward_5"); onActivated: mpv.seek(-5) }
    Shortcut { sequence: shortcuts.keyForAction("seek_forward_30"); onActivated: mpv.seek(30) }
    Shortcut { sequence: shortcuts.keyForAction("seek_backward_30"); onActivated: mpv.seek(-30) }
    Shortcut { sequence: shortcuts.keyForAction("seek_forward_60"); onActivated: mpv.seek(60) }
    Shortcut { sequence: shortcuts.keyForAction("seek_backward_60"); onActivated: mpv.seek(-60) }
    Shortcut { sequence: shortcuts.keyForAction("volume_up"); onActivated: mpv.setVolume(mpv.volume + 5) }
    Shortcut { sequence: shortcuts.keyForAction("volume_down"); onActivated: mpv.setVolume(mpv.volume - 5) }
    Shortcut { sequence: shortcuts.keyForAction("fullscreen"); onActivated: player.toggleFullscreen() }
    Shortcut { sequence: shortcuts.keyForAction("exit_fullscreen"); onActivated: { if (isFullscreen) player.setFullscreen(false) } }
    Shortcut { sequence: shortcuts.keyForAction("mute_toggle"); onActivated: mpv.setMuted(!mpv.muted) }
    Shortcut { sequence: shortcuts.keyForAction("playlist_toggle"); onActivated: player.togglePlaylist() }
    Shortcut { sequence: shortcuts.keyForAction("frame_step"); onActivated: mpv.frameStep() }
    Shortcut { sequence: shortcuts.keyForAction("frame_back"); onActivated: mpv.frameBackStep() }
    Shortcut { sequence: shortcuts.keyForAction("speed_up"); onActivated: mpv.setSpeed(Math.min(mpv.speed + 0.25, 4.0)) }
    Shortcut { sequence: shortcuts.keyForAction("speed_down"); onActivated: mpv.setSpeed(Math.max(mpv.speed - 0.25, 0.25)) }
    Shortcut { sequence: shortcuts.keyForAction("speed_reset"); onActivated: mpv.setSpeed(1.0) }
    Shortcut { sequence: shortcuts.keyForAction("chapter_next"); onActivated: chapterModel.nextChapter() }
    Shortcut { sequence: shortcuts.keyForAction("chapter_prev"); onActivated: chapterModel.prevChapter() }
    Shortcut { sequence: shortcuts.keyForAction("open_file"); onActivated: fileDialog.open() }
    Shortcut { sequence: shortcuts.keyForAction("open_url"); onActivated: urlDialog.open() }
    Shortcut { sequence: shortcuts.keyForAction("quit"); onActivated: Qt.quit() }
    Shortcut { sequence: shortcuts.keyForAction("screenshot"); onActivated: { mpv.screenshot(); osd.show(qsTr("Screenshot saved")) } }
    Shortcut { sequence: shortcuts.keyForAction("sub_cycle"); onActivated: mpv.cycleSubtitleTrack() }
    Shortcut { sequence: shortcuts.keyForAction("sub_delay_inc"); onActivated: mpv.setSubtitleDelay(mpv.subtitleDelay + 0.5) }
    Shortcut { sequence: shortcuts.keyForAction("sub_delay_dec"); onActivated: mpv.setSubtitleDelay(mpv.subtitleDelay - 0.5) }
    Shortcut { sequence: shortcuts.keyForAction("sub_scale_up"); onActivated: mpv.setSubtitleScale(mpv.subtitleScale + 0.1) }
    Shortcut { sequence: shortcuts.keyForAction("sub_scale_down"); onActivated: mpv.setSubtitleScale(mpv.subtitleScale - 0.1) }
    Shortcut { sequence: shortcuts.keyForAction("settings"); onActivated: settingsPopup.open() }

    // ─── Menu Bar ───
    menuBar: MenuBar {
        visible: !isFullscreen
        Menu {
            title: qsTr("&File")
            Action { text: qsTr("&Open File..."); shortcut: "Ctrl+O"; onTriggered: fileDialog.open() }
            Action { text: qsTr("Open &URL..."); shortcut: "Ctrl+Shift+O"; onTriggered: urlDialog.open() }
            MenuSeparator {}
            Menu {
                id: recentMenu
                title: qsTr("Recent Files")
                enabled: historyModel.count > 0

                Instantiator {
                    model: historyModel.recentFiles(10)
                    delegate: MenuItem {
                        text: modelData.title
                        onTriggered: player.openFromHistory(modelData.path)
                    }
                    onObjectAdded: function(index, object) { recentMenu.insertItem(index, object) }
                    onObjectRemoved: function(index, object) { recentMenu.removeItem(object) }
                }

                MenuSeparator {}
                Action {
                    text: qsTr("Clear Recent Files")
                    onTriggered: historyModel.clearAll()
                }
            }
            MenuSeparator {}
            Action { text: qsTr("&Quit"); shortcut: "Ctrl+Q"; onTriggered: Qt.quit() }
        }
        Menu {
            title: qsTr("&Playback")
            Action { text: mpv.paused ? qsTr("&Play") : qsTr("&Pause"); onTriggered: mpv.togglePause() }
            Action { text: qsTr("&Stop"); onTriggered: mpv.stop() }
            MenuSeparator {}
            Action { text: qsTr("&Next"); onTriggered: mpv.playlistNext() }
            Action { text: qsTr("P&revious"); onTriggered: mpv.playlistPrev() }
            MenuSeparator {}
            Menu {
                title: qsTr("Chapters")
                enabled: chapterModel.count > 0
                Action { text: qsTr("Next Chapter"); onTriggered: chapterModel.nextChapter() }
                Action { text: qsTr("Previous Chapter"); onTriggered: chapterModel.prevChapter() }
                MenuSeparator {}

                Instantiator {
                    model: chapterModel
                    delegate: MenuItem {
                        text: model.title + " (" + model.timeText + ")"
                        checkable: true
                        checked: model.isCurrent
                        onTriggered: chapterModel.seekToChapter(model.chapterIndex)
                    }
                    onObjectAdded: function(index, object) {
                        recentMenu.parent.insertItem(index + 3, object)
                    }
                    onObjectRemoved: function(index, object) { recentMenu.parent.removeItem(object) }
                }
            }
            MenuSeparator {}
            Menu {
                title: qsTr("S&peed")
                Action { text: "0.5x"; onTriggered: mpv.setSpeed(0.5) }
                Action { text: "0.75x"; onTriggered: mpv.setSpeed(0.75) }
                Action { text: "1.0x"; onTriggered: mpv.setSpeed(1.0) }
                Action { text: "1.25x"; onTriggered: mpv.setSpeed(1.25) }
                Action { text: "1.5x"; onTriggered: mpv.setSpeed(1.5) }
                Action { text: "2.0x"; onTriggered: mpv.setSpeed(2.0) }
            }
        }
        Menu {
            title: qsTr("&Video")
            Action { text: qsTr("&Fullscreen"); shortcut: "F"; onTriggered: player.toggleFullscreen() }
            Action {
                text: qsTr("&Screenshot")
                shortcut: "Ctrl+S"
                onTriggered: { mpv.screenshot(); osd.show(qsTr("Screenshot saved")) }
            }
            MenuSeparator {}
            Action { text: qsTr("Video &Filters..."); onTriggered: filterDialog.open() }
        }
        Menu {
            title: qsTr("&Audio")
            Repeater {
                model: audioModel
                MenuItem {
                    text: model.description
                    checkable: true
                    checked: model.isSelected
                    onTriggered: mpv.setAudioTrack(model.trackId)
                }
            }
        }
        Menu {
            title: qsTr("&Subtitle")
            Action { text: qsTr("&Load Subtitle File..."); onTriggered: subtitleDialog.open() }
            Action { text: qsTr("Cycle Track"); onTriggered: mpv.cycleSubtitleTrack() }
            MenuSeparator {}
            MenuItem {
                text: qsTr("None")
                checkable: true
                checked: mpv.subtitleTrack === 0
                onTriggered: mpv.setSubtitleTrack(0)
            }
            Repeater {
                model: subtitleModel
                MenuItem {
                    text: model.description
                    checkable: true
                    checked: model.isSelected
                    onTriggered: mpv.setSubtitleTrack(model.trackId)
                }
            }
        }
        Menu {
            title: qsTr("&Window")
            Action { text: qsTr("&Playlist"); shortcut: "P"; onTriggered: player.togglePlaylist() }
            Action {
                text: qsTr("&Chapters")
                enabled: chapterModel.count > 0
                onTriggered: player.toggleChapters()
            }
            Action { text: qsTr("&History"); onTriggered: player.toggleHistory() }
            MenuSeparator {}
            Action { text: qsTr("S&ettings"); shortcut: "Ctrl+,"; onTriggered: settingsPopup.open() }
        }
        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About IINA")
                onTriggered: aboutDialog.open()
            }
        }
    }

    // ─── File Dialogs ───
    FileDialog {
        id: fileDialog
        title: qsTr("Open Media File")
        nameFilters: [
            qsTr("Video files") + " (*.mp4 *.mkv *.avi *.webm *.mov *.flv *.wmv *.m4v *.mpg *.mpeg *.ts *.vob *.3gp)",
            qsTr("Audio files") + " (*.mp3 *.flac *.ogg *.wav *.aac *.m4a *.wma *.opus)",
            qsTr("All files") + " (*)"
        ]
        fileMode: FileDialog.OpenFiles
        onAccepted: player.openFiles(selectedFiles)
    }

    FileDialog {
        id: subtitleDialog
        title: qsTr("Load Subtitle File")
        nameFilters: [
            qsTr("Subtitle files") + " (*.srt *.ass *.ssa *.sub *.vtt *.lrc *.idx *.sup *.smi)",
            qsTr("All files") + " (*)"
        ]
        onAccepted: player.openSubtitleFile(selectedFile)
    }

    // ─── URL Dialog ───
    Dialog {
        id: urlDialog
        title: qsTr("Open URL")
        anchors.centerIn: parent
        width: 500
        standardButtons: Dialog.Ok | Dialog.Cancel
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            Label { text: qsTr("Enter URL:") }
            TextField {
                id: urlField
                Layout.fillWidth: true
                placeholderText: "https://..."
                selectByMouse: true
                onAccepted: urlDialog.accept()
            }
        }
        onAccepted: {
            if (urlField.text.length > 0) {
                player.openUrl(urlField.text)
                urlField.clear()
            }
        }
        onOpened: { urlField.forceActiveFocus(); urlField.selectAll() }
    }

    // ─── About Dialog ───
    Dialog {
        id: aboutDialog
        title: qsTr("About IINA")
        anchors.centerIn: parent
        width: 360
        standardButtons: Dialog.Ok
        modal: true
        ColumnLayout {
            spacing: 12
            Label {
                text: qsTr("IINA for Windows")
                font.pixelSize: 20
                font.bold: true
            }
            Label { text: qsTr("Version") + " " + app.version }
            Label {
                text: qsTr("The modern video player for Windows.")
                wrapMode: Text.WordWrap
            }
            Label {
                text: qsTr("Based on iina/iina (macOS). Licensed under GPL-3.0.")
                wrapMode: Text.WordWrap
                opacity: 0.7
            }
            Label {
                text: qsTr("Powered by mpv and Qt")
                opacity: 0.5
            }
        }
    }

    // ─── Right-click Context Menu ───
    Menu {
        id: contextMenu
        Action { text: mpv.paused ? qsTr("Play") : qsTr("Pause"); onTriggered: mpv.togglePause() }
        Action { text: qsTr("Stop"); onTriggered: mpv.stop() }
        MenuSeparator {}
        Action { text: qsTr("Fullscreen"); onTriggered: player.toggleFullscreen() }
        Action { text: qsTr("Screenshot"); onTriggered: { mpv.screenshot(); osd.show(qsTr("Screenshot saved")) } }
        MenuSeparator {}
        Menu {
            title: qsTr("Speed")
            Action { text: "0.5x"; onTriggered: mpv.setSpeed(0.5) }
            Action { text: "1.0x"; onTriggered: mpv.setSpeed(1.0) }
            Action { text: "1.5x"; onTriggered: mpv.setSpeed(1.5) }
            Action { text: "2.0x"; onTriggered: mpv.setSpeed(2.0) }
        }
        MenuSeparator {}
        Action { text: qsTr("Open File..."); onTriggered: fileDialog.open() }
        Action { text: qsTr("Settings"); onTriggered: settingsPopup.open() }
    }

    // ─── Main Layout ───
    RowLayout {
        anchors.fill: parent
        spacing: 0

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Video + mouse interactions
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                VideoArea {
                    id: videoArea
                    anchors.fill: parent
                }

                OSD {
                    id: osd
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.topMargin: 40
                }

                MouseArea {
                    id: videoMouseArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    hoverEnabled: true

                    onClicked: function(mouse) {
                        if (mouse.button === Qt.RightButton) {
                            contextMenu.popup()
                        }
                    }

                    onDoubleClicked: function(mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            player.toggleFullscreen()
                        }
                    }

                    onWheel: function(wheel) {
                        if (wheel.modifiers & Qt.ShiftModifier) {
                            var seekAmt = wheel.angleDelta.y > 0 ? 5 : -5
                            mpv.seek(seekAmt)
                        } else {
                            var volDelta = wheel.angleDelta.y > 0 ? 5 : -5
                            mpv.setVolume(mpv.volume + volDelta)
                        }
                    }

                    onPositionChanged: {
                        controlBar.visible = true
                        hideTimer.restart()
                    }
                }
            }

            ControlBar {
                id: controlBar
                Layout.fillWidth: true
            }
        }

        // Side panels
        PlaylistPanel {
            id: playlistPanel
            Layout.preferredWidth: 280
            Layout.fillHeight: true
            visible: player.playlistVisible
        }

        ChapterPanel {
            id: chapterPanel
            Layout.preferredWidth: 280
            Layout.fillHeight: true
            visible: player.chaptersVisible
        }

        HistoryPanel {
            id: historyPanel
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            visible: player.historyVisible
        }
    }

    // ─── Drag and Drop ───
    DropArea {
        anchors.fill: parent
        keys: ["text/uri-list"]

        onEntered: function(drag) { dropOverlay.visible = true }
        onExited: { dropOverlay.visible = false }
        onDropped: function(drop) {
            dropOverlay.visible = false
            if (drop.hasUrls) player.dropFiles(drop.urls)
        }
    }

    Rectangle {
        id: dropOverlay
        anchors.fill: parent
        visible: false
        color: "#80000000"
        z: 50

        Rectangle {
            anchors.centerIn: parent
            width: 200
            height: 100
            radius: 12
            color: "#cc1a1a1a"
            border.color: Material.accent
            border.width: 2

            Label {
                anchors.centerIn: parent
                text: qsTr("Drop files to open")
                font.pixelSize: 14
            }
        }
    }

    // ─── Popup Dialogs ───
    SettingsPage { id: settingsPopup }
    FilterPanel { id: filterDialog }
    ShortcutSettings { id: shortcutSettingsDialog }

    // ─── Auto-hide in fullscreen ───
    Timer {
        id: hideTimer
        interval: 3000
        onTriggered: {
            if (isFullscreen && mpv.playing && !mpv.paused) {
                controlBar.visible = false
            }
        }
    }

    // ─── OSD feedback connections ───
    Connections {
        target: mpv
        function onScreenshotTaken(path) {
            osd.show(qsTr("Screenshot saved"))
        }
        function onSubtitleDelayChanged() {
            osd.show(qsTr("Subtitle delay: ") + mpv.subtitleDelay.toFixed(1) + "s")
        }
    }
}

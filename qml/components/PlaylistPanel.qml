import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
    id: root
    color: "#222222"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: 8
            spacing: 8

            Label {
                text: qsTr("Playlist")
                font.pixelSize: 14
                font.bold: true
                Layout.fillWidth: true
            }

            Label {
                text: mpv.playlistCount + " " + qsTr("items")
                font.pixelSize: 11
                opacity: 0.6
            }

            ToolButton {
                text: "+"
                font.pixelSize: 16
                onClicked: addFileDialog.open()
                ToolTip.text: qsTr("Add files")
                ToolTip.visible: hovered
            }

            ToolButton {
                text: "\u2715"
                font.pixelSize: 14
                onClicked: mpv.playlistClear()
                enabled: mpv.playlistCount > 0
                ToolTip.text: qsTr("Clear playlist")
                ToolTip.visible: hovered
            }
        }

        // Divider
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#333333"
        }

        // Playlist items
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: playlistModel
            clip: true
            currentIndex: playlistModel.currentIndex

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

            delegate: ItemDelegate {
                width: listView.width
                height: 42
                highlighted: model.isCurrent

                contentItem: RowLayout {
                    spacing: 8

                    // Playing indicator
                    Label {
                        text: model.isCurrent ? "\u25B6" : ""
                        font.pixelSize: 10
                        color: Material.accent
                        Layout.preferredWidth: 16
                    }

                    // Title
                    Label {
                        text: model.title
                        elide: Text.ElideMiddle
                        Layout.fillWidth: true
                        font.pixelSize: 12
                        opacity: model.isCurrent ? 1.0 : 0.8
                    }

                    // Remove button
                    ToolButton {
                        text: "\u2715"
                        font.pixelSize: 10
                        opacity: 0.5
                        visible: hoverHandler.hovered
                        onClicked: mpv.playlistRemove(model.itemIndex)

                        HoverHandler { id: hoverHandler }
                    }
                }

                onClicked: mpv.playlistPlayIndex(model.itemIndex)
                onDoubleClicked: mpv.playlistPlayIndex(model.itemIndex)
            }

            // Empty state
            Label {
                anchors.centerIn: parent
                visible: listView.count === 0
                text: qsTr("Playlist is empty")
                opacity: 0.4
            }
        }
    }

    // Separator line on left edge
    Rectangle {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: "#333333"
    }

    FileDialog {
        id: addFileDialog
        title: qsTr("Add to Playlist")
        nameFilters: [
            qsTr("Media files") + " (*.mp4 *.mkv *.avi *.webm *.mov *.flv *.wmv *.mp3 *.flac *.ogg *.wav)",
            qsTr("All files") + " (*)"
        ]
        fileMode: FileDialog.OpenFiles
        onAccepted: {
            for (var i = 0; i < selectedFiles.length; i++) {
                var path = selectedFiles[i].toString().replace("file:///", "")
                mpv.playlistAppend(path)
            }
        }
    }
}

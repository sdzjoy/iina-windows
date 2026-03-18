import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

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
                text: qsTr("History")
                font.pixelSize: 14
                font.bold: true
                Layout.fillWidth: true
            }

            Label {
                text: historyModel.count + " " + qsTr("items")
                font.pixelSize: 11
                opacity: 0.6
            }

            ToolButton {
                text: "\u2715"
                font.pixelSize: 14
                onClicked: clearConfirmDialog.open()
                enabled: historyModel.count > 0
                ToolTip.text: qsTr("Clear history")
                ToolTip.visible: hovered
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#333333"
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: historyModel
            clip: true

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

            delegate: ItemDelegate {
                width: listView.width
                height: 56

                contentItem: ColumnLayout {
                    spacing: 2

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Label {
                            text: model.title
                            elide: Text.ElideMiddle
                            Layout.fillWidth: true
                            font.pixelSize: 12
                        }

                        // Remove button
                        ToolButton {
                            text: "\u2715"
                            font.pixelSize: 10
                            opacity: 0.5
                            visible: itemHover.hovered
                            onClicked: historyModel.removeEntry(model.index)
                            HoverHandler { id: itemHover }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        // Progress bar
                        Rectangle {
                            Layout.preferredWidth: 60
                            height: 3
                            radius: 1.5
                            color: "#404040"

                            Rectangle {
                                width: parent.width * (model.progress || 0)
                                height: parent.height
                                radius: parent.radius
                                color: Material.accent
                            }
                        }

                        Label {
                            text: model.lastPlayedText
                            font.pixelSize: 10
                            opacity: 0.5
                            Layout.fillWidth: true
                        }
                    }
                }

                onClicked: player.openFromHistory(model.path)
            }

            // Empty state
            Label {
                anchors.centerIn: parent
                visible: listView.count === 0
                text: qsTr("No playback history")
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

    Dialog {
        id: clearConfirmDialog
        title: qsTr("Clear History")
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Yes | Dialog.No

        Label {
            text: qsTr("Clear all playback history?")
        }

        onAccepted: historyModel.clearAll()
    }
}

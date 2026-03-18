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
                text: qsTr("Chapters")
                font.pixelSize: 14
                font.bold: true
                Layout.fillWidth: true
            }

            Label {
                text: chapterModel.count + " " + qsTr("chapters")
                font.pixelSize: 11
                opacity: 0.6
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
            model: chapterModel
            clip: true
            currentIndex: chapterModel.currentIndex

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

            delegate: ItemDelegate {
                width: listView.width
                height: 48
                highlighted: model.isCurrent

                contentItem: RowLayout {
                    spacing: 8

                    // Chapter number
                    Label {
                        text: (model.chapterIndex + 1).toString()
                        font.pixelSize: 11
                        color: model.isCurrent ? Material.accent : "#888888"
                        Layout.preferredWidth: 24
                        horizontalAlignment: Text.AlignRight
                    }

                    // Chapter info
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: model.title
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                            font.pixelSize: 12
                            font.bold: model.isCurrent
                        }

                        Label {
                            text: model.timeText
                            font.pixelSize: 10
                            font.family: "Consolas, monospace"
                            opacity: 0.5
                        }
                    }

                    // Playing indicator
                    Label {
                        text: model.isCurrent ? "\u25B6" : ""
                        font.pixelSize: 10
                        color: Material.accent
                    }
                }

                onClicked: chapterModel.seekToChapter(model.chapterIndex)
            }

            // Empty state
            Label {
                anchors.centerIn: parent
                visible: listView.count === 0
                text: qsTr("No chapters")
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
}

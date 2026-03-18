import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs

Dialog {
    id: root
    title: qsTr("Settings")
    anchors.centerIn: parent
    width: 560
    height: 500
    modal: true
    standardButtons: Dialog.Close

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton { text: qsTr("General") }
            TabButton { text: qsTr("Video") }
            TabButton { text: qsTr("Audio") }
            TabButton { text: qsTr("Subtitle") }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // General tab
            ScrollView {
                ColumnLayout {
                    width: parent.width
                    spacing: 16
                    padding: 16

                    GroupBox {
                        title: qsTr("Behavior")
                        Layout.fillWidth: true
                        ColumnLayout {
                            CheckBox {
                                text: qsTr("Resume playback position")
                                checked: player.resumePlayback
                                onToggled: player.setResumePlayback(checked)
                            }
                            CheckBox {
                                text: qsTr("Pause on minimize")
                            }
                        }
                    }

                    GroupBox {
                        title: qsTr("Interface")
                        Layout.fillWidth: true
                        ColumnLayout {
                            CheckBox {
                                text: qsTr("Dark theme")
                                checked: true
                                enabled: false
                            }
                        }
                    }

                    GroupBox {
                        title: qsTr("Keyboard Shortcuts")
                        Layout.fillWidth: true
                        ColumnLayout {
                            Button {
                                text: qsTr("Configure Shortcuts...")
                                onClicked: shortcutSettingsDialog.open()
                                flat: true
                            }
                        }
                    }
                }
            }

            // Video tab
            ScrollView {
                ColumnLayout {
                    width: parent.width
                    spacing: 16
                    padding: 16

                    GroupBox {
                        title: qsTr("Hardware Decoding")
                        Layout.fillWidth: true
                        ColumnLayout {
                            ComboBox {
                                id: hwdecCombo
                                Layout.fillWidth: true
                                model: ["auto-safe", "auto", "d3d11va", "dxva2", "no"]
                                currentIndex: 0
                                onActivated: mpv.setProperty("hwdec", currentText)
                            }
                            Label {
                                text: qsTr("'auto-safe' is recommended for most systems")
                                font.pixelSize: 11
                                opacity: 0.6
                                wrapMode: Text.WordWrap
                            }
                        }
                    }

                    GroupBox {
                        title: qsTr("Screenshot")
                        Layout.fillWidth: true
                        ColumnLayout {
                            spacing: 8
                            RowLayout {
                                Label { text: qsTr("Format:"); Layout.preferredWidth: 80 }
                                ComboBox {
                                    id: ssFormatCombo
                                    Layout.fillWidth: true
                                    model: ["png", "jpg", "webp"]
                                    currentIndex: model.indexOf(mpv.screenshotFormat)
                                    onActivated: mpv.setScreenshotFormat(currentText)
                                }
                            }
                            RowLayout {
                                Label { text: qsTr("Directory:"); Layout.preferredWidth: 80 }
                                Label {
                                    text: mpv.screenshotDirectory || qsTr("(Pictures folder)")
                                    font.pixelSize: 11
                                    elide: Text.ElideMiddle
                                    Layout.fillWidth: true
                                    opacity: 0.7
                                }
                                Button {
                                    text: qsTr("Browse...")
                                    flat: true
                                    onClicked: ssDirDialog.open()
                                }
                            }
                        }
                    }

                    GroupBox {
                        title: qsTr("Video Filters")
                        Layout.fillWidth: true
                        ColumnLayout {
                            Button {
                                text: qsTr("Open Video Filters...")
                                onClicked: filterDialog.open()
                                flat: true
                            }
                        }
                    }
                }
            }

            // Audio tab
            ScrollView {
                ColumnLayout {
                    width: parent.width
                    spacing: 16
                    padding: 16

                    GroupBox {
                        title: qsTr("Volume")
                        Layout.fillWidth: true
                        ColumnLayout {
                            RowLayout {
                                Label { text: qsTr("Default volume:") }
                                SpinBox {
                                    from: 0
                                    to: 150
                                    value: 100
                                    stepSize: 5
                                }
                                Label { text: "%" }
                            }
                        }
                    }
                }
            }

            // Subtitle tab
            ScrollView {
                ColumnLayout {
                    width: parent.width
                    spacing: 16
                    padding: 16

                    GroupBox {
                        title: qsTr("Subtitle Appearance")
                        Layout.fillWidth: true
                        ColumnLayout {
                            spacing: 8

                            RowLayout {
                                Label { text: qsTr("Font size:"); Layout.preferredWidth: 100 }
                                SpinBox {
                                    id: subFontSize
                                    from: 16
                                    to: 72
                                    value: 36
                                    stepSize: 2
                                    onValueModified: mpv.setProperty("sub-font-size", value)
                                }
                            }

                            RowLayout {
                                Label { text: qsTr("Font:"); Layout.preferredWidth: 100 }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: [qsTr("Default"), "Arial", "Segoe UI", "Consolas", "Noto Sans"]
                                    onActivated: {
                                        if (currentIndex === 0) mpv.setSubtitleFont("")
                                        else mpv.setSubtitleFont(currentText)
                                    }
                                }
                            }

                            RowLayout {
                                Label { text: qsTr("Color:"); Layout.preferredWidth: 100 }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: [
                                        qsTr("White (Default)"),
                                        qsTr("Yellow"),
                                        qsTr("Cyan"),
                                        qsTr("Green")
                                    ]
                                    onActivated: {
                                        var colors = ["#FFFFFFFF", "#FFFFFF00", "#FF00FFFF", "#FF00FF00"]
                                        mpv.setSubtitleColor(colors[currentIndex])
                                    }
                                }
                            }

                            RowLayout {
                                Label { text: qsTr("Border:"); Layout.preferredWidth: 100 }
                                Slider {
                                    Layout.fillWidth: true
                                    from: 0
                                    to: 5
                                    value: 2
                                    stepSize: 0.5
                                    onMoved: mpv.setSubtitleBorderSize(value)
                                }
                            }

                            RowLayout {
                                Label { text: qsTr("Position:"); Layout.preferredWidth: 100 }
                                Slider {
                                    Layout.fillWidth: true
                                    from: 0
                                    to: 100
                                    value: 100
                                    stepSize: 5
                                    onMoved: mpv.setSubtitlePosition(Math.round(value))
                                }
                                Label {
                                    text: qsTr("Bottom")
                                    font.pixelSize: 10
                                    opacity: 0.5
                                }
                            }

                            RowLayout {
                                Label { text: qsTr("Delay:"); Layout.preferredWidth: 100 }
                                SpinBox {
                                    from: -100
                                    to: 100
                                    value: Math.round(mpv.subtitleDelay * 10)
                                    stepSize: 5
                                    onValueModified: mpv.setSubtitleDelay(value / 10.0)

                                    textFromValue: function(value) {
                                        return (value / 10.0).toFixed(1) + "s"
                                    }
                                    valueFromText: function(text) {
                                        return Math.round(parseFloat(text) * 10)
                                    }
                                }
                            }
                        }
                    }

                    GroupBox {
                        title: qsTr("Auto-loading")
                        Layout.fillWidth: true
                        ColumnLayout {
                            CheckBox {
                                text: qsTr("Auto-load subtitles from same directory")
                                checked: true
                            }
                            Label {
                                text: qsTr("Matches subtitle files with the same base name as the media file")
                                font.pixelSize: 11
                                opacity: 0.5
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }
    }

    FolderDialog {
        id: ssDirDialog
        title: qsTr("Select Screenshot Directory")
        onAccepted: {
            var path = selectedFolder.toString().replace("file:///", "")
            mpv.setScreenshotDirectory(path)
        }
    }
}

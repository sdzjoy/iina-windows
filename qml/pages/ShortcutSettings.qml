import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Dialog {
    id: root
    title: qsTr("Keyboard Shortcuts")
    anchors.centerIn: parent
    width: 560
    height: 500
    modal: true
    standardButtons: Dialog.Close

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        // Category tabs
        TabBar {
            id: categoryBar
            Layout.fillWidth: true

            Repeater {
                model: shortcuts.categories()
                TabButton {
                    text: modelData
                    width: implicitWidth
                }
            }
        }

        // Shortcut list
        ListView {
            id: shortcutList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: {
                var cats = shortcuts.categories()
                if (categoryBar.currentIndex >= 0 && categoryBar.currentIndex < cats.length)
                    return shortcuts.shortcutsByCategory(cats[categoryBar.currentIndex])
                return []
            }

            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

            delegate: ItemDelegate {
                width: shortcutList.width
                height: 44

                contentItem: RowLayout {
                    spacing: 12

                    Label {
                        text: modelData.label
                        font.pixelSize: 12
                        Layout.fillWidth: true
                    }

                    // Key display / edit
                    Rectangle {
                        Layout.preferredWidth: 140
                        height: 28
                        radius: 4
                        color: keyEdit.activeFocus ? "#333333" : "#2a2a2a"
                        border.color: keyEdit.activeFocus ? Material.accent
                            : modelData.isModified ? "#555555" : "#3a3a3a"
                        border.width: 1

                        Label {
                            anchors.centerIn: parent
                            text: modelData.currentKey || qsTr("(none)")
                            font.pixelSize: 11
                            font.family: "Consolas, monospace"
                            opacity: modelData.currentKey ? 1.0 : 0.4
                            visible: !keyEdit.activeFocus
                        }

                        TextInput {
                            id: keyEdit
                            anchors.centerIn: parent
                            font.pixelSize: 11
                            font.family: "Consolas, monospace"
                            color: "white"
                            visible: activeFocus
                            text: qsTr("Press key...")
                            readOnly: true

                            Keys.onPressed: function(event) {
                                var parts = []
                                if (event.modifiers & Qt.ControlModifier) parts.push("Ctrl")
                                if (event.modifiers & Qt.ShiftModifier) parts.push("Shift")
                                if (event.modifiers & Qt.AltModifier) parts.push("Alt")

                                // Only accept non-modifier keys
                                var keyName = ""
                                switch (event.key) {
                                case Qt.Key_Control: case Qt.Key_Shift:
                                case Qt.Key_Alt: case Qt.Key_Meta:
                                    return // Don't accept lone modifiers
                                case Qt.Key_Escape:
                                    keyEdit.focus = false
                                    return
                                case Qt.Key_Space: keyName = "Space"; break
                                case Qt.Key_Return: keyName = "Return"; break
                                case Qt.Key_Backspace: keyName = "Backspace"; break
                                case Qt.Key_Delete: keyName = "Delete"; break
                                case Qt.Key_Left: keyName = "Left"; break
                                case Qt.Key_Right: keyName = "Right"; break
                                case Qt.Key_Up: keyName = "Up"; break
                                case Qt.Key_Down: keyName = "Down"; break
                                default:
                                    keyName = String.fromCharCode(event.key)
                                }

                                if (keyName) {
                                    parts.push(keyName)
                                    var seq = parts.join("+")
                                    shortcuts.setKey(modelData.action, seq)
                                    keyEdit.focus = false
                                }
                                event.accepted = true
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: keyEdit.forceActiveFocus()
                        }
                    }

                    // Reset button
                    ToolButton {
                        text: "\u21BA"
                        font.pixelSize: 14
                        visible: modelData.isModified
                        onClicked: shortcuts.resetToDefault(modelData.action)
                        ToolTip.text: qsTr("Reset to default")
                        ToolTip.visible: hovered
                    }
                }
            }
        }

        // Footer
        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            Button {
                text: qsTr("Reset All to Defaults")
                flat: true
                onClicked: shortcuts.resetAllToDefaults()
            }
        }
    }
}

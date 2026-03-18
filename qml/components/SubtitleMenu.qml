import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Menu {
    id: root
    title: qsTr("Subtitle")

    MenuItem {
        text: qsTr("None")
        checkable: true
        checked: mpv.subtitleTrack === 0
        onTriggered: mpv.setSubtitleTrack(0)
    }

    MenuSeparator {
        visible: subtitleRepeater.count > 0
    }

    Repeater {
        id: subtitleRepeater
        model: subtitleModel
        MenuItem {
            text: model.description
            checkable: true
            checked: model.isSelected
            onTriggered: mpv.setSubtitleTrack(model.trackId)
        }
    }

    MenuSeparator {}

    MenuItem {
        text: qsTr("Load Subtitle File...")
        onTriggered: {
            // Access the subtitleDialog defined in Main.qml
            root.parent.parent.parent.parent.subtitleDialog.open()
        }
    }
}

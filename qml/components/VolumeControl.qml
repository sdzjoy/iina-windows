import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

RowLayout {
    spacing: 2

    ToolButton {
        icon.source: mpv.muted || mpv.volume <= 0
            ? "qrc:/resources/icons/volume-mute.svg"
            : "qrc:/resources/icons/volume.svg"
        icon.width: 20
        icon.height: 20
        onClicked: mpv.setMuted(!mpv.muted)
        ToolTip.text: mpv.muted ? qsTr("Unmute") : qsTr("Mute (M)")
        ToolTip.visible: hovered
    }

    Slider {
        id: volumeSlider
        Layout.preferredWidth: 90
        from: 0
        to: 150
        value: mpv.volume
        stepSize: 1

        onMoved: mpv.setVolume(value)

        ToolTip {
            parent: volumeSlider.handle
            visible: volumeSlider.pressed
            text: Math.round(volumeSlider.value) + "%"
        }

        // Mouse wheel support
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            onWheel: function(wheel) {
                var delta = wheel.angleDelta.y > 0 ? 5 : -5
                mpv.setVolume(mpv.volume + delta)
            }
        }
    }
}

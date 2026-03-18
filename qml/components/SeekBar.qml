import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Item {
    id: root
    height: 24

    property real value: mpv.duration > 0 ? mpv.position / mpv.duration : 0
    property bool seeking: false

    // Background track
    Rectangle {
        id: track
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: mouseArea.containsMouse || seeking ? 6 : 3
        radius: height / 2
        color: "#404040"

        Behavior on height { NumberAnimation { duration: 100 } }

        // Progress fill
        Rectangle {
            width: parent.width * root.value
            height: parent.height
            radius: parent.radius
            color: Material.accent

            Behavior on width {
                enabled: !seeking
                NumberAnimation { duration: 50 }
            }
        }

        // Seek handle
        Rectangle {
            x: track.width * root.value - width / 2
            anchors.verticalCenter: parent.verticalCenter
            width: mouseArea.containsMouse || seeking ? 14 : 0
            height: width
            radius: width / 2
            color: Material.accent
            visible: mouseArea.containsMouse || seeking

            Behavior on width { NumberAnimation { duration: 100 } }
        }
    }

    // Hover time tooltip
    ToolTip {
        id: hoverTooltip
        visible: mouseArea.containsMouse && !seeking && mpv.duration > 0
        delay: 0
        timeout: -1
        x: mouseArea.mouseX - width / 2
        y: -height - 4

        contentItem: Label {
            text: {
                if (mpv.duration <= 0) return ""
                var pos = (mouseArea.mouseX / track.width) * mpv.duration
                return formatTime(Math.max(0, Math.min(pos, mpv.duration)))
            }
            font.pixelSize: 11
            font.family: "Consolas, monospace"
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        anchors.topMargin: -4
        anchors.bottomMargin: -4
        hoverEnabled: true
        preventStealing: true

        onPressed: function(mouse) {
            if (mpv.duration <= 0) return
            seeking = true
            var percent = mouse.x / track.width * 100
            mpv.seekPercent(Math.max(0, Math.min(percent, 100)))
        }

        onPositionChanged: function(mouse) {
            if (seeking && mpv.duration > 0) {
                var percent = mouse.x / track.width * 100
                mpv.seekPercent(Math.max(0, Math.min(percent, 100)))
            }
        }

        onReleased: {
            seeking = false
        }
    }

    function formatTime(seconds) {
        if (isNaN(seconds) || seconds < 0) seconds = 0
        var totalSecs = Math.floor(seconds)
        var hours = Math.floor(totalSecs / 3600)
        var mins = Math.floor((totalSecs % 3600) / 60)
        var secs = totalSecs % 60
        if (hours > 0)
            return hours + ":" + String(mins).padStart(2, '0') + ":" + String(secs).padStart(2, '0')
        else
            return String(mins).padStart(2, '0') + ":" + String(secs).padStart(2, '0')
    }
}

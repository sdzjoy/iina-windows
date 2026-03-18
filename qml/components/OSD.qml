import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Item {
    id: root
    width: osdLabel.implicitWidth + 32
    height: osdLabel.implicitHeight + 16
    visible: false
    z: 100

    function show(text) {
        osdLabel.text = text
        root.visible = true
        hideTimer.restart()
    }

    // Listen for volume/speed changes to show OSD
    Connections {
        target: mpv
        function onVolumeChanged() {
            if (mpv.muted)
                root.show(qsTr("Volume: Muted"))
            else
                root.show(qsTr("Volume: ") + Math.round(mpv.volume) + "%")
        }
        function onSpeedChanged() {
            root.show(qsTr("Speed: ") + mpv.speed.toFixed(2) + "x")
        }
    }

    Timer {
        id: hideTimer
        interval: 1500
        onTriggered: root.visible = false
    }

    Rectangle {
        anchors.fill: parent
        radius: 6
        color: "#cc000000"
    }

    Label {
        id: osdLabel
        anchors.centerIn: parent
        font.pixelSize: 14
        color: "white"
    }

    // Fade animation
    Behavior on visible {
        NumberAnimation {
            target: root
            property: "opacity"
            from: root.visible ? 1 : 0
            to: root.visible ? 0 : 1
            duration: 200
        }
    }
}

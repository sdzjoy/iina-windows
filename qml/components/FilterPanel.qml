import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Dialog {
    id: root
    title: qsTr("Video & Audio Filters")
    anchors.centerIn: parent
    width: 400
    height: 480
    modal: true
    standardButtons: Dialog.Close

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        GroupBox {
            title: qsTr("Video Adjustments")
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 8

                FilterSlider {
                    label: qsTr("Brightness")
                    value: mpv.brightness
                    onValueChanged: mpv.setBrightness(value)
                }

                FilterSlider {
                    label: qsTr("Contrast")
                    value: mpv.contrast
                    onValueChanged: mpv.setContrast(value)
                }

                FilterSlider {
                    label: qsTr("Saturation")
                    value: mpv.saturation
                    onValueChanged: mpv.setSaturation(value)
                }

                FilterSlider {
                    label: qsTr("Gamma")
                    value: mpv.gamma
                    onValueChanged: mpv.setGamma(value)
                }

                FilterSlider {
                    label: qsTr("Hue")
                    value: mpv.hue
                    onValueChanged: mpv.setHue(value)
                }
            }
        }

        Button {
            text: qsTr("Reset All")
            Layout.alignment: Qt.AlignHCenter
            onClicked: mpv.resetFilters()
            highlighted: true
            flat: true
        }
    }

    // Reusable filter slider component
    component FilterSlider: RowLayout {
        property string label: ""
        property alias value: slider.value

        spacing: 8

        Label {
            text: label
            font.pixelSize: 12
            Layout.preferredWidth: 80
        }

        Slider {
            id: slider
            Layout.fillWidth: true
            from: -100
            to: 100
            stepSize: 1
            value: 0

            onMoved: parent.valueChanged(value)
        }

        Label {
            text: Math.round(slider.value).toString()
            font.pixelSize: 11
            font.family: "Consolas, monospace"
            Layout.preferredWidth: 32
            horizontalAlignment: Text.AlignRight
        }

        signal valueChanged(real value)
    }
}

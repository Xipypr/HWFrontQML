import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import ".."

Dialog {
    id: root

    signal setDeviceNameSelected()
    signal changeLayoutSelected()

    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width - 32, 360)
    modal: true
    focus: true
    padding: 20
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    contentItem: ColumnLayout {
        spacing: 10

        Label {
            Layout.fillWidth: true
            text: qsTr("Device settings")
            color: Theme.textPrimary
            font.pixelSize: 18
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Set device name")
            onClicked: {
                root.setDeviceNameSelected()
                root.close()
            }
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Change widget layout")
            onClicked: {
                root.changeLayoutSelected()
                root.close()
            }
        }
    }
}

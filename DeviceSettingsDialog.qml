import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

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

    background: Rectangle {
        radius: 14
        color: "#111827"
        border.color: "#334155"
        border.width: 1
    }

    contentItem: ColumnLayout {
        spacing: 10

        Label {
            Layout.fillWidth: true
            text: "Настройки устройства"
            color: "#E2E8F0"
            font.pixelSize: 18
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            Layout.fillWidth: true
            text: "Задать имя устройства"
            onClicked: {
                root.setDeviceNameSelected()
                root.close()
            }
        }

        Button {
            Layout.fillWidth: true
            text: "Изменить компоновку элементов"
            onClicked: {
                root.changeLayoutSelected()
                root.close()
            }
        }
    }
}

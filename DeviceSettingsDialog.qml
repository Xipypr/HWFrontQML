import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min((parent ? parent.width : 460) - 32, 460)
    modal: true
    title: "Настройки устройства"
    standardButtons: Dialog.Close
    padding: 16

    signal setAliasRequested()
    signal editLayoutRequested()

    contentItem: ColumnLayout {
        spacing: 12

        Button {
            Layout.fillWidth: true
            text: "Задать имя устройства"
            onClicked: root.setAliasRequested()
        }

        Button {
            Layout.fillWidth: true
            text: "Изменить компоновку виджетов"
            onClicked: root.editLayoutRequested()
        }
    }
}

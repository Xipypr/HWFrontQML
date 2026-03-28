import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min((parent ? parent.width : 460) - 32, 460)
    modal: true
    title: "Псевдоним устройства"
    standardButtons: Dialog.Ok | Dialog.Cancel
    padding: 16

    property string aliasValue: ""

    signal aliasSaved(string aliasValue)

    onOpened: {
        aliasField.text = aliasValue
        aliasField.selectAll()
        aliasField.forceActiveFocus()
    }

    onAccepted: {
        root.aliasSaved(aliasField.text.trim())
    }

    contentItem: ColumnLayout {
        spacing: 8

        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            color: "#CBD5E1"
            text: "Псевдоним будет отображаться вместо исходного имени устройства."
        }

        TextField {
            id: aliasField
            Layout.fillWidth: true
            placeholderText: "Например: Мой игровой ПК"
        }
    }
}

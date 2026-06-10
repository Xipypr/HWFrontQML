import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    property string initialAlias: ""
    signal aliasSubmitted(string alias)

    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width - 32, 360)
    modal: true
    focus: true
    padding: 20
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    title: qsTr("Device alias")

    onOpened: {
        aliasField.text = initialAlias
        aliasField.forceActiveFocus()
        aliasField.selectAll()
    }

    contentItem: ColumnLayout {
        spacing: 10

        TextField {
            id: aliasField
            Layout.fillWidth: true
            placeholderText: qsTr("Example: Work PC")
            maximumLength: 64
            onAccepted: saveButton.clicked()
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: 8

            Button {
                text: qsTr("Cancel")
                onClicked: root.close()
            }

            Button {
                id: saveButton
                text: qsTr("Save")
                enabled: aliasField.text.trim().length > 0
                onClicked: {
                    root.aliasSubmitted(aliasField.text.trim())
                    root.close()
                }
            }
        }
    }
}

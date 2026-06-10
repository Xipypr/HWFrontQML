import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root
    modal: true
    focus: true
    title: qsTr("Settings")
    standardButtons: Dialog.Close
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    x: Math.max(0, (parent ? parent.width - width : 0) / 2)
    y: Math.max(0, (parent ? parent.height - height : 0) / 2)
    width: Math.min(parent ? parent.width - 32 : 420, 420)

    contentItem: ColumnLayout {
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                Layout.fillWidth: true
                text: qsTr("Language")
                wrapMode: Text.WordWrap
            }

            ComboBox {
                id: languageComboBox
                Layout.preferredWidth: 150
                model: languageManager.availableLanguages
                textRole: "label"

                function updateCurrentIndex() {
                    for (let i = 0; i < model.length; ++i) {
                        if (model[i].code === languageManager.currentLanguage) {
                            currentIndex = i
                            return
                        }
                    }
                    currentIndex = 0
                }

                Component.onCompleted: updateCurrentIndex()
                onActivated: function(index) {
                    const selectedLanguage = model[index]
                    if (selectedLanguage)
                        languageManager.currentLanguage = selectedLanguage.code
                }

                Connections {
                    target: languageManager
                    function onCurrentLanguageChanged() {
                        languageComboBox.updateCurrentIndex()
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                Layout.fillWidth: true
                text: qsTr("Save sessions and widget layout")
                wrapMode: Text.WordWrap
            }

            Switch {
                id: persistSessionStateSwitch
                checked: sessionManager.persistSessionState
                onToggled: sessionManager.persistSessionState = checked
            }
        }

        Label {
            Layout.fillWidth: true
            text: persistSessionStateSwitch.checked
                  ? qsTr("The next launch will restore saved sessions and widget layout.")
                  : qsTr("Saved sessions are cleared. The next launch will start fresh.")
            wrapMode: Text.WordWrap
            font.pixelSize: 12
            opacity: 0.75
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                Layout.fillWidth: true
                text: qsTr("Keep screen awake")
                wrapMode: Text.WordWrap
            }

            Switch {
                id: keepScreenAwakeCheckBox
                checked: sessionManager.keepScreenAwake
                onToggled: sessionManager.keepScreenAwake = checked
            }
        }

        Label {
            Layout.fillWidth: true
            text: keepScreenAwakeCheckBox.checked
                  ? qsTr("The screen will stay on while the app is running.")
                  : qsTr("The screen will turn off according to system settings.")
            wrapMode: Text.WordWrap
            font.pixelSize: 12
            opacity: 0.75
        }
    }
}

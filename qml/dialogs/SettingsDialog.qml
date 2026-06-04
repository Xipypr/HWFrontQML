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
                text: qsTr("Сохранять сессии и layout виджетов")
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
                  ? qsTr("При следующем запуске приложение восстановит сохранённые сессии и расположение виджетов.")
                  : qsTr("Сохранённые сессии очищены. Следующий запуск начнётся с чистого листа.")
            wrapMode: Text.WordWrap
            font.pixelSize: 12
            opacity: 0.75
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                Layout.fillWidth: true
                text: qsTr("Не гасить экран")
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
                  ? qsTr("Экран будет оставаться включённым, пока приложение запущено.")
                  : qsTr("Экран будет выключаться по системным настройкам.")
            wrapMode: Text.WordWrap
            font.pixelSize: 12
            opacity: 0.75
        }
    }
}

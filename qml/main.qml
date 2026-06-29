import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import "."
import "pages"
import "dialogs"

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hardware Monitor")
    id: root
    color: Theme.background
    Material.theme: Theme.isLight ? Material.Light : Material.Dark
    Material.accent: Theme.accent
    Material.primary: Theme.headerBackground
    Material.background: Theme.background
    Material.foreground: Theme.textPrimary

    function goToStartPage() {
        swipeView.currentIndex = 0
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent

        PageAuthForm {
            id: pageAuth
            onSettingsRequested: settingsDialog.open()
            onSessionSelected: (sessionId) => swipeToSessionPage(sessionId)
        }

        Repeater {
            model: sessionManager.connectedSessionsModel

            delegate: PageDevicesInfo {
                id: devicePage
                sessionId: model.sessionId
                destop_name: model.displayName
                deviceAlias: model.alias
                sessionState: model.state
                onHomeRequested: root.goToStartPage()
            }
        }
    }

    SettingsDialog {
        id: settingsDialog
        parent: Overlay.overlay
    }

    function swipeToSessionPage(sessionId) {
        if (!sessionId || sessionId.length === 0)
            return

        const sessionIndex = sessionManager.indexOfConnectedSession(sessionId)
        if (sessionIndex < 0)
            return

        swipeView.currentIndex = sessionIndex + 1
    }
}

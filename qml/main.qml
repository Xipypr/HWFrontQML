import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Window 2.12
import "."
import "pages"
import "dialogs"
import "controls"

ApplicationWindow {
    id: root

    readonly property bool customTitleBar: Qt.platform.os !== "android"
                                           && Qt.platform.os !== "ios"

    function windowFlags() {
        if (customTitleBar)
            return Qt.Window | Qt.FramelessWindowHint

        return Qt.Window
    }

    function materialTheme() {
        if (Theme.isLight)
            return Material.Light

        return Material.Dark
    }

    function contentTopMargin() {
        if (customTitleBar)
            return titleBar.height

        return 0
    }

    width: 640
    height: 480
    minimumWidth: 420
    minimumHeight: 320
    visible: true
    title: qsTr("Hardware Monitor")
    flags: root.windowFlags()
    color: Theme.background
    Material.theme: root.materialTheme()
    Material.accent: Theme.accent
    Material.primary: Theme.headerBackground
    Material.background: Theme.background
    Material.foreground: Theme.textPrimary

    function goToStartPage() {
        swipeView.currentIndex = 0
    }

    WindowTitleBar {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        window: root
        visible: root.customTitleBar
        z: 10
    }

    WindowResizeFrame {
        anchors.fill: parent
        window: root
        enabled: root.customTitleBar
        z: 100
    }

    SwipeView {
        id: swipeView
        anchors.top: parent.top
        anchors.topMargin: root.contentTopMargin()
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        PageAuthForm {
            id: pageAuth
            onSettingsRequested: settingsDialog.open()
            onSessionSelected: (sessionId) => root.swipeToSessionPage(sessionId)
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

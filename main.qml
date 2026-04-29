import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hardware Monitor")
    id: root

    property var deviceAliasBySession: ({})

    function aliasForSession(sessionId) {
        if (!sessionId || sessionId.length === 0)
            return ""

        return deviceAliasBySession[sessionId] || ""
    }

    function setAliasForSession(sessionId, alias) {
        if (!sessionId || sessionId.length === 0)
            return

        const mapCopy = Object.assign({}, deviceAliasBySession)
        if (alias && alias.length > 0)
            mapCopy[sessionId] = alias
        else
            delete mapCopy[sessionId]

        deviceAliasBySession = mapCopy
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent

        PageAuthForm {
            id: pageAuth
            aliasForSessionCallback: root.aliasForSession
            onSessionSelected: (sessionId) => swipeToSessionPage(sessionId)
            onDeviceAliasChangedByUser: (sessionId, alias) => setAliasForSession(sessionId, alias)
        }

        Repeater {
            model: sessionManager.connectedSessionIds

            delegate: PageDevicesInfo {
                sessionId: modelData
                deviceAlias: root.aliasForSession(modelData)
                onDeviceAliasChangedByUser: (sessionId, alias) => root.setAliasForSession(sessionId, alias)
            }
        }
    }

    function swipeToSessionPage(sessionId) {
        if (!sessionId || sessionId.length === 0)
            return

        const sessionIndex = sessionManager.connectedSessionIds.indexOf(sessionId)
        if (sessionIndex < 0)
            return

        swipeView.currentIndex = sessionIndex + 1
    }
}

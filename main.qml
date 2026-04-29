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

    SwipeView {
        id: swipeView
        anchors.fill: parent

        PageAuthForm {
            id: pageAuth
            onSessionSelected: (sessionId) => swipeToSessionPage(sessionId)
        }

        Repeater {
            model: sessionManager.connectedSessionIds

            delegate: PageDevicesInfo {
                sessionId: modelData
                deviceAlias: sessionManager.aliasForSession(modelData)
                onDeviceAliasChangedByUser: (sessionId, alias) => sessionManager.setSessionAlias(sessionId, alias)
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

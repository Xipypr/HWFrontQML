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

    function goToStartPage() {
        swipeView.currentIndex = 0
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent

        PageAuthForm {
            id: pageAuth
            onSessionSelected: (sessionId) => swipeToSessionPage(sessionId)
        }

        Repeater {
            model: sessionManager.connectedSessionsModel

            delegate: PageDevicesInfo {
                id: devicePage
                sessionId: model.sessionId
                deviceAlias: model.alias
                sessionState: model.state
                onHomeRequested: root.goToStartPage()
            }
        }
    }

    function swipeToSessionPage(sessionId) {
        if (!sessionId || sessionId.length === 0)
            return

        for (let i = 0; i < sessionManager.connectedSessionsModel.rowCount(); ++i) {
            const row = sessionManager.connectedSessionsModel.get(i)
            if (row.sessionId === sessionId) {
                swipeView.currentIndex = i + 1
                return
            }
        }
    }
}

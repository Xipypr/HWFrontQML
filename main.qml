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

    Component {
        id: devicePageComponent
        PageDevicesInfo {
            onHomeRequested: root.goToStartPage()
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent

        PageAuthForm {
            id: pageAuth
            onSessionSelected: (sessionId) => swipeToSessionPage(sessionId)
        }

        Repeater {
            model: sessionManager.sessionsModel

            delegate: Loader {
                active: model.hasDevice
                sourceComponent: devicePageComponent

                property string delegateSessionId: model.sessionId
                property string delegateAlias: model.alias
                property int delegateState: model.state

                onLoaded: {
                    if (!item)
                        return
                    item.sessionId = delegateSessionId
                    item.deviceAlias = delegateAlias
                    item.sessionState = delegateState
                }
            }
        }
    }

    function swipeToSessionPage(sessionId) {
        if (!sessionId || sessionId.length === 0)
            return

        let connectedIndex = 0
        for (let i = 0; i < sessionManager.sessionsModel.rowCount(); ++i) {
            const row = sessionManager.sessionsModel.get(i)
            if (!row.hasDevice)
                continue
            if (row.sessionId === sessionId) {
                swipeView.currentIndex = connectedIndex + 1
                return
            }
            connectedIndex += 1
        }
    }
}

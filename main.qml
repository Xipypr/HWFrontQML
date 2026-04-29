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

    ListModel {
        id: devicePagesModel
    }

    function indexOfSession(sessionId) {
        for (let i = 0; i < devicePagesModel.count; ++i) {
            if (devicePagesModel.get(i).sessionId === sessionId)
                return i
        }
        return -1
    }

    function syncDevicePages() {
        if (!sessionManager || !sessionManager.sessionIds)
            return

        const ids = sessionManager.sessionIds

        for (let i = devicePagesModel.count - 1; i >= 0; --i) {
            const modelSessionId = devicePagesModel.get(i).sessionId
            if (ids.indexOf(modelSessionId) === -1 || !sessionManager.hasDeviceForSession(modelSessionId))
                devicePagesModel.remove(i)
        }

        for (let j = 0; j < ids.length; ++j) {
            const sessionId = ids[j]
            if (sessionManager.hasDeviceForSession(sessionId) && indexOfSession(sessionId) === -1)
                devicePagesModel.append({ sessionId: sessionId })
        }
    }

    Connections {
        target: sessionManager

        function onSessionIdsChanged() {
            root.syncDevicePages()
        }

        function onDeviceReady(sessionId, deviceRef) {
            if (root.indexOfSession(sessionId) === -1)
                devicePagesModel.append({ sessionId: sessionId })
        }

        function onSessionRemoved(sessionId) {
            const index = root.indexOfSession(sessionId)
            if (index >= 0)
                devicePagesModel.remove(index)

            if (swipeView.currentIndex > devicePagesModel.count)
                swipeView.currentIndex = 0
        }
    }

    Component.onCompleted: syncDevicePages()

    SwipeView {
        id: swipeView
        anchors.fill: parent

        PageAuthForm {
            id: pageAuth
        }

        Repeater {
            model: devicePagesModel

            delegate: PageDevicesInfo {
                sessionId: model.sessionId
            }
        }
    }
}

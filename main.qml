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
    property bool devicePageVisible: false
    property bool allowDevicePageActivation: false
    property string activeSessionId: ""

    Loader {
            id: pagesLoader

            anchors.fill: parent
            asynchronous: true
        }

    Connections{
        target: core

        function onDeviceReady(sessionId, deviceRef) {
            if (allowDevicePageActivation && activeSessionId === sessionId) {
                devicePageVisible = true
            }
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: 0
        interactive: devicePageVisible

        PageAuthForm {
            id: pageAuth

            onConnectedDeviceDeleted: {
                devicePageVisible = false
            }

            onConnectionStateChanged: (allowDevicePageActivationValue) => {
                allowDevicePageActivation = allowDevicePageActivationValue
                if (!allowDevicePageActivation) {
                    devicePageVisible = false
                }
            }

            onSessionSelected: (sessionId) => {
                activeSessionId = sessionId
            }
        }

        Loader {
            id: pageDeviceInfoLoader
            active: devicePageVisible
            source: "PageDevicesInfo.qml"
            visible: devicePageVisible
            onLoaded: {
                if (item)
                    item.sessionId = root.activeSessionId
            }
        }
    }

    onActiveSessionIdChanged: {
        if (pageDeviceInfoLoader.item) {
            pageDeviceInfoLoader.item.sessionId = activeSessionId
        }
    }

    onDevicePageVisibleChanged: {
        if (devicePageVisible && swipeView.currentIndex === 0) {
            swipeView.currentIndex = 1
        } else if (!devicePageVisible && swipeView.currentIndex > 0) {
            swipeView.currentIndex = 0
        }
    }
}

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import SessionData 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hardware Monitor")
    id: root
    property bool devicePageVisible: false
    property bool allowDevicePageActivation: false

    Loader {
            id: pagesLoader

            anchors.fill: parent
            asynchronous: true
//            source: "Background_1.qml"

//            onStatusChanged: console.log("status", status,  "item", item)
//            onLoaded: item.color = "green"
        }

    Connections{
        target: core

        function onSessionStateChanged(state) {
            if (state === SessionState.connected && allowDevicePageActivation) {
                devicePageVisible = true
            }

            if (state !== SessionState.connected) {
                devicePageVisible = false
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
        }

        Loader {
            id: pageDeviceInfoLoader
            active: devicePageVisible
            source: "PageDevicesInfo.qml"
            visible: devicePageVisible
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

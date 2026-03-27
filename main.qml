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

    Component {
        id: authPageComponent

        PageAuthForm {
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
    }

    Component {
        id: devicePageComponent

        PageDevicesInfo {
        }
    }

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

        function onDeviceCreated() {
            if (allowDevicePageActivation) {
                devicePageVisible = true
            }
        }
    }

    header: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Page 1")
        }
        TabButton {
            visible: devicePageVisible
            text: qsTr(core.device().name)
        }
    }


    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: true

        Repeater {
            model: devicePageVisible ? 2 : 1

            Loader {
                sourceComponent: index === 0 ? authPageComponent : devicePageComponent
            }
        }
    }

    onDevicePageVisibleChanged: {
        if (!devicePageVisible && swipeView.currentIndex > 0) {
            swipeView.currentIndex = 0
        }
    }
}

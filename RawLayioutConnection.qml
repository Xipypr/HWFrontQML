import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0
import SessionData 1.0

Item {
    id: root

    property int horizontalMargin: 10
    property bool compactMode: width < 560
    property bool awaitingDeviceCreation: false

    readonly property var currentSession: core.session
    readonly property int currentSessionState: core.sessionState
    readonly property bool isConnected: currentSessionState === SessionState.connected
    readonly property bool isConnecting: currentSessionState === SessionState.connecting

    implicitHeight: contentLayout.implicitHeight + 20

    signal removeThisObject(bool removeConnectedDevicePage)
    signal connectionStateChanged(bool allowDevicePageActivation)

    ColumnLayout {
        id: contentLayout
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: root.horizontalMargin
        anchors.rightMargin: root.horizontalMargin
        spacing: 8

        Label {
            text: "Введите IP-адрес"
            Layout.fillWidth: root.compactMode
            Layout.preferredWidth: controlsLayout.implicitWidth
            Layout.alignment: root.compactMode ? Qt.AlignLeft : Qt.AlignHCenter
            horizontalAlignment: Text.AlignLeft
        }

        GridLayout {
            id: controlsLayout
            columns: root.compactMode ? 1 : 4
            columnSpacing: 8
            rowSpacing: 8
            Layout.fillWidth: root.compactMode
            Layout.alignment: root.compactMode ? Qt.AlignLeft : Qt.AlignHCenter

            ConnectedDeviceCard {
                id: hostInfo
                Layout.fillWidth: root.compactMode
                Layout.preferredWidth: 240
                Layout.maximumWidth: root.compactMode ? Number.POSITIVE_INFINITY : 240
                connected: root.isConnected
                deviceName: root.currentSession.displayName
            }

            Button{
                id: connectButton
                text: root.isConnecting ? "Stop" : (root.isConnected ? "Reconnect" : "Connect Device")
                Layout.fillWidth: root.compactMode
                Layout.preferredWidth: Math.max(connectTextMetrics.width, stopTextMetrics.width) + leftPadding + rightPadding
                enabled: hostInfo.acceptableInput || root.isConnecting
                onClicked: clickConnectButton()

                function clickConnectButton(){
                    if (root.isConnecting)
                    {
                        stopSendingRequests()
                    }
                    else
                    {
                        sendRequest()
                    }
                }

                function sendRequest(){
                    awaitingDeviceCreation = true
                    root.connectionStateChanged(true)
                    core.onMakeGetRequest(hostInfo.inputText)
                }

                function stopSendingRequests(){
                    awaitingDeviceCreation = false
                    root.connectionStateChanged(false)
                    core.disconnectSession()
                }

            }

            TextMetrics {
                id: connectTextMetrics
                font: connectButton.font
                text: "Connect Device"
            }

            TextMetrics {
                id: stopTextMetrics
                font: connectButton.font
                text: "Stop connecting"
            }

            LinearBusyIndicator {
                id: connectingIndicator
                running: root.isConnecting
                Layout.fillWidth: root.compactMode
                Layout.preferredWidth: root.compactMode ? 0 : implicitWidth
                Layout.preferredHeight: implicitHeight
            }

            Button{
                id: deleteDevice
                text: "Delete Device"
                Layout.fillWidth: root.compactMode
                onClicked: {
                    const removeConnectedDevicePage = root.isConnected
                    awaitingDeviceCreation = false
                    core.disconnectSession()
                    root.connectionStateChanged(false)
                    root.removeThisObject(removeConnectedDevicePage)
                }
            }
        }

        Connections{
            target: core

            function onSessionStateChanged(state) {
                if (state !== SessionState.connected)
                    return

                if (!awaitingDeviceCreation)
                    return

                awaitingDeviceCreation = false
            }
        }
    }
}

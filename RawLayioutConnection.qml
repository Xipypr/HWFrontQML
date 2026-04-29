import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
    id: root

    property int connectionInitialized: 0
    property int horizontalMargin: 10
    property bool compactMode: width < 560
    property string connectedDeviceName: ""
    property string deviceAlias: ""
    property bool awaitingDeviceCreation: false
    property string sessionId: ""

    implicitHeight: contentLayout.implicitHeight + 20

    signal removeThisObject(bool removeConnectedDevicePage)
    signal connectionStateChanged(bool allowDevicePageActivation)
    signal sessionSelected(string sessionId)

    function hasValidSessionId(sessionId) {
        return !!sessionId && sessionId.length > 0
    }

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
            visible: root.connectionInitialized !== 1
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
                connected: root.connectionInitialized === 1
                deviceName: root.deviceAlias.length > 0 ? root.deviceAlias : root.connectedDeviceName
                onDeviceLabelClicked: {
                    if (root.hasValidSessionId(root.sessionId))
                        root.sessionSelected(root.sessionId)
                }
                onDeviceLabelRightClicked: aliasInputDialog.open()
            }

            Button{
                id: connectButton
                text: "Connect Device"
                Layout.fillWidth: root.compactMode
                Layout.preferredWidth: Math.max(connectTextMetrics.width, stopTextMetrics.width) + leftPadding + rightPadding
                enabled: hostInfo.acceptableInput || connectingIndicator.running
                onClicked: clickConnectButton()

                function clickConnectButton(){
                    if (connectingIndicator.running)
                    {
                        stopSendingRequests()
                    }
                    else
                    {
                        sendRequest()
                    }
                }

                function sendRequest(){
                    if (!root.hasValidSessionId(root.sessionId)) {
                        root.sessionId = sessionManager.createSession(hostInfo.inputText)
                    }
                    awaitingDeviceCreation = true
                    root.connectionStateChanged(true)
                    if (root.hasValidSessionId(root.sessionId)) {
                        const sessionCore = sessionManager.coreForSession(root.sessionId)
                        if (sessionCore)
                            sessionCore.onMakeGetRequest(hostInfo.inputText)
                    }
                    connectButton.text = "Stop"
                    connectingIndicator.running = true
                }

                function stopSendingRequests(){
                    awaitingDeviceCreation = false
                    root.connectionStateChanged(false)
                    connectButton.text = "Connect Device"
                    connectingIndicator.running = false
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
                running: false
                Layout.fillWidth: root.compactMode
                Layout.preferredWidth: root.compactMode ? 0 : implicitWidth
                Layout.preferredHeight: implicitHeight
            }

            Button{
                id: deleteDevice
                text: "Delete Device"
                Layout.fillWidth: root.compactMode
                onClicked: {
                    const removeConnectedDevicePage = connectionInitialized === 1
                    connectionInitialized = 0
                    awaitingDeviceCreation = false
                    root.connectedDeviceName = ""
                    root.deviceAlias = ""
                    if (root.hasValidSessionId(root.sessionId))
                        sessionManager.removeSession(root.sessionId)
                    root.sessionId = ""
                    root.connectionStateChanged(false)
                    root.removeThisObject(removeConnectedDevicePage)
                }
            }
        }



    AliasInputDialog {
        id: aliasInputDialog
        initialAlias: root.deviceAlias
        onAliasSubmitted: function(alias) {
            root.deviceAlias = alias
        }
    }

        Connections{
            target: sessionManager

            function onDeviceReady(sessionId, deviceRef) {
                if (!awaitingDeviceCreation || root.sessionId !== sessionId)
                    return

                awaitingDeviceCreation = false
                connectingIndicator.running = false
                connectButton.text = "Reconnect"
                connectionInitialized = 1
                root.connectedDeviceName = deviceRef.name
                if (sessionManager.sessionsModel.rowCount() === 1)
                    root.sessionSelected(sessionId)
            }
        }
    }
}

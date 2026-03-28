import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.1

Item {
    id: root

    property int connectionInitialized: 0
    property int horizontalMargin: 10
    property bool compactMode: width < 560
    property string connectedDeviceName: ""
    property bool awaitingDeviceCreation: false

    implicitHeight: contentLayout.implicitHeight + 20

    signal removeThisObject(bool removeConnectedDevicePage)
    signal connectionStateChanged(bool allowDevicePageActivation)

    Settings {
        id: aliasSettings
        category: "DeviceAliases"
        fileName: "device_aliases.ini"
        property string aliasesJson: "{}"
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
                deviceName: root.connectedDeviceName
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
                    awaitingDeviceCreation = true
                    root.connectionStateChanged(true)
                    core.onMakeGetRequest(hostInfo.inputText)
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
                    root.connectionStateChanged(false)
                    root.removeThisObject(removeConnectedDevicePage)
                }
            }
        }

        Connections{
            target: core

            function onDeviceCreated() {
                if (!awaitingDeviceCreation)
                    return

                awaitingDeviceCreation = false
                connectingIndicator.running = false
                connectButton.text = "Reconnect"
                connectionInitialized = 1
                root.connectedDeviceName = resolveDeviceAlias(core.device().name)
            }
        }
    }




    Connections {
        target: aliasSettings

        function onAliasesJsonChanged() {
            if (connectionInitialized === 1)
                root.connectedDeviceName = resolveDeviceAlias(core.device().name)
        }
    }
function parseAliases() {
        try {
            return JSON.parse(aliasSettings.aliasesJson)
        } catch (err) {
            return {}
        }
    }

    function resolveDeviceAlias(deviceName) {
        if (!deviceName || deviceName.length === 0)
            return ""

        let aliases = parseAliases()
        return aliases[deviceName] || deviceName
    }

}

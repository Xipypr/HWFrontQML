import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
    id: root

    property int connectionInitialized: 0
    property int horizontalMargin: 12
    property real wideControlsWidth: textField.Layout.preferredWidth
                                     + connectButton.Layout.preferredWidth
                                     + connectingIndicator.implicitWidth
                                     + deleteDevice.implicitWidth
                                     + controlsLayout.columnSpacing * 3
    property bool compactMode: contentLayout.width < wideControlsWidth

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
            Layout.preferredWidth: textField.Layout.preferredWidth
            Layout.alignment: root.compactMode ? Qt.AlignLeft : Qt.AlignHCenter
        }

        GridLayout {
            id: controlsLayout
            columns: root.compactMode ? 1 : 4
            columnSpacing: 8
            rowSpacing: 8
            Layout.fillWidth: root.compactMode
            Layout.alignment: root.compactMode ? Qt.AlignLeft : Qt.AlignHCenter

            TextField {
                id: textField
                focus: true
                Layout.fillWidth: root.compactMode
                Layout.preferredWidth: 240
                Layout.maximumWidth: root.compactMode ? Number.POSITIVE_INFINITY : 240
                verticalAlignment: TextInput.AlignVCenter
                validator: RegularExpressionValidator {
                    regularExpression: /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                }
                Component.onCompleted: {
                    textField.forceActiveFocus()
                }
            }

            Button{
                id: connectButton
                text: "Connect Device"
                Layout.fillWidth: root.compactMode
                Layout.preferredWidth: Math.max(connectTextMetrics.width, stopTextMetrics.width) + leftPadding + rightPadding
                enabled: textField.acceptableInput || connectingIndicator.running
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
                    root.connectionStateChanged(true)
                    core.onMakeGetRequest(textField.text)
                    connectButton.text = "Stop"
                    connectingIndicator.running = true
                }

                function stopSendingRequests(){
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
                    root.connectionStateChanged(false)
                    root.removeThisObject(removeConnectedDevicePage)
                }
            }
        }

        Connections{
            target: core

            function onDeviceCreated() {
                connectingIndicator.running = false
                connectButton.text = "Reconnect"
                connectionInitialized = 1
            }
        }
    }
}

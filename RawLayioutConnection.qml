import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
    id: root

    property int connectionInitialized: 0

    signal removeThisObject(bool removeConnectedDevicePage)
    signal connectionStateChanged(bool allowDevicePageActivation)

    RowLayout {
        id: layout
        anchors.margins: 10
        anchors.fill: parent

        Label {
            text: "Введите IP-адрес"
        }

        TextField {
            id: textField
            focus: true
            text: "192.168.1.68"
//            placeholderText: qsTr("192.168.1.1")
//            inputMask: "000.000.000.000"
//            validator: RegularExpressionValidator {
//                regularExpression:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
//            }
            Component.onCompleted: {
                    textField.forceActiveFocus()
                }
        }

        Button{
            id: connectButton
            text: "Connect Device"
            Layout.preferredWidth: Math.max(connectTextMetrics.width, stopTextMetrics.width) + leftPadding + rightPadding
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
            Layout.preferredWidth: implicitWidth
            Layout.preferredHeight: implicitHeight
        }

        Button{
            id: deleteDevice
            text: "Delete Device"
            onClicked: {
                const removeConnectedDevicePage = connectionInitialized === 1
                connectionInitialized = 0
                root.connectionStateChanged(false)
                root.removeThisObject(removeConnectedDevicePage)
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

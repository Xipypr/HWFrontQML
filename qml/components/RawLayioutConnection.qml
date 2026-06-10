import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0
import SessionState 1.0
import "../controls"
import "../dialogs"

Item {
    id: root

    property bool connectionInitialized: false
    property int horizontalMargin: 10
    property bool compactMode: width < 560
    readonly property int buttonHorizontalBreathingRoom: 12
    property string connectedDeviceName: ""
    property string deviceAlias: ""
    property string target: ""

    property bool awaitingDeviceCreation: false
    property string sessionId: ""
    property int sessionState: SessionState.IDLE

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
            visible: !root.connectionInitialized
            text: qsTr("Enter IP address")
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
                connected: root.connectionInitialized
                deviceName: root.deviceAlias.length > 0 ? root.deviceAlias : root.connectedDeviceName
                sessionState: root.sessionState
                inputText: root.target
                onDeviceLabelClicked: {
                    if (root.hasValidSessionId(root.sessionId))
                        root.sessionSelected(root.sessionId)
                }
                onDeviceLabelRightClicked: aliasInputDialog.open()
            }

            Button{
                id: connectButton
                readonly property real requiredTextWidth: Math.max(connectTextMetrics.width, stopTextMetrics.width, reconnectTextMetrics.width)
                readonly property real requiredWidth: requiredTextWidth + leftPadding + rightPadding + root.buttonHorizontalBreathingRoom
                text: connectingIndicator.running
                      ? qsTr("Stop")
                      : (root.connectionInitialized ? qsTr("Reconnect") : qsTr("Connect Device"))
                Layout.fillWidth: root.compactMode
                Layout.minimumWidth: requiredWidth
                Layout.preferredWidth: requiredWidth
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
                    //TODO FIX Here, need to remove here calling to core
                    sessionManager.setSessionTarget(root.sessionId, hostInfo.inputText)
                    awaitingDeviceCreation = true
                    root.connectionStateChanged(true)
                    if (root.hasValidSessionId(root.sessionId)) {
                        const sessionCore = sessionManager.coreForSession(root.sessionId)
                        if (sessionCore)
                            sessionCore.onStartConnection(hostInfo.inputText)
                    }
                    connectingIndicator.running = true
                }

                function stopSendingRequests(){
                    awaitingDeviceCreation = false
                    root.connectionStateChanged(false)
                    connectingIndicator.running = false
                    const sessionCore = sessionManager.coreForSession(root.sessionId)
                    if (sessionCore)
                        sessionCore.onCloseConnection()
                }

            }

            TextMetrics {
                id: connectTextMetrics
                font: connectButton.font
                text: qsTr("Connect Device")
            }

            TextMetrics {
                id: stopTextMetrics
                font: connectButton.font
                text: qsTr("Stop")
            }

            TextMetrics {
                id: reconnectTextMetrics
                font: connectButton.font
                text: qsTr("Reconnect")
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
                readonly property real requiredTextWidth: Math.max(deleteTextMetrics.width, deleteEnglishTextMetrics.width)
                readonly property real requiredWidth: requiredTextWidth + leftPadding + rightPadding + root.buttonHorizontalBreathingRoom
                text: qsTr("Delete Device")
                Layout.fillWidth: root.compactMode
                Layout.minimumWidth: requiredWidth
                Layout.preferredWidth: requiredWidth
                onClicked: {
                    const removeConnectedDevicePage = connectionInitialized
                    connectionInitialized = false
                    awaitingDeviceCreation = false
                    root.connectedDeviceName = ""
                    if (root.hasValidSessionId(root.sessionId))
                        sessionManager.setDeviceAlias(root.sessionId, "")
                    if (root.hasValidSessionId(root.sessionId))
                        sessionManager.removeSession(root.sessionId)
                    root.sessionId = ""
                    root.connectionStateChanged(false)
                    root.removeThisObject(removeConnectedDevicePage)
                }
            }

            TextMetrics {
                id: deleteTextMetrics
                font: deleteDevice.font
                text: qsTr("Delete Device")
            }

            TextMetrics {
                id: deleteEnglishTextMetrics
                font: deleteDevice.font
                text: "Delete Device"
            }
        }



    AliasInputDialog {
        id: aliasInputDialog
        initialAlias: root.deviceAlias
        onAliasSubmitted: function(alias) {
            if (root.hasValidSessionId(root.sessionId))
                sessionManager.setDeviceAlias(root.sessionId, alias)
        }
    }

        Connections{
            target: sessionManager

            function onDeviceReady(sessionId, displayName) {
                if (!awaitingDeviceCreation || root.sessionId !== sessionId)
                    return

                awaitingDeviceCreation = false
                connectingIndicator.running = false
                connectionInitialized = true
                root.connectedDeviceName = displayName
                if (sessionManager.sessionsModel.rowCount() === 1)
                    root.sessionSelected(sessionId)
            }
        }
    }
}

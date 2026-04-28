import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    id: root
    signal connectedDeviceDeleted()
    signal connectionStateChanged(bool allowDevicePageActivation)
    signal settingsRequested()
    signal sessionSelected(string sessionId)

    header: DeviceStatusHeader {
        width: root.width
        headerText: qsTr("Settings")
        showIndicator: false
        horizontalPadding: 12
        onClicked: root.settingsRequested()
    }

    footer: Button {
        text: "Add Device"
        onClicked: addDevice()
    }

    Flickable {
        id: scrollArea
        anchors.fill: parent
        clip: true
        contentWidth: width
        contentHeight: contentColumn.implicitHeight

        Column {
            id: contentColumn
            width: scrollArea.width
            spacing: 4

            Repeater {
                id: pendingRepeater
                model: pendingModel

                delegate: RawLayioutConnection {
                    width: contentColumn.width
                    height: implicitHeight
                    onRemoveThisObject: (removeConnectedDevicePage) => removePending(index, removeConnectedDevicePage)
                    onConnectionStateChanged: (allowDevicePageActivation) => root.connectionStateChanged(allowDevicePageActivation)
                    onSessionSelected: (sessionId) => root.sessionSelected(sessionId)
                }
            }

            Repeater {
                model: sessionManager.sessionsModel

                delegate: RawLayioutConnection {
                    width: contentColumn.width
                    height: implicitHeight
                    sessionId: model.sessionId
                    onRemoveThisObject: (removeConnectedDevicePage) => removeExisting(removeConnectedDevicePage)
                    onConnectionStateChanged: (allowDevicePageActivation) => root.connectionStateChanged(allowDevicePageActivation)
                    onSessionSelected: (sessionId) => root.sessionSelected(sessionId)
                }
            }
        }
    }

    ListModel {
        id: pendingModel
    }

    Component.onCompleted: {
        if (pendingModel.count === 0 && sessionManager.sessionsModel.rowCount() === 0) {
            pendingModel.append({})
        }
    }

    function addDevice() {
        pendingModel.append({})
    }

    function removePending(index, removeConnectedDevicePage) {
        if (index >= 0 && index < pendingModel.count)
            pendingModel.remove(index)

        if (removeConnectedDevicePage)
            connectedDeviceDeleted()

        if (pendingModel.count === 0 && sessionManager.sessionsModel.rowCount() === 0)
            pendingModel.append({})
    }

    function removeExisting(removeConnectedDevicePage) {
        if (removeConnectedDevicePage)
            connectedDeviceDeleted()

        if (pendingModel.count === 0 && sessionManager.sessionsModel.rowCount() === 0)
            pendingModel.append({})
    }
}

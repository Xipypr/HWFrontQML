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

    footer: Button{
                text: "Add Device"
                onClicked: addDevice()
            }

    ListView {
        id: listView
        anchors.fill: parent
        clip: true
        readonly property real centeredVerticalMargin: Math.max(0, (height - contentHeight) / 2)
        topMargin: centeredVerticalMargin
        bottomMargin: centeredVerticalMargin
        delegate: RawLayioutConnection{
            width: listView.width
            height: implicitHeight
            sessionId: model.sessionId
            onRemoveThisObject: (removeConnectedDevicePage) => removeDevice(index, removeConnectedDevicePage)
            onConnectionStateChanged: (allowDevicePageActivation) => root.connectionStateChanged(allowDevicePageActivation)
            onSessionSelected: (sessionId) => root.sessionSelected(sessionId)
        }

        model: sessionManager.sessionsModel
    }

    Component.onCompleted: addDevice()

    function addDevice()
    {
        sessionManager.appendSession()
    }

    function removeDevice(index, removeConnectedDevicePage)
    {
        if (removeConnectedDevicePage)
        {
            connectedDeviceDeleted()
        }
    }
}

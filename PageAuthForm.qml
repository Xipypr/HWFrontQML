import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    id: root
    signal connectedDeviceDeleted()
    signal connectionStateChanged(bool allowDevicePageActivation)
    signal settingsRequested()
    signal sessionSelected(string sessionId)
    signal deviceAliasChangedByUser(string sessionId, string alias)
    property var aliasForSessionCallback: null

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
            deviceAlias: model.sessionId && model.sessionId.length > 0 && root.aliasForSessionCallback ? root.aliasForSessionCallback(model.sessionId) : ""
            onRemoveThisObject: (removeConnectedDevicePage) => removeDevice(index, removeConnectedDevicePage)
            onConnectionStateChanged: (allowDevicePageActivation) => root.connectionStateChanged(allowDevicePageActivation)
            onSessionSelected: (sessionId) => root.sessionSelected(sessionId)
            onDeviceAliasChangedByUser: (sessionId, alias) => root.deviceAliasChangedByUser(sessionId, alias)
        }

        model: sessionManager.sessionsModel
    }

    Component.onCompleted: addDevice()

    function addDevice()
    {
        sessionManager.createSession("")
    }

    function removeDevice(index, removeConnectedDevicePage)
    {
        if (removeConnectedDevicePage)
        {
            connectedDeviceDeleted()
        }
    }
}

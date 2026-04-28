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

    // ListView для представления данных в виде списка
        ListView {
            id: listView
            // Размещаем его в оставшейся части окна приложения
            anchors.fill: parent
            clip: true
            readonly property real centeredVerticalMargin: Math.max(0, (height - contentHeight) / 2)
            topMargin: centeredVerticalMargin
            bottomMargin: centeredVerticalMargin
            delegate: RawLayioutConnection{
                id: delegate
                width: listView.width
                height: implicitHeight
                onRemoveThisObject: (removeConnectedDevicePage) => removeDevice(index, removeConnectedDevicePage)
                onConnectionStateChanged: (allowDevicePageActivation) => root.connectionStateChanged(allowDevicePageActivation)
                onSessionSelected: (sessionId) => root.sessionSelected(sessionId)
            }

            // Сама модель, в которой будут содержаться все элементы
            model: sessionManager.sessionsModel
        }

        function addDevice()
        {
            console.log("Adding deivce")
        }

        function removeDevice(index, removeConnectedDevicePage)
        {
            console.log("Deleting " + index)
            if (removeConnectedDevicePage)
            {
                connectedDeviceDeleted()
            }
            console.log(listView.count)
        }
}

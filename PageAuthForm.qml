import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    id: root
    signal connectedDeviceDeleted()
    signal connectionStateChanged(bool allowDevicePageActivation)
    signal settingsRequested()

    header: ToolBar {
        id: settingsHeader
        implicitHeight: 56

        background: Rectangle {
            id: headerBackground
            color: headerTap.pressed ? "#223150" : "#17233A"

            Behavior on color {
                ColorAnimation { duration: 120 }
            }
        }

        TapHandler {
            id: headerTap
            onTapped: root.settingsRequested()
        }

        Label {
            anchors.fill: parent
            anchors.leftMargin: 12
            verticalAlignment: Text.AlignVCenter
            text: qsTr("Settings")
            font.bold: true
            color: "#E2E8F0"
        }
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
            }

            // Сама модель, в которой будут содержаться все элементы
            model: ListModel {
                id: listModel // задаём ей id для обращения
            }
        }

        Component.onCompleted: addDevice()

        function addDevice()
        {
            console.log("Adding deivce")
            listModel.append({})
            console.log(listView.count)
        }

        function removeDevice(index, removeConnectedDevicePage)
        {
            console.log("Deleting " + index)
            if (index < 0 || index >= listModel.count) {
                console.warn("Skip delete: invalid index " + index + ", count=" + listModel.count)
                return
            }
            listModel.remove(index)
            if (removeConnectedDevicePage)
            {
                connectedDeviceDeleted()
            }
            console.log(listView.count)
        }
}

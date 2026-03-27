import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Page {
    id: root
    signal connectedDeviceDeleted()
    signal connectionStateChanged(bool allowDevicePageActivation)

    footer: Button{
                text: "Add Device"
                onClicked: addDevice()
            }

    // ListView для представления данных в виде списка
        ListView {
            id: listView
            // Размещаем его в оставшейся части окна приложения
            anchors.fill: parent
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

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Page {
    id: root

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
                height: 60

                Component.onCompleted: {
                        delegate.removeThisObject.connect(removeDevice)
                    }
            }

            // Сама модель, в которой будут содержаться все элементы
            model: ListModel {
                id: listModel // задаём ей id для обращения
            }
        }


        function addDevice()
        {
            console.log("AAAAAAAA")
            listModel.append({"index": listView.count})
            console.log(listView.count)
        }

        function removeDevice(index)
        {
            console.log("Deleting " + index)
            listModel.remove({index})
            console.log(listView.count)
        }
}

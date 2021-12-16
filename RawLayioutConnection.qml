import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
    id: root

    property int index

    signal removeThisObject(int index)

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
            placeholderText: qsTr("192.168.1.1")
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
            onClicked: sendRequest()

            function sendRequest(){
                console.log(root.index)
                //!!! TODO Переделать
                if (connectingIndicator.running)
                {
                    console.log(textField.text)
                    connectButton.text = "Connect Device"
                    connectingIndicator.running = false
                }
                else
                {
                    console.log(textField.text)
                    connectButton.text = "Stop connecting"
                    connectingIndicator.running = true
                }
            }
        }

        BusyIndicator{
            id: connectingIndicator
            visible: true
            running: false
        }

        Button{
            id: deleteDevice
            text: "Delete Device"
            onClicked: root.removeThisObject(root.index)
        }
    }
}

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import "progbarstyles"
import DeviceData 1.0

Page {
    id: root

    property var objectsArray: []
    property var desktop_device: ({})
    property string destop_name: "My_device"

    ColumnLayout {
        spacing: 0
        anchors.fill: parent
        RowLayout {
            spacing: 0
            Layout.preferredHeight: 20
            Cir_1 {
                id: test
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                titleText: "CPU"
                titleColor: "red"
                p_cur: 45

            }
            Cir_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                titleText: "GPU"
                titleColor: "green"
                p_cur: 68
            }
        }

        RowLayout {
            spacing: 0
            Layout.preferredHeight: 20
            Ver_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                p_cur: 80
            }
            Ver_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
            Ver_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                p_cur: 80
            }
            Ver_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                p_cur: 12
            }
        }

        Connections{
            target: core

            function onDeviceCreated() {
                desktop_device = core.device();
                if ( desktop_device.type === Device.DESKTOP )
                {
                    destop_name = desktop_device.name;
                    objectsArray = desktop_device.devicesList();
                    console.log(objectsArray[0].name)
                }
            }
        }
    }
}

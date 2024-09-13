import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import "progbarstyles"
import DeviceData 1.0

Page {
    id: root

    property var objectsArray: []
    property var desktop_device: ({})
    property string destop_name: core.device().name

    ColumnLayout {
        spacing: 0
        anchors.fill: parent
        RowLayout {
            spacing: 0
            Layout.preferredHeight: 20
            Cir_1 {
                id: cpu
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                titleText: "CPU"
                titleColor: "red"
                p_cur: 45

            }

            Cir_3 {
                id: ram
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                p_cur: 76
            }

            Cir_1 {
                id: gpu
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                titleText: "GPU"
                titleColor: "green"
                p_cur: 68
            }

            Text {
                id: txt
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
                    parseDevices();
                }
            }

            function parseDevices() {
                for (let i = 0; i < objectsArray.length; ++i) {
                    switch (objectsArray[i].type) {
                    case Device.MOTHERBOARD:
                        parseMotherBoard(i);
                        break;

                    case Device.PROCESSOR:
                        parseProc(i);
                        break;

                    case Device.MEMORY:
                        parseMemory(i);
                        break;

                    case Device.VIDEO_CARD:
                        parseVideocard(i);
                        break;

                    case Device.HARD_DISK:
                        parseHdd(i);
                        break;
                    }
                }
            }

            function parseMotherBoard(iter)
            {
//                console.log(objectsArray[iter].name)
            }

            function parseProc(iter)
            {
//                console.log(objectsArray[iter].name)
                let procObject = objectsArray[iter]
                cpu.titleText = procObject.name.substring(0, 3)
                cpu.p_cur = procObject.loading
            }

            function parseMemory(iter)
            {
//                console.log(objectsArray[iter].name)
                let memObject = objectsArray[iter]
                ram.prcnt = memObject.loading
            }

            function parseVideocard(iter)
            {
//                console.log(objectsArray[iter].name)
                let videoObject = objectsArray[iter]
                gpu.titleText = videoObject.name.substring(0, 6)
                gpu.p_cur = videoObject.loading
            }

            function parseHdd(iter)
            {
//                console.log(objectsArray[iter].name)
            }



        }
    }
}

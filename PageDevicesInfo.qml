import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import DeviceData 1.0

Page {
    id: root

    property var objectsArray: []
    property var desktop_device: ({})
    property string destop_name: core.device().name
    property var diskMetrics: []
    property var diskNamesSeen: ({})

    header: DeviceStatusHeader {
        width: root.width
        deviceName: destop_name
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: width > 900 ? 3 : width > 580 ? 2 : 1
            columnSpacing: 12
            rowSpacing: 12

            MetricCard {
                id: cpu
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.preferredHeight: 160
                Layout.minimumHeight: 150
                title: "CPU"
                value: 45
                variant: "segments"
            }

            MetricCard {
                id: ram
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.preferredHeight: 160
                Layout.minimumHeight: 150
                title: "RAM"
                value: 76
                variant: "ring"
            }

            MetricCard {
                id: gpu
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.preferredHeight: 160
                Layout.minimumHeight: 150
                title: "GPU"
                value: 68
                variant: "linear"
            }

            Repeater {
                model: root.diskMetrics

                delegate: MetricCard {
                    Layout.fillWidth: true
                    Layout.fillHeight: false
                    Layout.preferredHeight: 160
                    Layout.minimumHeight: 150
                    title: modelData.title
                    value: modelData.value
                    variant: "linear"
                }
            }
        }

        Connections {
            target: core

            function onDeviceCreated() {
                desktop_device = core.device();
                if (desktop_device.type === Device.DESKTOP)
                {
                    destop_name = desktop_device.name;
                    objectsArray = desktop_device.devicesList();
                    parseDevices();
                }
            }

            function parseDevices() {
                diskMetrics = [];
                diskNamesSeen = ({});
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
                //console.log(objectsArray[iter].name)
            }

            function parseProc(iter)
            {
                let procObject = objectsArray[iter]
                cpu.title = procObject.name.substring(0, 12)
                cpu.value = procObject.loading
            }

            function parseMemory(iter)
            {
                let memObject = objectsArray[iter]
                ram.value = memObject.loading
            }

            function parseVideocard(iter)
            {
                let videoObject = objectsArray[iter]
                gpu.title = videoObject.name.substring(0, 12)
                gpu.value = videoObject.loading
            }

            function parseHdd(iter)
            {
                let diskObject = objectsArray[iter]
                let fullDiskName = diskObject.name
                if (diskNamesSeen[fullDiskName]) {
                    return;
                }

                let updatedSeenNames = Object.assign({}, diskNamesSeen)
                updatedSeenNames[fullDiskName] = true
                diskNamesSeen = updatedSeenNames

                let updatedMetrics = diskMetrics.slice()
                updatedMetrics.push({
                    "title": fullDiskName.substring(0, 14),
                    "value": diskObject.loading
                })
                diskMetrics = updatedMetrics
            }
        }
    }
}

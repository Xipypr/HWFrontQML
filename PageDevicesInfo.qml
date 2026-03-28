import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import DeviceData 1.0

Page {
    id: root

    property var objectsArray: []
    property var desktop_device: ({})
    property string destop_name: core.device().name
    property int customWidgetCounter: 1

    ListModel {
        id: widgetModel
    }

    function resetDefaultWidgets() {
        widgetModel.clear()
        widgetModel.append({ key: "cpu", title: "CPU", value: 45, variant: "segments" })
        widgetModel.append({ key: "ram", title: "RAM", value: 76, variant: "ring" })
        widgetModel.append({ key: "gpu", title: "GPU", value: 68, variant: "linear" })
    }

    function findWidgetIndex(widgetKey) {
        for (let i = 0; i < widgetModel.count; ++i) {
            if (widgetModel.get(i).key === widgetKey)
                return i
        }
        return -1
    }

    function updateWidgetData(widgetKey, widgetTitle, widgetValue) {
        const index = findWidgetIndex(widgetKey)
        if (index >= 0) {
            widgetModel.setProperty(index, "title", widgetTitle)
            widgetModel.setProperty(index, "value", widgetValue)
        }
    }

    Component.onCompleted: resetDefaultWidgets()

    header: DeviceStatusHeader {
        width: root.width
        headerText: destop_name
        onClicked: {
            deviceSettingsDialog.open()
        }
    }

    DeviceSettingsDialog {
        id: deviceSettingsDialog
        onSetDeviceNameSelected: {
            console.log("Device settings: set device name clicked")
        }

        onChangeLayoutSelected: {
            widgetLayoutDialog.open()
        }
    }

    WidgetLayoutDialog {
        id: widgetLayoutDialog
        widgetsModel: widgetModel

        onMoveWidgetUp: function(index) {
            if (index > 0)
                widgetModel.move(index, index - 1, 1)
        }

        onMoveWidgetDown: function(index) {
            if (index < widgetModel.count - 1)
                widgetModel.move(index, index + 1, 1)
        }

        onDeleteWidget: function(index) {
            if (index >= 0 && index < widgetModel.count)
                widgetModel.remove(index, 1)
        }

        onAddWidget: function(widgetKey, widgetTitle, widgetVariant) {
            let finalTitle = widgetTitle
            if (widgetKey === "custom") {
                finalTitle = "Новый виджет " + customWidgetCounter
                customWidgetCounter += 1
            }
            widgetModel.append({ key: widgetKey, title: finalTitle, value: 0, variant: widgetVariant })
        }
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

            Repeater {
                model: widgetModel

                delegate: MetricCard {
                    Layout.fillWidth: true
                    Layout.fillHeight: false
                    Layout.preferredHeight: 160
                    Layout.minimumHeight: 150
                    title: model.title
                    value: model.value
                    variant: model.variant
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
                updateWidgetData("cpu", procObject.name.substring(0, 12), procObject.loading)
            }

            function parseMemory(iter)
            {
                let memObject = objectsArray[iter]
                updateWidgetData("ram", "RAM", memObject.loading)
            }

            function parseVideocard(iter)
            {
                let videoObject = objectsArray[iter]
                updateWidgetData("gpu", videoObject.name.substring(0, 12), videoObject.loading)
            }

            function parseHdd(iter)
            {
                //console.log(objectsArray[iter].name)
            }
        }
    }
}

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import DeviceData 1.0

Page {
    id: root

    readonly property var availableVariants: [
        { label: "Из карточек", value: "" },
        { label: "Segments", value: "segments" },
        { label: "Ring", value: "ring" },
        { label: "Linear", value: "linear" },
        { label: "Arc 180°", value: "arc180" }
    ]
    property string globalWidgetVariant: ""
    property var objectsArray: []
    property var desktop_device: ({})
    property string destop_name: core.device().name
    property int nextWidgetId: 1

    ListModel {
        id: widgetModel
    }

    function resetDefaultWidgets() {
        widgetModel.clear()
        widgetModel.append({ uid: nextWidgetId++, key: "cpu", title: "CPU", value: 45, variant: "arc180", variantOverride: "" })
        widgetModel.append({ uid: nextWidgetId++, key: "ram", title: "RAM", value: 76, variant: "segments", variantOverride: "" })
        widgetModel.append({ uid: nextWidgetId++, key: "gpu", title: "GPU", value: 68, variant: "linear", variantOverride: "" })
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

    function currentValuesById() {
        const values = ({})
        for (let i = 0; i < widgetModel.count; ++i) {
            const item = widgetModel.get(i)
            values[item.uid] = item.value
        }
        return values
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

        onApplyLayout: function(widgets) {
            const latestValues = currentValuesById()
            widgetModel.clear()
            for (let i = 0; i < widgets.length; ++i) {
                const item = widgets[i]
                const uid = item.uid !== undefined ? item.uid : nextWidgetId++
                widgetModel.append({
                    uid: uid,
                    key: item.key,
                    title: item.title,
                    value: latestValues[uid] !== undefined ? latestValues[uid] : item.value,
                    variant: item.variant,
                    variantOverride: item.variantOverride !== undefined ? item.variantOverride : ""
                })
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: "Глобальный режим:"
                color: "#E2E8F0"
            }

            ComboBox {
                id: globalVariantCombo
                model: root.availableVariants
                textRole: "label"

                Component.onCompleted: {
                    for (let i = 0; i < model.length; ++i) {
                        if (model[i].value === root.globalWidgetVariant) {
                            currentIndex = i
                            break
                        }
                    }
                }

                onActivated: root.globalWidgetVariant = model[index].value
            }

            Item { Layout.fillWidth: true }
        }

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
                    globalVariant: root.globalWidgetVariant
                    variantOverride: model.variantOverride !== undefined ? model.variantOverride : ""

                    onVariantOverrideSelected: function(mode) {
                        widgetModel.setProperty(index, "variantOverride", mode)
                    }
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
                let hddObject = objectsArray[iter]
                updateWidgetData("hdd", hddObject.name.substring(0, 12), hddObject.loading)
            }
        }
    }
}

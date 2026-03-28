import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import DeviceData 1.0

Page {
    id: root

    property var objectsArray: []
    property var desktop_device: ({})
    property string destop_name: core.device() ? core.device().name : ""
    property string deviceAlias: ""
    property var availableSensors: []

    ListModel {
        id: selectedWidgetsModel
    }

    Component.onCompleted: ensureInitialWidgets()

    header: DeviceStatusHeader {
        width: root.width
        deviceName: deviceAlias.length > 0 ? deviceAlias : destop_name
        onClicked: deviceSettingsDialog.open()
    }

    DeviceSettingsDialog {
        id: deviceSettingsDialog
        onSetAliasRequested: aliasDialog.open()
        onEditLayoutRequested: widgetLayoutDialog.open()
    }

    DeviceAliasDialog {
        id: aliasDialog
        aliasValue: deviceAlias
        onAliasSaved: function(newAlias) {
            deviceAlias = newAlias
            core.setDeviceAlias(destop_name, newAlias)
        }
    }

    DeviceLayoutDialog {
        id: widgetLayoutDialog
        availableSensors: root.availableSensors
        selectedWidgetsModel: selectedWidgetsModel
        defaultVariantFn: defaultVariant
        refreshValuesFn: updateWidgetValues
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
                model: selectedWidgetsModel

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
                if (!desktop_device)
                    return

                if (desktop_device.type === Device.DESKTOP)
                {
                    destop_name = desktop_device.name;
                    objectsArray = desktop_device.devicesList();
                    rebuildAvailableSensors();
                    deviceAlias = core.deviceAlias(destop_name);
                    if (hasOnlyPlaceholderWidgets()) {
                        selectedWidgetsModel.clear()
                    }
                    initializeDefaultWidgets();
                    updateWidgetValues();
                }
            }
        }
    }

    function initializeDefaultWidgets()
    {
        if (selectedWidgetsModel.count > 0)
            return

        appendDefaultWidget(Device.PROCESSOR, "Загрузка CPU")
        appendDefaultWidget(Device.MEMORY, "Загрузка RAM")
        appendDefaultWidget(Device.VIDEO_CARD, "Загрузка GPU")
    }

    function ensureInitialWidgets()
    {
        if (selectedWidgetsModel.count > 0)
            return

        selectedWidgetsModel.append({
            title: "CPU",
            sensorIndex: -1,
            sensorName: "CPU",
            metricKey: "loading",
            metricLabel: "Нагрузка, %",
            variant: "segments",
            value: 45
        })

        selectedWidgetsModel.append({
            title: "RAM",
            sensorIndex: -1,
            sensorName: "RAM",
            metricKey: "loading",
            metricLabel: "Нагрузка, %",
            variant: "ring",
            value: 76
        })

        selectedWidgetsModel.append({
            title: "GPU",
            sensorIndex: -1,
            sensorName: "GPU",
            metricKey: "loading",
            metricLabel: "Нагрузка, %",
            variant: "linear",
            value: 68
        })
    }

    function hasOnlyPlaceholderWidgets()
    {
        if (selectedWidgetsModel.count === 0)
            return false

        for (let i = 0; i < selectedWidgetsModel.count; ++i) {
            if (selectedWidgetsModel.get(i).sensorIndex >= 0)
                return false
        }
        return true
    }

    function appendDefaultWidget(deviceType, fallbackTitle)
    {
        for (let i = 0; i < availableSensors.length; ++i) {
            let sensor = availableSensors[i]
            if (sensor.type !== deviceType)
                continue

            for (let m = 0; m < sensor.metrics.length; ++m) {
                if (sensor.metrics[m].key === "loading") {
                    selectedWidgetsModel.append({
                        title: sensor.name || fallbackTitle,
                        sensorIndex: sensor.index,
                        sensorName: sensor.name,
                        metricKey: "loading",
                        metricLabel: sensor.metrics[m].label,
                        variant: defaultVariant("loading"),
                        value: 0
                    })
                    return
                }
            }
        }
    }

    function rebuildAvailableSensors()
    {
        availableSensors = []
        for (let i = 0; i < objectsArray.length; ++i) {
            let sensor = objectsArray[i]
            let metricOptions = collectMetrics(sensor)
            if (metricOptions.length === 0)
                continue

            availableSensors.push({
                index: i,
                type: sensor.type,
                name: sensor.name,
                metrics: metricOptions
            })
        }
    }

    function collectMetrics(sensorObject)
    {
        let options = []
        let metricMap = [
            { key: "loading", label: "Нагрузка, %" },
            { key: "temperature", label: "Температура, °C" },
            { key: "power", label: "Потребление, W" },
            { key: "frequency", label: "Частота, MHz" },
            { key: "usage", label: "Использование" }
        ]

        for (let i = 0; i < metricMap.length; ++i) {
            let metric = metricMap[i]
            if (sensorObject[metric.key] !== undefined) {
                options.push(metric)
            }
        }

        return options
    }

    function updateWidgetValues()
    {
        for (let i = 0; i < selectedWidgetsModel.count; ++i) {
            let widget = selectedWidgetsModel.get(i)
            if (widget.sensorIndex < 0 || widget.sensorIndex >= objectsArray.length)
                continue

            let sensorObject = objectsArray[widget.sensorIndex]
            let rawValue = sensorObject[widget.metricKey]
            let value = rawValue !== undefined ? rawValue : 0
            selectedWidgetsModel.setProperty(i, "value", value)

            if (!widget.title || widget.title.length === 0) {
                selectedWidgetsModel.setProperty(i, "title", sensorObject.name)
            }
        }
    }

    function defaultVariant(metricKey)
    {
        switch (metricKey) {
        case "temperature":
            return "linear"
        case "power":
            return "segments"
        default:
            return "ring"
        }
    }

}

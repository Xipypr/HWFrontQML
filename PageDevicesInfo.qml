import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import DeviceData 1.0

Page {
    id: root

    property var objectsArray: []
    property var desktop_device: ({})
    property string destop_name: core.device().name
    property var availableSensors: []

    ListModel {
        id: selectedWidgetsModel
    }

    Component.onCompleted: ensureInitialWidgets()

    header: DeviceStatusHeader {
        width: root.width
        deviceName: destop_name
        onClicked: cardsEditDialog.open()
    }

    Dialog {
        id: cardsEditDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min((parent ? parent.width : root.width) - 32, 560)
        modal: true
        title: "Редактор карточек"
        standardButtons: Dialog.Close
        padding: 16

        contentItem: ColumnLayout {
            spacing: 10

            Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: "Управляйте набором виджетов: меняйте порядок, удаляйте и добавляйте новые на основе доступных сенсоров/метрик."
                color: "#CBD5E1"
            }

            Frame {
                Layout.fillWidth: true
                Layout.preferredHeight: 220
                background: Rectangle {
                    radius: 6
                    color: "#1E293B"
                    border.color: "#334155"
                }

                ListView {
                    id: selectedWidgetsView
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8
                    clip: true
                    model: selectedWidgetsModel

                    delegate: Rectangle {
                        width: selectedWidgetsView.width
                        height: 48
                        radius: 6
                        color: "#253247"

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            spacing: 8

                            Label {
                                Layout.fillWidth: true
                                text: model.title + "  ·  " + model.sensorName + " / " + model.metricLabel
                                color: "#E2E8F0"
                                elide: Text.ElideRight
                            }

                            ToolButton {
                                text: "↑"
                                enabled: index > 0
                                onClicked: selectedWidgetsModel.move(index, index - 1, 1)
                            }

                            ToolButton {
                                text: "↓"
                                enabled: index < selectedWidgetsModel.count - 1
                                onClicked: selectedWidgetsModel.move(index, index + 1, 1)
                            }

                            ToolButton {
                                text: "✕"
                                onClicked: selectedWidgetsModel.remove(index)
                            }
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    text: "Добавить виджет"
                    enabled: availableSensors.length > 0
                    onClicked: addWidgetDialog.open()
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    color: "#94A3B8"
                    text: availableSensors.length > 0
                          ? "Доступно сенсоров: " + availableSensors.length
                          : "Сначала подключите устройство с данными сенсоров"
                }
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                color: "#94A3B8"
                text: "Следующий шаг — добавить персональные настройки виджета (тип визуализации, диапазоны, пороги)."
            }
        }
    }



    AddWidgetDialog {
        id: addWidgetDialog
        availableSensors: root.availableSensors

        onWidgetChosen: function(widgetData) {
            widgetData.variant = defaultVariant(widgetData.metricKey)
            selectedWidgetsModel.append(widgetData)
            updateWidgetValues()
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
                if (desktop_device.type === Device.DESKTOP)
                {
                    destop_name = desktop_device.name;
                    objectsArray = desktop_device.devicesList();
                    rebuildAvailableSensors();
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

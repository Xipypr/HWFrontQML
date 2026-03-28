import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import DeviceData 1.0

Page {
    id: root

    property var objectsArray: []
    property var desktop_device: ({})
    property string destop_name: core.device().name

    // [{ sensorName, metricKey, metricName, value, variant }]
    property var widgetCards: []
    // [{ name, type, variant, metrics: [{ key, name, value }] }]
    property var availableSensors: []

    property int selectedSensorIndex: 0
    property int selectedMetricIndex: 0

    header: DeviceStatusHeader {
        width: root.width
        deviceName: destop_name
        highlighted: true
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Rectangle {
            Layout.fillWidth: true
            radius: 12
            color: Qt.rgba(23 / 255, 35 / 255, 58 / 255, 0.95)
            border.color: Qt.rgba(100 / 255, 116 / 255, 139 / 255, 0.5)
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 8

                Label {
                    text: qsTr("Добавить виджет")
                    color: "#E2E8F0"
                    font.bold: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    ComboBox {
                        id: sensorCombo
                        Layout.fillWidth: true
                        model: root.availableSensors
                        textRole: "name"
                        onCurrentIndexChanged: {
                            root.selectedSensorIndex = Math.max(0, currentIndex)
                            root.selectedMetricIndex = 0
                        }
                    }

                    ComboBox {
                        id: metricCombo
                        Layout.fillWidth: true
                        model: root.availableSensors.length > 0 && root.selectedSensorIndex < root.availableSensors.length
                               ? root.availableSensors[root.selectedSensorIndex].metrics : []
                        textRole: "name"
                        onCurrentIndexChanged: root.selectedMetricIndex = Math.max(0, currentIndex)
                    }

                    Button {
                        text: qsTr("Добавить")
                        enabled: sensorCombo.count > 0 && metricCombo.count > 0
                        onClicked: root.addWidgetFromSelection()
                    }
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.min(contentHeight, 110)
                    clip: true
                    model: root.widgetCards
                    spacing: 4
                    delegate: Rectangle {
                        width: ListView.view.width
                        height: 30
                        radius: 8
                        color: Qt.rgba(51 / 255, 65 / 255, 85 / 255, 0.65)

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 6
                            spacing: 8

                            Label {
                                Layout.fillWidth: true
                                text: (index + 1) + ". " + modelData.sensorName + " / " + modelData.metricName
                                color: "#BFDBFE"
                                elide: Text.ElideRight
                            }

                            ToolButton {
                                text: "↑"
                                enabled: index > 0
                                onClicked: root.moveWidget(index, index - 1)
                            }

                            ToolButton {
                                text: "↓"
                                enabled: index < root.widgetCards.length - 1
                                onClicked: root.moveWidget(index, index + 1)
                            }

                            ToolButton {
                                text: "✕"
                                onClicked: root.removeWidget(index)
                            }
                        }
                    }
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: width > 900 ? 3 : width > 580 ? 2 : 1
            columnSpacing: 12
            rowSpacing: 12

            Repeater {
                model: root.widgetCards

                delegate: MetricCard {
                    Layout.fillWidth: true
                    Layout.fillHeight: false
                    Layout.preferredHeight: 160
                    Layout.minimumHeight: 150
                    title: modelData.sensorName
                    value: modelData.value
                    variant: modelData.variant
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
                let nextSensors = []
                for (let i = 0; i < objectsArray.length; ++i) {
                    let dev = objectsArray[i]
                    let sensor = mapDeviceToSensor(dev)
                    if (!sensor)
                        continue

                    let existingIndex = findSensorIndex(nextSensors, sensor.name)
                    if (existingIndex === -1) {
                        nextSensors.push(sensor)
                    }
                }

                availableSensors = nextSensors
                refreshWidgetValues()

                if (widgetCards.length === 0) {
                    addDefaultWidgets()
                }

                clampSelectionIndexes()
            }

            function findSensorIndex(sensors, name) {
                for (let i = 0; i < sensors.length; ++i) {
                    if (sensors[i].name === name)
                        return i
                }
                return -1
            }

            function mapDeviceToSensor(deviceObject) {
                let metrics = []

                if (deviceObject.loading !== undefined)
                    metrics.push({ "key": "loading", "name": qsTr("Загрузка"), "value": deviceObject.loading })
                if (deviceObject.temperature !== undefined)
                    metrics.push({ "key": "temperature", "name": qsTr("Температура"), "value": deviceObject.temperature })

                if (metrics.length === 0)
                    return null

                let variant = "segments"
                if (deviceObject.type === Device.MEMORY)
                    variant = "ring"
                else if (deviceObject.type === Device.VIDEO_CARD || deviceObject.type === Device.HARD_DISK)
                    variant = "linear"

                return {
                    "name": deviceObject.name,
                    "type": deviceObject.type,
                    "variant": variant,
                    "metrics": metrics
                }
            }
        }
    }

    function clampSelectionIndexes() {
        if (selectedSensorIndex >= availableSensors.length)
            selectedSensorIndex = Math.max(0, availableSensors.length - 1)

        let metricCount = 0
        if (availableSensors.length > 0)
            metricCount = availableSensors[selectedSensorIndex].metrics.length

        if (selectedMetricIndex >= metricCount)
            selectedMetricIndex = Math.max(0, metricCount - 1)
    }

    function addWidgetFromSelection() {
        if (availableSensors.length === 0)
            return

        let sensor = availableSensors[selectedSensorIndex]
        if (!sensor || sensor.metrics.length === 0)
            return

        let metric = sensor.metrics[selectedMetricIndex]
        let next = widgetCards.slice()
        next.push({
            "sensorName": sensor.name,
            "metricKey": metric.key,
            "metricName": metric.name,
            "value": metric.value,
            "variant": sensor.variant
        })
        widgetCards = next
    }

    function addDefaultWidgets() {
        let next = []

        for (let i = 0; i < availableSensors.length; ++i) {
            let sensor = availableSensors[i]
            if (sensor.metrics.length === 0)
                continue

            let isCore = sensor.type === Device.PROCESSOR
                      || sensor.type === Device.MEMORY
                      || sensor.type === Device.VIDEO_CARD
                      || sensor.type === Device.HARD_DISK

            if (!isCore)
                continue

            next.push({
                "sensorName": sensor.name,
                "metricKey": sensor.metrics[0].key,
                "metricName": sensor.metrics[0].name,
                "value": sensor.metrics[0].value,
                "variant": sensor.variant
            })
        }

        widgetCards = next
    }

    function refreshWidgetValues() {
        if (widgetCards.length === 0)
            return

        let next = []
        for (let i = 0; i < widgetCards.length; ++i) {
            let widget = widgetCards[i]
            let updated = null

            for (let j = 0; j < availableSensors.length; ++j) {
                let sensor = availableSensors[j]
                if (sensor.name !== widget.sensorName)
                    continue

                for (let m = 0; m < sensor.metrics.length; ++m) {
                    let metric = sensor.metrics[m]
                    if (metric.key === widget.metricKey) {
                        updated = {
                            "sensorName": widget.sensorName,
                            "metricKey": widget.metricKey,
                            "metricName": widget.metricName,
                            "value": metric.value,
                            "variant": sensor.variant
                        }
                        break
                    }
                }

                if (updated)
                    break
            }

            if (updated)
                next.push(updated)
        }

        widgetCards = next
    }

    function removeWidget(index) {
        if (index < 0 || index >= widgetCards.length)
            return

        let next = widgetCards.slice()
        next.splice(index, 1)
        widgetCards = next
    }

    function moveWidget(from, to) {
        if (from === to || from < 0 || to < 0 || from >= widgetCards.length || to >= widgetCards.length)
            return

        let next = widgetCards.slice()
        let moving = next.splice(from, 1)[0]
        next.splice(to, 0, moving)
        widgetCards = next
    }
}

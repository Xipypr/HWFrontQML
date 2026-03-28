import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min((parent ? parent.width : 480) - 32, 460)
    modal: true
    title: "Добавить виджет"
    standardButtons: Dialog.Ok | Dialog.Cancel
    padding: 16

    property var availableSensors: []
    property var metricOptions: []

    signal widgetChosen(var widgetData)

    onOpened: {
        if (availableSensors.length > 0) {
            sensorSelector.currentIndex = 0
            refreshMetricOptions()
            metricSelector.currentIndex = 0
        }
    }

    onAccepted: {
        if (sensorSelector.currentIndex < 0 || metricSelector.currentIndex < 0)
            return

        let sensor = availableSensors[sensorSelector.currentIndex]
        let metric = metricOptions[metricSelector.currentIndex]
        widgetChosen({
            title: sensor.name,
            sensorIndex: sensor.index,
            sensorName: sensor.name,
            metricKey: metric.key,
            metricLabel: metric.label,
            variant: "ring",
            value: 0
        })
    }

    function refreshMetricOptions() {
        if (sensorSelector.currentIndex < 0 || sensorSelector.currentIndex >= availableSensors.length) {
            metricOptions = []
            return
        }
        metricOptions = availableSensors[sensorSelector.currentIndex].metrics
    }

    contentItem: ColumnLayout {
        spacing: 10

        Label {
            text: "Сенсор"
            color: "#CBD5E1"
        }

        ComboBox {
            id: sensorSelector
            Layout.fillWidth: true
            model: root.availableSensors
            textRole: "name"
            onCurrentIndexChanged: {
                root.refreshMetricOptions()
                metricSelector.currentIndex = 0
            }
        }

        Label {
            text: "Метрика"
            color: "#CBD5E1"
        }

        ComboBox {
            id: metricSelector
            Layout.fillWidth: true
            model: root.metricOptions
            textRole: "label"
        }
    }
}

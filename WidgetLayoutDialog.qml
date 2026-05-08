import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    required property var widgetsModel

    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width - 32, 440)
    modal: true
    focus: true
    padding: 20
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property var deviceOptions: []
    property var metricOptions: []
    property string addError: ""

    function selectedDeviceId() {
        if (deviceCombo.currentIndex < 0 || deviceCombo.currentIndex >= deviceOptions.length)
            return ""

        return deviceOptions[deviceCombo.currentIndex].deviceId || ""
    }

    function selectedMetricId() {
        if (metricCombo.currentIndex < 0 || metricCombo.currentIndex >= metricOptions.length)
            return ""

        return metricOptions[metricCombo.currentIndex].metricId || ""
    }

    function refreshDevices() {
        const currentDeviceId = selectedDeviceId()
        deviceOptions = widgetsModel.availableDevices()
        deviceCombo.currentIndex = -1

        for (let i = 0; i < deviceOptions.length; ++i) {
            if (deviceOptions[i].deviceId === currentDeviceId) {
                deviceCombo.currentIndex = i
                break
            }
        }

        if (deviceCombo.currentIndex < 0 && deviceOptions.length > 0)
            deviceCombo.currentIndex = 0

        refreshMetrics()
    }

    function refreshMetrics() {
        const currentMetricId = selectedMetricId()
        metricOptions = widgetsModel.availableMetricsForDevice(selectedDeviceId())
        metricCombo.currentIndex = -1

        for (let i = 0; i < metricOptions.length; ++i) {
            if (metricOptions[i].metricId === currentMetricId) {
                metricCombo.currentIndex = i
                break
            }
        }

        if (metricCombo.currentIndex < 0 && metricOptions.length > 0)
            metricCombo.currentIndex = 0
    }

    onOpened: {
        addError = ""
        refreshDevices()
    }

    contentItem: ColumnLayout {
        spacing: 12

        Label {
            Layout.fillWidth: true
            text: "Компоновка виджетов"
            color: "#E2E8F0"
            font.pixelSize: 18
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
        }

        ListView {
            id: widgetsList
            Layout.fillWidth: true
            Layout.preferredHeight: 260
            clip: true
            spacing: 8
            model: root.widgetsModel

            delegate: Rectangle {
                width: widgetsList.width
                height: 52
                radius: 8
                color: "#1E293B"
                border.width: 1
                border.color: "#334155"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            Layout.fillWidth: true
                            text: model.title
                            color: "#E2E8F0"
                            elide: Text.ElideRight
                        }

                        Label {
                            Layout.fillWidth: true
                            text: model.metricId + (model.unit ? " · " + model.unit : "")
                            color: "#94A3B8"
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }
                    }

                    ToolButton {
                        text: "↑"
                        enabled: index > 0
                        onClicked: root.widgetsModel.moveWidget(index, index - 1)
                    }

                    ToolButton {
                        text: "↓"
                        enabled: root.widgetsModel && index < widgetsList.count - 1
                        onClicked: root.widgetsModel.moveWidget(index, index + 1)
                    }

                    ToolButton {
                        text: "✕"
                        onClicked: root.widgetsModel.removeWidget(model.widgetId)
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            ComboBox {
                id: deviceCombo
                Layout.fillWidth: true
                Layout.preferredWidth: 190
                model: root.deviceOptions
                textRole: "label"
                enabled: root.deviceOptions.length > 0
                onCurrentIndexChanged: {
                    root.addError = ""
                    root.refreshMetrics()
                }
            }

            ComboBox {
                id: metricCombo
                Layout.fillWidth: true
                Layout.preferredWidth: 130
                model: root.metricOptions
                textRole: "label"
                enabled: root.metricOptions.length > 0
                onCurrentIndexChanged: root.addError = ""
            }

            Button {
                text: "Добавить"
                enabled: root.deviceOptions.length > 0 && root.metricOptions.length > 0
                onClicked: {
                    const deviceId = root.selectedDeviceId()
                    const metricId = root.selectedMetricId()
                    if (!root.widgetsModel.addWidgetForMetric(deviceId, metricId, "segments"))
                        root.addError = "Метрика уже добавлена или недоступна."
                    else
                        root.addError = ""
                }
            }
        }

        Label {
            Layout.fillWidth: true
            visible: root.addError.length > 0
            text: root.addError
            color: "#F87171"
            wrapMode: Text.WordWrap
            font.pixelSize: 12
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Item { Layout.fillWidth: true }

            Button {
                text: "Закрыть"
                onClicked: root.close()
            }
        }
    }
}

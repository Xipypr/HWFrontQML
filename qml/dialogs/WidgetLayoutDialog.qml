import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import ".."

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
            text: qsTr("Widget layout")
            color: Theme.textPrimary
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
                color: Theme.surfaceRaised
                border.width: 1
                border.color: Theme.border

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
                            color: Theme.textPrimary
                            elide: Text.ElideRight
                        }

                        Label {
                            Layout.fillWidth: true
                            text: model.metricId + (model.unit ? " · " + model.unit : "")
                            color: Theme.textSecondary
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }
                    }

                    ToolButton {
                        icon.source: "qrc:/icons/arrow_up.png"
                        icon.width: 16
                        icon.height: 16
                        enabled: index > 0
                        onClicked: root.widgetsModel.moveWidget(index, index - 1)
                    }

                    ToolButton {
                        icon.source: "qrc:/icons/arrow_down.png"
                        icon.width: 16
                        icon.height: 16
                        enabled: index < widgetsList.count - 1
                        onClicked: root.widgetsModel.moveWidget(index, index + 1)
                    }

                    ToolButton {
                        icon.source: "qrc:/icons/delete.png"
                        icon.width: 16
                        icon.height: 16
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
                popup.width: root.contentItem.width
                popup.x: -deviceCombo.mapToItem(root.contentItem, 0, 0).x
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
                popup.width: root.contentItem.width
                popup.x: -metricCombo.mapToItem(root.contentItem, 0, 0).x
                onCurrentIndexChanged: root.addError = ""
            }

            Button {
                text: qsTr("Add")
                enabled: root.deviceOptions.length > 0 && root.metricOptions.length > 0
                onClicked: {
                    const deviceId = root.selectedDeviceId()
                    const metricId = root.selectedMetricId()
                    if (!root.widgetsModel.addWidgetForMetric(deviceId, metricId))
                        root.addError = qsTr("The metric has already been added or is unavailable.")
                    else
                        root.addError = ""
                }
            }
        }

        Label {
            Layout.fillWidth: true
            visible: root.addError.length > 0
            text: root.addError
            color: Theme.critical
            wrapMode: Text.WordWrap
            font.pixelSize: 12
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Item { Layout.fillWidth: true }

            Button {
                text: qsTr("Close")
                onClicked: root.close()
            }
        }
    }
}

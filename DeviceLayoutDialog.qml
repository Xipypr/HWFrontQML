import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min((parent ? parent.width : 560) - 32, 560)
    modal: true
    title: "Изменить компоновку виджетов"
    standardButtons: Dialog.Close
    padding: 16

    property var availableSensors: []
    property var selectedWidgetsModel
    property var defaultVariantFn
    property var refreshValuesFn

    Dialog {
        id: addMetricDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min((parent ? parent.width : 460) - 32, 460)
        modal: true
        title: "Добавить виджет"
        standardButtons: Dialog.Ok | Dialog.Cancel
        padding: 16

        property var metricOptions: []

        onOpened: {
            if (root.availableSensors.length > 0) {
                sensorSelector.currentIndex = 0
                refreshMetricOptions()
                metricSelector.currentIndex = 0
            }
        }

        onAccepted: {
            if (!root.selectedWidgetsModel)
                return

            if (sensorSelector.currentIndex < 0 || metricSelector.currentIndex < 0)
                return

            let sensor = root.availableSensors[sensorSelector.currentIndex]
            let metric = metricOptions[metricSelector.currentIndex]
            let variant = "ring"
            if (root.defaultVariantFn)
                variant = root.defaultVariantFn(metric.key)

            root.selectedWidgetsModel.append({
                title: sensor.name,
                sensorIndex: sensor.index,
                sensorName: sensor.name,
                metricKey: metric.key,
                metricLabel: metric.label,
                variant: variant,
                value: 0
            })

            if (root.refreshValuesFn)
                root.refreshValuesFn()
        }

        function refreshMetricOptions() {
            if (sensorSelector.currentIndex < 0 || sensorSelector.currentIndex >= root.availableSensors.length) {
                metricOptions = []
                return
            }
            metricOptions = root.availableSensors[sensorSelector.currentIndex].metrics
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
                    addMetricDialog.refreshMetricOptions()
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
                model: addMetricDialog.metricOptions
                textRole: "label"
            }
        }
    }

    contentItem: ColumnLayout {
        spacing: 10

        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: "Управляйте набором виджетов: меняйте порядок, удаляйте и добавляйте новые на основе доступных сенсоров/метрик. Можно добавлять повторяющиеся виджеты."
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
                model: root.selectedWidgetsModel

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
                            onClicked: root.selectedWidgetsModel.move(index, index - 1, 1)
                        }

                        ToolButton {
                            text: "↓"
                            enabled: index < root.selectedWidgetsModel.count - 1
                            onClicked: root.selectedWidgetsModel.move(index, index + 1, 1)
                        }

                        ToolButton {
                            text: "✕"
                            onClicked: root.selectedWidgetsModel.remove(index)
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "Добавить виджет"
                enabled: root.availableSensors.length > 0
                onClicked: addMetricDialog.open()
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                color: "#94A3B8"
                text: root.availableSensors.length > 0
                      ? "Доступно сенсоров: " + root.availableSensors.length
                      : "Сначала подключите устройство с данными сенсоров"
            }
        }
    }
}

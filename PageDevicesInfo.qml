import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import DeviceData 1.0
import SessionState 1.0

Page {
    id: root

    property var objectsArray: []
    property var desktop_device: ({})
    property string sessionId: ""
    property string destop_name: ""
    property string deviceAlias: ""
    property int sessionState: SessionState.IDLE

    property int nextWidgetId: 1

    signal homeRequested()

    ListModel {
        id: widgetModel
    }

    function defaultWidgetDefinitions() {
        return [
            { key: Device.PROCESSOR, title: "CPU", value: 0, variant: "arc180" },
            { key: Device.MEMORY, title: "RAM", value: 0, variant: "segments" },
            { key: Device.VIDEO_CARD, title: "GPU", value: 0, variant: "linear" },
            { key: Device.HARD_DISK, title: "HDD", value: 0, variant: "linear" }
        ]
    }

    function resetDefaultWidgets() {
        applyDiscoveredLayout(defaultWidgetDefinitions())
    }

    function metricConfigForKey(metricKey) {
        const defaults = defaultWidgetDefinitions()
        for (let i = 0; i < defaults.length; ++i) {
            if (defaults[i].key === metricKey)
                return defaults[i]
        }
        return null
    }

    function hasMetricAvailable(metricType) {
        if (metricType === -1)
            return false

        for (let i = 0; i < objectsArray.length; ++i) {
            if (objectsArray[i].type === metricType)
                return true
        }

        return false
    }

    function applyDiscoveredLayout(layoutItems) {
        widgetModel.clear()
        const isListModel = layoutItems && layoutItems.count !== undefined && layoutItems.get !== undefined
        const itemsCount = isListModel ? layoutItems.count : layoutItems.length
        for (let i = 0; i < itemsCount; ++i) {
            const item = isListModel ? layoutItems.get(i) : layoutItems[i]
            const config = metricConfigForKey(item.key)
            const metricType = item.key
            if (!hasMetricAvailable(metricType))
                continue

            widgetModel.append({
                uid: item.uid !== undefined ? item.uid : nextWidgetId++,
                key: metricType,
                title: item.title !== undefined ? item.title : (config ? config.title : "METRIC"),
                value: item.value !== undefined ? item.value : (config ? config.value : 0),
                variant: item.variant !== undefined ? item.variant : (config ? config.variant : "linear")
            })
        }
    }

    function findWidgetIndex(widgetKey) {
        for (let i = 0; i < widgetModel.count; ++i) {
            if (widgetModel.get(i).key === widgetKey)
                return i
        }
        return -1
    }

    function updateWidgetData(metricType, widgetTitle, widgetValue) {
        const index = findWidgetIndex(metricType)
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

    function openDeviceSettingsDialog() {
        if (!deviceSettingsDialogLoader.active)
            deviceSettingsDialogLoader.active = true

        if (deviceSettingsDialogLoader.status === Loader.Ready && deviceSettingsDialogLoader.item) {
            deviceSettingsDialogLoader.item.open()
            return
        }

        deviceSettingsDialogLoader.pendingOpen = true
    }

    header: DeviceStatusHeader {
        width: root.width
        headerText: root.deviceAlias.length > 0 ? root.deviceAlias : destop_name
        showHomeButton: true
        sessionState: root.sessionState
        onClicked: {
            root.openDeviceSettingsDialog()
        }
        onHomeClicked: root.homeRequested()
    }

    Component {
        id: deviceSettingsDialogComponent
        DeviceSettingsDialog {
            onSetDeviceNameSelected: {
                aliasInputDialog.open()
            }

            onChangeLayoutSelected: {
                widgetLayoutDialog.open()
            }
        }
    }

    Loader {
        id: deviceSettingsDialogLoader
        active: false
        sourceComponent: deviceSettingsDialogComponent
        property bool pendingOpen: false

        onLoaded: {
            if (pendingOpen && item) {
                pendingOpen = false
                item.open()
            }
        }
    }

    AliasInputDialog {
        id: aliasInputDialog
        initialAlias: root.deviceAlias
        onAliasSubmitted: function(alias) {
            if (root.sessionId && root.sessionId.length > 0)
                sessionManager.setDeviceAlias(root.sessionId, alias)
        }
    }

    WidgetLayoutDialog {
        id: widgetLayoutDialog
        widgetsModel: widgetModel

        onApplyLayout: function(widgets) {
            const latestValues = currentValuesById()
            const filtered = []

            for (let i = 0; i < widgets.length; ++i) {
                const item = widgets[i]
                const metricType = item.key
                if (!hasMetricAvailable(metricType))
                    continue

                const uid = item.uid !== undefined ? item.uid : nextWidgetId++
                filtered.push({
                    uid: uid,
                    key: metricType,
                    title: item.title,
                    value: latestValues[uid] !== undefined ? latestValues[uid] : item.value,
                    variant: item.variant
                })
            }

            applyDiscoveredLayout(filtered)
        }
    }


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        GridLayout {
            visible: widgetModel.count > 0
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
                    onVariantSelected: function(mode) {
                        widgetModel.setProperty(index, "variant", mode)
                    }
                }
            }
        }

        Label {
            visible: widgetModel.count === 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: "Нет доступных метрик"
            color: "#808080"
            font.pixelSize: 18
        }

        Connections {
            target: sessionManager

            function onDeviceReady(sessionId, deviceRef) {
                if (!root.sessionId || root.sessionId !== sessionId)
                    return

                desktop_device = deviceRef
                if (desktop_device.type === Device.DESKTOP)
                {
                    destop_name = desktop_device.name
                    objectsArray = desktop_device.devicesList()
                    if (widgetModel.count > 0)
                        applyDiscoveredLayout(widgetModel)
                    else
                        resetDefaultWidgets()
                    parseDevices()
                }
            }

            function parseDevices() {
                for (let i = 0; i < objectsArray.length; ++i) {
                    switch (objectsArray[i].type) {
                    case Device.MOTHERBOARD:
                        parseMotherBoard(i)
                        break

                    case Device.PROCESSOR:
                        parseProc(i)
                        break

                    case Device.MEMORY:
                        parseMemory(i)
                        break

                    case Device.VIDEO_CARD:
                        parseVideocard(i)
                        break

                    case Device.HARD_DISK:
                        parseHdd(i)
                        break
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
                updateWidgetData(Device.PROCESSOR, procObject.name.substring(0, 12), procObject.loading)
            }

            function parseMemory(iter)
            {
                let memObject = objectsArray[iter]
                updateWidgetData(Device.MEMORY, "RAM", memObject.loading)
            }

            function parseVideocard(iter)
            {
                let videoObject = objectsArray[iter]
                updateWidgetData(Device.VIDEO_CARD, videoObject.name.substring(0, 12), videoObject.loading)
            }

            function parseHdd(iter)
            {
                let hddObject = objectsArray[iter]
                updateWidgetData(Device.HARD_DISK, hddObject.name.substring(0, 12), hddObject.loading)
            }
        }
    }
}

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import DeviceData 1.0
import SessionState 1.0
import DashboardModels 1.0

Page {
    id: root

    property var objectsArray: []
    property var desktop_device: ({})
    property string sessionId: ""
    property string destop_name: ""
    property string deviceAlias: ""
    property int sessionState: SessionState.IDLE


    signal homeRequested()

    DashboardMetricsModel {
        id: widgetModel
    }

    function resetDefaultWidgets() {
        if (widgetModel.count > 0)
            return

        widgetModel.addWidget("cpu", "CPU", 45, "arc180", true)
        widgetModel.addWidget("ram", "RAM", 76, "segments", true)
        widgetModel.addWidget("gpu", "GPU", 68, "linear", true)
    }

    function updateWidgetData(widgetId, widgetTitle, widgetValue) {
        widgetModel.updateWidget(widgetId, widgetTitle, widgetValue, true)
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

    Component.onCompleted: resetDefaultWidgets()

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
                    onVariantSelected: function(mode) {
                        widgetModel.setVariant(model.widgetId, mode)
                    }
                }
            }
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

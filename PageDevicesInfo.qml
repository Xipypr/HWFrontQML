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

    property var widgetModel: sessionManager.dashboardModelForSession(root.sessionId)

    function resetDefaultWidgets() {
        if (widgetModel.rowCount() > 0)
            return

        widgetModel.addWidgetByType(DashboardMetricsModel.Cpu)
        widgetModel.addWidgetByType(DashboardMetricsModel.Ram)
        widgetModel.addWidgetByType(DashboardMetricsModel.Gpu)

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

    }
}

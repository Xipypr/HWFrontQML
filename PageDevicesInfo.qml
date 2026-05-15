import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import SessionState 1.0
import DashboardModels 1.0

Page {
    id: root

    property string sessionId: ""
    property string destop_name: ""
    property string deviceAlias: ""
    property int sessionState: SessionState.IDLE


    signal homeRequested()

    property var widgetModel: sessionManager.dashboardModelForSession(root.sessionId)

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
    }


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        GridView {
            id: gridView

            Layout.fillWidth: true
            Layout.fillHeight: true

            readonly property int columnCount: width > 900 ? 3 : width > 580 ? 2 : 1
            readonly property int rowCount: Math.ceil(count / columnCount)
            readonly property int verticalMargin: Math.max(6, Math.floor((height - rowCount * cellHeight) / 2))

            clip: true
            model: widgetModel
            cellWidth: width / columnCount
            cellHeight: 172
            topMargin: verticalMargin
            bottomMargin: verticalMargin
            boundsBehavior: Flickable.DragAndOvershootBounds
            ScrollBar.vertical: ScrollBar { }

            delegate: MetricCard {
                width: gridView.cellWidth - 12
                height: 160
                title: model.title
                value: model.value
                unit: model.unit
                variant: model.variant
                onVariantSelected: function(mode) {
                    widgetModel.setVariant(model.widgetId, mode)
                }
            }
        }

    }
}

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
    readonly property bool deviceAvailable: sessionState === SessionState.CONNECTED
    readonly property string unavailableDescription: {
        switch (sessionState) {
        case SessionState.CONNECTING:
        case SessionState.RECONNECTING:
            return qsTr("Подключение ещё не установлено. Данные виджетов появятся, когда устройство снова станет доступно.")
        case SessionState.ERROR:
            return qsTr("Проверьте устройство и подключение, затем попробуйте подключиться снова.")
        case SessionState.IDLE:
        case SessionState.DISCONNECTED:
        default:
            return qsTr("Устройство отключено или ещё не подключено. Подключите устройство, чтобы увидеть виджеты.")
        }
    }

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
            visible: root.deviceAvailable
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

        Item {
            visible: !root.deviceAvailable
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                anchors.centerIn: parent
                width: Math.min(parent.width, 420)
                height: implicitHeight
                implicitHeight: unavailableContent.implicitHeight + 48
                radius: 16
                color: Qt.rgba(27 / 255, 36 / 255, 51 / 255, 0.86)
                border.width: 1
                border.color: Qt.rgba(100 / 255, 116 / 255, 139 / 255, 0.55)

                ColumnLayout {
                    id: unavailableContent
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 12

                    StatusIndicator {
                        Layout.alignment: Qt.AlignHCenter
                        sessionState: root.sessionState
                        width: 14
                        height: 14
                    }

                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Устройство недоступно")
                        color: "#F8FAFC"
                        font.pixelSize: 20
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                    }

                    Label {
                        Layout.fillWidth: true
                        text: root.unavailableDescription
                        color: "#CBD5E1"
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }

    }
}

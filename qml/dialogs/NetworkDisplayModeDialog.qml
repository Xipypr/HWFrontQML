import QtQuick 2.15
import QtQuick.Controls 2.15
import DashboardDisplay 1.0

Dialog {
    id: dialog

    property int initialDisplayMode: DashboardDisplay.NetworkVertical

    signal displayModeSelected(int displayMode)

    readonly property var displayModeOptions: [
        { label: qsTr("Vertical"), displayMode: DashboardDisplay.NetworkVertical },
        { label: qsTr("Horizontal"), displayMode: DashboardDisplay.NetworkHorizontal }
    ]

    modal: true
    focus: true
    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width - 32, 280)
    padding: 16
    title: qsTr("Display mode")
    standardButtons: Dialog.Ok | Dialog.Cancel

    onOpened: displayModeCombo.currentIndex =
                  initialDisplayMode === DashboardDisplay.NetworkHorizontal ? 1 : 0
    onAccepted: {
        const selected = displayModeOptions[displayModeCombo.currentIndex]
        displayModeSelected(selected
                            ? selected.displayMode
                            : DashboardDisplay.NetworkVertical)
    }

    contentItem: ComboBox {
        id: displayModeCombo
        model: dialog.displayModeOptions
        textRole: "label"
        width: parent.width
    }
}

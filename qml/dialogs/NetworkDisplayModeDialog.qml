import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: dialog

    property string initialVariant: "networkVertical"

    signal variantSelected(string variant)

    readonly property var variantOptions: [
        { label: qsTr("Vertical"), value: "networkVertical" },
        { label: qsTr("Horizontal"), value: "networkHorizontal" }
    ]

    function variantIndex(variant) {
        for (let i = 0; i < variantOptions.length; ++i) {
            if (variantOptions[i].value === variant)
                return i
        }
        return 0
    }

    modal: true
    focus: true
    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width - 32, 280)
    padding: 16
    title: qsTr("Display mode")
    standardButtons: Dialog.Ok | Dialog.Cancel

    onOpened: variantCombo.currentIndex = variantIndex(initialVariant)
    onAccepted: {
        const selected = variantOptions[variantCombo.currentIndex]
        variantSelected(selected ? selected.value : "networkVertical")
    }

    contentItem: ComboBox {
        id: variantCombo
        model: dialog.variantOptions
        textRole: "label"
        width: parent.width
    }
}

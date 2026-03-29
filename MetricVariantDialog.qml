import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    property string selectedMode: ""
    readonly property var variantOptions: [
        { label: "Default", value: "" },
        { label: "Segments", value: "segments" },
        { label: "Ring", value: "ring" },
        { label: "Linear", value: "linear" },
        { label: "Arc 180°", value: "arc180" }
    ]
    signal modeSelected(string mode)

    modal: true
    focus: true
    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width - 32, 320)
    padding: 16
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onOpened: optionsView.currentIndex = indexForMode(selectedMode)

    function indexForMode(mode) {
        for (let i = 0; i < variantOptions.length; ++i) {
            if (variantOptions[i].value === mode)
                return i
        }
        return 0
    }

    title: "Режим отображения"

    contentItem: ColumnLayout {
        spacing: 8

        ListView {
            id: optionsView
            Layout.fillWidth: true
            Layout.preferredHeight: 210
            clip: true
            model: root.variantOptions
            spacing: 6

            delegate: Rectangle {
                width: optionsView.width
                height: 38
                radius: 8
                color: ListView.isCurrentItem ? "#334155" : "#1E293B"
                border.width: 1
                border.color: ListView.isCurrentItem ? "#38BDF8" : "#475569"

                Text {
                    anchors.centerIn: parent
                    text: modelData.label
                    color: "#E2E8F0"
                    font.pixelSize: 14
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: optionsView.currentIndex = index
                    onDoubleClicked: applyButton.clicked()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Item { Layout.fillWidth: true }

            Button {
                text: "Отмена"
                onClicked: root.close()
            }

            Button {
                id: applyButton
                text: "Применить"
                onClicked: {
                    if (optionsView.currentIndex < 0)
                        optionsView.currentIndex = 0
                    const chosen = root.variantOptions[optionsView.currentIndex]
                    root.modeSelected(chosen ? chosen.value : "")
                    root.close()
                }
            }
        }
    }
}

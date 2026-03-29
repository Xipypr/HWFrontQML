import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    property var widgetsModel: null
    signal applyLayout(var widgets)

    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width - 32, 440)
    modal: true
    focus: true
    padding: 20
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    readonly property var widgetTemplates: [
        { label: "CPU", key: "cpu", title: "CPU", variant: "segments" },
        { label: "RAM", key: "ram", title: "RAM", variant: "ring" },
        { label: "GPU", key: "gpu", title: "GPU", variant: "linear" },
        { label: "Новый виджет", key: "custom", title: "Новый виджет", variant: "segments" }
    ]

    property int selectedTemplateIndex: 0

    ListModel {
        id: editableModel
    }

    function syncFromSource() {
        editableModel.clear()
        if (!root.widgetsModel)
            return

        for (let i = 0; i < root.widgetsModel.count; ++i) {
            const item = root.widgetsModel.get(i)
            editableModel.append({
                key: item.key,
                title: item.title,
                value: item.value,
                variant: item.variant
            })
        }
    }

    function exportWidgets() {
        const widgets = []
        for (let i = 0; i < editableModel.count; ++i) {
            widgets.push(editableModel.get(i))
        }
        return widgets
    }

    function nextCustomWidgetTitle() {
        let maxNumber = 0
        for (let i = 0; i < editableModel.count; ++i) {
            const item = editableModel.get(i)
            if (item.key !== "custom")
                continue

            const match = /^Новый виджет\s+(\d+)$/.exec(item.title)
            if (match && match.length > 1) {
                const number = parseInt(match[1], 10)
                if (!isNaN(number))
                    maxNumber = Math.max(maxNumber, number)
            }
        }
        return "Новый виджет " + (maxNumber + 1)
    }

    onOpened: syncFromSource()

    contentItem: ColumnLayout {
        spacing: 12

        Label {
            Layout.fillWidth: true
            text: "Компоновка виджетов"
            color: "#E2E8F0"
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
            model: editableModel

            delegate: Rectangle {
                width: widgetsList.width
                height: 48
                radius: 8
                color: "#1E293B"
                border.width: 1
                border.color: "#334155"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    Label {
                        Layout.fillWidth: true
                        text: title
                        color: "#E2E8F0"
                        elide: Text.ElideRight
                    }

                    ToolButton {
                        text: "↑"
                        enabled: index > 0
                        onClicked: editableModel.move(index, index - 1, 1)
                    }

                    ToolButton {
                        text: "↓"
                        enabled: index < editableModel.count - 1
                        onClicked: editableModel.move(index, index + 1, 1)
                    }

                    ToolButton {
                        text: "✕"
                        onClicked: editableModel.remove(index, 1)
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            ComboBox {
                id: addWidgetCombo
                Layout.fillWidth: true
                model: root.widgetTemplates
                textRole: "label"
                onCurrentIndexChanged: root.selectedTemplateIndex = currentIndex
            }

            Button {
                text: "Добавить"
                onClicked: {
                    const item = root.widgetTemplates[root.selectedTemplateIndex];
                    let title = item.title
                    if (item.key === "custom")
                        title = root.nextCustomWidgetTitle()
                    editableModel.append({ key: item.key, title: title, value: 0, variant: item.variant })
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
                text: "Принять"
                onClicked: {
                    root.applyLayout(root.exportWidgets())
                    root.close()
                }
            }
        }
    }
}

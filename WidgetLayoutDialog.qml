import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    property var widgetsModel: null

    signal moveWidgetUp(int index)
    signal moveWidgetDown(int index)
    signal deleteWidget(int index)
    signal addWidget(string widgetKey, string widgetTitle, string widgetVariant)

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
            model: root.widgetsModel

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
                        onClicked: root.moveWidgetUp(index)
                    }

                    ToolButton {
                        text: "↓"
                        enabled: root.widgetsModel && index < root.widgetsModel.count - 1
                        onClicked: root.moveWidgetDown(index)
                    }

                    ToolButton {
                        text: "✕"
                        onClicked: root.deleteWidget(index)
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
                    root.addWidget(item.key, item.title, item.variant)
                }
            }
        }
    }
}

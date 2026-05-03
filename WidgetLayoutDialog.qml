import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    property var widgetsModel: null

    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Math.min(parent.width - 32, 440)
    modal: true
    focus: true
    padding: 20
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    readonly property var widgetTemplates: [
        { label: "CPU", key: "cpu" },
        { label: "RAM", key: "ram" },
        { label: "GPU", key: "gpu" },
        { label: "HDD (180°)", key: "hdd" }
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
                        text: model.title
                        color: "#E2E8F0"
                        elide: Text.ElideRight
                    }

                    ToolButton {
                        text: "↑"
                        enabled: index > 0
                        onClicked: root.widgetsModel.moveWidget(index, index - 1)
                    }

                    ToolButton {
                        text: "↓"
                        enabled: root.widgetsModel && index < widgetsList.count - 1
                        onClicked: root.widgetsModel.moveWidget(index, index + 1)
                    }

                    ToolButton {
                        text: "✕"
                        onClicked: root.widgetsModel.removeWidget(model.widgetId)
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
                    const item = root.widgetTemplates[root.selectedTemplateIndex]
                    root.widgetsModel.addWidgetByType(item.key)
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
        }
    }
}

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Dialog {
    id: root

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min((parent ? parent.width : 560) - 32, 560)
    modal: true
    title: "Изменить компоновку виджетов"
    standardButtons: Dialog.Close
    padding: 16

    property var selectedWidgetsModel

    contentItem: ColumnLayout {
        spacing: 10

        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            color: "#CBD5E1"
            text: "Диалог компоновки виджетов вынесен в отдельный компонент для логического разделения."
        }

        Frame {
            Layout.fillWidth: true
            Layout.preferredHeight: 220
            background: Rectangle {
                radius: 6
                color: "#1E293B"
                border.color: "#334155"
            }

            ListView {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8
                clip: true
                model: root.selectedWidgetsModel

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 44
                    radius: 6
                    color: "#253247"

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        color: "#E2E8F0"
                        elide: Text.ElideRight
                        text: (model.title || "Виджет")
                    }
                }
            }
        }
    }
}

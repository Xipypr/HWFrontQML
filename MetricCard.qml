import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle {
    id: card

    property string title: "CPU"
    property int value: 0
    property string subtitle: "Realtime Load"

    readonly property color accentColor: value >= 90 ? "#EF4444" : value >= 70 ? "#F59E0B" : "#22C55E"
    readonly property string statusText: value >= 90 ? "CRITICAL" : value >= 70 ? "WARNING" : "NORMAL"

    radius: 16
    color: Qt.rgba(27 / 255, 36 / 255, 51 / 255, 0.86)
    border.width: 1
    border.color: Qt.rgba(100 / 255, 116 / 255, 139 / 255, 0.55)

    Rectangle {
        anchors.fill: parent
        anchors.margins: -1
        radius: parent.radius + 2
        color: "transparent"
        border.width: 2
        border.color: Qt.rgba(card.accentColor.r, card.accentColor.g, card.accentColor.b, 0.25)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: card.title
                color: "#BFDBFE"
                font.pixelSize: 14
                font.bold: true
            }

            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: card.accentColor

                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 0.25; duration: 450 }
                    NumberAnimation { from: 0.25; to: 1.0; duration: 450 }
                }
            }

            Item { Layout.fillWidth: true }

            Text {
                text: card.statusText
                color: card.accentColor
                font.pixelSize: 11
                font.bold: true
            }
        }

        Text {
            text: card.value + "%"
            color: "#F8FAFC"
            font.pixelSize: 42
            font.bold: true
        }

        RowLayout {
            spacing: 4
            Layout.fillWidth: true

            Repeater {
                model: 10

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 10
                    radius: 3
                    color: index < Math.ceil(card.value / 10) ? card.accentColor : "#334155"
                }
            }
        }

        Text {
            text: card.subtitle
            color: "#94A3B8"
            font.pixelSize: 11
        }
    }

    Behavior on value {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }
}

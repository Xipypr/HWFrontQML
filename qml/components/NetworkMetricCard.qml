pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import "../dialogs"
import DashboardDisplay 1.0

DashboardCard {
    id: card

    property string title: "N/A"
    property real downloadValue: 0
    property real uploadValue: 0
    property string unit: ""
    property int displayMode: DashboardDisplay.NetworkVertical

    signal displayModeSelected(int displayMode)

    readonly property color downloadColor: "#93C5FD"
    readonly property color uploadColor: "#C4B5FD"
    readonly property string valueFontFamily: "Consolas"
    accentColor: downloadColor

    function formattedValue(value) {
        return (Math.round(value * 10) / 10).toFixed(1)
    }

    function openVariantDialog() {
        displayModeDialog.initialDisplayMode = displayMode
        displayModeDialog.open()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Text {
            Layout.fillWidth: true
            text: card.title
            color: "#BFDBFE"
            font.pixelSize: 14
            font.bold: true
            elide: Text.ElideRight
        }

        Loader {
            Layout.fillWidth: true
            Layout.fillHeight: true
            sourceComponent: card.displayMode === DashboardDisplay.NetworkHorizontal
                             ? horizontalContent
                             : verticalContent
        }
    }

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: card.openVariantDialog()
    }

    TapHandler {
        acceptedButtons: Qt.LeftButton
        gesturePolicy: TapHandler.WithinBounds
        onLongPressed: card.openVariantDialog()
    }

    NetworkDisplayModeDialog {
        id: displayModeDialog
        onDisplayModeSelected: function(selectedDisplayMode) {
            card.displayModeSelected(selectedDisplayMode)
        }
    }

    Component {
        id: verticalContent

        ColumnLayout {
            spacing: 0

            NetworkMetricRow {
                Layout.fillWidth: true
                Layout.fillHeight: true
                arrow: "↓"
                label: qsTr("Download")
                valueText: card.formattedValue(card.downloadValue) + card.unit
                accentColor: card.downloadColor
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#334155"
            }

            NetworkMetricRow {
                Layout.fillWidth: true
                Layout.fillHeight: true
                arrow: "↑"
                label: qsTr("Upload")
                valueText: card.formattedValue(card.uploadValue) + card.unit
                accentColor: card.uploadColor
            }
        }
    }

    Component {
        id: horizontalContent

        RowLayout {
            spacing: 12

            NetworkMetricColumn {
                Layout.fillWidth: true
                Layout.fillHeight: true
                arrow: "↓"
                label: qsTr("Download")
                valueText: card.formattedValue(card.downloadValue) + card.unit
                accentColor: card.downloadColor
            }

            Rectangle {
                Layout.preferredWidth: 1
                Layout.fillHeight: true
                Layout.topMargin: 4
                Layout.bottomMargin: 4
                color: "#334155"
            }

            NetworkMetricColumn {
                Layout.fillWidth: true
                Layout.fillHeight: true
                arrow: "↑"
                label: qsTr("Upload")
                valueText: card.formattedValue(card.uploadValue) + card.unit
                accentColor: card.uploadColor
            }
        }
    }

    component NetworkMetricRow: RowLayout {
        required property string arrow
        required property string label
        required property string valueText
        required property color accentColor

        spacing: 8

        Text {
            text: parent.arrow
            color: parent.accentColor
            font.pixelSize: 20
            font.weight: Font.Black
        }

        Label {
            Layout.fillWidth: true
            text: parent.label
            color: parent.accentColor
            font.pixelSize: 14
            font.bold: true
        }

        Text {
            text: parent.valueText
            color: "#F8FAFC"
            font.pixelSize: 25
            font.family: card.valueFontFamily
            font.bold: true
        }
    }

    component NetworkMetricColumn: ColumnLayout {
        id: metricColumn

        required property string arrow
        required property string label
        required property string valueText
        required property color accentColor

        spacing: 6

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 6

            Text {
                text: metricColumn.arrow
                color: metricColumn.accentColor
                font.pixelSize: 20
                font.weight: Font.Black
            }

            Label {
                text: metricColumn.label
                color: metricColumn.accentColor
                font.pixelSize: 14
                font.bold: true
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: metricColumn.valueText
            color: "#F8FAFC"
            font.pixelSize: 24
            font.family: card.valueFontFamily
            font.bold: true
        }
    }

    Behavior on downloadValue {
        NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
    }

    Behavior on uploadValue {
        NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
    }
}

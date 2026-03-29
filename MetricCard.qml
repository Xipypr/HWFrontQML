import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle {
    id: card

    property string title: "N/A"
    property int value: 0
    // segments | ring | linear | arc180 (filled 180° segment)
    property string variant: "segments"

    readonly property int safeValue: Math.max(0, Math.min(100, value))
    readonly property color accentColor: safeValue >= 90 ? "#EF4444" : safeValue >= 70 ? "#F59E0B" : "#22C55E"
    readonly property string statusText: safeValue >= 90 ? "CRITICAL" : safeValue >= 70 ? "WARNING" : "NORMAL"
    readonly property int valueFontSize: variant === "ring" ? 34 : 42

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
        anchors.margins: variant === "arc180" ? 8 : 14
        spacing: variant === "ring" ? 8 : (variant === "arc180" ? 4 : 10)

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: card.title
                color: "#BFDBFE"
                font.pixelSize: 14
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
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

            Text {
                text: card.statusText
                color: card.accentColor
                font.pixelSize: 11
                font.bold: true
            }
        }

        Text {
            text: card.safeValue + "%"
            visible: card.variant !== "arc180"
            color: "#F8FAFC"
            font.pixelSize: card.valueFontSize
            font.bold: true
        }

        Loader {
            Layout.fillWidth: true
            Layout.fillHeight: card.variant === "arc180"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: card.variant === "arc180" ? 104 : (card.variant === "ring" ? 56 : 12)
            sourceComponent: card.variant === "ring"
                             ? ringViz
                             : (card.variant === "linear" ? linearViz
                                                          : (card.variant === "arc180" ? arc180Viz : segmentsViz))
        }

    }

    Component {
        id: segmentsViz

        RowLayout {
            spacing: 4
            Repeater {
                model: 10
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 10
                    radius: 3
                    color: index < Math.ceil(card.safeValue / 10) ? card.accentColor : "#334155"
                }
            }
        }
    }

    Component {
        id: linearViz

        Rectangle {
            implicitHeight: 12
            radius: 6
            color: "#334155"

            Rectangle {
                width: parent.width * card.safeValue / 100
                height: parent.height
                radius: 6
                color: card.accentColor
                Behavior on width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
            }
        }
    }

    Component {
        id: ringViz

        Item {
            implicitHeight: 56
            implicitWidth: 56

            Canvas {
                id: ringCanvas
                anchors.centerIn: parent
                width: 56
                height: 56
                antialiasing: true
                smooth: true
                renderTarget: Canvas.FramebufferObject

                property real progress: card.safeValue / 100

                onPaint: {
                    var ctx = getContext("2d");
                    var lineWidth = 8;
                    var radius = 20;
                    var centerX = width / 2;
                    var centerY = height / 2;

                    ctx.setTransform(1, 0, 0, 1, 0, 0);
                    ctx.clearRect(0, 0, width, height);
                    ctx.lineWidth = lineWidth;
                    ctx.lineCap = "round";

                    ctx.strokeStyle = "#334155";
                    ctx.beginPath();
                    ctx.arc(centerX, centerY, radius, 0, Math.PI * 2, false);
                    ctx.stroke();

                    ctx.strokeStyle = card.accentColor;
                    ctx.beginPath();
                    ctx.arc(centerX, centerY, radius, -Math.PI / 2, -Math.PI / 2 + Math.PI * 2 * progress, false);
                    ctx.stroke();
                }

                onProgressChanged: requestPaint()
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()

                Connections {
                    target: card
                    function onAccentColorChanged() { ringCanvas.requestPaint(); }
                }

                Component.onCompleted: requestPaint()
            }
        }
    }

    Component {
        id: arc180Viz

        Item {
            implicitHeight: 104
            implicitWidth: 150

            Canvas {
                id: arcCanvas
                anchors.fill: parent
                anchors.margins: 2
                antialiasing: true
                smooth: true
                renderTarget: Canvas.FramebufferObject

                property real progress: card.safeValue / 100

                onPaint: {
                    var ctx = getContext("2d");
                    var outerRadius = Math.max(10, Math.min(width / 2 - 6, height - 10));
                    var innerRadius = outerRadius * 0.62;
                    var centerX = width / 2;
                    var centerY = height - 6;
                    var startAngle = Math.PI;
                    var progressAngle = startAngle + Math.PI * progress;

                    ctx.setTransform(1, 0, 0, 1, 0, 0);
                    ctx.clearRect(0, 0, width, height);

                    // Base 180° segment (ring segment)
                    ctx.fillStyle = "#334155";
                    ctx.beginPath();
                    ctx.arc(centerX, centerY, outerRadius, startAngle, 0, false);
                    ctx.arc(centerX, centerY, innerRadius, 0, startAngle, true);
                    ctx.closePath();
                    ctx.fill();

                    // Active filled segment (0..180°)
                    if (progress > 0) {
                        ctx.fillStyle = card.accentColor;
                        ctx.beginPath();
                        ctx.arc(centerX, centerY, outerRadius, startAngle, progressAngle, false);
                        ctx.arc(centerX, centerY, innerRadius, progressAngle, startAngle, true);
                        ctx.closePath();
                        ctx.fill();
                    }
                }

                onProgressChanged: requestPaint()
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()

                Connections {
                    target: card
                    function onAccentColorChanged() { arcCanvas.requestPaint(); }
                }

                Component.onCompleted: requestPaint()
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 18
                text: card.safeValue + "%"
                color: "#F8FAFC"
                font.pixelSize: 24
                font.bold: true
            }
        }
    }

    Behavior on value {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutCubic
        }
    }
}

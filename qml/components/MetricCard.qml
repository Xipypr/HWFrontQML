import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle {
    id: card

    property string title: "N/A"
    property real value: 0
    property string unit: ""
    property string metricId: ""
    property bool showProgressBar: true
    // segments | ring | linear | arc180
    property string variant: "segments"
    signal variantSelected(string mode)
    readonly property var variantOptions: [
        { label: "Segments", value: "segments" },
        { label: "Ring", value: "ring" },
        { label: "Linear", value: "linear" },
        { label: "Arc 180°", value: "arc180" }
    ]

    readonly property int warningThreshold: 70
    readonly property int criticalThreshold: 90
    readonly property color valueOnlyAccentColor: "#38BDF8"
    readonly property color normalAccentColor: "#22C55E"
    readonly property color warningAccentColor: "#F59E0B"
    readonly property color criticalAccentColor: "#EF4444"
    readonly property int safeValue: Math.max(0, Math.min(100, value))
    readonly property bool useStatusColor: showProgressBar && metricId !== "batteryLevel"
    readonly property bool isCriticalValue: useStatusColor && safeValue >= criticalThreshold
    readonly property bool isWarningValue: useStatusColor && safeValue >= warningThreshold && !isCriticalValue
    readonly property color accentColor: resolveAccentColor()
    readonly property string statusText: resolveStatusText()
    readonly property int valueFontSize: 42
    readonly property int compactValueFontSize: 24
    readonly property string valueFontFamily: "Consolas"
    readonly property string valueWidthSampleText: (showProgressBar ? "100" : "0000.0") + unit
    readonly property real valueTextWidth: valueWidthProbe.implicitWidth
    readonly property real compactValueTextWidth: compactValueWidthProbe.implicitWidth

    function formattedValue() {
        const rounded = Math.round(value * 10) / 10
        return showProgressBar ? Math.round(rounded).toString() : rounded.toFixed(1)
    }

    function resolveAccentColor() {
        if (!showProgressBar)
            return valueOnlyAccentColor

        if (!useStatusColor)
            return normalAccentColor

        if (isCriticalValue)
            return criticalAccentColor

        if (isWarningValue)
            return warningAccentColor

        return normalAccentColor
    }

    function resolveStatusText() {
        if (!useStatusColor)
            return ""

        if (isCriticalValue)
            return "CRITICAL"

        if (isWarningValue)
            return "WARNING"

        return "NORMAL"
    }

    function resolveVizComponent() {
        switch (variant) {
        case "ring":
            return ringViz
        case "linear":
            return linearViz
        case "arc180":
            return arc180Viz
        default:
            return segmentsViz
        }
    }

    function variantIndex(mode) {
        for (let i = 0; i < variantOptions.length; ++i) {
            if (variantOptions[i].value === mode)
                return i
        }
        return 0
    }

    function openVariantDialog() {
        if (!variantDialogLoader.active)
            variantDialogLoader.active = true

        if (variantDialogLoader.status === Loader.Ready && variantDialogLoader.item) {
            variantDialogLoader.item.initialIndex = variantIndex(variant)
            variantDialogLoader.item.open()
            return
        }

        variantDialogLoader.pendingOpen = true
    }

    radius: 16
    color: Qt.rgba(27 / 255, 36 / 255, 51 / 255, 0.86)
    border.width: 1
    border.color: Qt.rgba(100 / 255, 116 / 255, 139 / 255, 0.55)

    Text {
        id: valueWidthProbe
        visible: false
        text: card.valueWidthSampleText
        font.family: card.valueFontFamily
        font.pixelSize: card.valueFontSize
        font.bold: true
    }

    Text {
        id: compactValueWidthProbe
        visible: false
        text: card.valueWidthSampleText
        font.family: card.valueFontFamily
        font.pixelSize: card.compactValueFontSize
        font.bold: true
    }

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
                visible: card.useStatusColor
                color: card.accentColor

                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 0.25; duration: 450 }
                    NumberAnimation { from: 0.25; to: 1.0; duration: 450 }
                }
            }

            Text {
                visible: card.useStatusColor
                text: card.statusText
                color: card.accentColor
                font.pixelSize: 11
                font.bold: true
            }
        }

        Text {
            text: card.formattedValue() + card.unit
            visible: !card.showProgressBar || (card.variant !== "arc180" && card.variant !== "ring")
            color: "#F8FAFC"
            font.pixelSize: card.valueFontSize
            font.family: card.valueFontFamily
            font.bold: true
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: card.valueTextWidth
            Layout.maximumWidth: parent.width
        }

        Loader {
            Layout.fillWidth: true
            Layout.fillHeight: card.variant === "arc180"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: card.variant === "arc180" ? 104 : (card.variant === "ring" ? 86 : 12)
            visible: card.showProgressBar
            sourceComponent: card.resolveVizComponent()
        }

    }

    TapHandler {
        enabled: card.showProgressBar
        acceptedButtons: Qt.RightButton
        onTapped: card.openVariantDialog()
    }

    TapHandler {
        enabled: card.showProgressBar
        acceptedButtons: Qt.LeftButton
        gesturePolicy: TapHandler.WithinBounds
        onLongPressed: card.openVariantDialog()
    }

    Component {
        id: variantDialogComponent
        Dialog {
            property int initialIndex: 0

            modal: true
            focus: true
            parent: Overlay.overlay
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: Math.min(parent.width - 32, 280)
            padding: 16
            title: "Режим отображения"
            standardButtons: Dialog.Ok | Dialog.Cancel

            onOpened: variantDialogCombo.currentIndex = initialIndex

            onAccepted: {
                const selected = card.variantOptions[variantDialogCombo.currentIndex]
                card.variantSelected(selected ? selected.value : "segments")
            }

            contentItem: ComboBox {
                id: variantDialogCombo
                model: card.variantOptions
                textRole: "label"
                width: parent.width
            }
        }
    }

    Loader {
        id: variantDialogLoader
        active: false
        sourceComponent: variantDialogComponent
        property bool pendingOpen: false

        onLoaded: {
            if (pendingOpen && item) {
                pendingOpen = false
                item.initialIndex = card.variantIndex(card.variant)
                item.open()
            }
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
            implicitHeight: 86
            implicitWidth: 86

            Canvas {
                id: ringCanvas
                anchors.centerIn: parent
                width: 86
                height: 86
                antialiasing: true
                smooth: true
                renderTarget: Canvas.FramebufferObject

                property real progress: card.safeValue / 100

                onPaint: {
                    var ctx = getContext("2d");
                    var lineWidth = 11;
                    var radius = (Math.min(width, height) - lineWidth) / 2 - 2;
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

            Text {
                anchors.centerIn: parent
                text: card.formattedValue() + card.unit
                color: "#F8FAFC"
                width: card.compactValueTextWidth
                horizontalAlignment: Text.AlignHCenter
                font.family: card.valueFontFamily
                font.pixelSize: card.compactValueFontSize
                font.bold: true
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
                    var baseOuterRadius = Math.max(10, Math.min(width / 2 - 6, height - 10));
                    var outerRadius = baseOuterRadius * 0.8;
                    var innerRadius = outerRadius * 0.72;
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
                anchors.verticalCenterOffset: 28
                text: card.formattedValue() + card.unit
                color: "#F8FAFC"
                width: card.compactValueTextWidth
                horizontalAlignment: Text.AlignHCenter
                font.family: card.valueFontFamily
                font.pixelSize: card.compactValueFontSize
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

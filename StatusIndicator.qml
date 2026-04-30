import QtQuick 2.15

Rectangle {
    id: root

    property string sessionState: "IDLE"

    readonly property color indicatorColor: {
        switch (sessionState) {
        case "CONNECTED":
            return "#22C55E"
        case "CONNECTING":
            return "#3B82F6"
        case "RECONNECTING":
            return "#EAB308"
        case "ERROR":
            return "#EF4444"
        case "IDLE":
        case "DISCONNECTED":
        default:
            return "#9CA3AF"
        }
    }

    readonly property bool pulse: sessionState === "CONNECTED"
                                 || sessionState === "CONNECTING"
                                 || sessionState === "RECONNECTING"
                                 || sessionState === "ERROR"

    width: 10
    height: 10
    radius: width / 2
    color: indicatorColor
    opacity: 1.0

    SequentialAnimation on opacity {
        running: root.pulse
        loops: Animation.Infinite
        NumberAnimation { to: 0.35; duration: 700; easing.type: Easing.InOutQuad }
        NumberAnimation { to: 1.0; duration: 700; easing.type: Easing.InOutQuad }
    }
}

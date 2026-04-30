import QtQuick 2.15

Rectangle {
    id: root

    property int sessionState: SessionState.IDLE

    readonly property color indicatorColor: {
        switch (sessionState) {
        case SessionState.CONNECTED:
            return "#22C55E"
        case SessionState.CONNECTING:
            return "#3B82F6"
        case SessionState.RECONNECTING:
            return "#EAB308"
        case SessionState.ERROR:
            return "#EF4444"
        case SessionState.IDLE:
        case SessionState.DISCONNECTED:
        default:
            return "#9CA3AF"
        }
    }

    readonly property bool pulse: sessionState === SessionState.CONNECTED
                                 || sessionState === SessionState.CONNECTING
                                 || sessionState === SessionState.RECONNECTING
                                 || sessionState === SessionState.ERROR

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

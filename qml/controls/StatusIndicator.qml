import QtQuick 2.15
import SessionState 1.0
import ".."

Rectangle {
    id: root

    property int sessionState: SessionState.IDLE

    readonly property color indicatorColor: {
        switch (sessionState) {
        case SessionState.CONNECTED:
            return Theme.normal
        case SessionState.CONNECTING:
            return Theme.connecting
        case SessionState.RECONNECTING:
            return Theme.reconnecting
        case SessionState.ERROR:
            return Theme.critical
        case SessionState.IDLE:
        case SessionState.DISCONNECTED:
        default:
            return Theme.inactive
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

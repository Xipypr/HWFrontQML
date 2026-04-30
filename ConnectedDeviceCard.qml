import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root

    property bool connected: false
    property string deviceName: ""
    property string sessionState: "IDLE"
    property alias inputText: hostInput.text
    readonly property bool acceptableInput: hostInput.acceptableInput
    signal deviceLabelClicked()
    signal deviceLabelRightClicked()

    implicitHeight: hostInput.implicitHeight

    TextField {
        id: hostInput
        anchors.fill: parent
        focus: true
        visible: !root.connected
        verticalAlignment: TextInput.AlignVCenter
        validator: RegularExpressionValidator {
            regularExpression: /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
        }
        Component.onCompleted: hostInput.forceActiveFocus()
    }

    Rectangle {
        id: connectedCard
        anchors.fill: parent
        visible: root.connected
        radius: 6
        color: "#2E3B52"
        border.color: "#4E5D78"
        border.width: 1
        clip: true

        Text {
            id: deviceNameLabel
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 28
            verticalAlignment: Text.AlignVCenter
            color: "#FFFFFF"
            elide: Text.ElideRight
            text: root.deviceName
        }

        StatusIndicator {
            id: statusDot
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            sessionState: root.sessionState
        }

        MouseArea {
            id: cardHoverArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: deviceNameLabel.truncated ? Qt.PointingHandCursor : Qt.ArrowCursor
            onPressed: pressAnimation.start()
            onReleased: releaseAnimation.start()
            onCanceled: releaseAnimation.start()
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: function(mouse) {
                if (mouse.button === Qt.RightButton)
                    root.deviceLabelRightClicked()
                else
                    root.deviceLabelClicked()
            }
        }

        ToolTip.visible: cardHoverArea.containsMouse && deviceNameLabel.truncated
        ToolTip.delay: 300
        ToolTip.text: root.deviceName

        NumberAnimation {
            id: pressAnimation
            target: connectedCard
            property: "scale"
            to: 0.97
            duration: 90
            easing.type: Easing.OutQuad
        }

        NumberAnimation {
            id: releaseAnimation
            target: connectedCard
            property: "scale"
            to: 1.0
            duration: 120
            easing.type: Easing.OutQuad
        }
    }
}

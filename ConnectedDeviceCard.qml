import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root

    property bool connected: false
    property string deviceName: ""
    property alias inputText: hostInput.text
    readonly property bool acceptableInput: hostInput.acceptableInput

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
            anchors.rightMargin: 12
            verticalAlignment: Text.AlignVCenter
            color: "#FFFFFF"
            elide: Text.ElideRight
            text: root.deviceName
        }

        MouseArea {
            id: cardHoverArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: deviceNameLabel.truncated ? Qt.PointingHandCursor : Qt.ArrowCursor
        }

        ToolTip.visible: cardHoverArea.containsMouse && deviceNameLabel.truncated
        ToolTip.delay: 300
        ToolTip.text: root.deviceName
    }
}

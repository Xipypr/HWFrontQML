import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root

    property string deviceName: ""

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

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Item {
    id: root
    property string deviceName: ""
    property color indicatorColor: "#22C55E"

    Layout.fillWidth: true
    implicitHeight: Math.max(nameLabel.implicitHeight, statusDot.height)

    RowLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            id: nameLabel
            Layout.fillWidth: true
            text: root.deviceName !== "" && root.deviceName !== "Unknown device"
                  ? root.deviceName
                  : qsTr("Unknown device")
            color: "#E2E8F0"
            font.pixelSize: 16
            font.bold: true
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }

        Rectangle {
            id: statusDot
            width: 10
            height: 10
            radius: width / 2
            color: root.indicatorColor

            SequentialAnimation on opacity {
                running: true
                loops: Animation.Infinite
                NumberAnimation { to: 0.35; duration: 700; easing.type: Easing.InOutQuad }
                NumberAnimation { to: 1.0; duration: 700; easing.type: Easing.InOutQuad }
            }
        }
    }
}

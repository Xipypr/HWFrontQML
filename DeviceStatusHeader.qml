import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

ToolBar {
    id: root
    property string deviceName: ""
    property color indicatorColor: "#22C55E"
    property int horizontalPadding: 12
    property int verticalPadding: 8
    signal clicked()

    Layout.fillWidth: true
    implicitHeight: 56

    background: Rectangle {
        color: headerTap.pressed ? "#223150" : "#17233A"

        Behavior on color {
            ColorAnimation { duration: 120 }
        }
    }

    TapHandler {
        id: headerTap
        onTapped: root.clicked()
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: horizontalPadding
        anchors.rightMargin: horizontalPadding
        anchors.topMargin: verticalPadding
        anchors.bottomMargin: verticalPadding
        spacing: 12

        Label {
            id: nameLabel
            Layout.fillWidth: true
            text: root.deviceName
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

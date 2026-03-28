import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

ToolBar {
    id: root
    property string headerText: ""
    property bool showIndicator: true
    property color indicatorColor: "#22C55E"
    property color pressedBackgroundColor: "#223150"
    signal clicked()

    implicitHeight: 56
    horizontalPadding: 16
    verticalPadding: 8

    background: Rectangle {
        color: headerTap.pressed ? root.pressedBackgroundColor : "#17233A"

        Behavior on color {
            ColorAnimation { duration: 120 }
        }
    }

    contentItem: RowLayout {
        spacing: 12

        Label {
            id: nameLabel
            Layout.fillWidth: true
            text: root.headerText
            color: "#E2E8F0"
            font.pixelSize: 16
            font.bold: true
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }

        Rectangle {
            id: statusDot
            visible: root.showIndicator
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

    TapHandler {
        id: headerTap
        onTapped: root.clicked()
    }
}

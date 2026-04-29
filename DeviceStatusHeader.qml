import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

ToolBar {
    id: root
    property string headerText: ""
    property bool showIndicator: true
    property color indicatorColor: "#22C55E"
    property color backgroundColor: "#17233A"
    property color pressedBackgroundColor: "#223150"
    property bool showHomeButton: false
    signal clicked()
    signal homeClicked()

    implicitHeight: 56
    horizontalPadding: 16
    verticalPadding: 0

    background: Rectangle {
        color: headerTap.pressed ? root.pressedBackgroundColor : root.backgroundColor

        Behavior on color {
            ColorAnimation { duration: 120 }
        }
    }

    contentItem: RowLayout {
        spacing: 12

        ToolButton {
            visible: root.showHomeButton
            id: homeButton
            text: "⌂"
            Layout.preferredWidth: root.height
            Layout.preferredHeight: root.height
            Layout.alignment: Qt.AlignVCenter
            font.pixelSize: 24
            onClicked: root.homeClicked()
            background: Rectangle {
                radius: 8
                color: homeButton.down ? "#31405F" : "transparent"
            }
            contentItem: Label {
                text: homeButton.text
                color: "#E2E8F0"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font: homeButton.font
            }
        }

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

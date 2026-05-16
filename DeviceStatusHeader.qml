import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import SessionState 1.0

ToolBar {
    id: root
    property string headerText: ""
    property bool showIndicator: true
    property int sessionState: SessionState.IDLE
    property color backgroundColor: "#17233A"
    property color pressedBackgroundColor: "#223150"
    property bool showHomeButton: false
    property string headerIconSource: ""
    signal clicked()
    signal homeClicked()

    implicitHeight: 56
    horizontalPadding: 16
    verticalPadding: 8

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
            icon.source: "qrc:/icons/home.png"
            Layout.preferredWidth: root.height - root.verticalPadding * 2
            Layout.preferredHeight: root.height - root.verticalPadding * 2
            Layout.alignment: Qt.AlignVCenter
            font.pixelSize: 24
            onClicked: root.homeClicked()
            background: Rectangle {
                radius: 8
                color: homeButton.down ? "#2D3F61" : "#223150"
                border.color: "#3A4B6B"
                border.width: 1
            }
        }

        ToolButton {
            id: headerIcon
            visible: !root.showHomeButton
            padding: 0
            focusPolicy: Qt.NoFocus
            icon.source: root.headerIconSource
            icon.width: 24
            icon.height: 24
            icon.color: "#E2E8F0"
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24
            Layout.alignment: Qt.AlignVCenter
            onClicked: root.clicked()
            background: Item {}
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

        StatusIndicator {
            id: statusDot
            visible: root.showIndicator
            sessionState: root.sessionState
        }
    }

    TapHandler {
        id: headerTap
        onTapped: root.clicked()
    }
}

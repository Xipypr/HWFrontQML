import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

ToolBar {
    id: root
    property string title: ""
    property bool showIndicator: true
    property color indicatorColor: "#22C55E"
    property color backgroundColor: "#17233A"
    property color pressedBackgroundColor: "#223150"
    property int headerHeight: 56
    signal clicked()

    implicitHeight: headerHeight
    horizontalPadding: 16
    verticalPadding: 8

    background: Rectangle {
        color: headerTap.pressed ? root.pressedBackgroundColor : root.backgroundColor

        Behavior on color {
            ColorAnimation { duration: 120 }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: root.horizontalPadding
        anchors.rightMargin: root.horizontalPadding
        anchors.topMargin: root.verticalPadding
        anchors.bottomMargin: root.verticalPadding
        spacing: 12

        Label {
            id: nameLabel
            Layout.fillWidth: true
            text: root.title
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

    MouseArea {
        id: headerTap
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}

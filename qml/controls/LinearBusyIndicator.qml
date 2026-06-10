import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Control {
    id: control

    property bool running: false
    readonly property color accentColor: Material.accent

    implicitWidth: 40
    implicitHeight: 8

    contentItem: Item {
        visible: control.running

        Rectangle {
            anchors.fill: parent
            radius: height / 2
            color: Qt.rgba(control.accentColor.r, control.accentColor.g, control.accentColor.b, 0.35)
        }

        Rectangle {
            id: movingBar
            width: parent.width * 0.35
            height: parent.height
            radius: height / 2
            color: control.accentColor
            x: 0
        }

        SequentialAnimation {
            running: control.running
            loops: Animation.Infinite

            NumberAnimation {
                target: movingBar
                property: "x"
                from: 0
                to: control.width - movingBar.width
                duration: 700
                easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                target: movingBar
                property: "x"
                from: control.width - movingBar.width
                to: 0
                duration: 700
                easing.type: Easing.InOutQuad
            }
        }
    }
}

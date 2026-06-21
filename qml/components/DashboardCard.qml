import QtQuick 2.15

Rectangle {
    id: cardFrame

    property color accentColor: "#38BDF8"

    radius: 16
    color: Qt.rgba(27 / 255, 36 / 255, 51 / 255, 0.86)
    border.width: 1
    border.color: Qt.rgba(100 / 255, 116 / 255, 139 / 255, 0.55)

    Rectangle {
        anchors.fill: parent
        anchors.margins: -1
        radius: parent.radius + 2
        color: "transparent"
        border.width: 2
        border.color: Qt.rgba(cardFrame.accentColor.r,
                              cardFrame.accentColor.g,
                              cardFrame.accentColor.b,
                              0.25)
    }
}

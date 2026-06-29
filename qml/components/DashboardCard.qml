import QtQuick 2.15
import ".."

Rectangle {
    id: cardFrame

    property color accentColor: Theme.valueOnly

    radius: 16
    color: Theme.cardSurface
    border.width: 1
    border.color: Theme.border

    Rectangle {
        anchors.fill: parent
        anchors.margins: -1
        radius: parent.radius + 2
        color: "transparent"
        border.width: 2
        border.color: Qt.rgba(cardFrame.accentColor.r,
                              cardFrame.accentColor.g,
                              cardFrame.accentColor.b,
                              Theme.cardAccentOpacity)
    }
}

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import ".."

Rectangle {
    id: titleBar

    required property Window window

    height: 35
    color: Theme.titleBarBackground

    Image {
        id: applicationIcon
        anchors.left: parent.left
        anchors.leftMargin: 9
        anchors.verticalCenter: parent.verticalCenter
        width: 18
        height: 18
        source: "qrc:/icons/app/icon_512.png"
        sourceSize: Qt.size(width, height)
        fillMode: Image.PreserveAspectFit
    }

    Label {
        anchors.left: applicationIcon.right
        anchors.leftMargin: 8
        anchors.right: windowButtons.left
        anchors.rightMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        text: titleBar.window.title
        color: Theme.titleBarText
        font.pixelSize: 13
        elide: Text.ElideRight
    }

    Item {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: windowButtons.left

        DragHandler {
            target: null
            acceptedButtons: Qt.LeftButton
            onActiveChanged: {
                if (active)
                    titleBar.window.startSystemMove()
            }
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton
            onDoubleTapped: titleBar.toggleMaximized()
        }
    }

    Row {
        id: windowButtons
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        CaptionButton {
            symbol: 0
            toolTipText: qsTr("Minimize")
            onClicked: titleBar.window.showMinimized()
        }

        CaptionButton {
            symbol: titleBar.maximizeSymbol()
            toolTipText: titleBar.maximizeToolTip()
            onClicked: titleBar.toggleMaximized()
        }

        CaptionButton {
            symbol: 3
            closeButton: true
            toolTipText: qsTr("Close")
            onClicked: titleBar.window.close()
        }
    }

    function toggleMaximized() {
        if (window.visibility === Window.Maximized)
            window.showNormal()
        else
            window.showMaximized()
    }

    function maximizeSymbol() {
        if (window.visibility === Window.Maximized)
            return 2

        return 1
    }

    function maximizeToolTip() {
        if (window.visibility === Window.Maximized)
            return qsTr("Restore")

        return qsTr("Maximize")
    }

    component CaptionButton: AbstractButton {
        id: captionButton

        required property int symbol
        property bool closeButton: false
        property string toolTipText: ""

        width: 42
        height: titleBar.height
        hoverEnabled: true

        ToolTip.visible: hovered
        ToolTip.delay: 700
        ToolTip.text: toolTipText

        function backgroundColor() {
            if (down) {
                if (closeButton)
                    return Theme.titleBarClosePressed

                return Theme.titleBarButtonPressed
            }

            if (hovered) {
                if (closeButton)
                    return Theme.titleBarCloseHover

                return Theme.titleBarButtonHover
            }

            return "transparent"
        }

        function iconColor() {
            if (closeButton && hovered)
                return Theme.titleBarCloseForeground

            return Theme.titleBarText
        }

        background: Rectangle {
            color: captionButton.backgroundColor()
        }

        contentItem: Item {
            Canvas {
                id: buttonIcon
                anchors.centerIn: parent
                width: 16
                height: 16

                onPaint: {
                    const context = getContext("2d")
                    context.clearRect(0, 0, width, height)
                    context.strokeStyle = captionButton.iconColor()
                    context.lineWidth = 1

                    if (captionButton.symbol === 0) {
                        context.beginPath()
                        context.moveTo(4, 11.5)
                        context.lineTo(12, 11.5)
                        context.stroke()
                    } else if (captionButton.symbol === 1) {
                        context.strokeRect(4.5, 4.5, 7, 7)
                    } else if (captionButton.symbol === 2) {
                        context.strokeRect(3.5, 5.5, 7, 7)
                        context.beginPath()
                        context.moveTo(5.5, 5.5)
                        context.lineTo(5.5, 3.5)
                        context.lineTo(12.5, 3.5)
                        context.lineTo(12.5, 10.5)
                        context.lineTo(10.5, 10.5)
                        context.stroke()
                    } else {
                        context.beginPath()
                        context.moveTo(4, 4)
                        context.lineTo(12, 12)
                        context.moveTo(12, 4)
                        context.lineTo(4, 12)
                        context.stroke()
                    }
                }

                Connections {
                    target: captionButton
                    function onHoveredChanged() { buttonIcon.requestPaint() }
                    function onSymbolChanged() { buttonIcon.requestPaint() }
                }

                Connections {
                    target: Theme
                    function onTitleBarTextChanged() { buttonIcon.requestPaint() }
                    function onTitleBarCloseForegroundChanged() { buttonIcon.requestPaint() }
                }
            }
        }
    }
}

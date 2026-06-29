pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Window 2.15

Item {
    id: frame

    required property Window window

    function cursorForEdge(edge) {
        if (edge === Qt.LeftEdge || edge === Qt.RightEdge)
            return Qt.SizeHorCursor

        if (edge === Qt.TopEdge || edge === Qt.BottomEdge)
            return Qt.SizeVerCursor

        if (edge === (Qt.TopEdge | Qt.LeftEdge)
                || edge === (Qt.BottomEdge | Qt.RightEdge)) {
            return Qt.SizeFDiagCursor
        }

        return Qt.SizeBDiagCursor
    }

    component ResizeHandle: MouseArea {
        required property int edge

        enabled: frame.window.visibility !== Window.Maximized
        acceptedButtons: Qt.LeftButton
        cursorShape: frame.cursorForEdge(edge)
        onPressed: frame.window.startSystemResize(edge)
    }

    ResizeHandle {
        edge: Qt.TopEdge
        x: 6
        width: parent.width - 12
        height: 6
        anchors.top: parent.top
    }

    ResizeHandle {
        edge: Qt.BottomEdge
        x: 6
        width: parent.width - 12
        height: 6
        anchors.bottom: parent.bottom
    }

    ResizeHandle {
        edge: Qt.LeftEdge
        y: 6
        width: 6
        height: parent.height - 12
        anchors.left: parent.left
    }

    ResizeHandle {
        edge: Qt.RightEdge
        y: 6
        width: 6
        height: parent.height - 12
        anchors.right: parent.right
    }

    ResizeHandle {
        edge: Qt.TopEdge | Qt.LeftEdge
        width: 6
        height: 6
        anchors.top: parent.top
        anchors.left: parent.left
    }

    ResizeHandle {
        edge: Qt.TopEdge | Qt.RightEdge
        width: 6
        height: 6
        anchors.top: parent.top
        anchors.right: parent.right
    }

    ResizeHandle {
        edge: Qt.BottomEdge | Qt.LeftEdge
        width: 6
        height: 6
        anchors.bottom: parent.bottom
        anchors.left: parent.left
    }

    ResizeHandle {
        edge: Qt.BottomEdge | Qt.RightEdge
        width: 6
        height: 6
        anchors.bottom: parent.bottom
        anchors.right: parent.right
    }
}

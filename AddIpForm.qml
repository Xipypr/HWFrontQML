import QtQuick 2.0
import QtQuick.Controls 2.15

Item {
    id: root
    Text {
        id: text
        text: qsTr("Add IP")
        anchors.verticalCenter: parent.Center
    }

    TextEdit{
        id: textEdit
        anchors.verticalCenter: parent.Center
        anchors.horizontalCenter: text.right
    }
}

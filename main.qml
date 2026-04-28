import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hardware Monitor")
    id: root

    SwipeView {
        id: swipeView
        anchors.fill: parent

        PageAuthForm {
            id: pageAuth
        }

        Repeater {
            model: sessionManager.sessionsModel

            delegate: PageDevicesInfo {
                sessionId: model.sessionId
            }
        }
    }
}

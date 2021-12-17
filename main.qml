import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.12

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Tabs")
    id: root

    Connections{
        target: connector

        function onTestSignal() {
            console.log("ПРИГШЛО");
        }
    }

    header: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Page 1")
        }
        TabButton {
            text: qsTr("Page 2")
        }
    }


    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        PageAuthForm {
        }

        Page {
            Text {
                text: qsTr("text")
            }
        }
    }
}

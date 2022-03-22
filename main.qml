import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import "progbarstyles"

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Tabs")
    id: root

    Loader {
            id: pagesLoader

            anchors.fill: parent
            asynchronous: true
//            source: "Background_1.qml"

//            onStatusChanged: console.log("status", status,  "item", item)
//            onLoaded: item.color = "green"
        }

    Connections{
        target: core
    }

    header: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Page 1")
        }
        TabButton {
            text: qsTr(pageDeviceInfo.destop_name)
        }
    }


    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        PageAuthForm {
        }

        PageDevicesInfo {
            id: pageDeviceInfo
        }
    }
}

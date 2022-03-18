import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import "progbarstyles"

Page {
    id: root

    property var objectsArray: []

    ColumnLayout {
        spacing: 0
        anchors.fill: parent
        RowLayout {
            spacing: 0
            Layout.preferredHeight: 20
            Cir_1 {
                id: test
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                titleText: "CPU"
                titleColor: "red"
                p_cur: 45

            }
            Cir_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                titleText: "GPU"
                titleColor: "green"
                p_cur: 68
            }
        }

        RowLayout {
            spacing: 0
            Layout.preferredHeight: 20
            Ver_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                p_cur: 80
            }
            Ver_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
            Ver_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                p_cur: 80
            }
            Ver_1 {
                implicitWidth: 20
                Layout.fillHeight: true
                Layout.fillWidth: true
                p_cur: 12
            }
        }

        Connections{
            target: core

            function onDeviceCreated() {
//                test.titleText = "AAAA"
                objectsArray.push(core.getDevice)
            }
        }

        /*
            RowLayout {
                spacing: 0
                Layout.preferredHeight: 40
                Rectangle {
                    implicitWidth: 40
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "darkBlue"
                }
                Rectangle {
                    implicitWidth: 20
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "blue"
                }
                Rectangle {
                    implicitWidth: 40
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "lightBlue"
                }
                Rectangle {
                    implicitWidth: 40
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "darkBlue"
                }
                Rectangle {
                    implicitWidth: 40
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "blue"
                }

                Rectangle {
                    implicitWidth: 40
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "lightBlue"
                }
                Rectangle {
                    implicitWidth: 40
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "blue"
                }
            }*/
    }
}

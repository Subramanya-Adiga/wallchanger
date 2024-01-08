import QtQuick
import QtQuick.Controls
import Qt.labs.platform as Platform

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true

    Platform.MenuBar {
        Platform.Menu {
            title: qsTr("&File")

            Platform.MenuItem {
                text: qsTr("&Open")
            }
            Platform.MenuItem {
                text: qsTr("&Save As...")
            }
            Platform.MenuItem {
                text: qsTr("&Quit")
                onTriggered: close()
            }
        }
    }

    property string list_selected_image: ""

    ListView {
        id: listView
        width: 256
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        highlightResizeVelocity: 600
        highlightMoveDuration: 1
        antialiasing: true
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        model:_mymodel

        delegate:Rectangle{
            id:delegateroot
            width: listView.width
            height: childrenRect.height
            border.color: "black"
            border.width:1

            Image{
                id:modelimage
                source:"file:/"+model.path+"/"+model.name
                anchors.horizontalCenter: delegateroot.horizontalCenter
                width:128
                height:128
                fillMode:Image.PreserveAspectFit

            }

          Text{
           id:modeltext
           text:model.name
           anchors.top:modelimage.bottom
           anchors.horizontalCenter: modelimage.horizontalCenter
           anchors.topMargin: 5
          }

          MouseArea{
              anchors.fill: parent
              acceptedButtons: Qt.LeftButton
              onDoubleClicked: {
                  list_selected_image = modelimage.source
              }
          }

        }
    }

    ScrollView {
        id: scrollView
        x: 262
        width: 1018
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left:listView.right
        anchors.bottomMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 0
        Image{
            id:scrollimage
            anchors.fill: parent
            source:list_selected_image
        }
    }
}

import QtQuick 2.0
import QtWinExtras 1.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1

Rectangle {
     id: appRect
     objectName: "mainRect"
     width: 500; height: 550
     color: "white"

     signal qmlSignal(int x, int y)//string msg)

     ListModel {
         id: appModel
         ListElement { name: ""}
         ListElement { name: "A"}
         ListElement { name: "B"}
         ListElement { name: "C"}
         ListElement { name: "D"}
         ListElement { name: "E"}
         ListElement { name: "F"}
         ListElement { name: "G"}
         ListElement { name: "H"}
         ListElement { name: ""}

         ListElement { name: "8"}
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darkpawn.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: "8"}

         ListElement { name: "7"}
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: "7"}

         ListElement { name: "6"}
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: "6"}

         ListElement { name: "5"}
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: "5"}

         ListElement { name: "4"}
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: "4"}

         ListElement { name: "3"}
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: "3"}

         ListElement { name: "2"}
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: "2"}

         ListElement { name: "1"}
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: ""; icon: "lightsquare.gif" }
         ListElement { name: ""; icon: "darksquare.gif" }
         ListElement { name: "1"}

         ListElement { name: ""}
         ListElement { name: "A"}
         ListElement { name: "B"}
         ListElement { name: "C"}
         ListElement { name: "D"}
         ListElement { name: "E"}
         ListElement { name: "F"}
         ListElement { name: "G"}
         ListElement { name: "H"}
         ListElement { name: ""}
     }

     Component {
         id: appDelegate

         Item {
             width: 50; height: 50

             Image {
                 id: myIcon
                 y: 0; anchors.horizontalCenter: parent.horizontalCenter
                 source: icon
             }
             Text {
                 anchors { top: myIcon.bottom; horizontalCenter: parent.horizontalCenter }
                 text: name
                 verticalAlignment: Text.AlignVCenter
                 horizontalAlignment: Text.AlignHCenter
             }
         }
     }

     /*Component {
         id: appHighlight
         Rectangle { width: 50; height: 50; color: "lightsteelblue" }
     }*/

     GridView {
         id: chessGrid
         anchors.fill: parent
         cellWidth: 50; cellHeight: 50
         highlight: appHighlight
         focus: true
         model: appModel
         delegate: appDelegate

         Rectangle {
             id: r8f1
             x: 50
             y: 50
             width: 50
             height: 50
             color: "#00000000"
         }

         Rectangle {
             id: r8f2
             x: 100
             y: 50
             width: 50
             height: 50
             color: "#00000000"
         }

         Rectangle {
             id: r8f3
             x: 150
             y: 50
             width: 50
             height: 50
             color: "#00000000"
         }

         Rectangle {
             id: r8f4
             x: 200
             y: 50
             width: 50
             height: 50
             color: "#00000000"
         }

         Rectangle {
             id: r8f5
             x: 250
             y: 50
             width: 50
             height: 50
             color: "#00000000"
         }

         Rectangle {
             id: r8f6
             x: 300
             y: 50
             width: 50
             height: 50
             color: "#00000000"
         }

         Rectangle {
             id: r8f7
             x: 350
             y: 50
             width: 50
             height: 50
             color: "#00000000"
         }

         Rectangle {
             id: r8f8
             x: 399
             y: 50
             width: 50
             height: 50
             color: "#00000000"
         }

         MouseArea {
             id: mouseArea1
             objectName: "boardMouseArea"
             x: 50
             y: 50
             width: 400
             height: 400

             onClicked: appRect.qmlSignal(mouse.x, mouse.y)
         }
     }
 }

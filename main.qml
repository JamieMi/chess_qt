import QtQuick 2.0
import QtWinExtras 1.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1


Rectangle {
     id: appRect
     objectName: "mainRect"
     width: 600; height: 500
     color: "white"

     signal signalBoardClick(int x, int y)
     signal signalNewGame()
     signal signalSaveGame()
     signal signalLoadGame()
     signal signalHuman()
     signal signalComputer()
     signal signalComputerMove()

    Rectangle {
         id: boardRect
         objectName: "boardRect"
         width: 500; height: 500
         color: "white"

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

         //Component {
         //    id: appHighlight
         //    Rectangle { width: 50; height: 50; color: "lightsteelblue" }
         //}

         GridView {
             id: chessGrid
             anchors.fill: parent
             cellWidth: 50; cellHeight: 50
             //highlight: appHighlight
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
                 id: mouseAreaBoard
                 objectName: "boardMouseArea"
                 x: 50
                 y: 50
                 width: 400
                 height: 400

                 onClicked: appRect.signalBoardClick(mouse.x, mouse.y)
             }
         }
     }

    Image {
        id: imageNew
        x: 516
        y: 42
        width: 60
        height: 60
        sourceSize.height: 123
        sourceSize.width: 123
        source: "new48.png"
    }

    Image {
        id: imageSave
        x: 516
        y: 103
        width: 60
        height: 60
        sourceSize.width: 123
        sourceSize.height: 123
        source: "save48.png"
    }

    Image {
        id: imageHuman
        x: 522
        y: 294
        width: 48
        height: 48
        sourceSize.width: 123
        sourceSize.height: 123
        source: "user_male_olive_green.png"
    }

    Image {
        id: imageComputer
        x: 522
        y: 348
        width: 48
        height: 48
        sourceSize.height: 123
        sourceSize.width: 123
        source: "user_computer.png"
    }

    Image {
        id: imageComputerMove
        x: 522
        y: 402
        width: 48
        height: 48
        sourceSize.width: 123
        sourceSize.height: 123
        source: "play_move.png"
    }

     Image {
         id: imageLoad
         x: 516
         y: 164
         width: 60
         height: 60
         sourceSize.height: 123
         sourceSize.width: 123
         source: "Load48.png"
     }

     MouseArea {
         id: mouseAreaNew
         x: 516
         y: 42
         width: 60
         height: 60
         onClicked: appRect.signalNewGame()
     }

     MouseArea {
         id: mouseAreaSave
         x: 516
         y: 103
         width: 60
         height: 60
         onClicked: appRect.signalSaveGame()
     }

     MouseArea {
         id: mouseAreaLoad
         x: 516
         y: 164
         width: 60
         height: 60
         onClicked: appRect.signalLoadGame()
     }

     MouseArea {
         id: mouseAreaHuman
         x: 522
         y: 294
         width: 48
         height: 48
         onClicked: appRect.signalHuman()
     }

     MouseArea {
         id: mouseAreaComputer
         x: 522
         y: 348
         width: 48
         height: 48
         onClicked: appRect.signalComputer()
     }

     MouseArea {
         id: mouseAreaComputerMove
         x: 522
         y: 402
         width: 48
         height: 48
         onClicked: appRect.signalComputerMove()
     }
}

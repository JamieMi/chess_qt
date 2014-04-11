import QtQuick 2.0
import QtWinExtras 1.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1


Rectangle {
     id: appRect
     objectName: "mainRect"
     width: 700; height: 550
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
         x: 25; y:25
         color: "white"

         ListModel {
             id: appModel
             ListElement { name: ""}
             ListElement { name: "\n\nA"}
             ListElement { name: "\n\nB"}
             ListElement { name: "\n\nC"}
             ListElement { name: "\n\nD"}
             ListElement { name: "\n\nE"}
             ListElement { name: "\n\nF"}
             ListElement { name: "\n\nG"}
             ListElement { name: "\n\nH"}
             ListElement { name: ""}

             ListElement { name: "\n  8"}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: "\n8  "}

             ListElement { name: "\n  7"}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: "\n7  "}

             ListElement { name: "\n  6"}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: "\n6 "}

             ListElement { name: "\n  5"}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: "\n5 "}

             ListElement { name: "\n  4"}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: "\n4  "}

             ListElement { name: "\n  3"}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: "\n3  "}

             ListElement { name: "\n  2"}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: "\n2  "}

             ListElement { name: "\n  1"}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: ""}
             ListElement { name: "\n1  "}

             ListElement { name: ""}
             ListElement { name: "\nA"}
             ListElement { name: "\nB"}
             ListElement { name: "\nC"}
             ListElement { name: "\nD"}
             ListElement { name: "\nE"}
             ListElement { name: "\nF"}
             ListElement { name: "\nG"}
             ListElement { name: "\nH"}
             ListElement { name: ""}
         }

         Component {
             id: appDelegate

             Item {
                 width: 50; height: 50

                 Image {
                     id: myIcon
                     y: 0; anchors.horizontalCenter: parent.horizontalCenter
                     //source: icon
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

             /*Rectangle {
                 id: r8f1
                 x: 550
                 y: 5
                 width: 50
                 height: 50
                 color: "#00000000"
                 border.width: 1
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
             }*/

             MouseArea {
                 id: mouseAreaBoard
                 objectName: "boardMouseArea"
                 x: 50
                 y: 50
                 width: 400
                 height: 400

                 onClicked: appRect.signalBoardClick(mouse.x, mouse.y)
             }

             Image {x: 50; y: 50; width: 50; height: 50; source: imageR8F1}
             Image {x: 100; y: 50; width: 50; height: 50; source: imageR8F2}
             Image {x: 150; y: 50; width: 50; height: 50; source: imageR8F3}
             Image {x: 200; y: 50; width: 50; height: 50; source: imageR8F4}
             Image {x: 250; y: 50; width: 50; height: 50; source: imageR8F5}
             Image {x: 300; y: 50; width: 50; height: 50; source: imageR8F6}
             Image {x: 350; y: 50; width: 50; height: 50; source: imageR8F7}
             Image {x: 400; y: 50; width: 50; height: 50; source: imageR8F8}
             Image {x: 50; y: 100; width: 50; height: 50; source: imageR7F1}
             Image {x: 100; y: 100; width: 50; height: 50; source: imageR7F2}
             Image {x: 150; y: 100; width: 50; height: 50; source: imageR7F3}
             Image {x: 200; y: 100; width: 50; height: 50; source: imageR7F4}
             Image {x: 250; y: 100; width: 50; height: 50; source: imageR7F5}
             Image {x: 300; y: 100; width: 50; height: 50; source: imageR7F6}
             Image {x: 350; y: 100; width: 50; height: 50; source: imageR7F7}
             Image {x: 400; y: 100; width: 50; height: 50; source: imageR7F8}
             Image {x: 50; y: 150; width: 50; height: 50; source: imageR6F1}
             Image {x: 100; y: 150; width: 50; height: 50; source: imageR6F2}
             Image {x: 150; y: 150; width: 50; height: 50; source: imageR6F3}
             Image {x: 200; y: 150; width: 50; height: 50; source: imageR6F4}
             Image {x: 250; y: 150; width: 50; height: 50; source: imageR6F5}
             Image {x: 300; y: 150; width: 50; height: 50; source: imageR6F6}
             Image {x: 350; y: 150; width: 50; height: 50; source: imageR6F7}
             Image {x: 400; y: 150; width: 50; height: 50; source: imageR6F8}
             Image {x: 50; y: 200; width: 50; height: 50; source: imageR5F1}
             Image {x: 100; y: 200; width: 50; height: 50; source: imageR5F2}
             Image {x: 150; y: 200; width: 50; height: 50; source: imageR5F3}
             Image {x: 200; y: 200; width: 50; height: 50; source: imageR5F4}
             Image {x: 250; y: 200; width: 50; height: 50; source: imageR5F5}
             Image {x: 300; y: 200; width: 50; height: 50; source: imageR5F6}
             Image {x: 350; y: 200; width: 50; height: 50; source: imageR5F7}
             Image {x: 400; y: 200; width: 50; height: 50; source: imageR5F8}

             Image {x: 50; y: 250; width: 50; height: 50; source: imageR4F1}
             Image {x: 100; y: 250; width: 50; height: 50; source: imageR4F2}
             Image {x: 150; y: 250; width: 50; height: 50; source: imageR4F3}
             Image {x: 200; y: 250; width: 50; height: 50; source: imageR4F4}
             Image {x: 250; y: 250; width: 50; height: 50; source: imageR4F5}
             Image {x: 300; y: 250; width: 50; height: 50; source: imageR4F6}
             Image {x: 350; y: 250; width: 50; height: 50; source: imageR4F7}
             Image {x: 400; y: 250; width: 50; height: 50; source: imageR4F8}

             Image {x: 50; y: 300; width: 50; height: 50; source: imageR3F1}
             Image {x: 100; y: 300; width: 50; height: 50; source: imageR3F2}
             Image {x: 150; y: 300; width: 50; height: 50; source: imageR3F3}
             Image {x: 200; y: 300; width: 50; height: 50; source: imageR3F4}
             Image {x: 250; y: 300; width: 50; height: 50; source: imageR3F5}
             Image {x: 300; y: 300; width: 50; height: 50; source: imageR3F6}
             Image {x: 350; y: 300; width: 50; height: 50; source: imageR3F7}
             Image {x: 400; y: 300; width: 50; height: 50; source: imageR3F8}

             Image {x: 50; y: 350; width: 50; height: 50; source: imageR2F1}
             Image {x: 100; y: 350; width: 50; height: 50; source: imageR2F2}
             Image {x: 150; y: 350; width: 50; height: 50; source: imageR2F3}
             Image {x: 200; y: 350; width: 50; height: 50; source: imageR2F4}
             Image {x: 250; y: 350; width: 50; height: 50; source: imageR2F5}
             Image {x: 300; y: 350; width: 50; height: 50; source: imageR2F6}
             Image {x: 350; y: 350; width: 50; height: 50; source: imageR2F7}
             Image {x: 400; y: 350; width: 50; height: 50; source: imageR2F8}

             Image {x: 50; y: 400; width: 50; height: 50; source: imageR1F1}
             Image {x: 100; y: 400; width: 50; height: 50; source: imageR1F2}
             Image {x: 150; y: 400; width: 50; height: 50; source: imageR1F3}
             Image {x: 200; y: 400; width: 50; height: 50; source: imageR1F4}
             Image {x: 250; y: 400; width: 50; height: 50; source: imageR1F5}
             Image {x: 300; y: 400; width: 50; height: 50; source: imageR1F6}
             Image {x: 350; y: 400; width: 50; height: 50; source: imageR1F7}
             Image {x: 400; y: 400; width: 50; height: 50; source: imageR1F8}


         }
     }

    Image {
        id: imageNew
        x: 577
        y: 76
        width: 60
        height: 60
        sourceSize.height: 123
        sourceSize.width: 123
        source: "new48.png"
    }

    Image {
        id: imageSave
        x: 577
        y: 137
        width: 60
        height: 60
        sourceSize.width: 123
        sourceSize.height: 123
        source: "save48.png"
    }

    Image {
        id: imageHuman
        x: 583
        y: 319
        width: 48
        height: 48
        sourceSize.width: 123
        sourceSize.height: 123
        source: "user_male_olive_green.png"
    }

    Image {
        id: imageComputer
        x: 583
        y: 373
        width: 48
        height: 48
        sourceSize.height: 123
        sourceSize.width: 123
        source: "user_computer.png"
    }

    Image {
        id: imageComputerMove
        x: 583
        y: 427
        width: 48
        height: 48
        sourceSize.width: 123
        sourceSize.height: 123
        source: "play_move.png"
    }

     Image {
         id: imageLoad
         x: 577
         y: 198
         width: 60
         height: 60
         sourceSize.height: 123
         sourceSize.width: 123
         source: "Load48.png"
     }

     MouseArea {
         id: mouseAreaNew
         x: 577
         y: 76
         width: 60
         height: 60
         onClicked: appRect.signalNewGame()
     }

     MouseArea {
         id: mouseAreaSave
         x: 577
         y: 137
         width: 60
         height: 60
         onClicked: appRect.signalSaveGame()
     }

     MouseArea {
         id: mouseAreaLoad
         x: 577
         y: 198
         width: 60
         height: 60
         onClicked: appRect.signalLoadGame()
     }

     MouseArea {
         id: mouseAreaHuman
         x: 583
         y: 319
         width: 48
         height: 48
         onClicked: appRect.signalHuman()
     }

     MouseArea {
         id: mouseAreaComputer
         x: 583
         y: 373
         width: 48
         height: 48
         onClicked: appRect.signalComputer()
     }

     MouseArea {
         id: mouseAreaComputerMove
         x: 583
         y: 427
         width: 48
         height: 48
         onClicked: appRect.signalComputerMove()
     }
}

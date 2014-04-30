import QtQuick 2.0
import QtWinExtras 1.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1


Rectangle {
     id: appRect
     objectName: "mainRect"
     width: 700; height: 550
     color: "white"
     state: appState

     signal signalBoardClick(int x, int y)
     signal signalBoardPress(int x, int y)
     signal signalPositionChange(int x, int y)
     signal signalNewGame()
     signal signalSaveGame()
     signal signalLoadGame()
     signal signalHuman()
     signal signalComputer()
     signal signalComputerMove()
     signal signalMoveComplete()
     signal signalMoveReady()

    // TO DO: This grid view is a little wasteful now - think about doing away with it.
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

         GridView {
             id: chessGrid
             interactive: false
             anchors.fill: parent
             cellWidth: 50; cellHeight: 50
             focus: true
             model: appModel
             delegate: appDelegate

             MouseArea {
                 id: mouseAreaBoard
                 objectName: "boardMouseArea"
                 x: 50
                 y: 50
                 width: 400
                 height: 400

                 onClicked:{
                     appRect.signalBoardClick(mouse.x, mouse.y)
                 }
                 onPressed:{
                     appRect.signalBoardPress(mouse.x, mouse.y)
                 }
                 onPositionChanged:{
                     appRect.signalPositionChange(mouse.x, mouse.y)
                 }
             }

             Image {id: pieceTransition; z: zTransition; x: xTransition; y: yTransition; width: 50; height: 50; opacity: opacityTransition; source: imageTransition}

             Image {id: piece1; z: zPiece1; x: xPiece1; y: yPiece1; width: 50; height: 50; opacity: opacity1; source: imagePiece1}
             Image {id: piece2; z: zPiece2; x: xPiece2; y: yPiece2; width: 50; height: 50; opacity: opacity2; source: imagePiece2}
             Image {id: piece3; z: zPiece3; x: xPiece3; y: yPiece3; width: 50; height: 50; opacity: opacity3; source: imagePiece3}
             Image {id: piece4; z: zPiece4; x: xPiece4; y: yPiece4; width: 50; height: 50; opacity: opacity4; source: imagePiece4}
             Image {id: piece5; z: zPiece5; x: xPiece5; y: yPiece5; width: 50; height: 50; opacity: opacity5; source: imagePiece5}
             Image {id: piece6; z: zPiece6; x: xPiece6; y: yPiece6; width: 50; height: 50; opacity: opacity6; source: imagePiece6}
             Image {id: piece7; z: zPiece7; x: xPiece7; y: yPiece7; width: 50; height: 50; opacity: opacity7; source: imagePiece7}
             Image {id: piece8; z: zPiece8; x: xPiece8; y: yPiece8; width: 50; height: 50; opacity: opacity8; source: imagePiece8}
             Image {id: piece9; z: zPiece9; x: xPiece9; y: yPiece9; width: 50; height: 50; opacity: opacity9; source: imagePiece9}
             Image {id: piece10; z: zPiece10; x: xPiece10; y: yPiece10; width: 50; height: 50; opacity: opacity10; source: imagePiece10}
             Image {id: piece11; z: zPiece11; x: xPiece11; y: yPiece11; width: 50; height: 50; opacity: opacity11; source: imagePiece11}
             Image {id: piece12; z: zPiece12; x: xPiece12; y: yPiece12; width: 50; height: 50; opacity: opacity12; source: imagePiece12}
             Image {id: piece13; z: zPiece13; x: xPiece13; y: yPiece13; width: 50; height: 50; opacity: opacity13; source: imagePiece13}
             Image {id: piece14; z: zPiece14; x: xPiece14; y: yPiece14; width: 50; height: 50; opacity: opacity14; source: imagePiece14}
             Image {id: piece15; z: zPiece15; x: xPiece15; y: yPiece15; width: 50; height: 50; opacity: opacity15; source: imagePiece15}
             Image {id: piece16; z: zPiece16; x: xPiece16; y: yPiece16; width: 50; height: 50; opacity: opacity16; source: imagePiece16}
             Image {id: piece17; z: zPiece17; x: xPiece17; y: yPiece17; width: 50; height: 50; opacity: opacity17; source: imagePiece17}
             Image {id: piece18; z: zPiece18; x: xPiece18; y: yPiece18; width: 50; height: 50; opacity: opacity18; source: imagePiece18}
             Image {id: piece19; z: zPiece19; x: xPiece19; y: yPiece19; width: 50; height: 50; opacity: opacity19; source: imagePiece19}
             Image {id: piece20; z: zPiece20; x: xPiece20; y: yPiece20; width: 50; height: 50; opacity: opacity20; source: imagePiece20}
             Image {id: piece21; z: zPiece21; x: xPiece21; y: yPiece21; width: 50; height: 50; opacity: opacity21; source: imagePiece21}
             Image {id: piece22; z: zPiece22; x: xPiece22; y: yPiece22; width: 50; height: 50; opacity: opacity22; source: imagePiece22}
             Image {id: piece23; z: zPiece23; x: xPiece23; y: yPiece23; width: 50; height: 50; opacity: opacity23; source: imagePiece23}
             Image {id: piece24; z: zPiece24; x: xPiece24; y: yPiece24; width: 50; height: 50; opacity: opacity24; source: imagePiece24}
             Image {id: piece25; z: zPiece25; x: xPiece25; y: yPiece25; width: 50; height: 50; opacity: opacity25; source: imagePiece25}
             Image {id: piece26; z: zPiece26; x: xPiece26; y: yPiece26; width: 50; height: 50; opacity: opacity26; source: imagePiece26}
             Image {id: piece27; z: zPiece27; x: xPiece27; y: yPiece27; width: 50; height: 50; opacity: opacity27; source: imagePiece27}
             Image {id: piece28; z: zPiece28; x: xPiece28; y: yPiece28; width: 50; height: 50; opacity: opacity28; source: imagePiece28}
             Image {id: piece29; z: zPiece29; x: xPiece29; y: yPiece29; width: 50; height: 50; opacity: opacity29; source: imagePiece29}
             Image {id: piece30; z: zPiece30; x: xPiece30; y: yPiece30; width: 50; height: 50; opacity: opacity30; source: imagePiece30}
             Image {id: piece31; z: zPiece31; x: xPiece31; y: yPiece31; width: 50; height: 50; opacity: opacity31; source: imagePiece31}
             Image {id: piece32; z: zPiece32; x: xPiece32; y: yPiece32; width: 50; height: 50; opacity: opacity32; source: imagePiece32}

             Image {
                 id: boardImage
                 x: 50
                 y: 50
                 width: 400
                 height: 400
                 source: "board.gif"
                 z: -1
             }
         }
     }

    Image {
        id: imageNew
        x: 580
        y: 141
        width: 60
        height: 60
        sourceSize.height: 123
        sourceSize.width: 123
        source: "new48.png"
    }

    Image {
        id: imageSave
        x: 580
        y: 202
        width: 60
        height: 60
        sourceSize.width: 123
        sourceSize.height: 123
        source: "save48.png"
    }

    Image {
        id: imageHuman
        x: 586
        y: 335
        width: 48
        height: 48
        visible: false
        sourceSize.width: 123
        sourceSize.height: 123
        source: "user_male_olive_green.png"
    }

    Image {
        id: imageComputer
        x: 586
        y: 396
        width: 48
        height: 48
        visible: false
        sourceSize.height: 123
        sourceSize.width: 123
        source: "user_computer.png"
    }

    Image {
        id: imageComputerMove
        x: 586
        y: 455
        width: 48
        height: 48
        sourceSize.width: 123
        sourceSize.height: 123
        source: "play_move.png"
    }

     Image {
         id: imageLoad
         x: 580
         y: 263
         width: 60
         height: 60
         sourceSize.height: 123
         sourceSize.width: 123
         source: "Load48.png"
     }

     MouseArea {
         id: mouseAreaNew
         x: 580
         y: 141
         width: 60
         height: 60
         onClicked: appRect.signalNewGame()
     }

     MouseArea {
         id: mouseAreaSave
         x: 580
         y: 202
         width: 60
         height: 60
         onClicked: appRect.signalSaveGame()
     }

     MouseArea {
         id: mouseAreaLoad
         x: 580
         y: 263
         width: 60
         height: 60
         onClicked: appRect.signalLoadGame()
     }

     MouseArea {
         id: mouseAreaHuman
         x: 586
         y: 335
         width: 48
         height: 48
         onClicked: appRect.signalHuman()
     }

     MouseArea {
         id: mouseAreaComputer
         x: 586
         y: 396
         width: 48
         height: 48
         onClicked: appRect.signalComputer()
     }

     MouseArea {
         id: mouseAreaComputerMove
         x: 586
         y: 455
         width: 48
         height: 48
         onClicked: appRect.signalComputerMove()
     }
     Text {
         id: text1
         x: 520
         y: 25
         width: 180
         height: 105
         text: qsTr(statusText)
         z: 2
         wrapMode: Text.WordWrap
         horizontalAlignment: Text.AlignHCenter
         verticalAlignment: Text.AlignTop
         font.pixelSize: 16
     }
     states: [
         State {
             name: "EndMoveState"
             PropertyChanges {
                 target: pieceTransition//stateOrigin
                 x: transitionTargetX
                 y: transitionTargetY
             }

             PropertyChanges {
                 target: appRect
                 border.color: "#ff0000"
             }
         }
     ]
     transitions: [
             Transition {
                 id: transition1
                     from: "*"; to: "EndMoveState"
                     NumberAnimation {
                         properties: "x,y";
                         easing.type: Easing.InOutQuad;
                         duration: 500
                     }
                     onRunningChanged:{
                         if(!running){
                             appRect.signalMoveComplete()
                         }
                     }
                 },
             Transition {
                 id: transition2
                     NumberAnimation {
                         properties: "x,y";
                         duration: 0
                     }
                     onRunningChanged:{
                         if(!running){
                             appRect.signalMoveReady()
                         }
                     }
                 }
             ]
}

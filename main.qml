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
             }

             Image {id: stateOrigin; z: zStateOrigin; x: xStateOrigin; y: yStateOrigin; width: 50; height: 50; opacity: 1; source: imageStateOrigin}

             // TO DO: now we have stateOrigin, we can remove x,y,z properties below?
             Image {id: imager8f1; z: zR8F1; x: 50; y: 50; width: 50; height: 50; opacity: opacityR8F1; source: imageR8F1}
             Image {id: imager8f2; z: zR8F2; x: 100; y: 50; width: 50; height: 50; opacity: opacityR8F2; source: imageR8F2}
             Image {id: imager8f3; z: zR8F3; x: 150; y: 50; width: 50; height: 50; opacity: opacityR8F3; source: imageR8F3}
             Image {id: imager8f4; z: zR8F4; x: 200; y: 50; width: 50; height: 50; opacity: opacityR8F4; source: imageR8F4}
             Image {id: imager8f5; z: zR8F5; x: 250; y: 50; width: 50; height: 50; opacity: opacityR8F5; source: imageR8F5}
             Image {id: imager8f6; z: zR8F6; x: 300; y: 50; width: 50; height: 50; opacity: opacityR8F6; source: imageR8F6}
             Image {id: imager8f7; z: zR8F7; x: 350; y: 50; width: 50; height: 50; opacity: opacityR8F7; source: imageR8F7}
             Image {id: imager8f8; z: zR8F8; x: 400; y: 50; width: 50; height: 50; opacity: opacityR8F8; source: imageR8F8}
             Image {id: imager7f1; z: zR7F1; x: 50; y: 100; width: 50; height: 50; opacity: opacityR7F1; source: imageR7F1}
             Image {id: imager7f2; z: zR7F2; x: 100; y: 100; width: 50; height: 50; opacity: opacityR7F2; source: imageR7F2}
             Image {id: imager7f3; z: zR7F3; x: 150; y: 100; width: 50; height: 50; opacity: opacityR7F3; source: imageR7F3}
             Image {id: imager7f4; z: zR7F4; x: 200; y: 100; width: 50; height: 50; opacity: opacityR7F4; source: imageR7F4}
             Image {id: imager7f5; z: zR7F5; x: 250; y: 100; width: 50; height: 50; opacity: opacityR7F5; source: imageR7F5}
             Image {id: imager7f6; z: zR7F6; x: 300; y: 100; width: 50; height: 50; opacity: opacityR7F6; source: imageR7F6}
             Image {id: imager7f7; z: zR7F7; x: 350; y: 100; width: 50; height: 50; opacity: opacityR7F7; source: imageR7F7}
             Image {id: imager7f8; z: zR7F8; x: 400; y: 100; width: 50; height: 50; opacity: opacityR7F8; source: imageR7F8}
             Image {id: imager6f1; z: zR6F1; x: 50; y: 150; width: 50; height: 50; opacity: opacityR6F1; source: imageR6F1}
             Image {id: imager6f2; z: zR6F2; x: 100; y: 150; width: 50; height: 50; opacity: opacityR6F2; source: imageR6F2}
             Image {id: imager6f3; z: zR6F3; x: 150; y: 150; width: 50; height: 50; opacity: opacityR6F3; source: imageR6F3}
             Image {id: imager6f4; z: zR6F4; x: 200; y: 150; width: 50; height: 50; opacity: opacityR6F4; source: imageR6F4}
             Image {id: imager6f5; z: zR6F5; x: 250; y: 150; width: 50; height: 50; opacity: opacityR6F5; source: imageR6F5}
             Image {id: imager6f6; z: zR6F6; x: 300; y: 150; width: 50; height: 50; opacity: opacityR6F6; source: imageR6F6}
             Image {id: imager6f7; z: zR6F7; x: 350; y: 150; width: 50; height: 50; opacity: opacityR6F7; source: imageR6F7}
             Image {id: imager6f8; z: zR6F8; x: 400; y: 150; width: 50; height: 50; opacity: opacityR6F8; source: imageR6F8}
             Image {id: imager5f1; z: zR5F1; x: 50; y: 200; width: 50; height: 50; opacity: opacityR5F1; source: imageR5F1}
             Image {id: imager5f2; z: zR5F2; x: 100; y: 200; width: 50; height: 50; opacity: opacityR5F2; source: imageR5F2}
             Image {id: imager5f3; z: zR5F3; x: 150; y: 200; width: 50; height: 50; opacity: opacityR5F3; source: imageR5F3}
             Image {id: imager5f4; z: zR5F4; x: 200; y: 200; width: 50; height: 50; opacity: opacityR5F4; source: imageR5F4}
             Image {id: imager5f5; z: zR5F5; x: 250; y: 200; width: 50; height: 50; opacity: opacityR5F5; source: imageR5F5}
             Image {id: imager5f6; z: zR5F6; x: 300; y: 200; width: 50; height: 50; opacity: opacityR5F6; source: imageR5F6}
             Image {id: imager5f7; z: zR5F7; x: 350; y: 200; width: 50; height: 50; opacity: opacityR5F7; source: imageR5F7}
             Image {id: imager5f8; z: zR5F8; x: 400; y: 200; width: 50; height: 50; opacity: opacityR5F8; source: imageR5F8}
             Image {id: imager4f1; z: zR4F1; x: 50; y: 250; width: 50; height: 50; opacity: opacityR4F1; source: imageR4F1}
             Image {id: imager4f2; z: zR4F2; x: 100; y: 250; width: 50; height: 50; opacity: opacityR4F2; source: imageR4F2}
             Image {id: imager4f3; z: zR4F3; x: 150; y: 250; width: 50; height: 50; opacity: opacityR4F3; source: imageR4F3}
             Image {id: imager4f4; z: zR4F4; x: 200; y: 250; width: 50; height: 50; opacity: opacityR4F4; source: imageR4F4}
             Image {id: imager4f5; z: zR4F5; x: 250; y: 250; width: 50; height: 50; opacity: opacityR4F5; source: imageR4F5}
             Image {id: imager4f6; z: zR4F6; x: 300; y: 250; width: 50; height: 50; opacity: opacityR4F6; source: imageR4F6}
             Image {id: imager4f7; z: zR4F7; x: 350; y: 250; width: 50; height: 50; opacity: opacityR4F7; source: imageR4F7}
             Image {id: imager4f8; z: zR4F8; x: 400; y: 250; width: 50; height: 50; opacity: opacityR4F8; source: imageR4F8}
             Image {id: imager3f1; z: zR3F1; x: 50; y: 300; width: 50; height: 50; opacity: opacityR3F1; source: imageR3F1}
             Image {id: imager3f2; z: zR3F2; x: 100; y: 300; width: 50; height: 50; opacity: opacityR3F2; source: imageR3F2}
             Image {id: imager3f3; z: zR3F3; x: 150; y: 300; width: 50; height: 50; opacity: opacityR3F3; source: imageR3F3}
             Image {id: imager3f4; z: zR3F4; x: 200; y: 300; width: 50; height: 50; opacity: opacityR3F4; source: imageR3F4}
             Image {id: imager3f5; z: zR3F5; x: 250; y: 300; width: 50; height: 50; opacity: opacityR3F5; source: imageR3F5}
             Image {id: imager3f6; z: zR3F6; x: 300; y: 300; width: 50; height: 50; opacity: opacityR3F6; source: imageR3F6}
             Image {id: imager3f7; z: zR3F7; x: 350; y: 300; width: 50; height: 50; opacity: opacityR3F7; source: imageR3F7}
             Image {id: imager3f8; z: zR3F8; x: 400; y: 300; width: 50; height: 50; opacity: opacityR3F8; source: imageR3F8}
             Image {id: imager2f1; z: zR2F1; x: 50; y: 350; width: 50; height: 50; opacity: opacityR2F1; source: imageR2F1}
             Image {id: imager2f2; z: zR2F2; x: 100; y: 350; width: 50; height: 50; opacity: opacityR2F2; source: imageR2F2}
             Image {id: imager2f3; z: zR2F3; x: 150; y: 350; width: 50; height: 50; opacity: opacityR2F3; source: imageR2F3}
             Image {id: imager2f4; z: zR2F4; x: 200; y: 350; width: 50; height: 50; opacity: opacityR2F4; source: imageR2F4}
             Image {id: imager2f5; z: zR2F5; x: 250; y: 350; width: 50; height: 50; opacity: opacityR2F5; source: imageR2F5}
             Image {id: imager2f6; z: zR2F6; x: 300; y: 350; width: 50; height: 50; opacity: opacityR2F6; source: imageR2F6}
             Image {id: imager2f7; z: zR2F7; x: 350; y: 350; width: 50; height: 50; opacity: opacityR2F7; source: imageR2F7}
             Image {id: imager2f8; z: zR2F8; x: 400; y: 350; width: 50; height: 50; opacity: opacityR2F8; source: imageR2F8}
             Image {id: imager1f1; z: zR1F1; x: 50; y: 400; width: 50; height: 50; opacity: opacityR1F1; source: imageR1F1}
             Image {id: imager1f2; z: zR1F2; x: 100; y: 400; width: 50; height: 50; opacity: opacityR1F2; source: imageR1F2}
             Image {id: imager1f3; z: zR1F3; x: 150; y: 400; width: 50; height: 50; opacity: opacityR1F3; source: imageR1F3}
             Image {id: imager1f4; z: zR1F4; x: 200; y: 400; width: 50; height: 50; opacity: opacityR1F4; source: imageR1F4}
             Image {id: imager1f5; z: zR1F5; x: 250; y: 400; width: 50; height: 50; opacity: opacityR1F5; source: imageR1F5}
             Image {id: imager1f6; z: zR1F6; x: 300; y: 400; width: 50; height: 50; opacity: opacityR1F6; source: imageR1F6}
             Image {id: imager1f7; z: zR1F7; x: 350; y: 400; width: 50; height: 50; opacity: opacityR1F7; source: imageR1F7}
             Image {id: imager1f8; z: zR1F8; x: 400; y: 400; width: 50; height: 50; opacity: opacityR1F8; source: imageR1F8}

             Rectangle {
                 id: rectangle1
                 x: 50
                 y: 50
                 width: 400
                 height: 400
                 color: "#ff0000"
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
                 target: stateOrigin
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

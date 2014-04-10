#include <QtGui/QGuiApplication>
#include <QApplication>
#include "qtquick2applicationviewer.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QQuickItem> // needed for QQuickView
#include <QObject> // needed to derive a class from QObject
#include <string>
#include <sstream>
#include "chess.h"
//#include <QMenu>
//#include <QMenuBar>

using namespace std;

void ChessGUI::boardClick(int x, int y) const{
    QMessageBox msgBox;
    stringstream ss;
    string xystring;
    ss << x << ":" << y;
    ss >> xystring;
    msgBox.setText(xystring.c_str());
    //msgBox.setInformativeText("Mouse position");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void ChessGUI::showMessage(string msg) const{
    QMessageBox msgBox;
    msgBox.setText(msg.c_str());
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void ChessGUI::newClick() const{
    showMessage("New game");
}

void ChessGUI::saveClick() const{
    showMessage("Save game");
}

void ChessGUI::loadClick() const{
    showMessage("Load game");
}

void ChessGUI::humanClick() const{
    showMessage("Player takes control of computer user");
}

void ChessGUI::computerClick() const{
    showMessage("Computer takes control");
}

void ChessGUI::computerMoveClick() const{
    showMessage("Computer plays move for user");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // QApplication, rather than QGUIApplication, is needed to handle widgets

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/Chess/main.qml"));
    viewer.showExpanded();

    // get root object
    QObject *item = viewer.rootObject();

    ChessGUI chessGUI;
    QObject::connect(item, SIGNAL(signalBoardClick(int,int)),
                        &chessGUI, SLOT(boardClickSlot(int,int)));

    QObject::connect(item, SIGNAL(signalNewGame()),
                        &chessGUI, SLOT(newSlot()));

    QObject::connect(item, SIGNAL(signalSaveGame()),
                        &chessGUI, SLOT(saveSlot()));

    QObject::connect(item, SIGNAL(signalLoadGame()),
                        &chessGUI, SLOT(loadSlot()));

    QObject::connect(item, SIGNAL(signalHuman()),
                        &chessGUI, SLOT(humanSlot()));

    QObject::connect(item, SIGNAL(signalComputer()),
                        &chessGUI, SLOT(computerSlot()));

    QObject::connect(item, SIGNAL(signalComputerMove()),
                        &chessGUI, SLOT(computerMoveSlot()));

    return app.exec();
}




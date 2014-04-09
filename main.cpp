#include <QtGui/QGuiApplication>
#include <QApplication>
#include "qtquick2applicationviewer.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QQuickItem> // needed for QQuickView
#include <QObject> // needed to derive a class from QObject
#include "chess.h"
#include <QMenu>
#include <QMenuBar>

#include <string>
#include <sstream>
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // QApplication, rather than QGUIApplication, is needed to handle widgets

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/Chess/main.qml"));
    viewer.showExpanded();

    // get root object
    QObject *item = viewer.rootObject();

    ChessGUI myClass;
    //ApplicationData myClass;
    QObject::connect(item, SIGNAL(qmlSignal(int,int)),
                        &myClass, SLOT(cppSlot(int,int)));

    return app.exec();
}




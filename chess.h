#ifndef CHESS_H
#define CHESS_H

#include <QObject> // needed to derive a class from QObject.
//Not sure why - in conjunction with deleting the build dir and reccompiling, this seemed to fix the vtable message

class ChessGUI : public QObject
{
public:
    void boardClick(int x, int y) const;
    Q_OBJECT
public:
    Q_INVOKABLE void cppSlot(int x, int y) const {
        boardClick(x,y);
    }
};

#endif // CHESS_H

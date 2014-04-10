#ifndef CHESS_H
#define CHESS_H

#include <QObject> // needed to derive a class from QObject.
//Not sure why - in conjunction with deleting the build dir and reccompiling, this seemed to fix the vtable message

class ChessGUI : public QObject
{
public:
    Q_OBJECT
private:
    void boardClick(int x, int y) const;
    void newClick() const;
    void saveClick() const;
    void loadClick() const;
    void humanClick() const;
    void computerClick() const;
    void computerMoveClick() const;
    void showMessage(std::string msg) const;
public:
    Q_INVOKABLE void boardClickSlot(int x, int y) const {boardClick(x,y);}
    Q_INVOKABLE void newSlot() const {newClick();}
    Q_INVOKABLE void saveSlot() const {saveClick();}
    Q_INVOKABLE void loadSlot() const {loadClick();}
    Q_INVOKABLE void humanSlot() const {humanClick();}
    Q_INVOKABLE void computerSlot() const {computerClick();}
    Q_INVOKABLE void computerMoveSlot() const {computerMoveClick();}
};

#endif // CHESS_H

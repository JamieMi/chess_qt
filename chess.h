#ifndef CHESS_H
#define CHESS_H

#include <set>
#include <vector>

#include "qtquick2applicationviewer.h" // needed to use a pointer to the class
#include <QObject> // needed to derive a class from QObject.

// structures

class position{
public:
    size_t row;
    size_t col;
    position(): row(-1),col(-1) { // TO DO : initialisation list
    }
    position(size_t r, size_t c):row(r),col(c){
    }
    // note: other comparisons not implemented, as they don't really make sense for our needs
};

class pastmove{
public:
    position startPos;
    position endPos;
    pastmove(position start, position end):startPos(start),endPos(end){}
    // to go in a set, these need a copy constructor(member) and comparison operators (external)
    pastmove(const pastmove& pm){
        startPos = pm.startPos;
        endPos = pm.endPos;
    }
};

class pastmoves{
public:
    std::multiset<pastmove> pastmoveSet; // the order of the moves doesn't matter
    // it's OK for this to start empty
};

class board{
public:
    std::vector<std::string> grid;
    board(const std::vector<std::string>& g){
        for (size_t i = 0; i < g.size(); ++i){
            grid.push_back(g[i]);
        }
    }
    // to go in a set, these need a copy constructor(member) and comparison operators (external)
    /*pastmove(const pastmove& pm){
        startPos = pm.startPos;
        endPos = pm.endPos;
    }*/
};

class pastboards{
public:
    std::multiset<board> boardSet; // the order of the moves doesn't matter
    // it's OK for this to start empty
};

class player{
    public:
        bool bComputer;
        std::string pieces;
        std::string name;
        size_t lastPieceMoved;
        std::vector<position> positions;
        std::vector<pastmoves> pieceMoves;
        void setPieces();
        player(std::string n){
            setPieces();
            name = n;
        }
        player(const player& pl);
        position& getPosition(size_t index){return positions[index];}
        void pushBackPos(position pos){positions.push_back(pos);}
            // is there a real purpose to this data hiding?
        const std::string& getName()const{return name;}
        void setName(std::string n){name = n;}
};

class gameobject{
public:
    std::vector<std::string> cboard;//current board
    pastboards pastBoards;
    size_t turn;
    std::vector<player> players;
    gameobject();
    gameobject(const gameobject& game);
    void createBoard(){for (size_t i = 0;i < 8; ++i) cboard.push_back("........");}
    // Note: board size not referred to here, as it would complicate the method...
    void resetBoard(){
        cboard.clear();
        createBoard();
    }
    void createPositions();
    void printBoard() /*const*/;
    void printPlayers() const;
    void printPieces() const;
    void placePieces();
    void loadGame(size_t& player);
    void saveGame(const size_t& player) const;
    void newGame();
    void makeBoard(); // all functionality common to New and Load
    bool validateMove(std::string cmd, size_t iplayer = 0) /*const*/;
    void getMoveCoords(const std::string& cmd, position& start, position& end) const;
    void movePiece(std::string cmd, size_t iplayer = 0, bool bAI = false);
    bool checkPath(const position& start_pos,
                    const position& end_pos,
                    const int& distance,
                    const bool& bDestEmpty,
                    const int& theirPiece,
                    const size_t& iplayer,
                    bool bCheckCheck) const;
    bool inCheck(const size_t& iPlayer, size_t& hiScore, position startPos, position endPos, /*const*/ gameobject* gm) const;
    bool inCheckMate(const size_t& player, const bool bMakeMove);
    bool isOurs( position& pos, size_t iPlayer) ;
    bool computerTurn(size_t iPlayer);
    std::string createMove(const position& startPos, const position& endPos) const ;
    void showStats() /*const*/;
    int getNumPieces(const int& iPlayer) const;
    bool isStalemate();
    char basicType(char p);
    size_t getCaptureValue(const char& piece);
};

class ChessGUI : public QObject
{
    Q_OBJECT
public:
    void displayBoardImages(gameobject* pgm, QtQuick2ApplicationViewer* pView) const;
private:
    void boardClick(int x, int y) const;
    void newClick() const;
    void saveClick() const;
    void loadClick() const;
    void humanClick() const;
    void computerClick() const;
    void computerMoveClick() const;
    void showMessage(std::string msg) const;
    std::string setSquareImage(gameobject* pgm, char& p, int row, int col) const;
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

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
    position(): row(-1),col(-1) {}
    position(size_t r, size_t c):row(r),col(c){}
    bool isValid();
};

class pastmove{
public:
    position startPos;
    position endPos;
    pastmove():startPos(position()),endPos(position()){}
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
    player(std::string n){
        setPieces();
        name = n;
    }
    pastmove lastMove;
    player(const player& pl);
    position& getPosition(size_t index){return positions[index];}
    void pushBackPos(position pos){positions.push_back(pos);}
        // is there a real purpose to this data hiding?
    const std::string& getName()const{return name;}
    void setPieces();
};

class gameobject{
public:
    int cPlayer;
    position GUImoveStart, GUImoveEnd;
    size_t turn;
    std::vector<player> players;
    std::vector<std::string> cboard;//current board
    gameobject();
    gameobject(const gameobject& game);
    bool isOurs( position& pos, int iPlayer) ;
    bool inCheck(const size_t& iPlayer, size_t& hiScore, position startPos, position endPos, gameobject* gm);
    std::string gameDetails();
    bool inCheckMate(const size_t& player, const bool bMakeMove);
    void movePiece(std::string cmd, size_t iplayer = 0, bool bAI = false);
    std::string createMove(const position& startPos, const position& endPos) const ;
    bool validateMove(std::string cmd, size_t iplayer = 0);
    bool bGameOver;
    bool isStalemate();
    bool loadGame();
    bool saveGame() const;
    void newGame();
    char basicType(char p);
    void printBoard();
    bool computerTurn(size_t iPlayer);
    size_t getTakenIndex() const {return opIndex;}
    void clearTakenIndex(){opIndex = -1;}
private:
    pastboards pastBoards;
    size_t opIndex; // recorded so that the image of a taken piece can be removed after a transition
    void createBoard(){for (size_t i = 0;i < 8; ++i) cboard.push_back("........");}
    // Note: board size not referred to here, as it would complicate the method...
    void resetBoard(){
        cboard.clear();
        createBoard();
    }
    void createPositions();
    void printPlayers() const;
    void placePieces();
    void makeBoard(); // all functionality common to New and Load
    void getMoveCoords(const std::string& cmd, position& start, position& end) const;
    bool checkPath(const position& start_pos,
                    const position& end_pos,
                    const int& distance,
                    const bool& bDestEmpty,
                    const int& theirPiece,
                    const size_t& iplayer,
                    bool bCheckCheck);
    void printPieces() const;
    int getNumPieces(const int& iPlayer) const;
    size_t getCaptureValue(const char& piece);
    bool scoreDirectionsLoop(size_t i, char p, bool bMove, std::string& sMove, size_t hiScore, position ourPos, size_t iPlayer);
    bool commonScoring(bool bInCheck, size_t hiScore, int ourValue, int theirValue, int kingValue, bool bAI, size_t iPlayer);
    std::string createProspectiveMove(size_t& hiScore, size_t moveScore, const position ourPos, const position endPos) const;
};

class ChessGUI : public QObject
{
    Q_OBJECT
public:
    bool bProcessing;
    ChessGUI(gameobject* pg, QtQuick2ApplicationViewer* pV){pGame = pg; pView = pV;}
    void displayBoardImages() const;
    void initialise();
    std::string getBoardRef(int x, int y) const;
    void showMessage(std::string msg) const;
    void setPieceImage(char p, position pos, size_t piece/*1-32*/, bool bTransition) const;
private:
    void boardClick(int x, int y);
    void newClick() const;
    void saveClick() const;
    void loadClick() const;
    void humanClick() const;
    void computerClick();
    void computerMoveClick();
    void transitionComplete();
    void completeMove();
    bool move();
    void moveReady();
    std::string setSquareImage(char& p, int row, int col) const;
    void recreateBoard() const;
    gameobject* pGame;
    QtQuick2ApplicationViewer* pView;
    void play();
    void executeMove(std::string cmd);
    void displayMove();
    void setOpacity(size_t iPiece, std::string opacity) const;
signals:
    void signalMoveReady();

public:
    Q_INVOKABLE void boardClickSlot(int x, int y) {boardClick(x,y);}
    Q_INVOKABLE void newSlot() const {newClick();}
    Q_INVOKABLE void saveSlot() const {saveClick();}
    Q_INVOKABLE void loadSlot() const {loadClick();}
    Q_INVOKABLE void humanSlot() const {humanClick();}
    Q_INVOKABLE void computerSlot() {computerClick();}
    Q_INVOKABLE void computerMoveSlot() {computerMoveClick();}
    Q_INVOKABLE void transitionCompleteSlot(){transitionComplete();}
    Q_INVOKABLE void moveReadySlot(){moveReady();}
};


#endif // CHESS_H

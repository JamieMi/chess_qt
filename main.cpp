#include <QtGui/QGuiApplication>
#include <QApplication>
#include "qtquick2applicationviewer.h"
#include <QDeclarativeEngine>
#include <QQmlContext>
#include <QMessageBox>
#include <QMouseEvent>
#include <QQuickItem> // needed for QQuickView
#include <QObject> // needed to derive a class from QObject
#include <string>
#include <iostream>
#include <string>
#include <iomanip> //setfill, setw
#include <algorithm> //for transform, to convert to lowercase, and random_shuffle
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <sstream>
#include <math.h>
#include <assert.h>

using namespace std;

#include "chess.h"

//#include <QMenu>
//#include <QMenuBar>

const int MAX_TURNS = 50;
const int FRAME_WIDTH = 1200;
const int FRAME_HEIGHT = 800;
const int BOARD_WIDTH = 400;
const int BUTTON_WIDTH = 50;
const int STATUS_WIDTH = 24;
const int STATUS_PAD = 10;
const int BOARD_FILES = 8;
const int BOARD_RANKS = 8;

#define __DEBUG__
#define __GUI__

#ifdef __GUI__
    bool bImages = true;
#else
    bool bImages = false;
#endif

// turns on debug output and pauses

void trace(string s){
#ifdef __DEBUG__
    cout << s << endl;
#endif
}

bool operator== (const position& pos, const position& pos2){return (pos2.col == pos.col && pos2.row == pos.row);}
bool operator!= (const position& pos, const position& pos2){return !(pos == pos2);}

// these must be free functions. Member functions run into problems with constness when they are called inline (e.g. member operator< can't call a member operator>)
bool operator== (const pastmove& pm,const pastmove& pm2) {return (pm2.startPos == pm.startPos && pm2.endPos == pm.endPos);}
bool operator!= (const pastmove& pm, const pastmove& pm2) {return (pm2.startPos != pm.startPos || pm2.endPos != pm.endPos);}
bool operator< ( const pastmove& pm,  const pastmove& pm2) {
    // lexicographic. position should really have these too and we should use those, but this is the only time we will need
    // position comparison operators other than == and !=
    if (pm2.startPos.col < pm.startPos.col)
        return true;
    else if (pm2.startPos.col > pm.startPos.col)
        return false;

    if (pm2.startPos.row < pm.startPos.row)
        return true;
    else if (pm2.startPos.row > pm.startPos.row)
        return false;

    if (pm2.endPos.col < pm.endPos.col)
        return true;
    else if (pm2.endPos.col > pm.endPos.col)
        return false;

    if (pm2.endPos.row < pm.endPos.row)
        return true;
    else if (pm2.endPos.row > pm.endPos.row)
        return false;
    else
        // equal
        return false;
}
bool operator> (const pastmove& pm, pastmove& pm2) {return pm2 < pm;}
bool operator>= (const pastmove& pm, pastmove& pm2) {return (pm2 < pm) || (pm == pm2);}
bool operator<= (const pastmove& pm, pastmove& pm2) {return (pm < pm2) || (pm == pm2);}

// these must be free functions. Member functions run into problems with constness when they are called inline (e.g. member operator< can't call a member operator>)
bool operator== (const board& pm,const board& pm2) {
    for (size_t i = 0; i < 8; ++i)
        if (pm.grid[i] != pm2.grid[i]) return false;
    return true;
}
// TO DO: normally these should accept const params, but for some reason it doesn't work here...
bool operator!= (const board& pm, const board& pm2) {return !(pm == pm2);}
bool operator< ( const board& pm,  const board& pm2) {
    // lexicographic. position should really have these too and we should use those, but this is the only time we will need
    // position comparison operators other than == and !=
    std::string boardStr, board2Str;
    for (size_t i = 0; i < pm.grid.size(); ++i){
        boardStr += pm.grid[i];
        board2Str += pm2.grid[i];
    }
    return (boardStr < board2Str);
}
bool operator> (const board& pm, board& pm2) {return pm2 < pm;}
bool operator>= (const board& pm, board& pm2) {return (pm2 < pm) || (pm == pm2);}
bool operator<= (const board& pm, board& pm2) {return (pm < pm2) || (pm == pm2);}


gameobject::gameobject(){
    turn = 1;// TO DO: member intialisation list

    players.push_back(player("Black"));
    players.push_back(player("White"));
}

// copy constructor
gameobject::gameobject(const gameobject& game){
    //vector<string>* pboard = new vector<string>;
    for (vector<string>::const_iterator itr = game.cboard.begin(); itr < game.cboard.end(); ++itr)
        cboard.push_back(*itr);
    for (vector<player>::const_iterator itr = game.players.begin(); itr < game.players.end(); ++itr)
        players.push_back(*itr); // copy construction in players necessary too
    turn = game.turn;
}

char gameobject::basicType(char p){
    // for those pieces that could be used in unusual moves (en passant, castling), we record the pieces
    // with a different character to track this. But we still want to know what they are, fundamentally.
    if (p == 'L') p = 'K';
    else if (p == 'E') p = 'P';
    else if (p == 'S') p = 'R';
    else if (p == 'l') p = 'k';
    else if (p == 'e') p = 'p';
    else if (p == 's') p = 'r';
    return p;
}

void player::setPieces(){
    bComputer = false;
    pieces = "PPPPPPPPSNBQLBNS";
    // P = pawn
    // E = pawn vulnerable to en passant
    // R = moved rook
    // S = unmoved rook
    // N = knight
    // B = bishop
    // Q = queen
    // K = moved king
    // L = unmoved king
    for (size_t i = 0; i < (BOARD_FILES * 2); ++i){
        pieceMoves.push_back(pastmoves());
    }
    lastPieceMoved = 0;
}

void gameobject::newGame(){
    players[0].setPieces();
    players[1].setPieces();
    for (size_t i = 0; i < 16; ++i){
        players[0].pieceMoves[i].pastmoveSet.clear();
        players[1].pieceMoves[i].pastmoveSet.clear();
    }
    pastBoards.boardSet.clear();
    createPositions();
    makeBoard();
}

void gameobject::makeBoard(){
    resetBoard();
    placePieces();
    printBoard();
}

void gameobject::createPositions(){
    players[0].positions.clear();
    players[1].positions.clear();
    //pawns
    for (size_t i = 0; i < BOARD_FILES; i++){
        players[0].pushBackPos(position(1,i));
        players[1].pushBackPos(position(BOARD_RANKS-2,i));
    }

    //others
    // TO DO: link this to the pieces list?
    for (size_t i = 0; i < BOARD_FILES; i++){
        players[0].pushBackPos(position(0,i));
        players[1].pushBackPos(position(BOARD_RANKS-1,i));
    }
}


void gameobject::placePieces(){
    size_t pl = 0; //player 1
    for (size_t j = 0; j < players.size(); ++j){
        for (size_t i = 0; i < players[j].pieces.size(); ++i){
            int row = players[j].getPosition(i).row;
            int col = players[j].getPosition(i).col;
            if (row > 7 || row < 0)
                continue;// piece has been taken
            string srow = cboard[row];
            char piece = players[j].pieces[i];
            srow[col] = piece;
            cboard[row] = srow;
        }
        ++pl;
    }
}

void gameobject::loadGame(size_t& player){
    // get names and positions
    string filename("saved_game.txt");
    ifstream input;
    input.open(filename);
    if (!input.is_open()){
        cout << "Saved game could not be loaded." << endl;
        input.clear();
        input.close();
        return;
    }

    // players already exists - just write over it
    for (size_t i = 0; i < players.size(); ++i){
        getline(input, players[i].name);
        size_t ps = players[i].positions.size();
        players[i].positions.clear();
        for (size_t y = 0; y < ps; ++y){
            stringstream ss;
            string ln;
            getline(input,ln);
            ss << ln;
            size_t row, col;
            ss >> players[i].pieces[y];
            ss >> row >> col;
            position pos(row, col);
            players[i].pushBackPos(pos);
        }
    }

    stringstream ss;
    string ln;
    getline(input,ln);
    ss << ln << endl;
    getline(input,ln);
    ss << ln;
    ss >> player >> turn;

    input.close();

    // reload the board
    makeBoard();
}

void gameobject::saveGame(const size_t& player) const{
    ofstream output;
    string filename("saved_game.txt");
    output.open(filename);
    if (!output.is_open()){
        cout << filename << " could not be saved." << endl;
        output.clear();
        output.close();
        return;
    }
    stringstream ss;
    for (size_t i = 0; i < players.size(); ++i){
        ss << players[i].name + "\n";
        for (size_t y = 0; y < players[i].positions.size(); ++y){
            position pos = players[i].positions[y];
            ss << players[i].pieces[y] << " " << pos.row << " " << pos.col << endl;
        }
    }

    ss << player << endl;
    ss << turn << endl;

    output << ss.str();
    output.close();
}

bool gameobject::isOurs( position& pos, size_t iplayer){
    for (size_t i = 0; i < players[iplayer].positions.size(); ++i){
        if (players[iplayer].positions[i] == pos)
            return true; // dude, bail
    }
    return false;
}

int gameobject::getNumPieces(const int& iplayer) const{
    int total = 0;
    for (size_t i = 0; i < players[iplayer].positions.size(); ++i){
        if (players[iplayer].positions[i].row <= 7)
            ++total;
    }
    return total;
}

bool gameobject::computerTurn(size_t iplayer){
    // calculate all moves - do the one with the highest value
    //return inCheckMate(iplayer, true); // TO DO
    return false;
    // (barring stalemate, should never return false though, or it would already have been detected)
}

string gameobject::createMove(const position& startPos, const position& endPos)const{
    string sMove;
    stringstream ss;
    char startCol = (char)(int(startPos.col) + (int)'A');
    char endCol = (char)(int(endPos.col) + (int) 'A');

    ss << startCol << (8-startPos.row) << '-' << endCol << (8-endPos.row);
    ss >> sMove;

    return sMove;
}

void gameobject::getMoveCoords(const string& cmd, position& start, position& end) const{
    stringstream ss;
    char cStartCol, cNewCol = ' ';
    size_t startRow, newRow, startCol, newCol = 0;
    char dummy;
    ss << cmd;
    ss >> cStartCol >> startRow >> dummy >> cNewCol >> newRow;
    cStartCol = toupper(cStartCol);
    cNewCol = toupper(cNewCol);
    startCol = cStartCol - int('A') + 1;
    newCol = cNewCol - int('A') + 1;
    start.row = 8 - startRow; // internally, the rows are ordered here from the top down, unlike ranks
    start.col = startCol -1;
    end.row = 8 - newRow;
    end.col = newCol -1;
}

void gameobject::printPieces() const{
    // TO DO: do this iterator fashion...
    for (size_t i = 0; i < players.size(); ++i){
        cout << players[i].getName() << endl;
            for (vector<position>::const_iterator itr = players[i].positions.begin(); itr != players[i].positions.end(); ++itr)
            cout << "Piece: " << itr->row << " : " << itr->col << endl;
    }
    cout << endl;
}

size_t gameobject::getCaptureValue(const char& piece){
    // add scores for takeover of their piece
    size_t value;
    if (basicType(piece) =='P') value = 3;
    else if (piece == 'N') value = 5;
    else if (basicType(piece) ==  'R') value = 6;
    else if (piece == 'B') value = 7;
    else if (piece == 'Q') value = 9;
    else if (basicType(piece) == 'K') value = 10;
    // not scoring an en passant move - but I think we could do that outside, as it's only possible to take a pawn anyway
    else value = 0; // may be a blank space
    return value;
}

void gameobject::printBoard(){
#ifdef __GUI__
#ifndef __DEBUG__
return;
#endif
#endif
    cout << setfill('_') << setw(65) << "" << endl;
    cout << endl << "Type 'load', 'save', 'new', 'quit', or a move e.g. (a1-b2)" << endl << "Type 'help' for a full list of instructions." << endl << endl;
    string y = "ABCDEFGH";
    cout << "     A  B  C  D  E  F  G  H" << endl;
    cout << "   " << (char)201 << setw(23) << setfill((char)205) << "" << (char) 187 << endl;

    for (size_t row = 0; row < BOARD_RANKS; ++row){
        cout << " " << (BOARD_RANKS - row) << " " << (char)186; // unfortunately I wrote this game without realising ranks are ordered from the bottom up
        for (size_t j = 0; j < BOARD_FILES; ++j){
            // do a conversion of the board now:
            char p = cboard[row][j];
            char cdir = (char) 31; // down (player 1)
            position pos(row,j);
            p = basicType(p);
            for (size_t i = 0; i < players[0].positions.size(); ++i){
                if (players[0].positions[i] == pos){
                    cdir = (char) 30; // up (player 2)
                    // convert player 1 to lower case:
                    p = tolower(p);
                    break;
                }
            }
            if (p == '.'){
                p = ' ';
                cdir = ' ';
            }
            cout << cdir << p;
            if (j < (BOARD_RANKS - 1)) cout << char(179);
        }
        cout << (char) 186 << " " << (8 - row) << endl;
        string filler;
        filler += char(196);
        filler += char(196);
        filler += char(197);
        if (row <7) cout << "   " << (char)186 << filler << filler << filler << filler << filler << filler << filler << char(196) << char(196) /*setw(24) << setfill(' ') << ""*/ << (char) 186 << endl;
    }
    cout << "   " << (char)200 << setw(23) << setfill((char)205) << "" << (char) 188 << endl;
    cout << "     A  B  C  D  E  F  G  H" << endl;
}


bool toEndline(const position& endPos, const size_t& player){
    return endPos.row == (1-player)*7;
}

bool offBackline(const position& ourPos, const position& endPos, const size_t& distance, const size_t& player){
    if ( (ourPos.row == player*7) && (abs((int)endPos.row - (int)ourPos.row) >= (int)distance) )
        return true;
    else
        return false;
}

// copy constructor
player::player(const player& pl){
    for (vector<position>::const_iterator itr = pl.positions.begin(); itr < pl.positions.end(); ++itr)
        // (interestingly, the < iterator comparison works for vectors but not multisets...)
        positions.push_back(*itr);
    for (vector<pastmoves>::const_iterator itr = pl.pieceMoves.begin(); itr < pl.pieceMoves.end(); ++itr)
        pieceMoves.push_back(*itr);
    //for (string::const_iterator itr = game.pieces.begin(); itr < game.pieces.end(); ++itr)
    //	pieces.push_back(*itr);
    name = pl.name;
    pieces = pl.pieces;
    bComputer = pl.bComputer;
    lastPieceMoved = 0;
}

//-----------------------------------------------------------------------------------------
//  GUI
//-----------------------------------------------------------------------------------------

void ChessGUI::boardClick(int x, int y) const{
    QMessageBox msgBox;
    stringstream ss;
    string xyString;
    ss << x << ":" << y;
    ss >> xyString;
    msgBox.setText(xyString.c_str());
    //msgBox.setInformativeText("Mouse position");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

string ChessGUI::setSquareImage(gameobject* pgm, char& p, int row, int col) const{
    string strImage;
    position pos(row, col);
    char btp = pgm->basicType(p);

    for (position each: pgm->players[0].positions){
        if (each == pos){
            // convert player 1 to lower case:
            btp = tolower(btp);
            break;
        }
    }

    char alt_txt(' ');
    if (bImages){
        if (((row*(BOARD_RANKS + 1))+col) % 2 == 0){
            strImage = "light";
        }else{
            strImage = "dark";
        }
    }else{
        strImage = "";
    }
    if (btp != '.'){
        alt_txt = btp;
        if (btp == 'p' && bImages) strImage += "pawnd";
        else if (btp == 'P' && bImages) strImage += "pawn";
        else if (btp == 'b' && bImages) strImage += "bishopd";
        else if (btp == 'B' && bImages) strImage += "bishop";
        else if (btp == 'r' && bImages) strImage += "rookd";
        else if (btp == 'R' && bImages) strImage += "rook";
        else if (btp == 'n' && bImages) strImage += "knightd";
        else if (btp == 'N' && bImages) strImage += "knight";
        else if (btp == 'k' && bImages) strImage += "kingd";
        else if (btp == 'K' && bImages) strImage += "king";
        else if (btp == 'q' && bImages) strImage += "queend";
        else if (btp == 'Q' && bImages) strImage += "queen";
    }
    if (strImage == "light" || strImage == "dark") strImage += "square";
    strImage += ".gif";
    p = alt_txt;
    return strImage;
}

void ChessGUI::displayBoardImages(gameobject* pgm, QtQuick2ApplicationViewer* pView) const{
    //create a list of buttons
    for (int r = 1; r < (BOARD_RANKS + 1); ++r){
        for (int c = 1; c < (BOARD_FILES + 1); ++c){
            char p = pgm->cboard[r-1][c-1];
            string strImage = setSquareImage(pgm, p, r-1, c-1);
            stringstream ss;
            string strImageName;
            ss << "imageR" << ((BOARD_RANKS + 1) - r) << "F" << (c);
            ss >> strImageName;

            if (bImages)
            {
                pView->rootContext()->setContextProperty( strImageName.c_str()/*"imageTestSource"*/ , strImage.c_str());
            }
            else
            {
                // TO DO: bother to create a text alternative?
            }
        }
    }
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

//-----------------------------------------------------------------------------------------
//  Main
//-----------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // QApplication, rather than QGUIApplication, is needed to handle widgets

    QtQuick2ApplicationViewer viewer;

    //initialise, otherwise errors on loading QML:
    for (int r = 1; r < (BOARD_RANKS + 1); ++r){
        for (int c = 1; c < (BOARD_FILES + 1); ++c){
            stringstream ss;
            string strImageName;
            ss << "imageR" << ((BOARD_RANKS + 1) - r) << "F" << (c);
            ss >> strImageName;
            if (bImages)
            {
                viewer.rootContext()->setContextProperty(strImageName.c_str(), "");
            }
        }
    }
    viewer.setMainQmlFile(QStringLiteral("qml/Chess/main.qml"));
    viewer.setIcon(QIcon("qml/Chess/chess.ico"));
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

    gameobject game;
    game.newGame();
    chessGUI.displayBoardImages(&game, &viewer);

    return app.exec();
}


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

#include "PromotionDlg.h"
#include "chess.h"


//#define __DEBUG__ // shows additional debug in console, irrespective of __GUI__
#define __GUI__ // shows using GUI - alternative is console mode

const int MAX_TURNS = 50;
const int FRAME_WIDTH = 1200;
const int FRAME_HEIGHT = 800;
const int BOARD_WIDTH = 400;
const int BUTTON_WIDTH = 50;
const int STATUS_WIDTH = 24;
const int STATUS_PAD = 10;
const int BOARD_FILES = 8;
const int BOARD_RANKS = 8;

enum selection {NO_SELECTION, FROM_SELECTED, TO_SELECTED};
selection selectionState = NO_SELECTION;

enum looptype{THEM_AGAINST_KING,	// check								M:1
              THEM_AGAINST_CURRENT,	// the opportunity cost (of not moving) M:1
              THEM_AGAINST_PIECE,	// against the piece we've just moved	M:1
              US_AGAINST_KING,		// check								M:1
              US_AGAINST_PIECE,		// against other pieces					1:M
              NO_MORE} checkDirection;

#ifdef __GUI__
    const bool bImages = true;
#ifdef __DEBUG__
    const bool bConsole = true;
#else
    const bool bConsole = false;
#endif
#else
    const bool bConsole = true;
    const bool bImages = false;
#endif

// turns on debug output and pauses

void trace(string s){
#ifdef __DEBUG__
    cout << s << endl;
#else
    s = "";
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

bool toEndline(const position& endPos, const size_t& player){
    return endPos.row == (1-player)*7;
}

bool offBackline(const position& ourPos, const position& endPos, const size_t& distance, const size_t& player){
    return (ourPos.row == player*7) && (abs((int)endPos.row - (int)ourPos.row) >= (int)distance);
}

bool position::isValid(){
    return (row < BOARD_RANKS && col < BOARD_FILES); // (uses size_t)
}

gameobject::gameobject(){
    turn = 1;
    cPlayer = 1;
    bGameOver = false;
    players.push_back(player("Black"));
    players.push_back(player("White"));
}

// copy constructor
gameobject::gameobject(const gameobject& game){
    for (vector<string>::const_iterator itr = game.cboard.begin(); itr < game.cboard.end(); ++itr)
        cboard.push_back(*itr);
    for (vector<player>::const_iterator itr = game.players.begin(); itr < game.players.end(); ++itr)
        players.push_back(*itr); // copy construction in players necessary too
    turn = game.turn;
    cPlayer = game.cPlayer;
    bGameOver = game.bGameOver;
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
    turn = 1;
    cPlayer = 1;
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
            if (row >= BOARD_RANKS || row < 0)
                continue;// piece has been taken
            string srow = cboard[row];
            char piece = players[j].pieces[i];
            srow[col] = piece;
            cboard[row] = srow;
        }
        ++pl;
    }
}

bool gameobject::loadGame(){
    string filename("saved_chess.txt");
    ifstream input;
    input.open(filename);
    if (!input.is_open()){
        input.clear();
        input.close();
        return false;
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
    ss >> cPlayer >> turn;

    input.close();

    // reload the board
    makeBoard();
    return true;
}

bool gameobject::saveGame() const{
    ofstream output;
    string filename("saved_chess.txt");
    output.open(filename);
    if (!output.is_open()){
        output.clear();
        output.close();
        return false;
    }
    stringstream ss;
    for (size_t i = 0; i < players.size(); ++i){
        ss << players[i].name + "\n";
        for (size_t y = 0; y < players[i].positions.size(); ++y){
            position pos = players[i].positions[y];
            ss << players[i].pieces[y] << " " << pos.row << " " << pos.col << endl;
        }
    }

    ss << cPlayer << endl;
    ss << turn << endl;

    output << ss.str();
    output.close();
    return true;
}

bool gameobject::isOurs( position& pos, int iPlayer){
    for (size_t i = 0; i < players[iPlayer].positions.size(); ++i){
        if (players[iPlayer].positions[i] == pos)
            return true;
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

bool gameobject::computerTurn(size_t iPlayer){
    // calculate all moves - do the one with the highest value
    return inCheckMate(iPlayer, true);
    // (barring stalemate, should never return false though, or it would already have been detected)
}

string gameobject::createMove(const position& startPos, const position& endPos)const{
    string sMove;
    stringstream ss;
    char startCol = (char)(int(startPos.col) + (int)'A');
    char endCol = (char)(int(endPos.col) + (int) 'A');
    ss << startCol << (BOARD_RANKS - startPos.row) << '-' << endCol << (BOARD_RANKS - endPos.row);
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
    start.row = BOARD_RANKS - startRow; // internally, the rows are ordered here from the top down, unlike ranks
    start.col = startCol -1;
    end.row = BOARD_RANKS - newRow;
    end.col = newCol -1;
}

void gameobject::printPieces() const{
    if (bConsole){
        for (size_t i = 0; i < players.size(); ++i){
            cout << players[i].getName() << endl;
                for (vector<position>::const_iterator itr = players[i].positions.begin(); itr != players[i].positions.end(); ++itr)
                cout << "Piece: " << itr->row << " : " << itr->col << endl;
        }
        cout << endl;
    }
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

bool gameobject::validateMove(string cmd, size_t iPlayer){
    if (cmd.size() != 5){
        if (bConsole) cout << "\nThat move is not allowed.\n" << endl;
        return false;
    }

    position startPos, endPos;
    getMoveCoords(cmd, startPos, endPos);

    // (note: if the index < 0, size_t returns the max for a size_t)
    if ((startPos.col >= BOARD_FILES) ||
        (startPos.row >= BOARD_RANKS) ||
        (endPos.col >= BOARD_FILES) ||
        (endPos.row >= BOARD_RANKS)) {
        if (bConsole) cout << endl << "That move is not allowed." << endl;
        return false;
    }
    int ourPiece = -1;
    int theirPiece = -1;
    if (cboard[startPos.row][startPos.col] == '.'){
        if (bConsole) cout << "There's no piece to move from there." << endl;
        return false;
    }
    else{// find out if it's one of ours
        for (size_t i = 0; i < players[iPlayer].positions.size(); ++i){
            if (players[iPlayer].positions[i] == startPos){
                ourPiece = i;
                break;
            }
        }
        if (ourPiece == -1){
            if (bConsole) cout << "That's not your piece to move..." << endl;
            return false;
        }
    }
    bool bDestEmpty = true;
    if (cboard[endPos.row][endPos.col] != '.'){
        bDestEmpty = false;
        for (size_t i = 0; i < players[1-iPlayer].positions.size(); ++i){
            if (players[1-iPlayer].positions[i] == endPos){
                theirPiece = i;
                break;
            }
        }
        if (theirPiece != -1){
            if (bConsole) cout << "You've taken a piece..." << endl;
        }
        else
        {
            if (bConsole) cout << "That position is occupied by one of your pieces already." << endl;
            return false;
        }
    }

    char piece = cboard[startPos.row][startPos.col];
    char destination = cboard[endPos.row][endPos.col];
    size_t dummyScore = 0;

    // pawns
    if (basicType(piece) == 'P'){
        // pawns can move 2 down from 2nd row, or 1, or 1 diagonally forward if there is another piece in the way
        if (iPlayer == 0 && (startPos.row == 1 && endPos.row == 3 && startPos.col == endPos.col)){
            //valid - 2 moves forward, if unoccupied
            if (!bDestEmpty || cboard[int(startPos.row + endPos.row)/2][startPos.col] != '.')
                return false;
            else
                return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
        }
        else if (iPlayer == 1 && (startPos.row == 6 && endPos.row == 4 && startPos.col == endPos.col)){
            //valid -  2 moves forward, if unoccupied
            if (!bDestEmpty || cboard[int(startPos.row + endPos.row)/2][startPos.col] != '.')
                return false;
            else
                return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
        }
        else if ((iPlayer == 0 && (endPos.row - startPos.row == 1)) ||
                 (iPlayer == 1 && (startPos.row - endPos.row == 1))){
            // could be valid
            if( (abs((int)endPos.col - (int)startPos.col) == 1 && destination != '.') ||
                (abs((int)endPos.col - (int)startPos.col) == 1 && destination == '.' && cboard[endPos.row + (iPlayer*2 -1)][endPos.col] == 'E')  ){
                //diagonal move, either to take a piece directly or to take a pawn on passant
                // move the right piece in the positions list, if it's one of theirs
                for (size_t i = 0; i < players[1-iPlayer].positions.size(); ++i){
                    if (players[1-iPlayer].positions[i] == endPos){
                        return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
                    }
                    if (players[1-iPlayer].positions[i].col == endPos.col &&
                        players[1-iPlayer].positions[i].row == (endPos.row +(iPlayer*2)-1) ){
                        if (players[1-iPlayer].pieces[i] == 'E'){
                            return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
                            // an en passant diagonal move
                        }
                    }
                }
                return false;
            }
            else if(endPos.col == startPos.col){
                //valid -  1 move forward, if unoccupied
                if (!bDestEmpty)
                    return false;
                else
                    return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
            } else return false;
        }
        else{
            return false;
        }
    }

    // knight
    // 2 x 1, anywhere
    if (piece == 'N'){
        if( (abs((int)endPos.col - (int)startPos.col) == 1 && abs((int)endPos.row - (int)startPos.row) == 2)  ||
            (abs((int)endPos.col - (int)startPos.col) == 2 && abs((int)endPos.row - (int)startPos.row) == 1) ){
            if (bDestEmpty || (theirPiece != -1))
                return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
            else
                return false;
        }
        else
            return false;
    }

    // rook
    if (basicType(piece) == 'R'){
        if (endPos.col == startPos.col || endPos.row == startPos.row){ // can't both be equal anyway
            // direction is OK, at least. Now check what's in the way...
            size_t distance = max( abs((int)endPos.col - (int)startPos.col), abs((int)endPos.row - (int)startPos.row) );
            if (checkPath(startPos, endPos, distance, bDestEmpty, theirPiece, iPlayer, false))
                return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
        }
        return false;
    }

    // bishop
    if (piece == 'B'){
        if( abs((int)endPos.col - (int)startPos.col) == abs((int)endPos.row - (int)startPos.row) ){ // horizontal distance = vertical
            // direction is OK, at least. Now check what's in the way...
            size_t distance = abs((int)endPos.col - (int)startPos.col);
            if (checkPath(startPos, endPos, distance, bDestEmpty, theirPiece, iPlayer, false))
                return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
        }
        return false;
    }

    // queen or king
    if (piece == 'Q' || basicType(piece) == 'K'){
        if( abs((int)endPos.col - (int)startPos.col) == abs((int)endPos.row - (int)startPos.row) ||	// horizontal distance = vertical, or
            (endPos.col == startPos.col || endPos.row == startPos.row) ){                       	// vertical or horizontal
            // direction is OK, at least. Now check what's in the way...
            size_t distance = max(  abs((int)endPos.col - (int)startPos.col) ,													// if diagonal
                                    max(abs((int)endPos.col - (int)startPos.col), abs((int)endPos.row - (int)startPos.row)) );	// if h/v
            if (distance == 1 || piece == 'Q'){ // Kings can only move one space
                if (checkPath(startPos, endPos, distance, bDestEmpty, theirPiece,iPlayer, false)){
                    return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
                }
            }
            else if (distance == 2 && (endPos.row == startPos.row) && (startPos.row == iPlayer*7)){
                // this may be a castling attempt:
                // Castling is permissible if and only if all of the following conditions hold:
                // 1. The king and the chosen rook are on the player's first rank.
                // 2. Neither the king nor the chosen rook have previously moved.
                //    (Use L for an unmoved king, and S for an unmoved rook)

                position rookPos;
                if (cboard[iPlayer*7][0] == 'S' && cboard[iPlayer*7][4] == 'L' && startPos.col == 4 && endPos.col == 2){
                    rookPos.col = 0;
                }
                else if (cboard[iPlayer*7][7] == 'S' && cboard[iPlayer*7][4] == 'L' && startPos.col == 4 && endPos.col == 6){
                    rookPos.col = 7;
                }
                else
                    return false;// one or both of the king and the rook have moved from their starting position (even if they've since returned
                rookPos.row = iPlayer*7;

                // 3. There are no pieces between the king and the chosen rook (NOT merely where the king moves to.)
                if (!checkPath(startPos, rookPos, distance, bDestEmpty, theirPiece, iPlayer,false))
                    return false;

                // 4. The king is not currently in check.
                if (inCheck(iPlayer, dummyScore, position(), position(), NULL) ) return false;

                // 5. The king does not pass through a square that is attacked by an enemy piece.
                //    - Modify check_path to call in_check for every square it passes through...
                if (!checkPath(startPos, endPos, distance, bDestEmpty, theirPiece, iPlayer, true)) //(true = bCheckCheck)
                    return false;

                // 6. The king does not end up in check. (True of any legal move.)
                return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
                //  endPos = new king pos, NOT the rook pos...
                // Note : I believe the rook manoeuvre will always be valid now, because if the king's path is clear, so must be the rook's.
            }
        }
        return false;
    }

    return false;// don't want to allow moves we haven't explicitly modelled
}

void gameobject::movePiece(string cmd, size_t iPlayer, bool bAI){
    position startPos, endPos;
    getMoveCoords(cmd, startPos, endPos);
    //bool bStalemate = false; // to implement

    size_t ourIndex = -1;
    // move the right piece in the positions list
    for (size_t i = 0; i < players[iPlayer].positions.size(); ++i){
        if (players[iPlayer].positions[i] == startPos){
            players[iPlayer].positions[i] = endPos;
            ourIndex = i;
            board storedboard(cboard);
            pastBoards.boardSet.insert(storedboard);
            pastmove pm(startPos, endPos);
            players[iPlayer].pieceMoves[i].pastmoveSet.insert(pm);
            break;
        }
    }

    // If we've taken a piece...
    for (size_t i = 0; i < players[1-iPlayer].positions.size(); ++i){
        if (players[1-iPlayer].positions[i] == endPos){
            position pos(-1,-1);// i.e. invalidate it
            opIndex = i; // because some time after the move, need to remove image of the taken piece
            players[1-iPlayer].positions[i] = pos;
            break;
        }
    }

    // move the piece on the board:
    char piece = cboard[startPos.row][startPos.col];
    // char origDest = cboard[endPos.row][endPos.col];
    cboard[startPos.row][startPos.col] = '.';
    assert (basicType(cboard[endPos.row][endPos.col]) != 'K'); // we should never have reached this point
    cboard[endPos.row][endPos.col] = piece;
    if (bConsole) cout << "Moved." << endl;
    players[iPlayer].lastPieceMoved = ourIndex;

    // if the king is moving from its start position...
    if (players[iPlayer].pieces[ourIndex] == 'L'){
        players[iPlayer].pieces[ourIndex] = 'K';
        cboard[endPos.row][endPos.col] = 'K';
        // is this a castling?
        if (abs((int)startPos.col - (int)endPos.col) == 2){
            // castling - we've moved the king, and now we have to move the rook to the space between
            if (startPos.col == 4 && endPos.col == 6){ // kingside rook - position 16
                players[iPlayer].pieces[15] = 'R';
                players[iPlayer].positions[15].col = 5;
                cboard[startPos.row][5] = 'R';
                cboard[startPos.row][7] = '.';
            }
            else // queenside
            {
                players[iPlayer].pieces[8] = 'R';
                players[iPlayer].positions[8].col = 3;
                cboard[startPos.row][3] = 'R';
                cboard[startPos.row][0] = '.';
            }
        }
    }

    // if the rook is moving from its start position...
    if (players[iPlayer].pieces[ourIndex] == 'S'){
        players[iPlayer].pieces[ourIndex] = 'R';
        cboard[endPos.row][endPos.col] = 'R';
    }

    // check for an en passant move:

    int vDir = 1; // down
    if (iPlayer == 1) vDir = -1; // up
    if ( piece == 'P' && cboard[endPos.row - vDir][endPos.col] == 'E' ){
        // capture the opponent's piece, en passant:
        cboard[endPos.row - vDir][endPos.col] = '.';
        if (bConsole) cout << "Pawn captured, en passant." << endl;
        for (size_t i = 0; i < players[1-iPlayer].positions.size(); ++i){
            if ( (players[1-iPlayer].positions[i].row == endPos.row - vDir) &&
                 (players[1-iPlayer].positions[i].col == endPos.col) )		{
                players[1-iPlayer].positions[i].row = -1;
                players[1-iPlayer].positions[i].col = -1;
            }
        }
    }

    // revert en passant - if any of the opponent's pawns are still en passant from the last move, then they will now revert to their normal status.
    for (size_t i = 0; i < players[1-iPlayer].positions.size(); ++i){
        if ( players[1-iPlayer].pieces[i] == 'E'){
            players[1-iPlayer].pieces[i] = 'P';
            if (players[1-iPlayer].positions[i].row <= 7)
                cboard[players[1-iPlayer].positions[i].row][players[1-iPlayer].positions[i].col] = 'P';
        }
    }

    // has our pawn become en passant in this move?
    if ( piece == 'P' && ((iPlayer == 0 && endPos.row == 3) || (iPlayer == 1 && endPos.row == 4)) ){
        // if this position was achieved with a 2 square move...
        if (abs(int(startPos.row) - int(endPos.row)) == 2){
            // and there is any adjacent opposing piece...
            for (size_t i = 0; i < players[1-iPlayer].positions.size(); ++i){
                if ( (players[1-iPlayer].positions[i].row == endPos.row) &&
                      abs(int(players[1-iPlayer].positions[i].col) - int(endPos.col)) == 1 ){
                          cboard[endPos.row][endPos.col] = 'E';    //signifies en passant; will be restored at the start of the next turn
                          players[iPlayer].pieces[ourIndex] = 'E';			// ditto - allows the state to be saved
                }
            }
        }
    }

    // Can a pawn be promoted?
    if ( piece == 'P' && ((iPlayer == 0 && endPos.row == 7) || (iPlayer == 1 && endPos.row == 0)) ){
        // Promotion
        if (bAI){
            cboard[endPos.row][endPos.col] = 'Q';

            for (size_t i = 0; i < players[iPlayer].pieces.size(); ++i){
                if (players[iPlayer].positions[i] == endPos)
                    players[iPlayer].pieces[i] = cboard[endPos.row][endPos.col];
            }

            if (bConsole) cout << "The pawn has been promoted." << endl;
        }
        else
        {
            string promoText = "The pawn has been promoted. What piece do you want to convert it to? You may choose a knight, rook, bishop or queen.";
            if (bConsole){
                cout << promoText << endl;
                bool bConverted = false;
                string input;
                while(!bConverted){
                    cin >> input;
                    transform(input.begin(), input.end(), input.begin(), ::tolower);
                    if (input == "queen"){
                        cboard[endPos.row][endPos.col] = 'Q';
                        bConverted = true;
                    }
                    if (input == "bishop"){
                        cboard[endPos.row][endPos.col] = 'B';
                        bConverted = true;
                    }
                    if (input == "rook"){
                        cboard[endPos.row][endPos.col] = 'R';
                        bConverted = true;
                    }
                    if (input == "knight"){
                        cboard[endPos.row][endPos.col] = 'N';
                        bConverted = true;
                    }

                    for (size_t i = 0; i < players[iPlayer].pieces.size(); ++i){
                        if (players[iPlayer].positions[i] == endPos)
                            players[iPlayer].pieces[i] = cboard[endPos.row][endPos.col];
                    }
                }
                cout << "Thank you. The pawn has been promoted." << endl;
            }
            else{
                PromotionDlg *dialog = new PromotionDlg();
                dialog->exec();
                cboard[endPos.row][endPos.col] = dialog->getPiece();
                for (size_t i = 0; i < players[iPlayer].pieces.size(); ++i){
                    if (players[iPlayer].positions[i] == endPos)
                        players[iPlayer].pieces[i] = cboard[endPos.row][endPos.col];
                }
            }
        }
    }

    printBoard();
    players[iPlayer].lastMove.endPos = endPos;
    players[iPlayer].lastMove.startPos = startPos;

    return;
}

bool gameobject::commonScoring(bool bInCheck, size_t hiScore, int ourValue, int theirValue, int kingValue, bool bAI, size_t iPlayer){
    if (checkDirection == THEM_AGAINST_KING){
        bInCheck = true;
        hiScore -= 500;
    }
    if (checkDirection == THEM_AGAINST_KING || checkDirection == THEM_AGAINST_PIECE){
        hiScore -= ourValue * (1 + (ourValue >= 9));
    }
    if (checkDirection == THEM_AGAINST_CURRENT)
        hiScore += ourValue * (1 + (ourValue >= 9));
    if (checkDirection == US_AGAINST_KING){
        if (getNumPieces(iPlayer) > 5)
            hiScore -= kingValue / 1.4; // don't provide too much encouragement to check before it's going to work
    }
    if (checkDirection == US_AGAINST_PIECE)
        hiScore += theirValue / 1.5;
    return (bAI && !bInCheck);
}

bool gameobject::inCheck(const size_t& player, size_t& finalHiScore, position startPos, position endPos, gameobject* gm){
    // make a copy of the board and positions arrays, only modified so that any tentative move is overlaid.
#ifdef __DEBUG__
    cout << "inCheck: " << startPos.row << ":" << startPos.col << " - " << endPos.row << ":" << endPos.col << endl;
#endif
    size_t hiScore = finalHiScore; // award 10 pts for a move that takes king - test check

    int iPlayer = player; // allows us to play about with the player without causing confusion outside
    bool bAI = false; // whether we are using this to grade a computer's move
    if (hiScore > 0)
        bAI = true;
    bool bInCheck = false;

    gameobject* pgm = const_cast<gameobject*>(this);
    if (gm != NULL){
        pgm = gm;
    }
    // i.e. either construct a copy of the actual game, or a simulated game passed in.

    gameobject pm(*pgm); // proposed move
    int ourValue, theirValue;
    ourValue = theirValue = 0;

    position nullpos; // for comparison
    int kingValue = getCaptureValue('K');

    if (startPos != nullpos){
        // add scores for takeovers
        int theirValue = getCaptureValue(cboard[endPos.row][endPos.col]);

        hiScore += theirValue * 2; //pieces taken immediately have to be more valuable than those could be taken in the net turn...
        // add the proposed move to the copy of the board

        pm.cboard[endPos.row][endPos.col] = pm.cboard[startPos.row][startPos.col];
        pm.cboard[startPos.row][startPos.col] = '.';

        for (size_t i = 0; i < pm.players[iPlayer].positions.size(); ++i){
            if (pm.players[iPlayer].positions[i] == startPos) {
                pm.players[iPlayer].positions[i] = endPos;
                break;
            }
        }
    }
    else{
        endPos = pm.players[iPlayer].positions[12]; // for when we call it to check check AFTER a move
        // required by the en passant code to show our king isn't a pawn
        assert (endPos.isValid());
    }
    // also need to remove a captured piece, if there is one.
    for (size_t index = 0; index < pm.players[1-iPlayer].positions.size(); ++index)
        if (pm.players[1-iPlayer].positions[index] == endPos)
            pm.players[1-iPlayer].positions[index] = position();

    // from this point on in the function, always use pm. syntax in front of members and function...

    checkDirection = THEM_AGAINST_KING;

    while (checkDirection != NO_MORE){
        // go through all pieces, and all their moves - if any can move on the king, return true
        // for each piece:
        position targetPos = endPos;
        if (bAI == false || (checkDirection == THEM_AGAINST_KING)){
            targetPos = pm.players[iPlayer].positions[12];
            // if the piece we're proposing moving is the king, then we don't want to check against where the king WAS, but rather where the king WILL BE.
            if (basicType(pm.cboard[endPos.row][endPos.col]) == 'K')
                targetPos = endPos;
        }
        else if (checkDirection == THEM_AGAINST_CURRENT)
            targetPos = startPos;
        else if (checkDirection == THEM_AGAINST_PIECE)
            targetPos = endPos;
        else if (checkDirection == US_AGAINST_KING)
            targetPos = pm.players[iPlayer].positions[12]; // (iplayer has been reversed at this point)
        else if (checkDirection == US_AGAINST_PIECE)
            targetPos = endPos;

        int opDir = iPlayer * 2 - 1;

        for (size_t index = 0; index < pm.players[1-iPlayer].positions.size(); ++index){
            char tPiece = pm.players[1-iPlayer].pieces[index];

            position opPos (pm.players[1-iPlayer].positions[index]);
            if (checkDirection == THEM_AGAINST_CURRENT){
                opPos = players[1-iPlayer].positions[index];
                // use existing grid, in case the move assumes the oppo piece is taken
            }
            if (!opPos.isValid())
                continue;
            if (checkDirection == US_AGAINST_PIECE){
                // in this context op_pos represents our piece, and target_pos is the opponent's
                targetPos = opPos;
                opPos = endPos;
                theirValue = getCaptureValue(pm.cboard[targetPos.row][targetPos.col]);
                tPiece = pm.cboard[endPos.row][endPos.col];
            }

            // pawns
            if (basicType(tPiece) == 'P'){
                // for all moves, capturing or not:
                if (checkDirection == THEM_AGAINST_PIECE && \
                    ((targetPos.row == 0 and iPlayer == 1) || (targetPos.row == (BOARD_RANKS-1) and iPlayer == 0))){
                    // if this will be a promotion move, the penalty should be the value of the queen
                    ourValue = getCaptureValue('Q');
                }
                if (targetPos == opPos && checkDirection == US_AGAINST_PIECE){
                    // no points for taking a piece we've already taken
                }
                else if (endPos == opPos && (checkDirection == THEM_AGAINST_KING || checkDirection == THEM_AGAINST_PIECE)){
                    // This piece would already have been taken, therefore it can't threaten our pieces.
                }
                // diagonal move creates check
                else{
                    if ((opPos.row + opDir == targetPos.row) && ( abs((int)opPos.col - (int)targetPos.col) == 1 )){
                        if (!commonScoring(bInCheck, hiScore, ourValue, theirValue, kingValue, bAI, iPlayer))
                            return true;
                    }

                    // en passant move creates check
                    if (tPiece == 'E' && opPos.row == targetPos.row && \
                        abs((int)opPos.col - (int)targetPos.col) == 1){
                        if (!commonScoring(bInCheck, hiScore, ourValue, theirValue, kingValue, bAI, iPlayer))
                            return true;
                    }
                }
            }

            // knight
            if (tPiece == 'N'){
                if (targetPos == opPos && checkDirection == US_AGAINST_PIECE){
                    // no points for taking a piece we've already taken
                }
                else if (endPos == opPos && (checkDirection == THEM_AGAINST_KING || checkDirection == THEM_AGAINST_PIECE)){
                }
                else if ((abs((int)opPos.row - (int)targetPos.row) == 1 && abs((int)opPos.col - (int)targetPos.col) == 2) || \
                    (abs((int)opPos.col - (int)targetPos.col) == 1 && abs((int)opPos.row - (int)targetPos.row) == 2)){
                    if (!commonScoring(bInCheck, hiScore, ourValue, theirValue, kingValue, bAI, iPlayer))
                        return true;
                }
            }

            // rook
            if (basicType(tPiece) == 'R'){
                if (targetPos == opPos && checkDirection == US_AGAINST_PIECE){
                    // no points for taking a piece we've already taken
                }
                else if (endPos == opPos && (checkDirection == THEM_AGAINST_KING || checkDirection == THEM_AGAINST_PIECE)){
                }
                else if (opPos.col == targetPos.col || opPos.row == targetPos.row){ // can't both be equal anyway
                    if (opPos == targetPos)
                        continue;
                    // direction is OK, at least. Now check what's in the way...
                    size_t distance = max( abs((int)opPos.col - (int)targetPos.col), abs((int)opPos.row - (int)targetPos.row));
                    if (pm.checkPath(opPos, targetPos, distance, false, true, iPlayer, false)){ // returns True if the target can be captured
                        if (!commonScoring(bInCheck, hiScore, ourValue, theirValue, kingValue, bAI, iPlayer))
                            return true;
                    }
                }
                // not assuming check:
                if (checkDirection == US_AGAINST_KING){
                    if ((abs((int)opPos.col - (int)targetPos.col) == 2 && abs((int)opPos.row - (int)targetPos.row) == 1) || \
                        (abs((int)opPos.col - (int)targetPos.col) == 2 && abs((int)opPos.row - (int)targetPos.row) == 1))
                        hiScore += 2; // small incentive for a move close enough to cut the king's moves, even if it's not check
                }
            }
            // bishop
            if (tPiece == 'B'){
                if (targetPos == opPos && checkDirection == US_AGAINST_PIECE){
                    // no points for taking a piece we've already taken
                }
                else if (endPos == opPos && (checkDirection == THEM_AGAINST_KING || checkDirection == THEM_AGAINST_PIECE)){
                }
                else if (abs((int)opPos.col - (int)targetPos.col) == abs((int)opPos.row - (int)targetPos.row)){ // horizontal distance = vertical
                    // direction is OK, at least. Now check what's in the way...
                    size_t distance = abs((int)opPos.col - (int)targetPos.col);
                    if (pm.checkPath(opPos, targetPos, distance, false, true, iPlayer, false)){
                        if (!commonScoring(bInCheck, hiScore, ourValue, theirValue, kingValue, bAI, iPlayer))
                            return true;
                    }
                }
                // not assuming check:
                if (checkDirection == US_AGAINST_KING){
                    if ( (abs((int)opPos.col) - (int)targetPos.col == 2) && (abs((int)opPos.row - (int)targetPos.row) == 2) )
                        hiScore += 3; // incentive for a move close enough to cut the king's moves
                }
            }

            // queen or king
            if (basicType(tPiece) == 'K' || tPiece == 'Q'){
                if (targetPos == opPos && checkDirection == US_AGAINST_PIECE){
                    // no points for taking a piece we've already taken
                }
                else if (endPos == opPos && (checkDirection == THEM_AGAINST_KING || checkDirection == THEM_AGAINST_PIECE)){
                }
                else if (abs((int)opPos.col - (int)targetPos.col) == abs((int)opPos.row - (int)targetPos.row) || \
                    (opPos.col == targetPos.col or opPos.row == targetPos.row)){
                    // direction is OK, at least. Now check what's in the way...
                    assert (opPos != targetPos);
                    if (opPos == targetPos)
                        continue;
                    size_t distance = max( abs((int)opPos.col - (int)targetPos.col) ,	\
                        max(abs((int)opPos.col - (int)targetPos.col), abs((int)opPos.row - (int)targetPos.row)) );	// if h/v
                    if (distance == 1 || tPiece == 'Q'){ // Kings can only move one space
                        if (pm.checkPath(opPos, targetPos, distance, false, true, iPlayer, false)){
                            if (!commonScoring(bInCheck, hiScore, ourValue, \
                                theirValue, kingValue, bAI, iPlayer))
                                return true;
                        }
                    }
                }
                // not assuming check:
                if (checkDirection == US_AGAINST_KING){
                    if (abs((int)opPos.col - (int)targetPos.col) == 2 || \
                        abs((int)opPos.row - (int)targetPos.row) == 2)
                        hiScore += 2; // incentive for a move close enough to cut the king's moves
                }
            }

        }

        if (bAI){
            checkDirection = static_cast<looptype>((int)checkDirection + 1);
            if (checkDirection == US_AGAINST_KING)
                iPlayer = 1 - iPlayer;
            else if (checkDirection == US_AGAINST_PIECE)
                iPlayer = 1 - iPlayer;
#ifdef __DEBUG__
            //cout << "checkDirection = " << (int)checkDirection << endl;
#endif
        }
        else checkDirection = NO_MORE;
    }

    // penalise the move if we moved the same piece in the previous turn
    if (bAI){
        if (players[iPlayer].pieces[players[iPlayer].lastPieceMoved] == pm.cboard[startPos.row][startPos.col]){
            hiScore -=3;
            if (pm.cboard[startPos.row][startPos.col] == 'Q' && getNumPieces(1-iPlayer) < 4){
                // penalise the move if we moved the same queen in the previous turn (in addition to the general penalty below)
                // this will give other pieces the chance to move in for the kill in the end game
                hiScore -= 5;
            }
        }
    }
    finalHiScore = hiScore;
#ifdef __DEBUG__
    cout << "finalHiScore = " << finalHiScore << endl;
#endif
    if (!bAI)
        return false;
    else
        return bInCheck;
}

bool gameobject::checkPath(const position& startPos, const position& endPos, const int& distance, const bool& bDestEmpty, const int& theirPiece, const size_t& iPlayer, bool bCheckCheck){
    // any direction
    int dirDown = ((int)endPos.row - (int)startPos.row) / distance; // i.e. can be -1, 0, or 1
    int dirRight = ((int)endPos.col - (int)startPos.col) / distance;
#ifdef __DEBUG__
    cout << "checkPath: " << startPos.row << ":" << startPos.col << " - " << endPos.row << ":" << endPos.col << " dirDown=" << dirDown << " dirRight=" << dirRight << endl;
#endif

    for (int i = 1; i <  distance; ++i){
        int colMovement = dirRight * i;
        int rowMovement = dirDown * i;
        position test(startPos.row + rowMovement, startPos.col + colMovement);
        if (!test.isValid())
            break;
        if (cboard[startPos.row + rowMovement][startPos.col + colMovement] != '.'){
            if (bConsole) cout << "Move invalid - intercepting piece found: " << cboard[startPos.row + rowMovement][startPos.col + colMovement] << endl;
            return false; // can't take the piece if it's only on the way, and it can't be our own
        }
        if (bCheckCheck){
            // further check required - this position may be empty, but for the castling we need to
            // know it cannot be attacked by another piece. In effect, we test as if the king were
            // in that space.
            size_t dummyScore = 0;
            if (inCheck(iPlayer, dummyScore, startPos, endPos, NULL))
                return false;
        }
    }

    // final check - the destination square:
    if (bDestEmpty || theirPiece != -1)
        return true;
    else
        return false;
}

bool gameobject::scoreDirectionsLoop(size_t i, char p, bool bMove, string& sMove, size_t hiScore, position ourPos, size_t iPlayer){
    const int NUM_DIRS = 4 * (1 + (basicType(p) == 'K' or p == 'Q'));
    size_t numOurPieces = getNumPieces(iPlayer);
    size_t numTheirPieces = getNumPieces(1 - iPlayer);
    int dx, dy;
    size_t moveScore;

    for (int dir = 0; dir < NUM_DIRS; ++dir){
        if (basicType(p) == 'R'){
            dx = dy = 0;
            if (dir == 0) dx = -1;
            else if (dir == 1) dx = 1;
            else if (dir == 2) dy = -1;
            else if (dir == 3) dy = 1;
        }
        else if (p == 'B'){
            dx = dy = -1;
            if (dir == 0) dx = dy = 1;
            else if (dir == 1) dx = 1;
            else if (dir == 2) dy = 1;
        }
        else if (basicType(p) == 'K' or p == 'Q'){
            dx = dy = 0;
            if (dir == 0) dy = -1;
            else if (dir == 1){
                dx = 1;
                dy = -1;
            }
            else if (dir == 2) dx = 1;
            else if (dir == 3) dx = dy = 1;
            else if (dir == 4) dy = 1;
            else if (dir == 5){
                dx = -1;
                dy = 1;
            }
            else if (dir == 6) dx = -1;
            else if (dir == 7) dx = dy = -1;
        }

        position endPos (ourPos.row, ourPos.col);
        // size_t offset = 1;
        bool bCont = true;
        while (bCont){
            endPos.row = endPos.row + dy;
            endPos.col = endPos.col + dx;
            // in bounds?
            if (!endPos.isValid())
                bCont = false;
            else{
                if (!isOurs(endPos, iPlayer)){ //  possible
                    moveScore = 1001;
                    if (!inCheck(iPlayer, moveScore, ourPos, endPos, NULL)){
                        if (bMove){
                            // TO DO:
                            /*pastmove pmv(ourPos, endPos);
                            if (players[iPlayer].pieceMoves[i].pastmoveSet[tuple(pmv)] > 0)
                                pm2 = tuple(pmv);
                            for l in players[iplayer].pieceMoves[i].pastmoveSet.elements():
                                s1, s2, s3, s4 = l
                                if ourPos.row == s1 && ourPos.col == s2 && endPos.row == s3 && endPos.col == s4:
                                    moveScore -= s.players[iPlayer].pieceMoves[i].pastmoveSet[l] * 3 # i.e. penalise repeat moves
                                    break;*/
                            if (p == 'B'){
                                if (offBackline(ourPos,endPos, 2, iPlayer))
                                    moveScore += 2;
                            }
                            if (basicType(p) == 'K'){
                                if ((numOurPieces < numTheirPieces) && (numOurPieces < 5) && (ourPos.row != (iPlayer*(BOARD_RANKS - 1))) \
                                    && (endPos.row == (iPlayer * (BOARD_RANKS-1))))
                                    moveScore -= 2; // subtle hint not to move king into the far home corner in the later stages when under pressure
                            }
                            if (hiScore < moveScore){
                                sMove = createProspectiveMove(hiScore, moveScore, ourPos, endPos);
                            }
                        }
                        else{
                            // False indicates only that we are not scoring - i.e. there is a valid move, therefore no further processing required
                            return false;
                        }
                    }
                    if (isOurs(endPos, 1-iPlayer)) // theirs - we can't go on then
                        bCont = false;
                }
                else
                    bCont = false;
            }
            if (players[iPlayer].pieces[i] == 'L' && dy == 0 && abs((int)endPos.col - (int)ourPos.col) == 2){
                // castling - taken from validate_move

                // Castling is permissible if and only if all of the following conditions hold:
                // 1. The king and the chosen rook are on the player's first rank.
                // 2. Neither the king nor the chosen rook have previously moved.
                //    (Use L for an unmoved king, and S for an unmoved rook)

                position rookPos;
                if (cboard[iPlayer*(BOARD_RANKS-1)][0] == 'S' && cboard[iPlayer*(BOARD_RANKS-1)][4] == 'L' \
                    && ourPos.col == 4 && endPos.col == 2)
                    rookPos.col = 0;
                else if (cboard[iPlayer*(BOARD_RANKS-1)][7] == 'S' && cboard[iPlayer*(BOARD_RANKS-1)][4] == 'L' && \
                    ourPos.col == 4 && endPos.col == 6)
                    rookPos.col = 7;
                else
                    bCont = false; // one or both of the king and the rook have moved from their starting position (even if they've since returned
                rookPos.row = iPlayer*(BOARD_RANKS-1);

                // TO DO: is theirPiece really a bool?
                int theirPiece = isOurs(endPos,1 - iPlayer) - 1; // technically this requires an actual index but for the moment, -1 means not theirs
                bool bDestEmpty = ((!isOurs(endPos,1 - iPlayer)) && (not isOurs(endPos,iPlayer)));

                // 3. There are no pieces between the king and the chosen rook (NOT merely where the king moves to.)
                if (!checkPath(ourPos, rookPos, abs((int)endPos.col - (int)ourPos.col), bDestEmpty, theirPiece, iPlayer,false))
                    bCont = false;

                // 4. The king is not currently in check.
                moveScore = 0;

                if (inCheck(iPlayer, moveScore, position(), position(), NULL))
                    bCont = false;

                // 5. The king does not pass through a square that is attacked by an enemy piece.
                //    - Modify check_path to call in_check for every square it passes through...
                if (!checkPath(ourPos, endPos, abs((int)endPos.col - (int)ourPos.col), bDestEmpty, theirPiece, iPlayer, true))
                    bCont = false;

                // 6. The king does not end up in check. (True of any legal move.)
                // Rook manouevre is already proven as valid
                moveScore = 1003;

                if (!inCheck(iPlayer, moveScore, ourPos, endPos, NULL)){ // new king pos, NOT the rook pos...
                    if (bMove){
                        /*pastmove pmv(ourPos, endPos);
                        moveScore -= players[iPlayer].pieceMoves[i].pastmoveSet[tuple(pmv)] * 3 // i.e. penalise repeat moves*/
                        if (hiScore < moveScore)
                            sMove = createProspectiveMove(hiScore, moveScore, ourPos, endPos);
                    }
                    else
                        return false;
                }
                else
                    bCont = false;
                // The move is good
            }
            if (basicType(players[iPlayer].pieces[i]) == 'K')
                bCont = false; // Kings can only move one space
        }
    }
    return true;
}

string gameobject::createProspectiveMove(size_t& hiScore, size_t moveScore, const position ourPos, const position endPos) const{
    hiScore = moveScore;
    return createMove(ourPos, endPos);
}

bool gameobject::isStalemate(){
    // TO DO
    return false;
}

string gameobject::gameDetails(){
    string gameDetails = players[cPlayer].name + "'s turn";
    return gameDetails;
}
bool gameobject::inCheckMate(const size_t& iPlayer, const bool bMove){
    // player here represents the next player, after the last move. We want to know if our player has any possible move.
    // Loop through every piece.
    int ourDir = 1 - iPlayer * 2;
    size_t hiScore = 0;
    string sMove;
    size_t moveScore = 1000;

    // to evaluate in random order...
    srand ( unsigned ( time(0) ) );
    vector<int> myIndexes;

    for (size_t iv = 0; iv < players[iPlayer].positions.size(); ++iv) myIndexes.push_back(iv);
    random_shuffle ( myIndexes.begin(), myIndexes.end() );

    //int inds[16] = {8,3,12,13,9,11,2,6,5,7,0,1,15,4,10,14};
    //vector<int> myIndexes(inds, inds+16);
    // use the above if we want to test this version against the python version

    position opKingPos(position(players[1 - iPlayer].positions[0]));

    for (size_t ind = 0; ind < players[iPlayer].positions.size(); ++ind){
        // translate from index array - this should make it consider moves from all the same pieces, in a random order
        int i = myIndexes[ind];
        position ourPos = players[iPlayer].positions[i];
        if (!ourPos.isValid())
            continue; // this piece was taken already
        position endPos;

        // pawns
        // diagonal move creates check
        char p = players[iPlayer].pieces[i];
        if (basicType(p) == 'P'){
            // one forward
            endPos.row = ourPos.row + ourDir;
            endPos.col = ourPos.col;
            if (!isOurs(endPos, iPlayer) && !isOurs(endPos, 1 - iPlayer)){
                moveScore = 1002;
                if (!inCheck(iPlayer, moveScore, ourPos, endPos, NULL)){
                    // this would get us out of check - we can leave now
                    if (bMove){
                        if ((ourPos.col > 0 && cboard[iPlayer * (BOARD_RANKS - 1)][ourPos.col - 1] == 'B') || \
                            (ourPos.col < (BOARD_FILES - 1) && cboard[iPlayer * (BOARD_RANKS -1)][ourPos.col + 1] == 'B')){
                            // frees a bishop
                            moveScore += 2;
                        }
                        if (toEndline(endPos, iPlayer)){
                            moveScore += 4;
                        }
                        if (hiScore < moveScore){
                            sMove = createProspectiveMove(hiScore, moveScore, ourPos, endPos);
                        }
                    }
                    else
                        return false;
                }
            }
            // two forward
            if ((ourPos.row == 1 && ourDir == 1) || (ourPos.row == (BOARD_RANKS-1) && ourDir == -1)){
                endPos.row = int(ourPos.row + ourDir * 2);
                endPos.col = int(ourPos.col);
                if (!isOurs(endPos, iPlayer)){
                    if (cboard[endPos.row][endPos.col] == '.' && cboard[int((ourPos.row + endPos.row)/2)][int(ourPos.col)] == '.'){
                        moveScore = 1003;
                        if (!inCheck(iPlayer, moveScore, ourPos, endPos, NULL)){
                            if (bMove){
                                if ((ourPos.col > 0 && cboard[iPlayer * (BOARD_RANKS-1)][ourPos.col - 1] == 'B') || \
                                    (ourPos.col < (BOARD_FILES - 1) && cboard[iPlayer * (BOARD_RANKS - 1)][ourPos.col + 1] == 'B')){
                                    // frees a bishop
                                    moveScore += 2;
                                }
                                if (hiScore < moveScore){
                                    sMove = createProspectiveMove(hiScore, moveScore, ourPos, endPos);
                                }
                            }
                            else
                                return false;
                        }
                    }
                }
            }
            // diagonals, taking a piece
            // try left, then right:
            for (int epDir = -1; epDir < 2; epDir += 2){
                if ((ourPos.col + epDir) < BOARD_FILES){
                    endPos.row = ourPos.row + ourDir;
                    endPos.col = ourPos.col + epDir;
                    position epPos(ourPos.row, endPos.col);
                    if (isOurs(endPos, 1 - iPlayer) && !(isOurs(epPos, 1 - iPlayer) && cboard[ourPos.row][endPos.col] == 'E')){
                        // if we call is_ours with the other player, it's effectively an "isTheirs" function
                        // this comparison also makes sure we don't steal the en passant case below
                        moveScore = 1002;
                        if (!inCheck(iPlayer, moveScore, ourPos, endPos, NULL)){
                            if (bMove){
                                if (toEndline(endPos, iPlayer))
                                    moveScore += 4;
                                if (hiScore < moveScore){
                                    sMove = createProspectiveMove(hiScore, moveScore, ourPos, endPos);
                                }
                            }
                            else
                                return false;
                        }
                    }
                }
            }

            // en passant - possibly still taking a piece
            // diagonals, taking a piece
            // try left, then right:
            for (int epDir = -1; epDir < 2; epDir += 2){
                if (((ourPos.col + epDir) < BOARD_FILES)){
                    endPos.row = ourPos.row + ourDir;
                    endPos.col = ourPos.col + epDir;
                    position epPos(ourPos.row, endPos.col);
                    if (epPos.isValid() && !isOurs(endPos, iPlayer)){
                        // here it gets difficult. It isn't enough to pass in the normal diagonal move,
                        // because the pawn we take en passant may also affect check. So we will
                        // create a temporary modified board for this single circumstance and
                        // pass a reference to it to check in, AND the diagonal move.
                        if (isOurs(epPos, 1-iPlayer) && cboard[ourPos.row][endPos.col] == 'E'){
                            gameobject gm(*this); // proposed move
                            // now replace the captured en passant pawn
                            gm.cboard[ourPos.row][endPos.col] = '.';
                            for (size_t index = 0; index < gm.players[1-iPlayer].positions.size(); ++index ){
                                if (gm.players[1 - iPlayer].positions[index] == epPos)
                                    gm.players[1 - iPlayer].positions[index] = position();
                            }
                            // if it turns out the position we move the pawn into also contains an opposing piece, this will be handled explicity in in_check.
                            moveScore = 1004;
                            if (!inCheck(iPlayer, moveScore, ourPos, endPos, &gm)){
                                if (bMove){
                                    if (hiScore < moveScore){
                                        sMove = createProspectiveMove(hiScore, moveScore, ourPos, endPos);
                                    }
                                }
                                else
                                    return false;
                            }
                        }
                    }
                }
            }
            // finally, promotion doesn't matter for determining check-mate, as a piece of any type can't move until the next go anyway
        }
        else if (p == 'N'){
            // the eight combinations:
            for (int y = -2; y < 3; ++y){
                for (int x = -2; x < 3; ++x){
                    if (x == 0 || y == 0) continue;
                    endPos.row = ourPos.row;
                    endPos.col = ourPos.col;
                    if (abs(x) != abs(y)){
                        endPos.row = endPos.row + y;
                        endPos.col = endPos.col + x;
                        if (!endPos.isValid())
                            continue; // out of bounds - try next combo
                        if (!isOurs(endPos, iPlayer)){
                            moveScore = 1001;
                            if (!inCheck(iPlayer, moveScore, ourPos, endPos, NULL)){
                                if (bMove){
                                    /*pastmove pmv(ourPos, endPos);
                                    if (players[iPlayer].pieceMoves[i].pastmoveSet[tuple(pmv)] > 0)
                                        pm2 = tuple(pmv);
                                    for (l in players[iPlayer].pieceMoves[i].pastmoveSet.elements()){
                                        s1, s2, s3, s4 = l;
                                        if (ourPos.row == s1 && ourPos.col == s2 && endPos.row == s3 && endPos.col == s4){
                                            moveScore -= players[iPlayer].pieceMoves[i].pastmoveSet[l] * 3; // i.e. penalise repeat moves
                                            break;
                                        }
                                    }*/ // TO DO
                                    if (offBackline(ourPos,endPos, 2, iPlayer))
                                        moveScore += 2;
                                    if (hiScore < moveScore){
                                        sMove = createProspectiveMove(hiScore, moveScore, ourPos, endPos);
                                    }
                                }
                                else
                                    return false;
                            }
                        }
                    }
                }
            }
        }
        else if (basicType(p) == 'R' || p == 'B' || basicType(p) == 'K' || p == 'Q'){
            if (!scoreDirectionsLoop(i, p, bMove, sMove, hiScore, ourPos, iPlayer)){
                // False indicates only that we are not scoring - i.e. there is a valid move, therefore no further processing required
                return false;
            }
        }
    }
    if (hiScore > 0 && bMove){
        movePiece(sMove, iPlayer, true); // computer makes a move for us
        if (bConsole) cout << sMove << endl;
    }
    return true;
}

void gameobject::printBoard(){
#ifdef __GUI__
#ifndef __DEBUG__
return;
#endif
#endif
    cout << setfill('_') << setw(65) << "" << endl;
    if (!bImages) cout << endl << "Type 'load', 'save', 'new', 'quit', or a move e.g. (a1-b2)" << endl << "Type 'help' for a full list of instructions." << endl << endl;

    string y = "ABCDEFGH";
    cout << "     A  B  C  D  E  F  G  H" << endl;
    cout << "   " << (char)201 << setw(23) << setfill((char)205) << "" << (char) 187 << endl;

    for (size_t row = 0; row < BOARD_RANKS; ++row){
        cout << " " << (BOARD_RANKS - row) << " " << (char)186; // unfortunately I wrote this game without realising ranks are ordered from the bottom up
        for (size_t j = 0; j < BOARD_FILES; ++j){
            // do a conversion of the board now:
            char p = cboard[row][j];
            char cdir = (char) 30; // down (player 1)
            position pos(row,j);
            p = basicType(p);
            for (size_t i = 0; i < players[0].positions.size(); ++i){
                if (players[0].positions[i] == pos){
                    cdir = (char) 31; // up (player 2)
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

// copy constructor
player::player(const player& pl){
    for (vector<position>::const_iterator itr = pl.positions.begin(); itr < pl.positions.end(); ++itr)
        positions.push_back(*itr);
    for (vector<pastmoves>::const_iterator itr = pl.pieceMoves.begin(); itr < pl.pieceMoves.end(); ++itr)
        pieceMoves.push_back(*itr);
    name = pl.name;
    pieces = pl.pieces;
    bComputer = pl.bComputer;
    lastPieceMoved = 0;
}

//-----------------------------------------------------------------------------------------
//  GUI
//-----------------------------------------------------------------------------------------

void ChessGUI::initialise(){
    //initialise, otherwise errors on loading QML:
    displayBoardImages();
    pView->rootContext()->setContextProperty("appState", "");
    pView->rootContext()->setContextProperty("statusText", pGame->gameDetails().c_str());
    pView->rootContext()->setContextProperty("xTransition", "50");
    pView->rootContext()->setContextProperty("yTransition", "50");
    pView->rootContext()->setContextProperty("zTransition", "-2");
    pView->rootContext()->setContextProperty("opacityTransition", "1");
    pView->rootContext()->setContextProperty("imageTransition", "pawn.ico");
    bProcessing = false;
}

void ChessGUI::boardClick(int x, int y){
    bProcessing = true;
    // 0-indexed from top left
    size_t cl = int(x/BUTTON_WIDTH);
    size_t rw = int(y/BUTTON_WIDTH);

    //is this ours?
    position pos = position(rw, cl);
    if (!pGame->isOurs(pos, pGame->cPlayer) && selectionState == NO_SELECTION){
    }
    else if (selectionState == NO_SELECTION){
        selectionState = FROM_SELECTED;
        pGame->GUImoveStart = position(rw, cl);
        size_t i;
        for (i = 0; i < pGame->players[pGame->cPlayer].positions.size(); ++i)
            if (pGame->players[pGame->cPlayer].positions[i] == pGame->GUImoveStart) break;
        i = i + (pGame->cPlayer*16) + 1;
        setOpacity(i, "0.3");
    }
    else if (selectionState == FROM_SELECTED){
        if (pGame->GUImoveStart.row == rw && pGame->GUImoveStart.col == cl){
            // deselect the move

            size_t i;
            for (i = 0; i < pGame->players[pGame->cPlayer].positions.size(); ++i)
                if (pGame->players[pGame->cPlayer].positions[i] == pGame->GUImoveStart) break;
            i = i + (pGame->cPlayer*16) + 1;
            setOpacity(i, "1");

            pGame->GUImoveStart.row = -1;
            pGame->GUImoveStart.col = -1;
            selectionState = NO_SELECTION;

            //string strImageOpacity = "opacity" + getBoardRef(cl,rw);
            //pView->rootContext()->setContextProperty(strImageOpacity.c_str(), "1");
        }
        else{
            selectionState = TO_SELECTED;
            pGame->GUImoveEnd = position(rw, cl);
            if (move()){                
                size_t i;
                for (i = 0; i < pGame->players[pGame->cPlayer].positions.size(); ++i)
                    if (pGame->players[pGame->cPlayer].positions[i] == pGame->GUImoveStart) break;
                i = i + (pGame->cPlayer*16) + 1;
                setOpacity(i, "1");
                displayMove();
                return;
            }
            else{
                selectionState = FROM_SELECTED;
                pGame->GUImoveEnd = position();// null
            }
        }
    }
    bProcessing = false;
}

void ChessGUI::executeMove(string cmd){
    // This will handle those aspects common to both human and computer moves, including changing player
    size_t score = 0;
    pGame->cPlayer = 1 - pGame->cPlayer;
    string labelText = pGame->gameDetails();
    if (pGame->inCheck(pGame->cPlayer, score, position(), position(), NULL)){
        labelText += "\nCHECK";
    }
    if (pGame->inCheckMate(pGame->cPlayer, false)){
        string msg = "CHECK MATE : " + pGame->players[1 - pGame->cPlayer].name + " wins";
        labelText += "\n" + msg;
        showMessage(msg);
        pGame->bGameOver = true;
        pGame->players[0].bComputer = false;
        pGame->players[1].bComputer = false;
    }
    else if (pGame->isStalemate()){
        labelText += "\nThe game is A DRAW - the same move has been made three times now.";
        pGame->bGameOver = true;
        pGame->players[0].bComputer = false;
        pGame->players[1].bComputer = false;
    }
    else if (pGame->cPlayer == 1){
        pGame->turn += 1;
    }
    if (pGame->turn > MAX_TURNS){
        char t[4];
        itoa(MAX_TURNS, t, 10);
        string msg = "DRAW - ";
        msg += t;
        msg += " moves have been made without victory." ;
        showMessage(msg);
        pGame->bGameOver = true;
        pGame->players[0].bComputer = false;
        pGame->players[1].bComputer = false;
    }
    labelText += "\n\n(" + pGame->players[1 - pGame->cPlayer].name + "'s move: " + cmd + ")";
    pView->rootContext()->setContextProperty("statusText", labelText.c_str());
}

bool ChessGUI::move(){
    if (!pGame->bGameOver){
        bProcessing = true;
        stringstream ss;
        string xyString;

        string cmd = pGame->createMove(pGame->GUImoveStart, pGame->GUImoveEnd);
        if (pGame->validateMove(cmd, pGame->cPlayer)){
            pGame->movePiece(cmd, pGame->cPlayer);
            executeMove(cmd);
            return true;
        }
    }
    return false;
    //invalid move
}

void ChessGUI::setOpacity(size_t iPiece, string opacityVal) const{
    if (bImages){
        string opacityStr = "opacity";
        string colorStr = "color";
        char cp[4];
        itoa(iPiece, cp, 10);
        opacityStr += cp;
        colorStr += cp;
        string colorVal = "";

        pView->rootContext()->setContextProperty(opacityStr.c_str(), opacityVal.c_str());
    }
}

void ChessGUI::setPieceImage(char p, position pos, size_t iPiece, bool bTransition) const{
    // initialise transition piece
    if (bImages){
        stringstream ss, ss2;
        string sx, sy;
        ss << ((pos.col + 1) * 50);
        ss >> sx;
        ss2 << ((pos.row + 1) * 50);
        ss2 >> sy;
        if (bTransition && pGame->cPlayer == 1)// just switched to player 2 in executeMove
            p = tolower(p); // To signal this is black, as setSquareImage has no context
        string strImage = setSquareImage(p, pos.row, pos.col);

        if (bTransition){
            pView->rootContext()->setContextProperty("imageTransition", strImage.c_str());
            pView->rootContext()->setContextProperty("xTransition", sx.c_str());
            pView->rootContext()->setContextProperty("yTransition", sy.c_str());
            pView->rootContext()->setContextProperty("zTransition", "2");
        }
        else{
            string imagePieceStr = "imagePiece";
            char cp[4];
            itoa(iPiece, cp, 10);
            imagePieceStr += cp;
            string sxID = "xPiece";
            string syID = "yPiece";
            string szID = "zPiece";
            sxID += cp;
            syID += cp;
            szID += cp;
            pView->rootContext()->setContextProperty(imagePieceStr.c_str(), strImage.c_str());
            pView->rootContext()->setContextProperty(sxID.c_str(), sx.c_str());
            pView->rootContext()->setContextProperty(syID.c_str(), sy.c_str());
            if (pos.isValid())
                pView->rootContext()->setContextProperty(szID.c_str(), "0");
            else{
                pView->rootContext()->setContextProperty(szID.c_str(), "-5");
                pView->rootContext()->setContextProperty(sxID.c_str(), "50");
                pView->rootContext()->setContextProperty(syID.c_str(), "50");
            }
        }
    }
}

void ChessGUI::displayMove(){
    if (bImages){
        // 1. for Transition
        char p = pGame->cboard[pGame->GUImoveEnd.row][pGame->GUImoveEnd.col];// because we have already moved the background data
        setPieceImage(p, pGame->GUImoveStart,0, true);

        // 2. This does the normal move, before the transition (i.e. create a space)
        size_t i;
        for (i = 0; i < pGame->players[1-pGame->cPlayer].positions.size(); ++i)
            if (pGame->players[1-pGame->cPlayer].positions[i] == pGame->GUImoveEnd) break;
        string zPieceStr = "zPiece";
        char cp[4];
        itoa(i + (1-pGame->cPlayer)*16 + 1, cp, 10);
        zPieceStr += cp;
        pView->rootContext()->setContextProperty(zPieceStr.c_str(), "-1");

        // 3. Set off the transition:
        stringstream ss, ss2;
        string sx, sy;
        ss << ((pGame->GUImoveEnd.col + 1)* 50);
        ss >> sx;
        ss2 << ((pGame->GUImoveEnd.row + 1) * 50);
        ss2 >> sy;
        pView->rootContext()->setContextProperty("transitionTargetX", sx.c_str());
        pView->rootContext()->setContextProperty("transitionTargetY", sy.c_str());
        pView->rootContext()->setContextProperty("zTransition", "1");
        pView->rootContext()->setContextProperty("opacityTransition", "1");
        string strState = "appState";
        pView->rootContext()->setContextProperty(strState.c_str(), "EndMoveState");

        // 4. Next we restore the z order of the moved image, and remove the destination image.
        // But we need to wait for a notification the transition is complete before we do this.
        // This is handled in completeMove().
    }
}

void ChessGUI::completeMove(){
    // triggered by the completion of the transition effect.
    pView->rootContext()->setContextProperty("zTransition", "-1");
    char p = pGame->cboard[pGame->GUImoveEnd.row][pGame->GUImoveEnd.col];
    string strImageName = setSquareImage(p, pGame->GUImoveEnd.row, pGame->GUImoveEnd.col);

    if (bImages){
        size_t i;
        for (i = 0; i < pGame->players[1-pGame->cPlayer].positions.size(); ++i)
            if (pGame->players[1-pGame->cPlayer].positions[i] == pGame->GUImoveEnd) break;
        // any opposing piece?
        setOpacity(i + (1-pGame->cPlayer)*16 + 1 , "1");
        setPieceImage(pGame->players[1-pGame->cPlayer].pieces[i], pGame->GUImoveEnd, i + (1-pGame->cPlayer)*16 + 1, false );
        if (pGame->getTakenIndex() < 16){
            setPieceImage(pGame->players[pGame->cPlayer].pieces[pGame->getTakenIndex()], position(), pGame->getTakenIndex() + (pGame->cPlayer)*16 + 1, false );
            pView->rootContext()->setContextProperty("zTransition", "-2");
            pGame->clearTakenIndex();
        }
    }

    selectionState = NO_SELECTION;
    pGame->GUImoveStart.row = -1;
    pGame->GUImoveStart.col = -1;
    pGame->GUImoveEnd.row = -1;
    pGame->GUImoveEnd.col = -1;

    string strState = "appState";
    pView->rootContext()->setContextProperty(strState.c_str(), "");

    bProcessing = false;
}

string ChessGUI::getBoardRef(int x, int y) const{
    // x and y 0-indexed, from top
    string ref;
    stringstream ss;
    ss << "R" << (BOARD_RANKS - y) << "F" << (x + 1);
    ss >> ref;
    return ref;
    // output 1-indexed, from bottom
}

string ChessGUI::setSquareImage(char& p, int row, int col) const{
    string strImage;
    position pos(row,col);
    char btp = pGame->basicType(p);

    for (position each: pGame->players[0].positions){
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
        if (btp == 'p' && bImages) strImage = "pawnd";
        else if (btp == 'P' && bImages) strImage = "pawn";
        else if (btp == 'b' && bImages) strImage = "bishopd";
        else if (btp == 'B' && bImages) strImage = "bishop";
        else if (btp == 'r' && bImages) strImage = "rookd";
        else if (btp == 'R' && bImages) strImage = "rook";
        else if (btp == 'n' && bImages) strImage = "knightd";
        else if (btp == 'N' && bImages) strImage = "knight";
        else if (btp == 'k' && bImages) strImage = "kingd";
        else if (btp == 'K' && bImages) strImage = "king";
        else if (btp == 'q' && bImages) strImage = "queend";
        else if (btp == 'Q' && bImages) strImage = "queen";
    }
    if (strImage == "light" || strImage == "dark")
        strImage += "square.gif";
    else
        strImage += ".ico";
    p = alt_txt;
    return strImage;
}

void ChessGUI::displayBoardImages() const{
    //create a list of buttons
    if (bImages){
        for (size_t player = 0; player < 2; ++player){
            for (size_t piece = 0; piece < pGame->players[player].pieces.length(); ++piece ){
                char cp = pGame->players[player].pieces[piece];
                if (!pGame->players[player].positions[piece].isValid()) cp = ' ';
                setPieceImage(cp, pGame->players[player].positions[piece], piece + (player*16) + 1, false );
                setOpacity(piece + (player*16) + 1 , "1");
            }
        }
    }
}

void ChessGUI::recreateBoard() const{
    displayBoardImages();
    pView->rootContext()->setContextProperty("statusText", pGame->gameDetails().c_str());
}

void ChessGUI::showMessage(string msg) const{
    QMessageBox msgBox;
    msgBox.setText(msg.c_str());
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

// Message handlers

void ChessGUI::humanClick()const{
    if (pGame->players[1-pGame->cPlayer].bComputer){
        pGame->players[1-pGame->cPlayer].bComputer = false;
        string msg = "You now have control of ";
        msg += pGame->players[1-pGame->cPlayer].name;
        showMessage(msg);
    }
    else{
        string msg = "You already have control of ";
        msg += pGame->players[1-pGame->cPlayer].name;
        showMessage(msg);
    }
}

void ChessGUI::computerClick(){
    if (!pGame->players[pGame->cPlayer].bComputer){
        pGame->players[pGame->cPlayer].bComputer = true;
        string msg = "The computer now has control of ";
        msg += pGame->players[pGame->cPlayer].name;
        showMessage(msg);
        bProcessing = true;
        play();
    }
    else{
        string msg = "The computer already has control of ";
        msg += pGame->players[pGame->cPlayer].name;
        showMessage(msg);
    }
}

void ChessGUI::computerMoveClick(){
    if (bProcessing == true){
        return;
    }
    bProcessing = true;
    play();
}

void ChessGUI::newClick() const{
    pGame->newGame();
    recreateBoard();
}

void ChessGUI::saveClick() const{
    if (!pGame->saveGame()){
        string error = "Game could not be saved.";
        if (bImages)
            showMessage(error);
        else
            cout << error << endl;
    }
}

void ChessGUI::loadClick() const{
    if (pGame->loadGame()){
        recreateBoard();
    }
    else{
        string error = "No saved game was found.";
        if (bImages)
            showMessage(error);
        else
            cout << error << endl;
    }
}

void ChessGUI::play(){
    if (pGame->bGameOver == false){
        pGame->computerTurn(pGame->cPlayer);// finds a move and makes it
        pGame->GUImoveStart = pGame->players[pGame->cPlayer].lastMove.startPos;
        pGame->GUImoveEnd = pGame->players[pGame->cPlayer].lastMove.endPos;
        string cmd = pGame->createMove(pGame->GUImoveStart, pGame->GUImoveEnd);
        bProcessing = true;
        executeMove(cmd);
        displayMove();
    }
}

// State notifications

void ChessGUI::transitionComplete(){
    completeMove();
}

void ChessGUI::moveReady(){
    //showMessage("Move ready");
}

//-----------------------------------------------------------------------------------------
//  Console
//-----------------------------------------------------------------------------------------

string userInput(){
    cout << "\n> ";
    string cmd;
    cin >> cmd;
    transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    return cmd;
}

//-----------------------------------------------------------------------------------------
//  Main
//-----------------------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // QApplication, rather than QGUIApplication, is needed to handle widgets
    QtQuick2ApplicationViewer viewer;

    gameobject game;
    game.newGame();

#ifdef __GUI__
    ChessGUI chessGUI(&game, &viewer);
    chessGUI.initialise();

    viewer.setMainQmlFile(QStringLiteral("qml/Chess/main.qml"));
    viewer.setIcon(QIcon("qml/Chess/chess.ico"));
    viewer.showExpanded();

    // get root object
    QObject *item = viewer.rootObject();

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

    QObject::connect(item, SIGNAL(signalMoveComplete()),
                        &chessGUI, SLOT(transitionCompleteSlot()));

    QObject::connect(item, SIGNAL(signalMoveReady()),
                        &chessGUI, SLOT(moveReadySlot()));

    return app.exec();
#endif
    // console mode goes here...
    cout << "Chess\n";
    cout << "=====\n";
    string cmd;
    while (true){
        size_t score = 0;
        // signals we're not valuing a move for AI (1000+ if so)
        cout << game.players[game.cPlayer].name + "'s turn:\n";

        if (game.turn > MAX_TURNS){
            cout << "\nThe game is A DRAW - " << MAX_TURNS << " moves have been made without victory.\n";
            game.bGameOver = true;
            game.players[0].bComputer = false;
            game.players[1].bComputer = false;
        }

        if (game.players[game.cPlayer].bComputer)
            cmd = "play";
        else
            cmd = userInput();

        // All these calls should be covered by their own exception handling. If they fail, they
        // don't necessarily prevent other options from working, so we don't quit the program.
        if (cmd == "exit") break;
        if (cmd == "new"){
            game.turn = 1;
            game.cPlayer = 1;
            game.newGame();
            game.bGameOver = false;
        }
        else if (cmd == "save")
            game.saveGame();
        else if (cmd == "open"){
            if (game.loadGame())
                game.bGameOver = false;
            else
                cout << "\nNo saved game was found.\n\n";
        }
        else if (cmd == "board")
            game.printBoard();
        else if (cmd == "checkcheck"){ // for debug only
            if (game.inCheck(game.cPlayer, score, position(), position(), NULL ))
                cout << "\nCHECK\n\n";
            else
                cout << "\nNo check\n\n";
        }
        else if (cmd == "computer"){
            if (!game.players[game.cPlayer].bComputer){
                game.players[game.cPlayer].bComputer = true;
                cout << "\nThe computer now has control of this player.\n\n";
            }
            else
                cout << "\nThe computer already has control of this player.\n\n";
        }
        else if (cmd == "me"){
            if (game.players[1-game.cPlayer].bComputer == true){
                game.players[game.cPlayer].bComputer = false;
                cout << "\nYou now have control of the other player.\n\n";
            }
            else
                cout << "\nYou already have control of the other player.\n\n";
        }
        else if (cmd == "play"){
            game.computerTurn(game.cPlayer); // finds a move and makes it
            game.cPlayer = !game.cPlayer;
            if (game.inCheck(game.cPlayer, score, position(), position(), NULL))
                cout << "\nCHECK\n";
            if (game.inCheckMate(game.cPlayer, false)){
                cout << "CHECK MATE\n";
                cout << game.players[1-game.cPlayer].name + " wins\n";
                game.bGameOver = true;
                game.players[0].bComputer = false;
                game.players[1].bComputer = false;
            }
            else if (game.isStalemate()){
                cout << "\nThe game is A DRAW - the same move has been made three times now.\n";
                game.bGameOver = true;
                game.players[0].bComputer = false;
                game.players[1].bComputer = false;
            }
            else if (game.cPlayer == 1)
                game.turn += 1;
        }
        else if (!game.bGameOver){
            // maybe this is an actual move then!
            if (game.validateMove(cmd, game.cPlayer)){
                cout << cmd << endl;
                game.movePiece(cmd, game.cPlayer);
                game.cPlayer = 1 - game.cPlayer;
                if (game.inCheck(game.cPlayer, score, position(), position(), NULL))
                    cout << "CHECK\n";
                if (game.inCheckMate(game.cPlayer, false)){
                    cout << "\nCHECK MATE\n";
                    cout << game.players[1-game.cPlayer].name + " wins\n";
                    game.bGameOver = true;
                    game.players[0].bComputer = false;
                    game.players[1].bComputer = false;
                }
                else if (game.isStalemate()){
                    cout << "\nThe game is A DRAW - the same move has been made three times now.\n";
                    game.bGameOver = true;
                    game.players[0].bComputer = false;
                    game.players[1].bComputer = false;
                }
                else if (game.cPlayer == 1)
                    game.turn += 1;
            }
        }
    }
    return 0;
}

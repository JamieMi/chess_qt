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

enum selection {NO_SELECTION, FROM_SELECTED, TO_SELECTED};
selection selectionState = NO_SELECTION;

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
    turn = 1;
    cPlayer = 1;
    bGameOver = false;
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
    start.row = BOARD_RANKS - startRow; // internally, the rows are ordered here from the top down, unlike ranks
    start.col = startCol -1;
    end.row = BOARD_RANKS - newRow;
    end.col = newCol -1;
}

void gameobject::printPieces() const{
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

bool gameobject::validateMove(string cmd, size_t iPlayer){
    if (cmd.size() != 5){
        cout << "\nThat move is not allowed.\n" << endl;
        return false;
    }

    position startPos, endPos;
    getMoveCoords(cmd, startPos, endPos);

    // (note: if the index < 0, size_t returns the max for a size_t)
    if ((startPos.col >= BOARD_FILES) ||
        (startPos.row >= BOARD_RANKS) ||
        (endPos.col >= BOARD_FILES) ||
        (endPos.row >= BOARD_RANKS)) {
        cout << endl << "That move is not allowed." << endl;
        return false;
    }
    int ourPiece = -1;
    int theirPiece = -1;
    if (cboard[startPos.row][startPos.col] == '.'){
        cout << "There's no piece to move from there." << endl;
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
            cout << "That's not your piece to move..." << endl;
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
            // cout << "You've taken a piece..." << endl;
        }
        else
        {
            cout << "That position is occupied by one of your pieces already." << endl;
            return false;
        }
    }

    char piece = cboard[startPos.row][startPos.col];
    char destination = cboard[endPos.row][endPos.col];
    size_t dummyScore = 0;

    // pawns
    //piece = toupper(piece);
    if (piece == 'P' || piece == 'E'){
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
            if( (abs(int(endPos.col - startPos.col)) == 1 && destination != '.') ||
                (abs(int(endPos.col - startPos.col)) == 1 && destination == '.' && cboard[endPos.row + (iPlayer*2 -1)][endPos.col] == 'E')  ){
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
        if( (abs(int(endPos.col - startPos.col)) == 1 && abs(int(endPos.row - startPos.row)) == 2)  ||
            (abs(int(endPos.col - startPos.col)) == 2 && abs(int(endPos.row - startPos.row)) == 1) ){
            if (bDestEmpty || (theirPiece != -1))
                return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
            else
                return false;
        }
        else
            return false;
    }

    // rook
    if (piece == 'R' || piece == 'S'){
        if (endPos.col == startPos.col || endPos.row == startPos.row){ // can't both be equal anyway
            // direction is OK, at least. Now check what's in the way...
            size_t distance = max( abs(int(endPos.col - startPos.col)), abs(int(endPos.row - startPos.row)) );
            if (checkPath(startPos, endPos, distance, bDestEmpty, theirPiece, iPlayer, false))
                return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
        }
        return false;
    }

    // bishop
    if (piece == 'B'){
        if( abs(int(endPos.col - startPos.col)) == abs(int(endPos.row - startPos.row)) ){ // horizontal distance = vertical
            // direction is OK, at least. Now check what's in the way...
            size_t distance = abs(int(endPos.col - startPos.col));
            if (checkPath(startPos, endPos, distance, bDestEmpty, theirPiece, iPlayer, false))
                return !inCheck(iPlayer, dummyScore, startPos, endPos, NULL); // can't leave ourself in check
        }
        return false;
    }

    // queen or king
    if (piece == 'Q' || piece == 'K' || piece == 'L'){
        if( abs(int(endPos.col - startPos.col)) == abs(int(endPos.row - startPos.row)) ||	// horizontal distance = vertical, or
            (endPos.col == startPos.col || endPos.row == startPos.row) ){					// vertical or horizontal
            // direction is OK, at least. Now check what's in the way...
            size_t distance = max(  abs(int(endPos.col - startPos.col)) ,													// if diagonal
                                    max(abs(int(endPos.col - startPos.col)), abs(int(endPos.row - startPos.row))) );	// if h/v
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
                //    - Oh bloody hell. Modify check_path to call in_check for every square it passes through...
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
    bool bStalemate = false;

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
            players[1-iPlayer].positions[i] = pos;
            break;
        }
    }

    // move the piece on the board:
    char piece = cboard[startPos.row][startPos.col];
    char origDest = cboard[endPos.row][endPos.col];
    cboard[startPos.row][startPos.col] = '.';
    assert (cboard[endPos.row][endPos.col] != 'K' && cboard[endPos.row][endPos.col] != 'L'); // we should never have reached this point
    cboard[endPos.row][endPos.col] = piece;
    cout << "Moved." << endl;
    players[iPlayer].lastPieceMoved = ourIndex;

    // if the king is moving from its start position...
    if (players[iPlayer].pieces[ourIndex] == 'L'){
        players[iPlayer].pieces[ourIndex] = 'K';
        cboard[endPos.row][endPos.col] = 'K';
        // is this a castling?
        if (abs(int(startPos.col) - int(endPos.col)) == 2){
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
        cout << "Pawn captured, en passant." << endl;
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

            cout << "The pawn has been promoted." << endl;
        }
        else
        {
            cout << "The pawn has been promoted. What piece do you want to convert it to? You may choose a knight, rook, bishop or queen."<< endl;
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
    }

    printBoard();
    return;
}

bool gameobject::inCheck(const size_t& player, size_t& finalHiScore, position startPos, position endPos, gameobject* gm)const{
    return false;
    // TO DO
}

bool gameobject::checkPath(const position& startPos, const position& endPos, const int& distance, const bool& bDestEmpty, const int& theirPiece, const size_t& iPlayer, bool bCheckCheck) const{
    return true;
    // TO DO
}

void gameobject::printBoard(){
#ifdef __GUI__
#ifndef __DEBUG__
return;
#endif
#endif
    cout << setfill('_') << setw(65) << "" << endl;
#ifndef __DEBUG
    cout << endl << "Type 'load', 'save', 'new', 'quit', or a move e.g. (a1-b2)" << endl << "Type 'help' for a full list of instructions." << endl << endl;
#endif
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
    return (ourPos.row == player*7) && (abs((int)endPos.row - (int)ourPos.row) >= (int)distance);
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
    if (bImages)
    {
        for (int r = 1; r < (BOARD_RANKS + 1); ++r){
            for (int c = 1; c < (BOARD_FILES + 1); ++c){
                string ref = getBoardRef(c, r);
                string strImageName = "image" + ref;
                string strImageOpacity = "opacity" + ref;
                pView->rootContext()->setContextProperty(strImageName.c_str(), "");
                pView->rootContext()->setContextProperty(strImageOpacity.c_str(), "1");
            }
        }
    }
}

void ChessGUI::boardClick(int x, int y){
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
        string strImageOpacity = "opacity" + getBoardRef(cl,rw);
        pView->rootContext()->setContextProperty(strImageOpacity.c_str(), "0.6");
    }
    else if (selectionState == FROM_SELECTED){
        if (pGame->GUImoveStart.row == rw && pGame->GUImoveStart.col == cl){
            // deselect the move
            pGame->GUImoveStart.row = -1;
            pGame->GUImoveStart.col = -1;
            selectionState = NO_SELECTION;
            string strImageOpacity = "opacity" + getBoardRef(cl,rw);
            pView->rootContext()->setContextProperty(strImageOpacity.c_str(), "1");
        }
        else{
            selectionState = TO_SELECTED;
            pGame->GUImoveEnd = position(rw, cl);
            if (move()){
                string strImageOpacity = "opacity" + getBoardRef(pGame->GUImoveStart.col,pGame->GUImoveStart.row);
                pView->rootContext()->setContextProperty(strImageOpacity.c_str(), "1");
                displayMove();
            }
            else{
                selectionState = FROM_SELECTED;
                pGame->GUImoveEnd = position();// null
            }
        }
    }
}

bool ChessGUI::move(){
    if (!pGame->bGameOver){
        stringstream ss;
        string xyString;

        string cmd = pGame->createMove(pGame->GUImoveStart, pGame->GUImoveEnd);
        if (pGame->validateMove(cmd, pGame->cPlayer)){
            pGame->movePiece(cmd, pGame->cPlayer);
            //executeMove(cmd); // TO DO. This will handle those aspects common to both human and computer moves, including changing player
            return true;
        }
    }
    return false;
    //invalid move
}

void ChessGUI::setStateOrigin(char p, position pos){
    // initialise stateOrigin
    if (bImages){
        string ref = getBoardRef(pos.col, pos.row);
        stringstream ss, ss2;
        string sx, sy;
        ss << ((pos.col + 1) * 50);
        ss >> sx;
        ss2 << ((pos.row + 1) * 50);
        ss2 >> sy;
        string strImage = setSquareImage(p, pos.row, pos.col);
        pView->rootContext()->setContextProperty("imageStateOrigin", strImage.c_str());
        pView->rootContext()->setContextProperty("xStateOrigin", sx.c_str());
        pView->rootContext()->setContextProperty("yStateOrigin", sy.c_str());
        pView->rootContext()->setContextProperty("zStateOrigin", "2");
    }
}

void ChessGUI::displayMove(){
    if (bImages){
        // 1. for stateOrigin
        char p = pGame->cboard[pGame->GUImoveEnd.row][pGame->GUImoveEnd.col];// because we have already moved the background data
        setStateOrigin(p, pGame->GUImoveStart);

        // 2. This does the normal move, before the transition (i.e. create a space)
        p = pGame->cboard[pGame->GUImoveStart.row][pGame->GUImoveStart.col];
        string strImageName = setSquareImage(p, pGame->GUImoveStart.row, pGame->GUImoveStart.col);
        string strImage = "image" + getBoardRef(pGame->GUImoveStart.col, pGame->GUImoveStart.row);
        pView->rootContext()->setContextProperty(strImage.c_str(), strImageName.c_str());

        // 3. Set off the transition:
        stringstream ss, ss2;
        string sx, sy;
        ss << ((pGame->GUImoveEnd.col + 1)* 50);
        ss >> sx;
        ss2 << ((pGame->GUImoveEnd.row + 1) * 50);
        ss2 >> sy;
        string strX = "transitionTargetX";
        pView->rootContext()->setContextProperty(strX.c_str(), sx.c_str());
        string strY = "transitionTargetY";
        pView->rootContext()->setContextProperty(strY.c_str(), sy.c_str());
        string strState = "appState";
        pView->rootContext()->setContextProperty(strState.c_str(), "EndMoveState");

        // 4. Next we restore the z order of the moved image, and replace the destination image.
        // But we need to wait for a notification the transition is complete before we do this.
        // This is handled in completeMove().
    }
}

void ChessGUI::completeMove(){
    // triggered by the completion of the transition effect.
    pView->rootContext()->setContextProperty("zStateOrigin", "-1");

    char p = pGame->cboard[pGame->GUImoveEnd.row][pGame->GUImoveEnd.col];
    string strImageName = setSquareImage(p, pGame->GUImoveEnd.row, pGame->GUImoveEnd.col);

    if (bImages){
        string strImage = "image" + getBoardRef(pGame->GUImoveEnd.col, pGame->GUImoveEnd.row);
        pView->rootContext()->setContextProperty(strImage.c_str(), strImageName.c_str());
    }

    selectionState = NO_SELECTION;
    pGame->GUImoveStart.row = -1;
    pGame->GUImoveStart.col = -1;
    pGame->GUImoveEnd.row = -1;
    pGame->GUImoveEnd.col = -1;

    string strState = "appState";
    pView->rootContext()->setContextProperty(strState.c_str(), "");
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
    position pos(row,
                 col);
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

void ChessGUI::displayBoardImages() const{
    //create a list of buttons
    if (bImages){
        for (int r = 1; r < (BOARD_RANKS + 1); ++r){
            for (int c = 1; c < (BOARD_FILES + 1); ++c){
                char p = pGame->cboard[r-1][c-1];
                string strImage = setSquareImage(p, r-1, c-1);
                string strImageName = "image" + getBoardRef(c-1, r-1);
                pView->rootContext()->setContextProperty( strImageName.c_str(), strImage.c_str());
            }
        }
    }
    else{
        // TO DO: bother to create a text alternative?
    }
}

void ChessGUI::showMessage(string msg) const{
    QMessageBox msgBox;
    msgBox.setText(msg.c_str());
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

// Stubs

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

// State notifications

void ChessGUI::transitionComplete(){
    completeMove();
}

void ChessGUI::moveReady(){
    //showMessage("Move ready");
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
    ChessGUI chessGUI(&game, &viewer);

    //chessGUI.initialise(); // TO DO - would be better

    //initialise, otherwise errors on loading QML:
    for (int r = 0; r < BOARD_RANKS; ++r){
        for (int c = 0; c < BOARD_FILES; ++c){
            // TO DO: could probably adapt setStateOrigin() to handle all this now
            string ref = chessGUI.getBoardRef(c, r);
            string strImageName = "image" + ref;
            string strImageOpacity = "opacity" + ref;
            stringstream ss, ss2;
            string sx, sy;
            ss << ((c+1) * 50);
            ss >> sx;
            ss2 << ((r+1) * 50);
            ss2 >> sy;
            string strImageX = "x" + ref;
            string strImageY = "y" + ref;
            string strImageZ = "z" + ref;
            if (bImages)
            {
                viewer.rootContext()->setContextProperty(strImageName.c_str(), "");
                viewer.rootContext()->setContextProperty(strImageOpacity.c_str(), "1");
                viewer.rootContext()->setContextProperty(strImageX.c_str(), sx.c_str());
                viewer.rootContext()->setContextProperty(strImageY.c_str(), sy.c_str());
                viewer.rootContext()->setContextProperty(strImageZ.c_str(), "1");
            }
        }
    }

    viewer.rootContext()->setContextProperty("appState", "");
    //initialise stateOrigin
    chessGUI.setStateOrigin('P', position(0,0));

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

    chessGUI.displayBoardImages();

    return app.exec();
}


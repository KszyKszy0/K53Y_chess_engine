#pragma once

#include "bitboard.h"
#include "enums.h"
#include "stateInfo.h"
#include <string>
#include <list>

using namespace std;

class Position
{
    public:
    Bitboard piecesBitboards[16];                                       //All pieces BB look enums.h

    void parseFEN(string fen, Bitboard (&bitboards)[16]);               //Creates position from FEN

    bool STM;

    //Make move doesnt care if its legal
    void makeMove(Move move);

    //Undo move doesnt care if its legal
    void undoMove(Move move);

    //Creates and add state of game to list
    void addState(int pas, int cast, int half, int full, int captureType);

    //Creates and add state of game to list
    void addState(StateInfo state);

    //List of game states
    StateInfo stateInfoList[1000];

    //Array of all pieces types on chessboard
    int piecesArray[64];

    bool isStalemate;

    bool isCheckmate;

    int stateCounter = 0;
};
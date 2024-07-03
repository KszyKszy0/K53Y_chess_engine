#pragma once
#include "bitboard.h"
#include <sstream>
#include "enums.h"
#include "stateInfo.h"
#include <list>
using namespace std;

class Position
{
    public:
    Bitboard piecesBitboards[15];                                       //All pieces BB look enums.h 

    void parseFEN(string fen, Bitboard (&bitboards)[15]);               //Creates position from FEN

    bool STM;

    void makeMove(Move move);
    
    void undoMove();

    list<StateInfo> stateInfoList;

    int piecesArray[64];
};
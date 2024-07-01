#pragma once
#include "bitboard.h"
#include <sstream>
#include "enums.h"

using namespace std;

class Position
{
    public:
    Bitboard piecesBitboards[15];                                       //All pieces BB look enums.h 

    void parseFEN(string fen, Bitboard (&bitboards)[15]);               //Creates position from FEN
};
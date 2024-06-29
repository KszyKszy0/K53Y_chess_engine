#pragma once
#include "bitboard.h"
#include <sstream>
#include "enums.h"

using namespace std;

class Position
{
    public:
    Bitboard piecesBitboards[15];

    void parseFEN(string fen, Bitboard (&bitboards)[15]);
};
#pragma once
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"

class MoveGenerator
{
    public:
    vector<Move> movesList;

    vector<Move> generateMoves(Position pos);

    void addMoves(int startSquare, Bitboard targets); 

    MoveGenerator(BB_utils BBM);

    BB_utils BBManager;
    
};
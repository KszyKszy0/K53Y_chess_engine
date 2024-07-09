#pragma once
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"

class MoveGenerator
{
    public:

    // 0 - quiets, 1 - captures
    void generateTypeMoves(Position pos, Bitboard target, vector<Move> &moveList, int type);

    void addMoves(int startSquare, Bitboard targets, vector<Move>& movesList, Position pos);

    MoveGenerator(BB_utils BBM);

    BB_utils BBManager;

    vector<Move> fullMovesList(Position pos);

};
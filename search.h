#pragma once
#include "position.h"
#include "move.h"
#include "movegen.h"
#include "eval.h"

class Search
{
    public:
    int negamax(int depth, int ply, int alpha, int beta,int color, MoveGenerator& moveGenerator, Position& pos, Evaluator& eval);

    Move search(Position& pos, MoveGenerator& mg, Evaluator& eval);

    Bitboard transpositionCount=0;

    Bitboard nodesCount=0;

    Bitboard matchedTranspositions=0;
};
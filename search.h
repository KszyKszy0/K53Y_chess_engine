#pragma once
#include "position.h"
#include "move.h"
#include "movegen.h"
#include "eval.h"
#include <chrono>

class Search
{
    public:
    int negamax(int depth, int ply, int alpha, int beta,int color, MoveGenerator& moveGenerator, Position& pos, Evaluator& eval, chrono::steady_clock::time_point start);

    Move search(Position& pos, MoveGenerator& mg, Evaluator& eval);

    Bitboard transpositionCount = 0;

    Bitboard nodesCount = 0;

    Bitboard matchedTranspositions = 0;

    Bitboard collisions = 0;

    Move bestMovePrevious;

    int oldEval = 0;

    bool isRepeated(Position& pos);

    int timeLimit;

    bool isCancelled;
};
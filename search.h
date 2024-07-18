#pragma once
#include "position.h"
#include "move.h"
#include "movegen.h"
#include "eval.h"

class Search
{
    public:
    int negamax(int depth, int ply, int alpha, int beta,int color, MoveGenerator& moveGenerator, Position& pos, Evaluator& eval);

    Move search();
};
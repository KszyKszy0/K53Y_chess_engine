#pragma once
#include "position.h"
#include "move.h"
#include "movegen.h"

class Search
{
    public:
    int negamax(int depth, int ply, int alpha, int beta,int color, MoveGenerator& moveGenerator, Position& pos);

    Move search();
};
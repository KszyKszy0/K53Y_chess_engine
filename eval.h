#pragma once
#include "position.h"
#include "bitboard.h"

class Evaluator
{
    public:
    int evaluate(Position& pos);

    int psqtArray[64]=
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 3, 3, 3, 3, 2, 2,
        2, 2, 3, 3, 3, 3, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        1, 1, 0, 0, 0, 0, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0
    };
};
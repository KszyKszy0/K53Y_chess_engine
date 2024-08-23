#pragma once
#include "position.h"
#include "move.h"
#include "movegen.h"
#include "eval.h"
#include <chrono>
#include "moveList.h"

class Search
{
    public:
    int negamax(int depth, int ply, int alpha, int beta,int color, MoveGenerator& moveGenerator, Position& pos, Evaluator& eval, chrono::steady_clock::time_point start);

    int quiescence(int depth, int ply, int alpha, int beta,int color, MoveGenerator& moveGenerator, Position& pos, Evaluator& eval, chrono::steady_clock::time_point start);

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

    Bitboard queiscenceNodes = 0;

    Bitboard quiescenceTT = 0;

    int MVVLVA[12][12]=
    {
        105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
        104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
        103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
        102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
        101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
        100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

        105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
        104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
        103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
        102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
        101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
        100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
    };

    int pvLength[64];

    Move pvTable[64][64];

    void updatePV(Move m, int ply);

};
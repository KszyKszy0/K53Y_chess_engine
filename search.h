#pragma once
#include "position.h"
#include "move.h"
#include "movegen.h"
#include "eval.h"
#include <chrono>
#include "moveList.h"
#include "accumulator.h"


int negamax(int depth, int ply, int alpha, int beta,int color, Position& pos, chrono::steady_clock::time_point start, Accumulator& accum);

int quiescence(int depth, int ply, int alpha, int beta,int color, Position& pos, chrono::steady_clock::time_point start, Accumulator& accum);

Move search(Position& pos, Accumulator& accum);

extern Bitboard transpositionCount;

extern Bitboard nodesCount;

extern Bitboard matchedTranspositions;

extern Bitboard collisions;

extern Move bestMovePrevious;

extern int oldEval;

bool isRepeated(Position& pos);

extern int timeLimit;

extern bool isCancelled;

extern Bitboard queiscenceNodes;

extern Bitboard quiescenceTT;

const int MVVLVA[12][12]=
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

extern int pvLength[64];

extern Move pvTable[64][64];

void updatePV(Move m, int ply);

extern Move killers[64];

extern Move historyHeuristic[64][64];

void savePosition(Position &pos, int negamaxScore);


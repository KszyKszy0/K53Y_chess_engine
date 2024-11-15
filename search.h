#pragma once
#include "position.h"
#include "move.h"
#include "movegen.h"
#include "eval.h"
#include <chrono>
#include "moveList.h"
#include "accumulator.h"

const int MAX_DEPTH = 64;



int negamax(int depth, int ply, int alpha, int beta,int color, Position& pos, chrono::steady_clock::time_point start);

int quiescence(int depth, int ply, int alpha, int beta,int color, Position& pos, chrono::steady_clock::time_point start);

Move search(Position& pos);

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

extern int pvLength[MAX_DEPTH];

extern Move pvTable[MAX_DEPTH][MAX_DEPTH];

void updatePV(Move m, int ply);

extern Move killers[MAX_DEPTH];

extern Move historyHeuristic[MAX_DEPTH][MAX_DEPTH];

void savePosition(Position &pos, int negamaxScore);


#pragma once
#include "position.h"
#include "move.h"
#include "movegen.h"
#include "eval.h"
#include <chrono>
#include "moveList.h"
#include "accumulator.h"

const int MAX_DEPTH = 64;

struct principalVariation
{
    int length = 0;
    Move list[MAX_DEPTH] = {0};
};

struct searchParams
{
    int timeLimit = 0;

    int increment = 0;

    int depthLimit = 0;

    int nodesLimit = 0;

    chrono::steady_clock::time_point start;
};

scoreType negamax(int depth, int ply, scoreType alpha, scoreType beta,int color, Position& pos, principalVariation& PV, searchParams& params);

scoreType quiescence(int depth, int ply, scoreType alpha, scoreType beta,int color, Position& pos, principalVariation& PV, searchParams& params);

Move search(Position& pos, searchParams params);

bool isRepeated(Position& pos);



extern Bitboard transpositionCount;

extern Bitboard nodesCount;

extern Bitboard matchedTranspositions;

extern Bitboard collisions;

extern Bitboard queiscenceNodes;

extern Bitboard quiescenceTT;


extern bool isCancelled;


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

extern Move killers[MAX_DEPTH];

extern Move historyHeuristic[64][64];

//Saves position in savefile with state and its eval 96 + 4 bytes = 100 bytes
void savePosition(std::array<int, INPUT_SIZE> state, float negamaxScore, Position& pos);

//Add state to position datagen list
void saveState(Position &pos);


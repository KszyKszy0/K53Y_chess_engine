#pragma once
#include "bitboard.h"
#include "move.h"


class TTEntry
{
    public:

    //Hash of the position
    Bitboard zorbistKey;

    //Score of the position
    int score;

    //Depth of the position
    //Can be negative since qsearch gives negative depth values
    int depth;

    //Bestmove in the position
    Move bestMove;

    //Type of the node can be
    //PVNODE - EXACT_SCORE simply searched within ab window
    //CUTNODE - LOWER_BOUND we made cutoff so we need to make sure that we use if it also performs cutoff
    //ALLNODE - UPPERBOUND position so bad that none move raises alpha also we need to make sure its score is lower than alpha
    int type;

    TTEntry();

    TTEntry(int scr, int dpth, Move best, int t, Bitboard key);
};

class TranspositionTable
{
    public:

    //Size of the Transposition table currently: 16777216
    Bitboard size = 1ULL << 24;

    //Pointer to transposition table
    TTEntry *transpositionTable;

    //Default constructor
    TranspositionTable();
};
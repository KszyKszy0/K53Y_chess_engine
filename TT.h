#pragma once
#include "bitboard.h"
#include "move.h"
#include "consts.h"


class TTEntry
{
    public:

    //Hash of the position
    Bitboard zorbistKey;

    //Score of the position
    scoreType score;

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

    Bitboard diskSize = 16'000'000;
    //Size of the Transposition table currently: 10MB
    Bitboard size = Bitboard(diskSize / sizeof(TTEntry));

    //Pointer to transposition table
    TTEntry *transpositionTable;

    //Default constructor
    TranspositionTable();

    //Reset
    void ResetTT();
};
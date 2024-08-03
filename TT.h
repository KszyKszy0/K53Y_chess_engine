#pragma once
#include "bitboard.h"
// #include "move.h"


class TTEntry
{
    public:

    Bitboard zorbistKey;
    int score;
    int depth;
    Move bestMove;
    int type;

    TTEntry();

    TTEntry(int scr, int dpth, Move best, int t, Bitboard key);
};

class TranspositionTable
{
    public:

    Bitboard size = 1ULL << 24;

    TTEntry *transpositionTable;

    TranspositionTable();
};
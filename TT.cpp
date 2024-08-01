#include "TT.h"


TTEntry::TTEntry()
{
    score = 0;
    depth = 0;
    bestMove = 0;
    type = 0;
    zorbistKey = 0;
}

TTEntry::TTEntry(int scr, int dpth, Move best, int t, Bitboard key)
{
    score = scr;
    depth = dpth;
    bestMove = best;
    type = t;
    zorbistKey = key;
}

TranspositionTable::TranspositionTable()
{
    transpositionTable = new TTEntry[size];
}
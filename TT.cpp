#include "TT.h"


TTEntry::TTEntry()
{
    score = 0;
    depth = -100;
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

void TranspositionTable::ResetTT()
{
    for(Bitboard i=0; i<size; i++)
    {
        transpositionTable[i].score = 0;
        transpositionTable[i].depth = -100;
        transpositionTable[i].bestMove = 0;
        transpositionTable[i].type = 0;
        transpositionTable[i].zorbistKey = 0;
    }
}
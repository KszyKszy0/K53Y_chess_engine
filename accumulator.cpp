#include "accumulator.h"
#include "nnue.h"
#include "enums.h"
#include "bitboard.h"

void Accumulator::removePiece(int type, int ind)
{
#ifdef NNUE
    //White index calculation
    int whiteAccumIndex = 64*type+ind;

    int blackAccumIndex = 0;

    //For black we need to "swap" piece types so white king is treated as black and so on...
    if(type <= WHITE_KING)
    {
        blackAccumIndex = 64*(type+6)+flipIndex(ind);
    }
    else if(type <= BLACK_KING)
    {
        blackAccumIndex = 64*(type-6)+flipIndex(ind);
    }


    //We only go to half because the second half is enemy accumulator
    for(int j=0; j < (l1_size/2); j++)
    {
        values[WHITE_ACC][j] -= L1_weights[j][whiteAccumIndex];
        values[BLACK_ACC][j] -= L1_weights[j][blackAccumIndex];
    }
#endif
}

void Accumulator::addPiece(int type, int ind)
{
#ifdef NNUE
    //White index calculation
    int whiteAccumIndex = 64*type+ind;

    int blackAccumIndex = 0;

    //For black we need to "swap" piece types so white king is treated as black and so on...
    if(type <= WHITE_KING)
    {
        blackAccumIndex = 64*(type+6)+flipIndex(ind);
    }
    else if(type <= BLACK_KING)
    {
        blackAccumIndex = 64*(type-6)+flipIndex(ind);
    }


    //We only go to half because the second half is enemy accumulator
    for(int j=0; j < (l1_size/2); j++)
    {
        values[WHITE_ACC][j] += L1_weights[j][whiteAccumIndex];
        values[BLACK_ACC][j] += L1_weights[j][blackAccumIndex];
    }
#endif
}

void Accumulator::initAccum(int (&arr)[64])
{
    resetAccum();
    //Loop through array of all pieces
    for(int i=0; i < 64; i++)
    {
        if(arr[i] <= BLACK_KING)
        {
            addPiece(arr[i],i);
        }
    }
}

void Accumulator::resetAccum()
{
#ifdef NNUE
    //Loop through array of all pieces
    for(int i=0; i < (l1_size/2); i++)
    {
        values[0][i] = 0;
        values[1][i] = 0;
    }
#endif
}
#include "accumulator.h"
#include "nnue.h"
#include "enums.h"
#include "bitboard.h"

void Accumulator::removePiece(int ind, int type)
{
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
        values[WHITE_ACC][j] -= L1_weights[whiteAccumIndex][j];
        values[BLACK_ACC][j] -= L1_weights[blackAccumIndex][j];
    }


}

void Accumulator::addPiece(int ind, int type)
{
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
        values[WHITE_ACC][j] += L1_weights[whiteAccumIndex][j];
        values[BLACK_ACC][j] += L1_weights[blackAccumIndex][j];
    }
}

void Accumulator::initAccum(int (&arr)[64])
{
    //Loop through array of all pieces
    for(int i=0; i < 64; i++)
    {
        if(arr[i] <= BLACK_KING)
        {
            addPiece(i,arr[i]);
        }
    }
}

void Accumulator::resetAccum()
{
    //Loop through array of all pieces
    for(int i=0; i < (l1_size/2); i++)
    {
        values[0][i] = 0;
        values[1][i] = 0;
    }
}
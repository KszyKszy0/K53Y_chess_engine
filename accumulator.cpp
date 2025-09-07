#include "accumulator.h"
#include "nnue.h"
#include "consts.h"
#include "enums.h"
#include "bitboard.h"
#include <immintrin.h>


void Accumulator::removePiece(int type, int ind)
{
#ifdef NNUE
    int whiteAccumIndex = 64 * type + ind;
    int blackAccumIndex = 0;

    if (type <= WHITE_KING)
        blackAccumIndex = 64 * (type + 6) + flipIndex(ind);
    else if (type <= BLACK_KING)
        blackAccumIndex = 64 * (type - 6) + flipIndex(ind);

    #ifdef INT16

    for(int i=0; i < L1_SIZE / 2; i += 1)
    {
        values[i] -= L1_weights[whiteAccumIndex][i];
        values[i + L1_SIZE / 2] -= L1_weights[blackAccumIndex][i];
    }

    #endif

    #if defined(FLOAT) || defined(FLOAT16)

    int half = L1_SIZE / 2;

    for(int i=0; i < half; i += 8)
    {
        __m256 white = _mm256_load_ps(&L1_weights[whiteAccumIndex][i]);
        __m256 val1  = _mm256_load_ps(&values[i]);
        __m256 res1  = _mm256_sub_ps(val1, white);
        _mm256_store_ps(&values[i], res1);

        __m256 black = _mm256_load_ps(&L1_weights[blackAccumIndex][i]);
        __m256 val2  = _mm256_load_ps(&values[i + half]);
        __m256 res2  = _mm256_sub_ps(val2, black);
        _mm256_store_ps(&values[i + half], res2);
    }

    #endif
#endif
}

void Accumulator::addPiece(int type, int ind)
{
#ifdef NNUE
    int whiteAccumIndex = 64 * type + ind;
    int blackAccumIndex = 0;

    if (type <= WHITE_KING)
        blackAccumIndex = 64 * (type + 6) + flipIndex(ind);
    else if (type <= BLACK_KING)
        blackAccumIndex = 64 * (type - 6) + flipIndex(ind);

    #ifdef INT16

    for(int i=0; i < L1_SIZE / 2; i += 1)
    {
        values[i] += L1_weights[whiteAccumIndex][i];
        values[i + L1_SIZE / 2] += L1_weights[blackAccumIndex][i];
    }

    #endif

    #if defined(FLOAT) || defined(FLOAT16)

    int half = L1_SIZE / 2;

    for(int i=0; i < half; i += 8)
    {
        __m256 white = _mm256_load_ps(&L1_weights[whiteAccumIndex][i]);
        __m256 val1  = _mm256_load_ps(&values[i]);
        __m256 res1  = _mm256_add_ps(val1, white);
        _mm256_store_ps(&values[i], res1);

        __m256 black = _mm256_load_ps(&L1_weights[blackAccumIndex][i]);
        __m256 val2  = _mm256_load_ps(&values[i + half]);
        __m256 res2  = _mm256_add_ps(val2, black);
        _mm256_store_ps(&values[i + half], res2);
    }

    #endif
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

    #ifdef INT16 
    //Loop through array of all pieces
    for(int i=0; i < L1_SIZE; i++)
    {
        values[i] = 0;
    }
    #endif

    #ifdef FLOAT
    //Loop through array of all pieces
    for(int i=0; i < L1_SIZE; i++)
    {
        values[i] = L1_bias[i % (L1_SIZE / 2)];
    }
    #endif

    #ifdef FLOAT16
    //Loop through array of all pieces
    for(int i=0; i < L1_SIZE; i++)
    {
        values[i] = L1_bias[i % (L1_SIZE / 2)];
    }
    #endif
#endif
}
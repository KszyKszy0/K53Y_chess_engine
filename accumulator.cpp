#include "accumulator.h"
#include "nnue.h"
#include "enums.h"
#include "bitboard.h"
#include <immintrin.h>


void Accumulator::removePiece(int type, int ind)
{
#ifdef NNUE
    int whiteAccumIndex = 64 * type + ind;
    int blackAccumIndex = 0;

    // Dla czarnych elementów przekształcamy indeks typu
    if(type <= WHITE_KING)
    {
        blackAccumIndex = 64 * (type + 6) + flipIndex(ind);
    }
    else if(type <= BLACK_KING)
    {
        blackAccumIndex = 64 * (type - 6) + flipIndex(ind);
    }

    // SIMD dla l1_size / 2 elementów, gdzie l1_size = 32
    for(int j = 0; j < L1_SIZE / 2; j += 8) // 8 floatów na jedną operację SIMD
    {
        // Załaduj wartości L1_weights dla białych i czarnych elementów
        __m256 whiteWeights = _mm256_loadu_ps(&L1_weights[whiteAccumIndex][j]);
        __m256 blackWeights = _mm256_loadu_ps(&L1_weights[blackAccumIndex][j]);

        // Załaduj aktualne wartości akumulatora
        __m256 whiteAccum = _mm256_loadu_ps(&values[j]);
        __m256 blackAccum = _mm256_loadu_ps(&values[j + (L1_SIZE / 2)]);

        // Odejmij wartości wag od akumulatorów
        whiteAccum = _mm256_sub_ps(whiteAccum, whiteWeights);
        blackAccum = _mm256_sub_ps(blackAccum, blackWeights);

        // Zapisz zaktualizowane wartości akumulatorów
        _mm256_storeu_ps(&values[j], whiteAccum);
        _mm256_storeu_ps(&values[j + (L1_SIZE / 2)], blackAccum);
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


    // Przetwarzanie SIMD dla l1_size / 2 elementów, gdzie l1_size = 32
    for(int j = 0; j < L1_SIZE / 2; j += 8) // 8 floatów na jedną operację SIMD
    {
        // Załaduj wartości L1_weights dla białych i czarnych elementów
        __m256 whiteWeights = _mm256_loadu_ps(&L1_weights[whiteAccumIndex][j]);
        __m256 blackWeights = _mm256_loadu_ps(&L1_weights[blackAccumIndex][j]);

        // Załaduj aktualne wartości akumulatora
        __m256 whiteAccum = _mm256_loadu_ps(&values[j]);
        __m256 blackAccum = _mm256_loadu_ps(&values[j + (L1_SIZE / 2)]);

        // Dodaj wartości wag do akumulatorów
        whiteAccum = _mm256_add_ps(whiteAccum, whiteWeights);
        blackAccum = _mm256_add_ps(blackAccum, blackWeights);

        // Zapisz zaktualizowane wartości akumulatorów
        _mm256_storeu_ps(&values[j], whiteAccum);
        _mm256_storeu_ps(&values[j + (L1_SIZE / 2)], blackAccum);
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
    for(int i=0; i < L1_SIZE; i++)
    {
        values[i] = 0;
    }
#endif
}
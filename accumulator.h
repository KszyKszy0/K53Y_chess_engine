#pragma once
#include "consts.h"
#include <cstdint>
#include <stdfloat>

class Accumulator
{
    public:

    //Two for colors and 16 for each
    #ifdef INT16
    alignas (64) int16_t values[L1_SIZE];
    #endif

    #if defined(FLOAT) || defined(FLOAT16)
    alignas (64) float values[L1_SIZE];
    #endif

    void removePiece(int type, int ind);

    void addPiece(int type, int ind);

    void initAccum(int (&arr)[64]);

    void resetAccum();
};
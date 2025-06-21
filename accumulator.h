#pragma once
#include "nnue.h"
class Accumulator
{
    public:

    //Two for colors and 16 for each
    float values[L1_SIZE];

    void removePiece(int type, int ind);

    void addPiece(int type, int ind);

    void initAccum(int (&arr)[64]);

    void resetAccum();
};
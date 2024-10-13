#pragma once

class Accumulator
{
    public:

    //Two for colors and 16 for each
    double values[2][16];

    void removePiece(int type, int ind);

    void addPiece(int type, int ind);

    void initAccum(int (&arr)[64]);

    void resetAccum();
};
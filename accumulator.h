#pragma once

class Accumulator
{
    public:

    //Two for colors and 16 for each
    int values[2][16];

    void removePiece(int ind, int type);

    void addPiece(int ind, int type);

    void initAccum(int (&arr)[64]);
};
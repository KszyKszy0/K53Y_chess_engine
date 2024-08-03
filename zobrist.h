#pragma once
#include "bitboard.h"
#include <cstddef>
#include <random>


class Zobrist
{
    public:

    Bitboard zobristTable[793];

    Zobrist()
    {
        //mersene prime generator
        std::mt19937_64 gen(0);
        std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

        for(int i=0; i<=792; i++)
        {
            //get random zobrist for each feature
            zobristTable[i] = dist(gen);
        }
    }
};
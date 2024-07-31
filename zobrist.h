#pragma once
#include "bitboard.h"

class Zobrist
{
    public:

    Bitboard zobristTable[793];

    Bitboard random_uint64() {
    Bitboard u1, u2, u3, u4;
        u1 = (Bitboard)(rand()) & 0xFFFF; u2 = (Bitboard)(rand()) & 0xFFFF;
        u3 = (Bitboard)(rand()) & 0xFFFF; u4 = (Bitboard)(rand()) & 0xFFFF;
        return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
    }

    Bitboard random_uint64_fewbits() {
    return random_uint64() & random_uint64() & random_uint64();
    }

    Zobrist()
    {
        for(int i=0; i<=792; i++)
        {
            zobristTable[i] = random_uint64_fewbits();
        }
    }
};
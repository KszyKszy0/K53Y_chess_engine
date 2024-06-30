#pragma once
#include <iostream>
#include <vector>

typedef unsigned long long Bitboard;

#define BOARD_SIZE 64

using namespace std;


class BB_utils
{
    public:

inline void setBit(Bitboard &bb, int index);

inline void clearBit(Bitboard &bb, int index);

inline void toggleBit(Bitboard &bb, int index);

inline bool isBitSet(Bitboard bb, int index);

inline int popLSB(Bitboard &bb);

inline int LSB(Bitboard &bb);

int popCount (Bitboard x);

void printBitboard(Bitboard bb);

Bitboard generateRookMoves(int square);

Bitboard generateBishopMoves(int square);

Bitboard generateKnightMoves(int square);

void init_bb();

vector<Bitboard> generateBlockers(Bitboard attackSet);

vector<vector<Bitboard>> bishopBlockers;

vector<vector<Bitboard>> rookBlockers;

Bitboard generateRectangularMask(int square1, int square2);

void generateRectangularLookup(Bitboard (&lookup)[64][64]);

Bitboard rectangularLookup[64][64];

Bitboard generateKingMoves(int square);

void generatePawnMoves(Bitboard (&pawnMove)[64], Bitboard (&pawnAttack)[64], bool isWhite);

Bitboard kingMoves[64];

Bitboard whitePawnMoves[64];
Bitboard whitePawnCaptures[64];

Bitboard blackPawnMoves[64];
Bitboard blackPawnCaptures[64];

};


#pragma once
#include <iostream>
#include <vector>
#include <stdint.h>
#include "enums.h"

typedef uint64_t Bitboard;

#define BOARD_SIZE 64

using namespace std;

static Bitboard MAX = 0xffffffffffffffff;

static Bitboard RANK_8 = 0xff00000000000000;

static Bitboard RANK_1 = 0xff;

//Sets bit at index to 1
void setBit(Bitboard &bb, int index);

//Sets bit at index to 0
void clearBit(Bitboard &bb, int index);

//Toggles bit between 0 and 1
void toggleBit(Bitboard &bb, int index);

//Toggles two bits between 0 and 1
void bitSwap(Bitboard &bb, int start, int target);

//Check if bit is set to 1
bool isBitSet(Bitboard &bb, int index);

//Returns index of LSB
int LSB(Bitboard &bb);

//Returns index of LSB and removes it
int popLSB(Bitboard &bb);

//Number of set bits
int popCount (Bitboard x);

//Print bitboards in 8x8 format
void printBitboard(Bitboard bb);

//Print array in 8x8 format
void printPieceArray(int array[64]);

//Changes index to white perspective (^56)
int flipIndex(int index);

class BB_utils
{
    public:

//Mask for square
Bitboard generateRookMoves(int square);

//Mask for square
Bitboard generateBishopMoves(int square);

//Direct moves of knight from square BB
Bitboard generateKnightMoves(int square);

//Setup of all BB (magic, moves, pieces)
BB_utils();

//Blocker generation for mask
vector<Bitboard> generateBlockers(Bitboard attackSet);

//List of all blockers for all squares
vector<vector<Bitboard>> bishopBlockers;

//List of all blockers for all squares
vector<vector<Bitboard>> rookBlockers;

//Squares between two squares (legality detection)
Bitboard generateRectangularMask(int square1, int square2);

//Generation of rectangular lookup
void generateRectangularLookup(Bitboard (&lookup)[64][64]);

//Array for rectangular lookup
Bitboard rectangularLookup[64][64];

//Direct moves of king from square BB
Bitboard generateKingMoves(int square);



// Generates pawn moves for color then captures
void generatePawnMoves(Bitboard (&pawnMove)[64], Bitboard (&pawnAttack)[64], bool isWhite);

//Array for king moves
Bitboard kingMoves[64];

//Array for knight moves
Bitboard knightMoves[64];

//Array for white pawn moves
Bitboard whitePawnMoves[64];

//Array for white pawn captures
Bitboard whitePawnCaptures[64];

//Array for black pawn moves
Bitboard blackPawnMoves[64];

//Array for black pawn captures
Bitboard blackPawnCaptures[64];

//Vector of legal bishop moves[square][magic]
vector<vector<Bitboard>> bishopMoves;

//Vector of legal rook moves[square][magic]
vector<vector<Bitboard>> rookMoves;

//Init magic rook vector
void initRookAttacks();

//Init magic bishop vector
void initBishopAttacks();

//Magic index conversion
int getMagicIndex(Bitboard blockers, Bitboard magic, int bits);

//BB moves for rook blockers mask
Bitboard generateRookBitboardAttacksBlockers(int sq, Bitboard blockers);

//BB moves for bishop blockers mask
Bitboard generateBishopBitboardAttacksBlockers(int sq, Bitboard blockers);

//Movement mask without edges
Bitboard rookMasks[64];

//Movement mask without edges
Bitboard bishopMasks[64];


//Magics and bits shift

Bitboard rooksMagics[64]=
{
  0x80004000976080,
  0x1040400010002000,
  0x4880200210000980,
  0x5280080010000482,
  0x200040200081020,
  0x2100080100020400,
  0x4280008001000200,
  0x1000a4425820300,
  0x29002100800040,
  0x4503400040201004,
  0x209002001004018,
  0x1131000a10002100,
  0x9000800120500,
  0x10e001804820010,
  0x29000402000100,
  0x2002000d01c40292,
  0x80084000200c40,
  0x10004040002002,
  0x201030020004014,
  0x80012000a420020,
  0x129010008001204,
  0x6109010008040002,
  0x950010100020004,
  0x803a0000c50284,
  0x80004100210080,
  0x200240100140,
  0x20004040100800,
  0x4018090300201000,
  0x4802010a00102004,
  0x2001000900040002,
  0x4a02104001002a8,
  0x2188108200204401,
  0x40400020800080,
  0x880402000401004,
  0x10040800202000,
  0x604410a02001020,
  0x200200206a001410,
  0x86000400810080,
  0x428200040600080b,
  0x2001000041000082,
  0x80002000484000,
  0x210002002c24000,
  0x401a200100410014,
  0x5021000a30009,
  0x218000509010010,
  0x4000400410080120,
  0x20801040010,
  0x29040040820011,
  0x4080400024800280,
  0x500200040100440,
  0x2880142001004100,
  0x412020400a001200,
  0x18c028004080080,
  0x884001020080401,
  0x210810420400,
  0x801048745040200,
  0x4401002040120082,
  0x408200210012,
  0x110008200441,
  0x2010002004100901,
  0x801000800040211,
  0x480d000400820801,
  0x820104201280084,
  0x1001040311802142,
};

Bitboard bishopsMagics[64]=
{
  0x1024b002420160,
  0x1008080140420021,
  0x2012080041080024,
  0xc282601408c0802,
  0x2004042000000002,
  0x12021004022080,
  0x880414820100000,
  0x4501002211044000,
  0x20402222121600,
  0x1081088a28022020,
  0x1004c2810851064,
  0x2040080841004918,
  0x1448020210201017,
  0x4808110108400025,
  0x10504404054004,
  0x800010422092400,
  0x40000870450250,
  0x402040408080518,
  0x1000980a404108,
  0x1020804110080,
  0x8200c02082005,
  0x40802009a0800,
  0x1000201012100,
  0x111080200820180,
  0x904122104101024,
  0x4008200405244084,
  0x44040002182400,
  0x4804080004021002,
  0x6401004024004040,
  0x404010001300a20,
  0x428020200a20100,
  0x300460100420200,
  0x404200c062000,
  0x22101400510141,
  0x104044400180031,
  0x2040040400280211,
  0x8020400401010,
  0x20100110401a0040,
  0x100101005a2080,
  0x1a008300042411,
  0x120a025004504000,
  0x4001084242101000,
  0xa020202010a4200,
  0x4000002018000100,
  0x80104000044,
  0x1004009806004043,
  0x100401080a000112,
  0x1041012101000608,
  0x40400c250100140,
  0x80a10460a100002,
  0x2210030401240002,
  0x6040aa108481b20,
  0x4009004050410002,
  0x8106003420200e0,
  0x1410500a08206000,
  0x92548802004000,
  0x1040041241028,
  0x120042025011,
  0x8060104054400,
  0x20004404020a0a01,
  0x40008010020214,
  0x4000050209802c1,
  0x208244210400,
  0x10140848044010,
};

int rookBits[64]=
{   12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12 };

int bishopBits[64]=
{   6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6 };

//End of magics and bits shifts

};


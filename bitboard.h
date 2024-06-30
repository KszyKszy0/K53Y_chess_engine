#pragma once
#include <iostream>
#include <vector>
#include <stdint.h>
#include "enums.h"

typedef uint64_t Bitboard;

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

// Bitboard rookMoves[64][4096];
// Bitboard bishopMoves[64][512];
vector<vector<Bitboard>> bishopMoves;

vector<vector<Bitboard>> rookMoves;

void initRookAttacks();
void initBishopAttacks();

inline int getMagicIndex(Bitboard blockers, Bitboard magic, int bits);

Bitboard generateRookBitboardAttacksBlockers(int sq, Bitboard blockers);

Bitboard generateBishopBitboardAttacksBlockers(int sq, Bitboard blockers);

Bitboard rooksMagics[64]=
{
  0xc80001828100040,
  0x26004408400010,
  0x1060040000202048,
  0x110141100800888,
  0x84420501a000802,
  0x1803002905002224,
  0x104380106000182,
  0x208201001041,
  0x4080118100002020,
  0x1c40120100004020,
  0x1009040000802020,
  0x884002800841010,
  0x2220068801201011,
  0x1911442000004022,
  0x4020802010011,
  0x220004400102001,
  0x4009608000842280,
  0x4060104318400210,
  0x811002400040020,
  0x4801520000c41010,
  0x4280020022204,
  0x413000408404041,
  0xc028800000101,
  0x100400e904000201,
  0x3800242000100a5,
  0xc0008008210321,
  0x2003002400001261,
  0x410028894a041001,
  0x800041040002901,
  0x1002080204080e,
  0x22218040040401,
  0x24084103280482,
  0x20002020801040,
  0x1020200020400812,
  0x4082000000c0061,
  0x4042024001200804,
  0x5040040000024188,
  0x100040008101a13,
  0x408108018080802,
  0x80408064000041,
  0x850400821800120,
  0x8100240008480b,
  0x6008080001100241,
  0x1eb0100200110248,
  0x30202216010004,
  0x802021000c6142,
  0x32200444020410a,
  0xa0202004580041,
  0x80410000406180,
  0x8220200002040a3,
  0x93000200a002804,
  0x40080080010010b2,
  0x40080101012a1304,
  0x4041000001012,
  0x10001000e840482,
  0xc04020200004885,
  0x1080006110001041,
  0x2081211102084250,
  0x1020001500014619,
  0x3810440a10000c23,
  0x410484200860422,
  0x180b0201090b2004,
  0x1100040124410282,
  0x1820a101000443,
};

Bitboard bishopsMagics[64]=
{
  0x1841160051a00401,
  0x202000012224a02,
  0x284003005412542,
  0x2800a220c40502,
  0x68005050040308,
  0x440802c810020230,
  0x240034002014417,
  0x45202411310208a,
  0x1a08410602016172,
  0x830464044100e08,
  0x411a31a010040808,
  0x200124000484405,
  0x60020c80000120a,
  0x6a001002583420a,
  0x10082020084c0051,
  0x1325202008030692,
  0x2002026a00045020,
  0x41051c000052122c,
  0x10200b000100803,
  0x720204000610032,
  0x6818603041604002,
  0x1041040010020001,
  0x301081420025204,
  0x42024420826001,
  0x404304000131801,
  0x1041090c00842100,
  0x144110020080240,
  0x1211004002202008,
  0x105080000500401,
  0x2080600c62080131,
  0x844100114022082,
  0x841080020804b00,
  0x8240800080210,
  0x4002144012084210,
  0x2052000060904,
  0x88400420500c14,
  0x434000204404,
  0x1840240800212081,
  0x4208c019244184,
  0x54da10030841,
  0x1200090011080a02,
  0x800140c000414c8,
  0x4400382000401058,
  0x100020140000105,
  0x610040040044421,
  0x20001202848110a,
  0x222011010300101,
  0x106608008012304,
  0x220429002014a02,
  0x110800a005104208,
  0x806201048200111,
  0x2082008a03000802,
  0x2041008034011420,
  0x401006005004434,
  0x90a429004080204,
  0x2041040950041802,
  0x1002200028441145,
  0x109004012802018a,
  0x65100405500801,
  0x284240458180a00,
  0x2002441020024020,
  0x4a10110101144112,
  0x1010a2c00005030,
  0x40c012121020424,
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


};


#include <iostream>
#include "bitboard.h"
#include "enums.h"
#include <string>
#include <vector>
typedef unsigned long long Bitboard;

#define BOARD_SIZE 64

using namespace std;



    inline void BB_utils::setBit(Bitboard &bb, int index) {
        bb = bb | (1ULL << index);
    }

    inline void BB_utils::clearBit(Bitboard &bb, int index) {
        bb = bb & ~(1ULL << index);
    }

    inline void BB_utils::toggleBit(Bitboard &bb, int index) {
        bb = bb ^(1ULL << index);
    }

    inline bool BB_utils::isBitSet(Bitboard bb, int index) {
        return (bb >> index) & 1ULL;
    }

    inline int BB_utils::popLSB(Bitboard &bb) {
        int index = LSB(bb);
        bb &= bb - 1;
        return index;
    }

    inline int BB_utils::LSB(Bitboard &bb){
        return __builtin_clzll(bb);
    }

    int BB_utils::popCount (Bitboard x) {
        int count = 0;
        while (x) {
        count++;
        x &= x - 1; 
        }
        return count;
    }

    void BB_utils::printBitboard(Bitboard bb) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            cout << (BB_utils::isBitSet(bb, index) ? '1' : '0') << ' ';
        }
        cout << '\n';
    }
    cout<< '\n';
    }



    //rook moves
    Bitboard BB_utils::generateRookMoves(int square) {
    Bitboard moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // a-h (horyzontalny ruch, pomijając skrajne pola)
    for (int f = 1; f < 7; ++f) {
        if (f != file) {
            setBit(moves, rank * 8 + f);
        }
    }

    // 1-8 (wertykalny ruch, pomijając skrajne pola)
    for (int r = 1; r < 7; ++r) {
        if (r != rank) {
            setBit(moves, r * 8 + file);
        }
    }

    return moves;
    }


    // bishop moves
    Bitboard BB_utils::generateBishopMoves(int square) {
    Bitboard moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // all diagonals, pomijając skrajne pola
    for (int d = 1; d < 7; ++d) {
        if (rank + d < 7 && file + d < 7) setBit(moves, (rank + d) * 8 + (file + d));
        if (rank + d < 7 && file - d > 0) setBit(moves, (rank + d) * 8 + (file - d));
        if (rank - d > 0 && file + d < 7) setBit(moves, (rank - d) * 8 + (file + d));
        if (rank - d > 0 && file - d > 0) setBit(moves, (rank - d) * 8 + (file - d));
    }

    return moves;
    }


    Bitboard BB_utils::generateKnightMoves(int square) {
    Bitboard moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    const int knightMoves[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    for (const auto& move : knightMoves) {
        int newRank = rank + move[0];
        int newFile = file + move[1];
        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
            setBit(moves, newRank * 8 + newFile);
        }
    }

    return moves;
    }


    void BB_utils::init_bb()
    {
        Bitboard rookMoves[BOARD_SIZE];
        Bitboard bishopMoves[BOARD_SIZE];
        Bitboard knightMoves[BOARD_SIZE];

        

        for (int square = 0; square < BOARD_SIZE; ++square) {
            rookMoves[square] = generateRookMoves(square);
            bishopMoves[square] = generateBishopMoves(square);
            knightMoves[square] = generateKnightMoves(square);
            kingMoves[square] = generateKingMoves(square);
            
            bishopBlockers.push_back(generateBlockers(bishopMoves[square]));

            rookBlockers.push_back(generateBlockers(rookMoves[square]));

            
            // rook_blockers[square] = (generateBlockers(rookMoves[square]));
            // bishop_blockers[square] = generateBlockers(bishopMoves[square]);
        }

        
        generatePawnMoves(whitePawnMoves, whitePawnCaptures, true);
        generatePawnMoves(blackPawnMoves, blackPawnCaptures, false);

        // initRookAttacks();

    }

    vector<Bitboard> BB_utils::generateBlockers(Bitboard attackSet) {
        vector<Bitboard> blockers;
        int count = 0;


        for (int i = 0; i < BOARD_SIZE; ++i) {
            if (BB_utils::isBitSet(attackSet, i)) {
                ++count;
            }
        }

        int combinations = 1 << count;

        for (int i = 0; i < combinations; ++i) {
            Bitboard blockerSet = 0ULL;
            int bitIndex = 0;
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (BB_utils::isBitSet(attackSet, j)) {
                    if (i & (1 << bitIndex)) {
                        BB_utils::setBit(blockerSet, j);
                    }
                    ++bitIndex;
                }
            }
            blockers.push_back(blockerSet);
        }

        return blockers;
    }

    Bitboard BB_utils::generateRectangularMask(int square1, int square2) {
    Bitboard mask = 0ULL;
    int rank1 = square1 / 8, file1 = square1 % 8;
    int rank2 = square2 / 8, file2 = square2 % 8;

    if (rank1 == rank2) {  // Ten sam rząd (horyzontalny ruch)
        int startFile = std::min(file1, file2) + 1;
        int endFile = std::max(file1, file2);
        for (int file = startFile; file < endFile; ++file) {
            setBit(mask, rank1 * 8 + file);
        }
    } else if (file1 == file2) {  // Ta sama kolumna (wertykalny ruch)
        int startRank = std::min(rank1, rank2) + 1;
        int endRank = std::max(rank1, rank2);
        for (int rank = startRank; rank < endRank; ++rank) {
            setBit(mask, rank * 8 + file1);
        }
    } else if (std::abs(rank1 - rank2) == std::abs(file1 - file2)) {  // Ta sama przekątna
        int rankStep = (rank2 > rank1) ? 1 : -1;
        int fileStep = (file2 > file1) ? 1 : -1;
        int rank = rank1 + rankStep, file = file1 + fileStep;
        while (rank != rank2 && file != file2) {
            setBit(mask, rank * 8 + file);
            rank += rankStep;
            file += fileStep;
        }
    }

    return mask;
    }


    void BB_utils::generateRectangularLookup(Bitboard (&lookup)[64][64]) {
    for (int square1 = 0; square1 < 64; ++square1) {
        for (int square2 = 0; square2 < 64; ++square2) {
            lookup[square1][square2] = generateRectangularMask(square1, square2);
        }
    }
    }

    Bitboard BB_utils::generateKingMoves(int square) {
    Bitboard moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    for (int r = -1; r <= 1; ++r) {
        for (int f = -1; f <= 1; ++f) {
            if (r == 0 && f == 0) continue;
            int newRank = rank + r;
            int newFile = file + f;
            if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                setBit(moves, newRank * 8 + newFile);
            }
        }
    }

    return moves;
    }

    void BB_utils::generatePawnMoves(Bitboard (&pawnMove)[64], Bitboard (&pawnAttack)[64], bool isWhite) {
    for (int square = 0; square < 64; ++square) {
        Bitboard moves = 0ULL;
        Bitboard attacks = 0ULL;
        int rank = square / 8;
        int file = square % 8;

        // Poruszanie do przodu
        if (isWhite) {
            if (rank < 7) {
                setBit(moves, (rank + 1) * 8 + file);
                if (rank == 1) { // Podwójny ruch z drugiego rzędu
                    setBit(moves, (rank + 2) * 8 + file);
                }
            }
        } else {
            if (rank > 0) {
                setBit(moves, (rank - 1) * 8 + file);
                if (rank == 6) { // Podwójny ruch z siódmego rzędu
                    setBit(moves, (rank - 2) * 8 + file);
                }
            }
        }

        // Bicie
        if (isWhite) {
            if (rank < 7 && file > 0) {
                setBit(attacks, (rank + 1) * 8 + (file - 1));
            }
            if (rank < 7 && file < 7) {
                setBit(attacks, (rank + 1) * 8 + (file + 1));
            }
        } else {
            if (rank > 0 && file > 0) {
                setBit(attacks, (rank - 1) * 8 + (file - 1));
            }
            if (rank > 0 && file < 7) {
                setBit(attacks, (rank - 1) * 8 + (file + 1));
            }
        }

        pawnMove[square] = moves;
        pawnAttack[square] = attacks;
    }
    }


    // void BB_utils::initRookAttacks() {
    // for (int square = 0; square < BOARD_SIZE; ++square) {
    //     for (int occupancy = 0; occupancy < rookBlockers[square].size(); ++occupancy) {
    //         int index = getMagicIndex(rookBlockers[square][occupancy], rooksMagics[square], rookBits[square]);
    //         Bitboard attacks = generateRookBitboardAttacksBlockers(square,rookBlockers[square][occupancy]);
    //         rookMoves[index][occupancy] = attacks; 
    //         cout<<attacks;
    //     }
    // }
    // }

    // void BB_utils::initBishopAttacks() {
    // for (int square = 0; square < BOARD_SIZE; ++square) {
    //     for (int occupancy = 0; occupancy < bishopBlockers[square].size(); ++occupancy) {
    //         int index = getMagicIndex(bishopBlockers[square][occupancy], bishopsMagics[square], bishopBits[square]);
    //         Bitboard attacks = generateBishopBitboardAttacksBlockers(square,bishopBlockers[square][occupancy]);
    //         bishopMoves[index][occupancy] = attacks; 
    //         cout<<attacks;
    //     }
    // }
    // }

    inline int BB_utils::getMagicIndex(Bitboard blockers, Bitboard magic, int bits)
    {
        return ((blockers * magic) >> (64 - bits));
    }

    Bitboard BB_utils::generateRookBitboardAttacksBlockers(int sq, Bitboard blockers) {
    Bitboard moves = 0;
    int row = sq / 8;
    int col = sq % 8;

    // Przesunięcia w prawo
    for (int j = col + 1; j < 8; ++j) {
        int temp = 8 * row + j;
        moves |= 1ULL << temp;
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    // Przesunięcia w lewo
    for (int j = col - 1; j >= 0; --j) {
        int temp = 8 * row + j;
        moves |= 1ULL << temp;
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    // Przesunięcia w dół
    for (int j = row + 1; j < 8; ++j) {
        int temp = 8 * j + col;
        moves |= 1ULL << temp;
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    // Przesunięcia w górę
    for (int j = row - 1; j >= 0; --j) {
        int temp = 8 * j + col;
        moves |= 1ULL << temp;
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    return moves;
}

Bitboard BB_utils::generateBishopBitboardAttacksBlockers(int sq, Bitboard blockers) {
    Bitboard moves = 0;
    int row = sq / 8;
    int col = sq % 8;

    // Diagonal w dół w lewo
    int r = row + 1;
    int c = col - 1;
    while (r < 8 && c >= 0) {
        moves |= 1ULL << (r * 8 + c);
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;
        ++r;
        --c;
    }

    // Diagonal w dół w prawo
    r = row + 1;
    c = col + 1;
    while (r < 8 && c < 8) {
        moves |= 1ULL << (r * 8 + c);
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;
        ++r;
        ++c;
    }

    // Diagonal w górę w lewo
    r = row - 1;
    c = col - 1;
    while (r >= 0 && c >= 0) {
        moves |= 1ULL << (r * 8 + c);
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;
        --r;
        --c;
    }

    // Diagonal w górę w prawo
    r = row - 1;
    c = col + 1;
    while (r >= 0 && c < 8) {
        moves |= 1ULL << (r * 8 + c);
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;
        --r;
        ++c;
    }

    return moves;
}

// void BB_utils::magicInit()
// {
//     rooksMagics = {
//   0xc80001828100040,
//   0x26004408400010,
//   0x1060040000202048,
//   0x110141100800888,
//   0x84420501a000802,
//   0x1803002905002224,
//   0x104380106000182,
//   0x208201001041,
//   0x4080118100002020,
//   0x1c40120100004020,
//   0x1009040000802020,
//   0x884002800841010,
//   0x2220068801201011,
//   0x1911442000004022,
//   0x4020802010011,
//   0x220004400102001,
//   0x4009608000842280,
//   0x4060104318400210,
//   0x811002400040020,
//   0x4801520000c41010,
//   0x4280020022204,
//   0x413000408404041,
//   0xc028800000101,
//   0x100400e904000201,
//   0x3800242000100a5,
//   0xc0008008210321,
//   0x2003002400001261,
//   0x410028894a041001,
//   0x800041040002901,
//   0x1002080204080e,
//   0x22218040040401,
//   0x24084103280482,
//   0x20002020801040,
//   0x1020200020400812,
//   0x4082000000c0061,
//   0x4042024001200804,
//   0x5040040000024188,
//   0x100040008101a13,
//   0x408108018080802,
//   0x80408064000041,
//   0x850400821800120,
//   0x8100240008480b,
//   0x6008080001100241,
//   0x1eb0100200110248,
//   0x30202216010004,
//   0x802021000c6142,
//   0x32200444020410a,
//   0xa0202004580041,
//   0x80410000406180,
//   0x8220200002040a3,
//   0x93000200a002804,
//   0x40080080010010b2,
//   0x40080101012a1304,
//   0x4041000001012,
//   0x10001000e840482,
//   0xc04020200004885,
//   0x1080006110001041,
//   0x2081211102084250,
//   0x1020001500014619,
//   0x3810440a10000c23,
//   0x410484200860422,
//   0x180b0201090b2004,
//   0x1100040124410282,
//   0x1820a101000443,
// };


// bishopsMagics[64] = {
//   0x1841160051a00401,
//   0x202000012224a02,
//   0x284003005412542,
//   0x2800a220c40502,
//   0x68005050040308,
//   0x440802c810020230,
//   0x240034002014417,
//   0x45202411310208a,
//   0x1a08410602016172,
//   0x830464044100e08,
//   0x411a31a010040808,
//   0x200124000484405,
//   0x60020c80000120a,
//   0x6a001002583420a,
//   0x10082020084c0051,
//   0x1325202008030692,
//   0x2002026a00045020,
//   0x41051c000052122c,
//   0x10200b000100803,
//   0x720204000610032,
//   0x6818603041604002,
//   0x1041040010020001,
//   0x301081420025204,
//   0x42024420826001,
//   0x404304000131801,
//   0x1041090c00842100,
//   0x144110020080240,
//   0x1211004002202008,
//   0x105080000500401,
//   0x2080600c62080131,
//   0x844100114022082,
//   0x841080020804b00,
//   0x8240800080210,
//   0x4002144012084210,
//   0x2052000060904,
//   0x88400420500c14,
//   0x434000204404,
//   0x1840240800212081,
//   0x4208c019244184,
//   0x54da10030841,
//   0x1200090011080a02,
//   0x800140c000414c8,
//   0x4400382000401058,
//   0x100020140000105,
//   0x610040040044421,
//   0x20001202848110a,
//   0x222011010300101,
//   0x106608008012304,
//   0x220429002014a02,
//   0x110800a005104208,
//   0x806201048200111,
//   0x2082008a03000802,
//   0x2041008034011420,
//   0x401006005004434,
//   0x90a429004080204,
//   0x2041040950041802,
//   0x1002200028441145,
//   0x109004012802018a,
//   0x65100405500801,
//   0x284240458180a00,
//   0x2002441020024020,
//   0x4a10110101144112,
//   0x1010a2c00005030,
//   0x40c012121020424,
// };

// rookBits[64] = { 12, 11, 11, 11, 11, 11, 11, 12,
//                             11, 10, 10, 10, 10, 10, 10, 11,
//                             11, 10, 10, 10, 10, 10, 10, 11,
//                             11, 10, 10, 10, 10, 10, 10, 11,
//                             11, 10, 10, 10, 10, 10, 10, 11,
//                             11, 10, 10, 10, 10, 10, 10, 11,
//                             11, 10, 10, 10, 10, 10, 10, 11,
//                             12, 11, 11, 11, 11, 11, 11, 12 };

// bishopBits[64] = { 6, 5, 5, 5, 5, 5, 5, 6,
//                               5, 5, 5, 5, 5, 5, 5, 5,
//                               5, 5, 7, 7, 7, 7, 5, 5,
//                               5, 5, 7, 9, 9, 7, 5, 5,
//                               5, 5, 7, 9, 9, 7, 5, 5,
//                               5, 5, 7, 7, 7, 7, 5, 5,
//                               5, 5, 5, 5, 5, 5, 5, 5,
//                               6, 5, 5, 5, 5, 5, 5, 6 };
// }
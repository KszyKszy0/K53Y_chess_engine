#include <iostream>
#include "bitboard.h"
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

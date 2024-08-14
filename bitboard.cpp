#include <iostream>
#include "bitboard.h"
#include "enums.h"
#include <string>
#include <vector>

using namespace std;

//Sets bit at index to 1
void setBit(Bitboard &bb, int index)
{ bb = bb | (1ULL << index); }

//Sets bit at index to 0
void clearBit(Bitboard &bb, int index)
{ bb = bb & ~(1ULL << index); }

//Toggles bit between 0 and 1
void toggleBit(Bitboard &bb, int index)
{ bb = bb ^(1ULL << index); }

//Toggles start and target between 0 and 1
void bitSwap(Bitboard &bb, int start, int target)
{ bb ^= 1ULL << start | 1ULL << target; }

//Check if bit is set to 1
bool isBitSet(Bitboard &bb, int index)
{ return (bb >> index) & 1ULL; }

//Returns index of LSB
int LSB(Bitboard &bb)
{
    return __builtin_ctzll(bb);
}

//Returns index of LSB and removes it
int popLSB(Bitboard &bb)
{
    int index = LSB(bb);
    bb &= bb - 1;
    return index;
}

//Number of set bits
int popCount (Bitboard x)
{
    return __builtin_popcountll(x);
    // int count = 0;
    // while (x) {
    // count++;
    // x &= x - 1;
    // }
    // return count;
}

//Print bitboards in 8x8 format
void printBitboard(Bitboard bb)
{
  for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            cout << (isBitSet(bb, index) ? '1' : '0') << ' ';
        }
        cout << '\n';
    }
    cout<< '\n';
}

//Print array in 8x8 format
void printPieceArray(int array[64])
{
  for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            cout << array[index] << ' ';
            if(array[index] < 10)
                cout<< ' ';
        }
        cout << '\n';
    }
    cout<< '\n';
}

//Rook moves for given square
//Used for mask generation
Bitboard BB_utils::generateRookMoves(int square)
{
    //Bitboard of moves
    Bitboard moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // a-h (horizontal move, skipping edges)
    for (int f = 1; f < 7; ++f) {
        if (f != file) {
            setBit(moves, rank * 8 + f);
        }
    }

    // 1-8 (vertical move, skipping edges)
    for (int r = 1; r < 7; ++r) {
        if (r != rank) {
            setBit(moves, r * 8 + file);
        }
    }

    return moves;
}


//Bishop moves for given square
//Used for mask generation
Bitboard BB_utils::generateBishopMoves(int square)
{
    //Bitboard of moves
    Bitboard moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // all diagonals, skipping edges
    for (int d = 1; d < 7; ++d) {
        if (rank + d < 7 && file + d < 7) setBit(moves, (rank + d) * 8 + (file + d));
        if (rank + d < 7 && file - d > 0) setBit(moves, (rank + d) * 8 + (file - d));
        if (rank - d > 0 && file + d < 7) setBit(moves, (rank - d) * 8 + (file + d));
        if (rank - d > 0 && file - d > 0) setBit(moves, (rank - d) * 8 + (file - d));
    }

    return moves;
}

//Knight moves
//Used for lookup for knights
Bitboard BB_utils::generateKnightMoves(int square)
{
    //Bitboard of moves
    Bitboard moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    //Offsets of moves from square
    const int knightMoves[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    //Foreach offset add that move to moves Bitboard
    //If it doesn't go off the board
    for (const auto& move : knightMoves) {

        //Rank and file after move
        int newRank = rank + move[0];
        int newFile = file + move[1];

        //Check if out of bounds
        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
            setBit(moves, newRank * 8 + newFile);
        }
    }

    return moves;
}

//Constructor
BB_utils::BB_utils()
{
    //Initialization of movegen features
    for (int square = 0; square < BOARD_SIZE; ++square) {

        //Initialize masks for pieces
        rookMasks[square] = generateRookMoves(square);
        bishopMasks[square] = generateBishopMoves(square);

        //Initialize moves for king and knight
        knightMoves[square] = generateKnightMoves(square);
        kingMoves[square] = generateKingMoves(square);

        //Generate blockers combinations for bishops and rooks (also queens)
        bishopBlockers.push_back(generateBlockers(bishopMasks[square]));
        rookBlockers.push_back(generateBlockers(rookMasks[square]));

    }

    //Generate pawn moves Lookup
    generatePawnMoves(whitePawnMoves, whitePawnCaptures, true);
    generatePawnMoves(blackPawnMoves, blackPawnCaptures, false);

    //Init magic rooks and bishop attacks
    initRookAttacks();
    initBishopAttacks();

    //Generate rectangular lookup
    generateRectangularLookup(rectangularLookup);
}

//Get blockers combinations for given mask
vector<Bitboard> BB_utils::generateBlockers(Bitboard attackSet)
{
    //Vector of blockers
    vector<Bitboard> blockers;
    int count = 0;

    //Get number of bits set in mask
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (isBitSet(attackSet, i)) {
            ++count;
        }
    }

    //Number of combinations is 2^bits == so == 1 << coount
    int combinations = 1 << count;

    //Foreach combination
    for (int i = 0; i < combinations; ++i)
    {
        //Reset blockers set
        Bitboard blockerSet = 0ULL;

        //Reset index of bit;
        int bitIndex = 0;

        // Loop through each square on the board
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            // Check if the current square is part of the attack set (e.g., valid target square)
            if (isBitSet(attackSet, j)) {

                // Check if the current bit in 'i' corresponds to a blocker at position 'j'
                // 'i' represents a combination of blocker positions encoded as bits
                if (i & (1 << bitIndex))
                {
                    // Set the bit in 'blockerSet' at position 'j'
                    setBit(blockerSet, j);
                }

                // Move to the next bit in the combination 'i'
                ++bitIndex;
            }
        }

        // Add the current blocker set to the list of all possible blocker sets
        blockers.push_back(blockerSet);
    }

    return blockers;
}

    Bitboard BB_utils::generateRectangularMask(int square1, int square2) {
    Bitboard mask = 0ULL;
    int rank1 = square1 / 8, file1 = square1 % 8;
    int rank2 = square2 / 8, file2 = square2 % 8;

    if (rank1 == rank2) {  // horizontal movement
        int startFile = std::min(file1, file2) + 1;
        int endFile = std::max(file1, file2);
        for (int file = startFile; file < endFile; ++file) {
            setBit(mask, rank1 * 8 + file);
        }
    } else if (file1 == file2) {  // vertical movement
        int startRank = std::min(rank1, rank2) + 1;
        int endRank = std::max(rank1, rank2);
        for (int rank = startRank; rank < endRank; ++rank) {
            setBit(mask, rank * 8 + file1);
        }
    } else if (std::abs(rank1 - rank2) == std::abs(file1 - file2)) {  // diagonal movement
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

        // forward movement
        if (isWhite) {
            if (rank < 7) {
                setBit(moves, (rank + 1) * 8 + file);
            }
        } else {
            if (rank > 0) {
                setBit(moves, (rank - 1) * 8 + file);
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


    void BB_utils::initRookAttacks() {
    for (int square = 0; square < BOARD_SIZE; ++square) {
        vector<Bitboard> tempSquare;
        tempSquare.resize(4096);

        for (int occupancy = 0; occupancy < rookBlockers[square].size(); ++occupancy) {

            int index = getMagicIndex(rookBlockers[square][occupancy], rooksMagics[square], rookBits[square]);

            Bitboard attacks = generateRookBitboardAttacksBlockers(square,rookBlockers[square][occupancy]);

            tempSquare[index] = attacks;
        }
        rookMoves.push_back(tempSquare);
    }
    }

    void BB_utils::initBishopAttacks() {
    for (int square = 0; square < BOARD_SIZE; ++square) {
        vector<Bitboard> tempSquare;
        tempSquare.resize(512);
        for (int occupancy = 0; occupancy < bishopBlockers[square].size(); ++occupancy) {

            int index = getMagicIndex(bishopBlockers[square][occupancy], bishopsMagics[square], bishopBits[square]);

            Bitboard attacks = generateBishopBitboardAttacksBlockers(square,bishopBlockers[square][occupancy]);

            tempSquare[index] = attacks;
        }
        bishopMoves.push_back(tempSquare);
    }
    }

    int BB_utils::getMagicIndex(Bitboard blockers, Bitboard magic, int bits)
    {
        return (int)((blockers * magic) >> (64 - bits));
    }

    Bitboard BB_utils::generateRookBitboardAttacksBlockers(int sq, Bitboard blockers) {
    Bitboard moves = 0;
    int row = sq / 8;
    int col = sq % 8;

    // right movement
    for (int j = col + 1; j < 8; ++j) {
        int temp = 8 * row + j;
        moves |= 1ULL << temp;
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    // left movement
    for (int j = col - 1; j >= 0; --j) {
        int temp = 8 * row + j;
        moves |= 1ULL << temp;
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    // down movement
    for (int j = row + 1; j < 8; ++j) {
        int temp = 8 * j + col;
        moves |= 1ULL << temp;
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    // up movement
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

    // Diagonal left down
    int r = row + 1;
    int c = col - 1;
    while (r < 8 && c >= 0) {
        moves |= 1ULL << (r * 8 + c);
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;
        ++r;
        --c;
    }

    // Diagonal right down
    r = row + 1;
    c = col + 1;
    while (r < 8 && c < 8) {
        moves |= 1ULL << (r * 8 + c);
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;
        ++r;
        ++c;
    }

    // Diagonal left up
    r = row - 1;
    c = col - 1;
    while (r >= 0 && c >= 0) {
        moves |= 1ULL << (r * 8 + c);
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;
        --r;
        --c;
    }

    // Diagonal right up
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
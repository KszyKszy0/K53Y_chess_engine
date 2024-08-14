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
            // Check if the current square is part of mask
            if (isBitSet(attackSet, j)) {

                //If it is part of mask increment bitIndex at the end of loop
                //Moreover if current bit index is in current combination in bit representation we add it to blockers for example
                //We consider 40th combinations so we got 101000
                //So if we are on 4th and 6th bitIndex we Setbit in our blockers
                //It makes sense because full blockers would mean only 1's on all length so if we got 14 bits
                //It would've been 14 1's where one means occupancy
                //Each 0 bits tells us that at this square there is no blockers
                if (i & (1 << bitIndex))
                {
                    // Set the bit in 'blockerSet' at position 'j'
                    setBit(blockerSet, j);
                }

                // Move to the next bit in the combination 'i'
                // Increment bitIndex so we chceck overlaping with next bit from mask with next combination bit setup
                // We increment it every time we get bit from mask so we are "tracking" mask bits with it
                ++bitIndex;
            }
        }

        // Add the current blocker set to the list of all possible blocker sets
        blockers.push_back(blockerSet);
    }

    return blockers;
}

Bitboard BB_utils::generateRectangularMask(int square1, int square2)
{
    //Init mask and sqaures values
    Bitboard mask = 0ULL;
    int rank1 = square1 / 8, file1 = square1 % 8;
    int rank2 = square2 / 8, file2 = square2 % 8;

    if (rank1 == rank2)
    {
        // Horizontal movement

        //Check which is smaller exclude start
        int startFile = std::min(file1, file2) + 1;
        int endFile = std::max(file1, file2);

        //Make "path" along it; go to < endFile so we leave one
        for (int file = startFile; file < endFile; ++file)
        {
            setBit(mask, rank1 * 8 + file);
        }

    } else if (file1 == file2)
    {
        // Vertical movement

        //Check which is smaller exclude start
        int startRank = std::min(rank1, rank2) + 1;
        int endRank = std::max(rank1, rank2);

        //Make "path" along it; go to < endRank so we leave one
        for (int rank = startRank; rank < endRank; ++rank)
        {
            setBit(mask, rank * 8 + file1);
        }

    } else if (std::abs(rank1 - rank2) == std::abs(file1 - file2))
    {
        // Diagonal movement

        //Check which step do we take
        int rankStep = (rank2 > rank1) ? 1 : -1;
        int fileStep = (file2 > file1) ? 1 : -1;

        //Add step to starting position
        int rank = rank1 + rankStep, file = file1 + fileStep;

        //Make lane using calculated steps until we reach file2 or rank2 (ending squares)
        while (rank != rank2 && file != file2) {
            setBit(mask, rank * 8 + file);
            rank += rankStep;
            file += fileStep;
        }
    }

    return mask;
}

//Generate rectangular lookup for all squares
void BB_utils::generateRectangularLookup(Bitboard (&lookup)[64][64])
{
    for (int square1 = 0; square1 < 64; ++square1)
    {
        for (int square2 = 0; square2 < 64; ++square2)
        {
            lookup[square1][square2] = generateRectangularMask(square1, square2);
        }
    }
}

//Generates king moves lookup
Bitboard BB_utils::generateKingMoves(int square)
{
    //Initialize square and moves
    Bitboard moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    //Go from -1 to +1 in both directions for bitboard with moves
    for (int r = -1; r <= 1; ++r)
    {
        for (int f = -1; f <= 1; ++f)
        {
            //Skip the move in place
            if (r == 0 && f == 0)
                continue;

            //Get new rank and new file
            int newRank = rank + r;
            int newFile = file + f;

            //If we don't go out of bounds add new moves
            if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8)
            {
                setBit(moves, newRank * 8 + newFile);
            }
        }
    }

    return moves;
}


//Generate pawn lookup for white and black
void BB_utils::generatePawnMoves(Bitboard (&pawnMove)[64], Bitboard (&pawnAttack)[64], bool isWhite)
{
    //Foreach square
    for (int square = 0; square < 64; ++square)
    {
        //Initialize moves attacks and square
        Bitboard moves = 0ULL;
        Bitboard attacks = 0ULL;
        int rank = square / 8;
        int file = square % 8;

        // For white we go up
        if (isWhite)
        {
            //We cannot be on rank 7 since it is last rank
            if (rank < 7)
            {
                setBit(moves, (rank + 1) * 8 + file);
            }
        } else // We are black we go down
        {
            //We cannot be on rank 0 since it is first rank
            if (rank > 0)
            {
                setBit(moves, (rank - 1) * 8 + file);
            }
        }

        // Captures
        if (isWhite)
        {
            // We can only capture before 7 rank

            //Skip left edge
            if (rank < 7 && file > 0)
            {
                setBit(attacks, (rank + 1) * 8 + (file - 1));
            }

            //Skip right edge
            if (rank < 7 && file < 7)
            {
                setBit(attacks, (rank + 1) * 8 + (file + 1));
            }

        } else // We are black
        {

            // We can only capture before 0 rank

            //Skip right edge
            if (rank > 0 && file > 0)
            {
                setBit(attacks, (rank - 1) * 8 + (file - 1));
            }

            //Skip left edge
            if (rank > 0 && file < 7)
            {
                setBit(attacks, (rank - 1) * 8 + (file + 1));
            }
        }

        //Update lookup tables
        pawnMove[square] = moves;
        pawnAttack[square] = attacks;
    }
}

//Magic lookup init
void BB_utils::initRookAttacks()
{
    //For every square
    for (int square = 0; square < BOARD_SIZE; ++square) {

        //Vector for this square combinations
        vector<Bitboard> tempSquare;

        //Max mask size 2^12 because of skipping edges so we initialize to max size
        tempSquare.resize(4096);

        //Foreach possible blockers setup we get index
        //Here we loop to 1 << bits so its less than 4096
        //4096 is upper bound because we don't know if we get magic index like 4095 or sth like that
        for (int occupancy = 0; occupancy < rookBlockers[square].size(); ++occupancy)
        {
            //Index calculated by magics
            int index = getMagicIndex(rookBlockers[square][occupancy], rooksMagics[square], rookBits[square]);

            //Attacks generated for given blockMask
            Bitboard attacks = generateRookBitboardAttacksBlockers(square,rookBlockers[square][occupancy]);

            //Assign attacks to magic index lookup table
            tempSquare[index] = attacks;
        }

        //Assign square vector to whole piece vector lookup
        rookMoves.push_back(tempSquare);
    }
}

//Magic lookup init
void BB_utils::initBishopAttacks()
{

    //For every square
    for (int square = 0; square < BOARD_SIZE; ++square)
    {

        //Vector for this square combinations
        vector<Bitboard> tempSquare;

        //Max mask size 2^9 because of skipping edges so we initialize to max size
        tempSquare.resize(512);

        //Foreach possible blockers setup we get index
        //Here we loop to 1 << bits so its less than 4096
        //4096 is upper bound because we don't know if we get magic index like 511 or sth like that
        for (int occupancy = 0; occupancy < bishopBlockers[square].size(); ++occupancy)
        {

            //Foreach possible blockers setup we get index
            //Here we loop to 1 << bits so its less than 4096
            //4096 is upper bound because we don't know if we get magic index like 4095 or sth like that
            int index = getMagicIndex(bishopBlockers[square][occupancy], bishopsMagics[square], bishopBits[square]);

            //Attacks generated for given blockMask
            Bitboard attacks = generateBishopBitboardAttacksBlockers(square,bishopBlockers[square][occupancy]);

            //Assign attacks to magic index lookup table
            tempSquare[index] = attacks;
        }

        //Assign square vector to whole piece vector lookup
        bishopMoves.push_back(tempSquare);
    }
}

//Magic index is calculated for 64 bits magics
//Bits are different for rook and bishop for each square
int BB_utils::getMagicIndex(Bitboard blockers, Bitboard magic, int bits)
{
    return (int)((blockers * magic) >> (64 - bits));
}

//Normal movegen like masks but for given blockers
Bitboard BB_utils::generateRookBitboardAttacksBlockers(int sq, Bitboard blockers)
{
    //Moves and square initialization
    Bitboard moves = 0;
    int row = sq / 8;
    int col = sq % 8;

    //Right movement
    for (int j = col + 1; j < 8; ++j)
    {
        //Index of current square we iterate
        int temp = 8 * row + j;

        //Append index to moves
        moves |= 1ULL << temp;

        //If blockers intersect with current index we end this direction
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    //Left movement
    for (int j = col - 1; j >= 0; --j)
    {
        //Index of current square we iterate
        int temp = 8 * row + j;

        //Append index to moves
        moves |= 1ULL << temp;

        //If blockers intersect with current index we end this direction
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    //Down movement
    for (int j = row + 1; j < 8; ++j)
    {
        //Index of current square we iterate
        int temp = 8 * j + col;

        //Append index to moves
        moves |= 1ULL << temp;

        //If blockers intersect with current index we end this direction
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    //Up movement
    for (int j = row - 1; j >= 0; --j)
    {
        //Index of current square we iterate
        int temp = 8 * j + col;

        //Append index to moves
        moves |= 1ULL << temp;

        //If blockers intersect with current index we end this direction
        if ((blockers & (1ULL << temp)) == (1ULL << temp))
            break;
    }

    return moves;
}

//Normal movegen like masks but for given blockers
Bitboard BB_utils::generateBishopBitboardAttacksBlockers(int sq, Bitboard blockers) {

    //Moves and square initialization
    Bitboard moves = 0;
    int row = sq / 8;
    int col = sq % 8;

    // Diagonal left down
    int r = row + 1;
    int c = col - 1;
    while (r < 8 && c >= 0)
    {
        //Append to moves current index
        moves |= 1ULL << (r * 8 + c);

        //If blockers intersect with current index we end this direction
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;

        //Update index values
        ++r;
        --c;
    }

    // Diagonal right down
    r = row + 1;
    c = col + 1;
    while (r < 8 && c < 8)
    {
        //Append to moves current index
        moves |= 1ULL << (r * 8 + c);

        //If blockers intersect with current index we end this direction
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;

        //Update index values
        ++r;
        ++c;
    }

    // Diagonal left up
    r = row - 1;
    c = col - 1;
    while (r >= 0 && c >= 0)
    {
        //Append to moves current index
        moves |= 1ULL << (r * 8 + c);

        //If blockers intersect with current index we end this direction
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;

        //Update index values
        --r;
        --c;
    }

    // Diagonal right up
    r = row - 1;
    c = col + 1;
    while (r >= 0 && c < 8)
    {
        //Append to moves current index
        moves |= 1ULL << (r * 8 + c);

        //If blockers intersect with current index we end this direction
        if ((blockers & (1ULL << (r * 8 + c))) == (1ULL << (r * 8 + c)))
            break;

        //Update index values
        --r;
        ++c;
    }

    return moves;
}
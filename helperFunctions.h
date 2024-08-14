#pragma once
#include <string>
#include <iostream>
#include "move.h"

// We take index of sqaure and convert it to name of square
inline std::string squareToName(int index)
{
    // We take 'a' char value and add file value from a-h
    // a = 0, b = 1, ..., h = 7
    // Then 'a' + file value gives us new char
    char file = 'a' + (index % 8);

    // Then we take '1' char value and add rank value from 1-8
    // 1st rank gives 0, 2nd rank gives 1, ..., 8th rank gives 7
    // Then 'a' + rank value gives us new char
    char rank = '1' + (index / 8);

    // Now we return strings combined
    return std::string(1, file) + std::string(1, rank);
}

// Take name of square and convert it to square index
// Considering its legal
inline int nameToSquare(std::string square)
{

    // Take file name as char
    char file = tolower(square[0]);

    // Then rank name as char
    char rank = square[1];

    // Index calculation is:
    // For file: 'file char' - 'a' so for example 'c' - 'a' = 2  [99 - 97 = 2]
    // For rank: 'rank char' - '1' so for example '4' - '1' = 3  [52 - 49 = 3]
    int index = (file - 'a') + (rank - '1') * 8;

    return index;
}

// Initializing castling rights from fen string
inline int castlingRights(std::string txt)
{
    // Default value = 0
    int value = 0;

    // For each char we add special number to the value
    // that corresponds to certain castling type
    // 8 => White short
    // 4 => White long
    // 2 => Black short
    // 1 => Black long
    for (char c : txt)
    {
        switch (c)
        {
        case 'K':
            value += 8;
            break;
        case 'Q':
            value += 4;
            break;
        case 'k':
            value += 2;
            break;
        case 'q':
            value += 1;
            break;
        default:
            break;
        }
    }

    return value;
}

// Print move in uci form
inline void printMove(Move m)
{
    // Prepare move info
    int startSquare = StartSquare(m);

    int targetSquare = TargetSqaure(m);

    int flags = Flags(m);

    // Use functions for name conversion
    // Print start sqaure and target square
    std::cout << squareToName(startSquare) << squareToName(targetSquare);

    // Add letter for promotion
    if (flags == KNIGHT_PROMOTION || flags == KNIGHT_PROMOTION_CAPTURE)
        std::cout << "n";
    if (flags == BISHOP_PROMOTION || flags == BISHOP_PROMOTION_CAPTURE)
        std::cout << "b";
    if (flags == ROOK_PROMOTION || flags == ROOK_PROMOTION_CAPTURE)
        std::cout << "r";
    if (flags == QUEEN_PROMOTION || flags == QUEEN_PROMOTION_CAPTURE)
        std::cout << "q";
}

// Return move in uci form
inline std::string moveToUci(Move move)
{

    // Prepare move info
    int startSquare = StartSquare(move);
    int targetSquare = TargetSqaure(move);
    int flags = Flags(move);

    // Print move in uci format
    std::string uciMove = squareToName(startSquare) + squareToName(targetSquare);

    // Add letter for promotion
    if (flags == KNIGHT_PROMOTION || flags == KNIGHT_PROMOTION_CAPTURE)
        uciMove += "n";
    if (flags == BISHOP_PROMOTION || flags == BISHOP_PROMOTION_CAPTURE)
        uciMove += "b";
    if (flags == ROOK_PROMOTION || flags == ROOK_PROMOTION_CAPTURE)
        uciMove += "r";
    if (flags == QUEEN_PROMOTION || flags == QUEEN_PROMOTION_CAPTURE)
        uciMove += "q";

    return uciMove;
}
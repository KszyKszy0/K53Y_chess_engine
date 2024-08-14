#pragma once
#include <stdint.h>

// Move definition
typedef uint16_t Move;

// Returns start square of move
inline int StartSquare(Move m)
{
    return m & 0x3F;
}

// Returns target square of move
inline int TargetSqaure(Move m)
{
    return (m >> 6) & 0x3F;
}

// Returns flags of move
inline int Flags(Move m)
{
    return (m >> 12) & 0b1111;
}

// Move flags
enum MoveFlag
{
    QUIET,
    PAWN_DOUBLE_MOVE,
    SHORT_CASTLE,
    LONG_CASTLE,
    CAPTURE,
    EN_PASSANT = 5,

    EMPTY_1 = 6,
    EMPTY_2 = 7,

    KNIGHT_PROMOTION = 8,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
    KNIGHT_PROMOTION_CAPTURE,
    BISHOP_PROMOTION_CAPTURE,
    ROOK_PROMOTION_CAPTURE,
    QUEEN_PROMOTION_CAPTURE
};

// Simple array wrapped with size and check counted
// Size and checks used for mate/stalemate detection
struct MoveList
{
    Move moveList[218];
    Move *cur = &moveList[0];
    Move *last;
    int checks;
    int size;

    MoveList()
    {
        checks = 0;
        size = 0;
        cur = &moveList[0];
    }

    // Access first element of Movelist
    Move *begin()
    {
        return &moveList[0];
    }

    // Access last element of Movelist
    Move *end()
    {
        return &moveList[size];
    }

    // Check if there is checkmate in current position
    // Size eqauls 0? && Checks greater than 0?
    bool isCheckmate()
    {
        return (size == 0) && (checks > 0);
    }

    // Check if there is stalemate in current position
    // Size eqauls 0? && Checks equal 0?
    bool isStalemate()
    {
        return (size == 0) && (checks == 0);
    }
};

// Simple move constructor
// Start square
// Target square << 6 (shifted 6 bits left)
// Flags << 12 (shifted 12 bits left)
inline Move createMove(int startSquare, int targetSquare, int flags)
{
    return startSquare + (targetSquare << 6) + (flags << 12);
}

// We take index of sqaure and convert it to name of square
inline string squareToName(int index)
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
    return string(1, file) + string(1, rank);
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
    cout << squareToName(startSquare) << squareToName(targetSquare);

    // Add letter for promotion
    if (flags == KNIGHT_PROMOTION || flags == KNIGHT_PROMOTION_CAPTURE)
        cout << "n";
    if (flags == BISHOP_PROMOTION || flags == BISHOP_PROMOTION_CAPTURE)
        cout << "b";
    if (flags == ROOK_PROMOTION || flags == ROOK_PROMOTION_CAPTURE)
        cout << "r";
    if (flags == QUEEN_PROMOTION || flags == QUEEN_PROMOTION_CAPTURE)
        cout << "q";
}

// Return move in uci form
inline string moveToUci(Move move)
{

    // Prepare move info
    int startSquare = StartSquare(move);
    int targetSquare = TargetSqaure(move);
    int flags = Flags(move);

    // Print move in uci format
    string uciMove = squareToName(startSquare) + squareToName(targetSquare);

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


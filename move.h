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

// Simple move constructor
// Start square
// Target square << 6 (shifted 6 bits left)
// Flags << 12 (shifted 12 bits left)
inline Move createMove(int startSquare, int targetSquare, int flags)
{
    return startSquare + (targetSquare << 6) + (flags << 12);
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

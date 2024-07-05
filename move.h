#pragma once
#include <stdint.h>

typedef uint16_t Move;

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



inline Move createMove(int startSquare, int targetSquare, int flags){
    return startSquare + (targetSquare << 6) + (flags << 12);
}

inline void printMove(Move m)
{
    int startSquare = m & 0b111111;
    int targetSquare = m>>6 & 0b111111;
    int flags = m>>12 & 0b1111;
    cout<<startSquare<<" "<<targetSquare<<" "<<flags<<endl;
}
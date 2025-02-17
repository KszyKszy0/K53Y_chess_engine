#pragma once
#include "enums.h"

class StateInfo
{
    public:
    //Enpassant square => 0 if none
    int enPassantSquare;

    //Bits on king and rook squares
    Bitboard castlingRights;

    //Halfmove counter 100 = draw, resets after capture or pawn move
    int halfMove;

    //Increments by one after black move
    int fullMove;

    //Info about piece that was captured this move, NOPIECE if move didnt capture
    int capturedPieceType;

    float accumulator[2][16];

    StateInfo(int pas, Bitboard cast, int half, int full, int captureType);

    StateInfo();
};


inline StateInfo::StateInfo(int pas, Bitboard cast, int half, int full, int captureType)
{
    enPassantSquare=pas;
    castlingRights=cast;
    halfMove=half;
    fullMove=full;
    capturedPieceType=captureType;
}

inline StateInfo::StateInfo()
{
    enPassantSquare=0;
    castlingRights= K | k | Q | q;
    halfMove=0;
    fullMove=1;
    capturedPieceType=NO_PIECE;
}
#pragma once

class StateInfo
{
    public:
    int enPassantSquare;

    int castlingRights;        //KQkq 8421

    int halfMove;

    int fullMove;

    int capturedPieceType;

    StateInfo(int pas, int cast, int half, int full, int captureType);

    StateInfo();
};


inline StateInfo::StateInfo(int pas, int cast, int half, int full, int captureType)
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
    castlingRights=15;
    halfMove=0;
    fullMove=1;
    capturedPieceType=NO_PIECE;
}
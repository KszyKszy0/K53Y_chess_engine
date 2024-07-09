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
};
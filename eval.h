#pragma once
#include "position.h"
#include "bitboard.h"


inline int evaluate(Position& pos)
{
    int pawns = (popCount(pos.piecesBitboards[WHITE_PAWN]) - popCount(pos.piecesBitboards[BLACK_PAWN]))*110;
    int knight = (popCount(pos.piecesBitboards[WHITE_KNIGHT]) - popCount(pos.piecesBitboards[BLACK_KNIGHT]))*300;
    int bishop = (popCount(pos.piecesBitboards[WHITE_BISHOP]) - popCount(pos.piecesBitboards[BLACK_BISHOP]))*350;
    int rook = (popCount(pos.piecesBitboards[WHITE_ROOK]) - popCount(pos.piecesBitboards[BLACK_ROOK]))*500;
    int queens = (popCount(pos.piecesBitboards[WHITE_QUEEN]) - popCount(pos.piecesBitboards[BLACK_QUEEN]))*900;
    return pawns + knight + bishop + rook + queens;
}

#include "eval.h"

int Evaluator::evaluate(Position& pos)
{
    int eval=0;
    int pawns = (popCount(pos.piecesBitboards[WHITE_PAWN]) - popCount(pos.piecesBitboards[BLACK_PAWN]))*110;
    int knight = (popCount(pos.piecesBitboards[WHITE_KNIGHT]) - popCount(pos.piecesBitboards[BLACK_KNIGHT]))*300;
    int bishop = (popCount(pos.piecesBitboards[WHITE_BISHOP]) - popCount(pos.piecesBitboards[BLACK_BISHOP]))*350;
    int rook = (popCount(pos.piecesBitboards[WHITE_ROOK]) - popCount(pos.piecesBitboards[BLACK_ROOK]))*500;
    int queens = (popCount(pos.piecesBitboards[WHITE_QUEEN]) - popCount(pos.piecesBitboards[BLACK_QUEEN]))*900;

    Bitboard white = pos.piecesBitboards[WHITE_PIECES];

    Bitboard black = pos.piecesBitboards[BLACK_PIECES];

    while(white)
    {
        int index = popLSB(white);
        eval += psqtArray[pos.piecesArray[index]][index];
    }

    while(black)
    {
        int index = popLSB(black);
        eval -= psqtArray[pos.piecesArray[index]][index];
    }

    return pawns + knight + bishop + rook + queens + eval;
}
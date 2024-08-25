#include "eval.h"

int Evaluator::evaluate(Position& pos)
{
    //Static eval
    int eval=0;

    //Material
    int pawns = (popCount(pos.piecesBitboards[WHITE_PAWN]) - popCount(pos.piecesBitboards[BLACK_PAWN]))*110;
    int knight = (popCount(pos.piecesBitboards[WHITE_KNIGHT]) - popCount(pos.piecesBitboards[BLACK_KNIGHT]))*300;
    int bishop = (popCount(pos.piecesBitboards[WHITE_BISHOP]) - popCount(pos.piecesBitboards[BLACK_BISHOP]))*350;
    int rook = (popCount(pos.piecesBitboards[WHITE_ROOK]) - popCount(pos.piecesBitboards[BLACK_ROOK]))*600;
    int queens = (popCount(pos.piecesBitboards[WHITE_QUEEN]) - popCount(pos.piecesBitboards[BLACK_QUEEN]))*1150;


    //PSQT
    Bitboard white = pos.piecesBitboards[WHITE_PIECES];

    Bitboard black = pos.piecesBitboards[BLACK_PIECES];

    while(white)
    {
        int index = popLSB(white);
        eval += psqtArray[pos.piecesArray[index]][flipIndex(index)];
    }

    while(black)
    {
        int index = popLSB(black);
        eval -= psqtArray[pos.piecesArray[index] - 6][index];
    }

    return pawns + knight + bishop + rook + queens + eval;
}



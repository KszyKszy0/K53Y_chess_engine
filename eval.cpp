#include "eval.h"
#include <fstream>

int Evaluator::evaluate(Position& pos)
{
    //Static eval
    int eval=0;

    //Material
    int pawns = (popCount(pos.piecesBitboards[WHITE_PAWN]) - popCount(pos.piecesBitboards[BLACK_PAWN]))*psqtArray[384];
    int knight = (popCount(pos.piecesBitboards[WHITE_KNIGHT]) - popCount(pos.piecesBitboards[BLACK_KNIGHT]))*psqtArray[385];
    int bishop = (popCount(pos.piecesBitboards[WHITE_BISHOP]) - popCount(pos.piecesBitboards[BLACK_BISHOP]))*psqtArray[386];
    int rook = (popCount(pos.piecesBitboards[WHITE_ROOK]) - popCount(pos.piecesBitboards[BLACK_ROOK]))*psqtArray[387];
    int queens = (popCount(pos.piecesBitboards[WHITE_QUEEN]) - popCount(pos.piecesBitboards[BLACK_QUEEN]))*psqtArray[388];


    //PSQT
    Bitboard white = pos.piecesBitboards[WHITE_PIECES];

    Bitboard black = pos.piecesBitboards[BLACK_PIECES];

    while(white)
    {
        int index = popLSB(white);
        eval += psqtArray[pos.piecesArray[index]*64 + flipIndex(index)];
    }

    while(black)
    {
        int index = popLSB(black);
        eval -= psqtArray[(pos.piecesArray[index] - 6)*64 + index];
    }

    return (pawns + knight + bishop + rook + queens + eval) * (pos.STM ? 1 : -1);
}

void Evaluator::readData()
{
    std::fstream file("weights.txt");
    int value = 0;
    for(int i=0; i < 389; i++)
    {
        file >> value;
        psqtArray[i] = value;
    }

    file.close();

    // for(int i=0; i < 389; i++)
    // {
    //     cout<<psqtArray[i]<<" ";
    // }
}
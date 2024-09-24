#include "eval.h"
#include <fstream>


int psqtArray[389] =
{

    //PAWN

        0, 0, 0, 0, 0, 0, 0, 0,
        6, 6, 6, 7, 7, 6, 6, 6,
        8, 7, 6, 7, 7, 6, 7, 8,
        7, 3, 4, 6, 6, 4, 3, 5,
        5, 1, 6, 7, 7, 5, 1, 4,
        5, 8, 1, 5, 5, 1, 8, 4,
        5, 6, 3, 1, 2, 3, 6, 5,
        0, 0, 0, 0, 0, 0, 0, 0,


    //KNIGHT
        -100, 0, 0, 0, 0, 0, 0, -100,
        3, 1, 4, 0, 0, 4, 1, 3,
        1, 6, 3, 6, 6, 3, 6, 1,
        2, 5, 4, 4, 4, 4, 5, 2,
        3, 3, 4, 3, 3, 4, 3, 3,
        1, 3, 5, 1, 1, 5, 3, 1,
        0, 0, 0, 3, 3, 0, 0, 0,
        -100, 0, 2, 0, 2, 0, 0, -100,

    //BISHOP
        -5, -5, 0, 0, 0, 0, -5, -5,
        0, 1, 2, 3, 3, 2, 1, 0,
        1, 5, 5, 8, 8, 4, 4, 1,
        2, 4, 5, 7, 7, 5, 4, 2,
        4, 4, 5, 3, 3, 5, 4, 4,
        -2, 4, 4, 7, 7, 4, 4, -2,
        3, 8, 3, 6, 6, 1, 8, 2,
        -5, -5, 4, 0, 0, 4, -5, -5,


    //ROOK
        5, 10, 10, 10, 10, 10, 10, 5,
        15, 15, 15, 15, 15, 15, 15, 15,
        0, 1, 1, 2, 2, 1, 1, 0,
        0, 0, 3, 3, 3, 3, 0, 0,
        0, 0, 3, 3, 3, 3, 0, 0,
        4, 4, 6, 6, 6, 6, 4, 4,
        8, 8, 8, 8, 8, 8, 8, 8,
        5, 0, 0, 10, 10, 10, 0, 5,


    //QUEEN
        -2, 2, 2, 1, 1, 2, 2, -2,
        10, 5, 10, 8, 8, 10, 5, 10,
        1, 7, 8, 9, 9, 8, 7, 1,
        1, 1, 6, 4, 4, 8, 7, 4,
        5, 0, 2, 4, 4, 2, 0, 3,
        3, 7, 2, 2, 2, 5, 7, 3,
        -4, -5, 10, 8, 8, -1, -4, -5,
        0, 0, 2, 4, 1, 0, 0, 0,


    //KING

        -10, 0, 1, 0, 0, 0, 1, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 0, 1, 1, 0, 0, -10,
        -10, 0, 0, 1, 1, 0, 0, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 1, 0, 0, 0, 1, -10,

        //Material values
        110, 300, 350, 600, 1150
};

int evaluate(Position& pos)
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

void readData()
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
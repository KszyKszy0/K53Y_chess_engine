#pragma once
#include <stdint.h>


//PieceTypes definition
enum pieceType
{   WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
    WHITE_PIECES, BLACK_PIECES, ALL_PIECES, NO_PIECE
};

//Squares to numebrs definition
enum enumSquares {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8,
  NO_SQUARE
};

//Colors definition
enum colors{
    WHITE=true, BLACK=false
};

//Node type for Transposition Table
enum nodeType{
    //In AB window
    EXACT_SCORE=1,
    //Cutoff
    LOWER_BOUND,
    //Fail low
    UPPER_BOUND
};


//Scores given for alpha, nomove, checkmate and time interrupt
//So we dont end up in not picking any move
//Also values need to be considered so we pick anything over nomove and alpha
//Also alpha should be less than nomove so at least something is picked
enum specialScores{

    //Used as lowest score so any move beats it in search
    NO_MOVE=2000000000,

    //Simply checkmate score BUT
    //Checkmate scores will always be smaller because of - ply
    //Checkmate value indicates that the search was cancelled because of time
    CHECKMATE=1000000000,

    //Used for alpha and beta so even NO_MOVE beats it so any move is picked in search
    INF=2100000000
};
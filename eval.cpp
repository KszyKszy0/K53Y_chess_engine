#include "eval.h"
#include <fstream>
#include "accumulator.h"
#include "nnue.h"



int psqtArray[389] =
{

    // //PAWN

    //     0, 0, 0, 0, 0, 0, 0, 0,
    //     6, 6, 6, 7, 7, 6, 6, 6,
    //     8, 7, 6, 7, 7, 6, 7, 8,
    //     7, 3, 4, 6, 6, 4, 3, 5,
    //     5, 1, 6, 7, 7, 5, 1, 4,
    //     5, 8, 1, 5, 5, 1, 8, 4,
    //     5, 6, 3, 1, 2, 3, 6, 5,
    //     0, 0, 0, 0, 0, 0, 0, 0,


    // //KNIGHT
    //     -100, 0, 0, 0, 0, 0, 0, -100,
    //     3, 1, 4, 0, 0, 4, 1, 3,
    //     1, 6, 3, 6, 6, 3, 6, 1,
    //     2, 5, 4, 4, 4, 4, 5, 2,
    //     3, 3, 4, 3, 3, 4, 3, 3,
    //     1, 3, 5, 1, 1, 5, 3, 1,
    //     0, 0, 0, 3, 3, 0, 0, 0,
    //     -100, 0, 2, 0, 2, 0, 0, -100,

    // //BISHOP
    //     -5, -5, 0, 0, 0, 0, -5, -5,
    //     0, 1, 2, 3, 3, 2, 1, 0,
    //     1, 5, 5, 8, 8, 4, 4, 1,
    //     2, 4, 5, 7, 7, 5, 4, 2,
    //     4, 4, 5, 3, 3, 5, 4, 4,
    //     -2, 4, 4, 7, 7, 4, 4, -2,
    //     3, 8, 3, 6, 6, 1, 8, 2,
    //     -5, -5, 4, 0, 0, 4, -5, -5,


    // //ROOK
    //     5, 10, 10, 10, 10, 10, 10, 5,
    //     15, 15, 15, 15, 15, 15, 15, 15,
    //     0, 1, 1, 2, 2, 1, 1, 0,
    //     0, 0, 3, 3, 3, 3, 0, 0,
    //     0, 0, 3, 3, 3, 3, 0, 0,
    //     4, 4, 6, 6, 6, 6, 4, 4,
    //     8, 8, 8, 8, 8, 8, 8, 8,
    //     5, 0, 0, 10, 10, 10, 0, 5,


    // //QUEEN
    //     -2, 2, 2, 1, 1, 2, 2, -2,
    //     10, 5, 10, 8, 8, 10, 5, 10,
    //     1, 7, 8, 9, 9, 8, 7, 1,
    //     1, 1, 6, 4, 4, 8, 7, 4,
    //     5, 0, 2, 4, 4, 2, 0, 3,
    //     3, 7, 2, 2, 2, 5, 7, 3,
    //     -4, -5, 10, 8, 8, -1, -4, -5,
    //     0, 0, 2, 4, 1, 0, 0, 0,


    // //KING

    //     -10, 0, 1, 0, 0, 0, 1, -10,
    //     -10, 0, 0, 0, 0, 0, 0, -10,
    //     -10, 0, 0, 0, 0, 0, 0, -10,
    //     -10, 0, 0, 1, 1, 0, 0, -10,
    //     -10, 0, 0, 1, 1, 0, 0, -10,
    //     -10, 0, 0, 0, 0, 0, 0, -10,
    //     -10, 0, 0, 0, 0, 0, 0, -10,
    //     -10, 0, 1, 0, 0, 0, 1, -10,

    //     //Material values
    //     110, 300, 350, 600, 1150




0,      0,      0,      0,      0,      0,      0,      0,
336,    362,    345,    319,    320,    330,    408,    310,
345,    311,    320,    344,    325,    320,    284,    359,
340,    272,    305,    330,    329,    299,    275,    304,
303,    262,    320,    342,    338,    302,    269,    280,
311,    363,    255,    300,    303,    256,    364,    289,
309,    332,    266,    246,    272,    279,    336,    300,
0,      0,      0,      0,      0,      0,      0,      0,


-5861,  179,    222,    226,    245,    -63,    135,    -6396,
178,    196,    235,    242,    237,    265,    183,    189,
179,    237,    194,    258,    300,    218,    237,    155,
197,    227,    224,    250,    218,    213,    211,    200,
183,    161,    203,    191,    194,    209,    140,    187,
138,    172,    210,    155,    161,    201,    165,    143,
138,    119,    116,    166,    173,    131,    158,    142,
-5975,  127,    143,    112,    153,    118,    128,    -6047,


-104,   -174,   236,    313,    340,    295,    -145,   193,
244,    276,    307,    302,    293,    313,    307,    224,
281,    315,    315,    343,    348,    323,    330,    278,
280,    283,    298,    339,    338,    327,    280,    266,
274,    287,    296,    302,    283,    303,    302,    278,
287,    272,    288,    311,    313,    289,    278,    303,
240,    333,    249,    297,    299,    248,    334,    255,
257,    208,    272,    231,    241,    268,    175,    292,


474,    461,    437,    463,    438,    432,    436,    417,
523,    523,    527,    538,    517,    520,    549,    510,
435,    400,    438,    457,    463,    435,    427,    440,
358,    420,    436,    410,    428,    392,    364,    328,
345,    348,    412,    385,    383,    373,    371,    352,
356,    422,    398,    383,    394,    376,    378,    345,
391,    402,    408,    418,    410,    405,    410,    390,
374,    358,    412,    427,    428,    426,    378,    371,


372,    360,    358,    281,    336,    303,    381,    388,
351,    333,    371,    373,    367,    359,    381,    364,
298,    353,    339,    372,    370,    367,    378,    310,
304,    300,    333,    318,    315,    352,    319,    298,
302,    289,    310,    300,    308,    287,    309,    271,
271,    324,    304,    292,    290,    296,    314,    279,
282,    308,    350,    328,    327,    322,    304,    303,
229,    271,    283,    292,    250,    235,    222,    189,


-601,   237,    32,     -60,    -60,    -60,    60,     -601,
-601,   9,      -3,     -18,    246,    -60,    -60,    -601,
-611,   -28,    1,      -71,    17,     12,     308,    -609,
-589,   -25,    15,     -59,    -15,    -26,    -35,    -450,
-300,   -76,    -46,    -9,     -32,    -84,    -24,    -59,
-123,   -61,    -47,    -48,    -45,    -46,    -52,    -99,
-160,   -40,    -42,    -47,    -43,    -38,    -40,    -106,
-117,   -34,    -30,    -55,    -40,    -49,    -27,    -131,


6358, 19489, 21175, 36024, 69350,
};

int evaluate(Position& pos, Accumulator& accum)
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

    // return firstLayer(accum.values, pos.STM) * (pos.STM ? 1 : -1);
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
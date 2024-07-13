#pragma once
#include <stdint.h>
#include <string>

typedef uint16_t Move;

enum MoveFlag
{
    QUIET,
    PAWN_DOUBLE_MOVE,
    SHORT_CASTLE,
    LONG_CASTLE,
    CAPTURE,
    EN_PASSANT = 5,

    EMPTY_1 = 6,
    EMPTY_2 = 7,

    KNIGHT_PROMOTION = 8,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
    KNIGHT_PROMOTION_CAPTURE,
    BISHOP_PROMOTION_CAPTURE,
    ROOK_PROMOTION_CAPTURE,
    QUEEN_PROMOTION_CAPTURE
};



inline Move createMove(int startSquare, int targetSquare, int flags){
    return startSquare + (targetSquare << 6) + (flags << 12);
}

inline string squareToName(int index)
{
    char file = 'a' + (index % 8);
    // Rzędy szachownicy od '1' do '8'
    char rank = '1' + (index / 8);

    return std::string(1, file) + std::string(1, rank);
}

inline int nameToSquare(std::string square) {

    char file = tolower(square[0]);
    // Rzędy od '1' do '8'
    char rank = square[1];

    // Obliczenie indeksu
    int index = (file - 'a') + (rank - '1') * 8;

    return index;
}

inline int castlingRights(std::string txt) {
    int value = 0;
    for(char c : txt)
    {
        switch(c)
        {
            case 'K':
            value+=8;
            break;
            case 'Q':
            value+=4;
            break;
            case 'k':
            value+=2;
            break;
            case 'q':
            value+=1;
            break;
            default:
            break;
        }
    }

    return value;
}

inline void printMove(Move m)
{
    int startSquare = m & 0b111111;
    int targetSquare = m>>6 & 0b111111;
    int flags = m>>12 & 0b1111;
    cout<<squareToName(startSquare)<<" "<<squareToName(targetSquare)<<" "<<flags<<endl;
}
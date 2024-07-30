#pragma once
#include <stdint.h>


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

struct MoveList
{
    Move moveList[218];
    Move* cur = &moveList[0];
    Move* last;
    int size;

    MoveList()
    {
        size = 0;
        cur = &moveList[0];
    }

    Move* begin() {
        return &moveList[0];
    }

    Move* end() {
        return &moveList[size];
    }
};

inline Move createMove(int startSquare, int targetSquare, int flags){
    return startSquare + (targetSquare << 6) + (flags << 12);
}

inline string squareToName(int index)
{
    char file = 'a' + (index % 8);
    // Rzędy szachownicy od '1' do '8'
    char rank = '1' + (index / 8);

    return string(1, file) + string(1, rank);
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
    cout<<squareToName(startSquare)<<squareToName(targetSquare);//<<flags<<endl;
    if(flags == KNIGHT_PROMOTION || flags == KNIGHT_PROMOTION_CAPTURE)
        cout<<"n";
    if(flags == BISHOP_PROMOTION || flags == BISHOP_PROMOTION_CAPTURE)
        cout<<"b";
    if(flags == ROOK_PROMOTION || flags == ROOK_PROMOTION_CAPTURE)
        cout<<"r";
    if(flags == QUEEN_PROMOTION || flags == QUEEN_PROMOTION_CAPTURE)
        cout<<"q";
}


inline string moveToUci(Move move) {
    int startSquare = move & 0x3F;        // 6 bitów na startowe pole
    int targetSquare = (move >> 6) & 0x3F; // Kolejne 6 bitów na docelowe pole
    int flags = (move >> 12) & 0xF;       // Ostatnie 4 bity na flagi

    string uciMove = squareToName(startSquare) + squareToName(targetSquare);

    // Dodawanie odpowiednich promocyjnych figur, jeśli jest to promocja
    switch (flags) {
        case KNIGHT_PROMOTION:
        case KNIGHT_PROMOTION_CAPTURE:
            uciMove += 'n';
            break;
        case BISHOP_PROMOTION:
        case BISHOP_PROMOTION_CAPTURE:
            uciMove += 'b';
            break;
        case ROOK_PROMOTION:
        case ROOK_PROMOTION_CAPTURE:
            uciMove += 'r';
            break;
        case QUEEN_PROMOTION:
        case QUEEN_PROMOTION_CAPTURE:
            uciMove += 'q';
            break;
        default:
            break;
    }

    return uciMove;
}

inline Move uciToMove(const std::string& uci, Position& pos, Move* moveList) {
    int startSquare = nameToSquare(uci.substr(0, 2));
    int targetSquare = nameToSquare(uci.substr(2, 2));
    int flags = QUIET;


    Move targetMove = 0;

    if (uci.length() == 5) {
        switch (uci[4]) {
            case 'n':
                flags = KNIGHT_PROMOTION;
                break;
            case 'b':
                flags = BISHOP_PROMOTION;
                break;
            case 'r':
                flags = ROOK_PROMOTION;
                break;
            case 'q':
                flags = QUEEN_PROMOTION;
                break;
        }
        if(pos.piecesArray[targetSquare] != NO_PIECE)
        {
            flags += 4;
        }
    }else
    {
        Move* temp = moveList;
        while(*temp != 0)
        {
            int mStartSquare = *temp & 0x3F;
            int mtargetSquare = (*temp >> 6) & 0x3F;
            if((mStartSquare == startSquare) && (mtargetSquare == targetSquare))
            {
                return *temp;
            }
            *temp++;
        }
    }



    // Dodatkowa logika do rozpoznawania innych flag (np. roszady, bicie, en passant)
    // Na potrzeby tego przykładu zakładamy, że nie są obecne inne specjalne ruchy

    return createMove(startSquare, targetSquare, flags);
}

inline int StartSquare(Move m)
{
    return m & 0x3F;
}

inline int TargetSqaure(Move m)
{
    return (m >> 6) & 0x3F;
}

inline int Flags(Move m)
{
    return (m >> 12) & 0b1111;
}
#include "position.h"
#include <sstream>
#include "enums.h"
using namespace std;

void Position::parseFEN(string fen, Bitboard (&bitboards)[15])
{
    BB_utils helper;
    for (int i = 0; i < 13; i++)
    {
        bitboards[i] = 0ULL;
    }

    istringstream ss(fen);
    string board, turn, castling, enPassant;
    int halfmove, fullmove;

    ss >> board >> turn >> castling >> enPassant >> halfmove >> fullmove;
    // cout<<board<<endl<<turn<<endl<<castling<<endl<<halfmove<<endl<<fullmove<<endl;

    int rank = 7;
    int file = 0;

    for (char ch : board)
    {
        if (ch == '/')
        {
            rank--;
            file = 0;
        }
        else if (isdigit(ch))
        {
            file += ch - '0';
        }
        else
        {
            int position = rank * 8 + file;

            switch (ch)
            {
            case 'P':
                helper.setBit(bitboards[WHITE_PAWN], position);
                break;
            case 'N':
                helper.setBit(bitboards[WHITE_KNIGHT], position);
                break;
            case 'B':
                helper.setBit(bitboards[WHITE_BISHOP], position);
                break;
            case 'R':
                helper.setBit(bitboards[WHITE_ROOK], position);
                break;
            case 'Q':
                helper.setBit(bitboards[WHITE_QUEEN], position);
                break;
            case 'K':
                helper.setBit(bitboards[WHITE_KING], position);
                break;
            case 'p':
                helper.setBit(bitboards[BLACK_PAWN], position);
                break;
            case 'n':
                helper.setBit(bitboards[BLACK_KNIGHT], position);
                break;
            case 'b':
                helper.setBit(bitboards[BLACK_BISHOP], position);
                break;
            case 'r':
                helper.setBit(bitboards[BLACK_ROOK], position);
                break;
            case 'q':
                helper.setBit(bitboards[BLACK_QUEEN], position);
                break;
            case 'k':
                helper.setBit(bitboards[BLACK_KING], position);
                break;
            }
            file++;
        }
    }

    // Set bitboard for all pieces
    for (int i = 0; i <= BLACK_KING; i++)
    {
        if (i < 6)
        {
            bitboards[WHITE_PIECES] |= bitboards[i];
        }
        else
        {
            bitboards[BLACK_PIECES] |= bitboards[i];
        }
        bitboards[ALL_PIECES] |= bitboards[i];
    }
}
#include "position.h"
#include <sstream>
#include "enums.h"
using namespace std;

void Position::parseFEN(string fen, Bitboard (&bitboards)[16])
{
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

    STM = turn == "w" ? WHITE : BLACK;

    for(int i=0; i<=63; i++)
    {
        piecesArray[i]=NO_PIECE;
    }

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
                setBit(bitboards[WHITE_PAWN], position);
                piecesArray[position]=WHITE_PAWN;
                break;
            case 'N':
                setBit(bitboards[WHITE_KNIGHT], position);
                piecesArray[position]=WHITE_KNIGHT;
                break;
            case 'B':
                setBit(bitboards[WHITE_BISHOP], position);
                piecesArray[position]=WHITE_BISHOP;
                break;
            case 'R':
                setBit(bitboards[WHITE_ROOK], position);
                piecesArray[position]=WHITE_ROOK;
                break;
            case 'Q':
                setBit(bitboards[WHITE_QUEEN], position);
                piecesArray[position]=WHITE_QUEEN;
                break;
            case 'K':
                setBit(bitboards[WHITE_KING], position);
                piecesArray[position]=WHITE_KING;
                break;
            case 'p':
                setBit(bitboards[BLACK_PAWN], position);
                piecesArray[position]=BLACK_PAWN;
                break;
            case 'n':
                setBit(bitboards[BLACK_KNIGHT], position);
                piecesArray[position]=BLACK_KNIGHT;
                break;
            case 'b':
                setBit(bitboards[BLACK_BISHOP], position);
                piecesArray[position]=BLACK_BISHOP;
                break;
            case 'r':
                setBit(bitboards[BLACK_ROOK], position);
                piecesArray[position]=BLACK_ROOK;
                break;
            case 'q':
                setBit(bitboards[BLACK_QUEEN], position);
                piecesArray[position]=BLACK_QUEEN;
                break;
            case 'k':
                setBit(bitboards[BLACK_KING], position);
                piecesArray[position]=BLACK_KING;
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

void Position::makeMove(Move move)
{
    int startSquare = move & 0b111111;
    int targetSquare = (move >> 6) & 0b111111;
    int flags = (move >> 12) & 0b1111;
    cout<<startSquare<<" "<<targetSquare<<" "<<flags<<endl;
}
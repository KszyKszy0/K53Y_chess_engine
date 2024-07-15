#include "position.h"
#include <sstream>
#include "enums.h"
#include "stateInfo.h"
#include "move.h"

using namespace std;

void Position::parseFEN(string fen, Bitboard (&bitboards)[16])
{
    //reseting bitboards
    for (int i = 0; i < 16; i++)
    {
        bitboards[i] = 0ULL;
    }

    istringstream ss(fen);
    string board, turn, castling, enPassant;
    int halfmove, fullmove;

    ss >> board >> turn >> castling >> enPassant >> halfmove >> fullmove;
    // cout<<board<<endl<<turn<<endl<<castling<<endl<<halfmove<<endl<<fULLlmove<<endl;

    int rank = 7;
    int file = 0;

    STM = turn == "w" ? WHITE : BLACK;

    //clear piece array
    for(int i=0; i<=63; i++)
    {
        piecesArray[i]=NO_PIECE;
    }

    //loop through fen
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

    //set bitboards[empty]
    bitboards[NO_PIECE] = ~bitboards[ALL_PIECES];

    //add game state to list
    addState(nameToSquare(enPassant),castlingRights(castling),halfmove,fullmove,NO_PIECE);
}

void Position::makeMove(Move move)
{
    int startSquare = move & 0b111111;
    int targetSquare = (move >> 6) & 0b111111;
    int flags = (move >> 12) & 0b1111;

    StateInfo tempState = stateInfoList.back();

    tempState.enPassantSquare = 0;
    if(flags == PAWN_DOUBLE_MOVE)
    {
        if(STM)
        {
            tempState.enPassantSquare = targetSquare-8;
        }else
        {
            tempState.enPassantSquare = targetSquare+8;
        }
    }


    if(flags == EN_PASSANT)
    {
        if(STM)
        {
            clearBit(piecesBitboards[BLACK_PIECES],targetSquare-8);
            clearBit(piecesBitboards[BLACK_PAWN],targetSquare-8);
            clearBit(piecesBitboards[ALL_PIECES],targetSquare-8);
            bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[WHITE_PAWN],startSquare,targetSquare);
            piecesArray[targetSquare-8] = NO_PIECE;
        }else
        {
            clearBit(piecesBitboards[WHITE_PIECES],targetSquare+8);
            clearBit(piecesBitboards[WHITE_PAWN],targetSquare+8);
            clearBit(piecesBitboards[ALL_PIECES],targetSquare+8);
            bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[BLACK_PAWN],startSquare,targetSquare);
            piecesArray[targetSquare+8] = NO_PIECE;
        }
        piecesArray[targetSquare] = piecesArray[startSquare];
        piecesArray[startSquare] = NO_PIECE;
    }


    if(flags == CAPTURE)
    {
        tempState.capturedPieceType = piecesArray[targetSquare];

        clearBit(piecesBitboards[piecesArray[targetSquare]],targetSquare);
        bitSwap(piecesBitboards[piecesArray[startSquare]],startSquare,targetSquare);
        clearBit(piecesBitboards[ALL_PIECES],startSquare);

        if(STM == WHITE)
        {
            clearBit(piecesBitboards[BLACK_PIECES],targetSquare);
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);
        }else
        {
            clearBit(piecesBitboards[WHITE_PIECES],targetSquare);
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);
        }
        piecesArray[targetSquare] = piecesArray[startSquare];
        piecesArray[startSquare] = NO_PIECE;
    }



    if(flags == QUIET || flags == PAWN_DOUBLE_MOVE)
    {
        bitSwap(piecesBitboards[piecesArray[startSquare]],startSquare,targetSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

        if(STM == WHITE)
        {
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);
        }else
        {
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);
        }
        piecesArray[targetSquare] = piecesArray[startSquare];
        piecesArray[startSquare] = NO_PIECE;
    }

    if(flags == SHORT_CASTLE)
    {
        bitSwap(piecesBitboards[piecesArray[startSquare]],startSquare,targetSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

        if(STM == WHITE)
        {
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_ROOK],7,5);

            bitSwap(piecesBitboards[ALL_PIECES],7,5);

            bitSwap(piecesBitboards[WHITE_PIECES],7,5);

            piecesArray[5] = WHITE_ROOK;
            piecesArray[7] = NO_PIECE;
        }else
        {
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[BLACK_ROOK],63,61);

            bitSwap(piecesBitboards[ALL_PIECES],63,61);

            bitSwap(piecesBitboards[BLACK_PIECES],63,61);

            piecesArray[63] = NO_PIECE;
            piecesArray[61] = BLACK_ROOK;
        }
        piecesArray[targetSquare] = piecesArray[startSquare];
        piecesArray[startSquare] = NO_PIECE;
    }

    if(flags == LONG_CASTLE)
    {
        bitSwap(piecesBitboards[piecesArray[startSquare]],startSquare,targetSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

        if(STM == WHITE)
        {
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_ROOK],0,3);

            bitSwap(piecesBitboards[ALL_PIECES],0,3);

            bitSwap(piecesBitboards[WHITE_PIECES],0,3);

            piecesArray[3] = WHITE_ROOK;
            piecesArray[0] = NO_PIECE;
        }else
        {
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[BLACK_ROOK],56,59);

            bitSwap(piecesBitboards[ALL_PIECES],56,59);

            bitSwap(piecesBitboards[BLACK_PIECES],56,59);

            piecesArray[56] = NO_PIECE;
            piecesArray[59] = BLACK_ROOK;
        }
        piecesArray[targetSquare] = piecesArray[startSquare];
        piecesArray[startSquare] = NO_PIECE;
    }


    if(flags == KNIGHT_PROMOTION)
    {
        clearBit(piecesBitboards[piecesArray[startSquare]],startSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

        if(STM == WHITE)
        {
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[WHITE_KNIGHT],targetSquare);

            piecesArray[targetSquare] = WHITE_KNIGHT;
        }else
        {
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[BLACK_KNIGHT],targetSquare);

            piecesArray[targetSquare] = BLACK_KNIGHT;
        }

        piecesArray[startSquare] = NO_PIECE;
    }

    if(flags == BISHOP_PROMOTION)
    {
        clearBit(piecesBitboards[piecesArray[startSquare]],startSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

        if(STM == WHITE)
        {
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[WHITE_BISHOP],targetSquare);

            piecesArray[targetSquare] = WHITE_BISHOP;
        }else
        {
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[BLACK_BISHOP],targetSquare);

            piecesArray[targetSquare] = BLACK_BISHOP;
        }

        piecesArray[startSquare] = NO_PIECE;
    }

    if(flags == ROOK_PROMOTION)
    {
        clearBit(piecesBitboards[piecesArray[startSquare]],startSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

        if(STM == WHITE)
        {
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[WHITE_ROOK],targetSquare);

            piecesArray[targetSquare] = WHITE_ROOK;
        }else
        {
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[BLACK_ROOK],targetSquare);

            piecesArray[targetSquare] = BLACK_ROOK;
        }

        piecesArray[startSquare] = NO_PIECE;
    }


    if(flags == QUEEN_PROMOTION)
    {
        clearBit(piecesBitboards[piecesArray[startSquare]],startSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

        if(STM == WHITE)
        {
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[WHITE_QUEEN],targetSquare);

            piecesArray[targetSquare] = WHITE_QUEEN;
        }else
        {
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[BLACK_QUEEN],targetSquare);

            piecesArray[targetSquare] = BLACK_QUEEN;
        }

        piecesArray[startSquare] = NO_PIECE;
    }

    piecesBitboards[NO_PIECE] = ~piecesBitboards[ALL_PIECES];


    tempState.halfMove++;
    if(!STM)
    {
        tempState.fullMove++;
    }

    STM = !STM;
    addState(tempState);
}

void Position::undoMove(Move move)
{
    int startSquare = move & 0b111111;
    int targetSquare = (move >> 6) & 0b111111;
    int flags = (move >> 12) & 0b1111;

    if(flags == QUIET || flags == PAWN_DOUBLE_MOVE)
    {

    bitSwap(piecesBitboards[piecesArray[targetSquare]],startSquare,targetSquare);

    bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

    if(STM == BLACK)
    {
        bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);
    }else
    {
        bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);
    }

    piecesArray[startSquare] = piecesArray[targetSquare];
    piecesArray[targetSquare] = NO_PIECE;
    }



    if(flags == CAPTURE)
    {
        int captureType = stateInfoList.back().capturedPieceType;

        setBit(piecesBitboards[captureType], targetSquare);

        setBit(piecesBitboards[ALL_PIECES], startSquare);

        bitSwap(piecesBitboards[piecesArray[targetSquare]],startSquare,targetSquare);

        if(STM == WHITE)
        {
            setBit(piecesBitboards[WHITE_PIECES],targetSquare);
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);
        }else
        {
            setBit(piecesBitboards[BLACK_PIECES],targetSquare);
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);
        }
        piecesArray[startSquare] = piecesArray[targetSquare];
        piecesArray[targetSquare] = captureType;
    }

    if(flags == EN_PASSANT)
    {
        if(!STM)
        {
            setBit(piecesBitboards[BLACK_PIECES],targetSquare-8);
            setBit(piecesBitboards[BLACK_PAWN],targetSquare-8);
            setBit(piecesBitboards[ALL_PIECES],targetSquare-8);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[WHITE_PAWN],startSquare,targetSquare);
            piecesArray[targetSquare-8] = BLACK_PAWN;
        }else
        {
            setBit(piecesBitboards[WHITE_PIECES],targetSquare+8);
            setBit(piecesBitboards[WHITE_PAWN],targetSquare+8);
            setBit(piecesBitboards[ALL_PIECES],targetSquare+8);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[BLACK_PAWN],startSquare,targetSquare);
            piecesArray[targetSquare+8] = WHITE_PAWN;
        }
        piecesArray[startSquare] = piecesArray[targetSquare];
        piecesArray[targetSquare] = NO_PIECE;
    }

    if(flags == SHORT_CASTLE)
    {
        bitSwap(piecesBitboards[piecesArray[targetSquare]],startSquare,targetSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

        if(STM == BLACK)
        {
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_ROOK],7,5);

            bitSwap(piecesBitboards[ALL_PIECES],7,5);

            bitSwap(piecesBitboards[WHITE_PIECES],7,5);

            piecesArray[5] = NO_PIECE;
            piecesArray[7] = WHITE_ROOK;
        }else
        {
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[BLACK_ROOK],63,61);

            bitSwap(piecesBitboards[ALL_PIECES],63,61);

            bitSwap(piecesBitboards[BLACK_PIECES],63,61);

            piecesArray[63] = BLACK_ROOK;
            piecesArray[61] = NO_PIECE;
        }
        piecesArray[startSquare] = piecesArray[targetSquare];
        piecesArray[targetSquare] = NO_PIECE;
    }

    if(flags == LONG_CASTLE)
    {
        bitSwap(piecesBitboards[piecesArray[targetSquare]],startSquare,targetSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

        if(STM == BLACK)
        {
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_ROOK],0,3);

            bitSwap(piecesBitboards[ALL_PIECES],0,3);

            bitSwap(piecesBitboards[WHITE_PIECES],0,3);

            piecesArray[0] = WHITE_ROOK;
            piecesArray[3] = NO_PIECE;
        }else
        {
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[BLACK_ROOK],56,59);

            bitSwap(piecesBitboards[ALL_PIECES],56,59);

            bitSwap(piecesBitboards[BLACK_PIECES],56,59);

            piecesArray[59] = NO_PIECE;
            piecesArray[56] = BLACK_ROOK;
        }
        piecesArray[startSquare] = piecesArray[targetSquare];
        piecesArray[targetSquare] = NO_PIECE;
    }


    if(flags == KNIGHT_PROMOTION)
    {

        if(STM == BLACK)
        {

            clearBit(piecesBitboards[WHITE_KNIGHT],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[WHITE_PAWN],startSquare);

            piecesArray[startSquare] = WHITE_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }else
        {

            clearBit(piecesBitboards[BLACK_KNIGHT],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[BLACK_PAWN],startSquare);

            piecesArray[startSquare] = BLACK_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }

    }

    if(flags == BISHOP_PROMOTION)
    {

        if(STM == BLACK)
        {

            clearBit(piecesBitboards[WHITE_BISHOP],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[WHITE_PAWN],startSquare);

            piecesArray[startSquare] = WHITE_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }else
        {

            clearBit(piecesBitboards[BLACK_BISHOP],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[BLACK_PAWN],startSquare);

            piecesArray[startSquare] = BLACK_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }

    }

    if(flags == ROOK_PROMOTION)
    {

        if(STM == BLACK)
        {

            clearBit(piecesBitboards[WHITE_ROOK],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[WHITE_PAWN],startSquare);

            piecesArray[startSquare] = WHITE_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }else
        {

            clearBit(piecesBitboards[BLACK_ROOK],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[BLACK_PAWN],startSquare);

            piecesArray[startSquare] = BLACK_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }

    }

    if(flags == QUEEN_PROMOTION)
    {

        if(STM == BLACK)
        {

            clearBit(piecesBitboards[WHITE_QUEEN],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[WHITE_PAWN],startSquare);

            piecesArray[startSquare] = WHITE_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }else
        {

            clearBit(piecesBitboards[BLACK_QUEEN],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[BLACK_PAWN],startSquare);

            piecesArray[startSquare] = BLACK_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }

    }

    piecesBitboards[NO_PIECE] = ~piecesBitboards[ALL_PIECES];

    STM = !STM;
    stateInfoList.pop_back();
}

StateInfo::StateInfo(int pas, int cast, int half, int full, int captureType)
{
    enPassantSquare=pas;
    castlingRights=cast;
    halfMove=half;
    fullMove=full;
    capturedPieceType=captureType;
}
void Position::addState(int pas, int cast, int half, int full, int captureType)
{
    StateInfo newState(pas,cast,half,full,captureType);
    stateInfoList.push_back(newState);
}
void Position::addState(StateInfo state)
{
    StateInfo newState(state.enPassantSquare,state.castlingRights,state.halfMove,state.fullMove,state.capturedPieceType);
    stateInfoList.push_back(newState);
}
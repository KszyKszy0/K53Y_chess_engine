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

    if(flags == 1)
    {
        if(STM)
        {
            tempState.enPassantSquare = targetSquare-8;
        }else
        {
            tempState.enPassantSquare = targetSquare+8;
        }
    }
    if(flags == 4)
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

    }else
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
    }




    piecesArray[targetSquare] = piecesArray[startSquare];
    piecesArray[startSquare] = NO_PIECE;

    piecesBitboards[NO_PIECE] = ~piecesBitboards[ALL_PIECES];

    STM = !STM;
    addState(tempState);
}

void Position::undoMove(Move move)
{
    int startSquare = move & 0b111111;
    int targetSquare = (move >> 6) & 0b111111;
    int flags = (move >> 12) & 0b1111;


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



    if(flags == 4)
    {
        int captureType = stateInfoList.back().capturedPieceType;

        setBit(piecesBitboards[captureType],targetSquare);

        setBit(piecesBitboards[ALL_PIECES],targetSquare);

        if(STM == WHITE)
        {
            setBit(piecesBitboards[WHITE_PIECES],targetSquare);
        }else
        {
            setBit(piecesBitboards[BLACK_PIECES],targetSquare);
        }
        piecesArray[targetSquare] = captureType;
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
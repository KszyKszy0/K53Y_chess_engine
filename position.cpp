#include "position.h"
#include <sstream>
#include "enums.h"
#include "stateInfo.h"
#include "move.h"
#include <string>

using namespace std;

void Position::parseFEN(string fen, Bitboard (&bitboards)[16])
{
    //stalemate checkmate flags off
    isStalemate = false;
    isCheckmate = false;

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

    //reset position hash
    positionHash = 0;

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
                positionHash ^= zobrist.zobristTable[WHITE_PAWN*64+position];
                break;
            case 'N':
                setBit(bitboards[WHITE_KNIGHT], position);
                piecesArray[position]=WHITE_KNIGHT;
                positionHash ^= zobrist.zobristTable[WHITE_KNIGHT*64+position];
                break;
            case 'B':
                setBit(bitboards[WHITE_BISHOP], position);
                piecesArray[position]=WHITE_BISHOP;
                positionHash ^= zobrist.zobristTable[WHITE_BISHOP*64+position];
                break;
            case 'R':
                setBit(bitboards[WHITE_ROOK], position);
                piecesArray[position]=WHITE_ROOK;
                positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+position];
                break;
            case 'Q':
                setBit(bitboards[WHITE_QUEEN], position);
                piecesArray[position]=WHITE_QUEEN;
                positionHash ^= zobrist.zobristTable[WHITE_QUEEN*64+position];
                break;
            case 'K':
                setBit(bitboards[WHITE_KING], position);
                piecesArray[position]=WHITE_KING;
                positionHash ^= zobrist.zobristTable[WHITE_KING*64+position];
                break;
            case 'p':
                setBit(bitboards[BLACK_PAWN], position);
                positionHash ^= zobrist.zobristTable[BLACK_PAWN*64+position];
                piecesArray[position]=BLACK_PAWN;
                break;
            case 'n':
                setBit(bitboards[BLACK_KNIGHT], position);
                positionHash ^= zobrist.zobristTable[BLACK_KNIGHT*64+position];
                piecesArray[position]=BLACK_KNIGHT;
                break;
            case 'b':
                setBit(bitboards[BLACK_BISHOP], position);
                positionHash ^= zobrist.zobristTable[BLACK_BISHOP*64+position];
                piecesArray[position]=BLACK_BISHOP;
                break;
            case 'r':
                setBit(bitboards[BLACK_ROOK], position);
                positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+position];
                piecesArray[position]=BLACK_ROOK;
                break;
            case 'q':
                setBit(bitboards[BLACK_QUEEN], position);
                positionHash ^= zobrist.zobristTable[BLACK_QUEEN*64+position];
                piecesArray[position]=BLACK_QUEEN;
                break;
            case 'k':
                setBit(bitboards[BLACK_KING], position);
                positionHash ^= zobrist.zobristTable[BLACK_KING*64+position];
                piecesArray[position]=BLACK_KING;
                break;
            }
            file++;
        }
    }

    //Setup rest of hash
    positionHash ^= zobrist.zobristTable[768+castlingRights(castling)];

    int enPassantFile = 0;
    if(enPassant[0] != '-')
        enPassantFile = enPassant[0] - 'a';

    positionHash ^= zobrist.zobristTable[784+enPassantFile];

    if(!STM)
        positionHash ^= zobrist.zobristTable[792];

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
    stateCounter = 0;
    if(enPassant[0] == '-')
    {
        addState(0,castlingRights(castling),halfmove,fullmove,NO_PIECE);
        return;
    }
    addState(nameToSquare(enPassant),castlingRights(castling),halfmove,fullmove,NO_PIECE);
}


void Position::makeMove(Move move)
{
    int startSquare = move & 0b111111;
    int targetSquare = (move >> 6) & 0b111111;
    int flags = (move >> 12) & 0b1111;

    StateInfo refernce = stateInfoList[stateCounter];
    StateInfo tempState = StateInfo(refernce.enPassantSquare,refernce.castlingRights,refernce.halfMove,refernce.fullMove,refernce.capturedPieceType);

    tempState.halfMove++;

    if(startSquare == 7 || targetSquare == 7)
    {
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
        tempState.castlingRights &= ~8;
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
    }
    if(startSquare == 0 || targetSquare == 0)
    {
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
        tempState.castlingRights &= ~4;
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
    }
    if(startSquare == 4)
    {
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
        tempState.castlingRights &= ~12;
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
    }

    if(startSquare == 63 || targetSquare == 63)
    {
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
        tempState.castlingRights &= ~2;
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
    }
    if(startSquare == 56 || targetSquare == 56)
    {
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
        tempState.castlingRights &= ~1;
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
    }
    if(startSquare == 60)
    {
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
        tempState.castlingRights &= ~3;
        positionHash ^= zobrist.zobristTable[768 + tempState.castlingRights];
    }

    if(tempState.enPassantSquare != 0)
        positionHash ^= zobrist.zobristTable[784 + (tempState.enPassantSquare % 8)];

    tempState.enPassantSquare = 0;



    if(flags == PAWN_DOUBLE_MOVE)
    {
        tempState.halfMove = 0;
        if(STM)
        {
            tempState.enPassantSquare = targetSquare-8;
            //add enpassant hashes
            positionHash ^= zobrist.zobristTable[784 + (tempState.enPassantSquare % 8)];
        }else
        {
            tempState.enPassantSquare = targetSquare+8;
            //add enpassant hashes
            positionHash ^= zobrist.zobristTable[784 + (tempState.enPassantSquare % 8)];
        }

        bitSwap(piecesBitboards[piecesArray[startSquare]],startSquare,targetSquare);

        //hash update
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+targetSquare];

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


    if(flags == EN_PASSANT)
    {
        tempState.halfMove = 0;

        //hash update
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+targetSquare];


        if(STM)
        {
            clearBit(piecesBitboards[BLACK_PIECES],targetSquare-8);
            clearBit(piecesBitboards[BLACK_PAWN],targetSquare-8);
            clearBit(piecesBitboards[ALL_PIECES],targetSquare-8);
            bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[WHITE_PAWN],startSquare,targetSquare);
            piecesArray[targetSquare-8] = NO_PIECE;

            //udpate capture hash
            positionHash ^= zobrist.zobristTable[BLACK_PAWN*64+targetSquare-8];
        }else
        {
            clearBit(piecesBitboards[WHITE_PIECES],targetSquare+8);
            clearBit(piecesBitboards[WHITE_PAWN],targetSquare+8);
            clearBit(piecesBitboards[ALL_PIECES],targetSquare+8);
            bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[BLACK_PAWN],startSquare,targetSquare);
            piecesArray[targetSquare+8] = NO_PIECE;

            //udpate capture hash
            positionHash ^= zobrist.zobristTable[WHITE_PAWN*64+targetSquare+8];
        }
        piecesArray[targetSquare] = piecesArray[startSquare];
        piecesArray[startSquare] = NO_PIECE;
    }


    if(flags == CAPTURE)
    {
        tempState.halfMove = 0;
        tempState.capturedPieceType = piecesArray[targetSquare];

        //hash update
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+targetSquare];

        //update capture hash
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+targetSquare];

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



    if(flags == QUIET)
    {
        //hash update
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+targetSquare];

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
        //king hash update
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+targetSquare];


        bitSwap(piecesBitboards[piecesArray[startSquare]],startSquare,targetSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

        if(STM == WHITE)
        {
            //rook hash update
            positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+7];
            positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+5];

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_ROOK],7,5);

            bitSwap(piecesBitboards[ALL_PIECES],7,5);

            bitSwap(piecesBitboards[WHITE_PIECES],7,5);

            piecesArray[5] = WHITE_ROOK;
            piecesArray[7] = NO_PIECE;
        }else
        {
            //rook hash update
            positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+63];
            positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+61];


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
        //king hash update
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+targetSquare];

        bitSwap(piecesBitboards[piecesArray[startSquare]],startSquare,targetSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

        if(STM == WHITE)
        {
            //rook hash update
            positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+0];
            positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+3];

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_ROOK],0,3);

            bitSwap(piecesBitboards[ALL_PIECES],0,3);

            bitSwap(piecesBitboards[WHITE_PIECES],0,3);

            piecesArray[3] = WHITE_ROOK;
            piecesArray[0] = NO_PIECE;
        }else
        {
            //rook hash update
            positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+56];
            positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+59];

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

    //simple promotions
    if((flags > 7) && (flags < 12))
    {
        tempState.halfMove = 0;

        int promotionType = Flags(move);


        //pawn hash remove
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];


        clearBit(piecesBitboards[piecesArray[startSquare]],startSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

        if(STM == WHITE)
        {
            promotionType -= 7;

            //udpate hash of made piece
            positionHash ^= zobrist.zobristTable[promotionType*64+targetSquare];

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[promotionType],targetSquare);

            piecesArray[targetSquare] = promotionType;
        }else
        {
            promotionType -= 1;

            //udpate hash of made piece
            positionHash ^= zobrist.zobristTable[promotionType*64+targetSquare];

            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[promotionType],targetSquare);

            piecesArray[targetSquare] = promotionType;
        }

        piecesArray[startSquare] = NO_PIECE;
    }

    //capture promotions
    if(flags > 11)
    {
        tempState.halfMove = 0;
        tempState.capturedPieceType = piecesArray[targetSquare];

        int promotionType = Flags(move);

        //pawn hash remove
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];

        //remove hash of taken piece
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+targetSquare];

        clearBit(piecesBitboards[piecesArray[startSquare]],startSquare);

        clearBit(piecesBitboards[ALL_PIECES], startSquare);

        if(STM == WHITE)
        {
            promotionType -= 11;

            //udpate hash of made piece
            positionHash ^= zobrist.zobristTable[promotionType*64+targetSquare];

            bitSwap(piecesBitboards[WHITE_PIECES], startSquare, targetSquare);

            setBit(piecesBitboards[promotionType],targetSquare);

            clearBit(piecesBitboards[piecesArray[targetSquare]],targetSquare);
            clearBit(piecesBitboards[BLACK_PIECES],targetSquare);

            piecesArray[targetSquare] = promotionType;
        }else
        {
            promotionType -= 5;

            //udpate hash of made piece
            positionHash ^= zobrist.zobristTable[promotionType*64+targetSquare];

            bitSwap(piecesBitboards[BLACK_PIECES], startSquare, targetSquare);

            setBit(piecesBitboards[promotionType],targetSquare);

            clearBit(piecesBitboards[piecesArray[targetSquare]],targetSquare);
            clearBit(piecesBitboards[WHITE_PIECES],targetSquare);

            piecesArray[targetSquare] = promotionType;
        }

        piecesArray[startSquare] = NO_PIECE;
    }

    piecesBitboards[NO_PIECE] = ~piecesBitboards[ALL_PIECES];

    if(piecesArray[targetSquare] == WHITE_PAWN || piecesArray[targetSquare] == BLACK_PAWN)
    {
        tempState.halfMove = 0;
    }
    if(!STM)
    {
        tempState.fullMove++;
    }

    STM = !STM;

    stateCounter++;

    //change move side
    positionHash ^= zobrist.zobristTable[792];

    addState(tempState);
}

void Position::undoMove(Move move)
{
    int startSquare = move & 0b111111;
    int targetSquare = (move >> 6) & 0b111111;
    int flags = (move >> 12) & 0b1111;

    if(flags == QUIET || flags == PAWN_DOUBLE_MOVE)
    {
        //hash update
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+targetSquare];

        if(flags == PAWN_DOUBLE_MOVE)
        {
            positionHash ^= zobrist.zobristTable[784+(stateInfoList[stateCounter].enPassantSquare % 8)];
        }


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
        int captureType = stateInfoList[stateCounter].capturedPieceType;

        //hash update
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+targetSquare];

        //take piece hash update
        positionHash ^= zobrist.zobristTable[captureType*64+targetSquare];

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
        //hash update
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+targetSquare];

        if(!STM)
        {
            //take piece hash update
            positionHash ^= zobrist.zobristTable[BLACK_PAWN*64+targetSquare-8];


            setBit(piecesBitboards[BLACK_PIECES],targetSquare-8);
            setBit(piecesBitboards[BLACK_PAWN],targetSquare-8);
            setBit(piecesBitboards[ALL_PIECES],targetSquare-8);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);
            bitSwap(piecesBitboards[WHITE_PAWN],startSquare,targetSquare);
            piecesArray[targetSquare-8] = BLACK_PAWN;
        }else
        {
            //take piece hash update
            positionHash ^= zobrist.zobristTable[WHITE_PAWN*64+targetSquare+8];


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
        //king hash update
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+targetSquare];

        bitSwap(piecesBitboards[piecesArray[targetSquare]],startSquare,targetSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

        if(STM == BLACK)
        {
            //rook hash update
            positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+7];
            positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+5];

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_ROOK],7,5);

            bitSwap(piecesBitboards[ALL_PIECES],7,5);

            bitSwap(piecesBitboards[WHITE_PIECES],7,5);

            piecesArray[5] = NO_PIECE;
            piecesArray[7] = WHITE_ROOK;
        }else
        {
            //rook hash update
            positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+63];
            positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+61];

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
        //king hash update
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+targetSquare];

        bitSwap(piecesBitboards[piecesArray[targetSquare]],startSquare,targetSquare);

        bitSwap(piecesBitboards[ALL_PIECES],startSquare,targetSquare);

        if(STM == BLACK)
        {
            //rook hash update
            positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+0];
            positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+3];

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            bitSwap(piecesBitboards[WHITE_ROOK],0,3);

            bitSwap(piecesBitboards[ALL_PIECES],0,3);

            bitSwap(piecesBitboards[WHITE_PIECES],0,3);

            piecesArray[0] = WHITE_ROOK;
            piecesArray[3] = NO_PIECE;
        }else
        {
            //rook hash update
            positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+56];
            positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+59];

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

    //simple promotion
    if((flags > 7) && (flags < 12))
    {
        int promotionType = Flags(move);


        if(STM == BLACK)
        {
            //hash update for pawn
            positionHash ^= zobrist.zobristTable[WHITE_PAWN*64+startSquare];

            promotionType -= 7;

            //hash update for promotion
            positionHash ^= zobrist.zobristTable[promotionType*64+targetSquare];

            clearBit(piecesBitboards[promotionType],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[WHITE_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[WHITE_PAWN],startSquare);

            piecesArray[startSquare] = WHITE_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }else
        {
            //hash update for pawn
            positionHash ^= zobrist.zobristTable[BLACK_PAWN*64+startSquare];

            promotionType -= 1;

            //hash update for promotion
            positionHash ^= zobrist.zobristTable[promotionType*64+targetSquare];

            clearBit(piecesBitboards[promotionType],targetSquare);

            bitSwap(piecesBitboards[ALL_PIECES],startSquare, targetSquare);

            bitSwap(piecesBitboards[BLACK_PIECES],startSquare,targetSquare);

            setBit(piecesBitboards[BLACK_PAWN],startSquare);

            piecesArray[startSquare] = BLACK_PAWN;
            piecesArray[targetSquare] = NO_PIECE;

        }

    }

    //promotions with captures
    if(flags > 11)
    {
        int captureType = stateInfoList[stateCounter].capturedPieceType;

        //hash update for capture
        positionHash ^= zobrist.zobristTable[captureType*64+targetSquare];

        int promotionType = Flags(move);

        setBit(piecesBitboards[captureType], targetSquare);

        setBit(piecesBitboards[ALL_PIECES], startSquare);

        if(STM == BLACK)
        {
            //hash update for pawn
            positionHash ^= zobrist.zobristTable[WHITE_PAWN*64+startSquare];

            promotionType -= 11;

            //hash update for promotion
            positionHash ^= zobrist.zobristTable[promotionType*64+targetSquare];

            clearBit(piecesBitboards[promotionType],targetSquare);

            setBit(piecesBitboards[WHITE_PAWN],startSquare);

            bitSwap(piecesBitboards[WHITE_PIECES], startSquare, targetSquare);



            setBit(piecesBitboards[BLACK_PIECES], targetSquare);

            piecesArray[targetSquare] = captureType;
            piecesArray[startSquare] = WHITE_PAWN;
        }else
        {
            //hash update for pawn
            positionHash ^= zobrist.zobristTable[BLACK_PAWN*64+startSquare];

            promotionType -= 5;

            //hash update for promotion
            positionHash ^= zobrist.zobristTable[promotionType*64+targetSquare];

            clearBit(piecesBitboards[promotionType],targetSquare);

            setBit(piecesBitboards[BLACK_PAWN],startSquare);

            bitSwap(piecesBitboards[BLACK_PIECES], startSquare, targetSquare);



            setBit(piecesBitboards[WHITE_PIECES], targetSquare);

            piecesArray[targetSquare] = captureType;
            piecesArray[startSquare] = BLACK_PAWN;
        }
    }

    //undo is called with min 2 states - one generated when parse fen and second when make move so its rather valid
    //castling update hash
    if(stateInfoList[stateCounter].castlingRights != stateInfoList[stateCounter-1].castlingRights)
    {
        //remove new castling hash
        positionHash ^= zobrist.zobristTable[768+stateInfoList[stateCounter].castlingRights];
        //set old castling hash
        positionHash ^= zobrist.zobristTable[768+stateInfoList[stateCounter-1].castlingRights];
    }

    //adding old enpassant possibility
    if(stateInfoList[stateCounter-1].enPassantSquare != 0)
        positionHash ^= zobrist.zobristTable[784+(stateInfoList[stateCounter-1].enPassantSquare % 8)];

    piecesBitboards[NO_PIECE] = ~piecesBitboards[ALL_PIECES];

    isCheckmate = false;
    isStalemate = false;

    STM = !STM;
    //change move side
    positionHash ^= zobrist.zobristTable[792];

    stateCounter--;
}


void Position::addState(int pas, int cast, int half, int full, int captureType)
{
    StateInfo newState(pas,cast,half,full,captureType);
    stateInfoList[stateCounter] = newState;
}
void Position::addState(StateInfo state)
{
    StateInfo newState(state.enPassantSquare,state.castlingRights,state.halfMove,state.fullMove,state.capturedPieceType);
    stateInfoList[stateCounter] = newState;
}
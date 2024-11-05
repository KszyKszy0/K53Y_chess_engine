#include "position.h"
#include <sstream>
#include "enums.h"
#include "stateInfo.h"
#include "move.h"
#include <string>


using namespace std;

void Position::parseFEN(string fen)
{

    //reseting bitboards
    for (int i = 0; i < 16; i++)
    {
        piecesBitboards[i] = 0ULL;
    }

    istringstream ss(fen);
    string board, turn, castling, enPassant;
    int halfmove, fullmove;

    ss >> board >> turn >> castling >> enPassant >> halfmove >> fullmove;

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
                setBit(piecesBitboards[WHITE_PAWN], position);
                piecesArray[position]=WHITE_PAWN;
                positionHash ^= zobrist.zobristTable[WHITE_PAWN*64+position];
                break;
            case 'N':
                setBit(piecesBitboards[WHITE_KNIGHT], position);
                piecesArray[position]=WHITE_KNIGHT;
                positionHash ^= zobrist.zobristTable[WHITE_KNIGHT*64+position];
                break;
            case 'B':
                setBit(piecesBitboards[WHITE_BISHOP], position);
                piecesArray[position]=WHITE_BISHOP;
                positionHash ^= zobrist.zobristTable[WHITE_BISHOP*64+position];
                break;
            case 'R':
                setBit(piecesBitboards[WHITE_ROOK], position);
                piecesArray[position]=WHITE_ROOK;
                positionHash ^= zobrist.zobristTable[WHITE_ROOK*64+position];
                break;
            case 'Q':
                setBit(piecesBitboards[WHITE_QUEEN], position);
                piecesArray[position]=WHITE_QUEEN;
                positionHash ^= zobrist.zobristTable[WHITE_QUEEN*64+position];
                break;
            case 'K':
                setBit(piecesBitboards[WHITE_KING], position);
                piecesArray[position]=WHITE_KING;
                positionHash ^= zobrist.zobristTable[WHITE_KING*64+position];
                break;
            case 'p':
                setBit(piecesBitboards[BLACK_PAWN], position);
                positionHash ^= zobrist.zobristTable[BLACK_PAWN*64+position];
                piecesArray[position]=BLACK_PAWN;
                break;
            case 'n':
                setBit(piecesBitboards[BLACK_KNIGHT], position);
                positionHash ^= zobrist.zobristTable[BLACK_KNIGHT*64+position];
                piecesArray[position]=BLACK_KNIGHT;
                break;
            case 'b':
                setBit(piecesBitboards[BLACK_BISHOP], position);
                positionHash ^= zobrist.zobristTable[BLACK_BISHOP*64+position];
                piecesArray[position]=BLACK_BISHOP;
                break;
            case 'r':
                setBit(piecesBitboards[BLACK_ROOK], position);
                positionHash ^= zobrist.zobristTable[BLACK_ROOK*64+position];
                piecesArray[position]=BLACK_ROOK;
                break;
            case 'q':
                setBit(piecesBitboards[BLACK_QUEEN], position);
                positionHash ^= zobrist.zobristTable[BLACK_QUEEN*64+position];
                piecesArray[position]=BLACK_QUEEN;
                break;
            case 'k':
                setBit(piecesBitboards[BLACK_KING], position);
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
            piecesBitboards[WHITE_PIECES] |= piecesBitboards[i];
        }
        else
        {
            piecesBitboards[BLACK_PIECES] |= piecesBitboards[i];
        }
        piecesBitboards[ALL_PIECES] |= piecesBitboards[i];
    }

    //set bitboards[empty]
    piecesBitboards[NO_PIECE] = ~piecesBitboards[ALL_PIECES];


    //add game state to list
    stateCounter = 0;
    positionHistory[stateCounter] = positionHash;

    accum.initAccum(piecesArray);
    if(enPassant[0] == '-')
    {
        addState(0,castlingRights(castling),halfmove,fullmove,NO_PIECE,accum);
        return;
    }
    addState(nameToSquare(enPassant),castlingRights(castling),halfmove,fullmove,NO_PIECE,accum);
}


std::string Position::getFEN() {
    std::string fen = "";

    // Encode the board position
    for (int rank = 7; rank >= 0; rank--) {
        int emptyCount = 0;

        for (int file = 0; file < 8; file++)
        {
            int position = rank * 8 + file;
            int piece = piecesArray[position];

            if (piece == NO_PIECE) {
                emptyCount++;
            }
            else
            {
                if (emptyCount > 0)
                {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }

                switch (piece)
                {
                    case WHITE_PAWN:   fen += 'P'; break;
                    case WHITE_KNIGHT: fen += 'N'; break;
                    case WHITE_BISHOP: fen += 'B'; break;
                    case WHITE_ROOK:   fen += 'R'; break;
                    case WHITE_QUEEN:  fen += 'Q'; break;
                    case WHITE_KING:   fen += 'K'; break;
                    case BLACK_PAWN:   fen += 'p'; break;
                    case BLACK_KNIGHT: fen += 'n'; break;
                    case BLACK_BISHOP: fen += 'b'; break;
                    case BLACK_ROOK:   fen += 'r'; break;
                    case BLACK_QUEEN:  fen += 'q'; break;
                    case BLACK_KING:   fen += 'k'; break;
                }
            }
        }

        if (emptyCount > 0) {
            fen += std::to_string(emptyCount);
        }

        if (rank > 0) {
            fen += '/';
        }
    }

    // Encode the active color (side to move)
    fen += (STM == WHITE) ? " w " : " b ";

    // Encode castling rights
    // std::string castlingRights = "";

    // if (piecesBitboards[WHITE_KING] & piecesBitboards[WHITE_ROOK] & 0x0000000000000081ULL) {
    //     castlingRights += 'K';
    // }
    // if (piecesBitboards[WHITE_KING] & piecesBitboards[WHITE_ROOK] & 0x0000000000000010ULL) {
    //     castlingRights += 'Q';
    // }
    // if (piecesBitboards[BLACK_KING] & piecesBitboards[BLACK_ROOK] & 0x8100000000000000ULL) {
    //     castlingRights += 'k';
    // }
    // if (piecesBitboards[BLACK_KING] & piecesBitboards[BLACK_ROOK] & 0x1000000000000000ULL) {
    //     castlingRights += 'q';
    // }

    // fen += (castlingRights.empty()) ? "-" : castlingRights;
    std::string rights = castlingRightsText(stateInfoList[stateCounter].castlingRights);

    if(rights != "")
    {
        fen += rights;
    } else
    {
        fen += "- ";
    }
    fen += " ";

    // Encode en passant target square
    if(stateInfoList[stateCounter].enPassantSquare != 0)
    {
        fen += squareToName(stateInfoList[stateCounter].enPassantSquare);
        fen += " ";
    }else
    {
        fen += "- ";
    }

    // Encode halfmove clock
    fen += std::to_string(stateInfoList[stateCounter].halfMove) + " ";

    // Encode fullmove number
    fen += std::to_string(stateInfoList[stateCounter].fullMove);

    return fen;
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
    }


    if(flags == QUIET || flags == PAWN_DOUBLE_MOVE || flags == CAPTURE)
    {

        //hash update
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+targetSquare];

        if(flags == CAPTURE)
        {
            tempState.halfMove = 0;

            accum.removePiece(piecesArray[targetSquare], targetSquare);

            //update capture hash
            positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+targetSquare];
        }

        clearBit(piecesBitboards[piecesArray[targetSquare]],targetSquare);
        clearBit(piecesBitboards[13 - !STM],targetSquare);

        bitSwap(piecesBitboards[piecesArray[startSquare]],startSquare,targetSquare);
        bitSwap(piecesBitboards[12 + !STM],startSquare,targetSquare);

        accum.removePiece(piecesArray[startSquare], startSquare);
        accum.addPiece(piecesArray[startSquare], targetSquare);

        //First we take the piece
        tempState.capturedPieceType = piecesArray[targetSquare];
        //Then we update it
        piecesArray[targetSquare] = piecesArray[startSquare];
        piecesArray[startSquare] = NO_PIECE;

    }


    if(flags == EN_PASSANT)
    {
        tempState.halfMove = 0;


        int direction = 16 * STM - 8; // -8 for black, +8 for white
        int enemyPawn = STM * 6;     // 0 for white pawn, 6 for black pawn
        int ourPawns = (1 - STM) * 6;  // 6 for black pawn, 0 for white pawn
        int enemiesPieces = 12 + STM;  // 13 for white pieces, 12 for black pieces
        int ourPieces = 13 - STM;  // 13 for black pieces, 12 for white pieces


        //hash update
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+targetSquare];
        // Update the capture hash (zobrist hashing)
        positionHash ^= zobrist.zobristTable[(enemyPawn * 64) + (targetSquare - direction)];


        // Clear opponent's pawn (white or black)
        clearBit(piecesBitboards[enemiesPieces], targetSquare - direction);
        clearBit(piecesBitboards[enemyPawn], targetSquare - direction);

        // Move the pieces
        bitSwap(piecesBitboards[ourPieces], startSquare, targetSquare);
        bitSwap(piecesBitboards[ourPawns], startSquare, targetSquare);


        accum.removePiece(piecesArray[startSquare], startSquare);
        accum.addPiece(piecesArray[startSquare], targetSquare);
        accum.removePiece(enemyPawn, targetSquare - direction);

        // Update the pieces array for captured piece
        piecesArray[targetSquare - direction] = NO_PIECE;
        piecesArray[targetSquare] = piecesArray[startSquare];
        piecesArray[startSquare] = NO_PIECE;
    }

    if(flags == SHORT_CASTLE || flags == LONG_CASTLE)
    {
        int rookStartSquare = 0;
        int rookTargetSquare = 0;
        switch (targetSquare)
        {
        case c1:
            rookStartSquare = a1;
            rookTargetSquare = d1;
            break;
        case g1:
            rookStartSquare = h1;
            rookTargetSquare = f1;
            break;
        case c8:
            rookStartSquare = a8;
            rookTargetSquare = d8;
            break;
        case g8:
            rookStartSquare = h8;
            rookTargetSquare = f8;
            break;

        default:
            break;
        }

        int playerOffset = STM;  // 1 for white, 0 for black
        int rookOffset = 9 - (playerOffset * 6);  // 9 for black rook, 3 for white rook
        int piecesOffset = 13 - playerOffset;     // 13 for black pieces, 12 for white pieces


        //king hash update
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+targetSquare];
        // Rook hash update
        positionHash ^= zobrist.zobristTable[rookOffset * 64 + rookStartSquare];
        positionHash ^= zobrist.zobristTable[rookOffset * 64 + rookTargetSquare];

        // Swap pieces on the bitboards
        bitSwap(piecesBitboards[piecesArray[startSquare]],startSquare,targetSquare);
        bitSwap(piecesBitboards[piecesOffset], startSquare, targetSquare);     // White/Black pieces swap
        bitSwap(piecesBitboards[rookOffset], rookStartSquare, rookTargetSquare); // White/Black rook swap
        bitSwap(piecesBitboards[piecesOffset], rookStartSquare, rookTargetSquare); // White/Black pieces swap

        accum.removePiece(piecesArray[startSquare], startSquare);
        accum.addPiece(piecesArray[startSquare], targetSquare);
        accum.removePiece(rookOffset, rookStartSquare);
        accum.addPiece(rookOffset, rookTargetSquare);


        // Update the pieces array
        piecesArray[rookStartSquare] = NO_PIECE; // Remove rook from its starting position
        piecesArray[rookTargetSquare] = rookOffset; // Place the rook at its target position
        piecesArray[targetSquare] = piecesArray[startSquare];
        piecesArray[startSquare] = NO_PIECE;
    }

    if(flags > 7)
    {
        tempState.halfMove = 0;

        int promotionType = flags;
        promotionType -= (flags > 11) * (11 * STM + 5 * (1 - STM)) + (flags <= 11) * (7 * STM + 1 * (1 - STM));

        //pawn hash remove
        positionHash ^= zobrist.zobristTable[piecesArray[startSquare]*64+startSquare];
        //udpate hash of made piece
        positionHash ^= zobrist.zobristTable[promotionType*64+targetSquare];

        if(flags > 11)
        {
            //remove hash of taken piece
            positionHash ^= zobrist.zobristTable[piecesArray[targetSquare]*64+targetSquare];

            accum.removePiece(piecesArray[targetSquare], targetSquare);
        }

        //Clear pawn from pawns
        clearBit(piecesBitboards[piecesArray[startSquare]],startSquare);
        //Clear possible enemy from enemies
        clearBit(piecesBitboards[piecesArray[targetSquare]],targetSquare);
        clearBit(piecesBitboards[13 - !STM],targetSquare);
        //Move the pawn
        bitSwap(piecesBitboards[12 + !STM],startSquare,targetSquare);
        //Update it to piece
        setBit(piecesBitboards[promotionType],targetSquare);


        accum.removePiece(piecesArray[startSquare], startSquare);
        accum.addPiece(promotionType, targetSquare);

        tempState.capturedPieceType = piecesArray[targetSquare];
        piecesArray[startSquare] = NO_PIECE;
        piecesArray[targetSquare] = promotionType;
    }

    piecesBitboards[ALL_PIECES] = piecesBitboards[WHITE_PIECES] | piecesBitboards[BLACK_PIECES];
    piecesBitboards[NO_PIECE] = ~piecesBitboards[ALL_PIECES];

    //If pawn move reset halfmove
    if(piecesArray[targetSquare] == WHITE_PAWN || piecesArray[targetSquare] == BLACK_PAWN)
    {
        tempState.halfMove = 0;
    }
    //After black move update fullmove
    if(!STM)
    {
        tempState.fullMove++;
    }

    //Change side to move
    STM = !STM;

    //Change move side
    positionHash ^= zobrist.zobristTable[792];

    //Number of states + starting one
    stateCounter++;
    positionHistory[stateCounter]=positionHash;

    for(int i=0; i<=1; i++)
    {
        for(int j=0; j <= 15; j++)
        {
            tempState.accumulator[i][j] = accum.values[i][j];
        }
    }
    addState(tempState);
}

void Position::undoMove(Move move)
{
    int startSquare = move & 0b111111;
    int targetSquare = (move >> 6) & 0b111111;
    int flags = (move >> 12) & 0b1111;

    STM = !STM;
    int ourPieces = 13 - STM; // 12 for white, 13 for black
    int enemyPieces = 12 + STM; // 13 for white, 12 for black
    int direction = 16 * STM - 8; // -8 for black, +8 for white
    int enemyPawn = STM * 6;     // 0 for white pawn, 6 for black pawn
    int ourPawns = (1 - STM) * 6;  // 6 for black pawn, 0 for white pawn
    int rookOffset = 9 - (STM * 6);  // 9 for black rook, 3 for white rook

    if(flags == QUIET || flags == PAWN_DOUBLE_MOVE || flags == CAPTURE)
    {
        int captureType = stateInfoList[stateCounter].capturedPieceType;

        if(flags == CAPTURE)
        {
            setBit(piecesBitboards[captureType],targetSquare);
            setBit(piecesBitboards[enemyPieces],targetSquare);
        }

        bitSwap(piecesBitboards[piecesArray[targetSquare]],startSquare,targetSquare);
        bitSwap(piecesBitboards[ourPieces],startSquare,targetSquare);

        piecesArray[startSquare] = piecesArray[targetSquare];
        piecesArray[targetSquare] = captureType;
    }

    if(flags == EN_PASSANT)
    {
        // Clear opponent's pawn (white or black)
        setBit(piecesBitboards[enemyPieces], targetSquare - direction);
        setBit(piecesBitboards[enemyPawn], targetSquare - direction);

        // Move the pieces
        bitSwap(piecesBitboards[ourPieces], startSquare, targetSquare);
        bitSwap(piecesBitboards[ourPawns], startSquare, targetSquare);

        // Update the pieces array for captured piece
        piecesArray[targetSquare - direction] = enemyPawn;
        piecesArray[startSquare] = piecesArray[targetSquare];
        piecesArray[targetSquare] = NO_PIECE;
    }

    if(flags == SHORT_CASTLE || flags == LONG_CASTLE)
    {
        int rookStartSquare = 0;
        int rookTargetSquare = 0;
        switch (targetSquare)
        {
        case c1:
            rookStartSquare = a1;
            rookTargetSquare = d1;
            break;
        case g1:
            rookStartSquare = h1;
            rookTargetSquare = f1;
            break;
        case c8:
            rookStartSquare = a8;
            rookTargetSquare = d8;
            break;
        case g8:
            rookStartSquare = h8;
            rookTargetSquare = f8;
            break;

        default:
            break;
        }


        // Swap pieces on the bitboards
        bitSwap(piecesBitboards[piecesArray[targetSquare]],startSquare,targetSquare);
        bitSwap(piecesBitboards[ourPieces], startSquare, targetSquare);     // White/Black pieces swap
        bitSwap(piecesBitboards[rookOffset], rookStartSquare, rookTargetSquare); // White/Black rook swap
        bitSwap(piecesBitboards[ourPieces], rookStartSquare, rookTargetSquare); // White/Black pieces swap

        // Update the pieces array
        piecesArray[rookStartSquare] = rookOffset; // Remove rook from its starting position
        piecesArray[rookTargetSquare] = NO_PIECE; // Place the rook at its target position
        piecesArray[startSquare] = piecesArray[targetSquare];
        piecesArray[targetSquare] = NO_PIECE;
    }

    if(flags > 7)
    {
        int promotionType = flags;
        promotionType -= (flags > 11) * (11 * STM + 5 * (1 - STM)) + (flags <= 11) * (7 * STM + 1 * (1 - STM));
        int captureType = stateInfoList[stateCounter].capturedPieceType;

        if(flags > 11)
        {
            //Place the captured piece
            setBit(piecesBitboards[captureType],targetSquare);

            //Place the piece on the enemies
            setBit(piecesBitboards[enemyPieces],targetSquare);
        }

        //Set pawn in pawns
        setBit(piecesBitboards[ourPawns],startSquare);
        //Move the pawn
        bitSwap(piecesBitboards[ourPieces],startSquare,targetSquare);
        //Update it to piece
        clearBit(piecesBitboards[promotionType],targetSquare);

        piecesArray[startSquare] = ourPawns;
        piecesArray[targetSquare] = captureType;
    }

    piecesBitboards[ALL_PIECES] = piecesBitboards[WHITE_PIECES] | piecesBitboards[BLACK_PIECES];
    piecesBitboards[NO_PIECE] = ~piecesBitboards[ALL_PIECES];


    stateCounter--;
    positionHash = positionHistory[stateCounter];
#ifdef NNUE
    for(int i=0; i<=1; i++)
    {
        for(int j=0; j <= 15; j++)
        {
            accum.values[i][j] = stateInfoList[stateCounter].accumulator[i][j];
        }
    }
#endif
}


void Position::addState(int pas, int cast, int half, int full, int captureType, Accumulator& acc)
{
    StateInfo newState(pas,cast,half,full,captureType);
#ifdef NNUE
    for(int i=0; i<=1; i++)
    {
        for(int j=0; j <= 15; j++)
        {
            newState.accumulator[i][j] = acc.values[i][j];
        }
    }
#endif


    stateInfoList[stateCounter] = newState;
}
void Position::addState(StateInfo state)
{
    StateInfo newState(state.enPassantSquare,state.castlingRights,state.halfMove,state.fullMove,state.capturedPieceType);
#ifdef NNUE
    for(int i=0; i<=1; i++)
    {
        for(int j=0; j <= 15; j++)
        {
            newState.accumulator[i][j] = state.accumulator[i][j];
        }
    }
#endif
    stateInfoList[stateCounter] = newState;
}
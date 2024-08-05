#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"
#include <vector>



void MoveGenerator::generatePawnMoves(Position& pos, Bitboard target, MoveList& moveList, int type, Bitboard pins)
{
    if(pos.STM == WHITE)
    {
    //pawn moves
    Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);       //Get pawn index
        if(!isBitSet(pins,index))           //Pin moves are generated in getPinners
        {
        if(type == 0)                       //If only quiets
        {
            addPawnWhiteQuiet(index, moveList, pos,target);
        }else
        {
            addPawnWhiteCaptures(index, moveList, pos,target);
        }
        }
    }
    }else
    {
    //pawn moves
    Bitboard iterated = pos.piecesBitboards[BLACK_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);       //Get pawn index
        if(!isBitSet(pins,index))           //Pin moves are generated in getPinners
        {
        if(type == 0)                       //If only quiets
        {
            addPawnBlackQuiet(index, moveList, pos,target);
        }else
        {
            addPawnBlackCaptures(index, moveList, pos,target);
        }
        }
    }
    }
}


void MoveGenerator::generateTypeMoves(Position& pos, Bitboard target, MoveList& moveList, Bitboard whatCanMove)
{
    while(whatCanMove)
    {
        int index = popLSB(whatCanMove);

        int pieceType = pos.piecesArray[index];

        if((pieceType == WHITE_KNIGHT) || (pieceType == BLACK_KNIGHT))
            addKnightsMoves(index, moveList, pos,target);

        if((pieceType == WHITE_BISHOP) || (pieceType == BLACK_BISHOP))
            addBishopsMoves(index, moveList, pos,target);

        if((pieceType == WHITE_ROOK) || (pieceType == BLACK_ROOK))
            addRookMoves(index, moveList, pos,target);

        if((pieceType == WHITE_QUEEN) || (pieceType == BLACK_QUEEN))
        {
            addBishopsMoves(index, moveList, pos,target);
            addRookMoves(index, moveList, pos,target);
        }
    }
}

//True white attacks on square, False black attacks on square POINTS on bb
Bitboard MoveGenerator::howManyAttacks(Position& pos, bool white, int index)
{
    Bitboard attacks = 0;
    //combine all pieces with its attacks from lookup tables for all pieces that attack square
    //"superpiece" at selected index used for check detection
    if(!white)
    {
        attacks |= BBManager.whitePawnCaptures[index] & pos.piecesBitboards[BLACK_PAWN];
        attacks |= BBManager.knightMoves[index] & pos.piecesBitboards[BLACK_KNIGHT];
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & (pos.piecesBitboards[BLACK_BISHOP] | pos.piecesBitboards[BLACK_QUEEN]);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & (pos.piecesBitboards[BLACK_ROOK] | pos.piecesBitboards[BLACK_QUEEN]);
        attacks |= BBManager.kingMoves[index] & pos.piecesBitboards[BLACK_KING];
    }else
    {
        attacks |= BBManager.blackPawnCaptures[index] & pos.piecesBitboards[WHITE_PAWN];
        attacks |= BBManager.knightMoves[index] & pos.piecesBitboards[WHITE_KNIGHT];
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & (pos.piecesBitboards[WHITE_BISHOP] | pos.piecesBitboards[WHITE_QUEEN]);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & (pos.piecesBitboards[WHITE_ROOK] | pos.piecesBitboards[WHITE_QUEEN]);
        attacks |= BBManager.kingMoves[index] & pos.piecesBitboards[WHITE_KING];
    }
    return attacks;
}

void MoveGenerator::fullMovesList(Position& pos, MoveList& moveList)
{
    //moveList that will be used for all substeps
    //preparations

    //index of king
    int kingIndex = LSB(pos.piecesBitboards[pos.STM ? WHITE_KING : BLACK_KING]);

    //bitboard of all pieces that attack square
    Bitboard checksAttacks = howManyAttacks(pos,!pos.STM,kingIndex);

    //Amount of checks; >2 skips most of generation
    int checks = popCount(checksAttacks);

    //rect lookup without the checking piece
    Bitboard checkTargets = 0;

    //index of checking piece
    int singleCheckIndex = 100;

    if(checks == 1)
    {
        //get index of checking piece
        singleCheckIndex = LSB(checksAttacks);
        //update check rect lookup
        checkTargets |= BBManager.rectangularLookup[kingIndex][singleCheckIndex];
        // setBit(checkTargets,index);
    }

    // if(checksAttacks == 0)
    // {
    //     checkTargets = pos.piecesBitboards[NO_PIECE]; //maybe dangerous because of pawn check ->empty rectangular lookup
    // }

    //this should be used for king only!!!! attacks on board without king

    Bitboard enemyAttacks = getSideAttacks(pos, !pos.STM, true);      //Get enemy attacks

    //kings Moves
    generateKingsMoves(pos, moveList, ~enemyAttacks, pos.STM, checks);

    //Check targets is for quiets
    //Capture targets is for captures
    if(checks < 2)
    {
        //pin generation for normal pieces (quiets and captures) and normal pins for pawns (quiet and captures) (without enpassant)
        Bitboard pins = getPinners(pos, pos.STM, moveList, checksAttacks == 0 ? MAX : checkTargets, checks);


        //if check it is bb with checking piece ELSE its full bitboard of enemies so it doesnt mess in pins only for one checking piece tho
        Bitboard captureTargets = singleCheckIndex == 100 ? MAX : 1ULL << singleCheckIndex;

        //if there is no check we set it to enemies pieces for captures
        captureTargets &= pos.STM ? pos.piecesBitboards[BLACK_PIECES] : pos.piecesBitboards[WHITE_PIECES];

        int enPassantSquare = pos.stateInfoList[pos.stateCounter].enPassantSquare;

        //only check en passant if its possible
        if(enPassantSquare != 0)
        {
            Bitboard enpassantCheckCapture = checkTargets;
            if((captureTargets & 1ULL<<singleCheckIndex) & (pos.piecesBitboards[WHITE_PAWN] | pos.piecesBitboards[BLACK_PAWN]))
            {
                setBit(enpassantCheckCapture,enPassantSquare);
            }
            if(pos.STM)
            {
                //enpassant moves and enpassant pins check targets dont have checking piece bcoz enpassant will never capture it
                addPawnWhiteEnpassant(moveList, pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : enpassantCheckCapture, enPassantSquare);
            }else
            {
                //enpassant moves and enpassant pins check targets dont have checking piece bcoz enpassant will never capture it
                addPawnBlackEnpassant(moveList, pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : enpassantCheckCapture, enPassantSquare);
            }
        }

        if(pos.STM == WHITE)
        {
            //quiets
            // generateTypeMoves(pos,checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checkTargets,moveList,pos.piecesBitboards[WHITE_PIECES] &~ pins);
            generatePawnMoves(pos,checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checkTargets,moveList,0,pins);

            generateTypeMoves(pos,checksAttacks == 0 ? (MAX &~ pos.piecesBitboards[WHITE_PIECES]) : (captureTargets | checkTargets), moveList, pos.piecesBitboards[WHITE_PIECES] &~ pins);

            //captures
            // generateTypeMoves(pos,captureTargets,moveList,pos.piecesBitboards[WHITE_PIECES] &~ pins);
            generatePawnMoves(pos,captureTargets,moveList,1,pins);
        }else
        {
            //quiets
            // generateTypeMoves(pos,checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checkTargets,moveList,pos.piecesBitboards[BLACK_PIECES] &~ pins);
            generatePawnMoves(pos,checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checkTargets,moveList,0,pins);

            generateTypeMoves(pos,checksAttacks == 0 ? (MAX &~ pos.piecesBitboards[BLACK_PIECES]) : (captureTargets | checkTargets), moveList, pos.piecesBitboards[BLACK_PIECES] &~ pins);

            //captures
            // generateTypeMoves(pos,captureTargets,moveList,pos.piecesBitboards[BLACK_PIECES] &~ pins);
            generatePawnMoves(pos,captureTargets,moveList,1,pins);
        }
    }
    moveList.checks = checks;
    *moveList.cur++ = 0;

}

void MoveGenerator::addMoves(int startSquare, Bitboard targers, MoveList& moveList, Position& pos)
{
    while(targers)
    {
        int index = popLSB(targers);                        //get piece
        int flags = 0;                                      //init flags default quiet
        if(isBitSet(pos.piecesBitboards[ALL_PIECES],index)) //is capture
        {
            flags = 4;                                      //change flag
        }
        //go to move creator
        *moveList.cur++= createMove(startSquare,index,flags);   //add move to moves list
        moveList.size++;
        // moveList.moveList[moveList.size++]=temp;
    }
}

void MoveGenerator::addMoves(int startSquare,int index,int flags, MoveList& moveList)
{
    //go to move creator
    *moveList.cur++ = createMove(startSquare,index,flags);      //add move to moves list
    moveList.size++;
    // moveList.moveList[moveList.size++]=temp;
}

void MoveGenerator::addMoves(int startSquare, Bitboard targers, MoveList& moveList, Position& pos, int flag)
{
    while(targers)
    {
        int index = popLSB(targers);
        // int flags = 0;
        // if(isBitSet(pos.piecesBitboards[ALL_PIECES],index))
        // {
        //     flags = 4;
        // }
        *moveList.cur++=createMove(startSquare,index,flag);                          //add move to moves list
        moveList.size++;
        // moveList.moveList[moveList.size++]=temp;
    }
}

MoveGenerator::MoveGenerator(BB_utils BBM)
{
    BBManager = BBM;
}

//combine all pieces with its attacks from lookup tables for all attacks from side
Bitboard MoveGenerator::getSideAttacks(Position& pos, bool white, bool forKingMoves)
{

    Bitboard attacks = 0;
    if(white)
    {


    Bitboard target = pos.piecesBitboards[ALL_PIECES];
    if(forKingMoves)
    {
        clearBit(target,LSB(pos.piecesBitboards[BLACK_KING]));
    }

    //pawn moves
    Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.whitePawnCaptures[index];
    }

    //white knights
    iterated = pos.piecesBitboards[WHITE_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.knightMoves[index];
    }

    //white bishops
    iterated = pos.piecesBitboards[WHITE_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(target & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
    }

    //white rooks
    iterated = pos.piecesBitboards[WHITE_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(target & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
    }

    //white queens
    iterated = pos.piecesBitboards[WHITE_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(target & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(target & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
    }

    //white kings
    iterated = pos.piecesBitboards[WHITE_KING];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.kingMoves[index];
    }
    }else
    {

    Bitboard target = pos.piecesBitboards[ALL_PIECES];
    if(forKingMoves)
    {
        clearBit(target,LSB(pos.piecesBitboards[WHITE_KING]));
    }


    //pawn moves
    Bitboard iterated = pos.piecesBitboards[BLACK_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.blackPawnCaptures[index];
    }

    //white knights
    iterated = pos.piecesBitboards[BLACK_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.knightMoves[index];
    }

    //white bishops
    iterated = pos.piecesBitboards[BLACK_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(target & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
    }

    //white rooks
    iterated = pos.piecesBitboards[BLACK_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(target & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
    }

    //white queens
    iterated = pos.piecesBitboards[BLACK_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(target & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(target & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
    }

    //white kings
    iterated = pos.piecesBitboards[BLACK_KING];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.kingMoves[index];
    }
    }
    return attacks;
}



Bitboard MoveGenerator::getPinners(Position& pos, bool white, MoveList& moveList, Bitboard tar, int checks)
{
    Bitboard allPinned = 0;         //returned bb with all pinned pieces for side
    int kingIndex = LSB(pos.piecesBitboards[white ? WHITE_KING : BLACK_KING]);      //index of king

    //attacks from king rook shape intersect with allies
    Bitboard kingRooksAttacks = BBManager.rookMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex],BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

    //new attacks omitting intersections
    Bitboard pinners = BBManager.rookMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] ^ kingRooksAttacks) & BBManager.rookMasks[kingIndex],BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & (pos.piecesBitboards[white ? BLACK_ROOK : WHITE_ROOK] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN]);

    //attacks from king bishop shape intersect with allies
    Bitboard kingsBishopAttacks = BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[kingIndex],BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

    //new attacks omitting intersections
    pinners |= BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] ^ kingsBishopAttacks) & BBManager.bishopMasks[kingIndex],BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & (pos.piecesBitboards[white ? BLACK_BISHOP : WHITE_BISHOP] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN]);

    // cout<<((pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex]) ^ kingRooksAttacks)<<endl;
    // cout<<BBManager.rookMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex]) ^ kingRooksAttacks,BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])]<<endl;
    // cout<<(pos.piecesBitboards[white ? BLACK_ROOK : WHITE_ROOK] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN])<<endl;
    while(pinners) //loop through all PINNERS
    {
        int pinnerIndex = popLSB(pinners);  //Get index of pinner

        //get lane of pin thorugh rectangular lookup
        Bitboard bbPinned = BBManager.rectangularLookup[pinnerIndex][kingIndex] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

        if(bbPinned == 0)   //safety check for position& where you check king but intersect with ally on other side
            continue;          //example rnbqkbnr/ppp1pppp/8/3p4/Q1P5/8/PP1PPPPP/RNB1KBNR b KQkq - 1 2 check from a4 but intersection on f7

        //pop index of lane to get pinned piece eg  lane: 1 1 1 1   &   piece in lane: 0 0 1 0  =  result 0 0 1 0
        //there can only be ONE PIECE in lane because in bbPinned we check for enemies so allies are omitted
        int pinnedIndex = LSB(bbPinned);

        // we add pinned index to all pinned bb
        setBit(allPinned,pinnedIndex);

        if(checks > 0)
            continue;

        // get target without pinner
        Bitboard target = BBManager.rectangularLookup[pinnerIndex][kingIndex];

        // add pinner to bitboard
        setBit(target,pinnerIndex);

        // combine it with check bb
        target &= tar;

        // get piecetype of pin
        int pieceType = pos.piecesArray[pinnedIndex];

        // movegen depending on piece
        switch (pieceType)
        {
        case WHITE_PAWN:
            addPawnWhiteQuiet(pinnedIndex, moveList, pos, target);
            addPawnWhiteCaptures(pinnedIndex, moveList, pos, target & pos.piecesBitboards[BLACK_PIECES]);
            break;
        case WHITE_BISHOP:
            addBishopsMoves(pinnedIndex, moveList, pos, target);
            break;
        case WHITE_ROOK:
            addRookMoves(pinnedIndex, moveList, pos, target);
            break;
        case WHITE_QUEEN:
            addBishopsMoves(pinnedIndex, moveList, pos, target);
            addRookMoves(pinnedIndex, moveList, pos, target);
            break;
        case BLACK_PAWN:
            addPawnBlackQuiet(pinnedIndex, moveList, pos, target);
            addPawnBlackCaptures(pinnedIndex, moveList, pos, target & pos.piecesBitboards[WHITE_PIECES]);
            break;
        case BLACK_BISHOP:
            addBishopsMoves(pinnedIndex, moveList, pos, target);
            break;
        case BLACK_ROOK:
            addRookMoves(pinnedIndex, moveList, pos, target);
            break;
        case BLACK_QUEEN:
            addBishopsMoves(pinnedIndex, moveList, pos, target);
            addRookMoves(pinnedIndex, moveList, pos, target);
            break;
        default:
            break;
        }
    }

    //attacks from king bishop shape intersect with allies
    // Bitboard kingsBishopAttacks = BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[kingIndex],BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

    //new attacks omitting intersections
    // Bitboard pinnersBishop = BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] ^ kingsBishopAttacks) & BBManager.bishopMasks[kingIndex],BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & (pos.piecesBitboards[white ? BLACK_BISHOP : WHITE_BISHOP] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN]);

    // while(pinnersBishop)
    // {
    //     //Get index of pinner
    //     int pinnerIndex = popLSB(pinnersBishop);

    //     //get lane of pin thorugh rectangular lookup
    //     Bitboard bbPinned = BBManager.rectangularLookup[pinnerIndex][kingIndex] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

    //     if(bbPinned == 0)   //safety check for position& where you check king but intersect with ally on other side
    //         continue;          //example rnbqkbnr/ppp1pppp/8/3p4/Q1P5/8/PP1PPPPP/RNB1KBNR b KQkq - 1 2 check from a4 but intersection on f7


    //     //lsb of lane pinned
    //     int pinnedIndex = LSB(bbPinned);

    //     //add pinned to all
    //     setBit(allPinned,pinnedIndex);




    //     //create target bitboard with pinner to king lane
    //     Bitboard target = BBManager.rectangularLookup[pinnerIndex][kingIndex];
    //     setBit(target,pinnerIndex);
    //     target &= tar;
    //     //piecetype movegen
    //     int pieceType = pos.piecesArray[pinnedIndex];
    //     // movegen depending on piece
    //     switch (pieceType)
    //     {
    //     case WHITE_PAWN:
    //         addPawnWhiteQuiet(pinnedIndex, moveList, pos, target);
    //         addPawnWhiteCaptures(pinnedIndex, moveList, pos, target & pos.piecesBitboards[BLACK_PIECES]);
    //         break;
    //     case WHITE_BISHOP:
    //         addBishopsMoves(pinnedIndex, moveList, pos, target);
    //         break;
    //     case WHITE_ROOK:
    //         addRookMoves(pinnedIndex, moveList, pos, target);
    //         break;
    //     case WHITE_QUEEN:
    //         addBishopsMoves(pinnedIndex, moveList, pos, target);
    //         addRookMoves(pinnedIndex, moveList, pos, target);
    //         break;
    //     case BLACK_PAWN:
    //         addPawnBlackQuiet(pinnedIndex, moveList, pos, target);
    //         addPawnBlackCaptures(pinnedIndex, moveList, pos, target & pos.piecesBitboards[WHITE_PIECES]);
    //         break;
    //     case BLACK_BISHOP:
    //         addBishopsMoves(pinnedIndex, moveList, pos, target);
    //         break;
    //     case BLACK_ROOK:
    //         addRookMoves(pinnedIndex, moveList, pos, target);
    //         break;
    //     case BLACK_QUEEN:
    //         addBishopsMoves(pinnedIndex, moveList, pos, target);
    //         addRookMoves(pinnedIndex, moveList, pos, target);
    //         break;
    //     default:
    //         break;
    //     }
    // }
    return allPinned;
}


void MoveGenerator::addPawnWhiteQuiet(int startSquare, MoveList& moveList, Position& pos, Bitboard target)
{
    // while(iterated)
    // {
    //     int index = popLSB(iterated);       //Get pawn index
    //     if(!isBitSet(pins,index))           //Pin moves are generated in getPinners
    //     {
    Bitboard possibleMoves = BBManager.whitePawnMoves[startSquare] & target;  //Default moves Targets are empty squares by default
    possibleMoves &= ~pos.piecesBitboards[ALL_PIECES];                 //this is probably useless
    if(startSquare / 8 == 1)                                                  //am i 2nd rank?
    {
        Bitboard doublePawnMove = (1ULL << (startSquare + 16));               //index of double pawn push
        Bitboard intersection = doublePawnMove & target;                //intersection with target (possible check protecion) probably useless in normal case scenario
        if(intersection != 0)                                           //if intersection with target we can consider legality
        {
        if(!isBitSet(pos.piecesBitboards[ALL_PIECES],startSquare+8))          //if there is nothing in front
        {
            if(!isBitSet(pos.piecesBitboards[ALL_PIECES],startSquare+16))     //if there is nothing on target square double push
            // setBit(possibleMoves,index+16);                             //we get xtra move
            addMoves(startSquare, 1ULL<<(startSquare+16), moveList, pos, PAWN_DOUBLE_MOVE);
        }
        }
    }
    if(startSquare / 8 == 6 && (((1ULL << startSquare+8) & possibleMoves) != 0))
    {
        addMoves(startSquare,startSquare+8,KNIGHT_PROMOTION,moveList);
        addMoves(startSquare,startSquare+8,BISHOP_PROMOTION,moveList);
        addMoves(startSquare,startSquare+8,ROOK_PROMOTION,moveList);
        addMoves(startSquare,startSquare+8,QUEEN_PROMOTION,moveList);
    }else
    {
        addMoves(startSquare, possibleMoves, moveList, pos);
    }
}

//only simple captures without enpassant
void MoveGenerator::addPawnWhiteCaptures(int startSquare, MoveList& moveList, Position& pos, Bitboard target)
{
    Bitboard possibleMoves = BBManager.whitePawnCaptures[startSquare] & target;
    if(startSquare / 8 == 6)
    {
        if(((1ULL << startSquare+7) & possibleMoves) != 0)
        {
            addMoves(startSquare,startSquare+7,KNIGHT_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare+7,BISHOP_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare+7,ROOK_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare+7,QUEEN_PROMOTION_CAPTURE,moveList);
        }
        if(((1ULL << startSquare+9) & possibleMoves) != 0)
        {
            addMoves(startSquare,startSquare+9,KNIGHT_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare+9,BISHOP_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare+9,ROOK_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare+9,QUEEN_PROMOTION_CAPTURE,moveList);
        }
    }else
    {
        addMoves(startSquare, possibleMoves, moveList, pos);
    }
}

//enpassant moves and pins
void MoveGenerator::addPawnWhiteEnpassant(MoveList& moveList, Position& pos, Bitboard target, int enPassantSquare)
{
    Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
    while(iterated)
    {
        int startSquare = popLSB(iterated);



        if(((1ULL << enPassantSquare) & BBManager.whitePawnCaptures[startSquare] & target) == (1ULL << enPassantSquare))
        {
            Bitboard enpassantAttacks = 0;
            int enpassantTakenIndex = enPassantSquare - 8;

            //remove pawn and enpassant target to see if its legal
            Bitboard removingPawns = 1ULL << startSquare | 1ULL << enpassantTakenIndex;


            int kingIndex = LSB(pos.piecesBitboards[WHITE_KING]);

            //try if we will be in check
            //target must include target square bcoz rnbqkbnr/1pp2ppp/p7/3pP3/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4 enpassant still blocks check
            enpassantAttacks |= BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] | 1ULL << enPassantSquare ) & BBManager.bishopMasks[kingIndex] &~removingPawns,BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & (pos.piecesBitboards[BLACK_BISHOP] | pos.piecesBitboards[BLACK_QUEEN]);
            enpassantAttacks |= BBManager.rookMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] | 1ULL << enPassantSquare )  & BBManager.rookMasks[kingIndex] &~removingPawns,BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & (pos.piecesBitboards[BLACK_ROOK] | pos.piecesBitboards[BLACK_QUEEN]);

            //if we are good to go gen moves
            if(enpassantAttacks == 0)
            {
            Bitboard possibleEnPassant = BBManager.whitePawnCaptures[startSquare] & (1ULL << enPassantSquare);
            addMoves(startSquare, possibleEnPassant, moveList, pos, EN_PASSANT);
            }
        }

    }
}

void MoveGenerator::addPawnBlackQuiet(int startSquare, MoveList& moveList, Position& pos, Bitboard target)
{
    // while(iterated)
    // {
    //     int index = popLSB(iterated);       //Get pawn index
    //     if(!isBitSet(pins,index))           //Pin moves are generated in getPinners
    //     {
    Bitboard possibleMoves = BBManager.blackPawnMoves[startSquare] & target;  //Default moves Targets are empty squares by default
    possibleMoves &= ~ pos.piecesBitboards[ALL_PIECES];                 //this is probably useless
    if(startSquare / 8 == 6)                                                  //am i 2nd rank?
    {
        Bitboard doublePawnMove = (1ULL << (startSquare - 16));               //index of double pawn push
        Bitboard intersection = doublePawnMove & target;                //intersection with target (possible check protecion) probably useless in normal case scenario
        if(intersection != 0)                                           //if intersection with target we can consider legality
        {
        if(!isBitSet(pos.piecesBitboards[ALL_PIECES],startSquare-8))          //if there is nothing in front
        {
            if(!isBitSet(pos.piecesBitboards[ALL_PIECES],startSquare-16))     //if there is nothing on target square double push
            // setBit(possibleMoves,index+16);                             //we get xtra move
            addMoves(startSquare, 1ULL<<(startSquare-16), moveList, pos, PAWN_DOUBLE_MOVE);
        }
        }
    }
    if(startSquare / 8 == 1 && (((1ULL << startSquare-8) & possibleMoves) != 0))
    {
        addMoves(startSquare,startSquare-8,KNIGHT_PROMOTION,moveList);
        addMoves(startSquare,startSquare-8,BISHOP_PROMOTION,moveList);
        addMoves(startSquare,startSquare-8,ROOK_PROMOTION,moveList);
        addMoves(startSquare,startSquare-8,QUEEN_PROMOTION,moveList);
    }else
    {
        addMoves(startSquare, possibleMoves, moveList, pos);
    }
}

//only simple captures without enpassant
void MoveGenerator::addPawnBlackCaptures(int startSquare, MoveList& moveList, Position& pos, Bitboard target)
{
    Bitboard possibleMoves = BBManager.blackPawnCaptures[startSquare] & target;
    if(startSquare / 8 == 1)
    {
        if(((1ULL << startSquare-7) & possibleMoves) != 0)
        {
            addMoves(startSquare,startSquare-7,KNIGHT_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare-7,BISHOP_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare-7,ROOK_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare-7,QUEEN_PROMOTION_CAPTURE,moveList);
        }
        if(((1ULL << startSquare-9) & possibleMoves) != 0)
        {
            addMoves(startSquare,startSquare-9,KNIGHT_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare-9,BISHOP_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare-9,ROOK_PROMOTION_CAPTURE,moveList);
            addMoves(startSquare,startSquare-9,QUEEN_PROMOTION_CAPTURE,moveList);
        }
    }else
    {
        addMoves(startSquare, possibleMoves, moveList, pos);
    }
}

//enpassant moves and pins
void MoveGenerator::addPawnBlackEnpassant(MoveList& moveList, Position& pos, Bitboard target, int enPassantSquare)
{
    Bitboard iterated = pos.piecesBitboards[BLACK_PAWN];
    while(iterated)
    {
        int startSquare = popLSB(iterated);

        if(((1ULL << enPassantSquare) & BBManager.blackPawnCaptures[startSquare] & target) == (1ULL << enPassantSquare))
        {
            Bitboard enpassantAttacks = 0;
            int enpassantTakenIndex = enPassantSquare + 8;

            //remove pawn and enpassant target to see if its legal
            Bitboard removingPawns = 1ULL << startSquare | 1ULL << enpassantTakenIndex;


            int kingIndex = LSB(pos.piecesBitboards[BLACK_KING]);

            //try if we will be in check
            //target must include target square bcoz rnbqkbnr/1pp2ppp/p7/3pP3/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4 enpassant still blocks check
            enpassantAttacks |= BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] | 1ULL << enPassantSquare )  & BBManager.bishopMasks[kingIndex] &~removingPawns,BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & (pos.piecesBitboards[WHITE_BISHOP] | pos.piecesBitboards[WHITE_QUEEN]);
            enpassantAttacks |= BBManager.rookMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] | 1ULL << enPassantSquare )  & BBManager.rookMasks[kingIndex] &~removingPawns,BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & (pos.piecesBitboards[WHITE_ROOK] | pos.piecesBitboards[WHITE_QUEEN]);

            if(enpassantAttacks == 0)
            {
                Bitboard possibleEnPassant = BBManager.blackPawnCaptures[startSquare] & (1ULL << enPassantSquare);
                addMoves(startSquare, possibleEnPassant, moveList, pos, EN_PASSANT);
            }
        }

    }
}


void MoveGenerator::addKnightsMoves(int startSquare, MoveList& moveList, Position& pos, Bitboard target)
{
    Bitboard possibleMoves = BBManager.knightMoves[startSquare] & target;  //lookup
    // possibleMoves &= ~ pos.piecesBitboards[pos.STM ? WHITE_PIECES : BLACK_PIECES];  // - friendly
    addMoves(startSquare, possibleMoves, moveList, pos);   //go to move creator
}

void MoveGenerator::addBishopsMoves(int startSquare, MoveList& moveList, Position& pos, Bitboard target)
{
    //lookup
    Bitboard possibleMoves = BBManager.bishopMoves[startSquare][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[startSquare],BBManager.bishopsMagics[startSquare],BBManager.bishopBits[startSquare])] & target;

    // - friendly
    // possibleMoves &= ~ pos.piecesBitboards[pos.STM ? WHITE_PIECES : BLACK_PIECES];

    //go to move creator
    addMoves(startSquare, possibleMoves, moveList, pos);
}

void MoveGenerator::addRookMoves(int startSquare, MoveList& moveList, Position& pos, Bitboard target)
{
    //lookup
    Bitboard possibleMoves = BBManager.rookMoves[startSquare][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[startSquare],BBManager.rooksMagics[startSquare],BBManager.rookBits[startSquare])] & target;

    // - friendly
    // possibleMoves &= ~ pos.piecesBitboards[pos.STM ? WHITE_PIECES : BLACK_PIECES];

    //go to move creator
    addMoves(startSquare, possibleMoves, moveList, pos);
}

void MoveGenerator::generateKingsMoves(Position& pos, MoveList& moveList, Bitboard target, bool white, int checks)
{
    //white kings
    int index = LSB(pos.piecesBitboards[white ? WHITE_KING : BLACK_KING]);

    Bitboard possibleMoves = BBManager.kingMoves[index] & target;
    possibleMoves &= ~ pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];
    addMoves(index, possibleMoves, moveList, pos);

    if(checks < 1)
    {
        int castlingRights = pos.stateInfoList[pos.stateCounter].castlingRights;
        if(white)
        {
            // Bitboard emptySpaces = 96; // 1ULL << 5 | 1ULL << 6;
            // Bitboard targetedEmptySpaces = emptySpaces & target;
            // Bitboard occupiedEmptySpaces = emptySpaces & pos.piecesBitboards[ALL_PIECES];
            if((castlingRights & 8) && ((96 & target) == 96) && ((96 & pos.piecesBitboards[ALL_PIECES]) == 0))
            {
                addMoves(index, 1ULL << 6, moveList, pos, 2);
            }
            // emptySpaces = 12; // 1ULL << 3 | 1ULL << 4;
            // targetedEmptySpaces = 12 & target;
            // occupiedEmptySpaces = 14 & pos.piecesBitboards[ALL_PIECES];
            if((castlingRights & 4) && ((12 & target) == 12) && ((14 & pos.piecesBitboards[ALL_PIECES]) == 0))
            {
                addMoves(index, 1ULL << 2, moveList, pos, 3);
            }
        }else
        {
            // Bitboard emptySpaces = 6917529027641081856; // 1ULL << 61 | 1ULL << 62;
            // Bitboard targetedEmptySpaces = emptySpaces & target;
            // Bitboard occupiedEmptySpaces = emptySpaces & pos.piecesBitboards[ALL_PIECES];
            if((castlingRights & 2) && ((6917529027641081856 & target) == 6917529027641081856) && ((6917529027641081856 & pos.piecesBitboards[ALL_PIECES]) == 0))
            {
                addMoves(index, 1ULL << 62, moveList, pos, 2);
            }
            // emptySpaces = 864691128455135232; // 1ULL << 58 | 1ULL << 59;
            // targetedEmptySpaces = 864691128455135232 & target;
            // occupiedEmptySpaces = 1008806316530991104 & pos.piecesBitboards[ALL_PIECES];
            if((castlingRights & 1) && ((864691128455135232 & target) == 864691128455135232) && ((1008806316530991104 & pos.piecesBitboards[ALL_PIECES]) == 0))
            {
                addMoves(index, 1ULL << 58, moveList, pos, 3);
            }
        }
    }

}
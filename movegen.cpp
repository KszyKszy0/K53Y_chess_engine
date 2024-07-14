#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"
#include <vector>


void MoveGenerator::generateTypeMoves(Position& pos, Bitboard target, vector<Move> &moveList, int type, int checks, Bitboard pins)
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
            addPawnWhiteQuiet(index,moveList,pos,target);
        }else
        {
            addPawnWhiteCaptures(index,moveList,pos,target);
        }
        }
    }

    //white knights
    iterated = pos.piecesBitboards[WHITE_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);   //Get knight index
        if(!isBitSet(pins,index))       //If not pinned gen Moves
        addKnightsMoves(index,moveList,pos,target);
        // Bitboard possibleMoves = BBManager.knightMoves[index] & target;
        // possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        // addMoves(index, possibleMoves, moveList, pos);
    }

    //white bishops
    iterated = pos.piecesBitboards[WHITE_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);   //Get bishop index
        if(!isBitSet(pins,index))       //If not pinned gen Moves and so on for each piece
        addBishopsMoves(index,moveList,pos,target);
        // Bitboard possibleMoves = BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & target;
        // possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        // addMoves(index, possibleMoves, moveList, pos);
    }

    //white rooks
    iterated = pos.piecesBitboards[WHITE_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))
        addRookMoves(index,moveList,pos,target);
        // Bitboard possibleMoves = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & target;
        // possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        // addMoves(index, possibleMoves, moveList, pos);
    }

    //white queens
    iterated = pos.piecesBitboards[WHITE_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))       //combination of rook and bishop
        {
        addBishopsMoves(index,moveList,pos,target);
        addRookMoves(index,moveList,pos,target);
        }
        // Bitboard possibleMoves = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & target;
        // possibleMoves |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & target;
        // possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        // addMoves(index, possibleMoves, moveList, pos);
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
            addPawnBlackQuiet(index,moveList,pos,target);
        }else
        {
            addPawnBlackCaptures(index,moveList,pos,target);
        }
        }
    }

    //white knights
    iterated = pos.piecesBitboards[BLACK_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))
        addKnightsMoves(index,moveList,pos,target);
        // Bitboard possibleMoves = BBManager.knightMoves[index] & target;
        // possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
        // addMoves(index, possibleMoves, moveList, pos);
    }

    //white bishops
    iterated = pos.piecesBitboards[BLACK_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))
        addBishopsMoves(index,moveList,pos,target);
        // Bitboard possibleMoves = BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & target;
        // possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
        // addMoves(index, possibleMoves, moveList, pos);
    }

    //white rooks
    iterated = pos.piecesBitboards[BLACK_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))
        addRookMoves(index,moveList,pos,target);
        // Bitboard possibleMoves = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & target;
        // possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
        // addMoves(index, possibleMoves, moveList, pos);
    }

    //white queens
    iterated = pos.piecesBitboards[BLACK_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))
        {
        addBishopsMoves(index,moveList,pos,target);
        addRookMoves(index,moveList,pos,target);
        }
        // Bitboard possibleMoves = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & target;
        // possibleMoves |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & target;
        // possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
        // addMoves(index, possibleMoves, moveList, pos);
    }
    //white kings
    // Bitboard iterated = pos.piecesBitboards[BLACK_KING];
    // while(iterated)
    // {
    //     int index = popLSB(iterated);
    //     Bitboard possibleMoves = BBManager.kingMoves[index] & target;
    //     possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
    //     addMoves(index, possibleMoves, moveList, pos);
    // }
    }
}

vector<Move> MoveGenerator::fullMovesList(Position& pos)
{
    vector<Move> movesList;  //movesList that will be used for all substeps
    //preparations

    int kingIndex = LSB(pos.piecesBitboards[pos.STM ? WHITE_KING : BLACK_KING]); //index of king

    //checking indexes of pieces points bb
    Bitboard checksAttacks = howManyAttacks(pos,!pos.STM,kingIndex);
    int checks = popCount(checksAttacks);       //Amount of checks; >2 skips most of generation

    Bitboard checkTargets = 0;          //lane of checks without the checker
    int singleCheckIndex = 100;         //index of checker

    if(checks == 1)
    {
        singleCheckIndex = LSB(checksAttacks);          //get index of checker
        checkTargets |= BBManager.rectangularLookup[kingIndex][singleCheckIndex];   //update check lanes bb
        // setBit(checkTargets,index);
    }

    // if(checksAttacks == 0)
    // {
    //     checkTargets = pos.piecesBitboards[NO_PIECE]; //maybe dangerous because of pawn check ->empty rectangular lookup
    // }

    //this should be used for king only!!!!
    Bitboard enemyAttacks = getSideAttacks(pos, !pos.STM, true);      //Get enemy attacks






    generateKingsMoves(pos, movesList, ~enemyAttacks, pos.STM);      //kings Moves


    if(checks < 2)
    {
    Bitboard pins = getPinners(pos,pos.STM,movesList,checksAttacks == 0 ? 0xffffffffffffffff : checkTargets);          //pin generation for normal pieces
                                                                //and normal pins pawns


    generateTypeMoves(pos,checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checkTargets,movesList,0,checks,pins); //quiets



    Bitboard captureTargets = singleCheckIndex == 100 ? 0xffffffffffffffff : 1ULL << singleCheckIndex;
    captureTargets &= pos.STM ? pos.piecesBitboards[BLACK_PIECES] : pos.piecesBitboards[WHITE_PIECES];

    if(pos.STM)
    {
        addPawnWhiteEnpassant(movesList, pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checkTargets);      //enpassant moves and enpassant pins
    }else
    {
        addPawnBlackEnpassant(movesList, pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checkTargets);      //enpassant moves and enpassant pins
    }

    // cout<<captureTargets<<endl;
    if(pos.STM == WHITE)
    {
        generateTypeMoves(pos,captureTargets,movesList,1,checks,pins);
    }else
    {
        generateTypeMoves(pos,captureTargets,movesList,1,checks,pins);
    }
    }
    return movesList;
}

void MoveGenerator::addMoves(int startSquare, Bitboard targers, vector<Move>& movesList, Position& pos)
{
    while(targers)
    {
        int index = popLSB(targers);                        //get piece
        int flags = 0;                                      //init flags default quiet
        if(isBitSet(pos.piecesBitboards[ALL_PIECES],index)) //is capture
        {
            flags = 4;                                      //change flag
        }
        Move temp = createMove(startSquare,index,flags);    //go to move creator
        movesList.push_back(temp);                          //add move to moves list
    }
}

void MoveGenerator::addMoves(int startSquare, Bitboard targers, vector<Move>& movesList, Position& pos, int flag)
{
    while(targers)
    {
        int index = popLSB(targers);
        // int flags = 0;
        // if(isBitSet(pos.piecesBitboards[ALL_PIECES],index))
        // {
        //     flags = 4;
        // }
        Move temp = createMove(startSquare,index,flag);
        movesList.push_back(temp);
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



Bitboard MoveGenerator::getPinners(Position& pos, bool white, vector<Move>& movesList, Bitboard tar)
{
    Bitboard allPinned = 0;         //returned bb with all pinned pieces for side
    int kingIndex = LSB(pos.piecesBitboards[white ? WHITE_KING : BLACK_KING]);      //index of king

    //attacks from king rook shape intersect with allies
    Bitboard kingRooksAttacks = BBManager.rookMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex],BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

    //new attacks omitting intersections
    Bitboard pinnersRook = BBManager.rookMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] ^ kingRooksAttacks) & BBManager.rookMasks[kingIndex],BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & (pos.piecesBitboards[white ? BLACK_ROOK : WHITE_ROOK] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN]);

    // cout<<((pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex]) ^ kingRooksAttacks)<<endl;
    // cout<<BBManager.rookMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex]) ^ kingRooksAttacks,BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])]<<endl;
    // cout<<(pos.piecesBitboards[white ? BLACK_ROOK : WHITE_ROOK] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN])<<endl;
    while(pinnersRook) //loop through all PINNERS
    {
        int pinnerIndex = popLSB(pinnersRook);  //Get index of pinner

        //get lane of pin thorugh rectangular lookup
        Bitboard bbPinned = BBManager.rectangularLookup[pinnerIndex][kingIndex] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

        if(bbPinned == 0)   //safety check for position& where you check king but intersect with ally on other side
            break;          //example rnbqkbnr/ppp1pppp/8/3p4/Q1P5/8/PP1PPPPP/RNB1KBNR b KQkq - 1 2 check from a4 but intersection on f7

        //pop index of lane to get pinned piece eg  lane: 1 1 1 1   &   piece in lane: 0 0 1 0  =  result 0 0 1 0
        //there can only be ONE PIECE in lane because in bbPinned we check for enemies so allies are omitted
        int pinnedIndex = LSB(bbPinned);

        // we add pinned index to all pinned bb
        setBit(allPinned,pinnedIndex);

        // get target without pinner
        Bitboard target = BBManager.rectangularLookup[pinnerIndex][kingIndex];

        // add pinner to bitboard
        setBit(target,pinnerIndex);

        target &= tar;

        // get piecetype of pin
        int pieceType = pos.piecesArray[pinnedIndex];

        // movegen depending on piece
        switch (pieceType)
        {
        case WHITE_PAWN:
            addPawnWhiteQuiet(pinnedIndex, movesList, pos, target);
            addPawnWhiteCaptures(pinnedIndex, movesList, pos, target & pos.piecesBitboards[BLACK_PIECES]);
            break;
        case WHITE_BISHOP:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            break;
        case WHITE_ROOK:
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        case WHITE_QUEEN:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        case BLACK_PAWN:
            addPawnBlackQuiet(pinnedIndex, movesList, pos, target);
            addPawnWhiteCaptures(pinnedIndex, movesList, pos, target & pos.piecesBitboards[WHITE_PIECES]);
            break;
        case BLACK_BISHOP:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            break;
        case BLACK_ROOK:
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        case BLACK_QUEEN:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        default:
            break;
        }
    }

    //attacks from king bishop shape intersect with allies
    Bitboard kingsBishopAttacks = BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[kingIndex],BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

    //new attacks omitting intersections
    Bitboard pinnersBishop = BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex((pos.piecesBitboards[ALL_PIECES] ^ kingsBishopAttacks) & BBManager.bishopMasks[kingIndex],BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & (pos.piecesBitboards[white ? BLACK_BISHOP : WHITE_BISHOP] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN]);

    while(pinnersBishop)
    {
        //Get index of pinner
        int pinnerIndex = popLSB(pinnersBishop);

        //get lane of pin thorugh rectangular lookup
        Bitboard bbPinned = BBManager.rectangularLookup[pinnerIndex][kingIndex] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

        if(bbPinned == 0)   //safety check for position& where you check king but intersect with ally on other side
            break;          //example rnbqkbnr/ppp1pppp/8/3p4/Q1P5/8/PP1PPPPP/RNB1KBNR b KQkq - 1 2 check from a4 but intersection on f7


        //lsb of lane pinned
        int pinnedIndex = LSB(bbPinned);

        //add pinned to all
        setBit(allPinned,pinnedIndex);




        //create target bitboard with pinner to king lane
        Bitboard target = BBManager.rectangularLookup[pinnerIndex][kingIndex];
        setBit(target,pinnerIndex);
        target &= tar;
        //piecetype movegen
        int pieceType = pos.piecesArray[pinnedIndex];
        // movegen depending on piece
        switch (pieceType)
        {
        case WHITE_PAWN:
            addPawnWhiteQuiet(pinnedIndex, movesList, pos, target);
            addPawnWhiteCaptures(pinnedIndex, movesList, pos, target & pos.piecesBitboards[BLACK_PIECES]);
            break;
        case WHITE_BISHOP:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            break;
        case WHITE_ROOK:
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        case WHITE_QUEEN:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        case BLACK_PAWN:
            addPawnBlackQuiet(pinnedIndex, movesList, pos, target);
            addPawnBlackCaptures(pinnedIndex, movesList, pos, target & pos.piecesBitboards[WHITE_PIECES]);
            break;
        case BLACK_BISHOP:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            break;
        case BLACK_ROOK:
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        case BLACK_QUEEN:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        default:
            break;
        }
    }
    return allPinned;
}


void MoveGenerator::addPawnWhiteQuiet(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target)
{
    // while(iterated)
    // {
    //     int index = popLSB(iterated);       //Get pawn index
    //     if(!isBitSet(pins,index))           //Pin moves are generated in getPinners
    //     {
    Bitboard possibleMoves = BBManager.whitePawnMoves[startSquare] & target;  //Default moves Targets are empty squares by default
    possibleMoves &= ~ pos.piecesBitboards[ALL_PIECES];                 //this is probably useless
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
            addMoves(startSquare, 1ULL<<(startSquare+16), movesList, pos, PAWN_DOUBLE_MOVE);
        }
        }
    }
    addMoves(startSquare, possibleMoves, movesList, pos);
}

//only simple captures without enpassant
void MoveGenerator::addPawnWhiteCaptures(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target)
{
    Bitboard possibleMoves = BBManager.whitePawnCaptures[startSquare] & target;
    addMoves(startSquare, possibleMoves, movesList, pos);
}

//enpassant moves and pins
void MoveGenerator::addPawnWhiteEnpassant(vector<Move>& movesList, Position& pos, Bitboard target)
{
    Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
    while(iterated)
    {
        int startSquare = popLSB(iterated);

        int enPassantSquare = pos.stateInfoList.back().enPassantSquare;
        if(enPassantSquare != 0)
        {
            if(((1ULL << enPassantSquare) & BBManager.whitePawnCaptures[startSquare] & target) == (1ULL << enPassantSquare))
            {
            Bitboard enpassantAttacks = 0;
            int enpassantTakenIndex = enPassantSquare - 8;


            Bitboard removingPawns = 1ULL << startSquare | 1ULL << enpassantTakenIndex;


            int kingIndex = LSB(pos.piecesBitboards[WHITE_KING]);
            enpassantAttacks |= BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[kingIndex] &~removingPawns,BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & (pos.piecesBitboards[BLACK_BISHOP] | pos.piecesBitboards[BLACK_QUEEN]);
            enpassantAttacks |= BBManager.rookMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex] &~removingPawns,BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & (pos.piecesBitboards[BLACK_ROOK] | pos.piecesBitboards[BLACK_QUEEN]);

            if(enpassantAttacks == 0)
            {
            Bitboard possibleEnPassant = BBManager.whitePawnCaptures[startSquare] & (1ULL << enPassantSquare);
            addMoves(startSquare, possibleEnPassant, movesList, pos, EN_PASSANT);
            }
            }
        }
    }
}

void MoveGenerator::addPawnBlackQuiet(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target)
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
            addMoves(startSquare, 1ULL<<(startSquare-16), movesList, pos, PAWN_DOUBLE_MOVE);
        }
        }
    }
    addMoves(startSquare, possibleMoves, movesList, pos);
}

//only simple captures without enpassant
void MoveGenerator::addPawnBlackCaptures(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target)
{
    Bitboard possibleMoves = BBManager.blackPawnCaptures[startSquare] & target;
    addMoves(startSquare, possibleMoves, movesList, pos);
}

//enpassant moves and pins
void MoveGenerator::addPawnBlackEnpassant(vector<Move>& movesList, Position& pos, Bitboard target)
{
    Bitboard iterated = pos.piecesBitboards[BLACK_PAWN];
    while(iterated)
    {
        int startSquare = popLSB(iterated);

        int enPassantSquare = pos.stateInfoList.back().enPassantSquare;
        if(enPassantSquare != 0)
        {
            if(((1ULL << enPassantSquare) & BBManager.blackPawnCaptures[startSquare] & target) == (1ULL << enPassantSquare))
            {
            Bitboard enpassantAttacks = 0;
            int enpassantTakenIndex = enPassantSquare + 8;


            Bitboard removingPawns = 1ULL << startSquare | 1ULL << enpassantTakenIndex;


            int kingIndex = LSB(pos.piecesBitboards[BLACK_KING]);
            enpassantAttacks |= BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[kingIndex] &~removingPawns,BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & (pos.piecesBitboards[WHITE_BISHOP] | pos.piecesBitboards[WHITE_QUEEN]);
            enpassantAttacks |= BBManager.rookMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex] &~removingPawns,BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & (pos.piecesBitboards[WHITE_ROOK] | pos.piecesBitboards[WHITE_QUEEN]);

            if(enpassantAttacks == 0)
            {
            Bitboard possibleEnPassant = BBManager.blackPawnCaptures[startSquare] & (1ULL << enPassantSquare);
            addMoves(startSquare, possibleEnPassant, movesList, pos, EN_PASSANT);
            }
            }
        }
    }
}


void MoveGenerator::addKnightsMoves(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target)
{
    Bitboard possibleMoves = BBManager.knightMoves[startSquare] & target;  //lookup
    possibleMoves &= ~ pos.piecesBitboards[pos.STM ? WHITE_PIECES : BLACK_PIECES];  // - friendly
    addMoves(startSquare, possibleMoves, movesList, pos);   //go to move creator
}

void MoveGenerator::addBishopsMoves(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target)
{
    //lookup
    Bitboard possibleMoves = BBManager.bishopMoves[startSquare][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[startSquare],BBManager.bishopsMagics[startSquare],BBManager.bishopBits[startSquare])] & target;

    // - friendly
    possibleMoves &= ~ pos.piecesBitboards[pos.STM ? WHITE_PIECES : BLACK_PIECES];

    //go to move creator
    addMoves(startSquare, possibleMoves, movesList, pos);
}

void MoveGenerator::addRookMoves(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target)
{
    //lookup
    Bitboard possibleMoves = BBManager.rookMoves[startSquare][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[startSquare],BBManager.rooksMagics[startSquare],BBManager.rookBits[startSquare])] & target;

    // - friendly
    possibleMoves &= ~ pos.piecesBitboards[pos.STM ? WHITE_PIECES : BLACK_PIECES];

    //go to move creator
    addMoves(startSquare, possibleMoves, movesList, pos);
}

Bitboard MoveGenerator::generateKingsMoves(Position& pos, vector<Move>& moveList, Bitboard target, bool white)
{
    //white kings
    Bitboard iterated = pos.piecesBitboards[white ? WHITE_KING : BLACK_KING];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.kingMoves[index] & target;
        possibleMoves &= ~ pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }
}
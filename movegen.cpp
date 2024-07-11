#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"
#include <vector>


void MoveGenerator::generateTypeMoves(Position pos, Bitboard target, vector<Move> &moveList, int type, int checks, Bitboard pins)
{

    if(pos.STM == WHITE)
    {
    //pawn moves
    Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))
        {
        if(type == 0)
        {
            Bitboard possibleMoves = BBManager.whitePawnMoves[index] & target;
            possibleMoves &= ~ pos.piecesBitboards[ALL_PIECES];
            if(index / 8 == 1)
            {
                if(!isBitSet(pos.piecesBitboards[ALL_PIECES],index+8))
                {
                    if(!isBitSet(pos.piecesBitboards[ALL_PIECES],index+16))
                    setBit(possibleMoves,index+16);
                }
            }
            addMoves(index, possibleMoves, moveList, pos);
        }else
        {
            Bitboard possibleMoves = BBManager.whitePawnCaptures[index] & target;
            addMoves(index, possibleMoves, moveList, pos);
        }
        }
    }

    //white knights
    iterated = pos.piecesBitboards[WHITE_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))
        addKnightsMoves(index,moveList,pos,target);
        // Bitboard possibleMoves = BBManager.knightMoves[index] & target;
        // possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        // addMoves(index, possibleMoves, moveList, pos);
    }

    //white bishops
    iterated = pos.piecesBitboards[WHITE_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))
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
        if(!isBitSet(pins,index))
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
        int index = popLSB(iterated);
        if(!isBitSet(pins,index))
        {
        if(type == 0)
        {
            Bitboard possibleMoves = BBManager.blackPawnMoves[index] & target;
            possibleMoves &= ~ pos.piecesBitboards[ALL_PIECES];
            if(index / 8 == 6)
            {
                if(!isBitSet(pos.piecesBitboards[ALL_PIECES],index-8))
                {
                    if(!isBitSet(pos.piecesBitboards[ALL_PIECES],index-16))
                    setBit(possibleMoves,index-16);
                }
            }
            addMoves(index, possibleMoves, moveList, pos);
        }else
        {
            Bitboard possibleMoves = BBManager.blackPawnCaptures[index] & target;
            addMoves(index, possibleMoves, moveList, pos);
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

vector<Move> MoveGenerator::fullMovesList(Position pos)
{
    vector<Move> movesList;
    Bitboard checksAttacks = howManyAttacks(pos,!pos.STM,LSB(pos.piecesBitboards[pos.STM ? WHITE_KING : BLACK_KING]));
    int checks = popCount(checksAttacks);
    Bitboard enemyAttacks = getSideAttacks(pos, !pos.STM);
    Bitboard pins = getPinners(pos,pos.STM,movesList);
    generateKingsEvasions(pos, movesList, ~enemyAttacks, pos.STM);
    generateTypeMoves(pos,checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checksAttacks,movesList,0,checks,pins); //quiets
    if(pos.STM == WHITE)
    {
        generateTypeMoves(pos,pos.piecesBitboards[BLACK_PIECES],movesList,1,checks,pins);
    }else
    {
        generateTypeMoves(pos,pos.piecesBitboards[WHITE_PIECES],movesList,1,checks,pins);
    }

    return movesList;
}

void MoveGenerator::addMoves(int startSquare, Bitboard targers, vector<Move>& movesList, Position pos)
{
    while(targers)
    {
        int index = popLSB(targers);
        int flags = 0;
        if(isBitSet(pos.piecesBitboards[ALL_PIECES],index))
        {
            flags = 4;
        }
        Move temp = createMove(startSquare,index,flags);
        movesList.push_back(temp);
    }
}

MoveGenerator::MoveGenerator(BB_utils BBM)
{
    BBManager = BBM;
}

Bitboard MoveGenerator::getSideAttacks(Position pos, bool white)
{
    Bitboard attacks = 0;
    if(white)
    {

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
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
    }

    //white rooks
    iterated = pos.piecesBitboards[WHITE_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
    }

    //white queens
    iterated = pos.piecesBitboards[WHITE_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
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
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
    }

    //white rooks
    iterated = pos.piecesBitboards[BLACK_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
    }

    //white queens
    iterated = pos.piecesBitboards[BLACK_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        attacks |= BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        attacks |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
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

//True white attacks on square, False black attacks on square
Bitboard MoveGenerator::howManyAttacks(Position pos, bool white, int index)
{
    Bitboard attacks = 0;
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

Bitboard MoveGenerator::generateKingsEvasions(Position pos, vector<Move>& moveList, Bitboard target, bool white)
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

Bitboard MoveGenerator::getPinners(Position pos, bool white, vector<Move>& movesList)
{
    Bitboard allPinned = 0;
    int kingIndex = LSB(pos.piecesBitboards[white ? WHITE_KING : BLACK_KING]);
    Bitboard kingRooksAttacks = BBManager.rookMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex],BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];
    Bitboard pinnersRook = BBManager.rookMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[kingIndex] ^ kingRooksAttacks,BBManager.rooksMagics[kingIndex],BBManager.rookBits[kingIndex])] & (pos.piecesBitboards[white ? BLACK_ROOK : WHITE_ROOK] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN]);

    while(pinnersRook)
    {
        int pinnerIndex = popLSB(pinnersRook);
        Bitboard bbPinned = BBManager.rectangularLookup[pinnerIndex][kingIndex] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];
        int pinnedIndex = LSB(bbPinned);
        setBit(allPinned,pinnedIndex);
        int pieceType = pos.piecesArray[pinnedIndex];
        Bitboard target = BBManager.rectangularLookup[pinnerIndex][kingIndex];
        setBit(target,pinnerIndex);
        switch (pieceType)
        {
        case WHITE_BISHOP:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            break;
        case WHITE_ROOK:
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        case WHITE_QUEEN:
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        case BLACK_BISHOP:
            addBishopsMoves(pinnedIndex, movesList, pos, target);
            break;
        case BLACK_ROOK:
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        case BLACK_QUEEN:
            addRookMoves(pinnedIndex, movesList, pos, target);
            break;
        default:
            break;
        }
    }

    Bitboard kingsBishopAttacks = BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[kingIndex],BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];
    Bitboard pinnersBishop = BBManager.bishopMoves[kingIndex][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[kingIndex] ^ kingsBishopAttacks,BBManager.bishopsMagics[kingIndex],BBManager.bishopBits[kingIndex])] & (pos.piecesBitboards[white ? BLACK_BISHOP : WHITE_BISHOP] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN]);
    while(pinnersBishop)
    {
        int pinnerIndex = popLSB(pinnersBishop);
        Bitboard bbPinned = BBManager.rectangularLookup[pinnerIndex][kingIndex] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];
        int pinnedIndex = LSB(bbPinned);
        setBit(allPinned,pinnedIndex);
        int pieceType = pos.piecesArray[pinnedIndex];
        Bitboard target = BBManager.rectangularLookup[pinnerIndex][kingIndex];
        setBit(target,pinnerIndex);
        switch (pieceType)
        {
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

void MoveGenerator::addKnightsMoves(int startSquare, vector<Move>& movesList, Position pos, Bitboard target)
{
    Bitboard possibleMoves = BBManager.knightMoves[startSquare] & target;
    possibleMoves &= ~ pos.piecesBitboards[pos.STM ? WHITE_PIECES : BLACK_PIECES];
    addMoves(startSquare, possibleMoves, movesList, pos);
}

void MoveGenerator::addBishopsMoves(int startSquare, vector<Move>& movesList, Position pos, Bitboard target)
{
    Bitboard possibleMoves = BBManager.bishopMoves[startSquare][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[startSquare],BBManager.bishopsMagics[startSquare],BBManager.bishopBits[startSquare])] & target;
    possibleMoves &= ~ pos.piecesBitboards[pos.STM ? WHITE_PIECES : BLACK_PIECES];
    addMoves(startSquare, possibleMoves, movesList, pos);
}

void MoveGenerator::addRookMoves(int startSquare, vector<Move>& movesList, Position pos, Bitboard target)
{
    Bitboard possibleMoves = BBManager.rookMoves[startSquare][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[startSquare],BBManager.rooksMagics[startSquare],BBManager.rookBits[startSquare])] & target;
    possibleMoves &= ~ pos.piecesBitboards[pos.STM ? WHITE_PIECES : BLACK_PIECES];
    addMoves(startSquare, possibleMoves, movesList, pos);
}
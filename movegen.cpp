#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"
#include <vector>


void MoveGenerator::generateTypeMoves(Position pos, Bitboard target, vector<Move> &moveList, int type)
{

    if(pos.STM == WHITE)
    {
    //pawn moves
    Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(type == 0)
        {
            Bitboard possibleMoves = BBManager.whitePawnMoves[index] & target;
            possibleMoves &= ~ pos.piecesBitboards[ALL_PIECES];
            if(index / 8 == 1)
            {
                if(!isBitSet(pos.piecesBitboards[ALL_PIECES],index+8))
                {
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

    //white knights
    iterated = pos.piecesBitboards[WHITE_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.knightMoves[index] & target;
        possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }

    //white bishops
    iterated = pos.piecesBitboards[WHITE_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & target;
        possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }

    //white rooks
    iterated = pos.piecesBitboards[WHITE_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & target;
        possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }

    //white queens
    iterated = pos.piecesBitboards[WHITE_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & target;
        possibleMoves |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & target;
        possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }

    //white kings
    iterated = pos.piecesBitboards[WHITE_KING];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.kingMoves[index] & target;
        possibleMoves &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }

    }else
    {
    //pawn moves
    Bitboard iterated = pos.piecesBitboards[BLACK_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);
        if(type == 0)
        {
            Bitboard possibleMoves = BBManager.blackPawnMoves[index] & target;
            possibleMoves &= ~ pos.piecesBitboards[ALL_PIECES];
            if(index / 8 == 6)
            {
                if(!isBitSet(pos.piecesBitboards[ALL_PIECES],index-8))
                {
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

    //white knights
    iterated = pos.piecesBitboards[BLACK_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.knightMoves[index] & target;
        possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }

    //white bishops
    iterated = pos.piecesBitboards[BLACK_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & target;
        possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }

    //white rooks
    iterated = pos.piecesBitboards[BLACK_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & target;
        possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }

    //white queens
    iterated = pos.piecesBitboards[BLACK_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])] & target;
        possibleMoves |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])] & target;
        possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }

    //white kings
    iterated = pos.piecesBitboards[BLACK_KING];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard possibleMoves = BBManager.kingMoves[index] & target;
        possibleMoves &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index, possibleMoves, moveList, pos);
    }
    }
}

vector<Move> MoveGenerator::fullMovesList(Position pos)
{
    vector<Move> movesList;
    generateTypeMoves(pos,pos.piecesBitboards[NO_PIECE],movesList,0); //quiets
    if(pos.STM == WHITE)
    {
        generateTypeMoves(pos,pos.piecesBitboards[BLACK_PIECES],movesList,1);
    }else
    {
        generateTypeMoves(pos,pos.piecesBitboards[WHITE_PIECES],movesList,1);
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
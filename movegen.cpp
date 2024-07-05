#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"
#include <vector>


vector<Move> MoveGenerator::generateMoves(Position pos)
{
    vector<Move> moveList;

    if(pos.STM == WHITE)
    {
    //pawn moves
    Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.whitePawnMoves[index];
        target &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index,target, moveList);
    }

    //white knights
    iterated = pos.piecesBitboards[WHITE_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.knightMoves[index];
        target &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index,target, moveList);
    }

    //white bishops
    iterated = pos.piecesBitboards[WHITE_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
        target &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index,target, moveList);
    }

    //white rooks
    iterated = pos.piecesBitboards[WHITE_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        target &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index,target, moveList);
    }

    //white queens
    iterated = pos.piecesBitboards[WHITE_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        target |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
        target &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index,target, moveList);
    }

    }else
    {
    //pawn moves
    Bitboard iterated = pos.piecesBitboards[BLACK_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.blackPawnMoves[index];
        target &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index,target, moveList);
    }

    //white knights
    iterated = pos.piecesBitboards[BLACK_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.knightMoves[index];
        target &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index,target, moveList);
    }

    //white bishops
    iterated = pos.piecesBitboards[BLACK_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
        target &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index,target, moveList);
    }

    //white rooks
    iterated = pos.piecesBitboards[BLACK_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        target &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index,target, moveList);
    }

    //white queens
    iterated = pos.piecesBitboards[BLACK_QUEEN];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.rookMasks[index],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        target |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES] & BBManager.bishopMasks[index],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
        target &= ~ pos.piecesBitboards[BLACK_PIECES];
        addMoves(index,target, moveList);
    }
    }
    return moveList;
}

void MoveGenerator::addMoves(int startSquare, Bitboard targers, vector<Move>& movesList)
{
    while(targers)
    {
        int index = popLSB(targers);
        Move temp = createMove(startSquare,index,0);
        movesList.push_back(temp);
    }
}

MoveGenerator::MoveGenerator(BB_utils BBM)
{
    BBManager = BBM;
}
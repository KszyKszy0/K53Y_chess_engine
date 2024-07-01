#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"


vector<Move> MoveGenerator::generateMoves(Position pos)
{

    //pawn moves
    Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.whitePawnMoves[index];
        addMoves(index,target);
    }

    //white knights
    iterated = pos.piecesBitboards[WHITE_KNIGHT];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.knightMoves[index];
        addMoves(index,target);
    }

    //white bishops
    iterated = pos.piecesBitboards[WHITE_BISHOP];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
        target &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index,target);
    }

    //white rooks
    iterated = pos.piecesBitboards[WHITE_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        target &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index,target);
    }

    //white queens
    iterated = pos.piecesBitboards[WHITE_ROOK];
    while(iterated)
    {
        int index = popLSB(iterated);
        Bitboard target = BBManager.rookMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES],BBManager.rooksMagics[index],BBManager.rookBits[index])];
        target |= BBManager.bishopMoves[index][BBManager.getMagicIndex(pos.piecesBitboards[ALL_PIECES],BBManager.bishopsMagics[index],BBManager.bishopBits[index])];
        target &= ~ pos.piecesBitboards[WHITE_PIECES];
        addMoves(index,target);
    }

    return movesList;
}

void MoveGenerator::addMoves(int startSquare, Bitboard targers)
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
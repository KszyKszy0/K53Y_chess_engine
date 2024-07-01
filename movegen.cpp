#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"


vector<Move> MoveGenerator::generateMoves(Position pos)
{
    while(pos.piecesBitboards[WHITE_PAWN])
    {
        cout<<pos.piecesBitboards[WHITE_PAWN]<<endl;
        int index = popLSB(pos.piecesBitboards[WHITE_PAWN]);
        cout<<index<<endl;
        Bitboard target = BBManager.whitePawnMoves[index];
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
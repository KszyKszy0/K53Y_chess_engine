#include "core.h"

core::core() : bbManager(BB_utils()), moveGenerator(bbManager)
{

}

int core::perft(int depth)
{
    int counter=0;
    if(depth==0)
    {
        return 1;
    }
    vector<Move> moveList = moveGenerator.generateMoves(pos);
    for(Move m : moveList)
    {
        printMove(m);
        pos.makeMove(m);
        counter += perft(depth-1);
        pos.undoMove(m);
    }
    return counter;
}
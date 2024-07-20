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

    Move moveList[218];
    moveGenerator.fullMovesList(pos, moveList);

    if(depth == 1)
    {
        int count = 0;
        for(Move m : moveList)
        {
            count++;
            if(m == 0)
                return count;
        }
    }

    for(Move m : moveList)
    {
        if(m == 0)
            return counter;
        // printMove(m);
        int localCounter = 0;

        pos.makeMove(m);

        localCounter += perft(depth-1);
        counter += localCounter;
        pos.undoMove(m);

        if(depth==6)
        {
            printMove(m);
            cout<<": "<<localCounter<<endl;
        }
    }
    return counter;
}

void core::uci() {
    std::cout << "id name K53Y Chess Engine" << std::endl;
    std::cout << "id author KszyKszy" << std::endl;
    std::cout << "uciok" << std::endl;
}

void core::isReady() {
    std::cout << "readyok" << std::endl;
}

void core::newGame(){
    pos.parseFEN(startingFen,pos.piecesBitboards);
    positionCounter = 0;
}

void core::newGame(string FEN){
    pos.parseFEN(FEN,pos.piecesBitboards);
    positionCounter = 0;
}

void core::setPosition(vector<string>& moves)
{
    // pos.parseFEN(startingFen,pos.piecesBitboards);
    // for(string s : moves)
    // {
    //     pos.makeMove(uciToMove(s));
    // }
    positionCounter = moves.size();
    if(moves.size() > 0)
    {
        string move = moves[positionCounter-1];
        Move movesList[218];
        moveGenerator.fullMovesList(pos, movesList);
        pos.makeMove(uciToMove(move,pos,movesList));
    }
}

void core::quit()
{
    exit(0);
}

void core::go()
{
    search.negamax(5, 0, -100000, 100000,pos.STM ? 1 : -1, moveGenerator, pos, eval);
}
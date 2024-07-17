#include "core.h"
#include <cassert>
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
    vector<Move> moveList = moveGenerator.fullMovesList(pos);
    if(depth == 1)
    {
        return moveList.size();
    }

    for(Move m : moveList)
    {
        // printMove(m);
        int localCounter = 0;

        pos.makeMove(m);

        localCounter += perft(depth-1);
        counter += localCounter;
        pos.undoMove(m);

        if(depth==6)
        {
            // cout<<"////////////////////////////////////////////////////////////////////"<<endl<<endl;
            printMove(m);
            cout<<": "<<localCounter<<endl;
        }
        // try
        // {
        //     if(temp != pos.piecesBitboards[BLACK_KNIGHT])
        //     {
        //         throw 1;
        //     }
        // }
        // catch(int code)
        // {
        //     for(int i=0; i<=15; i++)
        //     {
        //         printBitboard(pos.piecesBitboards[i]);
        //         cout<<endl;
        //     }
        //     printPieceArray(pos.piecesArray);
        //     break;
        // }

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
}

void core::newGame(string FEN){
    pos.parseFEN(FEN,pos.piecesBitboards);
}

void core::setPosition(vector<string>& moves)
{
    // pos.parseFEN(startingFen,pos.piecesBitboards);
    // for(string s : moves)
    // {
    //     pos.makeMove(uciToMove(s));
    // }
    int len = moves.size();
    int temps = pos.stateInfoList.size();
    if(moves.size() > 0)
    {
        StateInfo tempState = pos.stateInfoList.back();
        int movesIndex = pos.stateInfoList.back().halfMove;
        string move = moves[movesIndex];
        vector<Move> movesList = moveGenerator.fullMovesList(pos);
        pos.makeMove(uciToMove(move,pos,movesList));
        // cout<<uciToMove(move);
    }
    // printPieceArray(pos.piecesArray);
}

void core::quit()
{
    exit(0);
}

void core::go()
{
    search.negamax(5, 0, -100000, 100000,pos.STM ? 1 : -1, moveGenerator, pos);
}
#include "core.h"
#include "position.h"

core::core() : bbManager(BB_utils()), moveGenerator(bbManager)
{

}

Bitboard core::perft(int depth)
{
    Bitboard counter=0;
    if(depth==0)
    {
        return 1;
    }

    MoveList moves;
    moveGenerator.fullMovesList(pos, moves);

    if(depth == 1)
    {
        // int count = 0;
        // for(Move m : moves.moveList)
        // {
        //     if(m == 0)
        //         return count;
        //     count++;
        // }
        // for(int i=0; i<=moves.size; i++)
        // {
        //     printMove(moves.moveList[i]);
        //     cout<<endl;
        // }
        return moves.size;
    }

    // for(int i=0; i<=moves.size-1; i++)
    // {
    //     int localCounter = 0;

    //     pos.makeMove(moves.moveList[i]);

    //     localCounter += perft(depth-1);
    //     counter += localCounter;
    //     pos.undoMove(moves.moveList[i]);

    //     if(depth==6)
    //     {
    //         printMove(moves.moveList[i]);
    //         cout<<": "<<localCounter<<endl;
    //     }
    // }
    for(Move m : moves)
    {
        if(m == 0)
            return counter;
        // printMove(m);
        Bitboard localCounter = 0;

        Bitboard hash = pos.positionHash;
        pos.makeMove(m);

        localCounter += perft(depth-1);
        counter += localCounter;
        pos.undoMove(m);
        if(hash != pos.positionHash)
        {
            cout<<"error";
        }

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
        MoveList moveList;
        moveGenerator.fullMovesList(pos, moveList);
        pos.makeMove(uciToMove(move,pos,moveList.moveList));
    }
}

void core::quit()
{
    exit(0);
}

void core::go()
{
    // search.negamax(5, 0, -100000, 100000,pos.STM ? 1 : -1, moveGenerator, pos, eval);
    search.search(pos,moveGenerator,eval);
}
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
    Bitboard allp = pos.piecesBitboards[ALL_PIECES];
    for(Move m : moveList)
    {
        // printMove(m);
        int localCounter = 0;
        Bitboard temp = pos.piecesBitboards[BLACK_KNIGHT];
        pos.makeMove(m);

        localCounter += perft(depth-1);
        counter += localCounter;
        pos.undoMove(m);

        if(depth==5)
        {
            cout<<"////////////////////////////////////////////////////////////////////"<<endl<<endl;
            printMove(m);
            cout<<localCounter<<endl;
        }
        if(allp!=pos.piecesBitboards[ALL_PIECES])
        {
            cout<<allp<<endl<<pos.piecesBitboards[ALL_PIECES]<<endl;
            cout<<"BTAK SIGMY";
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
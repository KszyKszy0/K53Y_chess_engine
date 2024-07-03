#include <iostream>
#include "bitboard.h"
#include <string>
#include <vector>
#include "position.h"
#include "move.h"
#include "movegen.h"

using namespace std;


int main()
{
    BB_utils BB_manager;

    BB_manager.init_bb();

    Position newGame;

    newGame.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", newGame.piecesBitboards);

    // for(int i=0; i<=14; i++)
    // {
    //     printBitboard(newGame.piecesBitboards[i]);

    //     cout<<endl;
    // }


    MoveGenerator generator(BB_manager);

    generator.generateMoves(newGame);


    // cout<<BB_manager.rookMoves[0][BB_manager.getMagicIndex(281474976711038,BB_manager.rooksMagics[0],BB_manager.rookBits[0])]<<endl;

    for(Move m : generator.movesList)
    {
        newGame.makeMove(m);
    }

    return 0;
}
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

    for(int i=0; i<=14; i++)
    {
        printBitboard(newGame.piecesBitboards[i]);
        
        cout<<endl;
    }
    
    
    Move test = createMove(a2,a4,3);
    cout<<test;

    MoveGenerator generator(BB_manager);

    generator.generateMoves(newGame);
    
    for(Move m : generator.movesList)
    {
        cout<<m<<endl;
    }

    return 0;
}
#include <iostream>
#include "bitboard.h"
#include <string>
#include <vector>
#include "position.h"


using namespace std;


int main()
{
    BB_utils BB_manager;

    BB_manager.init_bb();
    
    Position newGame;

    newGame.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", newGame.piecesBitboards);

    for(int i=0; i<=14; i++)
    {
        BB_manager.printBitboard(newGame.piecesBitboards[i]);
        
        cout<<endl;
    }
    
    return 0;
}
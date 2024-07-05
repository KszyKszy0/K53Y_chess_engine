#include <iostream>
#include "bitboard.h"
#include <string>
#include <vector>
#include "position.h"
#include "move.h"
#include "movegen.h"
#include "core.h"

using namespace std;


int main()
{
    core Engine = core();

    Engine.pos.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Engine.pos.piecesBitboards);

    vector<Move> moves = Engine.moveGenerator.generateMoves(Engine.pos);

    for(Move m : moves)
    {
        Engine.pos.makeMove(m);
    }

    for(int i=0; i<=14; i++)
    {
        printBitboard(Engine.pos.piecesBitboards[i]);
        cout<<endl;
    }

    return 0;
}
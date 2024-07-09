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

    vector<Move> moves = Engine.moveGenerator.fullMovesList(Engine.pos);

    // for(Move m : moves)
    // {
    //     Engine.pos.makeMove(m);
    //     Engine.pos.undoMove(m);
    // }

    // Engine.pos.makeMove(createMove(chessSquareToIndex("e2"),chessSquareToIndex("e4"),0));

    // Engine.pos.makeMove(createMove(chessSquareToIndex("f7"),chessSquareToIndex("f5"),0));

    // Engine.pos.makeMove(createMove(chessSquareToIndex("e4"),chessSquareToIndex("f5"),CAPTURE));

    // Engine.pos.undoMove(createMove(chessSquareToIndex("e4"),chessSquareToIndex("f5"),CAPTURE));

    // Engine.pos.makeMove(createMove(chessSquareToIndex("g7"),chessSquareToIndex("g6"),0));

    // Engine.pos.makeMove(createMove(chessSquareToIndex("f5"),chessSquareToIndex("g6"),CAPTURE));

    cout<<Engine.perft(3);

    // for(int i=0; i<=15; i++)
    // {
    //     printBitboard(Engine.pos.piecesBitboards[i]);
    //     cout<<endl;
    // }

    // printPieceArray(Engine.pos.piecesArray);

    return 0;
}
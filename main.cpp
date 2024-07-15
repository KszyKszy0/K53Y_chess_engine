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

    string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Engine.pos.parseFEN("rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1", Engine.pos.piecesBitboards);

    // Engine.moveGenerator.getPinners(Engine.pos,true);

    // vector<Move> moves = Engine.moveGenerator.fullMovesList(Engine.pos);

    // for(Move m : moves)
    // {
    //     Engine.pos.makeMove(m);
    //     Engine.pos.undoMove(m);
    // }

    // Engine.pos.makeMove(createMove(nameToSquare("e1"),nameToSquare("g1"),SHORT_CASTLE));

    // Engine.pos.undoMove(createMove(nameToSquare("e1"),nameToSquare("g1"),SHORT_CASTLE));
    // Engine.pos.makeMove(createMove(nameToSquare("b7"),nameToSquare("b5"),QUIET));

    // Engine.pos.undoMove(createMove(nameToSquare("c7"),nameToSquare("c6"),QUIET));
    // cout<<Engine.pos.stateInfoList.back().enPassantSquare;

    // Engine.pos.makeMove(createMove(nameToSquare("a4"),nameToSquare("b5"),CAPTURE));
    // Engine.pos.undoMove(createMove(nameToSquare("a4"),nameToSquare("b5"),CAPTURE));

    // Engine.pos.makeMove(createMove(nameToSquare("d4"),nameToSquare("e5"),CAPTURE));

    // Engine.pos.undoMove(createMove(nameToSquare("d4"),nameToSquare("e5"),CAPTURE));

    // Engine.pos.makeMove(createMove(nameToSquare("b2"),nameToSquare("b4"),PAWN_DOUBLE_MOVE));

    // Engine.pos.undoMove(createMove(nameToSquare("e4"),nameToSquare("f5"),CAPTURE));

    // Engine.pos.makeMove(createMove(nameToSquare("g7"),nameToSquare("g6"),0));

    // Engine.pos.makeMove(createMove(nameToSquare("f5"),nameToSquare("g6"),CAPTURE));

    cout<<Engine.perft(6)<<endl;

    // vector<Move> temp = Engine.moveGenerator.fullMovesList(Engine.pos);

    // for(Move m : temp)
    // {
    //     printMove(m);
    // }

    // for(int i=0; i<=15; i++)
    // {
    //     printBitboard(Engine.pos.piecesBitboards[i]);
    //     cout<<endl;
    // }

    // printPieceArray(Engine.pos.piecesArray);

    return 0;
}
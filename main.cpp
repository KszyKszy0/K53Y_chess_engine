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

    Engine.pos.parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ", Engine.pos.piecesBitboards);

    // Engine.moveGenerator.getPinners(Engine.pos,true);

    // vector<Move> moves = Engine.moveGenerator.fullMovesList(Engine.pos);

    // for(Move m : moves)
    // {
    //     Engine.pos.makeMove(m);
    //     Engine.pos.undoMove(m);
    // }

    // Engine.pos.makeMove(createMove(nameToSquare("e2"),nameToSquare("e4"),QUIET));
    // cout<<Engine.pos.stateInfoList.back().castlingRights<<endl;

    // Engine.pos.makeMove(createMove(nameToSquare("e7"),nameToSquare("e5"),QUIET));
    // cout<<Engine.pos.stateInfoList.back().castlingRights<<endl;

    // Engine.pos.makeMove(createMove(nameToSquare("e1"),nameToSquare("e2"),QUIET));
    // cout<<Engine.pos.stateInfoList.back().castlingRights<<endl;

    // Engine.pos.undoMove(createMove(nameToSquare("e1"),nameToSquare("e2"),QUIET));
    // cout<<Engine.pos.stateInfoList.back().castlingRights<<endl;
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

    cout<<Engine.perft(5)<<endl;

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
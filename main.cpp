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

    Engine.pos.parseFEN(startingFen, Engine.pos.piecesBitboards);

    // Engine.moveGenerator.getPinners(Engine.pos,true);

    // vector<Move> moves = Engine.moveGenerator.fullMovesList(Engine.pos);

    // for(Move m : moves)
    // {
    //     Engine.pos.makeMove(m);
    //     Engine.pos.undoMove(m);
    // }

    Engine.pos.makeMove(createMove(nameToSquare("e2"),nameToSquare("e4"),1));

    cout<<Engine.pos.stateInfoList.back().enPassantSquare;

    // Engine.pos.makeMove(createMove(chessSquareToIndex("f7"),chessSquareToIndex("f5"),0));

    // Engine.pos.makeMove(createMove(chessSquareToIndex("e4"),chessSquareToIndex("f5"),CAPTURE));

    // Engine.pos.undoMove(createMove(chessSquareToIndex("e4"),chessSquareToIndex("f5"),CAPTURE));

    // Engine.pos.makeMove(createMove(chessSquareToIndex("g7"),chessSquareToIndex("g6"),0));

    // Engine.pos.makeMove(createMove(chessSquareToIndex("f5"),chessSquareToIndex("g6"),CAPTURE));

    // cout<<Engine.perft(4);

    // for(int i=0; i<=15; i++)
    // {
    //     printBitboard(Engine.pos.piecesBitboards[i]);
    //     cout<<endl;
    // }

    // printPieceArray(Engine.pos.piecesArray);

    return 0;
}
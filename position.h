#pragma once

#include "bitboard.h"
#include "enums.h"
#include "stateInfo.h"
#include <string>
#include "TT.h"
#include "zobrist.h"
#include "move.h"
#include "helperFunctions.h"
#include "accumulator.h"


using namespace std;

class Position
{
    public:

    const static int MAX_GAME_LENGTH = 2000;

    //All pieces BB look enums.h
    Bitboard piecesBitboards[16];

    //Creates position from FEN
    void parseFEN(string fen);

    //Gets FEN from position
    std::string getFEN();

    Zobrist zobrist;

    Bitboard positionHash;

    TranspositionTable TT;

    Bitboard positionHistory[MAX_GAME_LENGTH];

    bool STM;

    //Make move doesnt care if its legal
    void makeMove(Move move);

    //Undo move doesnt care if its legal
    void undoMove(Move move);

    //Creates and add state of game to list
    void addState(int pas, int cast, int half, int full, int captureType, Accumulator& acc);

    //Creates and add state of game to list
    void addState(StateInfo state);

    //List of game states
    StateInfo stateInfoList[MAX_GAME_LENGTH];

    //Array of all pieces types on chessboard
    int piecesArray[64];

    int stateCounter = 0;

    Accumulator accum;
};
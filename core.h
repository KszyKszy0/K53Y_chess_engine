#pragma once
#include "movegen.h"
#include "position.h"
#include "search.h"
#include "eval.h"
#include "accumulator.h"
// #include "TT.h"

class core
{
public:

    // Class describes chess position
    // it also works as game
    Position pos;

    // Starting fen of starting position in chess game
    string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    // Number of moves taken for fen so we only move once
    // per uci call
    int positionCounter;

    // Simple constructor
    // for member initialization
    core();

    // Simple perft from position that is currently set
    Bitboard perft(int depth);



    // Uci simple commands

    void uci();
    void isReady();

    // End of simple commands



    // Uci game commands

    // Creates new game from startins position
    void newGame();

    // Creates new game from given fen
    void newGame(string FEN);

    // Makes moves given in vector considering position counter
    // also increments position Counter by 2
    // because of enemy move and our move
    void setPosition(vector<string> &moves);

    void setPosition(vector<string>& moves, string FEN);

    // Starts search
    void go();

    // Performs perft on a given depth
    void goPerft(int depth);

    // Stops everything
    void quit();

    // Stops search
    void stop();

    //Sets time from uci
    void setTime(int wTime, int bTime, int wInc, int bInc, int moveTime);

    void setLimits(int depth, int nodes);

    void printState();

    void readNNUE();

    void eval();

    void state();
};
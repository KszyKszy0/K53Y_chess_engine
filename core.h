#include "movegen.h"
#include "position.h"
#include "search.h"
#include "eval.h"

class core
{
    public:
    BB_utils bbManager;
    MoveGenerator moveGenerator;
    Position pos;
    Search search;
    Evaluator eval;

    string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    int positionCounter;

    core();

    int perft(int depth);

    void uci();
    void isReady();


    void newGame();
    void newGame(string FEN);

    void setPosition(vector<string>& moves);

    void go();
    void quit();
};
#include "bitboard.h"
#include "movegen.h"
#include "position.h"

class core
{
    public:
    BB_utils bbManager;
    MoveGenerator moveGenerator;
    Position pos;

    core();

    int perft(int depth);
};
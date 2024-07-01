#include "move.h"



Move Move_utils::createMove(int startSquare, int targetSquare, int flags)
{
    return startSquare + (targetSquare << 6) + (flags << 12);
}
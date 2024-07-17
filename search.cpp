#include "search.h"
#include "movegen.h"
#include "eval.h"

int Search::negamax(int depth, int ply, int alpha, int beta, int color, MoveGenerator& moveGenerator, Position& pos)
{
    vector<Move> moveList = moveGenerator.fullMovesList(pos);

    if(pos.isCheckmate)
    {
        return -100000+ply;
        // return -10000000+ply;
          // Prefer faster checkmate
    }

    if(pos.isStalemate)
    {
        return 0;
    }


    if(depth == 0)
    {
        return evaluate(pos) * color;
    }





    int best = -10000000;
    Move bestMove = 0;
    for(Move m : moveList)
    {
        pos.makeMove(m);
        int value = -negamax(depth - 1, ply + 1, -beta, -alpha, -color, moveGenerator, pos);
        pos.undoMove(m);

        if(value > best)
        {
            best = value;
            if(ply == 0)
            {
                bestMove = m;
            }
        }
        if(best > alpha)
        {
            alpha = best;
        }
        if(alpha >= beta)
        {
            break;  // Alpha-beta pruning
        }
    }

    if(ply == 0)
    {
        pos.makeMove(bestMove);
        cout << "bestmove " << moveToUci(bestMove) << endl;
    }
    return best;
}
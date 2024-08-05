#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "TT.h"
#include <chrono>

int Search::negamax(int depth, int ply, int alpha, int beta, int color, MoveGenerator& moveGenerator, Position& pos, Evaluator& eval, chrono::steady_clock::time_point start)
{


    Bitboard key = pos.positionHash;

    TTEntry entry = pos.TT.transpositionTable[key % pos.TT.size];

    Move transpositionMove = 0;

    //we set it to all node in case it fails low
    int newFlag = UPPER_BOUND;

    nodesCount++;

    if(key == entry.zorbistKey)
    {
        transpositionCount++;
        transpositionMove = entry.bestMove;
        if((entry.depth >= depth) && (ply > 0))
        {
            if((entry.type == EXACT_SCORE) || ((entry.type == LOWER_BOUND) && (entry.score >= beta)) || ((entry.type == UPPER_BOUND) && (entry.score < alpha)))
            {
                matchedTranspositions++;
                return entry.score;
            }
        }
    }else if(entry.zorbistKey != 0)
    {
        collisions++;
    }

    MoveList moveList;
    moveGenerator.fullMovesList(pos,moveList);

    if(pos.isCheckmate)
    {
        return -CHECKMATE+ply;
        // return -10000000+ply;
          // Prefer faster checkmate
    }

    if(pos.isStalemate || (pos.stateInfoList[pos.stateCounter].halfMove >= 50))
    {
        return 0;
    }


    if(depth == 0)
    {
        return eval.evaluate(pos) * color;
    }



    if (transpositionMove != 0)
    {
        // Move* move = &moveList.moveList[1];
        for (size_t i = 0; i < moveList.size; ++i)
        {
            if (moveList.moveList[i] == transpositionMove)
            {
                std::swap(moveList.moveList[0], moveList.moveList[i]);
            }
            // if(Flags(moveList.moveList[i] == CAPTURE))
            // {
            //     std::swap(*move++,moveList.moveList[i]);
            // }
        }
    }

    // if(ply==0)
    // {
    //     cout<<"Depth: "<<depth<<" transposition move: ";
    //     printMove(transpositionMove);
    //     cout<<endl;
    // }

    int best = -NO_MOVE;
    Move bestMove = 0;
    for(Move m : moveList)
    {
        if(m == 0)
            break;

        if((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > 5000)
        {
            if(ply >= 1)
            {
                return (ply % 2 == 1) ? CHECKMATE : -CHECKMATE;
            }
            if(ply == 0)
            {
                if(best != -CHECKMATE)
                    oldEval = best;

                cout<<"info depth "<<depth;
                // cout<<" Change Best Move to: ";
                // printMove(bestMove);
                cout<<" score cp "<<best<<endl;
                return bestMove;
            }
        }

        int captureExtension = Flags(m) == CAPTURE ? 1 : 0;

        Bitboard hash = pos.positionHash;
        pos.makeMove(m);
        int value = -negamax(depth - 1 + captureExtension, ply + 1, -beta, -alpha, -color, moveGenerator, pos, eval, start);
        pos.undoMove(m);




        if(hash != pos.positionHash)
        {
            cout<<"error";
        }

        if(value > best)
        {
            best = value;

            bestMove = m;
        }
        if(best > alpha)
        {
            //new best move found
            newFlag = EXACT_SCORE;

            alpha = best;
        }
        if(alpha >= beta)
        {
            //move causes cutoff
            newFlag = LOWER_BOUND;

            break;  // Alpha-beta pruning
        }
    }

    if(depth > entry.depth)
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }

    if(ply == 0)
    {
        oldEval = best;
        cout<<"info depth "<<depth;
        // cout<<" Change Best Move to: ";
        // printMove(bestMove);
        cout<<" score cp "<<best<<endl;
        // if((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > 5000)
        // {
        //     cout<<"TIME CUTOFF!!"<<endl;
        // }
        return bestMove;
    }


    return best;
}

Move Search::search(Position& pos, MoveGenerator& mg, Evaluator& eval)
{
    Move bestMove = 0;
    oldEval = 0;
    auto start = chrono::steady_clock::now();
    for(int i=1; i<=20; i++)
    {
        // bestMovePrevious = bestMove;
        bestMove = negamax(i, 0, -100000, 100000,pos.STM ? 1 : -1, mg, pos, eval, start);
        if(bestMove != 0)
        {
            bestMovePrevious = bestMove;
        }
        auto end = chrono::steady_clock::now();
        // if(chrono::duration_cast<chrono::milliseconds>(end - start).count() > 3000)
        // {
        //     break;
        // }
    }
    pos.makeMove(bestMovePrevious);
    cout << "bestmove " << moveToUci(bestMovePrevious) << endl;
    return bestMovePrevious;
}
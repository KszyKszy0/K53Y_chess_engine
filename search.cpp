#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "TT.h"
#include <chrono>

int Search::negamax(int depth, int ply, int alpha, int beta, int color, MoveGenerator& moveGenerator, Position& pos, Evaluator& eval, chrono::steady_clock::time_point start)
{
    //this gives info about checkmate and stalemate so it must be the first thing to consider
    MoveList moveList;
    moveGenerator.fullMovesList(pos,moveList);


    Bitboard key = pos.positionHash;

    TTEntry entry = pos.TT.transpositionTable[key % pos.TT.size];

    Move transpositionMove = 0;

    //we set it to all node in case it fails low
    int newFlag = UPPER_BOUND;

    nodesCount++;



    if(moveList.isCheckmate())
    {
        return -CHECKMATE+ply;
        // return -10000000+ply;
          // Prefer faster checkmate
    }

    if((moveList.isStalemate() || (pos.stateInfoList[pos.stateCounter].halfMove >= 100) || (isRepeated(pos))) && (ply > 0))
    {
        return 0;
    }

    if(depth == 0)
    {
        return quiescence(depth, ply, alpha, beta, color, moveGenerator, pos, eval, start);
        // return eval.evaluate(pos) * color;
    }

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
    Move bestMove = moveList.moveList[0];
    for(Move m : moveList)
    {
        if(m == 0)
            break;

        if((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            isCancelled = true;
            if(ply >= 1)
            {
                return CHECKMATE;
            }
            if(ply == 0)
            {
                if((best != -CHECKMATE) && (best != -NO_MOVE))
                    oldEval = best;

                // cout<<"info depth "<<depth;
                // cout<<" Change Best Move to: ";
                // printMove(bestMove);
                // cout<<" score cp "<<oldEval;

                //in case of first move cancel we return nullmove so its detected in seatch when bestmove != 0
                return bestMove;
            }
        }

        // int captureExtension = Flags(m) == CAPTURE ? 1 : 0;

        Bitboard hash = pos.positionHash;
        pos.makeMove(m);
        int value = -negamax(depth - 1, ply + 1, -beta, -alpha, -color, moveGenerator, pos, eval, start);
        pos.undoMove(m);

        if((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            isCancelled = true;
            if(ply >= 1)
            {
                return CHECKMATE;
            }
            if(ply == 0)
            {
                if((best != -CHECKMATE) && (best != -NO_MOVE))
                    oldEval = best;

                // cout<<"info depth "<<depth;
                // cout<<" Change Best Move to: ";
                // printMove(bestMove);
                // cout<<" score cp "<<oldEval;

                //in case of first move cancel we return nullmove so its detected in seatch when bestmove != 0
                return bestMove;
            }
        }



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

    if((depth > entry.depth) && !isCancelled)
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }

    if(ply == 0)
    {
        if((best != -CHECKMATE) && (best != -NO_MOVE))
            oldEval = best;

        // cout<<"info depth "<<depth;
        // cout<<" Change Best Move to: ";
        // printMove(bestMove);
        // cout<<" score cp "<<oldEval;

        //in case of first move cancel we return nullmove so its detected in seatch when bestmove != 0
        return bestMove;
    }

    return best;
}

Move Search::search(Position& pos, MoveGenerator& mg, Evaluator& eval)
{
    Move bestMove = 0;
    oldEval = 0;
    auto start = chrono::steady_clock::now();
    timeLimit = 5000;
    isCancelled = false;
    for(int depth=2; depth<=40; depth++)
    {
        // bestMovePrevious = bestMove;
        nodesCount = 0;
        queiscenceNodes = 0;
        bestMove = negamax(depth, 0, -100000000, 100000000,pos.STM ? 1 : -1, mg, pos, eval, start);
        if(bestMove != 0)
        {
            bestMovePrevious = bestMove;
        }else
        {
            cout<<"Nie ma ruchu"<<endl;
        }
        auto end = chrono::steady_clock::now();

        float time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        cout<<"info depth "<<depth;
        cout<<" score cp "<<oldEval;
        cout<<" nodes "<<nodesCount;
        cout<<" nps "<<(int)(nodesCount/(time/1000));
        cout<<" time "<<time;
        cout<<" quiescence nodes "<<queiscenceNodes;
        cout<<" quiescenceTT "<<quiescenceTT;
        cout<<endl;

        if((oldEval >= CHECKMATE-5) || (oldEval <= -CHECKMATE+5))
        {
            break;
        }
        if(chrono::duration_cast<chrono::milliseconds>(end - start).count() > timeLimit)
        {
            if(depth > 5)
                break;

            isCancelled = false;
            timeLimit+=5000;
        }
    }
    pos.makeMove(bestMovePrevious);
    cout<<endl;
    cout << "bestmove " << moveToUci(bestMovePrevious) << endl;
    return bestMovePrevious;
}

bool Search::isRepeated(Position& pos)
{
    int counter = 0;
    for(int i=0; i<=pos.stateCounter; i++)
    {
        if(pos.positionHash == pos.positionHistory[i])
        {
            counter++;
            if(counter >= 2)
                return true;
        }
    }
    return false;
}





int Search::quiescence(int depth, int ply, int alpha, int beta, int color, MoveGenerator& moveGenerator, Position& pos, Evaluator& eval, chrono::steady_clock::time_point start)
{

    int newFlag = UPPER_BOUND;

    Bitboard key = pos.positionHash;

    TTEntry entry = pos.TT.transpositionTable[key % pos.TT.size];

    Move transpositionMove = 0;

    if(key == entry.zorbistKey)
    {
        if(entry.depth >= depth)
        {
        transpositionCount++;
        transpositionMove = entry.bestMove;

            if((entry.type == EXACT_SCORE) || ((entry.type == LOWER_BOUND) && (entry.score >= beta)) || ((entry.type == UPPER_BOUND) && (entry.score < alpha)))
            {
                matchedTranspositions++;
                quiescenceTT++;
                return entry.score;
            }
        }
    }else if(entry.zorbistKey != 0)
    {
        collisions++;
    }


    queiscenceNodes++;


    int evaluation = eval.evaluate(pos) * color;

    if( evaluation >= beta )
    {
        return evaluation;
    }
    if( evaluation > alpha )
    {
        alpha = evaluation;
    }


    //this gives info about checkmate and stalemate so it must be the first thing to consider
    MoveList moveList;
    moveGenerator.fullCapturesList(pos,moveList);
    int MVVs[218];
    if (transpositionMove != 0)
    {
        // Move* move = &moveList.moveList[1];
        for (int i = 0; i < moveList.size; ++i)
        {
            if (moveList.moveList[i] == transpositionMove)
            {
                std::swap(moveList.moveList[0], moveList.moveList[i]);
                break;
            }

            // for(int j=0; j<=i; j++)
            // {
            //     if(MVV > MVVs[j])
            //     {
            //         std::swap(moveList.moveList[j], moveList.moveList[i]);
            //         std::swap(MVVs[j], MVVs[i]);
            //         break;
            //     }
            // }
            // if(Flags(moveList.moveList[i] == CAPTURE))
            // {
            //     std::swap(*move++,moveList.moveList[i]);
            // }
        }
    }else
    {
        for (int i = 0; i < moveList.size; ++i)
        {
            int MVV = MVVLVA [ pos.piecesArray[ StartSquare(moveList.moveList[i]) ] ] [ pos.piecesArray [ TargetSqaure(moveList.moveList[i] ) ] ];
            MVVs[i] = MVV;
        }
        for (int i = 0; i < moveList.size; ++i)
        {
            for (int j = i+1; j < moveList.size; ++j)
            {
                if(MVVs[j] > MVVs[i])
                {
                    std::swap(moveList.moveList[i], moveList.moveList[j]);
                    std::swap(MVVs[i], MVVs[j]);
                }
            }
        }
    }

    Move bestMove = moveList.moveList[0];
    int best = evaluation;
    for(Move m : moveList)
    {
        if(m == 0)
            break;

        if((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            isCancelled = true;
            return CHECKMATE;
        }

        // int captureExtension = Flags(m) == CAPTURE ? 1 : 0;

        Bitboard hash = pos.positionHash;
        pos.makeMove(m);
        int value = -quiescence(depth - 1, ply + 1, -beta, -alpha, -color, moveGenerator, pos, eval, start);
        pos.undoMove(m);

        if((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            isCancelled = true;
            return CHECKMATE;
        }



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
            alpha = best;
            newFlag = EXACT_SCORE;
        }
        if(alpha >= beta)
        {
            //move causes cutoff
            newFlag = LOWER_BOUND;

            break;
        }
    }

    if((depth > entry.depth) && !isCancelled)
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }

    return alpha;
}
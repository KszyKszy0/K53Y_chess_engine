#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "TT.h"
#include <chrono>

int Search::negamax(int depth, int ply, int alpha, int beta, int color, MoveGenerator &moveGenerator, Position &pos, Evaluator &eval, chrono::steady_clock::time_point start)
{
    // This gives info about checkmate and stalemate so it must be the first thing to consider
    MoveList moveList;
    moveGenerator.fullMovesList(pos, moveList);

    // Hash of current position
    Bitboard key = pos.positionHash;

    // Lookup of entry
    TTEntry entry = pos.TT.transpositionTable[key % pos.TT.size];

    // Possible transpositon move
    Move transpositionMove = 0;

    // We set it to all node in case it fails low
    int newFlag = UPPER_BOUND;

    // Increment node count
    nodesCount++;

    // Checkmate check
    if (moveList.isCheckmate())
    {
        // Prefer faster checkmate
        return -CHECKMATE + ply;
    }

    //Draw check
    if ((moveList.isStalemate() || (pos.stateInfoList[pos.stateCounter].halfMove >= 100) || (isRepeated(pos))) && (ply > 0))
    {
        return 0;
    }

    //Entering Quiescence at the end of search
    if (depth == 0)
    {
        return quiescence(depth, ply, alpha, beta, color, moveGenerator, pos, eval, start);
        // return eval.evaluate(pos) * color;
    }

    //Possible transposition lookup
    if (key == entry.zorbistKey)
    {
        //Transposition count increment for dev and debug purposes
        transpositionCount++;

        //We can always take entry move if we match in TT
        //Thats because there should be no fault entries
        //We don't add entries when cancelling search
        transpositionMove = entry.bestMove;

        //If depth from TT is at least same as current we can immediately take score
        if ((entry.depth >= depth) && (ply > 0))
        {
            //Check for node type (matching ab window) (cutoff) (fail low)
            if ((entry.type == EXACT_SCORE) || ((entry.type == LOWER_BOUND) && (entry.score >= beta)) || ((entry.type == UPPER_BOUND) && (entry.score < alpha)))
            {
                //Matched transposition count increment for dev and debug purposes
                matchedTranspositions++;

                return entry.score;
            }
        }
    }
    else if (entry.zorbistKey != 0)
    {
        // Collisions counter for dev and debug purposes
        collisions++;
    }

    //If we get move from TT but we couldn't use score
    //Move the TT move to the first place in moveList
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

    //We init best to nomove so we always take something
    //Because its smaller than alpha
    int best = -NO_MOVE;

    //In case we stop search on first move from 1 ply
    //We take the first move straight away
    //It should be from TT from previous search
    Move bestMove = moveList.moveList[0];

    //Loop through all moves
    for (Move m : moveList)
    {
        //If we encounter null move stop
        if (m == 0)
            break;

        //If we encounter hard time limit:
        //1. Set cancel flag so we don't store faulty entries to TT
        //2. Cancel search by a worst possible outcome so we pick it if its first move
        //BUT when we searched at least one other move, this one won't be picked
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            //Cancel flag set
            isCancelled = true;

            //At any ply that isn't from root we just return really bad score
            if (ply >= 1)
            {
                return CHECKMATE;
            }

            //At root ply
            if (ply == 0)
            {
                //If we detect that at least one move was searched at root
                //We can change oldEval to best so we get true evaluation in logs
                if ((best != -CHECKMATE) && (best != -NO_MOVE))
                    oldEval = best;


                // In case of first move cancel we return nullmove so its detected in seatch when bestmove != 0
                return bestMove;
            }
        }

        //Make move
        pos.makeMove(m);

        //Go deeper with negamax
        int value = -negamax(depth - 1, ply + 1, -beta, -alpha, -color, moveGenerator, pos, eval, start);

        //Undo move
        pos.undoMove(m);



        //If we encounter hard time limit:
        //1. Set cancel flag so we don't store faulty entries to TT
        //2. Cancel search by a worst possible outcome so we pick it if its first move
        //BUT when we searched at least one other move, this one won't be picked
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            //Cancel flag set
            isCancelled = true;

            //At any ply that isn't from root we just return really bad score
            if (ply >= 1)
            {
                return CHECKMATE;
            }

            //At root ply
            if (ply == 0)
            {
                //If we detect that at least one move was searched at root
                //We can change oldEval to best so we get true evaluation in logs
                if ((best != -CHECKMATE) && (best != -NO_MOVE))
                    oldEval = best;


                // In case of first move cancel we return nullmove so its detected in seatch when bestmove != 0
                return bestMove;
            }
        }

        //If we beat best
        //Set it current best
        //And bestmove
        if (value > best)
        {
            best = value;

            bestMove = m;
        }

        //If we beat alpha
        //Make it EXACT_SCORE
        //New possible pv found
        if (best > alpha)
        {
            // new best move found
            newFlag = EXACT_SCORE;

            alpha = best;
        }

        //If alpha exceeds beta break
        if (alpha >= beta)
        {
            // move causes cutoff
            newFlag = LOWER_BOUND;

            break; // Alpha-beta pruning
        }
    }

    //Store transposition with flag unless search is being cancelled
    if ((depth > entry.depth) && !isCancelled)
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }


    //At root ply
    if (ply == 0)
    {
        //If we detect that at least one move was searched at root
        //We can change oldEval to best so we get true evaluation in logs
        if ((best != -CHECKMATE) && (best != -NO_MOVE))
            oldEval = best;


        // In case of first move cancel we return nullmove so its detected in seatch when bestmove != 0
        return bestMove;
    }

    return best;
}

Move Search::search(Position &pos, MoveGenerator &mg, Evaluator &eval)
{
    //Initializing starting values

    Move bestMove = 0;

    oldEval = 0;

    auto start = chrono::steady_clock::now();

    timeLimit = 5000;

    isCancelled = false;


    //Iterative deepening loop
    for (int depth = 2; depth <= 40; depth++)
    {
        //Reset node count
        nodesCount = 0;
        queiscenceNodes = 0;

        //Start negamax for current depth
        bestMove = negamax(depth, 0, -100000000, 100000000, pos.STM ? 1 : -1, mg, pos, eval, start);
        if (bestMove != 0)
        {
            //We didn't get nullmove we can take that move
            bestMovePrevious = bestMove;
        }

        //Time check and uci info
        auto end = chrono::steady_clock::now();

        float time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        cout << "info depth " << depth;
        cout << " score cp " << oldEval;
        cout << " nodes " << nodesCount;
        cout << " nps " << (int)(nodesCount / (time / 1000));
        cout << " time " << time;
        cout << " quiescence nodes " << queiscenceNodes;
        cout << " quiescenceTT " << quiescenceTT;
        cout << endl;

        //If we have checkmate near don't search any deeper
        if ((oldEval >= CHECKMATE - 5) || (oldEval <= -CHECKMATE + 5))
        {
            break;
        }

        //Time cutoff
        if (chrono::duration_cast<chrono::milliseconds>(end - start).count() > timeLimit)
        {
            //Only if depth is not shallow
            if (depth > 5)
                break;

            //If depth was small we add time to consider more
            isCancelled = false;
            timeLimit += 5000;
        }
    }

    //After ID loop we print best move to uci
    pos.makeMove(bestMovePrevious);
    cout << endl;
    cout << "bestmove " << moveToUci(bestMovePrevious) << endl;
    return bestMovePrevious;
}

//3 fold repetition check
bool Search::isRepeated(Position &pos)
{
    //How many times this position has encountered
    int counter = 0;

    //Simple loop with chcecking position hash in history
    for (int i = 0; i <= pos.stateCounter; i++)
    {
        if (pos.positionHash == pos.positionHistory[i])
        {
            counter++;
            if (counter >= 2)
                return true;
        }
    }
    return false;
}



int Search::quiescence(int depth, int ply, int alpha, int beta, int color, MoveGenerator &moveGenerator, Position &pos, Evaluator &eval, chrono::steady_clock::time_point start)
{

    int newFlag = UPPER_BOUND;

    Bitboard key = pos.positionHash;

    TTEntry entry = pos.TT.transpositionTable[key % pos.TT.size];

    Move transpositionMove = 0;

    if (key == entry.zorbistKey)
    {
        if (entry.depth >= depth)
        {
            transpositionCount++;
            transpositionMove = entry.bestMove;

            if ((entry.type == EXACT_SCORE) || ((entry.type == LOWER_BOUND) && (entry.score >= beta)) || ((entry.type == UPPER_BOUND) && (entry.score < alpha)))
            {
                matchedTranspositions++;
                quiescenceTT++;
                return entry.score;
            }
        }
    }
    else if (entry.zorbistKey != 0)
    {
        collisions++;
    }

    queiscenceNodes++;

    int evaluation = eval.evaluate(pos) * color;

    if (evaluation >= beta)
    {
        return evaluation;
    }
    if (evaluation > alpha)
    {
        alpha = evaluation;
    }

    // this gives info about checkmate and stalemate so it must be the first thing to consider
    MoveList moveList;
    moveGenerator.fullCapturesList(pos, moveList);
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
    }
    else
    {
        for (int i = 0; i < moveList.size; ++i)
        {
            int MVV = MVVLVA[pos.piecesArray[StartSquare(moveList.moveList[i])]][pos.piecesArray[TargetSqaure(moveList.moveList[i])]];
            MVVs[i] = MVV;
        }
        for (int i = 0; i < moveList.size; ++i)
        {
            for (int j = i + 1; j < moveList.size; ++j)
            {
                if (MVVs[j] > MVVs[i])
                {
                    std::swap(moveList.moveList[i], moveList.moveList[j]);
                    std::swap(MVVs[i], MVVs[j]);
                }
            }
        }
    }

    Move bestMove = moveList.moveList[0];
    int best = evaluation;
    for (Move m : moveList)
    {
        if (m == 0)
            break;

        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            isCancelled = true;
            return CHECKMATE;
        }

        // int captureExtension = Flags(m) == CAPTURE ? 1 : 0;

        Bitboard hash = pos.positionHash;
        pos.makeMove(m);
        int value = -quiescence(depth - 1, ply + 1, -beta, -alpha, -color, moveGenerator, pos, eval, start);
        pos.undoMove(m);

        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            isCancelled = true;
            return CHECKMATE;
        }

        if (hash != pos.positionHash)
        {
            cout << "error";
        }

        if (value > best)
        {
            best = value;
            bestMove = m;
        }
        if (best > alpha)
        {
            alpha = best;
            newFlag = EXACT_SCORE;
        }
        if (alpha >= beta)
        {
            // move causes cutoff
            newFlag = LOWER_BOUND;

            break;
        }
    }

    if ((depth > entry.depth) && !isCancelled)
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }

    return alpha;
}
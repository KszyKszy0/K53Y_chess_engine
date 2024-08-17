#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "TT.h"
#include <chrono>
#include "moveList.h"
#include "helperFunctions.h"

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
        return quiescence(depth - 1, ply + 1, alpha, beta, color, moveGenerator, pos, eval, start);
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

    //We can store TT entry if we searched at least two moves and seached is being cancelled
    int movesSearched = 0;

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

            //Save newest best move if possible
            if ((depth > entry.depth) && (!isCancelled || (movesSearched >= 2)))
            {
                pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
            }

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
                //If there was no time cutoff AND we at least searched one move
                //There is one in million chance that time cutoff will appear at the start of first ply before first move
                if ((best != -CHECKMATE) && (best != -NO_MOVE))
                    oldEval = best;


                // In case of first move cancel we return first move from list beacuse it is from TT
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

            //Save newest best move if possible
            if ((depth > entry.depth) && (!isCancelled || (movesSearched >= 2)))
            {
                pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
            }

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
                //If there was no time cutoff AND we at least searched one move
                //There is one in million chance that time cutoff will appear at the start of first ply before first move
                if ((best != -CHECKMATE) && (best != -NO_MOVE))
                    oldEval = best;


                // In case of first move cancel we return first move from list beacuse it is from TT
                return bestMove;
            }
        }

        //Moves searched without time cutoff
        movesSearched++;


        //If alpha exceeds beta break
        if (alpha >= beta)
        {
            // move causes cutoff
            newFlag = LOWER_BOUND;

            break; // Alpha-beta pruning
        }

        //If we beat best
        //Set it current best
        //And bestmove
        if (value > best)
        {
            best = value;

            bestMove = m;

            //If we beat alpha
            //Make it EXACT_SCORE
            //New possible pv found
            if (best > alpha)
            {
                // new best move found
                newFlag = EXACT_SCORE;

                alpha = best;
            }
        }

        if(ply == 0)
        {
            printMove(m);
            std::cout<<" "<<value<<endl;
            std::cout<<"BEST MOVE: ";
            printMove(bestMove);
            cout<<" "<<best<<endl;
        }

    }

    //Store transposition with flag unless search is being cancelled or save it during cutoff if two or more moves were fully searched
    if ((depth > entry.depth) && (!isCancelled || (movesSearched >= 2)))
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }
    // else
    // {
    //     if(ply == 0)
    //     {
    //         std::cout<<"Not replaced on ply 0!?!?"<<endl;
    //     }
    // }


    //At root ply
    if (ply == 0)
    {
        //If we detect that at least one move was searched at root
        //We can change oldEval to best so we get true evaluation in logs
        //If there was no time cutoff AND we at least searched one move
        //There is one in million chance that time cutoff will appear at the start of first ply before first move
        if ((best != -CHECKMATE) && (best != -NO_MOVE))
            oldEval = best;


        // In case of first move cancel we return first move from list beacuse it is from TT
        return bestMove;
    }

    //Return best value from node up
    return best;
}

Move Search::search(Position &pos, MoveGenerator &mg, Evaluator &eval)
{
    //Initializing starting values

    //Best move we got current search
    Move bestMove = 0;

    //Old evaluation of position
    //It is not updated when time cutoff
    //So we get reasonable value
    oldEval = 0;

    //Timer for time management
    auto start = chrono::steady_clock::now();

    //Currently hard limit
    // timeLimit = 5000;

    //Flag checking if search is cancelled used for
    //Not storing faulty transpositions in TT
    isCancelled = false;

    //Reset node count
    nodesCount = 0;

    //Default starting depth
    int startingDepth = 2;

    if(pos.positionHash == pos.TT.transpositionTable[pos.positionHash % pos.TT.size].zorbistKey)
    {
        startingDepth = pos.TT.transpositionTable[pos.positionHash % pos.TT.size].depth;
    }

    //Iterative deepening loop
    for (int depth = startingDepth; depth <= 100; depth++)
    {
        //Reset node count
        // nodesCount = 0;
        // queiscenceNodes = 0;

        //Start negamax for current depth
        bestMove = negamax(depth, 0, -INF, INF, pos.STM ? 1 : -1, mg, pos, eval, start);
        if (bestMove != 0)
        {
            //We didn't get nullmove we can take that move
            bestMovePrevious = bestMove;
        }

        //Time check and uci info
        auto end = chrono::steady_clock::now();

        long long time = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        std::cout << "info depth " << depth;
        std::cout << " score cp " << oldEval;
        std::cout << " nodes " << nodesCount;
        std::cout << " nps " << (long long)(nodesCount / (time / (double)1000000000));
        std::cout << " time " << (long long)(time/(double)1000000);
        // cout << " quiescence nodes " << queiscenceNodes;
        // cout << " quiescenceTT " << quiescenceTT;

        //
        //          PV PRINTING SEGMENT
        //
        std::cout << " pv ";

        // Move pvl[100];
        // for(int i=1; i <= depth; i++)
        // {
        //     if( pos.TT.transpositionTable[pos.positionHash % pos.TT.size].zorbistKey == pos.positionHash )
        //     {
        //         printMove(pos.TT.transpositionTable[pos.positionHash % pos.TT.size].bestMove);
        //         pvl[i] = pos.TT.transpositionTable[pos.positionHash % pos.TT.size].bestMove;
        //         pos.makeMove(pos.TT.transpositionTable[pos.positionHash % pos.TT.size].bestMove);
        //         cout<<" ";
        //     }
        // }
        // for(int i=depth; i >= 1; i--)
        // {
        //     if( pos.TT.transpositionTable[pos.positionHash % pos.TT.size].zorbistKey == pos.positionHash )
        //     {
        //         pos.undoMove(pvl[i]);
        //     }
        // }


        if(pos.TT.transpositionTable[pos.positionHash % pos.TT.size].zorbistKey == pos.positionHash)
        {
            printMove(pos.TT.transpositionTable[pos.positionHash % pos.TT.size].bestMove);
        }


        // std::cout << moveToUci(bestMovePrevious);
        std::cout << endl;

        //
        //          END OF PV SEGMENT
        //

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
            timeLimit += 3000;
        }
    }

    //After ID loop we print best move to uci
    pos.makeMove(bestMovePrevious);
    std::cout << endl;
    std::cout << "bestmove " << moveToUci(bestMovePrevious) << endl;
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



int Search::quiescence(int depth, int ply, int alpha, int beta, int color,
                       MoveGenerator &moveGenerator, Position &pos,
                       Evaluator &eval, chrono::steady_clock::time_point start)
{
    // Initialize a new flag for the type of bound (Upper Bound by default)
    int newFlag = UPPER_BOUND;

    // Calculate the key for the current position in the transposition table
    Bitboard key = pos.positionHash;

    // Retrieve the transposition table entry for the current position
    TTEntry entry = pos.TT.transpositionTable[key % pos.TT.size];

    // Initialize the transposition move (no move by default)
    Move transpositionMove = 0;

    // Check if the transposition table entry is valid for the current position
    if (key == entry.zorbistKey)
    {
        // If the stored depth is sufficient for this search
        if (entry.depth >= depth)
        {
            transpositionCount++; // Increment counter for transpositions
            transpositionMove = entry.bestMove; // Use the best move from the transposition table

            // If the stored score is usable (exact score or bound-based pruning)
            if ((entry.type == EXACT_SCORE) ||
                ((entry.type == LOWER_BOUND) && (entry.score >= beta)) ||
                ((entry.type == UPPER_BOUND) && (entry.score < alpha)))
            {
                matchedTranspositions++; // Increment counter for matched transpositions
                quiescenceTT++; // Increment counter for quiescence hits in the TT
                return entry.score; // Return the stored score
            }
        }
    }
    else if (entry.zorbistKey != 0)
    {
        // If there's a collision (another position with the same key), increment collision counter
        collisions++;
    }

    // Increment counter for nodes visited in the quiescence search
    queiscenceNodes++;

    // Perform a static evaluation of the position
    int evaluation = eval.evaluate(pos) * color;

    // Beta cutoff: if the evaluation is greater than or equal to beta, return the evaluation
    if (evaluation >= beta)
    {
        return evaluation;
    }
    // Alpha update: if the evaluation is better than alpha, update alpha
    if (evaluation > alpha)
    {
        alpha = evaluation;
    }

    // Generate a list of capture moves (quiescence search considers only captures)
    MoveList moveList;
    moveGenerator.fullCapturesList(pos, moveList);

    // Array to store MVV (Most Valuable Victim) values for sorting
    int MVVs[218];

    // If there's a move from the transposition table, place it at the top of the move list
    if (transpositionMove != 0)
    {
        for (int i = 0; i < moveList.size; ++i)
        {
            if (moveList.moveList[i] == transpositionMove)
            {
                std::swap(moveList.moveList[0], moveList.moveList[i]); // Move TT move to the front
                break;
            }
        }
    }
    else
    {
        // Sort the moves based on MVV (Most Valuable Victim) heuristic
        for (int i = 0; i < moveList.size; ++i)
        {
            int MVV = MVVLVA[pos.piecesArray[StartSquare(moveList.moveList[i])]]
                               [pos.piecesArray[TargetSqaure(moveList.moveList[i])]];
            MVVs[i] = MVV;
        }
        for (int i = 0; i < moveList.size; ++i)
        {
            for (int j = i + 1; j < moveList.size; ++j)
            {
                if (MVVs[j] > MVVs[i])
                {
                    std::swap(moveList.moveList[i], moveList.moveList[j]); // Sort by MVV
                    std::swap(MVVs[i], MVVs[j]);
                }
            }
        }
    }

    // Initialize best move and best score with the evaluation from static evaluation
    Move bestMove = moveList.moveList[0];
    int best = evaluation;

    // Iterate through all moves in the move list
    for (Move m : moveList)
    {
        if (m == 0) // Check for end of move list
            break;

        // Check if the time limit for the search has been reached
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            isCancelled = true; // Mark the search as canceled
            return CHECKMATE;   // Return a checkmate score
        }


        // Make the move on the position
        pos.makeMove(m);

        // Recursively call quiescence search with negated scores and swapped bounds
        int value = -quiescence(depth - 1, ply + 1, -beta, -alpha, -color, moveGenerator, pos, eval, start);

        // Undo the move after evaluation
        pos.undoMove(m);

        // Check if the time limit for the search has been reached again
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit)
        {
            isCancelled = true; // Mark the search as canceled
            return CHECKMATE;   // Return a checkmate score
        }

        // Beta cutoff: if alpha exceeds or equals beta, cut off the search
        if (alpha >= beta)
        {
            newFlag = LOWER_BOUND; // Set flag for a lower bound cutoff
            break; // Exit the loop as further moves are irrelevant
        }

        // If the move yields a better score than the current best score
        if (value > best)
        {
            best = value; // Update the best score
            bestMove = m; // Update the best move

            // Alpha update: if the best score is better than alpha, update alpha
            if (best > alpha)
            {
                alpha = best;
                newFlag = EXACT_SCORE; // Update flag to exact score
            }
        }
    }

    // Store the result in the transposition table if the search depth was sufficient and search wasn't canceled
    if ((depth > entry.depth) && !isCancelled)
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }

    // Return the best score found (alpha)
    return alpha;
}
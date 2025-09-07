#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "TT.h"
#include <chrono>
#include "moveList.h"
#include "helperFunctions.h"
#include "accumulator.h"
#include <fstream>
#include <math.h>
#include <algorithm>




bool isCancelled;

Bitboard queiscenceNodes;
Bitboard quiescenceTT;
Bitboard transpositionCount;
Bitboard nodesCount;
Bitboard matchedTranspositions;
Bitboard collisions;


Move killers[MAX_DEPTH];
Move historyHeuristic[64][64];

scoreType negamax(int depth, int ply, scoreType alpha, scoreType beta, int color, Position &pos, principalVariation& PV, searchParams& params)
{
    //Checks if we are in null window search
    bool isPV = (alpha != beta - 1);

    // This gives info about checkmate and stalemate so it must be the first thing to consider
    MoveList moveList;
    fullMovesList(pos, moveList);

    // Hash of current position
    Bitboard key = pos.positionHash;

    // Lookup of entry
    TTEntry entry = pos.TT.transpositionTable[key % pos.TT.size];

    // Possible transpositon move
    Move transpositionMove = 0;

    // We set it to all node in case it fails low
    int newFlag = UPPER_BOUND;

    //Check extensions
    if(moveList.checks > 0)
    {
        depth++;
    }

    if((nodesCount >= params.nodesLimit) && (params.nodesLimit > 0))
    {
        return CHECKMATE;
    }

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
    if ((depth == 0) || ((ply == params.depthLimit) && (params.depthLimit != 0)))
    {
        return quiescence(depth, ply, alpha, beta, color, pos, PV, params);
    }

    //Possible transposition lookup
    if (key == entry.zorbistKey)
    {
        //We can always take entry move if we match in TT
        //Thats because there should be no fault entries
        //We don't add entries when cancelling search
        transpositionMove = entry.bestMove;

        //If depth from TT is at least same as current we can immediately take score
        if ((entry.depth >= depth) && (ply > 0) && !isPV)
        {
            //Check for node type (matching ab window) (cutoff) (fail low)
            if ((entry.type == EXACT_SCORE) || ((entry.type == LOWER_BOUND) && (entry.score >= beta)) || ((entry.type == UPPER_BOUND) && (entry.score <= alpha)))
            {
                scoreType score = entry.score;
                if(entry.score > (CHECKMATE - 100))
                {
                    score = entry.score - ply;
                } 
                if(entry.score < (-CHECKMATE + 100))
                {
                    score = entry.score + ply;
                }
                return score;
            }
        }
    }


    //MOVE ORDERING

    // Array to store move value for sorting
    int MVVs[218] = {0};


    // If there's a move from the transposition table, place it at the top of the move list

    // Sort the moves based on MVV (Most Valuable Victim) heuristic
    for (int i = 0; i < moveList.size; ++i)
    {
        //TT move
        if (moveList.moveList[i] == transpositionMove)
        {
            // std::swap(moveList.moveList[0], moveList.moveList[i]); // Move TT move to the front
            MVVs[i] = 1000000;
        }

        //MVVLVA
        if (Flags(moveList.moveList[i]) == CAPTURE)
        {
            int MVV = MVVLVA[pos.piecesArray[StartSquare(moveList.moveList[i])]]
                                [pos.piecesArray[TargetSqaure(moveList.moveList[i])]];
            MVVs[i] += MVV*1000;
        }else
        {
            MVVs[i] += historyHeuristic[StartSquare(moveList.moveList[i])][TargetSqaure(moveList.moveList[i])];
        }

        //TT move
        if (moveList.moveList[i] == killers[ply])
        {
            // std::swap(moveList.moveList[0], moveList.moveList[i]); // Move TT move to the front
            MVVs[i] += 90000;
        }
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

    //We init best to nomove so we always take something
    //Because its smaller than alpha
    scoreType best = -NO_MOVE;

    //In case we stop search on first move from 1 ply
    //We take the first move straight away
    //It should be from TT from previous search
    //Best move will be 0 if the size is 0 because list ends with 0
    Move bestMove = moveList.moveList[0];


    //Currently for pvs purpose
    int movesSearched = 0;

    //Loop through all moves
    for (Move m : moveList)
    {
        //If we encounter null move stop
        if (m == 0)
            break;

        //If node isn't terminal create temporaryPV
        principalVariation tempVar;


        //If we encounter hard time limit:
        //1. Set cancel flag so we don't store faulty entries to TT
        //2. Cancel search by a worst possible outcome so we pick it if its first move
        //BUT when we searched at least one other move, this one won't be picked
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - params.start)).count() > params.timeLimit || isCancelled)
        {
            //Cancel flag set
            isCancelled = true;

            //At any ply that isn't from root we just return really bad score
            if (ply >= 1)
            {
                return CHECKMATE;
            }
        }

        int eval = evaluate(pos);

        //NMP
        if(!isPV
        && (moveList.checks == 0)
        && (eval >= beta)
        && (depth > 2)
        && (pos.positionHistory[pos.stateCounter] != (pos.positionHistory[pos.stateCounter-1] ^ pos.zobrist.zobristTable[792]))
        )
        {
            int reducedDepth = max(depth - 4, 0);
            pos.makeNullMove();
            scoreType value = -negamax(reducedDepth, ply + 1, -beta, -beta + 1, -color, pos, tempVar, params);
            pos.undoNullMove();

            if(value >= beta)
            {
                return value;
            }
        }

        //Make move
        pos.makeMove(m);

        int reduction = 0;

        if((Flags(m) < 4) &&
        (movesSearched > 4) &&
        (depth > 4))
        {
            reduction = max(0.0, 1 + log(depth) * log(movesSearched) / 3);
        }

        scoreType value = 0;

        if(movesSearched == 0)
        {
            //Standard alpha beta search with normal window
            value = -negamax(depth - 1, ply + 1, -beta, -alpha, -color, pos, tempVar, params);
        }else
        {
            //If it is not first move we search with null window
            value = -negamax(depth - 1 - reduction, ply + 1, -alpha - 1, -alpha, -color, pos, tempVar, params);

            //If value is inside alpha beta bound we research with full window
            if(value > alpha && value < beta)
            {
                value = -negamax(depth - 1, ply + 1, -beta, -alpha, -color, pos, tempVar, params);
            }
        }

        //Undo move
        pos.undoMove(m);

        //Update moves count
        movesSearched++;

        //If we encounter hard time limit:
        //1. Set cancel flag so we don't store faulty entries to TT
        //2. Cancel search by a worst possible outcome so we pick it if its first move
        //BUT when we searched at least one other move, this one won't be picked
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - params.start)).count() > params.timeLimit || isCancelled)
        {
            //Cancel flag set
            isCancelled = true;

            //At any ply that isn't from root we just return really bad score
            if (ply >= 1)
            {
                return CHECKMATE;
            }
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

                PV.length = 1 + tempVar.length;
                PV.list[0] = bestMove;

                for(int i=0; i < tempVar.length; i++)
                {
                    if(i+1 < MAX_DEPTH)
                    {
                        PV.list[1+i] = tempVar.list[i];
                    }
                }
            }
        }

        //If alpha exceeds beta break
        if (alpha >= beta)
        {
            // move causes cutoff
            newFlag = LOWER_BOUND;

            //If move was quiet
            if(Flags(m) < CAPTURE)
            {
                //Update killers
                killers[ply] = m;

                //Update history heuristic
                historyHeuristic[StartSquare(m)][TargetSqaure(m)] += depth * depth;
            }

            break; // Alpha-beta pruning
        }

    }

    if(!isCancelled)
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }

#ifdef DATAGEN
    // We exclude mating positions
    // Only collect at specific depth
    // The search is not cancelled
    // The first move of sequence is quiet
    // The score of node is exact 
    // We are not in check
    if((best > -90000) && (best < 90000) && (depth >= 5) && !isCancelled && Flags(bestMove) < 4 && newFlag == EXACT_SCORE && moveList.checks == 0 && popCount(pos.piecesBitboards[ALL_PIECES]) > 19)
    {
        // Static evaluation of node should not differ much from search value of node
        float staticEval = evaluate(pos);
        if((abs(staticEval - best) < 100) && (abs(best) > 15))
        {
            savePosition(pos.getState(), best / 100.f, pos);
        }
    }
#endif

    //Return best value from node up
    return best;
}

Move search(Position &pos, searchParams params)
{
    //Initializing starting values

    //Best move we got current search
    Move bestMove = 0;

    //Timer for time management
    auto start = chrono::steady_clock::now();

    params.start = start;

    //Flag checking if search is cancelled used for
    //Not storing faulty transpositions in TT
    isCancelled = false;

    //Reset node count
    nodesCount = 0;

    for(int i=0; i<=63; i++)
    {
        killers[i] = 0;

        for(int j=0; j<=63; j++)
        {
            historyHeuristic[i][j] = 0;
        }
    }

    scoreType alpha = -INF;
    scoreType beta = INF;
    int aspirationWindowMargin = 20 * evalScale;

    Move bestMoveSoFar = 0;
    scoreType searchScore = 0;
    principalVariation PVMain;
    for(int i=0; i < MAX_DEPTH; i++)
    {
        PVMain.list[i] = 0;
    }
    PVMain.length = 0;

    //Iterative deepening loop
    for (int depth = 1; depth < MAX_DEPTH; depth++)
    {
        if(depth > 4)
        {
            alpha = max(-(scoreType)CHECKMATE, searchScore - aspirationWindowMargin);
            beta = min((scoreType)CHECKMATE, searchScore + aspirationWindowMargin);
        }
         
        //Start negamax for current depth
        int color = pos.STM ? 1 : -1;

        while (true) 
        {
            searchScore = negamax(depth, 0, alpha, beta, color, pos, PVMain, params);
            
            if(isCancelled)
            {
                break;
            }

            if(searchScore <= alpha)
            {
                alpha = max(-(scoreType)CHECKMATE, alpha - aspirationWindowMargin);
                continue;
            }

            if(searchScore >= beta)
            {
                beta = min((scoreType)CHECKMATE, beta + aspirationWindowMargin);
                continue;
            }
            break;
        }
        bestMoveSoFar = PVMain.list[0];

        aspirationWindowMargin += 10 * evalScale;

        searchScore = negamax(depth, 0, alpha, beta, color, pos, PVMain, params);

        //Time check and uci info
        auto end = chrono::steady_clock::now();


        //Print only info from full depths
        if((chrono::duration_cast<chrono::milliseconds>(end - start).count() < params.timeLimit) /*&& !isCancelled*/)
        {
            int mili = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            std::cout << "info depth " << depth;
            if(abs(searchScore) > (CHECKMATE - 100))
            {
                if(searchScore > 0)
                {
                    std::cout << " score mate "<< (CHECKMATE - searchScore) / 2 + 1;
                }else
                {
                    std::cout << " score mate -"<< (searchScore + CHECKMATE) / 2 + 1;
                }
            }else
            {
                std::cout << " score cp " << (int)searchScore / evalScale;
            }
            std::cout << " nodes " << nodesCount;
            std::cout << " nps " << (int)(1000 * (nodesCount / (1 + mili)));
            std::cout << " time " << mili;

            //
            //          PV PRINTING SEGMENT
            //
            std::cout << " pv ";

            for(int pvl = 0; pvl < PVMain.length; pvl++)
            {
                //If we see mate score and search deeper the pv will be shorter than depth
                if(PVMain.list[pvl] != 0)
                {
                    printMove(PVMain.list[pvl]);
                    std::cout<<" ";
                }
            }
            std::cout<<endl;
            //
            //          END OF PV SEGMENT
            //

        }

        if ((depth >= params.depthLimit) && (params.depthLimit != 0) || ((nodesCount >= params.nodesLimit) && (params.nodesLimit != 0)))
        {
            break;
        }

        //Time cutoff
        if (chrono::duration_cast<chrono::milliseconds>(end - start).count() > params.timeLimit || isCancelled)
        {
            break;
        }

    }

    //After ID loop we print best move to uci
    std::cout << "info nodes "<<nodesCount<<endl; 
    std::cout << "bestmove " << moveToUci(bestMoveSoFar) << endl;
    
    
    #ifdef DATAGEN2
    saveState(pos);

    int color = pos.STM ? 1 : -1; // Who was on move at mate position
    int label = 0;

    // If mate detected
    if (searchScore > 90000 || searchScore < -90000)
    {
        // Determine winner from eval sign
        if (searchScore > 90000)
            label = 1; // white delivered mate
        else
            label = -1; // black delivered mate

        // Save each collected position with label from STM's perspective
        for (auto& state : pos.datagenPositions)
        {
            savePosition(state, label*color, pos);
        }

        // Clear stored positions
        pos.datagenPositions.clear();
    }
    #endif

    return bestMoveSoFar;
}

//3 fold repetition check
bool isRepeated(Position &pos)
{
    //How many times this position has encountered
    int counter = 0;

    //Simple loop with chcecking position hash in history
    for (int i = 0; i <= pos.stateCounter; i++)
    {
        if (pos.positionHash == pos.positionHistory[i])
        {
            counter++;
            if (counter >= 3)
                return true;
        }
    }
    return false;
}



scoreType quiescence(int depth, int ply, scoreType alpha, scoreType beta, int color,
                     Position &pos, principalVariation& PV, searchParams& params)
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
            transpositionMove = entry.bestMove; // Use the best move from the transposition table

            // If the stored score is usable (exact score or bound-based pruning)
            //Check for node type (matching ab window) (cutoff) (fail low)
            if ((entry.type == EXACT_SCORE) || ((entry.type == LOWER_BOUND) && (entry.score >= beta)) || ((entry.type == UPPER_BOUND) && (entry.score <= alpha)))
            {
                scoreType score = entry.score;
                if(entry.score > (CHECKMATE - 100))
                {
                    score = entry.score - ply;
                } 
                if(entry.score < (-CHECKMATE + 100))
                {
                    score = entry.score + ply;
                }
                return score;
            }
        }
    }

    // Perform a static evaluation of the position
    scoreType evaluation = evaluate(pos);

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
    fullCapturesList(pos, moveList);
        

    //MOVE ORDERING

    // Array to store move value for sorting
    int MVVs[218] = {0};


    // If there's a move from the transposition table, place it at the top of the move list

    // Sort the moves based on MVV (Most Valuable Victim) heuristic
    for (int i = 0; i < moveList.size; ++i)
    {
        //TT move
        if (moveList.moveList[i] == transpositionMove)
        {
            // std::swap(moveList.moveList[0], moveList.moveList[i]); // Move TT move to the front
            MVVs[i] = 1000;
        }

        //MVVLVA
        int MVV = MVVLVA[pos.piecesArray[StartSquare(moveList.moveList[i])]]
                            [pos.piecesArray[TargetSqaure(moveList.moveList[i])]];
        MVVs[i] += MVV;
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

    // Initialize best move and best score with the evaluation from static evaluation
    // If there is no moves the first one will be zero which means end of the list
    Move bestMove = moveList.moveList[0];
    scoreType best = evaluation;

    // Iterate through all moves in the move list
    for (Move m : moveList)
    {
        if (m == 0) // Check for end of move list
            break;

        //If node isn't terminal create temporaryPV
        principalVariation tempVar;

        // Check if the time limit for the search has been reached
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - params.start)).count() > params.timeLimit || isCancelled)
        {
            isCancelled = true; // Mark the search as canceled
            return CHECKMATE;   // Return a checkmate score
        }


        // Make the move on the position
        pos.makeMove(m);

        // Recursively call quiescence search with negated scores and swapped bounds
        scoreType value = -quiescence(depth - 1, ply + 1, -beta, -alpha, -color, pos, tempVar, params);

        // Undo the move after evaluation
        pos.undoMove(m);

        // Check if the time limit for the search has been reached again
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - params.start)).count() > params.timeLimit || isCancelled)
        {
            isCancelled = true; // Mark the search as canceled
            return CHECKMATE;   // Return a checkmate score
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

                // PV.length = 1 + tempVar.length;
                // PV.list[0] = bestMove;

                // for(int i=0; i < tempVar.length; i++)
                // {
                    // if(i+1 < MAX_DEPTH)
                    // {
                        // PV.list[1+i] = tempVar.list[i];
                    // }
                // }
            }
        }

        // Beta cutoff: if alpha exceeds or equals beta, cut off the search
        if (alpha >= beta)
        {
            newFlag = LOWER_BOUND; // Set flag for a lower bound cutoff
            break; // Exit the loop as further moves are irrelevant
        }
    }

    // Store the result in the transposition table if the search depth was sufficient and search wasn't canceled
    if ((depth >= entry.depth) && !isCancelled)
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }

    // Return the best score found
    return best;
}

void saveState(Position &pos)
{
    pos.datagenPositions.push_back(pos.getState());
}

void savePosition(std::array<int, INPUT_SIZE> state, float target, Position& pos) 
{
    // Open file in binary mode
    std::ofstream file(pos.datagenFile, std::ios::binary | std::ios::app);

    if (!file) {
        std::cerr << "Nie można otworzyć pliku do zapisu: " << pos.datagenFile << std::endl;
        return;
    }

    // Create a buffer to store packed bits
    uint8_t buffer = 0;
    int bitCount = 0;

    // Pack 0/1 values from state[] into bytes
    for (int i = 0; i < INPUT_SIZE; ++i) {
        buffer <<= 1;  // Shift bits left by 1
        buffer |= state[i];  // Add the current state bit (0 or 1)
        bitCount++;

        // If buffer is full (8 bits), write it to the file
        if (bitCount == 8) {
            file.write(reinterpret_cast<const char*>(&buffer), sizeof(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }

    // Write remaining bits (if any)
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);  // Align bits to the left
        file.write(reinterpret_cast<const char*>(&buffer), sizeof(buffer));
    }

    // Write the target as a 32-bit float (multiplied by 100, assuming it's an int divided by 100)
    float target_f = target;
    file.write(reinterpret_cast<const char*>(&target_f), sizeof(target_f));

    // Close the file
    file.close();
}
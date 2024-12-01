#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "TT.h"
#include <chrono>
#include "moveList.h"
#include "helperFunctions.h"
#include "accumulator.h"
#include <fstream>






bool isCancelled;

Bitboard queiscenceNodes;
Bitboard quiescenceTT;
Bitboard transpositionCount;
Bitboard nodesCount;
Bitboard matchedTranspositions;
Bitboard collisions;

Move bestMovePrevious;
int oldEval;

Move killers[MAX_DEPTH];
Move historyHeuristic[MAX_DEPTH][MAX_DEPTH];

int negamax(int depth, int ply, int alpha, int beta, int color, Position &pos, principalVariation& PV, searchParams& params)
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

        #ifndef DATAGEN
        //If depth from TT is at least same as current we can immediately take score
        if ((entry.depth >= depth) && (ply > 0) && !isPV)
        {
            //Check for node type (matching ab window) (cutoff) (fail low)
            if ((entry.type == EXACT_SCORE) || ((entry.type == LOWER_BOUND) && (entry.score >= beta)) || ((entry.type == UPPER_BOUND) && (entry.score < alpha)))
            {
                return entry.score;
            }
        }
        #endif
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
    int best = -NO_MOVE;

    //In case we stop search on first move from 1 ply
    //We take the first move straight away
    //It should be from TT from previous search
    //Best move will be 0 if the size is 0 because list ends with 0
    Move bestMove = moveList.moveList[0];


    //Currently for pvs purpose
    int movesSearched = 0;

    //Check extensions
    if(moveList.checks > 0)
    {
        depth++;
    }

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

        //Make move
        pos.makeMove(m);


        int value = 0;

        if(movesSearched == 0)
        {
            //Standard alpha beta search with normal window
            value = -negamax(depth - 1, ply + 1, -beta, -alpha, -color, pos, tempVar, params);
        }else
        {
            //If it is not first move we search with null window
            value = -negamax(depth - 1, ply + 1, -alpha - 1, -alpha, -color, pos, tempVar, params);

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

    //Store transposition with flag unless search is being cancelled or save it during cutoff if two or more moves were fully searched
    if ((depth >= entry.depth) && (!isCancelled))
    {
        pos.TT.transpositionTable[key % pos.TT.size] = TTEntry(best, depth, bestMove, newFlag, key);
    }

#ifdef DATAGEN
    if((best > -90000) && (best < 90000) && popCount(pos.piecesBitboards[ALL_PIECES]) >= 6 && depth >= 5 && !isCancelled)
        savePosition(pos, best);
#endif


    //At root ply
    if (ply == 0)
    {
        //Update move eval
        if ((best != -CHECKMATE) && (best != -NO_MOVE))
            oldEval = best;

        // In case of first move cancel we return first move from list beacuse it is from TT
        // 0 means take the move from old ID iteration
        if(isCancelled)
        {
            return 0;
        }else
        {
            return bestMove;
        }

    }

    //Return best value from node up
    return best;
}

Move search(Position &pos, searchParams params)
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

    //Iterative deepening loop
    for (int depth = 1; depth < MAX_DEPTH; depth++)
    {
        principalVariation PVMain;
        for(int i=0; i < MAX_DEPTH; i++)
        {
            PVMain.list[i] = 0;
        }
        PVMain.length = 0;
        //Start negamax for current depth
        bestMove = negamax(depth, 0, -INF, INF, pos.STM ? 1 : -1, pos, PVMain, params);
        if (bestMove != 0)
        {
            //We didn't get nullmove we can take that move
            bestMovePrevious = bestMove;
        }

        //Time check and uci info
        auto end = chrono::steady_clock::now();


        //Print only info from full depths
        if((chrono::duration_cast<chrono::milliseconds>(end - start).count() < params.timeLimit) && !isCancelled)
        {
            int mili = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            std::cout << "info depth " << depth;
            std::cout << " score cp " << oldEval;
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
            // std::cout<<" pvb "<<PVMain.length;
            // clearPV(depth);
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



#ifdef DATAGEN
    // if(oldEval > -90000 && oldEval < 90000 && Flags(bestMovePrevious) != CAPTURE && popCount(pos.piecesBitboards[ALL_PIECES]) >= 6)
    //     savePosition(pos, oldEval);
#endif

    //After ID loop we print best move to uci
    std::cout << "bestmove " << moveToUci(bestMovePrevious) << endl;
    return bestMovePrevious;
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



int quiescence(int depth, int ply, int alpha, int beta, int color,
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
            if ((entry.type == EXACT_SCORE) ||
                ((entry.type == LOWER_BOUND) && (entry.score >= beta)) ||
                ((entry.type == UPPER_BOUND) && (entry.score < alpha)))
            {
                return entry.score; // Return the stored score
            }
        }
    }

    // Perform a static evaluation of the position
    int evaluation = evaluate(pos);

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
    int best = evaluation;

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
        int value = -quiescence(depth - 1, ply + 1, -beta, -alpha, -color, pos, tempVar, params);

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

                PV.length = 1 + tempVar.length;
                PV.list[0] = bestMove;
                // if(PV.length >= 9)
                // {
                //     cout<<"STOP";
                // }

                for(int i=0; i < tempVar.length; i++)
                {
                    if(i+1 < MAX_DEPTH)
                    {
                        PV.list[1+i] = tempVar.list[i];
                    }
                }
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

void savePosition(Position &pos, int target) {

    int state[768] = {0};

    if(pos.STM)
    {
        for(int i=0; i<=63; i++)
        {
            int index = 0;
            if(pos.piecesArray[i] <= BLACK_KING)
            {
                index = 64*pos.piecesArray[i]+i;
                state[index] = 1;
            }
        }
    }else
    {
        for(int i=0; i<=63; i++)
        {
            int index = 0;
            if(pos.piecesArray[i] <= BLACK_KING)
            {
                if(pos.piecesArray[i] <= WHITE_KING)
                {
                    index = 64*(pos.piecesArray[i]+6)+flipIndex(i);
                }
                else if(pos.piecesArray[i] <= BLACK_KING)
                {
                    index = 64*(pos.piecesArray[i]-6)+flipIndex(i);
                }
                state[index] = 1;
            }
        }
    }


    // Open file in binary mode
    std::ofstream file("binary_data.bin", std::ios::binary | std::ios::app);

    if (!file) {
        std::cerr << "Nie można otworzyć pliku do zapisu: " << "binary_data.bin" << std::endl;
        return;
    }

    // Create a buffer to store packed bits
    uint8_t buffer = 0;
    int bitCount = 0;

    // Pack 0/1 values from state[] into bytes
    for (int i = 0; i < 768; ++i) {
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
    float target_f = target / 100.0f;
    file.write(reinterpret_cast<const char*>(&target_f), sizeof(target_f));

    // Close the file
    file.close();
}
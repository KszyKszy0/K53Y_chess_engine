#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "TT.h"
#include <chrono>
#include "moveList.h"
#include "helperFunctions.h"
#include "accumulator.h"
#include <fstream>




int timeLimit;
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

int pvLength[MAX_DEPTH];
Move pvTable[MAX_DEPTH][MAX_DEPTH];

int negamax(int depth, int ply, int alpha, int beta, int color, Position &pos, chrono::steady_clock::time_point start)
{
    pvLength[ply] = ply;

    //Checks if we are in null window search
    bool PV = (alpha != beta - 1);

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
        return quiescence(depth, ply, alpha, beta, color, pos, start);
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
        if ((entry.depth >= depth) && (ply > 0) && !PV)
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

        //If we encounter hard time limit:
        //1. Set cancel flag so we don't store faulty entries to TT
        //2. Cancel search by a worst possible outcome so we pick it if its first move
        //BUT when we searched at least one other move, this one won't be picked
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit || isCancelled)
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
            value = -negamax(depth - 1, ply + 1, -beta, -alpha, -color, pos, start);
        }else
        {
            //If it is not first move we search with null window
            value = -negamax(depth - 1, ply + 1, -alpha - 1, -alpha, -color, pos, start);

            //If value is inside alpha beta bound we research with full window
            if(value > alpha && value < beta)
            {
                value = -negamax(depth - 1, ply + 1, -beta, -alpha, -color, pos, start);
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
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit || isCancelled)
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

                updatePV(bestMove, ply);
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
    if(best > -90000 && best < 90000 && Flags(bestMovePrevious) != CAPTURE && popCount(pos.piecesBitboards[ALL_PIECES]) >= 6 && depth >= 4 && !isCancelled && moveList.checks == 0)
        savePosition(pos, best);
#endif


    //At root ply
    if (ply == 0)
    {
        //Update move eval
        if ((best != -CHECKMATE) && (best != -NO_MOVE))
            oldEval = best;

        // In case of first move cancel we return first move from list beacuse it is from TT
        return bestMove;
    }

    //Return best value from node up
    return best;
}

Move search(Position &pos)
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

        //Start negamax for current depth
        bestMove = negamax(depth, 0, -INF, INF, pos.STM ? 1 : -1, pos, start);
        if (bestMove != 0)
        {
            //We didn't get nullmove we can take that move
            bestMovePrevious = bestMove;
        }

        //Time check and uci info
        auto end = chrono::steady_clock::now();


        //Print only info from full depths
        if(chrono::duration_cast<chrono::milliseconds>(end - start).count() < timeLimit && !isCancelled)
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

            for(int pvl = 0; pvl < depth; pvl++)
            {
                printMove(pvTable[0][pvl]);
                std::cout<<" ";
            }

            std::cout<<endl;
            //
            //          END OF PV SEGMENT
            //
        }

        //Time cutoff
        if (chrono::duration_cast<chrono::milliseconds>(end - start).count() > timeLimit || isCancelled)
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
            if (counter >= 2)
                return true;
        }
    }
    return false;
}



int quiescence(int depth, int ply, int alpha, int beta, int color,
                     Position &pos, chrono::steady_clock::time_point start)
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

        // Check if the time limit for the search has been reached
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit || isCancelled)
        {
            isCancelled = true; // Mark the search as canceled
            return CHECKMATE;   // Return a checkmate score
        }


        // Make the move on the position
        pos.makeMove(m);

        // Recursively call quiescence search with negated scores and swapped bounds
        int value = -quiescence(depth - 1, ply + 1, -beta, -alpha, -color, pos, start);

        // Undo the move after evaluation
        pos.undoMove(m);

        // Check if the time limit for the search has been reached again
        if ((chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start)).count() > timeLimit || isCancelled)
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

void updatePV(Move m, int ply)
{
    pvTable[ply][ply] = m;

    for(int nextPly = ply + 1; nextPly < pvLength[ply + 1]; nextPly++)
    {
        pvTable[ply][nextPly] = pvTable[ply + 1][nextPly];
    }

    pvLength[ply] = pvLength[ply + 1];
}



// void savePosition(Position &pos, int negamaxScore)
// {
//     //TreeStrap


//     //Create State before making moves
//     //384 psqt + 5 material inputs
//     // int state[389] = {0};


//     // for(int i=0; i<=63; i++)
//     // {
//     //     //If square is empty go to the next square
//     //     if(pos.piecesArray[i] == NO_PIECE)
//     //     {
//     //         continue;
//     //     }

//     //     //If the piece is white
//     //     if(pos.piecesArray[i] <= WHITE_KING)
//     //     {
//     //         //Add current position to state index
//     //         //Flip index to get correct psqt value
//     //         int startIndex = pos.piecesArray[i]*64;

//     //         int bonusIndex = flipIndex(i);
//     //         state[startIndex + bonusIndex] += 1;
//     //     }else //piece is black
//     //     {
//     //         //Add current position to state index but with negative sign
//     //         //Black pieces get -6 because of indexing enums
//     //         state[(pos.piecesArray[i]-6) * 64 + i] -= 1;
//     //     }
//     // }

//     // //Pawn count
//     // state[384] = popCount(pos.piecesBitboards[WHITE_PAWN]) - popCount(pos.piecesBitboards[BLACK_PAWN]);

//     // //Knight count
//     // state[385] = popCount(pos.piecesBitboards[WHITE_KNIGHT]) - popCount(pos.piecesBitboards[BLACK_KNIGHT]);

//     // //Bishop count
//     // state[386] = popCount(pos.piecesBitboards[WHITE_BISHOP]) - popCount(pos.piecesBitboards[BLACK_BISHOP]);

//     // //Rook count
//     // state[387] = popCount(pos.piecesBitboards[WHITE_ROOK]) - popCount(pos.piecesBitboards[BLACK_ROOK]);

//     // //Queen count
//     // state[388] = popCount(pos.piecesBitboards[WHITE_QUEEN]) - popCount(pos.piecesBitboards[BLACK_QUEEN]);


//     int state[768] = {0};

//     if(pos.STM)
//     {
//         for(int i=0; i<=63; i++)
//         {
//             int index = 0;
//             if(pos.piecesArray[i] <= BLACK_KING)
//             {
//                 index = 64*pos.piecesArray[i]+i;
//                 state[index] = 1;
//             }
//         }
//     }else
//     {
//         for(int i=0; i<=63; i++)
//         {
//             int index = 0;
//             if(pos.piecesArray[i] <= BLACK_KING)
//             {
//                 if(pos.piecesArray[i] <= WHITE_KING)
//                 {
//                     index = 64*(pos.piecesArray[i]+6)+flipIndex(i);
//                 }
//                 else if(pos.piecesArray[i] <= BLACK_KING)
//                 {
//                     index = 64*(pos.piecesArray[i]-6)+flipIndex(i);
//                 }
//                 state[index] = 1;
//             }
//         }
//     }


//     //Get static evaluation
//     int evaluation = evaluate(pos);

//     //Open file
//     std::fstream file("test.txt", ios::app);

//     if (!file) {
//         std::cerr << "Nie można otworzyć pliku do zapisu: " << "test.txt" << std::endl;
//     }

//     //Write state to file
//     for (int i = 0; i < 768; ++i) {
//         file << state[i];
//         file << " ";
//     }

//     //Get target best if node doesn't fail low or high
//     //If we don't fail on either bound we take the error by difference with minimax value of node
//     int target = negamaxScore;

//     //If we are below alpha that means we underestimate position
//     // if(alpha > evaluation)
//     // {
//     //     target = alpha;
//     // }

//     // //If we are above beta that means we overestimate position
//     // if(beta < evaluation)
//     // {
//     //     target = beta;
//     // }

//     // file << " " << evaluation << " " << target;
//     file << target / float(100);
//     // file << " " <<pos.getFEN();


//     file << endl;

//     // Close file
//     file.close();
// }




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
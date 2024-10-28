#include "core.h"
#include "position.h"
#include "accumulator.h"
#include "nnue.h"
#include "fstream"


float L1_weights[l1_size][inputSize];
float L1_bias[l1_size];
float L2_weights[l2_size][l1_size];
float L2_bias[l2_size];
float output_weights[l2_size];
float output_bias;

// Highly unoptimized function TODO!!!
inline Move uciToMove(const std::string &uci, Position &pos, Move *moveList)
{
    // Prepare move info
    int startSquare = nameToSquare(uci.substr(0, 2));
    int targetSquare = nameToSquare(uci.substr(2, 2));
    int flags = QUIET;

    // Default move
    Move targetMove = 0;

    // Check for promotion flags
    if (uci.length() == 5)
    {
        switch (uci[4])
        {
        case 'n':
            flags = KNIGHT_PROMOTION;
            break;
        case 'b':
            flags = BISHOP_PROMOTION;
            break;
        case 'r':
            flags = ROOK_PROMOTION;
            break;
        case 'q':
            flags = QUEEN_PROMOTION;
            break;
        }
        if (pos.piecesArray[targetSquare] != NO_PIECE)
        {
            flags += 4;
        }
    }
    else
    {
        // Loop through moveList to check if moves start and target match
        // If so we make to move because only one move in position can have same start and destination
        Move *temp = moveList;
        while (*temp != 0)
        {
            int mStartSquare = *temp & 0x3F;
            int mtargetSquare = (*temp >> 6) & 0x3F;
            if ((mStartSquare == startSquare) && (mtargetSquare == targetSquare))
            {
                return *temp;
            }
            *temp++;
        }
    }

    // Return move using start, target and flags
    return createMove(startSquare, targetSquare, flags);
}

core::core()
{
    magicInit();
    readNNUE();
    newGame();
    // int state[768] = {0};

    // MoveList moveList;
    // fullMovesList(pos, moveList);
    // Move t = uciToMove("d4d5",pos,moveList.begin());
    // pos.makeMove(t);
    // pos.undoMove(t);
    // pos.makeMove(t);
    // pos.undoMove(t);
    // if(pos.STM)
    // {
    //     for(int i=0; i<=63; i++)
    //     {
    //         int index = 0;
    //         if(pos.piecesArray[i] <= BLACK_KING)
    //         {
    //             index = 64*pos.piecesArray[i]+i;
    //             state[index] = 1;
    //         }
    //     }
    // }else
    // {
    //     for(int i=0; i<=63; i++)
    //     {
    //         int index = 0;
    //         if(pos.piecesArray[i] <= BLACK_KING)
    //         {
    //             if(pos.piecesArray[i] <= WHITE_KING)
    //             {
    //                 index = 64*(pos.piecesArray[i]+6)+flipIndex(i);
    //             }
    //             else if(pos.piecesArray[i] <= BLACK_KING)
    //             {
    //                 index = 64*(pos.piecesArray[i]-6)+flipIndex(i);
    //             }
    //             state[index] = 1;
    //         }
    //     }
    // }
    // for(int i=0; i < 768; i++)
    // {
    //     cout<<state[i]<<", ";
    // }
    // int counter = 0;
    // for(int i=0; i < 768; i++)
    // {
    //     cout<<state[i]<<", ";
    //     counter++;
    //     if(counter % 8 == 0)
    //     {
    //         cout<<endl;
    //     }
    //     if(counter % 64 == 0)
    //     {
    //         cout<<endl;
    //     }
    // }

    cout<<evaluate(pos);
}



Bitboard core::perft(int depth)
{
    Bitboard counter=0;
    if(depth==0)
    {
        return 1;
    }

    MoveList moves;
    fullMovesList(pos, moves);

    if(depth == 1)
    {
        // int count = 0;
        // for(Move m : moves.moveList)
        // {
        //     if(m == 0)
        //         return count;
        //     count++;
        // }
        // for(int i=0; i<=moves.size; i++)
        // {
        //     printMove(moves.moveList[i]);
        //     cout<<endl;
        // }
        return moves.size;
    }

    // for(int i=0; i<=moves.size-1; i++)
    // {
    //     int localCounter = 0;

    //     pos.makeMove(moves.moveList[i]);

    //     localCounter += perft(depth-1);
    //     counter += localCounter;
    //     pos.undoMove(moves.moveList[i]);

    //     if(depth==6)
    //     {
    //         printMove(moves.moveList[i]);
    //         cout<<": "<<localCounter<<endl;
    //     }
    // }
    for(Move m : moves)
    {
        if(m == 0)
            return counter;
        // printMove(m);
        Bitboard localCounter = 0;

        bool side = pos.STM;
        Bitboard hash = pos.positionHash;
        pos.makeMove(m);

        localCounter += perft(depth-1);
        counter += localCounter;
        pos.undoMove(m);
        if(hash != pos.positionHash)
        {
            cout<<"error";
        }
        if(pos.STM != side)
        {
            cout<<"error";
        }

        if(depth==7)
        {
            printMove(m);
            cout<<": "<<localCounter<<endl;
        }
    }
    return counter;
}

void core::uci() {
    std::cout << "id name K53Y Chess Engine" << std::endl;
    std::cout << "id author KszyKszy" << std::endl;
    std::cout << "uciok" << std::endl;
}

void core::isReady() {
    std::cout << "readyok" << std::endl;
}

void core::newGame(){
    pos.TT.ResetTT();
    pos.parseFEN(startingFen);
    positionCounter = 0;
    // pos.accum.resetAccum();
    // pos.accum.initAccum(pos.piecesArray);
}

void core::newGame(string FEN){
    pos.TT.ResetTT();
    pos.parseFEN(FEN);
    positionCounter = 0;
    // pos.accum.resetAccum();
    // pos.accum.initAccum(pos.piecesArray);
}

void core::setPosition(vector<string>& moves)
{
    pos.parseFEN(startingFen);
    for(string s : moves)
    {
        MoveList moveList;
        fullMovesList(pos, moveList);
        pos.makeMove(uciToMove(s,pos,moveList.begin()));
    }
    // positionCounter = moves.size();
    // if(moves.size() > 0)
    // {
    //     string move = moves[positionCounter-1];
    //     MoveList moveList;
    //     moveGenerator.fullMovesList(pos, moveList);
    //     pos.makeMove(uciToMove(move,pos,moveList.moveList));
    // }
    // std::cout<<endl;
    // std::cout<<pos.getFEN()<<endl;
}

void core::setPosition(vector<string>& moves, string FEN)
{
    pos.parseFEN(FEN);
    for(string s : moves)
    {
        MoveList moveList;
        fullMovesList(pos, moveList);
        pos.makeMove(uciToMove(s,pos,moveList.begin()));
    }
    // positionCounter = moves.size();
    // if(moves.size() > 0)
    // {
    //     string move = moves[positionCounter-1];
    //     MoveList moveList;
    //     moveGenerator.fullMovesList(pos, moveList);
    //     pos.makeMove(uciToMove(move,pos,moveList.moveList));
    // }
    // std::cout<<endl;
    // std::cout<<pos.getFEN()<<endl;
}

void core::quit()
{
    isCancelled = true;
    exit(0);
}

void core::stop()
{
    isCancelled = true;
}

void core::go()
{
    // negamax(5, 0, -100000, 100000,pos.STM ? 1 : -1, moveGenerator, pos, eval);
    search(pos);
}

void core::setTime(int wTime, int bTime)
{
    if(pos.STM)
    {
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 28 )
        {
            timeLimit = wTime / (float)60;
            return;
        }
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 16 )
        {
            timeLimit = wTime / (float)40;
            return;
        }
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 4 )
        {
            timeLimit = wTime / (float)20;
            return;
        }
        timeLimit = wTime / (float)15;
        return;
    }else
    {
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 28 )
        {
            timeLimit = bTime / (float)60;
            return;
        }
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 16 )
        {
            timeLimit = bTime / (float)40;
            return;
        }
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 4 )
        {
            timeLimit = bTime / (float)20;
            return;
        }
        timeLimit = bTime / (float)15;
        return;
    }
}

void core::readNNUE()
{
    std::fstream file("NNUE.txt");
    double value;
    for(int i=0; i < (l1_size / 2); i++)
    {
        for(int j=0; j < inputSize; j++)
        {
            file >> value;
            L1_weights[i][j] = value;
        }
    }
    for(int i=0; i < l1_size; i++)
    {
        file >> value;
        L1_bias[i] = value;
    }

    for(int i=0; i < 16; i++)
    {
        for(int j=0; j < 32; j++)
        {
            file >> value;
            L2_weights[i][j] = value;
        }
    }
    for(int i=0; i < l2_size; i++)
    {
        file >> value;
        L2_bias[i] = value;
    }

    for(int i=0; i < l2_size; i++)
    {
        file >> value;
        output_weights[i] = value;
    }


    file >> value;
    output_bias = value;
    file.close();

    // for(int i=0; i < 32; i++)
    // {
    //     for(int j=0; j < 768; j++)
    //     {
    //         cout<<L1_weights[j][i]<<" ";
    //     }
    //     cout<<endl;
    // }
    // for(int i=0; i < 32; i++)
    // {
    //     cout<<L1_bias[i]<<" ";
    // }
    // cout<<endl;
    // for(int i=0; i < 32; i++)
    // {
    //     cout<<L2_weights[i]<<" ";
    // }
    // cout<<endl;
    // cout<<output_bias;
    // cout<<endl;
}
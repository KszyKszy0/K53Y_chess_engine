#include "core.h"
#include "position.h"
#include "accumulator.h"
#include "nnue.h"


int L1_weights[inputSize][l1_size];
int L1_bias[l1_size] = {0};
int L2_weights[l1_size];
int output_bias = 0;

core::core()
{
    magicInit();

    for(int i=0; i < 768; i++)
    {
        for(int j=0; j < 32; j++)
        {
            int value = i / 64;
            int points;
            if(value == 0)
                points = 1;
            if(value == 1)
                points = 3;
            if(value == 2)
                points = 3;
            if(value == 3)
                points = 5;
            if(value == 4)
                points = 9;
            if(value == 5)
                points = 0;

            if(value == 6)
                points = -1;
            if(value == 7)
                points = -3;
            if(value == 8)
                points = -3;
            if(value == 9)
                points = -5;
            if(value == 10)
                points = -9;
            if(value == 11)
                points = 0;

            L1_weights[i][j] = points;
            L2_weights[j] = 1;
        }
    }

    int lineCounter = 0;
    int wholeCounter = 0;
    for(int i=0; i < 768; i++)
    {
        int sum = 0;
        for(int j=0; j < 32; j++)
        {
            sum += L1_weights[i][j];
        }
        sum /= 32;
        cout<<sum<<" ";
        lineCounter++;
        if(lineCounter >= 8)
        {
            cout<<endl;
            lineCounter=0;
        }
        wholeCounter++;
        if(wholeCounter >= 64)
        {
            cout<<endl;
            wholeCounter=0;
        }
    }
    newGame();
    cout<<evaluate(pos);
}

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
    pos.accum.resetAccum();
    pos.accum.initAccum(pos.piecesArray);
}

void core::newGame(string FEN){
    pos.TT.ResetTT();
    pos.parseFEN(FEN);
    positionCounter = 0;
    pos.accum.resetAccum();
    pos.accum.initAccum(pos.piecesArray);
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
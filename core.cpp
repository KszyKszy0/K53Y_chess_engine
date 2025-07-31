#include "core.h"
#include "position.h"
#include "accumulator.h"
#include "nnue.h"
#include "fstream"
#include <stdfloat>

#ifdef INT16
alignas(32)int16_t L1_weights[INPUT_SIZE][L1_SIZE / 2];
alignas(32) int16_t L1_bias[L1_SIZE / 2];
alignas(32) int16_t L2_weights[L2_SIZE][L1_SIZE];
alignas(32) int16_t L2_bias[L2_SIZE];
alignas(32) int16_t output_weights[L2_SIZE];
alignas(32) int16_t output_bias;
#endif

#ifdef FLOAT
alignas(32) float L1_weights[INPUT_SIZE][L1_SIZE / 2];
alignas(32) float L1_bias[L1_SIZE / 2];
alignas(32) float L2_weights[L2_SIZE][L1_SIZE];
alignas(32) float L2_bias[L2_SIZE];
alignas(32) float output_weights[L2_SIZE];
alignas(32) float output_bias;
#endif

#ifdef FLOAT16
alignas(32) float L1_weights[INPUT_SIZE][L1_SIZE / 2];
alignas(32) float L1_bias[L1_SIZE / 2];
alignas(64) bfloat16_t L2_weights[L2_SIZE][L1_SIZE];
alignas(32) float L2_bias[L2_SIZE];
alignas(32) float output_weights[L2_SIZE];
alignas(32) float output_bias;
#endif

core::core()
{
    magicInit();
    readNNUE();
    newGame();
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
        return moves.size;
    }


    for(Move m : moves)
    {
        if(m == 0)
            return counter;
        Bitboard localCounter = 0;

        bool side = pos.STM;
        Bitboard hash = pos.positionHash;

        pos.makeMove(m);

        localCounter += perft(depth-1);
        counter += localCounter;

        pos.undoMove(m);
    }
    return counter;
}

void core::goPerft(int depth)
{
    MoveList moves;
    fullMovesList(pos, moves);
    Bitboard fullCounter = 0;
    for(Move m : moves)
    {
        pos.makeMove(m);
        Bitboard result = perft(depth - 1);
        fullCounter += result;
        pos.undoMove(m);
        printMove(m);
        cout<<": "<<result<<endl;
    }
    std::cout<<"Nodes searched: "<<fullCounter<<endl;
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
}

void core::newGame(string FEN){
    pos.TT.ResetTT();
    pos.parseFEN(FEN);
    positionCounter = 0;
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
    // cout<<pos.getFEN();
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

void core::go(searchParams params)
{
    search(pos, params);
}

void core::setTime(int wTime, int bTime, int wInc, int bInc, int moveTime, searchParams& params)
{
    if(moveTime != 0)
    {
        params.timeLimit = moveTime;
        return;
    }
    if(pos.STM)
    {
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 28 )
        {
            params.timeLimit = wTime / (float)60;
            return;
        }
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 16 )
        {
            params.timeLimit = wTime / (float)40;
            return;
        }
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 4 )
        {
            params.timeLimit = wTime / (float)20;
            return;
        }
        params.increment = wInc;
        params.timeLimit = wTime / (float)15;
        return;
    }else
    {
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 28 )
        {
            params.timeLimit = bTime / (float)60;
            return;
        }
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 16 )
        {
            params.timeLimit = bTime / (float)40;
            return;
        }
        if( popCount(pos.piecesBitboards[ALL_PIECES]) >= 4 )
        {
            params.timeLimit = bTime / (float)20;
            return;
        }
        params.increment = bInc;
        params.timeLimit = bTime / (float)15;
        return;
    }
}

void core::readNNUE()
{
    std::fstream file("NNUE.txt");
    
    #ifdef INT16
    int value;
    for(int i=0; i < (L1_SIZE / 2); i++)
    {
        for(int j=0; j < INPUT_SIZE; j++)
        {
            file >> value;
            L1_weights[j][i] = static_cast<int8_t>(value);
        }
    }
    for(int i=0; i < (L1_SIZE / 2); i++)
    {
        file >> value;
        L1_bias[i] = static_cast<int8_t>(value);
    }

    for(int i=0; i < L2_SIZE; i++)
    {
        for(int j=0; j < L1_SIZE; j++)
        {
            file >> value;
            L2_weights[i][j] = static_cast<int8_t>(value);
        }
    }
    for(int i=0; i < L2_SIZE; i++)
    {
        file >> value;
        L2_bias[i] = static_cast<int8_t>(value);
    }

    for(int i=0; i < L2_SIZE; i++)
    {
        file >> value;
        output_weights[i] = static_cast<int8_t>(value);
    }

    file >> value;
    output_bias = static_cast<int8_t>(value);
    #endif

    #ifdef FLOAT
    float value;
    for(int i=0; i < (L1_SIZE / 2); i++)
    {
        for(int j=0; j < INPUT_SIZE; j++)
        {
            file >> value;
            L1_weights[j][i] = static_cast<float>(value);
        }
    }
    for(int i=0; i < (L1_SIZE / 2); i++)
    {
        file >> value;
        L1_bias[i] = static_cast<float>(value);
    }

    for(int i=0; i < L2_SIZE; i++)
    {
        for(int j=0; j < L1_SIZE; j++)
        {
            file >> value;
            L2_weights[i][j] = static_cast<float>(value);
        }
    }
    for(int i=0; i < L2_SIZE; i++)
    {
        file >> value;
        L2_bias[i] = static_cast<float>(value);
    }

    for(int i=0; i < L2_SIZE; i++)
    {
        file >> value;
        output_weights[i] = static_cast<float>(value);
    }

    file >> value;
    output_bias = static_cast<float>(value);
    #endif

    #ifdef FLOAT16
    float value;
    for(int i=0; i < (L1_SIZE / 2); i++)
    {
        for(int j=0; j < INPUT_SIZE; j++)
        {
            file >> value;
            L1_weights[j][i] = float(value);
        }
    }
    for(int i=0; i < (L1_SIZE / 2); i++)
    {
        file >> value;
        L1_bias[i] = float(value);
    }

    for(int i=0; i < L2_SIZE; i++)
    {
        for(int j=0; j < L1_SIZE; j++)
        {
            file >> value;
            L2_weights[i][j] = bfloat16_t(value);
        }
    }
    for(int i=0; i < L2_SIZE; i++)
    {
        file >> value;
        L2_bias[i] = bfloat16_t(value);
    }

    for(int i=0; i < L2_SIZE; i++)
    {
        file >> value;
        output_weights[i] = float(value);
    }

    file >> value;
    output_bias = float(value);
    #endif

    file.close();
}

void core::printState()
{
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

    int counter = 0;
    for(int i=0; i < 768; i++)
    {
        cout<<state[i]<<", ";
        counter++;
        if(counter % 8 == 0)
        {
            cout<<endl;
        }
        if(counter % 64 == 0)
        {
            cout<<endl;
        }
    }

    cout<<"[";
    for(int i=0; i < 768; i++)
    {
        cout<<state[i];
        if(i < 767)
        {
            cout<<", ";
        }
    }
    cout<<"]";
    cout<<endl;
}

void core::eval()
{
    pos.printBoard();
    cout<<pos.getFEN()<<endl;
    cout<<"eval: "<<evaluate(pos)<<endl;
}

void core::state()
{
    printState();
}

void core::fen()
{
    cout<<pos.getFEN()<<endl;
}

void core::readWeights()
{
    #if defined(INT16) || defined(FLOAT) || defined(FLOAT16)
    readNNUE();
    cout<<"info bias "<<output_bias<<endl;
    #endif
}
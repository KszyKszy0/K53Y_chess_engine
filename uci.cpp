#include "uci.h"
#include "core.h"
#include <sstream>
#include <thread>

// Split splits given string `s` to tokens
// that are separated by `delimiter`
// returns vector of tokens
std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens; // Returned vector
    std::string token;               // Temporary token
    std::istringstream tokenStream(s); // String stream based on input

    // Loop through stream and add tokens to vector
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens; //Return vetor with tokens
}


void uciLoop(core& engine)
{
    string line;
    while (std::getline(std::cin, line)) {
        std::vector<string> tokens = split(line, ' ');

        //Skip if none
        if (tokens.empty()) continue;

        //Engine info
        if (tokens[0] == "uci")
        {
            engine.uci();
        }
        //readyok
        if (tokens[0] == "isready")
        {
            engine.isReady();
        }
        //Clear TT set FEN default
        if (tokens[0] == "ucinewgame") {
            engine.newGame();
        }
        //Set fen default or passed into
        if (tokens[0] == "position")
        {
            //Set starting position
            if (tokens.size() > 1 && tokens[1] == "startpos")
            {
                std::vector<std::string> moves;

                //If we get any moves pass them to function to make them
                if (tokens.size() > 2 && tokens[2] == "moves")
                {
                    moves.assign(tokens.begin() + 3, tokens.end());
                }

                //Make any moves from given fen checks legality
                engine.setPosition(moves);
            }

            //Create position from FEN
            if (tokens.size() > 1 && tokens[1] == "fen")
            {
                std::vector<std::string> moves;

                // 2 POSITION + 3 STM + 4 CASTLING RIGHTS + 5 ENPASSANT + 6 HALFMOVE + 7 FULLMOVE
                string FEN = tokens[2] + " " + tokens[3] + " " + tokens[4] + " " + tokens[5] + " " + tokens[6] + " " + tokens[7];

                // If we got more thats our moves [8] is 'moves'
                if(tokens.size() > 8)
                {
                    moves.assign(tokens.begin() + 9,tokens.end());
                }

                //Makes all moves from given fen checks legalality
                engine.setPosition(moves,FEN);
            }
        }
        //Setup search
        if (tokens[0] == "go")
        {
            //Set white and black timers
            int wTime = 0;
            int bTime = 0;
            int wInc = 0;
            int bInc = 0;

            int moveTime = 0;

            int nodes = 0;
            int depth = 0;

            //Loop through tokens
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                //If wee see wtime set i+1 to wtime
                if (tokens[i] == "wtime" && i + 1 < tokens.size())
                {
                    wTime = std::stoi(tokens[i + 1]);
                }
                //If wee see btime set i+1 to wtime
                if (tokens[i] == "btime" && i + 1 < tokens.size())
                {
                    bTime = std::stoi(tokens[i + 1]);
                }
                //If wee see wtime set i+1 to wtime
                if (tokens[i] == "wInc" && i + 1 < tokens.size())
                {
                    wInc = std::stoi(tokens[i + 1]);
                }
                //If wee see btime set i+1 to wtime
                if (tokens[i] == "bInc" && i + 1 < tokens.size())
                {
                    bInc = std::stoi(tokens[i + 1]);
                }
                if (tokens[i] == "movetime" && i + 1 < tokens.size())
                {
                    moveTime = std::stoi(tokens[i + 1]);
                }
                if (tokens[i] == "depth" && i + 1 < tokens.size())
                {
                    depth = std::stoi(tokens[i + 1]);
                }
                if (tokens[i] == "nodes" && i + 1 < tokens.size())
                {
                    nodes = std::stoi(tokens[i + 1]);
                }
            }
            //Set time based on uci
            engine.setTime(wTime, bTime, wInc, bInc, moveTime);

            //Start search
            std::thread search(&core::go, &engine);
            search.detach();

        }
        //Closes engine
        if (tokens[0] == "quit")
        {
            engine.quit();
        }
        //Stops search and outputs current bestmove
        if (tokens[0] == "stop")
        {
            engine.stop();
        }
        //Makes perft
        if (tokens[0] == "perft")
        {
            //Check depth and tokens count
            if(tokens.size() > 1)
            {
                int d = stoi(tokens[1]);
                engine.goPerft(d);
            }
        }
        if (tokens[0] == "setoption")
        {
            if(tokens.size() >= 5)
            {
                if(tokens[1] == "name" && tokens[2] == "Hash" && tokens[3] == "value")
                {
                    Bitboard size = stoi(tokens[4]);
                    if(size < 2000)
                    {
                        size = 2000;
                    }
                    if(size > 10000000000)
                    {
                        size = 10000000000;
                    }
                    engine.pos.TT.diskSize = size;
                    engine.pos.TT.ResetTT();
                    cout<<"info string set Hash to "<<size<<endl;
                }
            }
        }
    }
}
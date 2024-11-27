#include "core.h"
#include <sstream>
#include <chrono>
#include <thread>
#include "uci.h"

using namespace std;


// std::vector<std::string> split(const std::string &s, char delimiter) {
//     std::vector<std::string> tokens;
//     std::string token;
//     std::istringstream tokenStream(s);
//     while (std::getline(tokenStream, token, delimiter)) {
//         tokens.push_back(token);
//     }
//     return tokens;
// }

int main()
{
    core engine = core();

    //Read weights from file if necessary
    // engine.eval.readData();

    uciLoop(engine);

    // string line;
    // while (std::getline(std::cin, line)) {
    //     std::vector<string> tokens = split(line, ' ');
    //     if (tokens.empty()) continue;

    //     if (tokens[0] == "uci") {
    //         engine.uci();
    //     } else if (tokens[0] == "isready") {
    //         engine.isReady();
    //     } else if (tokens[0] == "ucinewgame") {
    //         engine.newGame();
    //     } else if (tokens[0] == "position") {
    //         if (tokens.size() > 1 && tokens[1] == "startpos") {
    //             std::vector<std::string> moves;
    //             if (tokens.size() > 2 && tokens[2] == "moves") {
    //                 moves.assign(tokens.begin() + 3, tokens.end());
    //             }
    //             engine.setPosition(moves);
    //         }
    //         if (tokens.size() > 1 && tokens[1] == "fen") {
    //             std::vector<std::string> moves;
    //             string FEN = tokens[2] + " " + tokens[3] + " " + tokens[4] + " " + tokens[5] + " " + tokens[6] + " " + tokens[7];
    //             if(tokens.size() > 8)
    //                 moves.assign(tokens.begin() + 9,tokens.end());

    //             engine.setPosition(moves,FEN);
    //         }
    //     } else if (tokens[0] == "go") {
    //         int wTime = 0;
    //         int bTime = 0;
    //         for (size_t i = 1; i < tokens.size(); ++i) {
    //             if (tokens[i] == "wtime" && i + 1 < tokens.size()) {
    //                 wTime = std::stoi(tokens[i + 1]);
    //             } else if (tokens[i] == "btime" && i + 1 < tokens.size()) {
    //                 bTime = std::stoi(tokens[i + 1]);
    //             }
    //         }
    //         engine.setTime(wTime, bTime);

    //         std::thread search(&core::go, &engine);
    //         search.detach();

    //         // engine.go();
    //     } else if (tokens[0] == "quit") {
    //         engine.quit();
    //     } else if (tokens[0] == "stop") {
    //         engine.stop();
    //     }else if (tokens[0] == "perft") {
    //         int d = stoi(tokens[1]);
    //         engine.goPerft(d);
    //     }
    // }

    return 0;
}
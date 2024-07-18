#include "core.h"
#include <sstream>

using namespace std;


std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main()
{
    core engine = core();

    string line;
    while (std::getline(std::cin, line)) {
        std::vector<string> tokens = split(line, ' ');
        if (tokens.empty()) continue;

        if (tokens[0] == "uci") {
            engine.uci();
        } else if (tokens[0] == "isready") {
            engine.isReady();
        } else if (tokens[0] == "ucinewgame") {
            engine.newGame();
        } else if (tokens[0] == "position") {
            if (tokens.size() > 1 && tokens[1] == "startpos") {
                std::vector<std::string> moves;
                if (tokens.size() > 2 && tokens[2] == "moves") {
                    moves.assign(tokens.begin() + 3, tokens.end());
                }
                engine.setPosition(moves);
            }
        } else if (tokens[0] == "go") {
            engine.go();
        } else if (tokens[0] == "quit") {
            engine.quit();
        }
    }
    // engine.pos.parseFEN(engine.startingFen,engine.pos.piecesBitboards);
    // cout<<engine.perft(6);
    // engine.pos.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 49 1",engine.pos.piecesBitboards);
    // engine.go();
    return 0;
}
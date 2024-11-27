#pragma once
#include <vector>
#include <string>
#include "core.h"

//Main loop for uci communication with engine
void uciLoop(core& engine);

//Function for splitting uci input into tokens
std::vector<std::string> split(const std::string &s, char delimiter);
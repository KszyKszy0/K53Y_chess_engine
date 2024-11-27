#pragma once
#include <vector>
#include <string>
#include "core.h"

void uciLoop(core& engine);

std::vector<std::string> split(const std::string &s, char delimiter);
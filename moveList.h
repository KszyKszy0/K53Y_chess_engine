#pragma once
#include "move.h"

// Simple array wrapped with size and check counted
// Size and checks used for mate/stalemate detection
class MoveList
{
    public:
    Move moveList[218];
    Move *cur = &moveList[0];
    Move *last;
    int checks;
    int size;

    MoveList()
    {
        checks = 0;
        size = 0;
        cur = &moveList[0];
    }

    // Access first element of Movelist
    Move *begin()
    {
        return &moveList[0];
    }

    // Access last element of Movelist
    Move *end()
    {
        return &moveList[size];
    }

    // Check if there is checkmate in current position
    // Size eqauls 0? && Checks greater than 0?
    bool isCheckmate()
    {
        return (size == 0) && (checks > 0);
    }

    // Check if there is stalemate in current position
    // Size eqauls 0? && Checks equal 0?
    bool isStalemate()
    {
        return (size == 0) && (checks == 0);
    }
};
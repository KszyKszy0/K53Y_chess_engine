#pragma once
#include "position.h"
#include "bitboard.h"



//Simple evaluation taking in consideration
//Psqt table and plain material
int evaluate(Position& pos);

//Simple psqt table for psqt eval
//Indexed [pieceType][square]
extern int psqtArray[389];


void readData();

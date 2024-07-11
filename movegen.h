#pragma once
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"

class MoveGenerator
{
    public:

    // 0 - quiets, 1 - captures
    void generateTypeMoves(Position pos, Bitboard target, vector<Move> &moveList, int type, int checks, Bitboard pins);

    //Generates moves from to doesnt care if legal
    void addMoves(int startSquare, Bitboard targets, vector<Move>& movesList, Position pos);

    //Constructor
    MoveGenerator(BB_utils BBM);

    BB_utils BBManager;

    //Returns all legal moves list
    vector<Move> fullMovesList(Position pos);

    Bitboard getSideAttacks(Position pos, bool white);

    Bitboard howManyAttacks(Position pos, bool white, int index);

    Bitboard generateKingsEvasions(Position pos, vector<Move>& moveList, Bitboard target, bool white);

    Bitboard getPinners(Position pos, bool white, vector<Move>& movesList);

    void addKnightsMoves(int startSquare, vector<Move>& movesList, Position pos, Bitboard target);

    void addBishopsMoves(int startSquare, vector<Move>& movesList, Position pos, Bitboard target);

    void addRookMoves(int startSquare, vector<Move>& movesList, Position pos, Bitboard target);

};
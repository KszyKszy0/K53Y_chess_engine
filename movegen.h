#pragma once
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"

class MoveGenerator
{
    public:

    // 0 - quiets, 1 - captures
    void generateTypeMoves(Position& pos, Bitboard target, vector<Move> &moveList, int type, int checks, Bitboard pins);

    //Generates moves from to doesnt care if legal
    void addMoves(int startSquare, Bitboard targets, vector<Move>& movesList, Position& pos);

    //Constructor
    MoveGenerator(BB_utils BBM);

    BB_utils BBManager;

    //Returns all legal moves list
    vector<Move> fullMovesList(Position& pos);

    Bitboard getSideAttacks(Position& pos, bool white, bool forKingMoves);

    Bitboard howManyAttacks(Position& pos, bool white, int index);

    Bitboard generateKingsMoves(Position& pos, vector<Move>& moveList, Bitboard target, bool white, int checks);

    Bitboard getPinners(Position& pos, bool white, vector<Move>& movesList, Bitboard target);


    void addPawnWhiteQuiet(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target);

    void addPawnWhiteCaptures(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target);

    void addPawnWhiteEnpassant(vector<Move>& movesList, Position& pos, Bitboard target, int enPassantSquare); //wazne



    void addPawnBlackQuiet(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target);

    void addPawnBlackCaptures(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target);

    void addPawnBlackEnpassant(vector<Move>& movesList, Position& pos, Bitboard target, int enPassantSquare); //wazne




    void addKnightsMoves(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target);

    void addBishopsMoves(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target);

    void addRookMoves(int startSquare, vector<Move>& movesList, Position& pos, Bitboard target);


    void addMoves(int startSquare, Bitboard targers, vector<Move>& movesList, Position& pos, int flag);

    void addMoves(int startSquare,int index,int flags, vector<Move>& movesList);

};
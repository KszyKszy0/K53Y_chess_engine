#pragma once
#include "bitboard.h"
#include "position.h"
#include "move.h"

class MoveGenerator
{
    public:

    void generateTypeMoves(Position& pos, Bitboard target, MoveList& moveList, Bitboard whatCanMove);

    // 0 - quiets, 1 - captures
    void generatePawnMoves(Position& pos, Bitboard target, MoveList& moveList, int type, Bitboard pins);

    //Generates moves from to doesnt care if legal
    void addMoves(int startSquare, Bitboard targets, MoveList& moveList, Position& pos);

    //Constructor
    MoveGenerator(BB_utils BBM);

    BB_utils BBManager;

    //Returns all legal moves list
    void fullMovesList(Position& pos, MoveList& moveList);

    Bitboard getSideAttacks(Position& pos, bool white, bool forKingMoves);

    Bitboard howManyAttacks(Position& pos, bool white, int index);

    Bitboard generateKingsMoves(Position& pos, MoveList& moveList, Bitboard target, bool white, int checks);

    Bitboard getPinners(Position& pos, bool white, MoveList& moveList, Bitboard target);


    void addPawnWhiteQuiet(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

    void addPawnWhiteCaptures(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

    void addPawnWhiteEnpassant(MoveList& moveList, Position& pos, Bitboard target, int enPassantSquare); //wazne



    void addPawnBlackQuiet(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

    void addPawnBlackCaptures(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

    void addPawnBlackEnpassant(MoveList& moveList, Position& pos, Bitboard target, int enPassantSquare); //wazne




    void addKnightsMoves(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

    void addBishopsMoves(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

    void addRookMoves(int startSquare, MoveList& moveList, Position& pos, Bitboard target);


    void addMoves(int startSquare, Bitboard targers, MoveList& moveList, Position& pos, int flag);

    void addMoves(int startSquare,int index,int flags, MoveList& moveList);

};
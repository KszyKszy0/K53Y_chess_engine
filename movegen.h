#pragma once
#include "bitboard.h"
#include "position.h"
#include "move.h"

class MoveGenerator
{
    public:

    void generateTypeMoves(Position& pos, Bitboard target, Move (&movesList)[218],int& movesListCounter, Bitboard whatCanMove);

    // 0 - quiets, 1 - captures
    void generatePawnMoves(Position& pos, Bitboard target, Move (&movesList)[218],int& movesListCounter, int type, Bitboard pins);

    //Generates moves from to doesnt care if legal
    void addMoves(int startSquare, Bitboard targets, Move (&movesList)[218],int& movesListCounter, Position& pos);

    //Constructor
    MoveGenerator(BB_utils BBM);

    BB_utils BBManager;

    //Returns all legal moves list
    void fullMovesList(Position& pos, Move (&movesList)[218]);

    Bitboard getSideAttacks(Position& pos, bool white, bool forKingMoves);

    Bitboard howManyAttacks(Position& pos, bool white, int index);

    Bitboard generateKingsMoves(Position& pos, Move (&movesList)[218],int& movesListCounter, Bitboard target, bool white, int checks);

    Bitboard getPinners(Position& pos, bool white, Move (&movesList)[218],int& movesListCounter, Bitboard target);


    void addPawnWhiteQuiet(int startSquare, Move (&movesList)[218],int& movesListCounter, Position& pos, Bitboard target);

    void addPawnWhiteCaptures(int startSquare, Move (&movesList)[218],int& movesListCounter, Position& pos, Bitboard target);

    void addPawnWhiteEnpassant(Move (&movesList)[218],int& movesListCounter, Position& pos, Bitboard target, int enPassantSquare); //wazne



    void addPawnBlackQuiet(int startSquare, Move (&movesList)[218],int& movesListCounter, Position& pos, Bitboard target);

    void addPawnBlackCaptures(int startSquare, Move (&movesList)[218],int& movesListCounter, Position& pos, Bitboard target);

    void addPawnBlackEnpassant(Move (&movesList)[218],int& movesListCounter, Position& pos, Bitboard target, int enPassantSquare); //wazne




    void addKnightsMoves(int startSquare, Move (&movesList)[218],int& movesListCounter, Position& pos, Bitboard target);

    void addBishopsMoves(int startSquare, Move (&movesList)[218],int& movesListCounter, Position& pos, Bitboard target);

    void addRookMoves(int startSquare, Move (&movesList)[218],int& movesListCounter, Position& pos, Bitboard target);


    void addMoves(int startSquare, Bitboard targers, Move (&movesList)[218],int& movesListCounter, Position& pos, int flag);

    void addMoves(int startSquare,int index,int flags, Move (&movesList)[218], int& movesListCounter);

};